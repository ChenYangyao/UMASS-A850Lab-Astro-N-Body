#include <mystd.h>
#include <omp.h>
#include <chrono>

using namespace MYSTD;
using namespace std::chrono;


int main(int argc, char const *argv[])
{

    double sum_shared = 0, sum_privated = 0;
    long n = 1000000000;

    auto t1 = steady_clock::now();
    #pragma omp parallel for default(none) shared(n) \
    private(sum_privated)
    for(long i=0; i<n; ++i){
        sum_privated += double(i);
    }
    auto t2 = steady_clock::now();

    #pragma omp parallel for default(none) shared(n, sum_shared)
    for(long i=0; i<n; ++i){
        sum_shared += double(i);
    }
    auto t3 = steady_clock::now();

    cout << duration_cast<duration<double>>(t2-t1).count()
        << '\t' << duration_cast<duration<double>>(t3-t2).count() << endl;

    return 0;
}

