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
    virtual void run( ) = 0;
    virtual void ans( double &value, double &err ) = 0;
    virtual void timing( double &value, double &err ) = 0;
    virtual ~Task(){ }
protected:
    static void chk_divisible( size_t n_task, size_t n_thread ){
        if( n_task % (4*n_thread) != 0 )
            HIPP::ErrLogic::throw_( HIPP::ErrLogic::eINVALIDARG, 
                emFLPFB, "  ... n_task ", n_task, " is not divisible by ", 
                n_thread ," * ", 4 );
    }
    static double integrand( double x ){
        return 1.0 / ( 1.0 + x*x );
    }
    static bool in_circle( double x, double y ){
        return x*x + y*y < 1.0;
    }
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