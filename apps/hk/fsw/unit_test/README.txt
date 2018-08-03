
                     -----------------------------------
                     | README.TXT for HK App Unit Test |
                     -----------------------------------


--------
Contents
--------

1. Environment
2. Make Instructions
  2.1 Compiler options and build sequence
  2.2 Directory/Folder Layout

3. Overall Coverage Statistics
  3.1 Comments on Functions with less than 100% Coverage



-----------
Environment
-----------

The round of unit tests run under CFS_FSW_DCR 11542 were executed in a 64-bit CentOS (Linux) Virtual 
Machine running under Parallels on an iMac.  Although it is a 64-bit OS, the code was compiled under 
a 32-bit configuration.

The makefile psp/fsw/pc-linux/make/compiler-opts.mak includes the following modified line:

     ARCH_OPTS=-m32 -fPIC
     
This allows the UTF files to be built as 32-bit code under the 64-bit OS.

The unit test reads instructions from the input file cs_utf_cmds.in (located in the unit_test directory), and 
generates an output file named cs_utf_test.out.


-----------------
Make instructions
-----------------


-----------------------------------
Compiler Options and Build Sequence
-----------------------------------

For the 64-bit CentOS environment, I had to add "-m32 -fPIC" to the DEFAULT-COPT definition, and "-m32" to the
LOPT definition in the unit test makefile.

The unit test makefile (named "makefile") contains options to build the unit test plus the tables.  The makefile 
makes a copy of the default table for use in the unit test.  It currently contains the same data as the default
table.

To re-run the unit test, perform the following steps in the /apps/hk/fsw/unit-test directory:

  make clean
  make
  ./utf_test_cs.bin
  make gcov

This will generate fresh coverage reports and output files.


-----------------------
Directory/Folder layout
-----------------------

The makefile uses relative paths (from the unit_test directory) to find the appropriate header and source files.  
The directory layout used is:


<top-level path>
 |
 |--cfs-mission <--from the "cfs-mission" MKS project
 |   |
 |   |--apps <-- from the "CFS_REPOSITORY" MKS project
 |   |   |
 |   |   |--hk
 |   |   |   |
 |   |   |   |--docs
 |   |   |   |
 |   |   |   |--fsw
 |   |   |   |   |
 |   |   |   |   |--for_build (not used in HK unit test)
 |   |   |   |   |
 |   |   |   |   |--mission_inc
 |   |   |   |   |
 |   |   |   |   |--platform_inc
 |   |   |   |   |
 |   |   |   |   |--public_inc
 |   |   |   |   |
 |   |   |   |   |--src
 |   |   |   |   |
 |   |   |   |   |--tables
 |   |   |   |   |
 |   |   |   |   |--unit_test <------- HK unit test is compiled and run from this directory
 |   |   |   |       |
 |   |   |   |       |--cf
 |   |   |   |       |   |
 |   |   |   |       |   |--apps
 |   |   |   |       |
 |   |   |   |       |--ram <--------- (No longer used for HK unit test)
 |   |   |   |       |
 |   |   |   |       |--cfe_hdr_files <-------- (No longer used for HK unit test)
 |   |   |   |
 |   |   |   |--test_and_ground (not used in HK unit test)
 |   |   |   
 |   |   |--inc (not used in HK unit test)   
 |   |
 |   |--build (not used in HK unit test)
 |   |
 |   |--cfe <--from the "MKS-CFE-PROJECT" MKS project
 |   |   |
 |   |   |--tools
 |   |   |   |
 |   |   |   |--elf2cfetbl
 |   |   |   |
 |   |   |   |--utf
 |   |   |       |
 |   |   |       |--src
 |   |   |       |
 |   |   |       |--inc
 |   |   |   
 |   |   |
 |   |   |--fsw
 |   |       |
 |   |       |--mission_inc
 |   |       |
 |   |       |--platform_inc
 |   |       |   |
 |   |       |   |--cpu1
 |   |       |    
 |   |       |--cfe-core
 |   |           |
 |   |           |--src
 |   |           |   |
 |   |           |   |--sb
 |   |           |   |
 |   |           |   |--time
 |   |           |   |
 |   |           |   |--es
 |   |           |   |
 |   |           |   |--evs
 |   |           |   |
 |   |           |   |--fs
 |   |           |   |
 |   |           |   |--tbl
 |   |           |   |
 |   |           |   |--inc
 |   |           |   
 |   |           |--os
 |   |               |
 |   |               |--inc
 |   |
 |   |--docs (not used in HK unit test)
 |   |
 |   |
 |   |--osal <--from the "MKS-OSAL-REPOSITORY" MKS project
 |   |   |
 |   |   |--build
 |   |   |   |
 |   |   |   |--inc
 |   |   |
 |   |   |--src
 |   |       |
 |   |       |--os
 |   |       |   |
 |   |       |   |--inc
 |   |       |   |
 |   |       |   |--posix
 |   |       |
 |   |       |--bsp
 |   |           |
 |   |           |--pc-linux
 |   |               |
 |   |               |--src
 |   |
 |   |
 |   |--psp <--from the "CFE-PSP-REPOSITORY" MKS project
 |       |
 |       |--fsw
 |           |
 |           |--inc
 |           |
 |           |--pc-linux
 |               |
 |               |--src
 |               |
 |               |--inc
 |
      


