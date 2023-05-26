#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"

int main(int argc, char **argv)
{
    struct timespec start;
    struct timespec end;

    char *filename_to_open = argv[1];
    char *filename_to_save = argv[2];

    if (strlen(filename_to_open) == 0 || strlen(filename_to_save) == 0)
    {
        printf("Invalid arguments: %s, %s\n", filename_to_open, filename_to_save);
        return -1;
    }

    // Flush filesystem cache so that is doesn't influence the measurements
    sync();

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    int file_descriptor = open(filename_to_open, O_RDONLY | __O_DIRECT, S_IRWXU);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    handle_error(file_descriptor, filename_to_open);

    int success = close(file_descriptor);
    handle_error(success, filename_to_open);

    long difference = calculate_time_difference(&start, &end);
    printf("%ld\n", difference);
    long difference_in_milliseconds = difference / MILLISECONDS_IN_ONE_SECOND;

    save_benchmark_result(difference_in_milliseconds, filename_to_save);
}