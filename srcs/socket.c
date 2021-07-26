#include "ft_ping.h"

int     create_skt()
{
    //Raw socket - if you use IPPROTO_ICMP, then kernel will fill in the correct ICMP header checksum,
    //              if IPPROTO_RAW, then it wont
    int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    // int sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    if (sktfd < 0)
    {
        printf("[ERROR] Socket can't be created: \n");
        perror(NULL), exit(1);
    }
    printf("Socket fd: %d\n", sktfd);
 
    // // Set max hops socket can makes
    // int ttl_hops = TTL;
    // // if (setsockopt(sktfd, IPPROTO_IP, IP_TTL, (char *)&ttl_hops, sizeof(ttl_hops)) < 0)
    // if (setsockopt(sktfd, 0, IP_TTL, (char *)&ttl_hops, sizeof(ttl_hops)) < 0)
    //     perror(NULL), exit(1);

    // Set receive timeout
    struct timeval rcv_timeout = {(int)RECVTIMEOUTMS / 1000, RECVTIMEOUTMS % 1000};
    if (setsockopt(sktfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(rcv_timeout)) < 0)
        perror(NULL), exit(1);

    int filter = ICMP_ECHOREPLY;
    if (setsockopt(sktfd, IPPROTO_ICMP, ICMP_FILTER, (char *)&filter, sizeof(filter)) < 0)
    // if (setsockopt(sktfd, IPPROTO_IP, ICMP_FILTER, (char *)&filter, sizeof(filter)) < 0)
    // if (setsockopt(sktfd, SOL_SOCKET, ICMP_FILTER, (char *)&filter, sizeof(filter)) < 0)
        perror(NULL), exit(1);

	// // We shall provide IP headers
    // int opt_val = 1;
	// if (setsockopt(sktfd, IPPROTO_IP, IP_HDRINCL, &opt_val, sizeof(int)) < 0)
    //     perror(NULL), exit(1);

	// // Allow socket to send datagrams to broadcast addresses
    // if (setsockopt(sktfd, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(int)) < 0)
    //     perror(NULL), exit(1);

    return sktfd;
}
