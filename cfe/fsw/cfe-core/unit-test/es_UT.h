/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software (cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    es_UT.h
**
** Purpose:
**    ES unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:23:27GMT-05:00 $
** $Revision: 1.1 $
**
*/
#ifndef _es_UT_h_
#define _es_UT_h_

/*
** Macro definitions
*/
/* Maximum size of startup script buffer (sized for lines * 80 chars/line) */
#define MAX_STARTUP_SCRIPT (80 * 5)

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "common_types.h"
#include "osapi.h"
#include "osapi-os-loader.h"
#include "cfe_version.h"
#include "cfe_es.h"
#include "cfe_es_cds.h"
#include "cfe_es_cds_mempool.h"
#include "cfe_esmempool.h"
#include "cfe_es_global.h"
#include "cfe_es_log.h"
#include "cfe_es_perf.h"
#include "cfe_es_shell.h"
#include "cfe_es_task.h"
#include "cfe_es_verify.h"
#include "cfe_es_start.h"
#include "ut_stubs.h"
#include "ut_osprintf_stubs.h"

/* ES unit test functions */
/*****************************************************************************/
/**
** \brief Initialize executive services (ES) unit tests and begin testing the
**        main ES startup function
**
** \par Description
**        This function initializes the unit test variables for subsequent
**        tests. It then performs a normal ES main startup, which constitutes
**        test #1.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Init, #UT_Text, #UT_SetCDSSize, #UT_SetSizeofESResetArea
** \sa #UT_SetStatusBSPResetArea, #UT_SetReadBuffer, #UT_SetRtnCode
** \sa #UT_SetDummyFuncRtn, #UT_SetBSPloadAppFileResult, #CFE_ES_Main
** \sa #UT_Report
**
******************************************************************************/
void TestInit(void);

/*****************************************************************************/
/**
** \brief Perform tests for non-normal ES main startup and provide coverage
**        for remaining functions in cfe_es_start.c
**
** \par Description
**        This function exercises the remaining (error condition) paths for the
**        ES main startup that were not covered in TestInit.  The remaining
**        functions in cfe_es_start.c for initializing the the ES reset
**        variables, the file system, and the application layer are then
**        tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_SetRtnCode, #UT_SetOSFail, #CFE_ES_Main, #UT_Report
** \sa #CFE_ES_SetupResetVariables, #UT_SetStatusBSPResetArea
** \sa #UT_SetSizeofESResetArea, #CFE_ES_InitializeFileSystems, #UT_SetBSPFail
** \sa #CFE_ES_CreateObjects
**
******************************************************************************/
void TestStartupErrorPaths(void);

/*****************************************************************************/
/**
** \brief Perform tests on the ES application control internal interface
**        functions contained in cfe_es_api.c
**
** \par Description
**        This function tests all paths for the application control functions
**        which are used for creating, starting, managing, and deleting
**        application and child tasks.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_SetReadBuffer, #UT_SetRtnCode, #CFE_ES_StartApplications
** \sa #UT_Report, #UT_SetOSFail, #CFE_ES_ParseFileEntry
** \sa #UT_SetBSPloadAppFileResult, #CFE_ES_AppCreate, #UT_SetDummyFuncRtn
** \sa #CFE_ES_LoadLibrary, #CFE_ES_ScanAppTable, #CFE_ES_ProcessControlRequest
** \sa #CFE_ES_ListResourcesDebug, #CFE_ES_GetAppInfo, #CFE_ES_CleanUpApp
** \sa #CFE_ES_CleanupTaskResources
**
******************************************************************************/
void TestApps(void);

/*****************************************************************************/
/**
** \brief Perform tests of the ES exception and reset log function contained in
**        cfe_es_erlog.c
**
** \par Description
**        This function tests the possible paths through the write to log
**        function including log roll overs, valid and over-sized contexts, and
**        both null and non null descriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #CFE_ES_WriteToERLog, #UT_Report
**
******************************************************************************/
void TestERLog(void);

/*****************************************************************************/
/**
** \brief Performs tests on the ES shell commanding system contained in
**        cfe_es_shell.c
**
** \par Description
**        This function tests the shell output command paths in the function
**        CFE_ES_ShellOutputCommand.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_SetOSFail, #UT_Report, #CFE_ES_ShellOutputCommand
** \sa #UT_SetRtnCode
**
******************************************************************************/
void TestShell(void);

/*****************************************************************************/
/**
** \brief Performs tests of the ground command functions contained in
**        cfe_es_task.c
**
** \par Description
**        This function tests the ground command paths for the commands handled
**        by CFE_ES_TaskPipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_SetRtnCode, #CFE_ES_TaskMain, #UT_Report, #UT_SetOSFail
** \sa #CFE_ES_TaskInit, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_SetBSPloadAppFileResult, #UT_SetStatusBSPResetArea
** \sa #CFE_ES_HousekeepingCmd, #CFE_ES_NoopCmd, #CFE_ES_ResetCmd
** \sa #CFE_ES_RestartCmd, #CFE_ES_ShellCmd, #CFE_ES_StartAppCmd
** \sa #CFE_ES_StopAppCmd, #CFE_ES_RestartAppCmd, #CFE_ES_ReloadAppCmd
** \sa #CFE_ES_QueryOneCmd, #CFE_ES_QueryAllCmd, #CFE_ES_QueryAllTasksCmd
** \sa #CFE_ES_ClearSyslogCmd, #CFE_ES_OverWriteSyslogCmd
** \sa #CFE_ES_WriteSyslogCmd, #CFE_ES_ClearERlogCmd, #CFE_ES_WriteERlogCmd
** \sa #CFE_ES_ResetPRCountCmd, #CFE_ES_SetMaxPRCountCmd, #CFE_ES_DeleteCDSCmd
** \sa #CFE_ES_DumpCDSRegCmd, #CFE_ES_TlmPoolStatsCmd, #CFE_ES_TaskPipe
**
******************************************************************************/
void TestTask(void);

/*****************************************************************************/
/**
** \brief Performs tests on the functions that implement the software timing
**        performance marker functions contained in cfe_es_perf.c
**
** \par Description
**        This function tests the performance logging functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #CFE_ES_SetupPerfVariables, #UT_Report, #UT_SetRtnCode
** \sa #UT_SetSBTotalMsgLen, #UT_SendMsg, #UT_SetBSPloadAppFileResult
** \sa #CFE_ES_PerfLogDump, #CFE_ES_PerfLogAdd, #CFE_ES_PerfStartDataCmd
** \sa #CFE_ES_PerfStopDataCmd, #CFE_ES_PerfSetFilterMaskCmd
** \sa #CFE_ES_PerfSetTriggerMaskCmd, #CFE_ES_PerfLogDump, #CFE_ES_PerfLogAdd
**
******************************************************************************/
void TestPerf(void);

/*****************************************************************************/
/**
** \brief Perform tests on the ES API functions contained in cfe_es_api.c
**
** \par Description
**        This function tests the application programmer's interface functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_SetRtnCode, #CFE_ES_ResetCFE, #UT_Report
** \sa #CFE_ES_GetResetType, #CFE_ES_RestartApp, #CFE_ES_ReloadApp
** \sa #CFE_ES_DeleteApp, #CFE_ES_ExitApp, #CFE_ES_RunLoop
** \sa #CFE_ES_RegisterApp, #CFE_ES_GetAppID, #CFE_ES_GetAppName
** \sa #CFE_ES_GetTaskInfo, #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask
** \sa #CFE_ES_ExitChildTask, #CFE_ES_RegisterChildTask, #CFE_ES_WriteToSysLog
** \sa #CFE_ES_RegisterDriver, #CFE_ES_UnloadDriver, #CFE_ES_CalculateCRC
** \sa #UT_SetBSPFail, #CFE_ES_RegisterCDS, #CFE_ES_CopyToCDS
** \sa #CFE_ES_RestoreFromCDS, #CFE_ES_LockSharedData, #CFE_ES_UnlockSharedData
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_GetGenCounterIDByName
** \sa #CFE_ES_DeleteGenCounter, #CFE_ES_IncrementGenCounter
** \sa #CFE_ES_GetGenCount, #CFE_ES_SetGenCount, #CFE_ES_ProcessCoreException
** \sa #CFE_ES_WaitForStartupSync
**
******************************************************************************/
void TestAPI(void);

/*****************************************************************************/
/**
** \brief Perform tests on critical data store functions contained in
**        cfe_es_cds.c
**
** \par Description
**        This function tests the CDS registry manipulation functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_ES_CDS_ValidateAppID
** \sa #UT_SetBSPFail, #CFE_ES_RebuildCDS, #UT_SetRtnCode
** \sa #CFE_ES_InitCDSRegistry, #UT_SetCDSSize, #CFE_ES_CDS_EarlyInit
** \sa #UT_SetCDSBSPCheckValidity, #CFE_ES_ValidateCDS, #UT_SetCDSReadGoodEnd
** \sa #CFE_ES_InitializeCDS, #CFE_ES_RebuildCDS, #CFE_ES_DeleteCDS
**
******************************************************************************/
void TestCDS(void);

/*****************************************************************************/
/**
** \brief Performs tests on the functions for managing the CDS discrete sized
**        memory pools contained in cfe_es_cds_mempools.c
**
** \par Description
**        This function tests the CDS memory pool create, allocate, rebuild,
**        read, and write functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_ES_CreateCDSPool
** \sa #CFE_ES_RebuildCDSPool, #UT_SetRtnCode, #UT_SetBSPFail
** \sa #CFE_ES_GetCDSBlock, #CFE_ES_PutCDSBlock, #CFE_ES_CDSBlockWrite
** \sa #CFE_ES_CDSBlockRead
**
******************************************************************************/
void TestCDSMempool(void);

/*****************************************************************************/
/**
** \brief Perform tests on the set of services for management of discrete sized
**        memory pools contained in cde_esmempool.c
**
** \par Description
**        This function tests the ES memory pool create, allocate, rebuild,
**        read, and write functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_ES_PoolCreateNoSem
** \sa #CFE_ES_PoolCreate, #CFE_ES_GetPoolBuf, #CFE_ES_GetPoolBufInfo
** \sa #CFE_ES_PutPoolBuf, #CFE_ES_ValidateHandle, #UT_SetRtnCode
** \sa #CFE_ES_GetMemPoolStats, #CFE_ES_PoolCreateEx, #CFE_ES_PoolCreateNoSem
**
******************************************************************************/
void TestESMempool(void);

#ifdef CFE_ARINC653
/*****************************************************************************/
/**
** \brief Perform tests on the  cFE applications from statically linked code
**
** \par Description
**        This function tests the code for creating and loading cFE
**        applications from statically linked code, and for initializing
**        static libraries.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_InitData, #UT_Report, CFE_ES_StaticAppCreate, #UT_SetOSFail
** \sa #UT_SetLibInitRtn, #CFE_ES_InitStaticLibrary
** \sa #CFE_ES_StartStaticApplications
**
******************************************************************************/
void TestStaticApp(void);
#endif

/*****************************************************************************/
/**
** \brief Unit test specific call to process SB messages
**
** \par Description
**        This function serves as a pass-through for messages coming from the
**        CFE_SB_SendMsg() stub function.  By using a common pass-through
**        function name, the stub can be generic for all of the tests for the
**        various services (i.e., EVS, TBL, etc.).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #CFE_ES_TaskPipe
**
******************************************************************************/
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr);

#endif /* _es_ut_h_ */
