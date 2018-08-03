/*
 * Filename: ci_testcase.c
 *
 * Purpose: This file contains unit test cases for the ci application
 * 
 */


/*
 * Includes
 */
#include "cfe.h"
#include "ci_app.h"

#include "utassert.h"
#include "uttest.h"
#include "utlist.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"
#include "ut_cfe_es_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include <errno.h>

#include "ci_stubs.h"


extern CI_AppData_t  g_CI_AppData;


/* ---------------------  Begin test cases  --------------------------------- */

/*******************************************************************************
**
**  CI_InitEvent Tests
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_InitEvent_RegisterFail(void)
{
    /* Setup Inputs */
    int32 expected = CI_ERROR;
    int32 actual = 0;
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, 
                            CFE_EVS_UNKNOWN_FILTER, 1);

    /* Execute Test */
    actual = CI_AppInit();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "InitEvent - Event Register Fail");
}

void Test_CI_InitEvent(void)
{
    /* Setup Inputs */
    int32 expected = CFE_SUCCESS;
    int32 actual = 0;

    /* Execute Test */
    actual = CI_InitEvent();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "InitEvent - Nominal");
}


/*******************************************************************************
**
**  CI_InitPipe Tests
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_InitPipe_CreatePipeFail(void)
{
    /* Setup Inputs */
    int32 expected = CI_ERROR;
    int32 actual = 0;
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, 
                            CFE_SB_BAD_ARGUMENT, 1);

    /* Execute Test */
    actual = CI_AppInit();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "InitPipe - SCH CreatePipe Fail");

    expected = CFE_SB_BAD_ARGUMENT;
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, 
                            CFE_SB_BAD_ARGUMENT, 2);
    
    /* Execute Test */
    actual = CI_InitPipe();
    
    /* Verify Outputs */
    UtAssert_True(actual == expected, "InitPipe - CMD CreatePipe Fail");
    UtAssert_True(g_CI_AppData.usCmdPipeDepth == CI_CMD_PIPE_DEPTH, 
                  "Side effect test.");
}

void Test_CI_InitPipe(void)
{
    /* Setup Inputs */
    int32 expected = CFE_SUCCESS; 
    int32 actual = 0;

    /* Execute Test */
    actual = CI_InitPipe();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "Init Pipe - Nominal");
}

/*******************************************************************************
**
**  CI_InitData Tests
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_InitData(void)
{
    /* Setup Inputs */
    int32 expected = CI_SUCCESS;
    int32 actual = 0;

    /* Execute Test */
    actual = CI_InitData();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "InitData - Nominal");
    UtAssert_True(g_CI_AppData.uiWakeupTimeout == CI_WAKEUP_TIMEOUT,
                  "Side effect test.");
}


/*******************************************************************************
**
**  CI_InitCustom Tests
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_CustomInit_Fail(void)
{
    /* Setup Inputs */
    int32 expected = CI_ERROR;
    int32 actual = 0;

    Ut_CI_SetReturnCode(UT_CI_CUSTOMINIT_INDEX, 
                        CI_ERROR, 1);

    /* Execute Test */
    actual = CI_CustomInit();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "CustomInit - Fail");
}

/*******************************************************************************
**
**  CI_AppInit Tests
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_AppInit(void)
{
    /* Setup Inputs */
    int32 expected = CI_SUCCESS;
    int32 actual = 0;

    /* Execute Test */
    actual = CI_AppInit();

    /* Verify Outputs */
    UtAssert_True(actual == expected, "AppInit - Nominal");
}


/*******************************************************************************
**
**  CI_AppMain Tests
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_AppMain_RegisterFail(void)
{
    g_CI_AppData.uiWakeupTimeout = 0;
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, 
                            -1, 1);
    
    /* Execute Test */
    CI_AppMain();

    UtAssert_True(g_CI_AppData.uiWakeupTimeout == 0,
                  "AppMain - RegisterApp Fail");
}


void Test_CI_AppMain_InitFail(void)
{
    Ut_CI_SetReturnCode(UT_CI_CUSTOMINIT_INDEX, 
                        CI_ERROR, 1);

    CI_AppMain();
    
    UtAssert_True(g_CI_AppData.uiWakeupTimeout == CI_WAKEUP_TIMEOUT,
                  "AppMain - AppInit Fail");
    
}

void Test_CI_AppMain_RcvMsgFail(void)
{
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, 
                            CFE_SB_BAD_ARGUMENT, 1);
    
    CI_AppMain();
    
    UtAssert_True(g_CI_AppData.uiRunStatus == CFE_ES_APP_ERROR,
                  "AppMain - RcvMsg Fail");

    /* For code coverage */
    CI_CleanupCallback();                  
}


