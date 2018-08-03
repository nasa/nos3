PROC $sc_$cpu_es_reset
;*******************************************************************************
;  Test Name:  $sc_$cpu_es_reset
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify the cFE Executive Services (ES)
;	software meets the requirements defined in the SRS for power-on and
;	processor resets.
;
;  Requirements Tested
;    cES1000	Upon receipt of a Command, the cFE shall generate a Software Bus
;		message that includes the following items:
;			a). Number of Registered Applications
;			b). Number of Registered Child Tasks
;			c). Number of Registered Shared Libraries
;			d). Reset Type
;			e). Reset Subtype
;			f). Number of entries in System Log
;			g). Percent of System Log Used
;			h). Current Exception and Reset Log Index
;			i). Number of Processor Resets
;			j). Maximum Number of Processor Resets before a
;			    Power-On Reset
;			k). Boot Source
;			l). ES Valid Command Counter
;			m). ES Invalid Command Counter
;    cES1001	Upon receipt of a Command, the cFE shall generate a NO-OP event
;		message.
;    cES1002	Upon receipt of a valid Command, the cFE shall increment a valid
;		Command counter.
;    cES1003	Upon receipt of an invalid Command, the cFE shall increment a
;		valid Command counter and generate an event message.
;    cES1004	Upon receipt of a Command, the cFE shall set to zero the valid
;		Command counter and invalid Command counter.
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
;    cES1012	Upon receipt of a Command, the cFE shall generate a file that
;		contains all properties and states of all cFE Applications that
;		are defined in the System Resources Definition.
;    cES1016	Upon receipt of a Command, the cFE shall copy the information
;		contained in the Executive Services System Log into a Command
;		specified file.
;    cES1016.1	If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename.
;    cES1017	The cFE shall maintain an Executive Services Exception and Reset;		Log which will log critical system data for exceptions and
;		resets including: 
;			a) A time stamp
;			b) Processor Context information
;			c) Critical system variables
;			d) ASCII string stating the reason for the reset
;    cES1019	Upon receipt of a Command, the cFE shall copy the information
;		contained in the Executive Services Exception and Reset Log
;		Buffer into a Command specified file.
;    cES1301	Upon receipt of a Request, the cFE shall provide the type of
;		last reset perfomed by the processor.
;    cES1301.1	The reset types include:
;			1) Power On Reset
;			2) Processor Reset
;    cES1317	Upon receipt of a Request, the cFE shall perform a Power On
;		Reset of the Core Flight Executive.
;    cES1318	Upon receipt of a Request, the cFE shall perform a Processor
;		Reset of the Core Flight Executive.
;    cES1500	Upon a Power-on Reset, the cFE shall identify the
;		<PLATFORM_DEFINED> Power On reset sub-type.
;    cES1501	Upon a Power-On Reset, the cFE shall clear the Executive
;		Services System Log.
;    cES1502	Upon a Power-On Reset, the cFE shall clear the Executive
;		Services Exception and Reset Log.
;    cES1503	Upon a Power-On Reset, the cFE shall clear the Volatile File
;		system.
;    cES1504	Upon a Power-On Reset, the cFE shall clear the contents of the
;		Critical Data Store.
;    cES1505	Upon a Power-On Reset, the cFE shall create all operating system
;		objects required by the cFE.
;    cES1506	Upon a Power-On Reset, the cFE shall mount the non-volatile file
;		system.
;    cES1507	Upon a Power-On Reset, the cFE shall create, format and mount
;		the volatile file system.
;    cES1508	Upon a Power-On Reset, the cFE shall process all entries in the
;		cFE Startup File located in the non-volatile file system.
;    cES1508.1	The cFE shall create and start cFE Applications according to the
;		entry in the cFE Startup file.
;    cES1508.2	The cFE shall create and initialize cFE Shared Libraries
;		according to the entry in the cFE Startup File.
;    cES1508.3	The cFE shall create and initialize cFE Device Drivers according
;		to the entry in the cFE Startup File.
;    cES1509	Upon a Power-On Reset, the cFE shall make an entry in the
;		Executive Services Exception and Reset Log, recording the Power
;		On Reset.
;    cES1510	Upon Processor Reset, the cFE shall identify the
;		<PLATFORM_DEFINED> Processor reset sub-type.
;    cES1511	Upon Processor Reset, the cFE shall preserve the Executive
;		Services System Log.
;    cES1512	Upon Processor Reset, the cFE shall preserve the Executive
;		Services Exception and Reset Log.
;    cES1513	Upon Processor Reset, the cFE shall preserve the Volatile File
;		system.
;    cES1514	Upon Processor Reset, the cFE shall preserve the contents of the
;		Critical Data Store.
;    cES1515	Upon Processor Reset, the cFE shall create all operating system
;		objects required by the cFE.
;    cES1516	Upon Processor Reset, the cFE shall mount the non-volatile file
;		system.
;    cES1517	Upon Processor Reset, the cFE shall check and mount the volatile
;		file system.
;    cES1518	Upon Processor Reset, the cFE shall process all entries in the
;		cFE Startup File located in the volatile file system.
;    cES1518.1	The cFE shall create and start cFE Applications according to the
;		entry in the cFE Startup File.
;    cES1518.2	The cFE shall create and initialize Shared Libraries according
;		to the entry in the cFE Startup File.
;    cES1519	If the system startup file is not present in the volatile file
;		system then the cFE shall process all entries in the cFE Startup
;		File located in the non-volatile file system.
;    cES1519.1	The cFE shall create and start cFE Applications according to the
;		entry in the cFE Startup File.
;    cES1519.2	The cFE shall create and initialize Shared Libraries according
;		to the entry in the cFE Startup File.
;    cES1520	Upon Processor Reset, the cFE shall make an entry in the
;		Executive Services Exception and Reset Log recording the
;		Processor Reset.
;    cES1521	Upon Processor Reset, the cFE shall preserve the following:
;			a) Boot Source
;			b) Reset Type
;			c) Reset Subtype
;			d) Reset Reason
;			e) Number of Processor Resets
;			f) Maximum Processor Resets
;			g) Number of entries in System Log
;			h) Percent of System Log Used
;
;  Prerequisite Conditions
;       The ASIST ground station is up and running.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	01/03/07	W. Moleski	Initial development.
;	09/18/14	W. Moleski	Added code to disable SCH Debug events
;					after restarts.
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
;  	ut_setupevents		Directive to look for a particular event
;				and increment a value in the CVT to
;				indicate receipt.
;  	ut_setrequirements	Directive to status cfe requirements.
;	ut_sendcmd     		Directive to send EVS commands Verifies command
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

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************

#define ES_1000		0
#define ES_1001		1
#define ES_1002		2
#define ES_1003		3
#define ES_1004		4
#define ES_1005		5
#define ES_10051	6
#define ES_1009		7
#define ES_1010		8
#define ES_1012		9
#define ES_1016		10
#define ES_10161	11
#define ES_1017		12
#define ES_1019		13
#define ES_1301		14
#define ES_13011	15
#define ES_1317		16
#define ES_1318		17
#define ES_1500		18
#define ES_1501		19
#define ES_1502		20
#define ES_1503		21
#define ES_1504		22
#define ES_1505		23
#define ES_1506		24
#define ES_1507		25
#define ES_1508		26
#define ES_15081	27
#define ES_15082	28
#define ES_15083	29
#define ES_1509		30
#define ES_1510		31
#define ES_1511		32
#define ES_1512		33
#define ES_1513		34
#define ES_1514		35
#define ES_1515		36
#define ES_1516		37
#define ES_1517		38
#define ES_1518		39
#define ES_15181	40
#define ES_15182	41
#define ES_1519		42
#define ES_15191	43
#define ES_15192	44
#define ES_1520		45
#define ES_1521		46

