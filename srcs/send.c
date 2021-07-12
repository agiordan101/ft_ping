#include "ft_ping.h"

// void    send_pkt(int sktfd, struct in_addr addr)
// {
//     struct sockaddr_in  destaddr;
//     char                *buff;
//     ssize_t             msgsend_len;

//     destaddr = (struct sockaddr_in){AF_INET, htons(42), addr, {0}}; //htons is prohibited by the subject

//     buff = "ping";
//     printf("Buffer: %s\n", buff);
//     msgsend_len = sendto(
//         sktfd,
//         buff, 4, 0,
//         (struct sockaddr *)&destaddr, sizeof(destaddr)
//     );

//     printf("msgsend_len: %ld\n", msgsend_len);
//     if (msgsend_len == -1)
//         perror(NULL), exit(1);


//     // Get an error
//     int error_code;
//     int error_code_size = sizeof(error_code);
//     getsockopt(sktfd, SKTOPT_LVL, SO_ERROR, &error_code, (socklen_t *)&error_code_size);
//     printf("error_code: %d\n\n", error_code);
// }



void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt)
{
    int                 msgsend_len;
    struct sockaddr_in  destaddr = (struct sockaddr_in){AF_INET, htons(42), addr, {0}}; //htons is prohibited by the subject;


	pkt->iphdr->ihl = sizeof(*pkt->iphdr) >> 2;
	pkt->iphdr->ttl = TTL;
	pkt->iphdr->protocol = IPPROTO_ICMP;
	
    pkt->icmphdr->type = ICMP_ECHO;
	pkt->icmphdr->code = rand();
	pkt->icmphdr->un.echo.id = getpid();
	pkt->icmphdr->un.echo.sequence = rand();

    msgsend_len = sendto(
        sktfd,
        (const void *)pkt, PKTSIZE, 0,
        (struct sockaddr *)&destaddr, sizeof(destaddr)
    );

    printf("msgsend_len: %d\n", msgsend_len);
    if (msgsend_len == -1)
        perror(NULL), exit(1);

    (void)pkt;
    (void)destaddr;
    (void)sktfd;
    (void)addr;
    (void)msgsend_len;
}