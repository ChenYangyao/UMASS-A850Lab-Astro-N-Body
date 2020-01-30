CXX = g++
MPI_CXX = mpicxx
CXXFLAGS = -std=c++11 -O3 -Wall -fopenmp -pthread
LDFLAGS = 
LDLIBS = -lmymathCC -lhippcntl -lmysys -lrt
MPI_LDLIBS =  -lhippmpi

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

%.simdout: %.cpp
	$(CXX) $(CXXFLAGS) -mavx2 $(LDFLAGS) -o $@ $< $(LDLIBS)

%.mpout: %.cpp
	$(MPI_CXX) $(CXXFLAGS) -DUSE_MPI $(LDFLAGS) -o $@ $< $(MPI_LDLIBS) $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
