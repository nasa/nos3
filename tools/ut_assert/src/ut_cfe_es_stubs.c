/*
**
** File: ut_cfe_es_stubs.c
**
** $Id: ut_cfe_es_stubs.c 1.3 2016/06/23 15:12:33EDT czogby Exp  $
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
** Purpose: Unit test stubs for cFE Executive Services routines
**
** $Log: ut_cfe_es_stubs.c  $
** Revision 1.3 2016/06/23 15:12:33EDT czogby 
** UT-Assert: Add Missing Stub Functions
** Revision 1.2 2016/06/23 14:39:03EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:30EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.10 2015/06/16 16:14:00EDT sstrege 
** Added copyright information
** Revision 1.9 2015/05/07 14:39:51EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.8 2015/05/01 12:41:43EDT czogby 
** Added UtAssert calls that fail to places where error messages are generated, added comments
** Revision 1.7 2015/03/17 18:26:23EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:19:05Z]
** Added SysLog functionality, added missing checks for function hook / return value, other minor fixes
** Revision 1.6 2015/03/10 15:21:45EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.5 2011/03/31 14:53:04EDT rmcgraw 
** Added functionality and supressed compiler warnings
** Revision 1.4 2011/03/23 17:08:18EDT rmcgraw 
** OS_FS_ERROR to OS_FS_SUCCESS for some OS file sys apis
** Revision 1.3 2011/03/09 10:26:12EST rmcgraw 
** Added SetRtnCode logic to PoolCreateEx
** Revision 1.2 2011/03/07 17:53:39EST sslegel 
** Added a default hook for ES_CFE_RunLoop
** Added additional return code support
** Revision 1.1 2011/02/15 11:13:01EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

/*
** Include section
*/

#include "cfe.h"
#include "utassert.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include <string.h>

Ut_CFE_ES_HookTable_t           Ut_CFE_ES_HookTable;
Ut_CFE_ES_ReturnCodeTable_t     Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_MAX_INDEX];

void Ut_CFE_ES_Reset(void)
{
    memset(&Ut_CFE_ES_HookTable, 0, sizeof(Ut_CFE_ES_HookTable));
    memset(&Ut_CFE_ES_ReturnCodeTable, 0, sizeof(Ut_CFE_ES_ReturnCodeTable));

    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_RUNLOOP_INDEX, &Ut_CFE_ES_RunLoopHook);
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WRITETOSYSLOG_INDEX, &Ut_CFE_ES_WriteToSysLog);
    Ut_CFE_ES_ClearSysLogQueue();
}

