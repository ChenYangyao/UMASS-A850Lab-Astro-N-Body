#include <stdio.h>
#include <ReadCmdList.h>
#include "adjust_io_tool_h5parallel.h"
#include "adjust_io_tool_mpi.h"

template<typename TaskT>
void dotask( string timingfile, size_t size, HIPP::MPI::Comm comm, 
    size_t repeat=10 )
{    
    string filedump = "../out/tempdump";
    Task *task = new TaskT( filedump, repeat, size, comm );
    
    task->run();

    if( comm.rank() == 0 ){
        double t, terr;
        task->timing(t,terr);
        ofstream fout( timingfile, ios_base::out | ios_base::app );
        fout << comm.size() << '\t' << task->io_size() << '\t' 
            << t << '\t' << terr << endl; 
    }
    delete task;
    remove( filedump.c_str() );
}

int main(int argc, char *argv[]){
    HIPP::MPI::Env env( argc, argv );
    auto comm = env.world();

    size_t item;
    string outbase;
    size_t minnproc, maxnproc;
    MYFILE::ReadCmdList( argc, (const char **)argv ).read(item, outbase, 
        minnproc, maxnproc);

    size_t nproc = minnproc;
    while( nproc <= maxnproc ){
        auto subcomm = 
            comm.split( size_t(comm.rank()) < nproc ? 1 : HIPP::MPI::UNDEFINED );
        if( ! subcomm.is_null() ){
            dotask<IO_H5Parallel>( outbase+ "/h5parallel.chnproc.timing", 
                item, subcomm, 5 );
            dotask<IO_MPI>( outbase+ "/mpi.chnproc.timing", item, 
                subcomm, 5 );
        }
        nproc *= 2;
    }
}


using namespace MYSTD;