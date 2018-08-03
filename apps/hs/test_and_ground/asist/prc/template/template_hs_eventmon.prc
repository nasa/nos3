PROC $sc_$cpu_hs_eventmon
;*******************************************************************************
;  Test Name:  hs_eventmon
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) Event Monitoring 
;	commands function properly. Invalid commands as well as anomalies will
;	be tested to ensure that the HS application handles these appropriately.
;
;  Requirements Tested
;    HS1000     Upon receipt of a No-Op command, HS shall increment the HS
;               Valid Command Counter and generate an event message.
;    HS1002	For all HS commands, if the length contained in the message
;		header is not equal to the expected length, HS shall reject the
;		command and issue an event message.
;    HS1003	If HS accepts any command as valid, HS shall execute the
;		command, increment the HS Valid Command Counter and issue an
;		event message.
;    HS1004	If HS rejects any command, HS shall abort the command execution,
;		increment the HS Command Rejected Counter and issue an event
;		message.
;    HS5000	The HS application shall compare each received event message
;		with the events defined in the Critical Event Table for up to
;		<PLATFORM_DEFINED> critical events.
;    HS5000.1	If the event received is defined in the Critical Event Table,
;		HS shall execute one of the table-defined actions:
;		  a. Restart Application that generated the Event
;		  b. cFE Processor Reset
;		  c. Delete the Application that generated the Event
;		  d. Send a Software Bus message
;		  e. Perform No Action
;    HS5000.1.1	If the action is to perform a cFE Processor Reset and the number
;		of cFE Processor Resets is less than the <PLATFORM_DEFINED> Max
;		Number of cFE Processor Resets, HS shall
;		  a. Increment the Number of cFE Processor Resets
;		  b. Set the Watchdog servicing flag to False
;		  c. Command the cFE Processor Reset
;    HS5000.1.2	If the action is to perform a cFE Processor Reset and the number
;		of cFE Processor Resets is greater-than-or-equal-to the
;		<PLATFORM_DEFINED> Max Number of cFE Processor Resets, HS shall
;		  a. Send an event message
;    HS5000.2	If the Application defined in the Critical Event Table is
;		unknown, HS shall increment the Critical Event Table 
;		Invalid/Unknown Apps counter.
;    HS5001	Upon receipt of an Enable Critical Event Monitoring Command,
;		HS shall
;		  a. Set the Enable Critical Event Monitoriing to Enabled
;		  b. Begin processing the Critical Event Table
;    HS5002	Upon receipt of a Disable Critical Event Monitoring Command,
;		HS shall
;		  a. Set the Enable Critical Event Monitoring to Disabled
;		  b. Stop processing the Critical Event Table
;    HS5003	HS shall support up to <PLATFORM_DEFINED> critical events.
;    HS5004	Upon receipt of a Critical Event Table update indication, HS 
;		shall validate the Critical Event Table by validating the action
;    HS5004.1	If the Critical Event Table fails validation, HS shall issue an
;		event message and disable Critical Event Monitoring.
;    HS7001	Upon receipt of a Set Max Processor Reset Command, HS shall set
;		the Maximum number of cFE Processor Resets commanded by HS to
;		the command-specified value.
;    HS7100	HS shall generate a housekeeping message containing the
;		following:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. Critical Application Monitoring Status (enable/disable)
;		  d. Critical Application Monitoring Status per table entry
;		     (enable/disable)
;		  e. Number of cFE Processor Resets (commanded by HS)
;		  f. maximum number of cFE Processor resets
;		  g. Critical Event Monitoring status (enable/disable)
;		  h. Count of Monitored Event Messages
;		  i. CPU Aliveness indicator (enable/disable)
;		  j. Execution Counter, for each table entry
;		  k. Number of Invalid/Unknown Apps contained in the Critical
;		     Event Table
;                 l. Peak CPU Utilization
;                 m. Average CPU utilization
;                 n. CPU Utilization Monitoring Enabled/Disabled
;    HS8000	Upon cFE Power On Reset, HS shall initialize the following data
;		to Zero:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. Monitor Critical Application - <PLATFORM_DEFINED>
;		  d. Critical Application Monitoring status per Application 
;		     enabled
;		  e. Monitor Critical Events - <PLATFORM_DEFINED>
;		  f. CPU Aliveness Indicator - <PLATFORM_DEFINED>
;		  g. Watchdog Time Flag - TRUE
;		  h. Set the Watchdog Timer to <PLATFORM_DEFINED> value
;		  i. Maximum number of cFE Processor Resets - 
;		     <PLATFORM_DEFINED> value
;		  j. Number of cFE Processor resets (commanded by HS)
;		  k. Number of invalid/Unknown Apps contained in the Critical
;		     Event table
;                 l. Peak CPU Utilization
;                 m. Average CPU utilization
;                 n. CPU Utilization Monitoring Enabled/Disabled - 
;		     <PLATFORM_DEFINED> value
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The HS commands and telemetry items exist in the GSE database.
;	The display pages exist for the HS Housekeeping.
;	A HS Test application (TST_HS) exists in order to fully test the HS
;		Application.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	06/23/09	Walt Moleski	Original Procedure.
;       01/12/11        Walt Moleski    Updated for HS 2.1.0.0
;       09/19/16        Walt Moleski    Updated for HS 2.3.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs that connect to
;                                       the host IP.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait	Wait for a specified telemetry point to update to a
;			specified value. 
;       ut_pfindicate	Print the pass fail status of a particular requirement
;			number.
;       ut_setupevents	Performs setup to verify that a particular event
;			message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			requirements array.
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_es_events.h"
#include "cfe_evs_events.h"
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "hs_platform_cfg.h"
#include "hs_tbldefs.h"
#include "hs_msgdefs.h"
#include "hs_events.h"
#include "tst_hs_events.h"

