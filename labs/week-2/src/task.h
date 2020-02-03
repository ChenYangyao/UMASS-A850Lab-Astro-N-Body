#ifndef _TASK_H_
#define _TASK_H_
#include <hippcntl.h>
#include <mystd.h>
#include <chrono>
#include <mystats.h>

using namespace MYSTD;
using namespace std::chrono;

class Task{
public:
    typedef std::size_t size_t;
    /**
     * run the algorithm multiple times, measure the mean and standard deviation
     * of PI and time cost.
     * 
     * The result of PI is returned by ans(), while the time cost is returned
     * by timing().
     * 
     * Derived classes may overload _run() to implement the real algorithm.
     */
    void run( ){
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
    void ans( double &value, double &err ) const noexcept
        { value = _answer; err = _err; }
    void timing( double &value, double &err ) const noexcept
        { value = _t; err = _t_err; }
    virtual ~Task() noexcept{ }
protected:
    virtual double _run() = 0;
    /**
     * in parallel implementation of the algorihtm, we require the number of
     * tasks is divisible by number of threads * 4, or number of process * 4.
     * This is only for clarity of implementation.
     */
    static void chk_divisible( size_t n_task, size_t n_thread ){
        if( n_task % (4*n_thread) != 0 )
            HIPP::ErrLogic::throw_( HIPP::ErrLogic::eINVALIDARG, 
                emFLPFB, "  ... n_task ", n_task, " is not divisible by ", 
                n_thread ," * ", 4 );
    }
    static double integrand( double x ) noexcept {
        return 1.0 / ( 1.0 + x*x );
    }
    static bool in_circle( double x, double y ) noexcept {
        return x*x + y*y < 1.0;
    }
    /**
     * give the mean and standard deviation of a vector or values.
     */
    static void stats( const vector<double> &values, double &mean, double &sd ){
        if( values.size() == 1 ){
            mean = values[0];
            sd = 0.;
            return;
        }
        mean = MYMATH::mean( values.data(), values.size()) ;
        sd = MYMATH::sd( values.data(), values.size()) ;
    }

    size_t _n_repeat;
    double _answer, _err;
    double _t, _t_err;
};


#endif	//_TASK_H_