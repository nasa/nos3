/************************************************************************
** File:
**    newsim_events.h
**
** Purpose:
**  Define NEWSIM application event IDs
**
*************************************************************************/

#ifndef _NEWSIM_EVENTS_H_
#define _NEWSIM_EVENTS_H_

/* Standard app event IDs */
#define NEWSIM_RESERVED_EID              0
#define NEWSIM_STARTUP_INF_EID           1
#define NEWSIM_LEN_ERR_EID               2
#define NEWSIM_PIPE_ERR_EID              3
#define NEWSIM_SUB_CMD_ERR_EID           4
#define NEWSIM_SUB_REQ_HK_ERR_EID        5
#define NEWSIM_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define NEWSIM_CMD_ERR_EID               10
#define NEWSIM_CMD_NOOP_INF_EID          11
#define NEWSIM_CMD_RESET_INF_EID         12
#define NEWSIM_CMD_ENABLE_INF_EID        13
#define NEWSIM_ENABLE_INF_EID            14
#define NEWSIM_ENABLE_ERR_EID            15
#define NEWSIM_CMD_DISABLE_INF_EID       16
#define NEWSIM_DISABLE_INF_EID           17
#define NEWSIM_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define NEWSIM_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define NEWSIM_DEVICE_TLM_ERR_EID        30
#define NEWSIM_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define NEWSIM_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define NEWSIM_UART_INIT_ERR_EID         40
#define NEWSIM_UART_CLOSE_ERR_EID        41

#endif /* _NEWSIM_EVENTS_H_ */
