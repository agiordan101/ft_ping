#include "ft_ping.h"

void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt)
{
    /*
        Cette fonction marche pour un serveur C en local
        Mais pas pour ping google
        Surement une autre architecture de buff (avec iphdr et icmphdr) est necessaire

        stalk https://github.com/dgibson/iputils/blob/master/ping.c
    */
  
    char buff[PKTSIZE] = "pingpong?";
	int                 msgsend_len;
    struct sockaddr_in  destaddr = (struct sockaddr_in){AF_INET, htons(1024), addr, {0}}; //htons is prohibited by the subject;

    msgsend_len = sendto(
        sktfd,
        buff, PKTSIZE, 0,
        // (const void *)pkt, PKTSIZE, 0,
        (struct sockaddr *)&destaddr, sizeof(destaddr)
        // (void *)ai_res, sizeof(destaddr)
    );

    printf("sendto() ret: %d\n", msgsend_len);
    (void)pkt;
}

// void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt)
// // void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt, struct sockaddr_in *ai_res)
// {
//     int                 msgsend_len;
//     struct sockaddr_in  destaddr = (struct sockaddr_in){AF_INET, htons(1024), addr, {0}}; //htons is prohibited by the subject;

// 	bzero((void *)pkt->buff, PKTSIZE);

// 	pkt->iphdr->version = 4;
// 	pkt->iphdr->ihl = sizeof(*pkt->iphdr) >> 2;
// 	pkt->iphdr->ttl = TTL;
// 	pkt->iphdr->protocol = IPPROTO_ICMP;
	
//     pkt->icmphdr->type = ICMP_ECHO;
// 	pkt->icmphdr->code = 0;
// 	pkt->icmphdr->un.echo.id = getpid();
// 	pkt->icmphdr->un.echo.sequence = 1;
// 	pkt->icmphdr->checksum = checksum((void *)pkt->icmphdr, sizeof(*pkt->icmphdr));

//     printf("checksum: %d\n", pkt->icmphdr->checksum);

//     msgsend_len = sendto(
//         sktfd,
//         pkt->buff, PKTSIZE, 0,
//         // (const void *)pkt, PKTSIZE, 0,
//         (struct sockaddr *)&destaddr, sizeof(destaddr)
//         // (void *)ai_res, sizeof(destaddr)
//     );

//     printf("sendto() ret: %d\n", msgsend_len);
//     printf("errno send: %d\n", errno);
//     if (msgsend_len == -1)
//         perror(NULL), exit(1);

//     (void)pkt;
//     (void)destaddr;
//     (void)sktfd;
//     (void)addr;
//     (void)msgsend_len;
//     printf("\n");
// }



// int sendto(int socket, char *buf, int buflen, int flags,
//            struct sockaddr *addr, int addrlen);
// int recvfrom(int socket, char *buf, int buflen, int flags,
//            struct sockaddr *addr, int *addrlen);

// int bytes_sent;
// int bytes_received;
// char data_sent[256];
// char data_received[256];
// struct sockaddr_in to;
// struct sockaddr from;
// int addrlen;
// int s;

// memset(&to, 0, sizeof(to));
// to.sin_family = AF_INET;
// to.sin_addr   = inet_addr(“129.5.24.1”);
// to.sin_port   = htons(1024);
// ⋮
// bytes_sent = sendto(s, data_sent, sizeof(data_sent), 0,
//            (struct sockaddr*)&to, sizeof(to));
// ⋮
// addrlen = sizeof(from); /* must be initialized */
// bytes_received = recvfrom(s, data_received,
//    sizeof(data_received), 0, &from, &addrlen)