/*
  Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

  This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
  limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
  for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
  any warranty that the software will be error free.

  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
  arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
  contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
  documentation or services provided hereunder

  ITC Team
  NASA IV&V
  ivv-itc@lists.nasa.gov
*/

#include "cfe.h"
#include "utassert.h"
#include "ut_ostimerapi_stubs.h"
#include <string.h>

Ut_OSTIMERAPI_HookTable_t           Ut_OSTIMERAPI_HookTable;
Ut_OSTIMERAPI_ReturnCodeTable_t     Ut_OSTIMERAPI_ReturnCodeTable[UT_OSTIMERAPI_MAX_INDEX];

void Ut_OSTIMERAPI_Reset(void)
{
    memset(&Ut_OSTIMERAPI_HookTable, 0, sizeof(Ut_OSTIMERAPI_HookTable));
    memset(&Ut_OSTIMERAPI_ReturnCodeTable, 0, sizeof(Ut_OSTIMERAPI_ReturnCodeTable));
}

void Ut_OSTIMERAPI_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_OSTIMERAPI_CREATE_INDEX)     { Ut_OSTIMERAPI_HookTable.OS_TimerCreate = FunPtr; }
    else if (Index == UT_OSTIMERAPI_SET_INDEX)        { Ut_OSTIMERAPI_HookTable.OS_TimerSet = FunPtr; }
    else if (Index == UT_OSTIMERAPI_DELETE_INDEX)     { Ut_OSTIMERAPI_HookTable.OS_TimerDelete = FunPtr; }
    else if (Index == UT_OSTIMERAPI_GETINFO_INDEX)    { Ut_OSTIMERAPI_HookTable.OS_TimerGetInfo = FunPtr; }
    else
    {
        printf("Unsupported OSTIMERAPI Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported OSTIMERAPI Index In SetFunctionHook Call");
    }
}

void Ut_OSTIMERAPI_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_OSTIMERAPI_MAX_INDEX)
    {
        Ut_OSTIMERAPI_ReturnCodeTable[Index].Value = RtnVal;
        Ut_OSTIMERAPI_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported OSTIMERAPI Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported OSTIMERAPI Index In SetReturnCode Call");
    }
}

boolean Ut_OSTIMERAPI_UseReturnCode(uint32 Index)
{
    if (Ut_OSTIMERAPI_ReturnCodeTable[Index].Count > 0)
    {
        Ut_OSTIMERAPI_ReturnCodeTable[Index].Count--;
        if (Ut_OSTIMERAPI_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_OSTIMERAPI_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }

    return(FALSE);
}

void Ut_OSTIMERAPI_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_OSTIMERAPI_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

/*
** Standard Timer system API
*/

int32 OS_TimerCreate(uint32 *timer_id, const char *timer_name, uint32 *clock_accuracy, OS_TimerCallback_t callback_ptr)
{
    /* Check for specified return */
    if (Ut_OSTIMERAPI_UseReturnCode(UT_OSTIMERAPI_CREATE_INDEX))
        return Ut_OSTIMERAPI_ReturnCodeTable[UT_OSTIMERAPI_CREATE_INDEX].Value;
 
    /* Check for Function Hook */
    if (Ut_OSTIMERAPI_HookTable.OS_TimerCreate)
        return Ut_OSTIMERAPI_HookTable.OS_TimerCreate(timer_id, timer_name, clock_accuracy, callback_ptr);

    return OS_SUCCESS;
}

int32 OS_TimerSet(uint32 timer_id, uint32 start_time, uint32 interval_time)
{
    /* Check for specified return */
    if (Ut_OSTIMERAPI_UseReturnCode(UT_OSTIMERAPI_SET_INDEX))
        return Ut_OSTIMERAPI_ReturnCodeTable[UT_OSTIMERAPI_SET_INDEX].Value;
 
    /* Check for Function Hook */
    if (Ut_OSTIMERAPI_HookTable.OS_TimerSet)
        return Ut_OSTIMERAPI_HookTable.OS_TimerSet(timer_id, start_time, interval_time);

    return OS_SUCCESS;
}

int32 OS_TimerDelete(uint32 timer_id)
{
    /* Check for specified return */
    if (Ut_OSTIMERAPI_UseReturnCode(UT_OSTIMERAPI_DELETE_INDEX))
        return Ut_OSTIMERAPI_ReturnCodeTable[UT_OSTIMERAPI_DELETE_INDEX].Value;
 
    /* Check for Function Hook */
    if (Ut_OSTIMERAPI_HookTable.OS_TimerDelete)
        return Ut_OSTIMERAPI_HookTable.OS_TimerDelete(timer_id);

    return OS_SUCCESS;
}

int32 OS_TimerGetInfo(uint32 timer_id, OS_timer_prop_t *timer_prop)
{
    /* Check for specified return */
    if (Ut_OSTIMERAPI_UseReturnCode(UT_OSTIMERAPI_GETINFO_INDEX))
        return Ut_OSTIMERAPI_ReturnCodeTable[UT_OSTIMERAPI_GETINFO_INDEX].Value;
 
    /* Check for Function Hook */
    if (Ut_OSTIMERAPI_HookTable.OS_TimerGetInfo)
        return Ut_OSTIMERAPI_HookTable.OS_TimerGetInfo(timer_id, timer_prop);

    return OS_SUCCESS;
}

