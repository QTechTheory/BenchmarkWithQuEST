MAX_NODE_MEM=256 # GiB
THREAD_VALS=( 8 16 ) # add more threads to this list if supported (biggest last)

TIME_OUT=60 # seconds
NUM_SAMPLES=5

make clean
make MULTITHREADED=1 DISTRIBUTED=0 GPUACCELERATED=0
mkdir -p results

for thr in ${THREAD_VALS[@]}; do
    export OMP_NUM_THREADS=$thr
    dur=0
    mem=1
    while [ $dur -lt $TIME_OUT ] && [ $mem -le $MAX_NODE_MEM ]; do
        start=$(date +%s)
        ./benchmark 1 $thr 0 0 $mem $NUM_SAMPLES
        end=$(date +%s)
        dur=$((end-start))
        mem=$((2*mem))
        echo "test duration: $dur (completes when > $TIME_OUT)"
    done    
done

./benchmark 1 ${THREAD_VALS[-1]} 0 0 $MAX_NODE_MEM 1