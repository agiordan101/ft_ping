#include "ft_ping.h"

int     create_skt()
{
    int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    // int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); //IPPROTO_RAW donesn't need to checksum

    if (sktfd < 0)
    {
        printf("[ERROR] Socket can't be created: \n");
        perror(NULL), exit(1);
    }
    printf("Socket fd: %d\n", sktfd);
 
    // Set max hops socket can makes
    int ttl_hops = TTL;
    if (setsockopt(sktfd, SKTOPT_LVL, IP_TTL, (char *)&ttl_hops, sizeof(ttl_hops)) < 0)
        perror(NULL), exit(1);

    // Set receive timeout
    struct timeval rcv_timeout = {1, 0};
    if (setsockopt(sktfd, SKTOPT_LVL, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(rcv_timeout)) < 0)
        perror(NULL), exit(1);

    // Set messages to receive: ICMP_ECHOREPLY
    int filter = ICMP_ECHOREPLY;
    // int filter = ICMP_ECHOREPLY;
    if (setsockopt(sktfd, SKTOPT_LVL, ICMP_FILTER, (char *)&filter, sizeof(filter)) < 0)
        perror(NULL), exit(1);

    int opt_val = 1;
	if (setsockopt(sktfd, SKTOPT_LVL, IP_HDRINCL, &opt_val, sizeof(int)) < 0)
        perror(NULL), exit(1);

    return sktfd;
}
