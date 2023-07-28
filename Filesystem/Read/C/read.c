#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"

#define BLOCK_SIZE 4096

int main(int argc, char **argv)
{
    char *file_to_read = argv[1];
    char *file_to_save = argv[2];

    struct timespec start;
    struct timespec end;

    struct stat file_stat;
    stat(file_to_read, &file_stat);
    off_t file_size = file_stat.st_size;

    int fd = open(file_to_read, O_RDONLY | __O_DIRECT, S_IRWXU);
    handle_error(fd, file_to_read);

    char *read_buffer = aligned_alloc(BLOCK_SIZE, sizeof(char) * file_size);

    handle_error(lseek(fd, 0, SEEK_SET), "seek()");

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    ssize_t result = read(fd, read_buffer, (size_t)file_size);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    handle_error(result, "read()");
    handle_error(close(fd), file_to_read);

    long difference = calculate_time_difference(&start, &end);
    float difference_in_microseconds = difference / (float)NANOSECONDS_IN_ONE_MICROSECOND;

    save_benchmark_result_float(difference_in_microseconds, file_to_save);
}