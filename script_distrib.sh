MAX_NODE_MEM=256 # GiB
NUM_THREADS=16
NUM_NODES=( 2 4 8 ) # add more 2^n until max number of nodes

TIME_OUT=60 # seconds
NUM_SAMPLES=5

make clean
make MULTITHREADED=1 DISTRIBUTED=1 GPUACCELERATED=0
mkdir -p results

export OMP_NUM_THREADS=$NUM_THREADS

for nodes in ${NUM_NODES[@]}; do
    dur=0
    mem=1
    while [ $dur -lt $TIME_OUT ] && [ $mem -le $MAX_NODE_MEM ]; do
        start=$(date +%s)
        mpirun -np $nodes ./benchmark $nodes $NUM_THREADS 0 0 $mem $NUM_SAMPLES
        end=$(date +%s)
        dur=$((end-start))
        mem=$((2*mem))
        echo "test duration: $dur (completes when > $TIME_OUT)"
    done
done