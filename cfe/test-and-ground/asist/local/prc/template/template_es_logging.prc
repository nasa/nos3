PROC $sc_$cpu_es_logging
;*******************************************************************************
;  Test Name:  $sc_$cpu_es_logging
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify the cFE Executive Services (ES)
;	software meets the requirements defined in the SRS for the defined 
;	Executive Services logs (System, Exception and Reset, and Logic
;	Analyzer Capture).
;
;  Requirements Tested
;    cES1005	Upon receipt of a Command, the cFE shall create the Command
;		specified Application by defining the Application in the System
;		Resources Definition using information from the Command
;		specified file, and beginning execution of the Application.
;    cES1005.1	The Command shall include the following parameters: 
;			a) Application Name
;			b) Application Entry Point
;			c) Application File Name
;			d) Application Stack Size
;			e) Application Priority
;			f) Application Memory Start Page
;			g) Application Memory Page Count
;    cES1009	Upon receipt of a Command, the cFE shall perform a Power On
;		Reset of the Core Flight Executive.
;    cES1010	Upon receipt of a Command, the cFE shall perform a Processor
;		Reset of the Core Flight Executive.
;    cES1014	The cFE shall maintain an Executive Services System Log which
;		contains a series of ASCII text strings describing significant
;		events or errors.
;    cES1014.1	Each entry in the Executive Services System Log shall be time
;		tagged with the time that the event happened.
;    cES1014.2	The cFE shall calculate the percentage of the Executive Services;		System Log that is used.
;   cES1014.2.1	If the Executive Services System Log is full and the System Log
;		Mode is set to OVERWRITE then the cFE shall write all new
;		entries from the top of the log.
;   cES1014.2.2	If the Executive Services System Log is full and the System Log
;		Mode is set to DISCARD then the cFE shall discard all new
;		entries.
;    cES1015	Upon receipt of a Command, the cFE shall clear the Executive
;		Services System Log.
;    cES1016	Upon receipt of a Command, the cFE shall copy the information
;		contained in the Executive Services System Log into a Command
;		specified file.
;    cES1016.1	If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename.
;    cES1017	The cFE shall maintain an Executive Services Exception and Reset
;		Log which will log critical system data for exceptions and
;		resets including: 
;			a) A time stamp
;			b) Processor Context information
;			c) Critical system variables
;			d) ASCII string stating the reason for the reset
;    cES1018	Upon receipt of a command, the cFE shall clear the Executive
;		Services Exception and Reset Log.
;    cES1019	Upon receipt of a Command, the cFE shall copy the information
;		contained in the Executive Services Exception and Reset Log
;		Buffer into a Command specified file.
;    cES1019.1	If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename.
;    cES1021	The cFE shall maintain an Executive Services Logic Analyzer
;		Capture Log for capturing application specified timestamps and
;		events for off-line perfromance analysis.
;    cES1022	Upon receipt of a Request, the cFE shall record the specified
;		Logic Analyzer Capture Tag in the Logic Analyzer Capture Log.
;    cES1022.1	The cFE shall store a timestamp along with the specified Logic
;		Analyzer Capture Tag.
;    cES1022.2	If the Logic Analyzer Capture Log is full, then the cFE shall
;		write all new entries from the top of the log.
;    cES1023	Upon receipt of a Command, the cFE shall copy the information
;		contained in the Logic Analyzer Capture Log into a Command
;		Specified file.
;    cES1023.1	If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename.
;    cES1024	Upon receipt of a Command, the cFE shall set the Processor
;		Resets counter to zero.
;    cES1025	Upon receipt of a Command, the cFE shall set the Maximum
;		Processor Resets counter to the Command specified value.
;    cES1028	Upon receipt of a Command, the cFE shall set the System Log Mode
;		to the Command specified mode, either overwrite or discard.
;    cES1509	Upon a Power On Reset, the cFE shall make an entry in the
;		Executive Services Exception and Reset Log, recording the Power
;		On Reset.
;    cES1510	Upon Processor Reset, the cFE shall identify the
;		<PLATFORM_DEFINED> Processor reset sub-type.
;    cES1511	Upon Processor Reset, the cFE shall preserve the Executive
;		Services System Log.
;    cES1512	Upon Processor Reset, the cFE shall preserve the Executive
;		Services Exception and Reset Log.
;    cES1520	Upon Processor Reset, the cFE shall make an entry in the
;		Executive Services Exception and Reset Log recording the
;		Processor Reset.
;    cES1702	The cFE shall detect all unmasked CPU exceptions.
;    cES1702.1	Upon detection of a CPU exception, the cFE shall add an entry in
;		the Executive Services Exception and Reset Log.
;    cES1702.2	If the CPU exception was caused by a cFE Application, the cFE
;		shall restart the cFE Application that caused the exception.
;    cES1703	The cFE shall detect all unmasked processor Floating Point
;		Exceptions.
;    cES1703.1	Upon detection of an unmasked Floating Point exception, the cFE
;		shall add an entry in the Executive Services Exception and Reset
;		Log.
;    cES1703.2	If the Floating Point exception was caused by a cFE Application,
;		the cFE shall restart the cFE Application that caused the
;		exception.
;    cES1706	The cFE shall support a <PLATFORM_DEFINED> byte Executive
;		Services System Log.
;    cES1707	The cFE shall support a (PLATFORM_DEFINED> byte Executive
;		Services Exception and Reset Log.
;    cES1709	If the cFE Core goes through <PLATFORM_DEFINED> Maximum
;		Processor Resets, the cFE shall initiate a Power-On Reset of the
;		cFE.
;
;  Prerequisite Conditions
;       The ASIST ground station is up and running.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	01/16/07	W. Moleski	Initial development.
;	02/06/12	W. Moleski	Added variable for ram disk
;	09/18/14	W. Moleski	Added code to disable SCH debug events
;       05/04/16        W. Moleski      Updated for 6.5.0 testing using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs that connect to
;                                       the host IP.
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;  	ut_pfindicate		Directive to print the pass fail status
;				of a particular requirement number.
;	ut_setupevents 		Directive to look for multiple events and 
;				increment a value for each event to indicate
;				receipt.
;  	ut_setrequirements	Directive to status cfe requirements.
;	ut_sendcmd     		Directive to send commands and verifies command
;				processed and command error counters.
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#include "ut_statusdefs.h"
#include "cfe_es_events.h"
#include "cfe_es_resetTypes.h"
#include "cfe_evs_events.h"
#include "tst_es_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************

#define ES_1005		0
#define ES_10051	1
#define ES_1009		2
#define ES_1010		3
#define ES_1014		4
#define ES_10141	5
#define ES_10142	6
#define ES_101421	7
#define ES_101422	8
#define ES_1015		9
#define ES_1016		10
#define ES_10161	11
#define ES_1017		12
#define ES_1018		13
#define ES_1019		14
#define ES_10191	15
#define ES_1021		16
#define ES_1022		17
#define ES_10221	18
#define ES_10222	19
#define ES_1023		20
#define ES_10231	21
#define ES_1024		22
#define ES_1025		23
#define ES_1028		24
#define ES_1509		25
#define ES_1510		26
#define ES_1511		27
#define ES_1512		28
#define ES_1520		29
#define ES_1522		30
#define ES_1702		31
#define ES_17021	32
#define ES_17022	33
#define ES_1703 	34
#define ES_17031	35
#define ES_17032	36
#define ES_1706		37
#define ES_1707		38
#define ES_1709		39

global ut_req_array_size = 39
global ut_requirement[0 .. ut_req_array_size]

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL i,j, numMaxErrors
LOCAL cmdcnt, errcnt

;**********************************************************************
; Initialize the global requirement values to "U" for Untested.
;**********************************************************************
FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

;**********************************************************************
; Set the local values 
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["ES_1005", "ES_1005.1", "ES_1009", "ES_1010", "ES_1014", "ES_1014.1", "ES_1014.2", "ES_1014.2.1", "ES_1014.2.2", "ES_1015", "ES_1016", "ES_1016.1", "ES_1017", "ES_1018", "ES_1019", "ES_1019.1", "ES_1021", "ES_1022", "ES_1022.1", "ES_1022.2", "ES_1023", "ES_1023.1", "ES_1024", "ES_1025", "ES_1028", "ES_1509", "ES_1510", "ES_1511", "ES_1512", "ES_1520","ES_1522","ES_1702", "ES_1702.1", "ES_1702.2", "ES_1703", "ES_1703.1", "ES_1703.2", "ES_1706", "ES_1707", "ES_1709" ]

local work_dir = %env("WORK")
local filename
local ramDir = "RAM:0"
local hostCPU = "$CPU"

write ";*********************************************************************"
write "; Step 1.0: System Log Test"
write ";*********************************************************************"
write "; Step 1.1: Command a Power-On Reset on $CPU"
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

ut_setrequirements ES_1009, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

;; Disable SCH Debug Events if SCH is running
local SCHisRunning = FALSE
for i = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_EVS_AppData[i].AppName = "SCH") then
    SCHisRunning = TRUE
  endif
