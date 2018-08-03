/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    es_UT.c
**
** Purpose:
**    Executive Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/09/23 18:20:57GMT-05:00 $
** $Revision: 1.18 $
**
*/

/*
** Includes
*/
#include "es_UT.h"

/*
** External global variables
*/
extern UT_SetRtn_t MutSemGiveRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t MutSemGetInfoRtn;
extern UT_SetRtn_t FSWriteHdrRtn;
extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t OSReadRtn;
extern UT_SetRtn_t OSlseekRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t BSPWriteCDSRtn;
extern UT_SetRtn_t BSPReadCDSRtn;
extern UT_SetRtn_t BlocksFreeRtn;
extern UT_SetRtn_t UnmountRtn;
extern UT_SetRtn_t EVSCleanUpRtn;
extern UT_SetRtn_t SB_SubscribeExRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t SBSendMsgRtn;
extern UT_SetRtn_t CountSemDelRtn;
extern UT_SetRtn_t MutSemDelRtn;
extern UT_SetRtn_t BinSemDelRtn;
extern UT_SetRtn_t QueueDelRtn;
extern UT_SetRtn_t HeapGetInfoRtn;
extern UT_SetRtn_t BSPGetCFETextRtn;
extern UT_SetRtn_t FSIsGzFileRtn;
extern UT_SetRtn_t ModuleLoadRtn;
extern UT_SetRtn_t ModuleUnloadRtn;
extern UT_SetRtn_t ModuleInfoRtn;
extern UT_SetRtn_t FSDecompressRtn;
extern UT_SetRtn_t FSExtractRtn;
extern UT_SetRtn_t SymbolLookupRtn;
extern UT_SetRtn_t OSPrintRtn;
extern UT_SetRtn_t TaskGetInfoRtn;
extern UT_SetRtn_t OSTaskExitRtn;
extern UT_SetRtn_t PSPRestartRtn;
extern UT_SetRtn_t OSBinSemTimedWaitRtn;
extern UT_SetRtn_t OSBinSemFlushRtn;
extern UT_SetRtn_t PSPPanicRtn;
extern UT_SetRtn_t TBLEarlyInitRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t OSCloseRtn;
extern UT_SetRtn_t PSPMemValRangeRtn;
extern UT_SetRtn_t OSTimerGetInfoRtn;
extern UT_SetRtn_t BinSemGetInfoRtn;
extern UT_SetRtn_t TimerGetInfoRtn2;
extern UT_SetRtn_t FDGetInfoRtn;
extern UT_SetRtn_t TimerDeleteRtn;
extern UT_SetRtn_t QueueGetInfoRtn;
extern UT_SetRtn_t CountSemGetInfoRtn;


extern CFE_ES_PerfData_t     *Perf;
extern CFE_ES_Global_t       CFE_ES_Global;
extern CFE_ES_CDSBlockDesc_t CFE_ES_CDSBlockDesc;
extern CFE_ES_TaskData_t     CFE_ES_TaskData;
extern CFE_SB_MsgId_t        UT_RcvMsgId;
extern CFE_ES_CDSPool_t      CFE_ES_CDSMemPool;

#ifdef CFE_ARINC653
extern CFE_ES_StaticStartupTable_t CFE_ES_StaticStartupTable[];
#endif

/*
** Global variables
*/
/* Create a startup script buffer for a maximum of 5 lines * 80 chars/line */
char StartupScript[MAX_STARTUP_SCRIPT];

/*
 * Macro to convert UT OSAL IDs to array index
 * This should work for both the opaque and non-opaque IDs as
 * provided by UT stub libraries (not for real code, though!)
 */
#define ES_UT_OSALID_TO_ARRAYIDX(x)         (x & 0xFF)


/*
** Functions
*/
#ifdef OSAL_OPAQUE_OBJECT_IDS
int32 ES_UT_SetupOSCleanupHook(void *UserObj, int32 StubRetcode,
                               uint32 CallCount,
                               const UT_StubContext_t *Context)
{
    uint32 ObjList[7];

    /* On the first call, Use the stub functions to generate one object of
     * each type
     */
    if (CallCount == 0)
    {
        OS_TaskCreate(&ObjList[0], NULL, NULL, NULL, 0, 0, 0);
        OS_QueueCreate(&ObjList[1], NULL, 0, 0, 0);
        OS_MutSemCreate(&ObjList[2], NULL, 0);
        OS_BinSemCreate(&ObjList[3], NULL, 0, 0);
        OS_CountSemCreate(&ObjList[4], NULL, 0, 0);
        OS_TimerCreate(&ObjList[5], NULL, NULL, NULL);
        ObjList[6] = OS_open(NULL, 0, 0);

        UT_SetDataBuffer((UT_EntryKey_t)&OS_ForEachObject, ObjList,
                          sizeof(ObjList), TRUE);
    }

    return StubRetcode;
}
#endif

static void ES_UT_SetupForOSCleanup(void)
{
#ifdef OSAL_OPAQUE_OBJECT_IDS
    UT_SetHookFunction(UT_KEY(OS_ForEachObject), ES_UT_SetupOSCleanupHook,
                       NULL);
#endif
}

static int32 ES_UT_IncrementU32CounterHook(void *UserObj, int32 StubRetcode,
                                           uint32 CallCount,
                                           const UT_StubContext_t *Context)
{
    uint32 *CounterPtr = (uint32 *)UserObj;

    ++(*CounterPtr);
    return StubRetcode;
}

void OS_Application_Startup(void)
{
    UT_Init("es");
    UT_Text("cFE ES Unit Test Output File\n\n");

    /* Set up the performance logging variable */
    Perf = (CFE_ES_PerfData_t *) &CFE_ES_ResetDataPtr->Perf;

    UT_ADD_TEST(TestInit);
    UT_ADD_TEST(TestStartupErrorPaths);
    UT_ADD_TEST(TestApps);
    UT_ADD_TEST(TestERLog);
    UT_ADD_TEST(TestShell);
    UT_ADD_TEST(TestTask);
    UT_ADD_TEST(TestPerf);
    UT_ADD_TEST(TestAPI);
    UT_ADD_TEST(TestCDS);
    UT_ADD_TEST(TestCDSMempool);
    UT_ADD_TEST(TestESMempool);

#ifdef CFE_ARINC653
    UT_ADD_TEST(TestStaticApp);
#endif
}

/*
** Reset variable values prior to a test
*/
void ES_ResetUnitTest(void)
{
    int j;

    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
        CFE_ES_Global.AppTable[j].RecordUsed = FALSE;
    }

    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        CFE_ES_Global.TaskTable[j].RecordUsed = FALSE;
    }

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = FALSE;
    }
} /* end ES_ResetUnitTest() */

void TestInit(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

    UT_SetCDSSize(128 * 1024);
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));

    /* Set up the reset area */
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));

    /* Go through ES_Main and cover normal paths */
    UT_SetDummyFuncRtn(OS_SUCCESS);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1,
                CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              PSPPanicRtn.value == 0 && PSPPanicRtn.count == 0,
              "CFE_ES_Main",
              "Normal startup");
}

void TestStartupErrorPaths(void)
{
    int j;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Startup Error Paths\n");
#endif

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';

    /* Perform ES main startup with a mutex creation failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_MUTCREATE_FAIL);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1,
                CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              PSPPanicRtn.value == CFE_PSP_PANIC_STARTUP_SEM &&
              PSPPanicRtn.count == 1,
              "CFE_ES_Main",
              "Mutex create failure");

    /* Perform ES main startup with a file open failure */
    UT_SetDummyFuncRtn(OS_SUCCESS);
    UT_SetOSFail(OS_OPEN_FAIL);
    CFE_ES_Main(CFE_ES_POWERON_RESET, 1, 1,
                (char *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CANNOT_OPEN_ES_APP_STARTUP,
              "CFE_ES_Main",
              "File open failure");

    /* Perform ES main startup with a startup sync failure */
    OSPrintRtn.value = 0;
    CFE_ES_Global.AppStartedCount++;
    CFE_ES_Main(CFE_ES_POWERON_RESET, 1, 1,
                (char *) CFE_ES_NONVOL_STARTUP_FILE);
    CFE_ES_Global.AppStartedCount--;
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CANNOT_OPEN_ES_APP_STARTUP +
                UT_OSP_STARTUP_SYNC_FAIL,
              "CFE_ES_Main",
              "Startup sync failure");

    /* Perform a power on reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_ES_POWERON_RESET,
                               CFE_ES_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_POR_HW_SPECIAL,
              "CFE_ES_SetupResetVariables",
              "Power on reset - Hardware special command");

    /* Perform a processor reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET,
                               CFE_ES_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_PROC_RESET_MAX_HW_SPECIAL,
              "CFE_ES_SetupResetVariables",
              "Processor reset - hardware special command");

    /* Perform a power on reset with an "other cause" sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_ES_POWERON_RESET, -1, 1);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_POR_OTHER,
              "CFE_ES_SetupResetVariables",
              "Other cause reset");

    /* Perform the maximum number of processor resets */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = FALSE;

    for (j = 0; j < CFE_ES_MAX_PROCESSOR_RESETS; j++)
    {
        CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET,
                                   CFE_ES_POWER_CYCLE, 1);
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount ==
                  CFE_ES_MAX_PROCESSOR_RESETS,
              "CFE_ES_SetupResetVariables",
              "Maximum processor resets");

    /* Attempt another processor reset after the maximum have occurred */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_ES_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount ==
                  CFE_ES_MAX_PROCESSOR_RESETS + 1 &&
              PSPRestartRtn.value == CFE_PSP_RST_TYPE_POWERON &&
              PSPRestartRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "Processor reset - power cycle; exceeded maximum "
                  "processor resets");

    /* Perform a power on reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET,
                               CFE_ES_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_POR_MAX_HW_SPECIAL,
              "CFE_ES_SetupResetVariables",
              "Processor reset - hardware special command; exceeded maximum "
                  "processor resets");

    /* Perform a processor reset with an reset area failure */
    ES_ResetUnitTest();
    UT_SetStatusBSPResetArea(OS_ERROR, 0, CFE_TIME_TONE_PRI);
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = TRUE;
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_ES_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              PSPPanicRtn.value == CFE_PSP_PANIC_MEMORY_ALLOC &&
              PSPPanicRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "Get reset area error");

    /* Perform a processor reset with the size of the reset area too small */
    ES_ResetUnitTest();
    UT_SetSizeofESResetArea(0);
    UT_SetStatusBSPResetArea(OS_SUCCESS, 0, CFE_TIME_TONE_PRI);
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_ES_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              PSPPanicRtn.value == CFE_PSP_PANIC_MEMORY_ALLOC &&
              PSPPanicRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "Reset area too small");

    /* Test initialization of the file systems specifying a power on reset
     * following a failure to create the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_INITFS_FAIL | OS_MOUNT_FAIL | OS_MKFS_FAIL);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_POWERON);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CREATE_VOLATILE +
                  UT_OSP_MOUNT_VOLATILE && OSPrintRtn.count == -3,
              "CFE_ES_InitializeFileSystems",
              "Power on reset; error creating volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to reformat the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_INITFS_FAIL | OS_MOUNT_FAIL | OS_MKFS_FAIL);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_INSUFF_FREE_SPACE +
                  UT_OSP_CREATE_VOLATILE + UT_OSP_INIT_VOLATILE +
                  UT_OSP_MOUNT_VOLATILE + UT_OSP_REFORMAT_VOLATILE &&
              OSPrintRtn.count == -8,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error reformatting volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following failure to get the volatile disk memory
     */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_GETVOLDISKMEM_FAIL);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_INSUFF_FREE_SPACE &&
              OSPrintRtn.count == -4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; cannot get memory for volatile disk");

    /* Test initialization of the file systems where the number of free blocks
     * reported is greater than the number of RAM disk sectors
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_MOUNT_FAIL);
    UT_SetRtnCode(&BlocksFreeRtn, CFE_ES_RAM_DISK_NUM_SECTORS + 1, 1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_MOUNT_VOLATILE &&
                OSPrintRtn.count == -3,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; truncate free block count");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remove the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_RMFS_FAIL);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_INSUFF_FREE_SPACE +
                UT_OSP_REMOVE_VOLATILE && OSPrintRtn.count == -4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error removing volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to unmount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&UnmountRtn, -1, 1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_INSUFF_FREE_SPACE +
                  UT_OSP_UNMOUNT_VOLATILE && OSPrintRtn.count == -4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error unmounting volatile (RAM) volume");

    /* Test successful initialization of the file systems */
    ES_ResetUnitTest();
    CFE_ES_InitializeFileSystems(4564564);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_MESSAGE && OSPrintRtn.count == -1,
              "CFE_ES_InitializeFileSystems",
              "Initialize file systems; successful");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_MOUNT_FAIL);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_INSUFF_FREE_SPACE +
                  UT_OSP_MOUNT_VOLATILE + UT_OSP_REMOUNT_VOLATILE &&
                  OSPrintRtn.count == -5,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error remounting volatile (RAM) volume");

    /* Test initialization of the file systems with an error determining the
     * number of blocks that are free on the volume
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BlocksFreeRtn, -1, 1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_DETERMINE_BLOCKS &&
                  OSPrintRtn.count == -2,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error determining blocks free on volume");

    /* Test reading the object table where a record used error occurs */
    ES_ResetUnitTest();
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);

    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        /* Mark all entries as "used" to ensure that the log message gets
         * output
         */
        CFE_ES_Global.TaskTable[j].RecordUsed = TRUE;
    }

    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_RECORD_USED &&
                  OSPrintRtn.count == -19,
              "CFE_ES_CreateObjects",
              "Record used error");

    /* Test reading the object table where an error occurs when
     * calling a function
     */
    ES_ResetUnitTest();

    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        /* Mark all entries as "used" to ensure that the log message gets
         * output
         */
        CFE_ES_Global.TaskTable[j].RecordUsed = TRUE;
    }

    UT_SetRtnCode(&TBLEarlyInitRtn, 0, 1);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_RECORD_USED + UT_OSP_EARLYINIT &&
                  OSPrintRtn.count == -20,
              "CFE_ES_CreateObjects",
              "Error returned when calling function");

    /* Test reading the object table where an error occurs when
     * creating a core app
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKCREATE_FAIL | OS_SEMCREATE_FAIL);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CORE_APP_CREATE &&
                  OSPrintRtn.count == -14,
              "CFE_ES_CreateObjects",
              "Error creating core application");

    /* Test reading the object table where all app slots are taken */
    ES_ResetUnitTest();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
    }

    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_FREE_CORE_APP_SLOTS * 5 &&
                  OSPrintRtn.count == -14,
              "CFE_ES_CreateObjects",
              "No free application slots available");

    /* Test reading the object table with a NULL function pointer */
    ES_ResetUnitTest();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
    }

    CFE_ES_ObjectTable[1].ObjectType = CFE_ES_FUNCTION_CALL;
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_FREE_CORE_APP_SLOTS * 5 +
                   UT_OSP_FUNCTION_POINTER && OSPrintRtn.count == -15,
              "CFE_ES_CreateObjects",
              "Bad function pointer");

    /* Test response to an invalid startup type */
    ES_ResetUnitTest();
    CFE_ES_Global.DebugVars.DebugFlag = 1;
    CFE_ES_SetupResetVariables(-1, CFE_ES_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.DebugVars.DebugFlag == 1,
              "CFE_ES_SetupResetVariables",
              "Invalid startup type");
    CFE_ES_Global.DebugVars.DebugFlag = 0;

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to initialize and mount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_INITFS_FAIL | OS_MOUNT_FAIL);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_INSUFF_FREE_SPACE +
                  UT_OSP_INIT_VOLATILE + UT_OSP_MOUNT_VOLATILE +
                  UT_OSP_REMOUNT_VOLATILE &&
              OSPrintRtn.count == -7,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error initializing and mounting volatile "
                "(RAM) volume");

    /* Test application sync delay where the operation times out */
    ES_ResetUnitTest();
    CFE_ES_Global.AppStartedCount++;
    UT_Report(__FILE__, __LINE__,
            CFE_ES_ApplicationSyncDelay(CFE_ES_SYSTEM_STATE_OPERATIONAL,
                                        CFE_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                                          CFE_SUCCESS,
              "CFE_ES_ApplicationSyncDelay",
              "Operation timed out");
    CFE_ES_Global.AppStartedCount--;
}

