#include "pi_omp.h"

int main(int argc, char const *argv[]){

    double ans, err, t, t_err;
    {
        PI_Omp_Quadrature omp_quad( omp_get_max_threads(), 1024*1024*256 );

        omp_quad.run();
        omp_quad.ans(ans, err);
        omp_quad.timing(t, t_err);

        cout << ans << ' ' << err << ", " << t << ' ' << t_err << endl;
    }

    {
        PI_Omp_MonteCarlo omp_mc( omp_get_max_threads(), 1024*1024 );

        omp_mc.run();
        omp_mc.ans(ans, err);
        omp_mc.timing(t, t_err);

        cout << ans << ' ' << err << ", " << t << ' ' << t_err << endl;
    }

    
    return 0;
}