#!/bin/bash

for i in  5000 10000 50000 100000 500000 1000000
do
    wd="$PWD/n$i"
    name="n$i"

    _cmd="msub -j mpi -c 16 -p 4 -mpiprocs 4 -m 16gb -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' 'mpirun -np 64 Gadget2 param.txt' 'date "+%T"'
done