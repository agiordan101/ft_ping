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

float			ft_abs(float x)
{
	return x < 0 ? -x : x;
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
