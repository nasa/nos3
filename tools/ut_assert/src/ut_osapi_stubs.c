/*
** File: ut_osapi_stubs.c
**
** $Id: ut_osapi_stubs.c 1.3 2016/06/23 14:39:04EDT czogby Exp  $
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
** Purpose: Unit test stubs for OSAPI routines.
**
** $Log: ut_osapi_stubs.c  $
** Revision 1.3 2016/06/23 14:39:04EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.2 2016/03/13 20:06:43EDT czogby 
** Add Missing Stub Functions to UT-Assert Library
** Revision 1.1 2016/02/08 20:53:31EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.10 2015/06/16 16:14:04EDT sstrege 
** Added copyright information
** Revision 1.9 2015/05/27 16:28:00EDT czogby 
** Change functions returning error by default to return success
** Revision 1.8 2015/05/07 14:39:53EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.7 2015/05/01 14:27:12EDT czogby 
** Add UtAssert fail where error messages, add comments, replace printf with UtPrintf
** Revision 1.6 2015/03/10 15:21:49EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:34:51Z]
** Added missing ability to call hook functions / set return values, updated inputs to int32 OS_IntAttachHandler
** Revision 1.5 2011/05/16 16:21:55EDT rmcgraw 
** Added hook functionality to Counting Semaphore APIs
** Revision 1.4 2011/05/16 14:42:42EDT rmcgraw 
** Added SetRtnCode processing to Counting Semaphore APIs
** Revision 1.3 2011/04/01 16:02:41EDT sslegel 
** Added (void) to unused parameters to avoid compiler warnings
** Revision 1.2 2011/03/08 15:42:09EST rmcgraw 
** Added OS_CountSemGetIdByName
** Revision 1.1 2011/02/15 11:13:05EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
*/

/*
** Include section
*/
#include "cfe.h"
#include "utassert.h"
#include "ut_osapi_stubs.h"
#include <time.h>
#include <string.h>

Ut_OSAPI_HookTable_t        Ut_OSAPI_HookTable;
Ut_OSAPI_ReturnCodeTable_t  Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MAX_INDEX];

void Ut_OSAPI_Reset(void)
{
    memset(&Ut_OSAPI_HookTable, 0, sizeof(Ut_OSAPI_HookTable));
    memset(&Ut_OSAPI_ReturnCodeTable, 0, sizeof(Ut_OSAPI_ReturnCodeTable));
}

