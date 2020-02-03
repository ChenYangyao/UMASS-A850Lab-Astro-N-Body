#ifndef _ADJUST_IO_TOOL_MPI_H_
#define _ADJUST_IO_TOOL_MPI_H_

#include "task.h"
#include <hippmpi.h>

class IO_MPI: public Task {
public:
    using Comm = HIPP::MPI::Comm;
    using ErrMPI = HIPP::MPI::ErrMPI;

    IO_MPI( string filename, int n_repeat, size_t n_elmt, 
        Comm comm ): _comm(comm) {
        ErrMPI::check(
            MPI_File_open( _comm.raw(), filename.c_str(), 
                MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &_fd ),
            emFLPFB);
        _n_repeat = n_repeat;
        _n_elmt = n_elmt;
        _n_elmt_eachproc = n_elmt/_comm.size();
        _data.assign( _n_elmt_eachproc * _n_elmt, 1.0 );

        auto filetype = HIPP::MPI::DOUBLE.contiguous( _n_elmt_eachproc*_n_elmt );
        MPI_File_set_view( _fd, 
            _comm.rank()*_n_elmt_eachproc*_n_elmt*sizeof(double), 
            MPI_DOUBLE, filetype.raw(), "native", MPI_INFO_NULL );
    }
    ~IO_MPI(){ 
        ErrMPI::check(MPI_File_close( &_fd ), emFLPFB); }
protected:
    MPI_File _fd;
    size_t _n_elmt, _n_elmt_eachproc;
    vector<double> _data;
    Comm _comm;

    virtual double _run() override {
        ErrMPI::check(
            MPI_File_write_at_all( _fd, 0, _data.data(), _data.size(), 
                MPI_DOUBLE, MPI_STATUS_IGNORE ), emFLPFB);
        ErrMPI::check(
            MPI_File_read_at_all( _fd, 0, _data.data(), _data.size(), 
                MPI_DOUBLE, MPI_STATUS_IGNORE ), emFLPFB );
        return sizeof(double)*double(_data.size())*_comm.size();
    }
};

#endif	//_ADJUST_IO_TOOL_MPI_H_