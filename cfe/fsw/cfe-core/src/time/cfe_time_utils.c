/*
** $Id: cfe_time_utils.c 1.10 2012/10/01 16:37:48GMT-05:00 aschoeni Exp  $
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
** 
**
**
**
** Purpose:  cFE Time Services (TIME) library utilities source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
** $Log: cfe_time_utils.c  $
** Revision 1.10 2012/10/01 16:37:48GMT-05:00 aschoeni 
** removed relative path in include (handle in makefile)
** Revision 1.9 2012/01/13 12:21:36EST acudmore 
** Changed license text to reflect open source
** Revision 1.8 2011/12/07 19:20:04EST aschoeni 
** Removed returns for TIME and SB for cleaning up apps
** Revision 1.7 2011/11/30 15:10:06EST jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.6 2011/01/18 16:05:51EST lwalling 
** Make sending 1hz command packet a configuration option
** Revision 1.5 2010/10/25 15:00:06EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 15:16:16EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/06/10 09:23:10EDT acudmore 
** Changed OS_Mem* and OS_BSP* calls to CFE_PSP_* calls
** Revision 1.2 2008/08/06 22:42:26EDT dkobe 
** Added CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:39EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/time/project.pj
** Revision 1.16 2007/04/27 12:31:06EDT njyanchik 
** I changed EVS and TIME to not have a telemetry counter for SB pipe reads in the main loop. 
** Instead, I have added a syslog message to state the problem
** Revision 1.15 2007/04/24 08:59:05EDT njyanchik 
** I added the changes tha LRO requested. They are specified in the DCR text.
** Revision 1.14 2007/04/11 11:02:10EDT njyanchik 
** Remove the TaskDelay in the main loop for TIME. I also addeda counterin telemtry that will
** notify the ground of an error in reading the command pipe.
** Revision 1.13 2007/04/04 08:14:54EDT njyanchik 
** In DCR 3190, two API's were changed in the OSAL for locking out interrupts. This change package
** changes TIME's and ES's use of those APIs
** Revision 1.12 2007/03/15 11:16:47EST njyanchik 
** I changed the interrupt enable/disable pair to use a lock key that records the previous state
** of the interrupts before disabling, and then use that key to re-enable the interrupts.
** The CFE core applications that use this pair were also fixed for this API change.
** Revision 1.11 2006/11/01 11:33:20EST njyanchik 
** This change removes any references to the CDS in TIME. TIME now uses  the ES
** Reset Area to store its critial variables. this was due to an issue of trying to call
** CDS functions before apps were registered (in the EarlyInit call).
** Revision 1.10 2006/10/30 14:09:29GMT-05:00 njyanchik 
** I changed TIME to use the new ES_CDS implementation. This involved using the 
** new functions, as well as modifying the CFE_TIME_TaskData structure as well as
** the CFE_TIME_DiagPacket structure. They both made reference to the address
** of the TIME CDS (the old implementation). Now they both use the new
** CFE_ES_CDSHandle_t. Also, the Unit Test for Time was updated. No new paths
** through the code were created, but since return codes from the CDS functions
** changed, there needed to be updates to the UT.
** Revision 1.9 2006/10/30 12:36:01GMT-05:00 dlkobe 
*/

/*
** Required header files...
*/
#include "cfe_time_utils.h"

#include "private/cfe_es_resetdata_typedef.h"