void Ut_OSAPI_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_OSAPI_TASKDELAY_INDEX)       { Ut_OSAPI_HookTable.OS_TaskDelay = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMFLUSH_INDEX)     { Ut_OSAPI_HookTable.OS_BinSemFlush = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMTAKE_INDEX)      { Ut_OSAPI_HookTable.OS_BinSemTake = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMTIMEDWAIT_INDEX) { Ut_OSAPI_HookTable.OS_BinSemTimedWait = FunPtr; }
    else if (Index == UT_OSAPI_MUTSEMTAKE_INDEX)      { Ut_OSAPI_HookTable.OS_MutSemTake = FunPtr; }
    else if (Index == UT_OSAPI_GETLOCALTIME_INDEX)    { Ut_OSAPI_HookTable.OS_GetLocalTime = FunPtr; }
    else if (Index == UT_OSAPI_QUEUEGET_INDEX)        { Ut_OSAPI_HookTable.OS_QueueGet = FunPtr; }
    else if (Index == UT_OSAPI_QUEUEPUT_INDEX)        { Ut_OSAPI_HookTable.OS_QueuePut = FunPtr; }
    else if (Index == UT_OSAPI_TASKDELETE_INDEX)      { Ut_OSAPI_HookTable.OS_TaskDelete = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMGIVE_INDEX)      { Ut_OSAPI_HookTable.OS_BinSemGive = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMCREATE_INDEX)  { Ut_OSAPI_HookTable.OS_CountSemCreate = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMDELETE_INDEX)  { Ut_OSAPI_HookTable.OS_CountSemDelete = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMGIVE_INDEX)    { Ut_OSAPI_HookTable.OS_CountSemGive = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMTAKE_INDEX)    { Ut_OSAPI_HookTable.OS_CountSemTake = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMTIMEDWAIT_INDEX)   { Ut_OSAPI_HookTable.OS_CountSemTimedWait = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMGETIDBYNAME_INDEX) { Ut_OSAPI_HookTable.OS_CountSemGetIdByName = FunPtr; }
    else if (Index == UT_OSAPI_COUNTSEMGETINFO_INDEX) { Ut_OSAPI_HookTable.OS_CountSemGetInfo = FunPtr; }
    else if (Index == UT_OSAPI_MUTSEMCREATE_INDEX)   { Ut_OSAPI_HookTable.OS_MutSemCreate = FunPtr; }
    else if (Index == UT_OSAPI_MUTSEMDELETE_INDEX)   { Ut_OSAPI_HookTable.OS_MutSemDelete = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMCREATE_INDEX)   { Ut_OSAPI_HookTable.OS_BinSemCreate = FunPtr; }
    else if (Index == UT_OSAPI_TASKINSTALLDELETEHANDLER_INDEX) { Ut_OSAPI_HookTable.OS_TaskInstallDeleteHandler = FunPtr; }
    else if (Index == UT_OSAPI_TASKSETPRIORITY_INDEX) { Ut_OSAPI_HookTable.OS_TaskSetPriority = FunPtr; }
    else if (Index == UT_OSAPI_TASKREGISTER_INDEX)  { Ut_OSAPI_HookTable.OS_TaskRegister = FunPtr; }
    else if (Index == UT_OSAPI_TASKGETID_INDEX)     { Ut_OSAPI_HookTable.OS_TaskGetId = FunPtr; }
    else if (Index == UT_OSAPI_TASKGETIDBYNAME_INDEX) { Ut_OSAPI_HookTable.OS_TaskGetIdByName = FunPtr; }
    else if (Index == UT_OSAPI_TASKGETINFO_INDEX)  { Ut_OSAPI_HookTable.OS_TaskGetInfo = FunPtr; }
    else if (Index == UT_OSAPI_QUEUECREATE_INDEX)  { Ut_OSAPI_HookTable.OS_QueueCreate = FunPtr; }
    else if (Index == UT_OSAPI_QUEUEDELETE_INDEX)  { Ut_OSAPI_HookTable.OS_QueueDelete = FunPtr; }
    else if (Index == UT_OSAPI_QUEUEGETIDBYNAME_INDEX) { Ut_OSAPI_HookTable.OS_QueueGetIdByName = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMDELETE_INDEX)  { Ut_OSAPI_HookTable.OS_BinSemDelete = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMGETIDBYNAME_INDEX) { Ut_OSAPI_HookTable.OS_BinSemGetIdByName = FunPtr; }
    else if (Index == UT_OSAPI_BINSEMGETINFO_INDEX) { Ut_OSAPI_HookTable.OS_BinSemGetInfo = FunPtr; }
    else if (Index == UT_OSAPI_MUTSEMGIVE_INDEX)    { Ut_OSAPI_HookTable.OS_MutSemGive = FunPtr; }
    else if (Index == UT_OSAPI_MUTSEMGETIDBYNAME_INDEX) { Ut_OSAPI_HookTable.OS_MutSemGetIdByName = FunPtr; }
    else if (Index == UT_OSAPI_MUTSEMGETINFO_INDEX) { Ut_OSAPI_HookTable.OS_MutSemGetInfo = FunPtr; }
    else if (Index == UT_OSAPI_MILLI2TICKS_INDEX)  { Ut_OSAPI_HookTable.OS_Milli2Ticks = FunPtr; }
    else if (Index == UT_OSAPI_TICK2MICROS_INDEX) { Ut_OSAPI_HookTable.OS_Tick2Micros = FunPtr; }
    else if (Index == UT_OSAPI_EXCATTACHHANDLER_INDEX) { Ut_OSAPI_HookTable.OS_ExcAttachHandler = FunPtr; }
    else if (Index == UT_OSAPI_EXCENABLE_INDEX)    { Ut_OSAPI_HookTable.OS_ExcEnable = FunPtr; }
    else if (Index == UT_OSAPI_EXCDISABLE_INDEX)   { Ut_OSAPI_HookTable.OS_ExcDisable = FunPtr; }
    else if (Index == UT_OSAPI_FPUEXCATTACHHANDLER_INDEX) { Ut_OSAPI_HookTable.OS_FPUExcAttachHandler = FunPtr; }
    else if (Index == UT_OSAPI_FPUEXCENABLE_INDEX) { Ut_OSAPI_HookTable.OS_FPUExcEnable = FunPtr; }
    else if (Index == UT_OSAPI_FPUEXCDISABLE_INDEX) { Ut_OSAPI_HookTable.OS_FPUExcDisable = FunPtr; }
    else if (Index == UT_OSAPI_FPUEXCSETMASK_INDEX) { Ut_OSAPI_HookTable.OS_FPUExcSetMask = FunPtr; }
    else if (Index == UT_OSAPI_FPUEXCGETMASK_INDEX) { Ut_OSAPI_HookTable.OS_FPUExcGetMask = FunPtr; }
    else if (Index == UT_OSAPI_INTATTACHHANDLER_INDEX) { Ut_OSAPI_HookTable.OS_IntAttachHandler = FunPtr; }
    else if (Index == UT_OSAPI_INTENABLE_INDEX)   { Ut_OSAPI_HookTable.OS_IntEnable = FunPtr; }
    else if (Index == UT_OSAPI_INTDISABLE_INDEX)  { Ut_OSAPI_HookTable.OS_IntDisable = FunPtr; }
    else if (Index == UT_OSAPI_INTUNLOCK_INDEX)   { Ut_OSAPI_HookTable.OS_IntUnlock = FunPtr; }
    else if (Index == UT_OSAPI_INTLOCK_INDEX)     { Ut_OSAPI_HookTable.OS_IntLock = FunPtr; }
    else if (Index == UT_OSAPI_INTSETMASK_INDEX)  { Ut_OSAPI_HookTable.OS_IntSetMask = FunPtr; }
    else if (Index == UT_OSAPI_INTGETMASK_INDEX)  { Ut_OSAPI_HookTable.OS_IntGetMask = FunPtr; }
    else if (Index == UT_OSAPI_INTACK_INDEX)      { Ut_OSAPI_HookTable.OS_IntAck = FunPtr; }
    else if (Index == UT_OSAPI_SYMBOLLOOKUP_INDEX)      { Ut_OSAPI_HookTable.OS_SymbolLookup = FunPtr; }
    else if (Index == UT_OSAPI_SYMBOLTABLEDUMP_INDEX)      { Ut_OSAPI_HookTable.OS_SymbolTableDump = FunPtr; }
    else
    {
        printf("Unsupported OSAPI Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported OSAPI Index In SetFunctionHook Call");
    }
}

