/************************************************************************
**
** $Id: ds_utest_app.c 1.6.1.1 2015/02/28 17:14:06EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Notes: 
**
**   Unit test for CFS Data Storage (DS) application source file "ds_app.c"
**
**   To direct text output to screen, 
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file, 
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
** 
** $Log: ds_utest_app.c  $
** Revision 1.6.1.1 2015/02/28 17:14:06EST sstrege 
** Added copyright information
** Revision 1.6 2009/12/07 13:40:39EST lwalling 
** Update DS unit tests, add unit test results files to MKS
** Revision 1.5 2009/09/01 15:22:57EDT lwalling 
** Cleanup comments
** Revision 1.4 2009/08/31 17:51:37EDT lwalling 
** Convert calls from DS_TableVerifyString() to CFS_VerifyString() with descriptive arg names
** Revision 1.3 2009/08/27 16:32:33EDT lwalling 
** Updates from source code review
** Revision 1.2 2009/08/13 10:01:26EDT lwalling 
** Updates to unit test source files
** Revision 1.1 2009/05/26 13:37:48EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/unit_test/project.pj
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

#include "ds_perfids.h"
#include "ds_msgids.h"
#include "ds_platform_cfg.h"

#include "ds_platform_cfg.h"
#include "ds_verify.h"

#include "ds_appdefs.h"
#include "ds_msgdefs.h"

#include "ds_msg.h"
#include "ds_app.h"
#include "ds_cmds.h"
#include "ds_file.h"
#include "ds_table.h"
#include "ds_events.h"
#include "ds_version.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** Global data external to this file
*************************************************************************/

extern  DS_AppData_t DS_AppData;   /* DS app global data */

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  DS_NoopCmd_t        *UT_NoopCmd;
extern  DS_ResetCmd_t       *UT_ResetCmd;
extern  DS_AppStateCmd_t    *UT_AppStateCmd;
extern  DS_FilterFileCmd_t  *UT_FilterFileCmd;
extern  DS_FilterTypeCmd_t  *UT_FilterTypeCmd;
extern  DS_FilterParmsCmd_t *UT_FilterParmsCmd;
extern  DS_DestTypeCmd_t    *UT_DestTypeCmd;
extern  DS_DestStateCmd_t   *UT_DestStateCmd;
extern  DS_DestPathCmd_t    *UT_DestPathCmd;
extern  DS_DestBaseCmd_t    *UT_DestBaseCmd;
extern  DS_DestExtCmd_t     *UT_DestExtCmd;
extern  DS_DestSizeCmd_t    *UT_DestSizeCmd;
extern  DS_DestAgeCmd_t     *UT_DestAgeCmd;
extern  DS_DestCountCmd_t   *UT_DestCountCmd;
extern  DS_CloseFileCmd_t   *UT_CloseFileCmd;

extern  DS_FilterTable_t     UT_FilterTbl;  
extern  DS_DestFileTable_t   UT_DestFileTbl;

extern  DS_FilterTable_t     DS_FilterTable;
extern  DS_DestFileTable_t   DS_DestFileTable;

extern  DS_FilterTable_t    *DS_FilterTblPtr;
extern  DS_DestFileTable_t  *DS_DestFileTblPtr;

/************************************************************************
** Local functions
*************************************************************************/

int32 SubscribeHook = 0;

