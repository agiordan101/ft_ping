#include "ft_ping.h"

/*

    Utilisation autorisé d'une globale seulement / static?
    Utiliser extern 
    FQDN ??? resolution dns ?
    Quelqu'il se passe si le packet non pas recu ? packet perdu ? Ou réel probleme ?
    -v -h
    resolution inver DNS
*/

t_gdata  gdata;

void            SIGINT_handler()
{
    // printf("ctrlc s: %lu\n", (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc s: %lu\n", (unsigned long)gdata.endtime.tv_sec);
    // printf("ctrlc s diff: %lu\n", (unsigned long)gdata.endtime.tv_sec - (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc ms: %lu\n", stats.begin_date.tv_usec);
    // printf("ctrlc ms: %lu\n", gdata.endtime.tv_usec);
    // printf("ctrlc ms diff: %lu\n", gdata.endtime.tv_usec - stats.begin_date.tv_usec);

    gdata.end_time = get_time();
    print_stats(&gdata.stats);
    gdata.pinging_loop = false;
    exit(0); // Not the right behavior
}

// int             pinging(struct in_addr addr, const char *ipv4, struct sockaddr_in *ai_res)
int             pinging(struct in_addr addr)
{
    struct sockaddr_in  destaddr = (struct sockaddr_in){
        AF_INET, 42, addr, {0}
    };
    t_pkt               pkt;
    int                 sktfd;
    int                 ret = -1;
    int                 p_seq = 0;

    sktfd = create_skt();
    init_pkt(&pkt, &destaddr);

    while (gdata.pinging_loop)
    // if (gdata.pinging_loop)
    // for (int i = 0; i < 10; i++)
    {
        // printf("\nPing ...\n");

        fill_pkt(&pkt, p_seq);
        send_pkt(sktfd, &pkt);
        // while (ret == -1)
        ret = recv_pkt(sktfd, &gdata.stats, p_seq);
        (void)ret;

        // printf("destaddr.sin_addr.s_addr: %d\n", destaddr.sin_addr.s_addr);
        // printf("destaddr.sin_port: %d\n", destaddr.sin_port);
        // printf("destaddr.sin_family: %d\n", destaddr.sin_family);

        // printf("End ping.\n");
        p_seq++;
    }
    close(sktfd);
    return 0;
}

void            parsing(char **argv, int argc)
{
    if (argc < 2)
    {
        printf("usage: ./ft_ping hostname");
        exit(0);
    }
    else
    {
        gdata.hostname = argv[1];
        gdata.start_time = get_time();
        gdata.pinging_loop = true;
        gdata.pid = getpid();
    }

}

struct in_addr  get_addr()
{
    struct in_addr  addr;

    struct hostent  *dns_lookup = gethostbyname(gdata.hostname);

    printf("addr(len): %s(%d)\n", dns_lookup->h_addr, dns_lookup->h_length);

    // Copy the binary address into struct in_addr
    memcpy(&addr, dns_lookup->h_addr, dns_lookup->h_length);

    // printf("Binary addr: %s\n", dns_lookup->h_addr);
    // printf("addr: %s\n", addr.s_addr);

    // Transform struct in_addr (Binary adress) to IP address text --- addr to ipv4
    inet_ntop(AF_INET, &addr, gdata.ipv4, INET_ADDRSTRLEN);

    // inet_ntop(AF_INET, &dns_lookup->h_addr, ipv4, INET_ADDRSTRLEN);

    printf("PING %s (%s) %d(%d) bytes of data.\n", gdata.hostname, gdata.ipv4, 56, 84);
    return addr;
}

struct in_addr  new_get_addr()
{
    struct addrinfo hints = {0, AF_INET, SOCK_RAW, IPPROTO_ICMP};
    struct addrinfo *res = NULL;
    struct in_addr  addr = {};

    int errcode = getaddrinfo(gdata.hostname, NULL, NULL, &res);
    // errcode = getaddrinfo(gdata.hostname, NULL, hints, res);
    printf("errcode: %d\n", errcode);

    while (res)
    {
        print_addrinfo(res);
        res = res->ai_next;
    }
    return addr;
}

int             main(int argc, char **argv)
{
    // printf("Sizeof IPHDR / ICMPHDR / PAYLOAD: %ld / %ld / %ld\n", IPHDR_SIZE, ICMPHDR_SIZE, PAYLOAD_SIZE);

    parsing(argv, argc);

    // When CTRL+C is pressed, SIGINT_handler() is called
    signal(SIGINT, SIGINT_handler);

    // struct in_addr addr = get_addr();
    struct in_addr addr = new_get_addr();

    exit(0);
    pinging(addr); //At this state, need only addr et ivp4

    return 0;
}
    // printf("h_name: %s / addrs: %p / h_addrtype: %d=?=%d / h_length: %d\n", dns_lookup->h_name, dns_lookup->h_addr_list, dns_lookup->h_addrtype, AF_INET, dns_lookup->h_length);
    // printf("IP serveur ntop: >%s<\n\n", ipv4);
