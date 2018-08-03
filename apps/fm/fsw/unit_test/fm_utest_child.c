/************************************************************************
**
** $Id: fm_utest_child.c 1.10 2014/11/13 17:03:57EST lwalling Exp  $
**
** Notes:
**
**   Unit test for CFS File Manager (FM) application source file "fm_child.c"
**
**   To direct text output to screen,
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file,
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
**
** $Log: fm_utest_child.c  $
** Revision 1.10 2014/11/13 17:03:57EST lwalling 
** Modified unit tests to remove temp directories and files after unit tests complete
** Revision 1.9 2014/11/12 11:14:25EST lwalling 
** Modified unit tests to provide greater coverage
** Revision 1.8 2014/10/22 17:50:59EDT lwalling 
** Allow zero as a valid semaphore ID, use FM_CHILD_SEM_INVALID instead
** Revision 1.7 2012/05/02 14:55:51EDT acudmore 
** More fixes to Delete All unit tests
** Revision 1.6 2012/05/02 14:09:04EDT acudmore 
** Updated paths in Delete All unit test
** Revision 1.5 2012/05/02 13:27:48EDT acudmore 
** Updated unit test for DeleteAllCmd changes
** Revision 1.4 2011/04/20 11:29:51EDT lwalling 
** Use signed args instead of unsigned for calls to FM_ChildDeleteOneFile()
** Revision 1.3 2010/02/25 13:47:05EST lwalling 
** Change return type for calls to FM_ChildDirListFileLoop
** Revision 1.2 2009/12/02 14:31:18EST lwalling 
** Update FM unit tests to match UTF changes
** Revision 1.1 2009/11/20 16:12:10EST lwalling 
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
#include "fm_child.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_version.h"
#include "fm_verify.h"

#include <stdlib.h>            /* System headers      */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** Global data external to this file
*************************************************************************/

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  int32 DecompressResult;