%liv (log_procedure) = logging

#define HS_1000		0
#define HS_1002		1
#define HS_1003		2
#define HS_1004		3
#define HS_5000		4
#define HS_50001a	5
#define HS_50001b	6
#define HS_50001c	7
#define HS_50001d	8
#define HS_500011	9
#define HS_500012	10
#define HS_50002	11
#define HS_5001		12
#define HS_5002		13
#define HS_5003		14
#define HS_5004		15
#define HS_50041	16
#define HS_7001		17
#define HS_7100		18
#define HS_8000		19

global ut_req_array_size = 19
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_1000","HS_1002","HS_1003","HS_1004","HS_5000","HS_5000.1a","HS_5000.1b","HS_5000.1c","HS_5000.1d","HS_5000.1.1","HS_5000.1.2","HS_5000.2","HS_5001","HS_5002","HS_5003","HS_5004","HS_5004.1","HS_7001","HS_7100","HS_8000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream, index
local HSAppName = HS_APP_NAME
local ramDir = "RAM:0"  
local defTblDir = "CF:0/apps"
local hostCPU = "$CPU"

;; Table Names
local AppMonTblName = HSAppName & "." & HS_AMT_TABLENAME
local EvtMonTblName = HSAppName & "." & HS_EMT_TABLENAME
local ExeCntTblName = HSAppName & "." & HS_XCT_TABLENAME
local MsgActTblName = HSAppName & "." & HS_MAT_TABLENAME

write ";***********************************************************************"
write ";  Step 1.0: Health and Safety Test Setup."
write ";***********************************************************************"
write ";  Step 1.1: Command a Power-on Reset on $CPU."
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_HS_HK
page $SC_$CPU_HS_EVTMON_TABLE
page $SC_$CPU_TST_HS_HK

write ";***********************************************************************"
write ";  Step 1.3: Create & load the default definition tables. "
write ";***********************************************************************"
;; Application Monitoring Table
s $sc_$cpu_hs_amt1

;; Parse the filename configuration parameters for the default table filenames
local amtFileName = HS_AMT_FILENAME 
local slashLoc = %locate(amtFileName,"/")