/*******************************************************************************
**
** CI_RcvMsg Test
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_RcvMsg_NoMsgError(void)
{
    int32 actual;
    int32 expected = CFE_SB_NO_MESSAGE;

    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    actual = CI_RcvMsg(CFE_SB_POLL);
    UtAssert_True(actual == expected, "RcvMsg - NoMsgError");
}

void Test_CI_RcvMsg_BadMsg(void)
{
    int32 actual;
    int32 expected = CFE_SUCCESS;
    CFE_SB_Msg_t msg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &msg;

    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    CFE_SB_SetMsgId(pMsg, 0);
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(msg));         
    Ut_CFE_SB_AddMsgToPipe(pMsg, g_CI_AppData.SchPipeId);

    actual = CI_RcvMsg(CFE_SB_PEND_FOREVER);
    UtAssert_True(actual == expected, "RcvMsg - Bad MID");
}

void Test_CI_RcvMsg_Wakeup(void)
{
    int32 actual;
    int32 expected = CFE_SUCCESS;
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;

    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    CFE_SB_SetMsgId(pMsg, CI_WAKEUP_MID);
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(cmdMsg));         
    Ut_CFE_SB_AddMsgToPipe(pMsg, g_CI_AppData.SchPipeId);

    actual = CI_RcvMsg(CFE_SB_PEND_FOREVER);
    UtAssert_True(actual == expected, "RcvMsg - Wakeup MID");
}

void Test_CI_RcvMsg_Timeout(void)
{
    int32 actual;
    int32 expected = CFE_SB_TIME_OUT;
    
    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    actual = CI_RcvMsg(10);
    UtAssert_True(actual == expected, "RcvMsg - timeout");
}
    

/*******************************************************************************
**
** CI_ProcessNewCmds Test 
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_ProcessNewCmds_BadMsg(void)
{
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;

    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    /* Send a Bad Command */
    CFE_SB_SetMsgId(pMsg, 0);
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(CI_NoArgCmd_t));         
    Ut_CFE_SB_AddMsgToPipe(pMsg, g_CI_AppData.CmdPipeId);

    CI_ProcessNewCmds();
    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 1,
                  "ProcessNewCmds - Bad Msg");
}


void Test_CI_ProcessNewCmds_AppCmd(void)
{
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;

    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    /* Send Noop Cmd Command */
    CFE_SB_SetMsgId(pMsg, CI_APP_CMD_MID);
    CFE_SB_SetCmdCode(pMsg, CI_NOOP_CC);
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(CI_NoArgCmd_t));                      

    Ut_CFE_SB_AddMsgToPipe(pMsg, g_CI_AppData.CmdPipeId);

    CI_ProcessNewCmds();
    UtAssert_True(g_CI_AppData.HkTlm.usCmdCnt == 1,
                  "ProcessNewCmds - AppCmd Msg");
}
    

void Test_CI_ProcessNewCmds_SendHk(void)
{
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;

    /* Initialize the Command pipe and subscribe to messages */
    CI_InitPipe();

    CFE_SB_SetMsgId(pMsg, CI_SEND_HK_MID);
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(CI_NoArgCmd_t));                      

    Ut_CFE_SB_AddMsgToPipe(pMsg, g_CI_AppData.CmdPipeId);

    CI_ProcessNewCmds();
    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 0,
                  "ProcessNewCmds - SendHk Msg");
}




/*******************************************************************************
**
** CI_ProcessNewAppCmds Test 
**
*******************************************************************************/

/*----------------------------------------------------------------------------*/
void Test_CI_ProcessNewAppCmds_Noop(void)
{
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;
    
    CFE_SB_SetMsgId(pMsg, CI_APP_CMD_MID);
    CFE_SB_SetCmdCode(pMsg, CI_NOOP_CC);
    CFE_SB_SetTotalMsgLength(pMsg, 20);                      

    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);
   
    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 1,
                  "ProcessNewAppCmds - NOOP_CC - Invalid Len.");
    
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(CI_NoArgCmd_t));
    
    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);

    UtAssert_True(g_CI_AppData.HkTlm.usCmdCnt == 1,
                  "ProcessNewAppCmds - NOOP_CC");
}


void Test_CI_ProcessNewAppCmds_Reset(void)
{
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;
    
    CFE_SB_SetMsgId(pMsg, CI_APP_CMD_MID);
    CFE_SB_SetCmdCode(pMsg, CI_RESET_CC);
    CFE_SB_SetTotalMsgLength(pMsg, 20);                      

    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);
   
    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 1,
                  "ProcessNewAppCmds - RESET_CC - Invalid Len.");
    
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(CI_NoArgCmd_t));
    
    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);

    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 0,
                  "ProcessNewAppCmds - RESET_CC");
}


void Test_CI_ProcessNewAppCmds_EnableTO(void)
{
    CI_EnableTOCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;
    
    CFE_SB_SetMsgId(pMsg, CI_APP_CMD_MID);
    CFE_SB_SetCmdCode(pMsg, CI_ENABLE_TO_CC);
    CFE_SB_SetTotalMsgLength(pMsg, 20);                      

    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);
   
    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 1,
                  "ProcessNewAppCmds - ENABLE_TO_CC - Invalid Len.");
    
    CFE_SB_SetTotalMsgLength(pMsg, sizeof(CI_EnableTOCmd_t));
    
    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);

    UtAssert_True(g_CI_AppData.HkTlm.usCmdCnt == 1,
                  "ProcessNewAppCmds - ENABLE_TO_CC");
}


