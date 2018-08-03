/*
**
** File: ut_cfe_es_stubs.h
**
** $Id: ut_cfe_es_stubs.h 1.3 2016/06/22 19:14:17EDT czogby Exp  $
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
** Purpose: cFE Executive Services Header file for unit test stubs
**
** $Log: ut_cfe_es_stubs.h  $
** Revision 1.3 2016/06/22 19:14:17EDT czogby 
** UT-Assert: Add Missing Stub Functions
** Revision 1.2 2016/06/22 18:39:54EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:27EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.5 2015/06/16 16:02:10EDT sstrege 
** Added copyright information
** Revision 1.4 2015/05/07 14:05:02EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.3 2015/03/17 18:16:43EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 17:28:05Z]
** Added/fixed function prototypes, added a new element to 
** Ut_CFE_ES_INDEX_t
** Revision 1.2 2015/03/10 15:19:30EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.1 2011/02/15 11:12:32EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_ES_STUBS_H_
#define UT_CFE_ES_STUBS_H_

typedef enum 
{
    UT_CFE_ES_GETRESETTYPE_INDEX,
    UT_CFE_ES_RESETCFE_INDEX,
    UT_CFE_ES_RESTARTAPP_INDEX,
    UT_CFE_ES_RELOADAPP_INDEX,
    UT_CFE_ES_DELETEAPP_INDEX,
    UT_CFE_ES_EXITAPP_INDEX,
    UT_CFE_ES_RUNLOOP_INDEX,
    UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX,
    UT_CFE_ES_REGISTERAPP_INDEX,
    UT_CFE_ES_GETAPPID_INDEX,
    UT_CFE_ES_GETAPPIDBYNAME_INDEX,
    UT_CFE_ES_GETAPPNAME_INDEX,
    UT_CFE_ES_GETAPPINFO_INDEX,
    UT_CFE_ES_GETTASKINFO_INDEX,
    UT_CFE_ES_REGISTERCHILDTASK_INDEX,
    UT_CFE_ES_CREATECHILDTASK_INDEX,
    UT_CFE_ES_DELETECHILDTASK_INDEX,
    UT_CFE_ES_EXITCHILDTASK_INDEX,
    UT_CFE_ES_INCREMENTTASKCOUNTER_INDEX,
    UT_CFE_ES_WRITETOSYSLOG_INDEX,
    UT_CFE_ES_UNLOADDRIVER_INDEX,
    UT_CFE_ES_CALCULATECRC_INDEX,
    UT_CFE_ES_REGISTERCDS_INDEX,
    UT_CFE_ES_COPYTOCDS_INDEX,
    UT_CFE_ES_RESTOREFROMCDS_INDEX,
    UT_CFE_ES_POOLCREATE_INDEX,
    UT_CFE_ES_POOLCREATEEX_INDEX,
    UT_CFE_ES_GETPOOLBUF_INDEX,
    UT_CFE_ES_GETPOOLBUFINFO_INDEX,
    UT_CFE_ES_PUTPOOLBUF_INDEX,
    UT_CFE_ES_GETMEMPOOLSTATS_INDEX,
    UT_CFE_ES_PERFLOGADD_INDEX,
    UT_CFE_ES_GETGENCOUNTERIDBYNAME_INDEX,
    UT_CFE_ES_GETGENCOUNT_INDEX,
    UT_CFE_ES_MAX_INDEX
} Ut_CFE_ES_INDEX_t;

typedef struct
{
    int32 (*CFE_ES_GetResetType)(uint32 *ResetSubtypePtr);
    int32 (*CFE_ES_ResetCFE)(uint32 ResetType);
    int32 (*CFE_ES_RestartApp)(uint32 AppID);
    int32 (*CFE_ES_ReloadApp)(uint32 AppID_API, const char *AppFileName);
    int32 (*CFE_ES_DeleteApp)(uint32 AppID);
    int32 (*CFE_ES_ExitApp)(uint32 ExitStatus);
    int32 (*CFE_ES_RunLoop)(uint32 *ExitStatus);
    int32 (*CFE_ES_WaitForStartupSync)(uint32 TimeOutMilliseconds);
    int32 (*CFE_ES_RegisterApp)(void);
    int32 (*CFE_ES_GetAppID)(uint32 *AppIdPtr);
    int32 (*CFE_ES_GetAppIDByName)(uint32 *AppIdPtr, const char *AppName);
    int32 (*CFE_ES_GetAppName)(const char *AppName, uint32 AppId, uint32 BufferLength);
    int32 (*CFE_ES_GetAppInfo)(CFE_ES_AppInfo_t *AppInfo, uint32 AppId);
    int32 (*CFE_ES_GetTaskInfo)(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId);
    int32 (*CFE_ES_RegisterChildTask)(void);
    int32 (*CFE_ES_CreateChildTask)(uint32 *TaskIdPtr, const char *TaskName, CFE_ES_ChildTaskMainFuncPtr_t  FunctionPtr, uint32 *StackPtr, uint32 StackSize, uint32 Priority, uint32 Flags);
    int32 (*CFE_ES_DeleteChildTask)(uint32 TaskId);
    int32 (*CFE_ES_ExitChildTask)(void);
    int32 (*CFE_ES_IncrementTaskCounter)(void);
    int32 (*CFE_ES_WriteToSysLog)(const char *SpecStringPtr, ...);
    int32 (*CFE_ES_UnloadDriver)(uint32 DriverId);
    int32 (*CFE_ES_CalculateCRC)(const void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC);
    int32 (*CFE_ES_RegisterCDS)(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name);
    int32 (*CFE_ES_CopyToCDS)(CFE_ES_CDSHandle_t Handle, void *DataToCopy);
    int32 (*CFE_ES_RestoreFromCDS)(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle);
    int32 (*CFE_ES_PoolCreate)(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size);
    int32 (*CFE_ES_PoolCreateEx)(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size, uint32 NumBlockSizes, uint32 *BlockSizes, uint16 UseMutex);
    int32 (*CFE_ES_GetPoolBuf)(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size);
    int32 (*CFE_ES_GetPoolBufInfo)(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr);
    int32 (*CFE_ES_PutPoolBuf)(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr);
    int32 (*CFE_ES_GetMemPoolStats)(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t  Handle);
    int32 (*CFE_ES_PerfLogAdd)(uint32 Marker, uint32 EntryExit);    
    int32 (*CFE_ES_GetGenCounterIDByName)(uint32 *CounterIdPtr, const char *CounterName);
    int32 (*CFE_ES_GetGenCount)(uint32 CounterId, uint32 *Count);
} Ut_CFE_ES_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_ES_ReturnCodeTable_t;

void Ut_CFE_ES_Reset(void);
void Ut_CFE_ES_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_ES_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_ES_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
