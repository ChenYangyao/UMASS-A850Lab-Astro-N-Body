#include <mystd.h>
#include <hippio.h>
#include <modeltree_walker.h>
using namespace MYSTD;
using namespace HIPP::IO;

struct halo_t{
    double mass, rvir, pos[3];
    long long boundid;
    halo_t( double _m, double _r, const  float*_p, long long _bid )
        :mass(_m), rvir(_r), boundid(_bid){ std::copy(_p, _p+3, pos); }
};
struct galaxy_t{
    double mass, sfr;
    galaxy_t( double _m, double _sfr )
        :mass(_m), sfr(_sfr){ }
};


int main(int argc, char const *argv[]){
    /**
     * load the data from original binary catalog
     */
    string catalogdir = "/scratch/yangyao/tree/",
        listfbase = "/scratch/yangyao/tree/List_ModelTree",
        grplistfname = listfbase + "_grp",
        gallistfname = listfbase + "_gal",
        mocklistfname = listfbase + "_mock",
        infolistfname = listfbase + "_info";

    ELUCID::ModelTree_Walker wk( catalogdir, grplistfname, gallistfname, 
        mocklistfname, infolistfname );

    vector<halo_t> halos;
    vector<galaxy_t> gals; 
    int cnt_halos = 0;
    while (wk.next_tree( false, true, true, false, true )){
        const auto &htr = wk.get_halotree();
        const auto &grptr = wk.get_grptree();
        const auto &galtr = wk.get_galtree();
        auto &grp = grptr[ grptr.rootgroup ];
        auto &gal = galtr[ galtr.rootgal ];
        auto &halo = htr[ grp.haloID ];
        halos.emplace_back( grp.haloMass, 
            ELUCID::Cosmo.R_virial( grp.haloMass, 0. ), 
            halo.Pos, halo.MostBoundID );
        gals.emplace_back( gal.mass_approx, gal.sfr );
        if( ++cnt_halos > 1000 ) break;
    }

    auto &rs = ELUCID::Rs;
    size_t nsnap = rs.LAST_SNAP+1;
    

    /**
     * output as HDF5 format
     */
    
    H5File file("Obj.EmpiricalModel.N3072.L500", "w");
    auto grpcosm = file.create_group("/cosmology"), 
        grphalo = file.create_group("/halos"), 
        grpgal = file.create_group("/galaxies");

    /**
     * output the cosmology imformation
     */
    vector<double> v; vector<long long> vll;
    vector<vector<string> > descrpts = { {"Reshift z"}, {"Scale factor a"} ,
        {"Cosmic time [Gyr/h]"} };
    vector<string> names = {"redshift", "scale-factor", "cosmic-time"};
#define write_cosm(key, id) { \
    auto dset = grpcosm.create_dataset<double>( names[id], {nsnap} );\
    dset.write( rs.key );\
    dset.create_attr<string>( "description", H5TypeStr::shape( descrpts[id] ))\
        .write( descrpts[id] );\
}
    write_cosm(redshift, 0)
    write_cosm(scale_factor, 1)
    write_cosm(time, 2)

    /**
     * output the halo catalog
     */
#define vectorize( vec, key, out ) {\
    out.resize( vec.size() );\
    for(size_t i=0; i<vec.size(); ++i){\
        out[i] = vec[i].key;\
    }\
}
#define vectorize3( vec, x, y, z, out ) {\
    out.resize( vec.size()*3 );\
    for(size_t i=0; i<vec.size(); ++i){\
        out[3*i] = vec[i].x;\
        out[3*i+1] = vec[i].y;\
        out[3*i+2] = vec[i].z;\
    }\
}
    descrpts = { {"halo mass [10^10 Mpc/h]"},{"virial radius [Mpc/h]"},
        {"coordiantes (x,y,z) [Mpc/h]"},
        {"most bound particle id"} };
    names = { "halo-mass", "rvir", "coord", "bound-id"};
#define write_halo(key, out, type, id){\
    vectorize(halos, key, out)\
    auto dset = grphalo.create_dataset<type>( names[id], { halos.size() } );\
    dset.write(out);\
    dset.create_attr<string>( "description", H5TypeStr::shape( descrpts[id] ) )\
        .write( descrpts[id] );\
}
    write_halo(mass, v, double, 0)
    write_halo(rvir, v, double, 1)
    vectorize3(halos, pos[0], pos[1], pos[2], v)
    {
        auto dset = grphalo.create_dataset<double>( 
            names[2], { halos.size(), 3 } );
        dset.write(v);
        dset.create_attr<string>("description", H5TypeStr::shape( descrpts[2] ))
            .write( descrpts[2] );
    }
    write_halo(boundid, vll, long long, 5)

    /**
     * output the galaxy catalog
     */
    descrpts = { {"stellar mass [10^10 Mpc/h]"},
        {"star formation rate [ 10 Msun/yr ]"} };
    names = { "stellar-mass", "SFR"};
#define write_galaxy(key, out, type, id){\
    vectorize(gals, key, out)\
    auto dset = grpgal.create_dataset<type>( names[id], { gals.size() } );\
    dset.write(out);\
    dset.create_attr<string>( "description", H5TypeStr::shape( descrpts[id] ) )\
        .write( descrpts[id] );\
}
    write_galaxy(mass, v, double, 0)
    write_galaxy(sfr, v, double, 1)


    return 0;
}