void Ut_OSAPI_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_OSAPI_MAX_INDEX) {
        Ut_OSAPI_ReturnCodeTable[Index].Value = RtnVal;
        Ut_OSAPI_ReturnCodeTable[Index].Count = CallCnt;
    } 
    else
    {
        printf("Unsupported OSAPI Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported OSAPI Index In SetReturnCode Call");
    }
}

boolean Ut_OSAPI_UseReturnCode(uint32 Index)
{
    if (Ut_OSAPI_ReturnCodeTable[Index].Count > 0)
    {
        Ut_OSAPI_ReturnCodeTable[Index].Count--;
        if (Ut_OSAPI_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_OSAPI_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_OSAPI_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_OSAPI_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

/**********************************************************************************
                                    TASK API
**********************************************************************************/

int32 OS_TaskCreate(uint32 *task_id, const char *task_name, osal_task_entry function_pointer, uint32 *stack_pointer, 
                    uint32 stack_size, uint32 priority, uint32 flags)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKCREATE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKCREATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskCreate)
        return(Ut_OSAPI_HookTable.OS_TaskCreate(task_id, task_name, function_pointer, stack_pointer, stack_size, priority, flags));

    return(OS_SUCCESS);
}

int32 OS_TaskDelete (uint32 task_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKDELETE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKDELETE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskDelete)
        return(Ut_OSAPI_HookTable.OS_TaskDelete(task_id));

    return(OS_SUCCESS);
}

