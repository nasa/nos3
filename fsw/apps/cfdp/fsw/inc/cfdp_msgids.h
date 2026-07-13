/************************************************************************
** File:
**   $Id: cfdp_msgids.h  $
**
** Purpose:
**  Define CFDP Message IDs
**
*************************************************************************/
#ifndef _CFDP_MSGIDS_H_
#define _CFDP_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define CFDP_CMD_MID              0x185A
/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define CFDP_REQ_HK_MID           0x185B 

/** \brief Message ID for waking up the processing cycle */
#define CFDP_WAKE_UP_MID 0x185C
/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define CFDP_HK_TLM_MID           0x085A 
#define CFDP_FILEDOWNLOAD_TLM_MID 0x085C

#endif /* _CFDP_MSGIDS_H_ */
