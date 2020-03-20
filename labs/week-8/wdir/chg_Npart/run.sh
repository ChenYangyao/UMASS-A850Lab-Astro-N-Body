#!/bin/bash

for i in  1000 5000 10000 50000 100000 500000 1000000
    wd="$PWD/n$i"
    name="n$i"
    
    _cmd="msub -j mpi -p 16 -n ${name} -d ${wd} -wd ${wd} -m 32gb"
    ${_cmd} 'date "+%T"' 'mpirun -np 16 Gadget2 param.txt' 'date "+%T"'
done