int32 OS_TaskInstallDeleteHandler(osal_task_entry function_pointer)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKINSTALLDELETEHANDLER_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKINSTALLDELETEHANDLER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskInstallDeleteHandler)
        return(Ut_OSAPI_HookTable.OS_TaskInstallDeleteHandler(function_pointer));
    
    return(OS_SUCCESS);
}

void OS_TaskExit(void)
{
    return;
}

int32 OS_TaskDelay(uint32 millisecond)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKDELAY_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKDELAY_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskDelay)
        return(Ut_OSAPI_HookTable.OS_TaskDelay(millisecond));

    return(OS_SUCCESS);
}

int32 OS_TaskSetPriority (uint32 task_id, uint32 new_priority)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKSETPRIORITY_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKSETPRIORITY_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskSetPriority)
        return(Ut_OSAPI_HookTable.OS_TaskSetPriority(task_id, new_priority));

    return (OS_SUCCESS);
}

int32 OS_TaskRegister (void)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKREGISTER_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKREGISTER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskRegister)
        return(Ut_OSAPI_HookTable.OS_TaskRegister());

    return (OS_SUCCESS);
}

uint32 OS_TaskGetId (void)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKGETID_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKGETID_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskGetId)
        return(Ut_OSAPI_HookTable.OS_TaskGetId());

    return (uint32)(7);
}

int32 OS_TaskGetIdByName (uint32 *task_id, const char *task_name)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKGETIDBYNAME_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKGETIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskGetIdByName)
        return(Ut_OSAPI_HookTable.OS_TaskGetIdByName(task_id, task_name));

    return (uint32)(7);
}

int32 OS_TaskGetInfo (uint32 task_id, OS_task_prop_t *task_prop)
{
    task_prop = NULL;        /* currently not implemented */

    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TASKGETINFO_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TASKGETINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_TaskGetInfo)
        return(Ut_OSAPI_HookTable.OS_TaskGetInfo(task_id, task_prop));

    return(OS_SUCCESS);
}

/****************************************************************************************
                              MESSAGE QUEUE API
*****************************************************************************************/

int32 OS_QueueCreate (uint32 *queue_id, const char *queue_name, uint32 queue_depth,
                       uint32 data_size, uint32 flags)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_QUEUECREATE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_QUEUECREATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_QueueCreate)
        return(Ut_OSAPI_HookTable.OS_QueueCreate(queue_id, queue_name, queue_depth, data_size, flags));

    return (OS_SUCCESS);
}

int32 OS_QueueDelete (uint32 queue_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_QUEUEDELETE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_QUEUEDELETE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_QueueDelete)
        return(Ut_OSAPI_HookTable.OS_QueueDelete(queue_id));

    return (OS_SUCCESS);
}

int32 OS_QueueGet (uint32 queue_id, void *data, uint32 size, uint32 *size_copied, int32 timeout)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_QUEUEGET_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_QUEUEGET_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_QueueGet)
        return(Ut_OSAPI_HookTable.OS_QueueGet(queue_id, data, size, size_copied, timeout));

    return (OS_SUCCESS);
}

int32 OS_QueuePut (uint32 queue_id, const void *data, uint32 size, uint32 flags)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_QUEUEPUT_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_QUEUEPUT_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_QueuePut)
        return(Ut_OSAPI_HookTable.OS_QueuePut(queue_id, data, size, flags));

    return (OS_SUCCESS);
}

int32 OS_QueueGetIdByName (uint32 *queue_id, const char *queue_name)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_QUEUEGETIDBYNAME_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_QUEUEGETIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_QueueGetIdByName)
        return(Ut_OSAPI_HookTable.OS_QueueGetIdByName(queue_id, queue_name));
    
    return (OS_SUCCESS);
}

