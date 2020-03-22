#!/bin/bash

alphas=(0.1000 0.2000 0.5000 1.0000 2.0000 5.0000 10.0000 20.0000 50.0000 100.0000)

for i in  {0..9}
do
    alpha=${alphas[i]}

    wd="$PWD/alpha$alpha"
    name="alpha$alpha"

    _cmd="msub -j mpi -c 2 -p 8 -mpiprocs 8 -m 12gb -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' "mpirun -np 16 Gadget2 param.txt" 'date "+%T"'
done