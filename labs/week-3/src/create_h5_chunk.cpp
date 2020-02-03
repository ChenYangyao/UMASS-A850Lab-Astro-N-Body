#include <hippio.h>
#include <mystd.h>
using namespace MYSTD;
using namespace HIPP::IO;

int main(int argc, char const *argv[])
{
    H5File file("../out/tempdump.chunk", "w");

    auto dcpl = H5Dataset::create_proplist( "c" );
    dcpl.set_chunk( {32, 32} );
    
    auto dset = file.create_dataset<double>( "/temp", {128*32, 128*32}, "excl",
        H5Proplist::defaultval, dcpl );

    dset.write( vector<double>(128*32*128*32, 1.0) );

    return 0;
}


