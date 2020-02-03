#ifndef _ADJUST_IO_TOOL_H5_H_
#define _ADJUST_IO_TOOL_H5_H_

#include "task.h"
#include <hippio.h>
using namespace HIPP::IO;

class IO_H5: public Task {
public:
    IO_H5( string filename, int n_repeat, size_t n_elmt )
        :_file( filename, "w" ), _dset(NULL)
    {
        _n_repeat = n_repeat;
        _n_elmt = n_elmt;
        _data.assign( _n_elmt * _n_elmt, 1.0 );

        _dset = _file.create_dataset<double>( 
            "/temp", {n_elmt, n_elmt} );
    }
    ~IO_H5(){}
protected:
    H5File _file;
    H5Dataset _dset;
    size_t _n_elmt;
    vector<double> _data;
    virtual double _run() override {
        _dset.write( _data );
        _dset.read( _data );
        return sizeof(double)*double(_n_elmt)*_n_elmt;
    }
};


#endif	//_ADJUST_IO_TOOL_H5_H_