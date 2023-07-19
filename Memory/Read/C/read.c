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

void allocate_and_fill(void **pages, int page_size)
{
    int page_count = MAX_ALLOCATED_MEMORY / page_size;
    for (int i = 0; i < page_count; i++)
    {
        pages[i] = malloc(page_size);
        memset(pages[i], 1, page_size);
    }
}

int main(int argc, char **argv)
{
    char *file_prefix = argv[1];

    struct timespec start;
    struct timespec end;

    long page_size = sysconf(_SC_PAGE_SIZE);
    long total_page_count = MAX_ALLOCATED_MEMORY / page_size;
    float time_sum = 0;
    int allocation_counter = 0;

    void *pages[76800];
    allocate_and_fill(pages, page_size);

    volatile int sum;

    for (int i = MAX_ALLOCATED_MEMORY / page_size; i >= 0; i--)
    {
        sum = 0;
        char *page = (char *)pages[i];

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        for (int j = 0; j < page_size; j++)
        {
            sum += (int)page[j];
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        float difference = calculate_time_difference(&start, &end) / (float)NANOSECONDS_IN_ONE_MICROSECOND;
        time_sum += difference;

        allocation_counter++;

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
