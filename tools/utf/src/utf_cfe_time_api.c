/*
** $Id: utf_cfe_time_api.c 1.7 2012/01/13 12:52:00GMT-05:00 acudmore Exp  $
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
** Purpose:  cFE Time Services (TIME) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:    Partially derived from SDO source code
**
** $Log: utf_cfe_time_api.c  $
** Revision 1.7 2012/01/13 12:52:00GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.6 2011/11/30 15:06:55EST jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.5 2010/10/25 15:06:33EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 14:57:43EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/10/13 14:55:28EDT wmoleski 
** Changes required to be compatible with cFE 6.0.0
** Revision 1.2 2009/06/24 09:18:15EDT rmcgraw 
** DCR8290:4 OS_BSP to CFE_PSP_ and OS_Mem to CFE_PSP_Mem changes
** Revision 1.1 2008/08/20 14:57:38EDT wfmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.2 2008/08/06 22:42:27EDT dkobe 
** Added CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:38EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/time/project.pj
** Revision 1.9 2007/03/07 10:54:24EST njyanchik 
** I added a IsToneGood bit to the information that CFE_TIME_GetClockInfo function returns. The decison on a 'good' tone is made in the 1Hz ISR. If the tone is about
** 1 second after the previous one, it is still considered good, otherwise, the tone is not good.
** Revision 1.8 2007/03/07 10:43:47EST njyanchik 
** I made a new API called CFE_TIME_GetClockInfo that is functionally equivilant to CFE_TIME_GetStateFlags. The reason I made a new function is because this function
** will get changed in a future DCR (3191).
** Revision 1.7 2006/07/28 10:17:12EDT njyanchik 
** Fix of LDRA errors
** Revision 1.6 2006/06/08 14:15:50EDT njyanchik 
** I added the appropriate legal headers to all of the time files
** Revision 1.5 2006/05/18 13:34:22EDT njyanchik 
** The conversions between microsecs and sub secs left 999999 us as 0xFFFFE000 subseconds,
** when 0xFFFFF000 is expected, causing the additions in the frequency to be off. Tthis change
** adds an extra factor that bumps the output so that 999999 meets 0xFFFFF000.
** Revision 1.4 2006/05/04 09:29:02EDT njyanchik 
** Added MET to default spacecraft time conversion
** API is CFE_TIME_SysTime_t CFE_TIME_MET2SCTime (CFE_TIME_SysTime_t METTime)
** Revision 1.3 2006/01/27 07:51:19EST njyanchik 
** modified CFE_TIME_Sub2MicroSecs to stop drift. If the Subseconds % 0x4000000 != 0 then we
**  will need to add 1 to the result. This will prevent the drift. Also, added in a check that prevents
** the number of microseconds from ever getting to 1000000.
** Revision 1.2 2005/07/21 15:28:30EDT lswalling 
** Add Time print API function
** Revision 1.1 2005/06/09 10:57:56EDT rperera 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/time/project.pj
*/


/*
** Required header files...
*/
#include "cfe_time_utils.h"
#include "utf_types.h"
#include "utf_cfe.h"
extern CFE_TIME_TaskData_t CFE_TIME_TaskData;

/* Function Hook Stuff */
typedef struct
{
    uint32  (*CFE_TIME_GetMETseconds)(void);
    uint32  (*CFE_TIME_GetMETsubsecs)(void);
    int32   (*CFE_TIME_RegisterSynchCallback)(CFE_TIME_SynchCallbackPtr_t);
    int32   (*CFE_TIME_UnregisterSynchCallback)(CFE_TIME_SynchCallbackPtr_t);
} UTF_TIME_HookTable_t;

UTF_TIME_HookTable_t UTF_TIME_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL
};

/* Return Code Stuff */
int32 cfe_time_api_return_value[NUM_OF_CFE_TIME_API_PROCS]=
{
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE
};

