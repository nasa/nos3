PROC $sc_$cpu_hs_reset
;*******************************************************************************
;  Test Name:  hs_reset
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) Application
;	initializes the appropriate data items based upon the type of
;	initialization that occurs (Application Reset, cFE Processor Reset, or
;	cFE Power-On Reset). This test also verifies that the proper
;	notifications occur if any anomalies exist with the data items stated
;	in the requirements.
;
;  Requirements Tested
;    HS1003	If HS accepts any command as valid, HS shall execute the
;		command, increment the HS Valid Command Counter and issue an
;		event message.
;    HS7001	Upon receipt of a Set Max Processor Resetc Command, HS shall set
;		the Maximum number of cFE Processor Resets commanded by HS to
;		the command-specified value.
;    HS7100	HS shall generate a housekeeping message containing the
;		following:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. Critical Application Monitoring Status (enable/disable)
;		  d. Critical Application Monitoring Status per table
;		     entry (enable/disable)
;		  e. Number of cFE Processor Resets (commanded by HS)
;		  f. maximum number of cFE Processor resets
;		  g. Critical Event Monitoring status (enable/disable)
;		  h. Count of Monitored Event Messages
;		  i. CPU Aliveness indicator (enable/disable)
;		  j. Execution Counter, for each table entry
;		  k. Number of Invalid/Unknown Apps contained in the
;		     Critical Event Table
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
;    HS8001	Upon cFE Processor Reset or HS Application Restart, HS preserves
;		the following:
;		  a. Number of cFE Processor Resets (commanded by HS)
;		  b. Maximum number of cFE Processor Resets
;    HS8002	Upon any initialization, HS shall subscribe to all event
;		messages.
;    HS8003	Upon any initialization, HS shall load the Critial Application
;		Table.
;    HS8003.1	If the Critial Application Table fails validation, HS shall
;		issue an event message an disable Critical Application
;		Monitoring.
;    HS8004	Upon any initialization, HS shall load the Critial Event Table.
;    HS8004.1	If the Critial Event Table fails validation, HS shall issue an 
;		event message an disable Critical Event Monitoring.
;    HS8005	Upon any initialization, HS shall load the Execution Counter
;		Table.
;    HS8005.1	If the Execution Counter Table fails validation, HS shall
;		  a. Issue an event message
;		  b. Report 0x'FFFFFFFF' for all <PLATFORM_DEFINED> items in
;		     the table
;    HS8006	Upon any initialization, HS shall wait until the cFE startup
;		synch has been received indicating all Applications have started
;    HS8006.1	If the startup synch is not received in <PLATFORM_DEFINED>
;		seconds, HS shall begin processing.
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

#define HS_1003		0
#define HS_7001		1
#define HS_7100		2
#define HS_8000		3
#define HS_8001		4
#define HS_8002		5
#define HS_8003		6
#define HS_80031	7
#define HS_8004		8
#define HS_80041	9
#define HS_8005		10
#define HS_80051	11
#define HS_8006		12
#define HS_80061	13

global ut_req_array_size = 13
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_1003","HS_7001","HS_7100","HS_8000","HS_8001","HS_8002","HS_8003","HS_8003.1","HS_8004","HS_8004.1","HS_8005","HS_8005.1","HS_8006","HS_8006.1" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, index
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
write ";  Step 1.5: Verify that the HS Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Set the HS HK packet ID and Critical Event Table appID based upon 
;; the cpu being used
;; CPU1 is the default
local hkPktId = "p0AD"
local amtAPID = "0F72"
local emtAPID = "0F73"
local xctAPID = "0F75"

if ("$CPU" = "CPU2") then
  hkPktId = "p1AD"
  amtAPID = "0F80"
  emtAPID = "0F81"
  xctAPID = "0F83"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2AD"
  amtAPID = "0F91"
  emtAPID = "0F92"
  xctAPID = "0F94"
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
  write "WatchdogFlag    = ", p@$SC_$CPU_TST_HS_WatchdogFlag
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
write ";  Step 1.7: Verify that the Critical Application, Critical Event and "
write ";  Execution Counter Tables are loaded and HS has subscribed to all "
write ";  events"
write ";***********************************************************************"
local index

;; Verify that HS has subscribed to all events
if ($SC_$CPU_HS_EVTMonCnt > 0) then
  write "<*> Passed (8002) - Events are being monitored."
  ut_setrequirements HS_8002, "P"
