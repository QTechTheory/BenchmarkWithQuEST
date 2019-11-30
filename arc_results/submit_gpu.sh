#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:v100:2
#SBATCH --partition=htc

#SBATCH --job-name=gpu
#SBATCH --mail-user=tyson.jones.input@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --account=oums-quantopo
#SBATCH --time=1:00:00

module purge
module load gpu/cuda/10.0.130

chmod +x script_gpu.sh
./script_gpu.sh

