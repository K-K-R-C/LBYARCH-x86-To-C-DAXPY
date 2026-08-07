#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

#define NUM_RUNS 30 // how many times we run each version to get the average time

// the 3 sizes we need to test,
int sizes[] = {
    1 << 20, // = 1,048,576
    1 << 24, // = 16,777,216
    1 << 28  // = 268,435,456
};
int num_sizes = 3;

// this is the assembly version
extern void daxpy(int n, double A, double* X, double* Y, double* Z);

// this is our C version of daxpy, used as the "answer key"
// to check if the assembly version gives the same output
void daxpy_c(int n, double A, double* X, double* Y, double* Z) {
    for (int i = 0; i < n; i++) {
        Z[i] = A * X[i] + Y[i];
    }
}

// fills X and Y with random numbers so we have something to test with
void initialize_vectors(double* X, double* Y, int n) {
    for (int i = 0; i < n; i++)
    {
        X[i] = (rand() % 1000) / 10.0;
        Y[i] = (rand() % 1000) / 10.0;
    }
}

// just prints the first 10 numbers of the result so we can eyeball check it
void print_first_ten(double* Z) {
    printf("First 10 elements:\n");

    for (int i = 0; i < 10; i++) {
        printf("%.6f\n", Z[i]);
    }
}

int main() {
    srand((unsigned int)time(NULL)); // makes the random numbers different each time we run

    // loop through all 3 required sizes (2^20, 2^24, 2^28)
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        double A = 2.0; // scalar value used in the daxpy formula

        printf("---------------------------------\n");
        printf("Vector size (n): %d\n", n);
        printf("---------------------------------\n");


        // make room in memory for our vectors
        // we use malloc here instead of a normal array because these sizes
        // are too big to fit on the stack (would crash if we tried)
        double* X = (double*)malloc(n * sizeof(double));
        double* Y = (double*)malloc(n * sizeof(double));
        double* Z_c = (double*)malloc(n * sizeof(double));    // result from C version
        double* Z_asm = (double*)malloc(n * sizeof(double));  // result from ASM version

        // malloc can fail if there's not enough memory
        // so we check here before trying to use any of these pointers
        if (X == NULL || Y == NULL || Z_c == NULL || Z_asm == NULL)
        {
            printf("Memory allocation failed for n = %d, skipping this size.\n\n", n);
            free(X);
            free(Y);
            free(Z_c);
            free(Z_asm);
            continue; // go to the next size in the loop
        }

        initialize_vectors(X, Y, n);

        // time the C version, 30 runs, get average
        // QueryPerformanceCounter is a Windows function that gives us a very precise timer
        // freq tells us how many "ticks" happen per second, so we can convert to milliseconds
        LARGE_INTEGER freq, t1, t2;
        QueryPerformanceFrequency(&freq);

        double total_time_c = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            QueryPerformanceCounter(&t1); // start the clock
            daxpy_c(n, A, X, Y, Z_c);     // this is the only part we actually time
            QueryPerformanceCounter(&t2); // stop the clock

            // convert the tick difference into milliseconds
            double elapsed_ms = (double)(t2.QuadPart - t1.QuadPart) * 1000.0 / freq.QuadPart;
            total_time_c += elapsed_ms;
        }

        double avg_time_c = total_time_c / NUM_RUNS; // average over 30 runs

        // time the asm version, 30 runs, get average
        double total_time_asm = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            QueryPerformanceCounter(&t1);
            daxpy(n, A, X, Y, Z_asm);  // calling our assembly function
            QueryPerformanceCounter(&t2);

            double elapsed_ms = (double)(t2.QuadPart - t1.QuadPart) * 1000.0 / freq.QuadPart;
            total_time_asm += elapsed_ms;
        }
        double avg_time_asm = total_time_asm / NUM_RUNS;

        // print first 10 elements of both
        printf("C version:\n");
        print_first_ten(Z_c);

        printf("\nASM version:\n");
        print_first_ten(Z_asm);

        // check if C and ASM actually give the same answer
        int correct = 1; // assume correct until proven otherwise
        for (int i = 0; i < n; i++)
        {
            // we don't check for an exact match because floating point math
            // can be off by a tiny tiny amount even when it's technically correct
            // so we just check if the difference is small enough to not matter
            if (fabs(Z_c[i] - Z_asm[i]) > 1e-9)
            {
                correct = 0;
                break; // no need to keep checking once we find one mismatch
            }
        }

        if (correct == 1) {
            printf("\nCorrectness check: PASS\n");
        }
        else {
            printf("\nCorrectness check: FAIL\n");
        }

        // print timing results 
        printf("C avg time: %f ms\n", avg_time_c);
        printf("ASM avg time: %f ms\n", avg_time_asm);
        printf("\n");

        // free the memory before next size
        // if we don't do this, we'd run out of memory since the sizes are so big
        free(X);
        free(Y);
        free(Z_c);
        free(Z_asm);
    }

    return 0;
}