#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#include <time.h>

static inline uint32_t lcg_rand(uint32_t *state) {
    *state = (1664525 * (*state) + 1013904223);
    return *state;
}

int main(int argc, char *argv[])
{
    long long num_trials = 1000000000;
    long long count = 0;
    double x, y;
    double pi_estimate;

    int num_threads = omp_get_max_threads();

    if (argc > 1) {
        num_threads = atoi(argv[1]);
        if (num_threads <= 0) {
            fprintf(stderr, "Invalid number of threads. Using default (%d).\n", num_threads);
            num_threads = omp_get_max_threads();
        }
    }

    int max_threads = omp_get_max_threads();

    omp_set_num_threads(num_threads);

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        uint32_t seed = (uint32_t)(time(NULL)) ^ (uint32_t)(omp_get_thread_num());

        long long local_count = 0;

        #pragma omp for schedule(static)
        for (long long i = 0; i < num_trials; i++) {
            x = (double)lcg_rand(&seed) / (double)UINT32_MAX;
            y = (double)lcg_rand(&seed) / (double)UINT32_MAX;

            if (x * x + y * y <= 1.0)
                local_count++;
        }

        #pragma omp atomic
        count += local_count;
    }

    double elapsed = omp_get_wtime() - start_time;

    pi_estimate = 4.0 * (double)count / (double)num_trials;

    printf("Estimated Pi = %.6f\n", pi_estimate);
    printf("Threads: %d, Elapsed time: %.4f seconds\n", num_threads, elapsed);

    return 0;
}