void Ut_CFE_ES_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_ES_RESETCFE_INDEX)             { Ut_CFE_ES_HookTable.CFE_ES_ResetCFE = FunPtr; }
    else if (Index == UT_CFE_ES_RESTARTAPP_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_RestartApp = FunPtr; }
    else if (Index == UT_CFE_ES_RELOADAPP_INDEX)            { Ut_CFE_ES_HookTable.CFE_ES_ReloadApp = FunPtr; }
    else if (Index == UT_CFE_ES_DELETEAPP_INDEX)            { Ut_CFE_ES_HookTable.CFE_ES_DeleteApp = FunPtr; }
    else if (Index == UT_CFE_ES_EXITAPP_INDEX)              { Ut_CFE_ES_HookTable.CFE_ES_ExitApp = FunPtr; }
    else if (Index == UT_CFE_ES_RUNLOOP_INDEX)              { Ut_CFE_ES_HookTable.CFE_ES_RunLoop = FunPtr; }
    else if (Index == UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX)   { Ut_CFE_ES_HookTable.CFE_ES_WaitForStartupSync = FunPtr; }
    else if (Index == UT_CFE_ES_REGISTERAPP_INDEX)          { Ut_CFE_ES_HookTable.CFE_ES_RegisterApp = FunPtr; }
    else if (Index == UT_CFE_ES_GETAPPID_INDEX)             { Ut_CFE_ES_HookTable.CFE_ES_GetAppID = FunPtr; }
    else if (Index == UT_CFE_ES_GETAPPIDBYNAME_INDEX)       { Ut_CFE_ES_HookTable.CFE_ES_GetAppIDByName = FunPtr; }
    else if (Index == UT_CFE_ES_GETAPPNAME_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_GetAppName = FunPtr; }
    else if (Index == UT_CFE_ES_GETAPPINFO_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_GetAppInfo = FunPtr; }
    else if (Index == UT_CFE_ES_GETTASKINFO_INDEX)          { Ut_CFE_ES_HookTable.CFE_ES_GetTaskInfo = FunPtr; }
    else if (Index == UT_CFE_ES_REGISTERCHILDTASK_INDEX)    { Ut_CFE_ES_HookTable.CFE_ES_RegisterChildTask = FunPtr; }
    else if (Index == UT_CFE_ES_CREATECHILDTASK_INDEX)      { Ut_CFE_ES_HookTable.CFE_ES_CreateChildTask = FunPtr; }
    else if (Index == UT_CFE_ES_DELETECHILDTASK_INDEX)      { Ut_CFE_ES_HookTable.CFE_ES_DeleteChildTask = FunPtr; }
    else if (Index == UT_CFE_ES_EXITCHILDTASK_INDEX)        { Ut_CFE_ES_HookTable.CFE_ES_ExitChildTask = FunPtr; }
    else if (Index == UT_CFE_ES_INCREMENTTASKCOUNTER_INDEX) { Ut_CFE_ES_HookTable.CFE_ES_IncrementTaskCounter = FunPtr; }
    else if (Index == UT_CFE_ES_WRITETOSYSLOG_INDEX)        { Ut_CFE_ES_HookTable.CFE_ES_WriteToSysLog = FunPtr; }
    else if (Index == UT_CFE_ES_UNLOADDRIVER_INDEX)         { Ut_CFE_ES_HookTable.CFE_ES_UnloadDriver = FunPtr; }
    else if (Index == UT_CFE_ES_CALCULATECRC_INDEX)         { Ut_CFE_ES_HookTable.CFE_ES_CalculateCRC = FunPtr; }
    else if (Index == UT_CFE_ES_REGISTERCDS_INDEX)          { Ut_CFE_ES_HookTable.CFE_ES_RegisterCDS = FunPtr; }
    else if (Index == UT_CFE_ES_COPYTOCDS_INDEX)            { Ut_CFE_ES_HookTable.CFE_ES_CopyToCDS = FunPtr; }
    else if (Index == UT_CFE_ES_RESTOREFROMCDS_INDEX)       { Ut_CFE_ES_HookTable.CFE_ES_RestoreFromCDS = FunPtr; }
    else if (Index == UT_CFE_ES_POOLCREATE_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_PoolCreate = FunPtr; }
    else if (Index == UT_CFE_ES_POOLCREATEEX_INDEX)         { Ut_CFE_ES_HookTable.CFE_ES_PoolCreateEx = FunPtr; }
    else if (Index == UT_CFE_ES_GETPOOLBUF_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_GetPoolBuf = FunPtr; }
    else if (Index == UT_CFE_ES_GETPOOLBUFINFO_INDEX)       { Ut_CFE_ES_HookTable.CFE_ES_GetPoolBufInfo = FunPtr; }
    else if (Index == UT_CFE_ES_PUTPOOLBUF_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_PutPoolBuf = FunPtr; }
    else if (Index == UT_CFE_ES_GETMEMPOOLSTATS_INDEX)      { Ut_CFE_ES_HookTable.CFE_ES_GetMemPoolStats = FunPtr; }
    else if (Index == UT_CFE_ES_PERFLOGADD_INDEX)           { Ut_CFE_ES_HookTable.CFE_ES_PerfLogAdd = FunPtr; }
    else
    {
        printf("Unsupported ES Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported ES Index In SetFunctionHook Call");
    }
}

void Ut_CFE_ES_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_ES_MAX_INDEX)
    {
        Ut_CFE_ES_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_ES_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported ES Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported ES Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_ES_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_ES_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_ES_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_ES_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_ES_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_ES_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_ES_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETRESETTYPE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetResetType)
        return Ut_CFE_ES_HookTable.CFE_ES_GetResetType(ResetSubtypePtr);

    return CFE_SUCCESS;
}

int32  CFE_ES_ResetCFE(uint32 ResetType)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_RESETCFE_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_RESETCFE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_ResetCFE)
        return Ut_CFE_ES_HookTable.CFE_ES_ResetCFE(ResetType);

    return CFE_SUCCESS;
}

int32 CFE_ES_RestartApp(uint32 AppID)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_RESTARTAPP_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_RESTARTAPP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_RestartApp)
        return Ut_CFE_ES_HookTable.CFE_ES_RestartApp(AppID);

    return CFE_SUCCESS;
}

int32 CFE_ES_ReloadApp(uint32 AppID, const char *AppFileName)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_RELOADAPP_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_RELOADAPP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_ReloadApp)
        return Ut_CFE_ES_HookTable.CFE_ES_ReloadApp(AppID, AppFileName);

    return CFE_SUCCESS;
}

