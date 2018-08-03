/*************************************************************************
** File:
**   $Id: hs_custom.h 1.3 2016/05/16 17:40:01EDT czogby Exp  $
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
**   Specification for the CFS Health and Safety (HS) mission specific
**   custom function interface
**
**   $Log: hs_custom.h  $
**   Revision 1.3 2016/05/16 17:40:01EDT czogby 
**   Move function prototypes and constants from hs_custom.c file to hs_custom.h file
**   Revision 1.2 2015/11/12 14:25:17EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.2 2015/03/03 12:16:02EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/10/13 18:49:05EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**   Revision 1.2 2010/09/29 18:27:42EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.1 2010/09/13 16:34:38EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**   Revision 1.1 2010/07/19 11:49:55EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/MMS-REPOSITORY/dev/eeprom-fsw/apps/hs/fsw/src/project.pj
**
**************************************************************************/
#ifndef _hs_custom_
#define _hs_custom_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Constants
*************************************************************************/
#define HS_UTIL_DIAG_REPORTS 4

/*
** Command Codes
** Custom codes must not conflict with those in hs_msgdefs.h
*/

/** \hscmd Report Util Diagnostics
**
**  \par Description
**       Reports the Utilization Diagnostics
**
**  \hscmdmnemonic \HS_REPORTDIAG
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - The #HS_UTIL_DIAG_REPORT_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**
**  \par Criticality
**       None
*/
#define HS_REPORT_DIAG_CC          12

/** \hscmd Set Utilization Calibration Parameters
**
**  \par Description
**       Sets the Utilization Calibration Parameter
**
**  \hscmdmnemonic \HS_SETUTILPARAMS
**
**  \par Command Structure
**       #HS_SetUtilParamsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Any parameter is set to 0.
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**
**  \par Criticality
**       None
*/
#define HS_SET_UTIL_PARAMS_CC      13

/** \hscmd Set Utilization Diagnostics Parameter
**
**  \par Description
**       Sets the Utilization Diagnostics parameter
**
**  \hscmdmnemonic \HS_SETUTILDIAG
**
**  \par Command Structure
**       #HS_SetUtilDiagCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**
**  \par Criticality
**       None
*/
#define HS_SET_UTIL_DIAG_CC        14

/*
** Event IDs
** Custom Event IDs must not conflict with those in hs_events.h
*/

/** \brief <tt> 'Error Creating Child Task for CPU Utilization Monitoring,RC=0x\%08X' </tt>
**  \event <tt> 'Error Creating Child Task for CPU Utilization Monitoring,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  is unable to create its child task via the #CFE_ES_CreateChildTask
**  API
**
**  The \c RC field contains the return status from the
**  #CFE_ES_CreateChildTask call that generated the error
*/
#define HS_CR_CHILD_TASK_ERR_EID 101

/** \brief <tt> 'Error Registering Sync Callback for CPU Utilization Monitoring,RC=0x\%08X' </tt>
**  \event <tt> 'Error Registering Sync Callback for CPU Utilization Monitoring,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  is unable to create its sync callback via the #CFE_TIME_RegisterSynchCallback
**  API
**
**  The \c RC field contains the return status from the
**  #CFE_TIME_RegisterSynchCallback call that generated the error.
*/
#define HS_CR_SYNC_CALLBACK_ERR_EID 102

/** \brief <tt> 'Mask 0x\%08X Base Time Ticks per Idle Ticks (frequency): \%i(\%i), \%i(\%i), \%i(\%i), \%i(\%i)' </tt>
**  \event <tt> 'Mask 0x\%08X Base Time Ticks per Idle Ticks (frequency): \%i(\%i), \%i(\%i), \%i(\%i), \%i(\%i)' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety receives the #HS_REPORT_DIAG_CC command.
**
**  The \c Mask field contains the current utilization diagnostics mask. The subsequent fields contain from lowest to
**  fourth lowest, the time time ticks per idle ticks, and the number of times it occurred (in parentheses).
*/
#define HS_UTIL_DIAG_REPORT_EID 103

