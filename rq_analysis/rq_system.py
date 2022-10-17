import numpy as np
from scipy.optimize import root
from scipy.integrate import quad, dblquad
from scipy.misc import derivative
import multiprocessing as mp
from time import perf_counter_ns


class RQSystem:
    def __init__(self, mu1, mu2, Lambda, Q, alpha):
        self.K = 3
        self.prepare(mu1, mu2, Lambda, Q, alpha)

    def prepare(self, mu1, mu2, Lambda, Q, alpha):
        self.mu1 = mu1
        self.mu2 = mu2
        self.Lambda = Lambda
        self.Q = Q
        self.alpha = alpha
        self.N = self.Q.shape[1]

        self.I = np.identity(self.N)
        self.E = np.ones((self.N, 1))
        self.EE = np.ones((self.N * self.K, 1))

        print('Lambda has shape {0},Q has shape {1}, N equals {2}'.format(
            self.Lambda.shape, self.Q.shape, self.N))

        def rEqSystem(r):
            F = np.empty(self.N)
            for i in range(self.N):
                F[i] = np.dot(r, self.Q[:, i], out=np.empty(1))[
                    0] + np.dot(r, self.E)[0]-1
            return F

        r = root(rEqSystem, np.ones(self.N))
        r = r.get('x')
        self.r = r

        def ReqSystem(R):

            F0 = r * np.linalg.inv(self.I + (self.Lambda + R[-1]*self.I) *
                                   np.linalg.inv(self.mu1*self.I - self.Q) + self.alpha * np.linalg.inv(self.mu2*self.I - self.Q))
            F0 = np.squeeze(np.asarray(F0)) - R[0:self.N]

            F1 = R[0:self.N] * (self.Lambda + R[-1]*self.I) * \
                np.linalg.inv(self.mu1*self.I - self.Q)
            F1 = np.squeeze(np.asarray(F1)) - R[self.N:self.N*2]

            F2 = self.alpha * R[0:self.N] * \
                np.linalg.inv(self.mu2*self.I - self.Q)
            F2 = np.squeeze(np.asarray(F2)) - R[self.N*2:self.N*3]

            F3 = (R[self.N:self.N*2] + R[self.N*2:self.N*3]) * \
                self.Lambda * self.E - R[-1] * np.dot(R[0:self.N], self.E)
            F3 = np.array([np.squeeze(np.asarray(F3)) -
                           np.sum(R[0:self.N] + R[self.N:self.N*2] + R[self.N*2:self.N*3] - r)])
            return np.concatenate((F0, F1, F2, F3))

        self.R = root(ReqSystem, np.ones(self.K*self.N+1)).get('x')

    def GMatrix(self, u1, u2=None):
        if not u2:
            u2 = u1
        row1 = np.vstack(
            (self.Q - self.Lambda - (self.R[-1] + self.alpha)*self.I, self.mu1*np.exp(1j * u1)*self.I, self.mu2*np.exp(1j * u2)*self.I))
        row2 = np.vstack(
            (self.Lambda + self.R[-1]*self.I, self.Q - self.mu1*self.I, np.zeros((self.N, self.N))))
        row3 = np.vstack(
            (self.alpha*self.I, np.zeros((self.N, self.N)), self.Q-self.mu2*self.I))
        return np.hstack((row1, row2, row3))

    def EX(self, u1, u2, t):
        eigval, eigvecs = np.linalg.eig(t*self.GMatrix(u1, u2))
        return eigvecs * np.diag(np.exp(eigval)) * np.linalg.inv(eigvecs)

    def FF(self, u1, u2, t):
        return self.R[0:self.N*self.K] * self.EX(u1, u2, t) * self.EE

    def SGMatrix(self, u):
        row1 = np.vstack(
            (self.Q - self.Lambda - (self.R[-1] + self.alpha)*self.I, self.mu1*np.exp(1j * u)*self.I, self.mu2*np.exp(1j * u)*self.I))
        row2 = np.vstack(
            (self.Lambda + self.R[-1]*self.I, self.Q - self.mu1*self.I, np.zeros((self.N, self.N))))
        row3 = np.vstack(
            (self.alpha*self.I, np.zeros((self.N, self.N)), self.Q-self.mu2*self.I))
        return np.hstack((row1, row2, row3))

    def SEX(self, u, t):
        eigval, eigvecs = np.linalg.eig(t*self.SGMatrix(u))
        return eigvecs * np.diag(np.exp(eigval)) * np.linalg.inv(eigvecs)

    def SFF(self, u, t):
        return self.R[0:self.N*self.K] * self.SEX(u, t) * self.EE

    def DST(self, m1, m2, t):
        def integrand(u1, u2):
            return (np.exp(-1j*u1*m1) * np.exp(-1j*u2*m2) * self.FF(u1, u2, t))

        def real_func(u1, u2):
            return np.real(integrand(u1, u2))

        def imag_func(u1, u2):
            return np.imag(integrand(u1, u2))

        reals = dblquad(real_func, -np.pi, np.pi, lambda u1: -
                        np.pi, lambda u2: np.pi)
        imags = dblquad(imag_func, -np.pi, np.pi, lambda u1: -
                        np.pi, lambda u2: np.pi)
        return (1/(2*np.pi)**2) * (reals[0]+imags[0])

    def SDST(self, m, t):
        def integrand(u):
            return (np.exp(-1j*u*m) * self.SFF(u, t))

        def real_func(u):
            return np.real(integrand(u))

        def imag_func(u):
            return np.imag(integrand(u))

        reals = quad(real_func, -np.pi, np.pi)
        imags = quad(imag_func, -np.pi, np.pi)
        return (1/(2*np.pi)) * (reals[0]+imags[0])

    def DSTRange(self, sizeXstart, sizeXend, sizeYstart, sizeYend, t):
        X = np.arange(0, sizeXend, 1)
        Y = np.arange(0, sizeYend, 1)
        X, Y = np.meshgrid(X, Y)
        #w, h = sizeX, sizeY
        args = []
        for x in range(sizeXstart, sizeXend):
            for y in range(sizeYstart, sizeYend):
                args.append((x, y, t))

        with mp.Pool(mp.cpu_count()) as pool:
            Z = pool.starmap(self.DST, args)
        Z = np.array(Z)
        return Z.reshape(sizeXend, sizeYend)

    def SDSTRange(self, sizeXend,  t):
        X = np.arange(0, sizeXend, 1)
        X
        #w, h = sizeX, sizeY
        args = []
        for x in X:
            args.append((x, t))

        with mp.Pool(mp.cpu_count()) as pool:
            Z = pool.starmap(self.SDST, args)
        Z = np.array(Z)
        return Z

    def SDSTRange_perf(self, sizeXend,  t):
        X = np.arange(0, sizeXend, 1)
        X
        #w, h = sizeX, sizeY
        args = []
        for x in X:
            args.append((x, t))
        ti = perf_counter_ns()
        with mp.Pool(mp.cpu_count()) as pool:
            Z = pool.starmap(self.SDST, args)
        Z = np.array(Z)
        ti = perf_counter_ns() - ti
        return Z, ti

    def FFRange(self, sizeXstart, sizeXend, sizeYstart, sizeYend, t):
        X = np.arange(0, sizeXend, 1)
        Y = np.arange(0, sizeYend, 1)
        X, Y = np.meshgrid(X, Y)
        #w, h = sizeX, sizeY
        args = []
        for x in range(sizeXstart, sizeXend):
            for y in range(sizeYstart, sizeYend):
                args.append((x, y, t))

        with mp.Pool(mp.cpu_count()) as pool:
            Z = pool.starmap(self.FF, args)
        return np.array([complex(s) for s in Z]).reshape(sizeXend, sizeYend)

    def SFFRange(self, sizeXend,  t):
        X = np.arange(0, sizeXend, 1)
        X
        #w, h = sizeX, sizeY
        args = []
        for x in X:
            args.append((x, t))

        with mp.Pool(mp.cpu_count()) as pool:
            Z = pool.starmap(self.SFF, args)
        return [complex(s) for s in Z]

    def partial_derivative(self, func, var=0, point=[], n=1):
        args = point[:]

        def wraps(x):
            args[var] = x
            return func(*args)
        return derivative(wraps, point[var], dx=1e-6, n=n)

    def InputVariation(self):
        self.D = np.zeros(self.Q.shape)
        self.B = self.Lambda + self.Q * self.D
        self.Lg = self.r*self.Lambda*self.E
        self.v = (2*self.Lg*self.r*np.linalg.inv(-(self.Q-self.B))
                  * self.E - 1)/self.Lg**2
        return np.sqrt(self.v)/(self.Lg**(-1))

    def Mean(self, pos, t, n):
        if n == 1:
            return (1/1j)*self.partial_derivative(self.FF, pos, [0, 0, t], n)
        if n == 2:
            return -self.partial_derivative(self.FF, pos, [0, 0, t], n)
        return 0

    def DSTRange_perf(self, sizeXstart, sizeXend, sizeYstart, sizeYend, t):
        X = np.arange(0, sizeXend, 1)
        Y = np.arange(0, sizeYend, 1)
        X, Y = np.meshgrid(X, Y)
        #w, h = sizeX, sizeY
        args = []
        for x in range(sizeXstart, sizeXend):
            for y in range(sizeYstart, sizeYend):
                args.append((x, y, t))
        ti = perf_counter_ns()
        with mp.Pool(mp.cpu_count()) as pool:
            Z = pool.starmap(self.DST, args)
        Z = np.array(Z)
        ti = perf_counter_ns() - ti
        return Z.reshape(sizeXend, sizeYend), ti

    def icfft(self, n, t):
        res = []
        iii = list(range(0, n-1))
        delta = (2*np.pi)/n
        for i in iii:
            res.append(complex(self.SFF(-np.pi + i * delta, t)))
        d = np.zeros(n, dtype=complex)
        for j in range(0, n-1):
            for k in range(0, n-1):
                d[j] += res[k]*np.exp(-2*np.pi*1j*(j/n)*k)
            d[j] *= 1/np.sqrt(n)
            d[j] = abs(d[j])/np.sqrt(n)
        return d

    def icfft_perf(self, n, t):
        res = []
        iii = list(range(0, n-1))
        delta = (2*np.pi)/n
        for i in iii:
            res.append(complex(self.SFF(-np.pi + i * delta, t)))
        ti = perf_counter_ns()
        d = np.zeros(n, dtype=complex)
        for j in range(0, n-1):
            for k in range(0, n-1):
                d[j] += res[k]*np.exp(-2*np.pi*1j*(j/n)*k)
            d[j] *= 1/np.sqrt(n)
            d[j] = abs(d[j])/np.sqrt(n)
        ti = perf_counter_ns() - ti
        return d, ti

    def icfft2(self, n, m, t):
        iii_n = list(range(0, n-1))
        iii_m = list(range(0, m-1))
        delta_n = (2*np.pi)/n
        delta_m = (2*np.pi)/m
        res_mat = []
        for i in iii_n:
            pre_res = []
            for j in iii_m:
                pre_res.append(
                    complex(self.FF(-np.pi + i * delta_n, -np.pi + j * delta_m, 5)))
            res_mat.append(pre_res)
        d = np.zeros((n, m), dtype=complex)
        for j in range(0, n-1):
            for i in range(0, m-1):
                for k in range(0, n-1):
                    for h in range(0, m-1):
                        d[j][i] += res_mat[k][h] * \
                            np.exp(-2*np.pi*1j*(j/n)*k - 2*np.pi*1j*(i/m)*h)
                d[j][i] *= 1/np.sqrt(n*m)
                d[j][i] = abs(d[j][i])/np.sqrt(n*m)
        return d

    def icfft2_perf(self, n, m, t):
        iii_n = list(range(0, n-1))
        iii_m = list(range(0, m-1))
        delta_n = (2*np.pi)/n
        delta_m = (2*np.pi)/m
        res_mat = []
        for i in iii_n:
            pre_res = []
            for j in iii_m:
                pre_res.append(
                    complex(self.FF(-np.pi + i * delta_n, -np.pi + j * delta_m, 5)))
            res_mat.append(pre_res)
        ti = perf_counter_ns()
        d = np.zeros((n, m), dtype=complex)
        for j in range(0, n-1):
            for i in range(0, m-1):
                for k in range(0, n-1):
                    for h in range(0, m-1):
                        d[j][i] += res_mat[k][h] * \
                            np.exp(-2*np.pi*1j*(j/n)*k - 2*np.pi*1j*(i/m)*h)
                d[j][i] *= 1/np.sqrt(n*m)
                d[j][i] = abs(d[j][i])/np.sqrt(n*m)
        ti = perf_counter_ns() - ti
        return d, ti
