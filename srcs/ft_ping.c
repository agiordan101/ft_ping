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

void            free_all()
{
    // printf("gdata.res: %p\n", gdata.res);
    if (gdata.res)
        freeaddrinfo(gdata.res);
    // printf("gdata.pkt: %p\n", gdata.pkt);
    // printf("gdata.pkt->buff: %p\n", gdata.pkt->buff);
    // if (gdata.pkt && gdata.pkt->buff)
    //     free((char *)gdata.pkt->buff);
}

void            freexit(int exit_code)
{
    free_all();
    exit(exit_code);
}

void            SIGINT_handler()
{
    // printf("ctrlc s: %lu\n", (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc s: %lu\n", (unsigned long)gdata.endtime.tv_sec);
    // printf("ctrlc s diff: %lu\n", (unsigned long)gdata.endtime.tv_sec - (unsigned long)stats.begin_date.tv_sec);
    // printf("ctrlc ms: %lu\n", stats.begin_date.tv_usec);
    // printf("ctrlc ms: %lu\n", gdata.endtime.tv_usec);
    // printf("ctrlc ms diff: %lu\n", gdata.endtime.tv_usec - stats.begin_date.tv_usec);

    gdata.pinging_loop = false;
}

// int             pinging(struct in_addr addr, const char *ipv4, struct sockaddr_in *ai_res)
int             pinging(struct in_addr addr)
{
    t_pkt               pkt;
    int                 sktfd;
    int                 p_seq = 0;
    struct timeval      nextpktsend_time;
    struct sockaddr_in  destaddr = (struct sockaddr_in){
        AF_INET, 42, addr, {0}
    };

    // printf("Send time: %ld k %ld\n", lastpktsent_time->tv_sec, lastpktsent_time->tv_usec);
    gdata.pkt = &pkt;
    sktfd = create_skt();
    init_pkt(&pkt, &destaddr);

    if (gdata.verbose)
        printf("Verbose mode on.\n");
    printf("PING %s (%s) %d(%d) bytes of data.\n", gdata.hostname, gdata.ipv4, 56, 84);

    gdata.start_time = get_time();
    nextpktsend_time = gdata.start_time;
    while (gdata.pinging_loop)
    {
        // printf("gdata.floodping: %d\n", gdata.floodping);
        if (gdata.floodping == true || isfirsttimeupper(get_time(), nextpktsend_time))
        {
            fill_pkt(&pkt, p_seq);
            send_pkt(sktfd, &pkt);
            recv_pkt(sktfd, &gdata.stats, p_seq);

            p_seq++;
            if (gdata.maxreplies != -1 && p_seq == gdata.maxreplies)
                break ;

            nextpktsend_time.tv_usec += 1000 * (gdata.dtime_pktsend % 1000);
            nextpktsend_time.tv_sec += gdata.dtime_pktsend / 1000 + (nextpktsend_time.tv_usec >= 1000000 ? 1 : 0);
            if (nextpktsend_time.tv_usec >= 1000000)
                nextpktsend_time.tv_usec -= 1000000;
        }
    }
    gdata.end_time = get_time();
    close(sktfd);
    return 0;
}

void            handle_options(char *opt, char *nextopt)
{
    if (!strcmp(opt, "-h"))
        print_usage();
    else if (!strcmp(opt, "-v"))
        gdata.verbose = true;
    else if (!strcmp(opt, "-f"))
        gdata.floodping = true;
    else if (!strcmp(opt, "-c") && nextopt)
    {
        gdata.maxreplies = atoi(nextopt);
        if (gdata.maxreplies <= 0)
            printf("Argument for option -c need to be an integer greater than 0, not '%s'\n", nextopt), freexit(EXIT_FAILURE);
    }
    else if (!strcmp(opt, "-i") && nextopt)
    {
        gdata.dtime_pktsend = atoi(nextopt);
        if (gdata.dtime_pktsend <= 0)
            printf("Argument for option -i need to be an integer greater than 0, not '%s'\n", nextopt), freexit(EXIT_FAILURE);
    }
    else if (!strcmp(opt, "-t") && nextopt)
    {
        gdata.ttl = atoi(nextopt);
        if (gdata.ttl <= 0)
            printf("Argument for option -t need to be an integer greater than 0, not '%s'\n", nextopt), freexit(EXIT_FAILURE);
    }
    else if (!strcmp(opt, "-W") && nextopt)
    {
        gdata.recv_timeout = atoi(nextopt);
        if (gdata.recv_timeout <= 0)
            printf("Argument for option -W need to be an integer greater than 0, not '%s'\n", nextopt), freexit(EXIT_FAILURE);
    }
}

void            init()
{
    gdata.pid = getpid();
    gdata.pinging_loop = true;
    gdata.dtime_pktsend = 1000;
    gdata.verbose = false;
    gdata.floodping = false;
    gdata.maxreplies = -1;
    gdata.ttl = TTL;
    gdata.recv_timeout = RECVTIMEOUTMS;
}

void            parsing(char **argv, int argc)
{
    if (argc < 2)
        print_usage();
    int i = 1;
    while (i < argc)
    {
        // printf("argv %d: %s\n", i, argv[i]);
        if (argv[i][0] == '-')
            handle_options(argv[i], i + 1 == argc ? NULL : argv[i + 1]);
        else
            gdata.hostname = argv[i];
        i++;
    }
    // printf("gdata.hostname: %s\n", gdata.hostname);
}

void      get_addr(struct in_addr *addr)
{
    int             errcode;
    struct addrinfo *res;
    struct addrinfo hints = {
        AI_CANONNAME, AF_INET, SOCK_RAW, IPPROTO_ICMP,
        0, NULL, NULL, NULL
    };

    if ((errcode = getaddrinfo(gdata.hostname, NULL, &hints, &gdata.res)))
        printf("[ERROR] Unable to find an internet address because getaddrinfo() failed. errcode: %d\n", errcode), freexit(EXIT_FAILURE);

    res = gdata.res;
    while (res)
    {
        if (res->ai_socktype == SOCK_RAW &&
            res->ai_family == AF_INET)
        {
            // printf("Find right struct\n");
            struct sockaddr_in *sockaddr_in = (struct sockaddr_in *)res->ai_addr;
            printf("sockaddr_in->sin_family: %d\n", sockaddr_in->sin_family);
            printf("sockaddr_in->sin_port: %d\n", sockaddr_in->sin_port);
            print_addrinfo(res);
            if (sockaddr_in)
            {
                memcpy(addr, &sockaddr_in->sin_addr, sizeof(sockaddr_in->sin_addr));
                return ;
            }
        }
        res = res->ai_next;
    }
    printf("[ERROR] Unable to find an internet address with AF_INET as family and SOCK_RAW as socktype.\n");
    freexit(EXIT_FAILURE);
}

int             main(int argc, char **argv)
{
    struct in_addr addr;
    // printf("Sizeof IPHDR / ICMPHDR / PAYLOAD: %ld / %ld / %ld\n", IPHDR_SIZE, ICMPHDR_SIZE, PAYLOAD_SIZE);

    init();
    parsing(argv, argc);

    // When CTRL+C is pressed, SIGINT_handler() is called
    signal(SIGINT, SIGINT_handler);

    // struct in_addr addr = get_addr();
    get_addr(&addr);

    // Transform struct in_addr (Binary adress) to IP address text --- addr to ipv4
    inet_ntop(AF_INET, &addr, gdata.ipv4, INET_ADDRSTRLEN);

    pinging(addr);
    print_stats(&gdata.stats);
    free_all();
    return 0;
}
