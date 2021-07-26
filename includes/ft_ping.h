#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>

#include <sys/uio.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <linux/icmp.h> // Need both
#include <netinet/ip.h>
// #include <netinet/icmp.h>

#include <arpa/inet.h>

#define PAYLOAD         "pingpong?"

#define IPHDR_SIZE      sizeof(struct iphdr)
#define ICMPHDR_SIZE    sizeof(struct icmphdr)
#define PAYLOAD_SIZE    sizeof(PAYLOAD)
#define PKTSIZE         IPHDR_SIZE + ICMPHDR_SIZE + PAYLOAD_SIZE

# define TTL            128

// #define RECVMSG_FLAGS   MSG_DONTWAIT
#define RECVMSG_FLAGS   MSG_WAITALL
#define RECVTIMEOUTMS   1000

#define MSGRECV_LEN     128
#define METADATA_LEN    420
// #define SKTOPT_LVL      IPPROTO_ICMP
// #define SKTOPT_LVL      IPPROTO_IP
// #define SKTOPT_LVL      SOL_SOCKET
// #define SKTOPT_LVL         SOL_RAW
// #define SKTOPT_LVL         SOL_IP

typedef struct      s_pkt
{
	char			*buff;
	struct iphdr    *iphdr;
	struct icmphdr  *icmphdr;
    char            *payload;
}				    t_pkt;

typedef struct      s_statistics {
    struct timeval  begin_date;         // Start time
    int             p_sent;             // Number of packets sent
    int             p_received;         // Number of packets received
    int             p_lost;             // Number of packets lost (p_sent - p_received)
    int             rtt_n;              // ?
    float           rtt_min;            // ?
    float           rtt_max;            // ?
    float           rtt_sum;            // ?
    float           rtt_mdev;           // ?
}                   t_statistics;

int     create_skt();
void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt, unsigned long daddr);
int     recv_pkt(int sktfd, t_pkt *pkt);

unsigned short	checksum(unsigned short *data, int len);
// struct sockaddr_in  *get_addrinfo(char *hostname);

struct timeval  get_time();