else
  write "<!> Failed (8002) - Events are not being monitored."
  ut_setrequirements HS_8002, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Enabled") then
  write "<*> Passed (8003) - Critical Application Table has been loaded."
  ut_setrequirements HS_8003, "P"
else
  ;; Dump the table 
  s get_tbl_to_cvt(ramDir,EvtMonTblName,"A","$cpu_hs_dumpamt",hostCPU,amtAPID)
  wait 5

  ;; If an entry contains an AppName, the table was loaded
  for index = 1 to HS_MAX_MONITORED_APPS do
    if ($SC_$CPU_HS_AMT[index].AppName <> "") then
      break
    endif
  enddo

  if (index < HS_MAX_MONITORED_APPS+1) then
    write "<*> Passed (8003) - Critical Application Table has been loaded."
    ut_setrequirements HS_8003, "P"
  else
    write "<!> Failed (8003) - Critical Application Table was not loaded."
    ut_setrequirements HS_8003, "F"
  endif
endif

;; Check the Event Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Enabled") then
  write "<*> Passed (8004) - Critical Event Table has been loaded."
  ut_setrequirements HS_8004, "P"
else
  ;; Dump the table 
  s get_tbl_to_cvt(ramDir, EvtMonTblName, "A", "$cpu_hs_dumpemt", hostCPU, emtAPID)
  wait 5

  ;; If an entry contains an AppName, the table was loaded
  for index = 1 to HS_MAX_MONITORED_EVENTS do
    if ($SC_$CPU_HS_EMT[index].AppName <> "") then
      break
    endif
  enddo

  if (index < HS_MAX_MONITORED_EVENTS+1) then
    write "<*> Passed (8004) - Critical Event Table has been loaded."
    ut_setrequirements HS_8004, "P"
  else
    write "<!> Failed (8004) - Critical Event Table was not loaded."
    ut_setrequirements HS_8004, "F"
  endif
endif

;; Check the Execution Counter Table
s get_tbl_to_cvt(ramDir,ExeCntTblName, "A", "$cpu_hs_dumpxct", hostCPU, xctAPID)
wait 5

;; If an entry contains a ResourceName, the table was loaded
for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  if ($SC_$CPU_HS_XCT[index].ResourceName <> "") then
    break
  endif
enddo

if (index < HS_MAX_EXEC_CNT_SLOTS+1) then
  write "<*> Passed (8005) - Critical Event Table has been loaded."
  ut_setrequirements HS_8005, "P"
else
  write "<!> Failed (8005) - Critical Event Table was not loaded."
  ut_setrequirements HS_8005, "F"
endif

write ";***********************************************************************"
write ";  Step 2.0: cFE Processor Reset Test "
write ";***********************************************************************"
write ";  Step 2.1: Send the TST_HS command to set the counters."
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

write ";*********************************************************************"
write ";  Step 2.2: Send the command to Set the Max cFE Resets to a different"
write ";  value from the default."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_SET_MAX_RESETS_DBG_EID, "DEBUG", 1

local cmdCtr = $SC_$CPU_HS_CMDPC + 1

;; Send the Command
/$SC_$CPU_HS_SetMaxResetCnt NewCount=25
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
write ";  Step 2.3: Save the Resets performed and Max Resets values in order "
write ";  to compare them with the values restored after the Reset."
write ";*********************************************************************"
local savedPRResets = $SC_$CPU_HS_PRResetCtr
local savedMaxResets = $SC_$CPU_HS_MaxResetCnt

write ";*********************************************************************"
write ";  Step 2.4: Perform a Processor Reset "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 2.5: Start the HS and TST_HS applications."
write ";*********************************************************************"
s $sc_$cpu_hs_start_apps("2.5")
wait 5

write ";***********************************************************************"
write ";  Step 2.6: Enable DEBUG Event Messages "
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
write ";  Step 2.7: Verify that the Procesor Reset counter and the Max Reset "
write ";  Count were restored. "
write ";***********************************************************************"

if ($SC_$CPU_HS_PRResetCtr = savedPRResets) AND ;;
   ($SC_$CPU_HS_MaxResetCnt = savedMaxResets) then
  write "<*> Passed (8001) - Counters were saved across a cFE Processor Reset"
  ut_setrequirements HS_8001, "P"
else
  write "<!> Failed (8001) - Counters were NOT restored after a cFE Processor Reset"
  ut_setrequirements HS_8001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.8: Verify the Critical Application, Critical Event, and  "
