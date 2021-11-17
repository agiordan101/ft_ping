# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <unistd.h>
# include <errno.h>
# include <netdb.h>
# include <signal.h>

# include <sys/uio.h>
# include <sys/time.h>
# include <sys/socket.h>

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# include <arpa/inet.h>

# define IPHDR_SIZE      sizeof(struct iphdr)
# define ICMPHDR_SIZE    sizeof(struct icmphdr)
# define PAYLOAD         "3.14159265358979323846264338327950288419716939937510582"
# define PAYLOAD_SIZE    sizeof(PAYLOAD) - 1
# define PKTSIZE         IPHDR_SIZE + ICMPHDR_SIZE + PAYLOAD_SIZE //-1 because PAYLOAD is a string
# define BUFF_SIZE       42

# define TTL             60
# define RECVTIMEOUTMS   1000
# define DTIMEPKT        1000

typedef struct      s_pkt
{
	char			buff[ICMPHDR_SIZE + PAYLOAD_SIZE];      //Buffer with headers and payload
	// struct icmp  *icmphdr;   // Point into buff
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
    char            reversed_hostname[BUFF_SIZE];
    char            ipv4[INET_ADDRSTRLEN];
    struct addrinfo *res;           // List of internet address
    struct sockaddr *ai_addr;
    size_t           ai_addrlen;
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
    void            (*print_recv)(t_statistics *, int, int);    // Different print function depending on hostname or ipv4 input
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

struct timeval  get_time();
// void			print_iphdr(struct ip *iphdr);
// void			print_icmphdr(struct icmp *icmphdr);
void			print_iphdr(struct iphdr *iphdr);
void			print_icmphdr(struct icmphdr *icmphdr);
void			print_msghdr(struct msghdr *msghdr);
void			print_addrinfo(struct addrinfo *addrinfo);

void            print_usage();
void			print_stats(t_statistics *stats);
void			print_recv_host(t_statistics *stats, int recvlen, int ttl);
void			print_recv_ip(t_statistics *stats, int recvlen, int ttl);

void            SIGINT_handler();
void            free_all();
void            freexit(int exit_code);
int 			isfirsttimeupper(struct timeval time, struct timeval to_cmp);

size_t          ft_strlenbin(char *str);
int             ft_atoi(const char *str);
int             ft_strcmp(const char *s1, const char *s2);
void        	ft_bzero(void *s, size_t n);
void	        *ft_memcpy(void *dest, const void *src, size_t n);
