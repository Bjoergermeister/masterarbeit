void configure_sockaddr(struct sockaddr_in *socket_address, char *address);
int open_socket();
void configure_ip_header(struct ip *ip, int length, struct sockaddr_in source, struct sockaddr_in destination);
void configure_icmp_header(struct icmp *icmp);