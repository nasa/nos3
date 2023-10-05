/************************************************************************
** File:
**    generic_star_tracker_events.h
**
** Purpose:
**  Define GENERIC_STAR_TRACKER application event IDs
**
*************************************************************************/

#ifndef _GENERIC_STAR_TRACKER_EVENTS_H_
#define _GENERIC_STAR_TRACKER_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_STAR_TRACKER_RESERVED_EID              0
#define GENERIC_STAR_TRACKER_STARTUP_INF_EID           1
#define GENERIC_STAR_TRACKER_LEN_ERR_EID               2
#define GENERIC_STAR_TRACKER_PIPE_ERR_EID              3
#define GENERIC_STAR_TRACKER_SUB_CMD_ERR_EID           4
#define GENERIC_STAR_TRACKER_SUB_REQ_HK_ERR_EID        5
#define GENERIC_STAR_TRACKER_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_STAR_TRACKER_CMD_ERR_EID               10
#define GENERIC_STAR_TRACKER_CMD_NOOP_INF_EID          11
#define GENERIC_STAR_TRACKER_CMD_RESET_INF_EID         12
#define GENERIC_STAR_TRACKER_CMD_ENABLE_INF_EID        13
#define GENERIC_STAR_TRACKER_ENABLE_INF_EID            14
#define GENERIC_STAR_TRACKER_ENABLE_ERR_EID            15
#define GENERIC_STAR_TRACKER_CMD_DISABLE_INF_EID       16
#define GENERIC_STAR_TRACKER_DISABLE_INF_EID           17
#define GENERIC_STAR_TRACKER_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define GENERIC_STAR_TRACKER_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define GENERIC_STAR_TRACKER_DEVICE_TLM_ERR_EID        30
#define GENERIC_STAR_TRACKER_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define GENERIC_STAR_TRACKER_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define GENERIC_STAR_TRACKER_UART_INIT_ERR_EID         40
#define GENERIC_STAR_TRACKER_UART_CLOSE_ERR_EID        41

#endif /* _GENERIC_STAR_TRACKER_EVENTS_H_ */
