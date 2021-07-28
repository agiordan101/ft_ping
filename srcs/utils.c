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

void			print_iphdr(struct iphdr *iphdr)
{
	printf("iphdr: %p\n", iphdr);
	printf("iphdr->version: %d\n", iphdr->version);
	printf("iphdr->ihl: %d\n", iphdr->ihl);
	printf("iphdr->tos: %d\n", iphdr->tos);
	printf("iphdr->tot_len: %d\n", iphdr->tot_len);
	printf("iphdr->id: %d\n", iphdr->id);
	printf("iphdr->frag_off: %d\n", iphdr->frag_off);
	printf("iphdr->ttl: %d\n", iphdr->ttl);
	printf("iphdr->protocol: %d\n", iphdr->protocol);
	printf("iphdr->saddr: %d\n", iphdr->saddr);
	printf("iphdr->daddr: %d\n", iphdr->daddr);
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