enddo

if (SCHisRunning = TRUE) then
  /$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION="SCH" DEBUG
  wait 10
endif


write ";*********************************************************************"
write "; Wait until the user acknowledges that all the data files generated "
write "; by this test procedure have been deleted from the image directory."
write ";*********************************************************************"
write "; Please make sure the following files do not exist in the image "
write "; directory. If they do, delete them."
write ";    1. $sc_$cpu_er110.log "
write ";    2. $sc_$cpu_er13.log "
write ";    3. $sc_$cpu_er21.log "
write ";    4. $sc_$cpu_er210.log "
write ";    5. $sc_$cpu_er23.log "
write ";    6. $sc_$cpu_er25.log "
write ";    7. $sc_$cpu_er283.log "
write ";    8. $sc_$cpu_er285.log "
write ";    9. $sc_$cpu_es_perf33.dat "
write ";   10. $sc_$cpu_es_perf37.dat "
write ";   11. $sc_$cpu_es_syslog1113.log "
write ";   12. $sc_$cpu_es_syslog1115.log "
write ";   13. $sc_$cpu_es_syslog1117.log "
write ";   14. $sc_$cpu_es_syslog113.log "
write ";   15. $sc_$cpu_es_syslog115.log "
write ";   16. $sc_$cpu_es_syslog12.log "
write ";   17. $sc_$cpu_es_syslog15.log "
write ";   18. $sc_$cpu_es_syslog19.log "
write ";"
write "; Enter g or go at the STOL prompt to continue."

wait

write ";*********************************************************************"
write "; Step 1.2: Get the System Log and verify that it contains the "
write ";           proper entries."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog12.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog12.log"
  if file_exists(filename) then
    write "<*> Passed (1016;1016.1) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_1016, "P"
    ut_setrequirements ES_10161, "P"
  else
    write "<!> Failed (1016;1016.1) - Did not download ES system log."
    ut_setrequirements ES_1016, "F"
    ut_setrequirements ES_10161, "F"
  endif
else
  write "<!> Failed (1016;1016.1) - Did not write system log."
  ut_setrequirements ES_1016, "F"
  ut_setrequirements ES_10161, "F"
endif

write ";*********************************************************************"
write "; Step 1.3: Retrieve the Exception and Reset log."
write ";*********************************************************************"
;;; Display the ER Log Window and verify by inspection that the only entry
;;; is the Power-On Reset
page $SC_$CPU_ES_ERLOG
wait 5

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er13.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1017;1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
endif

;; Check the Exception & Reset Log for an entry
local erLogIndex = $SC_$CPU_ES_ERLOGINDEX
if ($SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetType = CFE_ES_POWERON_RESET AND ;;
    $SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed (1509) - ER Log contains an entry indicating the Power-On Reset occurred."
  ut_setrequirements ES_1509, "P"
else
  write "<!> Failed (1509) - ER Log does not contain an entry for the Power-on Reset."
  ut_setrequirements ES_1509, "F"
endif

write ";*********************************************************************"
write "; Step 1.4: Startup the TST_ES application."
write ";*********************************************************************"
;  Dump all running apps
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - App Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - App Info retrieval. Expected Event message ",CFE_ES_ALL_APPS_EID, " was not received"
endif

local found_app = FALSE
local stream

;;Loop thru the table of apps again to verify that the TST_ES app is not running
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if (found_app = FALSE) then
    if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
      found_app = TRUE
    endif
  endif
enddo

if (found_app = FALSE) then
  write "; Starting the TST_ES application. "
  s load_start_app ("TST_ES", hostCPU)
  wait 5

  ;; Dump all running apps again to verify that the TST_ES app is running
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1

  s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
  wait 5

  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - App Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - App Info retrieval. Expected Event message ",CFE_ES_ALL_APPS_EID, " was not received"
  endif

  ;;Loop thru the table of apps again to verify that the TST_ES app is running
  found_app = FALSE
  for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
    if (found_app = FALSE) then
      if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
        found_app = TRUE
      endif
    endif
  enddo

  if (found_app = TRUE) then
    write "<*> Passed (1005;1005.1) - TST_ES app started"
    ut_setrequirements ES_1005, "P"
    ut_setrequirements ES_10051, "P"

    ;; Send commands to subscribe to the TST_ES HK packet
    ;; CPU1 is the default
    stream = x'90D'

    if ("$CPU" = "CPU2") then
       stream = x'A0D'
    elseif ("$CPU" = "CPU3") then
       stream = x'B0D'
    endif

    /$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

    ;; Add an Event Filter for the TST_ES HK Request Event in order to
    ;; only receive this event 1 time rather than every 4 seconds
    /$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
    wait 5
  else
    write "<!> Failed (1005;1005.1) - Failed to start the TST_ES app"
    ut_setrequirements ES_1005, "F"
    ut_setrequirements ES_10051, "F"
  endif
else
  write "; TST_ES application is already running. "
endif

wait 5

write ";*********************************************************************"
write "; Step 1.5: Get the System Log and verify that it contains the events "
write ";           generated in the above step."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=""/ram/waltsSys.log"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1014;1016) - Wrote System Log."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1014;1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1014, "P"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1014;1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1014, "F"
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"waltsSys.log","$sc_$cpu_es_syslog15.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog15.log"
  if file_exists(filename) then
    write "<*> Passed (1014;1014.1;1016) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_1014, "P"
    ut_setrequirements ES_10141, "A"
    ut_setrequirements ES_1016, "P"
  else
  write "<!> Failed (1014;1014.1;1016) - Did not download ES system log."
    ut_setrequirements ES_1014, "F"
    ut_setrequirements ES_10141, "F"
    ut_setrequirements ES_1016, "F"
  endif
else
  write "<!> Failed (1016) - Did not write system log."
  ut_setrequirements ES_1016, "F"
endif

write ";*********************************************************************"
write "; Step 1.6: Command the System Log write mode to OVERWRITE."
write ";*********************************************************************"
;; Send the command to set the mode to OVERWRITE
cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_OVERWRITESYSLOGMODE OVERWRITE

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1028) - Set SysLogMode to OVERWRITE command."
  ut_setrequirements ES_1028, "P"
