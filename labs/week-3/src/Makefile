CXX = g++
MPICXX = mpicxx
CXXFLAGS = -std=c++11 -O3 -Wall
LDFLAGS = 
LDLIBS = -lmymathCC -lhippio -lhippcntl -lhdf5
MPILDLIBS = -lmymathCC -lhippmpi -lhippcntl -L${lib}/parallel/hdf5 -lhdf5

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)
%.mpout: %.cpp
	$(MPICXX) $(CXXFLAGS) -DUSE_MPI $(LDFLAGS) -o $@ $< $(MPILDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
