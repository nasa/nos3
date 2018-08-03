/************************************************************************
** File:
**   $Id: hs_app.h 1.3 2016/05/16 17:28:40EDT czogby Exp  $
**
**   Copyright © 2007-2016 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**   Unit specification for the Core Flight System (CFS)
**   Health and Safety (HS) Application.
**
** Notes:
**
**   $Log: hs_app.h  $
**   Revision 1.3 2016/05/16 17:28:40EDT czogby 
**   Move function prototype from hs_app.c file to hs_app.h file
**   Revision 1.2 2015/11/12 14:25:26EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.13 2015/05/04 11:59:07EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.12 2015/05/04 10:59:53EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.11 2015/05/01 16:48:31EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.10 2015/03/03 12:16:00EST sstrege 
**   Added copyright information
**   Revision 1.9 2011/10/13 18:45:07EDT aschoeni 
**   Updated hs utilization calibration functions
**   Revision 1.8 2010/11/19 17:58:24EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.7 2010/10/14 17:43:25EDT aschoeni 
**   Change assumptions concerning utilization time period
**   Revision 1.6 2010/10/01 15:18:18EDT aschoeni 
**   Added Telemetry point to track message actions
**   Revision 1.5 2010/09/29 18:25:55EDT aschoeni 
**   Added Idle Task
**   Revision 1.4 2009/06/02 16:38:44EDT aschoeni 
**   Updated telemetry and internal status to support HS Internal Status bit flags
**   Revision 1.3 2009/05/21 16:10:55EDT aschoeni 
**   Updated based on errors found during unit testing
**   Revision 1.2 2009/05/04 17:44:27EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:32EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hs_app_h_
#define _hs_app_h_

/************************************************************************
** Includes
*************************************************************************/
#include "hs_msg.h"
#include "hs_tbl.h"
#include "cfe.h"
#include "cfe_msgids.h"
#include "cfe_platform_cfg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name HS Pipe Parameters */
/** \{ */
#define HS_CMD_PIPE_NAME      "HS_CMD_PIPE"
#define HS_EVENT_PIPE_NAME    "HS_EVENT_PIPE"
#define HS_WAKEUP_PIPE_NAME   "HS_WAKEUP_PIPE"
/** \} */

/**
** \name HS CDS Buffer String */
/** \{ */
#define HS_CDSNAME      "HS_CDS"
/** \} */

/************************************************************************
** Type Definitions
*************************************************************************/
/**
**  \brief HS CDS Data Structure
*/
typedef struct
{

    uint16         ResetsPerformed;/**< \brief Number of Resets Performed */
    uint16         ResetsPerformedNot;/**< \brief Inverted Resets Performed for validation */
    uint16         MaxResets;/**< \brief Max Number of Resets Allowed */
    uint16         MaxResetsNot;/**< \brief Inverted Max Number of Resets Allowed for validation */

} HS_CDSData_t;

