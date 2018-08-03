PROC $sc_$cpu_hs_appmon
;*******************************************************************************
;  Test Name:  hs_appmon
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) Application 
;	Monitoring command function properly. Invalid commands as well as
;	anomalies will be tested to ensure that the HS application handles
;	these appropriately.
;
;  Requirements Tested
;    HS1002	For all HS commands, if the length contained in the message
;		header is not equal to the expected length, HS shall reject the
;		command and issue an event message.
;    HS1003	If HS accepts any command as valid, HS shall execute the
;		command, increment the HS Valid Command Counter and issue an
;		event message.
;    HS1004	If HS rejects any command, HS shall abort the command execution,
;		increment the HS Command Rejected Counter and issue an event
;		message.
;    HS2000	The HS application shall verify that each application defined in
;		the Critical Application Table is executing.
;    HS2000.1	If the entry indicates that the application is a cFE Core
;		Application and it has not executed for the corresponding table-
;		defined number of HS execution cycles, HS shall perform one of
;		the table-defined actions:
;		  a. cFE Processor Reset
;		  b. Send an Event message
;		  c. Send a Software Bus message
;		  d. Perform No Action
;    HS2000.1.1	If the action is to perform a cFE Processor Reset and the number
;		of cFE Processor Resets is less than the <PLATFORM_DEFINED> Max
;		Number of cFE Processor Resets, HS shall
;		  a. Increment the Number of cFE Processor Resets
;		  b. Set the Watchdog servicing flag to False
;		  c. Command the cFE Processor Reset
;    HS2000.1.2	If the action is to perform a cFE Processor Reset and the number
;		of cFE Processor Resets is greater-than-or-equal-to the
;		<PLATFORM_DEFINED> Max Number of cFE Processor Resets, HS shall
;		  a. Send an event message
;    HS2000.2	If the entry indicates that the application is not a cFE Core
;		Application and it has not executed for the corresponding table-
;		defined number of HS execution cycles, HS shall execute one of
;		the table-defined actions:
;		  a. Restart the application (that failed to check-in)
;		  b. cFE Processor Reset
;		  c. Send an Event message
;		  d. Send a Software Bus message
;		  e. Perform No Action
;    HS2000.2.1	If the action is to perform a cFE Processor Reset and the number
;		of cFE Processor Resets is less than the <PLATFORM_DEFINED> Max
;		Number of cFE Processor Resets, HS shall
;		  a. Increment the Number of cFE Processor Resets
;		  b. Set the Watchdog servicing flag to False
;		  c. Command the cFE Processor Reset
;    HS2000.2.2	If the action is to perform a cFE Processor Reset and the number
;		of cFE Processor Resets is greater-than-or-equal-to the
;		<PLATFORM_DEFINED> Max Number of cFE Processor Resets, HS shall
;		  a. Send an event message
;    HS2000.2.3	If the action is to perform an Application Restart, HS shall
;		disable the entry in the Critical Application Table.
;    HS2000.3	If the entry in the table references an unresolvable application
;		(i.e., not registered with cFE), HS shall issue an event message
;    HS2001	Upon receipt of an Enable Critical Application Monitoring
;		Command, HS shall
;		  a. Enable all entries in the Critical Application Table
;		  b. Execute the Critical Application Table
;    HS2002	Upon receipt of a Disable Critical Application Monitoring
;		Command, HS shall stop processing the Critical Application Table
;    HS2003	HS shall support up to <PLATFORM_DEFINED> critical applications.
;    HS2004	Upon receipt of a Critical Application Table update indication,
;		HS shall validate the Critical Application Table by validating
;		the action.
;    HS2004.1	If the Critical Application Table fails validation, HS shall
;		issue an event message.
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
;		  l. Peak CPU Utilization
;		  m. Average CPU utilization
;		  n. CPU Utilization Monitoring Enabled/Disabled
;    HS8000	Upon cFE Power On Reset, HS shall initialize the following data
;		to Zero:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. Monitor Critical Applications - <PLATFORM_DEFINED>
;		  d. Critical Application Monitoring status per Application
;		     enabled
;		  e. Monitor Critical Events - <PLATFORM_DEFINED>
;		  f. CPU Aliveness Indicator - <PLATFORM_DEFINED>
;		  g. Watchdog Time Flag - TRUE
;		  h. Set the Watchdog Timer to <PLATFORM_DEFINED> value
;		  i. Maximum number of cFE Processor Resets - <PLATFORM_DEFINED>;		     value
;		  j. Number of cFE Processor resets (commanded by HS)
;		  k. Number of invalid/Unknown Apps contained in the Critical
;		     Event table
;		  l. Peak CPU Utilization
;		  m. Average CPU utilization
;		  n. CPU Utilization Monitoring Enabled/Disabled -
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
;	06/22/09	Walt Moleski	Original Procedure.
;	01/12/11	Walt Moleski	Updated for HS 2.1.0.0
;       09/16/16        Walt Moleski    Updated for HS 2.3.0.0 using CPU1 for
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
#include "lc_platform_cfg.h"

