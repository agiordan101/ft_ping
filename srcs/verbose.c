#include "ft_ping.h"

/*
	USELESS FUNCTIONS - HELP TO DEBUG
*/

void			print_msghdr(struct msghdr *msghdr)
{
	char	buff[1024];

	printf("msghdr: %p\n", msghdr);

	// msg_name
	bzero(buff, sizeof(buff));
	ft_memcpy(buff, msghdr->msg_name, msghdr->msg_namelen);
    printf("msghdr->msg_name (len=%d): >%s<\n", msghdr->msg_namelen, buff);

	// msg_iov
    printf("msghdr->msg_iovlen: %ld\n", msghdr->msg_iovlen);
    int i = -1;
    while (++i < (int)msghdr->msg_iovlen)
    {
        bzero(buff, sizeof(buff));
        ft_memcpy(buff, msghdr->msg_iov[i].iov_base, msghdr->msg_iov[i].iov_len);
        printf(" - msghdr->msgiov[%d].iov_base (len=%d): >%s<\n", i, (int)msghdr->msg_iov[i].iov_len, buff);
    }

	// msg_control
	bzero(buff, sizeof(buff));
	ft_memcpy(buff, msghdr->msg_control, msghdr->msg_controllen);
    printf("msghdr.msg_control (len=%ld): >%s<\n", msghdr->msg_controllen, buff);
	printf("msghdr->msg_flags: %d\n", msghdr->msg_flags);
    printf("\n");
}

void			print_iphdr(struct iphdr *iphdr)
{
    char			ipv4[INET_ADDRSTRLEN];
	struct in_addr	addr;

	printf("iphdr: %p\n", iphdr);
	printf("iphdr->version: %d\n", iphdr->version);
	printf("iphdr->ihl: %d\n", iphdr->ihl);
	printf("iphdr->tos: %d\n", iphdr->tos);
	printf("iphdr->tot_len: %d\n", iphdr->tot_len);
	printf("iphdr->id: %d\n", iphdr->id);
	printf("iphdr->frag_off: %d\n", iphdr->frag_off);
	printf("iphdr->ttl: %d\n", iphdr->ttl);
	printf("iphdr->protocol: %d\n", iphdr->protocol);

	addr = (struct in_addr){iphdr->saddr};
	inet_ntop(AF_INET, &addr, ipv4, INET_ADDRSTRLEN);
	printf("iphdr->saddr: %s\n", ipv4);

	addr = (struct in_addr){iphdr->daddr};
	inet_ntop(AF_INET, &addr, ipv4, INET_ADDRSTRLEN);
	printf("iphdr->daddr: %s\n", ipv4);
    printf("\n");
}

void			print_icmphdr(struct icmphdr *icmphdr)
{
	printf("icmphdr: %p\n", icmphdr);
	printf("icmphdr->type: %d\n", icmphdr->type);
    printf("icmphdr->code: %d\n", icmphdr->code);
    printf("icmphdr->checksum: %d\n", icmphdr->checksum);
    printf("icmphdr->un.echo.sequence: %d\n", icmphdr->un.echo.sequence);
    printf("icmphdr->un.echo.id: %d\n", icmphdr->un.echo.id);
    printf("icmphdr->un.gateway: %d\n", icmphdr->un.gateway);
    printf("\n");
}

void			print_addrinfo(struct addrinfo *addrinfo)
{
	printf("addrinfo: %p\n", addrinfo);
	printf("addrinfo->ai_flags: %d\n", addrinfo->ai_flags);
	printf("addrinfo->ai_family: %d\n", addrinfo->ai_family);
	printf("addrinfo->ai_socktype: %d\n", addrinfo->ai_socktype);
	printf("addrinfo->ai_protocol: %d\n", addrinfo->ai_protocol);
	printf("addrinfo->ai_addr: %p (len=%d)\n", addrinfo->ai_addr, addrinfo->ai_addrlen);
	printf("addrinfo->ai_canonname: %s\n", addrinfo->ai_canonname);
	printf("addrinfo->ai_next: %p\n", addrinfo->ai_next);
    printf("\n");
}

/*
	FUNCTIONS USE IN THIS PROJECT
*/

void			print_recv_ip(t_statistics *stats, int recvlen, int ttl)
{
	printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
		recvlen - IPHDR_SIZE,
		gdata.ipv4,
		stats->p_sent,
		ttl,
		stats->pkt_dtime
	);
}

void			print_recv_host(t_statistics *stats, int recvlen, int ttl)
{
	printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
		recvlen - IPHDR_SIZE,
		gdata.reversed_hostname,
		gdata.ipv4,
		stats->p_sent,
		ttl,
		stats->pkt_dtime
	);
}

void			print_stats(t_statistics *stats)
{
    printf("\n--- %s ping statistics ---\n", gdata.hostname);
    printf("%d packets transmitted, %d received, %g%% packet loss, time %ldms\n",
        stats->p_sent,
        stats->p_received,
        stats->p_sent ? (100.0 * (stats->p_sent - stats->p_received) / (float)stats->p_sent) : 0,
        (gdata.end_time.tv_sec - gdata.start_time.tv_sec) * 1000 + (gdata.end_time.tv_usec - gdata.start_time.tv_usec) / 1000
    );
	// printf("stats->rtt_mdiffsum: %.3f\n", stats->rtt_mdiffsum);
    if (stats->p_received)
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats->rtt_min,
			stats->rtt_avg,
			stats->rtt_max,
			stats->p_received ? (stats->rtt_mdiffsum / stats->p_received) : 0.
		);
	else
		printf("\n");
}

void			print_usage()
{
	printf("\nUsage\n  ft_ping [options] <destination>\n");
	printf("\nOptions:\n  <destination>\t\tdns name or ip address\n");
	printf("  -h\t\t\tprint help and exit\n");
	printf("  -v\t\t\tverbose output\n");
	printf("  -f\t\t\tflood ping\n");
	printf("  -c <count>\t\tstop after <count> replies\n");
	printf("  -i <time>\t\twait <time> milliseconds between each packet\n");
	printf("  -t <ttl>\t\tdefine time to live\n");
	printf("  -W <timeout>\t\ttime to wait for response\n");
	freexit(EXIT_SUCCESS);
}