else
  write "<!> Failed (1028) - Set SysLogMode command."
  ut_setrequirements ES_1028, "F"
endif

write ";*********************************************************************"
write "; Step 1.7: Command a Processor Reset."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

ut_setrequirements ES_1010, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

write ";*********************************************************************"
write "; Step 1.8: Verify that the System Log write mode is set to DISCARD   "
write ";           and the Reset Subtype is set to COMMAND. "
write ";*********************************************************************"
if ($SC_$CPU_ES_SYSLOGMODE = 1) then
  write "<*> Passed (1522) - System Log write mode set to DISCARD after Processor Reset"
  ut_setrequirements ES_1522, "P"
else
  write "<!> Failed (1522) - System Log write mode did not get reset to DISCARD after Processor Reset"
  ut_setrequirements ES_1522, "F"
endif

; Check the Reset Subtype
if ($SC_$CPU_ES_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed (1510) - Reset Subtype indicates the correct value."
  ut_setrequirements ES_1510, "P"
else
  write "<!> Failed (1510) - Reset Type does not indicate Command as expected."
  ut_setrequirements ES_1510, "F"
endif

write ";*********************************************************************"
write "; Step 1.9: Get the System Log and verify that it contains the "
write ";           proper entries."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog19.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog19.log"
  if file_exists(filename) then
    write "<*> Passed (1016;1511) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_1016, "P"
    ut_setrequirements ES_1511, "A"
  else
  write "<!> Failed (1016;1511) - Did not download ES system log."
    ut_setrequirements ES_1016, "F"
    ut_setrequirements ES_1511, "F"
  endif
else
  write "<!> Failed (1016;1511) - Did not write system log."
  ut_setrequirements ES_1016, "F"
  ut_setrequirements ES_1511, "F"
endif

write ";*********************************************************************"
write "; Step 1.10: Retrieve the Exception and Reset log."
write ";*********************************************************************"
; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1
cmdcnt = $SC_$CPU_ES_CMDPC + 1

;; Rather than doing this, send the actual command and 
;; manually ftp it to the ground
/$SC_$CPU_ES_WRITEERLOG2FILE ERLOGFILENAME=""

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1019;1019.1) - Sent the Write ER Log to File Command."
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_10191, "P"
else
  write "<!> Failed (1019;1019.1) - Write ER Log to File Command failed."
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_10191, "F"
endif

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1019;1019.1;1512;1520) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_10191, "P"
  ut_setrequirements ES_1512, "A"
  ut_setrequirements ES_1520, "A"
else
  write "<!> Failed (1019;1019.1;1512;1520) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_10191, "F"
  ut_setrequirements ES_1512, "F"
  ut_setrequirements ES_1520, "F"
endif

;; Get the file to the ground whether or not the event msg was rcv'd
s ftp_file (ramDir,"cfe_erlog.log","$sc_$cpu_er110.log",hostCPU,"G")
wait 5

;; Check if the file above exists and pass the requirement if it does
filename = work_dir & "/image/$sc_$cpu_er110.log"
if file_exists(filename) then
  write "<*> Passed (1019;1019.1) - Retrieved the ER Log from $CPU."
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_10191, "P"
else
  write "<!> Failed (1019;1019.1) - Did not retrieved the ER Log from $CPU."
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_10191, "F"
endif

write ";*********************************************************************"
write "; Step 1.11: System Log size test."
write ";*********************************************************************"
write "; Step 1.11.1: Start the TST_ES test application."
write ";*********************************************************************"
write "; Starting the TST_ES application. "
s load_start_app ("TST_ES", hostCPU)
wait 5

;; Send commands to subscribe to the TST_ES HK packet
;; CPU1 is the default
stream = x'90D'

if ("$CPU" = "CPU2") then
  stream = x'A0D'
elseif ("$CPU" = "CPU3") then
  stream = x'B0D'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

;; Add an Event Filter for the TST_ES HK Request Event in order to
;; only receive this event 1 time rather than every 4 seconds
/$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
wait 5

write ";*********************************************************************"
write "; Step 1.11.2: Verify the size of the System Log. If it is not full, "
write ";              send entries until the System Log is full."
write ";*********************************************************************"
;; CFE_ES_SYSTEM_LOG_SIZE defines the MAX size of the System Log
;; $SC_$CPU_ES_SYSLOGBYTEUSED contains the actual size of the System Log
;; $SC_$CPU_ES_SYSLOGENTRIES contains the entries in the System Log
write "; Max System Log size = ", CFE_ES_SYSTEM_LOG_SIZE 
write "; Max System Log size (HK) = ", $SC_$CPU_ES_SYSLOGSIZE 
write "; Actual System Log size = ", $SC_$CPU_ES_SYSLOGBYTEUSED 
write "; Actual System Log entries = ", $SC_$CPU_ES_SYSLOGENTRIES 

if ($SC_$CPU_ES_SYSLOGBYTEUSED < CFE_ES_SYSTEM_LOG_SIZE) then
  write "; System Log is not full. Sending entries until the log is full. "

  ;; Here we can just send the TST_ES_WRITE2SYSLOG command
  while ($SC_$CPU_ES_SYSLOGBYTEUSED < CFE_ES_SYSTEM_LOG_SIZE) do
    ut_setupevents "$SC", "$CPU", "TST_ES", TST_ES_WRSYSLOG_INF_EID, "INFO", 1
    cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

    /$SC_$CPU_TST_ES_Write2SysLog Message="Writing an entry to fill up the System Log"

    ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Wrote Sys Log message."
    else
      write "<!> Failed - Could not write to the Sys Log."
    endif
    wait 2 
  enddo
