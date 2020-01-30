#include "pi_linuxfork.h"

int main(int argc, char const *argv[]){
    double ans, err, t, t_err;
    {
        PI_Linuxfork_Quadrature quad( 8, 1024*1024*256 );

        quad.run();
        quad.ans(ans, err);
        quad.timing(t, t_err);
        cout << ans << ' ' << err << ", " << t << ' ' << t_err << endl;
    }
    
    return 0;
}