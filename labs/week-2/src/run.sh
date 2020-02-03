#! /bin/bash

nproc=(1 2 4 8 16 32 64 4 4 4 4 4 4 4 4 4 4 4 4 4)
ntask=(134217728 134217728 134217728 134217728 134217728 134217728 134217728 16 32 128 512 2048 8192 32768 131072 524288 2097152 8388608 33554432 134217728 536870912)

int=0
while [ $int -lt 20 ]
do
    echo $int
    ./perf_measure.out "omp" "quad" ${nproc[$int]} ${ntask[$int]} >> out/omp.quad 
    ./perf_measure.out "omp" "mc" ${nproc[$int]} ${ntask[$int]} >> out/omp.mc 
    ./perf_measure.simdout "omp" "quad" ${nproc[$int]} ${ntask[$int]} >> out/omp.quad.simd
    ./perf_measure.out "pthreads" "quad" ${nproc[$int]} ${ntask[$int]} >> out/pthreads.quad 
    ./perf_measure.out "linuxfork" "quad" ${nproc[$int]} ${ntask[$int]} >> out/linuxfork.quad

    mpirun -n ${nproc[$int]} ./perf_measure.mpi.mpout "quad" ${ntask[$int]} >> out/mpi.quad
    mpirun -n ${nproc[$int]} ./perf_measure.mpi.mpout "mc" ${ntask[$int]} >> out/mpi.mc

    let "int++"
done