void TestApps(void)
{
    int NumBytes;
    int Return;
    int j;
    CFE_ES_AppInfo_t AppInfo;
    char LongFileName[OS_MAX_PATH_LEN + 9];
    uint32 Id, Id2, Id3, Id4;
    uint32 TestObjId, TestObjId2, TestObjId3, TestObjId4;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Apps\n");
#endif

    /* Test starting an application where the startup script is too long */
    ES_ResetUnitTest();
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_fghfghfghfghfg"
            "hfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghLIB, "
            "0, 0, 0x0, 1; CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, "
            "70, 4096, 0x0, 1; CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, "
            "SCH_APP, 120, 4096, 0x0, 1; CFE_APP, /cf/apps/to.bundle, "
            "TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_FILE_LINE_TOO_LONG +
                 UT_OSP_ES_APP_STARTUP_OPEN && OSPrintRtn.count == -9,
              "CFE_ES_StartApplications",
              "Line too long");

    /* Create a valid startup script for subsequent tests */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);

    /* Test starting an application with an error reading the startup file */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSReadRtn, -1, 1);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_STARTUP_READ +
                 UT_OSP_ES_APP_STARTUP_OPEN && OSPrintRtn.count == -3,
              "CFE_ES_StartApplications",
              "Error reading startup file");

    /* Test starting an application with an end-of-file returned by the
     * OS read
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSReadRtn, 0, 1);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_ES_APP_STARTUP_OPEN &&
                 OSPrintRtn.count == -2,
              "CFE_ES_StartApplications",
              "End of file reached");

    /* Test starting an application with an open failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_OPEN_FAIL);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CANNOT_OPEN_ES_APP_STARTUP &&
                 OSPrintRtn.count == -3,
              "CFE_ES_StartApplications",
              "Can't open ES application startup file");

    /* Test successfully starting an application */
    ES_ResetUnitTest();
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_ES_APP_STARTUP_OPEN &&
                 OSPrintRtn.count == -9,
              "CFE_ES_StartApplications",
              "Start application; successful");

    /* Test parsing the startup script with an invalid CFE driver type */
    ES_ResetUnitTest();
    strncpy(StartupScript,
            "CFE_DRV /cf/apps/tst_lib.bundle TST_LIB_Init TST_LIB 0 0 0x0 1",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ParseFileEntry(StartupScript) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Unimplemented CFE driver type");

    /* Test parsing the startup script with an unknown entry type */
    ES_ResetUnitTest();
    strncpy(StartupScript,
            "UNKNOWN /cf/apps/tst_lib.bundle TST_LIB_Init TST_LIB 0 0 0x0 1",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ParseFileEntry(StartupScript) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Unknown entry type");

    /* Test parsing the startup script with an invalid file entry */
    ES_ResetUnitTest();
    strcpy(StartupScript, "");
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ParseFileEntry(StartupScript) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Invalid file entry");

    /* Test application loading and creation with a task creation failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename",
                              "EntryPoint",
                              "AppName",
                              170,
                              4096,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == UT_OSP_APP_CREATE,
              "CFE_ES_AppCreate",
              "Task create failure");

    /* Test successful application loading and creation  */
    ES_ResetUnitTest();
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS,
              "CFE_ES_AppCreate",
              "Application load/create; successful");

    /* Test application loading and creation with a file
     * decompression failure
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSDecompressRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == UT_OSP_DECOMPRESS_APP,
              "CFE_ES_AppCreate",
              "Decompression failure");

    /* Test application loading, creation and decompression */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSDecompressRtn, 0, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
              OSPrintRtn.value == UT_OSP_TABLE_SLOT_IN_USE,
              "CFE_ES_AppCreate",
              "Decompression; successful");

    /* Test application loading and creation where the file name cannot be
     * extracted from the path
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSExtractRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
                OSPrintRtn.value == UT_OSP_EXTRACT_FILENAME_UT,
              "CFE_ES_AppCreate",
              "File name extraction failure");

    /* Test application loading and creation where the file cannot be loaded */
    UT_InitData();
    UT_SetRtnCode(&ModuleLoadRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
                OSPrintRtn.value == UT_OSP_EXTRACT_FILENAME_UT55,
              "CFE_ES_AppCreate",
              "File load failure");

    /* Test application loading and creation where all app slots are taken */
    ES_ResetUnitTest();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
    }

    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_IncrementU32CounterHook,
                       &CFE_ES_Global.AppReadyCount);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
                OSPrintRtn.value == UT_OSP_NO_FREE_APP_SLOTS,
              "CFE_ES_AppCreate",
              "No free application slots available");

    /* Test application loading and creation where the entry point symbol
     * cannot be found
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&SymbolLookupRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == UT_OSP_CANNOT_FIND_SYMBOL,
              "CFE_ES_AppCreate",
              "Entry point symbol lookup failure");

    /* Test application loading and creation where the application file name
     * is too long
     */
    UT_InitData();
    strcpy(LongFileName, "ut57/");

    for (j = 0; j < OS_MAX_PATH_LEN; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    Return = CFE_ES_AppCreate(&Id,
                              LongFileName,
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == UT_OSP_EXTRACT_FILENAME_UT57,
              "CFE_ES_AppCreate",
              "Application file name too long");

    /* Test application loading and creation where the application path + file
     * name is too long
     */
    UT_InitData();
    strcpy(LongFileName, "ut58/");

    for (j = 0; j < OS_MAX_PATH_LEN - 6; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    Return = CFE_ES_AppCreate(&Id,
                              LongFileName,
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == UT_OSP_APP_PATH_FILE_TOO_LONG,
              "CFE_ES_AppCreate",
              "Application file name + path too long");

    /* Test shared library loading and initialization where the initialization
     * routine returns an error
     */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = FALSE;
    }

    UT_SetDummyFuncRtn(OS_ERROR);
    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                "LibName");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_SHARED_LIBRARY_INIT,
              "CFE_ES_LoadLibrary",
              "Load shared library initialization failure");

    /* Test successful shared library loading and initialization with a
     * gzip'd library
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS,
              "CFE_ES_LoadLibrary",
              "Decompress library; successful");

    /* Test shared library loading and initialization with a gzip'd library
     * where the decompression fails
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSDecompressRtn, -1, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_DECOMPRESS_LIBRARY,
              "CFE_ES_LoadLibrary",
              "Unable to decompress library");

    /* Test shared library loading and initialization where file name cannot
     * be extracted from the path
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSExtractRtn, -1, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_EXTRACT_FILENAME_CF,
              "CFE_ES_LoadLibrary",
              "Unable to extract file name from path");

    /* Test shared library loading and initialization where the file name is
     * too long
     */
    UT_InitData();
    strcpy(LongFileName, "ut46/");

    for (j = 0; j < OS_MAX_PATH_LEN; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                LongFileName,
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_EXTRACT_FILENAME_UT46,
              "CFE_ES_LoadLibrary",
              "Unable to extract file name from path; file name too long");

    /* Test shared library loading and initialization where the library path +
     * file name is too long
     */
    UT_InitData();
    strcpy(LongFileName, "ut47/");

    for (j = 0; j < OS_MAX_PATH_LEN - 6; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                LongFileName,
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_LIB_PATH_FILE_TOO_LONG,
              "CFE_ES_LoadLibrary",
              "Library path + file name too long");

    /* Test shared library loading and initialization where the library
     * fails to load
     */
    UT_InitData();
    UT_SetRtnCode(&ModuleLoadRtn, -1, 1);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_LOAD_SHARED_LIBRARY,
              "CFE_ES_LoadLibrary",
              "Load shared library failure");

    /* Test shared library loading and initialization where the library
     * entry point symbol cannot be found
     */
    UT_InitData();
    UT_SetRtnCode(&SymbolLookupRtn, -1, 1);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_FIND_LIBRARY,
              "CFE_ES_LoadLibrary",
              "Could not find library initialization symbol");

    /* Test shared library loading and initialization where there are no
     * library slots available
     */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = TRUE;
    }

    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                "LibName");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == UT_OSP_LIBRARY_SLOTS,
              "CFE_ES_LoadLibrary",
              "No free library slots");

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    CFE_ES_Global.AppTable[Id].StateRecord.AppTimer = 0;
    CFE_ES_ScanAppTable();
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PCR_ERR2_EID &&
              CFE_ES_Global.AppTable[Id].StateRecord.AppTimer == 0,
              "CFE_ES_ScanAppTable",
              "Waiting; process control request");

    /* Test scanning and acting on the application table where the timer
     * has not expired for a waiting application
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    CFE_ES_Global.AppTable[Id].StateRecord.AppTimer = 5;
    CFE_ES_ScanAppTable();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.AppTable[Id].StateRecord.AppTimer == 4,
              "CFE_ES_ScanAppTable",
              "Decrement timer");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_ScanAppTable();
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PCR_ERR2_EID &&
              CFE_ES_Global.AppTable[Id].StateRecord.AppTimer > 0,
              "CFE_ES_ScanAppTable",
              "Stopped; process control request");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_Global.AppTable[Id].StateRecord.AppTimer = 5;

    CFE_ES_ScanAppTable();
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == -1 &&
              CFE_ES_Global.AppTable[Id].StateRecord.AppTimer == 5,
              "CFE_ES_ScanAppTable",
              "Initializing; process control request");

   /* Test a control action request on an application with an
     * undefined control request state
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest = 0;
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PCR_ERR2_EID,
              "CFE_ES_ProcessControlRequest",
              "Unknown state (default)");

    /* Test a successful control action request to exit an application */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.FileName,
            "/ram/Filename", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[Id].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.EntryPoint,
            "NotNULL", OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[Id].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[Id].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction = 0;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
      CFE_ES_RUNSTATUS_APP_EXIT;
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_EXIT_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application; successful");

    /* Test a control action request to exit an application where the
     * request fails
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest = CFE_ES_RUNSTATUS_APP_EXIT;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_EXIT_APP_ERR_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application failure");

    /* Test a control action request to stop an application where the
     * request fails
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_DELETE;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_STOP_ERR3_EID,
              "CFE_ES_ProcessControlRequest",
              "Stop application failure");

    /* Test a control action request to restart an application where the
     * request fails due to a CleanUpApp error
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_RESTART;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR4_EID,
              "CFE_ES_ProcessControlRequest",
              "Restart application failure; CleanUpApp error");

    /* Test a control action request to restart an application where the
     * request fails due to an AppCreate error
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_RESTART;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR3_EID,
              "CFE_ES_ProcessControlRequest",
              "Restart application failure; AppCreate error");

    /* Test a control action request to reload an application where the
     * request fails due to a CleanUpApp error
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_RELOAD;

    /* Use the OSAL stubs to populate the MainTaskId and ModuleId fields with
     * something valid
     */
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR4_EID,
              "CFE_ES_ProcessControlRequest",
              "Reload application failure; CleanUpApp error");

    /* Test a control action request to reload an application where the
     * request fails due to an AppCreate error
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_RELOAD;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR3_EID,
              "CFE_ES_ProcessControlRequest",
              "Reload application failure; AppCreate error");

    /* Test a successful control action request to exit an application that
     * has an error
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[Id].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[Id].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[Id].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction = 0;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
      CFE_ES_RUNSTATUS_APP_ERROR;
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ERREXIT_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application on error; successful");

    /* Test a control action request to exit an application that
     * has an error where the request fails
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
      CFE_ES_RUNSTATUS_APP_ERROR;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ERREXIT_APP_ERR_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application on error failure");

    /* Test a successful control action request to stop an application */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[Id].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[Id].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[Id].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_DELETE;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_STOP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Stop application; successful");

    /* Test a successful control action request to restart an application */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[Id].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[Id].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[Id].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_RESTART;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Restart application; successful");

    /* Test a successful control action request to reload an application */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[Id].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[Id].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[Id].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_RELOAD;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Reload application; successful");

    /* Test a control action request for an application that has an invalid
     * state (exception)
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT",
                  NULL, NULL, 0, 0, 0);
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[Id].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[Id].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[Id].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_SYS_EXCEPTION;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(Id);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PCR_ERR1_EID,
              "CFE_ES_ProcessControlRequest",
              "Invalid state");

    /* Test listing the OS resources in use */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSPrintRtn, 0, 0);

    /* Fail the first time, then succeed on the second in order to test
     * both paths
     */
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetInfo), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResourcesDebug() == CFE_SUCCESS &&
              OSPrintRtn.count == 7,
              "CFE_ES_ListResourcesDebug",
              "Get task info failed");

    /* Test populating the application information structure with data */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_SUCCESS,
              "CFE_ES_GetAppInfo",
              "Get application information; successful");

    /* Test populating the application information structure with data using
     * a null application information pointer
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(NULL, Id) == CFE_ES_ERR_BUFFER,
              "CFE_ES_GetAppInfo",
              "Null application information pointer");

    /* Test populating the application information structure using an
     * inactive application ID
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppInfo",
              "Application ID not active");

    /* Test populating the application information structure using an
     * application ID value greater than the maximum allowed
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo,
                                CFE_ES_MAX_APPLICATIONS) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppInfo",
              "Application ID exceeds maximum");

    /* Test populating the application information structure using a valid
     * application ID, but with a failure to retrieve the module information
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;

    UT_SetRtnCode(&ModuleInfoRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_SUCCESS,
              "CFE_ES_GetAppInfo",
              "Module not found");

    /* Test deleting an application and cleaning up its resources with OS
     * delete and close failures
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    ES_UT_SetupForOSCleanup();

    /* Use the "TaskCreate" stub to populate the MainTaskId field with
     * something valid
     */
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT", NULL,
                  NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    UT_SetOSFail(OS_TASKDELETE_FAIL | OS_CLOSE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Task OS delete and close failure");

    /* Test deleting an application and cleaning up its resources with a
     * mutex delete failure
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id;
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Task mutex delete failure");

    /* Test deleting an application and cleaning up its resources with a
     * failure to unload the module
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId, "UT", NULL,
                  NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    UT_SetRtnCode(&ModuleUnloadRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Module unload failure");

    /* Test deleting an application and cleaning up its resources where the
     * EVS application cleanup fails
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId, "UT", NULL,
                  NULL, 0, 0, 0);
    OS_ModuleLoad(&CFE_ES_Global.AppTable[0].StartParams.ModuleId, NULL, NULL);
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "EVS application cleanup failure");

    /* Test cleaning up the OS resources for a task with a failure
     *  deleting mutexes
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) ==
                CFE_ES_MUT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Mutex delete failure");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * binary semaphores
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    ES_UT_SetupForOSCleanup();
    UT_SetRtnCode(&BinSemDelRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) ==
                CFE_ES_BIN_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Binary semaphore delete failure");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * counting semaphores
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    ES_UT_SetupForOSCleanup();
    UT_SetRtnCode(&CountSemDelRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) ==
                CFE_ES_COUNT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Counting semaphore failure");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting queues
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    ES_UT_SetupForOSCleanup();
    UT_SetRtnCode(&QueueDelRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) ==
                CFE_ES_QUEUE_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Queue delete failure");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting timers
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    ES_UT_SetupForOSCleanup();
    /* Just set OS_TimerDelete to fail.  There is no requirement
     * that the code call OS_TimerGetInfo first.
     */
    UT_SetDeferredRetcode(UT_KEY(OS_TimerDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) ==
                CFE_ES_TIMER_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Timer delete failure");

    /* Test cleaning up the OS resources for a task with a failure
     * closing files
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    ES_UT_SetupForOSCleanup();
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_SetOSFail(OS_CLOSE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) != CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "File close failure");

    /* Test cleaning up the OS resources for a task with a failure
     * to delete the task
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_SetOSFail(OS_TASKDELETE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) == CFE_ES_TASK_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Task delete failure");

    /* Test successfully cleaning up the OS resources for a task */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) == CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "Clean up task OS resources; successful");

    /* Test parsing the startup script for a cFE application and a restart
     * application exception action
     */
    ES_ResetUnitTest();
    strncpy(StartupScript,
            "CFE_APP /cf/apps/tst_lib.bundle TST_LIB_Init TST_LIB 0 0 0x0 0",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    CFE_ES_Global.AppStartedCount = 0;
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ParseFileEntry(StartupScript) == CFE_SUCCESS &&
              CFE_ES_Global.AppStartedCount == 1,
              "CFE_ES_ParseFileEntry",
              "CFE application; restart application on exception");

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    CFE_ES_Global.AppTable[Id].StateRecord.AppTimer = 0;
    CFE_ES_ScanAppTable();
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == -1 &&
              CFE_ES_Global.AppTable[Id].StateRecord.AppTimer == 0,
              "CFE_ES_ScanAppTable",
              "Waiting; process control request");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test scanning and acting on the application table where the application
     * is already running
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    CFE_ES_Global.AppTable[Id].StateRecord.AppTimer = 0;
    CFE_ES_ScanAppTable();
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == -1 &&
              CFE_ES_Global.AppTable[Id].StateRecord.AppTimer == 0,
              "CFE_ES_ScanAppTable",
              "Running; process control request");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test deleting an application and cleaning up its resources where the
     * application ID matches the main task ID
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    OS_TaskCreate(&TestObjId3, "UT", NULL, NULL, 0, 0, 0);
    Id3 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId3);
    CFE_ES_Global.TaskTable[Id2].TaskId = TestObjId3;
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id].StartParams.ModuleId, NULL, NULL);
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId3;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id) == CFE_SUCCESS &&
              CFE_ES_Global.TaskTable[Id2].RecordUsed == TRUE,
              "CFE_ES_CleanUpApp",
              "Main task ID matches task ID");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test deleting an application and cleaning up its resources where the
     * application ID doesn't match the main task ID
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    OS_TaskCreate(&TestObjId3, "UT", NULL, NULL, 0, 0, 0);
    Id3 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId3);
    OS_TaskCreate(&TestObjId4, "UT", NULL, NULL, 0, 0, 0);
    Id4 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId4);
    CFE_ES_Global.TaskTable[Id2].TaskId = Id3;
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id4;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = Id2;
    UT_SetOSFail(OS_TASKDELETE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id4) == CFE_ES_APP_CLEANUP_ERR &&
              CFE_ES_Global.TaskTable[Id2].RecordUsed == TRUE,
              "CFE_ES_CleanUpApp",
              "Main task ID doesn't match task ID");

    /* Test deleting an application and cleaning up its resources where the
     * application ID doesn't match and the application is a core application
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    OS_TaskCreate(&TestObjId3, "UT", NULL, NULL, 0, 0, 0);
    Id3 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId3);
    CFE_ES_Global.TaskTable[Id].TaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id2;
    CFE_ES_Global.RegisteredExternalApps = 1;
    CFE_ES_Global.AppTable[Id3].TaskInfo.MainTaskId = TestObjId3;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id3) == CFE_SUCCESS &&
              CFE_ES_Global.TaskTable[Id].RecordUsed == TRUE &&
              CFE_ES_Global.RegisteredExternalApps == 1,
              "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test successfully deleting an application and cleaning up its resources
     * and the application is an external application
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    CFE_ES_Global.TaskTable[Id].TaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id2;
    CFE_ES_Global.RegisteredExternalApps = 1;
    CFE_ES_Global.AppTable[Id2].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.AppTable[Id2].Type = CFE_ES_APP_TYPE_EXTERNAL;
    OS_ModuleLoad(&CFE_ES_Global.AppTable[Id2].StartParams.ModuleId, "UT",
                  "ut-module");
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(Id2) == CFE_SUCCESS &&
              CFE_ES_Global.TaskTable[Id].RecordUsed == FALSE &&
              CFE_ES_Global.RegisteredExternalApps == 0,
              "CFE_ES_CleanUpApp",
              "Successful application cleanup; external application");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test cleaning up the OS resources for a task with failure to
     * obtain information on mutex, binary, and counter semaphores, and
     * queues, timers, and file descriptors
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_FDGetInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(TestObjId) == CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "Get OS information failures");

    /* Test cleaning up the OS resources for a task with failure to
     * obtain information on mutex, binary, and counter semaphores, and
     * queues, timers, and file descriptors
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_FDGetInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResourcesDebug() == CFE_SUCCESS,
              "CFE_ES_ListResourcesDebug",
              "Get OS information failures");

    /* Fail the first time, then succeed on the second in order to test
     * both paths
     */
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_FDGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "Get task info failed");

    /* Fail the file write the second time in order to test the second path */
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "File write failed (second path)");

    /* Fail the file write the third time in order to test the third path */
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 3);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "File write failed (third path)");

    /* Fail the file write the fourth time in order to test the fourth path */
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 4);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "File write failed (fourth path)");

    /* Fail the file write the fifth time in order to test the fifth path */
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 5);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "File write failed (fifth path)");

    /* Fail the file write the sixth time in order to test the sixth path */
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 6);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "File write failed (sixth path)");

    /* Fail the file write the seventh time in order to test the seventh
     * path
     */
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 7);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListResources(0) == OS_ERROR,
              "CFE_ES_ListResources",
              "File write failed (seventh path)");
}

