import numpy as np
from .units import Units
import scipy.interpolate as interp
import scipy.integrate as integ
import scipy
import mystats
import collections

class HaloProfileBase:
    def __init__(self):
        print('%s Profile: Interp begins ...'%(self['name']))
        self._tags.update({ 
            'G': self['units']['G'], 'interp': interp.PchipInterpolator,
            'integ': integ.quad, 'vols': 4.0/3.0*np.pi*self['rs']**3,
        })
        self._interp_IM()
        self._interp_IV()
        self._interp_Isigma()
        self._interp_If()
    def __getitem__(self, key):
        return self._tags[key]
    def rho(self, r):
        return self['rho0']*self._Irho( r/self['rs'] )
    def M(self, r):
        return 3.0*self['Ms']*self._IM( r/self['rs'] )
    def V(self, r):
        return self['V0'] + 3.0*self['Vs']*self._IV( r/self['rs'] )
    def sigma2(self, r):
        x = r/self['rs']
        ans = 3.0*self['Vs']/self._Irho(x)*self._Isigma(x)
        if isinstance(ans, collections.Iterable):
            ans[ans<0.] = 0.
        else:
            if ans<0.: ans = 0.
        return ans
    def r_at_M(self, M):
        ans = self._inv_IM( M/(3.0*self['Ms']) )*self['rs']
        return ans
    def fE(self, E):
        xE = np.log(self['IE_eps']+(E-self['V0'])/(3.0*self['Vs']))
        ans = self['fE0'] * self._IfxE(xE)
        
        #xE_last = self['xEs'][-400]
        #if isinstance(xE, collections.Iterable):
        #    ans[ xE >= xE_last ] = 0.
        #else:
        #    if xE >= xE_last: ans = 0.
        
        return ans
    def _Irhoy(self, y):
        return self._Irho( np.exp(y) )
    def _interp_IM(self):
        Irhoy = self._Irhoy
        def f( y ):
            return np.exp( 3.0*y ) * Irhoy(y)
        y_vals = self['y_vals']
        IMys = [ self['integ']( f, self['yin'], y )[0] for y in y_vals ]
        _IMy = self['interp'](y_vals, IMys)
        self._IMy = _IMy
        self._IM = lambda x: _IMy( np.log(x) )
        Ms = self['mass'] / (3.0*_IMy(self['yvir']))
        self._tags.update({'Ms': Ms,
            'rho0':Ms/self['vols'],  'Vs':self['G']*Ms/self['rs']})
        _inv_IMy = self['interp']( IMys, y_vals )
        self._inv_IMy = mystats.InterpBound( IMys[0], IMys[-1], _inv_IMy )
        self._inv_IM = lambda IM: np.exp(_inv_IMy(IM) )
        print('  Interp for M done, IM = [%.4g...%.4g]'%( IMys[0], IMys[-1] ))
    def _interp_IV(self):
        IMy = self._IMy
        def f( y ):
            return IMy(y) / np.exp(y)
        y_vals = self['y_vals']
        IVys = [ self['integ']( f, self['yin'], y )[0] for y in y_vals ]
        _IVy = self['interp']( y_vals, IVys )
        self._IVy = _IVy
        self._IV = lambda x: _IVy( np.log(x) )
        self._tags['V0'] = -3.0*self['Vs']*IVys[-1]
        print('  Interp for V done, IV = [%.4g...%.4g]'%( IVys[0], IVys[-1] ))
    def _interp_Isigma(self):
        IMy, Irhoy = self._IMy, self._Irhoy
        def f(y):
            return IMy(y)*Irhoy(y) / np.exp(y)
        y_vals = self['y_vals']
        Isigmays = []
        for i, y in enumerate(y_vals):
            _int = self['integ']( f, y, self['yt'] )[0]
            Isigmays.append(_int)
        _Isigmay = self['interp'](y_vals, Isigmays)
        self._Isigmay = _Isigmay
        self._Isigma = lambda x: _Isigmay( np.log(x) )
        print('  Interp for sigma done, Isigma = [%.4g...%.4g]'%( Isigmays[0], \
            Isigmays[-1] ))
    def _interp_If(self):
        Irho_eps = 1.0
        IV_eps = q_eps = IE_eps =  0.01
        Int_eps = 1.0
        a_eps = -np.log(q_eps)+q_eps
        self._tags.update( {'Irho_eps':Irho_eps, 
            'IV_eps':IV_eps, 'q_eps':q_eps, 'IE_eps':IE_eps, 
            'Int_eps':Int_eps, 'a_eps':a_eps} )
        
        # make the integrand
        # It consists of a detivative drho / dV, or dx_rho / dx_V
        Irhos, IVs = self._Irhoy(self['y_vals']), self._IVy(self['y_vals'])
        xrhos, xVs = np.log( Irho_eps + Irhos ), np.log( IV_eps + IVs )
        _xrhoxV = self['interp']( xVs, xrhos )
        _dxrhoxV = _xrhoxV.derivative()
        expq_eps = np.exp(q_eps)
        def f(q, IE):
            expq = np.exp(q)
            A = expq - expq_eps
            IV = IE + A*A
            xV = np.log( IV_eps + IV )
            xrho = _xrhoxV(xV)
            return expq * _dxrhoxV( xV ) * np.exp(xrho) / np.exp(xV)
        
        IEs = IVs + 0.
        IEt = IVs[-1]
        Ints = []
        for i, IE in enumerate(IEs):
            f2int = lambda q: f(q, IE)
            q_lo, q_hi = q_eps, np.log(np.sqrt(IEt-IE)+expq_eps)
            Ints.append( self['integ']( f2int, q_lo, q_hi, 
                epsrel=1.0e-4, limit=500 )[0] )
        Ints = np.array(Ints)
        
        # make derivative again
        xEs = np.log( IE_eps + IEs ) 
        xInts = np.log( Int_eps - Ints )
        self._tags.update({'xEs':xEs,'xInts':xInts,'IEs':IEs})
        _xIntxE = self['interp']( xEs, xInts )
        _dxIntdxE = mystats.InterpBound(xEs[0], xEs[-1], _xIntxE.derivative())
        _xIntxE = mystats.InterpBound(xEs[0], xEs[-1], _xIntxE)
        def _IfxE(xE):
            xInt = _xIntxE(xE)
            dxIntdxE = _dxIntdxE(xE)
            return - np.exp(xInt) / np.exp(xE) * dxIntdxE
        #self._IfxE = mystats.InterpBound(xEs[0], xEs[-400], _IfxE) 
        self._IfxE = mystats.InterpBound(xEs[0], xEs[-1], _IfxE) 
        self._tags['fE0'] = 2.0/( np.sqrt(8.0)*np.pi*np.pi ) * self['rho0'] \
            / ( 3.0*self['Vs'] )**1.5
        print('  Interp for fE done, xE = [%.4g...%.4g], xInt = [%.4g...%.4g]'%( \
            xEs[0], xEs[-1], xInts[0], xInts[-1] ))


