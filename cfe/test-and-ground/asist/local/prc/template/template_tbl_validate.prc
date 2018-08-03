PROC $sc_$cpu_tbl_validate
;*******************************************************************************
;  Test Name:  $sc_$cpu_tbl_validate
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to validate the Table Services spacecraft ID
;	and processor ID validation in a table load image file. This capability
;	was added in cFE 6.3.0.0 in response to DCR #15179. Also, the cFE must
;	be configured to use Table Validation via the configuration parameters
;	contained in cfe_platform_cfg.h (CFE_TBL_VALID_SCID_COUNT and 
;	CFE_TABLE_VALID_PRID_COUNT)
;
;  Requirements Tested
;	No requirements are associated with this capability.
;
;  Prerequisite Conditions
;       None 
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name		Description
;	02/08/12	W. Moleski	Initial implementation.
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;  	ut_setupevents		Directive to look for a particular event
;				and increment a value in the CVT to
;				indicate receipt.
;   $sc_$cpu_tst_tbl_apps_start Procedure that checks if the TST_TBL and 
;				TST_TBL2 applications are running and starts
;				them if they are not.
;	get_tbl_to_cvt		Procedure that dumps the specified table from
;				the processor and loads it into the cvt
;	ut_tlmwait		Directive that waits for the specified telemetry
;				condition to be met
;	create_tbl_file_from_cvt Procedure that creates a load file from the
;				specified arguments and cvt
;	load_table		Procedure that takes the specified file and
;				transfers the file to the specified processor
;				and then issues a TBL_LOAD command using the
;				file.
;	get_file_to_cvt		Procedure to load the specified file into the
;				specified cvt
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_evs_events.h"
#include "cfe_tbl_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************
;; None

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL i
LOCAL cmdcnt, errcnt, num_free_buffers, rawcmd
LOCAL tbl_1_pkt, tbl_2_pkt
LOCAL tbl_1_appid, tbl_2_appid
LOCAL valsuccesscnt, valfailedcnt, valnumreqcnt, valrespcnt
LOCAL tbl_1_index, tbl_2_index

local ramDir = "RAM:0"

;**********************************************************************
; Set the local values 
;**********************************************************************
;; CPU1 is the default
tbl_1_pkt = "0FA2"
tbl_2_pkt = "0FA3"
tbl_1_appid = 4002
tbl_2_appid = 4003

if ("$CPU" = "CPU2") then
  tbl_1_pkt = "0FC0"
  tbl_2_pkt = "0FC1"
  tbl_1_appid = 4032
  tbl_2_appid = 4033
elseif ("$CPU" = "CPU3") then
  tbl_1_pkt = "0FE0"
  tbl_2_pkt = "0FE1"
  tbl_1_appid = 4064
  tbl_2_appid = 4065
endif

write ";*********************************************************************"
write "; Step 1.0: Proc setup. "
write ";*********************************************************************"
write "; Step 1.1: Command a Power-On Reset on $CPU "
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
for i = 1 to $SC_$CPU_TBL_NUMTABLES do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_01") then
    tbl_1_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
    tbl_2_index = i
  endif

  write "Table #",i," Name = ", $SC_$CPU_RF[i].$SC_$CPU_TBL_NAME 
enddo

write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index

write ";**********************************************************************"
write "; Step 2.0: Partial table load test (double buffer table = Table #2)  "
write ";**********************************************************************"
write "; Step 2.1: Dump the active and inactive buffers prior to commanding  "
write "; the first table load to Table #2. The buffers should contain the     "
write "; following values: "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  = -5                  element[1]  =  0 "
write "; element[2]  = -4                  element[2]  =  0 "
write "; element[3]  = -3                  element[3]  =  0 "
write "; element[4]  = -2                  element[4]  =  0 "
write "; element[5]  = -1                  element[5]  =  0 "
write "; element[6]  =  0                  element[6]  =  0 "
write "; element[7]  =  1                  element[7]  =  0 "
write "; element[8]  =  2                  element[8]  =  0 "
write "; element[9]  =  3                  element[9]  =  0 "
write "; element[10] =  5                  element[10] =  0 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp","$CPU",tbl_2_pkt)

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

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed - Dump filename is correct."
else
  write "<!> Failed - Dump filename not the expected name."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "; Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Passed - Expected Event Msg ", CFE_TBL_WRITE_DUMP_INF_EID, " but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp","$CPU",tbl_2_pkt)

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

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed - Dump filename is correct."
else
  write "<!> Failed - Dump filename not the expected name."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed -  Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Passed - Expected Event Msg ", CFE_TBL_WRITE_DUMP_INF_EID, " but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 2.2: Perform a partial load to Table #2. Elements 2 and 6 are "