int32 CFE_ES_DeleteApp(uint32 AppID)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_DELETEAPP_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_DELETEAPP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_DeleteApp)
        return Ut_CFE_ES_HookTable.CFE_ES_DeleteApp(AppID);

    return CFE_SUCCESS;
}

void CFE_ES_ExitApp(uint32 ExitStatus)
{
    /* Can't specify return value - this is a void function */

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_ExitApp)
        Ut_CFE_ES_HookTable.CFE_ES_ExitApp(ExitStatus);

    return;
}

int32 CFE_ES_RunLoop(uint32 *ExitStatus)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_RUNLOOP_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_RUNLOOP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_RunLoop)
        return Ut_CFE_ES_HookTable.CFE_ES_RunLoop(ExitStatus);

    return TRUE;
}

void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds)
{
    /* Can't specify return value - this is a void function */

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_WaitForStartupSync)
        Ut_CFE_ES_HookTable.CFE_ES_WaitForStartupSync(TimeOutMilliseconds);

    return;
}

int32 CFE_ES_RegisterApp(void)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_REGISTERAPP_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_REGISTERAPP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_RegisterApp)
        return Ut_CFE_ES_HookTable.CFE_ES_RegisterApp();

    return CFE_SUCCESS;
}

int32 CFE_ES_GetAppID(uint32 *AppIdPtr)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETAPPID_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETAPPID_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetAppID)
        return Ut_CFE_ES_HookTable.CFE_ES_GetAppID(AppIdPtr);

    return CFE_SUCCESS;
}

int32 CFE_ES_GetAppIDByName(uint32 *AppIdPtr, const char *AppName)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETAPPIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetAppIDByName)
        return Ut_CFE_ES_HookTable.CFE_ES_GetAppIDByName(AppIdPtr, AppName);

    return CFE_SUCCESS;
}

int32 CFE_ES_GetAppName(char *AppName, uint32 AppId, uint32 BufferLength)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETAPPNAME_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETAPPNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetAppName)
        return Ut_CFE_ES_HookTable.CFE_ES_GetAppName(AppName, AppId, BufferLength);

    return CFE_SUCCESS;
}

int32 CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETAPPINFO_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETAPPINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetAppInfo)
        return Ut_CFE_ES_HookTable.CFE_ES_GetAppInfo(AppInfo, AppId);

    return CFE_SUCCESS;
}

int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETTASKINFO_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETTASKINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetTaskInfo)
        return Ut_CFE_ES_HookTable.CFE_ES_GetTaskInfo(TaskInfo, TaskId);

    return CFE_SUCCESS;
}

int32  CFE_ES_RegisterChildTask(void)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_REGISTERCHILDTASK_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_REGISTERCHILDTASK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_RegisterChildTask)
        return Ut_CFE_ES_HookTable.CFE_ES_RegisterChildTask();

    return CFE_SUCCESS;
}

int32  CFE_ES_CreateChildTask(uint32                          *TaskIdPtr,
                              const char                      *TaskName,
                              CFE_ES_ChildTaskMainFuncPtr_t    FunctionPtr,
                              uint32                          *StackPtr,
                              uint32                           StackSize,
                              uint32                           Priority,
                              uint32                           Flags)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_CREATECHILDTASK_INDEX].Value;
 
    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_CreateChildTask)
        return Ut_CFE_ES_HookTable.CFE_ES_CreateChildTask(TaskIdPtr, TaskName, FunctionPtr, StackPtr, StackSize, Priority, Flags);

    return CFE_SUCCESS;
}

int32 CFE_ES_DeleteChildTask(uint32 TaskId)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_DELETECHILDTASK_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_DELETECHILDTASK_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_DeleteChildTask)
        return Ut_CFE_ES_HookTable.CFE_ES_DeleteChildTask(TaskId);

    return CFE_SUCCESS;
}

void CFE_ES_ExitChildTask(void)
{
    /* Can't specify return value - this is a void function */

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_ExitChildTask)
        Ut_CFE_ES_HookTable.CFE_ES_ExitChildTask();

    return;
}

void  CFE_ES_IncrementTaskCounter(void)
{
    /* Can't specify return value - this is a void function */

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_IncrementTaskCounter)
        Ut_CFE_ES_HookTable.CFE_ES_IncrementTaskCounter();

    return;
}

int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    va_list  Ptr;
    char     SysLogEntryText[CFE_ES_MAX_SYSLOG_MSG_SIZE];
  
    va_start(Ptr, SpecStringPtr);
    vsprintf(SysLogEntryText, SpecStringPtr, Ptr);
    va_end(Ptr);

    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_WRITETOSYSLOG_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_WRITETOSYSLOG_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_WriteToSysLog)
         return(Ut_CFE_ES_HookTable.CFE_ES_WriteToSysLog(SysLogEntryText));

    return CFE_SUCCESS;
}

