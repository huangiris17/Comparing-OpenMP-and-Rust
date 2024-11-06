#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#define COMPARE(A, B) ((A > B)? 1: -1)
#define UP -1
#define DOWN 1
#define PARALLEL_THRESHOLD 16384 

void copy_array(int* source, int* destination, int size);
bool is_power_of_two(int n);
void print_arr(int* arr, int size);
void bitonic_sort(int* arr, int start, int cnt, int dir);
void bitonic_merge(int* arr, int start, int cnt, int dir);
void bitonic_sort_p(int* arr, int start, int cnt, int dir);
void bitonic_merge_p(int* arr, int start, int cnt, int dir);
void swap(int*arr, int a, int b);

int main(int argc, char* argv[]) {

	clock_t start, end, start_p, end_p;
	double execution_time, execution_time_p;

	if (argc < 2) {
		fprintf(stderr, "Please insert array size.\n");
		return 1;
	}

	/* Get array size from command line */
	int array_size = strtol(argv[1], NULL, 10);
	if (!is_power_of_two(array_size)) {
		fprintf(stderr, "Please insert array size with the power of two.\n");
		return 1;
	}

	/* Generate array with random numebrs */
	int* arr = malloc(array_size*sizeof(int));
	srand(time(NULL));
	for (int i = 0; i < array_size; i++) {
		arr[i] = rand() % 30;
	}
	int* arr_p = malloc(array_size*sizeof(int));
	copy_array(arr, arr_p, array_size);

	/* printf("Original Array: ");
	print_arr(arr, array_size); */

	/* Apply bitonic sort */
	start = clock();
	bitonic_sort(arr, 0, array_size, UP);
	end = clock();

        /* printf("Sorted Array: ");
        print_arr(arr, array_size); */
	
	execution_time = ((double) end - start) / CLOCKS_PER_SEC;
	printf("Execution time: %f seconds\n", execution_time);

        /* Apply parallelized bitonic sort */
        start_p = clock();
     	#pragma omp parallel
	{
		#pragma omp single
		{
		bitonic_sort_p(arr_p, 0, array_size, UP);
		}
	}
        end_p = clock();

        /* printf("Sorted Array using parallelism: ");
        print_arr(arr_p, array_size); */

        execution_time_p = ((double) end_p - start_p) / CLOCKS_PER_SEC;
        printf("Execution time using parallelism: %f seconds\n", execution_time_p);
	printf("Speedup: %f\n", execution_time / execution_time_p);
	
	free(arr);
	free(arr_p);
}

void copy_array(int* source, int* destination, int size) {
    for (int i = 0; i < size; i++) {
        destination[i] = source[i];
    }
}

bool is_power_of_two(int n) {
	return n > 0 && (n & (n - 1)) == 0;
}


void print_arr(int* arr, int size) {
	for (int i = 0; i < size; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void bitonic_sort(int* arr, int start, int cnt, int dir) {
	if (cnt <= 1) return;
	int size = cnt / 2;
	
	/* Sort the first half in increasing order */
	bitonic_sort(arr, start, size, UP);
	/* Sort the second half in decreasing order */
	bitonic_sort(arr, start + size, size, DOWN);

	/* Merge the two sorted parts */
	bitonic_merge(arr, start, cnt, dir);
}

void bitonic_merge(int* arr, int start, int cnt, int dir) {
	if (cnt <= 1) return;
	int size = cnt / 2;

	/* Compare and swap first and second half of the array */
	for (int i = start; i < start + size; i++) {
		if (dir != COMPARE(arr[i], arr[i + size])) {
			swap(arr, i, i + size);
		}
	}
	
	bitonic_merge(arr, start, size, dir);
	bitonic_merge(arr, start + size, size, dir);
}

void bitonic_sort_p(int* arr, int start, int cnt, int dir) {
	if (cnt <= PARALLEL_THRESHOLD) {
		bitonic_sort(arr, start, cnt, dir);
		return;
	}

	int size = cnt / 2;

	#pragma omp task
	bitonic_sort_p(arr, start, size, UP);
	
	#pragma omp task
	bitonic_sort_p(arr, start + size, size, DOWN);

	#pragma omp taskwait
	bitonic_merge_p(arr, start, cnt, dir);
}

void bitonic_merge_p(int* arr, int start, int cnt, int dir) {
    	if (cnt <= PARALLEL_THRESHOLD) {
	    	bitonic_merge(arr, start, cnt, dir);
		return;
	}   
        
	int size = cnt / 2;

	/* Compare and swap first and second half of the array */
	#pragma omp parallel for
        for (int i = start; i < start + size; i++) {
                if (dir != COMPARE(arr[i], arr[i + size])) {
                        swap(arr, i, i + size);
                }
        }

	#pragma omp task
	bitonic_merge(arr, start, size, dir);
	#pragma omp task
	bitonic_merge(arr, start + size, size, dir);
	#pragma omp taskwait
}

void swap(int*arr, int a, int b) {
	int temp = arr[b];
	arr[b] = arr[a];
	arr[a] = temp;
}
