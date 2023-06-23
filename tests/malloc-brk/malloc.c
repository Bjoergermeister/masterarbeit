#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "../../common/C/benchmark.h"
#include "../../common/C/timing.h"

#define MAX_ALLOCATED_MEMORY (1024 * 1024 * 300) // 300 Megabytes

int main(int argc, char **argv)
{
    struct timespec start;
    struct timespec end;

    long total_allocated_memory = 0;
    long page_size = sysconf(_SC_PAGE_SIZE);
    float time_sum = 0;
    int allocation_success = 0;
    int allocation_counter = 0;

    while (total_allocated_memory < MAX_ALLOCATED_MEMORY)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        malloc(page_size);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        if (allocation_success != 0)
        {
            printf("%ld\n", total_allocated_memory);
            perror("");
            exit(-1);
        }

        float time_difference = calculate_time_difference(&start, &end) / (float)NANOSECONDS_IN_ONE_MICROSECOND;
        time_sum += time_difference;

        allocation_counter++;
        total_allocated_memory += page_size;

        if (allocation_counter % 600 == 0 && allocation_counter > 0)
        {
            float time = time_sum / 600.0f;
            printf("%f\n", time);
            time_sum = 0;
        }
    }

    while (1)
    {
    }

    return 0;
}