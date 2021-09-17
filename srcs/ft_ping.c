#include "ft_ping.h"

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
    gdata.reversed_hostname[0] = '?';
    gdata.print_recv = print_recv_host;
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
        if (argv[i][0] == '-')
            handle_options(argv[i], i + 1 == argc ? NULL : argv[i + 1]);
        else
            gdata.hostname = argv[i];
        i++;
    }
}

void	    get_reversed_hostname()
{
    int ret;
	if ((ret = getnameinfo(gdata.ai_addr, gdata.ai_addrlen,
		gdata.reversed_hostname, sizeof(gdata.reversed_hostname), NULL, 0, NI_NAMEREQD)) < 0)
        perror(NULL), printf("[ERROR] Unable to fetch reversed hostname (%s) of ipv4 addr (error code %d / %d): %s\n", gdata.reversed_hostname, ret, EAI_FAMILY, gdata.ipv4), freexit(EXIT_FAILURE);
    // printf("Reversed hostname: %s\n", gdata.reversed_hostname);
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
        printf("ft_ping: %s: Name or service not known\n", gdata.hostname), freexit(EXIT_FAILURE);

    res = gdata.res;
    while (res)
    {
        if (res->ai_socktype == SOCK_RAW &&
            res->ai_family == AF_INET)
        {
            // printf("Find right struct\n");
            gdata.ai_addr = res->ai_addr;
            gdata.ai_addrlen = res->ai_addrlen;
            struct sockaddr_in *sockaddr_in = (struct sockaddr_in *)res->ai_addr;
            // printf("sockaddr_in->sin_family: %d\n", sockaddr_in->sin_family);
            // printf("sockaddr_in->sin_port: %d\n", sockaddr_in->sin_port);
            // print_addrinfo(res);
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

    if (getuid() != 0) // root is always 0
    {
        printf("ft_ping needs to have root access.\n");
        return 1;
    }

    init();
    parsing(argv, argc);
    if (gdata.verbose)
        printf("Verbose mode on.\n");

    // When CTRL+C is pressed, SIGINT_handler() is called
    signal(SIGINT, SIGINT_handler);

    get_addr(&addr);

    // Transform struct in_addr (Binary adress) to IP address text --- addr to ipv4
    inet_ntop(AF_INET, &addr, gdata.ipv4, INET_ADDRSTRLEN);

    // Switch between hotsname and ipv4 input
    if (strcmp(gdata.hostname, gdata.ipv4))
        get_reversed_hostname();
    else
        gdata.print_recv = print_recv_ip;

    pinging(addr);

    print_stats(&gdata.stats);
    free_all();
    return 0;
}
