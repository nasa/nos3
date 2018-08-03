proc $SC_$CPU_tbl_integration
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose: The purpose of this procedure is to perform Build Integration
;          Testing of Table Services.  
;
; Note:	This test requires 2 Test Applications.  
;         1) Test Application A has a non-shared table and a 
;            shared table that it shares with Test Application B.
;         2) Test Application A must have at least one single and one double
;            buffer table 
;         3) Test Application A must have a table that has a validation function
;
;  History:
;  20NOV05    Initial development of this proc         MOB
;  17JAN06    Modifying proc to work with cFE 3.1      WFM
;  10JAN08    Updated for cFE 5.0                      SJJ
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%LIV(USE_ASK_POPUP) = TRUE

#include "cfe_platform_cfg.h"
#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_evs_events.h"
#include "cfe_tbl_events.h"
#include "tst_tbl_events.h"
#include "cfe_es_events.h"

local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt
local proc_error_cnt
local event_packet_ctr
local file_time
local app_info_file_index
local found_app
local num_free_buffers
local table4_initialized
local table2_loaded

LOCAL tbl_1_pkt, tbl_2_pkt
LOCAL tbl_1_appid, tbl_2_appid
LOCAL tbl_1_index, tbl_2_index, tbl_3_index, tbl_4_index

;**********************************************************************
; Set the local values 
;**********************************************************************
if ("$CPU" = "CPU1" OR "$CPU" = "") then
  tbl_1_pkt = "0FA2"
  tbl_2_pkt = "0FA3"
  tbl_1_appid = 4002
  tbl_2_appid = 4003
elseif ("$CPU" = "CPU2") then
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
write "; Step 1: Setup for the test of Table Services                        "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 1.1: Enable DEBUG Event Messages "
write ";*********************************************************************"

Write "*********  Enabling Debug Event Messages *********"

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
   write "<*> Success: Debug events have been enabled."
else
   write "<!> Error: Could not enable Debug events."
endif

wait 5

write ";*********************************************************************"
write "; Step 1.2: Load the Test Applications. "
write ";*********************************************************************"
write "BEFORE loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"

start $sc_$cpu_tst_tbl_apps_start ("1.2")

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
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.AppDef_tbl_03") then
    tbl_3_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.CriticalTbl_04") then
    tbl_4_index = i
  endif

  write "Table #",i," Name = ", $SC_$CPU_RF[i].$SC_$CPU_TBL_NAME 
enddo

write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index
write "Default Table #3 is at index #" & tbl_3_index
write "Default Table #4 is at index #" & tbl_4_index

wait 10

write ";*********************************************************************"
write "; Step 1.4: Dump the Table Registry and verify 4 tables exist. "
write ";*********************************************************************"
;  At this point a table registery should have been created with 4 tables in it
page $SC_$CPU_TBL_REGISTRY

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")