/******************************************************************************
**  Function: UTF_TIME_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a SB function call.
*/
void UTF_TIME_set_function_hook(int Index, void *FunPtr)
{
    if      (Index == CFE_TIME_GETMETSECONDS_HOOK)           { UTF_TIME_HookTable.CFE_TIME_GetMETseconds = FunPtr; }
    else if (Index == CFE_TIME_GETMETSUBSECS_HOOK)           { UTF_TIME_HookTable.CFE_TIME_GetMETsubsecs = FunPtr; }
    else if (Index == CFE_TIME_REGISTERSYNCHCALLBACK_HOOK)   { UTF_TIME_HookTable.CFE_TIME_RegisterSynchCallback = FunPtr; }
    else if (Index == CFE_TIME_UNREGISTERSYNCHCALLBACK_HOOK) { UTF_TIME_HookTable.CFE_TIME_UnregisterSynchCallback = FunPtr; }
    else                                       { UTF_error("Invalid TIME Hook Index In Set Hook Call %d", Index); }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TIME_Set_Api_Return_Code() --                                   */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_TIME_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode)
{
    /* Check that ApiIndex is in the valid range */
    if ( (ApiIndex >= NUM_OF_CFE_TIME_API_PROCS ) || (ApiIndex < 0 ) )
    {
        UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_TIME_Set_Api_Return_Code\n", ApiIndex);
        return -1;
    }
    /* If index is valid, assign return value and report action */
    cfe_time_api_return_value[ApiIndex] = ReturnCode;

    if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        UTF_put_text("UTF Info: Return code for CFE TIME Api #%d returned to default\n", ApiIndex);
    }
    else
    {
        UTF_put_text("UTF Info: Return value for CFE TIME Api #%d set to %lx\n", ApiIndex, ReturnCode);
    }
    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TIME_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_TIME_Use_Default_Api_Return_Code(int32 ApiIndex)
{
    int32 Success;

    /* Check that ApiIndex is in the valid range */
    if ( (ApiIndex >= NUM_OF_CFE_TIME_API_PROCS ) || (ApiIndex < 0 ) )
    {
        UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_TIME_Use_Default_Api_Return_Code\n", ApiIndex);
        return -1;
    }

    /* If index is valid, assign return value and report action */
    Success = UTF_CFE_TIME_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);

    return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_SB_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_SB_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_TIME_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;

    /* Check for correct number of arguments */
    if (argc != 3)
    {
        UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_TIME_Set_Api_Return_Code. Expected 2.\n", argc -1 );
        UTF_exit();
    }

    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);

    /* Call function to set API return code */
    UTF_CFE_TIME_Set_Api_Return_Code(Index, Code);
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TIME_Use_Default_Api_Return_Code() --                        */
/*  Script interface function for UTF_CFE_TIME_Set_Api_Return_Code.        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_TIME_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

    /* Check for correct number of arguments */
    if (argc != 2)
    {
        UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_TIME_Set_Api_Return_Code. Expected 1.\n", argc -1 );
        UTF_exit();
    }

    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);

    /* Call function to set API return code */
    UTF_CFE_TIME_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetTime() -- get time in default format (TAI vs UTC)           */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t   CFE_TIME_GetTime(void)
{
    CFE_TIME_SysTime_t CurrentTime;

#if (CFE_TIME_CFG_DEFAULT_TAI == TRUE)

    CurrentTime = CFE_TIME_GetTAI();

#else

    CurrentTime = CFE_TIME_GetUTC();

#endif

    return(CurrentTime);

} /* End of CFE_TIME_GetTime() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetTAI() -- get time in TAI format (TAI vs UTC)                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t   CFE_TIME_GetTAI(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t tai;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Calculate current TAI...
    */
    tai = CFE_TIME_CalculateTAI(&Reference);

    return(tai);

} /* End of CFE_TIME_GetTAI() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetUTC() -- get time in UTC format (TAI vs UTC)                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t   CFE_TIME_GetUTC(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t utc;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Calculate current UTC...
    */
    utc = CFE_TIME_CalculateUTC(&Reference);

    return(utc);

} /* End of CFE_TIME_GetUTC() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_METtoSCTime() -- convert MET Time to default spacecraft time   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t CFE_TIME_MET2SCTime (CFE_TIME_SysTime_t METTime)
{

    CFE_TIME_SysTime_t STCF;
    CFE_TIME_SysTime_t TIATime;
    CFE_TIME_SysTime_t UTCTime;
    CFE_TIME_SysTime_t ReturnTime;
    CFE_TIME_SysTime_t LeapSecsAsSysTime;
    int16              LeapSeconds;
    
    STCF = CFE_TIME_GetSTCF();

    /* TIA = MET + STCF */
    TIATime = CFE_TIME_Add(METTime, STCF);

    LeapSeconds = CFE_TIME_GetLeapSeconds();

    /* Put leap seconds in correct format */
    LeapSecsAsSysTime.Seconds       = LeapSeconds;
    LeapSecsAsSysTime.Subseconds    = 0;
    
    /* UTC Time = TIA Time - Leap Seconds */
    UTCTime = CFE_TIME_Subtract(TIATime, LeapSecsAsSysTime);