write ";  Execution Counter Tables were loaded and events were subscribed to."
write ";***********************************************************************"
;; Verify that HS has subscribed to all events
if ($SC_$CPU_HS_EVTMonCnt > 0) then
  write "<*> Passed (8002) - Events are being monitored."
  ut_setrequirements HS_8002, "P"
else
  write "<!> Failed (8002) - Events are not being monitored."
  ut_setrequirements HS_8002, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Enabled") then
  write "<*> Passed (8003) - Critical Application Table has been loaded."
  ut_setrequirements HS_8003, "P"
else
  ;; Dump the table 
  s get_tbl_to_cvt(ramDir,EvtMonTblName,"A","$cpu_hs_dumpamt",hostCPU,amtAPID)
  wait 5

  ;; If an entry contains an AppName, the table was loaded
  for index = 1 to HS_MAX_MONITORED_APPS do
    if ($SC_$CPU_HS_AMT[index].AppName <> "") then
      break
    endif
  enddo

  if (index < HS_MAX_MONITORED_APPS+1) then
    write "<*> Passed (8003) - Critical Application Table has been loaded."
    ut_setrequirements HS_8003, "P"
  else
    write "<!> Failed (8003) - Critical Application Table was not loaded."
    ut_setrequirements HS_8003, "F"
  endif
endif

;; Check the Event Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Enabled") then
  write "<*> Passed (8004) - Critical Event Table has been loaded."
  ut_setrequirements HS_8004, "P"
else
  ;; Dump the table 
  s get_tbl_to_cvt(ramDir, EvtMonTblName, "A", "$cpu_hs_dumpemt", hostCPU, emtAPID)
  wait 5

  ;; If an entry contains an AppName, the table was loaded
  for index = 1 to HS_MAX_MONITORED_EVENTS do
    if ($SC_$CPU_HS_EMT[index].AppName <> "") then
      break
    endif
  enddo

  if (index < HS_MAX_MONITORED_EVENTS+1) then
    write "<*> Passed (8004) - Critical Event Table has been loaded."
    ut_setrequirements HS_8004, "P"
  else
    write "<!> Failed (8004) - Critical Event Table was not loaded."
    ut_setrequirements HS_8004, "F"
  endif
endif

;; Check the Execution Counter Table
s get_tbl_to_cvt(ramDir, ExeCntTblName, "A", "$cpu_hs_dumpxct", hostCPU, xctAPID)
wait 5

;; If an entry contains a ResourceName, the table was loaded
for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  if ($SC_$CPU_HS_XCT[index].ResourceName <> "") then
    break
  endif
enddo

if (index < HS_MAX_EXEC_CNT_SLOTS+1) then
  write "<*> Passed (8005) - Execution Counter Table has been loaded."
  ut_setrequirements HS_8005, "P"
else
  write "<!> Failed (8005) - Execution Counter Table was not loaded."
  ut_setrequirements HS_8005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Application Reset Tests "
write ";*********************************************************************"
write ";  Step 3.1: Check the Resets Performed counter to ensure it is not set"
write ";  to the default value. If it is, set it to a different value. "
write ";*********************************************************************"
;; Check the PRResetCtr
if ($SC_$CPU_HS_PRResetCtr = 0) then
  ut_setupevents "$SC","$CPU","TST_HS",TST_HS_SET_RESETS_PERFORMED_INF_EID,"INFO", 1

  /$SC_$CPU_TST_HS_SetResetsPerformed

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Expected Event Msg ",TST_HS_SET_RESETS_PERFORMED_INF_EID," rcv'd."
  else
    write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_SET_RESETS_PERFORMED_INF_EID,"."
  endif
endif

write ";*********************************************************************"
write ";  Step 3.2: Set the Max Reset Count to a different value. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_SET_MAX_RESETS_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_HS_CMDPC + 1

;; Send the Command
/$SC_$CPU_HS_SetMaxResetCnt NewCount=15
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

write ";*********************************************************************"
write ";  Step 3.3: Save the data and Disable the Watchdog timer "
write ";*********************************************************************"
;; Save the values
savedPRResets = $SC_$CPU_HS_PRResetCtr
savedMaxResets = $SC_$CPU_HS_MaxResetCnt

;; Before stopping the HS application, disable the Watchdog timer
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_DISABLE_WATCHDOG_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_DisableWatchdog

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Stop the HS and TST_HS applications "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 2
                                                                               
/$SC_$CPU_ES_DELETEAPP Application="TST_HS"
wait 4
/$SC_$CPU_ES_DELETEAPP Application=HSAppName
wait 4
                                                                               
ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS and TST_HS stop app commands sent properly."
else
  write "<!> Failed - Stop App commands did not increment CMDPC."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Start the HS and TST_HS applications."