global ut_req_array_size = 46
global ut_requirement[0 .. ut_req_array_size]

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL i,j, numMaxErrors
LOCAL cmdcnt, errcnt
local ramDir = "RAM:0"
local hostCPU = "$CPU"

;**********************************************************************
; Initialize the global requirement values to "U" for Untested.
;**********************************************************************
FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

;**********************************************************************
; Set the local values 
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["ES_1000", "ES_1001", "ES_1002", "ES_1003", "ES_1004", "ES_1005", "ES_1005.1", "ES_1009", "ES_1010", "ES_1012", "ES_1016", "ES_1016.1", "ES_1017", "ES_1019", "ES_1301", "ES_1301.1", "ES_1317", "ES_1318", "ES_1500", "ES_1501", "ES_1502", "ES_1503", "ES_1504", "ES_1505", "ES_1506", "ES_1507", "ES_1508", "ES_1508.1", "ES_1508.2", "ES_1508.3", "ES_1509", "ES_1510", "ES_1511", "ES_1512", "ES_1513", "ES_1514", "ES_1515", "ES_1516", "ES_1517", "ES_1518","ES_1518.1", "ES_1518.2", "ES_1519", "ES_1519.1", "ES_1519.2", "ES_1520","ES_1521" ]

write ";*********************************************************************"
write "; Step 1.0: Perform the first Power-On Reset "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 1.1: Cycle the power on $CPU"
write ";*********************************************************************"

write " Go to the CPU and ground yourself."
write " Cycle the power on the front of the CPU rack."
write " Wait for at least 30 seconds."
wait 30

write " Turn the power back on."
wait 20

close_data_center
wait 75

cfe_startup $CPU
wait 5

ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

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
write "; Step 1.2: Verify the ES Housekeeping Telemetry items."
write ";*********************************************************************"
write "; Registered CORE apps    = ", $SC_$CPU_ES_RegCoreApps
write "; Registered Ext apps     = ", $SC_$CPU_ES_RegExtApps
write "; Registered Tasks        = ", $SC_$CPU_ES_REGTASKS
write "; Registered Libraries    = ", $SC_$CPU_ES_REGLIBS
write "; Processor Resets        = ", $SC_$CPU_ES_ProcResetCnt
write "; Boot Source             = ", $SC_$CPU_ES_BootSource
write "; Valid Command Counter   = ", $SC_$CPU_ES_CMDPC
write "; Invalid Command Counter = ", $SC_$CPU_ES_CMDEC
write "; Reset Type              = ", $SC_$CPU_ES_ResetType
write "; Reset Subtype           = ", $SC_$CPU_ES_ResetSubtype

if ($SC_$CPU_ES_RegExtApps = 3 AND $SC_$CPU_ES_RegCoreApps = 5 AND ;;
    $SC_$CPU_ES_ProcResetCnt = 0 AND $SC_$CPU_ES_BootSource = 1 AND ;;
    $SC_$CPU_ES_CMDPC = 0 AND  $SC_$CPU_ES_CMDEC = 0 AND ;;
    $SC_$CPU_ES_ResetType = CFE_ES_POWERON_RESET AND ;;
    $SC_$CPU_ES_ResetSubtype = CFE_ES_POWER_CYCLE) then
  write "<*> Passed (1000;1301.1;1500) - ES HK items are correct."
  ut_setrequirements ES_1000, "P"
  ut_setrequirements ES_13011, "P"
  ut_setrequirements ES_1500, "P"
else
  write "<!> Failed (1000;1301.1;1500) - ES HK items are not as expected."
  ut_setrequirements ES_1000, "F"
  ut_setrequirements ES_13011, "F"
  ut_setrequirements ES_1500, "F"
endif

write ";*********************************************************************"
write "; Step 1.3: Remove files that are used to verify requirements."
write "; 	   Please make sure the following files do not exist in the "
write "; 	   image directory. If they do, delete them."
write ";	   1. cfe_es_startup_nv.scr"
write "; 	   2. cfe_es_startup_NV_311.scr"
write ";	   3. cfe_es_startup_nv_410.scr"
write ";	   4. cfe_es_startup_vol_Verify"
write ";	   5. es_testfile_1_Verify"
write ";	   6. es_testfile_2_Verify"
write ";	   7. es_testfile_3_Verify"
write ";	   8. es_testfile_4_Verify"
write ";	   9. es_testfile_5_Verify"
write ";	  10. es_testfile_6_Verify"
write ";	  11. $sc_$cpu_es_syslog145.log"
write ";*********************************************************************"
write "; Enter g or go at the STOL prompt to continue."
wait

write ";*********************************************************************"
write "; Step 1.4: Retrieve the Exception and Reset log."
write ";*********************************************************************"
;;; Display the ER Log Window and verify by inspection that the only entry
;;; is the Power-On Reset
page $SC_$CPU_ES_ERLOG
wait 10

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

s get_file_to_cvt (ramDir,"cfe_erlog.log","$sc_$cpu_reset_er13.log",hostCPU)
wait 10

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1017;1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1017, "A"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1017;1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1017, "F"
  ut_setrequirements ES_1019, "F"
endif

write ";*********************************************************************"
write "; Step 1.5: Verify the non-volatile file system."
write ";*********************************************************************"
write "; Step 1.5.1: Verify the non-volatile file system. This step verifies "
write ";             that it is mounted, and formatted. The file system is "
write ";             mounted if the startup file can be ftp'd to the ground. "
write ";             The file system is formatted if it does not contain any "
write ";             files."
write ";*********************************************************************"
;; ftp the file referenced by CFE_ES_NONVOL_STARTUP_FILE from the spacecraft
s ftp_file ("CF:0/apps","cfe_es_startup.scr","cfe_es_startup_nv.scr",hostCPU,"G")
wait 10

;; Verify that the file was retrieved from the spacecraft
local work_dir = %env("WORK")
local filename = work_dir & "/image/cfe_es_startup_nv.scr"

if file_exists(filename) then
  write "<*> Passed (1506;1508) - Non-volatile Startup file successfully downloaded"
  ut_setrequirements ES_1506, "P"
  ut_setrequirements ES_1508, "P"
else
  write "<!> Failed (1506;1508) - Non-volatile Starup file did not download properly."
  ut_setrequirements ES_1506, "F"
  ut_setrequirements ES_1508, "F"
endif
 
write ";*********************************************************************"
write "; Step 1.5.2: Verify the volatile file system. This step verifies that"
write ";             the file system is formatted if it does not contain any "
write ";             files."
write ";*********************************************************************"
write " Verify the volatile file system. Enter 'g' or 'go' when completed."
wait