#include <string.h>



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_LatchClock() -- query local clock                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t CFE_TIME_LatchClock(void)
{
    CFE_TIME_SysTime_t LatchTime;
    OS_time_t LocalTime;

    /*
    ** Get time in O/S format (seconds : microseconds)...
    */
    CFE_PSP_GetTime(&LocalTime);

    /*
    ** Convert time to cFE format (seconds : 1/2^32 subseconds)...
    */
    LatchTime.Seconds = LocalTime.seconds;
    LatchTime.Subseconds = CFE_TIME_Micro2SubSecs(LocalTime.microsecs);

    return(LatchTime);

} /* End of CFE_TIME_LatchClock() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_QueryResetVars() -- query contents of Reset Variables  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_QueryResetVars(void)
{

    CFE_TIME_ResetVars_t LocalResetVars;
    uint32 DefSubsMET;
    uint32 DefSubsSTCF;
    int32 status;
    
    uint32 resetAreaSize;
    cpuaddr resetAreaAddr;
    CFE_ES_ResetData_t  *CFE_TIME_ResetDataPtr;
   
    /*
    ** Get the pointer to the Reset area from the BSP
    */
    status = CFE_PSP_GetResetArea (&(resetAreaAddr), &(resetAreaSize));
      
    if (status != CFE_PSP_SUCCESS)
    {
        /* There is something wrong with the Reset Area */
        CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_BAD;
    }
    
    else
    {
        CFE_TIME_ResetDataPtr = (CFE_ES_ResetData_t *)resetAreaAddr;
        
        /* Get the structure from the Reset Area */
        LocalResetVars = CFE_TIME_ResetDataPtr -> TimeResetVars;

        /*
        ** Verify TIME data signature and clock signal selection...
        **    (other data fields have no verifiable limits)
        */
        if ((LocalResetVars.Signature == CFE_TIME_RESET_SIGNATURE) &&
            ((LocalResetVars.ClockSignal == CFE_TIME_TONE_PRI) ||
            (LocalResetVars.ClockSignal == CFE_TIME_TONE_RED)))
        {
            /*
            ** Initialize TIME to valid  Reset Area values...
            */
            CFE_TIME_TaskData.AtToneMET    = LocalResetVars.CurrentMET;
            CFE_TIME_TaskData.AtToneSTCF   = LocalResetVars.CurrentSTCF;
            CFE_TIME_TaskData.AtToneDelay  = LocalResetVars.CurrentDelay;
            CFE_TIME_TaskData.AtToneLeaps  = LocalResetVars.LeapSeconds;
            CFE_TIME_TaskData.ClockSignal  = LocalResetVars.ClockSignal;

            CFE_TIME_TaskData.DataStoreStatus  = CFE_TIME_RESET_AREA_EXISTING;
        }    
        else
        {   
            /*
            ** We got a blank area from the reset variables
            */
            CFE_TIME_TaskData.DataStoreStatus  = CFE_TIME_RESET_AREA_NEW;
        }

    }
    /*
    ** Initialize TIME to default values if no valid Reset data...
    */
    if (CFE_TIME_TaskData.DataStoreStatus != CFE_TIME_RESET_AREA_EXISTING)
    {
        DefSubsMET  = CFE_TIME_Micro2SubSecs(CFE_TIME_DEF_MET_SUBS);
        DefSubsSTCF = CFE_TIME_Micro2SubSecs(CFE_TIME_DEF_STCF_SUBS);

        CFE_TIME_TaskData.AtToneMET.Seconds      = CFE_TIME_DEF_MET_SECS;
        CFE_TIME_TaskData.AtToneMET.Subseconds   = DefSubsMET;
        CFE_TIME_TaskData.AtToneSTCF.Seconds     = CFE_TIME_DEF_STCF_SECS;
        CFE_TIME_TaskData.AtToneSTCF.Subseconds  = DefSubsSTCF;
        CFE_TIME_TaskData.AtToneLeaps            = CFE_TIME_DEF_LEAPS;
        CFE_TIME_TaskData.ClockSignal            = CFE_TIME_TONE_PRI;
        CFE_TIME_TaskData.AtToneDelay.Seconds    = 0;
        CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    }
    
    return;

} /* End of CFE_TIME_QueryResetVars() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_UpdateResetVars() -- update contents of Reset Variables*/
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_UpdateResetVars(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_ResetVars_t LocalResetVars;
    uint32 resetAreaSize;
    cpuaddr resetAreaAddr;
    CFE_ES_ResetData_t  *CFE_TIME_ResetDataPtr;
    /*
    ** Update the data only if our Reset Area is valid...
    */
    if (CFE_TIME_TaskData.DataStoreStatus != CFE_TIME_RESET_AREA_ERROR)
    {
   
        /* Store all of our critical variables to a ResetVars_t
         * then copy that to the Reset Area */
        LocalResetVars.Signature = CFE_TIME_RESET_SIGNATURE;

        LocalResetVars.CurrentMET   = Reference->CurrentMET;
        LocalResetVars.CurrentSTCF  = Reference->AtToneSTCF;
        LocalResetVars.CurrentDelay = Reference->AtToneDelay;
        LocalResetVars.LeapSeconds  = Reference->AtToneLeaps;

        LocalResetVars.ClockSignal  = CFE_TIME_TaskData.ClockSignal;
   
        /*
        ** Get the pointer to the Reset area from the BSP
        */
        if (CFE_PSP_GetResetArea (&(resetAreaAddr), &(resetAreaSize)) == CFE_PSP_SUCCESS)
        {
           CFE_TIME_ResetDataPtr = (CFE_ES_ResetData_t *)resetAreaAddr;
           CFE_TIME_ResetDataPtr -> TimeResetVars = LocalResetVars;
        }
    }

    return;

} /* End of CFE_TIME_UpdateResetVars() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_InitData() -- initialize global time task data         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_InitData(void)
{
    uint32  i = 0;
    
    /*
    ** Initialize task command execution counters...
    */
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;

    /*
    ** Initialize task configuration data...
    */
    strcpy(CFE_TIME_TaskData.PipeName, CFE_TIME_TASK_PIPE_NAME);
    CFE_TIME_TaskData.PipeDepth = CFE_TIME_TASK_PIPE_DEPTH;
    
    /*
    ** Try to get values used to compute time from Reset Area...
    */
    CFE_TIME_QueryResetVars();

    /*
    ** Remaining data values used to compute time...
    */
    CFE_TIME_TaskData.AtToneLatch = CFE_TIME_LatchClock();

    /*
    ** Data values used to define the current clock state...
    */
    CFE_TIME_TaskData.ClockSetState  = CFE_TIME_NOT_SET;
    CFE_TIME_TaskData.ClockFlyState  = CFE_TIME_IS_FLY;