write ";*********************************************************************"
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
write ";  Step 3.7: Verify that the Procesor Reset counter and the Max Reset "
write ";  Count were restored. "
write ";***********************************************************************"

if ($SC_$CPU_HS_PRResetCtr = savedPRResets) AND ;;
   ($SC_$CPU_HS_MaxResetCnt = savedMaxResets) then
  write "<*> Passed (8001) - Counters were saved across a cFE Processor Reset"
  ut_setrequirements HS_8001, "P"
else
  write "<!> Failed (8001) - Counters were NOT restored after a cFE Processor Reset"
  ut_setrequirements HS_8001, "F"
endif

write ";***********************************************************************"
write ";  Step 3.8: Verify the Critical Application, Critical Event, and  "
write ";  Execution Counter Tables were loaded and events were subscribed to."
write ";***********************************************************************"
;; Verify that HS has subscribed to all events
if ($SC_$CPU_HS_EVTMonCnt > 0) then
  write "<*> Passed (8002) - Events are being monitored."
  ut_setrequirements HS_8002, "P"
else
  write "<!> Failed (8002) - Events are not being monitored."
  ut_setrequirements HS_8002, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Enabled") then
  write "<*> Passed (8003) - Critical Application Table has been loaded."
  ut_setrequirements HS_8003, "P"
else
  ;; Dump the table 
  s get_tbl_to_cvt(ramDir,EvtMonTblName,"A","$cpu_hs_dumpamt",hostCPU,amtAPID)
  wait 5

  ;; If an entry contains an AppName, the table was loaded
  for index = 1 to HS_MAX_MONITORED_APPS do
    if ($SC_$CPU_HS_AMT[index].AppName <> "") then
      break
    endif
  enddo

  if (index < HS_MAX_MONITORED_APPS+1) then
    write "<*> Passed (8003) - Critical Application Table has been loaded."
    ut_setrequirements HS_8003, "P"
  else
    write "<!> Failed (8003) - Critical Application Table was not loaded."
    ut_setrequirements HS_8003, "F"
  endif
endif

;; Check the Event Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Enabled") then
  write "<*> Passed (8004) - Critical Event Table has been loaded."
  ut_setrequirements HS_8004, "P"
else
  ;; Dump the table 
  s get_tbl_to_cvt(ramDir, EvtMonTblName, "A", "$cpu_hs_dumpemt", hostCPU, emtAPID)
  wait 5

  ;; If an entry contains an AppName, the table was loaded
  for index = 1 to HS_MAX_MONITORED_EVENTS do
    if ($SC_$CPU_HS_EMT[index].AppName <> "") then
      break
    endif
  enddo

  if (index < HS_MAX_MONITORED_EVENTS+1) then
    write "<*> Passed (8004) - Critical Event Table has been loaded."
    ut_setrequirements HS_8004, "P"
  else
    write "<!> Failed (8004) - Critical Event Table was not loaded."
    ut_setrequirements HS_8004, "F"
  endif
endif

;; Check the Execution Counter Table
s get_tbl_to_cvt(ramDir, ExeCntTblName, "A", "$cpu_hs_dumpxct", hostCPU, xctAPID)
wait 5

;; If an entry contains a ResourceName, the table was loaded
for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  if ($SC_$CPU_HS_XCT[index].ResourceName <> "") then
    break
  endif
enddo

if (index < HS_MAX_EXEC_CNT_SLOTS+1) then
  write "<*> Passed (8005) - Execution Counter Table has been loaded."
  ut_setrequirements HS_8005, "P"
else
  write "<!> Failed (8005) - Execution Counter Table was not loaded."
  ut_setrequirements HS_8005, "F"
endif

wait 5


write ";*********************************************************************"
write ";  Step 4.0: Anomaly Tests"
write ";*********************************************************************"
write ";  Step 4.1: Remove the default tables from the onboard processor."
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
write ";  Step 4.2: Perform a cFE Power-On Reset "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 4.3: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("4.3")
wait 5

write ";*********************************************************************"
write ";  Step 4.4: Verify the Load error events are generated and that the "
write ";  appropriate Monitoring is disabled. "
write ";*********************************************************************"
;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8003.1) - Critical Application Table failed to load as expected"
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Table load failure message not rcv'd"
  ut_setrequirements HS_80031, "F"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8004.1) - Critical Event Table failed to load as expected"
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Table load failure message not rcv'd"
  ut_setrequirements HS_80041, "F"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8005.1) - Execution Counter Table failed to load as expected"
  ut_setrequirements HS_80051, "P"
