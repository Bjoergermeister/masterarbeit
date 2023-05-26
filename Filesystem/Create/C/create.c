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

    char *file_to_create = argv[1];
    char *file_to_save = argv[2];

    if (strlen(file_to_create) == 0 || strlen(file_to_save) == 0)
    {
        printf("Invalid arguments: %s, %s\n", file_to_create, file_to_save);
        return -1;
    }

    // Flush filesystem cache so that is doesn't influence the measurements
    sync();

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    int file_descriptor = open(file_to_create, O_RDONLY | __O_DIRECT | O_CREAT, S_IRWXU);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    handle_error(file_descriptor, file_to_create);

    int success = close(file_descriptor);
    handle_error(success, file_to_create);

    long difference = calculate_time_difference(&start, &end);
    long difference_in_milliseconds = difference / MILLISECONDS_IN_ONE_SECOND;

    save_benchmark_result(difference_in_milliseconds, file_to_save);
}