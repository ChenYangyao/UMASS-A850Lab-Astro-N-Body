#ifndef _ADJUST_IO_TOOL_H5PARALLEL_H_
#define _ADJUST_IO_TOOL_H5PARALLEL_H_

#include "task.h"
#include <hippmpi.h>
#include <parallel/hdf5/hdf5.h>

class IO_H5Parallel: public Task {
public:
    using Comm = HIPP::MPI::Comm;
    using ErrMPI = HIPP::MPI::ErrMPI;
    

    IO_H5Parallel( string filename, int n_repeat, size_t n_elmt, 
        Comm comm ): _comm(comm) {
        _n_repeat = n_repeat;
        _n_elmt = n_elmt;
        _n_elmt_eachproc = n_elmt/_comm.size();

        auto fapl = H5Pcreate( H5P_FILE_ACCESS );
        H5Pset_fapl_mpio( fapl, _comm.raw(), MPI_INFO_NULL );
        _file = H5Fcreate( filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, fapl );
        H5Pclose( fapl );

        hsize_t fdims[2] = { n_elmt, n_elmt };
        _fspace = H5Screate_simple( 2, fdims, NULL );
        _dset = H5Dcreate( _file, "/temp", H5T_NATIVE_DOUBLE, _fspace, 
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
    
        _dxpl = H5Pcreate( H5P_DATASET_XFER );
        H5Pset_dxpl_mpio( _dxpl, H5FD_MPIO_COLLECTIVE );
        hsize_t start[2] = { _comm.rank()*_n_elmt_eachproc, 0 },
            count[2] = { _n_elmt_eachproc, _n_elmt };
        H5Sselect_hyperslab( _fspace, H5S_SELECT_SET, 
            start, NULL, count, NULL );
        _memspace = H5Screate_simple( 2, count, NULL );
        _data.assign( _n_elmt_eachproc*_n_elmt, 1.0 );
    }
    ~IO_H5Parallel(){ 
        H5Dclose( _dset );
        H5Sclose( _fspace );
        H5Sclose( _memspace );
        H5Pclose( _dxpl );
        H5Fclose( _file );
    }
protected:
    hid_t _file, _dset, _fspace, _dxpl, _memspace;
    size_t _n_elmt, _n_elmt_eachproc;
    vector<double> _data;
    Comm _comm;

    virtual double _run() override {
        H5Dwrite( _dset, H5T_NATIVE_DOUBLE, _memspace, 
            _fspace, _dxpl, _data.data() );
        H5Dread( _dset, H5T_NATIVE_DOUBLE, _memspace, 
            _fspace, _dxpl, _data.data() );
        return sizeof(double)*double(_data.size())*_comm.size();
    }
};


#endif	//_ADJUST_IO_TOOL_H5PARALLEL_H_