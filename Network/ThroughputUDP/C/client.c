// Global defines
#define _DEFAULT_SOURCE 1

#include <ifaddrs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"
#include "../../../common/C/network.h"

#define BYTES_IN_ONE_GIGABYTE (1024 * 1024 * 1024)
#define RESPONSE_BUFFER_SIZE 100

void process_response(char *response, float *throughput, int *packet_count)
{
    *throughput = *(float *)response;
    *packet_count = *(int *)(response + sizeof(int) + 1);
}

int main(int argc, char **argv)
{
    char *destination_ip_address = argv[1];
    char *save_filename = argv[2];

    struct timespec start;
    struct timespec end;

    struct sockaddr_in destination_address;
    configure_sockaddr(&destination_address, destination_ip_address, 5000);

    int socket = open_socket(SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr *dest = (struct sockaddr *)&destination_address;
    socklen_t dest_length = sizeof(struct sockaddr *);

    char buffer[1460];
    memset(buffer, 'A', 1460);

    // Send data until BYTES_IN_ONE_GIGABYTE - sizeof(buffer) bytes are send
    int send_count = 0;
    long total_bytes_send = 0;
    while (total_bytes_send < BYTES_IN_ONE_GIGABYTE - sizeof(buffer))
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

    // recv timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    handle_error(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)), "setsockopt");

    // Send end flag
    int iteration = 1;
    char response_buffer[RESPONSE_BUFFER_SIZE];

    while (true)
    {
        // Set the beginning of the send buffer to the iteration count. This tells the server how often the stop flag has
        // been send so he can determine if the timeout has to be subtracted or not
        int *iteration_address = (int *)buffer;
        *iteration_address = iteration;

        int bytes_send = sendto(socket, buffer, sizeof(buffer), 0, dest, sizeof(struct sockaddr));
        if (bytes_send == -1)
        {
            perror("sendto(): ");
            exit(-1);
        }

        // Receive response from server which contains the result
        int bytes_received = recvfrom(socket, response_buffer, RESPONSE_BUFFER_SIZE, 0, dest, &dest_length);
        if (bytes_received > 0)
            break;

        // If the send package was lost (because of UDP), the server will not receive it and therefore not send a reply
        // Therefore, the timeout of recvfrom will trigger and errno is set to EAGAIN or EWOULDBLOCK.
        // in this case, the package should be resent
        if (bytes_received == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            iteration++;
            continue;
        }

        // if none of the above is true, an unexpected error occured
        perror("recvfrom(): ");
        exit(-1);
    }

    printf("Send %ld bytes (%d packets)\n", total_bytes_send, send_count);

    float throughput = 0;
    int received_count = 0;
    process_response(response_buffer, &throughput, &received_count);
    float packet_success_percentage = (100.0f / send_count) * received_count;

    save_benchmark_result_multiple(throughput, packet_success_percentage, save_filename);
}