;; loop until all slashes are found for the Application Monitoring  Table Name
while (slashLoc <> 0) do
  amtFileName = %substring(amtFileName,slashLoc+1,%length(amtFileName))
  slashLoc = %locate(amtFileName,"/")
enddo

write "==> Default Application Monitoring Table filename = '",amtFileName,"'"

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_amt1",amtFileName,hostCPU,"P")
wait 10

;; Event Monitoring Table
s $sc_$cpu_hs_emt1

;; Parse the filename configuration parameters for the default table filenames
local emtFileName = HS_EMT_FILENAME
slashLoc = %locate(emtFileName,"/")

;; loop until all slashes are found for the Event Monitoring Table Name
while (slashLoc <> 0) do
  emtFileName = %substring(emtFileName,slashLoc+1,%length(emtFileName))
  slashLoc = %locate(emtFileName,"/")
enddo

write "==> Default Event Monitoring Table filename = '",emtFileName,"'"

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_emt1",emtFileName,hostCPU,"P")
wait 10

;; Message Actions Table
s $sc_$cpu_hs_mat1

;; Parse the filename configuration parameter for the default table filenames
local matFileName = HS_MAT_FILENAME
slashLoc = %locate(matFileName,"/")

;; loop until all slashes are found for the Message Actions Table Name
while (slashLoc <> 0) do
  matFileName = %substring(matFileName,slashLoc+1,%length(matFileName))
  slashLoc = %locate(matFileName,"/")
enddo

write "==> Default Message Actions Table filename = '",matFileName,"'"

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_mat1",matFileName,hostCPU,"P")
wait 10

;; Execution Counter Table
s $sc_$cpu_hs_xct1

;; Parse the filename configuration parameter for the default table filenames
local xctFileName = HS_XCT_FILENAME
slashLoc = %locate(xctFileName,"/")

;; loop until all slashes are found for the Execution Counter Table Name
while (slashLoc <> 0) do
  xctFileName = %substring(xctFileName,slashLoc+1,%length(xctFileName))
  slashLoc = %locate(xctFileName,"/")
enddo

write "==> Default Execution Counter Table filename = '",xctFileName,"'"

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_xct1",xctFileName,hostCPU,"P")
wait 10

write ";***********************************************************************"
write ";  Step 1.4:  Start the Health and Safety (HS) and Test Applications.   "
write ";***********************************************************************"
s $sc_$cpu_hs_start_apps("1.4")
wait 5

write ";***********************************************************************"
write ";  Step 1.5: Enable DEBUG Event Messages "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the HS application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=HSAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";***********************************************************************"
write ";  Step 1.6: Verify that the HS Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Set the HS HK packet ID and Critical Event Table appID based upon 
;; the cpu being used
;; CPU1 is the default
local hkPktId = "p0AD"
local emtAPID = "0F73"

if ("$CPU" = "CPU2") then
  hkPktId = "p1AD"
  emtAPID = "0F81"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2AD"
  emtAPID = "0F92"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7100) - Housekeeping packet is being generated."
  ut_setrequirements HS_7100, "P"
else
  write "<!> Failed (7100) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements HS_7100, "F"
endif

