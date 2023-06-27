#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "benchmark.h"

#define BUFFER_LENGTH 10

FILE *open_file(char *filename)
{
    FILE *fd = fopen(filename, "a");
    if (fd == NULL)
    {
        char buffer[50];
        sprintf(buffer, "Fehler beim Öffnen der Datei '%s': ", filename);
        perror(buffer);
        exit(-1);
    }

    return fd;
}

void save_benchmark_result_multiple(long time1, long time2, char *filename)
{
    FILE *fd = open_file(filename);

    int result = fprintf(fd, "%ld, %ld\n", time1, time2);
    handle_error(result, filename);

    int success = fclose(fd);
    handle_error(success, filename);
}

void save_benchmark_result(long time, char *filename)
{
    FILE *fd = open_file(filename);

    int result = fprintf(fd, "%ld\n", time);
    if (result <= 0)
    {
        perror(filename);
    }
    int success = fclose(fd);
    handle_error(success, filename);
}

void save_benchmark_result_float(float time, char *filename)
{
    FILE *fd = open_file(filename);

    int result = fprintf(fd, "%f\n", time);
    if (result <= 0)
    {
        perror(filename);
    }
    int success = fclose(fd);
    handle_error(success, filename);
}

void save_benchmark_result_partial(float time, int count, char *prefix)
{
    char filename[50];
    snprintf(filename, 50, "%s_%d.txt", prefix, count);
    save_benchmark_result_float(time, filename);
}

void handle_error(int return_value, char *filename)
{
    if (return_value == 0)
        return;

    char *message = "Ein Fehler ist aufgetreten: ";
    char string[256];
    snprintf(string, 256, "%s %s", message, filename);

    perror(string);
    exit(-1);
}