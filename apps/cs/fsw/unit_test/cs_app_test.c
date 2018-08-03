 /*************************************************************************
 ** File:
 **   $Id: cs_app_test.c 1.14 2017/03/29 17:29:00EDT mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file cs_app.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_app_test.h"
#include "cs_app.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
#include "cs_version.h"
#include "cs_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_psp_watchdog_stubs.h"
#include "ut_cfe_psp_timer_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

int32 CS_APP_TEST_CFE_TBL_LoadHook(CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr )
{
    return CFE_SUCCESS;
}

uint32 CS_APP_TEST_CFE_ES_RestoreFromCDS_Hook(uint8* DataStoreBuffer, uint32 DataStoreHandle)
{
    DataStoreBuffer[0] = CS_STATE_ENABLED;
    DataStoreBuffer[1] = CS_STATE_ENABLED;
    DataStoreBuffer[2] = CS_STATE_ENABLED;
    DataStoreBuffer[3] = CS_STATE_ENABLED;

    DataStoreBuffer[4] = CS_STATE_ENABLED;
    DataStoreBuffer[5] = CS_STATE_ENABLED;
    
    return CFE_SUCCESS;
}

int32 CS_AppInit (void);

int32 CS_AppPipe (CFE_SB_MsgPtr_t MessagePtr);

void CS_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr);

int32 CS_CreateRestoreStatesFromCDS(void);

void CS_App_TestCmdTlmAlign(void)
{
    /* Ensures the command and telemetry structures are padded to and aligned to 32-bits */
#define CMD_STRUCT_DATA_IS_32_ALIGNED(x) ((sizeof(x) - CFE_SB_CMD_HDR_SIZE) % 4) == 0
#define TLM_STRUCT_DATA_IS_32_ALIGNED(x) ((sizeof(x) - CFE_SB_TLM_HDR_SIZE) % 4) == 0
    
    UtAssert_True (TLM_STRUCT_DATA_IS_32_ALIGNED(CS_HkPacket_t),      "CS_HkPacket_t is 32-bit aligned");
    
    UtAssert_True (CMD_STRUCT_DATA_IS_32_ALIGNED(CS_NoArgsCmd_t),     "CS_NoArgsCmd_t is 32-bit aligned");
    UtAssert_True (CMD_STRUCT_DATA_IS_32_ALIGNED(CS_GetEntryIDCmd_t), "CS_GetEntryIDCmd_t is 32-bit aligned");
    UtAssert_True (CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EntryCmd_t), "CS_EntryCmd_t is 32-bit aligned");
    UtAssert_True (CMD_STRUCT_DATA_IS_32_ALIGNED(CS_TableNameCmd_t),  "CS_TableNameCmd_t is 32-bit aligned");
    UtAssert_True (CMD_STRUCT_DATA_IS_32_ALIGNED(CS_AppNameCmd_t),    "CS_AppNameCmd_t is 32-bit aligned");
    UtAssert_True (CMD_STRUCT_DATA_IS_32_ALIGNED(CS_OneShotCmd_t),    "CS_OneShotCmd_t is 32-bit aligned");

}

void CS_AppMain_Test_Nominal(void)
{
    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    CS_AppMain();
    
    /* Verify results */
    
    /* 2 Messages Tested elsewhere so we can ignore them here. INFO:CS Initialized  and ERROR:Invalid Command pipe  */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_EXIT_INF_EID, CFE_EVS_INFORMATION, "App terminating, RunStatus:0x00000001"),
            "App terminating, RunStatus:0x00000001");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("CS App terminating, RunStatus:0x00000001, RC:0x00000000\n"),
        "CS App terminating, RunStatus:0x00000001, RC:0x00000000\n");
    /* Generates 2 event messages we don't care about in this test */

} /* end CS_AppMain_Test_Nominal */

