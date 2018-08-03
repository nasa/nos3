
README for CFS Limit Checker (LC) unit tests run on 1/15/09

CDS Configuration Setting
-------------------------
#define LC_SAVE_TO_CDS not commented out in lc_platform_cfg.h
This readme is for unit test output WITH the CDS used during
application initialization.

Platform
--------
Cygwin on Windows XP 

Supporting Software Used:
-------------------------
cFE v5.2/OSAL 2.12 with bundled UTF

Coverage Summary
----------------
$ gcov lc_action.c
File `../src/lc_action.c'
Lines executed:100.00% of 183
../src/lc_action.c:creating `lc_action.c.gcov'

$ gcov lc_app.c
File `../src/lc_app.c'
Lines executed:94.06% of 202
../src/lc_app.c:creating `lc_app.c.gcov'

$ gcov lc_cmds.c
File `../src/lc_cmds.c'
Lines executed:97.87% of 422
../src/lc_cmds.c:creating `lc_cmds.c.gcov'

$ gcov lc_custom.c
File `../src/lc_custom.c'
Lines executed:100.00% of 12
../src/lc_custom.c:creating `lc_custom.c.gcov'

$ gcov lc_watch.c
File `../src/lc_watch.c'
Lines executed:94.65% of 318
../src/lc_watch.c:creating `lc_watch.c.gcov'

Comments On Code Not Covered
----------------------------
In the function LC_InitFromCDS in file lc_app.c.gcov

     Lines: 659, 661, 675, 678
     In the current version of the UTF, the function CFE_ES_RegisterCDS
     does not support function hooks. This is the only way that these
     two error events could be forced since we needed error returns on
     only the second or third calls to these functions, but success
     on others.

     Lines: 753, 759, 795, 801
     In the current version of the UTF, the function CFE_ES_RestoreFromCDS
     does not support function hooks. This is the only way that these
     two error events could be forced since we needed error returns on
     only the second or third calls to these functions, but success
     on others.

In the function LC_ExitApp in file lc_app.c.gcov

     Lines: 1107, 1113, 1143, 1149
     In the current version of the UTF, the function CFE_ES_CopyToCDS
     does not support function hooks. This is the only way that these
     two error events could be forced since we needed error returns on
     only the second or third calls to these functions, but success
     on others.

In the function LC_AcquirePointers in file lc_cmds.c.gcov

     Lines: 1302, 1307, 1309, 1313
     Lines: 1342, 1347, 1349, 1353
     In the current version of the UTF, the function CFE_TBL_DumpToBuffer
     does not support function hooks or preset return codes and allowing this
     function to be called during unit testing resulted in a core dump.
     This was most likely because the unit tests do not actually use UTF
     table services but work with local data structures to simulate table 
     data.

     Line: 1366
     In the current version of the UTF, the function CFE_TBL_GetStatus
     does not support function hooks so there was no way to force an error
     return on only the second call (and not the first) to this function.
     
In the function LC_GetSizedWPData in file lc_watch.c.gcov

     Lines: 941, 942, 962, 963, 973, 974, 993, 994
     Lines: 1005 to 1008, 1032 to 1037
     These tests were run on an x86 machine which is Little Endian. To
     execute these lines would require execution on a big endian machine
     (such as a PowerPC) that would have resulted in an alternate
     set of unexecuted lines. Since we already have two sets of unit 
     test output based upon CDS use, it didn't seem worthwhile to add
     additional ones just for these cases. 
