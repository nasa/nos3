PROC $sc_$cpu_tbl_reset
;*******************************************************************************
;  Test Name:  $sc_$cpu_tbl_func
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify that the cFE Table Services (TBL)
;	software meets the requirements defined in the SRS for processor resets.
;
;  Requirements Tested
;    cTBL6500	Upon a Power-On Reset, all Table resources will be freed.
;    cTBL6501	Upon a Processor Reset, all Table resources will be freed.
;    cTBL6501.1	If the Table is a critical table then the contents of the Table
;		will be preserved.
;
;  Prerequisite Conditions
;       None 
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	05/01/06	W. Moleski	Initial development.
;	11/20/06	W. Moleski	Modifications for cFE 4.0.
;	02/08/12	W. Moleski	Added variable for ram disk and replaced
;					ut_setupevt with ut_setupevents
;       06/01/16        W. Moleski      Added variable for CPU to connect and
;                                       retrieve files from (hostCPU) since this
;                                       may be different from the command CPU.
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
#include "cfe_tbl_events.h"
#include "tst_tbl_events.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************

#define TS_6500		0
#define TS_6501		1
#define TS_65011	2

;******* LOCAL Defines
#define TST_TBL_NUM_DEFAULT_TABLES		4

global ut_req_array_size = 2
global ut_requirement[0 .. ut_req_array_size]

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL i,j, numMaxErrors
LOCAL cmdcnt, errcnt, num_free_buffers, numTables
LOCAL tbl_1_pkt, tbl_2_pkt
LOCAL tbl_1_appid, tbl_2_appid
LOCAL tbl_1_Reset, tbl_2_A_Reset, tbl_2_I_Reset
LOCAL tbl_1_active_default[1 .. 10]
LOCAL tbl_2_active_default[1 .. 10]
LOCAL tbl_2_inactive_default[1 .. 10]
LOCAL tbl_1_index, tbl_2_index
LOCAL numExpectedTables

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
local cfe_requirements[0 .. ut_req_array_size] = ["TS_6500", "TS_6501", "TS_6501.1"]

;; CPU1 is the default
tbl_1_pkt = "0FA2"
tbl_2_pkt = "0FA3"
tbl_1_appid = 4002
tbl_2_appid = 4003

;;if ("$CPU" = "CPU2") then
;;  tbl_1_pkt = "0FC0"
;;  tbl_2_pkt = "0FC1"
;;  tbl_1_appid = 4032
;;  tbl_2_appid = 4033
;;elseif ("$CPU" = "CPU3") then
;;  tbl_1_pkt = "0FE0"
;;  tbl_2_pkt = "0FE1"
;;  tbl_1_appid = 4064
;;  tbl_2_appid = 4065
;;endif

write ";**********************************************************************"
write "; Step 1.0:  Initialize the CPU for this test."
write ";**********************************************************************"
write "; Step 1.1: Command a Power-On Reset on $CPU"
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
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
write "; Step 1.2: Load the Test Applications (if necessary). "
write ";*********************************************************************"
write "BEFORE loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"
                                                                                
if ("$CPU" = "") then
  start tst_tbl_apps_start ("1.2")
else
  start $sc_$cpu_tst_tbl_apps_start ("1.2")
endif

Write "AFTER loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"

write ";*********************************************************************"
write "; Step 1.3: Search the Table Registry for the locations of the default"
write "; TST_TBL application tables."
write ";*********************************************************************"
local crit_tbl_index = 0

for i = 1 to $SC_$CPU_TBL_NUMTABLES do
    if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_01") then
        tbl_1_index = i
    elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
        tbl_2_index = i
    elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.CriticalTbl_04") then
        crit_tbl_index = i
    endif
enddo
                                                                                
write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index
write "Critical Table is at index #" & crit_tbl_index

write ";*********************************************************************"
write "; Step 1.4: Dump the Critical Table and verify that the default values"
write "; are loaded."
write ";*********************************************************************"
;;; Dump the Active buffer for the critical table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.CriticalTbl_04","A","active_tbl_4_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Critical Table Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 11 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 12 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 13 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 20 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 15 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 16 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 17 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 50 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 22 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 55) then
  write "<*> Passed - Critical Table Active buffer contains Default Values."
else
  write "<!> Failed - Critical Table Active buffer does not contain default values."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.0: Table Modification Tests "
write ";**********************************************************************"
write "; Step 2.1: Dump the default buffers for table #1 and #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
  tbl_1_active_default[i] = $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent."
else
  write "<!> Failed - Inactive Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid," received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
  tbl_2_inactive_default[i] = $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent."
else
  write "<!> Failed - Active Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
  tbl_2_active_default[i] = $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 2.2: Load the default double buffer table (Table #2) with a full"
write "; load."
write ";**********************************************************************"
;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = 0
$SC_$CPU_TST_TBL_TABLE2.element[2] = 1
$SC_$CPU_TST_TBL_TABLE2.element[3] = 2
$SC_$CPU_TST_TBL_TABLE2.element[4] = 3
$SC_$CPU_TST_TBL_TABLE2.element[5] = 4
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[7] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[8] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[9] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 1", "dbl_tbl_ld_1","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent."
else
  write "<!> Failed - Load command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

; check for the successful load event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.3: Dump the buffers for table #2 and verify their contents."
write ";**********************************************************************"
;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent."
else
  write "<!> Failed - Inactive Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid," received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed - Table #2 Inactive buffer contains changes made."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain changes loaded."
endif

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent."
else
  write "<!> Failed - Active Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

tbl_2_A_Reset = 0
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]

  ;;; Verify the Active buffer against the default values
  if (tbl_2_active_default[i] <> $SC_$CPU_TST_TBL_TABLE2.element[i]) then
    tbl_2_A_Reset = 1
  endif
