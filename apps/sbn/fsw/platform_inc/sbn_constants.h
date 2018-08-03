/******************************************************************************
** File: sbn_constants.h
**
**      Copyright (c) 2004-2016, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This software may be used only pursuant to a United States government
**      sponsored project and the United States government may not be charged
**      for use thereof.
**
** Purpose:
**      This header file contains prototypes for private functions and type
**      definitions for the Software Bus Network Application.
**
** Authors:   J. Wilmot/GSFC Code582
**            R. McGraw/SSI
**            C. Knight/ARC Code TI
******************************************************************************/
#include "cfe.h"

#ifndef _sbn_constants_h_
#define _sbn_constants_h_

#define SBN_OK                        0
#define SBN_ERROR                     (-1)
#define SBN_IF_EMPTY                  (-2)
#define SBN_NOT_IMPLEMENTED           (-3)

#define SBN_MAX_MSG_SIZE              1400
#define SBN_MAX_SUBS_PER_PEER         256 
#define SBN_MAX_PEERNAME_LENGTH       8
#define SBN_DONT_CARE                 0
#define SBN_MAX_PEER_PRIORITY         16

/* at most process this many SB messages per peer per wakeup */
#define SBN_MAX_MSG_PER_WAKEUP        32

#define SBN_IPv4                      1
#define SBN_IPv6                      2  /* not implemented */
#define SBN_SPACEWIRE_RMAP            3  /* not implemented */
#define SBN_SPACEWIRE_PKT             4  /* not implemented */
#define SBN_SHMEM                     5
#define SBN_SERIAL                    6
#define SBN_1553                      7  /* not implemented */

#define SBN_MAIN_LOOP_DELAY           200 /* milli-seconds */

/* A peer can either be disconnected (and we have it marked as "ANNOUNCING")
 * or it is connected and we expect traffic or if we don't see any in a period
 * of time, we send a heartbeat to see if the peer is alive.
 */

/* How many seconds to wait between announce messages. */
#define SBN_ANNOUNCE_TIMEOUT          10

/* If I don't send out traffic for a period of time, send out a heartbeat
 * so that my peer doesn't think I've disappeared. (This should be shorter
 * than the SBN_HEARTBEAT_TIMEOUT below!)
 */
#define SBN_HEARTBEAT_SENDTIME        5

/* How many seconds since I last saw a message from my peer do I mark it as
 * timed out?
 */
#define SBN_HEARTBEAT_TIMEOUT         10

#define SBN_PEER_PIPE_DEPTH           64
#define SBN_DEFAULT_MSG_LIM           8
#define SBN_ITEMS_PER_FILE_LINE       6
#define SBN_MSG_BUFFER_SIZE           (SBN_PEER_PIPE_DEPTH * 2) /* uint8 */

/* Interface Roles */
#define SBN_HOST         1
#define SBN_PEER         2

/* SBN States */
#define SBN_ANNOUNCING                0
#define SBN_HEARTBEATING              1

#endif /* _sbn_constants_h_ */
/*****************************************************************************/
