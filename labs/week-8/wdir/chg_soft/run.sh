#!/bin/bash

softs=(0.0500 0.1000 0.2000 0.5000 1.0000 2.0000 5.0000 10.0000 50.0000 100.0000)

for i in  {0..9}
do
    soft=${softs[i]}

    wd="$PWD/soft$soft"
    name="soft$soft"

    _cmd="msub -j mpi -c 1 -p 16 -mpiprocs 16 -m 24gb -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' "mpirun -np 16 Gadget2 param.txt" 'date "+%T"'
done