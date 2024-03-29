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

#define STOP_FLAG "00000000"
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

void write_response(char *buffer, long bytes, int packets, long time_difference)
{
    float throughput = calculate_throughput(bytes, time_difference);

    int *throughput_address = (int *)buffer;
    int *packets_address = (int *)(buffer + sizeof(int) + 1);

    *throughput_address = *(int *)&throughput;
    *packets_address = packets;
}

bool is_stop_flag(char *client_message)
{
    return (strncmp(STOP_FLAG, client_message, STOP_FLAG_LENGTH) == 0);
}

int main(int argc, char **argv)
{
    char *server_ip_address = argv[1];

    struct timespec start;
    struct timespec end;

    socklen_t length = sizeof(struct sockaddr);
    struct sockaddr_in client_address;
    struct sockaddr_in server_address;
    configure_sockaddr(&server_address, server_ip_address, PORT);

    int socket = open_socket(SOCK_DGRAM, IPPROTO_UDP);

    int new_size = 1024 * 1024 * 16;
    handle_error(setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &new_size, sizeof(int)), "setsockopt(): ");

    struct sockaddr *server = (struct sockaddr *)&server_address;
    handle_error(bind(socket, server, sizeof(struct sockaddr)), "bind(): ");

    struct sockaddr *client = (struct sockaddr *)&client_address;

    char client_message[1500];

    while (true)
    {
        long total_bytes_received = 0;
        int receive_count = 0;
        bool is_first_packet = true;

        do
        {
            int bytes_received = recvfrom(socket, client_message, 1500, 0, client, &length);
            if (bytes_received < 0)
            {
                perror("recvfrom(): ");
                exit(-1);
            }

            if (is_first_packet)
            {
                is_first_packet = false;
                clock_gettime(CLOCK_MONOTONIC, &start);
            }

            receive_count++;
            total_bytes_received += bytes_received;
        } while (client_message[0] == 'A');

        clock_gettime(CLOCK_MONOTONIC, &end);

        long difference = calculate_time_difference(&start, &end);
        long nanoseconds_in_one_second = 5000000000;
        int iteration_count = *(int *)client_message;
        if (iteration_count > 1)
            difference -= nanoseconds_in_one_second * (iteration_count - 1);

        if (receive_count != 735440)
        {
            printf("Missing packets! (Received: %d, missing: %d)\n", receive_count, 735440 - receive_count);
        }
        // Send response with time to client
        size_t buffer_size = 100;
        char response[buffer_size];
        write_response(response, total_bytes_received, receive_count, difference);

        int result = sendto(socket, response, buffer_size, 0, client, length);
        if (result == -1)
        {
            perror("sendto(): ");
            exit(-1);
        }
    }

    close(socket);
}