else
  write "<!> Failed (8005.1) - Execution Counter Table load failure message not rcv'd"
  ut_setrequirements HS_80051, "F"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Actions Table failed to load as expected"
else
  write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (8003.1) - Critical Application Monitoring State is DISABLED."
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Monitoring is not DISABLED"
  ut_setrequirements HS_80031, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (8004.1) - Critical Event Monitoring State is DISABLED."
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Monitoring is not DISABLED"
  ut_setrequirements HS_80041, "F"
endif

write ";*********************************************************************"
write ";  Step 4.5: Perform a Processor Reset "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 4.6: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("4.6")
wait 5

write ";***********************************************************************"
write ";  Step 4.7: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 4.8: Verify the Load error events are generated and that the "
write ";  appropriate Monitoring is disabled. "
write ";*********************************************************************"
;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8003.1) - Critical Application Table failed to load as expected"
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Table load failure message not rcv'd"
  ut_setrequirements HS_80031, "F"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8004.1) - Critical Event Table failed to load as expected"
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Table load failure message not rcv'd"
  ut_setrequirements HS_80041, "F"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8005.1) - Execution Counter Table failed to load as expected"
  ut_setrequirements HS_80051, "P"
else
  write "<!> Failed (8005.1) - Execution Counter Table load failure message not rcv'd"
  ut_setrequirements HS_80051, "F"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Actions Table failed to load as expected"
else
  write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (8003.1) - Critical Application Monitoring State is DISABLED."
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Monitoring is not DISABLED"
  ut_setrequirements HS_80031, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (8004.1) - Critical Event Monitoring State is DISABLED."
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Monitoring is not DISABLED"
  ut_setrequirements HS_80041, "F"
endif

write ";*********************************************************************"
write ";  Step 4.9: Send the TST_HS command to disable the Watchdog timer."
write ";*********************************************************************"
;; Before stopping the HS application, disable the Watchdog timer
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_DISABLE_WATCHDOG_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_DisableWatchdog

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.10: Stop the HS and TST_HS applications."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 2
                                                                               
/$SC_$CPU_ES_DELETEAPP Application="TST_HS"
wait 4
/$SC_$CPU_ES_DELETEAPP Application=HSAppName
wait 4
                                                                               
ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS and TST_HS stop app commands sent properly."
else
  write "<!> Failed - Stop App commands did not increment CMDPC."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.11: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("4.11")
wait 5

write ";***********************************************************************"
write ";  Step 4.12: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 4.13: Verify the Load error events are generated and that the "
write ";  appropriate Monitoring is disabled. "
write ";*********************************************************************"
;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8003.1) - Critical Application Table failed to load as expected"
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Table load failure message not rcv'd"
  ut_setrequirements HS_80031, "F"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8004.1) - Critical Event Table failed to load as expected"
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Table load failure message not rcv'd"
  ut_setrequirements HS_80041, "F"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8005.1) - Execution Counter Table failed to load as expected"
  ut_setrequirements HS_80051, "P"
else
  write "<!> Failed (8005.1) - Execution Counter Table load failure message not rcv'd"
  ut_setrequirements HS_80051, "F"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Actions Table failed to load as expected"
else
  write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (8003.1) - Critical Application Monitoring State is DISABLED."
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Monitoring is not DISABLED"
  ut_setrequirements HS_80031, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (8004.1) - Critical Event Monitoring State is DISABLED."
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Monitoring is not DISABLED"
  ut_setrequirements HS_80041, "F"
endif

write ";***********************************************************************"
write ";  Step 4.14: Create & load the invalid definition tables. "
write ";***********************************************************************"
;; Application Monitoring Table
s $sc_$cpu_hs_amt11

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_amt11",amtFileName,hostCPU,"P")
wait 10

;; Event Monitoring Table
s $sc_$cpu_hs_emt8

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_emt8",emtFileName,hostCPU,"P")
wait 10

;; Message Actions Table
s $sc_$cpu_hs_mat2

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_mat2",matFileName,hostCPU,"P")
wait 10

;; Execution Counter Table
s $sc_$cpu_hs_xct4