/******************************************************************************************
                                  SEMAPHORE API
******************************************************************************************/

int32 OS_BinSemCreate (uint32 *sem_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMCREATE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMCREATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemCreate)
        return(Ut_OSAPI_HookTable.OS_BinSemCreate(sem_id, sem_name, sem_initial_value, options));

    return (OS_SUCCESS);
}

int32 OS_BinSemDelete (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMDELETE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMDELETE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemDelete)
        return(Ut_OSAPI_HookTable.OS_BinSemDelete(sem_id));
    
    return (OS_SUCCESS);
}

int32 OS_BinSemFlush (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMFLUSH_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMFLUSH_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemFlush)
        return(Ut_OSAPI_HookTable.OS_BinSemFlush(sem_id));

    return (OS_SUCCESS);
}

int32 OS_BinSemGive (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMGIVE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMGIVE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemGive)
        return(Ut_OSAPI_HookTable.OS_BinSemGive(sem_id));

    return (OS_SUCCESS);
}

int32 OS_BinSemTake (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMTAKE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMTAKE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemTake)
        return(Ut_OSAPI_HookTable.OS_BinSemTake(sem_id));

    return (OS_SUCCESS);
}

int32 OS_BinSemTimedWait (uint32 sem_id, uint32 msecs)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMTIMEDWAIT_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMTIMEDWAIT_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemTimedWait)
        return(Ut_OSAPI_HookTable.OS_BinSemTimedWait(sem_id, msecs));

    return (OS_SUCCESS);
}

int32 OS_BinSemGetIdByName (uint32 *sem_id, const char *sem_name)
{
    sem_id = NULL;             /* currently not implemented */

    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMGETIDBYNAME_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMGETIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemGetIdByName)
        return(Ut_OSAPI_HookTable.OS_BinSemGetIdByName(sem_id, sem_name));

    return(OS_SUCCESS);
}

int32 OS_BinSemGetInfo (uint32 sem_id, OS_bin_sem_prop_t *bin_prop)
{
    bin_prop = NULL;        /* currently not implemented */

    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_BINSEMGETINFO_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_BINSEMGETINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_BinSemGetInfo)
        return(Ut_OSAPI_HookTable.OS_BinSemGetInfo(sem_id, bin_prop));

    return(OS_SUCCESS);
}

int32 OS_CountSemCreate (uint32 *sem_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMCREATE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMCREATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemCreate)
        return(Ut_OSAPI_HookTable.OS_CountSemCreate(sem_id, sem_name, sem_initial_value, options));

    return (OS_SUCCESS);
}

int32 OS_CountSemDelete (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMDELETE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMDELETE_INDEX].Value;   

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemDelete)
        return(Ut_OSAPI_HookTable.OS_CountSemDelete(sem_id)); 
    
    return (OS_SUCCESS);
}

int32 OS_CountSemGive (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMGIVE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMGIVE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemGive)
        return(Ut_OSAPI_HookTable.OS_CountSemGive(sem_id));

    return (OS_SUCCESS);
}

int32 OS_CountSemTake (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMTAKE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMTAKE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemTake)
        return(Ut_OSAPI_HookTable.OS_CountSemTake(sem_id));

    return OS_SUCCESS;
}

int32 OS_CountSemTimedWait (uint32 sem_id, uint32 msecs)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMTIMEDWAIT_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMTIMEDWAIT_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemTimedWait)
        return(Ut_OSAPI_HookTable.OS_CountSemTimedWait(sem_id,msecs));

    return OS_SUCCESS;
}

int32 OS_CountSemGetIdByName (uint32 *sem_id, const char *sem_name)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMGETIDBYNAME_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMGETIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemGetIdByName)
        return(Ut_OSAPI_HookTable.OS_CountSemGetIdByName(sem_id, sem_name));

    return OS_SUCCESS;
}

