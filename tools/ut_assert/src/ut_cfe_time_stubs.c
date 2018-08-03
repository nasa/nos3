/*
**
** File: ut_cfe_time_stubs.c
**
** $Id: ut_cfe_time_stubs.c 1.3 2016/06/23 15:08:51EDT czogby Exp  $
**
** Copyright 2012-2013 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Other Rights Reserved.  
**
** This software was created at NASA's Goddard Space Flight Center.
** This software is governed by the NASA Open Source Agreement and may be 
** used, distributed and modified only pursuant to the terms of that 
** agreement.
**
** Purpose: Unit test stubs for cFE Time Services routines
**
** $Log: ut_cfe_time_stubs.c  $
** Revision 1.3 2016/06/23 15:08:51EDT czogby 
** UT-Assert: Add Missing Stub Functions
** Revision 1.2 2016/03/13 19:52:51EDT czogby 
** Add Missing Stub Functions to UT-Assert Library
** Revision 1.1 2016/02/08 20:53:31EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.6 2015/06/16 16:14:03EDT sstrege 
** Added copyright information
** Revision 1.5 2015/05/07 14:39:51EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.4 2015/05/01 14:25:01EDT czogby 
** Add UtAssert fail where error messages, add CFE_TIME_Print, add CFE_TIME_Compare
** Revision 1.3 2015/03/17 18:24:12EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:32:00Z]
** Added missing functions
** Revision 1.2 2015/03/10 15:21:48EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.1 2011/02/15 11:13:05EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "utassert.h"
#include "ut_cfe_time_stubs.h" 
#include "ut_cfe_time_hooks.h"
#include <string.h>

Ut_CFE_TIME_HookTable_t         Ut_CFE_TIME_HookTable;
Ut_CFE_TIME_ReturnCodeTable_t   Ut_CFE_TIME_ReturnCodeTable[UT_CFE_TIME_MAX_INDEX];

void Ut_CFE_TIME_Reset(void)
{
    memset(&Ut_CFE_TIME_HookTable, 0, sizeof(Ut_CFE_TIME_HookTable));
    memset(&Ut_CFE_TIME_ReturnCodeTable, 0, sizeof(Ut_CFE_TIME_ReturnCodeTable));

    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_ADD_INDEX, &Ut_CFE_TIME_AddHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_SUBTRACT_INDEX, &Ut_CFE_TIME_SubtractHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_SUB2MICROSECS_INDEX, &Ut_CFE_TIME_Sub2MicroSecsHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_MICRO2SUBSECS_INDEX, &Ut_CFE_TIME_Micro2SubSecsHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_PRINT_INDEX, &Ut_CFE_TIME_PrintHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &Ut_CFE_TIME_CompareHook);
}

void Ut_CFE_TIME_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_TIME_GETUTC_INDEX)        { Ut_CFE_TIME_HookTable.CFE_TIME_GetUTC = FunPtr; }
    else if (Index == UT_CFE_TIME_GETTAI_INDEX)        { Ut_CFE_TIME_HookTable.CFE_TIME_GetTAI = FunPtr; }
    else if (Index == UT_CFE_TIME_GETTIME_INDEX)       { Ut_CFE_TIME_HookTable.CFE_TIME_GetTime = FunPtr; }
    else if (Index == UT_CFE_TIME_MICRO2SUBSECS_INDEX) { Ut_CFE_TIME_HookTable.CFE_TIME_Micro2SubSecs = FunPtr; }
    else if (Index == UT_CFE_TIME_SUB2MICROSECS_INDEX) { Ut_CFE_TIME_HookTable.CFE_TIME_Sub2MicroSecs = FunPtr; }
    else if (Index == UT_CFE_TIME_ADD_INDEX)           { Ut_CFE_TIME_HookTable.CFE_TIME_Add = FunPtr; }
    else if (Index == UT_CFE_TIME_SUBTRACT_INDEX)      { Ut_CFE_TIME_HookTable.CFE_TIME_Subtract = FunPtr; }
    else if (Index == UT_CFE_TIME_PRINT_INDEX)         { Ut_CFE_TIME_HookTable.CFE_TIME_Print = FunPtr; }
    else if (Index == UT_CFE_TIME_COMPARE_INDEX)       { Ut_CFE_TIME_HookTable.CFE_TIME_Compare = FunPtr; }
    else if (Index == UT_CFE_TIME_FS2CFESECONDS_INDEX) { Ut_CFE_TIME_HookTable.CFE_TIME_FS2CFESeconds = FunPtr; }
    else
    {
        printf("Unsupported TIME Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported TIME Index In SetFunctionHook Call");
    }
}

void Ut_CFE_TIME_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_TIME_MAX_INDEX)
    {
        Ut_CFE_TIME_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_TIME_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported TIME Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported TIME Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_TIME_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_TIME_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_TIME_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_TIME_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_TIME_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_TIME_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_TIME_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

CFE_TIME_SysTime_t  CFE_TIME_GetUTC(void)
{
    CFE_TIME_SysTime_t  Time;
    
    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_GetUTC)
        return Ut_CFE_TIME_HookTable.CFE_TIME_GetUTC();
    
    Time.Seconds = 0;
    Time.Subseconds = 0;
    
    return Time;
}

CFE_TIME_SysTime_t  CFE_TIME_GetTAI(void)
{
    CFE_TIME_SysTime_t  Time;
    
    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_GetTAI)
        return Ut_CFE_TIME_HookTable.CFE_TIME_GetTAI();
    
    Time.Seconds = 0;
    Time.Subseconds = 0;
    
    return Time;
}

CFE_TIME_SysTime_t  CFE_TIME_GetTime(void)
{
    CFE_TIME_SysTime_t  Time;
    
    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_GetTime)
        return Ut_CFE_TIME_HookTable.CFE_TIME_GetTime();
    
    Time.Seconds = 0;
    Time.Subseconds = 0;
    
    return Time;
}

uint32  CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    /* Check for specified return */
    if (Ut_CFE_TIME_UseReturnCode(UT_CFE_TIME_MICRO2SUBSECS_INDEX))
        return Ut_CFE_TIME_ReturnCodeTable[UT_CFE_TIME_MICRO2SUBSECS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_Micro2SubSecs)
        return Ut_CFE_TIME_HookTable.CFE_TIME_Micro2SubSecs(MicroSeconds);
    
    return(0);
}

