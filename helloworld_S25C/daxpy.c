#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

#define NUM_RUNS 30

int sizes[] = {
    1 << 20,
    1 << 24,
    1 << 28
};
int num_sizes = 3;

extern void daxpy(int n, double A, double* X, double* Y, double* Z);

void daxpy_c(int n, double A, double* X, double* Y, double* Z) {
    for (int i = 0; i < n; i++) {
        Z[i] = A * X[i] + Y[i];
    }
}

void initialize_vectors(double* X, double* Y, int n) {
    for (int i = 0; i < n; i++)
    {
        X[i] = (rand() % 1000) / 10.0;
        Y[i] = (rand() % 1000) / 10.0;
    }
}

void print_first_ten(double* Z) {
    printf("First 10 elements:\n");

    for (int i = 0; i < 10; i++) {
        printf("%.6f\n", Z[i]);
    }
}

int main() {
    srand((unsigned int)time(NULL));

    // Main loop for all 3 required sizes (20, 24, 28)
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        double A = 2.0;

        printf("---------------------------------\n");
        printf("Vector size (n): %d\n", n);
        printf("---------------------------------\n");

        // allocate the vectors for this size
        double* X = (double*)malloc(n * sizeof(double));
        double* Y = (double*)malloc(n * sizeof(double));
        double* Z_c = (double*)malloc(n * sizeof(double));
        double* Z_asm = (double*)malloc(n * sizeof(double));

        // check that all 4 allocations actually succeeded
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
        LARGE_INTEGER freq, t1, t2;
        QueryPerformanceFrequency(&freq);

        double total_time_c = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            QueryPerformanceCounter(&t1);
            daxpy_c(n, A, X, Y, Z_c);
            QueryPerformanceCounter(&t2);

            double elapsed_ms = (double)(t2.QuadPart - t1.QuadPart) * 1000.0 / freq.QuadPart;
            total_time_c += elapsed_ms;
        }
        double avg_time_c = total_time_c / NUM_RUNS;

        // time the asm version, 30 runs, get average
        double total_time_asm = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            QueryPerformanceCounter(&t1);
            daxpy(n, A, X, Y, Z_asm);
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

        // check correctness: compare C and asm results
        int correct = 1; // assume correct until proven otherwise
        for (int i = 0; i < n; i++)
        {
            if (fabs(Z_c[i] - Z_asm[i]) > 1e-9)
            {
                correct = 0;
                break;
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
        free(X);
        free(Y);
        free(Z_c);
        free(Z_asm);
    }

    return 0;
}