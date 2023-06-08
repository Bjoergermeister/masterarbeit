unsigned short calculate_checksum(unsigned short *addr, int len);
char *get_interface_family(struct ifaddrs *ifaddr);
int skip_interface(struct ifaddrs *interface);
void configure_sockaddr(struct sockaddr_in *socket_address, char *address);
int open_socket();
void configure_ip_header(struct ip *ip, int length, struct sockaddr_in source, struct sockaddr_in destination);
void configure_icmp_header(struct icmp *icmp);