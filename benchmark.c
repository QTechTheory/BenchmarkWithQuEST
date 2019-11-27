/** Rudimentary enchmarking of multi-core, distributed and GPU hardware through QuEST.
 * Compile using 'make' and run with './benchmark', which will explain args
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "QuEST.h"


QuESTEnv env;
Qureg qureg;


void circTest(void) {
    int numQb = qureg.numQubitsRepresented;
    
    initPlusState(qureg);
    
    // some arbitrary circuit acting on every qubit
    for (int rep=0; rep<2; rep++) {
        for (int i=0; i<numQb; i++)
            hadamard(qureg, i);
        for (int i=0; i<numQb; i++)
            sqrtSwapGate(qureg, i, (i+1)%numQb);
        for (int i=0; i<numQb; i++)
            controlledRotateY(qureg, i, (i+2)%numQb, .3);
    }
    
    syncQuESTEnv(env);
}


void memTest(void) {
    int numQb = qureg.numQubitsRepresented;
    
    initPlusState(qureg);
    
    // just copying data back and forth between RAM and VRAM
    // (arbitrarily modifying to spook off compilers)
    for (int rep=0; rep<5; rep++) {
        copyStateFromGPU(qureg);
        
        // multiply by i (phase fac of PI)
        int ind = rep % qureg.numAmpsTotal;
        qreal im = qureg.stateVec.imag[ind];
        qureg.stateVec.imag[ind] = qureg.stateVec.real[ind];
        qureg.stateVec.real[ind] = - im;
        
        copyStateToGPU(qureg);
        sqrtSwapGate(qureg, rep%numQb, (rep+1)%numQb);
    }
    
    syncQuESTEnv(env);
}


long double timeFuncCall(void (*func)(void)) {

    // start timing
    struct timeval timeInst;
    gettimeofday(&timeInst, NULL);
    long double startTime = (
		timeInst.tv_sec + (long double) timeInst.tv_usec/pow(10,6));
        
    func();
    
    // stop timing
    gettimeofday(&timeInst, NULL);
    long double endTime = (
        timeInst.tv_sec + (long double) timeInst.tv_usec/pow(10,6));
        
    return endTime - startTime;
}


void timeRepeatFuncCalls(
    void (*func)(void), int numReps, 
    long double *avDur, long double *varDur
) {    
    long double sumDur = 0;
    long double sumSquaredDur = 0;
    
    // disable line-buffering to indicate progress
    setbuf(stdout, NULL);
    
    // repeatedly time the test
    for (int i=0; i<numReps; i++) {
        
        // indicate progress (only if numReps>1)
        for (int percent=0; percent<=100 && (numReps>1); percent+=10)
            if (i == floor(percent * numReps / 100))
                if (env.rank == 0) {
                    printf("%d%% ", percent);
                    break;
                }
                    
        // perform test
        long double dur = timeFuncCall(func);
        sumDur += dur;
        sumSquaredDur += dur*dur;
    }
        
    // E[dur] = sum(dur)/numReps
    *avDur = sumDur/numReps;
    
    // Var[dur] = E[dur^2] - E[dur]^2 = sum(dur^2)/N - E[dur]^2
    *varDur = (sumSquaredDur/numReps) - (*avDur)*(*avDur);
}

void assert(int cond, char* msg) {
    if (!cond) {
        if (env.rank == 0)
            printf("ERROR: %s\nExiting...\n", msg);
        exit(1);
    }
}


int main (int narg, char *varg[]) {
    
    // setup immediately to get rank info
    env = createQuESTEnv();
    
    if (narg != 7 && narg != 8) {
        
        if (env.rank == 0)
            printf(
                "Insufficient args! (requires 6 or 7, given %d) Run as:\n\n"
                "./benchmark NUM_NODES NUM_THREADS IS_GPU TEST_TYPE MEM_SIZE NUM_SAMPLES [FN_SUFFIX]\n\n"
                "where\n\n"
                "NUM_NODES is the number of employed nodes (1 if not distributed).\n\t"
                    "Must be a power of 2.\n\n"
                "NUM_THREADS is the number of employed threads.\n\t"
                    "If >1, QuEST must be compiled in multithreaded mode (using OpenMP).\n\n"
                "IS_GPU is 0 or 1 to indicate whether benchmark is GPU-accelerated.\n\t"
                    "if 1, then necessarily NUM_NODES=1 and NUM_THREADS=1.\n\n"
                "TEST_TYPE is 0 or 1 to indicate which test to perform.\n\t"
                    "0: an arbitrary circuit operating on all qubits\n\t"
                    "1: copying of memory back and forth between RAM and VRAM\n\t"
                    "   (latter should only be used if IS_GPU=1)\n\n"
                "MEM_SIZE is the total per-node (or per-GPU) memory available, in GiB.\n\t"
                    "if IS_GPU=0, this is the total RAM per node (e.g. 64)\n\t"
                    "if IS_GPU=1, this is the total VRAM of the GPU (e.g. 4)\n\t"
                    "Testing will be performed with as big a quantum state-vector as can fit, "
                    "calculated via:\n\t\t"
                        "NUM_QUBITS = Floor[26 + Log2[MEM_SIZE - .5]]\n\t"
                    "which assumes double-precision and an overhead of ~500 MiB.\n\t"
                    "Note distributed tests will simulate 1 fewer qubit, due to comm overheads.\n\n"
                "NUM_SAMPLES is the number of repetitions of the test (informing the mean and variance).\n\n"
                "FN_SUFFIX is an optional string (containing no spaces) to suffix to the output filename.\n\n"
                "The average duration and variance of the test is written to file (by node rank 0):\n\t"
                "'results/data_[NUM_NODES]n_[NUM_THREADS]t_[IS_GPU]g_[MEM_SIZE]m_[NUM_QUBITS]q_[NUM_SAMPLES]s_test[TEST_TYPE][FN_SUFFIX].txt'\n"
                "(where NUM_QUBITS is derived from MEM_SIZE and NUM_NODES) along with "
                "copies of these arguments.\n\n", narg-1);
        exit(1);
    }
    
    // get cmd args
    int i=1;
    int NUM_NODES   = atoi(varg[i++]);
    int NUM_THREADS = atoi(varg[i++]);
    int IS_GPU      = atoi(varg[i++]);
    int TEST_TYPE   = atoi(varg[i++]);
    int MEM_SIZE    = atoi(varg[i++]);
    int NUM_SAMPLES = atoi(varg[i++]);
    char* FN_SUFFIX = (narg==8)? varg[i++]:"";
    
    // validate args 
    assert((NUM_NODES&(NUM_NODES-1))==0, "NUM_NODES must be an exponent of 2.");
    if (IS_GPU)
        assert(NUM_NODES==1 && NUM_THREADS==1, "IS_GPU=1 requires NUM_NODES=NUM_THREADS=1.");
    if (TEST_TYPE == 1)
        assert(IS_GPU, "TEST_TYPE=1 requires IS_GPU=1.");
    assert(MEM_SIZE >= 1, "MEM_SIZE must be at least 1 GiB.");
        
    int NUM_QUBITS = floor(26 + log2(NUM_NODES*MEM_SIZE - .5)) + ((NUM_NODES>1)? (-1):0);
    
    // report args
    char argsBuff[1000];
    sprintf(argsBuff, 
        "NUM_NODES: %d\nNUM_THREADS: %d\nIS_GPU: %d\n"
        "TEST_TYPE: %d\nMEM_SIZE: %d\nNUM_QUBITS: %d\nNUM_SAMPLES: %d\nFN_SUFFIX: %s\n",
        NUM_NODES, NUM_THREADS, IS_GPU, TEST_TYPE, MEM_SIZE, NUM_QUBITS, NUM_SAMPLES, FN_SUFFIX);
        
    if (env.rank == 0)
        printf("\n%s\nSimulating...\n", argsBuff);
    
    // setup QuEST
    qureg = createQureg(NUM_QUBITS, env);
    void (*testFunc)(void) = (TEST_TYPE==1)? memTest : circTest;
    
    // perform benchmark
    long double avDur, varDur;
    timeRepeatFuncCalls(testFunc, NUM_SAMPLES, &avDur, &varDur);
    
    if (env.rank == 0)
        printf("\nDone!\nDuration: %Lg (mean) %Lg (variance)\n\n", avDur, varDur);
    
    // create output filename
    char fnBuff[500];
    sprintf(fnBuff,
        "results/data_%dn_%dt_%dg_%dm_%dq_%ds_test%d%s.txt",
        NUM_NODES, NUM_THREADS, IS_GPU, MEM_SIZE, NUM_QUBITS, NUM_SAMPLES, TEST_TYPE, FN_SUFFIX);
        
    if (env.rank == 0)
        printf("Writing results to:\n%s\n\n", fnBuff);

    // write results to file
    if (env.rank == 0) {
        FILE *fp = fopen(fnBuff, "w");
        if (fp) {
            fprintf(fp,
                "args:\n%s\nfn:\n%s\n\nresults:\nMEAN DURATION: %.10Lf\nVARIANCE: %.10Lf\n",
                argsBuff, fnBuff, avDur, varDur);
            fclose(fp);
        } else
            printf("FAILED to write to file %s\n", fnBuff);
    }
        
    // cleanup
    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;
}
