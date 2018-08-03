PROC $sc_$cpu_hs_cpuhog
;*******************************************************************************
;  Test Name:  hs_cpuhog
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) application
;	performs the CPU Utilization function according to its requirements.
;
;  Requirements Tested
;    HS1003     If HS accepts any command as valid, HS shall execute the
;               command, increment the HS Valid Command Counter and issue an
;               event message.
;    HS6008	HS shall report peak CPU utilization during an interval of
;		<PLATFORM_DEFINED> HS execution cycles over the last
;		<PLATFORM_DEFINED> intervals.
;    HS6009	HS shall report average CPU utilization over the last
;		<PLATFORM_DEFINED> intervals of <PLATFORM_DEFINED> HS execution
;		cycles each.
;    HS6010	If the CPU Utilization exceeds <PLATFORM_DEFINED> % for
;		<PLATFORM_DEFINED> intervals and CPU Utilization monitoring is
;		enabled then HS shall issue an event message.
;    HS6011     Upon receipt of an Enable CPU Utilization Monitoring, HS shall
;               begin monitoring CPU utilization.
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
;	01/19/11	Walt Moleski	Original Procedure.
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
#include "cfe_evs_events.h"
#include "hs_platform_cfg.h"
#include "hs_tbldefs.h"
#include "hs_msgdefs.h"
#include "hs_events.h"
#include "tst_hs_events.h"

%liv (log_procedure) = logging

#define HS_1003		0
#define HS_6008		1
#define HS_6009		2
#define HS_6010		3
#define HS_6011		4
#define HS_7100		5
#define HS_8000		6

global ut_req_array_size = 6
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_1003","HS_6008","HS_6009","HS_6010","HS_6011","HS_7100","HS_8000" ]

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
   ($SC_$CPU_HS_CPUHOGState = HS_CPUHOG_DEFAULT_STATE) AND ;;
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
write ";  Step 2.0: CPU Hogging Test."
write ";***********************************************************************"
write ";  Step 2.1: Enable CPU Utilization "
write ";***********************************************************************"
;; Send the command to Enable CPU Utilization
ut_setupevents "$SC","$CPU",{HSAppName},HS_ENABLE_CPUHOG_DBG_EID,"DEBUG", 1

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

if (p@$SC_$CPU_HS_CPUHogState = "Enabled") then
  write "<*> Passed - CPU Utilization State set to 'Enabled'."
else
  write "<!> Failed - CPU Utilization State does not indicate 'Enabled'."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the TST_HS commands to hog the cpu."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_HOG_CPU_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_CPUMON_HOGGING_ERR_EID, "ERROR", 3

cmdCtr = $SC_$CPU_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_HS_SetUtilMask UtilMask=x'3F'

ut_tlmwait $SC_$CPU_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - HS Set Util Mask command sent properly."
else
  write "<!> Failed - HS Set Util Mask command did not increment CMDPC."
endif

;; Check for the command event message
;;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
;;if (UT_TW_Status = UT_Success) then
;;  write "<*> Passed - Expected Event Msg ",TST_HS_SET_UTIL_MASK_INF_EID," rcv'd."
;;else
;;  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_SET_UTIL_MASK_INF_EID,"."
;;endif

;; Send the CPU Hog command
cmdCtr = $SC_$CPU_TST_HS_CMDPC + 1
;; Send the Command
/$SC_$CPU_TST_HS_HOGCPU HogCount=100000

ut_tlmwait $SC_$CPU_TST_HS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_HS Hog CPU command sent properly."
else
  write "<!> Failed - TST_HS Hog CPU command did not increment CMDPC."
endif

;; Check for the command event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_HS_HOG_CPU_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_HS_HOG_CPU_INF_EID,"."
endif

;; Check if the Average and Peak Utilization are > 0
if ($SC_$CPU_HS_CPUUtilAve > 0) then
  write "<*> Passed (6009) - Average CPU Utilization reported."
  ut_setrequirements HS_6009, "P"
else
  write "<!> Failed (6009) - Average CPU Utilization was not reported."
  ut_setrequirements HS_6009, "F"
endif

if ($SC_$CPU_HS_CPUUtilPeak > 0) then
  write "<*> Passed (6008) - Peak CPU Utilization reported."
  ut_setrequirements HS_6008, "P"
else
  write "<!> Failed (6008) - Peak CPU Utilization was not reported."
  ut_setrequirements HS_6008, "F"
endif

;; Check for the HS Error event message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6010) - Expected CPU Hogging Error Event Msg ",HS_CPUMON_HOGGING_ERR_EID," rcv'd."
  ut_setrequirements HS_6010, "P"
else
  write "<!> Failed (6010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",HS_CPUMON_HOGGING_ERR_EID,"."
  ut_setrequirements HS_6010, "F"
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
write ";  End procedure $SC_$CPU_hs_cpuhog"
write ";*********************************************************************"
ENDPROC
