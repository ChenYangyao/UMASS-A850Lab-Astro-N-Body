#! /bin/bash

nproc=(1 2 4 8 16 32 64 8 8 8 8 8 8 8 8 8 8 8 8 8)
ntask=(32 128 512 2048 8192 32768 131072 524288 2097152 8388608 33554432 134217728 536870912)

int=0
while [ $int -lt 20 ]
do
    echo $int
    ./perf_measure.out "omp" "quad" ${nproc[$i]} ${ntask[$i]} >> out/omp.quad 
    ./perf_measure.out "omp" "mc" ${nproc[$i]} ${ntask[$i]} >> out/omp.mc 
    ./perf_measure.simdout "omp" "quad" ${nproc[$i]} ${ntask[$i]} >> out/omp.mc.simd
    ./perf_measure.out "pthreads" "quad" ${nproc[$i]} ${ntask[$i]} >> out/pthreads.quad 
    ./perf_measure.out "linuxfork" "quad" ${nproc[$i]} ${ntask[$i]} >> out/linuxfork.quad
done