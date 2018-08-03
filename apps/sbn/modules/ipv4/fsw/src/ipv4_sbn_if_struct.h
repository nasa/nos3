#ifndef _ip_sbn_if_struct_h_
#define _ip_sbn_if_struct_h_

#define IPV4_ITEMS_PER_FILE_LINE 2

typedef struct
{
    char Addr[16];
    int  Port;
} IPv4_SBNEntry_t;

typedef struct
{
    char            Addr[16];
    int                Port;
    int                SockId;
} IPv4_SBNHostData_t;

typedef struct
{
    char            Addr[16];
    int             Port;
    int             SockId;
} IPv4_SBNPeerData_t;

#endif /* _ipv4_sbn_if_struct_h_ */