class NFWProfile(HaloProfileBase):
    def __init__(self, mass, rs, rvir=None, rt=None, rw=None, 
        r0=None, rcore=None, units=None, name='NFW', tags_kw = {} ):
        self._tags = {'mass': mass, 'name': name}
        self._tags.update(tags_kw)
        
        _rs = rs
        _rvir = rvir if rvir is not None else 15.0*_rs
        _rt = rt if rt is not None else 2.0*_rvir
        _rw = rw if rw is not None else 0.2*_rvir
        _r0 = r0 if r0 is not None else 1.0e-4*_rs
        _rin = 0.1*_r0
        _rcore = rcore if rcore is not None else 1.0e-10*_rs
        r_vals = np.array([_rin, _r0, _rcore, 
            _rs, _rvir, _rw, _rt],dtype=float)
        
        for ir, rpost in enumerate(('in', '0', 'core', 's', 'vir', 'w', 't')):
            r_val = r_vals[ir]
            self._tags.update({ 
                'r'+rpost: r_val, 'x'+rpost: r_val/_rs, 
                'y'+rpost: np.log(r_val/_rs) })
        
        self._tags['units'] = units if units else Units()
        y_ins = np.linspace(self['y0'], np.log( 0.8*self['xt'] ), 1500 )
        y_outs = np.log( np.linspace(0.801*self['xt'], self['xt'], 400) )
        self._tags['y_vals'] = np.hstack( (y_ins, y_outs) )
        HaloProfileBase.__init__(self)
    def _Irho(self, x):
        xp1 = 1.0+x
        xpc = self['xcore']+x
        return 1.0 / ( xpc*xp1*xp1 ) * \
            scipy.special.erf( (self['xt']-x)/self['xw'] )
    
class HernquistProfile(NFWProfile):
    def __init__(self, mass, rs, rvir=None, rt=None, rw=None, 
        r0=None, rcore=None, units=None, name='Hernquist' ):
        NFWProfile.__init__(self, mass, rs, rvir, rt, rw, 
            r0, rcore, units, name)
    def _Irho(self, x):
        xp1 = 1.0+x
        xpc = self['xcore']+x
        return 1.0/( xpc*xp1*xp1*xp1 ) * \
            scipy.special.erf( (self['xt']-x)/self['xw'] )


class DoublePowerlawProfile(NFWProfile):
    def __init__(self, alpha, beta, gamma,
        mass, rs, rvir=None, rt=None, rw=None,
        r0=None, rcore=None, units=None, name='Double power-law' ):
        tags_kw = { 'alpha': alpha, 'beta': beta, 'gamma':gamma }
        NFWProfile.__init__(self, mass, rs, rvir, rt, rw,
            r0, rcore, units, name, tags_kw=tags_kw)
    def _Irho(self, x):
        alpha, beta, gamma = self['alpha'], self['beta'], self['gamma']
        xpc = self['xcore']+x
        xin = np.power( xpc, gamma )
        xout = np.power( 1.0+np.power(x, 1.0/alpha), (beta-gamma)*alpha )
        return 1.0 / ( xin * xout )