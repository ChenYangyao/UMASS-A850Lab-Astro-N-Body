#ifndef _PI_MPI_H_
#define _PI_MPI_H_
#include "task.h"
#include <hippmpi.h>
#include <my_random.h>

class PI_MPI_Quadrature: public Task{
public:
    typedef HIPP::MPI::Comm comm_t;

    PI_MPI_Quadrature( comm_t comm, size_t n_step, size_t n_repeat = 100 )
        : _comm(comm), _n_step(n_step)
        { _n_repeat = n_repeat; chk_divisible( _n_step, comm.size() ); }
    virtual ~PI_MPI_Quadrature() noexcept{}
protected:
    comm_t _comm;
    size_t _n_step;
    virtual double _run( ) override{
        size_t rank = _comm.rank(), size = _comm.size();
        size_t _n_step_eachproc = _n_step / size,
            b = _n_step_eachproc*rank, e = b + _n_step_eachproc;
        double h = 1.0 / _n_step, sum = 0.;
        for(size_t i=b; i<e; ++i){
            sum += integrand( (i + 0.5)*h );
        }
        double ans = 0.;
        _comm.reduce( &sum, &ans, 1, HIPP::MPI::DOUBLE, "+", 0 );
        return ans * 4.0 * h;
    }
};

class PI_MPI_MonteCarlo: public Task{
public:
    typedef HIPP::MPI::Comm comm_t;

    PI_MPI_MonteCarlo( comm_t comm, size_t n_step, size_t n_repeat = 100 )
        : _comm(comm), _n_step(n_step), _rng(-1.0, 1.0, comm.rank())
        { _n_repeat = n_repeat; chk_divisible( _n_step, comm.size() ); }
    virtual ~PI_MPI_MonteCarlo() noexcept{}
protected:
    comm_t _comm;
    size_t _n_step;
    MYMATH::RandomsFloat<> _rng;

    virtual double _run( ) override{
        size_t size = _comm.size();
        size_t _n_step_eachproc = _n_step / size, sum_hit = 0;
        for(size_t i=0; i<_n_step_eachproc; ++i){
            double x = _rng(), y = _rng();
            sum_hit += in_circle(x, y);
        }
        double inbuf = sum_hit, outbuf;
        _comm.reduce( &inbuf, &outbuf, 1, HIPP::MPI::DOUBLE, "+", 0 );
        return outbuf*4.0/double(_n_step);
    }
};

#endif	//_PI_MPI_H_