;; Check the HK tlm items to see if they are initialized properly
if ($SC_$CPU_HS_CMDPC = 0) AND ($SC_$CPU_HS_CMDEC = 0) AND ;;
   ($SC_$CPU_HS_PRResetCtr = 0) AND ;;
   ($SC_$CPU_HS_MaxResetCnt = HS_MAX_RESTART_ACTIONS) AND ;;
   ($SC_$CPU_HS_InvalidEVTAppCnt = 0) AND ;;
   ($SC_$CPU_HS_AppMonState = HS_APPMON_DEFAULT_STATE) AND ;;
   ($SC_$CPU_HS_EvtMonState = HS_EVENTMON_DEFAULT_STATE) AND ;;
   (p@$SC_$CPU_TST_HS_WatchdogFlag = "TRUE") AND ;;
   ($SC_$CPU_HS_CPUAliveState = HS_ALIVENESS_DEFAULT_STATE) then
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly."
  ut_setrequirements HS_8000, "P"
  ;;***************************************************************************
  ;; Need to determine how to check the Watchdog Timer
  ;;***************************************************************************
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT initialized properly at startup."
  write "CMDPC           = ", $SC_$CPU_HS_CMDPC
  write "CMDEC           = ", $SC_$CPU_HS_CMDEC
  write "PRResetCtr      = ", $SC_$CPU_HS_PRResetCtr
  write "MaxResetCount   = ", $SC_$CPU_HS_MaxResetCnt
  write "InvalidEVTApps  = ", $SC_$CPU_HS_InvalidEVTAppCnt
  write "AppMonState     = ", p@$SC_$CPU_HS_AppMonState
  write "EventMonState   = ", p@$SC_$CPU_HS_EvtMonState
  write "CPUAliveState   = ", p@$SC_$CPU_HS_CPUAliveState
  ut_setrequirements HS_8000, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Send the Disable Event Monitoring command."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{HSAppName},HS_DISABLE_EVENTMON_DBG_EID,"DEBUG",1

local cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_DisableEvtMon

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - HS Disable Event Monitoring command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_5002, "P"
else
  write "<!> Failed (1003;5002) - HS Disable Event Monitoring command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_5002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Expected Event Msg ",HS_DISABLE_EVENTMON_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_5002, "P"
else
  write "<!> Failed (1003;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_DISABLE_EVENTMON_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_5002, "F"
endif

wait 5

;; Check that the Critical Event Monitoting Parameter is set to Disabled
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (2002) - Telemetry indicates Event Monitoring State is Enabled."
  ut_setrequirements HS_2002, "P"
else
  write "<!> Failed (2002) - Telemetry indicates incorrect Event Monitoring State of ", p@$SC_$CPU_HS_EVTMonState,". Expected Disabled."
  ut_setrequirements HS_2002, "F"
endif

write ";***********************************************************************"
write ";  Step 2.2: Send the Disable Event Monitoring command with an invalid "
write ";  length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR",1

local errcnt = $SC_$CPU_HS_CMDEC + 1

;; Set the CPU1 raw command as the default
rawcmd = "18AEc00000020589"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc00000020589"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc00000020589"
endif

ut_sendrawcmd "$SC_$CPU_HS", (rawcmd)

ut_tlmwait $SC_$CPU_HS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements HS_1002, "P"
  ut_setrequirements HS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements HS_1002, "F"
  ut_setrequirements HS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HS_LEN_ERR_EID, "."
  ut_setrequirements HS_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send the Enable Event Monitoring command with an invalid "
write ";  length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
 
;; Set the CPU1 raw command as the default
rawcmd = "18AEc00000020488"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc00000020488"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc00000020488"
endif

ut_sendrawcmd "$SC_$CPU_HS", (rawcmd)

ut_tlmwait $SC_$CPU_HS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements HS_1002, "P"
  ut_setrequirements HS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements HS_1002, "F"
  ut_setrequirements HS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HS_LEN_ERR_EID, "."
  ut_setrequirements HS_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Send the Enable Event Monitoring command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU",{HSAppName},HS_ENABLE_EVENTMON_DBG_EID,"DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_EnableEvtMon

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - HS Enable Event Monitoring command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_5001, "P"
else
  write "<!> Failed (1003;5001) - HS Enable Event Monitoring command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_5001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - Expected Event Msg ",HS_ENABLE_EVENTMON_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_5001, "P"
else
  write "<!> Failed (1003;5001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_ENABLE_EVENTMON_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_5001, "F"
endif

wait 5

;; Check that the Critical Event Monitoting Parameter is set to Disabled
if (p@$SC_$CPU_HS_EVTMonState = "Enabled") then
  write "<*> Passed (5001) - Telemetry indicates Event Monitoring State is Enabled."
  ut_setrequirements HS_5001, "P"
else
  write "<!> Failed (5001) - Telemetry indicates incorrect Event Monitoring State of ", p@$SC_$CPU_HS_EVTMonState,". Expected Enabled."
  ut_setrequirements HS_5001, "F"
