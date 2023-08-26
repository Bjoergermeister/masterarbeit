#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "../../../common/C/benchmark.h"
#include "../../../common/C/timing.h"
#include "../../../common/C/ipc.h"

#define MAX_ALLOCATED_MEMORY (1024 * 1024 * 300) // 300 Megabytes
#define OUTPUT_FILENAME_MAX_LENGTH 50

void allocate_and_fill(void **pages, int page_size)
{
    void *current_break = sbrk(0);

    int page_count = MAX_ALLOCATED_MEMORY / page_size;
    for (int i = 0; i < page_count; i++)
    {
        int success = brk(current_break + page_size);
        handle_error(success, "brk()");

        pages[i] = current_break;
        memset(pages[i], 1, page_size);
        current_break += page_size;
    }
}

int main(int argc, char **argv)
{
    // Setup ipc resources
    char *filename = argv[1];
    int shared_memory_id = 0;
    int message_queue_id = 0;
    initialize_ipc(MODE_USER, filename, &message_queue_id, &shared_memory_id);
    float *address = get_shared_memory(shared_memory_id);

    // Notify the receiver process that we are ready to measure and than wait for a second so the receiver has enough time
    // to copy the data from the last process
    notify_receiver(message_queue_id);
    usleep(50000);

    struct timespec start;
    struct timespec end;

    long page_size = sysconf(_SC_PAGE_SIZE);
    float time_sum = 0;
    int allocation_counter = 0;

    void *pages[76800];
    allocate_and_fill(pages, page_size);

    volatile int sum;

    long total_page_count = MAX_ALLOCATED_MEMORY / page_size;
    for (int i = total_page_count; i >= 0; i--)
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
            float time = time_sum / 600.0f;
            int index = (allocation_counter / 600) - 1;
            address[index] = time;
            time_sum = 0;
        }
    }

    return 0;
}
