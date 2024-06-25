/************************************************************************
** File:
**   $Id: newsim_msgids.h  $
**
** Purpose:
**  Define NEWSIM Message IDs
**
*************************************************************************/
#ifndef _NEWSIM_MSGIDS_H_
#define _NEWSIM_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define NEWSIM_CMD_MID              0x17FA /* TODO: Change this for your app */ 

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define NEWSIM_REQ_HK_MID           0x17FB /* TODO: Change this for your app */

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define NEWSIM_HK_TLM_MID           0x07FA /* TODO: Change this for your app */
#define NEWSIM_DEVICE_TLM_MID       0x07FB /* TODO: Change this for your app */

#endif /* _NEWSIM_MSGIDS_H_ */