void TestERLog(void)
{
    int Return;
    uint32 Context = 4;
    char Context2[1000];

#ifdef UT_VERBOSE
    UT_Text("Begin Test Exception and Reset Log\n");
#endif

    /* Test initial rolling over log entry,
     * null description,
     * and non-null context with small size
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ERLogIndex = CFE_ES_ER_LOG_ENTRIES + 1;
    CFE_ES_ResetDataPtr->ERLog[0].ContextSize = 0;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_PSP_RST_TYPE_POWERON,
                                 1,
                                 NULL,
                                 &Context,
                                 sizeof(int));
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
              !strcmp(CFE_ES_ResetDataPtr->ERLog[0].Description,
                      "No Description String Given.") &&
              CFE_ES_ResetDataPtr->ERLogIndex == 1 &&
              CFE_ES_ResetDataPtr->ERLog[0].ContextSize == sizeof(int),
              "CFE_ES_WriteToERLog",
              "Log entries exceeded; no description; valid context size");

    /* Test initial rolling over log entry,
     * null description,
     * and non-null context with zero size
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ERLogIndex = 0;
    CFE_ES_ResetDataPtr->ERLog[0].ContextSize = 0;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_PSP_RST_TYPE_POWERON,
                                 1,
                                 NULL,
                                 &Context,
                                 0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
            CFE_ES_ResetDataPtr->ERLogIndex == 1 &&
            CFE_ES_ResetDataPtr->ERLog[0].ContextSize == 0,
              "CFE_ES_WriteToERLog",
              "Log entries exceeded; no description; invalid context size");

    /* Test rolling over log entry at end,
     * non-null description,
     * and non-null context with large size
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ERLogIndex = CFE_ES_ER_LOG_ENTRIES - 1;
    CFE_ES_ResetDataPtr->ERLog[CFE_ES_ER_LOG_ENTRIES - 1].ContextSize = 0;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_PSP_RST_TYPE_POWERON,
                                 1,
                                 "LogDescription",
                                 (uint32 *) &Context2,
                                 9999999);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
              !strcmp(CFE_ES_ResetDataPtr->ERLog[
                          CFE_ES_ER_LOG_ENTRIES - 1].Description,
                      "LogDescription") &&
              CFE_ES_ResetDataPtr->ERLogIndex == 0 &&
              CFE_ES_ResetDataPtr->ERLog[
                  CFE_ES_ER_LOG_ENTRIES - 1].ContextSize == 9999999,
              "CFE_ES_WriteToERLog",
              "Log entries at maximum; description; oversized context");

    /* Test non-rolling over log entry,
     * null description,
     * and null context
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ERLogIndex = 0;
    CFE_ES_ResetDataPtr->ERLog[0].ContextSize = 0;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_PSP_RST_TYPE_POWERON,
                                 1,
                                 NULL,
                                 NULL,
                                 1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
            CFE_ES_ResetDataPtr->ERLogIndex == 1 &&
            CFE_ES_ResetDataPtr->ERLog[0].ContextSize == 0,
              "CFE_ES_WriteToERLog",
              "No log entry rollover; no description; no context");
}

void TestShell(void)
{
    uint32 Id, Id2;
    uint32 TestObjId, TestObjId2;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Shell Command\n");
#endif

    /* Test shell output command using a non-existent ES command */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ES_NoSuchThing",
                                        "./FS_Test_File_Name") ==
                  CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "Invalid ES command");

    /* Test shell output command using a valid OS command */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ls", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send OS command");

    /* Test shell output command using an ES list applications command */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].TaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ES_ListApps", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send ES list applications command");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test shell output command using an ES list tasks command */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].TaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ES_ListTasks", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send ES list tasks command");
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;

    /* Test shell output command using an ES list resources command.  Alter
     * the OS_lseek() response to increase branch path coverage
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSlseekRtn, CFE_ES_MAX_SHELL_PKT + 1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ES_ListResources", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send ES list resources command");

    /* Test shell output command using a valid OS command but with a failed
     * OS lseek
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSlseekRtn, CFE_ES_MAX_SHELL_PKT - 2, 1);
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ls", "") == CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "OS command; OS lseek failure");

    /* Test shell output command using a valid OS command to start sending
     * packets down
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSlseekRtn, CFE_ES_MAX_SHELL_PKT * 2 + 1, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ls", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Multiple packets to send down");

    /* Test shell output command using a valid ES command but with a failed
     * OS create
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ES_ListApps",
                                        "") == CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "ES command; OS create failure");

    /* Test shell output command using a valid ES command but with a failed
     * OS lseek
     */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ShellOutputCommand("ES_ListApps",
                                        "") == CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "ES command; OS lseek failed");

    /* Test list application function with a failed OS write */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 1);
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id2].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListApplications(Id) == OS_ERROR,
              "CFE_ES_ListApplications",
              "File write error");
    CFE_ES_Global.AppTable[Id].RecordUsed = FALSE;
    CFE_ES_Global.AppTable[Id2].RecordUsed = FALSE;

    /* Test list application function with a failed OS seek */
    ES_ResetUnitTest();
    UT_SetRtnCode(&OSlseekRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListTasks(0) == OS_ERROR,
              "CFE_ES_ListTasks",
              "File seek error");

    /* Test list application function with a failed OS write */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListTasks(0) == OS_ERROR,
              "CFE_ES_ListTasks",
              "File write error");

    /* Test list application function with a failed OS write on a
     * subsequent pass
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id2;
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id2;
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListTasks(0) == OS_ERROR,
              "CFE_ES_ListTasks",
              "File write error (second pass)");

    /* Test list application function with a failure to get the task
     * information
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FileWriteRtn, OS_ERROR, 3);
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ListTasks(0) == CFE_ES_ERR_TASKID,
              "CFE_ES_ListTasks",
              "Get task info error");
}

void TestTask(void)
{
    uint32                      Id;
    uint32                      TestObjId;
    CFE_SB_MsgPtr_t             msgptr;
    CFE_ES_NoArgsCmd_t          NoArgsCmd;
    CFE_ES_RestartCmd_t         RestartCmd;
    CFE_ES_ShellCmd_t           ShellCmd;
    CFE_ES_StartAppCmd_t        StartAppCmd;
    CFE_ES_AppNameCmd_t         AppNameCmd;
    CFE_ES_AppReloadCmd_t       ReloadAppCmd;
    CFE_ES_QueryAllCmd_t        QueryAllCmd;
    CFE_ES_OverWriteSysLogCmd_t OverwriteSysLogCmd;
    CFE_ES_WriteSyslogCmd_t     WriteSyslogCmd;
    CFE_ES_WriteERlogCmd_t      WriteERlogCmd;
    CFE_ES_SetMaxPRCountCmd_t   SetMaxPRCountCmd;
    CFE_ES_DeleteCDSCmd_t       DeleteCDSCmd;
    CFE_ES_TlmPoolStatsCmd_t    TlmPoolStatsCmd;
    CFE_ES_DumpCDSRegCmd_t      DumpCDSRegCmd;
    CFE_ES_QueryAllTasksCmd_t   QueryAllTasksCmd;
    Pool_t                      UT_TestPool;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Task\n");
#endif

    /* Reset the log index; CFE_ES_TaskMain() calls CFE_ES_TaskInit() which
     * sets SystemLogMode to DISCARD, which can result in a log overflow
     * depending on the value that the index has reached from previous tests
     */
    CFE_ES_ResetDataPtr->SystemLogIndex = 0;

    /* Test task main process loop with a command pipe error */
    ES_ResetUnitTest();
    UT_RcvMsgId = 0xab;
    CFE_ES_TaskMain();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_COMMAND_PIPE &&
                 OSPrintRtn.count == -2,
              "CFE_ES_TaskMain",
              "Command pipe error");

    /* Test task main process loop with an initialization failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKREGISTER_FAIL);
    CFE_ES_TaskMain();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_APP_INIT + UT_OSP_REGISTER_APP &&
                 OSPrintRtn.count == -3,
              "CFE_ES_TaskMain",
              "Task initialization fail");

    /* Test task main process loop with bad checksum information */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPGetCFETextRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS &&
              CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum == 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum fail");

    /* Test successful task main process loop */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS &&
              CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum != 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum success");

    /* Test task main process loop with a register app failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKREGISTER_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_ES_ERR_APP_REGISTER,
             "CFE_ES_TaskInit",
              "Register application fail");

    /* Test task main process loop with a with an EVS register failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&EVS_RegisterRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -1,
              "CFE_ES_TaskInit",
              "EVS register fail");

    /* Test task main process loop with a SB pipe create failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&SB_CreatePipeRtn, -2, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -2,
              "CFE_ES_TaskInit",
              "SB pipe create fail");

    /* Test task main process loop with a HK packet subscribe failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&SB_SubscribeExRtn, -3, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -3,
              "CFE_ES_TaskInit",
              "HK packet subscribe fail");

    /* Test task main process loop with a ground command subscribe failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&SB_SubscribeExRtn, -4, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -4,
              "CFE_ES_TaskInit",
              "Ground command subscribe fail");

    /* Test task main process loop with an init event send failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&EVS_SendEventRtn, -5, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -5,
              "CFE_ES_TaskInit",
              "Initialization event send fail");

    /* Test task main process loop with version event send failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&EVS_SendEventRtn, -6, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -6,
              "CFE_ES_TaskInit",
              "Version event send fail");

    /* Set the log mode to OVERWRITE; CFE_ES_TaskInit() sets SystemLogMode to
     * DISCARD, which can result in a log overflow depending on the value that
     * the index reaches during subsequent tests
     */
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LOG_OVERWRITE;

    /* Test a successful HK request */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_SEND_HK_MID, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree > 0,
              "CFE_ES_HousekeepingCmd",
              "HK packet - get heap successful");

    /* Test the HK request with a get heap failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&HeapGetInfoRtn, -1, 1);
    UT_SendMsg(msgptr, CFE_ES_SEND_HK_MID, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree == 0,
              "CFE_ES_HousekeepingCmd",
              "HK packet - get heap fail");

    /* Test successful no-op command */
    ES_ResetUnitTest();
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_NOOP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_NOOP_INF_EID,
            "CFE_ES_NoopCmd",
            "No-op");

    /* Test successful reset counters command */
    ES_ResetUnitTest();
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESET_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESET_INF_EID,
              "CFE_ES_ResetCmd",
              "Reset counters");

    /* Test successful cFE restart */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_RestartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &RestartCmd;
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    RestartCmd.Payload.RestartType = CFE_PSP_RST_TYPE_PROCESSOR;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              PSPRestartRtn.value == CFE_PSP_RST_TYPE_PROCESSOR &&
              PSPRestartRtn.count == 1,
              "CFE_ES_RestartCmd",
              "Restart cFE");

    /* Test cFE restart with bad restart type */
    ES_ResetUnitTest();
    RestartCmd.Payload.RestartType = 4524;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_BOOT_ERR_EID,
              "CFE_ES_RestartCmd",
              "Invalid restart type");

    /* Test shell command failure */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_ShellCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &ShellCmd;
    strncpy((char *) ShellCmd.Payload.CmdString, "ES_NOAPP",
            sizeof(ShellCmd.Payload.CmdString));
    ShellCmd.Payload.OutputFilename[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SHELL_CMD_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SHELL_ERR_EID,
              "CFE_ES_ShellCmd",
              "Shell command fail");

    /* Test successful shell command */
    ES_ResetUnitTest();
    strncpy((char *) ShellCmd.Payload.CmdString, "ls",
            sizeof(ShellCmd.Payload.CmdString));
    ShellCmd.Payload.OutputFilename[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SHELL_CMD_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SHELL_INF_EID,
              "CFE_ES_ShellCmd",
              "Shell command success");

    /* Test successful app create */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_StartAppCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &StartAppCmd;
    strncpy((char *) StartAppCmd.Payload.AppFileName, "filename",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "appNameIntentionallyTooLongToFitIntoDestinationBuffer",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 160;
    StartAppCmd.Payload.StackSize = 8192;
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_INF_EID,
              "CFE_ES_StartAppCmd",
              "Start application from file name");

    /* Test app create with an OS task create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Start application from file name fail");

    /* Test app create with the file name too short */
    ES_ResetUnitTest();
    strncpy((char *) StartAppCmd.Payload.AppFileName, "123",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "appName",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 160;
    StartAppCmd.Payload.StackSize = 12096;
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_INVALID_FILENAME_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Invalid file name");

    /* Test app create with a null application entry point */
    ES_ResetUnitTest();
    strncpy((char *) StartAppCmd.Payload.AppFileName, "filename",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "appName",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 160;
    StartAppCmd.Payload.StackSize = 12096;
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value ==
                CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Application entry point null");

    /* Test app create with a null application name */
    ES_ResetUnitTest();
    strncpy((char *) StartAppCmd.Payload.AppFileName, "filename",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 160;
    StartAppCmd.Payload.StackSize = 12096;
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_NULL_APP_NAME_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Application name null");

    /* Test app create with with an invalid exception action */
    ES_ResetUnitTest();
    strncpy((char *) StartAppCmd.Payload.AppFileName, "filename",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "appName",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 160;
    StartAppCmd.Payload.StackSize = 12096;
    StartAppCmd.Payload.ExceptionAction = 65534;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_EXC_ACTION_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Invalid exception action");

    /* Test app create with a bad stack size */
    ES_ResetUnitTest();
    strncpy((char *) StartAppCmd.Payload.AppFileName, "filename",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "appName",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 160;
    StartAppCmd.Payload.StackSize = 0;
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_STACK_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Stack size too small");

    /* Test app create with a bad priority */
    ES_ResetUnitTest();
    strncpy((char *) StartAppCmd.Payload.AppFileName, "filename",
            sizeof(StartAppCmd.Payload.AppFileName));
    strncpy((char *) StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) StartAppCmd.Payload.Application, "appName",
            sizeof(StartAppCmd.Payload.Application));
    StartAppCmd.Payload.Priority = 1000;
    StartAppCmd.Payload.StackSize = 12096;
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_PRIORITY_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Priority is too large");

    /* Test successful app stop */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppNameCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.Name, "CFE_ES", 
        sizeof(CFE_ES_Global.AppTable[Id].StartParams.Name));
    CFE_ES_Global.AppTable[Id].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    strncpy((char *) AppNameCmd.Payload.Application, "CFE_ES",
            sizeof(AppNameCmd.Payload.Application));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_STOP_DBG_EID,
              "CFE_ES_StopAppCmd",
              "Stop application initiated");

    /* Test app stop failure */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_STOP_ERR1_EID,
              "CFE_ES_StopAppCmd",
              "Stop application failed");

    /* Test app stop with a bad app name */
    ES_ResetUnitTest();
    strncpy((char *) AppNameCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(AppNameCmd.Payload.Application));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_STOP_ERR2_EID,
              "CFE_ES_StopAppCmd",
              "Stop application bad name");

    /* Test successful app restart */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppNameCmd_t));
    strncpy((char *) AppNameCmd.Payload.Application, "CFE_ES",
            sizeof(AppNameCmd.Payload.Application));
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name,
            "CFE_ES", OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_DBG_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application initiated");

    /* Test app restart with a bad app name */
    ES_ResetUnitTest();
    strncpy((char *) AppNameCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(AppNameCmd.Payload.Application));
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR2_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application bad name");

    /* Test failed app restart */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    strncpy((char *) AppNameCmd.Payload.Application, "CFE_ES", 
        sizeof(AppNameCmd.Payload.Application));
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.Name,
            "CFE_ES", OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR1_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application failed");

    /* Test successful app reload */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    msgptr = (CFE_SB_MsgPtr_t) &ReloadAppCmd;
    strncpy((char *) ReloadAppCmd.Payload.AppFileName, "New_Name",
            sizeof(ReloadAppCmd.Payload.AppFileName));
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppReloadCmd_t));
    strncpy((char *) ReloadAppCmd.Payload.Application, "CFE_ES",
            sizeof(ReloadAppCmd.Payload.Application));
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_DBG_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application initiated");

    /* Test app reload with a bad app name */
    ES_ResetUnitTest();
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    strncpy((char *) ReloadAppCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(ReloadAppCmd.Payload.Application));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR2_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application bad name");
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;

    /* Test failed app reload */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    strncpy((char *) ReloadAppCmd.Payload.Application, "CFE_ES",
            sizeof(ReloadAppCmd.Payload.Application));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR1_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application failed");

    /* Test successful telemetry packet request for single app data */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppNameCmd_t));
    strncpy((char *) AppNameCmd.Payload.Application, "CFE_ES",
            sizeof(AppNameCmd.Payload.Application));
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ONE_APP_EID,
              "CFE_ES_QueryOneCmd",
              "Query application - success");

    /* Test telemetry packet request for single app data with failure of
     * CFE_SB_SendMsg; SBSendMsgRtn count must be set to 2 in order to
     * allow the command message through, but then fail sending the
     * application status telemetry packet message
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_SetRtnCode(&SBSendMsgRtn, -1, 2);
    strncpy((char *) CFE_ES_Global.AppTable[Id].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[Id].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) AppNameCmd.Payload.Application, "CFE_ES",
            sizeof(AppNameCmd.Payload.Application));
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ONE_ERR_EID,
              "CFE_ES_QueryOneCmd",
              "Query application - SB send message fail");

    /* Test telemetry packet request for single app data with a bad app name */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    strncpy((char *) AppNameCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(AppNameCmd.Payload.Application));
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ONE_APPID_ERR_EID,
              "CFE_ES_QueryOneCmd",
              "Query application - bad application name");

    /* Test successful write of all app data to file */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_QueryAllCmd_t));
    strncpy((char *) QueryAllCmd.Payload.QueryAllFileName, "AllFilename",
            sizeof(QueryAllCmd.Payload.QueryAllFileName));
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ALL_APPS_EID,
              "CFE_ES_QueryAllCmd",
              "Query all applications - success");

    /* Test write of all app data to file with a null file name */
    ES_ResetUnitTest();
    QueryAllCmd.Payload.QueryAllFileName[0] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ALL_APPS_EID,
              "CFE_ES_QueryAllCmd",
              "Query all applications - null file name");

    /* Test write of all app data to file with a write header failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, OS_FS_ERROR, 1);
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_WRHDR_ERR_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; write header");

    /* Test write of all app data to file with a file write failure */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_SetOSFail(OS_WRITE_FAIL);
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TASKWR_ERR_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; task write");

    /* Test write of all app data to file with a file create failure */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_SetOSFail(OS_CREAT_FAIL);
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_OSCREATE_ERR_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; OS create");

    /* Test successful write of all task data to a file */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    QueryAllTasksCmd.Payload.QueryAllFileName[0] = '\0';
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllTasksCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TASKINFO_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file written");
    strncpy((char *) QueryAllTasksCmd.Payload.QueryAllFileName, "filename",
            sizeof(QueryAllTasksCmd.Payload.QueryAllFileName));

    /* Test write of all task data to a file with write header failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllTasksCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TASKINFO_WRHDR_ERR_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; write header");

    /* Test write of all task data to a file with a task write failure */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_SetOSFail(OS_WRITE_FAIL);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllTasksCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TASKINFO_WR_ERR_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; task write");

    /* Test write of all task data to a file with an OS create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllTasksCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TASKINFO_OSCREATE_ERR_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; OS create");

    /* Test successful clearing of the system log */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SYSLOG1_INF_EID,
              "CFE_ES_ClearSyslogCmd",
              "Clear ES log data");

    /* Test successful overwriting of the system log using discard mode */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_OverWriteSysLogCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &OverwriteSysLogCmd;
    OverwriteSysLogCmd.Payload.Mode = CFE_ES_LOG_DISCARD;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_OVERWRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SYSLOGMODE_EID,
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log received (discard mode)");

    /* Test overwriting the system log using an invalid mode */
    ES_ResetUnitTest();
    OverwriteSysLogCmd.Payload.Mode = 9342;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_OVERWRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ERR_SYSLOGMODE_EID,
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log using invalid mode");

    /* Test successful writing of the system log */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_WriteSyslogCmd_t));
    strncpy(WriteSyslogCmd.Payload.SysLogFileName, "filename",
            sizeof(WriteSyslogCmd.Payload.SysLogFileName));
    msgptr = (CFE_SB_MsgPtr_t) &WriteSyslogCmd;
    CFE_ES_TaskData.HkPacket.Payload.SysLogEntries = 123;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SYSLOG2_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; success");

    /* Test writing the system log using a null file name */
    ES_ResetUnitTest();
    WriteSyslogCmd.Payload.SysLogFileName[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SYSLOG2_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; null file name");

    /* Test writing the system log with an OS create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    strncpy((char *) WriteSyslogCmd.Payload.SysLogFileName, "",
            sizeof(WriteSyslogCmd.Payload.SysLogFileName));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SYSLOG2_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; OS create");

    /* Test writing the system log with an OS write failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_WRITE_FAIL);
    strncpy((char *) WriteSyslogCmd.Payload.SysLogFileName, "",
            sizeof(WriteSyslogCmd.Payload.SysLogFileName));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; OS write");

    /* Test writing the system log with a write header failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, OS_FS_ERROR, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; write header");

    /* Test successful clearing of the E&R log */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;

    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ERLOG1_INF_EID,
              "CFE_ES_ClearERlogCmd",
              "Clear E&R log");

    /* Test successful writing of the E&R log */
    ES_ResetUnitTest();
    strncpy(WriteERlogCmd.Payload.ERLogFileName, "filename",
            sizeof(WriteERlogCmd.Payload.ERLogFileName));
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_WriteERlogCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &WriteERlogCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ERLOG2_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; success");

    /* Test writing the E&R log with an OS create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    strncpy((char *) WriteERlogCmd.Payload.ERLogFileName, "",
            sizeof(WriteERlogCmd.Payload.ERLogFileName));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ERLOG2_ERR_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; OS create");

    /* Test writing the E&R log with an OS write failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_WRITE_FAIL);
    strncpy((char *) WriteERlogCmd.Payload.ERLogFileName, "n",
            sizeof(WriteERlogCmd.Payload.ERLogFileName));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; OS write");

    /* Test writing the E&R log with a write header failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, OS_FS_ERROR, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; write header");

    /* Test writing the E&R log with a reset area failure */
    ES_ResetUnitTest();
    UT_SetStatusBSPResetArea(OS_ERROR, 0, CFE_TIME_TONE_PRI);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RST_ACCESS_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; reset area");

    /* Test clearing the log with a bad size in the verify command
     * length call
     */
    ES_ResetUnitTest();
    UT_SetStatusBSPResetArea(OS_SUCCESS, 0, CFE_TIME_TONE_PRI);
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_SetMaxPRCountCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ClearERlogCmd",
              "Packet length error");

    /* Test resetting and setting the max for the processor reset count */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESET_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_RESET_PR_COUNT_EID,
              "CFE_ES_ResetPRCountCmd",
              "Set processor reset count to zero");

    /* Test setting the maximum processor reset count */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_SetMaxPRCountCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &SetMaxPRCountCmd;
    SetMaxPRCountCmd.Payload.MaxPRCount = 3;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SET_MAX_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SET_MAX_PR_COUNT_EID,
              "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count");

    /* Test failed deletion of specified CDS */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_DeleteCDSCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &DeleteCDSCmd;
    strncpy(DeleteCDSCmd.Payload.CdsName, "CFE_ES.CDS_NAME",
            sizeof(DeleteCDSCmd.Payload.CdsName));
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "CFE_ES.CDS_NAME", OS_MAX_API_NAME);

    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_DELETE_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; error");

    /* Test failed deletion of specified critical table CDS */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.Registry[0].Table = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_DELETE_TBL_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; wrong type");
    CFE_ES_Global.CDSVars.Registry[0].Table = FALSE;

    /* Test successful deletion of a specified CDS */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.Registry[0].MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 1);

    /* Set up the block to read what we need to from the CDS */
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_CDSBlockDesc.SizeUsed =  512;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_DELETED_INFO_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; success");

    /* Test deletion of a specified CDS with the owning app being active */
    ES_ResetUnitTest();
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "CFE_ES.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 1);
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].Table = FALSE;
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_OWNER_ACTIVE_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; owner active");

    /* Test deletion of a specified CDS with the name not found */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.Registry[0].MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 1);
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_BAD",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    CFE_ES_Global.CDSVars.Registry[0].Taken = FALSE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_NAME_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; not found");

    /* Test successful dump of CDS to file using the default dump file name */
    ES_ResetUnitTest();
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_DumpCDSRegCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &DumpCDSRegCmd;
    DumpCDSRegCmd.Payload.DumpFilename[0] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_REG_DUMP_INF_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; success (default dump file)");

    /* Test dumping of the CDS to a file with a bad FS write header */
    ES_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_WRITE_CFE_HDR_ERR_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; write header");

    /* Test dumping of the CDS to a file with an OS create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CREATING_CDS_DUMP_ERR_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; OS create");

    /* Test dumping of the CDS to a file with an OS write failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_WRITE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_DUMP_ERR_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; OS write");

    /* Test telemetry pool statistics retrieval with an invalid handle */
    ES_ResetUnitTest();
    memset(&UT_TestPool, 0, sizeof(UT_TestPool));
    memset(&TlmPoolStatsCmd, 0, sizeof(TlmPoolStatsCmd));
    CFE_SB_SET_MEMADDR(TlmPoolStatsCmd.Payload.PoolHandle, &UT_TestPool);
    UT_TestPool.Start = (cpuaddr *)&UT_TestPool;
    UT_TestPool.Size = 64;
    UT_TestPool.End = (cpuaddr)UT_TestPool.Start;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_TlmPoolStatsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &TlmPoolStatsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_TLM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_INVALID_POOL_HANDLE_ERR_EID,
              "CFE_ES_TlmPoolStatsCmd",
              "Telemetry pool; bad handle");

    /* Test successful telemetry pool statistics retrieval */
    ES_ResetUnitTest();
    memset(&UT_TestPool, 0, sizeof(UT_TestPool));
    UT_TestPool.Start = (cpuaddr *)&UT_TestPool;
    UT_TestPool.Size = 64;
    UT_TestPool.End = ((cpuaddr)UT_TestPool.Start) + UT_TestPool.Size;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_TLM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TLM_POOL_STATS_INFO_EID,
              "CFE_ES_TlmPoolStatsCmd",
              "Telemetry pool; success");

    /* Test the command pipe message process with an invalid command */
    ES_ResetUnitTest();
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC + 2);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CC1_ERR_EID,
              "CFE_ES_TaskPipe",
              "Invalid ground command");

    /* Test locking the CDS registry with a mutex take failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&MutSemTakeRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_LockCDSRegistry() == OS_ERROR,
              "CFE_ES_LockCDSRegistry",
              "Mutex take failed");

    /* Test unlocking the CDS registry with a mutex give failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&MutSemGiveRtn, OS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_UnlockCDSRegistry() == OS_ERROR,
              "CFE_ES_UnlockCDSRegistry",
              "Mutex give failed");

    /* Test sending a no-op command with an invalid command length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_NOOP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_NoopCmd",
              "No-op; invalid command length");

    /* Test sending a reset counters command with an invalid command length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESET_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ResetCmd",
              "Reset counters; invalid command length");

    /* Test sending a cFE restart command with an invalid command length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_RestartCmd",
              "Restart cFE; invalid command length");

    /* Test cFE restart with a power on reset */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_RestartCmd_t));
    RestartCmd.Payload.RestartType = CFE_PSP_RST_TYPE_POWERON;
    msgptr = (CFE_SB_MsgPtr_t) &RestartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value != CFE_ES_BOOT_ERR_EID,
              "CFE_ES_RestartCmd",
              "Power on reset restart type");

    /* Test sending a shell command with an invalid command length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SHELL_CMD_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ShellCmd",
              "Shell command; invalid command length");

    /* Test sending a start application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Start application command; invalid command length");

    /* Test start application command with a processor restart on application
     * exception
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_StartAppCmd_t));
    StartAppCmd.Payload.ExceptionAction = CFE_ES_APP_EXCEPTION_PROC_RESTART;
    StartAppCmd.Payload.Priority = 160;
    msgptr = (CFE_SB_MsgPtr_t) &StartAppCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_START_INF_EID,
              "CFE_ES_StartAppCmd",
              "Processor restart on application exception");

    /* Test sending a stop application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_StopAppCmd",
              "Stop application command; invalid command length");

    /* Test sending a restart application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application command; invalid command length");

    /* Test sending a reload application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application command; invalid command length");

    /* Test sending a write request for a single application with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_QueryOneAppCmd",
              "Query one application command; invalid command length");

    /* Test sending a write request for all applications with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_QueryAllAppCmd",
              "Query all applications command; invalid command length");

    /* Test write of all app data to file with a file open failure */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_QueryAllCmd_t));
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_ALL_APPS_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; file open");

    /* Test sending a write request for all tasks with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_QueryAllAppCmd",
              "Query all tasks command; invalid command length");

    /* Test write of all task data to file with a file open failure */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_QueryAllTasksCmd_t));
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_TASKINFO_EID,
              "CFE_ES_QueryAllCmd",
              "Write task information file fail; file open");

    /* Test sending a request to clear the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ClearSyslogCmd",
              "Clear system log command; invalid command length");

    /* Test sending a request to overwrite the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_OVERWRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_OverwriteSyslogCmd",
              "Overwrite system log command; invalid command length");

    /* Test sending a request to write the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log command; invalid command length");

    /* Test successful overwriting of the system log using overwrite mode */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_OverWriteSysLogCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &OverwriteSysLogCmd;
    OverwriteSysLogCmd.Payload.Mode = CFE_ES_LOG_OVERWRITE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_OVERWRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_SYSLOGMODE_EID,
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log received (overwrite mode)");

    /* Test sending a request to write the error log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_WriteErrlogCmd",
              "Write error log command; invalid command length");

    /* Test sending a request to reset the processor reset count with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESET_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ResetPRCountCmd",
              "Reset processor reset count command; invalid command length");

    /* Test sending a request to set the maximum processor reset count with
     * an invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SET_MAX_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count command; invalid "
                "command length");

    /* Test sending a request to delete the CDS with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete CDS command; invalid command length");

    /* Test sending a telemetry pool statistics retrieval command with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(0);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_TLM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Telemetry pool command; invalid command length");

    /* Test successful dump of CDS to file using a specified dump file name */
    ES_ResetUnitTest();
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_DumpCDSRegCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &DumpCDSRegCmd;
    strncpy(DumpCDSRegCmd.Payload.DumpFilename, "DumpFile", sizeof(DumpCDSRegCmd.Payload.DumpFilename));
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_CDS_REG_DUMP_INF_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; success (dump file specified)");
} /* end TestTask */

