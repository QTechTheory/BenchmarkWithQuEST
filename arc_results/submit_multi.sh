#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16

# will auto use 64 GiB nodes

#SBATCH --job-name=multi
#SBATCH --mail-user=tyson.jones.input@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --reservation=nqit
#SBATCH --account=oums-quantopo
#SBATCH --time=24:00:00

module purge
module load gcc

chmod +x script_multi.sh
./script_multi.sh
