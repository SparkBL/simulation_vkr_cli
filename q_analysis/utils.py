import numpy as np
from multiprocessing import Process
import q_analysis.simulation as rq

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

def run_cpu_tasks_in_parallel(tasks):
    running_tasks = [Process(target=task) for task in tasks]
    for running_task in running_tasks:
        running_task.start()
    for running_task in running_tasks:
        running_task.join()


class PyModel:
    components = {}
    routers = {}
    time = 0
    end = 1
    __queue__ = np.array([])
    def aggregate(self,l):
       self.__queue__ =  np.append(self.__queue__,l)

    def next_step(self):
        if len(self.__queue__)!=0:
            self.time = np.min(self.__queue__)
            self.__queue__ = np.delete(self.__queue__, self.__queue__.argmin())
        return self.time
    
    def is_done(self):
        return self.time>=self.end
    
    def add_producer(self,p,l):
        self.components.update({l:p})

    def component_at(self,l):
        return self.components[l]
    
    def router_at(self,l):
        return self.routers[l]
    
    def add_connection(self,from_c,from_s,to_c,to_s):
        self.routers.update({f'{from_c}:{from_s}:{to_c}:{to_s}':rq.Router()})
        self.components[from_c].output_connect(from_s,self.routers[f'{from_c}:{from_s}:{to_c}:{to_s}'])
        self.components[to_c].input_connect(to_s,self.routers[f'{from_c}:{from_s}:{to_c}:{to_s}'])
        return f'{from_c}:{from_s}:{to_c}:{to_s}'
        
    def add_hanging_output_noqueue(self,from_c,from_s):
        self.routers.update({f'onq:{from_c}:{from_s}':rq.OutputRouter()})
        self.components[from_c].output_connect(from_s,self.routers[f'onq:{from_c}:{from_s}'])
        return f'onq:{from_c}:{from_s}'
        

    def set_time(self,t):
        self.time = t
    
    def set_end(self,t):
        self.end = t


def find_nearest(array, value):
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return (idx,array[idx])