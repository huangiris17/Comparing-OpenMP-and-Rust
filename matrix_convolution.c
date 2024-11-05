#include <stdio.h> 
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#define FILTER_SIZE 3

void convolution(int** input, int filter[FILTER_SIZE][FILTER_SIZE], int** output, int size, int padding);
void convolution_parallel(int** input, int filter[FILTER_SIZE][FILTER_SIZE], int** output, int size, int padding);
void freeMatrix(int** matrix, int size);
int** createMatrix(int size);
void printMatrix(int** matrix, int size);


int main(int argc, char* argv[]) {

	clock_t start, end, start_p, end_p;
	double execution_time, execution_time_p;
	
	/* Check if matrix size is provided */
	if (argc < 2) {
		fprintf(stderr, "Please insert matrix size.");
		return 1;
	}

	/* Get matrix size from command line */
	int matrix_size = strtol(argv[1], NULL, 10);

	/* Allocate memroy for intput matrix*/
	int** matrix = createMatrix(matrix_size);
	/* Allocate memory for output matrix */
	int output_size = matrix_size - FILTER_SIZE + 1;
	int** output_matrix = createMatrix(output_size);

	/* Fill the matrix with random numbers */
	srand(time(NULL));
	for (int rol = 0; rol < matrix_size; rol++)
	{
		for (int col = 0; col < matrix_size; col++) 
		{
			matrix[rol][col] = rand() % 10;
		}
	}	

	/* Print the matrix */
	//printf("Input Matrix:\n");
	//printMatrix(matrix, matrix_size);

	/* Create the filter matrix*/
	int filter[FILTER_SIZE][FILTER_SIZE] = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	};

	/* Apply matrix convolution */
	int padding = 1;
	start = clock();
	convolution(matrix, filter, output_matrix, matrix_size, padding);
	end = clock();
	execution_time = ((double) end - start) / CLOCKS_PER_SEC;

	start_p = clock();
	convolution_parallel(matrix, filter, output_matrix, matrix_size, padding);
	end_p = clock();
	execution_time_p = ((double) end_p - start_p) / CLOCKS_PER_SEC;
        
	/* Print the output matrix */
	//printf("Output Matrix:\n");
	/* printMatrix(output_matrix, output_size); */

	/* Print the execution time */
	printf("Execution time: %f seconds\n", execution_time);
	printf("Execution time using parallelism: %f seconds\n", execution_time_p);

        /* Free the matrix */
	freeMatrix(matrix, matrix_size);
	freeMatrix(output_matrix, output_size);

}

void convolution (int** input, int filter[FILTER_SIZE][FILTER_SIZE], int** output, int input_size, int padding)
{
	for (int i = padding; i < input_size - padding; i++) {
		for (int j = padding; j < input_size - padding; j++) {
			int sum = 0;

			// Apply the filter matrix over the input matrix region
			for (int fi = -1; fi < FILTER_SIZE - 1; fi++) {
				for (int fj = -1; fj < FILTER_SIZE - 1; fj++) {
					sum += input[i + fi][j + fj] * filter[fi + 1][fj + 1];
				}
			}

			// Store the result in the output matrix
			output[i - padding][j - padding] = sum;
		}
	}
}

void convolution_parallel (int** input, int filter[FILTER_SIZE][FILTER_SIZE], int** output, int input_size, int padding)
{
        //omp_set_num_threads(4);
	int chunk_size = 256;
	printf("Parallel with chunk size: %d\n", chunk_size);
	#pragma omp parallel for schedule(static, 256)
        for (int i = padding; i < input_size - padding; i++) {
                for (int j = padding; j < input_size - padding; j++) {
                        int sum = 0;

                        // Apply the filter matrix over the input matrix region
                        for (int fi = -1; fi < FILTER_SIZE - 1; fi++) {
                                for (int fj = -1; fj < FILTER_SIZE - 1; fj++) {
                                        sum += input[i + fi][j + fj] * filter[fi + 1][fj + 1];
                                }
                        }

                        // Store the result in the output matrix
                        output[i - padding][j - padding] = sum;
                }
        }
}

void freeMatrix(int** matrix, int size)
{
	/* Free the matrix */
        for (int rol = 0; rol < size; rol++)
        {
                free(matrix[rol]);
        }
        free(matrix);
}

int** createMatrix(int size)
{
        int** matrix = malloc(size*sizeof(int*));
        for (int rol = 0; rol < size; rol++)
        {
                matrix[rol] = malloc(size*sizeof(int));
        }
	return matrix;
}

void printMatrix(int** matrix, int size)
{
        for (int rol = 0; rol < size; rol++)
        {
                for (int col = 0; col < size; col++)
                {
                        printf(" %d ", matrix[rol][col]);
                }
                printf("\n");
        }
}
