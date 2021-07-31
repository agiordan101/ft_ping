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
	pkt->icmphdr->un.echo.sequence = gdata.stats.p_sent++;
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

    printf("sendto() ret:   %d\n", msgsend_len);
    printf("sendto() errno: %d\n", errno);
    if (msgsend_len == -1)
        perror(NULL), exit(1);
    else
        gdata.stats.pktsend_time = get_time();
    printf("\n");
}

void    update_stats(t_statistics *stats)
{
    /*
        Update Sum to re-compute Avg each times.
        Compute Avg to update rtt_mdiffsum.
        Update rtt_mdiffsum to compute mdev at the end.
    */
    float   dtime =
        stats->pktrecv_time.tv_sec - stats->pktsend_time.tv_sec +
        (stats->pktrecv_time.tv_usec - stats->pktsend_time.tv_usec) / 1000.0;
    if (dtime < 0)  // WTTFFF ??? Pourquoi 1000 de diff ?  fin = debut - 1000 + temps du ping
        dtime += 1000;
    stats->p_received++;
    if (dtime < stats->rtt_min || !stats->rtt_min)
        stats->rtt_min = dtime;
    if (stats->rtt_max < dtime)
        stats->rtt_max = dtime;
    stats->rtt_sum += dtime;
    stats->rtt_avg = stats->rtt_sum / stats->p_received;
    stats->rtt_mdiffsum += ft_abs(dtime - stats->rtt_avg);
    // printf("%ld bytes from par21s22-in-f4.1e100.net (142.250.178.132): icmp_seq=4 ttl=107 time=42.3 ms\n", PKTSIZE);
    
    // printf("send: %f\n", stats->pktsend_time.tv_sec + stats->pktsend_time.tv_usec / 1000.0);
    // printf("recv: %f\n", stats->pktrecv_time.tv_sec + stats->pktrecv_time.tv_usec / 1000.0);
    // printf("dtime: %f\n", dtime);
    // printf("avg: %f\n", stats->rtt_avg);
    // printf("Add abs(%f) to stats->rtt_mdiffsum\n", dtime - stats->rtt_avg);
    // printf("Add %f to stats->rtt_mdiffsum\n", ft_abs(dtime - stats->rtt_avg));
    // if (dtime < 0)
    //     printf("wtff dtime < 0: %f\n", dtime), exit(1);
}

int     recv_pkt(int sktfd)
{
    struct msghdr       msghdr;
    char                namebuff[BUFF_SIZE];
    char                recvbuff[PKTSIZE];
    char                controlbuff[BUFF_SIZE];
    // struct sockaddr_in  buff;
    struct iovec        msgiov;
    int                 recvmsg_len = -1;

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
    recvmsg_len = recvmsg(sktfd, &msghdr, MSG_WAITALL); // Or flag MSG_DONTWAIT
    gdata.stats.pktrecv_time = get_time();

    printf("recvmsg() ret:   %d\n", recvmsg_len);
    printf("recvmsg() errno: %d\n", errno);
    if (recvmsg_len == -1)
        perror(NULL);
    printf("\n");

    // print_msghdr(&msghdr);

    struct iphdr *iphdr = (struct iphdr *)recvbuff;
    // print_iphdr(iphdr);

    struct icmphdr *icmphdr = (struct icmphdr *)(recvbuff + (iphdr->ihl * 4));
    // print_icmphdr(icmphdr);

    if (icmphdr->type != ICMP_ECHOREPLY ||
        icmphdr->code != 0 ||
        icmphdr->un.echo.id != gdata.pid ||
        icmphdr->un.echo.sequence != gdata.stats.p_sent - 1)
        printf("icmphdr recieved is wrong\n"), exit(1);
    else
        update_stats(&gdata.stats);
    
    printf("\n");
    return recvmsg_len;
}