endif

write ";*********************************************************************"
write ";  Step 3.0: Event Action Tests "
write ";*********************************************************************"
write ";  Step 3.1: Create and load a new Critical Event Table that contains "
write ";  an entry that will restart the TST_HS application when a TST_HS NOOP"
write ";  Event occurs. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt2",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.2: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

;; Wait for the event monitor table update message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event Table"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.4: Send the TST_HS Noop Command to trigger the app restart "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","TST_HS", TST_HS_NOOP_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{HSAppName}, HS_EVENTMON_RESTART_ERR_EID,"ERROR", 2
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_INIT_INF_EID, "INFO", 3

;; Send the command
/$SC_$CPU_TST_HS_NOOP

;; Wait for the NOOP Event
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS NOOP event rcv'd"
else
  write "<*> Failed - Did not rcv the TST_HS NOOP event"
endif

;; Wait for the HS Error Event
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000;5000.1) - HS Event Monitor restart application error event rcv'd"
  ut_setrequirements HS_5000, "P"
  ut_setrequirements HS_50001a, "P"
else
  write "<*> Failed - Did not rcv the HS Event Monitor restart application error event"
  ut_setrequirements HS_5000, "F"
  ut_setrequirements HS_50001a, "F"
endif

;; Wait for the TST_HS Init Event
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS Application initialized"
else
  write "<*> Failed - Did not rcv the initialize event for TST_HS"
endif

write ";*********************************************************************"
write ";  Step 3.5: Create and load a new Critical Event Table that contains "
write ";  an entry that will trigger a cFE Processor Reset when an HS_NOOP "
write ";  event is received."
write ";*********************************************************************"
s $sc_$cpu_hs_emt3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt3",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.6: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.7: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event  table"
  ut_setrequirements HS_5004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.8: Send the HS NOOP Command and wait for HS to trigger the "
write ";  cFE Processor Reset.  "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_NOOP_INF_EID, "INFO", 1
;; NOTE: The following event may not be rcv'd by the proc since the action is
;; to issue a Processor Reset. The uart should contain the event
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EVENTMON_PROC_ERR_EID, "ERROR", 2

;; Before sending the command, save the Reset Counter
local expectedResetCtr = $SC_$CPU_HS_PRResetCtr + 1

;; Send the command
/$SC_$CPU_HS_NOOP

;; Wait for the HS NOOP Event Message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1

;; Set the requirement to 'A' for the Processor Reset
ut_setrequirements HS_50001b, "A"

;; Wait for the reset to occur
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 3.9: Start the Health and Safety (HS) and Test Applications  "
write ";*********************************************************************"
s $sc_$cpu_hs_start_apps("3.9")
wait 5

write ";***********************************************************************"
write ";  Step 3.10: Enable DEBUG Event Messages "
write ";***********************************************************************"
cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the HS application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=HSAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif


write ";***********************************************************************"
write ";  Step 3.11: Verify the number of cFE Processor Resets counter "
write ";  incremented by 1."
write ";*********************************************************************"
;; Check the Reset Counter
if ($SC_$CPU_HS_PRResetCtr = expectedResetCtr) then
  write "<*> Passed (5000.1.1) - cFE Processor Reset Counter incremented as expected."
  ut_setrequirements HS_500011, "P"
else
  write "<*> Failed (5000.1.1) - The cFE Processor Reset Counter indicated ",$SC_$CPU_HS_PRResetCtr,". Expected ",expectedResetCtr
  ut_setrequirements HS_500011, "F"
endif

write ";***********************************************************************"
write ";  Step 3.12: Create and load a new Critical Event Table that contains"
write ";  an entry that will delete the TST_HS application when a TST_HS_NOOP"
write ";  Event occurs. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt4",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.13: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.14: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event Table"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.15: Send the TST_HS_NOOP Command and verify that HS deletes "
write ";  the TST_HS application. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_NOOP_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EVENTMON_DELETE_ERR_EID, "ERROR", 2

;; Send the command
/$SC_$CPU_TST_HS_NOOP