else
  write "; System Log is already full. Skipping this step."
endif

write ";*********************************************************************"
write "; Step 1.11.3: Retrieve the System Log and verify it is full."
write ";*********************************************************************"
write "; Actual System Log size = ", $SC_$CPU_ES_SYSLOGBYTEUSED 
write "; Actual System Log entries = ", $SC_$CPU_ES_SYSLOGENTRIES 

if ($SC_$CPU_ES_SYSLOGBYTEUSED = CFE_ES_SYSTEM_LOG_SIZE) then
  write "<*> Passed (1706) - System Log is full."
  ut_setrequirements ES_1706, "P"
else
  write "<!> Failed (1706) - System log is not full"
  ut_setrequirements ES_1706, "F"
endif

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1016) - Wrote System Log."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog1113.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog1113.log"
  if file_exists(filename) then
    write "<*> Passed (1016) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_1016, "P"
  else
  write "<!> Failed (1016) - Did not download ES system log."
    ut_setrequirements ES_1016, "F"
  endif
else
  write "<!> Failed (1016) - Did not write system log."
  ut_setrequirements ES_1016, "F"
endif

write ";*********************************************************************"
write "; Step 1.11.4: Create one more System Log entry with the write mode "
write "; 	      set to discard any new entries."
write ";*********************************************************************"
;; Set the System Log mode to DISCARD (OVERWRITE = 0; DISCARD = 1).
if ($SC_$CPU_ES_SYSLOGMODE = 0) then
  ;; Send the command to set the mode to DISCARD
  cmdcnt = $SC_$CPU_ES_CMDPC + 1

  /$SC_$CPU_ES_OVERWRITESYSLOGMODE DISCARD

  ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1028) - Set SysLogMode to DISCARD command."
    ut_setrequirements ES_1028, "P"
  else
    write "<!> Failed (1028) - Set SysLogMode command."
    ut_setrequirements ES_1028, "F"
  endif
endif

ut_setupevents "$SC", "$CPU", "TST_ES", TST_ES_WRSYSLOG_INF_EID, "INFO", 1
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_Write2SysLog Message="Writing a message to a Full System Log that should get discarded."

ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Wrote Sys Log message."
else
  write "<!> Failed - Could not write to the Sys Log."
endif
wait 5

write ";*********************************************************************"
write "; Step 1.11.5: Retrieve the System Log and verify that the entry above"
write ";              is not in the log and that the log remained full."
write ";*********************************************************************"
;; Verify that the System Log is still full
if ($SC_$CPU_ES_SYSLOGBYTEUSED = CFE_ES_SYSTEM_LOG_SIZE) then
  write "<*> Passed (1706) - System Log is full."
  ut_setrequirements ES_1706, "P"
else
  write "<!> Failed (1706) - System log is not full"
  ut_setrequirements ES_1706, "F"
endif

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog1115.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog1115.log"
  if file_exists(filename) then
    write "<*> Passed (1014.2.2;1016) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_101422, "A"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1014.2.2;1016) - Did not download ES system log."
    ut_setrequirements ES_101422, "F"
    ut_setrequirements ES_1016, "F"
  endif
else
  write "<!> Failed (1014.2.2;1016) - Did not write system log."
  ut_setrequirements ES_101422, "F"
  ut_setrequirements ES_1016, "F"
endif

write ";*********************************************************************"
write "; Step 1.11.6: Create one more System Log entry with the write mode set"
write "; 	     to overwrite the existing entries."
write ";*********************************************************************"
;; Set the System Log mode to OVERWRITE (OVERWRITE = 0; DISCARD = 1).
if ($SC_$CPU_ES_SYSLOGMODE = 1) then
  ;; Send the command to set the mode to OVERWRITE
  cmdcnt = $SC_$CPU_ES_CMDPC + 1

  /$SC_$CPU_ES_OVERWRITESYSLOGMODE OVERWRITE

  ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1028) - Set SysLogMode to OVERWRITE command."
    ut_setrequirements ES_1028, "P"
  else
    write "<!> Failed (1028) - Set SysLogMode command."
    ut_setrequirements ES_1028, "F"
  endif
endif

ut_setupevents "$SC", "$CPU", "TST_ES", TST_ES_WRSYSLOG_INF_EID, "INFO", 1
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_Write2SysLog Message="Writing a message to a Full System Log that should overwrite the first entry."

ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Wrote Sys Log message."
else
  write "<!> Failed - Could not write to the Sys Log."
endif
wait 5

write ";*********************************************************************"
write "; Step 1.11.7: Retrieve the System Log and verify that the entry above"
write ";              is not in the log and that the log remained full."
write ";*********************************************************************"
;; If the mode is OVERWRITE, the first entry will be overwritten
;; Verify that the System Log is still full
write "Bytes Used = ",$SC_$CPU_ES_SYSLOGBYTEUSED

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog1117.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog1117.log"
  if file_exists(filename) then
    write "<*> Passed (1014.2.1;1016) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_101421, "A"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1014.2.1;1016) - Did not download ES system log."
    ut_setrequirements ES_101421, "F"
    ut_setrequirements ES_1016, "F"
  endif
else
  write "<!> Failed (1014.2.1;1016) - Did not write system log."
  ut_setrequirements ES_1016, "F"
  ut_setrequirements ES_101421, "F"
endif

write ";*********************************************************************"
write "; Step 1.12: Clear the System Log."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG1_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_ES_CLEARSYSLOG"
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1015) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1015, "P"
  else
    write "<!> Failed (1015) - Expected Event Message ", CFE_ES_SYSLOG1_INF_EID, " not received."
    ut_setrequirements ES_1015, "F"
  endif
endif

write ";*********************************************************************"
write "; Step 1.13: Retreive the System Log and verify that it is empty."
write ";*********************************************************************"
write "; Actual System Log size = ", $SC_$CPU_ES_SYSLOGBYTEUSED 
write "; Actual System Log entries = ", $SC_$CPU_ES_SYSLOGENTRIES 

if ($SC_$CPU_ES_SYSLOGBYTEUSED = 0) then
  write "<*> Passed (1015) - System Log was empty."
  ut_setrequirements ES_1015, "P"
else
  write "<!> Failed (1015) - System Log was not empty after clear command."
  ut_setrequirements ES_1015, "F"
endif

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog113.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog113.log"
  if file_exists(filename) then
    write "<*> Passed (1016) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_1016, "P"
  else
  write "<!> Failed (1016) - Did not download ES system log."
    ut_setrequirements ES_1016, "F"
  endif
else
  write "<!> Failed (1016) - Did not write system log."
  ut_setrequirements ES_1016, "F"
endif

