import numpy as np

class CoordsCvt:
    def __init__(self):
        pass
    
    @staticmethod
    def sphere_to_cart(theta, phi):
        return np.array([np.sin(theta)*np.cos(phi), 
            np.sin(theta)*np.sin(phi), np.cos(theta)])

    @staticmethod
    def normalize(v):
        return v / np.linalg.norm(v, axis=1).reshape((len(v),-1))

    @staticmethod
    def rt_to_cart(x, v):
        ''' radial-tangential coordinate to cartesian
        Parameters
        ----------
        x: (n,3) array-like
            spatial position
        v: (n,3) array-like
            vector to be transformed
        Return (n,3) array-like, the cartesian coordinate
        '''
        vnew_z = np.array(x)
        vold_z = np.array([0,0,1])
        vnew_x = np.cross( vold_z, vnew_z )
        vnew_y = np.cross( vnew_z, vnew_x )

        vnew_x = CoordsCvt.normalize(vnew_x)
        vnew_y = CoordsCvt.normalize(vnew_y)
        vnew_z = CoordsCvt.normalize(vnew_z)

        v = np.array(v)
        return vnew_x*v[:,[0]]+vnew_y*v[:,[1]]+vnew_z*v[:,[2]]



