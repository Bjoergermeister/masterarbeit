// Global defines
#define _DEFAULT_SOURCE 1

#include <ifaddrs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"
#include "../../../common/C/network.h"

#define BYTES_IN_ONE_GIGABYTE (1024 * 1024 * 256)
#define MESSAGE_LENGTH 1460

float process_response(char *response, float *throughput, int *packet_count)
{
    *throughput = *(float *)response;
    *packet_count = *(int *)(response + sizeof(int) + 1);
}

int main(int argc, char **argv)
{
    char *source_ip_address = argv[1];
    char *destination_ip_address = argv[2];
    char *save_filename = argv[3];

    struct timespec start;
    struct timespec end;

    struct sockaddr_in destination_address;
    configure_sockaddr(&destination_address, destination_ip_address, 5000);

    struct sockaddr *destination = (struct sockaddr *)&destination_address;
    socklen_t destination_length = sizeof(struct sockaddr);

    char message[MESSAGE_LENGTH];
    memset(message, 'A', MESSAGE_LENGTH);

    int socket = open_socket(SOCK_STREAM, IPPROTO_IP);
    handle_error(connect(socket, destination, destination_length), "connect()");

    long total_send_bytes = 0;
    int total_send_packets = 0;

    while (total_send_bytes < BYTES_IN_ONE_GIGABYTE)
    {
        if (BYTES_IN_ONE_GIGABYTE - total_send_bytes <= MESSAGE_LENGTH)
        {
            memset(message + (MESSAGE_LENGTH - 8), '\0', 8);
        }

        ssize_t bytes_send = send(socket, message, MESSAGE_LENGTH, 0);
        if (bytes_send == -1)
        {
            perror("send()");
            exit(-1);
        }

        total_send_bytes += bytes_send;
        total_send_packets++;
    }

    // Get response with throughput from server
    ssize_t received_bytes = recv(socket, message, MESSAGE_LENGTH, 0);
    if (received_bytes == -1)
    {
        perror("recv()");
        exit(-1);
    }

    float throughput = 0;
    int packet_count = 0;
    process_response(message, &throughput, &packet_count);

    // Close connection and reset buffer
    // Reset end flag
    memset(message, 'A', MESSAGE_LENGTH);
    handle_error(close(socket), "close()");

    save_benchmark_result_float(throughput, save_filename);
    return 0;
}