write ";*********************************************************************"
write "; Step 1.14: Generate a System Log entry that contains a description  "
write ";            string that is too long for the log. A string greater than"
write ";            122 characters is required."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_ES", TST_ES_WRSYSLOG_INF_EID, "INFO", 1
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_Write2SysLog Message="Writing a message to the System Log that is longer than the message size that the System log can handle. The last char =x is marked with an x after the = that we just passed."

ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Wrote Sys Log message."
else
  write "<!> Failed - Could not write to the Sys Log."
endif

wait 5

write ";*********************************************************************"
write "; Step 1.15: Retrieve the System Log and verify that the description  "
write ";            contained in the above entry was truncated."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog115.log",hostCPU,"G")
  wait 5

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog115.log"
  if file_exists(filename) then
    write "<*> Passed (1016) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Did not download ES system log."
    ut_setrequirements ES_1016, "F"
  endif
else
  write "<!> Failed (1016) - Did not write system log."
  ut_setrequirements ES_1016, "F"
endif

write ";*********************************************************************"
write "; Step 1.16: Verify that the cFE calculates the number of bytes used "
write ";            and number of entries in the System Log."
write ";*********************************************************************"
ut_setrequirements ES_10142, "A"

write ";*********************************************************************"
write "; Step 1.17: Send the WRITESYSLOG2FILE command without a path "
write ";            specification for the filename."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_ES_CMDEC + 1

/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="nopathname.log"

ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - WriteSysLog command failed as expected."
else
  write "<!> Failed - WriteSysLog command did not increment the error counter."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message", CFE_ES_SYSLOG2_ERR_EID, " not received."
endif

write ";*********************************************************************"
write "; Step 2.0: Exception and Reset Log Test"
write ";*********************************************************************"
write "; Step 2.1: Retrieve the Exception and Reset Log and verify that it "
write ";           has not changed."
write ";*********************************************************************"
; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er21.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1017;1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
endif

write ";*********************************************************************"
write "; Step 2.2: Non-Core Application Exception test. Send the TST_ES app a"
write ";           command that generates an exception. Verify that the CPU "
write ";	   resets."
write ";*********************************************************************"
/$SC_$CPU_TST_ES_GenException 
wait 10

;; After the exception occurs, verify that the CPU is reset
;; Process this like a Processor Reset
ut_setrequirements ES_1702, "P"
ut_setrequirements ES_1703, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif
wait 5

;; Verify that a Processor Reset occurred
if ($SC_$CPU_ES_RESETTYPE = CFE_ES_PROCESSOR_RESET) then
  write "<*> Passed (1702.2;1703.2) - Telemetry indicated a Processor Reset occurred."
  ut_setrequirements ES_17022, "P"
  ut_setrequirements ES_17032, "P"
else
  write "<!> Failed (1702.2;1703.2) - Telemetry does not indicate a Processor Reset occurred."
  ut_setrequirements ES_17022, "F"
  ut_setrequirements ES_17032, "F"
endif

write ";*********************************************************************"
write "; Step 2.3: Retrieve the Exception and Reset Log and verify that it "
write ";           contains an entry for the exception generated above."
write ";*********************************************************************"
; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er23.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019;1702.1;1703.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_17021, "A"
  ut_setrequirements ES_17031, "A"
else
  write "<!> Failed (1017;1019;1702.1;1703.1) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_17021, "F"
  ut_setrequirements ES_17031, "F"
endif

write ";*********************************************************************"
write "; Step 2.4: Command a Processor Reset."
write ";*********************************************************************"
;; Set the reset counter to 0 so that a Processor Reset actually occurs
/$SC_$CPU_ES_RESETPRCNT
wait 5

/$SC_$CPU_ES_PROCESSORRESET
wait 10

ut_setrequirements ES_1010, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

write ";*********************************************************************"
write "; Step 2.5: Retrieve the Exception and Reset Log and verify that it "
write ";           contains an entry for the Processor Reset."
write ";*********************************************************************"
; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er25.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019;1512;1520) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_1512, "A"
  ut_setrequirements ES_1520, "A"
else
  write "<!> Failed (1017;1019;1512;1520) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_1512, "F"
  ut_setrequirements ES_1520, "F"
endif

write ";*********************************************************************"
write "; Step 2.6: Start the TST_ES application with the Exception action"
write "; 	   set to RESTARTAPP."
write ";*********************************************************************"
;; Load and Start the TST_ES Application with the Reset action set to RESTARTAPP
s load_app (ramDir,"TST_ES", hostCPU)
wait 5
                                                                                
/$SC_$CPU_ES_STARTAPP APPLICATION="TST_ES" APP_ENTRY_PT="TST_ES_TaskMain" APP_FILE_NAME="/ram/tst_es.o" STACKSIZE=x'2000' PRIORITY=x'c8' RESTARTAPP
wait 5

write ";*********************************************************************"
write "; Step 2.7: Non-Core Application Exception test. Send the TST_ES app a"
write ";           command that generates an exception. Verify that the "
write ";           TST_ES application restarts."
write ";*********************************************************************"
;; Setup for the TST_ES initialization event
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_RESTART_APP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_ES, TST_ES_INIT_INF_EID, INFO, 2
                                                                                
local TST_ES_Started = FALSE

/$SC_$CPU_TST_ES_GenException 
wait 20

;; Look for expected event #1
if (($SC_$CPU_find_event[1].num_found_messages = 1) AND ;;
     ($SC_$CPU_find_event[2].num_found_messages = 1)) then
  write "<*> Passed (1702.2;1703.2) - TST_ES restarted after exception"
  ut_setrequirements ES_17022, "P"
  ut_setrequirements ES_17032, "P"
  TST_ES_Started = TRUE
else
  write "<!> Failed (1702.2;1703.2) - TST_ES app did not restart after an exception"
  write "<!> Failed - Expected Event Messages ", CFE_ES_RESTART_APP_INF_EID, " & ", TST_ES_INIT_INF_EID, " not received."
  ut_setrequirements ES_17022, "F"
  ut_setrequirements ES_17032, "F"
endif

write ";*********************************************************************"
write "; Step 2.8.0: Exception and Reset Log Overflow Test."
write ";*********************************************************************"
write "; Step 2.8.1: Make sure the TST_ES Application is running."
write ";*********************************************************************"
;; Check if the TST_ES Application was started in the step above
if (TST_ES_Started = FALSE) then
  write "; Starting the TST_ES application. "
  s load_start_app ("TST_ES", hostCPU)
  wait 5
endif

;; Send commands to subscribe to the TST_ES HK packet
;; CPU1 is the default
stream = x'90D'

if ("$CPU" = "CPU2") then
  stream = x'A0D'
elseif ("$CPU" = "CPU3") then
  stream = x'B0D'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

;; Add an Event Filter for the TST_ES HK Request Event in order to
;; only receive this event 1 time rather than every 4 seconds
/$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
wait 5

