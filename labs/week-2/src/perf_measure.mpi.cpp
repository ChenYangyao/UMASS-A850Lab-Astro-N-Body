#include "pi_mpi.h"
#include "ReadCmdList.h"

int main(int argc, char **argv){
    HIPP::MPI::Env env( argc, argv );
    auto comm = env.world();

    MYFILE::ReadCmdList cmdl(argc, (const char **)argv);
    string method;
    size_t npoints;
    cmdl.read(method, npoints);
    cmdl.show_info( cerr, 0 );

    Task *task;
    if( method == "quad" )
        task = new PI_MPI_Quadrature( comm, npoints, 20 );
    else if( method == "mc" )
        task = new PI_MPI_MonteCarlo( comm, npoints, 20 );
    else
        HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG,
            emFLPFB, " ... method ", method);    
    
    task->run();
    if(  comm.rank() == 0 ){
        double ans, err, t, t_err;
        task->ans(ans, err);
        task->timing(t, t_err);
        printf("%.10g %.10g %.10g %.10g %.10g %.10g\n", 
            double(comm.size()), 
            double(npoints), 
            ans, err, t, t_err );
    }
    delete task;
    return 0;
}
