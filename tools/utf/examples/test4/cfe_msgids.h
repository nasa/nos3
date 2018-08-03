/******************************************************************************
** File: cfe_msgids.h
** $Id: cfe_msgids.h 1.1 2008/04/17 07:09:15GMT-05:00 ruperera Exp  $
**
** Purpose:
**   This header file contains the Message Id's for messages used by the
**   cFE core.
**
** Author:   R.McGraw/SSI
**
** Notes:
**   This file should not contain messages defined by cFE external
**   applications.
**
** $Date: 2008/04/17 07:09:15GMT-05:00 $
** $Revision: 1.1 $
** $Log: cfe_msgids.h  $
** Revision 1.1 2008/04/17 07:09:15GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test4/project.pj
** Revision 1.2 2006/05/05 15:47:00EDT nsschweiss 
** Added macro definitions needed for test.
** Revision 1.1 2006/03/21 16:44:16EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test4/project.pj
**  2005/09/21 Revised for TT template task. nschweiss
**  2005/07/25 Updated for QQ template task. nschweiss
** Revision 1.1 2005/06/09 10:57:58EDT rperera 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-REPOSITORY/config/cpu1/inc/project.pj
** Revision 1.7  2005/04/28 20:30:34  rmcgraw
** Renamed BOXx to CPUx
**
** Revision 1.6  2005/04/21 18:48:26  rmcgraw
** Replaced FM defines with FS
**
** Revision 1.5  2005/04/18 20:53:11  swalling
** added message ID for ES task telemetry packet
**
** Revision 1.4  2005/04/14 19:38:07  rmcgraw
** Added SB Net and Stat Tlm Msg Ids
**
** Revision 1.3  2005/04/08 22:10:22  swalling
** added FS task message definitions
**
** Revision 1.2  2005/04/05 18:25:48  swalling
** added TIME diagnostics tlm packet ID
**
** Revision 1.1  2005/04/04 19:42:00  acudmore
** added cpu1 config
**
** Revision 1.3  2005/03/22 15:59:31  rmcgraw
** Changed some Time pkts to have global MsgIds
**
** Revision 1.2  2005/03/18 22:39:28  swalling
** fixed typo -- CFE_TIME_CMD_MSG was MSSG
**
** Revision 1.1  2005/03/18 21:39:30  rmcgraw
** Initial Release
**
**
**
******************************************************************************/
#ifndef _cfe_msgids_
#define _cfe_msgids_

/*
** Includes
*/

#include "cfe_mission_cfg.h"
#define CFE_TT_HK_TLM_MSG       13
#define CFE_TT_SEND_HK_MSG      26
#define CFE_TT_CMD_MSG          9
/*
** cFE Command Message Id's
*/
#define CFE_EVS_CMD_MID         CFE_CMD_MID_BASE_CPU1 + CFE_EVS_CMD_MSG         /* 0x1801 */
#define CFE_FS_CMD_MID          CFE_CMD_MID_BASE_CPU1 + CFE_FS_CMD_MSG          /* 0x1802 */
#define CFE_SB_CMD_MID          CFE_CMD_MID_BASE_CPU1 + CFE_SB_CMD_MSG          /* 0x1803 */
#define CFE_TBL_CMD_MID         CFE_CMD_MID_BASE_CPU1 + CFE_TBL_CMD_MSG         /* 0x1804 */
#define CFE_TIME_CMD_MID        CFE_CMD_MID_BASE_CPU1 + CFE_TIME_CMD_MSG        /* 0x1805 */
#define CFE_ES_CMD_MID          CFE_CMD_MID_BASE_CPU1 + CFE_ES_CMD_MSG          /* 0x1806 */
#define CFE_TT_CMD_MID          CFE_CMD_MID_BASE_CPU1 + CFE_TT_CMD_MSG          /* 0X1807 */

