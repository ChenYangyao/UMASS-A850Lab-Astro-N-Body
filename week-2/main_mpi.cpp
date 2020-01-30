#include "pi_mpi.h"

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto comm = env.world();
    double ans, err, t, t_err;
    {
        PI_MPI_Quadrature mpi_quad( comm, 1024*1024 );

        mpi_quad.run();
        if( comm.rank() == 0 ){
            mpi_quad.ans(ans, err);
            mpi_quad.timing(t, t_err);

            cout << ans << ' ' << err << ", " << t << ' ' << t_err << endl;
        }
    }

    {
        PI_MPI_MonteCarlo mpi_mc( comm, 1024*1024 );

        mpi_mc.run();
        if( comm.rank() == 0 ){
            mpi_mc.ans(ans, err);
            mpi_mc.timing(t, t_err);

            cout << ans << ' ' << err << ", " << t << ' ' << t_err << endl;
        }
    }
    
    return 0;
}