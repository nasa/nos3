##############################################################################
## $Id: README.txt 1.7 2017/03/29 19:30:34EDT mdeschu Exp  $
##
## Purpose: CFS CS application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
##############################################################################


-------------------------
CS Unit Test Instructions
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

CS 2.4.0 Unit Test Results:


Tests Executed:    319
Assert Pass Count: 1458
Assert Fail Count: 0

gcov: '../src/cs_table_processing.c' 100.00%  449
gcov: '../src/cs_memory_cmds.c' 100.00%  150
gcov: '../src/cs_compute.c' 100.00%  371
gcov: '../src/cs_app_cmds.c' 100.00%  89
gcov: '../src/cs_app.c' 100.00%  324
gcov: '../src/cs_cmds.c' 100.00%  208
gcov: '../src/cs_eeprom_cmds.c' 100.00%  149
gcov: '../src/cs_table_cmds.c' 100.00%  89
gcov: '../src/cs_utils.c' 100.00%  321

                
==========================================================================
cs_table_processing.c - 100.00% coverage

==========================================================================
cs_memory_cmds.c - 100.00% coverage

==========================================================================
cs_compute.c - 100.00% coverage

==========================================================================
cs_app_cmds.c - 100.00% coverage

==========================================================================
cs_app.c - 100.00% coverage

==========================================================================
cs_cmds.c - 100.00% coverage

==========================================================================
cs_eeprom_cmds.c - 100.00% coverage

==========================================================================
cs_table_cmds.c - 100.00% coverage

==========================================================================
cs_utils.c - 100.00% coverage

==========================================================================
