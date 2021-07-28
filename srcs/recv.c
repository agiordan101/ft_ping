#include "ft_ping.h"

// void	init_header(void)
// {
// 	t_res	*res;

// 	res = &g_params->response;
// 	ft_bzero((void *)g_params->pckt.buf, PACKET_PING_SIZE);
// 	ft_bzero(res, sizeof(t_res));
// 	res->iov->iov_base = (void *)g_params->pckt.buf;
// 	res->iov->iov_len = sizeof(g_params->pckt.buf);
// 	res->msg.msgiov = res->iov;
// 	res->msg.msgiovlen = 1;
// 	res->msg.msg_name = NULL;
// 	res->msg.msg_namelen = 0;
// 	res->msg.msg_flags = MSG_DONTWAIT;
// }

int    recv_pkt(int sktfd)
{
    // char rbuf[sizeof(struct iphdr) + sizeof(struct icmphdr)];
    char rbuf[sizeof(struct iphdr) + sizeof(struct icmphdr) + PAYLOAD_SIZE];
    struct sockaddr_in raddr;
    socklen_t raddr_len;

    int recvmsg_len = recvfrom(sktfd,
        rbuf, sizeof(rbuf),
        0 /* flags */,
        (struct sockaddr*)&raddr, &raddr_len
    );
    printf("recvmsg() ret:   %d\n", recvmsg_len);
    printf("recvmsg() errno: %d\n\n", errno);

    struct iphdr    *iphdr = (struct iphdr *)rbuf;
    struct icmphdr  *icmphdr = (struct icmphdr *)(rbuf + (iphdr->ihl * 4));
    char            *payload = (char *)(icmphdr + ICMPHDR_SIZE);

    print_iphdr(iphdr);
    print_icmphdr(icmphdr);
    printf("Payload: >%s<\n", payload);
    return recvmsg_len;
}

// int     recv_pkt(int sktfd)
// {
//     struct msghdr       msghdr;
//     char                buff[IPHDR_SIZE + ICMPHDR_SIZE];
//     // struct sockaddr_in  buff;
//     struct iovec        msgiov;
//     struct icmphdr      icmphdr_buff;
//     char                metadata[METADATA_LEN];
//     int                 recvmsg_len = -1;

//     // while (recvmsg_len == -1)
//     // {
//     msgiov = (struct iovec){&icmphdr_buff, sizeof(icmphdr_buff)};

//     msghdr = (struct msghdr){
//         // (void *)&buff, sizeof(buff),
//         (void *)&buff, sizeof(buff),
//         &msgiov, 1,
//         metadata, METADATA_LEN,
//         0
//     };

//     // ret = -1 => EAGAIN => Nothing to receive
//     recvmsg_len = recvmsg(sktfd, &msghdr, RECVMSG_FLAGS);

//     printf("recvmsg() ret:   %d\n", recvmsg_len);
//     printf("recvmsg() errno: %d\n", errno);
//     // }

//     // printf("msg_recv: >%s<\n", msg_recv);
//     // printf("msghdr flags: >%d< =?= >%d<\n", msghdr.msg_flags, RECVMSG_FLAGS);
//     // int ret = write(1, msghdr.msg_control, msghdr.msg_controllen);
//     // (void)ret;

//     printf("\n");
//     printf("buff: >%s<\n", buff);

//     struct iphdr *iphdr = (struct iphdr *)buff;
//     print_icmphdr(icmphdr);

//     struct icmphdr *icmphdr = (struct icmphdr *)(buff + (iphdr->ihl * 4));
//     print_icmphdr(icmphdr);
//     print_icmphdr(&icmphdr_buff);    

//     // printf("msghdr.msg_name->sin_addr.s_addr (struct sockaddr_in): %d\n", buff.sin_addr.s_addr);
//     // printf("msghdr.msg_flags (int) (MSG_TRUNC=%d): %d --- %d %d %d %d %d\n", MSG_TRUNC, msghdr.msg_flags, MSG_EOR, MSG_TRUNC, MSG_CTRUNC, MSG_OOB, MSG_ERRQUEUE);

//     // printf("msghdr.msg_iovlen (size_t): %ld\n", msghdr.msg_iovlen);
//     // int i = 0;
//     // while (i < (int)msghdr.msg_iovlen)
//     // {
//     //     char buff[1024];
//     //     bzero(buff, sizeof(buff));
//     //     strncpy(buff, msghdr.msg_iov[i].iov_base, msghdr.msg_iov[i].iov_len);
//     //     printf("msghdr.msgiov[%d].iov_base (len=%d): >%s<\n", i, (int)msghdr.msg_iov[i].iov_len, buff);
//     //     i++;
//     // }

//     // char *buff = calloc(msghdr.msg_controllen, sizeof(char));
//     // memcpy(buff, msghdr.msg_control, msghdr.msg_controllen);
//     // printf("msghdr.msg_control (void *): >%s<\n", buff);
//     // printf("msghdr.msg_controllen (size_t): %ld\n", msghdr.msg_controllen);

//     if (recvmsg_len == -1)
//         perror(NULL);
//         // perror(NULL), exit(1);
//     printf("\n");
//     return recvmsg_len;
// }