void TestPerf(void)
{
    uint32 Id;
    uint32 TestObjId;
    CFE_SB_MsgPtr_t msgptr;
    CFE_ES_PerfStartCmd_t PerfStartCmd;
    CFE_ES_PerfStopCmd_t PerfStopCmd;
    CFE_ES_PerfSetFilterMaskCmd_t PerfSetFilterMaskCmd;
    CFE_ES_PerfSetTrigMaskCmd_t PerfSetTrigMaskCmd;

    extern CFE_ES_PerfLogDump_t CFE_ES_PerfLogDumpStatus;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Performance Log\n");
#endif

    /* Test successful performance mask and value initialization */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    Perf->MetaData.State = CFE_ES_PERF_MAX_STATES;
    CFE_ES_Global.TaskTable[Id].AppId = 0;
    CFE_ES_SetupPerfVariables(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_SetupPerfVariables",
              "Idle data collection");

    /* Test successful performance data collection start in START
     * trigger mode
     */
    ES_ResetUnitTest();
    PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Collect performance data; mode START");

    /* Test successful performance data collection start in CENTER
     * trigger mode
     */
    ES_ResetUnitTest();
    PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_CENTER;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Collect performance data; mode CENTER");

    /* Test successful performance data collection start in END
     * trigger mode
     */
    ES_ResetUnitTest();
    PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_END;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Collect performance data; mode END");

    /* Test performance data collection start with an invalid trigger mode
     * (too high)
     */
    ES_ResetUnitTest();
    PerfStartCmd.Payload.TriggerMode = (CFE_ES_PERF_TRIGGER_END + 1);
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_TRIG_ERR_EID,
              "CFE_ES_PerfStartDataCmd",
              "Trigger mode out of range (high)");

    /* Test performance data collection start with an invalid trigger mode
     * (too low)
     */
    ES_ResetUnitTest();
    PerfStartCmd.Payload.TriggerMode = 0xffffffff;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_TRIG_ERR_EID,
              "CFE_ES_PerfStartDataCmd",
              "Trigger mode out of range (low)");

    /* Test performance data collection start with a file write in progress */
    ES_ResetUnitTest();
    CFE_ES_PerfLogDumpStatus.DataToWrite = 1;
    PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_ERR_EID,
              "CFE_ES_PerfStartDataCmd",
              "Cannot collect performance data; write in progress");
    CFE_ES_PerfLogDumpStatus.DataToWrite = 0;

    /* Test performance data collection by sending another valid
     * start command
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Start collecting performance data");

    /* Test successful performance data collection stop */
    PerfStopCmd.Payload.DataFileName[0] = '\0';
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStopCmd_t));
    CFE_ES_PerfLogDumpStatus.DataToWrite = 0;
    msgptr = (CFE_SB_MsgPtr_t) &PerfStopCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop collecting performance data");

    /* Test performance data collection stop with an OS create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_LOG_ERR_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop performance data command; OS create fail");

    /* Test performance data collection stop with an OS task create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_ERR1_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop performance data command; OS task create fail");

    /* Test successful performance data collection stop with a non-default
         file name */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    strncpy(PerfStopCmd.Payload.DataFileName, "filename", 
        sizeof(PerfStopCmd.Payload.DataFileName));
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop collecting performance data (non-default file name)");

    /* Test performance data collection stop with a file write in progress */
    ES_ResetUnitTest();
    CFE_ES_PerfLogDumpStatus.DataToWrite = 1;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_ERR2_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop performance data command ignored");

    /* Test performance filter mask command with out of range filter
         mask value */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfSetFilterMaskCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfSetFilterMaskCmd;
    PerfSetFilterMaskCmd.Payload.FilterMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETFILTERMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_FILTMSKERR_EID,
              "CFE_ES_PerfSetFilterMaskCmd",
              "Performance filter mask command error; index out of range");

    /* Test successful performance filter mask command */
    ES_ResetUnitTest();
    PerfSetFilterMaskCmd.Payload.FilterMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK / 2;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETFILTERMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_FILTMSKCMD_EID,
              "CFE_ES_PerfSetFilterMaskCmd",
              "Set performance filter mask command received");

    /* Test successful performance filter mask command with minimum filter
         mask value */
    ES_ResetUnitTest();
    PerfSetTrigMaskCmd.Payload.TriggerMaskNum = 0;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfSetTrigMaskCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfSetTrigMaskCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_TRIGMSKCMD_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Set performance trigger mask command received; minimum index");

    /* Test successful performance filter mask command with maximum filter
     * mask value
     */
    ES_ResetUnitTest();
    PerfSetTrigMaskCmd.Payload.TriggerMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK - 1;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_TRIGMSKCMD_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Set performance trigger mask command received; maximum index");

    /* Test successful performance filter mask command with a greater than the
     * maximum filter mask value
     */
    ES_ResetUnitTest();
    PerfSetTrigMaskCmd.Payload.TriggerMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK + 1;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_TRIGMSKERR_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Performance trigger mask command error; index out of range");

    /* Test successful performance log dump */
    ES_ResetUnitTest();
    CFE_ES_PerfLogDumpStatus.DataFileDescriptor = OS_open(NULL, 0, 0);
    CFE_ES_PerfLogDump();
    UT_GetStubRetcodeAndCount(UT_KEY(OS_close), &OSCloseRtn.value,
                              &OSCloseRtn.count);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_PERF_DATAWRITTEN_EID &&
                  OSCloseRtn.value == OS_FS_SUCCESS && OSCloseRtn.count == 1,
              "CFE_ES_PerfLogDump",
              "Performance log dump; success");

    /* Test performance log dump with a cFE header write failure */
    ES_ResetUnitTest();
    CFE_ES_PerfLogDumpStatus.DataFileDescriptor = OS_open(NULL, 0, 0);
    Perf->MetaData.DataCount = 7;
    UT_SetRtnCode(&FileWriteRtn, sizeof(CFE_ES_PerfDataEntry_t), 4);
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    CFE_ES_PerfLogDump();
    UT_GetStubRetcodeAndCount(UT_KEY(OS_close), &OSCloseRtn.value,
                              &OSCloseRtn.count);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID &&
                  OSCloseRtn.value == OS_FS_SUCCESS && OSCloseRtn.count == 1,
              "CFE_ES_PerfLogDump",
              "Performance log dump; cFE header write failed");

    /* Test performance log dump with a metadata write failure */
    ES_ResetUnitTest();
    CFE_ES_PerfLogDumpStatus.DataFileDescriptor = OS_open(NULL, 0, 0);
    Perf->MetaData.DataCount = 7;
    UT_SetOSFail(OS_CREAT_FAIL | OS_WRITE_FAIL);
    CFE_ES_PerfLogDump();
    UT_GetStubRetcodeAndCount(UT_KEY(OS_close), &OSCloseRtn.value,
                              &OSCloseRtn.count);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID &&
                  OSCloseRtn.value == OS_FS_SUCCESS && OSCloseRtn.count == 1,
              "CFE_ES_PerfLogDump",
              "Performance log dump; metadata write failed");

    /* Test performance log dump with a data write failure */
    ES_ResetUnitTest();
    CFE_ES_PerfLogDumpStatus.DataFileDescriptor = OS_open(NULL, 0, 0);
    Perf->MetaData.DataCount = 7;
    UT_SetRtnCode(&FileWriteRtn, sizeof(CFE_ES_PerfDataEntry_t) + 1, 4);
    CFE_ES_PerfLogDump();
    UT_GetStubRetcodeAndCount(UT_KEY(OS_close), &OSCloseRtn.value,
                              &OSCloseRtn.count);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID &&
                  OSCloseRtn.value == OS_FS_SUCCESS && OSCloseRtn.count == 1,
              "CFE_ES_PerfLogDump",
              "Performance log dump; data write failed");

    /* Test successful addition of a new entry to the performance log */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerCount = CFE_ES_PERF_DATA_BUFFER_SIZE +1;
    Perf->MetaData.InvalidMarkerReported = FALSE;
    Perf->MetaData.DataEnd = CFE_ES_PERF_DATA_BUFFER_SIZE +1 ;
    CFE_ES_PerfLogAdd(CFE_ES_PERF_MAX_IDS, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.InvalidMarkerReported == TRUE,
              "CFE_ES_PerfLogAdd",
              "Invalid performance marker");

    /* Test addition of a new entry to the performance log with START
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.InvalidMarkerReported = TRUE;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.DataCount = CFE_ES_PERF_DATA_BUFFER_SIZE + 1;
    Perf->MetaData.TriggerMask[0] = 0xFFFF;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_START &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; START");

    /* Test addition of a new entry to the performance log with CENTER
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_CENTER;
    Perf->MetaData.TriggerCount = CFE_ES_PERF_DATA_BUFFER_SIZE / 2 + 1 ;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_CENTER &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; CENTER");

    /* Test addition of a new entry to the performance log with END
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_END;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_END &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; END");

    /* Test addition of a new entry to the performance log with an invalid
     * marker after an invalid marker has already been reported
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.InvalidMarkerReported = 2;
    CFE_ES_PerfLogAdd(CFE_ES_PERF_MAX_IDS + 1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.InvalidMarkerReported = 2,
              "CFE_ES_PerfLogAdd",
              "Invalid marker after previous invalid marker");

    /* Test addition of a new entry to the performance log with a marker that
     * is not in the filter mask
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.FilterMask[0] = 0x0;
    Perf->MetaData.DataEnd = 0;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.DataEnd == 0,
              "CFE_ES_PerfLogAdd",
              "Marker not in filter mask");

    /* Test addition of a new entry to the performance log with the data count
     * below the maximum allowed
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.DataCount = 0;
    Perf->MetaData.FilterMask[0] = 0xffff;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.DataCount == 1,
              "CFE_ES_PerfLogAdd",
              "Data count below maximum");

    /* Test addition of a new entry to the performance log with a marker that
     * is not in the trigger mask
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerMask[0] = 0x0;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State != CFE_ES_PERF_TRIGGERED,
              "CFE_ES_PerfLogAdd",
              "Marker not in trigger mask");

    /* Test addition of a new entry to the performance log with a start
     * trigger mode and the trigger count is less the buffer size
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = 0;
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.TriggerMask[0] = 0xffff;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.TriggerCount == 1,
              "CFE_ES_PerfLogAdd",
              "Start trigger mode; trigger count less than the buffer size");

    /* Test addition of a new entry to the performance log with a center
     * trigger mode and the trigger count is less than half the buffer size
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = CFE_ES_PERF_DATA_BUFFER_SIZE / 2 - 2;
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_CENTER;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State != CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Center trigger mode; trigger count less than half the "
                "buffer size");

    /* Test addition of a new entry to the performance log with an invalid
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = 0;
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = -1;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State != CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Invalid trigger mode");

    /* Test performance data collection start with an invalid message length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t) + 1);
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value != CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Invalid message length");

    /* Test performance data collection stop with an invalid message length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStopCmd_t) + 1);
    msgptr = (CFE_SB_MsgPtr_t) &PerfStopCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value != CFE_ES_PERF_STOPCMD_EID,
              "CFE_ES_PerfStopDataCmd",
              "Invalid message length");

    /* Test performance data filer mask with an invalid message length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfSetFilterMaskCmd_t) + 1);
    msgptr = (CFE_SB_MsgPtr_t) &PerfSetFilterMaskCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETFILTERMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value != CFE_ES_PERF_FILTMSKCMD_EID,
              "CFE_ES_PerfSetFilterMaskCmd",
              "Invalid message length");

    /* Test performance data trigger mask with an invalid message length */
    ES_ResetUnitTest();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfSetTrigMaskCmd_t) + 1);
    msgptr = (CFE_SB_MsgPtr_t) &PerfSetFilterMaskCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value != CFE_ES_PERF_TRIGMSKCMD_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Invalid message length");
}