#if (CFE_TIME_CFG_DEFAULT_TAI == TRUE)

    ReturnTime = TIATime;

#else
    ReturnTime = UTCTime;

#endif

    return (ReturnTime);
}/* end CFE_TIME_MET2SCTime() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetClockState() -- get time data (status)                      */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_ClockState_t   CFE_TIME_GetClockState(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_ClockState_t state;
    
    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Determine the current clock state...
    */
    state = (CFE_TIME_ClockState_t) CFE_TIME_CalculateState(&Reference);

    return(state);

} /* End of CFE_TIME_GetClockState() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_GetClockInfo() -- Convert state data to flag values    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint16 CFE_TIME_GetClockInfo(void)
{
    uint16 StateFlags = 0;

    /* Handle Preset Return Code */
    if (cfe_time_api_return_value[CFE_TIME_GETCLOCKINFO_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_time_api_return_value[CFE_TIME_GETCLOCKINFO_PROC];
    }

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

    /* 
    ** The tone is good 
    */
    if (CFE_TIME_TaskData.IsToneGood == TRUE)
    {
        StateFlags |= CFE_TIME_FLAG_GDTONE;
    }   

    return(StateFlags);

} /* End of CFE_TIME_GetClockInfo() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetLeapSeconds() -- get time data (leap seconds)               */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int16   CFE_TIME_GetLeapSeconds(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.AtToneLeaps);

} /* End of CFE_TIME_GetLeapSeconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetSTCF() -- get time data (Spacecraft Time Correlation Factor)*/
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t   CFE_TIME_GetSTCF(void)
{
    CFE_TIME_Reference_t Reference;
    
    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.AtToneSTCF);

} /* End of CFE_TIME_GetSTCF() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetMET() -- get time data (Mission Elapsed Timer)              */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t   CFE_TIME_GetMET(void)
{
    CFE_TIME_Reference_t Reference;
    
     /* Zero out the Reference variable because we pass it into
     * a function before using it
     */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));


    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.CurrentMET);

} /* End of CFE_TIME_GetMET() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetMETseconds() -- get time data (seconds portion of MET)      */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32   CFE_TIME_GetMETseconds(void)
{
    CFE_TIME_Reference_t Reference;


    if (UTF_TIME_HookTable.CFE_TIME_GetMETseconds)
        return(UTF_TIME_HookTable.CFE_TIME_GetMETseconds());

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.CurrentMET.Seconds);

} /* End of CFE_TIME_GetMETseconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetMETsubsecs() -- get time data (sub-seconds portion of MET)  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32   CFE_TIME_GetMETsubsecs(void)
{
    CFE_TIME_Reference_t Reference;


    if (UTF_TIME_HookTable.CFE_TIME_GetMETsubsecs)
        return(UTF_TIME_HookTable.CFE_TIME_GetMETsubsecs());

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    CFE_PSP_MemSet(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.CurrentMET.Subseconds);

} /* End of CFE_TIME_GetMETsubsecs() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Add() -- add two CFE_TIME_SysTime_t values                     */
/*                                                                         */
/*   result = time1 + time2                                                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t  CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t Result;

    Result.Subseconds = Time1.Subseconds + Time2.Subseconds;

    /*
    ** Check for sub-seconds roll-over
    */
    if (Result.Subseconds < Time1.Subseconds)
    {
        Result.Seconds = (Time1.Seconds + Time2.Seconds) + 1;
    }
    else
    {
        Result.Seconds = Time1.Seconds + Time2.Seconds;
    }

    return(Result);

} /* End of CFE_TIME_Add() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Subtract() -- subtract a CFE_TIME_SysTime_t value from another */
/*                                                                         */
/*   result = time1 - time2                                                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t  CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t Result;

    Result.Subseconds = Time1.Subseconds - Time2.Subseconds;

    if (Result.Subseconds > Time1.Subseconds)
    {
        Result.Seconds = (Time1.Seconds - Time2.Seconds) - 1;
    }
    else
    {
        Result.Seconds = Time1.Seconds - Time2.Seconds;
    }

    return(Result);

} /* End of CFE_TIME_Subtract() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Compare() -- compare two CFE_TIME_SysTime_t values             */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_Compare_t  CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    CFE_TIME_Compare_t Result;

    if (TimeA.Seconds > TimeB.Seconds)
    {
        /*
        ** Assume rollover if difference is too large...
        */
        if ((TimeA.Seconds - TimeB.Seconds) > CFE_TIME_NEGATIVE)
        {
            Result = CFE_TIME_A_LT_B;
        }
        else
        {
            Result = CFE_TIME_A_GT_B;
        }
    }
    else if (TimeA.Seconds < TimeB.Seconds)
    {
        /*
        ** Assume rollover if difference is too large...
        */
        if ((TimeB.Seconds - TimeA.Seconds) > CFE_TIME_NEGATIVE)
        {
            Result = CFE_TIME_A_GT_B;
        }
        else
        {
            Result = CFE_TIME_A_LT_B;
        }
    }
    else
    {
        /*
        ** Seconds are equal, check sub-seconds
        */
        if (TimeA.Subseconds > TimeB.Subseconds)
        {
            Result = CFE_TIME_A_GT_B;
        }
        else if (TimeA.Subseconds < TimeB.Subseconds)
        {
            Result = CFE_TIME_A_LT_B;
        }
        else
        {
            Result = CFE_TIME_EQUAL;
        }
    }

    return(Result);

} /* End of CFE_TIME_Compare() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Sub2MicroSecs() -- convert sub-seconds to micro-seconds        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32  CFE_TIME_Sub2MicroSecs(uint32 SubSeconds)
{
    uint32 MicroSeconds;
	
    /* 0xffffdf00 subseconds = 999999 microseconds, so anything greater 
     * than that we set to 999999 microseconds, so it doesn't get to
     * a million microseconds */
    
	if (SubSeconds > 0xffffdf00)
	{
			MicroSeconds = 999999;
	}
    else
    {
        /*
        **  Convert a 1/2^32 clock tick count to a microseconds count
        **
        **  Conversion factor is  ( ( 2 ** -32 ) / ( 10 ** -6 ) ).
        **
        **  Logic is as follows:
        **    x * ( ( 2 ** -32 ) / ( 10 ** -6 ) )
        **  = x * ( ( 10 ** 6  ) / (  2 ** 32 ) )
        **  = x * ( ( 5 ** 6 ) ( 2 ** 6 ) / ( 2 ** 26 ) ( 2 ** 6) )
        **  = x * ( ( 5 ** 6 ) / ( 2 ** 26 ) )
        **  = x * ( ( 5 ** 3 ) ( 5 ** 3 ) / ( 2 ** 7 ) ( 2 ** 7 ) (2 ** 12) )
        **
        **  C code equivalent:
        **  = ( ( ( ( ( x >> 7) * 125) >> 7) * 125) >> 12 )
        */   

    	MicroSeconds = (((((SubSeconds >> 7) * 125) >> 7) * 125) >> 12);
    

        /* if the Subseconds % 0x4000000 != 0 then we will need to
         * add 1 to the result. the & is a faster way of doing the % */  
	    if ((SubSeconds & 0x3ffffff) != 0)
    	{
	    	MicroSeconds++;
    	}
    
        /* In the Micro2SubSecs conversion, we added an extra anomaly
         * to get the subseconds to bump up against the end point,
         * 0xFFFFF000. This must be accounted for here. Since we bumped
         * at the half way mark, we must "unbump" at the same mark 
         */
        if (MicroSeconds > 500000)
        {
            MicroSeconds --;
        }
        
    } /* end else */
    
    return(MicroSeconds);

} /* End of CFE_TIME_Sub2MicroSecs() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Micro2SubSecs() -- convert micro-seconds to sub-seconds        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32  CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    uint32 SubSeconds;

    /*
    ** Conversion amount must be less than one second
    */
    if (MicroSeconds > 999999)
    {
        SubSeconds = 0xFFFFFFFF;
    }
    else
    {
    /*
    **  Convert micro-seconds count to sub-seconds (1/2^32) count
    **
    **  Conversion factor is  ( ( 10 ** -6 ) / ( 2 ** -20 ).
    **
    **  Logic is as follows:
    **    x * ( ( 10 ** -6 ) / ( 2 ** -32 ) )
    **  = x * ( ( 2 ** 32 ) / ( 10 ** 6 ) )
    **  = x * ( ( ( 2 ** 26 ) ( 2 ** 6) ) / ( ( 5 ** 6 ) ( 2 ** 6 ) ) )
    **  = x * ( ( 2 ** 26 ) / ( 5 ** 6 ) )
    **  = x * ( ( ( 2 ** 11) ( 2 ** 3) (2 ** 12) ) / ( 5( 5 ** 5 ) ) )
    **  = x * ( ( ( ( ( 2 ** 11 ) / 5 ) * ( 2 ** 3 ) ) / ( 5 ** 5 ) ) * (2 ** 12) )
    **
    **  C code equivalent:
    **  = ( ( ( ( ( x << 11 ) / 5 ) << 3 ) / 3125 ) << 12 )
    **
    **  Conversion factor was reduced and factored accordingly
    **  to minimize precision loss and register overflow.
    */
        SubSeconds = ( ( ( ( MicroSeconds << 11 ) / 5 ) << 3 ) / 3125 ) << 12;

        /* To get the SubSeconds to "bump up" against 0xFFFFF000 when 
         * MicroSeconds = 9999999, we add in another anomaly to the 
         * conversion at the half-way point  (500000 us). This will bump
         * all of the subseconds up by 0x1000, so 999999 us == 0xFFFFF00,
         * 999998 == 0xFFFFE000, etc. This extra anomaly is accounted for
         * in the Sub2MicroSecs conversion as well.
         */
        
        if (SubSeconds > 0x80001000)
        {
           SubSeconds += 0x1000;
        }

    }

    return(SubSeconds);

} /* End of CFE_TIME_Micro2SubSecs() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_CFE2FSSeconds() --  convert cFE seconds to file system seconds */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32 CFE_TIME_CFE2FSSeconds(uint32 SecondsCFE)
{
    /*
    ** Using a signed integer allows the factor to be negative...
    */
    int32 ConvertFactor = CFE_TIME_FS_FACTOR;

    /*
    ** File system time = cFE time + conversion factor...
    */
    uint32 SecondsFS = SecondsCFE + (uint32) ConvertFactor;

    /*
    ** Prevent file system time from going below zero...
    */
    if (ConvertFactor < 0)
    {
        if (-ConvertFactor > SecondsCFE)
        {
            SecondsFS = 0;
        }
    }

    return(SecondsFS);

} /* End of CFE_TIME_CFE2FSSeconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_FS2CFESeconds() --  convert file system seconds to cFE seconds */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32 CFE_TIME_FS2CFESeconds(uint32 SecondsFS)
{
    /*
    ** Using a signed integer allows the factor to be negative...
    */
    int32 ConvertFactor = CFE_TIME_FS_FACTOR;

    /*
    ** cFE time = file system time - conversion factor...
    */
    uint32 SecondsCFE = SecondsFS - (uint32) ConvertFactor;

    /*
    ** Prevent cFE time from going below zero...
    */
    if (ConvertFactor > SecondsFS)
    {
        SecondsCFE = 0;
    }

    return(SecondsCFE);

} /* End of CFE_TIME_FS2CFESeconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Print() --  create text string representing date and time      */
/*                                                                         */
/* Notes:                                                                  */
/*                                                                         */
/*  1) The value of the time argument is simply added to the configuration */
/*     definitions for the ground epoch and converted into a fixed length  */
/*     string in the buffer provided by the caller.                        */
/*                                                                         */
/*  2) A loss of data during the string conversion will occur if the       */
/*     computed year exceeds 9999.  However, a year that large would       */
/*     require an unrealistic definition for the ground epoch since        */
/*     the maximum amount of time represented by a CFE_TIME_SysTime        */
/*     structure is approximately 136 years.                               */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    uint32 NumberOfYears;
    uint32 NumberOfDays;
    uint32 NumberOfHours;
    uint32 NumberOfMinutes;
    uint32 NumberOfSeconds;
    uint32 NumberOfMicros;
    uint32 DaysInThisYear;

    boolean StillCountingYears = TRUE;

    /*
    ** Convert the cFE time (offset from epoch) into calendar time...
    */
    NumberOfMinutes = (TimeToPrint.Seconds / 60) + CFE_TIME_EPOCH_MINUTE;
    NumberOfSeconds = (TimeToPrint.Seconds % 60) + CFE_TIME_EPOCH_SECOND;

    /*
    ** Adding the epoch "seconds" after computing the minutes avoids
    **    overflow problems when the input time value (seconds) is
    **    at, or near, 0xFFFFFFFF...
    */
    while (NumberOfSeconds >= 60)
    {
        NumberOfMinutes++;
        NumberOfSeconds -= 60;
    }

    /*
    ** Compute the years/days/hours/minutes...
    */
    NumberOfHours   = (NumberOfMinutes / 60) + CFE_TIME_EPOCH_HOUR;
    NumberOfMinutes = (NumberOfMinutes % 60);

    /*
    ** Unlike hours and minutes, epoch days are counted as Jan 1 = day 1...
    */
    NumberOfDays  = (NumberOfHours / 24) + (CFE_TIME_EPOCH_DAY - 1);
    NumberOfHours = (NumberOfHours % 24);

    NumberOfYears = CFE_TIME_EPOCH_YEAR;

    /*
    ** Convert total number of days into years and remainder days...
    */
    while (StillCountingYears)
    {
        /*
        ** Set number of days in this year (leap year?)...
        */
        DaysInThisYear = 365;

        if ((NumberOfYears % 4) == 0)
        {
            if ((NumberOfYears % 100) != 0)
            {
                DaysInThisYear = 366;
            }
            else if ((NumberOfYears % 400) == 0)
            {
                DaysInThisYear = 366;
            }
        }

        /*
        ** When we have less than a years worth of days, we're done...
        */
        if (NumberOfDays < DaysInThisYear)
        {
            StillCountingYears = FALSE;
        }
        else
        {
            /*
            ** Add a year and remove the number of days in that year...
            */
            NumberOfYears++;
            NumberOfDays -= DaysInThisYear;
        }
    }

    /*
    ** Unlike hours and minutes, days are displayed as Jan 1 = day 1...
    */
    NumberOfDays++;

    /*
    ** After computing microseconds, convert to 5 digits from 6 digits...
    */
    NumberOfMicros = CFE_TIME_Sub2MicroSecs(TimeToPrint.Subseconds) / 10;

    /*
    ** Build formatted output string (yyyy-ddd-hh:mm:ss.xxxxx)...
    */
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 1000); NumberOfYears = NumberOfYears % 1000;
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 100);  NumberOfYears = NumberOfYears % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfYears % 10);
    *PrintBuffer++ = '-';

    *PrintBuffer++ = '0' + (char) (NumberOfDays / 100); NumberOfDays = NumberOfDays % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfDays / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfDays % 10);
    *PrintBuffer++ = '-';

    *PrintBuffer++ = '0' + (char) (NumberOfHours / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfHours % 10);
    *PrintBuffer++ = ':';

    *PrintBuffer++ = '0' + (char) (NumberOfMinutes / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfMinutes % 10);
    *PrintBuffer++ = ':';

    *PrintBuffer++ = '0' + (char) (NumberOfSeconds / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfSeconds % 10);
    *PrintBuffer++ = '.';

    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 10000); NumberOfMicros = NumberOfMicros % 10000;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 1000);  NumberOfMicros = NumberOfMicros % 1000;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 100);   NumberOfMicros = NumberOfMicros % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfMicros % 10);
    *PrintBuffer++ = '\0';

    return;

} /* End of CFE_TIME_Print() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalTone() --  external tone signal was detected           */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_TIME_ExternalTone(void)
{
    /*
    ** Call tone signal ISR (OK if called from non-ISR context)...
    */
    CFE_TIME_Tone1HzISR();

    return;

} /* End of CFE_TIME_ExternalTone() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_RegisterSynchCallback() --  register synch callback func       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32  CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)   
{
    int32  Status = CFE_SUCCESS;
    uint32 i = 0;

    if (UTF_TIME_HookTable.CFE_TIME_RegisterSynchCallback)
        return(UTF_TIME_HookTable.CFE_TIME_RegisterSynchCallback(CallbackFuncPtr));

    /* Handle Preset Return Code */
    if (cfe_time_api_return_value[CFE_TIME_REGISTERSYNCHCALLBACK_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_time_api_return_value[CFE_TIME_REGISTERSYNCHCALLBACK_PROC];
    }

    while ((i<CFE_TIME_MAX_NUM_SYNCH_FUNCS) && (CFE_TIME_TaskData.SynchCallback[i].Ptr != NULL))
    {
        i++;
    }
    
    if (i<CFE_TIME_MAX_NUM_SYNCH_FUNCS)
    {
        Status = CFE_ES_GetAppID(&CFE_TIME_TaskData.SynchCallback[i].App);
        if (Status == CFE_SUCCESS)
        {
            CFE_TIME_TaskData.SynchCallback[i].Ptr = CallbackFuncPtr;
        }
    }
    else
    {
        Status = CFE_TIME_TOO_MANY_SYNCH_CALLBACKS;
    }
    
    return Status;
} /* End of CFE_TIME_RegisterSynchCallback() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_UnregisterSynchCallback() --  unregister synch callback func   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32  CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)   
{
    int32  Status = CFE_SUCCESS;
    uint32 i = 0;
    uint32 AppId;
    
    if (UTF_TIME_HookTable.CFE_TIME_UnregisterSynchCallback)
        return(UTF_TIME_HookTable.CFE_TIME_UnregisterSynchCallback(CallbackFuncPtr));

    /* Handle Preset Return Code */
    if (cfe_time_api_return_value[CFE_TIME_UNREGISTERSYNCHCALLBACK_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_time_api_return_value[CFE_TIME_UNREGISTERSYNCHCALLBACK_PROC];
    }

    Status = CFE_ES_GetAppID(&AppId);
    
    if (Status == CFE_SUCCESS)
    {
        while (i<CFE_TIME_MAX_NUM_SYNCH_FUNCS) 
        {
            if ((CFE_TIME_TaskData.SynchCallback[i].App == AppId) &&
                (CFE_TIME_TaskData.SynchCallback[i].Ptr == CallbackFuncPtr))
            {
                break;
            }
            
            i++;
        }
        
        if (i<CFE_TIME_MAX_NUM_SYNCH_FUNCS)
        {
            CFE_TIME_TaskData.SynchCallback[i].App = 0;
            CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
        }
        else
        {
            Status = CFE_TIME_CALLBACK_NOT_REGISTERED;
        }
            
    }
    
    return Status;
} /* End of CFE_TIME_UnregisterSynchCallback() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalMET() -- time data from external source (MET)          */
/*                                                                         */
/* Notes: 1) Time Servers use this call as the signal to send the          */
/*           "time at the tone" command packet.                            */
/*                                                                         */
/*        2) This function is only included in the API if the Time         */
/*           Server has been configured to accept time data from an        */
/*           external source (MET).                                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if (CFE_TIME_CFG_SRC_MET == TRUE)
void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET)
{
    /*
    ** Process external MET data...
    */
    CFE_TIME_ToneSendMET(NewMET);

    return;

} /* End of CFE_TIME_ExternalMET() */
#endif /* CFE_TIME_CFG_SRC_MET  */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalGPS() -- time data from external source (GPS)          */
/*                                                                         */
/* Notes: 1) Time Servers use this call as the signal to send the          */
/*           "time at the tone" command packet.                            */
/*                                                                         */
/*        2) This function is only included in the API if the Time         */
/*           Server has been configured to accept time data from an        */
/*           external source (GPS).                                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if (CFE_TIME_CFG_SRC_GPS == TRUE)
void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps)
{
    /*
    ** Process external GPS time data...
    */
    CFE_TIME_ToneSendGPS(NewTime, NewLeaps);

    return;

} /* End of CFE_TIME_ExternalGPS() */
#endif /* CFE_TIME_CFG_SRC_GPS */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalTime() -- time data from external source (S/C, etc.)   */
/*                                                                         */
/* Notes: 1) Time Servers use this call as the signal to send the          */
/*           "time at the tone" command packet.                            */
/*                                                                         */
/*        2) This function is only included in the API if the Time         */
/*           Server has been configured to accept time data from an        */
/*           external source (spacecraft time).                            */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if (CFE_TIME_CFG_SRC_TIME == TRUE)
void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime)
{
    /*
    ** Process external time data...
    */
    CFE_TIME_ToneSendTime(NewTime);

    return;

} /* End of CFE_TIME_ExternalTime() */
#endif /* CFE_TIME_CFG_SRC_TIME */


/************************/
/*  End of File Comment */
/************************/