enddo

if (tbl_2_A_Reset = 0) then
  write "<*> Passed - Active Table #2 contains the default values."
else
  write "<!> Failed - Active Table #2 does not contain the default values."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.4: Validate the default inactive double buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - InActive Table 2 validation failed."
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table 2 validated successfully."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table 2 validation failed."
  write "<!> Failed - Event Message not received for validation command."
endif
                                                                                
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
else
  write "<!> Failed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.5: Activate table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_UPDATE_SUCCESS_INF_EID, "."
endif

wait 5

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
endif

wait 5

write ";**********************************************************************"
write "; Step 2.6: Dump the buffers for table #2 and verify their contents."
write ";**********************************************************************"
;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent."
else
  write "<!> Failed - Inactive Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid," received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump against the active default values. 
;;; If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = tbl_2_active_default[1] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = tbl_2_active_default[2] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = tbl_2_active_default[3] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = tbl_2_active_default[4] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = tbl_2_active_default[5] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = tbl_2_active_default[6] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = tbl_2_active_default[7] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = tbl_2_active_default[8] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = tbl_2_active_default[9] AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = tbl_2_active_default[10]) then
  write "<*> Passed - Table #2 Inactive buffer contains default active buffer values."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain default active buffer values."
endif

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent."
else
  write "<!> Failed - Active Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed - Table #2 Active buffer contains changes made."
else
  write "<!> Failed - Table #2 Active buffer does not contain changes loaded."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.7: Load the default single buffer table (Table #1)."
write ";**********************************************************************"
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 1
$SC_$CPU_TST_TBL_TABLE1.element[2] = 2
$SC_$CPU_TST_TBL_TABLE1.element[3] = 3
$SC_$CPU_TST_TBL_TABLE1.element[4] = 4
$SC_$CPU_TST_TBL_TABLE1.element[5] = 5
$SC_$CPU_TST_TBL_TABLE1.element[6] = 6
$SC_$CPU_TST_TBL_TABLE1.element[7] = 7
$SC_$CPU_TST_TBL_TABLE1.element[8] = 8
$SC_$CPU_TST_TBL_TABLE1.element[9] = 9
$SC_$CPU_TST_TBL_TABLE1.element[10] = 10

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load 1", "sngl_tbl_ld_1","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.8: Dump the buffers for Table #1 and verify their contents."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<*> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed - Table #1 Inactive buffer contains changes made."
else
  write "<!> Failed - Table #1 Inactive buffer does not contain changes loaded."
endif

wait 5
 
;;; Dump the Active buffer for table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = tbl_1_active_default[1] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = tbl_1_active_default[2] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = tbl_1_active_default[3] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = tbl_1_active_default[4] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = tbl_1_active_default[5] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = tbl_1_active_default[6] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = tbl_1_active_default[7] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = tbl_1_active_default[8] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = tbl_1_active_default[9] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = tbl_1_active_default[10]) then
  write "<*> Passed - Table #1 Active buffer did not change."
else
  write "<!> Failed - Table #1 Active buffer changed."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.9: Validate the default inactive single buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - InActive Table 1 validation failed."
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table 1 validated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table 1 validation failed."
  Write "<!> Failed - Event Message not received for validation command."
endif
                                                                                
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
else
  write "<!> Failed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Step 2.10: Activate table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into Table #1."
else
  write "<!> Failed - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD
endif

wait 5

write ";**********************************************************************"
write "; Step 2.11: Dump the active buffer for Table #1 and verify contents.  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Dump command sent successfully."
else
  write "<!> Failed - Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed - Table #1 Active buffer contains changes made."
