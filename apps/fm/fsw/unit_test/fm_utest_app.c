/************************************************************************
**
** $Id: fm_utest_app.c 1.3 2009/12/02 14:31:21EST lwalling Exp  $
**
** Notes:
**
**   Unit test for CFS File Manager (FM) application source file "fm_app.c"
**
**   To direct text output to screen,
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file,
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
**
** $Log: fm_utest_app.c  $
** Revision 1.3 2009/12/02 14:31:21EST lwalling 
** Update FM unit tests to match UTF changes
** Revision 1.2 2009/11/20 15:40:39EST lwalling 
** Unit test updates
** Revision 1.1 2009/11/13 16:36:09EST lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/unit_test/project.pj
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"
#include "utf_cfe.h"

#include "fm_defs.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_events.h"
#include "fm_app.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_version.h"
#include "fm_verify.h"

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** Global data external to this file
*************************************************************************/

extern FM_FreeSpaceTable_t FM_FreeSpaceTable;

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  FM_HousekeepingCmd_t     *UT_HousekeepingCmd;

/************************************************************************
** Local function prototypes
*************************************************************************/

void CreateTestFile(char *Filename, int SizeInKs, boolean LeaveOpen);

/************************************************************************
** Local data
*************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file fm_app.c                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_app(void)
{
    int32 iResult;

    uint16 CmdAcceptedCounter;
    uint16 CmdRejectedCounter;

    uint32 TestCount = 0;
    uint32 FailCount = 0;


    /*
    ** Tests for function FM_AppMain()...
    **
    **   (1)  CFE_ES_RegisterApp error
    **   (2)  CFE_SB_RcvMsg error
    */

    /* (1) CFE_ES_RegisterApp error */
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERAPP_PROC, -1);
    FM_AppMain();
	UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERAPP_PROC);
    /* There is no failure detection for test (1) */

    /* (2) CFE_SB_RcvMsg error */
/*
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RUNLOOP_PROC, TRUE);
	UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_RCVMSG_PROC, -1);
    FM_AppMain();
	UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_RCVMSG_PROC);
	UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_RUNLOOP_PROC);
*/
    /* There is no failure detection for test (2) */



    /*
    ** Tests for function FM_AppInit()...
    **
    **   (1)  CFE_EVS_Register error
    **   (2)  CFE_SB_CreatePipe error
    **   (3)  CFE_SB_Subscribe error
    **   (4)  CFE_SB_Subscribe error
    **   (5)  FM_TableInit error
    **   (6)  Success
    */

    /* (1) CFE_EVS_Register error */
	UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, -1);
    iResult = FM_AppInit();
	UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_REGISTER_PROC);
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("FM_AppInit() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) CFE_SB_CreatePipe error */
	UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, -1);
    iResult = FM_AppInit();
	UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_CREATEPIPE_PROC);
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("FM_AppInit() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) CFE_SB_Subscribe error - HK request commands */
    iResult = FM_AppInit();
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("FM_AppInit() -- test failed (3)\n");
        FailCount++;
    }


    /* (4) CFE_SB_Subscribe error - FM ground commands */
    iResult = FM_AppInit();
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("FM_AppInit() -- test failed (4)\n");
        FailCount++;
    }


    /* (5) FM_TableInit error */
	UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, -1);
    iResult = FM_AppInit();
	UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_REGISTER_PROC);
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("FM_AppInit() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) Success */
    iResult = FM_AppInit();
    TestCount++;
    if (iResult != CFE_SUCCESS)
    {
        UTF_put_text("FM_AppInit() -- test failed (6)\n");
        FailCount++;
    }



    /*
    ** Tests for function FM_ProcessPkt()...
    **
    **   (1)  invalid message ID
    */

    /* (1) invalid message ID */
    CmdAcceptedCounter = FM_GlobalData.CommandCounter;
    CmdRejectedCounter = FM_GlobalData.CommandErrCounter;
    CFE_SB_InitMsg(UT_HousekeepingCmd, 0x1800, sizeof(FM_HousekeepingCmd_t), TRUE);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_HousekeepingCmd);
    TestCount++;
    if ((CmdAcceptedCounter != FM_GlobalData.CommandCounter) ||
        (CmdRejectedCounter != FM_GlobalData.CommandErrCounter))
    {
        UTF_put_text("FM_ProcessPkt() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ProcessCmd()...
    **
    **   (1)  invalid command code
    */

    /* (1) invalid command code */
    CmdRejectedCounter = FM_GlobalData.CommandErrCounter;
    CFE_SB_InitMsg(UT_HousekeepingCmd, FM_CMD_MID, sizeof(FM_HousekeepingCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_HousekeepingCmd, 99);
    FM_ProcessCmd((CFE_SB_MsgPtr_t) UT_HousekeepingCmd);
    TestCount++;
    if (CmdRejectedCounter == FM_GlobalData.CommandErrCounter)
    {
        UTF_put_text("FM_ProcessPkt() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ReportHK()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdAcceptedCounter = FM_GlobalData.CommandCounter;
    CmdRejectedCounter = FM_GlobalData.CommandErrCounter;
    CFE_SB_InitMsg(UT_HousekeepingCmd, FM_SEND_HK_MID, sizeof(FM_HousekeepingCmd_t) - 1, TRUE);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_HousekeepingCmd);
    TestCount++;
    if ((CmdAcceptedCounter != FM_GlobalData.CommandCounter) ||
        (CmdRejectedCounter != FM_GlobalData.CommandErrCounter))
    {
        UTF_put_text("FM_ReportHK() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdAcceptedCounter = FM_GlobalData.CommandCounter;
    CmdRejectedCounter = FM_GlobalData.CommandErrCounter;
    CFE_SB_InitMsg(UT_HousekeepingCmd, FM_SEND_HK_MID, sizeof(FM_HousekeepingCmd_t) + 1, TRUE);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_HousekeepingCmd);
    TestCount++;
    if ((CmdAcceptedCounter != FM_GlobalData.CommandCounter) ||
        (CmdRejectedCounter != FM_GlobalData.CommandErrCounter))
    {
        UTF_put_text("FM_ReportHK() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) good command packet, neither table is required */
    CmdAcceptedCounter = FM_GlobalData.CommandCounter;
    CmdRejectedCounter = FM_GlobalData.CommandErrCounter;
    CFE_SB_InitMsg(UT_HousekeepingCmd, FM_SEND_HK_MID, sizeof(FM_HousekeepingCmd_t), TRUE);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_HousekeepingCmd);
    TestCount++;
    if ((CmdAcceptedCounter != FM_GlobalData.CommandCounter) ||
        (CmdRejectedCounter != FM_GlobalData.CommandErrCounter))
    {
        UTF_put_text("FM_ReportHK() -- test failed (3)\n");
        FailCount++;
    }





    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("fm_app.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_app() */