/** \brief <tt> 'Utilization Parms set: Mult1: \%d Div: \%d Mult2: \%d' </tt>
**  \event <tt> 'Utilization Parms set: Mult1: \%d Div: \%d Mult2: \%d' </tt>
**
**  \par Type: Debug
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety successfully processes the #HS_SET_UTIL_PARAMS_CC command.
**
**  The Mult1, Div, and Mult2 fields contain the updated utilization parameters.
*/
#define HS_SET_UTIL_PARAMS_DBG_EID 104

/** \brief <tt> 'Utilization Parms Error: No parameter may be 0: Mult1: \%d Div: \%d Mult2: \%d' </tt>
**  \event <tt> 'Utilization Parms Error: No parameter may be 0: Mult1: \%d Div: \%d Mult2: \%d' </tt>
**
**  \par Type: Error
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety fails to processes the #HS_SET_UTIL_PARAMS_CC command.
**  due to a 0 as at least one of the parameters.
**
**  The Mult1, Div, and Mult2 fields contain the utilization parameters from the command.
*/
#define HS_SET_UTIL_PARAMS_ERR_EID 105

/** \brief <tt> 'Utilization Diagnostics Mask has been set to \%08X' </tt>
**  \event <tt> 'Utilization Diagnostics Mask has been set to \%08X' </tt>
**
**  \par Type: Debug
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety successfully processes the #HS_SET_UTIL_DIAG_CC command.
**
**  The Mask parameter contains the new mask value.
*/
#define HS_SET_UTIL_DIAG_DBG_EID 106

/*************************************************************************
** Command Structure Definitions
*************************************************************************/
typedef struct
{
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];
    int32          Mult1;
    int32          Div;
    int32          Mult2;
} HS_SetUtilParamsCmd_t;

typedef struct
{
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32         Mask;
} HS_SetUtilDiagCmd_t;

/*************************************************************************
** Custom Global Data Structure
*************************************************************************/
typedef struct
{
  int32    UtilMult1;/**< \brief CPU Utilization Conversion Factor Multiplication 1 */
  int32    UtilDiv;/**< \brief CPU Utilization Conversion Factor Division */
  int32    UtilMult2;/**< \brief CPU Utilization Conversion Factor Multiplication 2 */

  uint32   UtilMask;/**< \brief Mask for determining Idle Tick length */
  uint32   UtilArrayIndex;/**< \brief Index for determining where to write in Util Array */
  uint32   UtilArrayMask;/**< \brief Mask for determining where to write in Util Array */
  uint32   UtilArray[HS_UTIL_TIME_DIAG_ARRAY_LENGTH];/**< \brief Array to store time stamps for determining idle tick length */

  uint32   ThisIdleTaskExec;/**< \brief Idle Task Exec Counter */
  uint32   LastIdleTaskExec;/**< \brief Idle Task Exec Counter at Previous Interval */
  uint32   LastIdleTaskInterval;/**< \brief Idle Task Increments during Previous Interval */
  uint32   UtilCycleCounter;/**< \brief Counter to determine when to monitor utilization */


   int32   IdleTaskRunStatus;/**< \brief HS Idle Task Run Status */
  uint32   IdleTaskID;/**< \brief HS Idle Task Task ID */
 
} HS_CustomData_t;

HS_CustomData_t HS_CustomData;

/*************************************************************************
** Exported Functions
*************************************************************************/

/************************************************************************/
/** \brief Initialize things needed for CPU Monitoring
**
**  \par Description
**       This function is intended to set up everything necessary for
**       CPU Utilization Monitoring at application startup. Currently,
**       this initializes the Idle Task, spawning the task itself,
**       and creating a 1Hz sync callback to mark the idle time.
**       It is called at the end of #HS_AppInit .
**       It may be updated to include other initializations, or 
**       modifications to already set parameters.
**
**  \par Assumptions, External Events, and Notes:
**       CFE_SUCCESS will be returned if all creation was performed
**       properly.
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_ES_CreateChildTask \endcode
**  \retstmt Return codes from #CFE_TIME_RegisterSynchCallback  \endcode
**  \endreturns
**
*************************************************************************/
int32 HS_CustomInit(void);

