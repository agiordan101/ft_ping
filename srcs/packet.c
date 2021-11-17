#include "ft_ping.h"

void    init_pkt(t_pkt *pkt, struct sockaddr_in *destaddr)
{
    // Data set one time for all packets
    pkt->icmphdr = (struct icmphdr *)pkt->buff;
    pkt->payload = pkt->buff + ICMPHDR_SIZE;
    ft_memcpy(pkt->payload, PAYLOAD, PAYLOAD_SIZE);
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
    int msgsend_len = sendto(
        sktfd,                                      // How
        pkt->icmphdr, ICMPHDR_SIZE + PAYLOAD_SIZE,  // What
        0,                                          // Flags
        pkt->daddr, sizeof(*pkt->daddr)             // Where
    );

    if (msgsend_len == -1)
        perror(NULL), freexit(EXIT_FAILURE);
    else
    {
        gdata.stats.pktsend_time = get_time();
        gdata.stats.p_sent++;
    }
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
}

void    recv_pkt(int sktfd, t_statistics *stats, int p_seq)
{
    struct msghdr       msghdr;
    char                namebuff[BUFF_SIZE];
    char                recvbuff[PKTSIZE];
    char                controlbuff[BUFF_SIZE];
    struct iovec        msgiov;
    int                 recvlen = -1;

    // printf("sizeof(recvbuff): %ld\n", sizeof(recvbuff));
    ft_bzero(recvbuff, PKTSIZE);
    msgiov = (struct iovec){&recvbuff, PKTSIZE};
    // msgiov = (struct iovec){&recvbuff, sizeof(recvbuff)};

    msghdr = (struct msghdr){
        (void *)&namebuff, sizeof(namebuff),
        &msgiov, 1,
        controlbuff, sizeof(controlbuff),
        0
    };

    recvlen = recvmsg(sktfd, &msghdr, MSG_WAITALL);
    stats->pktrecv_time = get_time();

    if (recvlen == -1)
    {
        if (gdata.pinging_loop == false)
            stats->p_sent--; // Do not distort statistics when CRTL+C crashes recvmsg() function
        else
            return ;
    }


    struct iphdr *iphdr = (struct iphdr *)recvbuff;
    struct icmphdr *icmphdr = (struct icmphdr *)(recvbuff + (iphdr->ihl * 4));

    char payload[PAYLOAD_SIZE + 1];
    ft_bzero(payload, PAYLOAD_SIZE + 1);
    ft_memcpy(payload, (char *)icmphdr + ICMPHDR_SIZE, PAYLOAD_SIZE);

    printf("recvlen: %d\n", recvlen);
    printf("PKTSIZE: %ld\n", PKTSIZE);
    printf("ICMPHDR_SIZE: %ld\n", ICMPHDR_SIZE);
    printf("iphdr->ihl: %d\n", iphdr->ihl);
    printf("icmphdr->type: %d = %d\ticmphdr->code: %d\t\n", icmphdr->type, ICMP_ECHOREPLY, icmphdr->code);
    printf("icmphdr->un.echo.id: %d\ticmphdr->un.echo.sequence: %d\t\n", icmphdr->un.echo.id, icmphdr->un.echo.sequence);
    printf("gdata.pid : %d\tp_seq: %d\t\n", gdata.pid , p_seq);
    printf("payload: %s (len=%ld)\tPAYLOAD: %s (len=%ld)\n", payload, strlen(payload), PAYLOAD, strlen(PAYLOAD));
    if (icmphdr->type == ICMP_ECHOREPLY &&
        icmphdr->code == 0 &&
        icmphdr->un.echo.id == gdata.pid &&
        icmphdr->un.echo.sequence <= p_seq &&
        !ft_strcmp(payload, PAYLOAD))
    {
        stats->p_received++; // Need to increase p_received before update_stats()
        update_stats(stats);
        gdata.print_recv(stats, recvlen, iphdr->ttl);
    }
}
