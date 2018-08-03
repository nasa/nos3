/************************************************************************
**
** $Id: fm_utest_cmds.c 1.6 2014/11/13 17:03:59EST lwalling Exp  $
**
** Notes:
**
**   Unit test for CFS File Manager (FM) application source file "fm_cmds.c"
**
**   To direct text output to screen,
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file,
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
**
** $Log: fm_utest_cmds.c  $
** Revision 1.6 2014/11/13 17:03:59EST lwalling 
** Modified unit tests to remove temp directories and files after unit tests complete
** Revision 1.5 2014/11/12 11:14:27EST lwalling 
** Modified unit tests to provide greater coverage
** Revision 1.4 2009/12/02 14:31:20EST lwalling 
** Update FM unit tests to match UTF changes
** Revision 1.3 2009/11/20 15:40:39EST lwalling 
** Unit test updates
** Revision 1.2 2009/11/13 16:26:54EST lwalling 
** Modify macro names, update unit tests
** Revision 1.1 2009/11/09 18:15:54EST lwalling 
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
#include "fm_child.h"
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

extern  FM_NoopCmd_t             *UT_NoopCmd        ;
extern  FM_ResetCmd_t            *UT_ResetCmd       ;
extern  FM_CopyFileCmd_t         *UT_CopyFileCmd    ;
extern  FM_MoveFileCmd_t         *UT_MoveFileCmd    ;
extern  FM_RenameFileCmd_t       *UT_RenameFileCmd  ;
extern  FM_DeleteFileCmd_t       *UT_DeleteFileCmd  ;
extern  FM_DeleteAllCmd_t        *UT_DeleteAllCmd   ;
extern  FM_DecompressCmd_t       *UT_DecompressCmd  ;
extern  FM_ConcatCmd_t           *UT_ConcatCmd      ;
extern  FM_GetFileInfoCmd_t      *UT_GetFileInfoCmd ;
extern  FM_GetOpenFilesCmd_t     *UT_GetOpenFilesCmd;
extern  FM_CreateDirCmd_t        *UT_CreateDirCmd   ;
extern  FM_DeleteDirCmd_t        *UT_DeleteDirCmd   ;
extern  FM_GetDirFileCmd_t       *UT_GetDirFileCmd  ;
extern  FM_GetDirPktCmd_t        *UT_GetDirPktCmd   ;
extern  FM_GetFreeSpaceCmd_t     *UT_GetFreeSpaceCmd;
extern  FM_SetTableStateCmd_t    *UT_SetTableStateCmd;

extern void CreateTestFile(char *Filename, int SizeInKs, boolean LeaveOpen);
extern int OpenFileHandle;


/************************************************************************
** Local function prototypes
*************************************************************************/