int32 CFE_ES_UnloadDriver(uint32 DriverId)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_UNLOADDRIVER_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_UNLOADDRIVER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_UnloadDriver)
         return(Ut_CFE_ES_HookTable.CFE_ES_UnloadDriver(DriverId));

    return CFE_SUCCESS;
}

uint32 CFE_ES_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_CALCULATECRC_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_CALCULATECRC_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_CalculateCRC)
         return(Ut_CFE_ES_HookTable.CFE_ES_CalculateCRC(DataPtr, DataLength, InputCRC, TypeCRC));

    return CFE_SUCCESS;
}

int32 CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_REGISTERCDS_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_REGISTERCDS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_RegisterCDS)
         return(Ut_CFE_ES_HookTable.CFE_ES_RegisterCDS(HandlePtr, BlockSize, Name));

    return CFE_SUCCESS;
}

int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_COPYTOCDS_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_COPYTOCDS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_CopyToCDS)
         return(Ut_CFE_ES_HookTable.CFE_ES_CopyToCDS(Handle, DataToCopy));

    return CFE_SUCCESS;
}

int32 CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_RESTOREFROMCDS_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_RESTOREFROMCDS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_RestoreFromCDS)
         return(Ut_CFE_ES_HookTable.CFE_ES_RestoreFromCDS(RestoreToMemory, Handle));

    return CFE_SUCCESS;
}

int32 CFE_ES_PoolCreate(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_POOLCREATE_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_POOLCREATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_PoolCreate)
         return(Ut_CFE_ES_HookTable.CFE_ES_PoolCreate(HandlePtr, MemPtr, Size));

    return CFE_SUCCESS;
}

int32 CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size, uint32 NumBlockSizes, uint32 *BlockSizes, uint16 UseMutex)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_POOLCREATEEX_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_POOLCREATEEX_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_PoolCreateEx)
         return(Ut_CFE_ES_HookTable.CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, NumBlockSizes, BlockSizes, UseMutex));
        
    return CFE_SUCCESS;
}

int32 CFE_ES_GetPoolBuf(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETPOOLBUF_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETPOOLBUF_INDEX].Value;
        
    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetPoolBuf)
        return Ut_CFE_ES_HookTable.CFE_ES_GetPoolBuf(BufPtr,HandlePtr,Size);      

    return CFE_SUCCESS;
}

int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETPOOLBUFINFO_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETPOOLBUFINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetPoolBufInfo)
         return(Ut_CFE_ES_HookTable.CFE_ES_GetPoolBufInfo(HandlePtr, BufPtr));

    return CFE_SUCCESS;
}

int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_PUTPOOLBUF_INDEX].Value;    

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_PutPoolBuf)
         return(Ut_CFE_ES_HookTable.CFE_ES_PutPoolBuf(HandlePtr, BufPtr));

    return CFE_SUCCESS;
}

int32 CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t  Handle)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETMEMPOOLSTATS_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETMEMPOOLSTATS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetMemPoolStats)
         return(Ut_CFE_ES_HookTable.CFE_ES_GetMemPoolStats(BufPtr, Handle));

    return CFE_SUCCESS;
}

void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
    /* Can't specify return value - this is a void function */

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_PerfLogAdd)
         Ut_CFE_ES_HookTable.CFE_ES_PerfLogAdd(Marker, EntryExit);

    return;
}


int32 CFE_ES_GetGenCounterIDByName(uint32 *CounterIdPtr, const char *CounterName)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETGENCOUNTERIDBYNAME_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETGENCOUNTERIDBYNAME_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetGenCounterIDByName)
         return(Ut_CFE_ES_HookTable.CFE_ES_GetGenCounterIDByName(CounterIdPtr, CounterName));

    return CFE_SUCCESS;
}

int32 CFE_ES_GetGenCount(uint32 CounterId, uint32 *Count)
{
    /* Check for specified return */
    if (Ut_CFE_ES_UseReturnCode(UT_CFE_ES_GETGENCOUNT_INDEX))
        return Ut_CFE_ES_ReturnCodeTable[UT_CFE_ES_GETGENCOUNT_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_ES_HookTable.CFE_ES_GetGenCount)
         return(Ut_CFE_ES_HookTable.CFE_ES_GetGenCount(CounterId, Count));

    return CFE_SUCCESS;
}
