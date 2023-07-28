#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"

#define BLOCK_SIZE 4096

long write_and_measure(int fd)
{
    struct timespec start;
    struct timespec end;

    lseek(fd, 0, SEEK_SET);

    char *buffer = (char *)malloc(BLOCK_SIZE);
    memset(buffer, 'A', BLOCK_SIZE);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    write(fd, buffer, 1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    free(buffer);

    return calculate_time_difference(&start, &end);
}

int main(int argc, char **argv)
{
    char *file_to_read = argv[1];
    char *file_to_save = argv[2];

    int fd = open(file_to_read, O_WRONLY | O_SYNC | __O_DIRECT, S_IRWXU);
    handle_error(fd, file_to_read);

    sync();

    float difference_first_write = write_and_measure(fd) / (float)NANOSECONDS_IN_ONE_MICROSECOND;
    float difference_second_write = write_and_measure(fd) / (float)NANOSECONDS_IN_ONE_MICROSECOND;

    handle_error(close(fd), file_to_read);

    save_benchmark_result_multiple(difference_first_write, difference_second_write, file_to_save);
}