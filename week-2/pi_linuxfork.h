#ifndef _PI_LINUXFORK_H_
#define _PI_LINUXFORK_H_
#include "task.h"
#include <mysys.h>
#include <mysys_ipc.h>

class PI_Linuxfork_Quadrature: public Task{
public:
    PI_Linuxfork_Quadrature( size_t n_proc, size_t n_step, size_t n_repeat = 100 )
        :_n_proc( n_proc ), _n_step(n_step), _n_repeat(n_repeat)
        { chk_divisible( _n_step, _n_proc ); }
    virtual void run( ) override;
    virtual void ans( double &value, double &err ) override 
        { value = _answer; err = _err; }
    virtual void timing( double &value, double &err ) override
        { value = _t; err = _t_err; }

    virtual ~PI_Linuxfork_Quadrature(){}
protected:
    size_t _n_proc, _n_step, _n_repeat;

    double _answer, _err;
    double _t, _t_err;
    double _run( );
};


inline void PI_Linuxfork_Quadrature::run( ){
    vector<double> ans(_n_repeat), t(_n_repeat);
    for(size_t i=0; i<_n_repeat; ++i){
        auto t1 = steady_clock::now();
        ans[i] = _run();
        auto t2 = steady_clock::now();
        t[i] = duration_cast<duration<double>>(t2-t1).count();
    }
    stats( ans, _answer, _err );
    stats( t, _t, _t_err );
}
inline double PI_Linuxfork_Quadrature::_run(){
    auto each_task = [this](size_t pid)->double{
        size_t _n_step_per_th = this->_n_step / this->_n_proc,
            b = pid * _n_step_per_th, e = b + _n_step_per_th;
        double sum = 0.;
        double h = 1.0/this->_n_step;
        for(size_t i=b; i<e; ++i){
            sum += integrand( (i + 0.5)*h );    
        }
        return sum * 4.0 * h;
    };

    vector<int> pids;
    MYSYS::MQueue mq("/pi_linuxfork", "rw");
    for(size_t i=1; i<_n_proc; ++i){
        int pid = fork();
        if( pid == 0 ){
            double sum = each_task(i);
            mq.write( &sum, 1 );
            exit(EXIT_SUCCESS);
        }else if( pid == -1 )
            HIPP::ErrSystem::throw_( pid, emFLPFB, "  ... fork failed");
    }
    mq.set_own();
    double sum = each_task(0);
    for(size_t i=1; i<_n_proc; ++i){
        double buf;
        mq.read(&buf, 1);
        sum += buf;
    }
    return sum;
}


#endif	//_PI_LINUXFORK_H_