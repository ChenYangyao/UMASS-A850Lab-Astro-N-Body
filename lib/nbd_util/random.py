from .coord import CoordsCvt
import numpy as np

class RNG:
    def __init__(self):
        pass
    
    @staticmethod
    def sphere( n ):
        cos_t = 2*RNG.uniform(n, 1.0e-10, 1.0-1.0e-10)-1.0
        t = np.arccos( cos_t )
        phi = 2.0*np.pi*RNG.uniform(n)
        return CoordsCvt.sphere_to_cart( t, phi ).T

    @staticmethod
    def circle( n ):
        t = RNG.uniform(n, 0., 2.0*np.pi)
        return np.array([np.cos(t), np.sin(t)]).T

    @staticmethod
    def uniform( n, lo=0., hi=1. ):
        return np.random.uniform( low = lo, high=hi, size=n )