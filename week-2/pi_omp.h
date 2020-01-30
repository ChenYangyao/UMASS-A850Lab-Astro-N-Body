#ifndef _PI_OMP_H_
#define _PI_OMP_H_
#include "task.h"
#include <omp.h>
#include <my_random.h>
#include <x86intrin.h>

class PI_Omp_Quadrature: public Task{
public:
    PI_Omp_Quadrature( size_t n_thread, size_t n_step, size_t n_repeat = 100 )
        :_n_thread( n_thread ), _n_step(n_step)
        { _n_repeat = n_repeat; chk_divisible( _n_step, _n_thread ); }
    virtual ~PI_Omp_Quadrature() noexcept{}
protected:
    size_t _n_thread, _n_step;

    virtual double _run( ) override{
        double h = 1.0 / _n_step, sum = 0.;
#ifndef __AVX2__
        #pragma omp parallel for default(none)\
            shared(_n_step, h) reduction(+:sum) num_threads(_n_thread)
        for(size_t i=0; i<_n_step; ++i){
            sum += integrand( (i + 0.5)*h );
        }
#else
        __m256d vsum = _mm256_setzero_pd(), 
            stride = _mm256_set_pd( 0.5*h, 1.5*h, 2.5*h, 3.5*h ),
            ones = _mm256_set1_pd(1.0);
        #pragma omp parallel default(none)\
            shared(_n_step, h, stride, ones, vsum) num_threads(_n_thread)
        {
            __m256d localsum = _mm256_setzero_pd();
            #pragma omp for
            for(size_t i=0; i<_n_step; i+=4){
                __m256d x = _mm256_add_pd( _mm256_set1_pd( i*h ), stride );
                __m256d y = _mm256_div_pd( ones, 
                    _mm256_add_pd( ones, _mm256_mul_pd(x,x) ) );
                localsum = _mm256_add_pd(localsum, y);
            }
            #pragma omp critical
            {
                vsum = _mm256_add_pd( vsum, localsum );
            }
        }
        double *buf = (double *)&vsum;
        sum += buf[0] + buf[1] + buf[2] + buf[3];
#endif
        return sum * 4.0 * h;
    }
};


class PI_Omp_MonteCarlo: public Task{
public:
    PI_Omp_MonteCarlo( size_t n_thread, size_t n_step, size_t n_repeat = 100 )
        :_n_thread( n_thread ), _n_step(n_step)
    { 
        _n_repeat = n_repeat;
        chk_divisible( _n_step, _n_thread );
        for(size_t i=0; i<n_thread; ++i)
            _rng.emplace_back( -1.0, 1.0, i );
    }
    virtual ~PI_Omp_MonteCarlo() noexcept{}
protected:
    size_t _n_thread, _n_step;
    vector<MYMATH::RandomsFloat<> > _rng;
    
    virtual double _run( ) override{
        size_t sum_hit = 0;
        #pragma omp parallel default(none)\
            shared(_n_step) reduction(+:sum_hit) num_threads(_n_thread)
        {
            auto & rng = _rng[ omp_get_thread_num() ];
            #pragma omp for
            for(size_t i=0; i<_n_step; ++i){
                double x = rng(), y = rng();
                sum_hit += in_circle( x, y );
            }
        }
        return sum_hit*4.0/double(_n_step);
    }
};

#endif	//_PI_OMP_H_