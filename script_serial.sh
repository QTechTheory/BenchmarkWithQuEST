TIME_OUT=120 # seconds
NUM_SAMPLES=5

make clean
make MULTITHREADED=0 DISTRIBUTED=0 GPUACCELERATED=0
mkdir -p results

dur=0
mem=1
while [ $dur -lt $TIME_OUT ]; do
    start=$(date +%s)
    ./benchmark 1 1 0 0 $mem $NUM_SAMPLES
    end=$(date +%s)
    dur=$((end-start))
    mem=$((2*mem))
    echo "test duration: $dur (completes when > $TIME_OUT)"
done