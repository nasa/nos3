/************************************************************************
** File:
**    cfdp_events.h
**
** Purpose:
**  Define CFDP application event IDs
**
*************************************************************************/

#ifndef _CFDP_EVENTS_H_
#define _CFDP_EVENTS_H_

/* Standard app event IDs */
#define CFDP_RESERVED_EID              0
#define CFDP_STARTUP_INF_EID           1
#define CFDP_LEN_ERR_EID               2
#define CFDP_PIPE_ERR_EID              3
#define CFDP_SUB_CMD_ERR_EID           4
#define CFDP_SUB_REQ_HK_ERR_EID        5
#define CFDP_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define CFDP_CMD_ERR_EID               10
#define CFDP_CMD_NOOP_INF_EID          11
#define CFDP_CMD_RESET_INF_EID         12
#define CFDP_CMD_ENABLE_INF_EID        13
#define CFDP_ENABLE_INF_EID            14
#define CFDP_ENABLE_ERR_EID            15
#define CFDP_CMD_DISABLE_INF_EID       16
#define CFDP_DISABLE_INF_EID           17
#define CFDP_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define CFDP_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define CFDP_DEVICE_TLM_ERR_EID        30
#define CFDP_REQ_HK_ERR_EID            31


/* Device specific telemetry event IDs */
#define CFDP_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define CFDP_UART_INIT_ERR_EID         40
#define CFDP_UART_CLOSE_ERR_EID        41

#define CFDP_CMD_UPLOAD_EID			42

#define CFDP_INIT_INF_EID (20)

#endif