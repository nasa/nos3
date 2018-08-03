PROC $sc_$cpu_es_b4p0_integration
;*******************************************************************************
;  Test Name:  $sc_$cpu_es_b4p0_integration
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
;	11/16/06	W. Moleski	Initial development.
;       11/17/06        M. Bartholomew  Added command verificaion
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

;******* LOCAL Defines
#define CFE_EVS_DEBUG_MSG_ENABLE_DBG_EID        20

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL cmdcnt, errcnt, found_app, stream

write ";*********************************************************************"
write "; Step 1: Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_EVS", CFE_EVS_DEBUG_MSG_ENABLE_DBG_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif
wait 5

write ";*********************************************************************"
write "; Step 2: Send the NOOP command. (Command Code 0)"
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_ES",  CFE_ES_NOOP_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_NOOP"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - ES NOOP command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - ES NOOP Event Message not rcv'd."
  endif
else
  write "<!> Failed - ES NOOP command failed."
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
  /raw 1806c0000001aa
elseif ("$CPU" = "CPU2") then
  /raw 1826c0000001aa
elseif ("$CPU" = "CPU3") then
  /raw 1846c0000001aa
endif

ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Error count incremented as expected."
else
  write "<!> Failed - Error count did not change."
endif

ut_setupevt "$SC", "$CPU", "CFE_ES",  CFE_ES_RESET_INF_EID, "INFO"

/$SC_$CPU_ES_RESETCTRS
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Reset Counters Event Message not rcv'd."
endif

;;; Check that the CMDPC and CMDEC are 0
ut_tlmwait $SC_$CPU_ES_CMDPC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - PC count reset as expected."
else
  write "<!> Failed - PC count did not reset to 0."
endif

ut_tlmwait $SC_$CPU_ES_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Error count reset as expected."
else
  write "<!> Failed - Error count did not reset to 0."
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

  /$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
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
  write "<*> Passed - Modify CDS Variables command sent successfully."
else
  write "<!> Failed - Modify CDS Variables command."
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
/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="/ram/b4ResetSys.log"
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
metSecs = $SC_$CPU_TIME_MET_Secs
metSubSecs = $SC_$CPU_TIME_MET_SubSecs
stcfSecs = $SC_$CPU_TIME_STCF_Secs
stcfSubSecs = $SC_$CPU_TIME_STCF_SubSecs
;;;delaySecs = $SC_$CPU_TIME_????
;;;delaySubSecs = $SC_$CPU_TIME_????

write ";*********************************************************************"
write "; Step 8.0: Perform a Processor reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_cmd
close_data_center
wait 50

write ";*********************************************************************"
write "; Step 8.1: Starting things up again. Sending the command to open up  "
write ";           the data center. "
write ";*********************************************************************"
open_data_center
wait 30

write ";*********************************************************************"
write "; Step 8.2: Sending command to open Telemetry. "
write ";*********************************************************************"
open_tlm
wait 10

write ";*********************************************************************"
write "; Step 8.3: Sending command to open Commanding. "
write ";*********************************************************************"
open_cmd $cpu
wait 10

write ";*********************************************************************"
write "; Step 8.4: Enable DEBUG Event Messages"
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_EVS", CFE_EVS_DEBUG_MSG_ENABLE_DBG_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif
wait 5

write ";*********************************************************************"
write "; Step 9: Dump the System and ER Logs after Processor Reset"
write ";*********************************************************************"
; Send the write sys log and ER log to file commands
; Verify that the system log indicates a "processor" reset
/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="/ram/afterResetSys.log"
wait 5

;;;; Need to use ftp_file for System Log
s ftp_file ("RAM:0","b4ResetSys.log","$sc_$cpu_b4ResetSys.log","$CPU","G")
wait 5

;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt ("RAM:0", "afterResetER.log", "$sc_$cpu_afterResetER.log", "$CPU","ES_ERLOG")
wait 5

write ";*********************************************************************"
write "; Step 10: Restart the TST_ES application"
write ";*********************************************************************"
; Set the HK CDS variables to 0 to verify they change when TST_ES starts
$SC_$CPU_TST_ES_CDSVAR1 = 0
$SC_$CPU_TST_ES_CDSVAR2 = 0
$SC_$CPU_TST_ES_CDSVAR3 = 0
$SC_$CPU_TST_ES_CDSVAR4 = 0
$SC_$CPU_TST_ES_CDSVAR5 = 0

; Restart the test app
cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_STARTAPP APPLICATION="TST_ES" APP_ENTRY_PT="TST_ES_TaskMain" APP_FILE_NAME="/ram/tst_es.o" STACKSIZE=x'1000' PRIORITY=x'c8' STARTSLOT=1 SLOTCOUNT=1

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Restart App command sent successfully."
else
  write "<!> Failed - Restart App command."