void TestAPI(void)
{
    uint32 Id, Id2;
    uint32 TestObjId, TestObjId2;
    char AppName[32];
    char CounterName[11];
    char CDSName[CFE_ES_CDS_MAX_NAME_LENGTH + 2];
    int i;
    uint32 ExceptionContext = 0;
    int32 Return;
    uint8 Data[12];
    uint32 ResetType;
    uint32 *ResetTypePtr;
    uint32 AppId;
    uint32 TaskId;
    uint32 TempSize;
    uint32 RunStatus;
    uint32 CounterId;
    uint32 CounterCount;
    CFE_ES_CDSHandle_t CDSHandle;
    CFE_ES_TaskInfo_t TaskInfo;

#ifdef UT_VERBOSE
    UT_Text("Begin Test API\n");
#endif

    /* Test resetting the cFE with a processor reset */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount - 1;
    CFE_ES_ResetCFE(ResetType);
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
                  OSPrintRtn.value == UT_OSP_POR_MAX_PROC_RESETS +
                  UT_OSP_PROC_RESET_COMMANDED && OSPrintRtn.count == -3,
              "CFE_ES_ResetCFE",
              "Processor reset");

    /* Test getting the reset type using a valid pointer and a null pointer */
    ES_ResetUnitTest();
    Return = CFE_ES_GetResetType(&ResetType);
    ResetTypePtr = NULL;
    CFE_ES_GetResetType(ResetTypePtr);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_PSP_RST_TYPE_PROCESSOR &&
              ResetTypePtr == NULL,
              "CFE_ES_GetResetType",
              "Get reset type successful");

    /* Test resetting the cFE with a power on reset */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_POWERON;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
                  OSPrintRtn.value == UT_OSP_POR_COMMANDED &&
                  OSPrintRtn.count == -2,
              "CFE_ES_ResetCFE",
              "Power on reset");

    /* Test resetting the cFE with an invalid argument */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_POWERON + 3;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ResetCFE",
              "Bad reset type");

    /* Test restarting an app that doesn't exist */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RestartApp(Id) == CFE_ES_ERR_APPID,
              "CFE_ES_RestartApp",
              "Bad application ID");

    /* Test restarting an app with an ID out of range (high) */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RestartApp(CFE_ES_MAX_APPLICATIONS) == CFE_ES_ERR_APPID,
              "CFE_ES_RestartApp",
              "Application ID too large");

    /* Test reloading an app that doesn't exist */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ReloadApp(Id, "filename") == CFE_ES_ERR_APPID,
              "CFE_ES_ReloadApp",
              "Bad application ID");

    /* Test deleting an app that doesn't exist */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteApp(Id) == CFE_ES_ERR_APPID,
              "CFE_ES_DeleteApp",
              "Bad application ID");

    /* Test exiting an app with an init error */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CORE_INIT +
                  UT_OSP_POR_MAX_PROC_RESETS && OSPrintRtn.count == -3,
              "CFE_ES_ExitApp",
              "Application initialization error");

    /* Test exiting an app with a runtime error */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CORE_RUNTIME &&
                  OSPrintRtn.count == -2,
              "CFE_ES_ExitApp",
              "Application runtime error");

    /* Test exiting an app with an exit error */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_ExitApp(1000);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CORE_APP_EXIT &&
                  OSPrintRtn.count == -2,
              "CFE_ES_ExitApp",
              "Application exit error");

