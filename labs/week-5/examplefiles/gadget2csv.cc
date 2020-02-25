// -*- C++ -*-
//
// Compile command for Supercloud:
// g++ -g -o gadget2csv -I/usr/include/hdf5/serial gadget2csv.cc -L/usr/lib64/hdf5/serial -lhdf5_cpp -lhdf5
//
// Compile command for Ubuntu:
// g++ -g -o gadget2csv -I/usr/include/hdf5/serial gadget2csv.cc -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5_cpp -lhdf5
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <set>

#include <H5Cpp.h>

#include <KDtree.H>

int main (int argc, char *argv[])
{
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " <type #> <start#> <end#>" << std::endl;
    return 1;
  }

  int type  = atoi(argv[1]);
  int start = atoi(argv[2]);
  int stop  = atoi(argv[3]);

  for (int ns=start; ns<=stop; ns++) {

    // Try to catch and HDF5 and parsing errors
    //
    try {
      // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    H5::Exception::dontPrint();
    
    std::ostringstream sfil;
    sfil << "snapshot_" << std::setw(3) << std::setfill('0') << ns << ".hdf5";

    const H5std_string FILE_NAME (sfil.str());
    const H5std_string GROUP_NAME_what ("/Header");
    
    H5::H5File    file( FILE_NAME, H5F_ACC_RDONLY );
    H5::Group     what(file.openGroup( GROUP_NAME_what ));

    double time, mass[6];

    {
      H5::Attribute attr(what.openAttribute("Time"));
      H5::DataType  type(attr.getDataType());
    
      attr.read(type, &time);
    }

    {
      H5::Attribute attr(what.openAttribute("MassTable"));
      H5::DataType  type(attr.getDataType());
    
      attr.read(type, mass);
    }
    
    for (int i=0; i<6; i++)
      if (mass[i]>0.0) {
	std::ostringstream sout;
	sout << "PartType" << i;
	std::cout << sout.str()  << " at time " << time << ": mass=" << mass[i] << std::endl;
	
	if (i == type) {

	  std::string grpnam = "/" + sout.str();
	  H5::Group grp(file.openGroup(grpnam));
	  H5::DataSet dataset = grp.openDataSet("Coordinates");
	  H5::DataSpace dataspace = dataset.getSpace();
	
	  // Get the number of dimensions in the dataspace.
	  //
	  int rank = dataspace.getSimpleExtentNdims();
	  
	  // Get the dimension size of each dimension in the dataspace and
	  // display them.
	  //
	  hsize_t dims[2];
	  int ndims = dataspace.getSimpleExtentDims( dims, NULL);
	  std::cout << "rank " << rank << ", dimensions " <<
	    (unsigned long)(dims[0]) << " x " <<
	    (unsigned long)(dims[1]) << std::endl;
	  
	  // Define the memory space to read dataset.
	  //
	  H5::DataSpace mspace(rank, dims);
	  
	  std::vector<float> buf(dims[0]*dims[1]);
	  dataset.read(&buf[0], H5::PredType::NATIVE_FLOAT, mspace, dataspace );
	  
	  // Compute density based on N-pt balls
	  //
	  const   int Npt = 8;
	  typedef point <double, 3> point3;
	  typedef kdtree<double, 3> tree3;

	  std::vector<point3> points;
	  for (int k=0; k<dims[0]; k++) {
	    points.push_back({buf[k*3+0], buf[k*3+1], buf[k*3+2]});
	  }

	  tree3 tree(points.begin(), points.end());

	  std::vector<double> density;
	  std::set<double> distinct;
	  for (int k=0; k<dims[0]; k++) {
	    auto ret = tree.nearestN(points[k], Npt);

	    /*
	    std::cout << "Distances:";
	    for (auto v : tree.getDist()) std::cout << " " << v;
	    std::cout << std::endl;
	    std::cout << "Cur point/nearest point: "
		      << std::sqrt(points[k].distance(std::get<1>(ret)))
		      << ", " << std::get<2>(ret) << std::endl;
	    std::cout << points[k] << std::endl;
	    std::cout << std::get<1>(ret) << std::endl;
	    */
	      
	    double volume = 4.0*M_PI/3.0*std::pow(std::get<2>(ret), 3.0);
	    distinct.insert(volume);
	    if (volume>0.0)
	      density.push_back(mass[i]*Npt/volume);
	    else
	      density.push_back(1.0e-18);
	  }

	  std::cout << distinct.size() << "/" << density.size()
		    << " distinct values" << std::endl;

	  // CSV output
	  //
	  std::ostringstream sout;
	  sout << "type_" << type << "-"
	       << std::setw(3) << std::setfill('0') << ns << ".csv";
	  std::ofstream out(sout.str());

	  out << std::setw(16) << "X" << ","
	      << std::setw(16) << "Y" << ","
	      << std::setw(16) << "Z" << ", "
	      << std::setw(16) << "Density" << std::endl;

	  for (int k=0; k<dims[0]; k++) {
	    out << std::setw(16) << buf[k*3+0] << ","
		<< std::setw(16) << buf[k*3+1] << ","
		<< std::setw(16) << buf[k*3+2] << ","
		<< std::setw(16) << density[k] << std::endl;
	  }
	}
      }
    // end particle type loop

  }
  // end of try block
  
  // catch failure caused by the H5File operations
  catch(H5::FileIException error)
    {
      error.printErrorStack();
      return -1;
    }
  
  // catch failure caused by the DataSet operations
  catch(H5::DataSetIException error)
    {
      error.printErrorStack();
      return -1;
    }
  
  // catch failure caused by the DataSpace operations
  catch(H5::DataSpaceIException error)
    {
      error.printErrorStack();
      return -1;
    }

  }
  // Snapshot loop
  
  return 0;  // successfully terminated
}