#define CFE_ES_SEND_HK_MID      CFE_CMD_MID_BASE_CPU1 + CFE_ES_SEND_HK_MSG      /* 0x1808 */
#define CFE_EVS_SEND_HK_MID     CFE_CMD_MID_BASE_CPU1 + CFE_EVS_SEND_HK_MSG     /* 0x1809 */
#define CFE_FS_SEND_HK_MID      CFE_CMD_MID_BASE_CPU1 + CFE_FS_SEND_HK_MSG      /* 0x180A */
#define CFE_SB_SEND_HK_MID      CFE_CMD_MID_BASE_CPU1 + CFE_SB_SEND_HK_MSG      /* 0x180B */
#define CFE_TBL_SEND_HK_MID     CFE_CMD_MID_BASE_CPU1 + CFE_TBL_SEND_HK_MSG     /* 0x180C */
#define CFE_TIME_SEND_HK_MID    CFE_CMD_MID_BASE_CPU1 + CFE_TIME_SEND_HK_MSG    /* 0x180D */
#define CFE_TT_SEND_HK_MID      CFE_CMD_MID_BASE_CPU1 + CFE_TT_SEND_HK_MSG      /* 0x180E */

#define CFE_TIME_TONE_CMD_MID   CFE_CMD_MID_BASE_CPU1 + CFE_TIME_TONE_CMD_MSG   /* 0x1810 */
#define CFE_TIME_1HZ_CMD_MID    CFE_CMD_MID_BASE_CPU1 + CFE_TIME_1HZ_CMD_MSG    /* 0x1811 */


/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID   CFE_CMD_MID_BASE_GLOB + CFE_TIME_DATA_CMD_MSG   /* 0x1860 */
#define CFE_TIME_FAKE_CMD_MID   CFE_CMD_MID_BASE_GLOB + CFE_TIME_FAKE_CMD_MSG   /* 0x1861 */
#define CFE_TIME_SEND_CMD_MID   CFE_CMD_MID_BASE_GLOB + CFE_TIME_SEND_CMD_MSG   /* 0x1862 */


/*
** CFE Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID       CFE_TLM_MID_BASE_CPU1 + CFE_ES_HK_TLM_MSG       /* 0x0800 */
#define CFE_EVS_HK_TLM_MID      CFE_TLM_MID_BASE_CPU1 + CFE_EVS_HK_TLM_MSG      /* 0x0801 */
#define CFE_FS_HK_TLM_MID       CFE_TLM_MID_BASE_CPU1 + CFE_FS_HK_TLM_MSG       /* 0x0802 */
#define CFE_SB_HK_TLM_MID       CFE_TLM_MID_BASE_CPU1 + CFE_SB_HK_TLM_MSG       /* 0x0803 */
#define CFE_TBL_HK_TLM_MID      CFE_TLM_MID_BASE_CPU1 + CFE_TBL_HK_TLM_MSG      /* 0x0804 */
#define CFE_TIME_HK_TLM_MID     CFE_TLM_MID_BASE_CPU1 + CFE_TIME_HK_TLM_MSG     /* 0x0805 */
#define CFE_TIME_DIAG_TLM_MID   CFE_TLM_MID_BASE_CPU1 + CFE_TIME_DIAG_TLM_MSG   /* 0x0806 */
#define CFE_EVS_EVENT_MSG_MID   CFE_TLM_MID_BASE_CPU1 + CFE_EVS_EVENT_MSG_MSG   /* 0x0808 */
#define CFE_SB_NETWORK_TLM_MID  CFE_TLM_MID_BASE_CPU1 + CFE_SB_NETWORK_TLM_MSG  /* 0x0809 */
#define CFE_SB_STATS_TLM_MID    CFE_TLM_MID_BASE_CPU1 + CFE_SB_STATS_TLM_MSG    /* 0x080A */
#define CFE_ES_APP_TLM_MID      CFE_TLM_MID_BASE_CPU1 + CFE_ES_APP_TLM_MSG      /* 0x080B */
#define CFE_TT_HK_TLM_MID       CFE_TLM_MID_BASE_CPU1 + CFE_TT_HK_TLM_MSG       /* 0x080C */


#endif
/* cfe_msgids.h */