;; Wait for the NOOP Event
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS NOOP event rcv'd"
else
  write "<*> Failed - Did not rcv the TST_HS NOOP event"
endif

;; Wait for the HS Error Event
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000;5000.1) - HS Event Monitor restart application error event rcv'd"
  ut_setrequirements HS_5000, "P"
  ut_setrequirements HS_50001c, "P"
else
  write "<*> Failed - Did not rcv the HS Event Monitor restart application error event"
  ut_setrequirements HS_5000, "F"
  ut_setrequirements HS_50001c, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.16: Start the TST_HS application again. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_HS",hostCPU,"TST_HS_AppMain")

; Wait for app startup events 
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_HS Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_HS not received."
    write "Event Message count = ",$SC_$CPU_num_found_messages
  endif
else
  write "<!> Failed - TST_HS Application start Event Message not received."
endif

;; Set CPU1 as the default
stream = x'093C'

if ("$CPU" = "CPU2") then
  stream = x'0A3C'
elseif ("$CPU" = "CPU3") then
  stream = x'0B3C'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 10

write ";***********************************************************************"
write ";  Step 3.17: Create and load a new Critical Event Table that contains"
write ";  an entry that will send a Software Bus message when an HS_NOOP Event"
write ";  occurs. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt5

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt5",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.18: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.19: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event Table"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.20: Send the HS_NOOP Command and verify that HS generates "
write ";  a Software Bus message that issues the TST_HS_NOOP command. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{HSAppName},HS_NOOP_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{HSAppName},HS_EVENTMON_MSGACTS_ERR_EID, "ERROR", 2

local expectedMsgActCtr = $SC_$CPU_HS_MsgActCnt + 1

;; Send the command
/$SC_$CPU_HS_NOOP

;; Wait for the NOOP Event
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - NOOP event rcv'd"
  ut_setrequirements HS_1000, "P"
  ut_setrequirements HS_1003, "P"
else
  write "<*> Failed (1000;1003) - Did not rcv the HS NOOP event"
  ut_setrequirements HS_1000, "F"
  ut_setrequirements HS_1003, "F"
endif

;; Wait for the HS Error Event
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000;5000.1) - HS Event Monitor Error event rcv'd"
  ut_setrequirements HS_5000, "P"
  ut_setrequirements HS_50001d, "P"
else
  write "<*> Failed - Did not rcv the HS Event Monitor Error event"
  ut_setrequirements HS_5000, "F"
  ut_setrequirements HS_50001d, "F"
endif

;; This wait is to ensure that the housekeeping packet updated before checking
;; the Message Actions Executed counter
wait 5

;; Check that the HK counter incremented
if ($SC_$CPU_HS_MsgActCnt = expectedMsgActCtr) then
  write "<*> Passed - SB Message Action counter incremented."
else
  write "<!> Failed - SB Message Action counter did not increment as expected."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.21: Send the command to Set the Max cFE Resets equal to the "
write ";  the number of cFE Processor Resets currently reported in the "
write ";  HS Housekeeping packet."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_SET_MAX_RESETS_DBG_EID, "DEBUG", 1

local origMaxResetCnt = $SC_$CPU_HS_MaxResetCnt
cmdCtr = $SC_$CPU_HS_CMDPC + 1

;; Send the Command
/$SC_$CPU_HS_SetMaxResetCnt NewCount=$SC_$CPU_HS_PRResetCtr
wait 5

ut_tlmwait $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;7001) - HS Set Max Processor Resets command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_7001, "P"
else
  write "<!> Failed (1003;7001) - HS Max Processor Resets command did not increment CMDPC."
  write "CMDPC = ",$SC_$CPU_HS_CMDPC, " Expected ", cmdCtr
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_7001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;7001) - Expected Event Msg ",HS_SET_MAX_RESETS_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_7001, "P"
else
  write "<!> Failed (1003;7001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_SET_MAX_RESETS_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_7001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.22: Load the Critical Event Table created in Step 3.5."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt3",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.23: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.24: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event table"
  ut_setrequirements HS_5004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.25: Send the HS NOOP Command and wait for HS to send the    "
