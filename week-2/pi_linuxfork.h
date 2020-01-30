#ifndef _PI_LINUXFORK_H_
#define _PI_LINUXFORK_H_
#include "task.h"
#include <mysys.h>
#include <mysys_ipc.h>

class PI_Linuxfork_Quadrature: public Task{
public:
    PI_Linuxfork_Quadrature( size_t n_proc, size_t n_step, 
        size_t n_repeat = 100 )
        :_n_proc( n_proc ), _n_step(n_step)
        { _n_repeat = n_repeat; chk_divisible( _n_step, _n_proc ); }
    virtual ~PI_Linuxfork_Quadrature(){}
protected:
    size_t _n_proc, _n_step;
    virtual double _run( ) override{
        // task to be executed at each process.
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

        // fork processes. Also open an message queue for passing result from
        // children to parent.
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
};


#endif	//_PI_LINUXFORK_H_