write ";*********************************************************************"
write "; Step 1.5.3: Get the EVS local event log."
write ";*********************************************************************"
;; Display the EVS Event Log page
page $SC_$CPU_EVS_LOG
wait 10

ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_WRLOG_EID, "DEBUG", 1
s get_file_to_cvt (ramDir, "cfe_evs.log", "$sc_$cpu_cfe_evs.log", hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
endif

write ";*********************************************************************"
write "; Step 1.5.4: Dump the applications properties"
write ";*********************************************************************"
page $SC_$CPU_ES_APP_INFO
wait 10
;  Dump all running apps
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1012;1508.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1012, "P"
  ut_setrequirements ES_15081, "P"
else
  write "<!> Failed (1012;1508.1) - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
  ut_setrequirements ES_1012, "F"
  ut_setrequirements ES_15081, "F"
endif

local app_index
;Loop thru the table of apps
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName <> "") then
    write "Application #", app_index, ":"
    write "  Name        => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppName
    write "  ID          => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppId
    write "  Task Name   => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskName
    write "  Task ID     => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskId
    write "  Child Tasks => ", $SC_$CPU_ES_ALE[app_index].ES_AL_ChildTasks
  endif
enddo

write ";*********************************************************************"
write "; Step 1.5.5: Get the System Log and verify that it contains the "
write ";             proper entries."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1016) - Wrote System Log."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed (1016) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements ES_1016, "P"
  else
    write "<!> Failed (1016) - Expected Event Message", CFE_ES_SYSLOG2_EID, " not received."
    ut_setrequirements ES_1016, "F"
  endif

  ;; Get the file to the ground whether or not the event msg was rcv'd
  s ftp_file (ramDir,"cfe_es_syslog.log","$sc_$cpu_es_syslog145.log",hostCPU,"G")
  wait 10

  ;; Check if the file above exists and pass the requirement if it does
  filename = work_dir & "/image/$sc_$cpu_es_syslog145.log"
  if file_exists(filename) then
    write "<*> Passed (1016.1;1505;1508.2) - ES System Log file downloaded successfully!"
    ut_setrequirements ES_10161, "P"
    ut_setrequirements ES_1505, "A"
    ut_setrequirements ES_15082, "A"
  else
  write "<!> Failed (1016;1505;1508.2) - Did not download ES system log."
    ut_setrequirements ES_10161, "F"
    ut_setrequirements ES_1505, "F"
    ut_setrequirements ES_15082, "F"
  endif
else
  write "<!> Failed (1016;1505;1508.2) - Did not write system log."
  ut_setrequirements ES_1016, "F"
  ut_setrequirements ES_1505, "F"
  ut_setrequirements ES_15082, "F"
endif

write ";*********************************************************************"
write "; Step 2.0: Command Tests to increment the counters."
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 2.1: NOOP Command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_NOOP_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_ES_NOOP"
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1001;1002) - NOOP command sent properly."
  ut_setrequirements ES_1001, "P"
  ut_setrequirements ES_1002, "P"
else
  write "<!> Failed (1001;1002) - NOOP command."
  ut_setrequirements ES_1001, "F"
  ut_setrequirements ES_1002, "F"
endif

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1001, "P"
else
  write "<!> Failed (1001) - Expected Event message ",CFE_ES_NOOP_INF_EID, " was not received"
  ut_setrequirements ES_1001, "F"
endif

write ";*********************************************************************"
write "; Step 2.2: Send an invalid Command in order to increment the command "
write ";           error counter."
write ";*********************************************************************"
;;;; Note that this is an ES command with an invalid 
;;;; function code = hex 'aa' (42 decimal)
errcnt = $SC_$CPU_ES_CMDEC + 1
                                                                                
;; Send commands to subscribe to the TST_ES HK packet
;; CPU1 is the default
/raw 1806c0000001aa

if ("$CPU" = "CPU2") then
  /raw 1826c0000001aa
elseif ("$CPU" = "CPU3") then
  /raw 1846c0000001aa
endif
                                                                                
ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Error count incremented as expected."
  ut_setrequirements ES_1003, "P"
else
  write "<!> Failed (1003) - Error count did not change."
  ut_setrequirements ES_1003, "F"
endif

write ";*********************************************************************"
write "; Step 2.3: Reset Counters Command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_RESET_INF_EID, "INFO", 1

/$SC_$CPU_ES_RESETCTRS
wait 5
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1004, "P"
else
  write "<!> Failed (1004) - Expected Event message ",CFE_ES_NOOP_INF_EID, " was not received"
  ut_setrequirements ES_1004, "F"
endif

; Check if the CMDPC counter reset
ut_tlmwait $SC_$CPU_ES_CMDPC, 0
                                                                                
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command counter was reset!"
  ut_setrequirements ES_1004, "P"
else
  write "<!> Failed (1004) - Command counter did not reset!"
  ut_setrequirements ES_1004, "F"
endif

; Check if the CMDEC counter reset
ut_tlmwait $SC_$CPU_ES_CMDEC, 0
                                                                                
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Error counter was reset!"
  ut_setrequirements ES_1004, "P"
else
  write "<!> Failed (1004) - Error counter did not reset!"
  ut_setrequirements ES_1004, "F"
endif

write ";*********************************************************************"
write "; Step 2.4: Send the NOOP Command again to increment the counter"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_NOOP_INF_EID, "INFO", 1

cmdcnt = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_NOOP

; Check if the CMDPC incremented
ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
                                                                                
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;1002) - NOOP Command executed properly."
  ut_setrequirements ES_1001, "P"
  ut_setrequirements ES_1002, "P"
else
  write "<!> Failed (1001;1002) - NOOP Command did not increment the CMDPC counter."
  ut_setrequirements ES_1001, "F"
  ut_setrequirements ES_1002, "F"
endif

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1001, "P"
else
  write "<!> Failed (1001) - Expected Event message ",CFE_ES_NOOP_INF_EID, " was not received"
  ut_setrequirements ES_1001, "F"
endif

write ";*********************************************************************"
write "; Step 2.5: Send an invalid Command in order to increment the command "
write ";           error counter."
write ";*********************************************************************"
;;;; Note that this is an ES command with an invalid 
;;;; function code = hex 'aa' (42 decimal)
errcnt = $SC_$CPU_ES_CMDEC + 1
                                                                                
; Send commands to subscribe to the TST_ES HK packet
;; CPU1 is the default
/raw 1806c0000001aa

if ("$CPU" = "CPU2") then
  /raw 1826c0000001aa
elseif ("$CPU" = "CPU3") then
  /raw 1846c0000001aa
endif
                                                                                
ut_tlmwait $SC_$CPU_ES_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Error count incremented as expected."
  ut_setrequirements ES_1003, "P"
else
  write "<!> Failed (1003) - Error count did not change."
  ut_setrequirements ES_1003, "F"
endif

write ";*********************************************************************"
write "; Step 3.0: Processor Reset Test 1"
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 3.1: Upload some files to the spacecraft in order to modify the"
write ";           contents of the volatile file system."
write ";*********************************************************************"
;; ftp several test files to the volatile file system
s ftp_file (ramDir,"es_testfile_1","es_testfile_1",hostCPU,"P")
wait 5
s ftp_file (ramDir,"es_testfile_2","es_testfile_2",hostCPU,"P")
wait 5

write ";*********************************************************************"
write "; Step 3.2: Modify the Critical data variables for TST_ES."
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 3.2.1: Startup the TST_ES application."
write ";*********************************************************************"
;  Dump all running apps
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1012;1508.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1012, "P"
  ut_setrequirements ES_15081, "P"