int32 OS_CountSemGetInfo (uint32 sem_id, OS_count_sem_prop_t *count_prop)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_COUNTSEMGETINFO_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_COUNTSEMGETINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_CountSemGetInfo)
        return(Ut_OSAPI_HookTable.OS_CountSemGetInfo(sem_id, count_prop));
    
    return OS_SUCCESS;
}

/****************************************************************************************
                                  MUTEX API
****************************************************************************************/

int32 OS_MutSemCreate (uint32 *sem_id, const char *sem_name, uint32 options)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MUTSEMCREATE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MUTSEMCREATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_MutSemCreate)
        return(Ut_OSAPI_HookTable.OS_MutSemCreate(sem_id, sem_name, options));

    return (OS_SUCCESS);
}

int32 OS_MutSemDelete (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MUTSEMDELETE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MUTSEMDELETE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_MutSemDelete)
        return(Ut_OSAPI_HookTable.OS_MutSemDelete(sem_id));
    
    return (OS_SUCCESS);
}

int32 OS_MutSemGive (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MUTSEMGIVE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MUTSEMGIVE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_MutSemGive)
        return(Ut_OSAPI_HookTable.OS_MutSemGive(sem_id));

    return (OS_SUCCESS);
}

int32 OS_MutSemTake (uint32 sem_id)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MUTSEMTAKE_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MUTSEMTAKE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_MutSemTake)
        return(Ut_OSAPI_HookTable.OS_MutSemTake(sem_id));

    return (OS_SUCCESS);
}

int32 OS_MutSemGetIdByName (uint32 *sem_id, const char *sem_name)
{
    sem_id = NULL;             /* currently not implemented */

    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MUTSEMGETIDBYNAME_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MUTSEMGETIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_MutSemGetIdByName)
        return(Ut_OSAPI_HookTable.OS_MutSemGetIdByName(sem_id, sem_name));

    return(OS_SUCCESS);
}

int32 OS_MutSemGetInfo (uint32 sem_id, OS_mut_sem_prop_t *mut_prop)
{
    mut_prop = NULL;        /* currently not implemented */

    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MUTSEMGETINFO_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MUTSEMGETINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_MutSemGetInfo)
        return(Ut_OSAPI_HookTable.OS_MutSemGetInfo(sem_id, mut_prop));

    return(OS_SUCCESS);
}

/****************************************************************************************
                                  Time/Tick Related API
****************************************************************************************/

int32 OS_Milli2Ticks (uint32 milli_seconds)
{
    uint32 num_of_ticks,tick_duration_usec ;

    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_MILLI2TICKS_INDEX))            
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_MILLI2TICKS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_Milli2Ticks)
        return(Ut_OSAPI_HookTable.OS_Milli2Ticks(milli_seconds));

    tick_duration_usec = OS_Tick2Micros() ;

    num_of_ticks = ( (milli_seconds * 1000) + tick_duration_usec -1 ) / tick_duration_usec ;

    return(num_of_ticks) ;
}

int32 OS_Tick2Micros (void)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_TICK2MICROS_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_TICK2MICROS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_Tick2Micros)
        return(Ut_OSAPI_HookTable.OS_Tick2Micros());

    return((1/(CLOCKS_PER_SEC))*1000); 
}

int32 OS_GetLocalTime(OS_time_t *time_struct)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_GETLOCALTIME_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_GETLOCALTIME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_GetLocalTime)
        return(Ut_OSAPI_HookTable.OS_GetLocalTime(time_struct));

    return (OS_SUCCESS);
}

/****************************************************************************************
                                  Exception API
****************************************************************************************/

int32 OS_ExcAttachHandler(uint32 ExceptionNumber, void (*ExceptionHandler)(uint32, const void *,uint32), int32 Parameter)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_EXCATTACHHANDLER_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_EXCATTACHHANDLER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_ExcAttachHandler)
        return(Ut_OSAPI_HookTable.OS_ExcAttachHandler(ExceptionNumber, ExceptionHandler, Parameter));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_ExcEnable(int32 ExceptionNumber)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_EXCENABLE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_EXCENABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_ExcEnable)
        return(Ut_OSAPI_HookTable.OS_ExcEnable(ExceptionNumber));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_ExcDisable(int32 ExceptionNumber)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_EXCDISABLE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_EXCDISABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_ExcDisable)
        return(Ut_OSAPI_HookTable.OS_ExcDisable(ExceptionNumber));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

