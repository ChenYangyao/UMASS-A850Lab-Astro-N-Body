import numpy as np
from .units import Units
import scipy.interpolate as interp
import scipy.integrate as integ
import scipy
import collections

class NFWProfile:
    def __init__(self, mass, rs, rt=None, rw=None, r0=None, units=None ):
        self.mass = mass
        self.rs = rs
        self.rt = rt if rt else 15.0*rs
        self.rw = rw if rw else rs
        self.r0 = r0 if r0 else 1.0e-5 * rs
        
        self.xs, self.xt, self.xw, self.x0 = 1.0, \
            self.rt/self.rs, self.rw/self.rs, self.r0 / self.rs
        self.ys, self.yt, self.yw, self.y0 = 0., \
            np.log( self.xt ), np.log( self.xw ), np.log( self.x0 )
        self.vols = 4.0/3.0*np.pi*rs*rs*rs
        
        self.units = units if units else Units()
        self.G =  self.units['G']
        
        #y1, y2 = self.y0, self.yt
        #ys = np.linspace( y1, y2, 1500 )
        ys = np.hstack( ( 
            np.linspace(self.y0, np.log(self.xt-self.xw), 1500 ), 
            np.log( np.linspace(self.xt-self.xw+0.001*self.xw, self.xt, 100) ) 
        ) )
        self._data = {'ys': ys }
        self.interp = interp.PchipInterpolator
        
        self._interp_IM()
        self._interp_IV()
        self._interp_Isigma()
        self._interp_If()
    def __getitem__(self, key):
        return self._data[key]
    def rho(self, r):
        return self.rho0 * self._Irho( r/self.rs )
    def M(self, r):
        return 3.0*self.Ms*self._IM( r/self.rs )
    def V(self, r):
        return self.V0 + 3.0 * self.Vs * self._IV( r/self.rs )
    def sigma2(self, r):
        x = r/self.rs
        ans = 3.0*self.Vs/self._Irho(x)*self._Isigma(x)
        if isinstance(ans, collections.Iterable):
            ans[ ans < 0. ] = 0.
        else:
            if ans < 0. : ans = 0.
        return ans
    def r_at_M(self, M):
        ans = self._inv_IM( M/(3.0*self.Ms) ) * self.rs
        if isinstance(ans, collections.Iterable):
            ans[ ans < 0. ] = 0.
            ans[ ans > self.rt ] = self.rt
        else:
            if ans < 0.: ans = 0.
            if ans > self.rt: ans = self.rt
        return ans
    def fE(self, E):
        xE = np.log(self['IE_eps'] + (E-self.V0)/(3.0*self.Vs))
        ans = self.fE0 * self._IfxE(xE)
        if isinstance(ans, collections.Iterable):
            ans[ans < 0.] = 0.
        else:
            if ans < 0.: ans = 0.
        return ans
    def _Irho(self, x):
        xp1 = 1.0+x
        return 1.0 / ( x*xp1*xp1 ) * scipy.special.erf( (self.xt-x)/self.xw )
    def _Irhoy(self, y):
        return self._Irho( np.exp(y) )
    def _interp_IM(self):
        Irhoy = self._Irhoy
        def f( y ):
            return np.exp( 3.0*y ) * Irhoy(y)
        ys = self['ys']
        IMys = [ integ.quad( f, self.y0, y )[0] for y in ys ]
        _IMy = self.interp(ys, IMys)
        self._IMy = _IMy
        self._IM = lambda x: _IMy( np.log(x) )
        self.Ms = self.mass / (3.0*IMys[-1])
        self.rho0 = self.Ms / self.vols
        self.Vs = self.G * self.Ms / self.rs
        print('interp for M done, IM = [%f...%f]'%( IMys[0], IMys[-1] ))
        _inv_IMy = self.interp( IMys, ys )
        self._inv_IMy = _inv_IMy
        self._inv_IM = lambda IM: np.exp(_inv_IMy(IM) )
        self._data['IMys'] = np.array(IMys)
        self._data['Irhoys'] = self._Irhoy( ys )
    def _interp_IV(self):
        IMy = self._IMy
        def f( y ):
            return IMy(y) / np.exp(y)
        ys = self['ys']
        IVys = [ integ.quad( f, self.y0, y )[0] for y in ys ]
        _IVy = self.interp( ys, IVys )
        self._IVy = _IVy
        self._IV = lambda x: _IVy( np.log(x) )
        self.V0 = -3.0*self.Vs*IVys[-1]
        print('interp for V done, IV = [%f...%f]'%( IVys[0], IVys[-1] ))
        self._data['IVys'] = np.array(IVys)
    def _interp_Isigma(self):
        IMy, Irhoy = self._IMy, self._Irhoy
        def f(y):
            return IMy(y)*Irhoy(y) / np.exp(y)
        ys = self['ys']
        Isigmays = [ integ.quad( f, y, self.yt )[0] for y in ys ]
        _Isigmay = self.interp(ys, Isigmays)
        self._Isigmay = _Isigmay
        self._Isigma = lambda x: _Isigmay( np.log(x) )
        print('interp for sigma done, Isigma = [%f...%f]'%( Isigmays[0], \
            Isigmays[-1] ))
        self._data['Isigmays'] = np.array(Isigmays)
    def _interp_If(self):
        Irho_eps = IV_eps = IE_eps = Int_eps = q_eps = 0.01
        self._data.update( dict(Irho_eps=Irho_eps, 
                IV_eps=IV_eps, IE_eps=IE_eps, Int_eps=Int_eps, q_eps=q_eps) )
        
        # make the integrand
        # It consists of a detivative drho / dV, or dx_rho / dx_V
        Irhos, IVs = self['Irhoys'], self['IVys']
        xrhos, xVs = np.log( Irho_eps + Irhos ), np.log( IV_eps + IVs )
        _xrhoxV = self.interp( xVs, xrhos )
        _dxrhoxV = _xrhoxV.derivative()
        
        # get integration
        expq_eps = np.exp(q_eps)
        def f(q, IE):
            expq = np.exp(q)
            IV = IE + (expq - expq_eps)**2
            xV = np.log( IV_eps + IV )
            xrho = _xrhoxV(xV)
            return expq * _dxrhoxV( xV ) * np.exp(xrho) / np.exp(xV)
        IEs = IVs + 0.
        IEt = IEs[-1]        
        Ints = np.array([ integ.quad( 
            (lambda q: f(q, IE)), 
            q_eps, np.log(np.sqrt(IEt-IE)+expq_eps), limit=1000 )[0] 
            for IE in IEs ])
        
        # make derivative again
        xEs = np.log( IE_eps + IEs ) 
        xInts = np.log( Int_eps - Ints )
        _xIntxE = self.interp( xEs, xInts )
        _dxIntdxE = _xIntxE.derivative()
        #dxIntdxE = _xIntxE.derivative()( xEs )
        #for i in range(len(xEs)-1):
        #    if dxIntdxE[i+1] < dxIntdxE[i]: dxIntdxE[i+1] = dxIntdxE[i]
        #_dxIntdxE = self.interp( xEs, dxIntdxE )
        self._data.update(dict(xEs=xEs,xInts=xInts))
        
        print('interp for fE done, xE = [%f...%f], xInt = [%f...%f]'%( \
            xEs[0], xEs[-1], xInts[0], xInts[-1] ))
        def _IfxE(xE):
            xInt = _xIntxE(xE)
            dxIntdxE = _dxIntdxE(xE)
            return - np.exp(xInt) / np.exp(xE) * dxIntdxE
        self._IfxE = _IfxE
        self.fE0 = 2.0/( np.sqrt(8.0)*np.pi*np.pi ) * self.rho0 \
            / ( 3.0*self.Vs )**1.5