/************************************************************************
** File:
**   $Id: generic_star_tracker_msgids.h  $
**
** Purpose:
**  Define GENERIC_STAR_TRACKER Message IDs
**
*************************************************************************/
#ifndef _GENERIC_STAR_TRACKER_MSGIDS_H_
#define _GENERIC_STAR_TRACKER_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_STAR_TRACKER_CMD_MID              0x1935 

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_STAR_TRACKER_REQ_HK_MID           0x1936

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_STAR_TRACKER_HK_TLM_MID           0x0935
#define GENERIC_STAR_TRACKER_DEVICE_TLM_MID       0x0936

#endif /* _GENERIC_STAR_TRACKER_MSGIDS_H_ */