---------------------------
Overall Coverage Statistics
---------------------------

                                                     Lines executed       Branches executed
                Filename                          Percent       Total   Percent         Total
gcov:   '../src/hk_app.c'                         100.00%       143      100.00%        53
gcov:   '../src/hk_utils.c'                      * 99.32%       148      100.00%        96
Processed 32 records
 
There are 3 filenames to process
 Removing hk_cpy_tbl.c.gcov from argument list (does not exist)
hk_utils.c.gcov:159: warning: branch  3 taken 0%
hk_utils.c.gcov:181: warning: branch  1 taken 0%
hk_utils.c.gcov:191: warning: branch  1 taken 0%
hk_utils.c.gcov:228: warning: branch  1 taken 0%
hk_utils.c.gcov:307: warning: branch  3 taken 0%
hk_utils.c.gcov:382: warning: branch  2 taken 0%
hk_utils.c.gcov:484: warning: branch  0 taken 0% (fallthrough)
hk_utils.c.gcov:487: warning: Line not executed
hk_utils.c.gcov:487: warning: call    0 never executed
hk_utils.c.gcov:527: warning: branch  1 taken 0%
hk_utils.c.gcov:527: warning: branch  3 taken 0%
hk_utils.c.gcov:527: warning: branch  5 taken 0%
hk_utils.c.gcov:537: warning: branch  1 taken 0%
hk_utils.c.gcov:537: warning: branch  2 taken 0%
hk_utils.c.gcov:567: warning: branch  3 taken 0%
Processed 2 files
!!! 15 warnings found !!!

---------------------------------------------------------------------------------------


Comments on Functions with less than 100% Coverage
-----------------------------------------------

Function  HK_CheckStatusOfTables from hk_utils.c


        -:  481:    /* Determine if the runtime table has a dump pending */   
        9:  482:    Status = CFE_TBL_GetStatus(HK_AppData.RuntimeTableHandle);
        -:  483:    
        9:  484:    if (Status == CFE_TBL_INFO_DUMP_PENDING)
        -:  485:    {
        -:  486:        /* Dump the specified Table, cfe tbl manager makes copy */
    #####:  487:        CFE_TBL_DumpToBuffer(HK_AppData.RuntimeTableHandle);       
        -:  488:
        -:  489:    }
        9:  490:    else if(Status != CFE_SUCCESS)
        -:  491:    {
        -:  492:        
        1:  493:        CFE_EVS_SendEvent (HK_UNEXPECTED_GETSTAT2_RET_EID, CFE_EVS_ERROR,
        -:  494:               "Unexpected CFE_TBL_GetStatus return (0x%08X) for Runtime Table", 
        -:  495:               Status);
        -:  496:    }
        -:  497:    
        -:  498:    return;


This line is executed when the call to CFE_TBL_GetStatus() returns a value of CFE_TBL_INFO_DUMP_PENDING. The
unit test framework allows us to set the return value to get to this block of code. However, the UTF
version of CFE_TBL_DumpToBuffer() causes a Segmentation Fault when it executes, so the unit test cannot 
execute this line of code.



BRANCH COVERAGE
---------------

Note that the CFS Development Standards Document (582-2007-043) does not explicitly require 100% branch 
coverage, only 100% code coverage.  However, full branch coverage is a goal for future HK unit tests. 

Comments on Functions with branches that never executed
-------------------------------------------------------

There are no functions with branches that never executed


Comments on Functions with branches taken 0%
--------------------------------------------

All branches that were taken 0% were part of decision conditional statements that executed other paths that led
to the same code (giving us 100% code coverage).  Future HK unit tests will attempt to provide full branch
coverage.
