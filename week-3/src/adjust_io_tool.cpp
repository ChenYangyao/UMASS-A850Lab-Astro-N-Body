#include <stdio.h>
#include <ReadCmdList.h>
using namespace MYSTD;

#ifndef USE_MPI
#include "adjust_io_tool_formated.h"
#include "adjust_io_tool_binary.h"
#include "adjust_io_tool_h5.h"

using namespace MYSTD;

template<typename TaskT>
void dotask( string timingfile, size_t size, size_t repeat=10 ){
    string filedump = "../out/tempdump";
    Task *task = new TaskT( filedump, repeat, size );
    
    task->run();
    
    double t, terr;
    task->timing(t,terr);
    
    ofstream fout( timingfile, ios_base::out | ios_base::app );
    fout << task->io_size() << '\t' << t << '\t' << terr << endl; 
    
    delete task;
    remove( filedump.c_str() );
}

int main(int argc, char const *argv[]){
    size_t max_item;
    string outbase;
    MYFILE::ReadCmdList( argc, argv ).read(max_item, outbase);

    size_t item = 64;
    while( item <= max_item ){
        dotask<IO_H5>( outbase+ "/h5.timing", item );
        dotask<IO_Binary>( outbase+ "/binary.timing", item );
        
        if( item <= 2*1024 )
        dotask<IO_Formated>( outbase+ "/formated.timing", item, 10 );

        item *= 2;
    }

}
    
#else

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
        fout << task->io_size() << '\t' << t << '\t' << terr << endl; 
    }

    delete task;
    remove( filedump.c_str() );
}

int main(int argc, char *argv[]){
    HIPP::MPI::Env env( argc, argv );
    auto comm = env.world();

    size_t max_item;
    string outbase;
    MYFILE::ReadCmdList( argc, (const char **)argv ).read(max_item, outbase);

    size_t item = 64;
    while( item <= max_item ){
        dotask<IO_H5Parallel>( outbase+ "/h5parallel.timing", item, comm );
        dotask<IO_MPI>( outbase+ "/mpi.timing", item, comm );
        item *= 2;
    }
}

#endif	//USE_MPI

