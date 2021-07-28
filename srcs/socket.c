#include "ft_ping.h"

int     create_skt()
{
    int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    // int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    if (sktfd < 0)
    {
        printf("[ERROR] Socket can't be created: \n");
        perror(NULL), exit(1);
    }
    printf("Socket fd: %d\n", sktfd);
 
    // Set max hops socket can makes
    int ttl_hops = TTL;
    // if (setsockopt(sktfd, IPPROTO_IP, IP_TTL, (char *)&ttl_hops, sizeof(ttl_hops)) < 0)
    if (setsockopt(sktfd, 0, IP_TTL, (char *)&ttl_hops, sizeof(ttl_hops)) < 0)
        perror(NULL), exit(1);

    // Set receive timeout
    struct timeval rcv_timeout = {(int)RECVTIMEOUTMS / 1000, RECVTIMEOUTMS % 1000};
    if (setsockopt(sktfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(rcv_timeout)) < 0)
        perror(NULL), exit(1);

    return sktfd;
}