else
  write "<!> Failed (1012;1508.1) - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
  ut_setrequirements ES_1012, "F"
  ut_setrequirements ES_15081, "F"
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
  wait 10

  ;; Dump all running apps again to verify that the TST_ES app is running
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
  s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
  wait 10

  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed (1012;1508.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements ES_1012, "P"
    ut_setrequirements ES_15081, "P"
  else
    write "<!> Failed (1012;1508.1) - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
    ut_setrequirements ES_1012, "F"
    ut_setrequirements ES_15081, "F"
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
write "; Step 3.2.2: Increment the TST_ES CDS variables and save the items "
write "; 	     that are preserved on a Processor Reset."
write ";*********************************************************************"
;;;; Save the current values of the CDS Variables
local cdsVar1, cdsVar2, cdsVar3, cdsVar4, cdsVar5

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

local newCDSVar1, newCDSVar2, newCDSVar3, newCDSVar4, newCDSVar5
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

;;;; Save the current values of other preserved variables
local bootSource, sysLogEntries, sysLogUsed
bootSource = $SC_$CPU_ES_BootSource
sysLogEntries = $SC_$CPU_ES_SYSLOGENTRIES
sysLogUsed = $SC_$CPU_ES_SYSLOGByteUsed
write "; BEFORE Reset - Boot Source = ", bootSource
write "; BEFORE Reset - System Log Entries = ", sysLogEntries
write "; BEFORE Reset - System Log Bytes Used = ", sysLogUsed
wait 5

write ";*********************************************************************"
write "; Step 3.3: Upload a non-default volatile startup file."
write ";*********************************************************************"
write " Please verify that the RAM:0/apps directory exists on $CPU by entering"
write " ls ""RAM:0"". If this directory does not exist, create it by entering "
write " mkdir ""RAM:0/apps""."
write " Type 'g' or 'go' when completed."
wait

;; ftp several test files to the volatile file system
s ftp_file (ramDir & "/apps","cfe_es_startup_vol.scr","cfe_es_startup.scr",hostCPU,"P")
wait 5

;; Upload the TST_ES2 application contained in the startup script
s load_app (ramDir,"TST_ES2", hostCPU)
wait 10

local tbl_1_pkt,tbl_2_pkt,tbl_1_appid,tbl_2_appid

;; Create the TST_ES2 Table load files
;; CPU1 is the default
tbl_1_pkt = "0FA4"
tbl_2_pkt = "0FA5"
tbl_1_appid = 4004
tbl_2_appid = 4005

if ("$CPU" = "CPU2") then
  tbl_1_pkt = "0FC2"
  tbl_2_pkt = "0FC3"
  tbl_1_appid = 4034
  tbl_2_appid = 4035
elseif ("$CPU" = "CPU3") then
  tbl_1_pkt = "0FE2"
  tbl_2_pkt = "0FE3"
  tbl_1_appid = 4066
  tbl_2_appid = 4067
endif

;; Set the table 1 values
$SC_$CPU_TST_ES_TABLE1_ELEMENT1 = 1
$SC_$CPU_TST_ES_TABLE1_ELEMENT2 = 2
$SC_$CPU_TST_ES_TABLE1_ELEMENT3 = 3
$SC_$CPU_TST_ES_TABLE1_PAD = 0

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 First Load", "FirstTblDef.dat","TST_ES2.MyFirstTable", "$SC_$CPU_TST_ES_TABLE1_ELEMENT1", "$SC_$CPU_TST_ES_TABLE1_PAD")

;; Set the table 2 values
$SC_$CPU_TST_ES_TABLE2_ELEMENT1 = 3
$SC_$CPU_TST_ES_TABLE2_ELEMENT2 = 2
$SC_$CPU_TST_ES_TABLE2_ELEMENT3 = 1
$SC_$CPU_TST_ES_TABLE2_PAD = 0

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Table #2 First Load", "SecondTblDef.dat","TST_ES2.MySecondTable", "$SC_$CPU_TST_ES_TABLE2_ELEMENT1", "$SC_$CPU_TST_ES_TABLE2_PAD")

;; Upload the TST_ES2 table load data files to /ram
s ftp_file (ramDir,"firsttbldef.dat","FirstTblDef.dat",hostCPU,"P")
wait 10
s ftp_file (ramDir,"secondtbldef.dat","SecondTblDef.dat",hostCPU,"P")
wait 10

write ";*********************************************************************"
write "; Step 3.4: Command a Processor Reset."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

ut_setrequirements ES_1010, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

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

;; Disable SCH Debug Events if SCH is running
SCHisRunning = FALSE
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
write "; Step 3.5: Dump the System and ER Logs after the Reset."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

; Verify that the system log indicates a "processor" reset
/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="/ram/es_syslog1.log"
wait 10

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1016) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1016, "P"
else
  write "<!> Failed (1016) - Expected Event message ",CFE_ES_SYSLOG2_EID, " was not received"
  ut_setrequirements ES_1016, "F"
endif

;; Set the preserve system log requirements to Analysis
ut_setrequirements ES_1511, "A"
ut_setrequirements ES_1515, "A"
ut_setrequirements ES_15182, "A"

;;;; Need to use ftp_file for System Log
s ftp_file (ramDir,"es_syslog1.log","$sc_$cpu_es_syslog1.log",hostCPU,"G")
wait 10

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt (ramDir, "es_erlog.log", "$sc_$cpu_es_erlog35.log", hostCPU,"ES_ERLOG")
wait 10

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1019, "F"
endif

;; Set the preserve ER log requirements to Analysis
ut_setrequirements ES_1512, "A"

;; Check the Exception & Reset Log for an entry
local erLogIndex = $SC_$CPU_ES_ERLOGINDEX
if ($SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetType = CFE_ES_PROCESSOR_RESET) then
  write "<*> Passed (1520) - ER Log contains an entry indicating the Processor Reset occurred."
  ut_setrequirements ES_1520, "P"
else
  write "<!> Failed (1520) - ER Log does not contain an entry for the Processor Reset."
  ut_setrequirements ES_1520, "F"
endif

write "ERLogIndex      = ", erLogIndex
write "ResetType       = ", p@$SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetType 
write "ResetSubType    = ", p@$SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetSubtype
write "HK ResetSubtype = ", p@$SC_$CPU_ES_ResetSubtype

