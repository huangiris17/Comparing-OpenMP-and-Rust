#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define INF 1000000000

void floyd_warshall(int n, int **dist) {
    int i, j, k;

    for (k = 0; k < n; ++k) {
        #pragma omp parallel for private(i, j) schedule(static)
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
    scanf("%d %d", &n, &m);

    int **dist = (int **)malloc(n * sizeof(int *));
    int i, j;
    for (i = 0; i < n; ++i) {
        dist[i] = (int *)malloc(n * sizeof(int));
        for (j = 0; j < n; ++j) {
            if (i == j)
                dist[i][j] = 0;
            else
                dist[i][j] = INF;
    }

    printf("Enter each edge and its weight (format: u v w):\n");
    for (i = 0; i < m; ++i) {
        int u, v, w;
        scanf("%d %d %d", &u, &v, &w);
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
