#include "ft_ping.h"

/*

    Utilisation autorisé d'une globale seulement / static?
    Utiliser extern 
    FQDN ???

*/
static char     *hostname = NULL;
static bool     pinging_loop = true;
t_statistics    stats = (t_statistics){};

void            SIGINT_handler()
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
    exit(0); // Not the right behavior
}

int             pinging(struct in_addr addr, const char *ipv4)
// int             pinging(struct in_addr addr, const char *ipv4, struct sockaddr_in *ai_res)
{
    t_pkt       pkt;
    int         sktfd;
    int         ret = -1;

    pkt.buff = malloc(PKTSIZE);
    pkt.iphdr = (struct iphdr *)pkt.buff;
    pkt.icmphdr = (struct icmphdr *)(pkt.iphdr + IPHDR_SIZE);
    pkt.payload = (char *)(pkt.icmphdr + ICMPHDR_SIZE);

    // printf("%p\t%p\t%p\n", pkt.buff, pkt.ip, pkt.icmp);

    printf("Sizeof IPHDR / ICMPHDR / PAYLOAD: %ld / %ld / %ld\n", IPHDR_SIZE, ICMPHDR_SIZE, PAYLOAD_SIZE);
    printf("Sizeof MSGHDR: %ld\n", sizeof(struct msghdr));
    // t_pkt pkt;
    // printf("iphdr size: %ld\n", sizeof(pkt.iphdr));
    // printf("ip size: %ld\n", sizeof(pkt.ip));
    // printf("icmphdr size: %ld\n", sizeof(pkt.icmphdr));
    // printf("icmp size: %ld\n", sizeof(pkt.icmp));
    // exit(0);

    sktfd = create_skt(hostname);
    // while (pinging_loop)
    if (pinging_loop)
    {
        printf("\nPing ...\n");
 
        send_pkt(sktfd, addr, &pkt, inet_addr(ipv4));
        while (ret == -1)
            ret = recv_pkt(sktfd);

        printf("End ping.\n");
    }
    close(sktfd);
    (void)ipv4;
    return 0;
}

// void            init()

int             main(int argc, char **argv)
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

    // Copy the binary address into struct in_addr
    struct in_addr  addr;
    memcpy(&addr, dns_lookup->h_addr, dns_lookup->h_length);

    //-------------------------
    // struct sockaddr_in *ai_res = get_addrinfo(hostname);
    //-------------------------



    // Make IPv4 string
    char    *ipv4 = malloc(INET_ADDRSTRLEN);

    // Transform struct in_addr to IP address text --- addr to ipv4
    inet_ntop(AF_INET, &addr, ipv4, INET_ADDRSTRLEN);

    printf("h_name: %s / addrs: %p / h_addrtype: %d=?=%d / h_length: %d\n", dns_lookup->h_name, dns_lookup->h_addr_list, dns_lookup->h_addrtype, AF_INET, dns_lookup->h_length);
    printf("IP serveur ntop: >%s<\n\n", ipv4);

    pinging(addr, ipv4);
    // pinging(addr, ipv4, ai_res);

    return 0;
}
