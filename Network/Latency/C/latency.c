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

int main(int argc, char **argv)
{
    char *source_ip_address = argv[1];
    char *destination_ip_address = argv[2];
    char *save_filename = argv[3];

    struct timespec start;
    struct timespec end;

    struct sockaddr_in source_address;
    configure_sockaddr(&source_address, source_ip_address);

    struct sockaddr_in destination_address;
    configure_sockaddr(&destination_address, destination_ip_address);

    // Create buffer to hold the packet data
    int packet_size = sizeof(struct ip) + sizeof(struct icmp);
    char packet[packet_size];
    memset(packet, 0, packet_size);

    // Place ip struct at the beginning of the buffer and and icmp struct immediatly after that
    struct ip *ip = (struct ip *)packet;
    struct icmp *icmp = (struct icmp *)(ip + 1);

    configure_ip_header(ip, sizeof(packet), source_address, destination_address);
    configure_icmp_header(icmp);

    int socket = open_socket();
    int bind_success = bind(socket, (struct sockaddr *)&source_address, sizeof(struct sockaddr));
    if (bind_success == -1)
    {
        perror("bind(): ");
        exit(-1);
    }

    /* Socket options, tell the kernel we provide the IP structure */
    int on = 1;
    if (setsockopt(socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt() for IP_HDRINCL error");
        exit(-1);
    }

    // Setup file descriptors for receiving the response and timeout
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(socket, &sockets);
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    struct sockaddr *dest = (struct sockaddr *)&destination_address;
    socklen_t dest_length = sizeof(struct sockaddr *);

    for (int i = 0; i < 100; i++)
    {
        char receive_buffer[packet_size];

        clock_gettime(CLOCK_MONOTONIC, &start);
        long bytes_send = sendto(socket, packet, packet_size, 0, dest, sizeof(struct sockaddr));

        ssize_t bytes_received = recvfrom(socket, &receive_buffer, packet_size, 0, dest, &dest_length);
        if (bytes_received == -1)
        {
            perror("recvfrom(): ");
            exit(-1);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        float difference = (float)calculate_time_difference(&start, &end) / NANOSECONDS_IN_ONE_MILLISECOND;
        save_benchmark_result_float(difference, save_filename);

        sleep(1);
    }
}