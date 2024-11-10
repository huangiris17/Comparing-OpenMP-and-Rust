#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    long long num_trials = 100000000;
    long long count = 0;
    double x, y;
    double pi_estimate;

    omp_set_num_threads(4);

    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num();

        #pragma omp for reduction(+:count)
        for (long long i = 0; i < num_trials; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0)
                count++;
        }
    }

    pi_estimate = 4.0 * (double)count / (double)num_trials;

    printf("Estimated Pi = %.6f\n", pi_estimate);

    return 0;
}
