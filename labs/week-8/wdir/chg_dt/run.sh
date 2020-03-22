#!/bin/bash

dts=(0.1000 0.2000 0.5000 1.0000 2.0000 5.0000 10.0000 20.0000 50.0000 100.0000)

for i in  {0..9}
do
    dt=${dts[i]}

    wd="$PWD/dt$dt"
    name="dt$dt"

    _cmd="msub -j mpi -c 1 -p 4 -mpiprocs 4 -m 12gb -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' "mpirun -np 4 Gadget2 param.txt" 'date "+%T"'
done