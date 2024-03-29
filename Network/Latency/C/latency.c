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
    configure_sockaddr(&source_address, source_ip_address, 0);

    struct sockaddr_in destination_address;
    configure_sockaddr(&destination_address, destination_ip_address, 0);

    // Create buffer to hold the packet data
    int packet_size = sizeof(struct ip) + sizeof(struct icmp);
    char packet[packet_size];
    memset(packet, 0, packet_size);

    // Place ip struct at the beginning of the buffer and and icmp struct immediatly after that
    struct ip *ip = (struct ip *)packet;
    struct icmp *icmp = (struct icmp *)(ip + 1);

    configure_ip_header(ip, sizeof(packet), source_address, destination_address);
    configure_icmp_header(icmp);

    int socket = open_socket(SOCK_RAW, IPPROTO_ICMP);
    int bind_success = bind(socket, (struct sockaddr *)&source_address, sizeof(struct sockaddr));
    handle_error(bind_success, "bind(): ");

    /* Socket options, tell the kernel we provide the IP structure */
    int on = 1;
    int setsockopt_success = setsockopt(socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
    handle_error(setsockopt_success, "setsockopt() fpor IP_HDRINCL error");

    // Setup file descriptors for receiving the response and timeout
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(socket, &sockets);

    struct sockaddr *dest = (struct sockaddr *)&destination_address;
    socklen_t dest_length = sizeof(struct sockaddr *);

    for (int i = 0; i < 100; i++)
    {
        char receive_buffer[packet_size];

        clock_gettime(CLOCK_MONOTONIC, &start);
        long bytes_send = sendto(socket, packet, packet_size, 0, dest, sizeof(struct sockaddr));

        ssize_t bytes_received = recvfrom(socket, &receive_buffer, packet_size, 0, dest, &dest_length);
        clock_gettime(CLOCK_MONOTONIC, &end);

        handle_errors(bytes_received, "recvfrom(): ");

        float difference = calculate_time_difference(&start, &end) / (float)NANOSECONDS_IN_ONE_MICROSECOND;
        save_benchmark_result_float(difference, save_filename);

        sleep(1);
    }
}