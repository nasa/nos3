/**********************************************************
** File: sbn_lib_defs.h
**
** Author: ejtimmon
** Date: 01 Sep 2015
**
** Purpose:
**  This file contains constants used by the SBN library.
**
**********************************************************/


#ifndef _sbn_lib_defs_h_
#define _sbn_lib_defs_h_

/* Message types definitions */
#define SBN_NO_MSG                    0
#define SBN_ANNOUNCE_MSG              0x0010
#define SBN_ANNOUNCE_ACK_MSG          0x0011
#define SBN_HEARTBEAT_MSG             0x0020
#define SBN_HEARTBEAT_ACK_MSG         0x0021
#define SBN_SUBSCRIBE_MSG             0x0030
#define SBN_UN_SUBSCRIBE_MSG          0x0040
#define SBN_APP_MSG                   0x0050
#define SBN_COMMAND_ACK_MSG           0x0060
#define SBN_COMMAND_NACK_MSG          0x0070


#endif /* _sbn_lib_defs_h_ */