extern void CreateTestFile(char *Filename, int SizeInKs, boolean LeaveOpen);
extern int OpenFileHandle;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file fm_child.c                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_child(void)
{
    FM_ChildQueueEntry_t *CmdArgs;
    os_dirp_t DirPtr;
    int32 iResult;
    boolean bResult;
    uint32 uArg1;
    uint32 uArg2;
    int    tempFd;

    uint32 TestCount = 0;
    uint32 FailCount = 0;

    CreateTestFile("/ram/sub/closed1.bin", 16, FALSE);
    CreateTestFile("/ram/sub/closed2.bin", 16, FALSE);
    CreateTestFile("/ram/sub2/closed1.bin", 16, FALSE);
    CreateTestFile("/ram/sub2/sub3/closed1.bin", 16, FALSE);
    CreateTestFile("/ram/sub2/sub3/open1.bin", 16, TRUE);


    /*
    ** Tests for function FM_ChildInit()...
    **
    **   (1)  CFE_ES_CreateChildTask error
    */

    /* (1) CFE_ES_CreateChildTask error */
    FM_GlobalData.ChildTaskID = 0;
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_CREATECHILDTASK_PROC, -1);
    FM_ChildInit();
	UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_CREATECHILDTASK_PROC);
    TestCount++;
    if (FM_GlobalData.ChildTaskID != 0)
    {
        UTF_put_text("FM_ChildInit() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildTask()...
    **
    **   (1)  CFE_ES_RegisterChildTask error
    **   (2)  OS_CountSemCreate error
    **   (3)  OS_CountSemTake error
    **   (4)  ChildReadIndex error
    */

    /* (1) CFE_ES_CreateChildTask error */
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCHILDTASK_PROC, -1);
    FM_ChildTask();
	UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERCHILDTASK_PROC);
    TestCount++;
    /* There is no failure case for test (1) */

    /* (2) OS_CountSemCreate error */
    FM_GlobalData.ChildSemaphore = OS_MAX_COUNT_SEMAPHORES + 1;
    FM_ChildTask();
    TestCount++;
    if (FM_GlobalData.ChildSemaphore != FM_CHILD_SEM_INVALID)
    {
        UTF_put_text("FM_ChildTask() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) OS_CountSemTake error */
    FM_GlobalData.ChildSemaphore = OS_MAX_COUNT_SEMAPHORES;
    FM_ChildTask();
    TestCount++;
    if (FM_GlobalData.ChildSemaphore != FM_CHILD_SEM_INVALID)
    {
        UTF_put_text("FM_ChildTask() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) ChildReadIndex error */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildReadIndex = FM_CHILD_QUEUE_DEPTH;
    FM_ChildTask();
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildTask() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildProcess()...
    **
    **   (1)  Invalid command code
    **   (2)  Invalid command queue read index
    */

    /* (1) Invalid command code */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_PSP_MemSet(&FM_GlobalData.ChildQueue[0], 0, sizeof(FM_ChildQueueEntry_t));
    FM_ChildProcess();
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildProcess() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid command queue read index */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildReadIndex  = FM_CHILD_QUEUE_DEPTH;
    FM_GlobalData.ChildWriteIndex = 0;
    CFE_PSP_MemSet(&FM_GlobalData.ChildQueue[0], 0, sizeof(FM_ChildQueueEntry_t));
    FM_ChildProcess();
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildProcess() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildCopyCmd()...
    **
    **   (1)  Invalid source filename
    */

    /* (1) Invalid source filename */
/*
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist.bin");
    strcpy(CmdArgs->Target, "/ram/sub/copy99.bin");
    FM_ChildCopyCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildCopyCmd() -- test failed (1)\n");
        FailCount++;
    }
*/

    /*
    ** Tests for function FM_ChildDeleteAllCmd()...
    **
    **   (1)  Invalid directory name
    **   (2)  Skip a subdirectory 
    **   (3)  Skip an open file in the directory 
    **   (4)  OS_remove error
    */

    /* (1) Invalid directory name */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist");
    FM_ChildDeleteAllCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildDeleteAllCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Skip a subdirectory */
    /*    The directory has subidrectories that will be skipped */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub2");
    FM_ChildDeleteAllCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildDeleteAllCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) Skip an open file in the directory */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub2/sub3");
    FM_ChildDeleteAllCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildDeleteAllCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) OS_remove error */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_REMOVE_PROC, -1);
    FM_ChildDeleteAllCmd(CmdArgs);
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_REMOVE_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildDeleteAllCmd() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildDeleteDirCmd()...
    **
    **   (1)  Invalid directory name
    */

    /* (1) Invalid directory name */
    FM_GlobalData.ChildCmdErrCounter = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist");
    FM_ChildDeleteDirCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildDeleteDirCmd() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildDecompressCmd()...
    **
    **   (1)  Invalid source filename
    */

    /* (1) Invalid source filename */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist");
    strcpy(CmdArgs->Target, "/ram/sub/zip1.bin");
    DecompressResult = -1;
    FM_ChildDecompressCmd(CmdArgs);
    DecompressResult = CFE_SUCCESS;
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildDecompressCmd() -- test failed (1)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_ChildConcatCmd()...
    **
    **   (1)  Invalid source1 filename
    **   (2)  Invalid source2 filename
    **   (3)  Invalid target filename
    */

    /* (1) Invalid source1 filename */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist");
    strcpy(CmdArgs->Source2, "/ram/sub/closed1.bin");
    strcpy(CmdArgs->Target, "/ram/sub/cc1.bin");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_CP_PROC, -1);
    FM_ChildConcatCmd(CmdArgs);
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CP_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildConcatCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid source2 filename */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/closed1.bin");
    strcpy(CmdArgs->Source2, "/ram/doesnotexist");
    strcpy(CmdArgs->Target, "/ram/sub/cc2.bin");
    FM_ChildConcatCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildConcatCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) Read from source failure */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/closed1.bin");
    strcpy(CmdArgs->Source2, "/ram/sub/closed2.bin");
    strcpy(CmdArgs->Target, "/ram/sub/cc3.bin");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_READ_PROC, -1);
    FM_ChildConcatCmd(CmdArgs);
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_READ_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildConcatCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) Write to target failure */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/closed1.bin");
    strcpy(CmdArgs->Source2, "/ram/sub/closed2.bin");
    strcpy(CmdArgs->Target, "/ram/sub/cc4.bin");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_WRITE_PROC, -1);
    FM_ChildConcatCmd(CmdArgs);
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_WRITE_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildConcatCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) Success */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/closed1.bin");
    strcpy(CmdArgs->Source2, "/ram/sub/closed2.bin");
    strcpy(CmdArgs->Target, "/ram/sub/cc5.bin");
    FM_ChildConcatCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter != 0)
    {
        UTF_put_text("FM_ChildConcatCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) Success - large enough for task delay */
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/cc5.bin");
    strcpy(CmdArgs->Source2, "/ram/sub/cc5.bin");
    strcpy(CmdArgs->Target, "/ram/sub/cc6.bin");
    FM_ChildConcatCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter != 0)
    {
        UTF_put_text("FM_ChildConcatCmd() -- test failed (6)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildFileInfoCmd()...
    **
    **   (1)  Invalid source filename, CRC = FM_IGNORE_CRC
    **   (2)  Invalid source filename, CRC = 999
    **   (3)  Invalid source filename, CRC = CFE_ES_CRC_16
    **   (4)  Valid source filename, CRC = 999
    **   (5)  Valid source filename, CRC = CFE_ES_CRC_16, no file
    **   (6)  Valid source filename, CRC = CFE_ES_CRC_16, read error
    **   (7)  Valid source filename, CRC = CFE_ES_CRC_16
    */

    /* (1) Invalid source filename, CRC = FM_IGNORE_CRC */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist1");
    CmdArgs->FileInfoCRC = FM_IGNORE_CRC;
    CmdArgs->FileInfoState = FM_NAME_IS_INVALID;
    CmdArgs->FileInfoSize  = 1;
    CmdArgs->FileInfoTime  = 1;
    FM_ChildFileInfoCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter != 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid source filename, CRC = 999 */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist2");
    CmdArgs->FileInfoCRC = 999;
    CmdArgs->FileInfoState = FM_NAME_IS_INVALID;
    CmdArgs->FileInfoSize  = 2;
    CmdArgs->FileInfoTime  = 2;
    FM_ChildFileInfoCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) Invalid source filename, CRC = CFE_ES_CRC_16 */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/doesnotexist3");
    CmdArgs->FileInfoCRC = CFE_ES_CRC_16;
    CmdArgs->FileInfoState = FM_NAME_IS_INVALID;
    CmdArgs->FileInfoSize  = 3;
    CmdArgs->FileInfoTime  = 3;
    FM_ChildFileInfoCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) Valid source filename, CRC = 999 */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/closed1.bin");
    CmdArgs->FileInfoCRC = 999;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;
    CmdArgs->FileInfoSize  = 4;
    CmdArgs->FileInfoTime  = 4;
    FM_ChildFileInfoCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) Valid source filename, CRC = CFE_ES_CRC_16, no file */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/closed1.bin");
    CmdArgs->FileInfoCRC = CFE_ES_CRC_16;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;
    CmdArgs->FileInfoSize  = 5;
    CmdArgs->FileInfoTime  = 5;
    FM_ChildFileInfoCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter == 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) Valid source filename, CRC = CFE_ES_CRC_16, read error */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/cc6.bin");
    CmdArgs->FileInfoCRC = CFE_ES_CRC_16;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;
    CmdArgs->FileInfoSize  = 6;
    CmdArgs->FileInfoTime  = 6;
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_READ_PROC, -1);
    FM_ChildFileInfoCmd(CmdArgs);
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_READ_PROC);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) Valid source filename, CRC = CFE_ES_CRC_16 */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/sub/cc6.bin");
    CmdArgs->FileInfoCRC = CFE_ES_CRC_16;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;
    CmdArgs->FileInfoSize  = 7;
    CmdArgs->FileInfoTime  = 7;
    FM_ChildFileInfoCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter != 0)
    {
        UTF_put_text("FM_ChildFileInfoCmd() -- test failed (7)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildDirListFileCmd()...
    **
    **   (1)  Invalid directory name
    */

    /* (1) Invalid directory name */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/xyz");
    strcpy(CmdArgs->Source2, "/ram/xyz/");
    strcpy(CmdArgs->Target, "/ram/sub/dirlist1.bin");
    FM_ChildDirListFileCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildDirListFileCmd() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildDirListPktCmd()...
    **
    **   (1)  Invalid directory name
    **   (2)  Invalid directory name length
    */

    /* (1) Invalid directory name */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram/xyz");
    strcpy(CmdArgs->Source2, "/ram/xyz/");
    CmdArgs->DirListOffset = 0;
    FM_ChildDirListPktCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdErrCounter == 0)
    {
        UTF_put_text("FM_ChildDirListPktCmd() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid directory name length */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    FM_GlobalData.ChildSemaphore  = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCount = 0;
    FM_GlobalData.ChildReadIndex  = 0;
    FM_GlobalData.ChildWriteIndex = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    strcpy(CmdArgs->Source1, "/ram");
    CFE_PSP_MemSet(CmdArgs->Source2, 'a', OS_MAX_PATH_LEN - 1);
    CmdArgs->DirListOffset = 0;
    FM_ChildDirListPktCmd(CmdArgs);
    TestCount++;
    if (FM_GlobalData.ChildCmdWarnCounter == 0)
    {
        UTF_put_text("FM_ChildDirListPktCmd() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildDirListFileInit()...
    **
    **   (1)  Invalid output filename
    **   (2)  File write error
    */

    /* (1) Invalid output filename */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    iResult = 0;
    bResult = FM_ChildDirListFileInit(&iResult, "/ram", "/ram/xxx/outfile.bin");
    TestCount++;
    if ((bResult == TRUE) || (iResult != 0))
    {
        UTF_put_text("FM_ChildDirListFileInit() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) File write error */
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    iResult = 0;
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_WRITE_PROC, -1);
    bResult = FM_ChildDirListFileInit(&iResult, "/ram", "/ram/sub/list2.bin");
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_WRITE_PROC);
    TestCount++;
    if ((bResult == TRUE) || (iResult != 0))
    {
        UTF_put_text("FM_ChildDirListFileInit() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ChildDirListFileLoop()...
    **
    **   (1)  Invalid file handle, write dir entry error
    **   (2)  Invalid file handle, name too long warnings, write stats error
    */

    /* (1) Invalid file handle, empty subdir, write stats error */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    DirPtr = OS_opendir("/ram/sub");
    FM_ChildDirListFileLoop(DirPtr, 99, "directory", "/ram/sub/", "filename");
    OS_closedir(DirPtr);
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter != 0)
    {
        UTF_put_text("FM_ChildDirListFileLoop() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid file handle, name too long warnings, write stats error */
    FM_GlobalData.ChildCmdCounter = 0;
    FM_GlobalData.ChildCmdWarnCounter = 0;
    FM_GlobalData.ChildCmdErrCounter = 0;
    CmdArgs = &FM_GlobalData.ChildQueue[0];
    CFE_PSP_MemSet(CmdArgs, 0, sizeof(FM_ChildQueueEntry_t));
    CFE_PSP_MemSet(CmdArgs->Source1, 'a', OS_MAX_PATH_LEN - 1);
    DirPtr = OS_opendir("/ram/sub");
    FM_ChildDirListFileLoop(DirPtr, 99, "directory", CmdArgs->Source1, "filename");
    OS_closedir(DirPtr);
    TestCount++;
    if (FM_GlobalData.ChildCmdCounter != 0)
    {
        UTF_put_text("FM_ChildDirListFileLoop() -- test failed (2)\n");
        FailCount++;
    }

    /*
    ** Tests for function FM_ChildSizeAndTime()...
    **
    **   (1)  Filename is invalid
    **   (2)  Filename is valid
    */

    /* (1) Filename is invalid */
    uArg1 = 0;
    uArg2 = 0;
    iResult = FM_ChildSizeAndTime("/ram/xxx.bin", &uArg1, &uArg2);
    TestCount++;
    if ((uArg1 != 0) || (uArg2 != 0))
    {
        UTF_put_text("FM_ChildSizeAndTime() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Filename is valid */
    uArg1 = 0;
    uArg2 = 0;
    iResult = FM_ChildSizeAndTime("/ram/sub/closed1.bin", &uArg1, &uArg2);
    TestCount++;
    if ((uArg1 == 0) || (uArg2 == 0))
    {
        UTF_put_text("FM_ChildSizeAndTime() -- test failed (2)\n");
        FailCount++;
    }


    OS_close(OpenFileHandle);
    OS_remove("/ram/sub2/sub3/open1.bin");
    OS_remove("/ram/sub2/closed1.bin");
    OS_remove("/ram/sub/closed1.bin");
    OS_remove("/ram/sub/closed2.bin");
    OS_remove("/ram/sub/cc5.bin");
    OS_remove("/ram/sub/cc6.bin");
    OS_remove("/ram/sub/list2.bin");

    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("fm_child.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_child() */








