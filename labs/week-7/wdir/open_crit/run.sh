#!/bin/bash

for i in 0 1 2 3 4 5 6 7 8
do
    for j in 0 1 2 3 4 5 6 7 8 9
    do
        wd="$PWD/runs_alpha${i}.${j}"
        name="runs${i}.${j}"
        
        _cmd="msub -j mpi -p 8 -d ${wd} -wd ${wd} -n ${name} -m 16gb"
        ${_cmd} 'date "+%T"' 'mpirun -np 8 Gadget2 param.txt' 'date "+%T"'
    done
done