else
  write "<!> Failed - Table #1 Active buffer does not contain changes loaded."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.12: Load the default double buffer table (Table #2) again.    "
write ";**********************************************************************"
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = (-5)
$SC_$CPU_TST_TBL_TABLE2.element[2] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[3] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[4] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[5] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[6] = 0
$SC_$CPU_TST_TBL_TABLE2.element[7] = 1
$SC_$CPU_TST_TBL_TABLE2.element[8] = 2
$SC_$CPU_TST_TBL_TABLE2.element[9] = 4
$SC_$CPU_TST_TBL_TABLE2.element[10] = 5

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 2", "dbl_tbl_ld_2","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.13: Dump and verify the Table #2 Load. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent successfully."
else
  write "<!> Failed - Active Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed - Table #2 Active buffer did not change."
else
  write "<!> Failed - Table #2 Active buffer does not contain changes loaded."
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent successfully."
else
  write "<!> Failed - Inactive Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed - Table #2 Inactive buffer contains changes made."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain changes loaded."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.14: Validate the default inactive double buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - InActive Table 2 validation failed."
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table 2 validated successfully."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table 2 validation failed."
  write "<!> Failed - Event Message not received for validation command."
endif
                                                                                
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
else
  write "<!> Failed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.15: Activate Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

wait 5

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_2") then
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
endif

wait 5

write ";**********************************************************************"
write "; Step 2.16: Dump and verify the Table #2 Load. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent successfully."
else
  write "<!> Failed - Active Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed - Table #2 Active buffer contains changes made."
else
  write "<!> Failed - Table #2 Active buffer does not contain changes loaded."
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent successfully."
else
  write "<!> Failed - Inactive Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed - Table #2 Inactive buffer contains default active buffer values."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain the correct values."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.17: Register the maximum number of tables. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_REGMAXTABLES_EID, INFO, 1

/$SC_$CPU_TST_TBL_REGMAXTBLS
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - The proper Event messages were not received."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

;;; Verify that the number of tables registered is the max
if ($SC_$CPU_TBL_NumTables = CFE_TBL_MAX_NUM_TABLES) then
  write "<*> Passed - The number of tables is as expected."
else
  write "<!> Failed - TLM indicates  ", $SC_$CPU_TBL_NUMTables, " registered. This should have been ",CFE_TBL_MAX_NUM_TABLES,"."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.18: Try to register one more table. Two errors should be "
write "; generated indicating that the maximum number of tables have been "
write "; registered. One error will be from CFE_TBL and the other from TST_TBL."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_REGISTER_ERR_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_REGISTRYFULL_ERR_EID, ERROR, 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="1MoreTable" TBLOPTS=X'0' TBLSIZE=X'28'
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid," received."
  if ($SC_$CPU_find_event[2].num_found_messages = 1) then
    write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received."
  else
    write "<!> Failed - Expected Event message ",TST_TBL_REGMAXTABLES_ERR_EID, " was not received."
  endif
else
    write "<!> Failed - The proper Event messages were not received."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

;;; Verify that the number of tables registered is the max
if ($SC_$CPU_TBL_NumTables = CFE_TBL_MAX_NUM_TABLES) then
  write "<*> Passed - The number of tables is as expected."
else
  write "<!> Failed - TLM indicates  ", $SC_$CPU_TBL_NUMTables, " registered. This should have been ",CFE_TBL_MAX_NUM_TABLES,"."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.19: Send an invalid Activate command in order to increment the "
write "; error counter. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_SUCH_TABLE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.invalid_name"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed - The command generated an error as expected."
else
  write "<!> Failed - The command worked when an error was expected."
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.20: Delete the default Critical Table. "
write ";**********************************************************************"
write "; Step 2.20.1: Verify that the table is a Critical Table and attempt to"
write ";              delete it from the CDS."
write ";**********************************************************************"
;;;; Dump the CDS Registry in order to verify that the Critical
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_CDS_REG_DUMP_INF_EID, "DEBUG", 1

s get_file_to_cvt (ramDir, "cfe_cds_reg.log", "$sc_$cpu_cds_reg.log", hostCPU)
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - CDS Registry Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - CDS Registry Info retrieval. Expected Event message ",CFE_ES_CDS_REG_DUMP_INF_EID, " was not received"
endif
                                                                                
;; Check if the CDS is in the Registry
local found_CDS = FALSE
local cdsRegIdx = 0
for cds_index = 1 to CFE_ES_CDS_MAX_NUM_ENTRIES do
  if ($SC_$CPU_ES_CDSREG[cds_index].CDSName = "TST_TBL.CriticalTbl_04") then
    found_CDS = TRUE
    cdsRegIdx = cds_index
  endif
enddo

if (found_CDS = TRUE) then
  write "<*> Passed - Critical Table found in CDS Registry."
  if (p@$SC_$CPU_ES_CDSREG[cdsRegIdx].CRITICALTBL = "TRUE") then
    write "---- Critical Table flag marked TRUE"
  else
    write "---- Critical Table flag marked FALSE"
  endif
endif

;; Try to delete the CDS before unregistering the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_IN_REGISTRY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_DELETECDS Tablename="TST_TBL.CriticalTbl_04"
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - The Critical Table delete incremented the error counter as expected."
else
  write "<!> Failed - The TBL_DeleteCDS command did not increment the error counter."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_TBL_IN_REGISTRY_ERR_EID, " was not received"
endif

write ";**********************************************************************"
write "; Step 2.20.2: Send a command to unregister the Critical Table. The "
write "; 	      table must be unregistered BEFORE deleting the CDS."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGSFREETBL_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_EID, INFO, 2

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="CriticalTbl_04"
wait 5

;; Verify the expected event messages were generated
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Expected Event Messages ",$SC_$CPU_find_event[1].eventid, " and ", $SC_$CPU_find_event[2].eventid, " received."
else
  write "<!> Failed - Expected Event Messages ",TST_TBL_UNREGSFREETBL_EID," and ",TST_TBL_UNREGISTERTBL_EID," not received."
endif

write ";**********************************************************************"
write "; Step 2.20.3: Send a command to delete the Critical Table CDS before "
write "; 	      stopping the TST_TBL application."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_DELETECDS Tablename="TST_TBL.CriticalTbl_04"
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - The Critical Table delete incremented the error counter as expected."
else
  write "<!> Failed - The TBL_DeleteCDS command did not increment the error counter."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID, " was not received"
endif

write ";**********************************************************************"
write "; Step 2.20.4: Send a command to delete the TST_TBL application. The "
write "; application that owns the CDS must not be running before deleting it."
write ";**********************************************************************"
;; Send the ES_DeleteApp command
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_INF_EID, "INFO", 2

cmdcnt = $SC_$CPU_ES_CMDPC + 1

; Delete the TST_TBL application
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_TBL"
wait 5

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - DeleteApp command executed properly."
else
  write "<!> Failed - DeleteApp command failed."
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
local found_app1 = FALSE
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_TBL") then
    found_app1 = TRUE
  endif
enddo

if (found_app1 = FALSE) then
  write "<*> Passed - TST_TBL application has been deleted."
else
  write "<!> Failed - TST_TBL application is still running."
endif

write ";**********************************************************************"
write "; Step 2.20.5: Send a command to delete the Critical Table. Verify the "
write "; the table is deleted. This table should be recreated by the Power-On"
write "; reset performed below."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_CDS_DELETED_INFO_EID, "INFO", 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

/$SC_$CPU_TBL_DELETECDS Tablename="TST_TBL.CriticalTbl_04"
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - The Critical Table was deleted."
else
  write "<!> Failed - The TBL_DeleteCDS command did not increment the valid command counter."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_TBL_CDS_DELETED_INFO_EID, " was not received"
endif

wait 5
;;;; Dump the CDS Registry in order to verify that the Critical
;;;; Table was deleted
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_CDS_REG_DUMP_INF_EID, "DEBUG", 1
s get_file_to_cvt (ramDir, "cfe_cds_reg.log", "$sc_$cpu_cds_reg.log", hostCPU)
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - CDS Registry Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - CDS Registry Info retrieval. Expected Event message ",CFE_ES_CDS_REG_DUMP_INF_EID, " was not received"
endif
                                                                                
;; Check if the CDS is in the Registry
local found_CDS = FALSE
for cds_index = 1 to CFE_ES_CDS_MAX_NUM_ENTRIES do
  if ($SC_$CPU_ES_CDSREG[cds_index].CDSName = "TST_TBL.CriticalTbl_04") then
    found_CDS = TRUE
  endif
enddo

if (found_CDS = FALSE) then
  write "<*> Passed - TST_TBL.CriticalTbl_04 was not found in CDS Registry."
else
  write "<!> Failed - TST_TBL.CriticalTbl_04 still exists in the CDS Registry."
endif

write ";**********************************************************************"
write "; Step 2.21: Send a command to delete a CDS that is not a Critical "
write "; 	    table."
write ";**********************************************************************"
;; Start the TST_ES application
write "; Starting the TST_ES application. "
s load_start_app ("TST_ES", hostCPU)
wait 5

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_NOT_IN_CRIT_REG_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_DELETECDS Tablename="TST_ES.TSTCDS"
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - The Critical Table delete incremented the error counter as expected."
else
  write "<!> Failed - The TBL_DeleteCDS command did not increment the error counter."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Expected Event message ",CFE_TBL_NOT_IN_CRIT_REG_ERR_EID, " was not received"
endif

write ";**********************************************************************"
write "; Step 3.0: Perform a Power-on Reset. "
write ";**********************************************************************"
write "; Step 3.1: Save all the Counters and necessary info that needs to be "
write "; verified when the power is restored. "
write ";**********************************************************************"
;; Save the command, error, and number of tables counters
cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC
numTables = $SC_$CPU_TBL_NumTables

write "Command Ctr = ", cmdcnt
write "Error Ctr = ", errcnt
write "Number of tables = ", numTables

wait 5

;; Print info from the table registry for the default tables 
for i = tbl_1_index to (tbl_1_index + TST_TBL_NUM_DEFAULT_TABLES-1) do
  ;; Print the appropriate info
  write "Default Table #",i, " info:"
  write "    Name: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Name
  write "    Size: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Size
  write "    Users: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_NumUsers
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_DumpOnly = 0) then
    write "    Dump Option: Load-Dump"
  else
    write "    Dump Option: Dump-Only"
  endif

  write "    Last File Loaded: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_LastFileUpd
