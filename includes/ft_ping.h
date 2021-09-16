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

#define IPHDR_SIZE      sizeof(struct iphdr)
#define ICMPHDR_SIZE    sizeof(struct icmphdr)
#define PAYLOAD         "Salut google, ca dis quoi le boss ?"
#define PAYLOAD_SIZE    sizeof(PAYLOAD)
// #define PKTSIZE         IPHDR_SIZE + ICMPHDR_SIZE
#define PKTSIZE         IPHDR_SIZE + ICMPHDR_SIZE + PAYLOAD_SIZE

# define TTL            60
# define RECVTIMEOUTMS  1000
# define DTIMEPKT       1000

# define BUFF_SIZE      420 //42?
// #define SKTOPT_LVL      IPPROTO_ICMP
// #define SKTOPT_LVL      IPPROTO_IP
// #define SKTOPT_LVL      SOL_SOCKET
// #define SKTOPT_LVL         SOL_RAW
// #define SKTOPT_LVL         SOL_IP

typedef struct      s_pkt
{
	char			buff[ICMPHDR_SIZE + PAYLOAD_SIZE];      //Buffer with headers and payload
	// struct iphdr    *iphdr;
	struct icmphdr  *icmphdr;   // Point into buff
    char            *payload;   // Point into buff
    struct sockaddr *daddr;     //Destination address
}				    t_pkt;

typedef struct      s_statistics {
    struct timeval  pktsend_time;
    struct timeval  pktrecv_time;
    float           pkt_dtime;          // milliseconds
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
    int             pid;
    char            *hostname;
    char            ipv4[INET_ADDRSTRLEN];
    struct addrinfo *res;           // List of internet address
    t_pkt           *pkt;
    struct timeval  start_time;     // Program start time
    struct timeval  end_time;       // Program end time
    int             pinging_loop;
    t_statistics    stats;
    int             dtime_pktsend;
    bool            verbose;
    bool            floodping;
    int             maxreplies;
    int             recv_timeout;
    int             ttl;
}                   t_gdata;

extern t_gdata      gdata;

int             pinging(struct in_addr addr);
int             create_skt();
void            init_pkt(t_pkt *pkt, struct sockaddr_in *destaddr);
void            fill_pkt(t_pkt *pkt, int p_seq);
void            send_pkt(int sktfd, t_pkt *pkt);
void            recv_pkt(int sktfd, t_statistics *stats, int p_seq);
void            update_stats(t_statistics *stats);

float			ft_abs(float x);
unsigned short	checksum(unsigned short *data, int len);
// struct sockaddr_in  *get_addrinfo(char *hostname);

struct timeval  get_time();
void			print_iphdr(struct iphdr *iphdr);
void			print_msghdr(struct msghdr *msghdr);
void			print_icmphdr(struct icmphdr *icmphdr);
void			print_addrinfo(struct addrinfo *addrinfo);

void            print_usage();
void			print_stats(t_statistics *stats);
void            print_successfull_recv(t_statistics *stats, int recvlen, int ttl);

void            SIGINT_handler();
void            free_all();
void            freexit(int exit_code);
int 			isfirsttimeupper(struct timeval time, struct timeval to_cmp);
