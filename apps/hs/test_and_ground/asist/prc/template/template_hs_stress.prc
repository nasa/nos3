PROC $sc_$cpu_hs_stress
;*******************************************************************************
;  Test Name:  hs_stress
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) Application
;	handles the maximum <PLATFORM_DEFINE> entries in the Critical
;	Application, Event and Execution Counter Tables.
;
;  Requirements Tested
;    HS2000.2.3	If the action is to perform an Application Restart, HS shall
;               disable the entry in the Critical Application Table.
;    HS2003	HS shall support up to <PLATFORM_DEFINED> critical applications.
;    HS3000	The HS Application shall maintain the Execution Counters defined
;		in the Execution Counter Table for up to <PLATFORM_DEFINED>
;		number of items.
;    HS5003	HS shall support up to <PLATFORM_DEFINED> critical events.
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
;		  l. Peak CPU Utilization
;		  m. Average CPU utilization
;		  n. CPU Utilization Monitoring Enabled/Disabled
;    HS8000	Upon cFE Power On Reset, HS shall initialize the following data
;		to Zero (or the value specified for the item below):
;		a. Valid Ground Command Counter
;		b. Ground Command Rejected Counter
;		c. Monitor Critical Application - <PLATFORM_DEFINED>
;		d. Critical Application Monitoring status per
;		   Application enabled
;		e. Monitor Critical Events - <PLATFORM_DEFINED>
;		f. CPU Aliveness Indicator - <PLATFORM_DEFINED>
;		g. Watchdog Time Flag - TRUE
;		h. Set the Watchdog Timer to <PLATFORM_DEFINED> value
;		i. Maximum number of cFE Processor Resets - 
;		   <PLATFORM_DEFINED> value
;		j. Number of cFE Processor resets (commanded by HS)
;		k. Number of invalid/Unknown Apps contained in the
;		   Critical Event table
;		l. Peak CPU Utilization
;		m. Average CPU utilization
;		n. CPU Utilization Monitoring Enabled/Disabled - 
;		   <PLATFORM_DEFINED> value.
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
;	07/13/09	Walt Moleski	Original Procedure.
;	01/12/11	Walt Moleski	Updated for HS 2.1.0.0
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
#include "to_lab_events.h"
#include "hs_platform_cfg.h"
#include "hs_tbldefs.h"
#include "hs_msgdefs.h"
#include "hs_events.h"
#include "tst_hs_events.h"

%liv (log_procedure) = logging

#define HS_200023	0
#define HS_2003		1
#define HS_3000		2
#define HS_5003		3
#define HS_7100		4
#define HS_8000		5

global ut_req_array_size = 5
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_2000.2.3","HS_2003","HS_3000","HS_5003","HS_7100","HS_8000" ]

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
page $SC_$CPU_TST_HS_HK
page $SC_$CPU_HS_APPMON_TABLE
page $SC_$CPU_HS_EVTMON_TABLE
page $SC_$CPU_HS_EXECNT_TABLE

write ";***********************************************************************"
write ";  Step 1.3: Create & load the default definition tables. "
write ";***********************************************************************"
;; Application Monitoring Table
s $sc_$cpu_hs_amt12

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
s ftp_file (defTblDir,"hs_def_amt12",amtFileName,hostCPU,"P")
wait 10

;; Event Monitoring Table
s $sc_$cpu_hs_emt6

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
s ftp_file (defTblDir,"hs_def_emt6",emtFileName,hostCPU,"P")
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
s $sc_$cpu_hs_xct3

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
s ftp_file (defTblDir,"hs_def_xct3",xctFileName,hostCPU,"P")
wait 10

write ";***********************************************************************"
write ";  Step 1.4:  Start the Applications that are not started automatically."
write ";***********************************************************************"
;; Start the cFE Test Applications
s load_start_app ("TST_EVS",hostCPU)
wait 5
s load_start_app ("TST_TBL",hostCPU)
wait 5
s load_start_app ("TST_TBL2",hostCPU)
wait 5
s load_start_app ("TST_ES2",hostCPU)
wait 5
s load_start_app ("TST_ES3",hostCPU)
wait 5
s load_start_app ("TST_ES4",hostCPU)
wait 5
s load_start_app ("TST_ES5",hostCPU)
wait 5
s load_start_app ("TST_ES6",hostCPU)
wait 5
s load_start_app ("TST_ES7",hostCPU)
wait 5
s load_start_app ("TST_ES8",hostCPU)
wait 5
s load_start_app ("TST_ES9",hostCPU)
wait 5
s load_start_app ("TST_ES10",hostCPU)
wait 5
s load_start_app ("TST_ES11",hostCPU)
wait 5
s load_start_app ("TST_ES12",hostCPU)
wait 5
s load_start_app ("TST_ES13",hostCPU)
wait 5
s load_start_app ("TST_ES14",hostCPU)
wait 5
s load_start_app ("TST_ES15",hostCPU)
wait 5
s load_start_app ("TST_ES16",hostCPU)
wait 5
s load_start_app ("TST_ES17",hostCPU)
wait 5
s load_start_app ("TST_ES18",hostCPU)
wait 5
s load_start_app ("TST_ES19",hostCPU)
wait 5
s load_start_app ("TST_ES20",hostCPU)
wait 5

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_RESTART_APP_INF_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", "TST_ES2", 1, "INFO", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_APPMON_RESTART_ERR_EID, "ERROR", 5