write "Waiting for LoadedOnce flag to be TRUE for table #1......"
wait until (p@$SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 1 has been loaded!"
write "Waiting for LoadedOnce flag to be TRUE for table #2......"
wait until (p@$SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 2 has been loaded!"
write "Waiting for LoadedOnce flag to be TRUE for table #3......"
wait until (p@$SC_$CPU_RF[tbl_3_index].$SC_$CPU_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 3 has been loaded!"
write "Waiting for LoadedOnce flag to be TRUE for table #4......"
wait until (p@$SC_$CPU_RF[tbl_4_index].$SC_$CPU_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 4 has been loaded!"

wait 10

write ";*********************************************************************"
write "; Step 2: Verify No-Op command (Command Code 0)"
write ";*********************************************************************"
ut_setupevt $SC, $CPU, CFE_TBL, CFE_TBL_NOOP_INF_EID, INFO

ut_sendcmd "$SC_$CPU_TBL_NOOP"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: No-Op command processed."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for NO-op command."
  endif
else
  write "<!> Error: Did not process No-Op command."
endif

wait 10

write ";*********************************************************************"
write "; Step 3: Verify Reset Counters Command (Command Code 1) "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 3.1:  Sending a command with an error so that the error counter"
write "; increments. An ES command with an invalid function code = hex 'aa'  "
write "; (42 decimal) is sent. "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_CC1_ERR_EID, "ERROR"

error_cmd_ctr = $SC_$CPU_TBL_CMDEC + 1

if ("$CPU" = "CPU1" OR "$CPU" = "") then
  ut_sendrawcmd "$SC_$CPU_TST_TBL", "1804c0000001aa"
elseif ("$CPU" = "CPU2") then
  ut_sendrawcmd "$SC_$CPU_TST_TBL", "1824c0000001aa"
elseif ("$CPU" = "CPU3") then
  ut_sendrawcmd "$SC_$CPU_TST_TBL", "1844c0000001aa"
endif

ut_tlmwait $SC_$CPU_TBL_CMDEC, {error_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Error counter incremented!"
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Invalid command."
  endif
else
  write "<!> Error: Error counter did not increment!"
endif

wait 10

write ";*********************************************************************"
write "; Step 3.2:  Sending the Reset Counters command"
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_RESET_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_RESETCTRS"

; Check if the CMDPC counter reset
ut_tlmwait $SC_$CPU_TBL_CMDPC, 0

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Command counter was reset!"
else
  write "<!> Error: Command counter did not reset!"
endif

ut_tlmwait $SC_$CPU_TBL_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Error counter was reset!"
else
  write "<!> Error: Error counter did not reset!"
endif

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  Write "<!> Error: Event Message not received for Reset Counters command."
endif

wait 10

write ";*********************************************************************"
write "; Step 4: Verify TBL Dump Table Command (Command Code 3)              "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 4.1: Dump the active table for table #1.     "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","A","table_01_active","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

; Verify the the data bytes are 01 02 03 04 05 06 07 08 09 10
; This will change with the new TST_TBL apps
; print the dumped table elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

write ";*********************************************************************"
write "; Step 4.2: Load a table for Table #1 in order to dump the inactive   "
write "; buffer. This step loads a table which will pass validation.         "
write "; This new table has 10 bytes worth of data "
write ";   01 02 03 04 05 06 07 08 09 10 "
write ";*********************************************************************"
success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1

ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO"

; Load the file dumped in the previous step in order to
; populate the inactive buffer
start load_table ("table_01_active", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

; Dump the table registry to see if load actually occurred

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10
 
; Verify the LoadinProgress Flag
write "Waiting for LOADBUFFERID to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Success: LOADBUFFERID indicates a load is pending."

write ";*********************************************************************"
write "; Step 4.3: Dump the inactive table for table #1. "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","I","tbl_01_inactive","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

; print the dumped table elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;****************************************
;  Verify that the files are the same????
;  HOW????
;****************************************

write ";*********************************************************************"
write "; Step 4.4: Dump the inactive table for table #2. "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_02","I","tbl_02_inactive","$CPU",tbl_2_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

write ";*********************************************************************"
write "; Step 4.5: Dump the active table for table #2.  "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_02","A","tbl_02_active","$CPU",tbl_2_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10
   
; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

write ";*********************************************************************"
write "; Step 4.6: Send the Load Abort command for Table #1 in order to clean"
write "; up after the ResetCtrs and Error tests. "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Load Abort command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load Abort command."
  endif
else
  write "<!> Error: Load Abort for Table #1 failed."
endif

wait 10

; Dump the table registry

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")

wait 10

; add validation for the Load in progress tlm point
write "Waiting for LOADBUFFERID to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID = -1) 
write "<*> Success: LOADBUFFERID indicates abort command was successful"

wait 10

write ";*********************************************************************"
write "; Step 5:  Verify TBL Load Table Command (Command Code 2) "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 5.1: Load a full table for Table 1.                            "
write "; This step loads a table which will pass validation. The test app    "
write "; verifies that the every element is between 01 and 10.               "
write "; This new table has 10 bytes worth of data "
write ";     10 09 08 07 06 05 04 03 02 01 "
write ";*********************************************************************"

; Load the desired values into the table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 10
$SC_$CPU_TST_TBL_TABLE1.element[2] = 9
$SC_$CPU_TST_TBL_TABLE1.element[3] = 8
$SC_$CPU_TST_TBL_TABLE1.element[4] = 7
$SC_$CPU_TST_TBL_TABLE1.element[5] = 6
$SC_$CPU_TST_TBL_TABLE1.element[6] = 5
$SC_$CPU_TST_TBL_TABLE1.element[7] = 4
$SC_$CPU_TST_TBL_TABLE1.element[8] = 3
$SC_$CPU_TST_TBL_TABLE1.element[9] = 2
$SC_$CPU_TST_TBL_TABLE1.element[10] = 1

start create_tbl_file_from_cvt ("$CPU",tbl_1_appid, "Full Table load", "tbl_01_valid_load","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

wait 5

success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1

ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO"

start load_table ("tbl_01_valid_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then

  write "<*> Success: Table Load command."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Free shared buffers = ", num_free_buffers
else
  write "<!> Error: Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/tbl_01_valid_load") then
	write "<*> Success: the correct file was loaded."
else
	write "<!> Error: the expected file was not loaded."
endif

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")

write "Waiting for LOADBUFFERID to change for table #1......"
 
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1) 
write "<*> Success: LOADBUFFERID changed."

wait 10
  
write ";*********************************************************************"
write "; Step 5.2: Dump the active Table 1 to make sure the table loaded.  "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","ACTIVE","tbl_01_52active","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10

write ";*********************************************************************"
write "; Step 5.3: Validate the inactive Table 1.  "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: InActive Table #1 validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table #1 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO"

  wait 20

  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Inactive Table #1 validated successfully."
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Inactive Table #1 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
    write "<*> Success: Inactive Table #1 validated successfully."
    write "<*> Success: Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif

;*******************************************************
; Can we automatically compare these 2 files? 
;*******************************************************

wait 10

write ";*********************************************************************"
write "; Step 5.4: Activate Table 1.                                         "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table #1 activate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table #1 activation failed."
endif

wait 20
; Look for the Update Success event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Success: Table #1 Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Error: Table #1 update failed."
  Write "<!> Error: Event Message not received for activate command."
endif

wait 10

write ";*********************************************************************"
write "; Step 5.5: Dump the Table Registry."
write ";*********************************************************************"

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")

wait 10
 
write ";*********************************************************************"
write "; Step 5.6: Dump active Table 1 and verify it is in use.              "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","ACTIVE","tbl_01_56active","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

;  Verify #available shared buffers is 4
if ($SC_$CPU_TBL_NumFreeshrBuf = 4) then
  write "<*> Success: There are 4 available shared buffers."
else
  write "<!> Error: There are ", $SC_$CPU_TBL_NumFreeshrBuf, " available shared buffers."
endif

;  What should I expect the Last Status Value to be? 
;  What is the last valid table name?

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_TBL_LastFileLoaded = "/ram/tbl_01_valid_load") then
  write "<*> Success: The correct file was loaded into Table #1."
else
  write "<!> Error: Table #1 was loaded with ", $SC_$CPU_TBL_LastFileLoaded
endif

wait 10

write ";*********************************************************************"
write "; Step 5.7: Load a partial table for Table #2. This table should be a "
write "; double buffered table."
write ";*********************************************************************"
ut_setupevt $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO

; Dump the active table - this was done in the previous step
; make a change to 2 bytes
$SC_$CPU_TST_TBL_TABLE2.element[3] = 1
$SC_$CPU_TST_TBL_TABLE2.element[4] = 2

; write new load file
s create_tbl_file_from_cvt ("$CPU",tbl_2_appid, "Partial Table load", "tbl_02_partial_load","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[3]", "$SC_$CPU_TST_TBL_TABLE2.element[4]")

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1

; Load Table #2 with the file written above
start load_table ("tbl_02_partial_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

write ";**********************************************************************"
write "; Step 5.8 : Validate the inactive buffer for Table #2. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2
                                                    
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Inactive Table #2 validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Error: InActive Table #2 validation failed."
endif
                                                                                
; Look for the Validation success event
wait 20
                                                                                
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Success: Inactive Table #2 validated."
  write "<*> Success: Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Error: Inactive Table #2 validation failed."
  write "<!> Error: Event Message not received for validation command."
endif

;; Look at the LastValBuf to see if it indicates INACTIVE
if (p@$SC_$CPU_TBL_LASTVALBUF = "Inactive") then
  write "<*> Success: TBL HK indicates the Inactive buffer was validated."
else
  write "<!> Error: TBL HK does not indicate the Inactive buffer was validated."
endif

wait 10


; Activate the load
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table #2 activate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table #2 activation failed."
endif

wait 20
; Look for the Update Success event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Success: Table #2 Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Error: Table #2 update failed."
  Write "<!> Error: Event Message not received for activate command."
endif

wait 10

; Dump the Table Registry

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Wait until the load has been processed
write "Waiting for table #2 to load....."
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID = -1)

; verify that the loaded file is now being used 
if ($SC_$CPU_TBL_LastFileLoaded = "/ram/tbl_02_partial_load") then
  write "<*> Success: Activation loaded the correct file into table #2."
else
  write "<!> Error: Activation failed to load the correct file into table #2."
endif

wait 10
; What should I expect the Last Status Value to be?
; What is the last valid table name?

write ";*********************************************************************"
write "; Step 6: Verify TBL Validate Table Command (Command Code 4)          "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 6.1: Load table #1 with a table file that will fail "
write "; validation."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

; Dump the Active table for Table #1
s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","ACTIVE","tbl_01_61active","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

; Modify the first byte so that it fails validation
$SC_$CPU_TST_TBL_TABLE1.element[1] = x'36'

; Write the file
s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Invalid Table load", "tbl_01_invalid","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

ut_setupevt $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO

; Setup the counters to look for
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1

; Load Table #1 with the file written above
start load_table ("tbl_01_invalid", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  wait 10
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

; Verify that the # of available shared buffers is one less than before
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Free shared buffers = ", num_free_buffers
else
  write "<!> Error: Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

wait 10

; Dump the table registry

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Dump the inactive table to verify its contents
s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","INACTIVE","tbl_01_61inactive","$CPU",tbl_1_pkt)

; Check if the first byte is what we set it to
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = x'36') then
  Write "<*> Success: Inactive buffer contains our load file."
else
  Write "<!> Error: Inactive buffer does not contain the specified load file."
endif

wait 10

write ";*********************************************************************"
write "; Step 6.2: Validate the Inactive table for table #1. This should     "
write "; fail validation."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: InActive Table #1 validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table #1 validation failed."
endif

; Look for the Validation failure event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_ERR_EID) then
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR"

  wait 20

  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Inactive Table #1 failed validation."
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Inactive Table #1 Passed validation when it should have failed."
    Write "<!> Error: Expected Event Msg not received for validation command."
  endif
else
  write "<*> Success: Inactive Table #1 failed validation."
  write "<*> Success: Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif

wait 10

write ";*********************************************************************"
write "; Step 6.3: Load table #1 with a table file that will pass "
write "; validation.  "
write ";*********************************************************************"

success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1
; Number of shared buffers should stay the same
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf

ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO"

start load_table ("tbl_01_valid_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then

  write "<*> Success: Table Load command."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Free shared buffers = ", num_free_buffers
else
  write "<!> Error: Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/tbl_01_valid_load") then
	write "<*> Success: the correct file was loaded."
else
	write "<!> Error: the expected file was not loaded."
endif

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")

write "Waiting for LOADBUFFERID to change for table #1......"
 
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1) 
write "<*> Success: LOADBUFFERID changed."

; if ($SC_$CPU_num_found_messages = 1) then
;   Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
; else
;   Write "<!> Error: Event Message not received for Registry Dump command."
; endif

wait 10
  
write ";*********************************************************************"
write "; Step 6.4: Dump the active Table 1 to make sure the table loaded.  "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","ACTIVE","tbl_01_64active","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10

write ";*********************************************************************"
write "; Step 6.5: Validate the inactive Table 1.  This should pass."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: InActive Table #1 validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table #1 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO"

  wait 20

  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Inactive Table #1 validated successfully."
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Inactive Table #1 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
    write "<*> Success: Inactive Table #1 validated successfully."
    write "<*> Success: Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif

write ";*********************************************************************"
write "; Step 6.6: Load table #2 with the table file that will pass "
write "; validation."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

; Dump the Active table for Table #2
s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_02","ACTIVE","tbl_02_66active","$CPU",tbl_2_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

table2_loaded = 0
wait 3 

ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO"

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1

; Load the valid table 2
start load_table ("tbl_02_66active", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    table2_loaded = 1
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

; Dump the Table Registry

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Wait until the load has been processed
write "Waiting for table #2 to load....."
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)

write ";*********************************************************************"
write "; Step 6.7: Validate the Inactive table for table #2. This should pass"
write "; validation."
write ";*********************************************************************"
; If table #2 loaded successfully, validate the buffer
if (table2_loaded = 1) then
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG"

  ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Success: InActive Table #2 validate command sent."
    if ($SC_$CPU_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    else
      Write "<!> Error: Event Message not received for Validate command."
    endif
  else
    write "<!> Error: InActive Table #2 validation failed."
  endif

  ; Look for the Validation success event
  if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
    ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO"
  
    wait 20
  
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Success: Inactive Table #2 validated successfully."
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    else
      write "<!> Error: Inactive Table #2 validation failed."
      Write "<!> Error: Event Message not received for validation command."
    endif
  else
    write "<*> Success: Inactive Table #2 validated."
    write "<*> Success: Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "*** Inactive Table #2 validation skipped because the file load command failed in the previous step."
endif

wait 10

write ";*********************************************************************"
write "; Step 6.8: Validate the Active table for table #1. This should pass"
write "; validation."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_VALIDATE ACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Active Table #1 validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table #1 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO"

  wait 20

  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Active Table #1 validated."
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Active Table #1 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
  write "<*> Success: Active Table #1 validated."
  write "<*> Success: Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif

wait 10

write ";*********************************************************************"
write "; Step 6.9: Validate the Active table for table #2. This should pass"
write "; validation."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_VALIDATE ACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Active Table #2 validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: Active Table #2 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO"
  
  wait 20
  
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Active Table #2 validated successfully."
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Active Table #2 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
  write "<*> Success: Active Table #2 validated."
  write "<*> Success: Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif

wait 10

write ";*********************************************************************"
write "; Step 7: Verify TBL Activate Table Command (Command Code 5)          "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 7.1: Activate Table #1 and verify that the table was activated.  "
write "; Dump Active Table #1 and verify that the loaded file is being used. "
write ";*********************************************************************"


ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table #1 activate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table #1 activation failed."
endif

wait 20
; Look for the Update Success event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Success: Table #1 Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Error: Table #1 update failed."
  Write "<!> Error: Event Message not received for activate command."
endif

wait 10

; Dump the Table Registry

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Dump Table #1 Active Table
; Wait until the load has been processed
write "Waiting for table #1 to load....."
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID = -1)

;;; Dump the active buffer
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"

s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_01","A","tbl_01_active","$CPU",tbl_1_pkt)

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
  else
    write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

; print the dumped table #1 elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

; verify that the loaded file is now being used 
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LastFileUpd = "/ram/tbl_01_valid_load") then
  write "<*> Success: Activation loaded the correct file into table #1."
else
  write "<!> Error: Activation failed to load the correct file into table #1."
  write "table loaded was: ", $SC_$CPU_TBL_LastFileLoaded
endif

wait 10

write ";*********************************************************************"
write "; Step 7.2: Activate Table #2 and verify that the table was activated."
write "; Dump Active Table #2 and verify that the loaded file is being used. "
write ";*********************************************************************"
; Only activate Table #2 if the load from step 6.3 worked
if (table2_loaded = 1) then
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2
  
  ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=""TST_TBL.dflt_tbl_02"""
  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Success: Table #2 activate command sent."
    if ($SC_$CPU_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    else
      Write "<!> Error: Event Message not received for Activate command."
    endif
  else
    write "<!> Error: Table #2 activation failed."
  endif
  
  wait 20
  ; Look for the Update Success event
  if ($SC_$CPU_find_event[2].num_found_messages = 1) then
    write "<*> Success: Table #2 Updated successfully."
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  else
    write "<!> Error: Table #2 update failed."
    Write "<!> Error: Event Message not received for activate command."
  endif
  
  wait 10
  
  ; Dump the Table Registry
  
  s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
  wait 10
  
  ; Dump Table #2 Active Table
  
  ;;; Dump the active buffer
  ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_WRITE_DUMP_INF_EID, "INFO"
  
  s get_tbl_to_cvt ("RAM:0","TST_TBL.dflt_tbl_02","A","tbl_02_active","$CPU",tbl_2_pkt)
  
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
      write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
    else
      write "<!> Error: Expected Event Msg not received for Dump command."
    endif
  endif
  
  wait 10 
  
  ; print the dumped table #2 elements
  write "Table #2 Active Buffer dumped:"
  for i = 1 to 10 do
    write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
  enddo
  
  ; verify that the loaded file is now being used 
  if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LastFileUpd = "/ram/tbl_02_66active") then
    write "<*> Success: Activation loaded the correct file into table #1."
  else
    write "<!> Error: Activation failed to load the correct file into table #1."
  endif
  
  wait 10
  
else
  write "Table #2 activation skipped."
endif
; Dump Table #2 Active Table -- WHY???
; verify that the loaded file is now being used
; NOTE: that the App B for the shared table will send an event msg when it gets the "table has been updated status".

wait 10
  
write ";*********************************************************************"
write "; Step 8: Verify TBL Dump Registry Command (Command Code 6)           "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_OVERWRITE_REG_DUMP_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_WRITEREG2FILE DUMPFILENAME="""""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Registry successfully dumped."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Table Registry command."
  endif
else
  Write "<!> Error: Table Registry Dump command failed."
endif

wait 10
 
;  Verify that all of the test app tables are there
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_01" AND  ;;
    $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Success: Table Registry contains the correct tables."
else
  write "<!> Error: Table Registry verification failed. The correct tables are not loaded."
endif

wait 10

write ";*********************************************************************"
write "; Step 9: Verify TBL Telemeter Table Registry Entry Command "
write "; (Command Code 7) Send command to telemeter the entry for Table #2  "
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_TLM_REG_CMD_INF_EID, "DEBUG"

ut_sendcmd "$SC_$CPU_TBL_TLMREG RTTABLENAME=""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Telemeter Registry command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  Write "<!> Error: Table Telementer Registry command failed."
endif

wait 10

write ";*********************************************************************"
write "; Step 10: Verify Command to Abort a Load (Command Code 9)            "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 10.1: Load Table #1 using the file dumped in Step 10.2.        "
write ";*********************************************************************"
success_cmd_ctr = $SC_$CPU_TBL_CMDPC + 1

ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO"

start load_table ("tbl_01_active", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

write ";*********************************************************************"
write "; Step 10.2: Dump the Table Registry to verify a load is pending.     "
write ";*********************************************************************"

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Verify the Load IP field
write "Waiting for LOADBUFFERID to change......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1) 
write "<*> Success: LOADBUFFERID changed"

wait 10

write ";*********************************************************************"
write "; Step 10.3: Send the Load Abort command for table #1."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO"

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Load Abort command sent."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load Abort command."
  endif
else
  write "<!> Error: Load Abort for Table #1 failed."
endif

wait 10 

write ";*********************************************************************"
write "; Step 10.4: Dump the Table Registry to verify the load has aborted.  "
write ";*********************************************************************"

s get_file_to_cvt ("RAM:0", "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Verify the Load IP field
write "Waiting for LOADBUFFERID to change......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID = -1) 
write "<*> Success: LOADBUFFERID changed"

wait 10

write ";*********************************************************************"
write "; Step 10.5: Dump the inactive table for table #1 and verify that the "
write "; inactive buffer is empty.          " 
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID, "ERROR"

ut_sendcmd "$SC_$CPU_TBL_DUMP INACTIVE DTABLENAME=""TST_TBL.dflt_tbl_01"" DFILENAME=""RAM:/tbl_01_inactive"""

if (UT_SC_Status = UT_SC_CmdFailure) then
  write "<*> Success: Inactive Table #1 dump generated the expected error."
  if ($SC_$CPU_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Error: Expected Error Event not received."
  endif
else
  write "<!> Error: Inactive Table #1 dump succeeded when a failure was expected."
endif

wait 10 

;  verify the # of shared buffers available is 4
if ($SC_$CPU_TBL_NumFreeshrBuf = 4) then
  write "<*> Success: There are 4 available shared buffers."
else
  write "<!> Error: There are ", $SC_$CPU_TBL_NumFreeshrBuf, " available shared buffers. Expected 4 buffers."
endif

write ";*********************************************************************"
write "; Step 11: Verify Command to Delete a Critical Table (Command Code 8) "
write ";*********************************************************************"
write "; Step 11.1: Verify that the table is a Critical Table and attempt to"
write ";              delete it from the CDS."
write ";**********************************************************************"
page $SC_$CPU_ES_CDS_REGISTRY

;;;; Dump the CDS Registry in order to verify that the Critical
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_CDS_REG_DUMP_INF_EID, "DEBUG"

s get_file_to_cvt ("RAM:0", "cfe_cds_reg.log", "$sc_$cpu_cds_reg.log", "$CPU")
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: CDS Registry Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Error: CDS Registry Info retrieval. Expected Event message ",CFE_ES_CDS_REG_DUMP_INF_EID, " was not received"
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

;; Verify that the table is flagged Critical
if (found_CDS = TRUE) then
  write "<*> Success: Table found in CDS Registry."
  if (p@$SC_$CPU_ES_CDSREG[cdsRegIdx].CRITICALTBL = "TRUE") then
    write "<*> Success: Critical Table flag marked TRUE"
  else
    write "<!> Error: Critical Table flag marked FALSE"
  endif
else
  write "<!> Error: Table NOT found in CDS Registry."
endif

;; Try to delete the table before unregistering the table
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_IN_REGISTRY_ERR_EID, "ERROR"

error_cmd_ctr = $SC_$CPU_TBL_CMDEC+1

ut_sendcmd "$SC_$CPU_TBL_DELETECDS Tablename=""TST_TBL.CriticalTbl_04"""
if (UT_SC_Status = UT_SC_CmdFailure) then
  write "<*> Success: Table Delete Critical Data Store command failed as expected."
  if ($SC_$CPU_num_found_messages = 1) then
     Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
     Write "<!> Error: Expected Event message ", CFE_TBL_IN_REGISTRY_ERR_EID, " was not received"
  endif
else
   Write "<!> Error: Table Delete Critical Data Store command succeeded when it should have failed."
endif

wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {error_cmd_ctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Delete Critical Data Store incremented the error counter as expected."
else
  write "<!> Error: The TBL_DeleteCDS command did not increment the error counter."
endif

write ";**********************************************************************"
write "; Step 11.2: Send a command to unregister the Critical Table. The "
write "; 	      table must be unregistered BEFORE deleting the CDS."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGSFREETBL_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_EID, INFO, 2

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="CriticalTbl_04"
wait 10

;; Verify the expected event messages were generated
if ($SC_$CPU_num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Success: Expected Event Messages ",$SC_$CPU_find_event[1].eventid, " and ", $SC_$CPU_find_event[2].eventid, " received."
else
  write "<!> Error: Expected Event Messages ", TST_TBL_UNREGSFREETBL_EID," and ",TST_TBL_UNREGISTERTBL_EID," not received."
endif

write ";**********************************************************************"
write "; Step 11.3: Send a command to delete the Critical Table CDS before "
write "; 	      stopping the TST_TBL application."
write ";**********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID, "ERROR"

error_cmd_ctr = $SC_$CPU_TBL_CMDEC+1

ut_sendcmd "$SC_$CPU_TBL_DELETECDS Tablename=""TST_TBL.CriticalTbl_04"""
if (UT_SC_Status = UT_SC_CmdFailure) then
  write "<*> Success: Table Delete Critical Data Store command failed as expected."
  if ($SC_$CPU_num_found_messages = 1) then
     Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
     Write "<!> Error: Expected Event message ",CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID, " was not received"
  endif
else
   Write "<!> Error: Table Delete Critical Data Store command succeeded when it should have failed."
endif

wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {error_cmd_ctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: The Critical Table delete incremented the error counter as expected."
else
  write "<!> Error: The TBL_DeleteCDS command did not increment the error counter."
endif

write ";**********************************************************************"
write "; Step 11.4: Send a command to delete the TST_TBL application. The "
write "; application that owns the CDS must not be running before deleting it."
write ";**********************************************************************"
;; Send the ES_DeleteApp command for TST_ES
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_INF_EID, "INFO", 2

success_cmd_ctr = $SC_$CPU_ES_CMDPC + 1

; Delete the TST_TBL application
ut_sendcmd "$SC_$CPU_ES_DELETEAPP APPLICATION=""TST_TBL"""
if (UT_SC_Status = UT_SC_Success) then
   write "<*> Success: ES_DeleteApp command succeeded."
   if ($SC_$CPU_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      Write "<!> Error: Expected Event message ", CFE_ES_STOP_DBG_EID, " was not received"
   endif
else
   Write "<!> Error: ES_DeleteApp command failed!"
endif

wait 10

ut_tlmwait $SC_$CPU_ES_CMDPC, {success_cmd_ctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: ES_DeleteApp command executed properly."
else
  write "<!> Error: ES_DeleteApp command failed."
endif

;; Check if the second event was rcv'd (may need to wait for this one)
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Success: Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Error: Expected Event Message ",CFE_ES_STOP_INF_EID," not received."
endif

;; Dump all running apps again to verify that the TST_TBL app is not running
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_ALL_APPS_EID, "DEBUG"
s get_file_to_cvt ("RAM:0","cfe_es_app_info.log","$sc_$cpu_es_app_info.log","$CPU")
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: App Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Error: App Info retrieval. Expected Event message ",CFE_ES_ALL_APPS_EID, " was not received"
endif

;; Verify that TST_TBL application is no longer listed
local found_app1 = FALSE
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_TBL") then
    found_app1 = TRUE
  endif
enddo

if (found_app1 = FALSE) then
  write "<*> Success: TST_TBL application has been deleted."
else
  write "<!> Error: TST_TBL application is still running."
endif

write ";**********************************************************************"
write "; Step 11.5: Send a command to delete the Critical Table. Verify the "
write "; the table is deleted. This table should be recreated by the Power-On"
write "; reset performed below."
write ";**********************************************************************"
ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_CDS_DELETED_INFO_EID, "INFO"

success_cmd_ctr = $SC_$CPU_TBL_CMDPC+1


ut_sendcmd "$SC_$CPU_TBL_DELETECDS Tablename=""TST_TBL.CriticalTbl_04"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Delete Critical Data Store command succeeded."
  if ($SC_$CPU_num_found_messages = 1) then
     Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
     Write "<!> Error: Expected Event message ",CFE_TBL_CDS_DELETED_INFO_EID, " was not received"
  endif
else
   Write "<!> Error: Table Delete Critical Data Store command failed!"
endif

wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {success_cmd_ctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: The Critical Table was deleted."
else
  write "<!> Error: The TBL_DeleteCDS command did not increment the valid command counter."
endif

wait 10
;;;; Dump the CDS Registry in order to verify that the Critical
;;;; Table was deleted
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_CDS_REG_DUMP_INF_EID, "DEBUG"
s get_file_to_cvt ("RAM:0", "cfe_cds_reg.log", "$sc_$cpu_cds_reg.log", "$CPU")
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: CDS Registry Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Error: CDS Registry Info retrieval. Expected Event message ",CFE_ES_CDS_REG_DUMP_INF_EID, " was not received"
endif
                                                                                
;; Check if the CDS is in the Registry
local found_CDS = FALSE
for cds_index = 1 to CFE_ES_CDS_MAX_NUM_ENTRIES do
  if ($SC_$CPU_ES_CDSREG[cds_index].CDSName = "TST_TBL.CriticalTbl_04") then
    found_CDS = TRUE
  endif
enddo

if (found_CDS = FALSE) then
  write "<*> Success: TST_TBL.CriticalTbl_04 was not found in CDS Registry."
else
  write "<!> Error: TST_TBL.CriticalTbl_04 still exists in the CDS Registry."
endif

write ";**********************************************************************"
write "; Step 11.6: Send a command to delete a CDS that is not a Critical "
write "; 	    table."
write ";**********************************************************************"
;; Start the TST_ES application
write "; Starting the TST_ES application. "
s load_start_app ("TST_ES", "$CPU")
wait 10

;;;; Dump the CDS Registry in order to verify that the table isn't flagged Critical
ut_setupevt "$SC", "$CPU", "CFE_ES", CFE_ES_CDS_REG_DUMP_INF_EID, "DEBUG"

s get_file_to_cvt ("RAM:0", "cfe_cds_reg.log", "$sc_$cpu_cds_reg.log", "$CPU")
wait 10

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Success: CDS Registry Info retrieval. Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Error: CDS Registry Info retrieval. Expected Event message ",CFE_ES_CDS_REG_DUMP_INF_EID, " was not received"
endif
                                                                                
;; Check if the CDS is in the Registry
found_CDS = FALSE
cdsRegIdx = 0
for cds_index = 1 to CFE_ES_CDS_MAX_NUM_ENTRIES do
  if ($SC_$CPU_ES_CDSREG[cds_index].CDSName = "TST_ES.TSTCDS") then
    found_CDS = TRUE
    cdsRegIdx = cds_index
  endif
enddo

;; Verify that the table is not flagged Critical
if (found_CDS = TRUE) then
  write "<*> Success: Table found in CDS Registry."
  if (p@$SC_$CPU_ES_CDSREG[cdsRegIdx].CRITICALTBL = "TRUE") then
    write "<!> Error: Critical Table flag marked TRUE"
  else
    write "<*> Success: Critical Table flag marked FALSE"
  endif
else
  write "<!> Error: Table NOT found in CDS Registry."
endif

ut_setupevt "$SC", "$CPU", "CFE_TBL", CFE_TBL_NOT_IN_CRIT_REG_ERR_EID, "ERROR"

error_cmd_ctr = $SC_$CPU_TBL_CMDEC+1


ut_sendcmd "$SC_$CPU_TBL_DELETECDS Tablename=""TST_ES.TSTCDS"""
if (UT_SC_Status = UT_SC_CmdFailure) then
  write "<*> Success: Table Delete Critical Data Store command failed as expected."
  if ($SC_$CPU_num_found_messages = 1) then
     Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
     Write "<!> Error: Expected Event message ",CFE_TBL_NOT_IN_CRIT_REG_ERR_EID, " was not received"
  endif
else
   Write "<!> Error: Table Delete Critical Data Store command succeeded when it should have failed."
endif

wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {error_cmd_ctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Success: The Critical Table delete incremented the error counter as expected."
else
  write "<!> Error: The TBL_DeleteCDS command did not increment the error counter."
endif

write ";**********************************************************************"
write "; Step 11.7: Perform a Power-on Reset. "
write ";**********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 80

cfe_startup $CPU
wait 5

wait 10 
 
     Write "Test complete"
ENDPROC