/************************************************************************
** Local data
*************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file fm_cmds.c                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_cmds(void)
{
    uint32 TestCount = 0;
    uint32 FailCount = 0;

    CreateTestFile("/ram/sub/closed1.bin", 16, FALSE);
    CreateTestFile("/ram/sub/closed2.bin", 16, FALSE);
    CreateTestFile("/ram/sub/closed4.bin", 16, FALSE);
    CreateTestFile("/ram/sub/closed5.bin", 16, FALSE);
    CreateTestFile("/ram/sub2/sub22/open1.bin", 16, TRUE);
    CreateTestFile("/ram/sub3/closed1.bin", 16, FALSE);
    CreateTestFile("/ram/sub3/closed2.bin", 16, FALSE);

    /*
    ** Tests for function FM_NoopCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  good command packet
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_NoopCmd, FM_CMD_MID, sizeof(FM_NoopCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_NoopCmd, FM_NOOP_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_NoopCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_NoopCmd, FM_CMD_MID, sizeof(FM_NoopCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_NoopCmd, FM_NOOP_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_NoopCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) good command packet, neither table is required */
    FM_GlobalData.CommandCounter = 0;
    CFE_SB_InitMsg(UT_NoopCmd, FM_CMD_MID, sizeof(FM_NoopCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_NoopCmd, FM_NOOP_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (FM_GlobalData.CommandCounter == 0)
    {
        UTF_put_text("FM_NoopCmd() -- test failed (3)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_ResetCountersCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  good command packet
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_ResetCmd, FM_CMD_MID, sizeof(FM_ResetCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ResetCmd, FM_RESET_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ResetCountersCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_ResetCmd, FM_CMD_MID, sizeof(FM_ResetCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ResetCmd, FM_RESET_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ResetCountersCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) good command packet, neither table is required */
    FM_GlobalData.CommandCounter = 9;
    CFE_SB_InitMsg(UT_ResetCmd, FM_CMD_MID, sizeof(FM_ResetCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ResetCmd, FM_RESET_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if (FM_GlobalData.CommandCounter != 0)
    {
        UTF_put_text("FM_ResetCountersCmd() -- test failed (3)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_FileCopyCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid Overwrite argument (not TRUE or FALSE)
    **   (4)  invalid source filename (does not exist)
    **   (5)  invalid target filename (closed) with Overwrite = FALSE
    **   (6)  valid source and target filenames (OS_cp error)
    **   (7)  valid target filename (closed) with Overwrite = TRUE
    **   (8)  valid source and target filenames
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid Overwrite argument (not TRUE or FALSE) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    UT_CopyFileCmd->Overwrite = 33;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid source filename (does not exist) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    UT_CopyFileCmd->Overwrite = FALSE;
    strcpy(UT_CopyFileCmd->Source, "/ram/doesnotexist.bin");
    strcpy(UT_CopyFileCmd->Target, "/ram/sub/copy1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid target filename (closed) with Overwrite = FALSE */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    UT_CopyFileCmd->Overwrite = FALSE;
    strcpy(UT_CopyFileCmd->Source, "/ram/sub/closed1.bin");
    strcpy(UT_CopyFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (6) valid source and target filenames (OS_cp error) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    UT_CopyFileCmd->Overwrite = TRUE;
    strcpy(UT_CopyFileCmd->Source, "/ram/sub/closed1.bin");
    strcpy(UT_CopyFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_CP_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CP_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) valid target filename (closed) with Overwrite = TRUE */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    UT_CopyFileCmd->Overwrite = TRUE;
    strcpy(UT_CopyFileCmd->Source, "/ram/sub/closed1.bin");
    strcpy(UT_CopyFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) valid source and target filenames */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_CopyFileCmd, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CopyFileCmd, FM_COPY_CC);
    UT_CopyFileCmd->Overwrite = FALSE;
    strcpy(UT_CopyFileCmd->Source, "/ram/sub/closed1.bin");
    strcpy(UT_CopyFileCmd->Target, "/ram/sub/closed3.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CopyFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileCopyCmd() -- test failed (8)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_FileMoveCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  valid source and target filenames (OS_mv error)
    **   (4)  invalid source filename (does not exist)
    **   (5)  invalid target filename (closed) Overwrite = FALSE
    **   (6


)  valid target filename (closed) Overwrite = TRUE
    **   (7)  valid source and target filenames (success)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) valid source and target filenames (OS_mv error) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    UT_MoveFileCmd->Overwrite = TRUE;
    strcpy(UT_MoveFileCmd->Source, "/ram/sub/closed3.bin");
    strcpy(UT_MoveFileCmd->Target, "/ram/sub/closed4.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_MV_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_MV_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid source filename (does not exist) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    UT_MoveFileCmd->Overwrite = FALSE;
    strcpy(UT_MoveFileCmd->Source, "/ram/doesnotexist.bin");
    strcpy(UT_MoveFileCmd->Target, "/ram/sub/closed4.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid target filename (closed) Overwrite = FALSE */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    UT_MoveFileCmd->Overwrite = FALSE;
    strcpy(UT_MoveFileCmd->Source, "/ram/sub/closed1.bin");
    strcpy(UT_MoveFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) valid target filename (closed) Overwrite = TRUE */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    UT_MoveFileCmd->Overwrite = TRUE;
    strcpy(UT_MoveFileCmd->Source, "/ram/sub/closed1.bin");
    strcpy(UT_MoveFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) valid source and target filenames (success) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_MoveFileCmd, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_MoveFileCmd, FM_MOVE_CC);
    UT_MoveFileCmd->Overwrite = FALSE;
    strcpy(UT_MoveFileCmd->Source, "/ram/sub/closed2.bin");
    strcpy(UT_MoveFileCmd->Target, "/ram/sub/closed1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_MoveFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileMoveCmd() -- test failed (7)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_RenameFileCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid source filename (does not exist)
    **   (4)  invalid target filename (closed)
    **   (5)  valid source and target filenames (OS_rename error)
    **   (6)  valid source and target filenames (success)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_RenameFileCmd, FM_CMD_MID, sizeof(FM_RenameFileCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_RenameFileCmd, FM_RENAME_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_RenameFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_RenameFileCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_RenameFileCmd, FM_CMD_MID, sizeof(FM_RenameFileCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_RenameFileCmd, FM_RENAME_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_RenameFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_RenameFileCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid source filename (does not exist) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_RenameFileCmd, FM_CMD_MID, sizeof(FM_RenameFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_RenameFileCmd, FM_RENAME_CC);
    strcpy(UT_RenameFileCmd->Source, "/ram/doesnotexist.bin");
    strcpy(UT_RenameFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_RenameFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_RenameFileCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid target filename (closed) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_RenameFileCmd, FM_CMD_MID, sizeof(FM_RenameFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_RenameFileCmd, FM_RENAME_CC);
    strcpy(UT_RenameFileCmd->Source, "/ram/closed4.bin");
    strcpy(UT_RenameFileCmd->Target, "/ram/closed1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_RenameFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_RenameFileCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid source and target filenames (OS_rename error) */
     FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_RenameFileCmd, FM_CMD_MID, sizeof(FM_RenameFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_RenameFileCmd, FM_RENAME_CC);
    strcpy(UT_RenameFileCmd->Source, "/ram/sub/closed4.bin");
    strcpy(UT_RenameFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_RenameFileCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_RENAME_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_RENAME_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_RenameFileCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) valid source and target filenames (success) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_RenameFileCmd, FM_CMD_MID, sizeof(FM_RenameFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_RenameFileCmd, FM_RENAME_CC);
    strcpy(UT_RenameFileCmd->Source, "/ram/sub/closed4.bin");
    strcpy(UT_RenameFileCmd->Target, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_RenameFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_RenameFileCmd() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_DeleteFileCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid filename (open)
    **   (4)  valid filename (OS_remove error)
    **   (5)  valid filename (success)
    **   (6)  valid filename -- FM_DELETE_INT_FC
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DeleteFileCmd, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteFileCmd, FM_DELETE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteFileCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DeleteFileCmd, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteFileCmd, FM_DELETE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteFileCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid source filename (open) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteFileCmd, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteFileCmd, FM_DELETE_CC);
    strcpy(UT_DeleteFileCmd->Filename, "/ram/sub2/sub22/open1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_DeleteFileCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid filename (OS_remove error) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteFileCmd, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteFileCmd, FM_DELETE_CC);
    strcpy(UT_DeleteFileCmd->Filename, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteFileCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_REMOVE_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_REMOVE_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_DeleteFileCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid filename (success) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteFileCmd, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteFileCmd, FM_DELETE_CC);
    strcpy(UT_DeleteFileCmd->Filename, "/ram/sub/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DeleteFileCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) valid filename (success) (FM_DELETE_INT_CC) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteFileCmd, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteFileCmd, FM_DELETE_INT_CC);
    strcpy(UT_DeleteFileCmd->Filename, "/ram/sub/closed1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DeleteFileCmd() -- test failed (6)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_DeleteAllFilesCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid directory name (filename)
    **   (4)  valid directory name (OS_remove error)
    **   (5)  valid directory name (contains invalid filename)
    **   (6)  valid directory name (contains filename too long)
    **   (7)  valid directory name (contains open file)
    **   (8)  valid directory name (contains sub-directory)
    **   (9)  valid directory name (valid contents)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid directory name (filename) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    strcpy(UT_DeleteAllCmd->Directory, "/ram/sub/closed5.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid directory name (OS_remove error) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    strcpy(UT_DeleteAllCmd->Directory, "/ram/sub");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_REMOVE_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_REMOVE_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid directory name (contains invalid filename) */

    /* (6) valid directory name (contains filename too long) */

    /* (7) valid directory name (contains open file) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    strcpy(UT_DeleteAllCmd->Directory, "/ram/sub2/sub22");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) valid directory name (contains sub-directory) */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    strcpy(UT_DeleteAllCmd->Directory, "/ram/sub2");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (8)\n");
        FailCount++;
    }

    /* (9) valid directory name (valid contents) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteAllCmd, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteAllCmd, FM_DELETE_ALL_CC);
    strcpy(UT_DeleteAllCmd->Directory, "/ram/sub");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteAllCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DeleteAllFilesCmd() -- test failed (9)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_DecompressFileCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid source filename (open)
    **   (4)  invalid target filename (closed)
    **   (5)  valid source and target filenames
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DecompressCmd, FM_CMD_MID, sizeof(FM_DecompressCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DecompressCmd, FM_DECOMPRESS_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DecompressCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DecompressFileCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DecompressCmd, FM_CMD_MID, sizeof(FM_DecompressCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DecompressCmd, FM_DECOMPRESS_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DecompressCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DecompressFileCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid source filename (open) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DecompressCmd, FM_CMD_MID, sizeof(FM_DecompressCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DecompressCmd, FM_DECOMPRESS_CC);
    strcpy(UT_DecompressCmd->Source, "/ram/sub2/sub22/open1.bin");
    strcpy(UT_DecompressCmd->Target, "/ram/sub/unzip1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DecompressCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DecompressFileCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid target filename (closed) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DecompressCmd, FM_CMD_MID, sizeof(FM_DecompressCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DecompressCmd, FM_DECOMPRESS_CC);
    strcpy(UT_DecompressCmd->Source, "/ram/sub3/closed1.bin");
    strcpy(UT_DecompressCmd->Target, "/ram/sub3/closed2.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DecompressCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DecompressFileCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid source and target filenames */
    FM_GlobalData.CommandCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DecompressCmd, FM_CMD_MID, sizeof(FM_DecompressCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DecompressCmd, FM_DECOMPRESS_CC);
    strcpy(UT_DecompressCmd->Source, "/ram/sub3/closed1.bin");
    strcpy(UT_DecompressCmd->Target, "/ram/sub/unzip1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DecompressCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandCounter == 0)
    {
        UTF_put_text("FM_DecompressFileCmd() -- test failed (5)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_ConcatenateFilesCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid source #1 filename (open)
    **   (4)  invalid source #2 filename (open)
    **   (5)  invalid target filename (closed)
    **   (6)  valid source and target filenames
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_ConcatCmd, FM_CMD_MID, sizeof(FM_ConcatCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ConcatCmd, FM_CONCAT_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ConcatCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ConcatenateFilesCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_ConcatCmd, FM_CMD_MID, sizeof(FM_ConcatCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ConcatCmd, FM_CONCAT_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ConcatCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ConcatenateFilesCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid source #1 filename (open) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_ConcatCmd, FM_CMD_MID, sizeof(FM_ConcatCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ConcatCmd, FM_CONCAT_CC);
    strcpy(UT_ConcatCmd->Source1, "/ram/sub2/sub22/open1.bin");
    strcpy(UT_ConcatCmd->Source2, "/ram/sub3/closed1.bin");
    strcpy(UT_ConcatCmd->Target, "/ram/sub3/closed3.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ConcatCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ConcatenateFilesCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid source #2 filename (open) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_ConcatCmd, FM_CMD_MID, sizeof(FM_ConcatCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ConcatCmd, FM_CONCAT_CC);
    strcpy(UT_ConcatCmd->Source1, "/ram/sub3/closed1.bin");
    strcpy(UT_ConcatCmd->Source2, "/ram/sub2/sub22/open1.bin");
    strcpy(UT_ConcatCmd->Target, "/ram/sub3/closed3.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ConcatCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ConcatenateFilesCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid target filename (open) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_ConcatCmd, FM_CMD_MID, sizeof(FM_ConcatCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ConcatCmd, FM_CONCAT_CC);
    strcpy(UT_ConcatCmd->Source1, "/ram/sub3/closed1.bin");
    strcpy(UT_ConcatCmd->Source2, "/ram/sub3/closed2.bin");
    strcpy(UT_ConcatCmd->Target, "/ram/sub2/sub22/open1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ConcatCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_ConcatenateFilesCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) valid source and target filenames */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_ConcatCmd, FM_CMD_MID, sizeof(FM_ConcatCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_ConcatCmd, FM_CONCAT_CC);
    strcpy(UT_ConcatCmd->Source1, "/ram/sub3/closed1.bin");
    strcpy(UT_ConcatCmd->Source2, "/ram/sub3/closed2.bin");
    strcpy(UT_ConcatCmd->Target, "/ram/sub3/closed3.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_ConcatCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_ConcatenateFilesCmd() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_FileInfoCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid source filename (bad chars)
    **   (4)  invalid source filename (does not exist)
    **   (5)  valid source directory name
    **   (6)  valid source filename (file - open)
    **   (7)  valid source filename (file - closed, CRC = none)
    **   (8)  valid source filename (file - closed, CRC = 16 bit)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid source filename (bad chars) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    strcpy(UT_GetFileInfoCmd->Filename, "/~!@#$%^/&*()-+");
    UT_GetFileInfoCmd->FileInfoCRC = FM_IGNORE_CRC;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid source filename (does not exist) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    strcpy(UT_GetFileInfoCmd->Filename, "/ram/sub3/closed4.bin");
    UT_GetFileInfoCmd->FileInfoCRC = FM_IGNORE_CRC;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid source directory name */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    strcpy(UT_GetFileInfoCmd->Filename, "/ram/sub3");
    UT_GetFileInfoCmd->FileInfoCRC = FM_IGNORE_CRC;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) valid source filename (file - open) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    strcpy(UT_GetFileInfoCmd->Filename, "/ram/sub2/sub22/open1.bin");
    UT_GetFileInfoCmd->FileInfoCRC = FM_IGNORE_CRC;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) valid source filename (file - closed, CRC = none) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    strcpy(UT_GetFileInfoCmd->Filename, "/ram/sub3/closed1.bin");
    UT_GetFileInfoCmd->FileInfoCRC = FM_IGNORE_CRC;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) valid source filename (file - closed, CRC = 16 bit) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetFileInfoCmd, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd, FM_GET_FILE_INFO_CC);
    strcpy(UT_GetFileInfoCmd->Filename, "/ram/sub3/closed2.bin");
    UT_GetFileInfoCmd->FileInfoCRC = CFE_ES_CRC_16;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFileInfoCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_FileInfoCmd() -- test failed (8)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_GetOpenFilesCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  valid packet length (cmd has no args)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetOpenFilesCmd, FM_CMD_MID, sizeof(FM_GetOpenFilesCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetOpenFilesCmd, FM_GET_OPEN_FILES_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetOpenFilesCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_GetOpenFilesCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetOpenFilesCmd, FM_CMD_MID, sizeof(FM_GetOpenFilesCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetOpenFilesCmd, FM_GET_OPEN_FILES_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetOpenFilesCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_GetOpenFilesCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) valid packet length (cmd has no args) */
    FM_GlobalData.CommandCounter = 0;
    CFE_SB_InitMsg(UT_GetOpenFilesCmd, FM_CMD_MID, sizeof(FM_GetOpenFilesCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetOpenFilesCmd, FM_GET_OPEN_FILES_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetOpenFilesCmd);
    TestCount++;
    if (FM_GlobalData.CommandCounter == 0)
    {
        UTF_put_text("FM_GetOpenFilesCmd() -- test failed (3)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_CreateDirectoryCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid directory name (already exists)
    **   (4)  valid directory name (OS_mkdir error)
    **   (5)  valid directory name (success)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CreateDirCmd, FM_CMD_MID, sizeof(FM_CreateDirCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CreateDirCmd, FM_CREATE_DIR_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CreateDirCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_CreateDirectoryCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_CreateDirCmd, FM_CMD_MID, sizeof(FM_CreateDirCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CreateDirCmd, FM_CREATE_DIR_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CreateDirCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_CreateDirectoryCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid directory name (already exists) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_CreateDirCmd, FM_CMD_MID, sizeof(FM_CreateDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CreateDirCmd, FM_CREATE_DIR_CC);
    strcpy(UT_CreateDirCmd->Directory, "/ram/sub");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CreateDirCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_CreateDirectoryCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid directory filename (OS_mkdir error) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_CreateDirCmd, FM_CMD_MID, sizeof(FM_CreateDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CreateDirCmd, FM_CREATE_DIR_CC);
    strcpy(UT_CreateDirCmd->Directory, "/ram/sub4");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CreateDirCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_MKDIR_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_MKDIR_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_CreateDirectoryCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid directory filename (success) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_CreateDirCmd, FM_CMD_MID, sizeof(FM_CreateDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_CreateDirCmd, FM_CREATE_DIR_CC);
    strcpy(UT_CreateDirCmd->Directory, "/ram/sub4");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_CreateDirCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_CreateDirectoryCmd() -- test failed (5)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_DeleteDirectoryCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid directory name (closed file)
    **   (4)  valid directory name (not empty)
    **   (5)  valid directory name (empty) - OS_rmdir error
    **   (6)  valid directory name (empty) - success
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DeleteDirCmd, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteDirCmd, FM_DELETE_DIR_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteDirCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteDirectoryCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_DeleteDirCmd, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteDirCmd, FM_DELETE_DIR_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteDirCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteDirectoryCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid directory name (closed file) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteDirCmd, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteDirCmd, FM_DELETE_DIR_CC);
    strcpy(UT_DeleteDirCmd->Directory, "/ram/sub3/closed1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteDirCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DeleteDirectoryCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid directory name (not empty) */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteDirCmd, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteDirCmd, FM_DELETE_DIR_CC);
    strcpy(UT_DeleteDirCmd->Directory, "/ram/sub3");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteDirCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_DeleteDirectoryCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid directory name (empty) - OS_rmdir error */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteDirCmd, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteDirCmd, FM_DELETE_DIR_CC);
    strcpy(UT_DeleteDirCmd->Directory, "/ram/sub4");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteDirCmd);
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_RMDIR_PROC, -1);
    FM_ChildLoop();
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_RMDIR_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_DeleteDirectoryCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) valid directory name (empty) - success */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_DeleteDirCmd, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_DeleteDirCmd, FM_DELETE_DIR_CC);
    strcpy(UT_DeleteDirCmd->Directory, "/ram/sub4");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_DeleteDirCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DeleteDirectoryCmd() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_DirListFileCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid directory name (does not exist)
    **   (4)  invalid output filename (already open)
    **   (5)  valid directory name and default output filename
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirFileCmd, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirFileCmd, FM_GET_DIR_FILE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListFileCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirFileCmd, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirFileCmd, FM_GET_DIR_FILE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListFileCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid directory name (does not exist) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirFileCmd, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirFileCmd, FM_GET_DIR_FILE_CC);
    strcpy(UT_GetDirFileCmd->Directory, "/ram/badsub");
    strcpy(UT_GetDirFileCmd->Filename, "/ram/sub3/list1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListFileCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid output filename (already open) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirFileCmd, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirFileCmd, FM_GET_DIR_FILE_CC);
    strcpy(UT_GetDirFileCmd->Directory, "/ram/sub3");
    strcpy(UT_GetDirFileCmd->Filename, "/ram/sub2/sub22/open1.bin");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirFileCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListFileCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid directory name and default output filename */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetDirFileCmd, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirFileCmd, FM_GET_DIR_FILE_CC);
    strcpy(UT_GetDirFileCmd->Directory, "/ram/sub3");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirFileCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DirListFileCmd() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_DirListPktCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid directory name (does not exist)
    **   (4)  valid directory name (offset = 1)
    **   (5)  valid directory name (offset = 0)
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirPktCmd, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirPktCmd, FM_GET_DIR_PKT_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirPktCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListMsgCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirPktCmd, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirPktCmd, FM_GET_DIR_PKT_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirPktCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListMsgCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid directory name (does not exist) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetDirPktCmd, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirPktCmd, FM_GET_DIR_PKT_CC);
    strcpy(UT_GetDirPktCmd->Directory, "/ram/badsub");
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirPktCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_DirListMsgCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid directory name (offset = 1) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetDirPktCmd, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirPktCmd, FM_GET_DIR_PKT_CC);
    strcpy(UT_GetDirPktCmd->Directory, "/ram/sub3");
    UT_GetDirPktCmd->DirListOffset = 1;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirPktCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DirListMsgCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) valid directory name (offset = 0) */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_SB_InitMsg(UT_GetDirPktCmd, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetDirPktCmd, FM_GET_DIR_PKT_CC);
    strcpy(UT_GetDirPktCmd->Directory, "/ram/sub3");
    UT_GetDirPktCmd->DirListOffset = 0;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetDirPktCmd);
    FM_ChildLoop();
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_DirListMsgCmd() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_FreeSpaceCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid table pointer (not loaded)
    **   (4)  valid table pointer
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetFreeSpaceCmd, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd, FM_GET_FREE_SPACE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FreeSpaceCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_GetFreeSpaceCmd, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd, FM_GET_FREE_SPACE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FreeSpaceCmd() -- test failed (2)\n");
        FailCount++;
    }


    /* (3) invalid table pointer (not loaded) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;
    CFE_SB_InitMsg(UT_GetFreeSpaceCmd, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd, FM_GET_FREE_SPACE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_FreeSpaceCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid table pointer */
    FM_GlobalData.CommandCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = &FM_FreeSpaceTable;
    CFE_SB_InitMsg(UT_GetFreeSpaceCmd, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd, FM_GET_FREE_SPACE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_GetFreeSpaceCmd);
    TestCount++;
    if (FM_GlobalData.CommandCounter == 0)
    {
        UTF_put_text("FM_FreeSpaceCmd() -- test failed (4)\n");
        FailCount++;
    }
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;



    /*
    ** Tests for function FM_SetTableStateCmd()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid table pointer (not loaded)
    **   (4)  valid table pointer - invalid index
    **   (5)  valid table pointer - valid index, invalid state
    **   (6)  valid table pointer - valid index, valid state, invalid current table entry state
    **   (7)  valid table pointer - valid index, valid state, valid current table entry state
    */

    /* (1) invalid packet length (too short) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t) - 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    FM_GlobalData.CommandErrCounter = 0;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t) + 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid table pointer (not loaded) */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    UT_SetTableStateCmd->TableEntryIndex = 0;
    UT_SetTableStateCmd->TableEntryState = FM_TABLE_ENTRY_DISABLED;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) valid table pointer - invalid index */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = &FM_FreeSpaceTable;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    UT_SetTableStateCmd->TableEntryIndex = FM_TABLE_ENTRY_COUNT;
    UT_SetTableStateCmd->TableEntryState = FM_TABLE_ENTRY_DISABLED;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (4)\n");
        FailCount++;
    }
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;

    /* (5) valid table pointer - valid index, invalid state */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = &FM_FreeSpaceTable;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    UT_SetTableStateCmd->TableEntryIndex = 0;
    UT_SetTableStateCmd->TableEntryState = FM_TABLE_ENTRY_UNUSED;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (5)\n");
        FailCount++;
    }
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;

    /* (6) valid table pointer - valid index, valid state, invalid current table entry state */
    FM_GlobalData.CommandErrCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = &FM_FreeSpaceTable;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    UT_SetTableStateCmd->TableEntryIndex = FM_TABLE_ENTRY_COUNT - 1;
    UT_SetTableStateCmd->TableEntryState = FM_TABLE_ENTRY_DISABLED;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandErrCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (6)\n");
        FailCount++;
    }
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;

    /* (7) valid table pointer - valid index, valid state, valid current table entry state */
    FM_GlobalData.CommandCounter = 0;
    FM_GlobalData.FreeSpaceTablePtr = &FM_FreeSpaceTable;
    CFE_SB_InitMsg(UT_SetTableStateCmd, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) UT_SetTableStateCmd, FM_SET_TABLE_STATE_CC);
    UT_SetTableStateCmd->TableEntryIndex = 1;
    UT_SetTableStateCmd->TableEntryState = FM_TABLE_ENTRY_ENABLED;
    FM_ProcessPkt((CFE_SB_MsgPtr_t) UT_SetTableStateCmd);
    TestCount++;
    if (FM_GlobalData.CommandCounter == 0)
    {
        UTF_put_text("FM_SetTableStateCmd() -- test failed (7)\n");
        FailCount++;
    }
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;



    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("fm_cmds.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    /*
    ** Delete files created during these tests...
    */
    OS_close(OpenFileHandle);
    OS_remove("/ram/sub2/sub22/open1.bin");
    OS_remove("/ram/sub3/closed1.bin");
    OS_remove("/ram/sub3/closed2.bin");
    OS_remove("/ram/sub3/closed3.bin");
    OS_remove("/ram/fm_dirlist.out");

    return;

} /* End of Test_cmds() */








