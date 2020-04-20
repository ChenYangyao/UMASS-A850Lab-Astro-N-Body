from .random import RNG
from .coord import CoordsCvt
from .units import Units
import numpy as np
import scipy.optimize as opt
import scipy.interpolate as interp

class Jeans:
    def __init__(self, profile):
        pf = profile
        self._tags = { 'profile': pf, 
            'mass': pf.M( pf['rvir'] ),
            'm_hi': 1., 'm_lo': 0., 
            'r0': pf['r0'], 'rt': pf['rt'], 'rw': pf['rw'] }
    def __getitem__(self, key):
        return self._tags[key]
    def get(self, n):
        pf = self['profile']
        M = RNG.uniform( n, self['m_lo'], self['m_hi'] ) * self['mass']
        r = pf.r_at_M( M )
        sigma = np.sqrt(pf.sigma2( r ))
        
        pos = RNG.sphere(n) * r.reshape((n,1))
        vel = np.random.randn(n, 3) * sigma.reshape((n,1))
        vel = vel - vel.mean( axis=0 )
        partmass = self['mass'] / n
        print('range: pos=%g, vel=%g, m_part=%g, sampl. rate=%g'%(\
            np.max(np.abs(pos)), np.max(np.abs(vel)), partmass, 1.0))
        return pos, vel, partmass

class EddInv:
    def __init__(self, profile):
        pf = profile
        
        Et = pf.V( pf['rt'] )
        
        f_opt = lambda x,y: 0.25*(3.0*x - x*x*x) + 0.5 - y
        rands = np.linspace(0, 1, 10000)
        xs = [ opt.root_scalar( 
            f_opt, args=(rand,), bracket=[-1,1], method='brentq' ).root \
              for rand in rands ]
        r2vr = interp.PchipInterpolator(rands, xs)
        
        self._tags = { 'profile': pf, 
            'mass': pf.M( pf['rvir'] ),
            'r2vr': r2vr, 'Et': Et,
            'm_hi': 1., 'm_lo': 0., 
            'r0': pf['r0'], 'rt': pf['rt'], 'rw': pf['rw'] }
    def __getitem__(self, key):
        return self._tags[key]
    def vmax(self, V):
        dV = self['Et'] - V
        if np.any(dV < -1.0e-10):
            raise Exception('Error V %.6g > Et %.6g'%(V[dV < -1.0e-10], 
                self['Et']))
        dV[ dV < 0. ] = 0.
        return np.sqrt( 2.0*dV )
    def samples(self, n):
        pf = self['profile']
        M = RNG.uniform( n, self['m_lo'], self['m_hi'] ) * self['mass']
        r = pf.r_at_M(M)
        V = pf.V(r)
        vmax = self.vmax(V)
        vmax2 = vmax*vmax
        fV = pf.fE(V)
        vr, vt = np.zeros(n), np.zeros(n)
        vr[ fV == 0. ] = 0.
        vt[ fV == 0. ] = 0.
        ids_todo = np.arange(n,dtype=int)[ fV != 0. ]
        n_trys = 0
        while len(ids_todo) > 0:
            n_ids = len(ids_todo)
            n_trys += n_ids
            acc, _vr, _vt = self.samples_at_r( r[ids_todo], 
                V[ids_todo], vmax[ids_todo], vmax2[ids_todo], fV[ids_todo]  )
            ids_acc = ids_todo[ acc ]
            vr[ids_acc] = _vr
            vt[ids_acc] = _vt
            ids_todo = ids_todo[ acc==False ]
            if len(ids_todo) < n/5000+5:
                break
        print('Begins to chunk implementation for %d samples'%(len(ids_todo)), 
            end='... ')
        for id_todo in ids_todo:
            base_n, fac_n, max_fac_n = 1000, 1, 10000
            while True:
                ones = np.ones(base_n*fac_n, dtype=float)
                n_trys += len(ones)
                acc, _vr, _vt = self.samples_at_r( r[id_todo]*ones, 
                    V[id_todo]*ones, vmax[id_todo]*ones, 
                    vmax2[id_todo]*ones, fV[id_todo]*ones )
                if len(_vr) > 0:
                    vr[id_todo] = _vr[0]
                    vt[id_todo] = _vt[0]
                    break
                if fac_n < max_fac_n: fac_n *= 2
                else: 
                    print('\n\tFor sample %d find difficulty, r=%g, fV=%g... '%(\
                        id_todo, r[id_todo], fV[id_todo]), end='')
                    vr[id_todo] = 0.;
                    vt[id_todo] = 0.
                    break
        return r, vr, vt, n_trys
    def samples_at_r(self, r, V, vmax, vmax2, fV ):
        pf = self['profile']
        n = len(r)
        R1, R2, R3 = RNG.uniform(3*n, 1.0e-6, 1-1.0e-6).reshape(3,n)
        z = np.sqrt( R1 )
        vr = self['r2vr']( R2 ) * vmax
        vr2 = vr*vr
        vt2 = z*z*(vmax2-vr2)
        fE = pf.fE( 0.5*(vr2+vt2)+V )
        accept = fE / fV > R3
        return accept, vr[accept], np.sqrt(vt2[accept])
    def get( self, n ):
        r, vr, vt, n_trys = self.samples(n)
        pos = RNG.sphere(n) * r.reshape((n,-1))
        circ=RNG.circle(n)*vt.reshape((n,-1))
        v = np.array([circ[:, 0], circ[:, 1], vr]).T
        v = CoordsCvt.rt_to_cart( pos, v )
        vel = v - v.mean( axis=0 )
        partmass = self['mass'] / n
        print('finish\n\trange: pos=%g, vel=%g, m_part=%g, sampl. rate=%g'%(\
            np.max(np.abs(pos)), np.max(np.abs(vel)), partmass, n/n_trys))
        return pos, vel, partmass