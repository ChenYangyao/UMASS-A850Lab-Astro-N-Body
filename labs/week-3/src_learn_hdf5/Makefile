CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall
LDFLAGS = 
LDLIBS = -lmymathCC -lelucid -lhippio -lhippcntl -lhdf5

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
