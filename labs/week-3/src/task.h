#ifndef _TASK_H_
#define _TASK_H_
#include <hippcntl.h>
#include <mystd.h>
#include <mystats.h>
#include <chrono>

using namespace MYSTD;
using namespace std::chrono;

class Task{
public:
    void run(){
        vector<double> t(_n_repeat);
        _io_size = 0.;
        for(size_t i=0; i<_n_repeat; ++i){
            auto t1 = steady_clock::now();
            _io_size += _run();
            auto t2 = steady_clock::now();
            t[i] = duration_cast<duration<double>>(t2-t1).count();
        }
        stats( t, _t, _t_err );
        _io_size /= _n_repeat;
    }
    void timing( double &value, double &err ) const noexcept
        { value = _t; err = _t_err; }
    double io_size() noexcept {  return _io_size;  }
    virtual ~Task() noexcept{ }
protected:
    size_t _n_repeat;
    double _t, _t_err;
    double _io_size;

    virtual double _run() = 0;
    static void stats( const vector<double> &values, double &mean, double &sd ){
        if( values.size() == 1 ){
            mean = values[0];
            sd = 0.;
            return;
        }
        mean = MYMATH::mean( values.data(), values.size()) ;
        sd = MYMATH::sd( values.data(), values.size()) ;
    }
};

#endif	//_TASK_H_