/**
**  \brief HS Global Data Structure
*/
typedef struct
{

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_MsgPtr_t         MsgPtr;/**< \brief Pointer to msg received on software bus */

    CFE_SB_PipeId_t         CmdPipe;/**< \brief Pipe Id for HS command pipe */
    CFE_SB_PipeId_t         WakeupPipe;/**< \brief Pipe Id for HS wakeup pipe */
    CFE_SB_PipeId_t         EventPipe;/**< \brief Pipe Id for HK event pipe */
    uint8          ServiceWatchdogFlag;/**< \brief Flag of current watchdog servicing state */

    uint8    CurrentAppMonState;/**< \brief Status of HS Application Monitor */
    uint8    CurrentEventMonState;/**< \brief Status of HS Events Monitor */
    uint8    CurrentAlivenessState;/**< \brief Status of HS Aliveness Indicator */
    uint8    ExeCountState;/**< \brief Status of Execution Counter Table */

    uint8    MsgActsState;/**< \brief Status of Message Actions Table */
    uint8    CDSState;/**< \brief Status of Critical Data Storing */
    uint8    AppMonLoaded;/**< \brief If AppMon Table is loaded */
    uint8    EventMonLoaded;/**< \brief If EventMon Table is loaded */

    uint8    CurrentCPUHogState;/**< \brief Status of HS CPU Hogging Indicator */
    uint8    SpareBytes[3];/**< \brief Spare bytes for 32 bit alignment padding */

    uint16   CmdCount;/**< \brief Number of valid commands received */
    uint16   CmdErrCount;/**< \brief Number of invalid commands received */

    uint32   EventsMonitoredCount;/**< \brief Total count of event messages monitored */

    uint16   MsgActCooldown[HS_MAX_MSG_ACT_TYPES];/**< \brief Counts until Message Actions is available */
    uint16   AppMonCheckInCountdown[HS_MAX_MONITORED_APPS];/**< \brief Counts until Application Monitor times out */

    uint32   AppMonEnables[((HS_MAX_MONITORED_APPS - 1) / HS_BITS_PER_APPMON_ENABLE)+1];/**< \brief AppMon state by monitor */

    uint32   AppMonLastExeCount[HS_MAX_MONITORED_APPS];/**< \brief Last Execution Count for application being checked */

    uint32   AlivenessCounter;/**< \brief Current Count towards the CPU Aliveness output period */

    uint32   MsgActExec; /**< \brief Number of Software Bus Message Actions Executed */

    uint32   RunStatus;/**< \brief HS App run status */

    uint32   CurrentCPUHoggingTime;/**< \brief Count of cycles that CPU utilization is above hogging threshold */
    uint32   MaxCPUHoggingTime;/**< \brief Count of hogging cycles after which an event reports hogging */
    uint32   CurrentCPUUtilIndex;/**< \brief Current index into the Utilization Tracker */
    uint32   UtilizationTracker[HS_UTIL_PEAK_NUM_INTERVAL];/**< \brief Array to store utilization from previous intervals */

    uint32   UtilCpuAvg;/**< \brief Current CPU Utilization Average */
    uint32   UtilCpuPeak;/**< \brief Current CPU Utilization Peak */

    CFE_TBL_Handle_t        AMTableHandle;/**< \brief Apps Monitor table handle */
    CFE_TBL_Handle_t        EMTableHandle;/**< \brief Events Monitor table handle */
    CFE_TBL_Handle_t        MATableHandle;/**< \brief Message Actions table handle */

#if HS_MAX_EXEC_CNT_SLOTS != 0
    CFE_TBL_Handle_t        XCTableHandle;/**< \brief Execution Counters table handle */
    HS_XCTEntry_t   *XCTablePtr;/**< \brief Ptr to Execution Counters table entry */
#endif

    HS_AMTEntry_t   *AMTablePtr;/**< \brief Ptr to Apps Monitor table entry */
    HS_EMTEntry_t   *EMTablePtr;/**< \brief Ptr to Events Monitor table entry */
    HS_MATEntry_t   *MATablePtr;/**< \brief Ptr to Message Actions table entry */

    CFE_ES_CDSHandle_t MyCDSHandle;/* \brief Handle to CDS memory block */
    HS_CDSData_t       CDSData;  /* \brief Copy of Critical Data */

    /*
    ** Housekeeping telemetry packet...
    */
    HS_HkPacket_t           HkPacket;/**< \brief HK Housekeeping Packet */

} HS_AppData_t;

/************************************************************************
** Exported Data
*************************************************************************/
extern HS_AppData_t     HS_AppData;

/************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief CFS Health and Safety (HS) application entry point
**
**  \par Description
**       Health and Safety application entry point and main process loop.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_AppMain(void);

/************************************************************************/
/** \brief Initialize the CFS Health and Safety (HS) application
**
**  \par Description
**       Health and Safety application initialization routine. This
**       function performs all the required startup steps to
**       initialize HS data structures and get the application
**       registered with the cFE services so it can begin to receive
**       command messages.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #HS_SbInit       \endcode
**  \retstmt Return codes from #HS_TblInit      \endcode
**  \endreturns
**
*************************************************************************/
int32 HS_AppInit(void);

/************************************************************************/
/** \brief Initialize Software Bus
**
**  \par Description
**       This function performs the steps required to setup the
**       cFE software bus for use by the HS application
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe  \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe  \endcode
**  \endreturns
**
*************************************************************************/
int32 HS_SbInit(void);

/************************************************************************/
/** \brief Initialize cFE Table Services
**
**  \par Description
**       This function performs those steps required to initialize the
**       relationship between the HS App and the cFE Table Services.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_TBL_Register         \endcode
**  \retstmt Return codes from #CFE_TBL_GetAddress       \endcode
**  \endreturns
**
*************************************************************************/
int32 HS_TblInit(void);

/************************************************************************/
/** \brief Perform Normal Periodic Processing
**
**  \par Description
**       This function performs the normal Health and Safety monitoring
**       functions including application, event and execution counters,
**       as well as servicing the watchdog, outputing the aliveness 
**       indicator, and receiving commands or HK requests.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #HS_AcquirePointers       \endcode
**  \retstmt Return codes from #HS_ProcessCommands       \endcode
**  \endreturns
**
*************************************************************************/
int32 HS_ProcessMain(void);

/************************************************************************/
/** \brief Process commands received from cFE Software Bus
**
**  \par Description
**       This function pulls messages from command pipe and processes
**       them accordingly.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_SB_RcvMsg            \endcode
**  \endreturns
**
*************************************************************************/
int32 HS_ProcessCommands(void);

#endif /* _hs_app_h_ */

/************************/
/*  End of File Comment */
/************************/
