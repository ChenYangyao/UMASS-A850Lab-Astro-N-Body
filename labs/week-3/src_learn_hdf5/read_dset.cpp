#include <mystd.h>
#include <hippio.h>
#include <modeltree_walker.h>
using namespace MYSTD;
using namespace HIPP::IO;

int main(int argc, char const *argv[]){
    H5File file("Obj.EmpiricalModel.N3072.L500", "r");

    vector<double> mass, rvir;
    file.open_dataset("/halos/halo-mass").read( mass );
    file.open_dataset("/halos/rvir").read( rvir );
    
    cout << "Example halos: mass, rvir" << endl;
    for(int i=0; i<10; ++i){
        cout << mass[i] << "\t" << rvir[i] << endl;
    }

    return 0;
}
