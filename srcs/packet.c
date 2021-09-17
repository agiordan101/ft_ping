#include "ft_ping.h"

void    init_pkt(t_pkt *pkt, struct sockaddr_in *destaddr)
{
    // Data set one time for all packets
    // if (!(pkt->buff = (char *)calloc(1, ICMPHDR_SIZE + PAYLOAD_SIZE)))
    //     perror(NULL), freexit(EXIT_FAILURE);
    pkt->icmphdr = (struct icmphdr *)pkt->buff;
    pkt->payload = pkt->buff + ICMPHDR_SIZE;
    memcpy(pkt->payload, PAYLOAD, PAYLOAD_SIZE);
    pkt->icmphdr->type = ICMP_ECHO;
	pkt->icmphdr->code = 0; // Useless
	pkt->icmphdr->un.echo.id = gdata.pid;
    pkt->daddr = (struct sockaddr *)destaddr;
}

void    fill_pkt(t_pkt *pkt, int p_seq)
{
	pkt->icmphdr->un.echo.sequence = p_seq;
    pkt->icmphdr->checksum = 0;
    pkt->icmphdr->checksum = checksum((void *)pkt->icmphdr, ICMPHDR_SIZE + PAYLOAD_SIZE);
}

void    send_pkt(int sktfd, t_pkt *pkt)
{
    // print_icmphdr(pkt->icmphdr);
    // printf("Payload: >%s<\n", pkt->payload);

    int msgsend_len = sendto(
        sktfd,                                  // How
        // pkt->icmphdr, sizeof(pkt->icmphdr),     // What
        pkt->icmphdr, ICMPHDR_SIZE + PAYLOAD_SIZE,     // What
        0, // Flags
        pkt->daddr, sizeof(*pkt->daddr)         // Where
    );

    // printf("sendto() ret:   %d\n", msgsend_len);
    // printf("sendto() errno: %d\n", errno);
    if (msgsend_len == -1)
        perror(NULL), freexit(EXIT_FAILURE);
    else
    {
        gdata.stats.pktsend_time = get_time();
        gdata.stats.p_sent++;
    }
    // printf("\n");
}

void    update_stats(t_statistics *stats)
{
    /*
        Update Sum to re-compute Avg each times.
        Compute Avg to update rtt_mdiffsum.
        Update rtt_mdiffsum to compute mdev at the end.
    */
    // Compute delta time for this packet
    stats->pkt_dtime = (stats->pktrecv_time.tv_usec - stats->pktsend_time.tv_usec) / 1000.0;
    if (stats->pktsend_time.tv_sec != stats->pktrecv_time.tv_sec)
        stats->pkt_dtime += (stats->pktrecv_time.tv_sec - stats->pktsend_time.tv_sec) * 1000;

    // Update statistics
    if (stats->pkt_dtime < stats->rtt_min || !stats->rtt_min)
        stats->rtt_min = stats->pkt_dtime;
    if (stats->rtt_max < stats->pkt_dtime)
        stats->rtt_max = stats->pkt_dtime;
    stats->rtt_sum += stats->pkt_dtime;
    stats->rtt_avg = stats->rtt_sum / stats->p_received;
    stats->rtt_mdiffsum += ft_abs(stats->pkt_dtime - stats->rtt_avg);
    
    // printf("send sec : %ld\n", stats->pktsend_time.tv_sec);
    // printf("send usec: %ld\n", stats->pktsend_time.tv_usec / 1000);

    // printf("send: %f\n", stats->pktsend_time.tv_sec + stats->pktsend_time.tv_usec / 1000.0);
    // printf("recv: %f\n", stats->pktrecv_time.tv_sec + stats->pktrecv_time.tv_usec / 1000.0);
    
    // printf("stats->pkt_dtime: %f\n", stats->pkt_dtime);
    // printf("avg: %f\n", stats->rtt_avg);
    // printf("Add abs(%f) to stats->rtt_mdiffsum\n", stats->pkt_dtime - stats->rtt_avg);
    // printf("Add %f to stats->rtt_mdiffsum\n", ft_abs(stats->pkt_dtime - stats->rtt_avg));
}

void    recv_pkt(int sktfd, t_statistics *stats, int p_seq)
{
    struct msghdr       msghdr;
    char                namebuff[BUFF_SIZE];
    char                recvbuff[PKTSIZE];
    char                controlbuff[BUFF_SIZE];
    struct iovec        msgiov;
    int                 recvlen = -1;

    bzero((void *)&namebuff, sizeof(namebuff));
    bzero((void *)&recvbuff, sizeof(recvbuff));
    bzero((void *)&controlbuff, sizeof(controlbuff));
    
    msgiov = (struct iovec){&recvbuff, sizeof(recvbuff)};

    msghdr = (struct msghdr){
        (void *)&namebuff, sizeof(namebuff),
        &msgiov, 1,
        controlbuff, sizeof(controlbuff),
        0
    };

    // ret = -1 => EAGAIN => Nothing to receive
    recvlen = recvmsg(sktfd, &msghdr, MSG_WAITALL); // Or flag MSG_DONTWAIT

    stats->pktrecv_time = get_time();

    // printf("stats->pkt_dtime: %ld ms / recvlen: %d\n", stats->pkt_dtime, recvlen);

    // printf("Send time: %ld k %ld\n", stats->pktsend_time.tv_sec, stats->pktsend_time.tv_usec);
    // printf("Recv time: %ld k %ld\n", stats->pktrecv_time.tv_sec, stats->pktrecv_time.tv_usec);
    // printf("stats->pkt_dtime: %f\n", stats->pkt_dtime);
    // printf("recvmsg() ret:   %d\n", recvlen);
    if (recvlen == -1)
    {
        if (gdata.pinging_loop == false)
            stats->p_sent--; // Do not distort statistics when CRTL+C crashes recvmsg() function
        else
            return ;
    }
    // print_msghdr(&msghdr);

    struct iphdr *iphdr = (struct iphdr *)recvbuff;

    struct icmphdr *icmphdr = (struct icmphdr *)(recvbuff + (iphdr->ihl * 4));

    char *payload = (char *)icmphdr + ICMPHDR_SIZE;
    // printf("Payload: %s\n", payload);

    if (icmphdr->type == ICMP_ECHOREPLY &&
        icmphdr->code == 0 &&
        icmphdr->un.echo.id == gdata.pid &&
        icmphdr->un.echo.sequence == p_seq &&
        !strcmp(payload, PAYLOAD))
    {
        stats->p_received++; // Need to increase p_received before update_stats()
        update_stats(stats);
        gdata.print_recv(stats, recvlen, iphdr->ttl);
    }
    // else
    // {
    //     printf("icmphdr recieved is wrong\n");
    //     printf("Attemps icmphdr->type %d / icmphdr->code %d / icmphdr->un.echo.id %d / icmphdr->un.echo.sequence %d\n", ICMP_ECHOREPLY, 0, gdata.pid, p_seq);
    //     printf("Receive icmphdr->type %d / icmphdr->code %d / icmphdr->un.echo.id %d / icmphdr->un.echo.sequence %d\n", icmphdr->type, icmphdr->code, icmphdr->un.echo.id, icmphdr->un.echo.sequence);
    //     print_iphdr(iphdr);
    //     print_icmphdr(icmphdr);
    //     freexit(EXIT_FAILURE);
    // }
}