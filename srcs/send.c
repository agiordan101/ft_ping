#include "ft_ping.h"

void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt, unsigned long daddr)
{
    int                 msgsend_len;
    struct sockaddr_in  destaddr = (struct sockaddr_in){AF_INET, htons(1024), addr, {0}}; //htons is prohibited by the subject;

	bzero((void *)pkt->buff, PKTSIZE);

	pkt->iphdr->version = 4;
	// pkt->iphdr->ihl = sizeof(*pkt->iphdr) >> 2; // Or 5 ???
	pkt->iphdr->ihl = 5; // Or sizeof(*pkt->iphdr) >> 2 ???
	pkt->iphdr->tot_len = htons(PKTSIZE);
	pkt->iphdr->ttl = TTL;
	pkt->iphdr->protocol = IPPROTO_ICMP;
	pkt->iphdr->saddr = inet_addr("127.0.1.1"); // USE inet_pton or inet_ntop
	pkt->iphdr->daddr = daddr;

    pkt->icmphdr->type = ICMP_ECHO;
	pkt->icmphdr->code = 0; // Useless
	pkt->icmphdr->un.echo.id = getpid();
	pkt->icmphdr->un.echo.sequence = 1;
	pkt->icmphdr->checksum = 0; // Useless
	pkt->icmphdr->checksum = checksum((void *)pkt->icmphdr, sizeof(struct icmphdr) + PAYLOAD_SIZE);

    // printf("checksum: %d\n", pkt->icmphdr->checksum);

    memcpy(pkt->payload, PAYLOAD, PAYLOAD_SIZE);

    msgsend_len = sendto(
        sktfd,
        pkt->icmphdr, ICMPHDR_SIZE, 0,
        (struct sockaddr *)&destaddr, sizeof(destaddr)
    );
    // msgsend_len = sendto(
    //     sktfd,
    //     pkt->buff, PKTSIZE, 0,
    //     (struct sockaddr *)&destaddr, sizeof(destaddr)
    // );

    printf("sendto() ret:   %d\n", msgsend_len);
    printf("sendto() errno: %d\n", errno);
    if (msgsend_len == -1)
        perror(NULL), exit(1);

    (void)pkt;
    (void)destaddr;
    (void)sktfd;
    (void)addr;
    (void)msgsend_len;
    printf("\n");
}
