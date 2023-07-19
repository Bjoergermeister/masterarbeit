#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "../../../common/C/benchmark.h"
#include "../../../common/C/timing.h"

#define MAX_ALLOCATED_MEMORY (1024 * 1024 * 300) // 300 Megabytes
#define OUTPUT_FILENAME_MAX_LENGTH 50

int main(int argc, char **argv)
{
    char *file_prefix = argv[1];

    struct timespec start;
    struct timespec end;

    void *page;
    long total_allocated_memory = 0;
    long page_size = sysconf(_SC_PAGE_SIZE);
    float time_sum = 0;
    int allocation_success = 0;
    int allocation_counter = 0;

    while (total_allocated_memory < MAX_ALLOCATED_MEMORY)
    {
        page = malloc(page_size);

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        memset(page, 1, page_size);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        float time_difference = calculate_time_difference(&start, &end) / (float)NANOSECONDS_IN_ONE_MICROSECOND;
        time_sum += time_difference;

        allocation_counter++;
        total_allocated_memory += page_size;

        // Save average time over 600 allocations in order to reduce result space
        if (allocation_counter % 600 == 0 && allocation_counter > 0)
        {
            float time = time_sum / 600;
            save_benchmark_result_partial(
                time,
                (int)(allocation_counter / 600),
                file_prefix);
            time_sum = 0;
        }
    }

    return 0;
}
