##############################################################################
## $Id: README.txt 1.1 2016/06/14 16:47:21EDT czogby Exp  $
##
## Purpose: UT-Assert example application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.1 2016/06/14 16:47:21EDT czogby 
## Initial revision
## Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/unit_test/project.pj
##############################################################################


-------------------------
UT-Assert Example Unit Test Instructions
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

UT-Assert Example 1.0.0.0 Unit Test Results:

Tests Executed:    6
Assert Pass Count: 18
Assert Fail Count: 0

gcov: '../src/example_app.c' 100.00%  7
gcov: '../src/example_cmds.c' 100.00%  7

==========================================================================
example_app.c - 100.00% coverage

==========================================================================
example_cmds.c - 100.00% coverage

==========================================================================
