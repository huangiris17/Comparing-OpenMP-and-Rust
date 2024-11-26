#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define INF 1000000000

void floyd_warshall(int n, int *dist) {
    int i, j, k;

    #pragma omp parallel private(i, j, k)
    {
        for (k = 0; k < n; ++k) {
            #pragma omp for schedule(static)
            for (i = 0; i < n; ++i) {
                for (j = 0; j < n; ++j) {
                    if (dist[i * n + k] != INF && dist[k * n + j] != INF) {
                        int new_dist = dist[i * n + k] + dist[k * n + j];
                        if (new_dist < dist[i * n + j]) {
                            dist[i * n + j] = new_dist;
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int i, j;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s -f <filename> <num_threads>\n", argv[0]);
        return 1;
    }

    char *filename = argv[2];
    int num_threads = atoi(argv[3]);

    omp_set_num_threads(num_threads);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s'.\n", filename);
        return 1;
    }

    int n, m;

    int *dist = (int *)malloc(n * n * sizeof(int));
    if (dist == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if (i == j)
                dist[i * n + j] = 0;
            else
                dist[i * n + j] = INF;
        }
    }

    for (i = 0; i < m; ++i) {
        int u, v, w;
        u--; v--;
        if (w < dist[u * n + v]) {
            dist[u * n + v] = w;
        }
    }

    fclose(file);

    double start_time = omp_get_wtime();
    floyd_warshall(n, dist);
    double end_time = omp_get_wtime();

    printf("Execution Time (s): %.6f\n", end_time - start_time);

    // printf("Shortest distance matrix:\n");
    // for (i = 0; i < n; ++i) {
    //     for (j = 0; j < n; ++j) {
    //         if (dist[i * n + j] == INF)
    //             printf("INF ");
    //         else
    //             printf("%d ", dist[i * n + j]);
    //     }
    //     printf("\n");
    // }

    free(dist);
    return 0;
}
