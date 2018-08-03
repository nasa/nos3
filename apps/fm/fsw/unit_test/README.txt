##############################################################################
## $Id: README.txt 1.6 2009/11/20 15:42:51EST lwalling Exp  $
##
## Purpose: CFS File Manager (FM) application unit test statistics page
##
## Author: S. Walling
##
## $Log: README.txt  $
## Revision 1.6 2009/11/20 15:42:51EST lwalling 
## Unit test updates
##
##############################################################################


These unit test results match source code dated November 20, 2009
-----------------------------------------------------------------


-------------------------
FM Unit Test Instructions
-------------------------

The host machine is a desktop PC with Windows XP pro
The unit tests are run in a cygwin session
GNU bash, version 3.00.16(14)-release (i686-pc-cygwin)
Install Cygwin with "all" options unless you know better

Change directory to .../fm/fsw/unit_test
Location depends on installation of CFS build release

Edit the makefile by modifying the search path information shown below

#
# Search path definitions
#
UTF_PATH=/cygdrive/c/sandbox/cfe/tools/utf
CFE_PATH=/cygdrive/c/sandbox/cfe
CFS_PATH=/cygdrive/c/sandbox/cfs
PSP_PATH=/cygdrive/c/sandbox/cfe-psp
OSAL_PATH=/cygdrive/c/sandbox/cfe-osal
BUILD_PATH=/cygdrive/c/sandbox/cfe/fsw/build
#

Enter the following commands at the command prompt:

make clean
make
./fm_utest.exe  --- to run the unit tests

Inspect the unit test output text file (fm_utest.out)

Note that the output file contains a great many error events
and lines of error text. This is to be expected as much of the
unit test effort is to exercise software error handlers.

The overall results are near the bottom of the output file:

*** FM -- Total test count = 191, total test errors = 0

Each section of the output file has intermediate results:

fm_child.c -- test count = 37, test errors = 0
fm_tbl.c -- test count = 6, test errors = 0




----------------------------------------
FM Unit Test Overall Coverage Statistics
----------------------------------------

File `../src/fm_app.c'
  Lines executed:100.00% of 120



File `../src/fm_cmd_utils.c'
  Lines executed:100.00% of 181



File `../src/fm_tbl.c'
  Lines executed:100.00% of 41



File `../src/fm_cmds.c'
  Lines executed:99.36% of 314

    function FM_DeleteDirectoryCmd called 6 returned 100% blocks executed 96%

        3:  742:        DirPtr = OS_opendir(CmdPtr->Directory);
        -:  743:
        3:  744:        if (DirPtr == NULL)
        -:  745:        {
    #####:  746:            CommandResult = FALSE;
        -:  747:
    #####:  748:            CFE_EVS_SendEvent(FM_DELETE_DIR_OS_ERR_EID, CFE_EVS_ERROR,
        -:  749:                             "%s error: OS_opendir failed: dir = %s",
        -:  750:                              CmdText, CmdPtr->Directory);
        -:  751:        }



File `../src/fm_child.c'
  Lines executed:98.52% of 406

    function FM_ChildTask called 3 returned 100% blocks executed 90%

        -:  116:                /* Pend on the "handshake" semaphore */
        1:  117:                Result = OS_CountSemTake(FM_GlobalData.ChildSemaphore);
        -:  118:
        -:  119:                /* Mark the period when this task is active */
        1:  120:                CFE_ES_PerfLogEntry(FM_CHILD_TASK_PERF_ID);
        -:  121:
        1:  122:                if (Result == CFE_SUCCESS)
        -:  123:                {
        -:  124:                    /* Invoke the child task command handler */
    #####:  125:                    FM_ChildProcess();
        -:  126:                }


    function FM_ChildConcatCmd called 7 returned 100% blocks executed 96%

        5:  452:        FileHandleTgt = OS_open(CmdArgs->Target, OS_READ_WRITE, 0);
        -:  453:
        5:  454:        if (FileHandleTgt < 0)
        -:  455:        {
    #####:  456:            FM_GlobalData.ChildCmdErrCounter++;
        -:  457:
        -:  458:            /* Send command failure event (error) */
    #####:  459:            CFE_EVS_SendEvent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR,
        -:  460:               "%s error: OS_open failed: result = %d, tgt = %s",
        -:  461:                CmdText, FileHandleTgt, CmdArgs->Target);
        -:  462:        }


    function FM_ChildDirListFileInit called 3 returned 100% blocks executed 89%

        -:  940:            /* Write blank FM directory statistics structure as a place holder */
        1:  941:            BytesWritten = OS_write(FileHandle, &FM_GlobalData.DirListFileStats, sizeof(FM_DirListFileStats_t));
        1:  942:            if (BytesWritten == sizeof(FM_DirListFileStats_t))
        -:  943:            {
        -:  944:                /* Return output file handle */
        1:  945:                *FileHandlePtr = FileHandle;
        -:  946:            }
        -:  947:            else
        -:  948:            {
    #####:  949:                CommandResult = FALSE;
    #####:  950:                FM_GlobalData.ChildCmdErrCounter++;
        -:  951:
        -:  952:                /* Send command failure event (error) */
    #####:  953:                CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
        -:  954:                   "%s error: OS_write blank stats failed: result = %d, expected = %d",
        -:  955:                    CmdText, BytesWritten, sizeof(FM_DirListFileStats_t));
        -:  956:            }


