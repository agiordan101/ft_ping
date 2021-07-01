#include "ft_ping.h"

static char     *hostname = NULL;
static bool     pinging_loop = true;
t_statistics    stats = (t_statistics){};

t_time      get_time()
{
    t_time  time;
    int     ret;

    if ((ret = gettimeofday(&time, NULL)))
    {
        printf("Unable to get time, gettimeofday() ret: %d\n", ret);
        exit(0);
    }
    return time;
}

void    SIGINT_handler()
{
    t_time end_date = get_time();
    // printf("ctrlc s: %lu\n", (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc s: %lu\n", (unsigned long)end_date.tv_sec);
    // printf("ctrlc s diff: %lu\n", (unsigned long)end_date.tv_sec - (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc ms: %lu\n", stats.begin_date.tv_usec);
    // printf("ctrlc ms: %lu\n", end_date.tv_usec);
    // printf("ctrlc ms diff: %lu\n", end_date.tv_usec - stats.begin_date.tv_usec);

    printf("--- %s ping statistics ---\n", hostname);
    printf("%d packets transmitted, %d received, %.2f%% packet loss, time %lds\n", stats.p_sent, stats.p_received, stats.p_sent ? (100 * stats.p_received / (float)stats.p_sent) : 0., end_date.tv_sec - stats.begin_date.tv_sec);
    printf("rtt min/avg/max/mdev = %.2f/%.2f/%.2f/%.2f ms\n", stats.rtt_min, stats.rtt_n ? (stats.rtt_sum / stats.rtt_n) : 0., stats.rtt_max, stats.rtt_mdev);
    pinging_loop = false;
}

int     open_socket()
{
    int sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_ICMP);
 
    if (sockfd < 0)
    {
        printf("[ERROR] Socket can't be created, code: %d\n", sockfd);
        exit(0);
    }
    // printf("socket fd: %d\n", sockfd);
    return sockfd;
}

int     main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: ./ft_ping hostname");
        exit(0);
    }
    else
    {
        hostname = argv[1];
        stats.begin_date = get_time();
    }

    printf("FT_PING %s\n", hostname);
    
    // When CTRL+C is pressed, SIGINT_handler() is called
    signal(SIGINT, SIGINT_handler);

    // Get hostname info
    struct hostent *dns_lookup = gethostbyname(hostname);

    // while (pinging_loop)
    if (pinging_loop)
    {
        // printf("pinging ...\n");
        int sockfd = open_socket(hostname);
        printf("sockfd: %d\n", sockfd);
        buff = "ping";
        int send_ret = sendto(sockfd, buff, sizeof(buff), 0, NULL, 0);
        
    }

    printf("dns_lookup: %p\n", dns_lookup);
    // printf("h_name: %s / h_addrtype: %d / h_length: %d\n", dns_lookup->h_name, dns_lookup->h_addrtype, dns_lookup->h_length);

    return 0;
}
