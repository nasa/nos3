/*
** File: utf_cfe_time_utils.c
**  $Id: utf_cfe_time_utils.c 1.6 2012/01/13 12:52:01GMT-05:00 acudmore Exp  $
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
** Purpose: This module defines the UTF versions of functions for the TIME API.  The real
**          functions are contained in the file cfe_time_utils.c.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:52:01GMT-05:00 $
** $Revision: 1.6 $
** $Log: utf_cfe_time_utils.c  $
** Revision 1.6 2012/01/13 12:52:01GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.5 2010/10/25 15:06:40EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 14:57:45EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/10/13 14:55:24EDT wmoleski 
** Changes required to be compatible with cFE 6.0.0
** Revision 1.2 2009/06/24 09:18:10EDT rmcgraw 
** DCR8290:4 OS_BSP to CFE_PSP_ and OS_Mem to CFE_PSP_Mem changes
** Revision 1.1 2008/04/17 08:09:39EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.8 2007/05/25 11:10:46EDT wfmoleski 
** Added new LatchClock api function and added a call to it.
** Revision 1.7 2006/12/21 08:37:55EST wfmoleski 
** 
** Revision 1.6 2006/11/13 07:41:51EST wfmoleski 
** Changed the UTF implementation to reflect the new OS_BSPGetTime call
** Revision 1.5 2006/06/21 15:39:02EDT wfmoleski 
** Added copyright prologs to utf_cfe_time files
** Revision 1.4 2006/06/04 17:43:13EDT nsschweiss 
** Removed UTF_set_time_hook.
** Revision 1.3 2006/05/09 15:32:10EDT nsschweiss 
** Adds time_hook function signature, set_time_hook function, and CalculateUTF function.
** Revision 1.2 2006/04/10 15:41:56EDT sslegel 
** Added code to set the stcf and leap seconds
** Revision 1.1 2006/03/24 14:30:34EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
*/

/*
** Include section
*/
#include "cfe_time_utils.h"
#include "utf_cfe_time_utils.h"

/*
** Time task global data...
*/
CFE_TIME_TaskData_t CFE_TIME_TaskData;

/******************************************************************************
**  Function:  UTF_set_leap_seconds()
**
**  Purpose: Set the value of the leap seconds.
*/
static int16 utf_leap_seconds = 0;
void UTF_set_leap_seconds(int16 leap_seconds)
{
    utf_leap_seconds = leap_seconds;
}

/******************************************************************************
**  Function:  UTF_set_stcf()
**
**  Purpose: Set the value of the STCF in system time format.
*/
static CFE_TIME_SysTime_t utf_stcf = { 0, 0 };
void UTF_set_stcf(CFE_TIME_SysTime_t stcf)
{
    utf_stcf = stcf;
}

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

void CFE_TIME_Tone1HzISR(void)
{
    return;
}

int16 CFE_TIME_CalculateState(CFE_TIME_Reference_t *Reference)
{
    (void) Reference;          /* refer to function argument to avoid compiler warning */

    return(CFE_TIME_VALID);
}

void CFE_TIME_GetReference(CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t  LatchTime = CFE_TIME_LatchClock();

    /*
    ** Fill out the time reference structure, to simplify things I set the
    ** the AtToneMET, AtToneLatch, CurrentLatch, and the CurrentMET all to the 
    ** same simulated time 
    */
    Reference->AtToneMET                = LatchTime;
    Reference->AtToneSTCF.Seconds       = utf_stcf.Seconds;
    Reference->AtToneSTCF.Subseconds    = utf_stcf.Subseconds;
    Reference->AtToneLeaps              = utf_leap_seconds;
    Reference->AtToneDelay.Seconds      = 0;
    Reference->AtToneDelay.Subseconds   = 0;
    Reference->AtToneLatch              = LatchTime;
    Reference->CurrentLatch             = LatchTime;
    Reference->ClockSetState            = CFE_TIME_WAS_SET;
    Reference->ClockFlyState            = CFE_TIME_NO_FLY;
    Reference->TimeSinceTone.Seconds    = 0;
    Reference->TimeSinceTone.Subseconds = 0;
    Reference->CurrentMET               = LatchTime;

    return;
} 

CFE_TIME_SysTime_t CFE_TIME_CalculateTAI(CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeAsTAI;

    TimeAsTAI = CFE_TIME_Add(Reference->CurrentMET, Reference->AtToneSTCF);

    return(TimeAsTAI);

} 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_CalculateUTC() -- calculate UTC from reference data    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t CFE_TIME_CalculateUTC(CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeAsUTC;

    TimeAsUTC = CFE_TIME_Add(Reference->CurrentMET, Reference->AtToneSTCF);
    TimeAsUTC.Seconds -= Reference->AtToneLeaps;

    return(TimeAsUTC);

} /* End of CFE_TIME_CalculateUTC() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSendMET()                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TIME_ToneSendMET(CFE_TIME_SysTime_t NewMET)
{
	(void) NewMET;   /* refer to function argument to avoid compiler warning */
	
	return CFE_SUCCESS;
}
