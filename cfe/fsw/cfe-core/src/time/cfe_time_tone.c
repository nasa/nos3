/*
** $Id: cfe_time_tone.c 1.11 2012/02/08 14:32:39GMT-05:00 jmdagost Exp  $
**
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
** Notes:    This module was created from a portion of the source file
**           "cfe_time_utils.c" because that file had grown too large.
**
**           This module contains functions related to the detection
**           and processing of the "time at the tone" event signal.
**
**           This module contains functions related to the detection
**           and processing of the local 1Hz interrupt.
**
** $Log: cfe_time_tone.c  $
** Revision 1.11 2012/02/08 14:32:39GMT-05:00 jmdagost 
** Replaced additional ifdefs with if preprocessor statements (missed in previous attempt).
** Revision 1.10 2012/01/13 12:21:35EST acudmore 
** Changed license text to reflect open source
** Revision 1.9 2011/11/30 15:10:05EST jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.8 2011/01/18 16:05:44EST lwalling 
** Make sending 1hz command packet a configuration option
** Revision 1.7 2010/10/04 17:04:52EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.6 2009/06/11 17:32:03EDT lwalling 
** Handle invalid time data due to unchanging time latch values
** Revision 1.5 2009/06/10 09:23:05EDT acudmore 
** Changed OS_Mem* and OS_BSP* calls to CFE_PSP_* calls
** Revision 1.4 2009/05/26 11:53:31EDT lwalling 
** Modify API interface to increment ExternalCount from external ToneSend functions
** Revision 1.3 2008/08/06 22:42:34EDT dkobe 
** Added CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.2 2008/07/31 15:41:36EDT apcudmore 
** Added execution counter API:
**   -- Added execution counter to ES internal task data
**   -- Added code to increment counter in RunLoop
**   -- Added code to report counter in GetInfo APIs
**   -- Added API to increment counter for child tasks
**   -- Added code to cFE Core apps to increment counters.
** Revision 1.1 2008/04/17 08:05:39EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/time/project.pj
** Revision 1.16 2007/04/24 08:59:04EDT njyanchik 
** I added the changes tha LRO requested. They are specified in the DCR text.
** Revision 1.15 2007/04/09 14:56:52EDT rjmcgraw 
** Changed performance ids to include CFE_
** Revision 1.14 2007/04/04 08:14:52EDT njyanchik 
** In DCR 3190, two API's were changed in the OSAL for locking out interrupts. This 
** change package changes TIME's and ES's use of those APIs
** Revision 1.13 2007/03/15 11:16:53EST njyanchik 
** I changed the interrupt enable/disable pair to use a lock key that records the 
** previous state of the interrupts before disabling, and then use that key to 
** re-enable the interrupts. The CFE core applications that use this pair were also 
** fixed for this API change.
** Revision 1.12 2007/03/07 12:03:36EST njyanchik 
** There were errors in the previous change package, so I fixed them
** Revision 1.11 2007/03/07 10:54:23EST njyanchik 
** I added a IsToneGood bit to the information that CFE_TIME_GetClockInfo function  
** returns. The decison on a 'good' tone is made in the 1Hz ISR. If the tone is about
** 1 second after the previous one, it is still considered good, otherwise, the tone 
** is not good.
**
*/


