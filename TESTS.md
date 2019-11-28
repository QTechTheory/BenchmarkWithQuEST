Running the tests
=================

To run each test, first
- edit `script_[name].sh` to fill in the constants at the top (explicit directions below)
- give it run permission, by `chmod +x script_[name].sh`
- ensure [`makefile`](makefile) lists the correct compiler
- execute via `./script_[name].sh` (or call that from within a job-submission script)

Each test involves sweeping a benchmark parameter to perform an increasingly expensive computation (i.e. more qubits, or more repetitions), until the computation
(execution of `./benchmark`) has reached a minimum duration. By default, this is 60 seconds (120s in serial test), though can be modified by changing
```
TIME_OUT=60 # seconds
```
A computation which surpasses `TIME_OUT` will **not** be interrupted; it will just thereafter complete the sweep of the parameter.
This is simply a mechanism to make sure tests aren't too quick, and control the order of magnitude of the time to complete the tests.
Users may wish to increase this if they have more time to ~~waste~~ spare, or if the variance of the benchmark results are unexpectedly large.
Note `TIME_OUT` concerns the duration of the execution of `benchmark`, and *not* that of each sample therein performed, nor the duration of the test script so far.

Some tests involve repeated sampling, slowing the test by a factor of `NUM_SAMPLES`, where by default 
```
NUM_SAMPLES=5
```
If a particular test is too slow to endure (e.g. at the earliest and slowest computation in a sweep), `NUM_SAMPLES` can be reduced (to minimum `1`).

> A warning when running tests with a job-scheduler: each `script_[name].sh` test will recompile the C code, and so
> cannot be run simultaneously!

## (1) Serial
[`script_serial.sh`](script_serial.sh) performs single-thread single-node benchmarking. It initially utilises `1 GiB` of RAM,
doubling memory (increasing simulated qubits by 1) for every successful benchmark until they exceed `TIME_OUT`. 
This is the slowest test, and is expected to reach `TIME_OUT` very quickly. 

It requires no modification. Simply run
```bash
./script_serial.sh
```

## (2) Multi-threaded
[`script_multi.sh`](script_multi.sh) performs multi-threaded single-node benchmarking. It repeats the previous benchmarking task, 
though with several arrangements of thread-use. It finally performs an additional benchmark, using all available memory and threads, which may take some time (it collects only `1` sample however).

Before running, modify [`script_multi.sh`](script_multi.sh) to set
```bash
MAX_NODE_MEM=256 # GiB
```
to be the RAM of the node (GiB), and add the maximum number of possible threads (unless it's `16`) to the end of the space-separated list
```bash
THREAD_VALS=( 8 16 )
```

Then, run
```bash
./script_multi.sh
```

> Note during the sweeping, even with all threads, it is not expected that anywhere near all the RAM (on the order of 100s of GiB) can be utilised before default `TIME_OUT` is reached.

## (3) Distributed
[`script_distrib.sh`](script_distrib.sh) performs multi-threaded and distributed benchmarking. Using all available threads, it will perform
distributed benchmarking using `2` nodes, `4` nodes, `8` nodes and on to as many possible (of the form `2^n`). 
For each, it will benchmark increasingly large computations, utilising more RAM per-node, until `TIME_OUT` is surpassed.
Finally, it performs an additional benchmark using all available resources, which may take some time (it collects only `1` sample however).

Before running, modify [`script_distrib.sh`](script_distrib.sh) to set
```bash
MAX_NODE_MEM=256 # GiB
```
to the RAM of each node (as before). 
Set
```bash
NUM_THREADS=16
```
to the maximum number of threads available to **each node**.
Add more elements (exponents of 2 only) to the space-separated list
```bash
NUM_NODES=( 2 4 8 )
```
until the last element is the maximum number of nodes employable.
Make sure these nodes are reachable through `mpirun`.

Then, run
```bash
./script_distrib.sh
```

## (4) GPU
[`script_gpu.sh`](script_gpu.sh) performs single-node GPU-accelerated benchmarking, and benchmarking of CPU-GPU bandwidth. 
Utilising the full (or, as much as can be used) VRAM of a single GPU, it will repeat the same benchmark with
progressively increasing number of samples (doubling, from `5`) until `TIME_OUT` is reached.
Finally, it will attempt to perform, using the previously reached number of samples, 
the same benchmarks simultaneously on all available GPUs. This is not to parallelise the task, but to check the performance loss due to
concurrency. This may greatly affect the CPU-GPU bandwidth (`test1`) result.

Before running, ensure `GPU_COMPUTE_CAPABILITY` is correctly set in the [`makefile`](makefile). Then
modify [`script_gpu.sh`](script_gpu.sh) to set
```bash
GPU_MEM=8 # GiB
```
to the VRAM of the GPU. Set
```bash
NUM_GPUS=1 # per-node
```
to be the number of GPUs accessible to the node. 

Then, run
```bash
./script_gpu.sh
```

Sending the results
=====================

Every successful execution of `benchmark` will result in a uniquely-named `.txt` file in `results/`. 
All of these (unmodified) can be sent (e.g. zipped) to Oxford. 
At this time, there is no need for analysing the results oneself.

Ball-park numbers for these tests, as measured on Oxford's ARCUS supercomputing platform, will eventually be published in [ARCRESULTS.md](ARCRESULTS.md).