enddo

wait 5

write ";**********************************************************************"
write "; Step 3.2: Perform a Power-on Reset. "
write ";**********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

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

write ";**********************************************************************"
write "; Step 3.3: Verify that the counters saved above are set to zero.      "
write ";**********************************************************************"
;;; Print out the current values of the tlm points being checked
write "Command Ctr = ", $SC_$CPU_TBL_CMDPC
write "Error Ctr = ", $SC_$CPU_TBL_CMDEC
write "Number of tables = ", $SC_$CPU_TBL_NumTables

; Check if the CMDPC counter reset
if ($SC_$CPU_TBL_CMDPC = 0) then
  write "<*> Passed (6500) - Command counter was reset!"
  ut_setrequirements TS_6500, "P"
else
  write "<!> Failed (6500) - Command counter did not reset!"
  ut_setrequirements TS_6500, "F"
endif

; Check if the CMDEC counter reset
if ($SC_$CPU_TBL_CMDEC = 0) then
  write "<*> Passed (6500) - Error counter was reset!"
  ut_setrequirements TS_6500, "P"
else
  write "<!> Failed (6500) - Error counter did not reset!"
  ut_setrequirements TS_6500, "F"
endif

; Check if the Number of Tables counter reset
if ($SC_$CPU_TBL_NumTables = numTables) then
  write "<*> Passed (6500) - Table counter was reset!"
  ut_setrequirements TS_6500, "P"
else
  write "<!> Failed (6500) - Table counter did not reset!"
  ut_setrequirements TS_6500, "F"
endif

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

