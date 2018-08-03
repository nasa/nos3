#ifndef _spw_sbn_if_struct_h_
#define _spw_sbn_if_struct_h_

/* Since the target implementation of the SpaceWire driver only operates point-to-point, this struct is a bit sparse
** The only necessary data is the SpaceWire device class and device name
*/
typedef struct {
    char DevClass[SBN_SPW_MAX_CHAR_NAME]	/* e.g. 'spw' from /sys/class */
	char DevInstance[SBN_SPW_MAX_CHAR_NAME]		/* e.g. 'spw0' from /dev */
} SPW_SBNEntry_t;

typedef struct {
	SPW_SBNEntry_t *spwEntry;
} SPW_SBNHostData_t;

typedef struct {
	SPW_SBNEntry_t *spwEntry;
} SPW_SBNPeerData_t;

#endif /* _spw_sbn_if_struct_h_ */
