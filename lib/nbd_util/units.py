import astropy.units as u
import astropy.constants as const

class Units:
    def __init__(self, 
                 ulength=u.kpc.to('cm'), 
                 umass=u.Msun.to('g')*1.0e10, uvel=1.0e5):
        self.uL = ulength * u.cm
        self.uM = umass * u.g
        self.uV = uvel * u.cm / u.s
        self.uT = self.uL / self.uV
        print( 'units: L=%s (%s), \n\tM=%s (%s), \n\tV=%s (%s), \n\tT=%s (%s)'\
            %( self.uL, self.uL.to('kpc'),
            self.uM, self.uM.to('Msun'),
            self.uV, self.uV.to('km/s'),
            self.uT, self.uT.to('Gyr') ) )
        self._get_const()
    def __getitem__(self, tag):
        return self.consts[tag]
    def _get_const(self):
        self.consts = {
            'G': (const.G / ( self.uL**3 / self.uT**2 / self.uM ) ).to(1).value,
            'Msun': ( u.Msun/self.uM ).to(1)
        }
        print('consts:', ', '.join([ '%s=%s'%(k,v) \
            for k,v in self.consts.items()]) )