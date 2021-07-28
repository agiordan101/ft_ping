#include "ft_ping.h"

void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt, unsigned long daddr)
{
    struct sockaddr_in  destaddr = (struct sockaddr_in){
        AF_INET,
        1024,
        addr,
        {0}
    }; //htons is prohibited by the subject;

	bzero((void *)pkt->buff, PKTSIZE);
    pkt->icmphdr->type = ICMP_ECHO;
	pkt->icmphdr->code = 0; // Useless
	pkt->icmphdr->un.echo.id = getpid();
	pkt->icmphdr->un.echo.sequence = 42;
	pkt->icmphdr->checksum = checksum((void *)pkt->icmphdr, sizeof(struct icmphdr));

    print_icmphdr(pkt->icmphdr);

    // memcpy(pkt->payload, PAYLOAD, PAYLOAD_SIZE);
    // printf("Payload: >%s<\n", pkt->payload);

    int msgsend_len = sendto(
        sktfd,
        pkt->icmphdr, sizeof(pkt->icmphdr),
        0, // Flags
        (struct sockaddr *)&destaddr, sizeof(destaddr)
    );

    printf("sendto() ret:   %d\n", msgsend_len);
    printf("sendto() errno: %d\n", errno);
    if (msgsend_len == -1)
        perror(NULL), exit(1);

    (void)pkt;
    (void)destaddr;
    (void)sktfd;
    (void)addr;
    (void)msgsend_len;
    (void)daddr;
    printf("\n");
}
