#include "adjust_h5_chunk.h"
using namespace MYSTD;

void dotask( string timingfile, const vector<size_t> &iosize, 
    const vector<size_t> &cache, const vector<size_t> &nslots,
    const vector<double> &w0 ){
    FILE_OF(fout, timingfile)
    for(const auto &s: iosize)
    for(const auto &c: cache)
    for(const auto &n: nslots)
    for(const auto &w: w0)
    for(const auto &sched: {0, 1, 2}){
        Task * task = new IO_H5Chunk( 
            "../out/tempdump.chunk", 5, 512*32/s, sched, s, c, n, w );
        task->run();
        double t, terr;
        task->timing(t,terr);
        fout << task->io_size() << '\t' << t << '\t' << terr << '\t'
            << s << '\t' << c << '\t' << n << '\t' << w << '\t' << sched << endl;
        delete task;
    }
    fout.close();
}


int main(int argc, char const *argv[]){
    /*
    dotask( "../out/local/chunk.iosize", 
        { 4, 8, 12, 16, 24, 32, 
        40, 48, 64, 72, 84, 96, 108, 115, 128 }, {1024*1024}, {521}, {0.75} );*/
    dotask( "../out/local/chunk.cache", {32}, 
        {128*128, 256*256, 512*512, 1024*1024, 2048*2048, 4096*4096, 8192*8192}, 
        {521}, {0.75} );
    dotask( "../out/local/chunk.nslots", {32}, {1024*1024}, 
        {17, 31, 73, 97, 157, 331, 521, 709, 1061, 1367, 2341, 3221}, {0.75} );
    dotask( "../out/local/chunk.w0", {32}, {1024*1024}, {521}, 
        {0.0, 0.1, 0.2, 0.4, 0.6, 0.75, 0.8, 0.9, 1.0} );
    return 0;
}
