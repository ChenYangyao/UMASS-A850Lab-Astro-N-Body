#include "pi_omp.h"
#include "pi_pthreads.h"
#include "pi_linuxfork.h"
#include "ReadCmdList.h"

int main(int argc, char **argv){

    MYFILE::ReadCmdList cmdl(argc, (const char **)argv);
    string parallel_type, method;
    size_t nproc, npoints;
    cmdl.read(parallel_type, method, nproc, npoints);

    Task *task;
    if( parallel_type == "omp" ){
        if( method == "quad" )
            task = new PI_Omp_Quadrature( nproc, npoints, 20 );
        else if( method == "mc" )
            task = new PI_Omp_MonteCarlo( nproc, npoints, 20 );
        else
            HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);    
    }else if( parallel_type == "pthreads" ){
        task = new PI_Pthreads_Quadrature( nproc, npoints, 20 );
    }else if( parallel_type == "linuxfork" ){
        task = new PI_Linuxfork_Quadrature( nproc, npoints, 20 );
    }else
        HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);
    
    task->run();
    double ans, err, t, t_err;
    task->ans(ans, err);
    task->timing(t, t_err);
    MYFILE::width_prt(
        cout, 15, ' ', nproc, npoints, ans, err, t, t_err ) << endl;
    delete task;

    return 0;
}
