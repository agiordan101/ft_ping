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
