// Global defines
#define _DEFAULT_SOURCE 1

#include <ifaddrs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>

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
#define MESSAGE_LENGTH 1460

float calculate_throughput(long bytes, long time_difference)
{
    float time_in_seconds = (float)time_difference / NANOSECONDS_IN_ONE_SECOND;
    float megabytes = (float)bytes / BYTES_IN_ONE_MEGABYTE;
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

bool has_end_flag(char *message, ssize_t message_length)
{
    return strncmp(message + (message_length - 8), STOP_FLAG, 8) == 0;
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
    handle_error(setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &new_size, sizeof(int)), "setsockopt()");

    struct sockaddr *server = (struct sockaddr *)&server_address;
    handle_error(bind(socket, server, sizeof(struct sockaddr)), "bind()");

    handle_error(listen(socket, 3), "listen()");

    while (true)
    {
        int accepted_socket = accept(socket, (struct sockaddr *)&client_address, &address_length);
        if (accepted_socket == -1)
        {
            perror("accept()");
            exit(-1);
        }

        char client_message[1460];
        long total_received_bytes = 0;
        int total_received_packets = 0;
        bool is_first_packet = true;
        ssize_t received_bytes = 0;
        do
        {
            received_bytes = recv(accepted_socket, client_message, sizeof(client_message), 0);
            if (received_bytes == -1)
            {
                perror("recv()");
                exit(-1);
            }

            total_received_bytes += received_bytes;
            total_received_packets++;

            if (is_first_packet)
            {
                clock_gettime(CLOCK_MONOTONIC, &start);
                is_first_packet = false;
            }

        } while (has_end_flag(client_message, received_bytes) == false);

        clock_gettime(CLOCK_MONOTONIC, &end);
        long difference = calculate_time_difference(&start, &end);
        write_response(client_message, total_received_bytes, total_received_packets, difference);

        ssize_t bytes_send = send(accepted_socket, client_message, MESSAGE_LENGTH, 0);
        if (bytes_send == -1)
        {
            perror("send()");
            exit(-1);
        }

        close(accepted_socket);
    }
}