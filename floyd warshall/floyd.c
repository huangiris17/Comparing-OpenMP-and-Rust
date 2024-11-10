#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define INF 1000000000  // A large value representing "infinity"

void floyd_warshall(int n, int **dist) {
    int i, j, k;

    for (k = 0; k < n; ++k) {
        #pragma omp parallel for private(j) schedule(static)
        for (i = 0; i < n; ++i) {
            for (j = 0; j < n; ++j) {
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    int new_dist = dist[i][k] + dist[k][j];
                    if (new_dist < dist[i][j]) {
                        dist[i][j] = new_dist;
                    }
                }
            }
        }
    }
}

int main() {
    int n, m;
    printf("Enter the number of vertices and edges: ");
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error reading the number of vertices and edges.\n");
        return 1;
    }

    if (n <= 0 || m < 0) {
        fprintf(stderr, "Number of vertices must be positive, and number of edges cannot be negative.\n");
        return 1;
    }

    int **dist = (int **)malloc(n * sizeof(int *));
    if (dist == NULL) {
        fprintf(stderr, "Memory allocation failed for distance matrix.\n");
        return 1;
    }
    int i, j;
    for (i = 0; i < n; ++i) {
        dist[i] = (int *)malloc(n * sizeof(int));
        if (dist[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for dist[%d].\n", i);
            for (j = 0; j < i; ++j) {
                free(dist[j]);
            }
            free(dist);
            return 1;
        }
        for (j = 0; j < n; ++j) {
            if (i == j)
                dist[i][j] = 0;
            else
                dist[i][j] = INF;
        }
    }

    printf("Enter each edge and its weight (format: u v w):\n");
    for (i = 0; i < m; ++i) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Error reading edge %d.\n", i + 1);
            for (j = 0; j < n; ++j) {
                free(dist[j]);
            }
            free(dist);
            return 1;
        }
        u--; v--;
        if (u < 0 || u >= n || v < 0 || v >= n) {
            fprintf(stderr, "Invalid vertex index: u=%d, v=%d (after adjusting for 1-based indexing).\n", u + 1, v + 1);
            // Free allocated memory before exiting
            for (j = 0; j < n; ++j) {
                free(dist[j]);
            }
            free(dist);
            return 1;
        }
        dist[u][v] = w;
    }

    floyd_warshall(n, dist);

    printf("Shortest distance matrix:\n");
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if (dist[i][j] == INF)
                printf("INF ");
            else
                printf("%d ", dist[i][j]);
        }
        printf("\n");
    }

    for (i = 0; i < n; ++i) {
        free(dist[i]);
    }
    free(dist);

    return 0;
}
