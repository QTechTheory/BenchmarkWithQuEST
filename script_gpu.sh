GPU_MEM=8 # GiB
NUM_GPUS=1 # per-node

TIME_OUT=60 # seconds

make clean
make MULTITHREADED=0 DISTRIBUTED=0 GPUACCELERATED=1
mkdir -p results

for test in 0 1; do
    dur=0
    samp=5
    while [ $dur -lt $TIME_OUT ]; do
        start=$(date +%s)
        ./benchmark 1 1 1 $test $GPU_MEM $samp
        end=$(date +%s)
        dur=$((end-start))
        samp=$((2*samp))
        echo "test duration: $dur (completes when > $TIME_OUT)"
    done
done

for test in 0 1; do
   for i in $(seq 0 $((NUM_GPUS-1)) ); do
       CUDA_VISIBLE_DEVICES=$i ./benchmark 1 1 1 $test $GPU_MEM $samp _simulGPU${i}_${NUM_GPUS} &
   done
   wait
done
