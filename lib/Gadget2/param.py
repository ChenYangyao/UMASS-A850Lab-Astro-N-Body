class Gadget2Param:
    r"""parameter file generator for Gadget-2.
    For the exact name and meaning of each parameter, see the Gadget-2 manual.

    Examples
    ----------
    # display the default parameters into the screen.
    p = Gadget2Param()
    p.show()

    # modify some of the parameters, and output info a file.
    # Unmodified parameters have default values.
    # You may then run the Gadget with
    # $ mpirun -np 4 Gadget2 param.txt
    p.set( 'SofteningHalo', 0.1 ).set( 'ErrTolForceAcc', 0.001 )\
        .set( 'UnitLength_in_cm', 3.08568e+23)\
        .set( 'InitCondFile', './IC' )
    filename='param.txt'
    p.dump(filename)

    # to retrive the current parameter value, use get().
    softening_len = p.get('SofteningHalo')

    # generate a series of directories for different runs.
    # Each of them has a parameter file with distinct softening value.
    import os
    for softening_len in .1, .5, 1., 5., 100.:
        dirname = 'run_soft%.2f'%softening_len
        os.system('mkdir -p %s/output'%dirname)
        
        Gadget2Param().set('InitCondFile', './IC')\
            .set('OutputDir', './output')\
            .set('SofteningHalo', softening_len)\
            .dump('%s/param.txt'%dirname)
    """
    def __init__(self):
        #  Relevant files
        self.InitCondFile = './IC', 's'
        self.OutputDir = 'output/', 's'
        self.EnergyFile         = 'energy.txt', 's'
        self.InfoFile           = 'info.txt', 's'
        self.TimingsFile        = 'timings.txt', 's'
        self.CpuFile            = 'cpu.txt', 's'
        self.RestartFile        = 'restart', 's'
        self.SnapshotFileBase   = 'snapshot', 's'
        self.OutputListFilename = 'parameterfiles/output_list.txt', 's'
        
        # CPU time -limit
        self.TimeLimitCPU       = 36000, 'd'            # = 10 hours
        self.ResubmitOn         = 0, 'd'
        self.ResubmitCommand    = 'my-scriptfile', 's'
        
        # Code options
        self.ICFormat                = 3, 'd'
        self.SnapFormat              = 3, 'd'
        self.ComovingIntegrationOn   = 0, 'd'
        self.TypeOfTimestepCriterion = 0, 'd'
        self.OutputListOn            = 0, 'd'
        self.PeriodicBoundariesOn    = 0, 'd'
        
        # Caracteristics of run
        self.TimeBegin               = 0.0,  'f'        # Begin of the simulation
        self.TimeMax                 = 10.0, 'f'        # End of the simulation
        self.Omega0                  = 0.,   'f'
        self.OmegaLambda             = 0.,   'f'
        self.OmegaBaryon             = 0.,   'f'
        self.HubbleParam             = 1.0,  'f'
        self.BoxSize                 = 0.,   'f'
        
        # Output frequency
        self.TimeBetSnapshot        = 1.0, 'f'
        self.TimeOfFirstSnapshot    = 0.,   'f'
        self.CpuTimeBetRestartFile     = 36000.0, 'f'   # here in seconds
        self.TimeBetStatistics         = 1.0,     'f'
        self.NumFilesPerSnapshot       = 1,       'd'
        self.NumFilesWrittenInParallel = 1,       'd'
        
        # Accuracy of time integration
        self.ErrTolIntAccuracy      = 0.025, 'f'
        self.CourantFac             = 0.15, 'f'     
        self.MaxSizeTimestep        = 0.01 , 'f' 
        self.MinSizeTimestep        = 0.0, 'f'
        
        # Tree algorithm, force accuracy, domain update frequency
        self.ErrTolTheta            = 0.5, 'f'
        self.TypeOfOpeningCriterion = 1,   'd'
        self.ErrTolForceAcc         = 0.005, 'f'
        self.TreeDomainUpdateFrequency    = 0.1, 'f'
        
        #  Further parameters of SPH
        self.DesNumNgb              = 50, 'd'
        self.MaxNumNgbDeviation     = 2,  'd'
        self.ArtBulkViscConst       = 0.8, 'f'
        self.InitGasTemp            = 0.,  'f'        # always ignored if set to 0 
        self.MinGasTemp             = 0.,  'f'
        
        # Memory allocation
        self.PartAllocFactor       = 1.5, 'f'
        self.TreeAllocFactor       = 0.8, 'f'
        self.BufferSize            = 25,  'd'        # in MByte
        

        # System of units
        self.UnitLength_in_cm         = 3.085677581467192e21, 'g'    #  1.0 kpc 
        self.UnitMass_in_g            = 1.988409870698051e43, 'g'    #  1.0e10 solar masses 
        self.UnitVelocity_in_cm_per_s = 1.0e5, 'g'                   #  1 km/sec 
        self.GravityConstantInternal  = 0.,    'f'
        
        # Softening lengths
        self.MinGasHsmlFractional = 0.25, 'f'

        self.SofteningGas       = 0., 'f'
        self.SofteningHalo      = 1.0, 'f'
        self.SofteningDisk      = 0.4,  'f'
        self.SofteningBulge     = 0.,   'f'           
        self.SofteningStars     = 0.,   'f'
        self.SofteningBndry     = 0.,   'f'

        self.SofteningGasMaxPhys       = 0., 'f'
        self.SofteningHaloMaxPhys      = 1.0, 'f'
        self.SofteningDiskMaxPhys      = 0.4, 'f'
        self.SofteningBulgeMaxPhys     = 0., 'f'           
        self.SofteningStarsMaxPhys     = 0., 'f'
        self.SofteningBndryMaxPhys     = 0., 'f'

        self.MaxRMSDisplacementFac = 0.2, 'f'
    def set(self, key, val):
        if key not in self.__dict__:
            raise Exception('key %s not valid'%key)
        oldval = self.__dict__[key]
        self.__dict__[key] = (val, oldval[1])
        return self
    def get(self, key, val):
        return self.__dict__[key]
    def show(self):
        for k,v in self.__dict__.items():
            if k[:2] != '__':
                print( '%s \t\t %%%s'%(k, v[1])%(v[0]) )
    def dump(self, fname):
        f = open( fname, 'w' )
        for k,v in self.__dict__.items():
            if k[:2] != '__':
                f.write( '%s \t\t %%%s\n'%(k, v[1])%(v[0]) )


if __name__ == "__main__":
    print("Gadget2 Parameter File Generator")
    print("Default parameters dumped into the file: _Gadget2_DefaultParam.txt")
    Gadget2Param().dump("_Gadget2_DefaultParam.txt")