void CS_AppMain_Test_RegisterAppError(void)
{
    /* Set to satisfy subsequent condition "Result != CFE_SUCCESS" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_AppMain();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RunStatus == CFE_ES_APP_ERROR, "CS_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF"),
        "App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("CS App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF\n"),
        "CS App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end CS_AppMain_Test_RegisterAppError */

void CS_AppMain_Test_AppInitError(void)
{
    /* Set to make subfunction CS_AppInit return -1 */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_AppMain();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RunStatus == CFE_ES_APP_ERROR, "CS_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF"),
        "App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("CS App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF\n"),
        "CS App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end CS_AppMain_Test_AppInitError */

void CS_AppMain_Test_RcvMsgError(void)
{
    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy subsequent condition "Result != CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_AppMain();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RunStatus == CFE_ES_APP_ERROR, "CS_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF"),
        "App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("CS App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF\n"),
        "CS App terminating, RunStatus:0x00000003, RC:0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end CS_AppMain_Test_RcvMsgError */

void CS_AppMain_Test_AppPipeError(void)
{
    /* Set to make CS_AppPipe return -1 */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, CS_SEND_HK_MID, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(CS_NoArgsCmd_t), 1);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 10);

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    CS_AppMain();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RunStatus == CFE_ES_APP_ERROR, "CS_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, RunStatus:0x00000003, RC:0xCA000001"),
        "App terminating, RunStatus:0x00000003, RC:0xCA000001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");
    /* Generates 3 event messages we don't care about in this test */
    
    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("CS App terminating, RunStatus:0x00000003, RC:0xCA000001\n"),
        "CS App terminating, RunStatus:0x00000003, RC:0xCA000001\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end CS_AppMain_Test_AppPipeError */