uint32  CFE_TIME_Sub2MicroSecs(uint32 SubSeconds)
{
    /* Check for specified return */
    if (Ut_CFE_TIME_UseReturnCode(UT_CFE_TIME_SUB2MICROSECS_INDEX))
        return Ut_CFE_TIME_ReturnCodeTable[UT_CFE_TIME_SUB2MICROSECS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_Sub2MicroSecs)
        return Ut_CFE_TIME_HookTable.CFE_TIME_Sub2MicroSecs(SubSeconds);
    
    return(0);
}

CFE_TIME_SysTime_t  CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t  Time;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_Add)
        return Ut_CFE_TIME_HookTable.CFE_TIME_Add(Time1, Time2);
    
    Time.Seconds = 0;
    Time.Subseconds = 0;

    return Time;
}

CFE_TIME_SysTime_t  CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t  Time;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_Subtract)
        return Ut_CFE_TIME_HookTable.CFE_TIME_Subtract(Time1, Time2);
 
    Time.Seconds = 0;
    Time.Subseconds = 0;

    return Time;
}

void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_Print)
        Ut_CFE_TIME_HookTable.CFE_TIME_Print(PrintBuffer, TimeToPrint);

    return;
}

CFE_TIME_Compare_t  CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_Compare)
        return Ut_CFE_TIME_HookTable.CFE_TIME_Compare(TimeA, TimeB);

    return TRUE;
}

uint32  CFE_TIME_FS2CFESeconds(uint32 SecondsFS)
{
    /* Check for specified return */
    if (Ut_CFE_TIME_UseReturnCode(UT_CFE_TIME_FS2CFESECONDS_INDEX))
        return Ut_CFE_TIME_ReturnCodeTable[UT_CFE_TIME_FS2CFESECONDS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_FS2CFESeconds)
        return Ut_CFE_TIME_HookTable.CFE_TIME_FS2CFESeconds(SecondsFS);
    
    return(0);
}

int32  CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    /* Check for specified return */
    if (Ut_CFE_TIME_UseReturnCode(UT_CFE_TIME_REGISTERSYNCHCALLBACK_INDEX))
        return Ut_CFE_TIME_ReturnCodeTable[UT_CFE_TIME_REGISTERSYNCHCALLBACK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_RegisterSynchCallback)
        return Ut_CFE_TIME_HookTable.CFE_TIME_RegisterSynchCallback(CallbackFuncPtr);
    
    return(0);
}

int32  CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr) 
{
    /* Check for specified return */
    if (Ut_CFE_TIME_UseReturnCode(UT_CFE_TIME_UNREGISTERSYNCHCALLBACK_INDEX))
        return Ut_CFE_TIME_ReturnCodeTable[UT_CFE_TIME_UNREGISTERSYNCHCALLBACK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TIME_HookTable.CFE_TIME_UnregisterSynchCallback)
        return Ut_CFE_TIME_HookTable.CFE_TIME_UnregisterSynchCallback(CallbackFuncPtr);
    
    return(0);
}
