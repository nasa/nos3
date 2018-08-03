
README for CFS Limit Checker (LCX) unit tests run on 10/01/12

CDS Configuration Setting
-------------------------
#define LC_SAVE_TO_CDS commented out in lc_platform_cfg.h
This readme is for unit test output WITHOUT the CDS used during
application initialization.

Platform
--------
MAC OS 10 using VMware installed Red Hat Linux 5

Supporting Software Used:
-------------------------
cFE v5.2/OSAL 2.12 with bundled UTF

Coverage Summary
----------------
$ gcov lc_action.c
File `../src/lc_action.c'
Lines executed:100.00% of 170
../src/lc_action.c:creating `lc_action.c.gcov'

$ gcov lc_app.c
File `../src/lc_app.c'
Lines executed:67.92% of 240
../src/lc_app.c:creating `lc_app.c.gcov'

$ gcov lc_cmds.c
File `../src/lc_cmds.c'
Lines executed:94.82% of 328
../src/lc_cmds.c:creating `lc_cmds.c.gcov'

$ gcov lc_custom.c
File `../src/lc_custom.c'
Lines executed:100.00% of 10
../src/lc_custom.c:creating `lc_custom.c.gcov'

$ gcov lc_watch.c
File `../src/lc_watch.c'
Lines executed:95.36% of 345
../src/lc_watch.c:creating `lc_watch.c.gcov'

Comments On Code Not Covered
----------------------------
In the function LC_AppMain in file lc_app.c.gcov

     Lines: 305, 310, 315, 320, 322, 330, 335
     The main run loop is common code used by most applications.

     Line: 368
     This line requires that Critical Data Store (CDS) is enabled.

In the function LC_TableInit in file lc_app.c.gcov

     Lines: 595, 612, 612, 631, 644, 649
     Lines: 651, 653, 655, 661, 663, 665
     Lines: 667, 674, 688, 690, 693, 695, 701
     These lines require that Critical Data Store (CDS) is enabled.

In the function LC_CreateDefinitionTables in file lc_app.c.gcov

     Lines: 837-38, 871-72, 877, 883-84, 934, 936
     These lines require that Critical Data Store (CDS) is enabled.

In the function LC_CreateTaskCDS in file lc_app.c.gcov

     Lines: all lines (38) in this function
     These lines require that Critical Data Store (CDS) is enabled.

In the function LC_LoadDefaultTables in file lc_app.c.gcov

     Lines: 1174, 1176, 1180
     These lines require that Critical Data Store (CDS) is enabled.

In the function LC_HousekeepingCmd in file lc_cmds.c.gcov

     Lines: 737, 739
     These lines require that Critical Data Store (CDS) is enabled.

In the function LC_UpdateTaskCDS in file lc_cmds.c.gcov

     Lines: all lines (15) in this function
     These lines require that Critical Data Store (CDS) is enabled.

In the function LC_GetSizedWPData in file lc_watch.c.gcov

     Lines: 1277-78, 1298-99, 1309-10
     Lines: 1329-30, 1341-44, 1368-71
     These tests were run on an x86 machine which is Little Endian. To
     execute these lines would require execution on a big endian machine
     (such as a PowerPC) that would have resulted in an alternate
     set of unexecuted lines. Since we already have two sets of unit 
     test output based upon CDS use, it didn't seem worthwhile to add
     additional ones just for these cases. 
     
