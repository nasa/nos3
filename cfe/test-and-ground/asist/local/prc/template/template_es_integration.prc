PROC $sc_$cpu_es_integration
;*******************************************************************************
;  Test Name:  $sc_$cpu_es_integration
;  Test Level: Build Integration
; 
;  Test Description
;	The purpose of this test is to perform Build Integration Testing of the
;	cFE Executive Services (ES).
;
;  Prerequisite Conditions
;       None 
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	11/16/06	W. Moleski	    Initial development.
;   11/17/06    M. Bartholomew  Added command verificaion
;   06/04/07    M. Bartholomew  Added commands for build 4.1
;   01/30/08    S. Jonke        Updated for build 5.0
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;				processed and command error counters.
;
;  Expected Test Results and Analysis
;**********************************************************************

#include "cfe_platform_cfg.h"
#include "ut_statusdefs.h"
#include "cfe_es_events.h"
#include "cfe_es_resetTypes.h"


;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL cmdcnt, errcnt, found_app, stream

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Enable Debug Events
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Write "*********  Enabling Debug Event Messages *********"

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
   write "<*> Success: Debug events have been enabled."
else
   write "<!> Error: Could not enable Debug events."
endif

wait 5

write ";*********************************************************************"
write "; Step 2: Send the NOOP command. (Command Code 0)"
write ";*********************************************************************"


