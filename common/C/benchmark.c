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

int save_benchmark_result_multiple(long time1, long time2, char *filename)
{
    FILE *fd = open_file(filename);

    int result = fprintf(fd, "%ld, %ld\n", time1, time2);
    if (handle_error(result, filename))
        exit(1);

    int success = fclose(fd);
    if (handle_error(success, filename))
        exit(-1);

    return 0;
}

int save_benchmark_result(long time, char *filename)
{
    FILE *fd = open_file(filename);

    int result = fprintf(fd, "%ld\n", time);
    handle_error(result, filename);

    int success = fclose(fd);
    handle_error(success, filename);

    return 0;
}

int handle_error(int return_value, char *filename)
{
    char *message = "Ein Fehler ist aufgetreten: ";
    char *str = malloc(strlen(message) + strlen(filename));
    sprintf(str, "%s %s", message, filename);

    if (return_value == -1)
    {
        perror(str);
        exit(-1);
    }
}