write ";*********************************************************************"
write "; Step 2.8.2: Verify the maximum number of ER Log Entries."
write ";*********************************************************************"
write "; Maximum ER Log Entries = ",CFE_ES_ER_LOG_ENTRIES
write "; Maximum ER Log Entries (HK) = ",$SC_$CPU_ES_ERLOGENTRIES
write "; Actual ER Log Entries = ",$SC_$CPU_ES_ERLOGINDEX

local entriesToFill, maxResets

if ($SC_$CPU_ES_ERLOGINDEX < CFE_ES_ER_LOG_ENTRIES) then
  write "; ER Log is not full. Sending entries until the log is full. "
  ;; Determine the number of entries needed to fill the ER Log
  entriesToFill = CFE_ES_ER_LOG_ENTRIES - $SC_$CPU_ES_ERLOGINDEX 
  write "; Need to add ", entriesToFill, " entries in the ER Log."

  for i = 1 to entriesToFill do
    ut_setupevents "$SC", "$CPU", "TST_ES", TST_ES_WRERLOG_INF_EID, "INFO", 1
    cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

    /$SC_$CPU_TST_ES_Write2ERLog Message="Writing an entry to fill up the ER Log"
    ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Wrote message to ER Log."
    else
      write "<!> Failed - Did not write a message to ER Log."
    endif
  enddo
else
  write "; ER Log is already full. Skipping this step. "
endif

write ";*********************************************************************"
write "; Step 2.8.3: Retrieve the ER Log and verify it is full."
write ";*********************************************************************"
write "; Maximum ER Log Entries = ",CFE_ES_ER_LOG_ENTRIES
write "; Actual ER Log Entries = ",$SC_$CPU_ES_ERLOGINDEX
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

;; Verify that the ER Log is full
s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er283.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019;1707) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_1707, "P"
else
  write "<!> Failed (1017;1019;1707) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_1707, "F"
endif

write ";*********************************************************************"
write "; Step 2.8.4: Generate one more entry in the ER Log."
write ";*********************************************************************"
;; Set a varible for the expected ER Log index
;;local expectedERIndex = 0
;;local expectedERIndex =  $SC_$CPU_ES_ERLOGINDEX+1
;;if (expectedERIndex = CFE_ES_ER_LOG_ENTRIES) then
;;  expectedERIndex = 0;
;;endif

ut_setupevents "$SC", "$CPU", "TST_ES", TST_ES_WRERLOG_INF_EID, "INFO", 1
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_Write2ERLog Message="ER Log entry after the log was full!!!"
ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Wrote message to ER Log."
else
  write "<!> Failed - Did not write a message to ER Log."
endif

write ";*********************************************************************"
write "; Step 2.8.5: Retrieve the ER Log and verify that the last message "
write ";             overwrote the oldest entry in the log. "
write ";*********************************************************************"
;; Verify that the ER Log overwrote the first entry

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er285.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019;1707) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
  ut_setrequirements ES_1707, "P"
else
  write "<!> Failed (1017;1019;1707) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
  ut_setrequirements ES_1707, "F"
endif

write ";*********************************************************************"
write "; Step 2.9: Send the command to clear the ER Log."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG1_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_ES_CLEARERLOG"
if (UT_SC_Status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1018) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1018, "P"
  else
    write "<!> Failed (1018) - Expected Event Message ", CFE_ES_SYSLOG1_INF_EID, " not received."
    ut_setrequirements ES_1018, "F"
  endif
endif

write ";*********************************************************************"
write "; Step 2.10: Retrieve the ER Log and verify it is empty."
write ";*********************************************************************"
;; Verify that the ER Log is empty
if ($SC_$CPU_ES_ERLOGINDEX = 0) then
  write "<*> Passed (1018) - ER Log is empty."
  ut_setrequirements ES_1018, "P"
else
  write "<!> Failed (1018) - ER Log was not cleared."
  ut_setrequirements ES_1018, "F"
endif

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_er210.log",hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1017;1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "P"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1017;1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
endif

write ";*********************************************************************"
write "; Step 2.11: Send the WRITEERLOG2FILE command without a path "
write ";            specification for the filename."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_ES_CMDEC + 1

/$SC_$CPU_ES_WRITEERLOG2FILE ERLOGFILENAME="nopathname.log"

ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - WriteERLog command failed as expected."
else
  write "<!> Failed - WriteERLog command did not increment the error counter."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message", CFE_ES_ERLOG2_ERR_EID, " not received."
endif

write ";*********************************************************************"
write "; Step 3.0: Performance Analyzer Capture Log Test."
write ";*********************************************************************"
write "; Step 3.1: Send the command to start recording Performance tags."
write ";*********************************************************************"
; Setup for the Performance Log DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_PERF_STARTCMD_EID, "DEBUG", 1

cmdcnt = $SC_$CPU_ES_CMDPC + 1
/$SC_$CPU_ES_STARTPERF TriggerCenter

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1022) - Sent the Start Performance Analyzer Capture Command."
  ut_setrequirements ES_1022, "P"
else
  write "<!> Failed (1022) - Start Performance Analyzer Capture Command did not increment the CMDPC."
  ut_setrequirements ES_1022, "F"
endif

;; Check for the expected event 
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1022) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements ES_1022, "P"
else
  write "<!> Failed (1022) - Expected Event Message ", CFE_ES_PERF_STARTCMD_EID, " not received."
  ut_setrequirements ES_1022, "F"
endif

wait 5

write "Perf State = ", p@$SC_$CPU_ES_PERFSTATE

write ";*********************************************************************"
write "; Step 3.2: Send the command to stop capturing the tags."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_PERF_STOPCMD_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_PERF_DATAWRITTEN_EID, DEBUG, 2

cmdcnt = $SC_$CPU_ES_CMDPC + 1
/$SC_$CPU_ES_STOPPERF DATAFILENAME=""

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1023) - Sent the Stop Performance Analyzer Capture Command."
  ut_setrequirements ES_1023, "P"
else
  write "<!> Failed (1023) - Stop Performance Analyzer Capture Command did not increment the CMDPC."
  ut_setrequirements ES_1023, "F"
endif

;; Look for expected event #1
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_ES_PERF_STOPCMD_EID, " not received."
endif

;; Look for expected event #2
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ",CFE_ES_PERF_DATAWRITTEN_EID, " not received."
endif

;; Verify that the PerfState = IDLE = 0
if (p@$SC_$CPU_ES_PERFSTATE = "Idle") then
  write "<*> Passed - Performance State is correct."
else
  write "<*> Failed - Performance State = ",p@$SC_$CPU_ES_PERFSTATE,". Expected 'Idle'"
endif

write ";*********************************************************************"
write "; Step 3.3: Retrieve the Performance Analyzer Capture Log."
write ";*********************************************************************"
;; Get the file to the ground whether or not the event msg was rcv'd
s ftp_file (ramDir,"cfe_es_perf.dat","$sc_$cpu_es_perf33.dat",hostCPU,"G")
wait 5

