#include "ft_ping.h"

void            free_all()
{
    if (gdata.res)
        freeaddrinfo(gdata.res);
}

void            freexit(int exit_code)
{
    free_all();
    exit(exit_code);
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
    int     		ret;

    if ((ret = gettimeofday(&time, NULL)))
    {
        printf("Unable to get time, gettimeofday() ret: %d\n", ret);
        freexit(EXIT_FAILURE);
    }
    return time;
}

int				isfirsttimeupper(struct timeval time, struct timeval to_cmp)
{
	if (time.tv_sec == to_cmp.tv_sec)
		return time.tv_usec > to_cmp.tv_usec ? 1 : 0;
	return time.tv_sec > to_cmp.tv_sec ? 1 : 0;
}
