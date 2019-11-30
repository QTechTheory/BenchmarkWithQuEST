#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1

#SBATCH --job-name=serial
#SBATCH --mail-user=tyson.jones.input@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --reservation=nqit
#SBATCH --account=oums-quantopo
#SBATCH --time=1:00:00

module purge
module load gcc

chmod +x script_serial.sh
./script_serial.sh