;; Upload the file created above to the default location
s ftp_file (defTblDir,"hs_def_xct4",xctFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 4.15: Perform a cFE Power-On Reset "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 4.16: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("4.16")
wait 5

write ";***********************************************************************"
write ";  Step 4.17: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 4.18: Verify the Load error events are generated and that the "
write ";  appropriate Monitoring is disabled. "
write ";*********************************************************************"
;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8003.1) - Critical Application Table failed to load as expected"
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Table load failure message not rcv'd"
  ut_setrequirements HS_80031, "F"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8004.1) - Critical Event Table failed to load as expected"
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Table load failure message not rcv'd"
  ut_setrequirements HS_80041, "F"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8005.1) - Execution Counter Table failed to load as expected"
  ut_setrequirements HS_80051, "P"
else
  write "<!> Failed (8005.1) - Execution Counter Table load failure message not rcv'd"
  ut_setrequirements HS_80051, "F"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Actions Table failed to load as expected"
else
  write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (8003.1) - Critical Application Monitoring State is DISABLED."
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Monitoring is not DISABLED"
  ut_setrequirements HS_80031, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (8004.1) - Critical Event Monitoring State is DISABLED."
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Monitoring is not DISABLED"
  ut_setrequirements HS_80041, "F"
endif

write ";*********************************************************************"
write ";  Step 4.19: Perform a Processor Reset "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 4.20: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("4.20")
wait 5

write ";***********************************************************************"
write ";  Step 4.21: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 4.22: Verify the Load error events are generated and that the "
write ";  appropriate Monitoring is disabled. "
write ";*********************************************************************"
;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8003.1) - Critical Application Table failed to load as expected"
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Table load failure message not rcv'd"
  ut_setrequirements HS_80031, "F"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8004.1) - Critical Event Table failed to load as expected"
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Table load failure message not rcv'd"
  ut_setrequirements HS_80041, "F"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8005.1) - Execution Counter Table failed to load as expected"
  ut_setrequirements HS_80051, "P"
else
  write "<!> Failed (8005.1) - Execution Counter Table load failure message not rcv'd"
  ut_setrequirements HS_80051, "F"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Actions Table failed to load as expected"
else
  write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (8003.1) - Critical Application Monitoring State is DISABLED."
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Monitoring is not DISABLED"
  ut_setrequirements HS_80031, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (8004.1) - Critical Event Monitoring State is DISABLED."
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Monitoring is not DISABLED"
  ut_setrequirements HS_80041, "F"
endif

write ";*********************************************************************"
write ";  Step 4.23: Send the TST_HS command to disable the Watchdog timer."
write ";*********************************************************************"
;; Before stopping the HS application, disable the Watchdog timer
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_DISABLE_WATCHDOG_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_DisableWatchdog

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.24: Stop the HS and TST_HS applications."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 2
                                                                               
/$SC_$CPU_ES_DELETEAPP Application="TST_HS"
wait 4
/$SC_$CPU_ES_DELETEAPP Application=HSAppName
wait 4
                                                                               
ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS and TST_HS stop app commands sent properly."
else
  write "<!> Failed - Stop App commands did not increment CMDPC."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.25: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("4.25")
wait 5

write ";***********************************************************************"
write ";  Step 4.26: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 4.27: Verify the Validation error events are generated and that"
write ";  the appropriate Monitoring is disabled. "
write ";*********************************************************************"
;; Verify that the Load Error message for the AMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8003.1) - Critical Application Table failed to load as expected"
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Table load failure message not rcv'd"
  ut_setrequirements HS_80031, "F"
endif

;; Verify that the Load Error message for the EMT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8004.1) - Critical Event Table failed to load as expected"
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Table load failure message not rcv'd"
  ut_setrequirements HS_80041, "F"
endif

;; Verify that the Load Error message for the XCT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8005.1) - Execution Counter Table failed to load as expected"
  ut_setrequirements HS_80051, "P"
else
  write "<!> Failed (8005.1) - Execution Counter Table load failure message not rcv'd"
  ut_setrequirements HS_80051, "F"
endif

;; Verify that the Load Error message for the MAT was rcv'd 
ut_tlmwait $sc_$cpu_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Actions Table failed to load as expected"
else
  write "<!> Failed - Message Actions Table load failure message not rcv'd"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_AppMonState = "Disabled") then
  write "<*> Passed (8003.1) - Critical Application Monitoring State is DISABLED."
  ut_setrequirements HS_80031, "P"
else
  write "<!> Failed (8003.1) - Critical Application Monitoring is not DISABLED"
  ut_setrequirements HS_80031, "F"
endif

;; Check the Application Monitoring State
if (p@$SC_$CPU_HS_EVTMonState = "Disabled") then
  write "<*> Passed (8004.1) - Critical Event Monitoring State is DISABLED."
  ut_setrequirements HS_80041, "P"
