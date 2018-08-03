PROC $sc_$cpu_cfe_buildtest (logDir,startProc)
;**********************************************************************
;  Test Name:  cfe_buildtest
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure executes all the cFE Build Verification Tests in
;       sequence.
;
;  Requirements Tested
;	See the individual test procedures for this information
;
;  Prerequisite Conditions
;	All the procedures exist for the CPU that you are executing this
;	procedure on.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	09/06/07	Walt Moleski	Updated for cFE 3.3 regression testing
;       11/29/10        Walt Moleski    Added logDir argument and updated the
;                                       ut_runproc calls to remove the $CPU
;
;  Arguments
;
;	Name			Description
;       logDir                  The directory name where to put the log files
;       startProc               The label to jump to in order to skip some tests
;
;  Procedures Called
;
;	Name			Description
;       ut_runproc      This utility procedure runs the specified procedure and
;                       creates output logs in the $WORK/test_logs directory if
;                       the logDir arguement is not specified. Otherwise, the
;                       log files reside in $WORK/test_logs/logDir.
;
;  Expected Test Results and Analysis
;	See the individual test procedures for this information.
;
;**********************************************************************

local stepNum = 1

;;; Determine where to start
if (startProc <> "") then
  goto {startProc}
endif

write ";**********************************************************************"
write ";  Step 1: Run the SB Commands Error Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_sb_cmds_err",logDir)
else
  start ut_runproc("$sc_$cpu_sb_cmds_err")
endif
stepNum = stepNum + 1

sb_dispipes:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the SB Disable Pipe Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_sb_dispipes",logDir)
else
  start ut_runproc("$sc_$cpu_sb_dispipes")
endif
stepNum = stepNum + 1

sb_enapipes:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the SB Enable Pipe Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_sb_enapipes",logDir)
else
  start ut_runproc("$sc_$cpu_sb_enapipes")
endif
stepNum = stepNum + 1

sb_reset:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the SB Reset Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_sb_reset",logDir)
else
  start ut_runproc("$sc_$cpu_sb_reset")
endif
stepNum = stepNum + 1

tbl_cmding:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the Table Commanding Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_tbl_cmding",logDir)
else
  start ut_runproc("$sc_$cpu_tbl_cmding")
endif
stepNum = stepNum + 1

tbl_func:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the TBL Functionality Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_tbl_func",logDir)
else
  start ut_runproc("$sc_$cpu_tbl_func")
endif
stepNum = stepNum + 1

tbl_reset:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the TBL Reset Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_tbl_reset",logDir)
else
  start ut_runproc("$sc_$cpu_tbl_reset")
endif
stepNum = stepNum + 1

time_cmd:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the Time Command Server Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_time_command_server_tai",logDir)
else
  start ut_runproc("$sc_$cpu_time_command_server_tai")
endif
stepNum = stepNum + 1

time_reset:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the Time Reset Server Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_time_resets_server_tai",logDir)
else
  start ut_runproc("$sc_$cpu_time_resets_server_tai")
endif
stepNum = stepNum + 1

evs_cmds:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the EVS Commands Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_evs_cmds",logDir)
else
  start ut_runproc("$sc_$cpu_evs_cmds")
endif
stepNum = stepNum + 1

evs_evt_msg_gen:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the EVS Event Message Generation Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_evs_evt_msg_gen",logDir)
else
  start ut_runproc("$sc_$cpu_evs_evt_msg_gen")
endif
stepNum = stepNum + 1

evs_log:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the EVS Log Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_evs_log",logDir)
else
  start ut_runproc("$sc_$cpu_evs_log")
endif
stepNum = stepNum + 1

evs_reset:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the EVS Reset Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_evs_reset",logDir)
else
  start ut_runproc("$sc_$cpu_evs_reset")
endif
stepNum = stepNum + 1

evs_bin_fltr:
write ";**********************************************************************"
write ";  Step ",stepNum,": Run the EVS Binary Filter Test"
write ";**********************************************************************"
if (logDir <> "") then
  start ut_runproc("$sc_$cpu_evs_bin_fltr",logDir)
else
  start ut_runproc("$sc_$cpu_evs_bin_fltr")
endif

write ";**********************************************************************"
write ";  End procedure $SC_$CPU_cfe_buildTest."
write ";**********************************************************************"
ENDPROC
