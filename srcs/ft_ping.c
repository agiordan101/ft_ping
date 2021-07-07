#include "ft_ping.h"

static char     *hostname = NULL;
static bool     pinging_loop = true;
t_statistics    stats = (t_statistics){};

struct timeval      get_time()
{
    struct timeval  time;
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
    struct timeval end_date = get_time();
    // printf("ctrlc s: %lu\n", (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc s: %lu\n", (unsigned long)end_date.tv_sec);
    // printf("ctrlc s diff: %lu\n", (unsigned long)end_date.tv_sec - (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc ms: %lu\n", stats.begin_date.tv_usec);
    // printf("ctrlc ms: %lu\n", end_date.tv_usec);
    // printf("ctrlc ms diff: %lu\n", end_date.tv_usec - stats.begin_date.tv_usec);

    printf("--- %s ping statistics ---\n", hostname);
    printf("%d packets transmitted, %d received, %.3f%% packet loss, time %lds\n", stats.p_sent, stats.p_received, stats.p_sent ? (100 * stats.p_received / (float)stats.p_sent) : 0., end_date.tv_sec - stats.begin_date.tv_sec);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", stats.rtt_min, stats.rtt_n ? (stats.rtt_sum / stats.rtt_n) : 0., stats.rtt_max, stats.rtt_mdev);
    pinging_loop = false;
}

int     open_socket()
{
    int sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_ICMP);
    // int sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

    if (sockfd < 0)
    {
        printf("[ERROR] Socket can't be created: \n");
        perror(NULL);
        exit(0);
    }
    // printf("socket fd: %d\n", sockfd);
    return sockfd;
}

int     pinging(struct in_addr addr, const char *ipv4)
{
    int                 sockfd;
    char                *buff;
    int                 send_ret;
    struct sockaddr_in  sockaddr;
    int                 ret;

    // while (pinging_loop)
    if (pinging_loop)
    {
        printf("pinging ...\n");

        sockfd = open_socket(hostname);
        printf("sockfd: %d\n\n", sockfd);

        // Set max hops socket can makes
        int ttl_hops = 100;
        setsockopt(sockfd, SOL_SOCKET, IP_TTL, &ttl_hops, sizeof(ttl_hops));

        // Set receive timeout
        struct timeval rcv_timeout = {10, 0};
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout));

        // verifier le retour des setsockopt
        // Choisir entre IPPROTO_ICMP et IPPROTO_RAW

        sockaddr = (struct sockaddr_in){AF_INET, 42, addr, {0}}; //htons is prohibited

        // sockaddr = (struct sockaddr_in){AF_INET, 42, (struct in_addr){0}, {0x0}}; //htons is prohibited
        // if ((ret = inet_pton(AF_INET, ipv4, &sockaddr.sin_addr)) == -1) // Convert IPv4 text address to struct in_addr
        //     perror(NULL), exit(0);

        // printf("inet_pton return: %d\n", ret);
        // printf("sockaddr: %d / %d / %d / >%s<\n\n", sockaddr.sin_family, sockaddr.sin_port, sockaddr.sin_addr.s_addr, sockaddr.sin_zero);
        // exit(0);

        buff = "ping?";
        send_ret = sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

        printf("send_ret: %d\n", send_ret);
        if (send_ret == -1)
            perror(NULL), exit(0);

        close(sockfd);
    }
    (void)ipv4;
    (void)ret;
    return 0;
}

int     main(int argc, char **argv)
{
    // char    *ret;
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
    struct hostent  *dns_lookup = gethostbyname(hostname);
    
    // Make in_addr struct
    struct in_addr  addr;
    memcpy(&addr.s_addr, dns_lookup->h_addr, dns_lookup->h_length); // Copy the binary address in struct in_addr

    // Make IPv4 string
    char    *ipv4 = malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &addr, ipv4, INET_ADDRSTRLEN);               // Transform struct in_addr address to text

    printf("h_name: %s / addrs: %p / h_addrtype: %d=?=%d / h_length: %d\n", dns_lookup->h_name, dns_lookup->h_addr_list, dns_lookup->h_addrtype, AF_INET, dns_lookup->h_length);
    printf("IP serveur ntop: >%s<\n\n", ipv4);

    pinging(addr, ipv4);

    return 0;
}
