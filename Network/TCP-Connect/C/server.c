// Global defines
#define _DEFAULT_SOURCE 1

#include <ifaddrs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"
#include "../../../common/C/network.h"

#define STOP_FLAG "\0\0\0\0\0\0\0\0"
#define STOP_FLAG_LENGTH 8
#define BYTES_IN_ONE_MEGABYTE (1024 * 1024)
#define PORT 5000

float calculate_throughput(long bytes, long time_difference)
{
    float time_in_seconds = (float)time_difference / NANOSECONDS_IN_ONE_SECOND;
    float megabytes = (float)bytes / BYTES_IN_ONE_MEGABYTE;
    printf("%f megabytes in %f seconds = %f mb/s\n", megabytes, time_in_seconds, megabytes / time_in_seconds);
    return megabytes / time_in_seconds;
}

void write_response(char *buffer, long bytes, long time_difference)
{
    float throughput = calculate_throughput(bytes, time_difference);
    long throughput_as_int = *(long *)&throughput;

    sprintf(buffer, "%ld\n", throughput_as_int);
}

int main(int argc, char **argv)
{
    char *server_ip_address = argv[1];
    char *client_ip_address = argv[2];
    char *save_filename = argv[3];

    struct timespec start;
    struct timespec end;

    struct sockaddr_in server_address;
    configure_sockaddr(&server_address, server_ip_address, PORT);

    struct sockaddr_in client_address;
    configure_sockaddr(&client_address, client_ip_address, PORT);

    socklen_t address_length = sizeof(struct sockaddr);

    int socket = open_socket(SOCK_STREAM, IPPROTO_TCP);

    int new_size = 1024 * 1024 * 16;
    handle_error(setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &new_size, sizeof(int)), "setsockopt(): ");

    struct sockaddr *server = (struct sockaddr *)&server_address;
    handle_error(bind(socket, server, sizeof(struct sockaddr)), "bind(): ");

    handle_error(listen(socket, 3), "listen()");

    while (true)
    {
        int accepted_socket = accept(socket, (struct sockaddr *)&client_address, &address_length);
        if (accepted_socket == -1)
        {
            perror("accept()");
            exit(-1);
        }
    }
}