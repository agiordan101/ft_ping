struct iovec {                    /* Scatter/gather array items */
    void  *iov_base;              /* Starting address */
    size_t iov_len;               /* Number of bytes to transfer */
};

struct msghdr {
    void         *msg_name;       /* optional address */
    socklen_t     msg_namelen;    /* size of address */
    struct iovec *msg_iov;        /* scatter/gather array */
    size_t        msg_iovlen;     /* # elements in msg_iov */
    void         *msg_control;    /* ancillary data, see below */
    size_t        msg_controllen; /* ancillary data buffer len */
    int           msg_flags;      /* flags on received message */
};

struct sockaddr_in {
    sa_family_t    sin_family; /* famille d'adresses : AF_INET     */
    uint16_t       sin_port;   /* port dans l'ordre d'octets réseau */
    struct in_addr sin_addr;   /* adresse Internet                  */
};

/* Adresse Internet */
struct in_addr {
    uint32_t       s_addr;     /* Adresse dans l'ordre d'octets réseau */
};


struct icmphdr
{
  u_int8_t type;                /* message type */
  u_int8_t code;                /* type sub-code */
  u_int16_t checksum;
  union
  {
    struct
    {
      u_int16_t        id;
      u_int16_t        sequence;
    } echo;                        /* echo datagram */
    u_int32_t        gateway;        /* gateway address */
    struct
    {
      u_int16_t        __unused;
      u_int16_t        mtu;
    } frag;                        /* path mtu discovery */
  } un;
};

uint8_t 	verhdrlen   // version ?
uint8_t 	service     // ?
uint16_t 	len         // tot_len ?
uint16_t 	ident       // ?
uint16_t 	frags       // ?
uint8_t 	ttl
uint8_t 	protocol
uint16_t 	chksum
struct in_addr 	src
struct in_addr 	dest
