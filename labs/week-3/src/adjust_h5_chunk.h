#ifndef _ADJUST_H5_CHUNK_H_
#define _ADJUST_H5_CHUNK_H_

#include "task.h"
#include <hippio.h>
#include <my_random.h>
using namespace HIPP::IO;


class IO_H5Chunk: public Task {
public:
    IO_H5Chunk( string filename, 
        int n_repeat, size_t io_nchunk,
        int io_schedule = 0,
        size_t io_chunksize = 32,
        size_t cache = 1024*1024, size_t nslots = 521, double w0 = 0.75,
        size_t chunksize = 32,
        size_t n_elmt = 128*32 )
        : _file(filename, "a"), _dset(NULL), 
            _memspace( {io_chunksize, io_chunksize} )
    {
        auto dapl = H5Dataset::create_proplist("a");
        dapl.set_chunk_cache( nslots, cache, w0 );
        _dset = _file.open_dataset( "/temp", dapl );

        _n_repeat = n_repeat;
        _io_nchunk = io_nchunk;

        _io_schedule = io_schedule;
        _io_chunksize = io_chunksize;

        _chunksize = chunksize;
        _n_elmt = n_elmt;

        _data.assign( io_chunksize*io_chunksize, 1.0 );
        if( _io_schedule == 0 || _io_schedule == 1 ){
            _rng = my_alloc<rng_t>( "rng", 0, n_elmt-io_chunksize );
        }else if( _io_schedule == 2 ){
            _locx = 0;
            _locy = 0;
            _rng = my_alloc<rng_t>( "rng", -1, 1 );
        }else
            HIPP::ErrLogic::throw_(HIPP::ErrLogic::eDOMAIN, emFLPFB, 
                "  invalid io schedule");
    }
    ~IO_H5Chunk(){  my_deAlloc( _rng ); }
protected:
    H5File _file;
    H5Dataset _dset;
    size_t _io_nchunk;

    int _io_schedule;
    size_t _io_chunksize;

    size_t _chunksize, _n_elmt;

    vector<double> _data;
    H5Dataspace _memspace;
    typedef MYMATH::RandomsInt<int> rng_t;
    rng_t *_rng;
    int _locx, _locy;
    
    virtual double _run() override {
        double _nbytes = 0.;
        for(size_t i=0; i<_io_nchunk; ++i){
            size_t x, y;
            next_loc(x,y);
            auto filespace = _dset.dataspace();
            filespace.select_hyperslab( {x,y}, {_io_chunksize, _io_chunksize} );
            _dset.write( _data, _memspace, filespace );
            _dset.read( _data, _memspace, filespace );
            _nbytes += sizeof(double)*_data.size();
        }
        return _nbytes;
    }
    void next_loc( size_t &x, size_t &y ){
        int _x = _rng->get(), _y = _rng->get();
        if( _io_schedule == 1 ){
            _x = (_x / _chunksize)*_chunksize;
            _y = (_y / _chunksize)*_chunksize;
        }else if( _io_schedule == 2 ){
            _x = _locx + _x*_io_chunksize;
            _y = _locy + _y*_io_chunksize;
            _x = (_x + _n_elmt - _io_chunksize) % (_n_elmt - _io_chunksize);
            _y = (_y + _n_elmt - _io_chunksize) % (_n_elmt - _io_chunksize);
            _locx = _x;
            _locy = _y;
        }
        x = _x;
        y = _y;
    }
};

#endif	//_ADJUST_H5_CHUNK_H_