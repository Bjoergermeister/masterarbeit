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

#define PORT 5000

int main(int argc, char **argv)
{
    char *server_ip_address = argv[1];

    struct timespec start;
    struct timespec end;

    socklen_t address_length = sizeof(struct sockaddr);
    struct sockaddr_in client_address;
    struct sockaddr_in server_address;
    configure_sockaddr(&server_address, server_ip_address, PORT);

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
        handle_error(close(accepted_socket), "close()");
    }
}