write ";  Reset Limit event message.  "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_NOOP_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_RESET_LIMIT_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EVENTMON_PROC_ERR_EID, "ERROR", 3

;; Send the command
/$SC_$CPU_HS_NOOP

;; Wait for the HS NOOP message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - Rcv'd the HS NOOP event message"
  ut_setrequirements HS_1000, "P"
  ut_setrequirements HS_1003, "P"
else
  write "<*> Failed (1000;1003) - Did not rcv the HS NOOP event message"
  ut_setrequirements HS_1000, "F"
  ut_setrequirements HS_1003, "F"
endif

;; Wait for the HS Reset Limit Error message
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000.1.2) - Rcv'd the expected HS Error Event message"
  ut_setrequirements HS_500012, "P"
else
  write "<*> Failed (5000.1.2) - Did not rcv the HS Error Event message as expected"
  ut_setrequirements HS_500012, "F"
endif

;; Wait for the HS Processor Reset Error message
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000.1) - Rcv'd the expected HS Error Event message"
  ut_setrequirements HS_50001, "P"
else
  write "<*> Failed (5000.1) - Did not rcv the HS Error Event message as expected"
  ut_setrequirements HS_50001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.26: Send the command to Set the Max cFE Resets back to its "
write ";  original value. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_SET_MAX_RESETS_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_SetMaxResetCnt NewCount=origMaxResetCnt

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;7001) - HS Set Max Processor Resets command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_7001, "P"
else
  write "<!> Failed (1003;7001) - HS Max Processor Resets command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_7001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;7001) - Expected Event Msg ",HS_SET_MAX_RESETS_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_7001, "P"
else
  write "<!> Failed (1003;7001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_SET_MAX_RESETS_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_7001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Miscellanous Tests "
write ";*********************************************************************"
write ";  Step 4.1: Create and load a new Critical Event Table that contains"
write ";  the <PLATFORM_DEFINED> number of entries. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt6

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt6",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.2: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table failed validation "
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.3: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event table"
  ut_setrequirements HS_5004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4: Dump the Critical Event Table "
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, EvtMonTblName, "A", "$cpu_hs_dumpemt", hostCPU, emtAPID)
wait 5

local index
;; Check that all the application slots are filled
for index = 1 to HS_MAX_MONITORED_EVENTS do
  if ($SC_$CPU_HS_EMT[index].AppName = "") then
    break
  endif
enddo

;; If all the table entries were filled, i will = HS_MAX_MONITORED_EVENTS + 1
if (index = HS_MAX_MONITORED_EVENTS+1) then
  write "<*> Passed (5003) - HS supported the maximum defined Critical Events"
  ut_setrequirements HS_5003, "P"
else
  write "<*> Failed (5003) - The Critical Event Table did not contain the maximum entries"
  write "Index of failure = ", index
  ut_setrequirements HS_5003, "F"
endif

write ";*********************************************************************"
write ";  Step 4.5: Create and load a new Critical Event Table that contains"
write ";  entries for two applications that will never execute. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt7

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt7",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.6: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.7: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event table"
  ut_setrequirements HS_5004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.8: Wait for several HS Housekeeping cycles and then check the"
write ";  Invalid/Unknown Apps counter to verify that it indicates 2 and does"
write ";  not increment every cycle."
write ";*********************************************************************"
;; Wait for 3 HK cycles in order to see if additional events will get generated
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_HS_InvalidEVTAppCnt = 2) then
    write "<*> Passed (5000.2) - HS Invalid/Unknown App Counter is correct."
    ut_setrequirements HS_50002, "P"
  else
    write "<!> Failed (5000.2) - HS Invalid/Unknown App Counter reported ",$SC_$CPU_HS_InvalidEVTAppCnt, "; Expected 2."
    ut_setrequirements HS_50002, "F"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9: Create and load a new Critical Event Table that contains"
write ";  multiple entries for the TST_HS NOOP event. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt9

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt9",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.10: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (5004) - Critical Event Table Validation successful message rcv'd"
    ut_setrequirements HS_5004, "P"
  else
    write "<!> Failed (5004) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.11: Activate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_5004, "P"
