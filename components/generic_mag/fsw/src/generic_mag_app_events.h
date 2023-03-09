/************************************************************************
** File:
**  generic_mag_app_events.h
**
** Purpose:
**  Define Generic_mag App Event IDs
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_MAG_APP_EVENTS_H_
#define _GENERIC_MAG_APP_EVENTS_H_

#define GENERIC_MAG_RESERVED_EID           0
#define GENERIC_MAG_STARTUP_INF_EID        1
#define GENERIC_MAG_COMMAND_ERR_EID        2
#define GENERIC_MAG_COMMANDNOP_INF_EID     3
#define GENERIC_MAG_COMMANDRST_INF_EID     4
#define GENERIC_MAG_INVALID_MSGID_ERR_EID  5
#define GENERIC_MAG_LEN_ERR_EID            6
#define GENERIC_MAG_PIPE_ERR_EID           7
#define GENERIC_MAG_CMD_DEVRST_INF_EID     8
#define GENERIC_MAG_UART_ERR_EID           9
#define GENERIC_MAG_UART_WRITE_ERR_EID    10
#define GENERIC_MAG_UART_READ_ERR_EID     11
#define GENERIC_MAG_COMMANDRAW_INF_EID    12
#define GENERIC_MAG_UART_MSG_CNT_DBG_EID  13
#define GENERIC_MAG_MUTEX_ERR_EID         14
#define GENERIC_MAG_CREATE_DEVICE_ERR_EID 15
#define GENERIC_MAG_DEVICE_REG_ERR_EID    16
#define GENERIC_MAG_DEVICE_REG_INF_EID    17

#define GENERIC_MAG_EVENT_COUNTS 17

#endif /* _GENERIC_MAG_APP_EVENTS_H_ */

/************************/
/*  End of File Comment */
/************************/