/************************************************************************/
/** \brief Clean up the functionality used for Utilization Monitoring
**
**  \par Description
**       This function is intended to clean up everything necessary for
**       CPU Utilization Monitoring at application termination. Currently,
**       this terminates the Idle Task, deleting the task itself,
**       and uncreating the 1Hz sync callback that marks the idle time.
**       It is called at the end of #HS_AppMain if HS is exiting cleanly.
**
**  \par Assumptions, External Events, and Notes:
**       Any resources that will not be cleaned up automatically be CFE
**       need to be cleaned up in this function.
**
*************************************************************************/
void HS_CustomCleanup(void);

/************************************************************************/
/** \brief Stub function for Utilization Monitoring
**
**  \par Description
**       This function is used as a passthrough to call #HS_MonitorUtilization
**       but can be modified to monitor utilization differently.
**       It is called during #HS_ProcessMain every HS cycle.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_CustomMonitorUtilization(void);

/************************************************************************/
/** \brief Stub function for Getting the Current Cycle Utilization
**
**  \par Description
**       This function is used to inform the Monitor Utilization function
**       of the current cycle utilization.
**       It is called during #HS_MonitorUtilization and should return a
**       value between 0 and #HS_UTIL_PER_INTERVAL_TOTAL.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
int32 HS_CustomGetUtil(void);

/************************************************************************/
/** \brief Process Custom Commands
**
**  \par Description
**       This function allows for hs_custom.c to define custom commands.
**       It will be called for any command code not already allocated
**       to a Health and Safety command. If a custom command is found,
**       then it is responsible for incrementing the command processed
**       or command error counter as appropriate.
**
**  \par Assumptions, External Events, and Notes:
**       If a command is found, this function MUST return #CFE_SUCCESS,
**       otherwise is must not return #CFE_SUCCESS
**
*************************************************************************/
int32 HS_CustomCommands(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Task that increments counters
**
**  \par Description
**       This child task is started by the HS initialization process. It
**       runs at the lowest priority on the system, incrementing a counter
**       when all other tasks are idle. This counter is used to determine
**       CPU Hogging (by being non-zero each cycle) and Utilization.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_IdleTask(void);

/************************************************************************/
/** \brief Increment the CPU Utilization Tracker Counter
**
**  \par Description
**       Utility function that increments the CPU Utilization tracking
**       counter, called by Idle Task. This counter is used to determine
**       both utilization and CPU Hogging.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #HS_UtilizationMark
**
*************************************************************************/
void HS_UtilizationIncrement(void);

/************************************************************************/
/** \brief Mark the CPU Utilization Tracker Counter
**
**  \par Description
**       Utility function that marks the CPU Utilization tracking
**       counter while saving the previous value to a variable for use in
**       calculating CPU Utilization and hogging.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #HS_UtilizationIncrement
**
*************************************************************************/
void HS_UtilizationMark(void);

/************************************************************************/
/** \brief Mark Idle Time Callback from Time App
**
**  \par Description
**       This function marks the idle time for the current interval.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_MarkIdleCallback(void);

/************************************************************************/
/** \brief Report Utilization Diagnostics information
**
**  \par Description
**       This function reports the Utilization Diagnostics data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_UtilDiagReport(void);

/************************************************************************/
/** \brief Set Utilization Paramters
**
**  \par Description
**       This function sets the Utilization Parameters.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_SetUtilParamsCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set Utilization Diagnostics Paramater
**
**  \par Description
**       This function sets the utilization diagnostics parameter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_SetUtilDiagCmd(CFE_SB_MsgPtr_t MessagePtr);


#endif /* _hs_custom_ */

/************************/
/*  End of File Comment */
/************************/

