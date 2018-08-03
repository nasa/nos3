/************************************************************************
** File:
**
** Purpose: 
**  Define TO Lab Event messages
**
** Notes:
**
** $Log: to_lab_events.h  $
** Revision 1.1 2008/04/30 13:45:11GMT-05:00 rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/src/project.pj
**
*************************************************************************/
#ifndef _to_lab_events_h_
#define _to_lab_events_h_

/*****************************************************************************/

/* Event message ID's */
#define TO_EVM_RESERVED            0

#define TO_INIT_INF_EID            1   
#define TO_CRCMDPIPE_ERR_EID       2
#define TO_TLMOUTENA_INF_EID       3 
#define TO_SUBSCRIBE_ERR_EID       4
#define TO_TLMPIPE_ERR_EID         5
#define TO_TLMOUTSOCKET_ERR_EID    6
#define TO_TLMOUTSTOP_ERR_EID      7
#define TO_MSGID_ERR_EID           8
#define TO_FNCODE_ERR_EID          9
#define TO_ADDPKT_ERR_EID         10
#define TO_REMOVEPKT_ERR_EID      11
#define TO_REMOVEALLPTKS_ERR_EID  12
#define TO_REMOVECMDTO_ERR_EID    13
#define TO_REMOVEHKTO_ERR_EID     14
#define TO_ADDPKT_INF_EID         15
#define TO_REMOVEPKT_INF_EID      16
#define TO_REMOVEALLPKTS_INF_EID  17
#define TO_NOOP_INF_EID           18

/******************************************************************************/

#endif  /* _to_lab_events_h_ */

