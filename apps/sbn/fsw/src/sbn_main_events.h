/************************************************************************
**   sbn_events.h
**   
**   2014/11/21 ejtimmon
**
**   Specification for the Software Bus Network event identifers.
**
*************************************************************************/

#ifndef _sbn_main_events_h_
#define _sbn_main_events_h_

#include "sbn_events.h"

#define SBN_INIT_EID FIRST_SBN_EID + 1 /* yes we skip the 0th */
#define SBN_APP_EXIT_EID FIRST_SBN_EID + 2
#define SBN_CR_CMD_PIPE_EID FIRST_SBN_EID + 3
#define SBN_SUB_REQ_EID FIRST_SBN_EID + 4
#define SBN_SUB_CMD_EID FIRST_SBN_EID + 5
#define SBN_CC_EID FIRST_SBN_EID + 6
#define SBN_MID_EID FIRST_SBN_EID + 7
#define SBN_HKREQ_LEN_EID FIRST_SBN_EID + 8
#define SBN_LEN_EID FIRST_SBN_EID + 9
#define SBN_NOOP_INF_EID FIRST_SBN_EID + 10
#define SBN_RESET_EID FIRST_SBN_EID + 11

#define SBN_HK_EID             FIRST_SBN_EID + 17
#define SBN_MSG_EID             FIRST_SBN_EID + 18
#define SBN_CMD_EID              FIRST_SBN_EID + 19
#define SBN_FILE_EID              FIRST_SBN_EID + 20
#define SBN_PROTO_EID             FIRST_SBN_EID + 21
#define SBN_PEER_EID       FIRST_SBN_EID + 25
#define SBN_INV_LINE_EID          FIRST_SBN_EID + 26

#define SBN_LSC_EID               FIRST_SBN_EID + 29
#define SBN_SUB_EID           FIRST_SBN_EID + 32
#define SBN_SB_EID           FIRST_SBN_EID + 37

#define SBN_PIPE_EID              FIRST_SBN_EID + 44

#define SBN_DEBUG_EID                 FIRST_SBN_EID + 46

#endif /* _sbn_main_events_h_ */