ut_setupevt "$SC", "$CPU", "CFE_ES",  CFE_ES_NOOP_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_NOOP"
if (UT_SC_Status = UT_SC_Success) then
   write "<*> Success: ES NOOP command sent successfully."
   if ($SC_$CPU_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      Write "<!> Error: ES NOOP Event Message not rcv'd."
   endif
else
   write "<!> Error: ES NOOP command failed."
endif

wait 5

write ";*********************************************************************"
write "; Step 3: Send the Reset Counters command. (Command Code 1)"
write ";*********************************************************************"


write "Sending a command with an error so that the error counter increments"
;;;; Note that this is an ES command with an invalid function code = hex 'aa' (42 decimal)
cmdcnt = $SC_$CPU_ES_CMDPC
errcnt = $SC_$CPU_ES_CMDEC + 1

; Send commands to subscribe to the TST_ES HK packet
if ("$CPU" = "CPU1" OR "$CPU" = "") then
  ut_sendrawcmd "$SC_$CPU_ES", "1806c0000001aa"
elseif ("$CPU" = "CPU2") then
  ut_sendrawcmd "$SC_$CPU_ES", "1826c0000001aa"
elseif ("$CPU" = "CPU3") then
  ut_sendrawcmd "$SC_$CPU_ES", "1846c0000001aa"
endif

ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Error count incremented as expected."
else
  write "<!> Error: Error count did not change."
endif

ut_setupevt "$SC", "$CPU", "CFE_ES",  CFE_ES_RESET_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_RESETCTRS"

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Error: Reset Counters Event Message not rcv'd."
endif

;;; Check that the CMDPC and CMDEC are 0
ut_tlmwait $SC_$CPU_ES_CMDPC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: PC count reset as expected."
else
  write "<!> Error: PC count did not reset to 0."
endif

ut_tlmwait $SC_$CPU_ES_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Error count reset as expected."
else
  write "<!> Error: Error count did not reset to 0."
endif
wait 5

write ";*********************************************************************"
write "; Step 4: Determine if the TST_ES Test application is running."
write ";*********************************************************************"


start get_file_to_cvt ("RAM:0", "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")
                                                                                
;Loop thru the table looking for the TST_ES app
for app_info_file_index = 1 to 32 do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_ES") then
    found_app = TRUE
  endif
enddo
                                                                                
wait 10

write ";*********************************************************************"
write "; Step 5: Start the TST_ES Application. "
write ";*********************************************************************"


if (found_app <> TRUE) then
  write "The TST_ES application is not running. It will be started."
  start load_start_app ("TST_ES", "$CPU")
  wait 10

  ; Send commands to subscribe to the TST_ES HK packet
  if ("$CPU" = "CPU1" OR "$CPU" = "") then
     stream = x'717'
  elseif ("$CPU" = "CPU2") then
     stream = x'737'
  elseif ("$CPU" = "CPU3") then
     stream = x'757'
  endif

  ut_sendcmd "$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'"
else
  write "The TST_ES Application is already running. Skipping startup!!!"
endif

wait 5

write ";*********************************************************************"
write "; Step 6: Send the CDS Modification command."
write ";*********************************************************************"


;;;; Save the current values of the CDS Variables
local cdsVar1, cdsVar2, cdsVar3, cdsVar4, cdsVar5
local newCDSVar1, newCDSVar2, newCDSVar3, newCDSVar4, newCDSVar5

cdsVar1 = $SC_$CPU_TST_ES_CDSVAR1
cdsVar2 = $SC_$CPU_TST_ES_CDSVAR2
cdsVar3 = $SC_$CPU_TST_ES_CDSVAR3
cdsVar4 = $SC_$CPU_TST_ES_CDSVAR4
cdsVar5 = $SC_$CPU_TST_ES_CDSVAR5

write "CDS Variable #1 = ", cdsVar1
write "CDS Variable #2 = ", cdsVar2
write "CDS Variable #3 = ", cdsVar3
write "CDS Variable #4 = ", cdsVar4
write "CDS Variable #5 = ", cdsVar5

cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_MODIFYCDSVARS
ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Modify CDS Variables command sent successfully."
else
  write "<!> Error: Modify CDS Variables command."
endif

wait 10

newCDSVar1 = $SC_$CPU_TST_ES_CDSVAR1
newCDSVar2 = $SC_$CPU_TST_ES_CDSVAR2
newCDSVar3 = $SC_$CPU_TST_ES_CDSVAR3
newCDSVar4 = $SC_$CPU_TST_ES_CDSVAR4
newCDSVar5 = $SC_$CPU_TST_ES_CDSVAR5

write "updated CDS Variable #1 = ", newCDSVar1
write "updated CDS Variable #2 = ", newCDSVar2
write "updated CDS Variable #3 = ", newCDSVar3
write "updated CDS Variable #4 = ", newCDSVar4
write "updated CDS Variable #5 = ", newCDSVar5
wait 5

write ";*********************************************************************"
write "; Step 7.0: Save the state of things before the Processor reset"
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 7.1: Write the System and ER Logs"
write ";*********************************************************************"

; Send the write sys log and ER log to file commands
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=""/ram/b4ResetSys.log"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Write Sys Log command processed."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Write Sys Log command."
  endif
else
  write "<!> Error: Did not process Write Sys Log command."
endif

wait 5

;;;; Need to use ftp_file for System Log
s ftp_file ("RAM:0","b4ResetSys.log","$sc_$cpu_b4ResetSys.log","$CPU","G")
wait 5

;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt ("RAM:0", "b4ResetER.log", "$sc_$cpu_b4ResetER.log", "$CPU","ES_ERLOG")
;;;/$SC_$CPU_ES_WRITEERLOG2FILE ERLOGFILENAME="/ram/b4ResetER.log"
wait 5

write ";*********************************************************************"
write "; Step 7.2: Save the ES and TIME CDS variable values"
write ";*********************************************************************"
;;; Save the ES CDS Values
local esResetType, esResetSubtype, esBootSrc, esprocResetCnt, esMaxCnt 
esResetType = $SC_$CPU_ES_ResetType
esResetSubtype = $SC_$CPU_ES_ResetSubtype
esBootSrc = $SC_$CPU_ES_BootSource
esprocResetCnt = $SC_$CPU_ES_ProcResetCnt
esMaxCnt = $SC_$CPU_ES_MaxProcResets

;; Need to add code to save the TIME CDS variables 
local leapSecs, clkSignal, metSecs, metSubSecs, stcfSecs, stcfSubSecs
;;;local delaySecs, delaySubSecs
leapSecs = $SC_$CPU_TIME_LeapSecs
clkSignal = $SC_$CPU_TIME_FlagPRI
metSecs = $SC_$CPU_TIME_METSecs
metSubSecs = $SC_$CPU_TIME_METSubsecs
stcfSecs = $SC_$CPU_TIME_STCFSecs
stcfSubSecs = $SC_$CPU_TIME_STCFSubsecs
;;;delaySecs = $SC_$CPU_TIME_????
;;;delaySubSecs = $SC_$CPU_TIME_????

write ";*********************************************************************"
write "; Step 8.0: Perform a Processor reset (command code 2)"
write ";*********************************************************************"
ut_sendcmd "$SC_$CPU_ES_PROCESSORRESET"
wait 10

close_data_center
wait 120


write ";*********************************************************************"
write "; Step 8.1: Starting things up again. Execute the proc to startup "
write ";           the cFE and ASIST"
write ";*********************************************************************"
cfe_startup $CPU
wait 30

write ";*******************************************************************"
write "; Step 8.4: Enable DEBUG Event Messages"
write ";*********************************************************************"

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
   write "<*> Success: Debug events have been enabled."
else
   write "<!> Error: Could not enable Debug events."
endif

wait 5

write ";*********************************************************************"
write "; Step 9: Dump the System and ER Logs after Processor Reset"
write ";*********************************************************************"
; Send the write sys log and ER log to file commands
; Verify that the system log indicates a "processor" reset
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=""/ram/afterResetSys.log"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Write Sys Log command processed."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Write Sys Log command."
  endif
else
  write "<!> Error: Did not process Write Sys Log command."
endif

wait 5

;;;; Need to use ftp_file for System Log
s ftp_file ("RAM:0","afterResetSys.log","$sc_$cpu_afterResetSys.log","$CPU","G")
;;s ftp_file ("RAM:0","b4ResetSys.log","$sc_$cpu_b4ResetSys.log","$CPU","G")
wait 5

;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt ("RAM:0", "afterResetER.log", "$sc_$cpu_afterResetER.log", "$CPU","ES_ERLOG")
wait 5

write ";*********************************************************************"
write "; Step 10.0: Determine if the TST_ES Test application is running."
write ";*********************************************************************"
start get_file_to_cvt ("RAM:0", "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")
                                                                                
;Loop thru the table looking for the TST_ES app
found_app = FALSE
for app_info_file_index = 1 to 32 do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_ES") then
    found_app = TRUE
  endif
enddo
                                                                                
wait 10

write ";*********************************************************************"
write "; Step 10.1: Start the TST_ES Application. "
write ";*********************************************************************"
if (found_app <> TRUE) then
  write "The TST_ES application is not running. It will be started."
  start load_start_app ("TST_ES", "$CPU")
  wait 10

  ; Send commands to subscribe to the TST_ES HK packet
  if ("$CPU" = "CPU1" OR "$CPU" = "") then
     stream = x'717'
  elseif ("$CPU" = "CPU2") then
     stream = x'737'
  elseif ("$CPU" = "CPU3") then
     stream = x'757'
  endif

  ut_sendcmd "$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'"
else
  write "The TST_ES Application is already running. Skipping startup!!!"
endif

wait 5

write ";*********************************************************************"
write "; Step 10.2: Restart the TST_ES application (Command Code 6)"
write ";*********************************************************************"
; Set the HK CDS variables to 0 to verify they change when TST_ES starts
$SC_$CPU_TST_ES_CDSVAR1 = 0
$SC_$CPU_TST_ES_CDSVAR2 = 0
$SC_$CPU_TST_ES_CDSVAR3 = 0
$SC_$CPU_TST_ES_CDSVAR4 = 0
$SC_$CPU_TST_ES_CDSVAR5 = 0

; Restart the test app
cmdcnt = $SC_$CPU_ES_CMDPC + 1

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RESTART_APP_DBG_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_ES_RESTARTAPP APPLICATION=""TST_ES"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Restart App command processed."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Restart App command."
  endif
else
  write "<!> Error: Did not process Restart App command."
endif

wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Restart App command sent successfully."
else
  write "<!> Error: Restart App command."
endif

wait 10

; Subscribe to the HK packet
ut_sendcmd "$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'"

wait 5

; Verify that the HK indicates the expected values for the CDS variables
cdsVar1 = $SC_$CPU_TST_ES_CDSVAR1
cdsVar2 = $SC_$CPU_TST_ES_CDSVAR2
cdsVar3 = $SC_$CPU_TST_ES_CDSVAR3
cdsVar4 = $SC_$CPU_TST_ES_CDSVAR4
cdsVar5 = $SC_$CPU_TST_ES_CDSVAR5

write "CDS Variable #1 = ", cdsVar1
write "CDS Variable #2 = ", cdsVar2
write "CDS Variable #3 = ", cdsVar3
write "CDS Variable #4 = ", cdsVar4
write "CDS Variable #5 = ", cdsVar5

; Compare the variables above with the ones saved before the reset
if (cdsVar1 = newCDSVar1 AND cdsVar2 = newCDSVar2 AND ;;
    cdsVar3 = newCDSVar3 AND cdsVar4 = newCDSVar4 AND ;;
    cdsVar5 = newCDSVar5) then
  write "<*> Success: CDS Variables were saved on Processor Reset"
else
  write "<!> Error: CDS Variables were lost on Processor Reset"
endif
wait 5

write ";*********************************************************************"
write "; Step 11: Verify the ES & TIME Variables saved on a reset"
write ";*********************************************************************"
;; Check the ES variables saved across a Processor Reset

;; Check that the Reset Type indicates a Processor Reset
if ($SC_$CPU_ES_ResetType = CFE_ES_PROCESSOR_RESET) then
  write "<*> Success: Reset Type indicates a Processor Reset."
else
  write "<!> Error: Reset type does not indicate a Processor Reset"
endif
;; Check that the Reset SubType indicates that the Reset was commanded
if ($SC_$CPU_ES_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Success: Reset Subtype indicates a commanded Processor Reset."
else
  write "<!> Error: Reset Subtype does not indicate a commanded Processor Reset"
endif
;; Check that the Boot Source is correct
if ($SC_$CPU_ES_BootSource = esBootSrc) then
  write "<*> Success: Boot Source remained the same."
else
  write "<!> Error: Boot Source changed from ",esBootSrc, " to ", $SC_$CPU_ES_BootSource
endif
;; Check that the Processor Reset Counter incremented
if ($SC_$CPU_ES_ProcResetCnt = esprocResetCnt + 1) then
  write "<*> Success: Processor Reset count preserved and incremented."
else
  write "<!> Error: Expected a Processor Reset count of ", esprocResetCnt + 1, " rcv'd ", $SC_$CPU_ES_ProcResetCnt
endif
;; Check that the Max Processor Reset Counter remained the same
if ($SC_$CPU_ES_MaxProcResets = esMaxCnt) then
  write "<*> Success: Max Processor Reset count preserved."
else
  write "<!> Error: Expected a Max Processor Reset count of ", esMaxCnt, " rcv'd ", $SC_$CPU_ES_MaxProcResets
endif
wait 5

;;; Need to add code to check the TIME CDS variables
leapSecs = $SC_$CPU_TIME_LeapSecs
clkSignal = $SC_$CPU_TIME_FlagPRI
metSecs = $SC_$CPU_TIME_METSecs
metSubSecs = $SC_$CPU_TIME_METSubsecs
stcfSecs = $SC_$CPU_TIME_STCFSecs
stcfSubSecs = $SC_$CPU_TIME_STCFSubsecs
;;;delaySecs = $SC_$CPU_TIME_????
;;;delaySubSecs = $SC_$CPU_TIME_????

write ";*********************************************************************"
write "; Step 12.0: Clear the System & ER Logs."
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 12.1: Clear the System Log. (command code 10) "
write ";*********************************************************************"
;; Save the current TLM points for the System Log
local sysLogBytesUsed, sysLogEntries
sysLogBytesUsed = $SC_$CPU_ES_SYSLOGBYTEUSED
sysLogEntries = $SC_$CPU_ES_SYSLOGENTRIES

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG1_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_CLEARSYSLOG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Clear System Log command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Clear System Log Event Message not rcv'd."
  endif
else
  write "<!> Error: Clear System Log command failed."
endif
wait 5

ut_tlmwait $SC_$CPU_ES_SYSLOGBYTEUSED, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: System Log Bytes used set to 0 as expected."
else
  write "<!> Error: Clear System Log did not reset Bytes Used which = ", $SC_$CPU_ES_SYSLOGBYTEUSED
endif

ut_tlmwait $SC_$CPU_ES_SYSLOGENTRIES, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: System Log Entries set to 0 as expected."
else
  write "<!> Error: Clear System Log did not reset Entries which = ", $SC_$CPU_ES_SYSLOGENTRIES
endif

; Send the write sys log command
ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=""/ram/emptySys.log"""
wait 5

;;;; Need to use ftp_file for System Log
s ftp_file ("RAM:0","emptySys.log","$sc_$cpu_emptySys.log","$CPU","G")
wait 5

write ";*********************************************************************"
write "; Step 12.2: Clear the Exception & Reset (ER) Log. (command code 12) "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG1_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_CLEARERLOG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Clear ER Log command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Clear ER Log Event Message not rcv'd."
  endif
else
  write "<!> Error: Clear ER Log command failed."
endif
wait 5 

; Send the write ER log command
;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt ("RAM:0", "emptyER.log", "$sc_$cpu_emptyER.log", "$CPU","ES_ERLOG")
wait 5 

write ";*********************************************************************"
write "; Step 13: Send the Delete App command. (command code 5)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_INF_EID, "INFO"

;; Attempt to Delete the TST_ES application
ut_sendcmd "$SC_$CPU_ES_DELETEAPP APPLICATION=""TST_ES"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Delete App command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Delete App Message not rcv'd."
  endif
else
  write "<!> Error: Delete App command failed."
endif

wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 14.0: Determine if the TST_ES Test application is running."
write ";*********************************************************************"
start get_file_to_cvt ("RAM:0", "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")
         
;Loop thru the table looking for the TST_ES app
found_app = FALSE
for app_info_file_index = 1 to 32 do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_ES") then
    found_app = TRUE
  endif
enddo

if (found_app <> TRUE) then
  write "<*> Success: TST_ES application was stopped."
else
  write "<!> Error: TST_ES application is still running!"
endif
                                                                                
wait 10

write ";*********************************************************************"
write "; Step 14.1: Start the TST_ES Application. "
write ";*********************************************************************"
if (found_app <> TRUE) then
  write "The TST_ES application is not running. It will be started."
  start load_start_app ("TST_ES", "$CPU")
  wait 10

  ; Send commands to subscribe to the TST_ES HK packet
  if ("$CPU" = "CPU1" OR "$CPU" = "") then
     stream = x'717'
  elseif ("$CPU" = "CPU2") then
     stream = x'737'
  elseif ("$CPU" = "CPU3") then
     stream = x'757'
  endif

  ut_sendcmd "$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'"
else
  write "The TST_ES Application is already running. Skipping startup!!!"
endif

wait 5

write ";*********************************************************************"
write "; Step 15: Send the Reload App command. (command code 7)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RELOAD_APP_INF_EID, "INFO"

;; Attempt to reload the TST_ES application
ut_sendcmd "$SC_$CPU_ES_RELOADAPP APPLICATION=""TST_ES"" APPFILENAME =""/ram/tst_es.o"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Reload App command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Reload App Event Message not rcv'd."
  endif
else
  write "<!> Error: Reload App command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 16: Send the Query App command. (command code 8)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_ONE_APP_EID, "DEBUG"

;; Attempt to query the TST_ES application
ut_sendcmd "$SC_$CPU_ES_QUERYAPP APPLICATION=""TST_ES"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Query App command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Query App Event Message not rcv'd."
  endif
else
  write "<!> Error: Query App command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 17: Send the Start Logic Analyzer Data command. (14)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_PERF_STARTCMD_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_ES_STARTPERF"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Start Logic Analyzer Data command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Start Logic Analyzer Data Event Message not rcv'd."
  endif
else
  write "<!> Error: Start Logic Analyzer Data command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 18: Send the Stop Logic Analyzer Data command. (15)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_PERF_STOPCMD_EID, "DEBUG"

;; Use the default file name
ut_sendcmd "$SC_$CPU_ES_STOPPERF DATAFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Stop Logic Analyzer Data command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Stop Logic Analyzer Data Event Message not rcv'd."
  endif
else
  write "<!> Error: Stop Logic Analyzer Data command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 19: Send the Logic Analyzer Filter Mask command. (16)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_PERF_FILTMSKCMD_EID, "DEBUG"

;; Use the default file name
ut_sendcmd "$SC_$CPU_ES_PERFFLTRMASK BANK1 FILTER_MASK=x'ffff'"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Logic Analyzer Filter Mask command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Logic Analyzer Filter Mask Event Message not rcv'd."
  endif
else
  write "<!> Error: Logic Analyzer Filter Mask command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 20: Send the Start Logic Analyzer Trigger Mask command. (17)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_PERF_TRIGMSKCMD_EID, "DEBUG"

;; Use the default file name
ut_sendcmd "$SC_$CPU_ES_PERFTRIGMASK BANK0 TRIGGER_MASK=x'5555'"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Start Logic Analyzer Trigger Mask command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Start Logic Analyzer Trigger Mask Event Message not rcv'd."
  endif
else
  write "<!> Error: Start Logic Analyzer Trigger Mask command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 21: Send the Overwrite System Log command. (18)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOGMODE_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_ES_OVERWRITESYSLOGMODE OVERWRITE"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Overwrite System Log Mode command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Overwrite System Log Mode Event Message not rcv'd."
  endif
else
  write "<!> Error: Overwrite System Log Mode command failed."
endif
wait 5

write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 22: Send the Reset Processor Reset Counter command. (19)"
write ";*********************************************************************"
local resetCnt = $SC_$CPU_ES_PROCRESETCNT

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RESET_PR_COUNT_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_RESETPRCNT"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Reset Processor Reset Counter command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Reset Processor Reset Counter Event Message not rcv'd."
  endif
else
  write "<!> Error: Reset Processor Reset Counter command failed."
endif
wait 5

ut_tlmwait $SC_$CPU_ES_PROCRESETCNT, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Processor reset count set to 0 as expected."
else
  write "<!> Error: Processor Reset count = ", $SC_$CPU_ES_PROCRESETCNT
endif
wait 5

write ";*********************************************************************"
write "; Step 23: Send the Set Max Processor Resets command. (20)"
write ";   This is the count of Processor Resets before an automatic Power-On"
write ";   Reset is performed."
write ";*********************************************************************"
local maxCnt = $SC_$CPU_ES_MAXPROCRESETS

ut_setupevt "$SC", "$CPU", "CFE_ES",  CFE_ES_SET_MAX_PR_COUNT_EID, "INFO"
ut_sendcmd "$SC_$CPU_ES_SETMAXPRCNT MAX_COUNT=10"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Set Max Processor Resets command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Set Max Processor Resets Event Message not rcv'd."
  endif
else
  write "<!> Error: Set Max Processor Resets command failed."
endif
wait 5

ut_tlmwait $SC_$CPU_ES_MAXPROCRESETS, 10
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Max Processor reset count set to 10 as expected."
else
  write "<!> Error: Max Processor Reset count = ", $SC_$CPU_ES_MAXPROCRESETS
endif
wait 5

write ";*********************************************************************"
write "; Step 24.0: Perform a Power-On reset"
write ";*********************************************************************"
ut_sendcmd "$SC_$CPU_ES_POWERONRESET"
wait 10

close_data_center
wait 50

write ";*********************************************************************"
write "; Step 24.1: Starting things up again. Sending the command to open up  "
write ";           the data center. "
write ";*********************************************************************"
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Step 25: Verify the System after a Power-On Reset. "
write ";*********************************************************************"

write "; ES Integration Test completed!!!"

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
