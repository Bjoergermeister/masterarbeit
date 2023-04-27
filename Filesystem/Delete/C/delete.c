#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"

#define BLOCK_SIZE 512

int main(int argc, char **argv)
{
    struct timespec start;
    struct timespec end;

    char *file_to_read = argv[1];
    char *file_to_save = argv[2];

    sync();

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    unlink(file_to_read);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    long difference = calculate_time_difference(&start, &end);
    return save_benchmark_result(difference, file_to_save);
}