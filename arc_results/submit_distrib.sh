#!/bin/bash

#SBATCH --nodes=32
#SBATCH --ntasks-per-node=1

#SBATCH --job-name=distrib
#SBATCH --mail-user=tyson.jones.input@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --reservation=nqit
#SBATCH --account=oums-quantopo
#SBATCH --time=24:00:00

module purge
module load openmpi/4.0.0__gcc-8.2.0 binutils/2.32
. enable_arcus-b_mpi.sh

# note script_distrib was modified to cater to ARCUS-B
# (the hosts were explicitly supplied to mpirun, trimmed
# to #nodes from the provided variable $MPI_HOSTS
# truncate hostlist for MPI.
# multiline-comment:
: '
nodes=...

old_IFS=${IFS-$' \t\n'}
IFS=' '
read -ra mpiargs <<< "$MPI_HOSTS"
IFS=','
read -ra hostnodes <<< "${mpiargs[3]}"
somenodes=$(echo "${hostnodes[@]:0:nodes}" | tr ' ' ',')
IFS=$old_IFS
...
mpirun -np $nodes -host $somenodes ...
' # (end comment)

chmod +x script_distrib.sh
source ./script_distrib.sh