;; Check if the file above exists and pass the requirement if it does
filename = work_dir & "/image/$sc_$cpu_es_perf33.dat"
if file_exists(filename) then
  write "<*> Passed (1021;1022.1;1023.1) - Performance Analyzer Capture Log file downloaded successfully!"
  ut_setrequirements ES_1021, "P"
  ut_setrequirements ES_10221, "A"
  ut_setrequirements ES_10231, "P"
else
  write "<!> Failed (1021;1022.1;1023.1) - Did not download Performance Analyzer Capture log."
  ut_setrequirements ES_1021, "F"
  ut_setrequirements ES_10221, "F"
  ut_setrequirements ES_10231, "F"
endif

write ";*********************************************************************"
write "; Step 3.4: Send the command to stop the TST_ES application."
write ";*********************************************************************"
;; Send the ES_DeleteApp command for TST_ES
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_INF_EID, "INFO", 2
                                                                                
cmdcnt = $SC_$CPU_ES_CMDPC + 1
                                                                                
; Delete the TST_ES application
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_ES"
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - DeleteApp TST_ES command executed properly."
else
  write "<!> Failed - DeleteApp TST_ES command failed."
endif
                                                                                
;; Check if the event was rcv'd
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_ES_STOP_DBG_EID, " was not received"
endif

;; Check if the second event was rcv'd (may need to wait for this one)
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ",CFE_ES_STOP_INF_EID," not received."
endif

;; Dump all running apps again to verify that the TST_ES app is running
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 5
                                                                                
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - App Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - App Info retrieval. Expected Event message ",CFE_ES_ALL_APPS_EID, " was not received"
endif
                                                                                
;; Verify that TST_ES is no longer listed
found_app = FALSE
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
    found_app = TRUE
  endif
enddo

if (found_app = FALSE) then
  write "TST_ES application is not running as expected."
else
  write "TST_ES application is still running. Step 3.5 will not work properly."
endif

write ";*********************************************************************"
write "; Step 3.5: Fill the Performace Capture Log. Verify that the log wraps"
write ";           the entries once the log becomes full."
write ";*********************************************************************"
;; Set the Trigger Mask to all zeros to allow the performance data to wrap
;; Bank0
cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_PERFTRIGMASK BANK0 TRIGGER_MASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Bank0 Trigger Mask command."
else
  write "<!> Failed - Bank0 Trigger Mask command."
endif

;; Check to see if the ES Housekeeping TLM reflects the change
if ($SC_$CPU_ES_PERFTRIGMASK[1] = 0) then
  write "<*> Passed - Bank0 Trigger Mask set to zero."
else
  write "<!> Failed - Bank0 Trigger Mask TLM does not reflect the value set."
endif

;; Bank1
cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_PERFTRIGMASK BANK1 TRIGGER_MASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Bank1 Trigger Mask set to zero."
else
  write "<!> Failed - Bank1 Trigger Mask command."
endif

;; Check to see if the ES Housekeeping TLM reflects the change
if ($SC_$CPU_ES_PERFTRIGMASK[2] = 0) then
  write "<*> Passed - Bank1 Trigger Mask set to zero."
else
  write "<!> Failed - Bank1 Trigger Mask TLM does not reflect the value set."
endif

;; Bank2
cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_PERFTRIGMASK BANK2 TRIGGER_MASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Bank2 Trigger Mask set to zero."
else
  write "<!> Failed - Bank2 Trigger Mask command."
endif

;; Check to see if the ES Housekeeping TLM reflects the change
if ($SC_$CPU_ES_PERFTRIGMASK[3] = 0) then
  write "<*> Passed - Bank2 Trigger Mask set to zero."
else
  write "<!> Failed - Bank2 Trigger Mask TLM does not reflect the value set."
endif

;; Bank3
cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_PERFTRIGMASK BANK3 TRIGGER_MASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Bank3 Trigger Mask set to zero."
else
  write "<!> Failed - Set Bank3 Trigger Mask command."
endif

;; Check to see if the ES Housekeeping TLM reflects the change
if ($SC_$CPU_ES_PERFTRIGMASK[4] = 0) then
  write "<*> Passed - Bank3 Trigger Mask set to zero."
else
  write "<!> Failed - Bank3 Trigger Mask TLM does not reflect the value set."
endif

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_PERF_STARTCMD_EID, "DEBUG", 1

cmdcnt = $SC_$CPU_ES_CMDPC + 1
/$SC_$CPU_ES_STARTPERF TriggerStart

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1022) - Sent the Start Performance Analyzer Capture Command."
  ut_setrequirements ES_1022, "P"
else
  write "<!> Failed (1022) - Start Performance Analyzer Capture Command did not increment the CMDPC."
  ut_setrequirements ES_1022, "F"
endif

;; Check for the expected event 
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1022) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements ES_1022, "P"
else
  write "<!> Failed (1022) - Expected Event Message ", CFE_ES_PERF_STARTCMD_EID, " not received."
  ut_setrequirements ES_1022, "F"
endif

;; Wait until the ES HK data point indicates the PERF log is full
wait until ($SC_$CPU_ES_PerfDataCnt = CFE_ES_PERF_DATA_BUFFER_SIZE)
s load_start_app ("TST_ES", hostCPU)
wait 5

;; Send commands to subscribe to the TST_ES HK packet
;; CPU1 is the default
stream = x'90D'

if ("$CPU" = "CPU2") then
  stream = x'A0D'
elseif ("$CPU" = "CPU3") then
  stream = x'B0D'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

;; Add an Event Filter for the TST_ES HK Request Event in order to
;; only receive this event 1 time rather than every 4 seconds
/$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
wait 5

;; Wait for the Performance Capture Log to generate data for TST_ES
wait 30

write ";*********************************************************************"
write "; Step 3.6: Send the command to stop capturing the tags."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_PERF_STOPCMD_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_PERF_DATAWRITTEN_EID, DEBUG, 2

cmdcnt = $SC_$CPU_ES_CMDPC + 1
/$SC_$CPU_ES_STOPPERF DATAFILENAME=""

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1023) - Sent the Stop Performance Analyzer Capture Command."
  ut_setrequirements ES_1023, "P"
else
  write "<!> Failed (1023) - Stop Performance Analyzer Capture Command did not increment the CMDPC."
  ut_setrequirements ES_1023, "F"
endif
wait 5

;; Look for expected event #1
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_ES_PERF_STOPCMD_EID, " not received."
endif

;; Wait until the Data Written event occurs
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_ES_PERF_DATAWRITTEN_EID, " not received."
endif

write ";*********************************************************************"
write "; Step 3.7: Retrieve the Performance Analyzer Capture Log."
write ";*********************************************************************"
;; Get the file to the ground whether or not the event msg was rcv'd
s ftp_file (ramDir,"cfe_es_perf.dat","$sc_$cpu_es_perf37.dat",hostCPU,"G")
wait 5

