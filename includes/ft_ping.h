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

#define PKTSIZE         84
#define PINGMSG         "pingpong?"
#define PINGMSG_LEN     (sizeof(PINGMSG))

# define TTL            64

// #define IPSIZE          sizeof(struct ip)
// #define ICMPSIZE        sizeof(struct icmp)
#define IPHDRSIZE       sizeof(struct iphdr)
#define ICMPHDRSIZE     sizeof(struct icmphdr)

#define MSGRECV_LEN     128
#define METADATA_LEN    128
#define SKTOPT_LVL      SOL_SOCKET
// #define SKTOPT_LVL         SOL_RAW

typedef struct      s_pkt
{
	char			*buff;
	struct iphdr    *iphdr;
	struct icmphdr  *icmphdr;
	// struct ip       *ip;
	// struct icmp     *icmp;
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
void    send_pkt(int sktfd, struct in_addr addr, t_pkt *pkt);
void    recv_pkt(int sktfd, t_pkt *pkt);

struct timeval  get_time();

/*
Implementation
The steps followed by a simple ping program are:

Take a hostname as input
Do a DNS lookup
DNS lookup can be done using gethostbyname(). The gethostbyname() function converts a normal human readable website and returns a structure of type hostent which contains IP address in form of binary dot notation and also address type.

Some ping programs like the one given with ubuntu support reverse DNS lookup.
Reverse DNS lookup is performed using getnameinfo(), and it converts dot notation IP address to hostname.

for example, the pinging of google.com frequently gives a strange address:
bom07s18-in-f14.1e100.net

This is as a result of a reverse DNS lookup.

Open a Raw socket using SOCK_RAW with protocol as IPPROTO_ICMP.
Note: raw socket requires superuser rights so you have to run this code using sudo
When crtl + C is pressed, ping gives a report. This interrupt is caught by an interrupt handler
which just sets our pinging looping condition to false.
Here comes the main ping sending loop.
We have to:
Set the ttl option to a value in the socket
TTL value is set to limit the number of hops a packet can make.
Set the timeout of the recv function
If timeout is not set, recv will wait forever, halting the loop.
Fill up the icmp packet
As follows:
Set packet header type to ICMP_ECHO.
Set id to pid of process
Fill msg part randomly.
Calculate checksum and fill it in checksum field.
Send the packet
Wait for it to be received.
The main problem here is that the packet received does not mean that that the destination is working.
Echo reply means destination is OK. Echo reply is sent from destination OS kernel.
*/