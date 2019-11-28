Benchmarking with QuEST
======================

This repo contains some rudimentary code to benchmark hardware, using the [QuEST](https://github.com/QuEST-Kit/QuEST) library.
The responsible code is exclusively in `benchmark.c`.

## Kinds of testing

There are two *types* of tests.
One simulates random quantum circuits using QuEST, and the other involves copying memory back and forth between RAM and VRAM (when run with a GPU).
The former test will be run on all hardware facilities (single-threaded, multithreaded, distributed, GPU-accelerated) while the latter only run with a GPU.

In combination, these tests motivate measures of clock-speed, caching time, network bandwidth and RAM to VRAM bandwidth. They are wrapped into four test scripts.

## Compiling

Modify [makefile](/makefile), and populate the fields `COMPILER`, `COMPILER_TYPE` and `GPU_COMPUTE_CAPABILITY`. 
You should use `gcc-8` (`COMPILER_TYPE = GNU`) preferably, though exceptions for (e.g.) CUDA-compability are ok. Any compatible `nvcc` and `mpicc` compilers are fine; if needed, aliases for these can be entered at lines 173 and 174 (`CUDA_COMPILER = nvcc` and `MPI_COMPILER = mpicc`).

The other fields `MULTITHREADED`, `DISTRIBUTED` and `GPUACCELERATED` will change automatically across the tests. It's ok to change `COMPILER` in the makefile between different tests.

To test compiling the serial `benchmark`, run
```bash
make
```
You should check all compilers/targets used by the tests are compatible:
```bash
make clean
make MULTITHREADED=1 DISTRIBUTED=0 GPUACCELERATED=0

make clean
make MULTITHREADED=1 DISTRIBUTED=1 GPUACCELERATED=0

make clean
make MULTITHREADED=0 DISTRIBUTED=0 GPUACCELERATED=1
```

## The benchmark executable

> Understanding the arguments to `benchmark` is not necessary for completing the tests.

Executable `benchmark` accepts cmd arguments:
```bash
./benchmark NUM_NODES NUM_THREADS IS_GPU TEST_TYPE MEM_SIZE NUM_SAMPLES [FN_SUFFIX]
```
which are each elaborated upon if `./benchmark` is called without any args. Some of these arguments will be automatically swept by the testing scripts, and some must be provided by the user (with info about the target hardware).

Note that using `MEM_SIZE` - the amount of available per-node memory (in GiB) - `benchmark` will perform as large a simulation (allocating as large a state-vector as can fit in memory) as is possible. It is ok to specify the full memory size, since `benchmark` will factor in overheads before allocating memory.
It is also ok to lie and declare a smaller-than-physical memory size if the otherwise simulated system is infeasibly large, and it is ok to specify any integer greater than 0 (not just powers-of-two). Some test scripts will automatically sweep this parameter.

Each execution of `benchmark` will write to file 
```
results/data_[NUM_NODES]n_[NUM_THREADS]t_[IS_GPU]g_[MEM_SIZE]m_[NUM_QUBITS]q_[NUM_SAMPLES]s_test[TEST_TYPE][FN_SUFFIX].txt
```
which is unique for every differentiable test.

## Running the tests

View instructions for running each test [here](TESTS.md).
