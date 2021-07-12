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

void    recv_pkt(int sktfd, t_pkt *pkt)
{
    // char            *msg_recv;
    char            *metadata;
    struct iovec    msg_iov;
    struct msghdr   msghdr;
    ssize_t         msgrecv_len;
 
    // msg_recv = calloc(1, MSGRECV_LEN);
    metadata = calloc(1, METADATA_LEN);
    // msg_iov = (struct iovec){&msg_recv, MSGRECV_LEN};

    msg_iov = (struct iovec){pkt->buff, sizeof(pkt->buff)};
    
    msghdr = (struct msghdr){
        "ft_ping", 7,
        &msg_iov, 1,
        &metadata, METADATA_LEN,
        0
    };

    sleep(1);
    //ret = -1 => EAGAIN => Nothing to receive
    msgrecv_len = recvmsg(sktfd, &msghdr, MSG_WAITALL);
    // msgrecv_len = recvmsg(sktfd, &msghdr, MSG_DONTWAIT);
    printf("errno: %d\n", errno);
    printf("msgrecv_len: %ld\n\n", msgrecv_len);

    // printf("msg_control:\n");
    // int ret = write(1, msghdr.msg_control, msghdr.msg_controllen);
    // (void)ret;

    int i = 0;
    while (i < (int)msghdr.msg_iovlen)
    {
        char buff[1024];
        bzero(buff, sizeof(buff));
        strncpy(buff, msghdr.msg_iov[i].iov_base, msghdr.msg_iov[i].iov_len);
        printf("msghdr.msg_iov[%d].iov_base: >%s<\n", i, buff);

        i++;
    }
    if (msgrecv_len == -1)
        perror(NULL), exit(1);
}