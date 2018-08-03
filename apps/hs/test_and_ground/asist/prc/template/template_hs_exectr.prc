PROC $sc_$cpu_hs_exectr
;*******************************************************************************
;  Test Name:  hs_exectr
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Health and Safety (HS) application
;	handles the Execution Counter Management requirements properly.
;
;	NOTE: If the Mission decides not to use Execution Counters, this test
;	      can be skipped
;
;  Requirements Tested
;    HS3000	The HS Application shall maintain the Execution Counters defined
;		in the Execution Counter Table for up to <PLATFORM_DEFINED>
;		number of items.
;    HS3000.1	If the item contained in the Execution Counter Table is unknown,
;		HS shall
;		   a. Set the Execution Counter Values for that entry to
;		      0x'FFFFFFFF'
;    HS3001	Upon receipt of an Execution Counter Table update indication,
;		HS shall validate the Execution Counter Table.
;    HS3001.1	If the Execution Counter table fails validation, HS shall:
;		   a. Issue an event message
;    HS7100	HS shall generate a housekeeping message containing the
;		following:
;		   a. Valid Ground Command Counter
;		   b. Ground Command Rejected Counter
;		   c. Critical Application Monitoring Status (enable/disable)
;		   d. Critical Application Monitoring Status per table
;		      entry (enable/disable)
;		   e. Number of cFE Processor Resets (commanded by HS)
;		   f. maximum number of cFE Processor resets
;		   g. Critical Event Monitoring status (enable/disable)
;		   h. Count of Monitored Event Messages
;		   i. CPU Aliveness indicator (enable/disable)
;		   j. Execution Counter, for each table entry
;		   k. Number of Invalid/Unknown Apps contained in the
;		      Critical Event Table
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
;	06/24/09	Walt Moleski	Original Procedure.
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
#include "hs_platform_cfg.h"
#include "hs_tbldefs.h"
#include "hs_msgdefs.h"
#include "hs_events.h"
#include "tst_hs_events.h"

%liv (log_procedure) = logging

#define HS_3000		0
#define HS_30001	1
#define HS_3001		2
#define HS_30011	3
#define HS_7100		4
#define HS_8000		5

global ut_req_array_size = 5
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HS_3000","HS_3000.1","HS_3001","HS_3001.1","HS_7100","HS_8000" ]

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
page $SC_$CPU_HS_EXECNT_TABLE
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
;; Set the HS HK packet ID and Critical Event Table appID based upon 
;; the cpu being used
;; CPU1 is the default
local hkPktId = "p0AD"
local xctAPID = "0F75"

if ("$CPU" = "CPU2") then
  hkPktId = "p1AD"
  xctAPID = "0F83"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2AD"
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
write ";  Step 1.7: Dump the Execution Counter Table"
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir,ExeCntTblName,"A","$cpu_hs_dumpxct",hostCPU,xctAPID)
wait 5

local index
;; For each entry in the table that contains a non-null ResourceName
;; Check that the execution counter is not set to 0xFFFFFFFF
for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  if ($SC_$CPU_HS_XCT[index].ResourceName = "") then
    break
  else
    if ($SC_$CPU_HS_ExecutionCtr[index] > 0) AND ;;
       (%hex($SC_$CPU_HS_ExecutionCtr[index]) < %hex(x'FFFFFFFF')) then
      write "<*> Passed (7100) - Execution Counter for '",$SC_$CPU_HS_XCT[index].ResourceName, "' = ",$SC_$CPU_HS_ExecutionCtr[index]
    else
      write "<!> Failed (7100) - Execution counter for entry ",index," is not within the proper range. Actual value = ",$SC_$CPU_HS_ExecutionCtr[index] 
    endif
  endif
enddo

write ";***********************************************************************"
write ";  Step 2.0: Anomaly Tests"
write ";***********************************************************************"
write ";  Step 2.1: Create and load a new Execution counter Table that contains"
write ";  several entries with applications that will never execute."
write ";*********************************************************************"
s $sc_$cpu_hs_xct2

local cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Critical Event Table
s load_table("hs_def_xct2",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Ececution Counter Table sent successfully."
else
  write "<!> Failed - Load command for Ececution Counter Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 2.2: Validate the Execution Counter Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Execution Counter Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ExeCntTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Execution Counter Table sent successfully."
else
  write "<!> Failed - Validate command for the Execution Counter Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3001) - Execution Counter Table Validation successful message rcv'd"
    ut_setrequirements HS_3001, "P"
  else
    write "<!> Failed (3001) - Did not rcv Execution Counter Table Validation successful message"
    ut_setrequirements HS_3001, "F"
  endif
else
  write "<!> Failed (3001) - Execution Counter Table Failed Validation"
  ut_setrequirements HS_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.3: Activate the Execution Counter Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ExeCntTblName

;; Wait for the event monitor table update message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_3001, "P"
else
  write "<*> Failed (3001) - Did not rcv the Table Update Success message for the Execution Counter Table"
  ut_setrequirements HS_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4: Verify that the entries containing the applications that "
write ";  did not execute contain the 0xFFFFFFFF value in the HK parameter."
write ";*********************************************************************"
;; The applications in slots 6 and 7 are the ones that did not execute
;; Therefore, check those Execution Counters in housekeeping
if ($SC_$CPU_HS_ExecutionCtr[6] = 0xFFFFFFFF) AND ;;
   ($SC_$CPU_HS_ExecutionCtr[7] = 0xFFFFFFFF) then
  write "<*> Passed (3000.1) - Execution Counters for items 6 and 7 were set properly."
  ut_setrequirements HS_30001, "P"
else
  write "<*> Failed (3000.1) - One or both of the Execution Counters for items 6 and 7 were not set properly."
  ut_setrequirements HS_30001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5: Remove the default tables from the onboard processor."
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
write ";  Step 2.6: Stop the HS and TST_HS applications by performing a cFE "
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
write ";  Step 2.7: Start the HS and TST_HS applications."
write ";*********************************************************************"
;; Starting events at 3 since the hs_start_apps proc uses slots 1 and 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_AMT_LD_ERR_EID, "ERROR", 3
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_EMT_LD_ERR_EID, "ERROR", 4
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCT_LD_ERR_EID, "ERROR", 5
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_MAT_LD_ERR_EID, "ERROR", 6

s $sc_$cpu_hs_start_apps("2.7")
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

;; Verify that all the Execution counters reported in 
;; Housekeeping are set to 0xFFFFFFFF
for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  if ($SC_$CPU_HS_ExecutionCtr[index] <> 0xFFFFFFFF) then
    break
  endif
enddo

if (index = HS_MAX_EXEC_CNT_SLOTS+1) then
  write "<*> Passed (3000.1) - Execution Counters are set as expected"
  ut_setrequirements HS_30001, "P"
else
  write "<!> Failed (3000.1) - The Execution Counters reported in housekeeping are NOT set as expected"
  write "Index of failure = ",index
  ut_setrequirements HS_30001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.8: Enable DEBUG Event Messages "
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
write ";  Step 2.9: Create and load a new Execution counter Table that contains"
write ";  invalid data."
write ";*********************************************************************"
s $sc_$cpu_hs_xct4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Execution Counter Table
s load_table("hs_def_xct4",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Execution Counter Table sent successfully."
else
  write "<!> Failed - Load command for Execution Counter Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 2.10: Validate the Execution Counter Table loaded above. "
write ";  This should fail validation. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCTVAL_ERR_EID, "ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Execution Counter Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ExeCntTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Execution Counter Table sent successfully."
else
  write "<!> Failed - Validate command for the Execution Counter Table did not execute successfully."
endif

;; Wait for the CFE Validation message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (3001;3001.1) - Execution Counter Table Validation failed as expected"
    ut_setrequirements HS_3001, "P"
    ut_setrequirements HS_30011, "P"
  else
    write "<!> Failed (3001;3001.1) - Execution Counter Table Validation passed when failure was expected"
    ut_setrequirements HS_3001, "F"
    ut_setrequirements HS_30011, "F"
  endif
else
  write "<!> Failed (3001) - Did not rcv any Execution Counter Table Validation message"
  ut_setrequirements HS_3001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.0: Support Tests"
write ";***********************************************************************"
write ";  Step 3.1: Create and load a new Execution counter Table that contains"
write ";  the maximum <PLATFORM_DEFINED> entries."
write ";*********************************************************************"
s $sc_$cpu_hs_xct3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the Execution Counter Table
s load_table("hs_def_xct3",hostCPU)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Execution Counter Table sent successfully."
else
  write "<!> Failed - Load command for Execution Counter Table did not execute successfully."
endif

write ";*********************************************************************"
write ";  Step 3.2: Validate the Execution Counter Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {HSAppName}, HS_XCTVAL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the Execution Counter Table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ExeCntTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for the Execution Counter Table sent successfully."
else
  write "<!> Failed - Validate command for the Execution Counter Table did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3001) - Execution Counter Table Validation successful message rcv'd"
    ut_setrequirements HS_3001, "P"
  else
    write "<!> Failed (3001) - Did not rcv Execution Counter Table Validation successful message"
    ut_setrequirements HS_3001, "F"
  endif
else
  write "<!> Failed (3001) - Execution Counter Table Failed Validation"
  ut_setrequirements HS_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3: Activate the Execution Counter Table loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ExeCntTblName

;; Wait for the event monitor table update message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Rcv'd the Table Update Success message"
  ut_setrequirements HS_3001, "P"
else
  write "<*> Failed (3001) - Did not rcv the Table Update Success message for the Execution Counter Table"
  ut_setrequirements HS_3001, "F"
endif

write ";***********************************************************************"
write ";  Step 3.4: Dump the Execution Counter Table"
write ";***********************************************************************"
s get_tbl_to_cvt(ramDir,ExeCntTblName,"A","$cpu_hs_dumpxct",hostCPU,xctAPID)
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

write ";*********************************************************************"
write ";  Step 4.0: Clean-up - Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_hs_exectr "
write ";*********************************************************************"
ENDPROC
