/************************************************************************
**
** $Id: fm_utest_utils.c 1.7 2014/11/13 17:03:58EST lwalling Exp  $
**
** Notes:
**
**   Unit test for CFS File Manager (FM) application source file "fm_cmd_utils.c"
**
**   To direct text output to screen,
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file,
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
**
** $Log: fm_utest_utils.c  $
** Revision 1.7 2014/11/13 17:03:58EST lwalling 
** Modified unit tests to remove temp directories and files after unit tests complete
** Revision 1.6 2014/11/12 11:14:26EST lwalling 
** Modified unit tests to provide greater coverage
** Revision 1.5 2014/10/22 17:51:00EDT lwalling 
** Allow zero as a valid semaphore ID, use FM_CHILD_SEM_INVALID instead
** Revision 1.4 2009/12/02 14:31:19EST lwalling 
** Update FM unit tests to match UTF changes
** Revision 1.3 2009/11/20 15:40:40EST lwalling 
** Unit test updates
** Revision 1.2 2009/11/17 13:40:50EST lwalling 
** Remove global open files list data structure
** Revision 1.1 2009/11/13 16:36:10EST lwalling 
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

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  FM_NoopCmd_t *UT_NoopCmd;

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
/* Unit tests for source file fm_cmd_utils.c                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_utils(void)
{
    char Buffer[32];
    boolean bResult;
    uint32  uResult;

    uint32 TestCount = 0;
    uint32 FailCount = 0;

    CreateTestFile("/ram/open1.bin", 16, TRUE);
    CreateTestFile("/ram/closed1.bin", 16, FALSE);


    /*
    ** Tests for function FM_IsValidCmdPktLength()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  valid command packet length
    */

    /* (1) invalid packet length (too short) */
    CFE_SB_InitMsg(UT_NoopCmd, FM_CMD_MID, sizeof(FM_NoopCmd_t) - 1, TRUE);
    bResult = FM_IsValidCmdPktLength((CFE_SB_MsgPtr_t) UT_NoopCmd, sizeof(FM_NoopCmd_t),
                                        101, "FM_IsValidCmdPktLength");
    TestCount++;
    if (bResult == TRUE)
    {
        UTF_put_text("FM_IsValidCmdPktLength() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CFE_SB_InitMsg(UT_NoopCmd, FM_CMD_MID, sizeof(FM_NoopCmd_t) + 1, TRUE);
    bResult = FM_IsValidCmdPktLength((CFE_SB_MsgPtr_t) UT_NoopCmd, sizeof(FM_NoopCmd_t),
                                        102, "FM_IsValidCmdPktLength");
    TestCount++;
    if (bResult == TRUE)
    {
        UTF_put_text("FM_IsValidCmdPktLength() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) valid packet length */
    CFE_SB_InitMsg(UT_NoopCmd, FM_CMD_MID, sizeof(FM_NoopCmd_t), TRUE);
    bResult = FM_IsValidCmdPktLength((CFE_SB_MsgPtr_t) UT_NoopCmd, sizeof(FM_NoopCmd_t),
                                        103, "FM_IsValidCmdPktLength");
    TestCount++;
    if (bResult == FALSE)
    {
        UTF_put_text("FM_IsValidCmdPktLength() -- test failed (3)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_GetOpenFilesData()...
    **
    **   (1)  get open files data (should have one open file)
    */

    /* (1) get open files data (should have one open file) */
    uResult = FM_GetOpenFilesData(NULL);
    TestCount++;
    if (uResult == 0)
    {
        UTF_put_text("FM_GetOpenFilesData() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_GetFilenameState()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    uResult = FM_GetFilenameState(Buffer, sizeof(Buffer), FALSE);
    TestCount++;
    if (uResult != FM_NAME_IS_INVALID)
    {
        UTF_put_text("FM_GetFilenameState() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/ram/none.no");
    uResult = FM_GetFilenameState(Buffer, sizeof(Buffer), FALSE);
    TestCount++;
    if (uResult != FM_NAME_IS_NOT_IN_USE)
    {
        UTF_put_text("FM_GetFilenameState() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    uResult = FM_GetFilenameState(Buffer, sizeof(Buffer), FALSE);
    TestCount++;
    if (uResult != FM_NAME_IS_FILE_OPEN)
    {
        UTF_put_text("FM_GetFilenameState() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    uResult = FM_GetFilenameState(Buffer, sizeof(Buffer), TRUE);
    TestCount++;
    if (uResult != FM_NAME_IS_FILE_CLOSED)
    {
        UTF_put_text("FM_GetFilenameState() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    uResult = FM_GetFilenameState(Buffer, sizeof(Buffer), TRUE);
    TestCount++;
    if (uResult != FM_NAME_IS_DIRECTORY)
    {
        UTF_put_text("FM_GetFilenameState() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyNameValid()...
    **
    **   (1)  FM_NAME_IS_INVALID (no terminator)
    **   (2)  FM_NAME_IS_NOT_IN_USE
    */

    /* (1) FM_NAME_IS_INVALID (no terminator) */
    strcpy(Buffer, "/a123/b456/c789");
    uResult = FM_VerifyNameValid(Buffer, strlen(Buffer), 101, "FM_VerifyNameValid");
    TestCount++;
    if (uResult != FM_NAME_IS_INVALID)
    {
        UTF_put_text("FM_VerifyNameValid() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/a123/b456/c789");
    uResult = FM_VerifyNameValid(Buffer, strlen(Buffer) + 1, 102, "FM_VerifyNameValid");
    TestCount++;
    if (uResult != FM_NAME_IS_NOT_IN_USE)
    {
        UTF_put_text("FM_VerifyNameValid() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyFileClosed()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    bResult = FM_VerifyFileClosed(Buffer, sizeof(Buffer), 101, "FM_VerifyFileClosed");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileClosed() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/none/none.no");
    bResult = FM_VerifyFileClosed(Buffer, sizeof(Buffer), 102, "FM_VerifyFileClosed");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileClosed() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    bResult = FM_VerifyFileClosed(Buffer, sizeof(Buffer), 103, "FM_VerifyFileClosed");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileClosed() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    bResult = FM_VerifyFileClosed(Buffer, sizeof(Buffer), 104, "FM_VerifyFileClosed");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyFileClosed() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    bResult = FM_VerifyFileClosed(Buffer, sizeof(Buffer), 105, "FM_VerifyFileClosed");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileClosed() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyFileExists()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    bResult = FM_VerifyFileExists(Buffer, sizeof(Buffer), 101, "FM_VerifyFileExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileExists() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/none/none.no");
    bResult = FM_VerifyFileExists(Buffer, sizeof(Buffer), 102, "FM_VerifyFileExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileExists() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    bResult = FM_VerifyFileExists(Buffer, sizeof(Buffer), 103, "FM_VerifyFileExists");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyFileExists() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    bResult = FM_VerifyFileExists(Buffer, sizeof(Buffer), 104, "FM_VerifyFileExists");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyFileExists() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    bResult = FM_VerifyFileExists(Buffer, sizeof(Buffer), 105, "FM_VerifyFileExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileExists() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyFileNoExist()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    bResult = FM_VerifyFileNoExist(Buffer, sizeof(Buffer), 101, "FM_VerifyFileNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNoExist() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/none/none.no");
    bResult = FM_VerifyFileNoExist(Buffer, sizeof(Buffer), 102, "FM_VerifyFileNoExist");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyFileNoExist() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    bResult = FM_VerifyFileNoExist(Buffer, sizeof(Buffer), 103, "FM_VerifyFileNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNoExist() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    bResult = FM_VerifyFileNoExist(Buffer, sizeof(Buffer), 104, "FM_VerifyFileNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNoExist() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    bResult = FM_VerifyFileNoExist(Buffer, sizeof(Buffer), 105, "FM_VerifyFileNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNoExist() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyFileNotOpen()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    bResult = FM_VerifyFileNotOpen(Buffer, sizeof(Buffer), 101, "FM_VerifyFileNotOpen");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNotOpen() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/none/none.no");
    bResult = FM_VerifyFileNotOpen(Buffer, sizeof(Buffer), 102, "FM_VerifyFileNotOpen");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyFileNotOpen() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    bResult = FM_VerifyFileNotOpen(Buffer, sizeof(Buffer), 103, "FM_VerifyFileNotOpen");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNotOpen() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    bResult = FM_VerifyFileNotOpen(Buffer, sizeof(Buffer), 104, "FM_VerifyFileNotOpen");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyFileNotOpen() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    bResult = FM_VerifyFileNotOpen(Buffer, sizeof(Buffer), 105, "FM_VerifyFileNotOpen");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyFileNotOpen() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyDirExists()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    bResult = FM_VerifyDirExists(Buffer, sizeof(Buffer), 101, "FM_VerifyDirExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirExists() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/none/none.no");
    bResult = FM_VerifyDirExists(Buffer, sizeof(Buffer), 102, "FM_VerifyDirExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirExists() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    bResult = FM_VerifyDirExists(Buffer, sizeof(Buffer), 103, "FM_VerifyDirExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirExists() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    bResult = FM_VerifyDirExists(Buffer, sizeof(Buffer), 104, "FM_VerifyDirExists");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirExists() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    bResult = FM_VerifyDirExists(Buffer, sizeof(Buffer), 105, "FM_VerifyDirExists");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyDirExists() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyDirNoExist()...
    **
    **   (1)  FM_NAME_IS_INVALID
    **   (2)  FM_NAME_IS_NOT_IN_USE
    **   (3)  FM_NAME_IS_FILE_OPEN
    **   (4)  FM_NAME_IS_FILE_CLOSED
    **   (5)  FM_NAME_IS_DIRECTORY
    */

    /* (1) FM_NAME_IS_INVALID */
    strcpy(Buffer, "~!@#$%^&*()_+");
    bResult = FM_VerifyDirNoExist(Buffer, sizeof(Buffer), 101, "FM_VerifyDirNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirNoExist() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) FM_NAME_IS_NOT_IN_USE */
    strcpy(Buffer, "/none/none.no");
    bResult = FM_VerifyDirNoExist(Buffer, sizeof(Buffer), 102, "FM_VerifyDirNoExist");
    TestCount++;
    if (bResult != TRUE)
    {
        UTF_put_text("FM_VerifyDirNoExist() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) FM_NAME_IS_FILE_OPEN */
    strcpy(Buffer, "/ram/open1.bin");
    bResult = FM_VerifyDirNoExist(Buffer, sizeof(Buffer), 103, "FM_VerifyDirNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirNoExist() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) FM_NAME_IS_FILE_CLOSED */
    strcpy(Buffer, "/ram/closed1.bin");
    bResult = FM_VerifyDirNoExist(Buffer, sizeof(Buffer), 104, "FM_VerifyDirNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirNoExist() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) FM_NAME_IS_DIRECTORY */
    strcpy(Buffer, "/ram");
    bResult = FM_VerifyDirNoExist(Buffer, sizeof(Buffer), 105, "FM_VerifyDirNoExist");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyDirNoExist() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_VerifyChildTask()...
    **
    **   (1)  Child task command queue is full
    **   (2)  Invalid command queue fill count
    **   (3)  Invalid command queue write index
    **   (4)  Child task semaphore does not exist
    */

    /* (1) Child task command queue is full */
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = FM_CHILD_QUEUE_DEPTH;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    bResult = FM_VerifyChildTask(101, "FM_VerifyChildTask");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyChildTask() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid command queue fill count */
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = FM_CHILD_QUEUE_DEPTH + 1;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    bResult = FM_VerifyChildTask(102, "FM_VerifyChildTask");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyChildTask() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) Invalid command queue write index */
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = FM_CHILD_QUEUE_DEPTH;
    bResult = FM_VerifyChildTask(103, "FM_VerifyChildTask");
    TestCount++;
    if (bResult != FALSE)
    {
        UTF_put_text("FM_VerifyChildTask() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) Child task semaphore does not exist */
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 99;
    FM_GlobalData.ChildReadIndex  = 99;
    FM_GlobalData.ChildWriteIndex = 99;
    bResult = FM_VerifyChildTask(104, "FM_VerifyChildTask");
    TestCount++;
    if (bResult == TRUE)
    {
        UTF_put_text("FM_VerifyChildTask() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_InvokeChildTask()...
    **
    **   (1)  Roll-over command queue write index
    */

    /* (1) Roll-over command queue write index */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = FM_CHILD_QUEUE_DEPTH - 1;
    FM_GlobalData.ChildWriteIndex = FM_CHILD_QUEUE_DEPTH - 1;
    CFE_PSP_MemSet(&FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex],
                   0, sizeof(FM_ChildQueueEntry_t));
    FM_InvokeChildTask();
    TestCount++;
    if (FM_GlobalData.ChildWriteIndex != 0)
    {
        UTF_put_text("FM_InvokeChildTask() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_AppendPathSep()...
    **
    **   (1)  String has terminator but no room for path separator
    **   (2)  String barely has room for path separator
    */

    /* (1) String has terminator but no room for path separator */
    strcpy(Buffer, "/a345/b678");
    uResult = strlen(Buffer);
    FM_AppendPathSep(Buffer, uResult + 1);
    TestCount++;
    if (Buffer[uResult] == '/')
    {
        UTF_put_text("FM_AppendPathSep() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) String barely has room for path separator */
    strcpy(Buffer, "/a345/b678");
    uResult = strlen(Buffer);
    FM_AppendPathSep(Buffer, uResult + 2);
    TestCount++;
    if (Buffer[uResult] != '/')
    {
        UTF_put_text("FM_AppendPathSep() -- test failed (2)\n");
        FailCount++;
    }

    OS_close(OpenFileHandle);
    OS_remove("/ram/open1.bin");
    OS_remove("/ram/closed1.bin");


    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("fm_cmd_utils.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_utils() */








