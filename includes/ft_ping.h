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

// # define ABS(x)         (x < 0 ? -x : x)
#define PAYLOAD         "pingpong?"

#define IPHDR_SIZE      sizeof(struct iphdr)
#define ICMPHDR_SIZE    sizeof(struct icmphdr)
#define PAYLOAD_SIZE    sizeof(PAYLOAD)
#define PKTSIZE         IPHDR_SIZE + ICMPHDR_SIZE
// #define PKTSIZE         IPHDR_SIZE + ICMPHDR_SIZE + PAYLOAD_SIZE

# define TTL            60
# define RECVTIMEOUTMS  314

# define BUFF_SIZE      420
// #define SKTOPT_LVL      IPPROTO_ICMP
// #define SKTOPT_LVL      IPPROTO_IP
// #define SKTOPT_LVL      SOL_SOCKET
// #define SKTOPT_LVL         SOL_RAW
// #define SKTOPT_LVL         SOL_IP

typedef struct      s_pkt
{
    struct sockaddr *daddr;     //Destination address
	char			*buff;      //Buffer with headers and payload
	// struct iphdr    *iphdr;
	struct icmphdr  *icmphdr;
    // char            *payload;
}				    t_pkt;

typedef struct      s_statistics {
    struct timeval  pktsend_time;
    struct timeval  pktrecv_time;
    float           pkt_dtime;
    int             p_sent;             // Number of packets sent
    int             p_received;         // Number of packets received
    int             p_lost;             // Number of packets lost (p_sent - p_received)
    float           rtt_min;            // Minimum dtime
    float           rtt_max;            // Maximum dtime
    float           rtt_sum;            // Sum of dtimes
    float           rtt_avg;            // Average of dtimes
    float           rtt_mdiffsum;       // Sum of differences between dtimes and average time
}                   t_statistics;

typedef struct      s_gdata
{
    struct timeval  start_time;     // Program start time
    struct timeval  end_time;       // Program end time
    int             pid;
    int             pinging_loop;
    char            *hostname;
    char            ipv4[INET_ADDRSTRLEN];
    t_statistics    stats;
}                   t_gdata;

extern t_gdata  gdata;

int     create_skt();
void    init_pkt(t_pkt *pkt, struct sockaddr_in *destaddr);
void    fill_pkt(t_pkt *pkt);
void    send_pkt(int sktfd, t_pkt *pkt);
int     recv_pkt(int sktfd, t_statistics *stats);
void    update_stats(t_statistics *stats);

float			ft_abs(float x);
unsigned short	checksum(unsigned short *data, int len);
// struct sockaddr_in  *get_addrinfo(char *hostname);

struct timeval  get_time();
void			print_iphdr(struct iphdr *iphdr);
void			print_msghdr(struct msghdr *msghdr);
void			print_icmphdr(struct icmphdr *icmphdr);
void			print_stats(t_statistics *stats);
void            print_successfull_recv(t_statistics *stats, int recvlen);
