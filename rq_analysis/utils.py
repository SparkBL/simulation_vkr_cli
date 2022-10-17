import numpy as np


def icfft2(n, m, matrix):
    d = np.zeros((n, m), dtype=complex)
    for j in range(0, n-1):
        for i in range(0, m-1):
            for k in range(0, n-1):
                for h in range(0, m-1):
                    d[j][i] += matrix[k][h] * \
                        np.exp(-2*np.pi*1j*(j/n)*k - 2*np.pi*1j*(i/m)*h)
            d[j][i] *= 1/np.sqrt(n*m)
    return d


def icfft(n, vector):
    d = np.zeros(n, dtype=complex)
    for j in range(0, n-1):
        for k in range(0, n-1):
            d[j] += vector[k]*np.exp(-2*np.pi*1j*(j/n)*k)
        d[j] *= 1/np.sqrt(n)
    return d


def k_distance(vec1, vec2):
    max = 0
    for i in range(0, np.minimum(len(vec1), len(vec2))):
        if abs(vec2[i] - vec1[i]) > max:
            max = abs(vec2[i] - vec1[i])
    return max


def k_distance2(mat1, mat2):
    max = 0
    for i in range(0, np.minimum(len(mat1), len(mat2))):
        for j in range(0, np.minimum(len(mat1[0]), len(mat2[0]))):
            if abs(mat2[i][j] - mat1[i][j]) > max:
                max = abs(mat2[i][j] - mat1[i][j])
    return max