%liv (log_procedure) = logging

#define HS_1002		0
#define HS_1003		1
#define HS_1004		2
#define HS_2000		3
#define HS_20001a	4
#define HS_20001b	5
#define HS_20001c	6
#define HS_200011	7
#define HS_200012	8
#define HS_20002a	9
#define HS_20002b	10
#define HS_20002c	11
#define HS_20002d	12
#define HS_200021	13
#define HS_200022	14
#define HS_200023	15
#define HS_20003	16
#define HS_2001		17
#define HS_2002		18
#define HS_2003		19
#define HS_2004		20
#define HS_20041	21
#define HS_7001		22
#define HS_7100		23
#define HS_8000		24

global ut_req_array_size = 24
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_1002","HS_1003","HS_1004","HS_2000","HS_2000.1a","HS_2000.1b","HS_2000.1c","HS_2000.1.1","HS_2000.1.2","HS_2000.2a","HS_2000.2b","HS_2000.2c","HS_2000.2d","HS_2000.2.1","HS_2000.2.2","HS_2000.2.3","HS_2000.3","HS_2001","HS_2002","HS_2003","HS_2004","HS_2004.1","HS_7001","HS_7100","HS_8000" ]

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
page $SC_$CPU_HS_APPMON_TABLE
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
wait 5

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
wait 5

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
wait 5

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
wait 5

write ";***********************************************************************"
write ";  Step 1.4:  Start the Health and Safety (HS) and Test Applications.   "
write ";***********************************************************************"
s $sc_$cpu_hs_start_apps("1.4")
wait 5

write ";***********************************************************************"
write ";  Step 1.5: Verify that the HS Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Set the HS HK packet ID and Critical Application Table appID based upon 
;; the cpu being used
;; CPU1 is the default
local hkPktId = "p0AD"
local amtAPID = "0F72"

if ("$CPU" = "CPU2") then
  hkPktId = "p1AD"
  amtAPID = "0F80"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2AD"
  amtAPID = "0F91"
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
   ($SC_$CPU_HS_CPUHOGState = HS_CPUHOG_DEFAULT_STATE) AND ;;
   (p@$SC_$CPU_TST_HS_WatchdogFlag = "TRUE") AND ;;
   ($SC_$CPU_HS_CPUAliveState = HS_ALIVENESS_DEFAULT_STATE) then
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly."
  ut_setrequirements HS_8000, "P"
  ;;***************************************************************************
  ;; The Watchdog Timer cannot be tested here and must be inspected in the fsw
  ;; The following items will vary based on the mission settings and should be
  ;; visually inspected after Power-ON:
  ;;   $SC_$CPU_HS_CPUUtilAve; $SC_$CPU_HS_CPUUtilPeak; $SC_$CPU_HS_APPStatus
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
write ";  Step 1.6: Enable DEBUG Event Messages "
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
write ";  Step 2.0: Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Send the Disable Application Monitoring command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_DISABLE_APPMON_DBG_EID, "DEBUG",1

local cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_DisableAppMon

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - HS Disable Application Monitoring command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_2002, "P"
else
  write "<!> Failed (1003;2002) - HS Disable Application Monitoring command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_2002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - Expected Event Msg ",HS_DISABLE_APPMON_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_2002, "P"
else
  write "<!> Failed (1003;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_DISABLE_APPMON_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_2002, "F"
endif

wait 5

;; Check that the Critical Application Monitoting Parameter is set to Disabled
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (2002) - Telemetry indicates Application Monitoring State is Disabled."
  ut_setrequirements HS_2002, "P"
else
  write "<!> Failed (2002) - Telemetry indicates incorrect Application Monitoring State of ", p@$SC_$CPU_HS_AppMonState,". Expected Disabled."
  ut_setrequirements HS_2002, "F"
endif

write ";***********************************************************************"
write ";  Step 2.2: Send the Disable Application Monitoring command with an "
write ";  invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR",1

local errcnt = $SC_$CPU_HS_CMDEC + 1

;; Set the CPU1 raw command as the default
rawcmd = "18AEc00000020389"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc00000020389"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc00000020389"
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
write ";  Step 2.3: Send the Enable Application Monitoring command with an "
write ";  invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HS_CMDEC + 1
 
;; Set the CPU1 raw command as the default
rawcmd = "18AEc00000020288"

if ("$CPU" = "CPU2") then
  rawcmd = "19AEc00000020288"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAEc00000020288"
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
write ";  Step 2.4: Send the Enable Application Monitoring command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_ENABLE_APPMON_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_EnableAppMon

ut_tlmwait  $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2001) - HS Enable Application Monitoring command sent properly."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_2001, "P"
else
  write "<!> Failed (1003;2001) - HS Enable Application Monitoring command did not increment CMDPC."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_2001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2001) - Expected Event Msg ",HS_ENABLE_APPMON_DBG_EID," rcv'd."
  ut_setrequirements HS_1003, "P"
  ut_setrequirements HS_2001, "P"
else
  write "<!> Failed (1003;2001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_ENABLE_APPMON_DBG_EID,"."
  ut_setrequirements HS_1003, "F"
  ut_setrequirements HS_2001, "F"
endif

wait 5

;; Check that the Critical Application Monitoting Parameter is set to Enabled
if (p@$SC_$CPU_HS_AppMonState = "Enabled") then
  write "<*> Passed (2001) - Telemetry indicates Application Monitoring State is Enabled."
  ut_setrequirements HS_2001, "P"
else
  write "<!> Failed (2001) - Telemetry indicates incorrect Application Monitoring State of ", p@$SC_$CPU_HS_AppMonState,". Expected Enabled."
  ut_setrequirements HS_2001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.5: Dump the Critical Application Table."
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir,AppMonTblName, "A", "$cpu_hs_dumpamt", hostCPU, amtAPID)
wait 5

;; Check that each application in the table is executing
;; Since there are 5 applications in amt1, the AppStatus field should be set
;; to 0x1F indicating that all 5 apps are enabled
if ($SC_$CPU_HS_AppStatus[1] = 0x1F) then
  write "<*> Passed (2000) - Telemetry indicates that all applications are executing."
  ut_setrequirements HS_2000, "P"
else
  write "<!> Failed (2000) - Telemetry indicates ", $SC_$CPU_HS_AppStatus,". Expected 0x1F."
  ut_setrequirements HS_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.0: cFE Core Application Tests "
write ";*********************************************************************"
write ";  Step 3.1: Create and load a new Critical Application Table that "
write ";  contains a Core cFE Application that will trigger a cFE Processor "
write ";  Reset. "
write ";*********************************************************************"
s $sc_$cpu_hs_amt2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt2",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.2: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
;; Before activating the table, save the Reset Counter
local expectedResetCtr = $SC_$CPU_HS_PRResetCtr + 1

ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
;; NOTE: The following event may not be rcv'd by the proc since the action is
;; to issue a Processor Reset. The uart should contain the event
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_PROC_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

