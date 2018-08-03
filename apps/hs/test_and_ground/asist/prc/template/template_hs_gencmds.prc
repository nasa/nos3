PROC $sc_$cpu_hs_gencmds
;*******************************************************************************
;  Test Name:  hs_gencmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) general commands
;	function properly. The NOOP, Reset Counters, CPU Aliveness, and 
;	miscellaneous commands will be tested. Invalid versions of these
;	commands will also be tested to ensure that the HS application handles
;	these properly.
;
;  Requirements Tested
;    HS1000	Upon receipt of a No-Op command, HS shall increment the HS
;		Valid Command Counter and generate an event message.
;    HS1001	Upon receipt of a Reset command, HS shall reset the following
;		housekeeping variables to a value of zero:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;    HS1002	For all HS commands, if the length contained in the message
;		header is not equal to the expected length, HS shall reject the
;		command and issue an event message.
;    HS1003	If HS accepts any command as valid, HS shall execute the
;		command, increment the HS Valid Command Counter and issue an
;		event message.
;    HS1004	If HS rejects any command, HS shall abort the command execution,
;		increment the HS Command Rejected Counter and issue an event
;		message.
;    HS6005	During each HS execution cycle, HS shall send a
;		<PLATFORM_DEFINED> character(s) to the UART port every
;		<PLATFORM_DEFINED> second(s).
;    HS6006	Upon receipt of an Enable CPU Aliveness Indicator command, HS
;		shall begin sending the <PLATFORM_DEFINED> heartbeat 
;		character(s) to the UART port.
;    HS6007	Upon receipt of a Disable CPU Aliveness Indicator command, HS
;		shall stop sending the <PLATFORM_DEFINED> heartbeat character(s)
;		to the UART port.
;    HS6011	Upon receipt of an Enable CPU Utilization Monitoring, HS shall
;		begin monitoring CPU utilization.
;    HS6012	Upon receipt of a Disable CPU Utilization Monitoring, HS shall
;		stop monitoring CPU utilization.
;    HS7000	Upon receipt of a Reset Processor Resets Command, HS shall set
;		the number of cFE Processor Resets commanded by HS to zero.
;    HS7001	Upon receipt of a Set Max Processor Resets Command, HS shall set
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
;		  d. Critical Application Monitoring status per
;		     Application enabled
;		  e. Monitor Critical Events - <PLATFORM_DEFINED>
;		  f. CPU Aliveness Indicator - <PLATFORM_DEFINED>
;		  g. Watchdog Time Flag - TRUE
;		  h. Set the Watchdog Timer to <PLATFORM_DEFINED> value
;		  i. Maximum number of cFE Processor Resets - 
;		     <PLATFORM_DEFINED> value
;		  j. Number of cFE Processor resets (commanded by HS)
;		  k. Number of invalid/Unknown Apps contained in the
;		     Critical Event table
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
;	06/19/09	Walt Moleski	Original Procedure.
;       01/12/11        Walt Moleski    Updated for HS 2.1.0.0
;       10/20/11        Walt Moleski    Added steps to execute the new commands
;					in HS 2.2.0.0
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
#include "cfe_evs_events.h"
#include "to_lab_events.h"
#include "hs_platform_cfg.h"
#include "hs_tbldefs.h"
#include "hs_msgdefs.h"
#include "hs_events.h"
#include "tst_hs_events.h"

%liv (log_procedure) = logging

#define HS_1000		0
#define HS_1001		1
#define HS_1002		2
#define HS_1003		3
#define HS_1004		4
#define HS_6005		5
#define HS_6006		6
#define HS_6007		7
#define HS_6011		8
#define HS_6012		9
#define HS_7000		10
#define HS_7001		11
#define HS_7100		12
#define HS_8000		13

global ut_req_array_size = 13
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_1000","HS_1001","HS_1002","HS_1003","HS_1004","HS_6005","HS_6006","HS_6007","HS_6011","HS_6012","HS_7000","HS_7001","HS_7100","HS_8000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream, index
local HSAppName = HS_APP_NAME
local defTblDir = "CF:0/apps"
local hostCPU = "$CPU"

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
page $SC_$CPU_TST_HS_HK

write ";***********************************************************************"
write ";  Step 1.3: Create & load the default definition tables. "
write ";***********************************************************************"
;; Application Monitoring Table
s $sc_$cpu_hs_amt1

