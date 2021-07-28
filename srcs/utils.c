#include "ft_ping.h"

// struct sockaddr_in  *get_addrinfo(char *hostname)
// {
// 	struct addrinfo hints;
// 	struct addrinfo *result;

// 	bzero(&hints, sizeof(hints));
// 	hints.ai_family = AF_INET;
// 	hints.ai_socktype = SOCK_RAW;
// 	hints.ai_protocol = IPPROTO_ICMP;
// 	if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
// 		return (NULL);
// 	return (struct sockaddr_in *)result->ai_addr;
// }

void			print_msghdr(struct msghdr *msghdr)
{
	char	buff[1024];
	int		rw = -1;

	printf("msghdr: %p\n", msghdr);

	// msg_name
	bzero(buff, sizeof(buff));
	memcpy(buff, msghdr->msg_name, msghdr->msg_namelen);
    printf("msghdr->msg_name (len=%d): >%s<\n", msghdr->msg_namelen, buff);
    // printf("msghdr->msg_name (len=%d): >\n", msghdr->msg_namelen);
	// rw = write(1, (char *)msghdr->msg_name, msghdr->msg_namelen);
	// printf("<\n");

	// msg_iov
    printf("msghdr->msg_iovlen: %ld\n", msghdr->msg_iovlen);
    int i = 0;
    while (i < (int)msghdr->msg_iovlen)
    {
        bzero(buff, sizeof(buff));
        memcpy(buff, msghdr->msg_iov[i].iov_base, msghdr->msg_iov[i].iov_len);
        printf(" - msghdr->msgiov[%d].iov_base (len=%d): >%s<\n", i, (int)msghdr->msg_iov[i].iov_len, buff);

		// printf(" - msghdr->msg_iov[%d].iov_base (len=%ld): >\n", i, msghdr->msg_iov[i].iov_len);
		// rw = write(1, msghdr->msg_iov[i].iov_base, msghdr->msg_iov[i].iov_len);
		// printf("<\n");

		i++;
    }

	// msg_control
	bzero(buff, sizeof(buff));
	memcpy(buff, msghdr->msg_control, msghdr->msg_controllen);
    printf("msghdr.msg_control (len=%ld): >%s<\n", msghdr->msg_controllen, buff);
    // printf("msghdr->msg_control (len=%ld): >\n", msghdr->msg_controllen);
	// rw = write(1, msghdr->msg_control, msghdr->msg_controllen);
	// printf("<\n");

	printf("msghdr->msg_flags: %d\n", msghdr->msg_flags);
    printf("\n");
	(void)rw;
	(void)buff;
}


void			print_iphdr(struct iphdr *iphdr)
{
    char			*ipv4 = malloc(INET_ADDRSTRLEN);
	struct in_addr	addr;

	printf("iphdr: %p\n", iphdr);
	printf("iphdr->version: %d\n", iphdr->version);
	printf("iphdr->ihl: %d\n", iphdr->ihl);
	printf("iphdr->tos: %d\n", iphdr->tos);
	printf("iphdr->tot_len: %d\n", iphdr->tot_len);
	printf("iphdr->id: %d\n", iphdr->id);
	printf("iphdr->frag_off: %d\n", iphdr->frag_off);
	printf("iphdr->ttl: %d\n", iphdr->ttl);
	printf("iphdr->protocol: %d\n", iphdr->protocol);

	addr = (struct in_addr){iphdr->saddr};
	inet_ntop(AF_INET, &addr, ipv4, INET_ADDRSTRLEN);
	printf("iphdr->saddr: %s\n", ipv4);

	addr = (struct in_addr){iphdr->daddr};
	inet_ntop(AF_INET, &addr, ipv4, INET_ADDRSTRLEN);
	printf("iphdr->daddr: %s\n", ipv4);
    printf("\n");
}

void			print_icmphdr(struct icmphdr *icmphdr)
{
	printf("icmphdr: %p\n", icmphdr);
	printf("icmphdr->type: %d\n", icmphdr->type);
    printf("icmphdr->code: %d\n", icmphdr->code);
    printf("icmphdr->checksum: %d\n", icmphdr->checksum);
    printf("icmphdr->un.echo.sequence: %d\n", icmphdr->un.echo.sequence);
    printf("icmphdr->un.echo.id: %d\n", icmphdr->un.echo.id);
    printf("icmphdr->un.gateway: %d\n", icmphdr->un.gateway);
    printf("\n");
}

unsigned short	checksum(unsigned short *data, int len)
{
	unsigned long	checksum;

	checksum = 0;
	while (len > 1)
	{
		checksum = checksum + *data++;
		len = len - sizeof(unsigned short);
	}
	if (len)
		checksum = checksum + *(unsigned char*)data;
	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum = checksum + (checksum >> 16);
	return (unsigned short)(~checksum);
}

struct timeval  get_time()
{
    struct timeval  time;
    int     ret;

    if ((ret = gettimeofday(&time, NULL)))
    {
        printf("Unable to get time, gettimeofday() ret: %d\n", ret);
        exit(0);
    }
    return time;
}