;; Wait for the Table Update message from CFE_TBL
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1

;; Set the requirement to 'A' for the Processor Reset
ut_setrequirements HS_20001a, "A"
wait 10

write ";*********************************************************************"
write ";  Step 3.4: Wait for HS to initiate the cFE Processor Reset.   "
write ";*********************************************************************"

close_data_center
wait 75
                                                                                
;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 3.5: Start the Health and Safety (HS) and Test Applications.   "
write ";***********************************************************************"
s $sc_$cpu_hs_start_apps("3.5")
wait 5

write ";***********************************************************************"
write ";  Step 3.6: Enable DEBUG Event Messages "
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
write ";  Step 3.7: Verify the number of cFE Processor Resets counter "
write ";  incremented by 1."
write ";***********************************************************************"
;; Check the Reset Counter
if ($SC_$CPU_HS_PRResetCtr = expectedResetCtr) then
  write "<*> Passed (2000.1.1) - cFE Processor Reset occurred and Counter incremented as expected."
  ut_setrequirements HS_200011, "P"
else
  write "<*> Failed (2000.1.1) - The cFE Processor Reset Counter indicated ",$SC_$CPU_HS_PRResetCtr,". Expected ",expectedResetCtr
  ut_setrequirements HS_200011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.8: Create and load a new Critical Application Table that "
write ";  contains a Core cFE Application that will trigger an Event message."
write ";*********************************************************************"
s $sc_$cpu_hs_amt3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt3",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.9: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.10: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_FAIL_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application table"
  ut_setrequirements HS_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.11: Wait for HS to generate the event message.  "
write ";*********************************************************************"
;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.1) - Rcv'd the expected HS Error Event message"
  ut_setrequirements HS_20001b, "P"
else
  write "<*> Failed (2000.1) - Did not rcv the HS Error Event message as expected"
  ut_setrequirements HS_20001b, "F"
endif

write ";*********************************************************************"
write ";  Step 3.12: Create and load a new Critical Application Table that "
write ";  contains a Core cFE Application that will trigger an SB message."
write ";*********************************************************************"
s $sc_$cpu_hs_amt4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt4",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.13: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.14: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_MSGACTS_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_NOOP_INF_EID, "INFO", 3

local expectedMsgActCtr = $SC_$CPU_HS_MsgActCnt + 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.15: Wait for HS to generate the SB message.  "
write ";*********************************************************************"
;; Look for the HS Error event
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.1) - Rcv'd the Message Action Error event"
  ut_setrequirements HS_20001c, "P"
else
  write "<*> Failed (2000.1) - Did not rcv the Message Action Error event"
  ut_setrequirements HS_20001c, "F"
endif

;; Look for the Event from the SB Message (HS_NOOP)
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.1) - Rcv'd the Event from the SB Message (HS_NOOP command) event"
  ut_setrequirements HS_20001c, "P"
else
  write "<*> Failed (2000.1) - Did not rcv the SB Message (HS_NOOP command) event"
  ut_setrequirements HS_20001c, "F"
endif

;; Check that the HK counter incremented
ut_tlmwait $SC_$CPU_HS_MsgActCnt, {expectedMsgActCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SB Message Action counter incremented."
else
  write "<!> Failed - SB Message Action counter did not increment as expected. Got ",$SC_$CPU_HS_MsgActCnt,"; Expected ",expectedMsgActCtr
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.16: Send the command to Set the Max cFE Resets equal to the "
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
write ";  Step 3.17: Load the Critical Application Table created in Step 3.1."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt2",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.18: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.19: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
;; Before activating the table, save the Reset Counter
expectedResetCtr = $SC_$CPU_HS_PRResetCtr + 1

ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_RESET_LIMIT_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.20: Wait for HS to send the expected Event Message rather than"
write ";  the cFE Processor Reset. "
write ";*********************************************************************"
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.1.2) - Rcv'd the expected Event message in place of the cFE Processor Reset"
  ut_setrequirements HS_200012, "P"