void Test_CI_ProcessNewAppCmds_Custom(void)
{
    CI_NoArgCmd_t cmdMsg;
    CFE_SB_MsgPtr_t pMsg = (CFE_SB_MsgPtr_t) &cmdMsg;
    
    CFE_SB_SetMsgId(pMsg, CI_APP_CMD_MID);
    CFE_SB_SetCmdCode(pMsg, 10);

    Ut_CI_SetReturnCode(UT_CI_CUSTOMAPPCMDS_INDEX, 
                        CI_ERROR, 1);
    
    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);
   
    UtAssert_True(g_CI_AppData.HkTlm.usCmdErrCnt == 1,
                  "ProcessNewAppCmds - Invalid Custom Cmd ID");
    
    /* Execute test */
    CI_ProcessNewAppCmds(pMsg);

    UtAssert_True(g_CI_AppData.HkTlm.usCmdCnt == 1,
                  "ProcessNewAppCmds - Valid Custom Cmd ID");
}


/* ------------------- End of test cases --------------------------------------*/



/*
 * CI_Setup
 *
 * Purpose:
 *   Called by the unit test tool to set up the app prior to each test
 */
void CI_Setup(void)
{  
    Ut_OSAPI_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_CFE_EVS_Reset();
    Ut_CFE_TBL_Reset();
}

/*
 * CI_TearDown
 *
 * Purpose:
 *   Called by the unit test tool to tear down the app after each test
 */
void CI_TearDown(void)
{
    CFE_PSP_MemSet((void*)&g_CI_AppData.HkTlm, 0x00, 
                   sizeof(g_CI_AppData.HkTlm));
}


/* CI_AddTestCase
 *
 * Purpose:
 *   Registers the test cases to execute with the unit test tool
 */
void CI_AddTestCase(void)
{
    /* CI_AppInit Tests */
    UtTest_Add(Test_CI_InitEvent_RegisterFail,  CI_Setup, CI_TearDown,
               "Test_CI_InitEvent_RegisterFail");
    UtTest_Add(Test_CI_InitEvent,  CI_Setup, CI_TearDown,
               "Test_CI_InitEvent");
    UtTest_Add(Test_CI_InitPipe_CreatePipeFail,  CI_Setup, CI_TearDown,
               "Test_CI_InitPipe_CreatePipeFail");
    UtTest_Add(Test_CI_InitPipe,  CI_Setup, CI_TearDown,
               "Test_CI_InitPipe");
    UtTest_Add(Test_CI_InitData,  CI_Setup, CI_TearDown,
               "Test_CI_InitData");
    UtTest_Add(Test_CI_CustomInit_Fail,  CI_Setup, CI_TearDown,
               "Test_CI_CustomInit_Fail");
    UtTest_Add(Test_CI_AppInit,  CI_Setup, CI_TearDown,
               "Test_CI_AppInit");

    /* CI_AppMain */
    UtTest_Add(Test_CI_AppMain_RegisterFail, CI_Setup, CI_TearDown,
               "Test_CI_AppMain_Registerfail");
    UtTest_Add(Test_CI_AppMain_InitFail, CI_Setup, CI_TearDown,
               "Test_CI_AppMain_Initfail");
    UtTest_Add(Test_CI_AppMain_RcvMsgFail, CI_Setup, CI_TearDown,
               "Test_CI_AppMain_RcvMsgfail");

    /* CI_RcvMsg */
    UtTest_Add(Test_CI_RcvMsg_NoMsgError,  CI_Setup, CI_TearDown,
              "Test_CI_RcvMsg_NoMsgError");
    UtTest_Add(Test_CI_RcvMsg_BadMsg,  CI_Setup, CI_TearDown,
              "Test_CI_RcvMsg_BadMsg");
    UtTest_Add(Test_CI_RcvMsg_Wakeup,  CI_Setup, CI_TearDown,
              "Test_CI_RcvMsg_Wakeup");
    UtTest_Add(Test_CI_RcvMsg_Timeout,  CI_Setup, CI_TearDown,
              "Test_CI_RcvMsg_Timeout");

    /* CI_ProcessNewCmds */
    UtTest_Add(Test_CI_ProcessNewCmds_BadMsg,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewCmds_BadMsg");
    UtTest_Add(Test_CI_ProcessNewCmds_AppCmd,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewCmds_AppCmd");
    UtTest_Add(Test_CI_ProcessNewCmds_SendHk,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewCmds_SendHk");

    /* CI_ProcessNewAppCmds */
    UtTest_Add(Test_CI_ProcessNewAppCmds_Noop,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewAppCmds_Noop");
    UtTest_Add(Test_CI_ProcessNewAppCmds_Reset,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewAppCmds_Reset");
    UtTest_Add(Test_CI_ProcessNewAppCmds_EnableTO,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewAppCmds_EnableTO");
    UtTest_Add(Test_CI_ProcessNewAppCmds_Custom,  CI_Setup, CI_TearDown,
               "Test_CI_ProcessNewAppCmds_Custom");
    

}


  

