#include "ft_ping.h"

int     create_skt()
{
    int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sktfd < 0)
    {
        printf("[ERROR] Socket can't be created: \n");
        perror(NULL), freexit(EXIT_FAILURE);
    }
    // printf("Socket fd: %d\n", sktfd);
 
    // Set max hops socket can makes
    if (setsockopt(sktfd, IPPROTO_IP, IP_TTL, (char *)&gdata.ttl, sizeof(gdata.ttl)) < 0)
        perror(NULL), freexit(EXIT_FAILURE);

    // Set receive timeout
    struct timeval rcv_timeout = {gdata.recv_timeout / 1000, 1000 * (gdata.recv_timeout % 1000)};
    // printf("Receive timeout: %ldk%ld\n", rcv_timeout.tv_sec, rcv_timeout.tv_usec);
    if (setsockopt(sktfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(rcv_timeout)) < 0)
        perror(NULL), freexit(EXIT_FAILURE);

    return sktfd;
}