#if 0
    /* Can't cover this path since it contains a while(1) (i.e.,
     * infinite) loop
     */
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR);
#endif

    /* Test successful run loop app run request */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    RunStatus = CFE_ES_RUNSTATUS_APP_RUN;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_APP_RUN;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == TRUE,
              "CFE_ES_RunLoop",
              "Request to run application");

    /* Test successful run loop app stop request */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    RunStatus = CFE_ES_RUNSTATUS_APP_RUN;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Request to stop running application");

    /* Test successful run loop app exit request */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    RunStatus = CFE_ES_RUNSTATUS_APP_EXIT;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Request to exit application");

    /* Test run loop with bad app ID */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    RunStatus = CFE_ES_RUNSTATUS_APP_EXIT;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Bad internal application ID");

    /* Test run loop with an invalid run status */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    RunStatus = 1000;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Invalid run status");

    /* Test run loop with startup sync code */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    RunStatus = CFE_ES_RUNSTATUS_APP_RUN;
    CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest =
        CFE_ES_RUNSTATUS_APP_RUN;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState =
        CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_Global.AppStartedCount = 1;
    CFE_ES_Global.AppReadyCount = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == TRUE &&
              CFE_ES_Global.AppReadyCount == 1,
              "CFE_ES_RunLoop",
              "Status change from initializing to run");

    /* Test successful CFE application registration */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterApp() == CFE_SUCCESS,
              "CFE_ES_RegisterApp",
              "Application registration successful");

    /* Test getting the cFE application ID by its name */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppID(&AppId) == CFE_SUCCESS,
              "CFE_ES_GetAppID",
              "Get application ID by name successful");

    /* Test getting the app name with a bad app ID */
    CFE_ES_Global.AppTable[4].RecordUsed = FALSE;
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppName(AppName, 4, 32) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppName",
              "Get application name by ID; bad application ID");

    /* Test getting the app name with that app ID out of range */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppName(AppName,
                                CFE_ES_MAX_APPLICATIONS + 2,
                                32) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppName",
              "Get application name by ID; ID out of range");

    /* Test successfully getting the app name using the app ID */
    ES_ResetUnitTest();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppName(AppName, 0, 32) == CFE_SUCCESS,
              "CFE_ES_GetAppName",
              "Get application name by ID successful");

    /* Test getting task information using the task ID */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, TestObjId) == CFE_SUCCESS,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID successful");

    /* Test getting task information using the task ID with parent inactive */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, Id) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; parent application not active");

    /* Test getting task information using the task ID with task inactive */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, Id) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; task not active");

    /* Test getting task information using the task ID with invalid task ID */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, 1000) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; invalid task ID");

    /* Test creating a child task with a bad app ID */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APPID,
              "CFE_ES_ChildTaskCreate",
              "Bad application ID");

    /* Test creating a child task with an OS task create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_CHILD_TASK_CREATE,
              "CFE_ES_ChildTaskCreate",
              "OS task create failed");

    /* Test creating a child task with a null task ID */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(NULL,
                                    "TaskName",
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task ID null");

    /* Test creating a child task with a null task name */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    NULL,
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task name null");

    /* Test creating a child task with a null task ID and name */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(NULL,
                                    NULL,
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task name and ID null");

    /* Test creating a child task with a null function pointer */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    NULL,
                                    (uint32*) AppName,
                                    32,
                                    2,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Function pointer null");

    /* Test creating a child task within a child task */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = 3;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_CHILD_TASK_CREATE,
              "CFE_ES_CreateChildTask",
              "Cannot call from a child task");

    /* Test successfully creating a child task */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS, "CFE_ES_CreateChildTask",
              "Create child task successful");

    /* Test deleting a child task with an invalid task ID */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(Id) == CFE_ES_ERR_TASKID,
              "CFE_ES_DeleteChildTask",
              "Task ID not in use");

    /* Test deleting a child task using a main task's ID */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(Id /*CFE_ES_Global.TaskTable[Id].TaskId*/) ==
                  CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK,
              "CFE_ES_DeleteChildTask",
              "Task ID belongs to a main task");

    /* Test successfully deleting a child task */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 0;
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId = 15;
    OS_TaskCreate(&CFE_ES_Global.TaskTable[1].TaskId, NULL, NULL, NULL,
                  0, 0, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(CFE_ES_Global.TaskTable[1].TaskId) ==
                CFE_SUCCESS,
              "CFE_ES_DeleteChildTask",
              "Delete child task successful");

    /* Test deleting a child task with an OS task delete failure */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    OS_TaskCreate(&CFE_ES_Global.TaskTable[1].TaskId, NULL, NULL, NULL,
                  0, 0, 0);
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId = 15;
    UT_SetOSFail(OS_TASKDELETE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(CFE_ES_Global.TaskTable[1].TaskId) <= 0,
              "CFE_ES_DeleteChildTask",
              "OS task delete failure");

    /* Test deleting a child task with the task ID out of range */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(OS_MAX_TASKS + 1) == CFE_ES_ERR_TASKID,
              "CFE_ES_DeleteChildTask",
              "Task ID too large");

    /* Test successfully exiting a child task */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[0].RecordUsed = FALSE;
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].TaskInfo.MainTaskId = 3;
    CFE_ES_ExitChildTask();
    UT_GetStubRetcodeAndCount(UT_KEY(OS_TaskExit), &OSTaskExitRtn.value,
                              &OSTaskExitRtn.count);
    UT_Report(__FILE__, __LINE__,
              OSTaskExitRtn.value == 1 && OSTaskExitRtn.count == 1,
              "CFE_ES_ExitChildTask",
              "Exit child task successful");

    /* Test exiting a child task within an app main task */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 0;
    OS_TaskCreate(&CFE_ES_Global.TaskTable[1].TaskId,
                  NULL, NULL, NULL, 0, 0, 0);
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId =
      CFE_ES_Global.TaskTable[1].TaskId;
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CANNOT_CALL_APP_MAIN &&
                  OSPrintRtn.count == -2,
              "CFE_ES_ExitChildTask",
              "Cannot call from a cFE application main task");

    /* Test exiting a child task with an error retrieving the app ID */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_GET_APP_ID && OSPrintRtn.count == -2,
              "CFE_ES_ExitChildTask",
              "Error calling CFE_ES_GetAppID");

    /* Test registering a child task with an OS task register failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKREGISTER_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterChildTask() == CFE_ES_ERR_CHILD_TASK_REGISTER,
              "CFE_ES_RegisterChildTask",
              "OS task register failed");

    /* Test successfully registering a child task */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterChildTask() == CFE_SUCCESS,
              "CFE_ES_RegisterChildTask",
              "Register child task successful");

    /* Test successfully adding a time-stamped message to the system log that
     * must be truncated
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_SYSTEM_LOG_SIZE - 10;
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LOG_DISCARD;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_SUCCESS,
              "CFE_ES_WriteToSysLog",
              "Add message to log that must be truncated");

    /* Reset the system log index to prevent an overflow in later tests */
    CFE_ES_ResetDataPtr->SystemLogIndex = 0;

    /* Test calculating a CRC on a range of memory using CRC type 8
     * NOTE: This capability is not currently implemented in cFE
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_8) == 0,
              "CFE_ES_CalculateCRC",
              "*Not implemented* CRC-8 algorithm");

    /* Test calculating a CRC on a range of memory using CRC type 16 */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_16) == 2688,
              "CFE_ES_CalculateCRC",
              "CRC-16 algorithm - memory read successful");

    /* Test calculating a CRC on a range of memory using CRC type 16 where the
     * memory reads fails
     */
    ES_ResetUnitTest();
    UT_SetBSPFail(CFE_PSP_INVALID_POINTER);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_16) == 4440,
              "CFE_ES_CalculateCRC",
              "CRC-16 algorithm - memory read failure");

    /* Test calculating a CRC on a range of memory using CRC type 32
     * NOTE: This capability is not currently implemented in cFE
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_32) == 0,
              "CFE_ES_CalculateCRC",
              "*Not implemented* CRC-32 algorithm");

    /* Test calculating a CRC on a range of memory using an invalid CRC type
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_CalculateCRC(&Data, 12, 345353, -1) == 0,
              "CFE_ES_CalculateCRC",
              "Invalid CRC type");

    /* Test CDS registering with a write CDS failure */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name3") == OS_ERROR,
              "CFE_ES_RegisterCDS",
              "Writing to BSP CDS failure");

    /* Test successful CDS registering */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Register CDS successful");

    /* Test CDS registering using an already registered name */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name") == CFE_ES_CDS_ALREADY_EXISTS,
              "CFE_ES_RegisterCDS",
              "Retrieve existing CDS");

    /* Test CDS registering using the same name, but a different size */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 6, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Get CDS of same name, but new size");

    /* Test CDS registering using a null name */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "") == CFE_ES_CDS_INVALID_NAME,
              "CFE_ES_RegisterCDS",
              "Invalid name size");

    /* Test CDS registering with no memory pool available */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    TempSize = CFE_ES_Global.CDSVars.MemPoolSize;
    CFE_ES_Global.CDSVars.MemPoolSize = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name") == CFE_ES_NOT_IMPLEMENTED,
              "CFE_ES_RegisterCDS",
              "No memory pool available");
    CFE_ES_Global.CDSVars.MemPoolSize = TempSize;

    /* Test CDS registering with a block size of zero */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 0,
                                 "Name") == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_RegisterCDS",
              "Block size zero");

    /* Test CDS registering with all the CDS registries taken */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;

    /* Set all the CDS registries to 'taken' */
    for (i = 0; i < CFE_ES_Global.CDSVars.MaxNumRegEntries; i++)
    {
        CFE_ES_Global.CDSVars.Registry[i].Taken = TRUE;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name2") == CFE_ES_CDS_REGISTRY_FULL,
              "CFE_ES_RegisterCDS",
              "No available entries");

    /* Restore all the CDS registries back to 'not taken' */
    for (i = 0; i < CFE_ES_Global.CDSVars.MaxNumRegEntries; i++)
    {
        CFE_ES_Global.CDSVars.Registry[i].Taken = FALSE;
    }

    /* Test CDS registering using a bad app ID */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name2") == CFE_ES_ERR_APPID,
              "CFE_ES_RegisterCDS",
              "Bad application ID");

    /* Register CDS to set up for the copy test */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].TaskInfo.MainTaskId = TestObjId;
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Register CDS successful (set up for copy test)");

    /* Test successfully copying to CDS */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CopyToCDS(CDSHandle, &TempSize) == CFE_SUCCESS,
              "CFE_ES_CopyToCDS",
              "Copy to CDS successful");

    /* Test successfully restoring from a CDS */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RestoreFromCDS(&TempSize, CDSHandle) == CFE_SUCCESS,
              "CFE_ES_RestoreFromCDS",
              "Restore from CDS successful");

    /* Test shared mutex take with a take error */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_SetRtnCode(&MutSemTakeRtn, -1, 1);
    CFE_ES_LockSharedData(__func__, 12345);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_MUTEX_TAKE && OSPrintRtn.count == -2,
              "CFE_ES_LockSharedData",
              "Mutex take error");

    /* Test shared mutex release with a release error */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    UT_SetRtnCode(&MutSemGiveRtn, -1, 1);
    CFE_ES_UnlockSharedData(__func__, 98765);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_MUTEX_GIVE && OSPrintRtn.count == -2,
              "CFE_ES_UnlockSharedData",
              "Mutex release error");

    /* Test successfully registering a generic counter */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId, "Counter1") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter",
              "Register counter successful");

    /* Test registering a generic counter that is already registered */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId,
                                        "Counter1") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Attempt to register an existing counter");

    /* Test registering the maximum number of generic counters */
    ES_ResetUnitTest();

    for (i = 1; i < CFE_ES_MAX_GEN_COUNTERS; i++)
    {

        snprintf(CounterName, 11, "Counter%d", i + 1);

        if (CFE_ES_RegisterGenCounter(&CounterId, CounterName) != CFE_SUCCESS)
        {
            break;
        }
    }

    UT_Report(__FILE__, __LINE__,
              i == CFE_ES_MAX_GEN_COUNTERS,
              "CFE_ES_RegisterGenCounter",
              "Register maximum number of counters");

    /* Test registering a generic counter after the maximum are registered */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId,
                                        "Counter999") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Maximum number of counters exceeded");

    /* Test getting a registered generic counter that doesn't exist */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCounterIDByName(&CounterId,
                                           "Counter999") ==
                  CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCounterIDByName",
              "Cannot get counter that does not exist");

    /* Test successfully getting a registered generic counter ID by name */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCounterIDByName(&CounterId,
                                           "Counter5") == CFE_SUCCESS,
              "CFE_ES_GetGenCounterIDByName",
              "Get generic counter ID successful");

    /* Test deleting a registered generic counter that doesn't exist */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteGenCounter(123456) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_DeleteGenCounter",
              "Cannot delete counter that does not exist");

    /* Test successfully deleting a registered generic counter by ID */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteGenCounter(CounterId) == CFE_SUCCESS,
              "CFE_ES_DeleteGenCounter",
              "Successful");

    /* Test successfully registering a generic counter to verify a place for
     * it is now available and to provide an ID for subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId, "CounterX") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter",
              "Register counter; back to maximum number");

    /* Test incrementing a generic counter that doesn't exist */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_IncrementGenCounter(CFE_ES_MAX_GEN_COUNTERS)
                == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_IncrementGenCounter",
              "Bad counter ID");

    /* Test successfully incrementing a generic counter */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_IncrementGenCounter(CounterId) == CFE_SUCCESS,
              "CFE_ES_IncrementGenCounter",
              "Increment counter successful");

    /* Test getting a generic counter value for a counter that doesn't exist */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCount(123456, &CounterCount) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount",
              "Bad counter ID");

    /* Test successfully getting a generic counter value */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCount(CounterId, &CounterCount) == CFE_SUCCESS &&
              CounterCount == 1,
              "CFE_ES_GetGenCount",
              "Get counter value successful");

    /* Test setting a generic counter value for a counter that doesn't exist */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SetGenCount(123456, 5) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_SetGenCount",
              "Bad counter ID");

    /* Test successfully setting a generic counter value */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SetGenCount(CounterId, 5) == CFE_SUCCESS,
              "CFE_ES_SetGenCount",
              "Set counter value successful");

    /* Test value retrieved from a generic counter value */
    ES_ResetUnitTest();
    CFE_ES_GetGenCount(CounterId, &CounterCount);
    UT_Report(__FILE__, __LINE__,
              (CounterCount == 5), "CFE_ES_SetGenCount",
              "Check value for counter set");

    /* Test handling of logging and reset after a core exception using
     * a non-running app
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
    strncpy((char *)CFE_ES_Global.AppTable[Id].StartParams.Name, "appName",
            sizeof(CFE_ES_Global.AppTable[Id].StartParams.Name));
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState = CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id;
    OS_TaskCreate(&CFE_ES_Global.TaskTable[Id2].TaskId, NULL, NULL, NULL,
                  0, 0, 0);
    CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP;
    CFE_ES_ProcessCoreException(TestObjId & 0xFFFF,
                                "Reason String",
                                &ExceptionContext,
                                sizeof(ExceptionContext));
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_CANNOT_RESTART_APP &&
                  OSPrintRtn.count == -2,
              "CFE_ES_ProcessCoreException",
              "Cannot restart a non-running application");

    /* Test handling of logging and reset after a core exception; processor
     * reset with no app restart
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
    Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
//    memset(CFE_ES_Global.TaskTable, 0, sizeof(CFE_ES_Global.TaskTable));
//    memset(CFE_ES_Global.AppTable, 0, sizeof(CFE_ES_Global.AppTable));
    strncpy((char *)CFE_ES_Global.AppTable[Id].StartParams.Name, "appName",
            sizeof(CFE_ES_Global.AppTable[Id].StartParams.Name));
    CFE_ES_Global.TaskTable[Id2].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id2].AppId = Id;
    OS_TaskCreate(&CFE_ES_Global.TaskTable[Id2].TaskId, NULL, NULL, NULL,
                  0, 0, 0);
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
    CFE_ES_Global.AppTable[3].StartParams.ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP + 1;
    CFE_ES_ProcessCoreException(TestObjId2 & 0xFFFF,
                                "Reason String",
                                &ExceptionContext,
                                sizeof(ExceptionContext));
    UT_Report(__FILE__, __LINE__,
              PSPRestartRtn.value == CFE_PSP_RST_TYPE_PROCESSOR &&
              PSPRestartRtn.count == 1,
              "CFE_ES_ProcessCoreException",
              "Processor reset with no application restart");

    /* Test handling of logging and reset after a core exception; power on
     * reset with no app restart
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 100;
    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
    CFE_ES_Global.AppTable[3].StartParams.ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP + 1;
    CFE_ES_ProcessCoreException(OS_MAX_TASKS - 1,
                                "Reason String",
                                &ExceptionContext,
                                sizeof(ExceptionContext));
    UT_Report(__FILE__, __LINE__,
              PSPRestartRtn.value == CFE_PSP_RST_TYPE_POWERON &&
              PSPRestartRtn.count == 1,
              "CFE_ES_ProcessCoreException",
              "Power on reset with no application restart");

    /* Test waiting for apps to initialize before continuing; transition from
     * initializing to running
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_OPERATIONAL;
    CFE_ES_Global.AppStartedCount = 1;
    CFE_ES_Global.AppReadyCount = 0;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState =
        CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_WaitForStartupSync(0);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.AppReadyCount == 1 &&
              CFE_ES_Global.AppTable[Id].StateRecord.AppState ==
                  CFE_ES_APP_STATE_RUNNING,
              "CFE_ES_WaitForStartupSync",
              "Transition from initializing to running");

    /* Test waiting for apps to initialize before continuing with the semaphore
     * already released
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppStartedCount = 1;
    CFE_ES_Global.AppReadyCount = 0;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState =
        CFE_ES_APP_STATE_RUNNING;
    CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_CORE_READY;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_WaitForStartupSync(99);

    /* Note - CFE_ES_WaitForStartupSync() returns void, nothing to check for
     * here.  This is for code coverage
     */
    UT_Report(__FILE__, __LINE__,
              1,
              "CFE_ES_WaitForStartupSync",
              "System state core ready");

    /* Test waiting for apps to initialize as an external app
     */
    ES_ResetUnitTest();
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[Id].AppId = Id;
    CFE_ES_Global.AppStartedCount = 1;
    CFE_ES_Global.AppReadyCount = 0;
    CFE_ES_Global.AppTable[Id].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[Id].StateRecord.AppState =
        CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_CORE_READY;
    CFE_ES_Global.AppTable[Id].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_WaitForStartupSync(99);

    /* Note - CFE_ES_WaitForStartupSync() returns void, nothing to check for
     * here.  This is for code coverage
     */
    UT_Report(__FILE__, __LINE__,
              1,
              "CFE_ES_WaitForStartupSync",
              "System state operational");

     /* Test adding a time-stamped message to the system log using an invalid
      * log mode
      */
     ES_ResetUnitTest();
     CFE_ES_ResetDataPtr->SystemLogMode = -1;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_ES_ERR_SYS_LOG_FULL &&
                   OSPrintRtn.value == UT_OSP_INVALID_LOG_MODE,
               "CFE_ES_WriteToSysLog",
               "Invalid log mode");

     /* Test successfully adding a time-stamped message to the system log that
      * causes the log index to be reset
      */
     ES_ResetUnitTest();
     CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_SYSTEM_LOG_SIZE;
     CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LOG_DISCARD;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_ES_ERR_SYS_LOG_FULL &&
                   OSPrintRtn.value == UT_OSP_SYSTEM_LOG_FULL,
               "CFE_ES_WriteToSysLog",
               "Add message to log that resets the log index");

     /* Test successfully adding a time-stamped message to the system log that
      * causes the log index to be reset
      */
     ES_ResetUnitTest();
     CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_SYSTEM_LOG_SIZE;
     CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LOG_OVERWRITE;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_SUCCESS &&
             CFE_ES_ResetDataPtr->SystemLogIndex < CFE_ES_SYSTEM_LOG_SIZE,
               "CFE_ES_WriteToSysLog",
               "Add message to log that resets the log index");

     /* Test run loop with an application error status */
     ES_ResetUnitTest();
     OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
     Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
     CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
     CFE_ES_Global.TaskTable[Id].AppId = Id;
     RunStatus = CFE_ES_APP_ERROR;
     CFE_ES_Global.AppTable[Id].StateRecord.AppControlRequest = CFE_ES_APP_ERROR;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == FALSE,
               "CFE_ES_RunLoop",
               "Application error run status");

     /* Test CDS registering using a name longer than the maximum allowed */
     ES_ResetUnitTest();
     OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
     Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
     CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
     CFE_ES_Global.TaskTable[Id].AppId = Id;

     for (i = 0; i < CFE_ES_CDS_MAX_NAME_LENGTH + 1; i++)
     {
         CDSName[i] = 'a';
     }

     CDSName[i] = '\0';

     UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, CDSName)
                 == CFE_ES_CDS_INVALID_NAME,
               "CFE_ES_RegisterCDS",
               "Invalid name size");

     /* Test registering a generic counter with a null counter ID pointer */
     ES_ResetUnitTest();
     UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(NULL,
                                         "Counter1") == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_RegisterGenCounter",
               "Attempt to register using a null counter ID pointer");

     /* Test registering a generic counter with a null counter name */
     ES_ResetUnitTest();

     for ( i = 0; i < CFE_ES_MAX_GEN_COUNTERS; i++ )
     {
        CFE_ES_Global.CounterTable[i].RecordUsed = FALSE;
     }

     UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId,
                                         NULL) == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_RegisterGenCounter",
               "Attempt to register using a null counter name");

     /* Test incrementing a generic counter where the record is not in use */
     ES_ResetUnitTest();
     CFE_ES_Global.CounterTable[CounterId].RecordUsed = FALSE;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_IncrementGenCounter(CounterId) == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_IncrementGenCounter",
               "Record not in use");

     /* Test setting a generic counter where the record is not in use */
     ES_ResetUnitTest();
     CFE_ES_Global.CounterTable[CounterId].RecordUsed = FALSE;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_SetGenCount(CounterId, 0) == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_SetGenCount",
               "Record not in use");

     /* Test getting a generic counter where the record is not in use */
     ES_ResetUnitTest();
     CFE_ES_Global.CounterTable[CounterId].RecordUsed = FALSE;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCount(CounterId, &CounterCount)
                 == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_GetGenCount",
               "Record not in use");

     /* Test getting a generic counter where the count is null */
     ES_ResetUnitTest();
     CFE_ES_Global.CounterTable[CounterId].RecordUsed = TRUE;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCount(CounterId, NULL)
                 == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_GetGenCount",
               "Null count");

     /* Test getting a registered generic counter ID using a null counter
      * pointer
      */
     ES_ResetUnitTest();
     UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCounterIDByName(NULL,
                                            "CounterX") == CFE_ES_BAD_ARGUMENT,
               "CFE_ES_GetGenCounterIDByName",
               "Null name");

     /* Test handling of logging and reset after failure to get the task info
      * from the OS
      */
     ES_ResetUnitTest();
     UT_SetDeferredRetcode(UT_KEY(OS_TaskGetInfo), 1, OS_INVALID_POINTER);
     CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
     CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
     CFE_ES_ProcessCoreException(0xFFFFFFFF,
                                 "Reason String",
                                 &ExceptionContext,
                                 sizeof(ExceptionContext));
     UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount == 1 &&
                 CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset == TRUE,
               "CFE_ES_ProcessCoreException",
               "Failure to get task info from OS");

     /* Test handling of logging and reset where the host ID doesn't match
      * the OS task ID
      */
     ES_ResetUnitTest();
     OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
     Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
     CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
     CFE_ES_Global.TaskTable[Id].AppId = Id;
     CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
     CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
     CFE_ES_Global.AppTable[Id].StartParams.ExceptionAction =
         CFE_ES_APP_EXCEPTION_RESTART_APP;
     CFE_ES_ProcessCoreException(0,
                                 "Reason String",
                                 &ExceptionContext,
                                 sizeof(ExceptionContext));
     UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == 0 &&
                   OSPrintRtn.count == -1,
               "CFE_ES_ProcessCoreException",
               "Host ID doesn't match OS task ID");

     /* Test handling of logging and reset where CFE_ES_GetTaskInfo fails */
     ES_ResetUnitTest();
     OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
     Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
     OS_TaskCreate(&TestObjId2, "UT", NULL, NULL, 0, 0, 0);
     Id2 = ES_UT_OSALID_TO_ARRAYIDX(TestObjId2);
     CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
     CFE_ES_Global.TaskTable[Id].AppId = Id;
     CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
     CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
     CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
     CFE_ES_Global.TaskTable[Id].AppId = Id2;
     CFE_ES_Global.AppTable[Id2].RecordUsed = FALSE;
     CFE_ES_Global.AppTable[Id2].StartParams.ExceptionAction =
         CFE_ES_APP_EXCEPTION_RESTART_APP;
     CFE_ES_ProcessCoreException(Id,
                                 "Reason String",
                                 &ExceptionContext,
                                 sizeof(ExceptionContext));
     UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == 0 &&
                   OSPrintRtn.count == -2,
               "CFE_ES_ProcessCoreException",
               "CFE_ES_GetTaskInfo failure");

     /* Test unsuccessful CDS registering */
     ES_ResetUnitTest();
     OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
     Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
     CFE_ES_Global.TaskTable[Id].RecordUsed = TRUE;
     CFE_ES_Global.TaskTable[Id].AppId = Id;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                  0xffffffff,
                                  "Name") == CFE_ES_ERR_MEM_BLOCK_SIZE,
               "CFE_ES_RegisterCDS",
               "Register CDS unsuccessful");
} /* end TestAPI */