else
  write "<!> Failed (8004.1) - Critical Event Monitoring is not DISABLED"
  ut_setrequirements HS_80041, "F"
endif

write ";*********************************************************************"
write ";  Step 5.0: Startup Synch Tests "
write ";*********************************************************************"
write ";  Step 5.1: Download the current startup script. "
write ";*********************************************************************"
s ftp_file (defTblDir,"cfe_es_startup.scr","hs_step51_startup.scr",hostCPU,"G")
wait 10

write ";*********************************************************************"
write ";  Step 5.2: Upload a new startup script that contains HS and TST_HS"
write ";*********************************************************************"
s ftp_file (defTblDir,"hs_step52_startup.scr","cfe_es_startup.scr",hostCPU,"P")

;; Upload the apps
s load_app(defTblDir,HSAppName,hostCPU)
wait 5
s load_app(defTblDir,"TST_HS",hostCPU)
wait 5

write ";*********************************************************************"
write ";  Step 5.3: Send the TST_HS command to delay at startup for longer "
write ";  than HS waits for the startup sync. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TST_HS_CMDPC + 1

/$SC_$CPU_TST_HS_SetStartupDelay
wait 5

ut_tlmwait  $SC_$CPU_TST_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS Set Startup Delay Flag command sent properly."
else
  write "<!> Failed - TST_HS Set Startup Delay Flag command. CMDPC = ",$SC_$CPU_TST_HS_CMDPC, "; expected ",cmdCtr
endif

wait 5

write ";***********************************************************************"
write ";  Step 5.4: Upload the default definition tables. "
write ";***********************************************************************"
;; Application Monitoring Table
s ftp_file (defTblDir,"hs_def_amt1",amtFileName,hostCPU,"P")
wait 10

;; Event Monitoring Table
s ftp_file (defTblDir,"hs_def_emt1",emtFileName,hostCPU,"P")
wait 10

;; Message Actions Table
s ftp_file (defTblDir,"hs_def_mat1",matFileName,hostCPU,"P")
wait 10

