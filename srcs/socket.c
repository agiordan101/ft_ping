#include "ft_ping.h"

int     create_skt()
{
    int sktfd;

    if ((sktfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        printf("[ERROR] Socket can't be created: %d\n", sktfd);
        perror(NULL), freexit(EXIT_FAILURE);
    }
    // printf("Socket fd: %d\n", sktfd);
 
    // Set max hops socket can makes
    if (setsockopt(sktfd, IPPROTO_IP, IP_TTL, (char *)&gdata.ttl, sizeof(gdata.ttl)) < 0)
        perror(NULL), freexit(EXIT_FAILURE);

    // printf("sfbzgb\n");
    // Set receive timeout
    struct timeval rcv_timeout = {gdata.recv_timeout / 1000, 1000 * (gdata.recv_timeout % 1000)};
    // printf("Receive timeout: %ldk%ld\n", rcv_timeout.tv_sec, rcv_timeout.tv_usec);
    if (setsockopt(sktfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(rcv_timeout)) < 0)
        perror(NULL), freexit(EXIT_FAILURE);

    return sktfd;
}

void            SIGINT_handler()
{
    gdata.pinging_loop = false;
}

int             pinging(struct in_addr addr)
{
    t_pkt               pkt;
    int                 sktfd;
    int                 p_seq = 0;
    struct timeval      nextpktsend_time;
    struct sockaddr_in  destaddr = (struct sockaddr_in){
        AF_INET, 42, addr, {0}
    };

    // printf("Send time: %ld k %ld\n", lastpktsent_time->tv_sec, lastpktsent_time->tv_usec);
    gdata.pkt = &pkt;
    sktfd = create_skt();
    init_pkt(&pkt, &destaddr);

    printf("PING %s (%s) %d(%d) bytes of data.\n", gdata.hostname, gdata.ipv4, 56, 84);

    gdata.start_time = get_time();
    nextpktsend_time = gdata.start_time;
    while (gdata.pinging_loop)
    {
        // printf("gdata.floodping: %d\n", gdata.floodping);
        if (gdata.floodping == true || isfirsttimeupper(get_time(), nextpktsend_time))
        {
            fill_pkt(&pkt, p_seq);
            send_pkt(sktfd, &pkt);
            recv_pkt(sktfd, &gdata.stats, p_seq);

            p_seq++;
            if (gdata.maxreplies != -1 && p_seq == gdata.maxreplies)
                break ;

            nextpktsend_time.tv_usec += 1000 * (gdata.dtime_pktsend % 1000);
            nextpktsend_time.tv_sec += gdata.dtime_pktsend / 1000 + (nextpktsend_time.tv_usec >= 1000000 ? 1 : 0);
            if (nextpktsend_time.tv_usec >= 1000000)
                nextpktsend_time.tv_usec -= 1000000;
        }
    }
    gdata.end_time = get_time();
    close(sktfd);
    return 0;
}