void TestCDS()
{
    uint32 Temp;

#ifdef UT_VERBOSE
    UT_Text("Begin Test CDS\n");
#endif

    /* Test validating the app ID using a bad ID value */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = CFE_ES_MAX_APPLICATIONS + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_ValidateAppID(&Temp) == CFE_ES_ERR_APPID,
              "CFE_ES_CDS_ValidateAppID",
              "Bad app ID");

    /* Test memory pool rebuild and registry recovery with an
     * unreadable registry
     */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES + 2;
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "Second read from CDS bad");

    /* Test CDS registry initialization with a CDS write failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPWriteCDSRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitCDSRegistry() == -1,
              "CFE_ES_InitCDSRegistry",
              "Failed to write registry size");

    /* Test successful CDS initialization */
    ES_ResetUnitTest();
    UT_SetCDSSize(128 * 1024);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization successful");

    /* Test CDS initialization with a read error */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == -1,
              "CFE_ES_CDS_EarlyInit",
              "Unrecoverable read error");

    /* Test CDS initialization with size below the minimum */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetCDSSize(1024);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == OS_SUCCESS &&
              BSPReadCDSRtn.count == 1,
              "CFE_ES_CDS_EarlyInit",
              "CDS size less than minimum");

    /* Test CDS initialization with size not obtainable */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_GETCDSSIZE_FAIL);
    UT_SetCDSSize(128 * 1024);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == OS_ERROR,
              "CFE_ES_CDS_EarlyInit",
              "Unable to obtain CDS size");

    /* Test CDS initialization with rebuilding not possible */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetCDSBSPCheckValidity(TRUE);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == OS_SUCCESS &&
              BSPReadCDSRtn.count == 0,
              "CFE_ES_CDS_EarlyInit",
              "Rebuilding not possible; create new CDS");

    /* Test CDS validation with second CDS read call failure */
    ES_ResetUnitTest();
    UT_SetCDSBSPCheckValidity(TRUE);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateCDS() == OS_ERROR,
              "CFE_ES_ValidateCDS",
              "CDS read (second call) failed");

    /* Test CDS validation with CDS read end check failure */
    ES_ResetUnitTest();
    UT_SetCDSReadGoodEnd(FALSE);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_ValidateCDS",
              "Reading from CDS failed end check");
    UT_SetCDSReadGoodEnd(TRUE);

    /* Test CDS validation with first CDS read call failure */
    ES_ResetUnitTest();
    UT_SetCDSBSPCheckValidity(FALSE);
    UT_SetRtnCode(&BSPReadCDSRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateCDS() == -1,
              "CFE_ES_ValidateCDS",
              "CDS read (first call) failed");

    /* Test CDS initialization where first write call to the CDS fails */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(128 * 1024) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "Clear CDS failed");

    /* Test CDS initialization where second write call to the CDS fails */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(0) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write (second call) failed");

    /* Test CDS initialization where third write call to the CDS fails */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(0) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write (third call) failed");

    /* Test rebuilding the CDS where the registry is too large */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES + 1;
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "Registry too large to recover");

    /* Test successfully rebuilding the CDS */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_SUCCESS,
              "CFE_ES_RebuildCDS",
              "CDS rebuild successful");

    /* Test rebuilding the CDS with the registry unreadable */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "CDS registry unreadable");

    /* Test deleting the CDS from the registry with a registry write failure */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].Table = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].MemHandle = 200;
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "NO_APP.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_ERROR, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteCDS("NO_APP.CDS_NAME", TRUE) == -1,
              "CFE_ES_DeleteCDS",
              "CDS registry write failed");

    /* Test deleting the CDS from the registry with the owner application
     * still active
     */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].Table = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].MemHandle = 200;
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "CFE_ES.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteCDS("CFE_ES.CDS_NAME", TRUE) ==
                  CFE_ES_CDS_OWNER_ACTIVE_ERR,
              "CFE_ES_DeleteCDS",
              "Owner application still active");

    /* Test CDS initialization where rebuilding the CDS is successful */
    ES_ResetUnitTest();
    UT_SetCDSBSPCheckValidity(TRUE);
    UT_SetCDSReadGoodEnd(FALSE);
    UT_SetCDSRebuild(TRUE);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization with successful rebuild");
    UT_SetCDSBSPCheckValidity(FALSE);
    UT_SetCDSReadGoodEnd(TRUE);
    UT_SetCDSRebuild(FALSE);

    /* Test CDS initialization where rebuilding the CDS is unsuccessful */
    ES_ResetUnitTest();
    UT_SetCDSBSPCheckValidity(TRUE);
    UT_SetCDSReadGoodEnd(FALSE);
    UT_SetCDSRebuild(TRUE);
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_ERROR, 1);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization with unsuccessful rebuild");
    UT_SetCDSBSPCheckValidity(FALSE);
    UT_SetCDSReadGoodEnd(TRUE);
    UT_SetCDSRebuild(FALSE);

    /* Test CDS initialization where write call to the CDS fails while filling
     * in extra uint32 space
     */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_ERROR, 5);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(32 * 4 - 4) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write failed while filling extra space");

    /* Test CDS initialization where creating the CDS pool fails */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(sizeof(CFE_ES_Global.CDSVars.ValidityField)) ==
                                     CFE_ES_BAD_ARGUMENT,
              "CFE_ES_InitializeCDS",
              "CDS pool create failed");

    /* Test deleting the CDS from the registry with a CDS name longer than the
     * maximum allowed
     */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.Registry[0].Name[0] = '\0';;

    for (Temp = 0; Temp < CFE_ES_CDS_MAX_FULL_NAME_LEN; Temp++)
    {
        strcat(CFE_ES_Global.CDSVars.Registry[0].Name, "a");
    }

    CFE_ES_Global.CDSVars.Registry[0].Name[CFE_ES_CDS_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteCDS(CFE_ES_Global.CDSVars.Registry[0].Name,
                               TRUE) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_DeleteCDS",
              "CDS name too long");
} /* End TestCDS */

