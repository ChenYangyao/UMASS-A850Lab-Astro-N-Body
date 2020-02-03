#ifndef _ADJUST_IO_TOOL_FORMATED_H_
#define _ADJUST_IO_TOOL_FORMATED_H_

#include "task.h"

class IO_Formated: public Task {
public:
    IO_Formated( string filename, int n_repeat, size_t n_elmt ){
        _file.open( filename.c_str(), ios_base::in | ios_base::out | 
            ios_base::trunc );
        if( !_file.is_open() )
            HIPP::ErrLogic::throw_( HIPP::ErrLogic::eDEFAULT, 
                "  ... open ", filename, " failed\n" );
        _n_repeat = n_repeat;
        _n_elmt = n_elmt;
        _data.assign( _n_elmt * _n_elmt, 1.0 );
    }
    ~IO_Formated(){ _file.close(); }
protected:
    std::fstream _file;
    size_t _n_elmt;
    vector<double> _data;
    virtual double _run() override {
        _file.seekp( ios_base::beg );
        for(const auto &x: _data){
            _file << x << ' ';
        }
        _file.seekg( ios_base::beg );
        for(auto &x: _data){
            _file >> x;
        }
        if( !_file )
            HIPP::ErrRuntime::throw_( 
                HIPP::ErrRuntime::eDEFAULT, "  ... IO failed\n" );
        return sizeof(double)*double( _data.size() );
    }
};

#endif	//_ADJUST_IO_TOOL_FORMATED_H_