#ifndef _PI_PTHREADS_H_
#define _PI_PTHREADS_H_
#include "task.h"
#include <thread>
#include <future>

class PI_Pthreads_Quadrature: public Task{
public:
    PI_Pthreads_Quadrature( size_t n_thread, size_t n_step, size_t n_repeat = 100 )
        :_n_thread( n_thread ), _n_step(n_step)
        {   _n_repeat = n_repeat;
            chk_divisible( _n_step, _n_thread ); 
        }
    virtual ~PI_Pthreads_Quadrature(){}
protected:
    size_t _n_thread, _n_step;
    virtual double _run( ) override{
        auto each_task = [this](size_t tid)->double{
            size_t _n_step_per_th = this->_n_step / this->_n_thread,
                b = tid * _n_step_per_th, e = b + _n_step_per_th;
            double sum = 0.;
            double h = 1.0/this->_n_step;
            for(size_t i=b; i<e; ++i){
                sum += integrand( (i + 0.5)*h );    
            }
            return sum * 4.0 * h;
        };
        vector<std::future<double> > _tasks; 
        for(size_t i=1; i<_n_thread; ++i) 
            _tasks.emplace_back( std::async(each_task, i) );
        double sum = each_task(0);
        for(auto &t: _tasks) sum += t.get();
        return sum;
    }
};


#endif	//_PI_PTHREADS_H_