;; Parse the filename configuration parameters for the default table filenames
local amtFileName = HS_AMT_FILENAME
local slashLoc = %locate(amtFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
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

;; loop until all slashes are found for the Destination File Table Name
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

;; loop until all slashes are found for the Destination File Table Name
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

;; loop until all slashes are found for the Destination File Table Name
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
;; Set the HS HK packet ID based upon the cpu being used
;; CPU1 is the default
local hkPktId = "p0AD"

if ("$CPU" = "CPU2") then
  hkPktId = "p1AD"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2AD"
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
write ";  Step 2.1: Send the NO-OP command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_NOOP_INF_EID, "INFO", 1

local cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the NO-OP Command
/$SC_$CPU_HS_NOOP

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - HS NO-OP command sent properly."
  ut_setrequirements HS_1000, "P"
  ut_setrequirements HS_1003, "P"
else
  write "<!> Failed (1000;1003) - HS NO-OP command did not increment CMDPC."
  ut_setrequirements HS_1000, "F"
  ut_setrequirements HS_1003, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - Expected Event Msg ",HS_NOOP_INF_EID," rcv'd."
  ut_setrequirements HS_1000, "P"
  ut_setrequirements HS_1003, "P"
else
  write "<!> Failed (1000;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_NOOP_INF_EID,"."
  ut_setrequirements HS_1000, "F"
  ut_setrequirements HS_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the NO-OP command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_HS_CMDEC + 1

;; Set the CPU1 raw command as the default
rawcmd = "18AEc000000200B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc000000200B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc000000200B0"
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
write ";  Step 2.3: Utilizing the TST_HS application, send the command that  "
write ";  will set all the counters that get reset to zero (0) by the Reset  "
write ";  command to a non-zero value."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_SET_COUNTERS_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_SetCounters

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_SET_COUNTERS_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_SET_COUNTERS_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Verify that all the counters are non-zero and send the   "
write ";  Reset command if so.                                               "
write ";***********************************************************************"
;; Check the HK telemetry
if ($SC_$CPU_HS_CMDPC > 0) AND ($SC_$CPU_HS_CMDEC > 0) THEN 
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {HSAppName}, HS_RESET_DBG_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_HS_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_HS_ResetCtrs
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",HS_RESET_DBG_EID," rcv'd."
    ut_setrequirements HS_1001, "P"
    ut_setrequirements HS_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_RESET_DBG_EID,"."
    ut_setrequirements HS_1001, "F"
    ut_setrequirements HS_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_HS_CMDPC = 0) AND ($SC_$CPU_HS_CMDEC = 0) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements HS_1001, "P"
  else
    write "<!> Failed (1001) - Counters did not reset to zero."
    ut_setrequirements HS_1001, "F"
  endif
else
  write "<!> Reset command not sent because at least 1 counter is set to 0."
endif

;; Write out the counters for verification
write "CMDPC           = ", $SC_$CPU_HS_CMDPC
write "CMDEC           = ", $SC_$CPU_HS_CMDEC

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Reset command with an invalid length.             "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEc000000201B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc000000201B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc000000201B0"
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
write ";  Step 2.6: Send the Disable CPU Aliveness command."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{HSAppName},HS_DISABLE_ALIVENESS_DBG_EID,"DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_DisableCPUAlive

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6007) - HS Disable CPU Aliveness command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6007, "P"
else
  write "<!> Failed (1003;6007) - HS Disable CPU Aliveness command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6007, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6007) - Expected Event Msg ",HS_NOOP_INF_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6007, "P"
else
  write "<!> Failed (1003;6007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_NOOP_INF_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6007, "F"
endif

wait 5

;; Check that the CPU Alive HK Parameter is set to Disabled
if (p@$SC_$CPU_HS_CPUAliveState = "Disabled") then
  write "<*> Passed (6007) - Telemetry indicates CPU Alive State is Disabled."
  ut_setrequirements HS_6007, "P"
else
  write "<!> Failed (6007) - Telemetry indicates incorrect CPU Alive State of ", p@$SC_$CPU_HS_CPUAliveState,". Expected Disabled."
  ut_setrequirements HS_6007, "F"
endif

write ";***********************************************************************"
write ";  Step 2.7: Send the Disable CPU Aliveness command with an invalid "
write ";  length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEc000000207B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc000000207B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc000000207B0"
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
write ";  Step 2.8: Send the Enable CPU Aliveness command."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{HSAppName},HS_ENABLE_ALIVENESS_DBG_EID,"DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_EnableCPUAlive

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - HS Enable CPU Aliveness command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6005, "A"
  ut_setrequirements HS_6006, "P"
else
  write "<!> Failed (1003;6006) - HS Enable CPU Aliveness command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6005, "F"
  ut_setrequirements HS_6006, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",HS_ENABLE_ALIVENESS_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_ENABLE_ALIVENESS_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6006, "F"
endif

wait 5

;; Check that the CPU Alive HK Parameter is set to Enabled
if (p@$SC_$CPU_HS_CPUAliveState = "Enabled") then
  write "<*> Passed (6006) - Telemetry indicates CPU Alive State is Enabled."
  ut_setrequirements HS_6006, "P"
else
  write "<!> Failed (6006) - Telemetry indicates incorrect CPU Alive State of ", p@$SC_$CPU_HS_CPUAliveState,". Expected Enabled."
  ut_setrequirements HS_6006, "F"
endif

write ";***********************************************************************"
write ";  Step 2.9: Send the Enable CPU Aliveness command with an invalid "
write ";  length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
 
;; Set the CPU1 raw command as the default
rawcmd = "18AEc000000206B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc000000206B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc000000206B0"
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
write ";  Step 2.10: Send the TST_HS command to increment the HS Resets "
write ";  Performed counter. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_SET_RESETS_PERFORMED_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_SetResetsPerformed

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_SET_RESETS_PERFORMED_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_SET_RESETS_PERFORMED_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.11: Send the Reset Processor Resets command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_RESET_RESETS_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_ResetPRCtr

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;7000) - HS Reset Processor Reset Counter command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_7000, "P"
else
  write "<!> Failed (1003;7000) - HS Reset Processor Reset Counter command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_7000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;7000) - Expected Event Msg ",HS_RESET_RESETS_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_7000, "P"
