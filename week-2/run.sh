#! /bin/bash

./perform_measure.out omp quad "out/omp.quad"
./perform_measure.out omp mc "out/omp.mc"
./perform_measure.out pthreads quad "out/pthreads.quad"
./perform_measure.out linuxfork quad "out/linuxfork.quad"

mpirun -n 128 ./perform_measure.out mpi quad "out/mpi.quad"
mpirun -n 128 ./perform_measure.out mpi mc "out/mpi.quad"