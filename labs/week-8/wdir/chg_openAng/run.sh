#!/bin/bash

alphas=(0.0001 0.0010 0.0020 0.0050 0.0100)

for i in  {0..4}
do
    alpha=${alphas[i]}

    wd="$PWD/alpha$alpha"
    name="a$alpha"

    _cmd="msub -j mpi -c 1 -p 16 -mpiprocs 16 -m 24gb -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' "mpirun -np 16 Gadget2 param.txt" 'date "+%T"'
done