;; Check if the file above exists and pass the requirement if it does
filename = work_dir & "/image/$sc_$cpu_es_perf37.dat"
if file_exists(filename) then
  write "<*> Passed (1021;1022.1;1022.2;1023.1) - Performance Analyzer Capture Log file downloaded successfully!"
  ut_setrequirements ES_1021, "P"
  ut_setrequirements ES_10221, "A"
  ut_setrequirements ES_10222, "A"
  ut_setrequirements ES_10231, "P"
else
  write "<!> Failed (1021;1022.1;1022.2;1023.1) - Did not download Performance Analyzer Capture log."
  ut_setrequirements ES_1021, "F"
  ut_setrequirements ES_10221, "F"
  ut_setrequirements ES_10222, "F"
  ut_setrequirements ES_10231, "F"
endif

write ";*********************************************************************"
write "; Step 3.8: Set the Performance Analyzer Filter Mask."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_PERF_FILTMSKCMD_EID, DEBUG, 1

;; Save the original value
local bank0Val = $SC_$CPU_ES_PerfFltrMask[1]

cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_PERFFLTRMASK BANK0 FILTER_MASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Bank0 Filter Mask command."
else
  write "<!> Failed - Bank0 Filter Mask command."
endif

;; Look for expected event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_ES_PERF_FILTMSKCMD_EID, " not received."
endif
wait 5

;; Check to see if the ES Housekeeping TLM reflects the change
if ($SC_$CPU_ES_PERFFLTRMASK[1] = 0) then
  write "<*> Passed - Bank0 Filter Mask set to zero."
else
  write "<!> Failed - Bank0 Filter Mask TLM does not reflect the value set."
endif

cmdcnt = $SC_$CPU_ES_CMDPC + 1

; Set it back to original value
/$SC_$CPU_ES_PERFFLTRMASK BANK0 FILTER_MASK=bank0Val
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Bank0 Filter Mask reset."
else
  write "<!> Failed - Reset Bank0 Filter Mask command."
endif

;; Look for expected event
if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_ES_PERF_FILTMSKCMD_EID, " not received."
endif

write ";*********************************************************************"
write "; Step 3.9: Send the PERFSTOP command without a path specification for"
write "; 	   the filename."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_PERF_LOG_ERR_EID, ERROR, 1

errcnt = $SC_$CPU_ES_CMDEC + 1
/$SC_$CPU_ES_STOPPERF DATAFILENAME="nopathname"

ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - StopPerf command failed as expected."
else
  write "<!> Failed - StopPerf command did not increment the CMDEC."
endif

;; Look for expected event #1
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_ES_PERF_LOG_ERR_EID, " not received."
endif

write ";*********************************************************************"
write "; Step 4.0: Maximum Processor Reset Test."
write ";*********************************************************************"
write "; Step 4.1: Reset the Processor Reset counter to zero (0). "
write ";*********************************************************************"
;; $SC_$CPU_ES_PROCRESETCNT contains the number of Processor Resets
if ($SC_$CPU_ES_PROCRESETCNT = 0) then
  ;; Perform a Processor Reset to increment the counter
  /$SC_$CPU_ES_PROCESSORRESET
  wait 10

  close_data_center
  wait 75

  cfe_startup $CPU
  wait 5

  ;; Need to turn debug events back on
endif

cmdcnt = $SC_$CPU_ES_CMDPC + 1
; Send the command to reset the counter
/$SC_$CPU_ES_RESETPRCNT
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1024) - Reset PR Counter Command."
  ut_setrequirements ES_1024, "P"
else
  write "<!> Failed (1024) - Reset PR Counter Command did not increment the CMDPC."
  ut_setrequirements ES_1024, "F"
endif
 
; Check the telemetry counter
if ($SC_$CPU_ES_PROCRESETCNT = 0) then
  write "<*> Passed (1024) - Processor Reset Counter set to 0."
  ut_setrequirements ES_1024, "P"
else
  write "<!> Failed (1024) - Processor Reset Counter did not reset."
  ut_setrequirements ES_1024, "F"
endif

write ";*********************************************************************"
write "; Step 4.2: Set the Max Processor Reset count to one more than the "
write "; 	   current Processor Reset counter."
write ";*********************************************************************"
;; $SC_$CPU_ES_MAXPROCRESETS contains the max Processor Resets before a POR
local maxResetCnt = $SC_$CPU_ES_PROCRESETCNT + 1
cmdcnt = $SC_$CPU_ES_CMDPC + 1

;; Setup for any event from the command
/$SC_$CPU_ES_SETMAXPRCNT Max_count=maxResetCnt
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1025) - Set Max PR Count Command."
  ut_setrequirements ES_1025, "P"
else
  write "<!> Failed (1025) - Set Max PR Count Command did not increment the CMDPC."
  ut_setrequirements ES_1025, "F"
endif
 
if ($SC_$CPU_ES_MAXPROCRESETS = maxResetCnt) then
  write "<*> Passed (1025) - Max PR Count set to ", maxResetCnt
  ut_setrequirements ES_1025, "P"
else
  write "<!> Failed (1025) - Max PR Count = ",$SC_$CPU_ES_MAXPROCRESETS," expected ",maxResetCnt
  ut_setrequirements ES_1025, "F"
endif

write ";*********************************************************************"
write "; Step 4.3: Determine the Processor Reset counter value and send the "
write "; 	   Processor Reset command until the Max is reached."
write ";*********************************************************************"
;; $SC_$CPU_ES_PROCRESETCNT contains the number of Processor Resets
;; $SC_$CPU_ES_MAXPROCRESETS contains the max Processor Resets before a POR
;; Determine the # of resets needed
local resetsToDo = $SC_$CPU_ES_MAXPROCRESETS - $SC_$CPU_ES_PROCRESETCNT

write "; Will perform ", resetsToDo+1, " Processor Resets"
;; When using commanded resets, the ES checks this count on the way down
;; to determine whether to do a Processor or Power-On Reset
for i = 0 to resetsToDo do
  /$SC_$CPU_ES_PROCESSORRESET
  wait 10

  close_data_center
  wait 75

  cfe_startup $CPU
  wait 5
enddo

write ";*********************************************************************"
write "; Step 4.4: After the last Processor Reset, verify that a Power-On "
write "; 	   Reset was performed."
write ";*********************************************************************"
;; $SC_$CPU_ES_PROCRESETCNT should be 0
;; $SC_$CPU_ES_RESETTYPE should specify POWERON

if ($SC_$CPU_ES_ResetType = CFE_ES_POWERON_RESET AND ;;
    $SC_$CPU_ES_PROCRESETCNT = 0) then
  write "<*> Passed (1709) - Power-On Reset occurred after last Processor reset."
  ut_setrequirements ES_1709, "P"
else
  write "<!> Failed (1709) - Power-On Reset did not occur after maximum Processor resets."
  ut_setrequirements ES_1709, "F"
endif

write "**** Requirements Status Reporting"

write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
