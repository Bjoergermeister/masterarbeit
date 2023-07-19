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
    char *file_to_read = argv[1];
    char *file_to_save = argv[2];

    struct timespec start;
    struct timespec end;

    int fd = open(file_to_read, O_RDONLY | __O_DIRECT, S_IRWXU);
    handle_error(fd, file_to_read);

    int file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *read_buffer = malloc(sizeof(char) * file_size);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    read(fd, read_buffer, file_size);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    handle_error(close(fd), file_to_read);

    long difference = calculate_time_difference(&start, &end);
    float difference_in_microseconds = difference / (float)NANOSECONDS_IN_ONE_MICROSECOND;

    save_benchmark_result_float(difference_in_microseconds, file_to_save);
}