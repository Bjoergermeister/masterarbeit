#include <stdlib.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "network.h"

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

int skip_interface(struct ifaddrs *interface)
{
    return (interface->ifa_addr == NULL || interface->ifa_addr->sa_family != AF_INET);
}

void configure_sockaddr(struct sockaddr_in *socket_address, char *address)
{
    socket_address->sin_family = AF_INET; // IPv4
    socket_address->sin_port = htons(0);
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
    ip->ip_id = htons(321);
    ip->ip_off = htons(0);
    ip->ip_hl = 5;
    ip->ip_len = htons(length);
    ip->ip_sum = cksum((unsigned short *)ip, ip->ip_hl);
    // Set ip addresses
    ip->ip_src = source_address.sin_addr;
    ip->ip_dst = destination_address.sin_addr;
}

void configure_icmp_header(struct icmp *icmp)
{
    icmp->icmp_code = 0;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_hun.ih_idseq.icd_id = 123;
    icmp->icmp_hun.ih_idseq.icd_seq = 0;
    icmp->icmp_cksum = cksum((unsigned short *)icmp, sizeof(struct icmp));
}