else
  write "<*> Failed (2000.1.2) - Did not rcv the expected Event message."
  ut_setrequirements HS_200012, "F"
endif

write ";*********************************************************************"
write ";  Step 3.21: Send the command to Set the Max cFE Resets back to its "
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
write ";  Step 4.0: cFE Non-Core Application Tests "
write ";*********************************************************************"
write ";  Step 4.1: Create and load a new Critical Application Table that "
write ";  contains an Application that will trigger an Application Restart. "
write ";*********************************************************************"
s $sc_$cpu_hs_amt5

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt5",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.2: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.3: Startup the Limit Checker (LC) application. The LC app "
write ";  is used to trigger actions by a cFE Non-Core application. "
write ";*********************************************************************"
write ";  Step 4.3.1: Create and load the table files required by the LC app."
write ";*********************************************************************"
s $SC_$CPU_lc_wdt1

;; Parse the filename configuration parameters for the default table filenames
local wdtFileName = LC_WDT_FILENAME 
slashLoc = %locate(wdtFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  wdtFileName = %substring(wdtFileName,slashLoc+1,%length(wdtFileName))
  slashLoc = %locate(wdtFileName,"/")
enddo

write "==> Default LC Watchpoint Table filename = '",wdtFileName,"'"

s ftp_file(ramDir, "lc_def_wdt1.tbl", wdtFileName, hostCPU, "P")

s $SC_$CPU_lc_adt1

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME 
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file(ramDir, "lc_def_adt1.tbl", adtFileName, hostCPU, "P")

write ";*********************************************************************"
write ";  Step 4.3.2: Start the LC app."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1

s load_start_app ("LC","$CPU", "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - LC Application Started"
else
  write "<!> Failed - CFE_ES start Event Message for LC not received."
endif

write ";*********************************************************************"
write ";  Step 4.4: Activate the Critical Application Table loaded in Step 4.2"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_RESTART_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.5: Wait for HS to initiate the Application Restart.   "
write ";*********************************************************************"
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2) - Rcv'd the expected Application Restart Error message"
  ut_setrequirements HS_20002a, "P"
else
  write "<*> Failed (2000.2) - Did not rcv the Application Restart Error message."
  ut_setrequirements HS_20002a, "F"
endif

wait 5

;; Verify that the HS application disabled this entry
write "AppStatus = ", $sc_$cpu_hs_appstatus
if ($SC_$CPU_HS_APPSTATUS = 0x1F) then
  write "<*> Passed (2000.2.3) - Application Status in HK indicates entry 6 is disabled."
  ut_setrequirements HS_200023, "P"
else
  write "<*> Failed (2000.2.3) - HK Application Status is not correct after the application restart."
  ut_setrequirements HS_200023, "F"
endif

write ";*********************************************************************"
write ";  Step 4.6: Create and load a new Critical Application Table that "
write ";  contains an Application that will trigger a cFE Processor Reset. "
write ";*********************************************************************"
s $sc_$cpu_hs_amt6

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt6",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.7: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.8: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
;; Before activating the table, save the Reset Counter
expectedResetCtr = $SC_$CPU_HS_PRResetCtr + 1

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

;; Set the Processor Reset requirement to 'A'
ut_setrequirements HS_20002b, "A"

write ";*********************************************************************"
write ";  Step 4.9: Wait for HS to initiate the cFE Processor Reset.   "
write ";*********************************************************************"
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 4.10: Start the Applications.   "
write ";***********************************************************************"
write ";  Step 4.10.1: Start the Health and Safety (HS) and Test Applications."
write ";***********************************************************************"
s $sc_$cpu_hs_start_apps("4.10.1")
wait 5

write ";*********************************************************************"
write ";  Step 4.10.2: Start the LC app."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1

s load_start_app ("LC",hostCPU, "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - LC Application Started"
else
  write "<!> Failed - CFE_ES start Event Message for LC not received."
endif

write ";***********************************************************************"
write ";  Step 4.11: Enable DEBUG Event Messages "
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
write ";  Step 4.12: Verify the number of cFE Processor Resets counter "
write ";  incremented by 1."
write ";***********************************************************************"
;; Check the Reset Counter
if ($SC_$CPU_HS_PRResetCtr = expectedResetCtr) then
  write "<*> Passed (2000.2.1) - cFE Processor Reset Counter incremented as expected."
  ut_setrequirements HS_200021, "P"
else
  write "<*> Failed (2000.2.1) - The cFE Processor Reset Counter indicated ",$SC_$CPU_HS_PRResetCtr,". Expected ",expectedResetCtr
  ut_setrequirements HS_200021, "F"
endif

write ";*********************************************************************"
write ";  Step 4.13: Create and load a new Critical Application Table that "
write ";  contains an Application that will trigger an Event message."
write ";*********************************************************************"
s $sc_$cpu_hs_amt7

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt7",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.14: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.15: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_FAIL_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application table"
  ut_setrequirements HS_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.16: Wait for HS to generate the event message.  "
write ";*********************************************************************"
;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2) - Rcv'd the expected HS Error Event message"
  ut_setrequirements HS_20002c, "P"
else
  write "<*> Failed (2000.2) - Did not rcv the HS Error Event message as expected"
  ut_setrequirements HS_20002c, "F"
endif

write ";*********************************************************************"
write ";  Step 4.17: Create and load a new Critical Application Table that "
write ";  contains a Core cFE Application that will trigger an SB message."
write ";*********************************************************************"
s $sc_$cpu_hs_amt8

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt8",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.18: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.19: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_MSGACTS_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_NOOP_INF_EID, "INFO", 3

expectedMsgActCtr = $SC_$CPU_HS_MsgActCnt + 1
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.20: Wait for HS to generate the SB message.  "
write ";*********************************************************************"
;; Look for the HS Error event
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2) - Rcv'd the Message Action Error event"
  ut_setrequirements HS_20002d, "P"
