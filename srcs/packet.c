#include "ft_ping.h"

void    init_pkt(t_pkt *pkt, struct sockaddr_in *destaddr)
{
    if (!(pkt->buff = (char *)calloc(1, PKTSIZE)))
        perror(NULL), exit(1);
    pkt->icmphdr = (struct icmphdr *)pkt->buff;
    pkt->icmphdr->type = ICMP_ECHO;
	pkt->icmphdr->code = 0; // Useless
	pkt->icmphdr->un.echo.id = gdata.pid;
    pkt->daddr = (struct sockaddr *)destaddr;
    // memcpy(pkt->payload, PAYLOAD, PAYLOAD_SIZE);
}

void    fill_pkt(t_pkt *pkt)
{
	pkt->icmphdr->un.echo.sequence = gdata.stats.p_sent;
    pkt->icmphdr->checksum = 0;
    pkt->icmphdr->checksum = checksum((void *)pkt->icmphdr, sizeof(struct icmphdr));
}

void    send_pkt(int sktfd, t_pkt *pkt)
{
    // print_icmphdr(pkt->icmphdr);
    // printf("Payload: >%s<\n", pkt->payload);

    int msgsend_len = sendto(
        sktfd,
        pkt->icmphdr, sizeof(pkt->icmphdr),
        0, // Flags
        pkt->daddr, sizeof(*pkt->daddr)
    );

    // printf("sendto() ret:   %d\n", msgsend_len);
    // printf("sendto() errno: %d\n", errno);
    if (msgsend_len == -1)
        perror(NULL), exit(1);
    else
    {
        // stats->p_sent++;
        gdata.stats.pktsend_time = get_time();
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

int     recv_pkt(int sktfd, t_statistics *stats)
{
    struct msghdr       msghdr;
    char                namebuff[BUFF_SIZE];
    char                recvbuff[PKTSIZE];
    char                controlbuff[BUFF_SIZE];
    // struct sockaddr_in  buff;
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

    // printf("recvmsg() ret:   %d\n", recvlen);
    // printf("recvmsg() errno: %d\n", errno);
    // if (recvlen == -1)
    //     perror(NULL);
    // printf("\n");

    // print_msghdr(&msghdr);

    struct iphdr *iphdr = (struct iphdr *)recvbuff;
    // print_iphdr(iphdr);

    struct icmphdr *icmphdr = (struct icmphdr *)(recvbuff + (iphdr->ihl * 4));
    // print_icmphdr(icmphdr);

    stats->p_sent++; // Save packet sending here to not distort CRTL+C statistics
    if (icmphdr->type != ICMP_ECHOREPLY ||
        icmphdr->code != 0 ||
        icmphdr->un.echo.id != gdata.pid ||
        icmphdr->un.echo.sequence != stats->p_sent - 1)
    {
        // printf("icmphdr recieved is wrong\n");
        // exit(1);
    }
    else
    {
        stats->p_received++; // Need to increase p_received before update_stats()
        // printf("Update stats...\n");

        // Compute ping for this packet (delta time)
        stats->pkt_dtime = (stats->pktrecv_time.tv_usec - stats->pktsend_time.tv_usec) / 1000.0;
        if (stats->pktsend_time.tv_sec != stats->pktrecv_time.tv_sec)
            stats->pkt_dtime += (stats->pktrecv_time.tv_sec - stats->pktsend_time.tv_sec) * 1000;

        update_stats(stats);
        print_successfull_recv(stats, recvlen);
    }
    
    // printf("\n");
    return recvlen;
}