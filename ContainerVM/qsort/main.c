#include <stdlib.h>
#include <stdio.h>

#include "../../common/C/timing.h"
#include "../../common/C/benchmark.h"

#define ARRAY_SIZE 10000000

void fill_array_randomly(int* array){
    for (int i = 0; i < ARRAY_SIZE; i++){
        array[i] = random() % ARRAY_SIZE;
    }
}

int compare(const void* a, const void* b){
    return (*(int*)a - *(int*)b);
}

int main(int argc, char **argv)
{
    struct timespec start;
    struct timespec end;

    int *array = (int *)malloc(sizeof(int) * ARRAY_SIZE);
    fill_array_randomly(array);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    qsort(array, ARRAY_SIZE, sizeof(int), compare);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    long difference = calculate_time_difference(&start, &end) / NANOSECONDS_IN_ONE_MICROSECOND;
    save_benchmark_result(difference, argv[1]);

    return 0;
}