/*
** Required header files...
*/
#include "cfe_time_utils.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSend() -- Send "time at the tone" (local time)     */
/*                                                                 */
/* There is a presumption that this function will be called at     */
/*    the appropriate time (relative to the tone) such that the    */
/*    "time at the tone" data command will arrive within the       */
/*    specified window for tone and data packet verification.      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_ToneSend(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t NewMET;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Get the new MET from the appropriate source...
    */
    if (Reference.ClockFlyState == CFE_TIME_IS_FLY)
    {
        /*
        ** At least one of the following conditions is true...
        **
        **  1) loss of tone signal
        **  2) loss of "time at the tone" data packet
        **  3) signal and packet not within valid window
        **  4) we were commanded into fly-wheel mode
        **
        ** Set the new MET to our fly-wheel best guess...
        */
        NewMET.Seconds = Reference.CurrentMET.Seconds;
    }
    else
    {
        /*
        ** MET seconds is the count of tone interrupts...
        */
        #if (CFE_TIME_CFG_VIRTUAL == TRUE)
        NewMET.Seconds = CFE_TIME_TaskData.VirtualMET;
        #endif

        /*
        ** Read MET seconds from a h/w register...
        */
        #if (CFE_TIME_CFG_VIRTUAL != TRUE)
        OS_GetLocalMET(&NewMET.Seconds);
        #endif
    }

    /*
    ** Add a second if the tone has not yet occurred...
    */
    #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    NewMET.Seconds++;
    #endif

    /*
    ** Need to fix this if the tone is not 1Hz...
    */
    NewMET.Subseconds = 0;

    /*
    ** Remainder of time values are unchanged...
    */
    CFE_TIME_Copy(&CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET, &NewMET);
    CFE_TIME_Copy(&CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneSTCF, &Reference.AtToneSTCF);
    CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneLeaps = Reference.AtToneLeaps;

    /*
    ** Current clock state is a combination of factors...
    */
    CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneState = CFE_TIME_CalculateState(&Reference);

    /*
    ** Send "time at the tone" command data packet...
    */
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.ToneDataCmd);

    /*
    ** Count of "time at the tone" commands sent with internal data...
    */
    CFE_TIME_TaskData.InternalCount++;

    return;

} /* End of CFE_TIME_ToneSend() */
#endif /* CFE_TIME_CFG_SERVER */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSendMET() -- Send "time at tone" (external MET)    */
/*                                                                 */
/* There is a presumption that this function will be called at     */
/*    the appropriate time (relative to the tone) such that the    */
/*    "time at the tone" data command will arrive within the       */
/*    specified window for tone and data packet verification.      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SRC_MET == TRUE)
int32 CFE_TIME_ToneSendMET(CFE_TIME_SysTime_t NewMET)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t Expected;
    CFE_TIME_SysTime_t MinValid;
    CFE_TIME_SysTime_t MaxValid;
    CFE_TIME_Compare_t MinResult;
    CFE_TIME_Compare_t MaxResult;

    int16 ClockState;
    int32 Result = CFE_SUCCESS;

    /* Start Performance Monitoring */
    CFE_ES_PerfLogEntry(CFE_TIME_SENDMET_PERF_ID );

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Ignore external time data if commanded to use local MET...
    */
    if (CFE_TIME_TaskData.ClockSource == CFE_TIME_USE_INTERN)
    {
        Result = CFE_TIME_INTERNAL_ONLY;

        /*
        ** Use internal clock but still send "time at the tone"...
        */
        CFE_TIME_ToneSend();
    }
    else
    {
        /*
        ** Get reference time values (local time, time at tone, etc.)...
        */
        CFE_TIME_GetReference(&Reference);

        /*
        ** cFE defines MET as being synchronized to the tone signal...
        */
        Expected.Seconds = Reference.CurrentMET.Seconds;
        Expected.Subseconds = 0;

        /*
        ** Add a second if the tone has not yet occurred...
        */
        #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
        Expected.Seconds++;
        #endif

        /*
        ** Compute minimum and maximum values for valid MET...
        */
        MinValid = CFE_TIME_Subtract(Expected, CFE_TIME_TaskData.MaxDelta);
        MaxValid = CFE_TIME_Add(Expected, CFE_TIME_TaskData.MaxDelta);

        /*
        ** Compare new MET to minimum and maximum MET...
        */
        MinResult = CFE_TIME_Compare(NewMET, MinValid);
        MaxResult = CFE_TIME_Compare(NewMET, MaxValid);

        /*
        ** Ignore bad external time data only if clock state is valid...
        */
        if ((Reference.ClockSetState == CFE_TIME_WAS_SET) &&
           ((MinResult == CFE_TIME_A_LT_B) || (MaxResult == CFE_TIME_A_GT_B)))
        {
            Result = CFE_TIME_OUT_OF_RANGE;

            /*
            ** Use internal clock but still send "time at the tone"...
            */
            CFE_TIME_ToneSend();
        }
        else
        {
            ClockState = CFE_TIME_CalculateState(&Reference);
            /*
            ** Set "time at the tone" command data packet arguments...
            */
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET   = NewMET;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneSTCF  = Reference.AtToneSTCF;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneLeaps = Reference.AtToneLeaps;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneState = ClockState;

            /*
            ** Send "time at the tone" command data packet...
            */
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.ToneDataCmd);

            /*
            ** Count of "time at the tone" commands sent with external data...
            */
            CFE_TIME_TaskData.ExternalCount++;
        }
    }

    /* Exit performance monitoring */
    CFE_ES_PerfLogExit(CFE_TIME_SENDMET_PERF_ID);
    return(Result);

} /* End of CFE_TIME_ToneSendMET() */
#endif /* CFE_TIME_CFG_SRC_MET */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSendGPS() -- Send "time at tone" (external GPS)    */
/*                                                                 */
/* There is a presumption that this function will be called at     */
/*    the appropriate time (relative to the tone) such that the    */
/*    "time at the tone" data command will arrive within the       */
/*    specified window for tone and data packet verification.      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SRC_GPS == TRUE)
int32 CFE_TIME_ToneSendGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t NewSTCF;
    CFE_TIME_SysTime_t NewMET;
    CFE_TIME_SysTime_t MinValid;
    CFE_TIME_SysTime_t MaxValid;
    CFE_TIME_Compare_t MinResult;
    CFE_TIME_Compare_t MaxResult;

    int16 ClockState;
    int32 Result = CFE_SUCCESS;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    
    /*
    ** Ignore external time data if commanded to use local MET...
    */
    if (CFE_TIME_TaskData.ClockSource == CFE_TIME_USE_INTERN)
    {
        Result = CFE_TIME_INTERNAL_ONLY;

        /*
        ** Use internal clock but still send "time at the tone"...
        */
        CFE_TIME_ToneSend();
    }
    else
    {
        /*
        ** Get reference time values (local time, time at tone, etc.)...
        */
        CFE_TIME_GetReference(&Reference);

        /*
        ** cFE defines MET as being synchronized to the tone signal...
        */
        NewMET.Seconds = Reference.CurrentMET.Seconds;
        NewMET.Subseconds = 0;

        /*
        ** Add a second if the tone has not yet occurred...
        */
        #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
        NewMET.Seconds++;
        #endif

        /*
        ** Remove MET from the new time value (leaves STCF)...
        */
        NewSTCF = CFE_TIME_Subtract(NewTime, NewMET);

        /*
        ** Restore leap seconds if default time format is UTC...
        */
        #if (CFE_TIME_CFG_DEFAULT_UTC == TRUE)
        NewSTCF.Seconds += NewLeaps;
        #endif

        /*
        ** Compute minimum and maximum values for valid STCF...
        */
        MinValid = CFE_TIME_Subtract(Reference.AtToneSTCF, CFE_TIME_TaskData.MaxDelta);
        MaxValid = CFE_TIME_Add(Reference.AtToneSTCF, CFE_TIME_TaskData.MaxDelta);

        /*
        ** Compare new STCF to minimum and maximum STCF...
        */
        MinResult = CFE_TIME_Compare(NewSTCF, MinValid);
        MaxResult = CFE_TIME_Compare(NewSTCF, MaxValid);

        /*
        ** If state is valid then ignore bad external time data...
        */
        if ((Reference.ClockSetState == CFE_TIME_WAS_SET) &&
           ((MinResult == CFE_TIME_A_LT_B) || (MaxResult == CFE_TIME_A_GT_B)))
        {
            Result = CFE_TIME_OUT_OF_RANGE;

            /*
            ** Use internal clock but still send "time at the tone"...
            */
            CFE_TIME_ToneSend();
        }
        else
        {
            ClockState = CFE_TIME_CalculateState(&Reference);
            /*
            ** Set "time at the tone" command data packet arguments...
            */
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET   = NewMET;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneSTCF  = NewSTCF;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneLeaps = NewLeaps;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneState = ClockState;

            /*
            ** Send "time at the tone" command data packet...
            */
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.ToneDataCmd);

            /*
            ** Count of "time at the tone" commands sent with external data...
            */
            CFE_TIME_TaskData.ExternalCount++;
        }
    }

    return(Result);

} /* End of CFE_TIME_ToneSendGPS() */
#endif /* CFE_TIME_CFG_SRC_GPS */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSendTime() -- Send "time at tone" (external time)  */
/*                                                                 */
/* There is a presumption that this function will be called at     */
/*    the appropriate time (relative to the tone) such that the    */
/*    "time at the tone" data command will arrive within the       */
/*    specified window for tone and data packet verification.      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SRC_TIME == TRUE)
int32 CFE_TIME_ToneSendTime(CFE_TIME_SysTime_t NewTime)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t NewSTCF;
    CFE_TIME_SysTime_t NewMET;
    CFE_TIME_SysTime_t MinValid;
    CFE_TIME_SysTime_t MaxValid;
    CFE_TIME_Compare_t MinResult;
    CFE_TIME_Compare_t MaxResult;

    int16 ClockState;
    int32 Result = CFE_SUCCESS;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Ignore external time data if commanded to use local MET...
    */
    if (CFE_TIME_TaskData.ClockSource == CFE_TIME_USE_INTERN)
    {
        Result = CFE_TIME_INTERNAL_ONLY;

        /*
        ** Use internal clock but still send "time at the tone"...
        */
        CFE_TIME_ToneSend();
    }
    else
    {
        /*
        ** Get reference time values (local time, time at tone, etc.)...
        */
        CFE_TIME_GetReference(&Reference);

        /*
        ** cFE defines MET as being synchronized to the tone signal...
        */
        NewMET.Seconds = Reference.CurrentMET.Seconds;
        NewMET.Subseconds = 0;

        /*
        ** Add a second if the tone has not yet occurred...
        */
        #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
        NewMET.Seconds++;
        #endif

        /*
        ** Remove MET from the new time value (leaves STCF)...
        */
        NewSTCF = CFE_TIME_Subtract(NewTime, NewMET);

        /*
        ** Restore leap seconds if default time format is UTC...
        */
        #if (CFE_TIME_CFG_DEFAULT_UTC == TRUE)
        NewSTCF.Seconds += Reference.AtToneLeaps;
        #endif

        /*
        ** Compute minimum and maximum values for valid STCF...
        */
        MinValid = CFE_TIME_Subtract(Reference.AtToneSTCF, CFE_TIME_TaskData.MaxDelta);
        MaxValid = CFE_TIME_Add(Reference.AtToneSTCF, CFE_TIME_TaskData.MaxDelta);

        /*
        ** Compare new STCF to minimum and maximum STCF...
        */
        MinResult = CFE_TIME_Compare(NewSTCF, MinValid);
        MaxResult = CFE_TIME_Compare(NewSTCF, MaxValid);

        /*
        ** If state is valid then ignore bad external time data...
        */
        if ((Reference.ClockSetState == CFE_TIME_WAS_SET) &&
           ((MinResult == CFE_TIME_A_LT_B) || (MaxResult == CFE_TIME_A_GT_B)))
        {
            Result = CFE_TIME_OUT_OF_RANGE;

            /*
            ** Use internal clock but still send "time at the tone"...
            */
            CFE_TIME_ToneSend();
        }
        else
        {
            ClockState = CFE_TIME_CalculateState(&Reference);
            /*
            ** Set "time at the tone" command data packet arguments...
            */
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET   = NewMET;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneSTCF  = NewSTCF;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneLeaps = Reference.AtToneLeaps;
            CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneState = ClockState;

            /*
            ** Send "time at the tone" command data packet...
            */
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.ToneDataCmd);

            /*
            ** Count of "time at the tone" commands sent with external data...
            */
            CFE_TIME_TaskData.ExternalCount++;
        }
    }

    return(Result);

} /* End of CFE_TIME_ToneSendTime() */
#endif /* CFE_TIME_CFG_SRC_TIME */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneData() -- process "time at tone" data packet       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ToneData(CFE_TIME_ToneDataCmd_Payload_t *ToneDataCmd)
{
    /*
    ** Save the time when the data packet was received...
    */
    CFE_TIME_TaskData.ToneDataLatch   = CFE_TIME_LatchClock();

    /*
    ** Save the data packet (may be a while before the data is used)...
    */
    CFE_TIME_Copy(&CFE_TIME_TaskData.PendingMET, &ToneDataCmd->AtToneMET);
    CFE_TIME_Copy(&CFE_TIME_TaskData.PendingSTCF, &ToneDataCmd->AtToneSTCF);
    CFE_TIME_TaskData.PendingLeaps = ToneDataCmd->AtToneLeaps;
    CFE_TIME_TaskData.PendingState = ToneDataCmd->AtToneState;

    /*
    ** If the data packet is designed to arrive after the tone...
    **
    ** Check to see if the most recent tone signal matches this
    **    data packet.  If so, we have a matched pair and can
    **    now start using the new data to compute time.
    */
    #if (CFE_TIME_AT_TONE_WAS == TRUE)
    CFE_TIME_ToneVerify(CFE_TIME_TaskData.ToneSignalLatch,
                        CFE_TIME_TaskData.ToneDataLatch);
    #endif

    /*
    ** If the data packet is designed to arrive before the tone...
    **
    ** We don't really need to do anything except to save the time
    **    and contents of this data packet.  (above)
    **
    ** Note that we do not immediately start using the data packet
    **    values to compute current time.  We continue to use the
    **    old tone/data combo until we get a new matched pair.
    */
    #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    #endif

    /*
    ** Maintain a count of tone data packets...
    */
    CFE_TIME_TaskData.ToneDataCount++;

    return;

} /* End of CFE_TIME_ToneData() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSignal() -- "tone signal" occurred recently        */
/*                                                                 */
/* This function is called upon receipt of a command indicating    */
/*    that a time at the tone signal was detected.  The mission    */
/*    dependent h/w or s/w that detected the tone signal latched   */
/*    the local clock and generated this command.  The use of a    */
/*    command announcing the tone signal ensures that this code    */
/*    is not called from within an interrupt handler.              */
/*                                                                 */
/* It is not a concern that some amount of time has elapsed since  */
/*    the tone actually occurred.  We are currently computing      */
/*    time as a delta (as measured on our local clock) from a      */
/*    previously latched tone.  It just doesn't matter if the      */
/*    size of the delta slightly exceeds a second.  The quality    */
/*    of our local clock will always be sufficient to measure      */
/*    time for a couple of seconds.                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ToneSignal(void)
{
    /*
    ** If the data packet is designed to arrive after the tone signal...
    **
    ** We don't really need to do anything except latch the local clock
    **    at the moment of the tone.  And that has already happened at
    **    the time when the tone was detected.
    **
    ** Note that we do not immediately start using this latched value to
    **    compute current time.  We continue to use the old tone/data
    **    combo until we get a new matched pair.
    */
    #if (CFE_TIME_AT_TONE_WAS == TRUE)
    #endif

    /*
    ** If the data packet is designed to arrive before the tone signal...
    **
    ** Check to see if the most recent data packet matches this
    **    tone signal.  If so, we have a matched pair and can
    **    now start using the new data to compute time.
    */
    #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    CFE_TIME_ToneVerify(CFE_TIME_TaskData.ToneDataLatch,
                        CFE_TIME_TaskData.ToneSignalLatch);
    #endif

    /*
    ** Maintain a count of tone signal packets...
    */
    CFE_TIME_TaskData.ToneSignalCount++;

    return;

} /* End of CFE_TIME_ToneSignal() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneVerify() -- validate tone and data packet          */
/*                                                                 */
/*                                                                 */
/* If the data packet is designed to arrive after the tone, then   */
/*                                                                 */
/*    Time1 = local clock latched at the detection of the tone     */
/*    Time2 = local clock latched at the arrival of the packet     */
/*                                                                 */
/*                                                                 */
/* If the data packet is designed to arrive before the tone, then  */
/*                                                                 */
/*    Time1 = local clock latched at the arrival of the packet     */
/*    Time2 = local clock latched at the detection of the tone     */
/*                                                                 */
/*                                                                 */
/* In either case, Time1 occurred before Time2                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ToneVerify(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_Compare_t result;
    CFE_TIME_SysTime_t elapsed;

    static CFE_TIME_SysTime_t PrevTime1 = {0,0};
    static CFE_TIME_SysTime_t PrevTime2 = {0,0};

    /*
    ** It is possible to call this func with static time value...
    */
    result = CFE_TIME_Compare(PrevTime1, Time1);
    if (result == CFE_TIME_EQUAL)
    {
        CFE_TIME_TaskData.ToneMatchErrors++;
    }
    else
    {
        result = CFE_TIME_Compare(PrevTime2, Time2);
        if (result == CFE_TIME_EQUAL)
        {
            CFE_TIME_TaskData.ToneMatchErrors++;
        }
        else
        {
            /*
            ** Compute elapsed time between tone and data packet...
            */
            result = CFE_TIME_Compare(Time1, Time2);
            if (result == CFE_TIME_A_GT_B)
            {
                /*
                ** Local clock has rolled over...
                */
                elapsed = CFE_TIME_Subtract(CFE_TIME_TaskData.MaxLocalClock, Time1);
                elapsed = CFE_TIME_Add(elapsed, Time2);
            }
            else
            {
                /*
                ** Normal case...
                */
                elapsed = CFE_TIME_Subtract(Time2, Time1);
            }

            /*
            ** Ensure that time between packet and tone is within limits...
            */
            if ((elapsed.Seconds != 0) ||
                (elapsed.Subseconds < CFE_TIME_TaskData.MinElapsed) ||
                (elapsed.Subseconds > CFE_TIME_TaskData.MaxElapsed))
            {
                /*
                ** Maintain count of tone vs data packet mis-matches...
                */
                CFE_TIME_TaskData.ToneMatchErrors++;
            }
            else
            {
                CFE_TIME_TaskData.ToneMatchCount++;

                /*
                ** Skip tone packet update if commanded into "flywheel" mode...
                */
                if (!CFE_TIME_TaskData.Forced2Fly)
                {
                    /*
                    ** Process "matching" tone and data packet...
                    */
                    CFE_TIME_ToneUpdate();
                }
            }
        }
    }

    PrevTime1 = Time1;
    PrevTime2 = Time2;

    return;

} /* End of CFE_TIME_ToneVerify() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneUpdate() -- process "matching" tone & data packet  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ToneUpdate(void)
{
    CFE_TIME_Reference_t Reference;
    boolean NewFlywheelStatus = FALSE;
    int32 IntFlags;

    /* Zero out the Reference before we send it into a function */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** If we have been flywheeling, VirtualMET may be incorrect
    **  (e.g. missing tone signals -- VirtualMET is tone count)
    */
    if ((CFE_TIME_TaskData.ClockSource == CFE_TIME_USE_INTERN) &&
        (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_IS_FLY))
    {
        /*
        ** Get reference time (before locking out interrupts)...
        */
        CFE_TIME_GetReference(&Reference);
    }

    /*
    ** Ensure that the change is made without interruption...
    */
    IntFlags = OS_IntLock();

    #if (CFE_TIME_CFG_SERVER == TRUE)
    /*
    ** Time servers cannot always use the new time data from the
    **    packet (saved as "pending" when the packet arrived).
    **
    ** If the time source is "internal" then the time data came
    **    from the same values that we would be updating, hence
    **    there is no need to do the update.  And if there has
    **    been a command to set new values during the moment
    **    between making the time at the tone packet and now,
    **    then we would want to use the command values anyway.
    **
    ** If the time source is "external" then things get complex.
    **    If the external data is MET then we only want to take
    **    the MET from the packet.  But, if the external data
    **    is "time" then we only want to take the STCF from the
    **    packet.  And, if the external data is GPS then we
    **    need to take both the STCF and the leap seconds from
    **    the packet.  Also, by definition, we cannot have both
    **    external data and a local h/w MET - so we don't need
    **    to worry about updating a local MET to external time.
    */
    CFE_TIME_TaskData.AtToneLatch = CFE_TIME_TaskData.ToneSignalLatch;

    if (CFE_TIME_TaskData.ClockSource == CFE_TIME_USE_INTERN)
    {
        /*
        ** If we have been flywheeling, VirtualMET may be incorrect
        **  (flywheel state is changed later in this function)
        */
        if (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_IS_FLY)
        {
            CFE_TIME_TaskData.VirtualMET = Reference.CurrentMET.Seconds;
        }

        /*
        ** Update "time at tone" to match virtual MET counter...
        **
        ** Note: It is OK to not bother with reading the h/w MET
        **       since we sync'ed them at the moment of the tone.
        */
        CFE_TIME_TaskData.AtToneMET.Seconds = CFE_TIME_TaskData.VirtualMET;
        CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    }
    else
    {
        /*
        ** Update "time at tone" with external MET data...
        */
        #if (CFE_TIME_CFG_SRC_MET == TRUE)
        CFE_TIME_TaskData.AtToneMET  = CFE_TIME_TaskData.PendingMET;
        CFE_TIME_TaskData.VirtualMET = CFE_TIME_TaskData.PendingMET.Seconds;
        #endif

        /*
        ** Update "time at tone" with external GPS data...
        **
        **  STCF = GPS time at the tone - local MET at the tone
        **  Leaps = GPS leaps
        **
        ** It is possible that a command changed the MET after it was used
        **    to calculate the pending STCF -- in which case the current
        **    time will jump next second when the STCF gets calculated
        **    again with the new MET value.  This (small) possibility can
        **    be prevented by switching to "internal" mode before sending
        **    set time commands...
        */
        #if (CFE_TIME_CFG_SRC_GPS == TRUE)
        CFE_TIME_TaskData.AtToneMET.Seconds = CFE_TIME_TaskData.VirtualMET;
        CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
        CFE_TIME_TaskData.AtToneSTCF  = CFE_TIME_TaskData.PendingSTCF;
        CFE_TIME_TaskData.AtToneLeaps = CFE_TIME_TaskData.PendingLeaps;
        #endif

        /*
        ** Update "time at tone" with external time data...
        **
        **  STCF = external time at the tone - local MET at the tone
        */
        #if (CFE_TIME_CFG_SRC_TIME == TRUE)
        CFE_TIME_TaskData.AtToneMET.Seconds = CFE_TIME_TaskData.VirtualMET;
        CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
        CFE_TIME_TaskData.AtToneSTCF = CFE_TIME_TaskData.PendingSTCF;
        #endif
    }

    /*
    ** With a "time" update, this server cannot be "flywheeling"
    **  (we won't get this update if commanded to flywheel)
    */
    if (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_IS_FLY)
    {
        CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
        CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
        NewFlywheelStatus = TRUE;
    }
    #endif /* CFE_TIME_CFG_SERVER */

    #if (CFE_TIME_CFG_CLIENT == TRUE)
    /*
    ** Set local clock latch time that matches the tone...
    */
    CFE_TIME_TaskData.AtToneLatch = CFE_TIME_TaskData.ToneSignalLatch;

    /*
    ** Time clients need all the "time at the tone" command data...
    */
    CFE_TIME_TaskData.AtToneMET   = CFE_TIME_TaskData.PendingMET;
    CFE_TIME_TaskData.AtToneSTCF  = CFE_TIME_TaskData.PendingSTCF;
    CFE_TIME_TaskData.AtToneLeaps = CFE_TIME_TaskData.PendingLeaps;

    /*
    ** Convert the server clock state into its component parts...
    */
    if (CFE_TIME_TaskData.PendingState == CFE_TIME_INVALID)
    {
        CFE_TIME_TaskData.ClockSetState  = CFE_TIME_NOT_SET;
        CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
    }
    else
    {
        CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;

        /*
        ** If the server is fly-wheel then the client must also
        **    report fly-wheel (even if it is not)...
        */
        if (CFE_TIME_TaskData.PendingState == CFE_TIME_FLYWHEEL)
        {
            CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
        }
        else
        {
            CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
        }
    }

    /*
    ** With a "time" update, this client cannot be "flywheeling"...
    **  (we won't get this update if commanded to flywheel)
    */
    if (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_IS_FLY)
    {
        CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
        NewFlywheelStatus = TRUE;
    }
    #endif /* CFE_TIME_CFG_CLIENT */

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_TaskData.VersionCount++;

    OS_IntUnlock(IntFlags);

    /*
    ** Wait until after interrupts are enabled to send event...
    */
    if (NewFlywheelStatus)
    {
        CFE_EVS_SendEvent(CFE_TIME_FLY_OFF_EID, CFE_EVS_INFORMATION,
                         "Stop FLYWHEEL");
    }

    return;

} /* End of CFE_TIME_ToneUpdate() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_Local1HzTimerCallback() -- 1Hz callback routine        */
/*                                                                 */
/* This is a wrapper around CFE_TIME_Local1HzISR that conforms to  */
/* the prototype of an OSAL Timer callback routine.                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_Local1HzTimerCallback(uint32 TimerId, void *Arg)
{
    CFE_TIME_Local1HzISR();
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_Tone1HzISR() -- Tone signal ISR                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_Tone1HzISR(void)
{

    CFE_TIME_SysTime_t ToneSignalLatch;
    CFE_TIME_SysTime_t Elapsed;
    CFE_TIME_Compare_t Result;

    /* Start Performance Monitoring */
    CFE_ES_PerfLogEntry(CFE_TIME_TONE1HZISR_PERF_ID);

    
    /*
    ** Latch the local clock when the tone signal occurred...
    */
    ToneSignalLatch = CFE_TIME_LatchClock();

    /*
    ** Compute elapsed time since the previous tone signal...
    */
    Result = CFE_TIME_Compare(ToneSignalLatch,
                              CFE_TIME_TaskData.ToneSignalLatch);

    if (Result == CFE_TIME_A_LT_B)
    {
        /*
        ** Local clock has rolled over...
        */
        Elapsed = CFE_TIME_Subtract(CFE_TIME_TaskData.MaxLocalClock,
                                    CFE_TIME_TaskData.ToneSignalLatch);
        Elapsed = CFE_TIME_Add(Elapsed, ToneSignalLatch);
    }
    else
    {
        /*
        ** Normal case...
        */
        Elapsed = CFE_TIME_Subtract(ToneSignalLatch, CFE_TIME_TaskData.ToneSignalLatch);
    }

    /*
    ** Verify that tone occurred ~1 second after previous tone...
    */
    if (((Elapsed.Seconds == 1) && (Elapsed.Subseconds < CFE_TIME_TaskData.ToneOverLimit)) ||
        ((Elapsed.Seconds == 0) && (Elapsed.Subseconds > CFE_TIME_TaskData.ToneUnderLimit)))
    {
        /*
        ** Maintain count of valid tone signal interrupts...
        **   (set to zero by reset command)
        */
        CFE_TIME_TaskData.ToneIntCount++;

        /* Since the tone occured ~1 seonds after the previous one, we
        ** can mark this tone as 'good'
        */
        CFE_TIME_TaskData.IsToneGood = TRUE;

        /*
        ** Maintain virtual MET as count of valid tone signal interrupts...
        **   (not set to zero by reset command)
        */
        #if (CFE_TIME_CFG_VIRTUAL == TRUE)
        CFE_TIME_TaskData.VirtualMET++;
        #endif

        /*
        ** Maintain virtual MET as count read from h/w MET register...
        */
        #if (CFE_TIME_CFG_VIRTUAL != TRUE)
        OS_GetLocalMET(&CFE_TIME_TaskData.VirtualMET);
        #endif

        /*
        ** Enable tone task (we can't send a SB message from here)...
        */
        OS_BinSemGive(CFE_TIME_TaskData.ToneSemaphore);
    }
    else
    {
        /*
        ** Maintain count of invalid tone signal interrupts...
        **   (set to zero by reset command)
        */
        CFE_TIME_TaskData.ToneIntErrors++;
        
        /* Since the tone didn't occur ~1 seonds after the previous one, we
        ** can mark this tone as 'not good'
        */
        CFE_TIME_TaskData.IsToneGood = FALSE;
    }

    /*
    ** Save local time latch of most recent tone signal...
    */
    CFE_TIME_TaskData.ToneSignalLatch = ToneSignalLatch;
    
    /* Notify registered time synchronization applications */
    CFE_TIME_NotifyTimeSynchApps();

    /* Exit performance monitoring */
    CFE_ES_PerfLogExit(CFE_TIME_TONE1HZISR_PERF_ID);

    return;

} /* End of CFE_TIME_Tone1HzISR() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_Tone1HzTask() -- Tone 1Hz task                         */
/*                                                                 */
/* This task exists solely to generate the tone signal command.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_Tone1HzTask(void)
{
    int32 Result = CFE_ES_RegisterChildTask();

    while (Result == CFE_SUCCESS)
    {    
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();

        /*
        ** Pend on semaphore given by tone ISR (above)...
        */
        Result = OS_BinSemTake(CFE_TIME_TaskData.ToneSemaphore);
        
        /* Start Performance Monitoring */
        CFE_ES_PerfLogEntry(CFE_TIME_TONE1HZTASK_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /*
            ** Send tone signal command packet...
            */
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.ToneSignalCmd);

            /*
            ** Maintain count of tone task wake-ups...
            */
            CFE_TIME_TaskData.ToneTaskCount++;
        }

        /* Exit performance monitoring */
        CFE_ES_PerfLogExit(CFE_TIME_TONE1HZTASK_PERF_ID);
    }

    /*
    ** This should never happen - but during development we
    **    had an error in the creation of the semaphore.
    */
    return;

} /* End of CFE_TIME_Tone1HzTask() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_Local1HzISR() -- Local 1Hz interrupt (not the tone)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_Local1HzISR(void)
{

    CFE_TIME_Reference_t Reference;

    /* Start Performance Monitoring */
    CFE_ES_PerfLogEntry(CFE_TIME_LOCAL1HZISR_PERF_ID);

    
    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    /*
    ** Apply 1Hz adjustment to STCF...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
    if ((CFE_TIME_TaskData.OneHzAdjust.Seconds != 0) ||
        (CFE_TIME_TaskData.OneHzAdjust.Subseconds != 0))
    {
        CFE_TIME_SysTime_t NewSTCF;

        if (CFE_TIME_TaskData.OneHzDirection == CFE_TIME_ADD_ADJUST)
        {
            NewSTCF = CFE_TIME_Add(CFE_TIME_TaskData.AtToneSTCF,
                                CFE_TIME_TaskData.OneHzAdjust);
        }
        else
        {
            NewSTCF = CFE_TIME_Subtract(CFE_TIME_TaskData.AtToneSTCF,
                                     CFE_TIME_TaskData.OneHzAdjust);
        }

        CFE_TIME_TaskData.AtToneSTCF = NewSTCF;

        /*
        ** Time has changed, force anyone reading time to retry...
        */
        CFE_TIME_TaskData.VersionCount++;
    }
    #endif /* CFE_TIME_CFG_SERVER */

    /*
    ** Get reference time (calculates time since tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** See if it has been long enough without receiving a time update
    **    to autonomously start "fly-wheel" mode...
    */
    if (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_NO_FLY)
    {
        if (Reference.TimeSinceTone.Seconds >= CFE_TIME_CFG_START_FLY)
        {
            /*
            ** Change current state to "fly-wheel"...
            */
            CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
            #if (CFE_TIME_CFG_SERVER == TRUE)
            CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
            #endif

            CFE_TIME_TaskData.AutoStartFly = TRUE;

            /*
            ** Force anyone currently reading time to retry...
            */
            CFE_TIME_TaskData.VersionCount++;
        }
    }

    /*
    ** See if it has been long enough without receiving a time update
    **    (or since last doing this update) to autonomously update the
    **    MET at the tone and local clock latched at the tone...
    */
    if (CFE_TIME_TaskData.ClockFlyState == CFE_TIME_IS_FLY)
    {
        if (Reference.TimeSinceTone.Seconds >= CFE_TIME_CFG_LATCH_FLY)
        {
            /*
            ** Update MET at tone and local clock latched at tone...
            **
            ** This does not increase the accuracy of the local clock,
            **    but it does avoid some problems.  It is not uncommon
            **    for a local clock to roll over after only a few
            **    seconds, so we try and keep the elapsed time since
            **    the "tone" to a relatively small number of seconds.
            **    We can handle a simple roll-over but need to prevent
            **    the local clock from completely wrapping around the
            **    time latched at the tone.
            */
            CFE_TIME_TaskData.AtToneMET    = Reference.CurrentMET;
            CFE_TIME_TaskData.AtToneLatch  = Reference.CurrentLatch;

            /*
            ** Force anyone currently reading time to retry...
            */
            CFE_TIME_TaskData.VersionCount++;
        }
    }

    CFE_TIME_TaskData.LocalIntCount++;

    /*
    ** Enable 1Hz task (we can't send a SB message from here)...
    */
    //OS_BinSemGive(CFE_TIME_TaskData.LocalSemaphore);
    // STF1:
    // With Free RTOS we must call this version because
    // we are in an ISR at this point.
    // nos3 will need to implement this function in the osal
    // and have it simple call OS_BinSemGive()
    OS_BinSemGiveFromISR(CFE_TIME_TaskData.LocalSemaphore);

    /* Exit performance monitoring */
    CFE_ES_PerfLogExit(CFE_TIME_LOCAL1HZISR_PERF_ID);

    return;

} /* End of CFE_TIME_Local1HzISR() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_Local1HzTask() -- Local 1Hz task (not the tone)        */
/*                                                                 */
/* This task exists solely to generate the 1Hz wake-up command.    */
/*                                                                 */
/* This is a temporary solution until a scheduler is implemented.  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_Local1HzTask(void)
{
    int32 Result = CFE_ES_RegisterChildTask();

    while (Result == CFE_SUCCESS)
    {
    
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();

        /*
        ** Pend on the 1HZ semaphore (given by local 1Hz ISR)...
        */
        Result = OS_BinSemTake(CFE_TIME_TaskData.LocalSemaphore);
                
        /* Start Performance Monitoring */
        CFE_ES_PerfLogEntry(CFE_TIME_LOCAL1HZTASK_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /*
            ** Send "info" event if we just started flywheel mode...
            */
            if (CFE_TIME_TaskData.AutoStartFly)
            {
                CFE_TIME_TaskData.AutoStartFly = FALSE;

                CFE_EVS_SendEvent(CFE_TIME_FLY_ON_EID,
                                  CFE_EVS_INFORMATION,
                                 "Start FLYWHEEL");
            }

            /*
            ** Send out a simulated tone signal (time server only)...
            */
            #if (CFE_TIME_CFG_SERVER == TRUE)
              #if (CFE_TIME_CFG_FAKE_TONE == TRUE)
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.FakeToneCmd);
              #endif
            #endif

            /*
            ** Send 1Hz (optional) timing packet...
            */
            #if (CFE_TIME_ENA_1HZ_CMD_PKT == TRUE)
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.Local1HzCmd);
            #endif

            CFE_TIME_TaskData.LocalTaskCount++;
        }

        /* Exit performance monitoring */
        CFE_ES_PerfLogExit(CFE_TIME_LOCAL1HZTASK_PERF_ID);
    }

    /*
    ** This should never happen - but during development we had an
    **    error in the creation of the semaphore.
    */
    return;

} /* End of CFE_TIME_Local1HzTask() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_NotifyTimeSynchApps() -- Call App Synch Callback Funcs */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_NotifyTimeSynchApps(void)
{
    uint32      i = 0;

    /*
    ** Notify applications that have requested tone synchronization
    */
    if (CFE_TIME_TaskData.IsToneGood)
    {
        for (i=0; i<CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
        {
            if (CFE_TIME_TaskData.SynchCallback[i].Ptr != NULL)
            {
                (CFE_TIME_TaskData.SynchCallback[i].Ptr)();
            }
        }
    }
    
    return;
}

/************************/
/*  End of File Comment */
/************************/