void TestCDSMempool(void)
{
    uint32                  MinCDSSize = CFE_ES_CDS_MIN_BLOCK_SIZE +
                                         sizeof(CFE_ES_CDSBlockDesc_t);
    CFE_ES_CDSBlockHandle_t BlockHandle;
    int                     Data;
    uint32                  i;

    extern uint32 CFE_ES_CDSMemPoolDefSize[];

#ifdef UT_VERBOSE
    UT_Text("Begin Test CDS memory pool\n");
#endif

    /* Set up the CDS block to read in the following tests */
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_CDSBlockDesc.SizeUsed =  512;

    /* Test creating the CDS pool with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CreateCDSPool(0, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_CreateCDSPool",
              "CDS pool size too small");

    /* Test rebuilding the CDS pool with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(0, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RebuildCDSPool",
              "CDS pool size too small");

    /* Test rebuilding the CDS pool with the CDS block unused */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL);
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "CDS block unused");

    /* Test rebuilding the CDS pool with a CDS read failure */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Error reading CDS");

    /* Test rebuilding the CDS pool with a CDS write failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL | BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Error writing CDS");

    /* Test rebuilding the CDS pool with a block not previously used */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 1;

    /* Set flags so as to fail on second CDS read */
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 1) == OS_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "CDS block not used before");
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;

    /* Test rebuilding the CDS pool with an invalid block descriptor */
    ES_ResetUnitTest();
    CFE_ES_CDSMemPoolDefSize[0] = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Invalid block descriptor");
    CFE_ES_CDSMemPoolDefSize[0] = CFE_ES_CDS_MAX_BLOCK_SIZE;

    /* Test successfully creating a pool where the offset = 0 */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CreateCDSPool(1000000, 0) == CFE_SUCCESS,
              "CFE_ES_CreateCDSPool",
              "Create with zero offset; successful");

    /* Test allocating a CDS block with a block size error */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle,
                                 800) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size error");

    /* Test returning a CDS block to the memory pool using an invalid
         block descriptor */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid block descriptor");

    /* Test creating a new pool and set up for getting a pre-made block */
    ES_ResetUnitTest();
    BlockHandle = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CreateCDSPool(1000000, 8) == CFE_SUCCESS,
              "CFE_ES_CreateCDSPool",
              "Create with non-zero offset; successful");

    /* Test successfully allocating a pre-made CDS block */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle, 800) == OS_SUCCESS,
              "CFE_ES_GetCDSBlock",
              "Get a CDS block; successful");

    /* Test successfully returning a CDS block back to the memory pool */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == OS_SUCCESS,
              "CFE_ES_PutCDSBlock",
              "Return a CDS block; successful");

    /* Test allocating a CDS block with a CDS read failure */
    ES_ResetUnitTest();
    UT_SetRtnCode(&BSPReadCDSRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle, 800) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_GetCDSBlock",
              "Error reading CDS");

    /* Test allocating a CDS block with a CDS write failure */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_SetRtnCode(&BSPWriteCDSRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle, 800) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_GetCDSBlock",
              "Error writing CDS");

    /* Test allocating a CDS block using a block size that's too large */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle,
                                 CFE_ES_CDS_MAX_BLOCK_SIZE + 1) ==
                  CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size too large");

    /* Test returning a CDS block to the memory pool with an
     * invalid CDS handle
     */
    ES_ResetUnitTest();
    BlockHandle = sizeof(CFE_ES_Global.CDSVars.ValidityField) - 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid CDS handle");

    /* Test returning a CDS block to the memory pool with a CDS read error */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    BlockHandle = 10;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_PutCDSBlock",
              "Error reading CDS");

    /* Test returning a CDS block to the memory pool with an invalid
     * block descriptor
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid block descriptor");

    /* Test returning a CDS block to the memory pool with the block size
     * too large
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid memory handle");

    /* Test returning a CDS block to the memory pool with a CDS write error */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = 452;
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_PutCDSBlock",
              "Error writing CDS");

    /* Test CDS block write using an invalid memory handle */
    ES_ResetUnitTest();
    BlockHandle = 7;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid memory handle");
    BlockHandle = 10;

    /* Test CDS block write with the block size too large */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Actual size too large");
    CFE_ES_CDSBlockDesc.ActualSize  = 452;

    /* Test CDS block write using an invalid (unused) block */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid CDS block");
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;

    /* Test CDS block write with a CDS write error (block descriptor) */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error writing block descriptor to CDS");

    /* Test CDS block write with a CDS write error (new data) */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_SUCCESS, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error writing new data to CDS");

    /* Test CDS block write with a CDS read error */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error reading CDS");

    /* Test CDS block read with an invalid memory handle */
    ES_ResetUnitTest();
    BlockHandle = 7;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid memory handle");
    BlockHandle = 10;

    /* Test CDS block read with the block size too large */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Actual size too large");
    CFE_ES_CDSBlockDesc.ActualSize = 452;

    /* Test CDS block read using an invalid (unused) block */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid CDS block");
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;

    /* Test CDS block read with a CRC mismatch */
    ES_ResetUnitTest();
    Data = CFE_ES_CDSBlockDesc.CRC;
    CFE_ES_CDSBlockDesc.CRC = 56456464;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data,
                                  BlockHandle) == CFE_ES_CDS_BLOCK_CRC_ERR,
              "CFE_ES_CDSBlockRead",
              "CRC doesn't match");
    CFE_ES_CDSBlockDesc.CRC = Data;

    /* Test CDS block read with a CDS read error (block descriptor) */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR,
              "CFE_ES_CDSBlockRead",
              "Error reading block descriptor from CDS");

    /* Test CDS block read with a CDS read error (block data) */
    ES_ResetUnitTest();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR,
              "CFE_ES_CDSBlockRead",
              "Error reading block data from CDS");

    /* Test allocating a CDS block with a block size error (path 2)*/
    ES_ResetUnitTest();
    CFE_ES_CDSMemPool.Current = CFE_ES_CDSMemPool.End;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle,
                                 800) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size error (second path)");

    /* Test rebuilding the CDS pool with an invalid block descriptor */
    ES_ResetUnitTest();
    CFE_ES_CDSMemPoolDefSize[0] = 0;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == OS_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "Invalid block descriptor");

    /* Test returning a CDS block to the memory pool with an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 0x1111;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid check bit pattern");

    /* Test returning a CDS block to the memory pool with an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 0x1111;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid check bit pattern");

    /* Test returning a CDS block to the memory pool with an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 0x1111;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid check bit pattern");

    /* Test returning a CDS block to the memory pool with an
     * invalid CDS handle (path 2)
     */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) -
            CFE_ES_CDSMemPool.MinBlockSize -
            sizeof(CFE_ES_Global.CDSVars.ValidityField) + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid CDS handle (second path)");

    /* Test CDS block write with a CDS read error (path 2) */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) -
            CFE_ES_CDSMemPool.MinBlockSize -
            sizeof(CFE_ES_Global.CDSVars.ValidityField) + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Error writing CDS (second path)");

    /* Test CDS block read with a CDS read error (path 2) */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) -
            CFE_ES_CDSMemPool.MinBlockSize -
            sizeof(CFE_ES_Global.CDSVars.ValidityField) + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Error reading CDS (second path)");

    /* Test CDS minimum memory pool size with no non-zero blocks defined */
    ES_ResetUnitTest();

    for (i = 0; i < CFE_ES_CDS_NUM_BLOCK_SIZES; i++)
    {
        CFE_ES_CDSMemPoolDefSize[i] = 0;
    }

    CFE_ES_CDSMemPool.MinBlockSize = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSReqdMinSize(1) == sizeof(CFE_ES_CDSBlockDesc_t),
              "CFE_ES_CDSReqdMinSize",
              "No non-zero blocks");
}

void TestESMempool(void)
{
    CFE_ES_MemHandle_t    HandlePtr;
    uint8                 Buffer[CFE_ES_MAX_BLOCK_SIZE];
    Pool_t                blk_address;
    uint8                 *address = NULL;
    uint8                 *address2 = NULL;
    Pool_t                *PoolPtr;
    CFE_ES_MemPoolStats_t Stats;
    uint32                BlockSizes[4];
    BD_t                  *BdPtr;
    CFE_ES_MemHandle_t    HandlePtr2;
    CFE_ES_MemHandle_t    HandlePtrSave;

#ifdef UT_VERBOSE
    UT_Text("Begin Test ES memory pool\n");
#endif

    /* Test creating memory pool without using a mutex with the pool size
      * too small
      */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateNoSem(&HandlePtr,
                                     Buffer,
                                     0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateNoSem",
              "Pool size too small");

    /* Test successfully creating memory pool without using a mutex */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateNoSem(&HandlePtr,
                                     Buffer,
                                     CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreateNoSem",
              "Memory pool create; successful");

    /* Test creating memory pool using a mutex with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr, Buffer, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreate",
              "Pool size too small");

    /* Test successfully creating memory pool using a mutex */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [1]; successful");

    /* Test successfully allocating a pool buffer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [1]; successful");

    /* Test successfully getting the size of an existing pool buffer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful");

    /* Test successfully getting the size of an existing pool buffer.  Use no
     * mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test successfully returning a pool buffer to the memory pool */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_PutPoolBuf",
              "Return buffer to the memory pool; successful");

    /* Test successfully allocating an additional pool buffer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [2]; successful");

    /* Test successfully returning a pool buffer to the second memory pool.
     * Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_PutPoolBuf",
              "Return buffer to the second memory pool; successful");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test handle validation using a handle with an address not on a
     * 32-bit boundary.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(3) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; not on 32-bit boundary");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test handle validation using a handle with an invalid memory address */
    ES_ResetUnitTest();
    PoolPtr = (Pool_t *) &HandlePtr2;
    PoolPtr->Start = (cpuaddr *)&HandlePtrSave;
    PoolPtr->Size = 64;
    UT_SetRtnCode(&PSPMemValRangeRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(HandlePtr2) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; bad memory address");

    /* Test handle validation using a handle where the first pool structure
     * field is not the pool start address
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(HandlePtr2) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; not pool start address");

    /* Test allocating a pool buffer where the memory handle is not the pool
     * start address
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address,
                                HandlePtr2,
                                256) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBuf",
              "Invalid handle; not pool start address");

    /* Test getting memory pool statistics where the memory handle is not
     * the pool start address
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetMemPoolStats(&Stats,
                                     HandlePtr2) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetMemPoolStats",
              "Invalid handle; not pool start address");

    /* Test handle validation using a handle where the pool size is not a
     * multiple of 4 bytes
     */
    ES_ResetUnitTest();
    PoolPtr = (Pool_t *) &HandlePtr2;
    PoolPtr->Start = (cpuaddr *)&HandlePtr2;
    PoolPtr->Size = 63;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(HandlePtr2) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; pool size not multiple of 4 bytes");

    /* Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address,
                                HandlePtr,
                                75000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Requested pool size too large");

    /* Test getting the size of an existing pool buffer using an
     * invalid handle
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, (uint32 *) address) ==
                  CFE_ES_BUFFER_NOT_IN_POOL,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle");

    /* Test returning a pool buffer using an invalid memory block */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid memory block");

    /* Test initializing a pre-allocated pool specifying a number of block
     * sizes greater than the maximum
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES + 2,
                                  BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Number of block sizes exceeds maximum");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and using the default block size
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  NULL,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (default block size)");

    /* Test initializing a pre-allocated pool where the memory pool address
     * is not 32-bit aligned
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) 145453,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  BlockSizes,
                                  CFE_ES_USE_MUTEX)== CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool address not 32-bit aligned");

    /* Test initializing a pre-allocated pool using an invalid mutex option */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  BlockSizes,
                                  2) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Invalid mutex option");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and specifying the block size with one block size set
     * to zero
     */
    ES_ResetUnitTest();
    BlockSizes[0] = 10;
    BlockSizes[1] = 50;
    BlockSizes[2] = 100;
    BlockSizes[3] = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                 (uint8 *) &blk_address,
                                 0,
                                 4,
                                 BlockSizes,
                                 CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (block size specified)");

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [2]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block
     */
    ES_ResetUnitTest();
    BdPtr = (BD_t *) ((uint8 *) address - sizeof(BD_t));
    BdPtr->Allocated = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; unallocated block");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    BdPtr = (BD_t *) ((uint8 *) address - sizeof(BD_t));
    BdPtr->Allocated = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; unallocated block (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test returning a pool buffer using an unallocated block */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Deallocate an unallocated block");

    /* Test returning a pool buffer using an unallocated block.  Use no mutex
     * in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Deallocate an unallocated block (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    BdPtr->Allocated = 0xaaaa;
    BdPtr->CheckBits = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; check bit pattern");

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern.  Use no mutex in order to get branch path
     * coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    BdPtr->Allocated = 0xaaaa;
    BdPtr->CheckBits = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; check bit pattern (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid/corrupted memory descriptor");

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid/corrupted memory descriptor (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [3]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [4]; successful");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum
     */
    ES_ResetUnitTest();
    BdPtr->CheckBits = 0x5a5a;
    BdPtr->Size =CFE_ES_MAX_BLOCK_SIZE +1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Pool buffer size exceeds maximum");

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    BdPtr->CheckBits = 0x5a5a;
    BdPtr->Size =CFE_ES_MAX_BLOCK_SIZE +1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Pool buffer size exceeds maximum (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address2,
                                HandlePtr,
                                99000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Pool buffer size exceeds maximum");

    /* Test handle validation using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(0) == FALSE,
              "CFE_ES_ValidateHandle",
              "NULL handle");

    /* Test returning a pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(0,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "NULL memory handle");

    /* Test allocating a pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address,
                                0,
                                256) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBuf",
              "NULL memory handle");

    /* Test getting the size of an existing pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(0,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "NULL memory handle");

    /* Test initializing a pre-allocated pool specifying a zero block size */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  0,
                                  NULL,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (zero block size)");

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address2,
                                HandlePtr,
                                99000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Pool buffer size exceeds maximum (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address,
                                HandlePtr,
                                1) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Requested pool size too large (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test getting the size of a pool buffer that is not in the pool */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) HandlePtr + 1) ==
                                        CFE_ES_BUFFER_NOT_IN_POOL,
              "CFE_ES_GetPoolBufInfo",
              "Invalid pool buffer");

    /* Test getting the size of a pool buffer with an invalid memory handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) HandlePtr + 1) ==
                                        CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid memory handle");
}

#ifdef CFE_ARINC653
void TestStaticApp(void)
{
    int                     j;
    uint32                  Id;
    uint32                  LibraryIdPtr;
    CFE_ES_AppMainFuncPtr_t MainFunc = 0;

    /* Test successful static app creation */
    ES_ResetUnitTest();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_SUCCESS &&
              CFE_ES_Global.TaskTable[1].RecordUsed == TRUE,
              "CFE_ES_StaticAppCreate",
              "Create static application; successful");

    /* Test static app creation with a task create failure */
    ES_ResetUnitTest();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_StaticAppCreate",
              "Task create error");

    /* Test static app creation specifying a slot already in use */
    ES_ResetUnitTest();
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_SUCCESS,
              "CFE_ES_StaticAppCreate",
              "Task slot in use");

    /* Test static app creation with no free slots available */
    ES_ResetUnitTest();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
        CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_StaticAppCreate",
              "No free application slots available");

    /* Test successful static shared library initialization */
    ES_ResetUnitTest();
    UT_SetLibInitRtn(CFE_SUCCESS);
    LibraryIdPtr = 1;
    CFE_ES_Global.LibTable[0].RecordUsed = FALSE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitStaticLibrary(&LibraryIdPtr,
                                       (CFE_ES_LibInitFuncPtr_t) UT_LibInit,
                                       "LibName") == CFE_SUCCESS &&
              LibraryIdPtr == 0,
              "CFE_ES_InitStaticLibrary",
              "Static library initialization; successful");

    /* Test static shared library initialization with a library load failure */
    ES_ResetUnitTest();
    UT_SetLibInitRtn(CFE_ES_ERR_LOAD_LIB);
    CFE_ES_Global.LibTable[0].RecordUsed = FALSE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitStaticLibrary(&LibraryIdPtr,
                                       (CFE_ES_LibInitFuncPtr_t) UT_LibInit,
                                       "LibName") == CFE_ES_ERR_LOAD_LIB,
              "CFE_ES_InitStaticLibrary",
              "Shared library initialization failure");

    /* Test static shared library initialization with no free
     * slots available
     */
    ES_ResetUnitTest();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = TRUE;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitStaticLibrary(&LibraryIdPtr,
                                       (CFE_ES_LibInitFuncPtr_t) UT_LibInit,
                                       "LibName") == CFE_ES_ERR_LOAD_LIB,
              "CFE_ES_InitStaticLibrary",
              "No free library slots available");

    /* Test successful static application initialization */
    ES_ResetUnitTest();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = FALSE;
    }

    strncpy(CFE_ES_StaticStartupTable[0].Name,
            "StaticAppName", OS_MAX_API_NAME);
    CFE_ES_StaticStartupTable[0].Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_APP;
    CFE_ES_StaticStartupTable[0].ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP + 1;
    CFE_ES_StartStaticApplications(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_StaticStartupTable[0].ExceptionAction ==
                  CFE_ES_APP_EXCEPTION_PROC_RESTART &&
              OSPrintRtn.value == UT_OSP_NO_MESSAGE,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Initialize static application; successful");

    /* Test successful static library initialization */
    ES_ResetUnitTest();
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_LIB;
    CFE_ES_StartStaticApplications(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_MESSAGE,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Initialize static shared library; successful");

    /* Test successful device driver initialization */
    ES_ResetUnitTest();
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_DRV;
    CFE_ES_StartStaticApplications(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_MESSAGE,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Device drivers not yet implemented");

    /* Test successful static application initialization null action */
    ES_ResetUnitTest();
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_NULL;
    CFE_ES_StartStaticApplications(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_MESSAGE,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* No action");

    /* Test successful static library initialization with an invalid
     * entry type
     */
    ES_ResetUnitTest();
    CFE_ES_StaticStartupTable[0].ObjectType = 0xff;
    CFE_ES_StartStaticApplications(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              OSPrintRtn.value == UT_OSP_NO_MESSAGE,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Unexpected entry type");
}
#endif

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
    CFE_ES_TaskPipe(MsgPtr);
}