else
  write "<*> Failed (2000.2) - Did not rcv the Message Action Error event"
  ut_setrequirements HS_20002d, "F"
endif

;; Look for the Event from the SB Message (TST_HS_NOOP)
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2) - Rcv'd the Event from the SB Message (TST_HS_NOOP command) event"
  ut_setrequirements HS_20002c, "P"
else
  write "<*> Failed (2000.2) - Did not rcv the SB Message (TST_HS_NOOP command) event"
  ut_setrequirements HS_20002c, "F"
endif

;; Check that the HK counter incremented
ut_tlmwait $SC_$CPU_HS_MsgActCnt, {expectedMsgActCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SB Message Action counter incremented."
else
  write "<!> Failed - SB Message Action counter did not increment as expected. Got ",$SC_$CPU_HS_MsgActCnt,"; Expected ",expectedMsgActCtr
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.21: Send the command to Set the Max cFE Resets equal to the "
write ";  the number of cFE Processor Resets currently reported in the "
write ";  HS Housekeeping packet."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_SET_MAX_RESETS_DBG_EID, "DEBUG", 1

origMaxResetCnt = $SC_$CPU_HS_MaxResetCnt
cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_SetMaxResetCnt NewCount=$SC_$CPU_HS_PRResetCtr

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
write ";  Step 4.22: Load the Critical Application Table created in Step 4.6."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt6",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 4.23: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.24: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_RESET_LIMIT_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.25: Wait for the expected HS Event Message rather than the  "
write ";  cFE Processor Reset. "
write ";*********************************************************************"
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2.2) - Rcv'd the expected Event message in place of the cFE Processor Reset"
  ut_setrequirements HS_200022, "P"
else
  write "<*> Failed (2000.2.2) - Did not rcv the expected Event message."
  ut_setrequirements HS_200022, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Miscellaneous Tests "
write ";*********************************************************************"
write ";  Step 5.1: Create and load a new Critical Application Table that "
write ";  contains the maximum number of Applications. "
write ";*********************************************************************"
s $sc_$cpu_hs_amt9

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt9",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 5.2: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 5.3: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_RESTART_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";***********************************************************************"
write ";  Step 5.4: Dump the Critical Application Table."
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir, AppMonTblName, "A", "$cpu_hs_dumpamt", hostCPU, amtAPID)
wait 5

