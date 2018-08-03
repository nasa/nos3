/*************************************************************************
** File:
**   $Id: hs_cmds.h 1.3 2016/05/16 17:33:11EDT czogby Exp  $
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
**   Specification for the CFS Health and Safety (HS) routines that
**   handle command processing
**
** Notes:
**
**   $Log: hs_cmds.h  $
**   Revision 1.3 2016/05/16 17:33:11EDT czogby 
**   Move function prototype from hs_cmds.c file to hs_cmds.h file
**   Revision 1.2 2015/11/12 14:25:21EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.4 2015/03/03 12:16:20EST sstrege 
**   Added copyright information
**   Revision 1.3 2011/10/13 18:45:25EDT aschoeni 
**   Updated for hs utilization calibration functions
**   Revision 1.2 2009/05/04 17:44:32EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:41EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
**************************************************************************/
#ifndef _hs_cmds_h_
#define _hs_cmds_h_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Process a command pipe message
**
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
void HS_AppPipe(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset counters
**
**  \par Description
**       Utility function that resets housekeeping counters to zero
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #HS_ResetCmd
**
*************************************************************************/
void HS_ResetCounters(void);

/************************************************************************/
/** \brief Verify message length
**
**  \par Description
**       Checks if the actual length of a software bus message matches
**       the expected length and sends an error event if a mismatch
**       occurs
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that
**                                 references the software bus message
**
**  \param [in]   ExpectedLength   The expected length of the message
**                                 based upon the command code
**
**  \returns
**  \retstmt Returns TRUE if the length is as expected      \endcode
**  \retstmt Returns FALSE if the length is not as expected \endcode
**  \endreturns
**
**  \sa #HS_LEN_ERR_EID
**
*************************************************************************/
boolean HS_VerifyMsgLength(CFE_SB_MsgPtr_t msg,
                           uint16          ExpectedLength);
/************************************************************************/
/** \brief Manages HS tables
**
**  \par Description
**       Manages load requests for the AppMon, EventMon, ExeCount and MsgActs
**       tables and update notification for the AppMon and MsgActs tables.
**       Also releases and acquires table addresses. Gets called at the start
**       of each processing cycle and on initialization.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #CFE_TBL_Manage
**
*************************************************************************/
void HS_AcquirePointers(void);

/************************************************************************/
/** \brief Housekeeping request
**
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
*************************************************************************/
void HS_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Noop command
**
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_NOOP_CC
**
*************************************************************************/
void HS_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset counters command
**
**  \par Description
**       Processes a reset counters ground command which will reset
**       the following HS application counters to zero:
**         - Command counter
**         - Command error counter
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_RESET_CC
**
*************************************************************************/
void HS_ResetCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process an enable critical applications monitor command
**
**  \par Description
**       Allows the critical applications to be monitored.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_ENABLE_APPMON_CC
**
*************************************************************************/
void HS_EnableAppMonCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a disable critical applications monitor command
**
**  \par Description
**       Stops the critical applications from be monitored.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_DISABLE_APPMON_CC
**
*************************************************************************/
void HS_DisableAppMonCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process an enable critical events monitor command
**
**  \par Description
**       Allows the critical events to be monitored.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_ENABLE_EVENTMON_CC
**
*************************************************************************/
void HS_EnableEventMonCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a disable critical events monitor command
**
**  \par Description
**       Stops the critical events from be monitored.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_DISABLE_EVENTMON_CC
**
*************************************************************************/
void HS_DisableEventMonCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process an enable aliveness indicator command
**
**  \par Description
**       Allows the aliveness indicator to be output to the UART.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_ENABLE_ALIVENESS_CC
**
*************************************************************************/
void HS_EnableAlivenessCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a disable aliveness indicator command
**
**  \par Description
**       Stops the aliveness indicator from being output on the UART.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_DISABLE_ALIVENESS_CC
**
*************************************************************************/
void HS_DisableAlivenessCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process an enable CPU Hogging indicator command
**
**  \par Description
**       Allows the CPU Hogging indicator to be output as an event.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_ENABLE_CPUHOG_CC
**
*************************************************************************/
void HS_EnableCPUHogCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a disable CPU Hogging indicator command
**
**  \par Description
**       Stops the CPU Hogging indicator from being output as an event.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_DISABLE_CPUHOG_CC
**
*************************************************************************/
void HS_DisableCPUHogCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a reset resets performed command
**
**  \par Description
**       Resets the count of HS performed resets maintained by HS.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_SET_MAX_RESETS_CC
**
*************************************************************************/
void HS_ResetResetsPerformedCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a set max resets command
**
**  \par Description
**       Sets the max number of HS performed resets to the specified value.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #HS_RESET_RESETS_PERFORMED_CC
**
*************************************************************************/
void HS_SetMaxResetsCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Refresh Critical Applications Monitor Status
**
**  \par Description
**       This function gets called when HS detects that a new critical
**       applications monitor table has been loaded or when a command
**       to enable the critical applications monitor is received: it then
**       refreshes the timeouts for application being monitored
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_AppMonStatusRefresh(void);

/************************************************************************/
/** \brief Refresh Message Actions Status
**
**  \par Description
**       This function gets called when HS detects that a new
**       message actions table has been loaded: it then
**       resets the cooldowns for all actions.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_MsgActsStatusRefresh(void);

#endif /* _hs_cmds_h_ */

/************************/
/*  End of File Comment */
/************************/
