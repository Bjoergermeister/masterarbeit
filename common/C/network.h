void configure_sockaddr(struct sockaddr_in *socket_address, char *address, int port);
int open_socket(int type, int protocol);
void configure_ip_header(struct ip *ip, int length, struct sockaddr_in source_address, struct sockaddr_in destination_address);
void configure_icmp_header(struct icmp *icmp);