/****************************************************************************************
                                  Floating Point Unit API
****************************************************************************************/

int32 OS_FPUExcAttachHandler(uint32 ExceptionNumber, void * ExceptionHandler, int32 Parameter)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_FPUEXCATTACHHANDLER_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_FPUEXCATTACHHANDLER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_FPUExcAttachHandler)
        return(Ut_OSAPI_HookTable.OS_FPUExcAttachHandler(ExceptionNumber, ExceptionHandler, Parameter));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcEnable(int32 ExceptionNumber)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_FPUEXCENABLE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_FPUEXCENABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_FPUExcEnable)
        return(Ut_OSAPI_HookTable.OS_FPUExcEnable(ExceptionNumber));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcDisable(int32 ExceptionNumber)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_FPUEXCDISABLE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_FPUEXCDISABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_FPUExcDisable)
        return(Ut_OSAPI_HookTable.OS_FPUExcDisable(ExceptionNumber));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcSetMask(uint32 Mask)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_FPUEXCSETMASK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_FPUEXCSETMASK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_FPUExcSetMask)
        return(Ut_OSAPI_HookTable.OS_FPUExcSetMask(Mask));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcGetMask(uint32 *Mask)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_FPUEXCGETMASK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_FPUEXCGETMASK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_FPUExcGetMask)
        return(Ut_OSAPI_HookTable.OS_FPUExcGetMask(Mask));

    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

/****************************************************************************************
                                  Interrupt API
****************************************************************************************/

int32 OS_IntAttachHandler(uint32 InterruptNumber, osal_task_entry InterruptHandler, int32 parameter)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTATTACHHANDLER_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTATTACHHANDLER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntAttachHandler)
        return(Ut_OSAPI_HookTable.OS_IntAttachHandler(InterruptNumber, InterruptHandler, parameter));
    
    return(OS_SUCCESS);
}

int32 OS_IntEnable(int32 Level)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTENABLE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTENABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntEnable)
        return(Ut_OSAPI_HookTable.OS_IntEnable(Level));
    
    return(OS_SUCCESS);
}

int32 OS_IntDisable(int32 Level)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTDISABLE_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTDISABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntDisable)
        return(Ut_OSAPI_HookTable.OS_IntDisable(Level));
    
    return(OS_SUCCESS);
}

int32 OS_IntUnlock (int32 IntLevel)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTUNLOCK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTUNLOCK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntUnlock)
        return(Ut_OSAPI_HookTable.OS_IntUnlock(IntLevel));
    
    return(OS_SUCCESS);
}

int32 OS_IntLock (void)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTLOCK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTLOCK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntLock)
        return(Ut_OSAPI_HookTable.OS_IntLock());

    return(OS_SUCCESS);
}

int32 OS_IntSetMask(uint32 Mask)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTSETMASK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTSETMASK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntSetMask)
        return(Ut_OSAPI_HookTable.OS_IntSetMask(Mask));
    
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_IntGetMask(uint32 *Mask)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTGETMASK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTGETMASK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntGetMask)
        return(Ut_OSAPI_HookTable.OS_IntGetMask(Mask));
    
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_IntAck(int32 InterruptNumber)
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_INTACK_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_INTACK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_IntAck)
        return(Ut_OSAPI_HookTable.OS_IntAck(InterruptNumber));
    
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

/****************************************************************************************
                                  Debug API
****************************************************************************************/

