/*************************************************************************
** File:
**   $Id: hs_custom.c 1.6 2016/09/07 18:49:18EDT mdeschu Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
**   Purpose: Health and Safety (HS) application custom component.
**   This custom code is designed to work without modification; however the
**   nature of specific platforms may make it desirable to modify this code
**   to work in a more efficient way, or to provide greater functionality.
**
**   $Log: hs_custom.c  $
**   Revision 1.6 2016/09/07 18:49:18EDT mdeschu 
**   All CFE_EVS_SendEvents with format warning arguments were explicitly cast
**   Revision 1.5 2016/08/05 09:43:30EDT mdeschu 
**   Ticket #26: Fix HS build errors with strict settings
**   
**       Fix minor issues causing build to fail:
**   
**           Extra argument in CFE_SendEvent() call
**           Unused variable in HS_CustomCleanup()
**   Revision 1.4 2016/07/22 21:13:44EDT czogby 
**   Function HS_CustomGetUtil Does Not Protect Against Divide-By-Zero
**   Revision 1.3 2016/05/16 17:40:01EDT czogby 
**   Move function prototypes and constants from hs_custom.c file to hs_custom.h file
**   Revision 1.2 2015/11/12 14:25:19EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.4 2015/04/27 15:39:49EDT lwalling 
**   Fix typos -- UtilMult2=HS_UTIL_CONV_DIV and UtilDiv=HS_UTIL_CONV_MULT2
**   Revision 1.3 2015/03/03 12:16:07EST sstrege 
**   Added copyright information
**   Revision 1.2 2011/10/17 16:45:21EDT aschoeni 
**   paramters are now signed integers
**   Revision 1.1 2011/10/13 18:48:59EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**   Revision 1.6 2011/08/08 14:56:08EDT aschoeni 
**   Moved idle task execution counter increment outside of nominal loop
**   Revision 1.5 2010/11/17 17:05:08EST aschoeni 
**   minor fixes for CPU utilization
**   Revision 1.4 2010/10/14 17:45:57EDT aschoeni 
**   Removed assumptions of rate of utilization measurement
**   Revision 1.3 2010/10/14 15:32:40EDT aschoeni 
**   Add skipping of 'first' match for calibration to avoid 0xFFFFFFFF match
**   Revision 1.2 2010/09/29 18:28:23EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.1 2010/09/13 16:34:37EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**   Revision 1.1 2010/07/19 11:49:54EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/MMS-REPOSITORY/dev/eeprom-fsw/apps/hs/fsw/src/project.pj
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_psp.h"
#include "osapi.h"
#include "hs_app.h"
#include "hs_cmds.h"
#include "hs_msg.h"
#include "hs_custom.h"
#include "hs_events.h"
#include "hs_monitors.h"
#include "hs_perfids.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Idle Task Main Process Loop                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HS_IdleTask(void)
{
    OS_time_t PSPTime = {0,0};

    HS_CustomData.IdleTaskRunStatus = CFE_ES_RegisterChildTask();

    while (HS_CustomData.IdleTaskRunStatus == CFE_SUCCESS)
    {

        /* Check to see if we are to mark the time. */
        if(((HS_CustomData.ThisIdleTaskExec & HS_CustomData.UtilMask) == HS_CustomData.UtilMask) &&
           (HS_CustomData.ThisIdleTaskExec > HS_CustomData.UtilMask))
        {
            /* Entry and Exit markers are for easy time marking only; not performance */
            CFE_ES_PerfLogEntry(HS_IDLETASK_PERF_ID);

            /* Increment the child task Execution Counter */
            CFE_ES_IncrementTaskCounter();

            /* update stamp and array */
            CFE_PSP_GetTime(&PSPTime);
            HS_CustomData.UtilArray[HS_CustomData.UtilArrayIndex & HS_CustomData.UtilArrayMask] = (uint32) PSPTime.microsecs;
            HS_CustomData.UtilArrayIndex++;

            CFE_ES_PerfLogExit(HS_IDLETASK_PERF_ID);
        }

        /* Call the Utilization Tracking function */
        HS_UtilizationIncrement();

    }

    /*
    ** If the run status is externally set to something else
    */
    return;

} /* End of HS_IdleTask() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize The Idle Task                                        */
/*                                                                 */
/* NOTE: For complete prolog information, see 'hs_custom.h'        */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_CustomInit(void)
{
    int32 Status = CFE_SUCCESS;

    /*
    ** Spawn the Idle Task
    */
    Status = CFE_ES_CreateChildTask(&HS_CustomData.IdleTaskID,      
                                     HS_IDLE_TASK_NAME,       
                                     HS_IdleTask,         
                                     HS_IDLE_TASK_STACK_PTR,  
                                     HS_IDLE_TASK_STACK_SIZE, 
                                     HS_IDLE_TASK_PRIORITY,   
                                     HS_IDLE_TASK_FLAGS);     
     
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_CR_CHILD_TASK_ERR_EID, CFE_EVS_ERROR,
                          "Error Creating Child Task for CPU Utilization Monitoring,RC=0x%08X",
                          (unsigned int)Status);
        return (Status);
    }

    /*
    ** Connect to CFE TIME's time reference marker (typically 1 Hz) for Idle Task Marking
    */
    Status = CFE_TIME_RegisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)&HS_MarkIdleCallback);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_CR_SYNC_CALLBACK_ERR_EID, CFE_EVS_ERROR,
                          "Error Registering Sync Callback for CPU Utilization Monitoring,RC=0x%08X",
                          (unsigned int)Status);
    }

    HS_CustomData.UtilMult1 = HS_UTIL_CONV_MULT1;
    HS_CustomData.UtilMult2 = HS_UTIL_CONV_MULT2;
    HS_CustomData.UtilDiv   = HS_UTIL_CONV_DIV;
    HS_CustomData.UtilMask  = HS_UTIL_DIAG_MASK;
    HS_CustomData.UtilCycleCounter = 0;
    HS_CustomData.UtilArrayIndex = 0;
    HS_CustomData.UtilArrayMask = HS_UTIL_TIME_DIAG_ARRAY_MASK;
    HS_CustomData.ThisIdleTaskExec = 0;
    HS_CustomData.LastIdleTaskExec = 0;
    HS_CustomData.LastIdleTaskInterval = 0;

    return(Status);

} /* end HS_CustomInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Terminate The Idle Task                                         */
/*                                                                 */
/* NOTE: For complete prolog information, see 'hs_custom.h'        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_CustomCleanup(void)
{
    /*
    ** Unregister the Sync Callback for CPU Monitoring
    */
    CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)&HS_MarkIdleCallback);

    /*
    ** Force the Idle Task to stop running
    */
    HS_CustomData.IdleTaskRunStatus = !CFE_SUCCESS;

    /*
    ** Delete the Idle Task
    */
    CFE_ES_DeleteChildTask(HS_CustomData.IdleTaskID);

} /* end HS_CustomCleanup */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Increment the Utilization Counter                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_UtilizationIncrement(void)
{
    HS_CustomData.ThisIdleTaskExec++;

    return;

} /* end HS_UtilizationIncrement */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Mark the Utilization Counter                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_UtilizationMark(void)
{
    static uint32 CycleCount = 0;

    CycleCount++;

    if(CycleCount >= HS_UTIL_CALLS_PER_MARK)
    {
       HS_CustomData.LastIdleTaskInterval = HS_CustomData.ThisIdleTaskExec - HS_CustomData.LastIdleTaskExec;
       HS_CustomData.LastIdleTaskExec = HS_CustomData.ThisIdleTaskExec;
       CycleCount = 0;
    }

    return;

} /* end HS_UtilizationMark */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Callback function that marks the Idle time                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_MarkIdleCallback(void)
{
    /*
    ** Capture the CPU Utilization (at a consistant time)
    */
    HS_UtilizationMark();

    return;

} /* End of HS_MarkIdleCallback() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Performs Utilization Monitoring and reporting                   */
/*                                                                 */
/* NOTE: For complete prolog information, see 'hs_custom.h'        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_CustomMonitorUtilization(void)
{
    HS_CustomData.UtilCycleCounter++;
    if (HS_CustomData.UtilCycleCounter >= HS_UTIL_CYCLES_PER_INTERVAL)
    {
        HS_MonitorUtilization();
        HS_CustomData.UtilCycleCounter = 0;
    }

} /* End of HS_CustomMonitorUtilization() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Processes any additional commands                               */
/*                                                                 */
/* NOTE: For complete prolog information, see 'hs_custom.h'        */
/*                                                                 */
/* This function MUST return CFE_SUCCESS if any command is found   */
/* and must NOT return CFE_SUCCESS if no command was found         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_CustomCommands(CFE_SB_MsgPtr_t MessagePtr)
{
    int32 Status = CFE_SUCCESS;

    uint16          CommandCode = 0;

    CommandCode = CFE_SB_GetCmdCode(MessagePtr);
    switch (CommandCode)
    {
        case HS_REPORT_DIAG_CC:
            HS_UtilDiagReport();
            break;

        case HS_SET_UTIL_PARAMS_CC:
            HS_SetUtilParamsCmd(MessagePtr);
            break;

        case HS_SET_UTIL_DIAG_CC:
            HS_SetUtilDiagCmd(MessagePtr);
            break;

        default:
            Status = !CFE_SUCCESS;
            break;

    } /* end CommandCode switch */

    return Status;

} /* End of HS_CustomCommands() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Report Utilization Diagnostics Information                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_UtilDiagReport(void)
{
    uint32 DiagValue[HS_UTIL_TIME_DIAG_ARRAY_LENGTH];
    uint32 DiagCount[HS_UTIL_TIME_DIAG_ARRAY_LENGTH];
    uint32 i = 0;
    uint32 j = 0;
    uint32 ThisValue = 0;
    boolean MatchFound = FALSE;
    
    uint32 Ordinal = 0;
    uint32 NewOrdinalIndex = 0;
    uint32 OutputValue[HS_UTIL_DIAG_REPORTS];
    uint32 OutputCount[HS_UTIL_DIAG_REPORTS];
    uint32 OutputOrdinal[HS_UTIL_DIAG_REPORTS];

    /* Initialize the arrays */
    for(i=0; i < HS_UTIL_TIME_DIAG_ARRAY_LENGTH; i++)
    {
        DiagValue[i] = 0xFFFFFFFF;
        DiagCount[i] = 0;
    }
    for(i=0; i < HS_UTIL_DIAG_REPORTS; i++)
    {
        OutputValue[i] = 0xFFFFFFFF;
        OutputCount[i] = 0;
        OutputOrdinal[i] = i;
    }

    /* Populate the arrays */
    for(i = 0; i < HS_UTIL_TIME_DIAG_ARRAY_LENGTH; i++)
    {
        if(i == 0)
        {
            ThisValue = HS_CustomData.UtilArray[i] - HS_CustomData.UtilArray[HS_UTIL_TIME_DIAG_ARRAY_LENGTH - 1];
        }
        else
        {
            ThisValue = HS_CustomData.UtilArray[i] - HS_CustomData.UtilArray[i - 1];
        }
        
        j = 0;
        MatchFound = FALSE;
        while((MatchFound == FALSE) && (j < HS_UTIL_TIME_DIAG_ARRAY_LENGTH))
        {
            if(ThisValue == DiagValue[j])
            {
                DiagCount[j]++;
                MatchFound = TRUE;
            }
            else if (DiagValue[j] == 0xFFFFFFFF)
            {
                DiagValue[j] = ThisValue;
                DiagCount[j]++;
                MatchFound = TRUE;
            }
            else
            {
                j++;
            }
        }

    }

    /* Calculate the lowest time jumps */
    i = 0;
    while((DiagValue[i] != 0xFFFFFFFF) && (i < HS_UTIL_TIME_DIAG_ARRAY_LENGTH))
    {
        Ordinal = 0;
        for(j = 0; j < HS_UTIL_DIAG_REPORTS; j++)
        {
            if (OutputValue[j] < DiagValue[i])
            {
                Ordinal++;
            }
        }
        if(Ordinal < HS_UTIL_DIAG_REPORTS)
        {
            /* Take over the outputs occupied by the current last */
            NewOrdinalIndex = OutputOrdinal[HS_UTIL_DIAG_REPORTS - 1];
            OutputValue[NewOrdinalIndex] = DiagValue[i];
            OutputCount[NewOrdinalIndex] = DiagCount[i];
            
            /* Slide everything else down */
            for(j = HS_UTIL_DIAG_REPORTS - 1; j > Ordinal; j--)
            {
                 OutputOrdinal[j] = OutputOrdinal[j - 1];
            }

            /* Point to the new output */
            OutputOrdinal[Ordinal] = NewOrdinalIndex;

        }

        i++;
    }
    
    /* Output the HS_UTIL_DIAG_REPORTS as en event */
    CFE_EVS_SendEvent(HS_UTIL_DIAG_REPORT_EID, CFE_EVS_INFORMATION,
                      "Mask 0x%08X Base Time Ticks per Idle Ticks (frequency): %i(%i), %i(%i), %i(%i), %i(%i)", (unsigned int)HS_CustomData.UtilMask,
                       (int)OutputValue[OutputOrdinal[0]], (int)OutputCount[OutputOrdinal[0]], (int)OutputValue[OutputOrdinal[1]], (int)OutputCount[OutputOrdinal[1]], 
                       (int)OutputValue[OutputOrdinal[2]], (int)OutputCount[OutputOrdinal[2]], (int)OutputValue[OutputOrdinal[3]], (int)OutputCount[OutputOrdinal[3]]);

    return;

} /* end HS_UtilDiagReport */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Report Utilization                                              */
/*                                                                 */
/* NOTE: For complete prolog information, see 'hs_custom.h'        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_CustomGetUtil(void)
{
   int32 CurrentUtil = 0;

   if (HS_CustomData.UtilDiv != 0)
   {
      CurrentUtil = HS_UTIL_PER_INTERVAL_TOTAL - (((HS_CustomData.LastIdleTaskInterval * HS_CustomData.UtilMult1) 
                                                  / HS_CustomData.UtilDiv) 
                                                 * HS_CustomData.UtilMult2);
   }

   return CurrentUtil;
}

void HS_SetUtilParamsCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_SetUtilParamsCmd_t);
    HS_SetUtilParamsCmd_t  *CmdPtr;

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        CmdPtr = ((HS_SetUtilParamsCmd_t *)MessagePtr);

        if((CmdPtr->Mult1 != 0) &&
           (CmdPtr->Mult2 != 0) &&
           (CmdPtr->Div != 0))
        {
            HS_CustomData.UtilMult1 = CmdPtr->Mult1;
            HS_CustomData.UtilMult2 = CmdPtr->Mult2;
            HS_CustomData.UtilDiv   = CmdPtr->Div;
            HS_AppData.CmdCount++;
            CFE_EVS_SendEvent (HS_SET_UTIL_PARAMS_DBG_EID, CFE_EVS_DEBUG,
                               "Utilization Parms set: Mult1: %d Div: %d Mult2: %d", 
                               (int)HS_CustomData.UtilMult1, (int)HS_CustomData.UtilDiv, (int)HS_CustomData.UtilMult2);
        }
        else
        {
            HS_AppData.CmdErrCount++;
            CFE_EVS_SendEvent (HS_SET_UTIL_PARAMS_ERR_EID, CFE_EVS_ERROR,
                               "Utilization Parms Error: No parameter may be 0: Mult1: %d Div: %d Mult2: %d", 
                               (int)CmdPtr->Mult1, (int)CmdPtr->Div, (int)CmdPtr->Mult2);
        }
    }

    return;

} /* end HS_SetUtilParamsCmd */

void HS_SetUtilDiagCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_SetUtilDiagCmd_t);
    HS_SetUtilDiagCmd_t  *CmdPtr;

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        CmdPtr = ((HS_SetUtilDiagCmd_t *)MessagePtr);

        HS_CustomData.UtilMask  = CmdPtr->Mask;
        CFE_EVS_SendEvent (HS_SET_UTIL_DIAG_DBG_EID, CFE_EVS_DEBUG,
                           "Utilization Diagnostics Mask has been set to %08X", 
                           (unsigned int)HS_CustomData.UtilMask);
    }

    return;

} /* end HS_SetUtilDiagCmd */

/************************/
/*  End of File Comment */
/************************/
