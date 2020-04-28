#!/bin/bash

for f in s0.0167  s0.0268  s0.0334  s0.0418  s0.0502  s0.0669  s0.0836  s0.1003
do
    wd="$PWD/$f"
    name="$f"

    _cmd="msub -j mpi -c 1 -p 8 -mpiprocs 8 -m 24gb -n ${name} -d ${wd} -wd ${wd} --join oe"
    ${_cmd} 'date "+%T"' "mpirun -np 8 Gadget2 param.txt" 'date "+%T"'
done