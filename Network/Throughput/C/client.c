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

float process_response(char *response)
{
    long long throughput_as_ll = strtoll(response, NULL, 10);
    return *(float *)&throughput_as_ll;
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

    int socket = open_socket(SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr *dest = (struct sockaddr *)&destination_address;
    socklen_t dest_length = sizeof(struct sockaddr *);

    char buffer[1460];
    memset(buffer, 'A', 1460);

    int send_count = 0;
    long total_bytes_send = 0;
    while (total_bytes_send < BYTES_IN_ONE_GIGABYTE)
    {
        if ((BYTES_IN_ONE_GIGABYTE - total_bytes_send) <= sizeof(buffer))
        {
            memset(buffer, '\0', 8);
        }

        int bytes_send = sendto(socket, buffer, sizeof(buffer), 0, dest, sizeof(struct sockaddr));
        if (bytes_send == -1)
        {
            perror("sendto(): ");
            exit(-1);
        }

        send_count++;
        total_bytes_send += bytes_send;
    }

    printf("Send %ld bytes (%d packets)\n", total_bytes_send, send_count);

    // Receive response from server which contains the result
    size_t buffer_size = 100; // Long enough for a float
    char response[buffer_size];
    int bytes_received = recvfrom(socket, response, buffer_size, 0, dest, &dest_length);
    if (bytes_received == -1)
    {
        perror("recvfrom(): ");
        exit(-1);
    }

    float result = process_response(response);
    save_benchmark_result_float(result, save_filename);
}