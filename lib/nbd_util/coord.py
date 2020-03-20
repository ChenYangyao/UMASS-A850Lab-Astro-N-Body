import numpy as np

class CoordsCvt:
    def __init__(self):
        pass
    
    @staticmethod
    def sphere_to_cart(theta, phi):
        return np.array([np.sin(theta)*np.cos(phi), 
            np.sin(theta)*np.sin(phi), np.cos(theta)])