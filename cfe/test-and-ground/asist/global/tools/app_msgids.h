/******************************************************************************
** File: app_msgids.h
** $Id: app_msgids.h 1.1 2008/04/17 07:06:19GMT-05:00 ruperera Exp  $
**
** Purpose:
**   This file contains application message id defines for a single processor
**
** Author:
**   R. McGraw/SSI
**
** $Date: 2008/04/17 07:06:19GMT-05:00 $
** $Revision: 1.1 $
** $Log: app_msgids.h  $
** Revision 1.1 2008/04/17 07:06:19GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/test-and-ground/asist/global/tools/project.pj
** Revision 1.1 2006/10/27 09:20:18EDT wfmoleski 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/test-and-ground/asist/global/tools/project.pj
** Revision 1.1.1.4 2005/11/10 13:31:26EST kkaudra 
** Added TST_TBL2 MID definitions
** Revision 1.1.1.3 2005/09/21 16:43:28EDT kkaudra 
** Added tst_tbl MID definitions
** Revision 1.1.1.2 2005/06/28 08:52:48EDT kkaudra 
** Added test application command and telemetry message Id's
** Revision 1.1.1.1 2005/06/09 10:57:58EDT kkaudra 
** Duplicate revision
** Revision 1.5  2005/05/26 12:29:01  kaudra
** changed APP_CMD_MID_BASE_CPU1 to APP_TST_CMD_MID_BASE_CPU1
**
** Revision 1.4  2005/05/19 11:27:58  kaudra
** changed test application command message id from BTA_CMD_MID to TST_EVS_CMD_MID
**
** Revision 1.3  2005/05/17 19:37:55  kaudra
** log message
**
** Revision 1.2  2005/04/28 20:30:34  rmcgraw
** Renamed BOXx to CPUx
**
** Revision 1.1  2005/04/04 19:42:00  acudmore
** added cpu1 config
**
** Revision 1.2  2005/03/23 19:11:24  rmcgraw
** Re-ordered top few lines
**
** Revision 1.1  2005/03/23 18:53:11  rmcgraw
** Initial Release
**
**
******************************************************************************/
#ifndef _app_msgids_
#define _app_msgids_

/*
** Includes
*/
#include "app_mission_cfg.h"


/*
** Application Command Message Id's
*/
#define TO_CMD_MID          APP_CMD_MID_BASE_CPU1 + TO_CMD_MSG          /* 0x1880 */
#define CI_CMD_MID          APP_CMD_MID_BASE_CPU1 + CI_CMD_MSG          /* 0x1881 */

/*
** Test Application Command Message Id's 
*/
#define TST_EVS_CMD_MID     APP_TST_CMD_MID_BASE_CPU1 + TST_EVS_CMD_MSG     /* 0X1882 */
#define TST_TIME_CMD_MID    APP_TST_CMD_MID_BASE_CPU1 + TST_TIME_CMD_MSG    /* 0X1883 */
#define TST_SB_CMD_MID      APP_TST_CMD_MID_BASE_CPU1 + TST_SB_CMD_MSG      /* 0X1885 */
#define TST_TBL_CMD_MID     APP_TST_CMD_MID_BASE_CPU1 + TST_TBL_CMD_MSG     /* 0X1886 */
#define TST_TBL2_CMD_MID    APP_TST_CMD_MID_BASE_CPU1 + TST_TBL2_CMD_MSG    /* 0X1887 */




#define TO_SEND_HK_MID         APP_CMD_MID_BASE_CPU1 + TO_SEND_HK_MSG                     /* 0x1890 */
#define CI_SEND_HK_MID         APP_CMD_MID_BASE_CPU1 + CI_SEND_HK_MSG                     /* 0x1891 */
#define TST_TIME_SEND_HK_MID   APP_TST_CMD_MID_BASE_CPU1 + TST_TIME_SEND_HK_MSG          /* 0x1884 */
#define TST_SB_SEND_HK_MID     APP_TST_CMD_MID_BASE_CPU1 + TST_SB_SEND_HK_MSG            /* 0x1893 */
#define TST_TBL_SEND_HK_MID    APP_TST_CMD_MID_BASE_CPU1 + TST_TBL_SEND_HK_MSG           /* 0x1894 */
#define TST_TBL2_SEND_HK_MID   APP_TST_CMD_MID_BASE_CPU1 + TST_TBL2_SEND_HK_MSG          /* 0x1895 */



/*
** Application Telemetry Message Id's
*/
#define TO_HK_TLM_MID       APP_TLM_MID_BASE_CPU1 + TO_HK_TLM_MSG       /* 0X0880 */
#define CI_HK_TLM_MID       APP_TLM_MID_BASE_CPU1 + CI_HK_TLM_MSG       /* 0X0881 */


#define TST_TIME_HK_TLM_MID       APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_HK_TLM_MSG        /* 0X0700 */
#define TST_TIME_TAI_TLM_MID      APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_TAI_TLM_MSG       /* 0X0701 */       
#define TST_TIME_UTC_TLM_MID      APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_UTC_TLM_MSG       /* 0X0702 */
#define TST_TIME_ST_TLM_MID       APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_ST_TLM_MSG        /* 0X0703 */
#define TST_TIME_MET_TLM_MID      APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_MET_TLM_MSG       /* 0X0704 */
#define TST_TIME_METSEC_TLM_MID   APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_METSEC_TLM_MSG    /* 0X0705 */
#define TST_TIME_METSUB_TLM_MID   APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_METSUB_TLM_MSG    /* 0X0706 */
#define TST_TIME_STCF_TLM_MID     APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_STCF_TLM_MSG      /* 0X0707 */
#define TST_TIME_LPSEC_TLM_MID    APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_LPSEC_TLM_MSG     /* 0X0708 */
#define TST_TIME_CLKST_TLM_MID    APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_CLKST_TLM_MSG     /* 0X0709 */
#define TST_TIME_ADD_TLM_MID      APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_ADD_TLM_MSG       /* 0X070A */
#define TST_TIME_SUBTIME_TLM_MID  APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_SUBTIME_TLM_MSG   /* 0X070B */
#define TST_TIME_COMP_TLM_MID     APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_COMP_TLM_MSG      /* 0X070C */
#define TST_TIME_SUBMICRO_TLM_MID APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_SUBMICRO_TLM_MSG  /* 0X070D */
#define TST_TIME_MICROSUB_TLM_MID APP_TST_TLM_MID_BASE_CPU1 + TST_TIME_MICROSUB_TLM_MSG  /* 0X070E */


#define TST_SB_HK_TLM_MID         APP_TST_TLM_MID_BASE_CPU1 + TST_SB_HK_TLM_MSG          /* 0X070F */
#define TST_SB_PIPE3_TLM_MID      APP_TST_TLM_MID_BASE_CPU1 + TST_SB_PIPE3_TLM_MSG       /* 0x0710 */

#define TST_TBL_HK_TLM_MID        APP_TST_TLM_MID_BASE_CPU1 + TST_TBL_HK_TLM_MSG         /* 0X0711 */
#define TST_TBL_MAXTBLST_TLM_MID  APP_TST_TLM_MID_BASE_CPU1 + TST_TBL_MAXTBLST_TLM_MSG   /* 0X0712 */
#define TST_TBL2_HK_TLM_MID       APP_TST_TLM_MID_BASE_CPU1 + TST_TBL2_HK_TLM_MSG        /* 0X0713 */

#define TO_DATA_TYPES_MID         APP_TLM_MID_BASE_CPU1 + TO_DATA_TYPES_MSG   /* 0X0890 */


#endif

