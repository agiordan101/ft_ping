#include "ft_ping.h"

void			print_msghdr(struct msghdr *msghdr)
{
	char	buff[1024];
	int		rw = -1;

	printf("msghdr: %p\n", msghdr);

	// msg_name
	bzero(buff, sizeof(buff));
	memcpy(buff, msghdr->msg_name, msghdr->msg_namelen);
    printf("msghdr->msg_name (len=%d): >%s<\n", msghdr->msg_namelen, buff);
    // printf("msghdr->msg_name (len=%d): >\n", msghdr->msg_namelen);
	// rw = write(1, (char *)msghdr->msg_name, msghdr->msg_namelen);
	// printf("<\n");

	// msg_iov
    printf("msghdr->msg_iovlen: %ld\n", msghdr->msg_iovlen);
    int i = 0;
    while (i < (int)msghdr->msg_iovlen)
    {
        bzero(buff, sizeof(buff));
        memcpy(buff, msghdr->msg_iov[i].iov_base, msghdr->msg_iov[i].iov_len);
        printf(" - msghdr->msgiov[%d].iov_base (len=%d): >%s<\n", i, (int)msghdr->msg_iov[i].iov_len, buff);

		// printf(" - msghdr->msg_iov[%d].iov_base (len=%ld): >\n", i, msghdr->msg_iov[i].iov_len);
		// rw = write(1, msghdr->msg_iov[i].iov_base, msghdr->msg_iov[i].iov_len);
		// printf("<\n");

		i++;
    }

	// msg_control
	bzero(buff, sizeof(buff));
	memcpy(buff, msghdr->msg_control, msghdr->msg_controllen);
    printf("msghdr.msg_control (len=%ld): >%s<\n", msghdr->msg_controllen, buff);
    // printf("msghdr->msg_control (len=%ld): >\n", msghdr->msg_controllen);
	// rw = write(1, msghdr->msg_control, msghdr->msg_controllen);
	// printf("<\n");

	printf("msghdr->msg_flags: %d\n", msghdr->msg_flags);
    printf("\n");
	(void)rw;
	(void)buff;
}

void			print_iphdr(struct iphdr *iphdr)
{
    char			*ipv4 = malloc(INET_ADDRSTRLEN);
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
	printf("addrinfo->ai_addr: %p (len=%ld)\n", addrinfo->ai_addr, addrinfo->ai_addrlen);
	printf("addrinfo->ai_canonname: %d\n", addrinfo->ai_canonname);
	printf("addrinfo->ai_next: %p\n", addrinfo->ai_next);
    printf("\n");
}

void			print_successfull_recv(t_statistics *stats, int recvlen, int ttl)
{
	printf("%d bytes from ??? (%s): icmp_seq=%d ttl=%d time=%.2f ms\n",
		recvlen,
		gdata.ipv4,
		stats->p_sent,
		ttl,
		stats->pkt_dtime
	);
	(void)recvlen;
	(void)stats;
}

void			print_stats(t_statistics *stats)
{
    printf("\n--- %s ping statistics ---\n", gdata.hostname);
    printf("%d packets transmitted, %d received, %f%% packet loss, time %ldms\n",
        stats->p_sent,
        stats->p_received,
        stats->p_sent ? (100.0 * (stats->p_sent - stats->p_received) / (float)stats->p_sent) : 0.,
        (gdata.end_time.tv_sec - gdata.start_time.tv_sec) * 1000 + (gdata.end_time.tv_usec - gdata.start_time.tv_usec) / 1000
    );
	// printf("stats->rtt_mdiffsum: %.3f\n", stats->rtt_mdiffsum);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
        stats->rtt_min,
        stats->rtt_avg,
        stats->rtt_max,
        stats->p_received ? (stats->rtt_mdiffsum / stats->p_received) : 0.
    );
}