endif

wait 10

; Subscribe to the HK packet
/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

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
  write "<*> Passed - CDS Variables were saved on Processor Reset"
else
  write "<!> Failed - CDS Variables were lost on Processor Reset"
endif
wait 5

write ";*********************************************************************"
write "; Step 11: Verify the ES & TIME Variables saved on a reset"
write ";*********************************************************************"
;; Check the ES variables saved across a Processor Reset

;; Check that the Reset Type indicates a Processor Reset
if ($SC_$CPU_ES_ResetType = CFE_ES_PROCESSOR_RESET) then
  write "<*> Passed - Reset Type indicates a Processor Reset."
else
  write "<!> Failed - Reset type does not indicate a Processor Reset"
endif
;; Check that the Reset SubType indicates that the Reset was commanded
if ($SC_$CPU_ES_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed - Reset Subtype indicates a commanded Processor Reset."
else
  write "<!> Failed - Reset Subtype does not indicate a commanded Processor Reset"
endif
;; Check that the Boot Source is correct
if ($SC_$CPU_ES_BootSource = esBootSrc) then
  write "<*> Passed - Boot Source remained the same."
else
  write "<!> Failed - Boot Source changed from ",esBootSrc, " to ", $SC_$CPU_ES_BootSource
endif
;; Check that the Processor Reset Counter incremented
if ($SC_$CPU_ES_ProcResetCnt = esprocResetCnt + 1) then
  write "<*> Passed - Processor Reset count preserved and incremented."
else
  write "<!> Failed - Expected a Processor Reset count of ", esprocResetCnt + 1, " rcv'd ", $SC_$CPU_ES_ProcResetCnt
endif
;; Check that the Max Processor Reset Counter remained the same
if ($SC_$CPU_ES_MaxProcResets = esMaxCnt) then
  write "<*> Passed - Max Processor Reset count preserved."
else
  write "<!> Failed - Expected a Max Processor Reset count of ", esMaxCnt, " rcv'd ", $SC_$CPU_ES_MaxProcResets
endif
wait 5

;;; Need to add code to check the TIME CDS variables
leapSecs = $SC_$CPU_TIME_LeapSecs
clkSignal = $SC_$CPU_TIME_FlagPRI
metSecs = $SC_$CPU_TIME_MET_Secs
metSubSecs = $SC_$CPU_TIME_MET_SubSecs
stcfSecs = $SC_$CPU_TIME_STCF_Secs
stcfSubSecs = $SC_$CPU_TIME_STCF_SubSecs
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

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_XLOG1_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_ES_CLEARSYSLOG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Clear System Log command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Clear System Log Event Message not rcv'd."
  endif
else
  write "<!> Failed - Clear System Log command failed."
endif
wait 5

ut_tlmwait $SC_$CPU_ES_SYSLOGBYTEUSED, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - System Log Bytes used set to 0 as expected."
else
  write "<!> Failed - Clear System Log did not reset Bytes Used which = ", $SC_$CPU_ES_SYSLOGBYTEUSED
endif

ut_tlmwait $SC_$CPU_ES_SYSLOGENTRIES, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - System Log Entries set to 0 as expected."
else
  write "<!> Failed - Clear System Log did not reset Entries which = ", $SC_$CPU_ES_SYSLOGENTRIES
endif

; Send the write sys log command
/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="/ram/emptySys.log"
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
  write "<*> Passed - Clear ER Log command sent successfully."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Clear ER Log Event Message not rcv'd."
  endif
else
  write "<!> Failed - Clear ER Log command failed."
endif
wait 5 

; Send the write ER log command
;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt ("RAM:0", "emptyER.log", "$sc_$cpu_emptyER.log", "$CPU","ES_ERLOG")
wait 5 

write ";*********************************************************************"
write "; Step 13: Send the Stop App command. (command code 5)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

;; Expect an error since this command is not implemented
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_ERR_EID, "ERROR"

;; Attempt to stop the TST_ES application
/$SC_$CPU_ES_STOPAPP APPLICATION="TST_ES"
wait 5

;; Look for the ERROR event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Stop App Error Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 14: Send the Reset App command. (command code 6)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

;; Expect an error since this command is not implemented
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RESET_APP_ERR_EID, "ERROR"

;; Attempt to reset the TST_ES application
/$SC_$CPU_ES_RESETAPP APPLICATION="TST_ES"
wait 5

;; Look for the ERROR event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Reset App Error Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 15: Send the Reload App command. (command code 7)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

;; Expect an error since this command is not implemented
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RELOAD_APP_ERR_EID, "ERROR"

;; Attempt to reload the TST_ES application
/$SC_$CPU_ES_RELOADAPP APPLICATION="TST_ES"
wait 5

;; Look for the ERROR event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Reload App Error Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 16: Send the Query App command. (command code 8)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

;; Expect an error since this command is not implemented
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_ONE_ERR_EID, "ERROR"

;; Attempt to query the TST_ES application
/$SC_$CPU_ES_QUERYAPP APPLICATION="TST_ES"
wait 5

;; Look for the ERROR event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Query App Error Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 17: Send the Start Logic Analyzer Data command. (14)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_STARTLADATA_INF_EID, "INFO"

/$SC_$CPU_ES_STARTLADATA
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Start Logic Analyzer Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 18: Send the Stop Logic Analyzer Data command. (15)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_STOPLADATA_INF_EID, "INFO"

;; Use the default file name
/$SC_$CPU_ES_STOPLADATA DATAFILENAME=""
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Stop Logic Analyzer Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 19: Send the Logic Analyzer Filter Mask command. (16)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_SETALFILTERMASK_INF_EID, "INFO"

;; Use the default file name
/$SC_$CPU_ES_LAFILTERMASK BANK1 FILTER_MASK=x'ffff'
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Logic Analyzer Filter Mask Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 20: Send the Start Logic Analyzer Trigger Mask command. (17)"
write ";*********************************************************************"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_SETALTRIGGERMASK_INF_EID, "INFO"

;; Use the default file name
/$SC_$CPU_ES_LATRIGGERMASK BANK0 TRIGGER_MASK=x'5555'
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Logic Analyzer Trigger Mask Event Message not rcv'd."
endif
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 21: Send the Overwrite System Log command. (18)"
write ";*********************************************************************"
;;ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RESET_PR_COUNT_EID, "INFO"
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

ut_sendcmd "$SC_$CPU_ES_OVERWRITESYSLOGMODE OVERWRITE"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Overwrite System Log Mode command sent successfully."
;;  if ($SC_$CPU_num_found_messages = 1) then
;;    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
;;  else
;;    write "<!> Failed - Overwrite System Log Mode Event Message not rcv'd."
;;  endif
else
  write "<!> Failed - Overwrite System Log Mode command failed."
endif
wait 5 
write "CMDPC = ", $SC_$CPU_ES_CMDPC, "; CMDEC = ", $SC_$CPU_ES_CMDEC

write ";*********************************************************************"
write "; Step 22: Send the Reset Processor Reset Counter command. (19)"
write ";*********************************************************************"
local resetCnt = $SC_$CPU_ES_PROCRESETCNT

ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_RESET_PR_COUNT_EID, "INFO"

/$SC_$CPU_ES_RESETPRCNT
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Reset Processor Reset Count Event Message not rcv'd."
endif
wait 5 

ut_tlmwait $SC_$CPU_ES_PROCRESETCNT, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Processor reset count set to 0 as expected."
else
  write "<!> Failed - Processor Reset count = ", $SC_$CPU_ES_PROCRESETCNT
endif
wait 5

write ";*********************************************************************"
write "; Step 23: Send the Set Max Processor Resets command. (20)"
write ";   This is the count of Processor Resets before an automatic Power-On"
write ";   Reset is performed."
write ";*********************************************************************"
local maxCnt = $SC_$CPU_ES_MAXPROCRESETS

ut_setupevt "$SC", "$CPU", "CFE_ES",  CFE_ES_SET_MAX_PR_COUNT_EID, "INFO"
/$SC_$CPU_ES_SETMAXPRCNT MAX_COUNT=10
wait 5

;; Look for the expected event
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Set Max Processor Reset Count Event Message not rcv'd."
endif
wait 5 

ut_tlmwait $SC_$CPU_ES_MAXPROCRESETS, 10
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Max Processor reset count set to 10 as expected."
else
  write "<!> Failed - Max Processor Reset count = ", $SC_$CPU_ES_MAXPROCRESETS
endif
wait 5

write ";*********************************************************************"
write "; Step 24.0: Perform a Power-On reset"
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_cmd
close_data_center
wait 50

write ";*********************************************************************"
write "; Step 24.1: Starting things up again. Sending the command to open up  "
write ";           the data center. "
write ";*********************************************************************"
open_data_center
wait 30

write ";*********************************************************************"
write "; Step 24.2: Sending command to open Telemetry. "
write ";*********************************************************************"
open_tlm
wait 10

write ";*********************************************************************"
write "; Step 24.3: Sending command to open Commanding. "
write ";*********************************************************************"
open_cmd $cpu
wait 10

write ";*********************************************************************"
write "; Step 25: Verify the System after a Power-On Reset. "
write ";*********************************************************************"

write "; ES Integration Test completed!!!"

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
