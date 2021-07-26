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

// void    recv_pkt(int sktfd, t_pkt *pkt)
// {
//     /*
//         Test reception google ECHO REPLY 
//     */
// 	printf("recvfrom() ...\n");

// 	// received echoed data back
//     int len = 100;
// 	char buffer[len];
// 	recvfrom(sktfd, buffer, len, 0, NULL, NULL);
// 	buffer[len] = '\0';
// 	printf("recieved: '%s'\n", buffer);

//     (void)pkt;
// }

int     recv_pkt(int sktfd, t_pkt *pkt)
{
    struct msghdr       msghdr;
    struct sockaddr_in  target;
    struct iovec        msgiov;
    struct icmphdr      icmphdr;
    char                metadata[METADATA_LEN];
    int                 recvmsg_len = -1;

    // while (recvmsg_len == -1)
    // {
    msgiov = (struct iovec){&icmphdr, sizeof(icmphdr)};

    msghdr = (struct msghdr){
        (void *)&target, sizeof(target),
        &msgiov, 1,
        metadata, METADATA_LEN,
        0
    };

    // ret = -1 => EAGAIN => Nothing to receive
    recvmsg_len = recvmsg(sktfd, &msghdr, RECVMSG_FLAGS);

    printf("recvmsg() ret:   %d\n", recvmsg_len);
    printf("recvmsg() errno: %d\n", errno);
    // }

    // printf("msg_recv: >%s<\n", msg_recv);
    // printf("pkt->buff: >%s<\n", pkt->buff);
    // printf("msghdr flags: >%d< =?= >%d<\n", msghdr.msg_flags, RECVMSG_FLAGS);
    // int ret = write(1, msghdr.msg_control, msghdr.msg_controllen);
    // (void)ret;

    printf("\n");

    printf("icmphdr type: %d\n", icmphdr.type);
    printf("icmphdr code: %d\n", icmphdr.code);

    printf("msghdr.msg_name->sin_addr.s_addr (struct sockaddr_in): %d\n", target.sin_addr.s_addr);
    printf("msghdr.msg_flags (int): %d --- %d %d %d %d %d\n", msghdr.msg_flags, MSG_EOR, MSG_TRUNC, MSG_CTRUNC, MSG_OOB, MSG_ERRQUEUE);

    printf("msghdr.msg_iovlen (size_t): %ld\n", msghdr.msg_iovlen);
    int i = 0;
    while (i < (int)msghdr.msg_iovlen)
    {
        char buff[1024];
        bzero(buff, sizeof(buff));
        strncpy(buff, msghdr.msg_iov[i].iov_base, msghdr.msg_iov[i].iov_len);
        printf("msghdr.msgiov[%d].iov_base (len=%d): >%s<\n", i, (int)msghdr.msg_iov[i].iov_len, buff);
        i++;
    }

    char *buff = calloc(msghdr.msg_controllen, sizeof(char));
    memcpy(buff, msghdr.msg_control, msghdr.msg_controllen);
    printf("msghdr.msg_control (void *): >%s<\n", buff);
    printf("msghdr.msg_controllen (size_t): %ld\n", msghdr.msg_controllen);

    if (recvmsg_len == -1)
        perror(NULL);
        // perror(NULL), exit(1);
    printf("\n");
    (void)pkt;
    return recvmsg_len;
}