#!/bin/bash

nparts=(5000 10000 50000 100000 500000 1000000)
ncs=(1 1 1 2 4 8)
nprocs=(2 4 8 8 8 8)
totalprocs=(2 4 8 16 32 64)
mems=("4gb" "8gb" "16gb" "16gb" "16gb" "16gb")

for i in  {0..5}
do
    npart=${nparts[i]}
    nc=${ncs[i]}
    nproc=${nprocs[i]}
    mem=${mems[i]}
    totalproc=${totalprocs[i]}

    wd="$PWD/n$npart"
    name="n$npart"

    _cmd="msub -j mpi -c ${nc} -p ${nproc} -mpiprocs ${nproc} -m ${mem} -n ${name} -d ${wd} -wd ${wd}"
    ${_cmd} 'date "+%T"' "mpirun -np ${totalproc} Gadget2 param.txt" 'date "+%T"'
done