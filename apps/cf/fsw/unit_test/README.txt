##############################################################################
## $Id: README.txt 1.2 2011/05/13 14:59:27EDT rmcgraw Exp  $
##
## Purpose: CFS CFDP (CF) application unit test statistics page
##
## Author: R. McGraw
##
## $Log: README.txt  $
## Revision 1.2 2011/05/13 14:59:27EDT rmcgraw 
## DCR13439:1 Added platform config param CF_STARTUP_SYNC_TIMEOUT
## Revision 1.1 2011/05/04 09:59:17EDT rmcgraw 
## Initial revision
## Member added to project
##
##############################################################################


-------------------------
CF Unit Test Instructions
-------------------------

This unit test was run on a Mac Book Pro and uses the ut-assert stubs for the 
cFE, OSAL and PSP. There is a copy of these stubs in the unit_test directory.
The ut-assert framework and stubs came from the FSW tools repository on
MKS server tlserver3. It is recommended that the ut-assert directory included in
the unit_test directory be left untouched. In other words, do not attempt
to update the ut-assert directory with the latest stubs from the FSW Tools 
repository on MKS tlserver3. 

This unit test uses header files from personal sandboxes of cFE6.0.0, OSAL3.1,
and PSP version 1.1. So you must have these officially released sandboxes on
your machine and the makefile must be updated to point to them.

Also, this test uses the cf_platform_cfg.h, cf_msgids.h and cf_cfgtable.c files
that are located in the unit_test directory.

To run the unit test enter the following commands at the command line prompt in
unit test directory.

make clean
make 
make run
make gcov

Due to schedule constraints, it was decided to end the unit test before 100%
coverage was achieved on all files. 

CF 2.1.1.0 Unit Test Results

Tests Executed:    206
Assert Pass Count: 651
Assert Fail Count: 6


Reason for Failures in CF Unit test


Code does not check cmd length when receiving a wakeup cmd
Running Test: Test_CF_WakeupCmdInvLen
FAIL: Event Count = 1, File: cf_testcase.c, Line: 1260
FAIL: Error Event Sent, File: cf_testcase.c, Line: 1261
FAIL: CF_AppData.Hk.ErrCounter = 1, File: cf_testcase.c, Line: 1262


Running Test: Test_CF_SetMibCmdUntermValue
/* Test partially commented out because engine tries to print unterminated string ****/
/* should be uncommented when CF intercepts the unterminated 'Value' in CF2.2.0 */
FAIL: Event Count = 1, File: cf_testcase.c, Line: 1900
FAIL: Error Event Sent, File: cf_testcase.c, Line: 1901
FAIL: CF_AppData.Hk.ErrCounter = 1, File: cf_testcase.c, Line: 1902


gcov: '../src/cf_app.c' 99.16%  357
gcov: '../src/cf_utils.c' 69.82%  381
gcov: '../src/cf_callbacks.c' 51.21%  371
gcov: '../src/cf_cmds.c' 98.98%  688
gcov: '../src/cf_playback.c' 92.08%  366
