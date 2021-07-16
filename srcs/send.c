#include "ft_ping.h"

void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt)
// void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt, struct sockaddr_in *ai_res)
{
    int                 msgsend_len;
    struct sockaddr_in  destaddr = (struct sockaddr_in){AF_INET, htons(42), addr, {0}}; //htons is prohibited by the subject;

	bzero((void *)pkt->iphdr, PKTSIZE);
	pkt->iphdr->version = 4;
	pkt->iphdr->ihl = sizeof(*pkt->iphdr) >> 2;
	pkt->iphdr->ttl = TTL;
	pkt->iphdr->protocol = IPPROTO_ICMP;
	
    pkt->icmphdr->type = ICMP_ECHO;
	pkt->icmphdr->code = 0;
	pkt->icmphdr->un.echo.id = getpid();
	pkt->icmphdr->un.echo.sequence = 1;
	pkt->icmphdr->checksum = checksum((void *)pkt->icmphdr, sizeof(*pkt->icmphdr));

    printf("checksum: %d\n", pkt->icmphdr->checksum);

    msgsend_len = sendto(
        sktfd,
        (const void *)pkt, PKTSIZE, 0,
        (struct sockaddr *)&destaddr, sizeof(destaddr)
        // (void *)ai_res, sizeof(destaddr)
    );

    printf("msgsend_len: %d\n", msgsend_len);
    printf("errno send: %d\n", errno);
    if (msgsend_len == -1)
        perror(NULL), exit(1);

    (void)pkt;
    (void)destaddr;
    (void)sktfd;
    (void)addr;
    (void)msgsend_len;
    printf("\n");
}