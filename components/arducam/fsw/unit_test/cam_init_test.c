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

#include "cam_init_test.h"
#include "cam_test_utils.h"

#include <cam_app.h>

#include <uttest.h>
#include <utassert.h>
#include <ut_cfe_es_stubs.h>
#include <ut_cfe_sb_stubs.h>
#include <ut_osapi_stubs.h>
#include <ut_ostimerapi_stubs.h>

#include <stdio.h>

extern void cam_Main(void);

/* test init - nominal */
static void CAM_Init_Test_Nominal(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    CAM_AppInit();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 0, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 0, "cam cmd error count");
}

/* test init - register app error */
static void CAM_Init_Test_RegisterError(void)
{
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, CFE_ES_ERR_APP_REGISTER, 1);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
}

/* test init - create pipe error */
static void CAM_Init_Test_PipeError(void)
{
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, CFE_SB_PIPE_RD_ERR, 1);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
}

/* test init - subscribe error */
static void CAM_Init_Test_SubscribeError(void)
{
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, CFE_SB_INTERNAL_ERR, 1);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, CFE_SB_INTERNAL_ERR, 2);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
}

/* test init - create mutex error */
static void CAM_Init_Test_MutexError(void)
{
    Ut_OSAPI_SetReturnCode(UT_OSAPI_MUTSEMCREATE_INDEX, OS_ERROR, 1);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
}

/* test init - create semaphore error */
static void CAM_Init_Test_BinSemError(void)
{
    Ut_OSAPI_SetReturnCode(UT_OSAPI_BINSEMCREATE_INDEX, OS_ERROR, 1);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
}

/* test init - create child task error */
static void CAM_Init_Test_ChildTaskError(void)
{
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, CFE_ES_ERR_CHILD_TASK_CREATE, 1);
    cam_Main();
    UtAssert_True(CAM_AppData.RunStatus == CFE_ES_APP_ERROR, "cam run status");
}

void CAM_Init_Test_AddTestCases(void)
{
    UtTest_Add(CAM_Init_Test_Nominal, CAM_Test_Setup, CAM_Test_TearDown,
               "cam init: nominal");
    //UtTest_Add(CAM_Init_Test_RegisterError, CAM_Test_Setup, CAM_Test_TearDown,
    //           "cam init: app reg error");
    UtTest_Add(CAM_Init_Test_PipeError, CAM_Test_Setup, CAM_Test_TearDown,
               "cam init: pipe error");
    UtTest_Add(CAM_Init_Test_SubscribeError, CAM_Test_Setup, CAM_Test_TearDown,
               "cam init: subscribe error");
    UtTest_Add(CAM_Init_Test_MutexError, CAM_Test_Setup, CAM_Test_TearDown,
               "cam init: mutex error");
    UtTest_Add(CAM_Init_Test_BinSemError, CAM_Test_Setup, CAM_Test_TearDown,
               "cam init: bin sem error");
    UtTest_Add(CAM_Init_Test_ChildTaskError, CAM_Test_Setup, CAM_Test_TearDown,
               "cam init: child task error");
}
