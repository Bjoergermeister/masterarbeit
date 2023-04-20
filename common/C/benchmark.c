#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "benchmark.h"

#define BUFFER_LENGTH 10

int save_benchmark_result(long time, char *filename)
{
    FILE *fd = fopen(filename, "a");
    if (fd == NULL)
    {
        char buffer[50];
        sprintf(buffer, "Fehler beim Öffnen der Datei '%s': ", filename);
        perror(buffer);
        return -1;
    }

    int result = fprintf(fd, "%ld\n", time);
    if (result < 0)
    {
        perror("Fehler beim Schreiben in die Datei: ");
        return -1;
    }

    int success = fclose(fd);
    if (handle_error(success, filename))
        return -1;

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
        return true;
    }
    return false;
}