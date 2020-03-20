import numpy as np 
import h5py

def IC_generator(filename, pos, vel, mass):
    npart = len(pos)
    attrs = {
        'BoxSize': 0., 'Flag_Cooling': 0,
        'Flag_Entropy_ICs': np.array([0]*6, dtype=int),
        'Flag_Feedback': 0,
        'Flag_Metals': 0,
        'Flag_Sfr': 0,
        'Flag_StellarAge': 0,
        'HubbleParam': 1.0,
        'MassTable': np.array([0., mass, 0., 0., 0., 0.], dtype=float),
        'NumFilesPerSnapshot': 1,
        'NumPart_ThisFile': np.array([0, npart, 0, 0, 0, 0], dtype=int),
        'NumPart_Total': np.array([0, npart, 0, 0, 0, 0], dtype=int),
        'NumPart_Total_HighWord': np.array([0]*6, dtype=int),
        'Omega0': 0.,
        'OmegaLambda': 0.,
        'Redshift': 0.,
        'Time': 0.
    }
    dsets = {
        'Coordinates': pos,
        'Velocities': vel,
        'ParticleIDs': np.arange(npart, dtype=int)
    }
    
    file = h5py.File( filename, 'w' )
    file_hd = file.create_group('Header')
    file_attrs = file_hd.attrs
    for k, v in attrs.items():
        file_attrs.create(k, data=v)
    grp = file.create_group('PartType1')
    for k, v in dsets.items():
        grp.create_dataset(k, data=v)