local index
;; Check that all the application slots are filled
for index = 1 to HS_MAX_MONITORED_APPS do
  if ($SC_$CPU_HS_AMT[index].AppName = "") then
    break
  endif
enddo

;; If all the table entries were filled, i will = HS_MAX_MONITORED_APPS
if (index = HS_MAX_MONITORED_APPS+1) then
  write "<*> Passed (2003) - HS supported the maximum defined Critical Applications"
  ut_setrequirements HS_2003, "P"
else
  write "<*> Failed (2003) - The Critical Application Table did not contain the maximum applications"
  write "Index of failure = ", index
  ut_setrequirements HS_2003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.5: Create and load a new Critical Application Table that "
write ";  contains two applications that will never execute. "
write ";*********************************************************************"
s $sc_$cpu_hs_amt10

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt10",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 5.6: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 5.7: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_FAIL_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 5.8: Verify that 2 event messages are received for the apps "
write ";  in the Critical Application Table that did not execute."
write ";*********************************************************************"
ut_tlmwait $sc_$cpu_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.3) - Rcv'd the proper error messages for the application that did not execute"
  ut_setrequirements HS_20003, "P"
else
  write "<*> Failed (2000.3) - Did not rcv the expected error messages"
  ut_setrequirements HS_20003, "F"
endif

;; Wait for 2 HK cycles in order to see if additional events will get generated
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 2) then
    write "<*> Passed - Only the initial error events were generated"
  else
    write "<!> Failed - More error events were generated than expected"
  endif
endif

write ";*********************************************************************"
write ";  Step 5.9: Create and load a new Critical Application Table that "
write ";  contains multiple entries for the HS application. "
write ";  NOTE: These actions will not get triggered but should be allowed"
write ";*********************************************************************"
s $sc_$cpu_hs_amt13

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt13",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 5.10: Validate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2004) - Critical Application Table Validation successful message rcv'd"
    ut_setrequirements HS_2004, "P"
  else
    write "<!> Failed (2004) - Did not rcv Critical Application Table Validation successful message"
    ut_setrequirements HS_2004, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Failed Validation"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 5.11: Activate the Critical Application Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Activate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2004) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_2004, "P"
else
  write "<*> Failed (2004) - Did not rcv the Table Update Success message for the Critical Application Table table"
  ut_setrequirements HS_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0: Table Validation Failure Tests"
write ";*********************************************************************"
write ";  Step 6.1: Create and load a new Critical Application Table that "
write ";  contains invalid data. "
write ";*********************************************************************"
s $sc_$cpu_hs_amt11

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Application Table
s load_table("hs_def_amt11",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Critical Application Table sent successfully."
else
  write "<!> Failed - Load command for Critical Application Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 6.2: Validate the Critical Application Table loaded above. Since"
write ";  the table contains invalid data, the validation is expected to fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMTVAL_ERR_EID, "ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Critical Application Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=AppMonTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Critical Application Table sent successfully."
else
  write "<!> Failed - Validate command for the Critical Application Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (2004;2004.1) - Critical Application Table Validation failed as expected"
    ut_setrequirements HS_2004, "P"
    ut_setrequirements HS_20041, "P"
  else
    write "<!> Failed (2004;2004.1) - Did not rcv the Critical Application Table Validation Error message"
    ut_setrequirements HS_2004, "F"
    ut_setrequirements HS_20041, "F"
  endif
else
  write "<!> Failed (2004) - Critical Application Table Validation failed message not rcv'd"
  ut_setrequirements HS_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 6.3: Remove the default Critical Application Table from the "
write ";  onboard processor."
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
write ";  Step 6.4: Stop the HS and TST_HS applications by performing a cFE "
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
write ";  Step 6.5: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("6.5")
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
write ";  Step 7.0: Clean-up - Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_hs_appmon "
write ";*********************************************************************"
ENDPROC