;; Start the Health and Safety Applications
s $sc_$cpu_hs_start_apps("1.4")
wait 5

;; Verify that TST_ES2 was restarted as indicated in the table
if ($sc_$cpu_find_event[5].num_found_messages = 1) AND ;;
   ($sc_$cpu_find_event[3].num_found_messages = 1) AND ;;
   ($sc_$cpu_find_event[4].num_found_messages = 1) then
    write "<*> Passed - TST_ES2 Application was restarted and the proper event messages were rcv'd."
else
    write "<!> Failed - Did not rcv the proper messages for the Critical Application Restart Error."
    write "--Event 3 messages = ",$sc_$cpu_find_event[3].num_found_messages
    write "--Event 4 messages = ",$sc_$cpu_find_event[4].num_found_messages
    write "--Event 5 messages = ",$sc_$cpu_find_event[5].num_found_messages
endif

write ";***********************************************************************"
write ";  Step 1.5: Verify that the HS Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Set the HS HK packet ID based upon the cpu being used
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
  ut_setrequirements HS_8000, "F"
endif

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
write ";  Step 2.0: Table Entry Verification Tests "
write ";***********************************************************************"
write ";  Step 2.1: Verify that the Application Monitoring Table is full"
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir, AppMonTblName, "A", "$cpu_hs_dumpamt", hostCPU, amtAPID)
wait 5

local index
local es2Slot=0
;; Check that all the application slots are filled
for index = 1 to HS_MAX_MONITORED_APPS do
  if ($SC_$CPU_HS_AMT[index].AppName = "") then
    break
  elseif ($SC_$CPU_HS_AMT[index].AppName = "TST_ES2") then
    es2Slot = index
  endif
enddo

;; If all the table entries were filled, index will = HS_MAX_MONITORED_APPS
if (index = HS_MAX_MONITORED_APPS+1) then
  write "<*> Passed (2003) - HS supported the maximum defined Critical Applications"
  ut_setrequirements HS_2003, "P"
else
  write "<*> Failed (2003) - The Critical Application Table did not contain the maximum applications"
  write "Index of failure = ", index
  ut_setrequirements HS_2003, "F"
endif

;; Verify that the Monitored Application Status indicates TST_ES2 is disabled
local tst_es2Mask = 2 ** es2Slot
local tst_es2Status = %and($SC_$CPU_HS_APPSTATUS,tst_es2Mask)
write "HS App Status  = ", %hex($SC_$CPU_HS_APPSTATUS)
write "TST_ES2 Mask   = ", tst_es2Mask
write "TST_ES2 Status = ", tst_es2Status
if (tst_es2Status = 0) then
  write "<*> Passed (2000.2.3) - Application Status in HK indicates TST_ES2 is disabled."
  ut_setrequirements HS_200023, "P"
else
  write "<*> Failed (2000.2.3) - HK Application Status is not correct after the application restart."
  ut_setrequirements HS_200023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Verify that the Event Monitoring Table is full"
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir, EvtMonTblName, "A", "$cpu_hs_dumpemt", hostCPU, emtAPID)
wait 5

;; Check that all the application slots are filled
for index = 1 to HS_MAX_MONITORED_EVENTS do
  if ($SC_$CPU_HS_EMT[index].AppName = "") then
    break
  endif
enddo

;; If all the table entries were filled, index will = HS_MAX_MONITORED_EVENTS + 1
if (index = HS_MAX_MONITORED_EVENTS+1) then
  write "<*> Passed (5003) - HS supported the maximum defined Critical Events"
  ut_setrequirements HS_5003, "P"
else
  write "<*> Failed (5003) - The Critical Event Table did not contain the maximum entries"
  write "Index of failure = ", index
  ut_setrequirements HS_5003, 
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Verify that the Execution Counter Table is full"
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir, ExeCntTblName, "A", "$cpu_hs_dumpxct", hostCPU, xctAPID)
wait 5

local index
;; For each entry in the table that contains a non-null ResourceName
;; Check that the execution counter is not set to 0xFFFFFFFF
for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  if ($SC_$CPU_HS_XCT[index].ResourceName = "") then
    break
  endif
enddo

if (index = HS_MAX_EXEC_CNT_SLOTS+1) then
  write "<*> Passed (3000) - The Execution Counter Table supports the maximum number of entries."
  ut_setrequirements HS_3000, "P"
else
  write "<!> Failed (3000) - The Execution Counter Table did not support the maximum number of entries."
  write "Index of failure = ",index
  ut_setrequirements HS_3000, "F"
endif

wait 10

write ";*********************************************************************"
write ";  Step 3.0: Clean-up - Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_hs_stress"
write ";*********************************************************************"
ENDPROC
