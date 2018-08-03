/*
**
** File:  ut_cfe_evs_stubs.c
**
** $Id: ut_cfe_evs_stubs.c 1.2 2016/07/08 17:22:25EDT czogby Exp  $
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
** Purpose: Unit test stubs for cFE Event Services routines
**
** $Log: ut_cfe_evs_stubs.c  $
** Revision 1.2 2016/07/08 17:22:25EDT czogby 
** Fix bug with event messages causing core dump when over limit
** Revision 1.1 2016/02/08 20:53:30EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.6 2015/06/16 16:14:01EDT sstrege 
** Added copyright information
** Revision 1.5 2015/05/07 14:39:51EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.4 2015/05/01 13:12:51EDT czogby 
** Added UtAssert call that fails to places where an error message is generated
** Revision 1.3 2015/03/17 18:21:47EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:25:56Z]
** Added missing checks for hook functions / return values
** Revision 1.2 2015/03/10 15:21:45EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.1 2011/02/15 11:13:02EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

/*
** Include section
*/

#include "cfe.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include <string.h>

Ut_CFE_EVS_HookTable_t          Ut_CFE_EVS_HookTable;
Ut_CFE_EVS_ReturnCodeTable_t    Ut_CFE_EVS_ReturnCodeTable[UT_CFE_EVS_MAX_INDEX];

void Ut_CFE_EVS_Reset(void)
{
    memset(&Ut_CFE_EVS_HookTable, 0, sizeof(Ut_CFE_EVS_HookTable));
    memset(&Ut_CFE_EVS_ReturnCodeTable, 0, sizeof(Ut_CFE_EVS_ReturnCodeTable));
    
    Ut_CFE_EVS_SetFunctionHook(UT_CFE_EVS_SENDEVENT_INDEX, &Ut_CFE_EVS_SendEventHook);
    Ut_CFE_EVS_ClearEventQueue();
}

void Ut_CFE_EVS_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_EVS_REGISTER_INDEX)    { Ut_CFE_EVS_HookTable.CFE_EVS_Register = FunPtr; }
    else if (Index == UT_CFE_EVS_SENDEVENT_INDEX)   { Ut_CFE_EVS_HookTable.CFE_EVS_SendEvent = FunPtr; }
    else
    {
        printf("Unsupported EVS Index In SetFunctionHook Call %lu", Index);
        UtAssert_True(FALSE, "Unsupported EVS Index In SetFunctionHook Call");
    }
}

void Ut_CFE_EVS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_EVS_MAX_INDEX)
    {
        Ut_CFE_EVS_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_EVS_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported EVS Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported EVS Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_EVS_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_EVS_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_EVS_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_EVS_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_EVS_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_EVS_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_EVS_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_EVS_Register (void *Filters, uint16 NumEventFilters, uint16 FilterScheme)
{
    /* Check for specified return */
    if (Ut_CFE_EVS_UseReturnCode(UT_CFE_EVS_REGISTER_INDEX))
        return Ut_CFE_EVS_ReturnCodeTable[UT_CFE_EVS_REGISTER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_EVS_HookTable.CFE_EVS_Register)
        return(Ut_CFE_EVS_HookTable.CFE_EVS_Register(Filters, NumEventFilters, FilterScheme));

    return CFE_SUCCESS;
}

int32 CFE_EVS_SendEvent (uint16 EventID, uint16 EventType, const char *Spec, ... )
{
    char     BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
    va_list  Ptr;
  
    va_start(Ptr, Spec);
    vsnprintf(BigBuf, CFE_EVS_MAX_MESSAGE_LENGTH, Spec, Ptr);
    va_end(Ptr);

    /* Check for specified return */
    if (Ut_CFE_EVS_UseReturnCode(UT_CFE_EVS_SENDEVENT_INDEX))
        return Ut_CFE_EVS_ReturnCodeTable[UT_CFE_EVS_SENDEVENT_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_EVS_HookTable.CFE_EVS_SendEvent)
         return(Ut_CFE_EVS_HookTable.CFE_EVS_SendEvent(EventID, EventType, BigBuf));
 
    return CFE_SUCCESS;
}

int32 CFE_EVS_SendTimedEvent (CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, const char *Spec, ... )
{
    /* Check for specified return */
    if (Ut_CFE_EVS_UseReturnCode(UT_CFE_EVS_SENDTIMEDEVENT_INDEX))
        return Ut_CFE_EVS_ReturnCodeTable[UT_CFE_EVS_SENDTIMEDEVENT_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_EVS_HookTable.CFE_EVS_SendTimedEvent)
         return(Ut_CFE_EVS_HookTable.CFE_EVS_SendTimedEvent(Time, EventID, EventType, Spec));

    return CFE_SUCCESS;
}

int32 CFE_EVS_SendEventWithAppID (uint16 EventID, uint16 EventType, uint32 AppID, const char *Spec, ... )
{
    /* Check for specified return */
    if (Ut_CFE_EVS_UseReturnCode(UT_CFE_EVS_SENDEVENTWITHAPPID_INDEX))
        return Ut_CFE_EVS_ReturnCodeTable[UT_CFE_EVS_SENDEVENTWITHAPPID_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_EVS_HookTable.CFE_EVS_SendEventWithAppID)
         return(Ut_CFE_EVS_HookTable.CFE_EVS_SendEventWithAppID(EventID, EventType, AppID, Spec));

    return CFE_SUCCESS;
}

int32 CFE_EVS_CleanUpApp (uint32 AppId)
{
    /* Check for specified return */
    if (Ut_CFE_EVS_UseReturnCode(UT_CFE_EVS_CLEANUPAPP_INDEX))
        return Ut_CFE_EVS_ReturnCodeTable[UT_CFE_EVS_CLEANUPAPP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_EVS_HookTable.CFE_EVS_CleanUpApp)
        return(Ut_CFE_EVS_HookTable.CFE_EVS_CleanUpApp(AppId));

    return CFE_SUCCESS;
}