else
  write "<*> Failed (5004) - Did not rcv the Table Update Success message for the Critical Event table"
  ut_setrequirements HS_5004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.12: Send the TST_HS_NOOP Command and verify that HS performs "
write ";  the proper actions. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","TST_HS", TST_HS_NOOP_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{HSAppName},HS_EVENTMON_MSGACTS_ERR_EID,"ERROR", 2
ut_setupevents "$SC","$CPU",{HSAppName},HS_EVENTMON_RESTART_ERR_EID,"ERROR", 3

;; Send the command
/$SC_$CPU_TST_HS_NOOP

;; Wait for the NOOP Event
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS NOOP event rcv'd"
else
  write "<*> Failed - Did not rcv the TST_HS NOOP event"
endif

;; Wait for the HS Error Event
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000;5000.1) - HS Event Monitor Message Action error event rcv'd"
  ut_setrequirements HS_5000, "P"
  ut_setrequirements HS_50001d, "P"
else
  write "<*> Failed - Did not rcv the HS Event Monitor Message Action error event"
  ut_setrequirements HS_5000, "F"
  ut_setrequirements HS_50001d, "F"
endif

;; Wait for the HS Error Event
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000;5000.1) - HS Event Monitor restart application error event rcv'd"
  ut_setrequirements HS_5000, "P"
  ut_setrequirements HS_50001c, "P"
else
  write "<*> Failed - Did not rcv the HS Event Monitor restart application error event"
  ut_setrequirements HS_5000, "F"
  ut_setrequirements HS_50001c, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Table Validation Failure Tests"
write ";*********************************************************************"
write ";  Step 5.1: Create and load a new Critical Event Table that contains "
write ";  invalid data. "
write ";*********************************************************************"
s $sc_$cpu_hs_emt8

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_emt8",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Event Table sent successfully."
else
  write "<!> Failed - Load command for Critical Event Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 5.2: Validate the Critical Event Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMTVAL_ERR_EID, "ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Event Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=EvtMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Event Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Event Table did not execute successfully."
endif

;; Wait for the CFE Validation message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (5004;5004.1) - Critical Event Table Validation failed as expected"
    ut_setrequirements HS_5004, "P"
    ut_setrequirements HS_50041, "P"
  else
    write "<!> Failed (5004;5004.1) - Did not rcv Critical Event Table Validation successful message"
    ut_setrequirements HS_5004, "F"
    ut_setrequirements HS_50041, "F"
  endif
else
  write "<!> Failed (5004) - Critical Event Table Failed Validation"
  ut_setrequirements HS_5004, "F"
endif

write ";*********************************************************************"
write ";  Step 5.3: Remove the default tables from the onboard processor."
write ";*********************************************************************"
;; Remove the default table files
s ftp_file (defTblDir,"na",amtFileName,hostCPU,"R")
wait 5
s ftp_file (defTblDir,"na",emtFileName,hostCPU,"R")
wait 5
s ftp_file (defTblDir,"na",matFileName,hostCPU,"R")
wait 5
s ftp_file (defTblDir,"na",xctFileName,hostCPU,"R")
wait 5

write ";*********************************************************************"
write ";  Step 5.4: Stop the HS and TST_HS applications by performing a cFE "
write ";  Processor Reset. "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 5.5: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("5.5")
wait 5

;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Critical Application Table failed to load as expected"
  else
    write "<!> Failed - Critical Application Table load failure message not rcv'd"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Critical Event Table failed to load as expected"
  else
    write "<!> Failed - Critical Event Table load failure message not rcv'd"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Execution Counter Table failed to load as expected"
  else
    write "<!> Failed - Execution Counter Table load failure message not rcv'd"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Message Actions Table failed to load as expected"
  else
    write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

write ";*********************************************************************"
write ";  Step 6.0: Clean-up - Send the Power-On Reset command.             "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
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

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_eventmon "
write ";*********************************************************************"
ENDPROC
