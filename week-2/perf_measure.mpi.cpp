#include "pi_omp.h"
#include "pi_pthreads.h"
#include "pi_linuxfork.h"
#ifdef USE_MPI
#include "pi_mpi.h"
#endif

#include "ReadCmdList.h"

int main(int argc, char **argv){

#ifdef USE_MPI
    HIPP::MPI::Env env(argc, argv);
    auto commw = env.world();
#endif
    MYFILE::ReadCmdList cmdl(argc, (const char **)argv);
    string parallel_type, method;
    string out_filename;
    cmdl.read(parallel_type, method, out_filename);
    cmdl.show_info();
    ofstream out_file;
#ifdef USE_MPI
    if( commw.rank() == 0 )
#endif
    out_file.open( out_filename.c_str() );
    
    vector<size_t> n_procs = {1, 2, 4, 8, 16, 32, 64, 128},
        n_points = { 16, 64, 256, 1024, 4096, 1024*16, 1024*64, 1024*256, 
            1024*1024 };
    for(auto &i: n_points) i *= 1024;
    size_t fix_nproc = 16, fix_npoints = 1024*1024*size_t(16);

    for(const auto &n_proc: n_procs){
        Task *task;
#ifdef USE_MPI
        auto comm = commw.split( commw.rank() < n_proc?1:MPI_UNDEFINED, 0 );
        if( ! comm.is_null() ){
            if( method == "quad" )
                task = new PI_MPI_Quadrature( comm, fix_npoints, 20 );
            else if( method == "mc" ){
                task = new PI_MPI_MonteCarlo( comm, fix_npoints, 20 );
            }else{
                HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);    
            }
#else
        if( parallel_type == "omp" ){
            if( method == "quad" )
                task = new PI_Omp_Quadrature( n_proc, fix_npoints, 20 );
            else if( method == "mc" ){
                task = new PI_Omp_MonteCarlo( n_proc, fix_npoints, 20 );
            }else{
                HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);    
            }
        }else if( parallel_type == "pthreads" ){
            task = new PI_Pthreads_Quadrature( n_proc, fix_npoints, 20 );
        }else if( parallel_type == "linuxfork" ){
            task = new PI_Linuxfork_Quadrature( n_proc, fix_npoints, 20 );
        }else{
            HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);
        }
#endif
        task->run();
        double ans, err, t, t_err;
        task->ans(ans, err);
        task->timing(t, t_err);
#ifdef USE_MPI
        if( comm.rank() == 0 )
#endif
        out_file << n_proc << '\t' << fix_npoints << '\t' 
            << ans << '\t' << err << '\t' << t << '\t' << t_err << endl;
        delete task;
#ifdef USE_MPI
        }
#endif
    }

    for(const auto &n_point: n_points){
        Task *task;
#ifdef USE_MPI
        auto comm = commw.split( commw.rank() < fix_nproc?1:MPI_UNDEFINED, 0 );
        if( ! comm.is_null() ){
            if( method == "quad" ){
                task = new PI_MPI_Quadrature( comm, n_point, 20 );
            }else if( method == "mc" ) {
                task = new PI_MPI_MonteCarlo( comm, n_point, 20 );
            }else
                HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);    
#else
        if( parallel_type == "omp" ){
            if( method == "quad" ){
                task = new PI_Omp_Quadrature( fix_nproc, n_point, 20 );
            }else if( method == "mc" ) {
                task = new PI_Omp_MonteCarlo( fix_nproc, n_point, 20 );
            }else{
                HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG);    
        }
        }else if( parallel_type == "pthreads" ){
            task = new PI_Pthreads_Quadrature( fix_nproc, n_point, 20 );
        }else if( parallel_type == "linuxfork" ){
            task = new PI_Linuxfork_Quadrature( fix_nproc, n_point, 20 );
        }
#endif
        task->run();
        double ans, err, t, t_err;
        task->ans(ans, err);
        task->timing(t, t_err);
#ifdef USE_MPI
    if( comm.rank() == 0 )
#endif
        out_file << fix_nproc << '\t' << n_point << '\t' 
            << ans << '\t' << err << '\t' << t << '\t' << t_err << endl;
        delete task;
#ifdef USE_MPI
        }
#endif
    }

    out_file.close();

    return 0;
}
