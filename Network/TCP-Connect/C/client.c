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

    struct sockaddr *destination = (struct sockaddr *)&destination_address;
    socklen_t destination_length = sizeof(struct sockaddr);

    for (int i = 0; i < 100; i++)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        handle_error(connect(socket, destination, destination_length), "connect()");
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

        float difference = calculate_time_difference(&start, &end) / (float)NANOSECONDS_IN_ONE_MICROSECOND;
        save_benchmark_result_float(difference, save_filename);
    }

    close(socket);
    return 0;
}