 /*************************************************************************
 ** File:
 **   $Id: example_cmds_test.c 1.1 2016/06/14 16:47:22EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit tests for example_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: example_cmds_test.c  $
 **   Revision 1.1 2016/06/14 16:47:22EDT czogby 
 **   Initial revision
 **   Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/unit_test/project.pj
 *************************************************************************/

#include "example_cmds_test.h"
#include "example_cmds.h"
#include "example_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"

int32 Example_Cmds_Test_UT_CFE_SB_SubscribeHook1(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                                                CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    return 5;
}

int32 Example_Cmds_Test_UT_CFE_SB_SubscribeHook2(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                                                CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    return 6;
}

void Example_Function2_Test_Case1(void)
{
    int32 Result;

    Example_GlobalData.Variable3 = 3;

    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SUBSCRIBE_INDEX, &Example_Cmds_Test_UT_CFE_SB_SubscribeHook1);

    /* Execute the function being tested */
    Result = Example_Function2();
    
    /* Verify results */
    UtAssert_True (Example_GlobalData.Variable4 == 4, "Example_GlobalData.Variable4 == 4");
    UtAssert_True (Result == 25, "Result == 25");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end Example_Function2_Test_Case1 */

void Example_Function2_Test_Case2(void)
{
    int32 Result;

    Example_GlobalData.Variable3 = 3;

    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SUBSCRIBE_INDEX, &Example_Cmds_Test_UT_CFE_SB_SubscribeHook2);

    /* Execute the function being tested */
    Result = Example_Function2();
    
    /* Verify results */
    UtAssert_True (Example_GlobalData.Variable4 == 4, "Example_GlobalData.Variable4 == 4");
    UtAssert_True (Result == 10, "Result == 10");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end Example_Function2_Test_Case2 */

void Example_Function2_Test_Case3(void)
{
    int32 Result;

    Example_GlobalData.Variable3 = 55;

    /* Execute the function being tested */
    Result = Example_Function2();
    
    /* Verify results */
    UtAssert_True (Example_GlobalData.Variable4 == 99, "Example_GlobalData.Variable4 == 99");
    UtAssert_True (Result == 10, "Result == 10");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end Example_Function2_Test_Case3 */


void Example_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(Example_Function2_Test_Case1, Example_Test_Setup, Example_Test_TearDown, "Example_Function2_Test_Case1");
    UtTest_Add(Example_Function2_Test_Case2, Example_Test_Setup, Example_Test_TearDown, "Example_Function2_Test_Case2");
    UtTest_Add(Example_Function2_Test_Case3, Example_Test_Setup, Example_Test_TearDown, "Example_Function2_Test_Case3");
} /* end Example_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