void CS_AppInit_Test_Nominal(void)
{
    char    Message[125];
    int32   Result;

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    sprintf(Message, "CS Initialized. Version %d.%d.%d.%d", CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(CS_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_Nominal */

void CS_AppInit_Test_NominalPowerOnReset(void)
{
    char    Message[125];
    int32   Result;

    CS_AppData.EepromCSState = 99;
    CS_AppData.MemoryCSState = 99;
    CS_AppData.AppCSState    = 99;
    CS_AppData.TablesCSState = 99;
        
    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);
    
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, CFE_ES_POWERON_RESET, 1);
        
    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    sprintf(Message, "CS Initialized. Version %d.%d.%d.%d", CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(CS_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    
    UtAssert_True (CS_AppData.EepromCSState == CS_EEPROM_TBL_POWERON_STATE, "CS_AppData.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True (CS_AppData.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE, "CS_AppData.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True (CS_AppData.AppCSState    == CS_APPS_TBL_POWERON_STATE  , "CS_AppData.AppCSState    == CS_APPS_TBL_POWERON_STATE"  );
    UtAssert_True (CS_AppData.TablesCSState == CS_TABLES_TBL_POWERON_STATE, "CS_AppData.TablesCSState == CS_TABLES_TBL_POWERON_STATE");
    
    UtAssert_True (CS_AppData.OSCSState      == CS_OSCS_CHECKSUM_STATE, "CS_AppData.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True (CS_AppData.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE, "CS_AppData.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_NominalPowerOnReset */

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)

void CS_AppInit_Test_NominalProcReset(void)
{
    char    Message[125];
    int32   Result;

    CS_AppData.EepromCSState = 99;
    CS_AppData.MemoryCSState = 99;
    CS_AppData.AppCSState    = 99;
    CS_AppData.TablesCSState = 99;
        
    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);
    
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, CFE_ES_PROCESSOR_RESET, 2);
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_RESTOREFROMCDS_INDEX, CS_APP_TEST_CFE_ES_RestoreFromCDS_Hook);
        
    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    sprintf(Message, "CS Initialized. Version %d.%d.%d.%d", CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(CS_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    
    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_ENABLED, "CS_AppData.EepromCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.MemoryCSState == CS_STATE_ENABLED, "CS_AppData.MemoryCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.AppCSState    == CS_STATE_ENABLED, "CS_AppData.AppCSState    == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.TablesCSState == CS_STATE_ENABLED, "CS_AppData.TablesCSState == CS_STATE_ENABLED");
    
    UtAssert_True (CS_AppData.OSCSState      == CS_STATE_ENABLED, "CS_AppData.OSCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.CfeCoreCSState == CS_STATE_ENABLED, "CS_AppData.CfeCoreCSState == CS_STATE_ENABLED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_NominalProcReset */

void CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS(void)
{
    int32   Result;

    CS_AppData.EepromCSState = 99;
    CS_AppData.MemoryCSState = 99;
    CS_AppData.AppCSState    = 99;
    CS_AppData.TablesCSState = 99;
    CS_AppData.OSCSState     = 99;
    CS_AppData.CfeCoreCSState = 99;
    
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, CFE_ES_PROCESSOR_RESET, 2);
    
    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    
    UtAssert_True (CS_AppData.EepromCSState == 99, "CS_AppData.EepromCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.MemoryCSState == 99, "CS_AppData.MemoryCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.AppCSState    == 99, "CS_AppData.AppCSState  == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == CS_STATE_ENABLED");

    UtAssert_True (CS_AppData.OSCSState      == 99, "CS_AppData.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True (CS_AppData.CfeCoreCSState == 99, "CS_AppData.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_CreateRestoreStatesFromCDS_Test_NoCDS */

void CS_CreateRestoreStatesFromCDS_Test_CDSSuccess(void)
{
    int32   Result;

    CS_AppData.EepromCSState = 99;
    CS_AppData.MemoryCSState = 99;
    CS_AppData.AppCSState    = 99;
    CS_AppData.TablesCSState = 99;
    CS_AppData.OSCSState     = 99;
    CS_AppData.CfeCoreCSState = 99;
    
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, CFE_ES_PROCESSOR_RESET, 2);
    
    /* Set CDS return calls */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, CFE_ES_CDS_ALREADY_EXISTS, 1);
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_RESTOREFROMCDS_INDEX, CS_APP_TEST_CFE_ES_RestoreFromCDS_Hook);
    
    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    
    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_ENABLED, "CS_AppData.EepromCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.MemoryCSState == CS_STATE_ENABLED, "CS_AppData.MemoryCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.AppCSState    == CS_STATE_ENABLED, "CS_AppData.AppCSState    == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.TablesCSState == CS_STATE_ENABLED, "CS_AppData.TablesCSState == CS_STATE_ENABLED");

    UtAssert_True (CS_AppData.OSCSState      == CS_STATE_ENABLED, "CS_AppData.OSCSState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.CfeCoreCSState == CS_STATE_ENABLED, "CS_AppData.CfeCoreCSState == CS_STATE_ENABLED");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_CreateRestoreStatesFromCDS_Test_CDSSuccess */


void CS_CreateRestoreStatesFromCDS_Test_CDSFail(void)
{
    int32   Result;

    CS_AppData.EepromCSState = 99;
    CS_AppData.MemoryCSState = 99;
    CS_AppData.AppCSState    = 99;
    CS_AppData.TablesCSState = 99;
    CS_AppData.OSCSState     = 99;
    CS_AppData.CfeCoreCSState = 99;
    
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, CFE_ES_PROCESSOR_RESET, 2);
    

    /* Set CDS return calls */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, CFE_ES_CDS_ALREADY_EXISTS, 1);
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RESTOREFROMCDS_INDEX, -1, 1);
    
    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    
    UtAssert_True (CS_AppData.EepromCSState == CS_EEPROM_TBL_POWERON_STATE, "CS_AppData.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True (CS_AppData.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE, "CS_AppData.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True (CS_AppData.AppCSState    == CS_APPS_TBL_POWERON_STATE  , "CS_AppData.AppCSState    == CS_APPS_TBL_POWERON_STATE"  );
    UtAssert_True (CS_AppData.TablesCSState == CS_TABLES_TBL_POWERON_STATE, "CS_AppData.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True (CS_AppData.OSCSState      == CS_OSCS_CHECKSUM_STATE, "CS_AppData.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True (CS_AppData.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE, "CS_AppData.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_CDS_ERR_EID, CFE_EVS_ERROR, "Critical Data Store access error = 0xFFFFFFFF"),
        "Critical Data Store access error = 0xFFFFFFFF");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    

} /* end CS_AppInit_Test_ProcResetRestoreCDSFail */
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE) */

void CS_AppInit_Test_EVSRegisterError(void)
{
    int32 Result;

    /* Set CFE_EVS_Register to return -1 in order to reach call to CFE_ES_WriteToSysLog */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("CS App: Error Registering For Event Services, RC = 0xFFFFFFFF\n"),
        "CS App: Error Registering For Event Services, RC = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end CS_AppInit_Test_EVSRegisterError */

void CS_AppInit_Test_SBCreatePipeError(void)
{
    int32   Result;

    /* Set to generate error message CS_INIT_SB_CREATE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_SB_CREATE_ERR_EID, CFE_EVS_ERROR, "Software Bus Create Pipe for command returned: 0xFFFFFFFF"),
        "Software Bus Create Pipe for command returned: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_SBCreatePipeError */

void CS_AppInit_Test_SBSubscribeHKError(void)
{
    int32   Result;

    /* Set to generate error message CS_INIT_SB_SUBSCRIBE_HK_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_SB_SUBSCRIBE_HK_ERR_EID, CFE_EVS_ERROR, "Software Bus subscribe to housekeeping returned: 0xFFFFFFFF"),
        "Software Bus subscribe to housekeeping returned: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_SBSubscribeHKError */

void CS_AppInit_Test_SBSubscribeBackgroundCycleError(void)
{
    int32   Result;

    /* Set to generate error message CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID, CFE_EVS_ERROR, "Software Bus subscribe to background cycle returned: 0xFFFFFFFF"),
        "Software Bus subscribe to background cycle returned: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_SBSubscribeBackgroundCycleError */

void CS_AppInit_Test_SBSubscribeCmdError(void)
{
    int32   Result;

    /* Set to generate error message CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID, CFE_EVS_ERROR, "Software Bus subscribe to command returned: 0xFFFFFFFF"),
        "Software Bus subscribe to command returned: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppInit_Test_SBSubscribeCmdError */

void CS_AppInit_Test_TableInitErrorEEPROM(void)
{
    int32   Result;

    /* Set to generate error message CS_INIT_EEPROM_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_EEPROM_ERR_EID, CFE_EVS_ERROR, "Table initialization failed for Eeprom: 0xFFFFFFFF"),
        "Table initialization failed for Eeprom: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppInit_Test_TableInitErrorEEPROM */

void CS_AppInit_Test_TableInitErrorMemory(void)
{
    int32   Result;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to generate error message CS_INIT_SB_CREATE_ERR_EID.
     * Combining a SetReturnCode and a SetFunctionHook in order to return CFE_SUCCESS all runs except the one specified */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 2);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, CS_APP_TEST_CFE_TBL_LoadHook);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_MEMORY_ERR_EID, CFE_EVS_ERROR, "Table initialization failed for Memory: 0xFFFFFFFF"),
        "Table initialization failed for Memory: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppInit_Test_TableInitErrorMemory */

void CS_AppInit_Test_TableInitErrorApps(void)
{
    int32   Result;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to generate error message CS_INIT_APP_ERR_EID.
     * Combining a SetReturnCode and a SetFunctionHook in order to return CFE_SUCCESS all runs except the one specified */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 3);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, CS_APP_TEST_CFE_TBL_LoadHook);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_APP_ERR_EID, CFE_EVS_ERROR, "Table initialization failed for Apps: 0xFFFFFFFF"),
        "Table initialization failed for Apps: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppInit_Test_TableInitErrorApps */

void CS_AppInit_Test_TableInitErrorTables(void)
{
    int32   Result;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to generate error message CS_INIT_TABLES_ERR_EID.
     * Combining a SetReturnCode and a SetFunctionHook in order to return CFE_SUCCESS all runs except the one specified */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 4);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, CS_APP_TEST_CFE_TBL_LoadHook);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_TABLES_ERR_EID, CFE_EVS_ERROR, "Table initialization failed for Tables: 0xFFFFFFFF"),
        "Table initialization failed for Tables: 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppInit_Test_TableInitErrorTables */

void CS_AppInit_Test_TextSegmentInfoError(void)
{
    int32   Result;
    char    Message[125];

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to generate error message CS_OS_TEXT_SEG_INF_EID */
    Ut_CFE_PSP_MEMORY_SetReturnCode(UT_CFE_PSP_MEMORY_GETKERNELTEXTSEGMENTINFO_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_OS_TEXT_SEG_INF_EID, CFE_EVS_INFORMATION, "OS Text Segment disabled due to platform"),
        "OS Text Segment disabled due to platform");

    sprintf(Message, "CS Initialized. Version %d.%d.%d.%d", CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(CS_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_AppInit_Test_TextSegmentInfoError */

void CS_AppPipe_Test_TableUpdateErrors(void)
{
    int32           Result;
    CS_HkPacket_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_SEND_HK_MID, sizeof(CS_HkPacket_t), TRUE);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_UPDATE_EEPROM_ERR_EID, CFE_EVS_ERROR, "Table update failed for Eeprom: 0xCC000001, checksumming Eeprom is disabled"),
        "Table update failed for Eeprom: 0xCC000001, checksumming Eeprom is disabled");

    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_DISABLED, "CS_AppData.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_UPDATE_MEMORY_ERR_EID, CFE_EVS_ERROR, "Table update failed for Memory: 0xCC000001, checksumming Memory is disabled"),
        "Table update failed for Memory: 0xCC000001, checksumming Memory is disabled");

    UtAssert_True (CS_AppData.MemoryCSState == CS_STATE_DISABLED, "CS_AppData.MemoryCSState == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_UPDATE_APP_ERR_EID, CFE_EVS_ERROR, "Table update failed for Apps: 0xCC000001, checksumming Apps is disabled"),
        "Table update failed for Apps: 0xCC000001, checksumming Apps is disabled");

    UtAssert_True (CS_AppData.AppCSState == CS_STATE_DISABLED, "CS_AppData.AppCSState == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_UPDATE_TABLES_ERR_EID, CFE_EVS_ERROR, "Table update failed for Tables: 0xCC000001, checksumming Tables is disabled"),
        "Table update failed for Tables: 0xCC000001, checksumming Tables is disabled");

    UtAssert_True (CS_AppData.TablesCSState == CS_STATE_DISABLED, "CS_AppData.TablesCSState == CS_STATE_DISABLED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 9, "Ut_CFE_EVS_GetEventQueueDepth() == 9");
    /* Generates 5 event messages we don't care about in this test */

} /* end CS_AppPipe_Test_TableUpdateErrors */

void CS_AppPipe_Test_BackgroundCycle(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_BACKGROUND_CYCLE_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_AppPipe_Test_BackgroundCycle */

void CS_AppPipe_Test_NoopCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_NOOP_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_NoopCmd */

void CS_AppPipe_Test_ResetCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RESET_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ResetCmd */

void CS_AppPipe_Test_OneShotCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ONESHOT_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_OneShotCmd */

void CS_AppPipe_Test_CancelOneShotCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_CANCEL_ONESHOT_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_CancelOneShotCmd */

void CS_AppPipe_Test_EnableAllCSCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_ALL_CS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableAllCSCmd */

void CS_AppPipe_Test_DisableAllCSCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_ALL_CS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableAllCSCmd */

void CS_AppPipe_Test_EnableCfeCoreCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_CFECORE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableCfeCoreCmd */

void CS_AppPipe_Test_DisableCfeCoreCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_CFECORE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableCfeCoreCmd */

void CS_AppPipe_Test_ReportBaselineCfeCoreCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_REPORT_BASELINE_CFECORE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ReportBaselineCfeCoreCmd */

void CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RECOMPUTE_BASELINE_CFECORE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd */

void CS_AppPipe_Test_EnableOSCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_OS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableOSCmd */

void CS_AppPipe_Test_DisableOSCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_OS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableOSCmd */

void CS_AppPipe_Test_ReportBaselineOSCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_REPORT_BASELINE_OS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ReportBaselineOSCmd */

void CS_AppPipe_Test_RecomputeBaselineOSCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RECOMPUTE_BASELINE_OS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_RecomputeBaselineOSCmd */

void CS_AppPipe_Test_EnableEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableEepromCmd */

void CS_AppPipe_Test_DisableEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableEepromCmd */

void CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_REPORT_BASELINE_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd */

void CS_AppPipe_Test_RecomputeBaselineEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RECOMPUTE_BASELINE_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_RecomputeBaselineEepromCmd */

void CS_AppPipe_Test_EnableEntryIDEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_ENTRY_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableEntryIDEepromCmd */

void CS_AppPipe_Test_DisableEntryIDEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_ENTRY_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableEntryIDEepromCmd */

void CS_AppPipe_Test_GetEntryIDEepromCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_GET_ENTRY_ID_EEPROM_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_GetEntryIDEepromCmd */

void CS_AppPipe_Test_EnableMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableMemoryCmd */

void CS_AppPipe_Test_DisableMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableMemoryCmd */

void CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_REPORT_BASELINE_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd */

void CS_AppPipe_Test_RecomputeBaselineMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RECOMPUTE_BASELINE_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_RecomputeBaselineMemoryCmd */

void CS_AppPipe_Test_EnableEntryIDMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_ENTRY_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableEntryIDMemoryCmd */

void CS_AppPipe_Test_DisableEntryIDMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_ENTRY_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableEntryIDMemoryCmd */

void CS_AppPipe_Test_GetEntryIDMemoryCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_GET_ENTRY_ID_MEMORY_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_GetEntryIDMemoryCmd */

void CS_AppPipe_Test_EnableTablesCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_TABLES_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableTablesCmd */

void CS_AppPipe_Test_DisableTablesCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_TABLES_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableTablesCmd */

void CS_AppPipe_Test_ReportBaselineTablesCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_REPORT_BASELINE_TABLE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ReportBaselineTablesCmd */

void CS_AppPipe_Test_RecomputeBaselineTablesCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RECOMPUTE_BASELINE_TABLE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_RecomputeBaselineTablesCmd */

void CS_AppPipe_Test_EnableNameTablesCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_NAME_TABLE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableNameTablesCmd */

void CS_AppPipe_Test_DisableNameTablesCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_NAME_TABLE_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableNameTablesCmd */

void CS_AppPipe_Test_EnableAppCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_APPS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableAppCmd */

void CS_AppPipe_Test_DisableAppCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_APPS_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableAppCmd */

void CS_AppPipe_Test_ReportBaselineAppCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_REPORT_BASELINE_APP_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_ReportBaselineAppCmd */

void CS_AppPipe_Test_RecomputeBaselineAppCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_RECOMPUTE_BASELINE_APP_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_RecomputeBaselineAppCmd */

void CS_AppPipe_Test_EnableNameAppCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_ENABLE_NAME_APP_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_EnableNameAppCmd */

void CS_AppPipe_Test_DisableNameAppCmd(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, CS_DISABLE_NAME_APP_CC);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_AppPipe_Test_DisableNameAppCmd */

void CS_AppPipe_Test_InvalidCCError(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_CC1_ERR_EID, CFE_EVS_ERROR, "Invalid ground command code: ID = 0x189F, CC = 99"),
        "Invalid ground command code: ID = 0x189F, CC = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppPipe_Test_InvalidCCError */

void CS_AppPipe_Test_InvalidMIDError(void)
{
    int32            Result;
    CS_NoArgsCmd_t   CmdPacket;

    CS_AppData.ChildTaskTable = -1;

    CFE_SB_InitMsg (&CmdPacket, 0x0099, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    Result = CS_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_MID_ERR_EID, CFE_EVS_ERROR, "Invalid command pipe message ID: 0x0099"),
        "Invalid command pipe message ID: 0x0099");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_AppPipe_Test_InvalidMIDError */

void CS_HousekeepingCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_SEND_HK_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.CmdCounter           = 1;
    CS_AppData.CmdErrCounter        = 2;
    CS_AppData.ChecksumState        = 3;
    CS_AppData.EepromCSState        = 4;
    CS_AppData.MemoryCSState        = 5;
    CS_AppData.AppCSState           = 6;
    CS_AppData.TablesCSState        = 7;
    CS_AppData.OSCSState            = 8;
    CS_AppData.CfeCoreCSState       = 9;
    CS_AppData.RecomputeInProgress  = 10;
    CS_AppData.OneShotInProgress    = 11;
    CS_AppData.EepromCSErrCounter   = 12;
    CS_AppData.MemoryCSErrCounter   = 13;
    CS_AppData.AppCSErrCounter      = 14;
    CS_AppData.TablesCSErrCounter   = 15;
    CS_AppData.CfeCoreCSErrCounter  = 16;
    CS_AppData.OSCSErrCounter       = 17;
    CS_AppData.CurrentCSTable       = 18;
    CS_AppData.CurrentEntryInTable  = 19;
    CS_AppData.EepromBaseline       = 20;
    CS_AppData.OSBaseline           = 21;
    CS_AppData.CfeCoreBaseline      = 22;
    CS_AppData.LastOneShotAddress   = 23;
    CS_AppData.LastOneShotSize      = 24;
    CS_AppData.LastOneShotChecksum  = 25;
    CS_AppData.PassCounter          = 26;

    /* Execute the function being tested */
    CS_HousekeepingCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");
    UtAssert_True (CS_AppData.HkPacket.CmdErrCounter == 2, "CS_AppData.HkPacket.CmdErrCounter == 2");
    UtAssert_True (CS_AppData.HkPacket.ChecksumState == 3, "CS_AppData.HkPacket.ChecksumState == 3");
    UtAssert_True (CS_AppData.HkPacket.EepromCSState == 4, "CS_AppData.HkPacket.EepromCSState == 4");
    UtAssert_True (CS_AppData.HkPacket.MemoryCSState == 5, "CS_AppData.HkPacket.MemoryCSState == 5");
    UtAssert_True (CS_AppData.HkPacket.AppCSState == 6, "CS_AppData.HkPacket.AppCSState == 6");
    UtAssert_True (CS_AppData.HkPacket.TablesCSState == 7, "CS_AppData.HkPacket.TablesCSState == 7");
    UtAssert_True (CS_AppData.HkPacket.OSCSState == 8, "CS_AppData.HkPacket.OSCSState == 8");
    UtAssert_True (CS_AppData.HkPacket.CfeCoreCSState == 9, "CS_AppData.HkPacket.CfeCoreCSState == 9");
    UtAssert_True (CS_AppData.HkPacket.RecomputeInProgress == 10, "CS_AppData.HkPacket.ChildTaskInUse == 10");
    UtAssert_True (CS_AppData.HkPacket.OneShotInProgress == 11, "CS_AppData.HkPacket.OneShotInProgress == 11");
    UtAssert_True (CS_AppData.HkPacket.EepromCSErrCounter == 12, "CS_AppData.HkPacket.EepromCSErrCounter == 12");
    UtAssert_True (CS_AppData.HkPacket.MemoryCSErrCounter == 13, "CS_AppData.HkPacket.MemoryCSErrCounter == 13");
    UtAssert_True (CS_AppData.HkPacket.AppCSErrCounter == 14, "CS_AppData.HkPacket.AppCSErrCounter == 14");
    UtAssert_True (CS_AppData.HkPacket.TablesCSErrCounter == 15, "CS_AppData.HkPacket.TablesCSErrCounter == 15");
    UtAssert_True (CS_AppData.HkPacket.CfeCoreCSErrCounter == 16, "CS_AppData.HkPacket.CfeCoreCSErrCounter == 16");
    UtAssert_True (CS_AppData.HkPacket.OSCSErrCounter == 17, "CS_AppData.HkPacket.OSCSErrCounter == 17");
    UtAssert_True (CS_AppData.HkPacket.CurrentCSTable == 18, "CS_AppData.HkPacket.CurrentCSTable == 18");
    UtAssert_True (CS_AppData.HkPacket.CurrentEntryInTable == 19, "CS_AppData.HkPacket.CurrentEntryInTable == 19");
    UtAssert_True (CS_AppData.HkPacket.EepromBaseline == 20, "CS_AppData.HkPacket.EepromBaseline == 20");
    UtAssert_True (CS_AppData.HkPacket.OSBaseline == 21, "CS_AppData.HkPacket.OSBaseline == 21");
    UtAssert_True (CS_AppData.HkPacket.CfeCoreBaseline == 22, "CS_AppData.HkPacket.CfeCoreBaseline == 22");
    UtAssert_True (CS_AppData.HkPacket.LastOneShotAddress == 23, "CS_AppData.HkPacket.LastOneShotAddress == 23");
    UtAssert_True (CS_AppData.HkPacket.LastOneShotSize == 24, "CS_AppData.HkPacket.LastOneShotSize == 24");
    UtAssert_True (CS_AppData.HkPacket.LastOneShotChecksum == 25, "CS_AppData.HkPacket.LastOneShotChecksum == 25");
    UtAssert_True (CS_AppData.HkPacket.PassCounter == 26, "CS_AppData.HkPacket.PassCounter == 26");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_HousekeepingCmd_Test_Nominal */

void CS_HousekeepingCmd_Test_InvalidMsgLength(void)
{
    CS_HkPacket_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_SEND_HK_MID, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    CS_HousekeepingCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x18A0, CC = 99, Len = 1, Expected = 8"),
        "Invalid msg length: ID = 0x18A0, CC = 99, Len = 1, Expected = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_HousekeepingCmd_Test_InvalidMsgLength */

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)

void CS_UpdateCDS_Test_Nominal(void)
{
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, CFE_SUCCESS, 1);
    CS_AppData.DataStoreHandle = 0x01;
        
    /* Execute the function being tested */
    CS_UpdateCDS();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_UpdateCDS_Test_Nominal */

void CS_UpdateCDS_Test_CopyToCDSFail(void)
{
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, -1, 1);
    CS_AppData.DataStoreHandle = 0x01;
        
    /* Execute the function being tested */
    CS_UpdateCDS();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_INIT_CDS_ERR_EID, CFE_EVS_ERROR, "Critical Data Store access error = 0xFFFFFFFF"),
        "Critical Data Store access error = 0xFFFFFFFF");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_UpdateCDS_Test_CopyToCDSFail */

void CS_UpdateCDS_Test_NullCDSHandle(void)
{
    CS_AppData.DataStoreHandle = 0;
    
    /* Execute the function being tested */
    CS_UpdateCDS();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_UpdateCDS_Test_NullCDSHandle */
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE) */


void CS_App_Test_AddTestCases(void)
{
    
    UtTest_Add(CS_App_TestCmdTlmAlign, CS_Test_Setup, CS_Test_TearDown, "CS_App_TestCmdTlmAlign");

    UtTest_Add(CS_AppMain_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_Nominal");
    UtTest_Add(CS_AppMain_Test_RegisterAppError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_RegisterAppError");
    UtTest_Add(CS_AppMain_Test_AppInitError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_AppInitError");
    UtTest_Add(CS_AppMain_Test_RcvMsgError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_RcvMsgError");
    UtTest_Add(CS_AppMain_Test_AppPipeError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_AppPipeError");

    UtTest_Add(CS_AppInit_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_Nominal");
    UtTest_Add(CS_AppInit_Test_EVSRegisterError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_EVSRegisterError");
    UtTest_Add(CS_AppInit_Test_SBCreatePipeError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_SBCreatePipeError");
    UtTest_Add(CS_AppInit_Test_SBSubscribeHKError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_SBSubscribeHKError");
    UtTest_Add(CS_AppInit_Test_SBSubscribeBackgroundCycleError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_SBSubscribeBackgroundCycleError");
    UtTest_Add(CS_AppInit_Test_SBSubscribeCmdError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_SBSubscribeCmdError");
    UtTest_Add(CS_AppInit_Test_TableInitErrorEEPROM, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_TableInitErrorEEPROM");
    UtTest_Add(CS_AppInit_Test_TableInitErrorMemory, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_TableInitErrorMemory");
     UtTest_Add(CS_AppInit_Test_TableInitErrorApps, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_TableInitErrorApps");
    UtTest_Add(CS_AppInit_Test_TableInitErrorTables, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_TableInitErrorTables");
    UtTest_Add(CS_AppInit_Test_TextSegmentInfoError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_TextSegmentInfoError");

    UtTest_Add(CS_AppPipe_Test_TableUpdateErrors, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_TableUpdateErrors");
    UtTest_Add(CS_AppPipe_Test_BackgroundCycle, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_BackgroundCycle");
    UtTest_Add(CS_AppPipe_Test_NoopCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_NoopCmd");
    UtTest_Add(CS_AppPipe_Test_ResetCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ResetCmd");
    UtTest_Add(CS_AppPipe_Test_OneShotCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_OneShotCmd");
    UtTest_Add(CS_AppPipe_Test_CancelOneShotCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_CancelOneShotCmd");
    UtTest_Add(CS_AppPipe_Test_EnableAllCSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableAllCSCmd");
    UtTest_Add(CS_AppPipe_Test_DisableAllCSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableAllCSCmd");
    UtTest_Add(CS_AppPipe_Test_EnableCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_DisableCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ReportBaselineCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_EnableOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableOSCmd");
    UtTest_Add(CS_AppPipe_Test_DisableOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableOSCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ReportBaselineOSCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_RecomputeBaselineOSCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableEepromCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableEepromCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_RecomputeBaselineEepromCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_GetEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_GetEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_EnableMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_DisableMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_RecomputeBaselineMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_GetEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_GetEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_EnableTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableTablesCmd");
    UtTest_Add(CS_AppPipe_Test_DisableTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableTablesCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ReportBaselineTablesCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_RecomputeBaselineTablesCmd");
    UtTest_Add(CS_AppPipe_Test_EnableNameTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableNameTablesCmd");
    UtTest_Add(CS_AppPipe_Test_DisableNameTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableNameTablesCmd");
    UtTest_Add(CS_AppPipe_Test_EnableAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableAppCmd");
    UtTest_Add(CS_AppPipe_Test_DisableAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableAppCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ReportBaselineAppCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_RecomputeBaselineAppCmd");
    UtTest_Add(CS_AppPipe_Test_EnableNameAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableNameAppCmd");
    UtTest_Add(CS_AppPipe_Test_DisableNameAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableNameAppCmd");
    UtTest_Add(CS_AppPipe_Test_InvalidCCError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_InvalidCCError");
    UtTest_Add(CS_AppPipe_Test_InvalidMIDError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_InvalidMIDError");

    UtTest_Add(CS_HousekeepingCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_HousekeepingCmd_Test_Nominal");
    UtTest_Add(CS_HousekeepingCmd_Test_InvalidMsgLength, CS_Test_Setup, CS_Test_TearDown, "CS_HousekeepingCmd_Test_InvalidMsgLength");
    
    UtTest_Add(CS_AppInit_Test_NominalPowerOnReset, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_NominalPowerOnReset");
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
    UtTest_Add(CS_AppInit_Test_NominalProcReset, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_NominalProcReset");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS, CS_Test_Setup, CS_Test_TearDown, "CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CDSSuccess, CS_Test_Setup, CS_Test_TearDown, "CS_CreateRestoreStatesFromCDS_Test_CDSSuccess");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CDSFail, CS_Test_Setup, CS_Test_TearDown, "CS_CreateRestoreStatesFromCDS_Test_CDSFail");
    
    UtTest_Add(CS_UpdateCDS_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_Nominal");
    UtTest_Add(CS_UpdateCDS_Test_CopyToCDSFail, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_CopyToCDSFail");
    UtTest_Add(CS_UpdateCDS_Test_NullCDSHandle, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_NullCDSHandle");
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE) */

} /* end CS_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
