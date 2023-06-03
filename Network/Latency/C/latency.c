// Global defines
#define _DEFAULT_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <features.h>
#include <stdbool.h>

#include <sys/socket.h>

#include "../../../common/C/timing.h"
#include "../../../common/C/benchmark.h"

unsigned short cksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}

char *get_interface_family(struct ifaddrs *ifaddr)
{
    switch (ifaddr->ifa_addr->sa_family)
    {
    case AF_PACKET:
        return "AF_PACKET";
    case AF_INET:
        return "IPv4";
    case AF_INET6:
        return "IPv6";
    default:
        return "?";
    }
}

int skip_interface(struct ifaddrs *interface)
{
    return (interface->ifa_addr == NULL || interface->ifa_addr->sa_family != AF_INET);
}

void get_interface_by_name(struct ifaddrs **result, char *name)
{
    struct ifaddrs *cursor;
    handle_error(getifaddrs(&cursor), "Get interfaces");

    for (; cursor != NULL; cursor = cursor->ifa_next)
    {
        if (skip_interface(cursor))
            continue;

        int size = strlen(name);
        if (strncmp(cursor->ifa_name, name, size) == 0)
        {
            *result = cursor;
            return;
        }
    }

    cursor = NULL;
}

void get_address_of_interface_by_name(struct sockaddr_in *socket_address, char *interface_name)
{
    struct ifaddrs *interface;
    get_interface_by_name(&interface, interface_name);
    socket_address = (struct sockaddr_in *)interface->ifa_addr;
}

void configure_sockaddr(struct sockaddr_in *socket_address, unsigned short port, char *address)
{
    socket_address->sin_family = AF_INET; // IPv4
    socket_address->sin_port = htons(port);
    inet_aton(address, &socket_address->sin_addr);
    memset(socket_address->sin_zero, 0, 8);
}

int open_socket()
{
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd == -1)
    {
        perror("Open socket");
        exit(-1);
    }

    return socket_fd;
}

void configure_ip_header(struct ip *ip, int length, struct sockaddr_in source_address, struct sockaddr_in destination_address)
{
    ip->ip_v = 4; // IPv4
    ip->ip_ttl = 255;
    ip->ip_tos = 0;
    ip->ip_p = IPPROTO_ICMP;
    ip->ip_id = htons(0);
    ip->ip_off = htons(0);
    ip->ip_hl = 5;
    ip->ip_len = htons(68);
    ip->ip_sum = cksum((unsigned short *)ip, ip->ip_hl);
    // Set ip addresses
    ip->ip_src = source_address.sin_addr;
    ip->ip_dst = destination_address.sin_addr;
}

void configure_icmp_header(struct icmp *icmp)
{
    icmp->icmp_code = 0;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_hun.ih_idseq.icd_id = 1;
    icmp->icmp_hun.ih_idseq.icd_seq = 0;
    icmp->icmp_cksum = cksum((unsigned short *)icmp, sizeof(struct icmp));
}

int main(int argc, char **argv)
{
    char *interface_name = "enp2s0";
    struct sockaddr_in source_address;
    configure_sockaddr(&source_address, 0, "192.168.178.37");
    // get_address_of_interface_by_name(&source_address, interface_name);

    struct sockaddr_in destination_address;
    int port = 0;
    configure_sockaddr(&destination_address, port, argv[1]);

    // Create buffer to hold the packet data
    int packet_size = sizeof(struct ip) + sizeof(struct icmp);
    char packet[packet_size];

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

    /*
    packet[0] = 0x08;
    packet[2] = 0x58;
    packet[3] = 0xb5;
    */
    struct sockaddr *dest = (struct sockaddr *)&destination_address;
    long bytes_send = sendto(socket, packet, packet_size, 0, dest, sizeof(struct sockaddr));
    if (bytes_send == -1)
    {
        perror("sendto: ");
        exit(-1);
    }
    /*
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(socket, &sockets);
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    int result = select(socket + 1, &sockets, NULL, NULL, &timeout);
    if (result == -1)
    {
        perror("select(): ");
        exit(-1);
    }
    */

    for (int i = 0; i < packet_size; i++)
    {
        unsigned char character = packet[i];
        printf("inputCstr[%d] = %d, %x, %c\n", i, character, character, character);
    }

    printf("%s, %ld\n", packet, sizeof(packet));
}