int32 OS_GetErrorName(int32 error_num, os_err_name_t * err_name)
{
    os_err_name_t local_name;
    uint32 return_code;

    return_code = OS_SUCCESS;

    switch (error_num)
    {
        case OS_SUCCESS:
            strcpy(local_name,"OS_SUCCESS"); break;
        case OS_ERROR:
            strcpy(local_name,"OS_ERROR"); break;
        case OS_INVALID_POINTER:
            strcpy(local_name,"OS_INVALID_POINTER"); break;
        case OS_ERROR_ADDRESS_MISALIGNED:
            strcpy(local_name,"OS_ADDRESS_MISALIGNED"); break;
        case OS_ERROR_TIMEOUT:
            strcpy(local_name,"OS_ERROR_TIMEOUT"); break;
        case OS_INVALID_INT_NUM:
            strcpy(local_name,"OS_INVALID_INT_NUM"); break;
        case OS_SEM_FAILURE:
            strcpy(local_name,"OS_SEM_FAILURE"); break;
        case OS_SEM_TIMEOUT:
            strcpy(local_name,"OS_SEM_TIMEOUT"); break;
        case OS_QUEUE_EMPTY:
            strcpy(local_name,"OS_QUEUE_EMPTY"); break;
        case OS_QUEUE_FULL:
            strcpy(local_name,"OS_QUEUE_FULL"); break;
        case OS_QUEUE_TIMEOUT:
            strcpy(local_name,"OS_QUEUE_TIMEOUT"); break;
        case OS_QUEUE_INVALID_SIZE:
            strcpy(local_name,"OS_QUEUE_INVALID_SIZE"); break;
        case OS_QUEUE_ID_ERROR:
            strcpy(local_name,"OS_QUEUE_ID_ERROR"); break;
        case OS_ERR_NAME_TOO_LONG:
            strcpy(local_name,"OS_ERR_NAME_TOO_LONG"); break;
        case OS_ERR_NO_FREE_IDS:
            strcpy(local_name,"OS_ERR_NO_FREE_IDS"); break;
        case OS_ERR_NAME_TAKEN:
            strcpy(local_name,"OS_ERR_NAME_TAKEN"); break;
        case OS_ERR_INVALID_ID:
            strcpy(local_name,"OS_ERR_INVALID_ID"); break;
        case OS_ERR_NAME_NOT_FOUND:
            strcpy(local_name,"OS_ERR_NAME_NOT_FOUND"); break;
        case OS_ERR_SEM_NOT_FULL:
            strcpy(local_name,"OS_ERR_SEM_NOT_FULL"); break;
        case OS_ERR_INVALID_PRIORITY:
            strcpy(local_name,"OS_ERR_INVALID_PRIORITY"); break;

        default: strcpy(local_name,"ERROR_UNKNOWN");
                 return_code = OS_ERROR;
    }
    strcpy((char*) err_name, local_name);
    return return_code;
}

void OS_printf( const char *String, ...)
{
    va_list     ptr;
    char msg_buffer [OS_BUFFER_SIZE];

    va_start(ptr,String);
    vsprintf(&msg_buffer[0], String, ptr);
    va_end(ptr);

    msg_buffer[OS_BUFFER_SIZE-1] = '\0';
    UtPrintf("%s", &msg_buffer[0]);
}
/****************************************************************************************
                                  Other
****************************************************************************************/

int32 OS_SymbolLookup( cpuaddr *SymbolAddress, const char *SymbolName )
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_SYMBOLLOOKUP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_SymbolLookup)
        return(Ut_OSAPI_HookTable.OS_SymbolLookup(SymbolAddress, SymbolName));
    
    return(OS_SUCCESS);
}

int32 OS_SymbolTableDump( const char *filename, uint32 SizeLimit )
{
    /* Check for specified return */
    if (Ut_OSAPI_UseReturnCode(UT_OSAPI_SYMBOLTABLEDUMP_INDEX))
        return Ut_OSAPI_ReturnCodeTable[UT_OSAPI_SYMBOLTABLEDUMP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_OSAPI_HookTable.OS_SymbolTableDump)
        return(Ut_OSAPI_HookTable.OS_SymbolTableDump(filename, SizeLimit));
    
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}
