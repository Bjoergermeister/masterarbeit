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

    void *current_break = sbrk(0);
    long total_allocated_memory = 0;
    long page_size = sysconf(_SC_PAGE_SIZE);
    float time_sum = 0;
    int allocation_counter = 0;

    while (total_allocated_memory < MAX_ALLOCATED_MEMORY)
    {
        int success = brk(current_break + page_size);
        handle_error(success, "brk()");

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        memset(current_break, 1, page_size);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        time_sum += calculate_time_difference(&start, &end) / (float)NANOSECONDS_IN_ONE_MICROSECOND;

        current_break += page_size;
        allocation_counter++;
        total_allocated_memory += page_size;

        // Save average time over 600 allocations in order to reduce result space
        if (allocation_counter % 600 == 0 && allocation_counter > 0)
        {
            int index = (allocation_counter / 600) - 1;
            float time = time_sum / 600.0f;
            address[index] = time;
            time_sum = 0;
        }
    }

    return 0;
}
