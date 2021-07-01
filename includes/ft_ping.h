#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <signal.h>
#include <netdb.h>
#include <sys/time.h>

// struct hostent {
//     char  *h_name;            /* official name of host */
//     char **h_aliases;         /* alias list */
//     int    h_addrtype;        /* host address type */
//     int    h_length;          /* length of address */
//     char **h_addr_list;       /* list of addresses */
// }

typedef struct  timeval t_time;

typedef struct  s_statistics {
    t_time      begin_date;
    int         p_sent;
    int         p_received;
    int         p_lost;
    int         rtt_n;
    float       rtt_min;
    float       rtt_max;
    float       rtt_sum;
    float       rtt_mdev;
}               t_statistics;