write "; changed to 2 and 4 respectively. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 2nd & 6th byte of the table
$SC_$CPU_TST_TBL_TABLE2.element[2] = 2
$SC_$CPU_TST_TBL_TABLE2.element[6] = 4

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 1", "dbl_tbl_ld_1","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[2]", "$SC_$CPU_TST_TBL_TABLE2.element[6]")

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Partial Load command sent."
else
  write "<!> Failed - Partial Load command failed."
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed - error counter remained the same."
else
  write "<!> Failed - error counter incremented."
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed - the correct file was loaded."
else
  write "<!> Failed - the expected file was not loaded."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

;; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.3: Validate the inactive buffer for Table #2. "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Failed - InActive Table #2 validation failed."
endif
                                                                                
; wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table #2 validated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table #2 validation failed."
endif

;; Look at the LastValBuf to see if it iindicates INACTIVE
if (p@$SC_$CPU_TBL_LASTVALBUF = "Inactive") then
  write "<*> Passed - TBL HK indicates the Inactive buffer was validated."
else
  write "<!> Failed - TBL HK does not indicate the Inactive buffer was validated."
endif

wait 5

write ";**********************************************************************"
write "; Step 2.4: Activate the load for Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
else   
  write "<*> Passed - Activate command sent properly." 
endif

; Wait for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
endif

;; Wait for the next HK packet
wait 5

;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed - the correct table was specified in HK"
else
  write "<!> Failed - HK specifies the incorrect Table has been updated. Expected 'TST_TBL.dflt_tbl_02'; Got '",$SC_$CPU_TBL_LASTUPDTBLNAME,"'"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/dbl_tbl_ld_1."
endif

;;; Here we should compare the TBL HK value with that in the Table Registry.
;;; If they are equal, then pass the display requirement
if ($SC_$CPU_TBL_LastUpdTime.$SC_$CPU_TBL_SECONDS = $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_SYSTIME.$SC_$CPU_TBL_ST_SECONDS) then
  write "<*> Passed - Table HK and Table Registry seconds are the same."
else	
  write "<!> Failed - Table HK and Table Registry seconds are different."
endif

if ($SC_$CPU_TBL_LastUpdTime.$SC_$CPU_TBL_SUBSECONDS = $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_SYSTIME.$SC_$CPU_TBL_ST_SUBSECONDS) then
  write "<*> Passed - Table HK and Table Registry sub-seconds are the same."
else	
  write "<!> Failed - Table HK and Table Registry sub-seconds are different."
endif

wait 5

write ";*********************************************************************"
write "; Step 2.5: Create a table load image with an invalid spacecraft ID."
write ";*********************************************************************"
;;; Change the 4th & 8th byte of the table
$SC_$CPU_TST_TBL_TABLE2.element[4] = 2
$SC_$CPU_TST_TBL_TABLE2.element[8] = 4

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Table #2 invalid load 1", "dbl_tbl_ld_bad","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]","85")

write ";*********************************************************************"
write "; Step 2.6: Attempt to load the table image created above. "
write "; This should fail. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_SPACECRAFT_ID_ERR_EID,"ERROR", 1

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

errcnt = $SC_$CPU_TBL_CMDEC+1

start load_table ("dbl_tbl_ld_bad", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command with an invalid spacecraft ID failed as expected."
else
  write "<!> Failed - Load command with an invalid spacecraft ID did not increment the CMDEC."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_SPACECRAFT_ID_ERR_EID, "."
endif

write ";*********************************************************************"
write "; Step 2.7: Create a table load image with an invalid processor ID."
write ";*********************************************************************"
local badProcessorID = "CPU3"

if ("$CPU" = "CPU2") then
  badProcessorID = "CPU1"
elseif ("$CPU" = "CPU3") then
  badProcessorID = "CPU2"
endif

s create_tbl_file_from_cvt (badProcessorID,tbl_2_appid,"Table #2 invalid load 2","dbl_tbl_ld_bad2","TST_TBL.dflt_tbl_02","$SC_$CPU_TST_TBL_TABLE2.element[1]","$SC_$CPU_TST_TBL_TABLE2.element[10]")

write ";*********************************************************************"
write "; Step 2.8: Attempt to load the table image created above. "
write "; This should fail. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_PROCESSOR_ID_ERR_EID,"ERROR", 1

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

errcnt = $SC_$CPU_TBL_CMDEC+1

start load_table ("dbl_tbl_ld_bad2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command with an invalid processor ID failed as expected."
else
  write "<!> Failed - Load command with an invalid processor ID did not increment the CMDEC."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_PROCESSOR_ID_ERR_EID, "."
endif

write ";*********************************************************************"
write "; Step 3.0: Clean-up. Command a Power-On Reset on $CPU "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
