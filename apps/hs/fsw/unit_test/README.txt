##############################################################################
## $Id: README.txt 1.3 2016/09/07 19:19:15EDT mdeschu Exp  $
##
## Purpose: CFS HS application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.3 2016/09/07 19:19:15EDT mdeschu 
## Update readme and logs
## Revision 1.2 2016/08/19 14:07:21EDT czogby 
## HS UT-Assert Unit Tests - Code Walkthrough Updates
## Revision 1.1 2016/06/24 14:31:55EDT czogby 
## Initial revision
## Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
##############################################################################


-------------------------
HS Unit Test Instructions
-------------------------
This unit test was run in a virtual machine running CentOS and uses the ut-assert stubs and default hooks for the 
cFE, OSAL and PSP. The ut-assert framework, stubs, and hooks are located in the directory cfe/tools/ut-assert and 
are configuration managed in MKS in the FSW tools repository on server tlserver3.

To run the unit test enter the following commands at the command line prompt in
unit test directory.

make clean
make 
make run
make gcov

NOTE: Some tests are dependent on changing certain values in ./hs_platform_cfg.h.  Because of this, the test suite should be 
run twice, in the following two configurations: default (HS_MAX_EXEC_CNT_SLOTS = 32) and 
alternate (HS_MAX_EXEC_CNT_SLOTS = 0).  Expected results for each configuration are shown in 
./hs_test_log_defaultconfig.txt and ./hs_test_log_altconfig.txt.

HS 2.3.0.0 Unit Test Results (with default configuration of hs_platform_cfg.h: HS_MAX_EXEC_CNT_SLOTS = 32):

Tests Executed:    142
Assert Pass Count: 679
Assert Fail Count: 0

gcov: '../src/hs_custom.c' 100.00%  146
gcov: '../src/hs_app.c' 99.49%  195
gcov: '../src/hs_cmds.c' 100.00%  275
gcov: '../src/hs_monitors.c' 100.00%  259

==========================================================================
hs_custom.c - 100.00% coverage

==========================================================================
hs_app.c - 99.49% coverage

This file actually has 100% coverage, but one conditional statement that takes up 2 lines always has the 2nd line marked as unexecuted, even though it is executed, due to a bug in gcov.

==========================================================================
hs_cmds.c - 100.00% coverage

==========================================================================
hs_monitors.c - 100.00% coverage

==========================================================================

HS 2.3.0.0 Unit Test Results (with alternate configuration of hs_platform_cfg.h: HS_MAX_EXEC_CNT_SLOTS = 0):

Tests Executed:    106
Assert Pass Count: 433
Assert Fail Count: 0

gcov: '../src/hs_custom.c' 100.00%  146
gcov: '../src/hs_app.c' 49.46%  186
gcov: '../src/hs_cmds.c' 90.32%  248
gcov: '../src/hs_monitors.c' 100.00%  232

==========================================================================
hs_custom.c - 100.00% coverage

=========================================================================
hs_app.c - 49.46% coverage

Low coverage percentage is because much of the code (and code of functions) is skipped when HS_MAX_EXEC_CNT_SLOTS = 0,
and so many of the unit tests are skipped in this case.

==========================================================================
hs_cmds.c - 90.32% coverage

Low coverage percentage is because much of the code (and code of functions) is skipped when HS_MAX_EXEC_CNT_SLOTS = 0,
and so many of the unit tests are skipped in this case.

==========================================================================
hs_monitors.c - 100.00% coverage

=========================================================================
