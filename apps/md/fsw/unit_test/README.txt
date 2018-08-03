Untested Paths in CFS Memory Dwell Unit Tests
Dec. 11, 2008

Note that UTF code for OS_MemValidateRange had to be modified in order
to generate these results.  As currently implemented, ranges of 1 
result in an invalid range error.
Walt will make this change to UTF when the corresponding change is made to
OSAL.  This is documented in DCR #4328.

* * * * *
md_app.c
* * * * *

Lines 970-973
This code handles the case that the extracted command code isn't
addressed in the command loop.  The command loop is only entered
_after_ a command code is found in the MD_CmdHandlerTbl structure.
So this code should only execute if a new command is added to the 
MD_CmdHandlerTbl and then is not addressed in the command handling
loop.

Line 1099
This line executes only when a match is made for a message id
that does not have a corresponding command code.  The routine
MD_SearchCmdHndlrTbl is not used for any such messages in this
application.

Line 1118
This message is for when no matching msg id is found in 
MD_CmdHandlerTbl.  This will never be reached because
only messages with valid message ids are passed to
MD_ExecRequest which calls this routine (MD_SearchCmdHndlrTbl.)


* * * * *
md_cmds.c
* * * * *

All lines tested.

* * * * *
md_dwell_pkt.c
* * * * *

Line 191
This line executes at read time if an invalid number of bytes
is specified.  This line should only be reached if dwell length
value becomes corrupted.


* * * * *
md_dwell_tbl.c
* * * * *
All lines tested.


* * * * *
md_utils.c
* * * * *
All lines tested.