;; Execution Counter Table
s ftp_file (defTblDir,"hs_def_xct1",xctFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 5.5: Perform a Procesor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";***********************************************************************"
write ";  Step 5.6: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 5.7: Add the Housekeeping subscriptions for HS and TST_HS. "
write ";*********************************************************************"
;; CPU1 is the default
local hs_stream = x'08AD'
local tst_hs_stream = x'093C'

if ("$CPU" = "CPU2") then
  hs_stream = x'09AD'
  tst_hs_stream = x'0A3C'
elseif ("$CPU" = "CPU3") then
  hs_stream = x'0AAD'
  tst_hs_stream = x'0B3C'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=hs_stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5

/$SC_$CPU_TO_ADDPACKET STREAM=tst_hs_stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5

write ";*********************************************************************"
write ";  Step 5.8: Verify that HS starts processing before TST_HS starts. "
write ";  If you get the CPU aliveness indicator before the Housekeeping "
write ";  Request from TST_HS, HS timed-out before receiving the startup sync."
write ";*********************************************************************"
;; Wait for the HS Timeout to expire
local hsTimeout = HS_STARTUP_SYNC_TIMEOUT / 1000
wait hsTimeout
ut_setrequirements HS_80061, "A"

write ";*********************************************************************"
write ";  Step 5.9: Disable the Watchdog timer "
write ";*********************************************************************"
;; Before stopping the HS application, disable the Watchdog timer
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_DISABLE_WATCHDOG_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_DisableWatchdog

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.10: Stop the HS and TST_HS applications "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 2
                                                                               
/$SC_$CPU_ES_DELETEAPP Application="TST_HS"
wait 4
/$SC_$CPU_ES_DELETEAPP Application=HSAppName
wait 4
                                                                               
ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS and TST_HS stop app commands sent properly."
else
  write "<!> Failed - Stop App commands did not increment CMDPC."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.11: Start the HS and TST_HS applications."
write ";*********************************************************************"
s $sc_$cpu_hs_start_apps("5.11")
wait 5

write ";***********************************************************************"
write ";  Step 5.12: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 5.13: Verify that HS starts processing before TST_HS starts. "
write ";  If you get the CPU aliveness indicator before the Housekeeping "
write ";  Request from TST_HS, HS timed-out before receiving the startup sync."
write ";*********************************************************************"
wait hsTimeout

write ";*********************************************************************"
write ";  Step 5.14: Upload a new startup script that only contains HS "
write ";*********************************************************************"
s ftp_file (defTblDir,"hs_step514_startup.scr","cfe_es_startup.scr",hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 5.15: Send the TST_HS command to remove the delay at startup. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TST_HS_CMDPC + 1

/$SC_$CPU_TST_HS_RemoveStartupDelay
wait 5

ut_tlmwait  $SC_$CPU_TST_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS Startup Delay Removed command sent properly."
else
  write "<!> Failed - TST_HS Remove Startup Delay command. CMDPC = ",$SC_$CPU_TST_HS_CMDPC, "; expected ",cmdCtr
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.16: Perform a Processor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";***********************************************************************"
write ";  Step 5.17: Startup the TST_HS application "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_INIT_INF_EID, "INFO", 2
                                                                                
s load_start_app ("TST_HS",hostCPU,"TST_HS_AppMain")
                                                                                
; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - TST_HS Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_HS not received."
    write "Event Message count = ",$SC_$CPU_num_found_messages
  endif
else
  write "<!> Failed - TST_HS Application start Event Message not received."
endif

/$SC_$CPU_TO_ADDPACKET STREAM=tst_hs_stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5

write ";***********************************************************************"
write ";  Step 5.18: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 5.19: Verify that HS starts processing after receiving the "
write ";  startup synch. "
write ";*********************************************************************"
wait 20
ut_setrequirements HS_8006, "A"

write ";*********************************************************************"
write ";  Step 5.20: Disable the Watchdog timer "
write ";*********************************************************************"
;; Before stopping the HS application, disable the Watchdog timer
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_DISABLE_WATCHDOG_INF_EID, "INFO", 1

/$SC_$CPU_TST_HS_DisableWatchdog

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_DISABLE_WATCHDOG_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.21: Stop the HS and TST_HS applications "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 2
                                                                               
/$SC_$CPU_ES_DELETEAPP Application="TST_HS"
wait 4
/$SC_$CPU_ES_DELETEAPP Application=HSAppName
wait 4
                                                                               
ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS and TST_HS stop app commands sent properly."
else
  write "<!> Failed - Stop App commands did not increment CMDPC."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.22: Start the HS and TST_HS applications."
write ";*********************************************************************"
s $sc_$cpu_hs_start_apps("5.22")
wait 5

write ";***********************************************************************"
write ";  Step 5.23: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 5.24: Verify that HS starts processing after receiving the "
write ";  startup synch. "
write ";*********************************************************************"
wait 20

write ";*********************************************************************"
write ";  Step 5.25: Upload a new startup script that only contains HS "
write ";*********************************************************************"
s ftp_file (defTblDir,"hs_step525_startup.scr","cfe_es_startup.scr",hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 5.26: Perform a Processor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

;;cfe_startup $CPU
cfe_startup {hostCPU}
wait 5

write ";***********************************************************************"
write ";  Step 5.27: Startup the TST_HS application "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_INIT_INF_EID, "INFO", 2
                                                                                
s load_start_app ("TST_HS",hostCPU,"TST_HS_AppMain")
                                                                                
; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - TST_HS Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_HS not received."
    write "Event Message count = ",$SC_$CPU_num_found_messages
  endif
else
  write "<!> Failed - TST_HS Application start Event Message not received."
endif

/$SC_$CPU_TO_ADDPACKET STREAM=hs_stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5
/$SC_$CPU_TO_ADDPACKET STREAM=tst_hs_stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5

write ";***********************************************************************"
write ";  Step 5.28: Enable DEBUG Event Messages "
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

write ";*********************************************************************"
write ";  Step 5.29: Verify that HS starts processing after receiving the "
write ";  startup synch. "
write ";*********************************************************************"
wait 20

write ";*********************************************************************"
write ";  Step 6.0: Clean-up - Send the Power-On Reset command.             "
write ";*********************************************************************"
write ";  Step 6.1: Upload the original startup script downloaded in Step 5.1"
write ";*********************************************************************"
s ftp_file (defTblDir,"hs_step51_startup.scr","cfe_es_startup.scr",hostCPU,"P")
wait 10

;; Remove the hs & tst_hs object files
s ftp_file (defTblDir,"na","hs.o",hostCPU,"R")
wait 5
s ftp_file (defTblDir,"na","tst_hs.o",hostCPU,"R")
wait 5

write ";*********************************************************************"
write ";  Step 6.2: Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_hs_reset "
write ";*********************************************************************"
ENDPROC
