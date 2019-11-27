Benchmarking with QuEST
======================

This repo contains some rudimentary code to benchmark hardware, using the [QuEST](https://github.com/QuEST-Kit/QuEST) library.
The responsible code is exclusively in `benchmark.c`.

## Kinds of testing

There are two types of tests.
One simulates random quantum circuits using QuEST, and the other involves copying memory back and forth between RAM and VRAM (when run with a GPU).
The former test should be run on all hardware facilities (single-threaded, multithreaded, distributed, GPU-accelerated) while the latter only run with a GPU.

The tests accept `MEM_SIZE`, the amount of available memory (in GiB), and will perform as large a simulation (allocating as large a state-vector as can fit in memory) as is possible. It is ok to lie and declare a smaller memory size if the otherwise simulated system is infeasibly large.

In combination, these tests motivate measures of clock-speed, caching time, network bandwidth and RAM to VRAM bandwidth.

## Compiling

Modify [makefile](/makefile), populating the fields `COMPILER`, `COMPILER_TYPE` and `GPU_COMPUTE_CAPABILITY`. 
The other fields `MULTITHREADED`, `DISTRIBUTED` and `GPUACCELERATED` will change across the tests. 

Then, compile `benchmark` using
```bash
make
```

## Running tests

First, ensure a results folder exists (with write permissions):
```bash
mkdir results
```

Executable `benchmark` accepts cmd arguments:
```bash
./benchmark NUM_NODES NUM_THREADS IS_GPU TEST_TYPE MEM_SIZE NUM_SAMPLES
```
which are each elaborated upon if `./benchmark` is called without any args.

The first four (`NUM_NODES`, `NUM_THREADS`, `IS_GPU`, `TEST_TYPE`) vary across tests, while the latter two must be decided by the user.
- `MEM_SIZE` is the total memory (in GiB) available per-node (or per-GPU) of the hardware.
- `NUM_SAMPLES` is how many repetitions of the benchmark should be performed, informing the mean performance.

`NUM_SAMPLES` should be chosen by the user to give a reasonably accurate average performance; if the simulation is too slow (because `MEM_SIZE` is big, so the studied system is large), use fewer samples (e.g. `5`). If the simulation is very quick (e.g. << 1s), use many samples (e.g. `100`).

Here are the kinds of tests to be run (note `make clean` should be re-run between recompiling for each test).

serial
```bash
make
./benchmark 1 1 0 0 MEM_SIZE NUM_SAMPLES
```

multithreaded
```bash
# T is the max number of threads possible, decided by user
make MULTITHREADED=1
export OMP_NUM_THREADS=T 
./benchmark 1 T 0 0 MEM_SIZE NUM_SAMPLES
```

distributed
```bash
# T is the max number of threads possible, decided by user
# N is the max number of available nodes, which must be a power of 2, decided by user
make MULTITHREADED=1 DISTRIBUTED=1
export OMP_NUM_THREADS=T
mpirun -np N benchmark N T 0 0 MEM_SIZE NUM_SAMPLES
```

GPU
```bash
make GPUACCELERATED=1

./benchmark 1 1 1 0 VMEM_SIZE NUM_SAMPLES

./benchmark 1 1 1 1 VMEM_SIZE NUM_SAMPLES
```

All needed information will be written to files, with filenames:
`
results/data_[NUM_NODES]n_[NUM_THREADS]t_[IS_GPU]g_[MEM_SIZE]m_[NUM_QUBITS]q_[NUM_SAMPLES]s_test[TEST_TYPE].txt
`
which is unique for every differentiable test.
