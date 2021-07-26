#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include <linux/icmp.h> // Need both
#include <netinet/ip.h>
// #include <netinet/icmp.h>

# define TTL            64
#define PKTSIZE         84
#define IPHDR_SIZE       sizeof(struct iphdr)
#define ICMPHDR_SIZE     sizeof(struct icmphdr)

typedef struct      s_pkt
{
	char			*buff;
	struct iphdr    *iphdr;
	struct icmphdr  *icmphdr;
	// struct ip       *ip;
	// struct icmp     *icmp;
}				    t_pkt;

void	ft_openskt()
{
	return ;
}

void	ft_send_small(int sockfd, struct sockaddr_in destaddr)
{
    char buff[PKTSIZE] = "pingpong?";
	int                 msgsend_len;

    msgsend_len = sendto(
        sockfd,
        buff, PKTSIZE, 0,
        // (const void *)pkt, PKTSIZE, 0,
        (struct sockaddr *)&destaddr, sizeof(destaddr)
        // (void *)ai_res, sizeof(destaddr)
    );

    printf("sendto() ret: %d\n", msgsend_len);
}

void	ft_send(int sockfd, struct sockaddr_in destaddr)
{
    t_pkt       pkt;
	int                 msgsend_len;
    // struct sockaddr_in  destaddr = (struct sockaddr_in){AF_INET, htons(1024), addr, {0}}; //htons is prohibited by the subject;
	
    printf("Sizeof structs IP / ICMP: %ld / %ld\n", IPHDR_SIZE, ICMPHDR_SIZE);
    printf("Buff size: %d\n", PKTSIZE);
	
    pkt.buff = malloc(PKTSIZE);
    pkt.iphdr = (struct iphdr *)pkt.buff;
    pkt.icmphdr = (struct icmphdr *)(pkt.iphdr + IPHDR_SIZE);
	bzero((void *)pkt.buff, PKTSIZE);

	pkt.iphdr->version = 4;
	pkt.iphdr->ihl = sizeof(*pkt.iphdr) >> 2;
	pkt.iphdr->ttl = TTL;
	pkt.iphdr->protocol = IPPROTO_ICMP;
	
    pkt.icmphdr->type = ICMP_ECHO;
	pkt.icmphdr->code = 0;
	pkt.icmphdr->un.echo.id = getpid();
	pkt.icmphdr->un.echo.sequence = 1;
	// pkt.icmphdr->checksum = checksum((void *)pkt.icmphdr, sizeof(*pkt.icmphdr));

    printf("checksum: %d\n", pkt.icmphdr->checksum);

    msgsend_len = sendto(
        sockfd,
        pkt.buff, PKTSIZE, 0,
        // (const void *)pkt, PKTSIZE, 0,
        (struct sockaddr *)&destaddr, sizeof(destaddr)
        // (void *)ai_res, sizeof(destaddr)
    );

    printf("sendto() ret: %d\n", msgsend_len);
    // printf("errno send: %d\n", errno);
}

void	ft_recv(int sockfd)
{
    char            *msg_recv;
    struct iovec    msg_iov;
    struct msghdr   msghdr;
    ssize_t         msgrecv_len = -1;

    msg_recv = calloc(1, 100);
	msg_iov = (struct iovec){msg_recv, sizeof(msg_recv)};

	msghdr = (struct msghdr){
		NULL, 0,
		// "ft_ping", 7,
		&msg_iov, 1,
		NULL, 0,
		// &metadata, METADATA_LEN,
		MSG_DONTWAIT
	};

	// ret = -1 => EAGAIN => Nothing to receive
	msgrecv_len = recvmsg(sockfd, &msghdr, MSG_DONTWAIT);

	printf("msgrecv_len: %ld\n", msgrecv_len);
    // printf("errno recv: %d\n", errno);
}

int main() {
	const char* server_name = "localhost";
	const int server_port = 8877;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// creates binary representation of server name
	// and stores it as sin_addr
	// http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
	inet_pton(AF_INET, server_name, &server_address.sin_addr);

	// htons: port in network order format
	server_address.sin_port = htons(server_port);

	// open socket
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return 1;
	}
	printf("sockfd: %d\n", sock);

	// ft_send(sock, server_address);
	ft_send_small(sock, server_address);
	int len = 100;

	// // data that will be sent to the server
	// const char* data_to_send = "Gangadhar Hi Shaktimaan hai";
	// // send data
	// int len =
	//     sendto(sock, data_to_send, strlen(data_to_send), 0,
	//            (struct sockaddr*)&server_address, sizeof(server_address));
	// printf("send len: %d\n", len);

	// ft_recv(sock);

	// received echoed data back
	char buffer[100];
	recvfrom(sock, buffer, len, 0, NULL, NULL);
	buffer[len] = '\0';
	printf("recieved: '%s'\n", buffer);

	// close the socket
	close(sock);
	return 0;
}