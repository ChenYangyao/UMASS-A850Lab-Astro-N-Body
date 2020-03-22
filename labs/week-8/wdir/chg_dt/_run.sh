#!/bin/bash

dts=(10.0000 20.0000 50.0000 100.0000)

for i in  {0..3}
do
    dt=${dts[i]}

    wd="$PWD/dt$dt"
    name="dt$dt"

    _cmd="msub -j mpi -c 5 -p 24 -mpiprocs 24 -m 48gb -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' "mpirun -np 32 Gadget2 param.txt" 'date "+%T"'
done