#ifndef _serial_events_h
#define _serial_events_h

#include "sbn_events.h"

#define SBN_SERIAL_EID          FIRST_SBN_SERIAL_EID + 1 /* skip 0th */
#define SBN_SERIAL_CONFIG_EID   FIRST_SBN_SERIAL_EID + 2
#define SBN_SERIAL_IO_EID       FIRST_SBN_SERIAL_EID + 3
#define SBN_SERIAL_QUEUE_EID    FIRST_SBN_SERIAL_EID + 4
#define SBN_SERIAL_RECEIVE_EID  FIRST_SBN_SERIAL_EID + 5
#define SBN_SERIAL_SEND_EID     FIRST_SBN_SERIAL_EID + 6

#endif /* _serial_events_h */
