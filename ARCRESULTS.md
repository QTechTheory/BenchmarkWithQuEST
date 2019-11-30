The folder [arc_results](arc_results/) contains the SLURM submit scripts (e.g. [`submit_serial.sh`](arc_results/submit_serial.sh)) used on ARCUS-B and ARCUS-HTC to generate the results (files `data_*.txt`).
Here's a quick compilation of the results. 

Below, `qb` denotes qubits, and time is given in units of seconds (variance is the square of this).
Note `<X GiB` refers to maximum system memory `X` offered for allocation, and *not* the size of the **used** memory (which is consistently half `X` in these tests, since only powers-of-2 GiB were offered).

## (1) Serial

As contained in `data_1n_1t_0g_*.txt`, generated from ARCUS-B (Haswell 16-core), compiled with `gcc 4.8.2`.

| size            | threads | mean time | variance |
|-----------------|---------|-----------|----------|
| 25 qb (<1 GiB)  | 1       | 33.0586   | 0.0001   |

## (2) Multi-threaded

As contained in `data_1n_*t_0g_*.txt`, generated from ARCUS-B (Haswell 16-core), compiled with `gcc 4.8.2`.

(5 samples):

| size            | threads | mean time | variance |
|-----------------|---------|-----------|----------|
| 25 qb (<1 GiB)  | 8       | 4.8283    | 0.0009   |
| 26 qb (<2 GiB)  | 8       | 9.9607    | 0.0002   |
| 27 qb (<4 GiB)  | 8       | 20.620    | 0.002    |

| size            | threads | mean time | variance |
|-----------------|---------|-----------|----------|
| 25 qb (<1 GiB)  | 16      | 3.16      | 0.01     |
| 26 qb (<2 GiB)  | 16      | 6.010     | 0.001    |
| 27 qb (<4 GiB)  | 16      | 13.0      | 0.2      |

(1 sample):

| size            | threads | mean time |
|-----------------|---------|-----------|
| 31 qb (<64 GiB) | 16      | 224       |

## (3) Distribued

As contained in `data_*n_16t*.txt`, generated from ARCUS-B (Haswell 16-core, Infiniband interconnect), compiled with `openmpi 4`, `gcc 8.2.0`.  It is suspected multithreading was failed to be employed, except for the final 35 qubit simulation.

Note `per-node size` includes the *total* simulation size in qubits, and the size available to each node (not necessarily fully utilised) which lead to that qubit number.

(5 samples):

| per-node size   | nodes | mean time | variance |
|-----------------|-------|-----------|----------|
| 25 qb (<1 GiB)  | 2     | 18.222    | 0.002    |
| 26 qb (<1 GiB)  | 4     | 6.834     | 0.006    |
| 27 qb (<2 GiB)  | 4     | 12.05     | 0.07     |
| 27 qb (<1 GiB)  | 8     | 8.583     | 0.006    |
| 28 qb (<2 GiB)  | 8     | 15.2      | 0.1      |
| 28 qb (<1 GiB)  | 16    | 10.288    | 0.002    |
| 29 qb (<2 GiB)  | 16    | 18.31     | 0.05     |
| 29 qb (<1 GiB)  | 32    | 26.9      | 0.3      |

(1 sample):

| per-node size   | nodes | mean time |
|-----------------|-------|-----------|
| 35 qb (<64 GiB) | 32    | 1776      |

## (4) GPU

As contained in `data_1n_1t_1g_*.txt`, generated from ARCUS-HTC (16 GiB Tesla v100, PCIe to Skylake 16-core Gold5120), compiled with `nvcc v10` around `gcc 4.8.5`.

Test 0 (circuit simulation, as above) of 29 qubits (<16 GiB), reporting the trial with the greatest number of samples:

| mean time | variance | samples |
|-----------|----------|---------|
| 3.197449  | 0.000007 | 20      |

Test 1 (bandwidth)

| mean time | variance | samples |
|-----------|----------|---------|
| 18.8      | 0.3      | 5       |

Test of above, utilising both GPUs simultaneously, and 10 samples.

| test | (GPU1) mean time | variance | (GPU2) mean time | variance |
|------|------------------|----------|------------------|----------|
| 0    | 3.20369          | 0.00004  | 3.20070          | 0.00003  |
| 1    | 37.2             | 0.7      | 37.2             | 0.7      |