write ";*********************************************************************"
write "; Step 3.6: Verify the Volatile File system has been preserved."
write ";*********************************************************************"
;; ftp the files uploaded in Step 3.1 to see if they still exist
s ftp_file (ramDir,"es_testfile_1","es_testfile_1_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/es_testfile_1_Verify"

if file_exists(filename) then
  write "<*> Passed (1513;1517) - Volatile file system contained test file 1."
  ut_setrequirements ES_1513, "P"
  ut_setrequirements ES_1517, "P"
else
  write "<!> Failed (1513;1517) - Unable to download test file 1 from Volatile File system."
  ut_setrequirements ES_1513, "F"
  ut_setrequirements ES_1517, "F"
endif

s ftp_file (ramDir,"es_testfile_2","es_testfile_2_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/es_testfile_2_Verify"

if file_exists(filename) then
  write "<*> Passed (1513;1517) - Volatile file system contained test file 2."
  ut_setrequirements ES_1513, "P"
  ut_setrequirements ES_1517, "P"
else
  write "<!> Failed (1513;1517) - Unable to download test file 2 from Volatile File system."
  ut_setrequirements ES_1513, "F"
  ut_setrequirements ES_1517, "F"
endif

; Set the HK CDS variables to 0 to verify they change when TST_ES starts
$SC_$CPU_TST_ES_CDSVAR1 = 0
$SC_$CPU_TST_ES_CDSVAR2 = 0
$SC_$CPU_TST_ES_CDSVAR3 = 0
$SC_$CPU_TST_ES_CDSVAR4 = 0
$SC_$CPU_TST_ES_CDSVAR5 = 0

;  Dump all running apps
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1012;1518.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1012, "P"
  ut_setrequirements ES_15181, "P"
else
  write "<!> Failed (1012;1518.1) - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
  ut_setrequirements ES_1012, "F"
  ut_setrequirements ES_15181, "F"
endif

found_app = FALSE
local found_app2 = FALSE
;;Loop thru the table of apps to verify that the TST_ES app is running
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
    found_app = TRUE
  elseif ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES2") then
    found_app2 = TRUE
  endif
enddo

if (found_app = TRUE) then
  write "TST_ES Application is running as expected"
  ; Subscribe to the HK packet
  /$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
  wait 10

  ;; Add an Event Filter for the TST_ES HK Request Event in order to
  ;; only receive this event 1 time rather than every 4 seconds
  /$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
  wait 5

  if (found_app2 = TRUE) then
    write "<*> Passed (1518) - Apps added to startup file are running."
    ut_setrequirements ES_1518, "P"
  else
    write "<!> Failed (1518) - All Apps in startup file did not start."
    ut_setrequirements ES_1518, "F"
  endif
endif

; Verify that the HK indicates the expected values for the CDS variables
cdsVar1 = $SC_$CPU_TST_ES_CDSVAR1
cdsVar2 = $SC_$CPU_TST_ES_CDSVAR2
cdsVar3 = $SC_$CPU_TST_ES_CDSVAR3
cdsVar4 = $SC_$CPU_TST_ES_CDSVAR4
cdsVar5 = $SC_$CPU_TST_ES_CDSVAR5

write "TST_ES CDS Variable #1 = ", cdsVar1
write "TST_ES CDS Variable #2 = ", cdsVar2
write "TST_ES CDS Variable #3 = ", cdsVar3
write "TST_ES CDS Variable #4 = ", cdsVar4
write "TST_ES CDS Variable #5 = ", cdsVar5

; Compare the variables above with the ones saved before the reset
if (cdsVar1 = newCDSVar1 AND cdsVar2 = newCDSVar2 AND ;;
    cdsVar3 = newCDSVar3 AND cdsVar4 = newCDSVar4 AND ;;
    cdsVar5 = newCDSVar5) then
  write "<*> Passed (1514) - TST_ES CDS Variables were saved on Processor Reset"
  ut_setrequirements ES_1514, "P"
else
  write "<!> Failed (1514) - TST_ES CDS Variables were lost on Processor Reset"
  ut_setrequirements ES_1514, "F"
endif
wait 5

write ";*********************************************************************"
write "; Step 3.7: Verify the ES Housekeeping Telemetry items."
write ";*********************************************************************"
write "; Registered CORE apps    = ", $SC_$CPU_ES_RegCoreApps
write "; Registered Ext apps     = ", $SC_$CPU_ES_RegExtApps
write "; Registered Tasks        = ", $SC_$CPU_ES_REGTASKS
write "; Registered Libraries    = ", $SC_$CPU_ES_REGLIBS
write "; Processor Resets        = ", $SC_$CPU_ES_ProcResetCnt
write "; Boot Source             = ", $SC_$CPU_ES_BootSource
write "; Valid Command Counter   = ", $SC_$CPU_ES_CMDPC
write "; Invalid Command Counter = ", $SC_$CPU_ES_CMDEC
write "; Reset Type              = ", $SC_$CPU_ES_ResetType
write "; Reset Subtype           = ", $SC_$CPU_ES_ResetSubtype
write "; System Log Entries      = ", $SC_$CPU_ES_SysLogEntries
write "; System Log Bytes Used   = ", $SC_$CPU_ES_SysLogByteUsed

if ($SC_$CPU_ES_RegExtApps = 5 AND $SC_$CPU_ES_RegCoreApps = 5 AND ;;
    $SC_$CPU_ES_ProcResetCnt = 1 AND $SC_$CPU_ES_BootSource = bootSource AND ;;
    $SC_$CPU_ES_CMDPC = 3 AND $SC_$CPU_ES_CMDEC = 0 AND ;;
    $SC_$CPU_ES_ResetType = CFE_ES_PROCESSOR_RESET AND ;;
    $SC_$CPU_ES_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed (1000;1510;1521) - ES HK items are correct."
  ut_setrequirements ES_1000, "P"
  ut_setrequirements ES_1510, "P"
  ut_setrequirements ES_1521, "P"
else
  write "<!> Failed (1000;1510;1521) - ES HK items are not as expected."
  ut_setrequirements ES_1000, "F"
  ut_setrequirements ES_1510, "F"
  ut_setrequirements ES_1521, "F"
endif

write ";*********************************************************************"
write "; Step 3.8: Verify the EVS log entries."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_WRLOG_EID, "DEBUG", 1
s get_file_to_cvt (ramDir, "cfe_evs.log", "$sc_$cpu_cfe_evs.log", hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
endif

write ";*********************************************************************"
write "; Step 3.9: Print the applications properties"
write ";*********************************************************************"
;Loop thru the table of apps
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName <> "") then
    write "Application #", app_index, ":"
    write "  Name      => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppName
    write "  ID        => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppId
    write "  Task Name => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskName
    write "  Task ID   => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskId
  endif
enddo

write ";*********************************************************************"
write "; Step 3.10: Send the Get Last Reset Type request."
write ";*********************************************************************"
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_GETLASTRESETTYPE
ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1301) - Get Last Reset Type command sent successfully."
  ut_setrequirements ES_1301, "P"
else
  write "<!> Failed (1301) - Get Last Reset Type command."
  ut_setrequirements ES_1301, "F"
endif

write ";*********************************************************************"
write "; Step 3.11: Verify the non-volatile file system. This step verifies "
write ";            that it is mounted. The file system is mounted if the   "
write ";            startup file can be ftp'd to the ground. "
write ";*********************************************************************"
;; ftp the file referenced by CFE_ES_NONVOL_STARTUP_FILE from the spacecraft
s ftp_file ("CF:0/apps","cfe_es_startup.scr","cfe_es_startup_NV_311.scr",hostCPU,"G")
wait 10

;; Verifiy that the file was retrieved from the spacecraft
filename = work_dir & "/image/cfe_es_startup_NV_311.scr"

if file_exists(filename) then
  write "<*> Passed (1516) - Non-volatile Startup file successfully downloaded"
  ut_setrequirements ES_1516, "P"
else
  write "<!> Failed (1516) - Non-volatile Starup file did not download properly."
  ut_setrequirements ES_1516, "F"
endif

write ";*********************************************************************"
write "; Step 4.0: Power-On Reset Test 2."
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 4.1: Upload a new Non-Volatile startup file and the object files"
write ";           for any added application contained in this file."
write ";*********************************************************************"
;; Delete the existing startup file
;;s ftp_file ("CF:0/apps","na","cfe_es_startup.scr",hostCPU,"R")
;;wait 10
s ftp_file ("CF:0/apps","cfe_es_startup_nv_1.scr","cfe_es_startup.scr",hostCPU,"P")
wait 10

;; Upload the TST_ES application contained in the startup script
s load_app ("CF:0/apps","TST_ES", hostCPU)
wait 10

write ";*********************************************************************"
write "; Step 4.2: Upload some files to the spacecraft in order to modify the"
write ";           contents of the volatile file system."
write ";*********************************************************************"
;; ftp several test files to the volatile file system
s ftp_file (ramDir,"es_testfile_3","es_testfile_3",hostCPU,"P")
wait 5
s ftp_file (ramDir,"es_testfile_4","es_testfile_4",hostCPU,"P")
wait 5

write ";*********************************************************************"
write "; Step 4.3: Modify the TST_ES CDS variables.                          "
write ";*********************************************************************"
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

;; Get the new values
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
write "; Step 4.4: Send the TST_ES command to request a Power-On Reset.      "
write ";*********************************************************************"
/$SC_$CPU_TST_ES_RESETCFE POWERON
wait 10

ut_setrequirements ES_1317, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

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

;; Disable SCH Debug Events if SCH is running
SCHisRunning = FALSE
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
write "; Step 4.5: Dump the System and ER Logs after the Reset."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

; Verify that the system log indicates a "Power-On" reset
/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="/ram/es_syslog45.log"
wait 5

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1016) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1016, "P"
else
  write "<!> Failed (1016) - Expected Event message ",CFE_ES_SYSLOG2_EID, " was not received"
  ut_setrequirements ES_1016, "F"
endif

;; Set the clear system log requirements to Analysis
ut_setrequirements ES_1501, "A"
ut_setrequirements ES_1505, "A"
ut_setrequirements ES_15082, "A"

;;;; Need to use ftp_file for System Log
s ftp_file (ramDir,"es_syslog45.log","$sc_$cpu_es_syslog45.log",hostCPU,"G")
wait 10

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt (ramDir, "cfe_erlog.log", "$sc_$cpu_es_erlog45.log", hostCPU)
wait 5

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1019, "F"
endif

;; Set the clear ER log requirements to Analysis
ut_setrequirements ES_1502, "A"
;;ut_setrequirements ES_1509, "A"

;; Check the Exception & Reset Log for an entry
erLogIndex = $SC_$CPU_ES_ERLOGINDEX
if ($SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetType = CFE_ES_POWERON_RESET AND ;;
    $SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed (1509) - ER Log contains an entry indicating the Power-On Reset occurred."
  ut_setrequirements ES_1509, "P"
else
  write "<!> Failed (1509) - ER Log does not contain an entry for the Power-on Reset."
  ut_setrequirements ES_1509, "F"
endif

write ";*********************************************************************"
write "; Step 4.6: Verify the Volatile File system has been cleared."
write ";*********************************************************************"
;; ftp the files uploaded in Step 4.1 to see if they have beed cleared
s ftp_file (ramDir,"es_testfile_3","es_testfile_3_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/es_testfile_3_Verify"

if file_exists(filename) then
  write "<!> Failed (1503;1507) - Volatile file system contained test file 3 when it was expected to be cleared."
  ut_setrequirements ES_1503, "F"
  ut_setrequirements ES_1507, "F"
else
  write "<*> Passed (1503;1507) - Unable to download test file 3 from Volatile File system as expected."
  ut_setrequirements ES_1503, "P"
  ut_setrequirements ES_1507, "P"
endif

s ftp_file (ramDir,"es_testfile_4","es_testfile_4_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/es_testfile_4_Verify"

if file_exists(filename) then
  write "<!> Failed (1503;1507) - Volatile file system contained test file 4 when it was expected to be cleared."
  ut_setrequirements ES_1503, "F"
  ut_setrequirements ES_1507, "F"
else
  write "<*> Passed (1503;1507) - Unable to download test file 4 from Volatile File system as expected."
  ut_setrequirements ES_1503, "P"
  ut_setrequirements ES_1507, "P"
endif

;  Dump all running apps
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1012;1508.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1012, "P"
  ut_setrequirements ES_15081, "P"
else
  write "<!> Failed (1012;1508.1) - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
  ut_setrequirements ES_1012, "F"
  ut_setrequirements ES_15081, "F"
endif

found_app = FALSE
;;Loop thru the table of apps again to verify that the TST_ES app is not running
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if (found_app = FALSE) then
    if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
      found_app = TRUE
    endif
  endif
enddo

if (found_app = TRUE) then
  write "TST_ES Application is running as expected"
  ; Subscribe to the HK packet
  /$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
  wait 10

  ;; Add an Event Filter for the TST_ES HK Request Event in order to
  ;; only receive this event 1 time rather than every 4 seconds
  /$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
  wait 5
else
  write "<!> Failed - TST_ES Application was not started"
  ;; Do I need to quit here or just start the TST_ES app???
endif

; Verify that the HK indicates the expected values for the CDS variables
cdsVar1 = $SC_$CPU_TST_ES_CDSVAR1
cdsVar2 = $SC_$CPU_TST_ES_CDSVAR2
cdsVar3 = $SC_$CPU_TST_ES_CDSVAR3
cdsVar4 = $SC_$CPU_TST_ES_CDSVAR4
cdsVar5 = $SC_$CPU_TST_ES_CDSVAR5

write "TST_ES CDS Variable #1 = ", cdsVar1
write "TST_ES CDS Variable #2 = ", cdsVar2
write "TST_ES CDS Variable #3 = ", cdsVar3
write "TST_ES CDS Variable #4 = ", cdsVar4
write "TST_ES CDS Variable #5 = ", cdsVar5

; Compare the variables above with the ones saved before the reset
if (cdsVar1 = 1 AND cdsVar2 = 2 AND cdsVar3 = 3 AND cdsVar4 = 4 AND ;;
    cdsVar5 = 5) then
  write "<*> Passed (1504) - TST_ES CDS Variables were reset to their default values on Power-On Reset"
  ut_setrequirements ES_1504, "P"
else
  write "<!> Failed (1504) - TST_ES CDS Variables were not set to the default values as expected"
  ut_setrequirements ES_1504, "F"
endif
wait 5

write ";*********************************************************************"
write "; Step 4.7: Verify the ES Housekeeping Telemetry items."
write ";*********************************************************************"
write "; Registered CORE apps    = ", $SC_$CPU_ES_RegCoreApps
write "; Registered Ext apps     = ", $SC_$CPU_ES_RegExtApps
write "; Registered Tasks        = ", $SC_$CPU_ES_REGTASKS
write "; Registered Libraries    = ", $SC_$CPU_ES_REGLIBS
write "; Processor Resets        = ", $SC_$CPU_ES_ProcResetCnt
write "; Boot Source             = ", $SC_$CPU_ES_BootSource
write "; Valid Command Counter   = ", $SC_$CPU_ES_CMDPC
write "; Invalid Command Counter = ", $SC_$CPU_ES_CMDEC
write "; Reset Type              = ", $SC_$CPU_ES_ResetType
write "; Reset Subtype           = ", $SC_$CPU_ES_ResetSubtype
write "; System Log Entries = ", $SC_$CPU_ES_SysLogEntries
write "; System Log Bytes Used = ", $SC_$CPU_ES_SysLogByteUsed

if ($SC_$CPU_ES_RegExtApps = 4 AND $SC_$CPU_ES_RegCoreApps = 5 AND ;;
    $SC_$CPU_ES_ProcResetCnt = 0 AND $SC_$CPU_ES_BootSource = 1 AND ;;
    $SC_$CPU_ES_CMDPC = 3 AND $SC_$CPU_ES_CMDEC = 0 AND ;;
    $SC_$CPU_ES_ResetType = CFE_ES_POWERON_RESET AND ;;
    $SC_$CPU_ES_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed (1000;1500) - ES HK items are correct."
  ut_setrequirements ES_1000, "P"
  ut_setrequirements ES_1500, "P"
else
  write "<!> Failed (1000;1500) - ES HK items are not as expected."
  ut_setrequirements ES_1000, "F"
  ut_setrequirements ES_1500, "F"
endif

write ";*********************************************************************"
write "; Step 4.8: Verify the EVS log entries."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_WRLOG_EID, "DEBUG", 1
s get_file_to_cvt (ramDir, "cfe_evs.log", "$sc_$cpu_cfe_evs48.log", hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
endif

write ";*********************************************************************"
write "; Step 4.9: Dump the applications properties"
write ";*********************************************************************"
;Loop thru the table of apps
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName <> "") then
    write "Application #", app_index, ":"
    write "  Name      => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppName
    write "  ID        => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppId
    write "  Task Name => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskName
    write "  Task ID   => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskId
  endif
enddo

write ";*********************************************************************"
write "; Step 4.10: Verify the non-volatile file system. This step verifies "
write ";            that it is mounted. The file system is mounted if the   "
write ";            startup file can be ftp'd to the ground. "
write ";*********************************************************************"
;; ftp the file referenced by CFE_ES_NONVOL_STARTUP_FILE from the spacecraft
s ftp_file ("CF:0/apps","cfe_es_startup.scr","cfe_es_startup_nv_410.scr",hostCPU,"G")
wait 10

;; Verify that the file was retrieved from the spacecraft
filename = work_dir & "/image/cfe_es_startup_nv_410.scr"

if file_exists(filename) then
  write "<*> Passed (1506) - Non-volatile Startup file successfully downloaded"
  ut_setrequirements ES_1506, "P"
else
  write "<!> Failed (1506) - Non-volatile Starup file did not download properly."
  ut_setrequirements ES_1506, "F"
endif

write ";*********************************************************************"
write "; Step 4.11: Sent the Get Last Reset Type request."
write ";*********************************************************************"
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_GETLASTRESETTYPE
ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1301) - Get Last Reset Type command sent successfully."
  ut_setrequirements ES_1301, "P"
else
  write "<!> Failed (1301) - Get Last Reset Type command."
  ut_setrequirements ES_1301, "F"
endif

write ";*********************************************************************"
write "; Step 5.0: Processor Reset Test #2 "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 5.1: Upload some files to the spacecraft in order to modify the"
write ";           contents of the volatile file system."
write ";*********************************************************************"
;; ftp several test files to the volatile file system
s ftp_file (ramDir,"es_testfile_5","es_testfile_5",hostCPU,"P")
wait 5
s ftp_file (ramDir,"es_testfile_6","es_testfile_6",hostCPU,"P")
wait 5

write ";*********************************************************************"
write "; Step 5.2: Modify the TST_ES CDS variables.                          "
write ";*********************************************************************"
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

;; Get the new values
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

;;;; Save the current values of other preserved variables
bootSource = $SC_$CPU_ES_BootSource
sysLogEntries = $SC_$CPU_ES_SYSLOGENTRIES
sysLogUsed = $SC_$CPU_ES_SYSLOGByteUsed
write "; BEFORE Reset - Boot Source = ", bootSource
write "; BEFORE Reset - System Log Entries = ", sysLogEntries
write "; BEFORE Reset - System Log Bytes Used = ", sysLogUsed
wait 5

write ";*********************************************************************"
write "; Step 5.3: Verify that the Volatile Startup file does not exist. This"
write ";           will force the cFE to use the Non-Volatile Startup file. "
write ";*********************************************************************"
;; Try to get the volatile startup file
s ftp_file (ramDir & "/apps","cfe_es_startup.scr","cfe_es_startup_vol_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/cfe_es_startup_vol_Verify"

if file_exists(filename) then
  write "<!> Failed - Volatile file system startup file exists!!!"
  write " Volatile file system startup file will be deleted."
  ;; Delete this file on $CPU
else
  write "<*> Passed - Volatile file system startup file does not exist"
endif

write ";*********************************************************************"
write "; Step 5.4: Send the TST_ES command to request a Processor Reset.      "
write ";*********************************************************************"
/$SC_$CPU_TST_ES_RESETCFE PROCESSOR
wait 10

ut_setrequirements ES_1318, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

;; Turn on DEBUG Events
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

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

;; Disable SCH Debug Events if SCH is running
SCHisRunning = FALSE
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
write "; Step 5.5: Dump the System and ER Logs after the Reset."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_SYSLOG2_EID, "DEBUG", 1

; Verify that the system log indicates a "processor" reset
/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME="/ram/es_syslog1.log"
wait 5

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1016) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1016, "P"
else
  write "<!> Failed (1016) - Expected Event message ",CFE_ES_SYSLOG2_EID, " was not received"
  ut_setrequirements ES_1016, "F"
endif

;; Set the preserve system log requirements to Analysis
ut_setrequirements ES_1511, "A"
ut_setrequirements ES_1515, "A"
ut_setrequirements ES_15192, "A"

;;;; Need to use ftp_file for System Log
s ftp_file (ramDir,"es_syslog1.log","$sc_$cpu_es_syslog1.log",hostCPU,"G")
wait 10

; Setup for the ERLog DEBUG event
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ERLOG2_EID, "DEBUG", 1

;;;; Can use get_file_to_cvt to get ER Log to the ground
s get_file_to_cvt (ramDir, "es_erlog.log", "$sc_$cpu_es_erlog55.log", hostCPU,"ES_ERLOG")
wait 5

; Check if the proper event was generated
if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1019) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1019, "P"
else
  write "<!> Failed (1019) - Expected Event message ",CFE_ES_ERLOG2_EID, " was not received"
  ut_setrequirements ES_1019, "F"
endif

;; Set the preserve ER log requirements to Analysis
ut_setrequirements ES_1512, "A"

;; Check the Exception & Reset Log for an entry
erLogIndex = $SC_$CPU_ES_ERLOGINDEX
if ($SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetType = CFE_ES_PROCESSOR_RESET) then
  write "<*> Passed (1520) - ER Log contains an entry indicating the Processor Reset occurred."
  ut_setrequirements ES_1520, "P"
else
  write "<!> Failed (1520) - ER Log does not contain an entry for the Processor Reset."
  ut_setrequirements ES_1520, "F"
endif

write "ERLogIndex      = ", erLogIndex
write "ResetType       = ", p@$SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetType 
write "ResetSubType    = ", p@$SC_$CPU_ES_ERLE[erLogIndex].ERLog_ResetSubtype
write "HK ResetSubtype = ", p@$SC_$CPU_ES_ResetSubtype

write ";*********************************************************************"
write "; Step 5.6: Verify the Volatile File system has been preserved."
write ";*********************************************************************"
;; ftp the files uploaded in Step 3.1 to see if they still exist
s ftp_file (ramDir,"es_testfile_5","es_testfile_5_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/es_testfile_5_Verify"

if file_exists(filename) then
  write "<*> Passed (1513;1517) - Volatile file system contained test file 5."
  ut_setrequirements ES_1513, "P"
  ut_setrequirements ES_1517, "P"
else
  write "<!> Failed (1513;1517) - Unable to download test file 5 from Volatile File system."
  ut_setrequirements ES_1513, "F"
  ut_setrequirements ES_1517, "F"
endif

s ftp_file (ramDir,"es_testfile_6","es_testfile_6_Verify",hostCPU,"G")
wait 5
filename = work_dir & "/image/es_testfile_6_Verify"

if file_exists(filename) then
  write "<*> Passed (1513;1517) - Volatile file system contained test file 6."
  ut_setrequirements ES_1513, "P"
  ut_setrequirements ES_1517, "P"
else
  write "<!> Failed (1513;1517) - Unable to download test file 6 from Volatile File system."
  ut_setrequirements ES_1513, "F"
  ut_setrequirements ES_1517, "F"
endif

; Set the HK CDS variables to 0 to verify they change when TST_ES starts
$SC_$CPU_TST_ES_CDSVAR1 = 0
$SC_$CPU_TST_ES_CDSVAR2 = 0
$SC_$CPU_TST_ES_CDSVAR3 = 0
$SC_$CPU_TST_ES_CDSVAR4 = 0
$SC_$CPU_TST_ES_CDSVAR5 = 0

;  Dump all running apps
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG", 1
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed (1012;1519.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements ES_1012, "P"
  ut_setrequirements ES_15191, "P"
else
  write "<!> Failed (1012;1519.1) - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
  ut_setrequirements ES_1012, "F"
  ut_setrequirements ES_15191, "F"
endif

found_app = FALSE
;;Loop thru the table of apps again to verify that the TST_ES app is not running
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if (found_app = FALSE) then
    if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
      found_app = TRUE
    endif
  endif
enddo

if (found_app = TRUE) then
  write "TST_ES Application is running as expected"
  ; Subscribe to the HK packet
  /$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
  wait 10

  ;; Add an Event Filter for the TST_ES HK Request Event in order to
  ;; only receive this event 1 time rather than every 4 seconds
  /$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
  wait 5

  write "<*> Passed (1519) - TST_ES App added to startup file is running."
  ut_setrequirements ES_1519, "P"
else
  write "<!> Failed (1519) - TST_ES App added to startup file did not start."
  ut_setrequirements ES_1519, "F"
endif

; Verify that the HK indicates the expected values for the CDS variables
cdsVar1 = $SC_$CPU_TST_ES_CDSVAR1
cdsVar2 = $SC_$CPU_TST_ES_CDSVAR2
cdsVar3 = $SC_$CPU_TST_ES_CDSVAR3
cdsVar4 = $SC_$CPU_TST_ES_CDSVAR4
cdsVar5 = $SC_$CPU_TST_ES_CDSVAR5

write "TST_ES CDS Variable #1 = ", cdsVar1
write "TST_ES CDS Variable #2 = ", cdsVar2
write "TST_ES CDS Variable #3 = ", cdsVar3
write "TST_ES CDS Variable #4 = ", cdsVar4
write "TST_ES CDS Variable #5 = ", cdsVar5

; Compare the variables above with the ones saved before the reset
if (cdsVar1 = newCDSVar1 AND cdsVar2 = newCDSVar2 AND ;;
    cdsVar3 = newCDSVar3 AND cdsVar4 = newCDSVar4 AND ;;
    cdsVar5 = newCDSVar5) then
  write "<*> Passed (1514) - TST_ES CDS Variables were saved on Processor Reset"
  ut_setrequirements ES_1514, "P"
else
  write "<!> Failed (1514) - TST_ES CDS Variables were lost on Processor Reset"
  ut_setrequirements ES_1514, "F"
endif
wait 5

write ";*********************************************************************"
write "; Step 5.7: Verify the ES Housekeeping Telemetry items."
write ";*********************************************************************"
write "; Registered CORE apps    = ", $SC_$CPU_ES_RegCoreApps
write "; Registered Ext apps     = ", $SC_$CPU_ES_RegExtApps
write "; Registered Tasks        = ", $SC_$CPU_ES_REGTASKS
write "; Registered Libraries    = ", $SC_$CPU_ES_REGLIBS
write "; Processor Resets        = ", $SC_$CPU_ES_ProcResetCnt
write "; Boot Source             = ", $SC_$CPU_ES_BootSource
write "; Valid Command Counter   = ", $SC_$CPU_ES_CMDPC
write "; Invalid Command Counter = ", $SC_$CPU_ES_CMDEC
write "; Reset Type              = ", $SC_$CPU_ES_ResetType
write "; Reset Subtype           = ", $SC_$CPU_ES_ResetSubtype
write "; System Log Entries = ", $SC_$CPU_ES_SysLogEntries
write "; System Log Bytes Used = ", $SC_$CPU_ES_SysLogByteUsed

if ($SC_$CPU_ES_RegExtApps = 4 AND $SC_$CPU_ES_RegCoreApps = 5 AND ;;
    $SC_$CPU_ES_ProcResetCnt = 1 AND $SC_$CPU_ES_BootSource = bootSource AND ;;
    $SC_$CPU_ES_CMDPC = 3 AND $SC_$CPU_ES_CMDEC = 0 AND ;;
    $SC_$CPU_ES_ResetType = CFE_ES_PROCESSOR_RESET AND ;;
    $SC_$CPU_ES_ResetSubtype = CFE_ES_RESET_COMMAND) then
  write "<*> Passed (1000;1510;1521) - ES HK items are correct."
  ut_setrequirements ES_1000, "P"
  ut_setrequirements ES_1510, "P"
  ut_setrequirements ES_1521, "P"
else
  write "<!> Failed (1000;1510;1521) - ES HK items are not as expected."
  ut_setrequirements ES_1000, "F"
  ut_setrequirements ES_1510, "F"
  ut_setrequirements ES_1521, "F"
endif

write ";*********************************************************************"
write "; Step 5.8: Verify the EVS log entries."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_WRLOG_EID, "DEBUG", 1
s get_file_to_cvt (ramDir, "cfe_evs.log", "$sc_$cpu_cfe_evs.log", hostCPU)
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_EVS_WRLOG_EID, " was not received"
endif

write ";*********************************************************************"
write "; Step 5.9: Print the applications properties"
write ";*********************************************************************"
;Loop thru the table of apps
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName <> "") then
    write "Application #", app_index, ":"
    write "  Name      => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppName
    write "  ID        => ", $SC_$CPU_ES_ALE[app_index].ES_AL_AppId
    write "  Task Name => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskName
    write "  Task ID   => ", $SC_$CPU_ES_ALE[app_index].ES_AL_TaskId
  endif
enddo

write ";*********************************************************************"
write "; Step 5.10: Sent the Get Last Reset Type request."
write ";*********************************************************************"
cmdcnt = $SC_$CPU_TST_ES_CMDPC + 1

/$SC_$CPU_TST_ES_GETLASTRESETTYPE
ut_tlmwait $SC_$CPU_TST_ES_CMDPC, {cmdcnt}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1301) - Get Last Reset Type command sent successfully."
  ut_setrequirements ES_1301, "P"
else
  write "<!> Failed (1301) - Get Last Reset Type command."
  ut_setrequirements ES_1301, "F"
endif

write ";*********************************************************************"
write "; Step 6.0: Test Clean-up."
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 6.1: Upload the original Non-Volatile startup file."
write ";*********************************************************************"
;; Restore the original Non-volatile startup file
s ftp_file ("CF:0/apps","cfe_es_startup_nv.scr","cfe_es_startup.scr",hostCPU,"P")
wait 10

;; Delete any files uploaded to the CF:0/apps directory
s ftp_file ("CF:0/apps","na","tst_es.o",hostCPU,"R")
wait 10

write ";*********************************************************************"
write "; Step 6.2: Command a Power-On Reset."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

ut_setrequirements ES_1009, "P"

close_data_center
wait 75

cfe_startup $CPU
wait 5

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
