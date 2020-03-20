from .random import RNG
import numpy as np
import scipy.optimize as opt

class Jeans:
    def __init__(self, profile):
        self.profile = profile
        self.r0, self.rt = profile.r0, profile.rt
        self.mass = profile.mass
    def get(self, n):
        M = RNG.uniform( n, 1.0e-4, 0.9999 ) * self.mass
        r = self.profile.r_at_M( M )
        sigma = np.sqrt(self.profile.sigma2( r ))
        
        pos = RNG.sphere(n) * r.reshape((n,1))
        vel = np.random.randn(n, 3) * sigma.reshape((n,1))
        vel = vel - vel.mean( axis=0 )
        partmass = self.mass / n
        print('range: pos=', np.max(np.abs(pos)), ', vel=', np.max(np.abs(vel)), 
              ', particle mass=', partmass)
        return pos, vel, partmass


class EddInv:
    def __init__(self, prof):
        self.prof = prof
        self.mass = prof.mass
        self.r0, self.rt = prof.r0, prof.rt
        self.Et = prof.V( prof.rt )
        self.f_optimize_target = lambda x,y: 1.5*x - 0.5*x*x*x - y
    def vmax(self, V):
        return np.sqrt( 2.0*(self.Et - V) )
    def one_sample(self):
        R0, R1, R2, R3 = RNG.uniform( 4, 1.0e-4, 0.9999 )
        M = R0 * self.mass
        r = self.prof.r_at_M( M )
        V = self.prof.V(r)
        vmax = self.vmax(V)
        
        z = np.sqrt(R1)
        vr = opt.root_scalar( self.f_optimize_target, 
            args=(R2,), bracket=[0., 1.], method='brentq' ).root * vmax
        vr2 = vr*vr
        vmax2 = vmax*vmax
        vt2 = z*z*( vmax2-vr2 )        
        fs = self.prof.fE( np.array([ (vr2+vt2)/2.0+V, V ]) )
        reject_thres = fs[0] / (fs[1]+1.0e-15)
        
        if R3 > reject_thres: return None
        vt = np.sqrt(vt2)
        return r, vr, vt
    def samples( self, n ):
        s = []
        cnt = 0
        while len(s) < n:
            sample = self.one_sample()
            cnt += 1
            if sample: s.append(sample)
        print('sampling rate = ', n/cnt)
        r, vr, vt = np.array(s).T
        pos = RNG.sphere(n) * r.reshape((n,-1))
        
        rn = RNG.uniform(n)
        vr[ rn<.5 ] = -vr[ rn<.5 ]
        
        phi = 2.0*np.pi*RNG.uniform(n)
        vx = np.cos(phi)*vt
        vy = np.sin(phi)*vt
        vz = vr
        vel = np.array([vx,vy,vz]).T
        vel = vel - vel.mean( axis=0 )
        partmass = self.mass / n
        print('range: pos=', np.max(np.abs(pos)), ', vel=', np.max(np.abs(vel)), 
              ', particle mass=', partmass)
        return pos, vel, partmass