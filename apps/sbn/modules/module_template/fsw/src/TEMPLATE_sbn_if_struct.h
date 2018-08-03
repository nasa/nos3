#ifndef _TEMPLATE_sbn_if_struct_h_
#define _TEMPLATE_sbn_if_struct_h_

typedef struct {
    /* TODO add values here that will be parsed from the SbnPeerData file */
} TEMPLATE_SBNEntry_t;

typedef struct {
    /* TODO add values here needed to use the host's data send, 
        data receive, and protocol send ports */
} TEMPLATE_SBNHostData_t;

typedef struct {
    /* TODO add values here needed to use the peer's protocol send (aka
        the host's protocol receive) port */
} TEMPLATE_SBNPeerData_t;

#endif /* _TEMPLATE_sbn_if_struct_h_ */
