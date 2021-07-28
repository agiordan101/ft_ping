#include "ft_ping.h"

// int    recv_pkt(int sktfd)
// {
//     // char rbuf[sizeof(struct iphdr) + sizeof(struct icmphdr)];
//     char rbuf[PKTSIZE];
//     struct sockaddr_in raddr;
//     socklen_t raddr_len;

//     int recvmsg_len = recvfrom(sktfd,
//         rbuf, sizeof(rbuf),
//         0 /* flags */,
//         (struct sockaddr*)&raddr, &raddr_len
//     );
//     printf("recvmsg() ret:   %d\n", recvmsg_len);
//     printf("recvmsg() errno: %d\n\n", errno);

//     struct iphdr    *iphdr = (struct iphdr *)rbuf;
//     print_iphdr(iphdr);

//     struct icmphdr  *icmphdr = (struct icmphdr *)(rbuf + (iphdr->ihl * 4));
//     print_icmphdr(icmphdr);

//     // char            *payload = calloc(1, PAYLOAD_SIZE);
//     // memcpy(payload, (char *)(icmphdr + sizeof(*icmphdr)), PAYLOAD_SIZE);
//     // printf("Payload: >%s<\n", payload);

//     return recvmsg_len;
// }

int     recv_pkt(int sktfd)
{
    struct msghdr       msghdr;
    char                namebuff[1024];
    char                recvbuff[PKTSIZE];
    char                controlbuff[1024];
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
    recvmsg_len = recvmsg(sktfd, &msghdr, RECVMSG_FLAGS);

    printf("recvmsg() ret:   %d\n", recvmsg_len);
    printf("recvmsg() errno: %d\n", errno);
    printf("\n");

    print_msghdr(&msghdr);

    struct iphdr *iphdr = (struct iphdr *)recvbuff;
    print_iphdr(iphdr);

    struct icmphdr *icmphdr = (struct icmphdr *)(recvbuff + (iphdr->ihl * 4));
    print_icmphdr(icmphdr);

    char *payload = (char *)(icmphdr + ICMPHDR_SIZE);
    // printf("payload: >%s<\n", payload);
    printf("payload: >");
    
    int mabite = write(1, payload, PAYLOAD_SIZE);
    (void)mabite;
    printf("<\n");

    if (recvmsg_len == -1)
        perror(NULL);
    printf("\n");
    return recvmsg_len;
}