#include "ft_ping.h"

void hexDump(char *desc, void *addr, int len) 
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\n", buff);
}



// void	init_header(void)
// {
// 	t_res	*res;

// 	res = &g_params->response;
// 	ft_bzero((void *)g_params->pckt.buf, PACKET_PING_SIZE);
// 	ft_bzero(res, sizeof(t_res));
// 	res->iov->iov_base = (void *)g_params->pckt.buf;
// 	res->iov->iov_len = sizeof(g_params->pckt.buf);
// 	res->msg.msg_iov = res->iov;
// 	res->msg.msg_iovlen = 1;
// 	res->msg.msg_name = NULL;
// 	res->msg.msg_namelen = 0;
// 	res->msg.msg_flags = MSG_DONTWAIT;
// }

void    recv_pkt(int sktfd, t_pkt *pkt)
{
    /*
        Test reception google ECHO REPLY 
    */
	printf("recvfrom() ...\n");

	// received echoed data back
    int len = 100;
	char buffer[len];
	recvfrom(sktfd, buffer, len, 0, NULL, NULL);
	buffer[len] = '\0';
	printf("recieved: '%s'\n", buffer);

    (void)pkt;
}

// void    recv_pkt(int sktfd, t_pkt *pkt)
// {
//     char            *msg_recv;
//     // char            *metadata;
//     struct iovec    msg_iov;
//     struct msghdr   msghdr;
//     ssize_t         msgrecv_len = -1;
 
//     msg_recv = calloc(1, MSGRECV_LEN);
//     // metadata = calloc(1, METADATA_LEN);

//     while (msgrecv_len == -1)
//     {

//         // msg_iov = (struct iovec){msg_recv, sizeof(msg_recv)};
//         msg_iov = (struct iovec){pkt->buff, sizeof(pkt->buff)}; // 'E' char appear in msghdr.msg_iov[0].iov_base response

//         msghdr = (struct msghdr){
//             NULL, 0,
//             // "ft_ping", 7,
//             &msg_iov, 1,
//             NULL, 0,
//             // &metadata, METADATA_LEN,
//             RECVMSG_FLAGS
//         };

//         // ret = -1 => EAGAIN => Nothing to receive
//         msgrecv_len = recvmsg(sktfd, &msghdr, RECVMSG_FLAGS);

//         printf("msgrecv_len: %ld\n", msgrecv_len);
//     }
//     printf("errno recv: %d\n", errno);

//     printf("msg_recv: >%s<\n", msg_recv);
//     printf("pkt->buff: >%s<\n", pkt->buff);
//     printf("msghdr flags: >%d< =?= >%d<\n", msghdr.msg_flags, RECVMSG_FLAGS);
//     // int ret = write(1, msghdr.msg_control, msghdr.msg_controllen);
//     // (void)ret;

//     int i = 0;
//     while (i < (int)msghdr.msg_iovlen)
//     {
//         char buff[1024];
//         bzero(buff, sizeof(buff));
//         strncpy(buff, msghdr.msg_iov[i].iov_base, msghdr.msg_iov[i].iov_len);
//         printf("msghdr.msg_iov[%d].iov_base: >%s<\n", i, buff);

//         i++;
//     }
//     if (msgrecv_len == -1)
//         perror(NULL), exit(1);
//     printf("\n");
//     (void)pkt;
// }