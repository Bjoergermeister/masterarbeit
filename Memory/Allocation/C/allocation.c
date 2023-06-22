#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "../../../common/C/benchmark.h"
#include "../../../common/C/timing.h"

#define MAX_ALLOCATED_MEMORY (1024 * 1024 * 300) // 300 Megabytes
int main(int argc, char **argv)
{
    char *file_prefix = argv[1];

    struct timespec start;
    struct timespec end;

    void *current_break = sbrk(0);
    void *first_break = current_break;
    void *previous_break = current_break;
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
        current_break += page_size;
        total_allocated_memory += page_size;

        if (allocation_counter % 600 == 0 && allocation_counter > 0)
        {
            float time = time_sum / 600.0f;
            save_benchmark_result_partial(
                time,
                (int)(allocation_counter / 600),
                file_prefix);
            time_sum = 0;
        }
    }

    return 0;
}