;; Print info from the table registry for the default tables 
for i = tbl_1_index to (tbl_1_index + TST_TBL_NUM_DEFAULT_TABLES-1) do
  ;; Print the appropriate info
  write "Default Table #",i, " info:"
  write "    Name: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Name
  write "    Size: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Size
  write "    Users: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_NumUsers
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_DumpOnly = 0) then
    write "    Dump Option: Load-Dump"
  else
    write "    Dump Option: Dump-Only"
  endif

  write "    Last File Loaded: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_LastFileUpd
enddo

wait 5

write ";**********************************************************************"
write "; Step 3.4: Start the Test applications for Table Services. "
write ";**********************************************************************"
write "BEFORE loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"
numExpectedTables = $SC_$CPU_TBL_NUMTABLES + TST_TBL_NUM_DEFAULT_TABLES

if ("$CPU" = "") then
  start tst_tbl_apps_start ("3.4")
else
  start $sc_$cpu_tst_tbl_apps_start ("3.4")
endif
                                                                                
Write "AFTER loading the test apps, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"

;;; Find the locations of the TST_TBL default tables
for i = 1 to $SC_$CPU_TBL_NUMTABLES do
    if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_01") then
        tbl_1_index = i
    endif
    if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
        tbl_2_index = i
    endif
enddo

write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index
                                                                                
; Check if the numTables counter reset to the default
ut_tlmwait $SC_$CPU_TBL_NumTables, {numExpectedTables}
                                                                                
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Number of registered tables is ",numExpectedTables
else
  write "<!> Failed - Number of registered tables counter = ",$SC_$CPU_TBL_NUMTABLES,"; expected ",numExpectedTables
endif

wait 5

write ";**********************************************************************"
write "; Step 3.5: Dump the buffers for the default tables and verify that the"
write "; default configuration exists."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Dump command sent successfully."
else
  write "<!> Failed - Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

;; Check for either event message
if ((SCX_CPU3_find_event[1].num_found_messages = 1) .OR. ;;
    (SCX_CPU3_find_event[2].num_found_messages = 1)) then
   write "<*> Passed - Dump Event message received"
else
   write "<!> Failed - Did not rcv either dump event message. Expected either event message ",CFE_TBL_WRITE_DUMP_INF_EID, "or ",CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

; print the dumped elements
tbl_1_Reset = 0
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]

  ;;; Verify that the default values are loaded
  if ($SC_$CPU_TST_TBL_TABLE1.element[i] <> tbl_1_active_default[i]) then
    tbl_1_Reset = 1;
  endif
enddo

if (tbl_1_Reset = 0) then
  write "<*> Passed (6500) - Table #1 active buffer contains default values."
  ut_setrequirements TS_6500, "P"
else
  write "<!> Failed (6500) - Table #1 Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6500, "F"
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent."
else
  write "<!> Failed - Inactive Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

;; Check for either event message
if ((SCX_CPU3_find_event[1].num_found_messages = 1) .OR. ;;
    (SCX_CPU3_find_event[2].num_found_messages = 1)) then
   write "<*> Passed - Dump Event message received"
else
   write "<!> Failed - Did not rcv either dump event message. Expected either event message ",CFE_TBL_WRITE_DUMP_INF_EID, "or ",CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

tbl_2_I_Reset = 0
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]

  ;;; Verify that the default values are loaded
  if ($SC_$CPU_TST_TBL_TABLE2.element[i] <> tbl_2_inactive_default[i]) then
    tbl_2_I_Reset = 1;
  endif
enddo

if (tbl_2_I_Reset = 0) then
  write "<*> Passed (6500) - Table #2 Inactive buffer contains default values."
  ut_setrequirements TS_6500, "P"
else
  write "<!> Failed (6500) - Table #2 Inactive buffer does not contain default values."
  ut_setrequirements TS_6500, "F"
endif
wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent."
else
  write "<!> Failed - Active Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

;; Check for either event message
if ((SCX_CPU3_find_event[1].num_found_messages = 1) .OR. ;;
    (SCX_CPU3_find_event[2].num_found_messages = 1)) then
   write "<*> Passed - Dump Event message received"
else
   write "<!> Failed - Did not rcv either dump event message. Expected either event message ",CFE_TBL_WRITE_DUMP_INF_EID, "or ",CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

tbl_2_A_Reset = 0
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]

  ;;; Verify that the default values are loaded
  if ($SC_$CPU_TST_TBL_TABLE2.element[i] <> tbl_2_active_default[i]) then
    tbl_2_A_Reset = 1;
  endif
enddo

if (tbl_2_A_Reset = 0) then
  write "<*> Passed (6500) - Table #2 Active buffer contains default values."
  ut_setrequirements TS_6500, "P"
else
  write "<!> Failed (6500) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6500, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.0: Processor Reset Test Setup"
write ";**********************************************************************"
write "; Step 4.1: Load the default single buffer table (Table #1)."
write ";**********************************************************************"
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 1
$SC_$CPU_TST_TBL_TABLE1.element[2] = 2
$SC_$CPU_TST_TBL_TABLE1.element[3] = 3
$SC_$CPU_TST_TBL_TABLE1.element[4] = 4
$SC_$CPU_TST_TBL_TABLE1.element[5] = 5
$SC_$CPU_TST_TBL_TABLE1.element[6] = 6
$SC_$CPU_TST_TBL_TABLE1.element[7] = 7
$SC_$CPU_TST_TBL_TABLE1.element[8] = 8
$SC_$CPU_TST_TBL_TABLE1.element[9] = 9
$SC_$CPU_TST_TBL_TABLE1.element[10] = 10

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load 1", "sngl_tbl_ld_1","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 4.2: Dump the buffers for Table #1 and verify their contents."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<*> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed - Table #1 Inactive buffer contains changes made."
else
  write "<!> Failed - Table #1 Inactive buffer does not contain changes loaded."