int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    SubscribeHook++;

    if ((SubscribeHook == 1) || (SubscribeHook == 3))
        return (-1);
    else
        return (CFE_SUCCESS);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file ds_app.c                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_app(void)
{
    int32 iResult;

    uint32 Counter32;

    uint16 CmdAcceptedCounter;
    uint16 CmdRejectedCounter;

    uint32 TestCount = 0;
    uint32 FailCount = 0;

    /*
    ** Initialize packet filter table contents for these tests...
    */
    memset(&UT_FilterTbl, 0, sizeof(UT_FilterTbl));

    UT_FilterTbl.Packet[DS_PACKETS_IN_FILTER_TABLE - 1].MessageID = DS_HK_TLM_MID;    

    /*
    ** Initialize destination file table contents for these tests...
    */
    memset(&UT_DestFileTbl, 0, sizeof(UT_DestFileTbl));

    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) NULL;

    /*
    ** Tests for function DS_AppMain()...
    **
    **   (1)  CFE_ES_RegisterApp error
    */

    /* (1) Coverage for CFE_ES_RegisterApp error */
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERAPP_PROC, -1);
    DS_AppMain();
	UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERAPP_PROC);


    /*
    ** Tests for function DS_AppInitialize()...
    **
    **   (1)  CFE_EVS_Register error
    **   (2)  CFE_SB_CreatePipe error
    **   (3)  CFE_SB_Subscribe error - HK request commands
    **   (4)  CFE_SB_Subscribe error - DS ground commands
    **
    **   (5)  DS_TableCreateCDS error - AppInit -> TableInit
    */

    /* (1) CFE_EVS_Register error */
	UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, -1);
    iResult = DS_AppInitialize();
	UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_REGISTER_PROC);
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("DS_AppInitialize() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) CFE_SB_CreatePipe error */
	UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, -1);
    iResult = DS_AppInitialize();
	UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_CREATEPIPE_PROC);
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("DS_AppInitialize() -- test failed (2)\n");
        FailCount++;
    }

    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *)&CFE_SB_SubscribeHook);

    /* (3) CFE_SB_Subscribe error - HK request commands */
    iResult = DS_AppInitialize();
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("DS_AppInitialize() -- test failed (3)\n");
        FailCount++;
    }


    /* (4) CFE_SB_Subscribe error - DS ground commands */
    iResult = DS_AppInitialize();
    TestCount++;
    if (iResult == CFE_SUCCESS)
    {
        UTF_put_text("DS_AppInitialize() -- test failed (4)\n");
        FailCount++;
    }


    /* (5) DS_TableCreateCDS error - AppInit -> TableInit */
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCDS_PROC, -1);
    iResult = DS_AppInitialize();
	UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERCDS_PROC);
    TestCount++;
    if (iResult != CFE_SUCCESS)
    {
        UTF_put_text("DS_AppInitialize() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_AppMain()...
    **
    **  We are going to defer the unit tests for the main process
    **  loop because the unit test environment will terminate the
    **  test session when the last input command is processed from
    **  the SB input file.  Refer to the last item in the primary
    **  unit test driver file (ds_utest.c), which will be the call
    **  to the function DS_AppMain().
    */
    CFE_ES_RegisterApp();

    /*
    ** Tests for function DS_AppInitialize()...
    **
    **  We are going to skip the unit tests for the failure cases
    **  in the startup initialization function because there are
    **  too many system dependencies to try and simulate here.
    **  And since the unit test makefile does not make loadable
    **  table files, we directly link the table file data and then
    **  load the tables via the Table Manager API functions for
    **  tables in memory.
    */
    DS_AppInitialize();
    DS_AppProcessHK();
    CFE_TBL_Load(DS_AppData.DestFileTblHandle, CFE_TBL_SRC_ADDRESS, &DS_DestFileTable);
    DS_TableManageDestFile();
    CFE_TBL_Load(DS_AppData.FilterTblHandle, CFE_TBL_SRC_ADDRESS, &DS_FilterTable);
    DS_TableManageFilter();
    DS_FilterTblPtr   = DS_AppData.FilterTblPtr;
    DS_DestFileTblPtr = DS_AppData.DestFileTblPtr;

    /*
    ** Set the path for each destination file to the unit test file system...
    */
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    strcpy(UT_DestPathCmd->Pathname, "/tt/");
    UT_DestPathCmd->FileTableIndex = 0;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    UT_DestPathCmd->FileTableIndex = 1;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    UT_DestPathCmd->FileTableIndex = 2;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    UT_DestPathCmd->FileTableIndex = 3;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    UT_DestPathCmd->FileTableIndex = 4;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    UT_DestPathCmd->FileTableIndex = 5;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);

    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t), TRUE);
    UT_DestStateCmd->EnableState = DS_ENABLED;
    UT_DestStateCmd->FileTableIndex = 0;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    UT_DestStateCmd->FileTableIndex = 1;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    UT_DestStateCmd->FileTableIndex = 2;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    UT_DestStateCmd->FileTableIndex = 3;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    UT_DestStateCmd->FileTableIndex = 4;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    UT_DestStateCmd->FileTableIndex = 5;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);

    CFE_SB_InitMsg(UT_AppStateCmd, DS_CMD_MID, sizeof(DS_AppStateCmd_t), TRUE);
    UT_AppStateCmd->EnableState = DS_ENABLED;
    DS_CmdSetAppState((CFE_SB_MsgPtr_t) UT_AppStateCmd);

    /*
    ** Tests for function DS_AppProcessMsg()...
    **
    **   (1)  msg ID = DS_CMD_MID,     state = dis, ptrs = ok
    **   (2)  msg ID = DS_SEND_HK_MID, state = dis, ptrs = ok
    **   (3)  msg ID = DS_HK_TLM_MID,  state = ena, ptrs = null
    **   (4)  msg ID = DS_HK_TLM_MID,  state = ena, ptrs = ok
    */

    /* (1) msg ID = DS_CMD_MID,     state = dis, ptrs = ok */
    DS_AppData.AppEnableState = DS_DISABLED;
    Counter32 = DS_AppData.DisabledPktCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_CMD_MID, sizeof(DS_NoopCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_NoopCmd, DS_NOOP_CC);
    DS_AppProcessMsg((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (Counter32 == DS_AppData.DisabledPktCounter)
    {
        UTF_put_text("DS_AppProcessMsg() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) msg ID = DS_SEND_HK_MID, state = dis, ptrs = ok */
    DS_AppData.AppEnableState = DS_DISABLED;
    Counter32 = DS_AppData.DisabledPktCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_SEND_HK_MID, sizeof(DS_NoopCmd_t), TRUE);
    DS_AppProcessMsg((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (Counter32 == DS_AppData.DisabledPktCounter)
    {
        UTF_put_text("DS_AppProcessMsg() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) msg ID = DS_HK_TLM_MID,  state = ena, ptrs = null */
    DS_AppData.AppEnableState = DS_ENABLED;
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    Counter32 = DS_AppData.IgnoredPktCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_HK_TLM_MID, sizeof(DS_NoopCmd_t), TRUE);
    DS_AppProcessMsg((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (Counter32 == DS_AppData.IgnoredPktCounter)
    {
        UTF_put_text("DS_AppProcessMsg() -- test failed (3)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (4) msg ID = DS_HK_TLM_MID,  state = ena, ptrs = ok */
    DS_AppData.AppEnableState = DS_ENABLED;
    Counter32 = DS_AppData.PassedPktCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_HK_TLM_MID, sizeof(DS_NoopCmd_t), TRUE);
    DS_AppProcessMsg((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (Counter32 == DS_AppData.PassedPktCounter)
    {
        UTF_put_text("DS_AppProcessMsg() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_AppProcessCmd()...
    **
    **   (1)  command code = DS_NOOP_CC
    **   (2)  command code = DS_RESET_CC
    **   (3)  command code = DS_SET_APP_STATE_CC
    **   (4)  command code = DS_SET_FILTER_FILE_CC
    **   (5)  command code = DS_SET_FILTER_TYPE_CC
    **   (6)  command code = DS_SET_FILTER_PARMS_CC
    **   (7)  command code = DS_SET_DEST_TYPE_CC
    **   (8)  command code = DS_SET_DEST_STATE_CC
    **   (9)  command code = DS_SET_DEST_PATH_CC
    **   (10) command code = DS_SET_DEST_BASE_CC
    **   (11) command code = DS_SET_DEST_EXT_CC
    **   (12) command code = DS_SET_DEST_SIZE_CC
    **   (13) command code = DS_SET_DEST_AGE_CC
    **   (14) command code = DS_SET_DEST_COUNT_CC
    **   (15) command code = DS_CLOSE_FILE_CC
    **   (16) command code = unknown
    */

    /* (1) command code = DS_NOOP_CC (valid) */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_CMD_MID, sizeof(DS_NoopCmd_t), FALSE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_NoopCmd, DS_NOOP_CC);
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) command code = DS_RESET_CC (valid) */
    CFE_SB_InitMsg(UT_ResetCmd, DS_CMD_MID, sizeof(DS_ResetCmd_t), FALSE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ResetCmd, DS_RESET_CC);
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if (DS_AppData.CmdAcceptedCounter != 0)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) command code = DS_SET_APP_STATE_CC (invalid ena/dis state) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_AppStateCmd, DS_CMD_MID, sizeof(DS_AppStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_AppStateCmd, DS_SET_APP_STATE_CC);
    UT_AppStateCmd->EnableState = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_AppStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) command code = DS_SET_FILTER_FILE_CC (invalid msg ID) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_FilterFileCmd, DS_SET_FILTER_FILE_CC);
    UT_FilterFileCmd->MessageID = DS_UNUSED;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) command code = DS_SET_FILTER_TYPE_CC (invalid msg ID) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_FilterTypeCmd, DS_SET_FILTER_TYPE_CC);
    UT_FilterTypeCmd->MessageID = DS_UNUSED;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) command code = DS_SET_FILTER_PARMS_CC (invalid msg ID) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_FilterParmsCmd, DS_SET_FILTER_PARMS_CC);
    UT_FilterParmsCmd->MessageID = DS_UNUSED;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) command code = DS_SET_DEST_TYPE_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestTypeCmd, DS_SET_DEST_TYPE_CC);
    UT_DestTypeCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) command code = DS_SET_DEST_STATE_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestStateCmd, DS_SET_DEST_STATE_CC);
    UT_DestStateCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (8)\n");
        FailCount++;
    }

    /* (9) command code = DS_SET_DEST_PATH_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestPathCmd, DS_SET_DEST_PATH_CC);
    UT_DestPathCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (9)\n");
        FailCount++;
    }

    /* (10) command code = DS_SET_DEST_BASE_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestBaseCmd, DS_SET_DEST_BASE_CC);
    UT_DestBaseCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (10)\n");
        FailCount++;
    }

    /* (11) command code = DS_SET_DEST_EXT_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestExtCmd, DS_SET_DEST_EXT_CC);
    UT_DestExtCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (11)\n");
        FailCount++;
    }

    /* (12) command code = DS_SET_DEST_SIZE_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestSizeCmd, DS_SET_DEST_SIZE_CC);
    UT_DestSizeCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (12)\n");
        FailCount++;
    }

    /* (13) command code = DS_SET_DEST_AGE_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestAgeCmd, DS_SET_DEST_AGE_CC);
    UT_DestAgeCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (13)\n");
        FailCount++;
    }

    /* (14) command code = DS_SET_DEST_COUNT_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DestCountCmd, DS_SET_DEST_COUNT_CC);
    UT_DestCountCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (14)\n");
        FailCount++;
    }

    /* (15) command code = DS_CLOSE_FILE_CC (invalid index arg) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_CloseFileCmd, DS_CMD_MID, sizeof(DS_CloseFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CloseFileCmd, DS_CLOSE_FILE_CC);
    UT_CloseFileCmd->FileTableIndex = 99;
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_CloseFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (15)\n");
        FailCount++;
    }

    /* (16) command code = unknown */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_CMD_MID, sizeof(DS_NoopCmd_t), FALSE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_NoopCmd, 99);
    DS_AppProcessCmd((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_AppProcessCmd() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_AppProcessHK()...
    **
    **   (1)  file table ptr = null, open files = no
    **   (2)  file table ptr = ok, open files = no
    **   (3)  file table ptr = ok, open files = yes
    */

    /* (1) file table ptr = null, open files = no */
    /* (test #1 is performed during initialization above) */

    /* (2) file table ptr = ok, open files = no */
    DS_AppProcessHK();

    /* (3) file table ptr = ok, open files = yes */
    /* (test #3 is performed later, during file tests) */


    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("ds_app.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_app() */