#if (CFE_TIME_CFG_SOURCE == TRUE)
    CFE_TIME_TaskData.ClockSource    = CFE_TIME_USE_EXTERN;
#else
    CFE_TIME_TaskData.ClockSource    = CFE_TIME_USE_INTERN;
#endif
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;

    /*
    ** Pending data values (from "time at tone" command data packet)...
    */
    CFE_TIME_TaskData.PendingMET.Seconds     = 0;
    CFE_TIME_TaskData.PendingMET.Subseconds  = 0;
    CFE_TIME_TaskData.PendingSTCF.Seconds    = 0;
    CFE_TIME_TaskData.PendingSTCF.Subseconds = 0;
    CFE_TIME_TaskData.PendingLeaps           = 0;
    CFE_TIME_TaskData.PendingState           = CFE_TIME_INVALID;

    /*
    ** STCF adjustment values...
    */
    CFE_TIME_TaskData.OneTimeAdjust.Seconds    = 0;
    CFE_TIME_TaskData.OneTimeAdjust.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds      = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds   = 0;

    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.OneHzDirection   = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.DelayDirection   = CFE_TIME_ADD_ADJUST;

    /*
    ** Local clock latch values...
    */
    CFE_TIME_TaskData.ToneSignalLatch.Seconds    = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_TaskData.ToneDataLatch.Seconds      = 0;
    CFE_TIME_TaskData.ToneDataLatch.Subseconds   = 0;

    /*
    ** Miscellaneous counters...
    */
    CFE_TIME_TaskData.ToneMatchCount  = 0;
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_TaskData.ToneSignalCount = 0;
    CFE_TIME_TaskData.ToneDataCount   = 0;
    CFE_TIME_TaskData.ToneIntCount    = 0;
    CFE_TIME_TaskData.ToneIntErrors   = 0;
    CFE_TIME_TaskData.ToneTaskCount   = 0;
    CFE_TIME_TaskData.VirtualMET      = CFE_TIME_TaskData.AtToneMET.Seconds;
    CFE_TIME_TaskData.VersionCount    = 0;
    CFE_TIME_TaskData.LocalIntCount   = 0;
    CFE_TIME_TaskData.LocalTaskCount  = 0;
    CFE_TIME_TaskData.InternalCount   = 0;
    CFE_TIME_TaskData.ExternalCount   = 0;

    /*
    ** Time window verification values...
    */
    CFE_TIME_TaskData.MinElapsed = CFE_TIME_Micro2SubSecs(CFE_TIME_MIN_ELAPSED);
    CFE_TIME_TaskData.MaxElapsed = CFE_TIME_Micro2SubSecs(CFE_TIME_MAX_ELAPSED);

    /*
    ** Range checking for external time source data...
    */
    #if (CFE_TIME_CFG_SOURCE == TRUE)
    CFE_TIME_TaskData.MaxDelta.Seconds    = CFE_TIME_MAX_DELTA_SECS;
    CFE_TIME_TaskData.MaxDelta.Subseconds = CFE_TIME_Micro2SubSecs(CFE_TIME_MAX_DELTA_SUBS);
    #else
    CFE_TIME_TaskData.MaxDelta.Seconds    = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 0;
    #endif

    /*
    ** Maximum local clock value (before roll-over)...
    */
    CFE_TIME_TaskData.MaxLocalClock.Seconds    = CFE_TIME_MAX_LOCAL_SECS;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = CFE_TIME_MAX_LOCAL_SUBS;

    /*
    ** Range limits for time between tone signal interrupts...
    */
    CFE_TIME_TaskData.ToneOverLimit  = CFE_TIME_Micro2SubSecs(CFE_TIME_CFG_TONE_LIMIT);
    CFE_TIME_TaskData.ToneUnderLimit = CFE_TIME_Micro2SubSecs((1000000 - CFE_TIME_CFG_TONE_LIMIT));

    /*
    ** Clock state has been commanded into (CFE_TIME_FLYWHEEL)...
    */
    CFE_TIME_TaskData.Forced2Fly = FALSE;

    /*
    ** Clock state has just transitioned into (CFE_TIME_FLYWHEEL)...
    */
    CFE_TIME_TaskData.AutoStartFly = FALSE;
    
    /*
    ** Clear the Synch Callback Registry of any garbage
    */
    for (i=0; i<CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
        CFE_TIME_TaskData.SynchCallback[i].App = 0;
    }

    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_SB_InitMsg(&CFE_TIME_TaskData.HkPacket,
                    CFE_TIME_HK_TLM_MID,
                    sizeof(CFE_TIME_HkPacket_t), TRUE);

    /*
    ** Initialize diagnostic packet (clear user data area)...
    */
    CFE_SB_InitMsg(&CFE_TIME_TaskData.DiagPacket,
                    CFE_TIME_DIAG_TLM_MID,
                    sizeof(CFE_TIME_DiagPacket_t), TRUE);

    /*
    ** Initialize "time at the tone" signal command packet...
    */
    CFE_SB_InitMsg(&CFE_TIME_TaskData.ToneSignalCmd,
                    CFE_TIME_TONE_CMD_MID,
                    sizeof(CFE_TIME_ToneSignalCmd_t), TRUE);

    /*
    ** Initialize "time at the tone" data command packet...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
    CFE_SB_InitMsg(&CFE_TIME_TaskData.ToneDataCmd,
                    CFE_TIME_DATA_CMD_MID,
                    sizeof(CFE_TIME_ToneDataCmd_t), TRUE);
    #endif

    /*
    ** Initialize simulated tone signal (time server only)...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
      #if (CFE_TIME_CFG_FAKE_TONE == TRUE)
    CFE_SB_InitMsg(&CFE_TIME_TaskData.FakeToneCmd,
                    CFE_TIME_FAKE_CMD_MID,
                    sizeof(CFE_TIME_FakeToneCmd_t), TRUE);
      #endif
    #endif

    /*
    ** Initialize local 1Hz "wake-up" command packet (optional)...
    */
    #if (CFE_TIME_ENA_1HZ_CMD_PKT == TRUE)
    CFE_SB_InitMsg(&CFE_TIME_TaskData.Local1HzCmd,
                    CFE_TIME_1HZ_CMD_MID,
                    sizeof(CFE_TIME_1HzCmd_t), TRUE);
    #endif

    return;

} /* End of CFE_TIME_InitData() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_GetStateFlags() -- Convert state data to flag values   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint16 CFE_TIME_GetStateFlags(void)
{
    uint16 StateFlags = 0;

    /*
    ** Spacecraft time has been set...
    */
    if (CFE_TIME_TaskData.ClockSetState == CFE_TIME_WAS_SET)
    {
        StateFlags |= CFE_TIME_FLAG_CLKSET;
    }
    /*
    ** This instance of Time Service is in FLYWHEEL mode...
    */
    if (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_IS_FLY)
    {
        StateFlags |= CFE_TIME_FLAG_FLYING;
    }
    /*
    ** Clock source set to "internal"...
    */
    if (CFE_TIME_TaskData.ClockSource == CFE_TIME_USE_INTERN)
    {
        StateFlags |= CFE_TIME_FLAG_SRCINT;
    }
    /*
    ** Clock signal set to "primary"...
    */
    if (CFE_TIME_TaskData.ClockSignal == CFE_TIME_TONE_PRI)
    {
        StateFlags |= CFE_TIME_FLAG_SIGPRI;
    }
    /*
    ** Time Server is in FLYWHEEL mode...
    */
    if (CFE_TIME_TaskData.ServerFlyState == CFE_TIME_IS_FLY)
    {
        StateFlags |= CFE_TIME_FLAG_SRVFLY;
    }
    /*
    ** This instance of Time Services commanded into FLYWHEEL...
    */
    if (CFE_TIME_TaskData.Forced2Fly)
    {
        StateFlags |= CFE_TIME_FLAG_CMDFLY;
    }
    /*
    ** One time STCF adjustment direction...
    */
    if (CFE_TIME_TaskData.OneTimeDirection == CFE_TIME_ADD_ADJUST)
    {
        StateFlags |= CFE_TIME_FLAG_ADDADJ;
    }
    /*
    ** 1 Hz STCF adjustment direction...
    */
    if (CFE_TIME_TaskData.OneHzDirection == CFE_TIME_ADD_ADJUST)
    {
        StateFlags |= CFE_TIME_FLAG_ADD1HZ;
    }
    /*
    ** Time Client Latency adjustment direction...
    */
    if (CFE_TIME_TaskData.DelayDirection == CFE_TIME_ADD_ADJUST)
    {
        StateFlags |= CFE_TIME_FLAG_ADDTCL;
    }
    /*
    ** This instance of Time Service is a "server"...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
    StateFlags |= CFE_TIME_FLAG_SERVER;
    #endif

    return(StateFlags);

} /* End of CFE_TIME_GetStateFlags() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_GetHkData() -- Report local housekeeping data          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_GetHkData(const CFE_TIME_Reference_t *Reference)
{

    /*
    ** Get command execution counters...
    */
    CFE_TIME_TaskData.HkPacket.Payload.CmdCounter = CFE_TIME_TaskData.CmdCounter;
    CFE_TIME_TaskData.HkPacket.Payload.ErrCounter = CFE_TIME_TaskData.ErrCounter;

    /*
    ** Current "as calculated" clock state...
    */
    CFE_TIME_TaskData.HkPacket.Payload.ClockStateAPI = (int16) CFE_TIME_CalculateState(Reference);

    /*
    ** Current clock state flags...
    */
    CFE_TIME_TaskData.HkPacket.Payload.ClockStateFlags = CFE_TIME_GetStateFlags();

    /*
    ** Leap Seconds...
    */
    CFE_TIME_TaskData.HkPacket.Payload.LeapSeconds = Reference->AtToneLeaps;

    /*
    ** Current MET and STCF time values...
    */
    CFE_TIME_TaskData.HkPacket.Payload.SecondsMET = Reference->CurrentMET.Seconds;
    CFE_TIME_TaskData.HkPacket.Payload.SubsecsMET = Reference->CurrentMET.Subseconds;

    CFE_TIME_TaskData.HkPacket.Payload.SecondsSTCF = Reference->AtToneSTCF.Seconds;
    CFE_TIME_TaskData.HkPacket.Payload.SubsecsSTCF = Reference->AtToneSTCF.Subseconds;

    /*
    ** 1Hz STCF adjustment values (server only)...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
    CFE_TIME_TaskData.HkPacket.Payload.Seconds1HzAdj = CFE_TIME_TaskData.OneHzAdjust.Seconds;
    CFE_TIME_TaskData.HkPacket.Payload.Subsecs1HzAdj = CFE_TIME_TaskData.OneHzAdjust.Subseconds;
    #endif

    /*
    ** Time at tone delay values (client only)...
    */
    #if (CFE_TIME_CFG_CLIENT == TRUE)
    CFE_TIME_TaskData.HkPacket.Payload.SecondsDelay = CFE_TIME_TaskData.AtToneDelay.Seconds;
    CFE_TIME_TaskData.HkPacket.Payload.SubsecsDelay = CFE_TIME_TaskData.AtToneDelay.Subseconds;
    #endif



    return;

} /* End of CFE_TIME_GetHkData() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_GetDiagData() -- Report diagnostics data               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_GetDiagData(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t TempTime;

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.AtToneMET, &Reference.AtToneMET);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.AtToneSTCF, &Reference.AtToneSTCF);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.AtToneDelay, &Reference.AtToneDelay);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.AtToneLatch, &Reference.AtToneLatch);

    CFE_TIME_TaskData.DiagPacket.Payload.AtToneLeaps   = Reference.AtToneLeaps;
    CFE_TIME_TaskData.DiagPacket.Payload.ClockStateAPI = CFE_TIME_CalculateState(&Reference);

    /*
    ** Data values that reflect the time (right now)...
    */
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.TimeSinceTone, &Reference.TimeSinceTone);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.CurrentLatch, &Reference.CurrentLatch);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.CurrentMET, &Reference.CurrentMET);
    TempTime = CFE_TIME_CalculateTAI(&Reference);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.CurrentTAI, &TempTime);
    TempTime = CFE_TIME_CalculateUTC(&Reference);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.CurrentUTC, &TempTime);

    /*
    ** Data values used to define the current clock state...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.ClockSetState  = Reference.ClockSetState;
    CFE_TIME_TaskData.DiagPacket.Payload.ClockFlyState  = Reference.ClockFlyState;
    CFE_TIME_TaskData.DiagPacket.Payload.ClockSource    = CFE_TIME_TaskData.ClockSource;
    CFE_TIME_TaskData.DiagPacket.Payload.ClockSignal    = CFE_TIME_TaskData.ClockSignal;
    CFE_TIME_TaskData.DiagPacket.Payload.ServerFlyState = CFE_TIME_TaskData.ServerFlyState;
    CFE_TIME_TaskData.DiagPacket.Payload.Forced2Fly     = (int16) CFE_TIME_TaskData.Forced2Fly;

    /*
    ** Clock state flags...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.ClockStateFlags = CFE_TIME_GetStateFlags();

    /*
    ** STCF adjustment direction values...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.OneTimeDirection = CFE_TIME_TaskData.OneTimeDirection;
    CFE_TIME_TaskData.DiagPacket.Payload.OneHzDirection   = CFE_TIME_TaskData.OneHzDirection;
    CFE_TIME_TaskData.DiagPacket.Payload.DelayDirection   = CFE_TIME_TaskData.DelayDirection;

    /*
    ** STCF adjustment values...
    */
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.OneTimeAdjust, &CFE_TIME_TaskData.OneTimeAdjust);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.OneHzAdjust, &CFE_TIME_TaskData.OneHzAdjust);

    /*
    ** Most recent local clock latch values...
    */
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.ToneSignalLatch, &CFE_TIME_TaskData.ToneSignalLatch);
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.ToneDataLatch, &CFE_TIME_TaskData.ToneDataLatch);

    /*
    ** Miscellaneous counters (subject to reset command)...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.ToneMatchCount  = CFE_TIME_TaskData.ToneMatchCount;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneMatchErrors = CFE_TIME_TaskData.ToneMatchErrors;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneSignalCount = CFE_TIME_TaskData.ToneSignalCount;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneDataCount   = CFE_TIME_TaskData.ToneDataCount;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneIntCount    = CFE_TIME_TaskData.ToneIntCount;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneIntErrors   = CFE_TIME_TaskData.ToneIntErrors;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneTaskCount   = CFE_TIME_TaskData.ToneTaskCount;
    CFE_TIME_TaskData.DiagPacket.Payload.VersionCount    = CFE_TIME_TaskData.VersionCount;
    CFE_TIME_TaskData.DiagPacket.Payload.LocalIntCount   = CFE_TIME_TaskData.LocalIntCount;
    CFE_TIME_TaskData.DiagPacket.Payload.LocalTaskCount  = CFE_TIME_TaskData.LocalTaskCount;

    /*
    ** Miscellaneous counters (not subject to reset command)...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.VirtualMET = CFE_TIME_TaskData.VirtualMET;

    /*
    ** Time window verification values (converted from micro-secs)...
    **
    ** Regardless of whether the tone follows the time packet, or vice
    **    versa, these values define the acceptable window of time for
    **    the second event to follow the first.  The minimum value may
    **    be as little as zero, and the maximum must be something less
    **    than a second.
    */
    CFE_TIME_TaskData.DiagPacket.Payload.MinElapsed = CFE_TIME_TaskData.MinElapsed;
    CFE_TIME_TaskData.DiagPacket.Payload.MaxElapsed = CFE_TIME_TaskData.MaxElapsed;

    /*
    ** Maximum local clock value (before roll-over)...
    */
    CFE_TIME_Copy(&CFE_TIME_TaskData.DiagPacket.Payload.MaxLocalClock, &CFE_TIME_TaskData.MaxLocalClock);

    /*
    ** Tone signal tolerance limits...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.ToneOverLimit  = CFE_TIME_TaskData.ToneOverLimit;
    CFE_TIME_TaskData.DiagPacket.Payload.ToneUnderLimit = CFE_TIME_TaskData.ToneUnderLimit;

    /*
    ** Reset Area access status...
    */
    CFE_TIME_TaskData.DiagPacket.Payload.DataStoreStatus  = CFE_TIME_TaskData.DataStoreStatus;

    return;

} /* End of CFE_TIME_GetDiagData() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_GetReference() -- get reference data (time at "tone")  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_GetReference(CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeSinceTone;
    CFE_TIME_SysTime_t CurrentMET;
    uint32 VersionCount;

    /*
    ** VersionCount is incremented when reference data is modified...
    */
    do
    {
        VersionCount = CFE_TIME_TaskData.VersionCount;

        Reference->CurrentLatch = CFE_TIME_LatchClock();

        Reference->AtToneMET    = CFE_TIME_TaskData.AtToneMET;
        Reference->AtToneSTCF   = CFE_TIME_TaskData.AtToneSTCF;
        Reference->AtToneLeaps  = CFE_TIME_TaskData.AtToneLeaps;
        Reference->AtToneDelay  = CFE_TIME_TaskData.AtToneDelay;
        Reference->AtToneLatch  = CFE_TIME_TaskData.AtToneLatch;

        Reference->ClockSetState  = CFE_TIME_TaskData.ClockSetState;
        Reference->ClockFlyState  = CFE_TIME_TaskData.ClockFlyState;

    } while (VersionCount != CFE_TIME_TaskData.VersionCount);

    /*
    ** Compute the amount of time "since" the tone...
    */
    if (CFE_TIME_Compare(Reference->CurrentLatch, Reference->AtToneLatch) == CFE_TIME_A_LT_B)
    {
        /*
        ** Local clock has rolled over since last tone...
        */
        TimeSinceTone = CFE_TIME_Subtract(CFE_TIME_TaskData.MaxLocalClock, Reference->AtToneLatch);
        TimeSinceTone = CFE_TIME_Add(TimeSinceTone, Reference->CurrentLatch);
    }
    else
    {
        /*
        ** Normal case -- local clock is greater than latch at tone...
        */
        TimeSinceTone = CFE_TIME_Subtract(Reference->CurrentLatch, Reference->AtToneLatch);
    }

    Reference->TimeSinceTone = TimeSinceTone;

    /*
    ** Add in the MET at the tone...
    */
    CurrentMET = CFE_TIME_Add(TimeSinceTone, Reference->AtToneMET);


    /*
    ** Synchronize "this" time client to the time server...
    */
    #if (CFE_TIME_CFG_CLIENT == TRUE)
    if (CFE_TIME_TaskData.DelayDirection == CFE_TIME_ADD_ADJUST)
    {
        CurrentMET = CFE_TIME_Add(CurrentMET, Reference->AtToneDelay);
    }
    else
    {
        CurrentMET = CFE_TIME_Subtract(CurrentMET, Reference->AtToneDelay);
    }
    #endif

    Reference->CurrentMET = CurrentMET;

    return;

} /* End of CFE_TIME_GetReference() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_CalculateTAI() -- calculate TAI from reference data    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t CFE_TIME_CalculateTAI(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeAsTAI;

    TimeAsTAI = CFE_TIME_Add(Reference->CurrentMET, Reference->AtToneSTCF);

    return(TimeAsTAI);

} /* End of CFE_TIME_CalculateTAI() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_CalculateUTC() -- calculate UTC from reference data    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t CFE_TIME_CalculateUTC(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeAsUTC;

    TimeAsUTC = CFE_TIME_Add(Reference->CurrentMET, Reference->AtToneSTCF);
    TimeAsUTC.Seconds -= Reference->AtToneLeaps;

    return(TimeAsUTC);

} /* End of CFE_TIME_CalculateUTC() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_CalculateState() -- determine current time state (per API)     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int16 CFE_TIME_CalculateState(const CFE_TIME_Reference_t *Reference)
{
    int16 ClockState;

    /*
    ** Determine the current clock state...
    */
    if (Reference->ClockSetState == CFE_TIME_WAS_SET)
    {
        if (Reference->ClockFlyState == CFE_TIME_NO_FLY)
        {
            /*
            ** CFE_TIME_VALID = clock set and not fly-wheeling...
            */
            ClockState = CFE_TIME_VALID;

            /*
            ** If the server is fly-wheel then the client must also
            **    report fly-wheel (even if it is not)...
            */
            #if (CFE_TIME_CFG_CLIENT == TRUE)
            if (CFE_TIME_TaskData.ServerFlyState == CFE_TIME_IS_FLY)
            {
                ClockState = CFE_TIME_FLYWHEEL;
            }
            #endif
        }
        else
        {
            /*
            ** CFE_TIME_FLYWHEEL = clock set and fly-wheeling...
            */
            ClockState = CFE_TIME_FLYWHEEL;
        }
    }
    else
    {
        /*
        ** CFE_TIME_INVALID = clock not set...
        */
        ClockState = CFE_TIME_INVALID;
    }


    return(ClockState);

} /* End of CFE_TIME_CalculateState() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetState() -- set clock state                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetState(int16 NewState)
{
    /*
    ** Ensure that change is made without interruption...
    */
    int32 IntFlags;

    IntFlags = OS_IntLock();

    /*
    ** If we get a command to set the clock to "flywheel" mode, then
    **    set a global flag so that we can choose to ignore time
    **    updates until we get another clock state command...
    */
    if (NewState == CFE_TIME_FLYWHEEL)
    {
        CFE_TIME_TaskData.Forced2Fly    = TRUE;
        CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
        #if (CFE_TIME_CFG_SERVER == TRUE)
        CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
        #endif
    }
    else if (NewState == CFE_TIME_VALID)
    {
        CFE_TIME_TaskData.Forced2Fly    = FALSE;
        CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    }
    else
    {
        CFE_TIME_TaskData.Forced2Fly    = FALSE;
        CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;
    }

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetState() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetSource() -- set clock source                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SOURCE == TRUE)
void CFE_TIME_SetSource(int16 NewSource)
{
    int32 IntFlags;
    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    CFE_TIME_TaskData.ClockSource = NewSource;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetSource() */
#endif /* CFE_TIME_CFG_SOURCE */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetSignal() -- set tone signal (pri vs red)            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SIGNAL == TRUE)
void CFE_TIME_SetSignal(int16 NewSignal)
{
    int32 IntFlags; 
    /*
    ** Select primary vs redundant tone interrupt signal...
    */
    OS_SelectTone(NewSignal);

    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    /*
    ** Maintain current tone signal selection for telemetry...
    */
    CFE_TIME_TaskData.ClockSignal = NewSignal;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetSignal() */
#endif /* CFE_TIME_CFG_SIGNAL */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetDelay() -- set tone delay (time client only)        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_CLIENT == TRUE)
void CFE_TIME_SetDelay(CFE_TIME_SysTime_t NewDelay, int16 Direction)
{
    int32 IntFlags;
    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    CFE_TIME_TaskData.AtToneDelay = NewDelay;
    CFE_TIME_TaskData.DelayDirection = Direction;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetDelay() */
#endif /* CFE_TIME_CFG_CLIENT */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetTime() -- set time (time server only)               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_SetTime(CFE_TIME_SysTime_t NewTime)
{
    int32 IntFlags; 
    /*
    ** The input to this function is a time value that includes MET
    **     and STCF.  If the default time format is UTC, the input
    **     time value has had leaps seconds removed from the total.
    */
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t NewSTCF;

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Remove current MET from the new time value (leaves STCF)...
    */
    NewSTCF = CFE_TIME_Subtract(NewTime, Reference.CurrentMET);

    /*
    ** Restore leap seconds if default time format is UTC...
    */
    #if (CFE_TIME_CFG_DEFAULT_UTC == TRUE)
    NewSTCF.Seconds += Reference.AtToneLeaps;
    #endif

    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    CFE_TIME_TaskData.AtToneSTCF = NewSTCF;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetTime() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetMET() -- set MET (time server only)                 */
/*                                                                 */
/* Note: This command will not have lasting effect if configured   */
/*       to get external time of type MET.  Also, there cannot     */
/*       be a local h/w MET and an external MET since both would   */
/*       need to be synchronized to the same tone signal.          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_SetMET(CFE_TIME_SysTime_t NewMET)
{
    int32 IntFlags;
    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    /*
    ** Update reference values used to compute current time...
    */
    CFE_TIME_TaskData.AtToneMET    = NewMET;
    CFE_TIME_TaskData.VirtualMET   = NewMET.Seconds;
    CFE_TIME_TaskData.AtToneLatch  = CFE_TIME_LatchClock();

    /*
    ** Update h/w MET register...
    */
    #if (CFE_TIME_CFG_VIRTUAL != TRUE)
    OS_SetLocalMET(NewMET.Seconds);
    #endif

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetMET() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetSTCF() -- set STCF (time server only)               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_SetSTCF(CFE_TIME_SysTime_t NewSTCF)
{
    int32 IntFlags;
    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    CFE_TIME_TaskData.AtToneSTCF = NewSTCF;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetSTCF() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetLeaps() -- set leap seconds (time server only)      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_SetLeaps(int16 NewLeaps)
{
    int32 IntFlags;
    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    CFE_TIME_TaskData.AtToneLeaps = NewLeaps;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetLeaps() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetAdjust() -- one time STCF adjustment (server only)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_SetAdjust(CFE_TIME_SysTime_t NewAdjust, int16 Direction)
{
    int32 IntFlags;
    CFE_TIME_SysTime_t NewSTCF;

    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    CFE_TIME_TaskData.OneTimeAdjust    = NewAdjust;
    CFE_TIME_TaskData.OneTimeDirection = Direction;

    if (Direction == CFE_TIME_ADD_ADJUST)
    {
        NewSTCF = CFE_TIME_Add(CFE_TIME_TaskData.AtToneSTCF, NewAdjust);
    }
    else
    {
        NewSTCF = CFE_TIME_Subtract(CFE_TIME_TaskData.AtToneSTCF, NewAdjust);
    }

    CFE_TIME_TaskData.AtToneSTCF = NewSTCF;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_SetAdjust() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_Set1HzAdj() -- 1Hz STCF adjustment (time server only)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_Set1HzAdj(CFE_TIME_SysTime_t NewAdjust, int16 Direction)
{
    int32 IntFlags;
    /*
    ** Ensure that change is made without interruption...
    */
    IntFlags = OS_IntLock();

    /*
    ** Store values for 1Hz adjustment...
    */
    CFE_TIME_TaskData.OneHzAdjust     = NewAdjust;
    CFE_TIME_TaskData.OneHzDirection  = Direction;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;
    OS_IntUnlock(IntFlags);

    return;

} /* End of CFE_TIME_Set1HzAdj() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_CleanUpApp() -- Free resources associated with App     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_TIME_CleanUpApp(uint32 AppId)
{
    uint32 i = 0;
    
    while (i<CFE_TIME_MAX_NUM_SYNCH_FUNCS)
    {
        if (CFE_TIME_TaskData.SynchCallback[i].App == AppId)
        {
            CFE_TIME_TaskData.SynchCallback[i].App = 0;
            CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
        }
        
        i++;
    }
    
    return CFE_SUCCESS;
}

/************************/
/*  End of File Comment */
/************************/