endif

wait 5
 
;;; Dump the Active buffer for table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = tbl_1_active_default[1] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = tbl_1_active_default[2] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = tbl_1_active_default[3] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = tbl_1_active_default[4] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = tbl_1_active_default[5] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = tbl_1_active_default[6] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = tbl_1_active_default[7] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = tbl_1_active_default[8] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = tbl_1_active_default[9] AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = tbl_1_active_default[10]) then
  write "<*> Passed - Table #1 Active buffer did not change."
else
  write "<!> Failed - Table #1 Active buffer changed."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.3: Validate the default inactive single buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - InActive Table 1 validation failed."
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table 1 validated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table 1 validation failed."
  Write "<!> Failed - Event Message not received for validation command."
endif
                                                                                
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
else
  write "<!> Failed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Step 4.4: Activate table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into Table #1."
else
  write "<!> Failed - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD
endif

wait 5

write ";**********************************************************************"
write "; Step 4.5: Dump the active buffer for Table #1 and verify contents.  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Dump command sent successfully."
else
  write "<!> Failed - Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed - Table #1 Active buffer contains changes made."
else
  write "<!> Failed - Table #1 Active buffer does not contain changes loaded."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.6: Load the default double buffer table (Table #2) again.    "
write ";**********************************************************************"
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = (-5)
$SC_$CPU_TST_TBL_TABLE2.element[2] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[3] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[4] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[5] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[6] = 0
$SC_$CPU_TST_TBL_TABLE2.element[7] = 1
$SC_$CPU_TST_TBL_TABLE2.element[8] = 2
$SC_$CPU_TST_TBL_TABLE2.element[9] = 4
$SC_$CPU_TST_TBL_TABLE2.element[10] = 5

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 2", "dbl_tbl_ld_2","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 4.7: Dump and verify the Table #2 Load. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent successfully."
else
  write "<!> Failed - Active Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed - Table #2 Active buffer did not change."
else
  write "<!> Failed - Table #2 Active buffer does not contain changes loaded."
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent successfully."
else
  write "<!> Failed - Inactive Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed - Table #2 Inactive buffer contains changes made."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain changes loaded."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.8: Validate the default inactive double buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - InActive Table 2 validation failed."
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table 2 validated successfully."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table 2 validation failed."
  write "<!> Failed - Event Message not received for validation command."
endif
                                                                                
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
else
  write "<!> Failed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.9: Activate Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_2") then
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
endif

wait 5

write ";**********************************************************************"
write "; Step 4.10: Dump and verify the Table #2 Load. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent successfully."
else
  write "<!> Failed - Active Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed - Table #2 Active buffer contains changes made."
else
  write "<!> Failed - Table #2 Active buffer does not contain changes loaded."
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent successfully."
else
  write "<!> Failed - Inactive Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed - Table #2 Inactive buffer contains default active buffer values."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain the correct values."
endif

wait 5
write ";**********************************************************************"
write "; Step 4.11: Register the maximum number of tables."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_REGMAXTABLES_EID, INFO, 1

/$SC_$CPU_TST_TBL_REGMAXTBLS
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
    write "<!> Failed - The proper Event messages were not received."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

;;; Verify that the number of tables registered is the max
if ($SC_$CPU_TBL_NumTables = CFE_TBL_MAX_NUM_TABLES) then
  write "<*> Passed - The number of tables is as expected."
else
  write "<!> Failed - TLM indicates  ", $SC_$CPU_TBL_NUMTables, " registered. This should have been ",CFE_TBL_MAX_NUM_TABLES,"."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.12: Try to register one more table. Two errors should be "
write "; generated indicating that the maximum number of tables have been "
write "; registered. One error will be from CFE_TBL and the other from TST_TBL."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_REGISTER_ERR_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_REGISTRYFULL_ERR_EID, ERROR, 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="1MoreTable" TBLOPTS=X'0' TBLSIZE=X'28'
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid," received."
  if ($SC_$CPU_find_event[2].num_found_messages = 1) then
    write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received."
  else
    write "<!> Failed - Expected Event message ",TST_TBL_REGMAXTABLES_ERR_EID, " was not received."
  endif
else
    write "<!> Failed - The proper Event messages were not received."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

;;; Verify that the number of tables registered is the max
if ($SC_$CPU_TBL_NumTables = CFE_TBL_MAX_NUM_TABLES) then
  write "<*> Passed - The number of tables is as expected."
else
  write "<!> Failed - TLM indicates  ", $SC_$CPU_TBL_NUMTables, " registered. This should have been ",CFE_TBL_MAX_NUM_TABLES,"."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.13: Send an invalid Activate command in order to increment the "
