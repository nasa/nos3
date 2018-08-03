##############################################################################
## $Id: README.txt 1.1 2009/12/07 13:43:53EST lwalling Exp  $
##
## Purpose: CFS Data Storage (DS) application unit test statistics page
##
## Author: S. Walling
##
## $Log: README.txt  $
## Revision 1.1 2009/12/07 13:43:53EST lwalling 
## Initial revision
## Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/unit_test/project.pj
##
##############################################################################


These unit test results match source code dated November 22, 2009
-----------------------------------------------------------------


-------------------------
DS Unit Test Instructions
-------------------------

The host machine is a desktop PC with Windows XP pro
The unit tests are run in a cygwin session
GNU bash, version 3.00.16(14)-release (i686-pc-cygwin)
Install Cygwin with "all" options unless you know better

Change directory to .../ds/fsw/unit_test
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
./ds_utest.exe  --- to run the unit tests

Inspect the unit test output text file (ds_utest.out)

Note that the output file contains a great many error events
and lines of error text. This is to be expected as much of the
unit test effort is to exercise software error handlers.

The overall results are near the bottom of the output file:

*** DS -- Total test count = 191, total test errors = 0

Each section of the output file has intermediate results:

ds_app.c -- test count = 20, test errors = 0
ds_cmds.c -- test count = 92, test errors = 0




----------------------------------------
FM Unit Test Overall Coverage Statistics
----------------------------------------

File `../src/ds_app.c'
  Lines executed:100.00% of 138



File `../src/ds_cmds.c'
  Lines executed:100.00% of 355



File `../src/ds_file.c'
  Lines executed:98.95% of 191

    function DS_FileWriteHeader called 5 returned 100% blocks executed 85%

        4:  295:        Result = OS_write(FileStatus->FileHandle, &DS_FileHeader, sizeof(DS_FileHeader_t));
        -:  296:
        4:  297:        if (Result == sizeof(DS_FileHeader_t))
        -:  298:        {
        -:  299:            /*
        -:  300:            ** Success - update file size and data rate counters...
        -:  301:            */
        4:  302:            DS_AppData.FileWriteCounter++;
        -:  303:
        4:  304:            FileStatus->FileSize   += sizeof(DS_FileHeader_t);
        4:  305:            FileStatus->FileGrowth += sizeof(DS_FileHeader_t);
        -:  306:        }
        -:  307:        else
        -:  308:        {
        -:  309:            /*
        -:  310:            ** Error - send event, close file and disable destination...
        -:  311:            */
    #####:  312:            DS_FileWriteError(FileIndex, sizeof(DS_FileHeader_t), Result);
        -:  313:        }


    function DS_FileUpdateHeader called 5 returned 100% blocks executed 89%

        3:  710:        Result = OS_write(FileStatus->FileHandle, &CurrentTime, sizeof(CFE_TIME_SysTime_t));
        -:  711:
        3:  712:        if (Result == sizeof(CFE_TIME_SysTime_t))
        -:  713:        {
        3:  714:            DS_AppData.FileUpdateCounter++;
        -:  715:        }
        -:  716:        else
        -:  717:        {
    #####:  718:            DS_AppData.FileUpdateErrCounter++;
        -:  719:        }



File `../src/ds_table.c'
  Lines executed:99.33% of 297

    function DS_TableManageDestFile called 12 returned 100% blocks executed 93%

        7:  263:        Result = CFE_TBL_GetStatus(DS_AppData.DestFileTblHandle);
        -:  264:
        7:  265:        if (Result == CFE_TBL_INFO_DUMP_PENDING)
        -:  266:        {
        -:  267:            /*
        -:  268:            ** Dump the current table data...
        -:  269:            */
    #####:  270:            CFE_TBL_DumpToBuffer(DS_AppData.DestFileTblHandle);       
        -:  271:        }


    function DS_TableManageFilter called 12 returned 100% blocks executed 91%

        7:  367:        Result = CFE_TBL_GetStatus(DS_AppData.FilterTblHandle);
        -:  368:
        7:  369:        if (Result == CFE_TBL_INFO_DUMP_PENDING)
        -:  370:        {
        -:  371:            /*
        -:  372:            ** Dump the current filter table data...
        -:  373:            */
    #####:  374:            CFE_TBL_DumpToBuffer(DS_AppData.FilterTblHandle);       
        -:  375:        }




