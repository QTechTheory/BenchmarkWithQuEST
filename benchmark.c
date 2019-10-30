/** Rudimentary enchmarking of multi-core, distributed and GPU hardware through QuEST.
 * Compile using 'make' and run with './benchmark', which will explain args
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "QuEST.h"

void circTest(Qureg qureg) {
    int numQb = qureg.numQubitsRepresented;
    
    initPlusState(qureg);
    
    for (int reps=0; reps<10; reps++) {
        for (int i=0; i<numQb; i++)
            hadamard(qureg, i);
        for (int i=0; i<numQb; i++)
            sqrtSwapGate(qureg, i, (i+1)%numQb);
        for (int i=0; i<numQb; i++)
            controlledRotateY(qureg, i, (i+2)%numQb, .3);
    }
}

void memTest(Qureg qureg) {
    
    initPlusState(qureg);
    
    for (int reps=0; reps<20; reps++) {
        copyStateFromGPU(qureg);
        
        // multiply by i (phase fac of PI)
        qreal im = qureg.stateVec.imag[reps];
        qureg.stateVec.imag[reps] = qureg.stateVec.real[reps];
        qureg.stateVec.real[reps] = - im;
        
        copyStateToGPU(qureg);
        hadamard(qureg, reps%qureg.numQubitsRepresented);
    }
}

int main (int narg, char *varg[]) {

    if (narg != 3) {
        printf(
            "Insufficient args! Run as:\n\n"
            "./benchmark TESTCODE NUMQUBITS\n\n"
            "where\n\n"
            "TESTCODE is 0 or 1:\n\t"
                "0 to run and time an arbitrary circuit\n\t"
                "1 to time direct state-vector modification\n\n"
            "NUMQUBITS should satisfy (in order to ~fill RAM):\n\t"
                "CPU/GPU: 16 * 2^NUMQUBITS bytes < RAM\n\t"
                "distributed: 2 * 16 * 2^NUMQUBITS bytes < RAM\n\n"
            "The duration (in seconds) of the test is written to file:\n\t"
            "'benchmark_results_test[TESTCODE]_[NUMQUBITS]qubits.txt'\n"
        );
        exit(1);
    }
    
    // setup
    int testCode = atoi(varg[1]);
    int numQb = atoi(varg[2]);
    QuESTEnv env = createQuESTEnv();
    Qureg qureg = createQureg(numQb, env);
    
    // start timing
    struct timeval timeInst;
    gettimeofday(&timeInst, NULL);
    long double startTime = (
		timeInst.tv_sec + (long double) timeInst.tv_usec/pow(10,6));
    
    // perform test
    if (testCode == 0)
        circTest(qureg);
    else
        memTest(qureg);
        
    // stop timing
    gettimeofday(&timeInst, NULL);
    long double endTime = (
        timeInst.tv_sec + (long double) timeInst.tv_usec/pow(10,6));
    long double duration = endTime - startTime;
    printf("duration: %.10Lf\n", duration);
        
    // prepare filename 
    char fn[500];
    sprintf(fn, "benchmark_results_test%d_%dqubits.txt", testCode, numQb);
    FILE *fp = fopen(fn, "w");
    if (fp) {
        fprintf(fp, "%.10Lf\n", duration);
        printf("written to: %s\n", fn);
        fclose(fp);
    } else {
        printf("FAILED to write to %s\n", fn);
    }
        
    // cleanup
    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;
}