write "; error counter. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_SUCH_TABLE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.invalid_name"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed - The command generated an error as expected."
else
  write "<!> Failed - The command worked when an error was expected."
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.14: Load the critical table (Table #4)."
write ";**********************************************************************"
;;; Although this step loads the critical table, Table 1 cvt is used to generate
;;; the load file and verify the table contents.
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 1
$SC_$CPU_TST_TBL_TABLE1.element[2] = 2
$SC_$CPU_TST_TBL_TABLE1.element[3] = 3
$SC_$CPU_TST_TBL_TABLE1.element[4] = 4
$SC_$CPU_TST_TBL_TABLE1.element[5] = 5
$SC_$CPU_TST_TBL_TABLE1.element[6] = 6
$SC_$CPU_TST_TBL_TABLE1.element[7] = 7
$SC_$CPU_TST_TBL_TABLE1.element[8] = 8
$SC_$CPU_TST_TBL_TABLE1.element[9] = 9
$SC_$CPU_TST_TBL_TABLE1.element[10] = 10

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Critical Table Load 1", "critical_tbl_ld_1","TST_TBL.CriticalTbl_04", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

start load_table ("critical_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for the critical table ......"
wait until ($SC_$CPU_RF[crit_tbl_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 4.15: Dump the buffers for the Crtical Table and verify their "
write ";            contents."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.CriticalTbl_04","I","inactive_tbl_4_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

;; Check for either event message
if ((SCX_CPU3_find_event[1].num_found_messages = 1) .OR. ;;
    (SCX_CPU3_find_event[2].num_found_messages = 1)) then
   write "<*> Passed - Dump Event message received"
else
   write "<!> Failed - Did not rcv either dump event message. Expected either event message ",CFE_TBL_WRITE_DUMP_INF_EID, "or ",CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

; print the dumped elements
write "Critical Table Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed - Critical Table Inactive buffer contains changes made."
else
  write "<!> Failed - Critical Table Inactive buffer does not contain changes loaded."
endif

wait 5
 
;;; Dump the Active buffer for the critical table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.CriticalTbl_04","A","active_tbl_4_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

;; Check for either event message
if ((SCX_CPU3_find_event[1].num_found_messages = 1) .OR. ;;
    (SCX_CPU3_find_event[2].num_found_messages = 1)) then
   write "<*> Passed - Dump Event message received"
else
   write "<!> Failed - Did not rcv either dump event message. Expected either event message ",CFE_TBL_WRITE_DUMP_INF_EID, "or ",CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

; print the dumped elements
write "Critical Table Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 11 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 12 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 13 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 20 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 15 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 16 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 17 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 50 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 22 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 55) then
  write "<*> Passed - Critical Table Active buffer did not change."
else
  write "<!> Failed - Critical Table Active buffer changed."
endif

wait 5

write ";**********************************************************************"
write "; Step 4.16: Validate the inactive buffer for the critical table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.CriticalTbl_04"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - Critical Table InActive buffer validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Critical Table InActive buffer validation failed."
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Critical Table Inactive buffer validated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table 1 validation failed."
  Write "<!> Failed - Event Message not received for validation command."
endif
                                                                                
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
else
  write "<!> Failed - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Step 4.17: Activate the critical table."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.CriticalTbl_04"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[crit_tbl_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/critical_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into the Critical Table."
else
  write "<!> Failed - Critical Table was loaded with ", $SC_$CPU_RF[crit_tbl_index].$SC_$CPU_TBL_LASTFILEUPD
endif

wait 5

write ";**********************************************************************"
write "; Step 4.18: Dump the active buffer for the Critical Table and verify "
write "; its contents.  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.CriticalTbl_04","A","active_crit_tbl_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Dump command sent successfully."
else
  write "<!> Failed - Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Critical Table Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed - Critical Table Active buffer contains changes made."
else
  write "<!> Failed - Critical Table Active buffer does not contain changes loaded."
endif

wait 5

write ";**********************************************************************"
write "; Step 5.0: Perform a Processor Reset. "
write ";**********************************************************************"
write "; Step 5.1: Save all the Counters and necessary info that needs to be "
write "; verified after the reset."
write ";**********************************************************************"
;; Save the command, error, and number of tables counters
cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC

write "Command Ctr = ", cmdcnt
write "Error Ctr = ", errcnt
write "Number of tables = ", $SC_$CPU_TBL_NumTables

wait 5

;; Print info from the table registry for the default tables 
for i = tbl_1_index to (tbl_1_index + TST_TBL_NUM_DEFAULT_TABLES-1) do
  ;; Print the appropriate info
  write "Default Table #",i, " info:"
  write "    Name: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Name
  write "    Size: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Size
  write "    Users: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_NumUsers
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_DumpOnly = 0) then
    write "    Dump Option: Load-Dump"
  else
    write "    Dump Option: Dump-Only"
  endif

  write "    Last File Loaded: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_LastFileUpd
enddo

;;;; Need to print the contents of the critical table to verify it was preserved
write "==> Processor Reset Counter = ",$SC_$CPU_ES_PROCRESETCNT
wait 5

write ";**********************************************************************"
write "; Step 5.2: Perform a Processor Reset. "
write ";**********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write "==> Processor Reset Counter = ",$SC_$CPU_ES_PROCRESETCNT

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

write ";**********************************************************************"
write "; Step 5.3: Verify that the counters saved above are set to zero.      "
write ";**********************************************************************"
;;; Print out the current values of the tlm points being checked
write "Command Ctr = ", $SC_$CPU_TBL_CMDPC
write "Error Ctr = ", $SC_$CPU_TBL_CMDEC
write "Number of tables = ", $SC_$CPU_TBL_NumTables

; Check if the CMDPC counter reset
if ($SC_$CPU_TBL_CMDPC = 0) then
  write "<*> Passed (6501) - Command counter was reset!"
  ut_setrequirements TS_6501, "P"
else
  write "<!> Failed (6501) - Command counter did not reset!"
  ut_setrequirements TS_6501, "F"
endif

; Check if the CMDEC counter reset
if ($SC_$CPU_TBL_CMDEC = 0) then
  write "<*> Passed (6501) - Error counter was reset!"
  ut_setrequirements TS_6501, "P"
else
  write "<!> Failed (6501) - Error counter did not reset!"
  ut_setrequirements TS_6501, "F"
endif

; Check if the Number of Tables counter reset
if ($SC_$CPU_TBL_NumTables = numTables) then
  write "<*> Passed (6501) - Table counter was reset!"
  ut_setrequirements TS_6501, "P"
else
  write "<!> Failed (6501) - Table counter did not reset!"
  ut_setrequirements TS_6501, "F"
endif

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 5

;; Print info from the table registry for the default tables 
for i = tbl_1_index to (tbl_1_index + TST_TBL_NUM_DEFAULT_TABLES-1) do
  ;; Print the appropriate info
  write "Default Table #",i, " info:"
  write "    Name: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Name
  write "    Size: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_Size
  write "    Users: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_NumUsers
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_DumpOnly = 0) then
    write "    Dump Option: Load-Dump"
  else
    write "    Dump Option: Dump-Only"
  endif

  write "    Last File Loaded: ",$SC_$CPU_RF[i].$SC_$CPU_TBL_LastFileUpd
enddo

wait 5

write ";**********************************************************************"
write "; Step 5.4: Start the Test applications for Table Services. "
write ";**********************************************************************"
write "BEFORE loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"
numExpectedTables = $SC_$CPU_TBL_NUMTABLES + TST_TBL_NUM_DEFAULT_TABLES

if ("$CPU" = "") then
  start tst_tbl_apps_start ("5.4")
else
  start $sc_$cpu_tst_tbl_apps_start ("5.4")
endif
                                                                                
Write "AFTER loading the test apps, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"

;;; Find the locations of the TST_TBL default tables
for i = 1 to $SC_$CPU_TBL_NUMTABLES do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_01") then
    tbl_1_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
    tbl_2_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.CriticalTbl_04") then
    crit_tbl_index = i
  endif
enddo

write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index
write "Critical Table is at index #" & crit_tbl_index
 
; Check if the numTables counter reset to the default
ut_tlmwait $SC_$CPU_TBL_NumTables, {numExpectedTables}
                                                                                
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Number of registered tables is ",numExpectedTables
else
  write "<!> Failed - Number of registered tables counter = ",$SC_$CPU_TBL_NUMTABLES,"; expected ",numExpectedTables
endif

wait 5

write ";**********************************************************************"
write "; Step 5.5: Dump the buffers for the default tables and verify that the"
write "; default configuration exists."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Dump command sent successfully."
else
  write "<!> Failed - Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
elseif ($SC_$CPU_find_event[2].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, " or ", CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

; print the dumped elements
tbl_1_Reset = 0
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]

  ;;; Verify that the default values are loaded
  if ($SC_$CPU_TST_TBL_TABLE1.element[i] <> tbl_1_active_default[i]) then
    tbl_1_Reset = 1;
  endif
enddo

if (tbl_1_Reset = 0) then
  write "<*> Passed (6501) - Table #1 active buffer contains default values."
  ut_setrequirements TS_6501, "P"
else
  write "<!> Failed (6501) - Table #1 Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6501, "F"
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Dump command sent."
else
  write "<!> Failed - Inactive Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid," received"
elseif ($SC_$CPU_find_event[2].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, " or ", CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

tbl_2_I_Reset = 0
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]

  ;;; Verify that the default values are loaded
  if ($SC_$CPU_TST_TBL_TABLE2.element[i] <> tbl_2_inactive_default[i]) then
    tbl_2_I_Reset = 1;
  endif
enddo

if (tbl_2_I_Reset = 0) then
  write "<*> Passed (6501) - Table #2 Inactive buffer contains default values."
  ut_setrequirements TS_6501, "P"
else
  write "<!> Failed (6501) - Table #2 Inactive buffer does not contain default values."
  ut_setrequirements TS_6501, "F"
endif
wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Active Dump command sent."
else
  write "<!> Failed - Active Dump command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
elseif ($SC_$CPU_find_event[2].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, " or ", CFE_TBL_OVERWRITE_DUMP_INF_EID
endif

tbl_2_A_Reset = 0
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]

  ;;; Verify that the default values are loaded
  if ($SC_$CPU_TST_TBL_TABLE2.element[i] <> tbl_2_active_default[i]) then
    tbl_2_A_Reset = 1;
  endif
enddo

if (tbl_2_A_Reset = 0) then
  write "<*> Passed (6501) - Table #2 Active buffer contains default values."
  ut_setrequirements TS_6501, "P"
else
  write "<!> Failed (6501) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6501, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 5.6: Dump the Critical Table active buffer and verify contents. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.CriticalTbl_04","A","active_crit_tbl_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Dump command sent successfully."
else
  write "<!> Failed - Dump command did not execute successfully."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - Error count did not increment."
else
  write "<!> Failed - Error counter incremented."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
   write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
endif

; print the dumped elements
write "Critical Table Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed (6501.1) - Critical Table contents preserved after a Processor Reset."
  ut_setrequirements TS_65011, "P"
else
  write "<!> Failed (6501.1) - Critical Table contents were not preserved."
  ut_setrequirements TS_65011, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 6.0: Command a Power-On Reset on $CPU in order to clean up."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

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