else
  write "<!> Failed (1003;7000) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_RESET_RESETS_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_7000, "F"
endif

wait 5

;; Check the counter to ensure it was reset
if ($SC_$CPU_HS_PRResetCtr = 0) then
  write "<*> Passed (7000) - The cFE Processor Resets Performed Counter was set to 0."
  ut_setrequirements HS_7000, "P"
else
  write "<!> Failed (7000) - The cFE Processor Resets Performed Counter was not reset."
  ut_setrequirements HS_7000, "F"
endif

write ";***********************************************************************"
write ";  Step 2.12: Send the Reset Processor Resets command with an invalid "
write ";  length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
 
;; Set the CPU1 raw command as the default
rawcmd = "18AEc000000208B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc000000208B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc000000208B0"
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
write ";  Step 2.13: Send the Set Max Processor Resets command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_SET_MAX_RESETS_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_SetMaxResetCnt NewCount=25

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

write ";***********************************************************************"
write ";  Step 2.14: Send the Set Max Processor Resets command with an invalid "
write ";  length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEc000000209B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc000000209B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc000000209B0"
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
write ";  Step 2.15: Send the Disable CPU Utilization Monitoring command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_DISABLE_CPUHOG_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_DisableCPUHog

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6012) - HS Disable CPU Utilization command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6012, "P"
else
  write "<!> Failed (1003;6012) - HS Disable CPU Utilization command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6012, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6012) - Expected Event Msg ",HS_DISABLE_CPUHOG_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6012, "P"
else
  write "<!> Failed (1003;6012) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_DISABLE_CPUHOG_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6012, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.16: Send the Disable CPU Utilization Monotoring command with "
write ";  an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEc00000020B83"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc00000020B83"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc00000020B83"
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
write ";  Step 2.17: Send the Enable CPU Utilization Monitoring command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_ENABLE_CPUHOG_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_EnableCPUHog

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6011) - HS Enable CPU Utilization command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6011, "P"
else
  write "<!> Failed (1003;6011) - HS Enable CPU Utilization command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6011, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6011) - Expected Event Msg ",HS_ENABLE_CPUHOG_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_6011, "P"
else
  write "<!> Failed (1003;6011) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_ENABLE_CPUHOG_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_6011, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.18: Send the Enable CPU Utilization Monotoring command with "
write ";  an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEc00000020A82"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc00000020A82"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc00000020A82"
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
write ";  Step 2.19: Send an invalid command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_CC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEc0000001AA"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc0000001AA"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc0000001AA"
endif

ut_sendrawcmd "$SC_$CPU_HS", (rawcmd)

ut_tlmwait $SC_$CPU_HS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements HS_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements HS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HS_CC_ERR_EID, "."
  ut_setrequirements HS_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.20: Send the Report Diagnostics command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, 103, "INFO", 1

;; Send the Command
/$SC_$CPU_HS_ReportDiag

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS Report Diagnostics event message was generated."
else
  write "<!> Failed - HS Report Diagnostics event message was not generated."
endif

write ";***********************************************************************"
write ";  Step 2.21: Send the Set CPU Utilization Parameters command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, 104, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1

;; Send the Command
/$SC_$CPU_HS_SetUtilParams Factor1=2500 Divisor=50505 Factor2=1

ut_tlmwait $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS Set CPU Utilization parameters command sent properly."
else
  write "<!> Failed - HS Set CPU Utilization parameters command did not increment the CMDPC."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message was generated."
else
  write "<!> Failed - Event message was not generated."
endif

write ";***********************************************************************"
write ";  Step 2.22: Send the Set CPU Utilization Parameters command with "
write ";  an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEC000000E0D62000009C40000C54900000001"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEC000000E0D63000009C40000C54900000001"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEC000000E0DCB000009C40000C54900000001"
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
write ";  Step 2.23: Send the Set CPU Utilization Mask command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, 106, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1

;; Send the Command
/$SC_$CPU_HS_SetUtilMask UtilMask=x'FFFF'

ut_tlmwait $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS Set CPU Utilization Mask command sent properly."
else
  write "<!> Failed - HS Set CPU Utilization Mask command did not increment the CMDPC."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message was generated."
else
  write "<!> Failed - Event message was not generated."
endif

write ";***********************************************************************"
write ";  Step 2.24: Send the Set CPU Utilization Mask command with "
write ";  an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
  
;; Set the CPU1 raw command as the default
rawcmd = "18AEC00000060E82FFFFFFFF"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEC00000060E83FFFFFFFF"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEC00000060E80FFFFFFFF"
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

write ";*********************************************************************"
write ";  Step 3.0: Clean-up - Send the Processor Reset command.             "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
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
write ";  End procedure $SC_$CPU_hs_gencmds"
write ";*********************************************************************"
ENDPROC
