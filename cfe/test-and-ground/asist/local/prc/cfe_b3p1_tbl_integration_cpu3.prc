proc cfe_b3p1_tbl_integration_cpu3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose: The purpose of this procedure is to perform Build Integration
;          Testing of Table Services.  
;
; Note:	   At the time of the writing of this procedure, there were no utilities
;	   to look for a particular event message. 
;	   Also note that this procedure is written for CPU3.
;     This test requires  2 Test Applications.  
;         1) Test Application A has a non-shared table and a 
;            shared table that it shares with Test Application B.
;         2) Test Application A must have at least one single and one double
;            buffer table 
;         3) Test Application A must have a table that has a validation function
;
;  History:
;  20NOV05    Initial development of this proc         MOB
;  17JAN06    Modifying proc to work with cFE 3.1      WFM
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%LIV(USE_ASK_POPUP) = TRUE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"

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

write ";*********************************************************************"
write "; Step 1: Setup for the test of Table Services                        "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 1.1: Enable DEBUG Event Messages "
write ";*********************************************************************"
;  In build 3, Event Messages of type DEBUG are disabled by default.

ut_setupevt "SCX", "CPU3", "CFE_EVS", 20, "DEBUG"
                                                                                
ut_sendcmd "SCX_CPU3_EVS_ENAEVENTTYPE DEBUG"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Debug events have been enabled."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Error: Could not enable Debug events."
endif

write ";*********************************************************************"
write "; Step 1.2: Load the Test Applications. "
write ";*********************************************************************"
write "BEFORE loading, there are " & SCX_CPU3_TBL_NUMTABLES & " tables registered"

start tst_tbl_apps_start_cpu3 ("1.2")

Write "AFTER loading, there are " & SCX_CPU3_TBL_NUMTABLES & " tables registered"

write ";*********************************************************************"
write "; Step 1.3:  Determine if Table 4 is initialized. If not, do it.      "
write ";*********************************************************************"
; Determine if table 4 has been initialized yet
table4_initialized = 0

if (p@SCX_CPU3_RF[4].SCX_CPU3_TBL_LOADEDONCE <> "TRUE") then
  Write "Loading the file that contains the default values for table 4."
  write "FTP the file to FSW (file = deflt_tbl_04)"
  s proc_ftp ("RAM:","deflt_tbl_04","deflt_tbl_04","CPU3","P")
  wait 10

  ut_setupevt "SCX", "CPU3", "CFE_TBL", 35, "DEBUG"

  write "Sending the TST_TBL_LOADFILE command "
  /SCX_CPU3_TST_TBL_LOADFILE

  wait 10

  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
    table4_initialized = 1
  else
    Write "<!> Error: Event Msg ",SCX_CPU3_evs_eventid," received for Load File command. Expected Load Success Event (35)."
    table4_initialized = 0
  endif
elseif (p@SCX_CPU3_RF[4].SCX_CPU3_TBL_LOADEDONCE = "TRUE") then
  table4_initialized = 1
endif 

wait 10

write ";*********************************************************************"
write "; Step 1.4: Dump the Table Registry and verify 4 tables exist. "
write ";*********************************************************************"
;  At this point a table registery should have been created with 4 tables in it
page SCX_CPU3_TBL_REGISTRY

ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Dump command."
endif

write "Waiting for LoadedOnce flag to be TRUE for table #1......"
wait until (p@SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 1 has been loaded!"
write "Waiting for LoadedOnce flag to be TRUE for table #2......"
wait until (p@SCX_CPU3_RF[2].SCX_CPU3_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 2 has been loaded!"
write "Waiting for LoadedOnce flag to be TRUE for table #3......"
wait until (p@SCX_CPU3_RF[3].SCX_CPU3_TBL_LOADEDONCE = "TRUE")
Write "<*> Success: Table 3 has been loaded!"
;**************************************************************
; Only check for this if the Load Succeeded in previous step
;**************************************************************
if (table4_initialized = 1) then
  write "Waiting for LoadedOnce flag to be TRUE for table #4......"
  wait until (p@SCX_CPU3_RF[4].SCX_CPU3_TBL_LOADEDONCE = "TRUE")
  Write "<*> Success: Table 4 has been loaded!"
endif

wait 10

write ";*********************************************************************"
write "; Step 2: Verify No-Op command (Command Code 0)"
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 10, "INFO"

ut_sendcmd "SCX_CPU3_TBL_NOOP"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: No-Op command processed."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
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
ut_setupevt "SCX", "CPU3", "CFE_TBL", 51, "ERROR"

error_cmd_ctr = SCX_CPU3_TBL_CMDEC + 1

ut_sendrawcmd "SCX_CPU3_TST_TBL", "1844c0000001aa"

ut_tlmwait SCX_CPU3_TBL_CMDEC, {error_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Error counter incremented!"
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
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
ut_setupevt "SCX", "CPU3", "CFE_TBL", 11, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_RESETCTRS"

; Check if the CMDPC counter reset
ut_tlmwait SCX_CPU3_TBL_CMDPC, 0

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Command counter was reset!"
else
  write "<!> Error: Command counter did not reset!"
endif

ut_tlmwait SCX_CPU3_TBL_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Success: Error counter was reset!"
else
  write "<!> Error: Error counter did not reset!"
endif

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
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
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","A","tbl_01_active","CPU3","0FE0")

if (SCX_CPU3_num_found_messages = 1) then
  write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

; Verify the the data bytes are 61 00 61 00 61 00 63 00
; This will change with the new TST_TBL apps
write "Table #1 Data dumped:"
write "    1st byte = ", SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE
write "    2nd byte = ", SCX_CPU3_TST_TBL_TABLE1.SECONDBYTE
write "    3rd byte = ", SCX_CPU3_TST_TBL_TABLE1.THIRDBYTE
write "    4th byte = ", SCX_CPU3_TST_TBL_TABLE1.FOURTHBYTE
write "    5th byte = ", SCX_CPU3_TST_TBL_TABLE1.FIFTHBYTE
write "    6th byte = ", SCX_CPU3_TST_TBL_TABLE1.SIXTHBYTE
write "    7th byte = ", SCX_CPU3_TST_TBL_TABLE1.SEVENTHBYTE
write "    8th byte = ", SCX_CPU3_TST_TBL_TABLE1.EIGHTHBYTE

write ";*********************************************************************"
write "; Step 4.2: Load a table for Table #1 in order to dump the inactive   "
write "; buffer. This step loads a table which will pass validation.         "
write "; This new table has 8 bytes worth of data - 61 00 61 00 61 00 63 00  "
write ";*********************************************************************"
success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

; Load the file dumped in the previous step in order to
; populate the inactive buffer
start load_table ("table_01_active", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

; Dump the registry to see if load actually occurred
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 5
 
if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

wait 5

; add validation for the Load in progress tlm point
write "Waiting for LoadInProgress flag to change for table #1......"
wait until (SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADINPROGRESS <> -1) 
write "<*> Success: LoadInProgress flag indicates a load is pending."

write ";*********************************************************************"
write "; Step 4.3: Dump the inactive table for table #1. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","I","tbl_01_inactive","CPU3","0FE0")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

write "Table #1 Data dumped:"
write "    1st byte = ", SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE
write "    2nd byte = ", SCX_CPU3_TST_TBL_TABLE1.SECONDBYTE
write "    3rd byte = ", SCX_CPU3_TST_TBL_TABLE1.THIRDBYTE
write "    4th byte = ", SCX_CPU3_TST_TBL_TABLE1.FOURTHBYTE
write "    5th byte = ", SCX_CPU3_TST_TBL_TABLE1.FIFTHBYTE
write "    6th byte = ", SCX_CPU3_TST_TBL_TABLE1.SIXTHBYTE
write "    7th byte = ", SCX_CPU3_TST_TBL_TABLE1.SEVENTHBYTE
write "    8th byte = ", SCX_CPU3_TST_TBL_TABLE1.EIGHTHBYTE

;****************************************
;  Verify that the files are the same????
;  HOW????
;****************************************

write ";*********************************************************************"
write "; Step 4.4: Dump the inactive table for table #2. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_02","I","tbl_02_inactive","CPU3","0FE1")

if (SCX_CPU3_num_found_messages = 1) then
  write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10

write "Table #2 Data dumped:"
write "    1st byte = ", SCX_CPU3_TST_TBL_TABLE2.FIRSTBYTE
write "    2nd byte = ", SCX_CPU3_TST_TBL_TABLE2.SECONDBYTE
write "    3rd byte = ", SCX_CPU3_TST_TBL_TABLE2.THIRDBYTE
write "    4th byte = ", SCX_CPU3_TST_TBL_TABLE2.FOURTHBYTE
write "    5th byte = ", SCX_CPU3_TST_TBL_TABLE2.FIFTHBYTE
write "    6th byte = ", SCX_CPU3_TST_TBL_TABLE2.SIXTHBYTE
write "    7th byte = ", SCX_CPU3_TST_TBL_TABLE2.SEVENTHBYTE
write "    8th byte = ", SCX_CPU3_TST_TBL_TABLE2.EIGHTHBYTE

write ";*********************************************************************"
write "; Step 4.5: Dump the active table for table #2.  "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_02","A","tbl_02_active","CPU3","0FE1")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10
   
write "Table #2 Data dumped:"
write "    1st byte = ", SCX_CPU3_TST_TBL_TABLE2.FIRSTBYTE
write "    2nd byte = ", SCX_CPU3_TST_TBL_TABLE2.SECONDBYTE
write "    3rd byte = ", SCX_CPU3_TST_TBL_TABLE2.THIRDBYTE
write "    4th byte = ", SCX_CPU3_TST_TBL_TABLE2.FOURTHBYTE
write "    5th byte = ", SCX_CPU3_TST_TBL_TABLE2.FIFTHBYTE
write "    6th byte = ", SCX_CPU3_TST_TBL_TABLE2.SIXTHBYTE
write "    7th byte = ", SCX_CPU3_TST_TBL_TABLE2.SEVENTHBYTE
write "    8th byte = ", SCX_CPU3_TST_TBL_TABLE2.EIGHTHBYTE

write ";*********************************************************************"
write "; Step 4.6:  Dump active table for table #3. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_03","A","tbl_03_active","CPU3","0FE2")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

write "Table #3 Data dumped:"
write "    1st byte = ", SCX_CPU3_TST_TBL_TABLE3.FIRSTBYTE
write "    2nd byte = ", SCX_CPU3_TST_TBL_TABLE3.SECONDBYTE
write "    3rd byte = ", SCX_CPU3_TST_TBL_TABLE3.THIRDBYTE
write "    4th byte = ", SCX_CPU3_TST_TBL_TABLE3.FOURTHBYTE
write "    5th byte = ", SCX_CPU3_TST_TBL_TABLE3.FIFTHBYTE
write "    6th byte = ", SCX_CPU3_TST_TBL_TABLE3.SIXTHBYTE
write "    7th byte = ", SCX_CPU3_TST_TBL_TABLE3.SEVENTHBYTE
write "    8th byte = ", SCX_CPU3_TST_TBL_TABLE3.EIGHTHBYTE

write ";*********************************************************************"
write "; Step 4.7:  Dump active table for table #4. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_04","A","tbl_04_active","CPU3","0FE3")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10 

write "Table #4 Data dumped:"
write "    1st byte = ", SCX_CPU3_TST_TBL_TABLE4.FIRSTBYTE
write "    2nd byte = ", SCX_CPU3_TST_TBL_TABLE4.SECONDBYTE
write "    3rd byte = ", SCX_CPU3_TST_TBL_TABLE4.THIRDBYTE
write "    4th byte = ", SCX_CPU3_TST_TBL_TABLE4.FOURTHBYTE
write "    5th byte = ", SCX_CPU3_TST_TBL_TABLE4.FIFTHBYTE
write "    6th byte = ", SCX_CPU3_TST_TBL_TABLE4.SIXTHBYTE
write "    7th byte = ", SCX_CPU3_TST_TBL_TABLE4.SEVENTHBYTE
write "    8th byte = ", SCX_CPU3_TST_TBL_TABLE4.EIGHTHBYTE

write ";*********************************************************************"
write "; Step 4.8:  Abort the Load for table #1 so that the steps below work."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 21, "INFO"

ut_sendcmd "SCX_CPU3_TBL_LOAD_ABORT ABTABLENAME=""TST_TBL.deflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Load Abort command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load Abort command."
  endif
else
  write "<!> Error: Load Abort for Table #1 failed."
endif

wait 5

; Dump the table registry
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 5
 
if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

; add validation for the Load in progress tlm point
write "Waiting for LoadInProgress flag to change for table #1......"
wait until (SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADINPROGRESS = -1) 
write "<*> Success: Load In Progress field changed"

wait 10

write ";*********************************************************************"
write "; Step 5:  Verify TBL Load Table Command (Command Code 2) "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 5.1: Load a full table for Table 1.                            "
write "; This step loads a table which will pass validation. The test app    "
write "; verifies that the first byte has a value of x'61'.                  "
write "; This new table has 8 bytes worth of data - 61 00 61 00 61 00 63 00  "
write ";*********************************************************************"

; Load the desired values into the table
SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE = x'61'
SCX_CPU3_TST_TBL_TABLE1.SECONDBYTE = 0
SCX_CPU3_TST_TBL_TABLE1.THIRDBYTE = x'61'
SCX_CPU3_TST_TBL_TABLE1.FOURTHBYTE = 0
SCX_CPU3_TST_TBL_TABLE1.FIFTHBYTE = x'61'
SCX_CPU3_TST_TBL_TABLE1.SIXTHBYTE = 0
SCX_CPU3_TST_TBL_TABLE1.SEVENTHBYTE = x'63'
SCX_CPU3_TST_TBL_TABLE1.EIGHTHBYTE = 0

s write_file ("CPU3",4032,"Full Table load", "tbl_01_valid_load","TST_TBL.deflt_tbl_01", "SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE", "SCX_CPU3_TST_TBL_TABLE1.EIGHTHBYTE")

wait 5

success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1
num_free_buffers = SCX_CPU3_TBL_numFreeshrBuf - 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

start load_table ("tbl_01_valid_load", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then

  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

ut_tlmwait SCX_CPU3_TBL_NumFreeshrBuf, {num_free_buffers} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Free shared buffers = ", num_free_buffers
else
  write "<!> Error: Expected ", num_free_buffers, " counter = ", SCX_CPU3_TBL_NumFreeshrBuf
endif

ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")

write "Waiting for LoadInProgress flag to change for table #1......"
wait until (SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADINPROGRESS <> -1) 
write "<*> Success: LoadInProgress flag changed."

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

wait 10
  
write ";*********************************************************************"
write "; Step 5.2: Dump the active Table 1 to make sure the table loaded.  "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","ACTIVE","tbl_01_52active","CPU3","0FE0")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

wait 10

write ";*********************************************************************"
write "; Step 5.3: Validate the inactive Table 1.  "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 16, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.deflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: InActive Table 1 validate command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table 1 validation failed."
endif

; Look for the Validation success event
if (SCX_CPU3_EVS_EVENTID <> 36) then
  ut_setupevt "SCX", "CPU3", "CFE_TBL", 36, "INFO"

  wait 20

  if (SCX_CPU3_num_found_messages = 1) then
    write "<*> Success: Inactive Table 1 validated successfully."
    write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Inactive Table 1 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
    write "<*> Success: Inactive Table 1 validated successfully."
    write "<*> Success: Event Msg ",SCX_CPU3_evs_eventid," Found!"
endif

;*******************************************************
; Can we automatically compare these 2 files? 
;*******************************************************

wait 10

write ";*********************************************************************"
write "; Step 5.4: Activate Table 1.                                         "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table 1 activated."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table 1 activation failed."
endif

wait 10

write ";*********************************************************************"
write "; Step 5.5: Dump the Table Registry."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Activate command."
endif

wait 10
 
write ";*********************************************************************"
write "; Step 5.6: Dump active Table 1 and verify it is in use.              "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","ACTIVE","tbl_01_56active","CPU3","0FE0")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

;  Verify #available shared buffers is 4
if (SCX_CPU3_TBL_NumFreeshrBuf = 4) then
  write "<*> Success: There are 4 available shared buffers."
else
  write "<!> Error: There are ", SCX_CPU3_TBL_NumFreeshrBuf, " available shared buffers."
endif

;  What should I expect the Last Status Value to be? 
;  What is the last valid table name?

; Check to see if the last file loaded specified the correct file
if (SCX_CPU3_RF[1].SCX_CPU3_TBL_LASTFILELOADED = "RAM:/tbl_01_valid_load") then
  write "<*> Success: The correct file was loaded into Table #1."
else
  write "<!> Error: Table #1 was loaded with ", SCX_CPU3_RF[1].SCX_CPU3_TBL_LASTFILELOADED
endif

wait 10

write ";*********************************************************************"
write "; Step 5.7: Load a partial table for Table #3. This table should be a "
write "; double buffered table. Right now, I cannot load to a double-buffered"
write "; table. So, I am loading a partial single-buffered table."
write ";*********************************************************************"
; Dump the active table - this was done in the previous step
; make a change to 2 bytes
SCX_CPU3_TST_TBL_TABLE3.THIRDBYTE = x'63'
SCX_CPU3_TST_TBL_TABLE3.FOURTHBYTE = x'36'

; write new load file
s write_file ("CPU3",4034,"Partial Table load", "tbl_03_partial_load","TST_TBL.deflt_tbl_03", "SCX_CPU3_TST_TBL_TABLE3.THIRDBYTE", "SCX_CPU3_TST_TBL_TABLE3.FOURTHBYTE")

; Load Table #3 with the file written above
start load_table ("tbl_03_partial_load", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

; Activate the load
ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_03"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table #3 activate command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table #3 activation failed."
endif

wait 10

; Dump the Table Registry
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

wait 10

; Wait until the load has been processed
write "Waiting for table #3 to load....."
wait until (p@SCX_CPU3_RF[3].SCX_CPU3_TBL_LOADPENDING = "FALSE")

; verify that the loaded file is now being used 
if (SCX_CPU3_RF[3].SCX_CPU3_TBL_LASTFILELOADED = "RAM:/tbl_03_partial_load") then
  write "<*> Success: Activation loaded the correct file into table #3."
else
  write "<!> Error: Activation failed to load the correct file into table #3."
endif

wait 10
; What should I expect the Last Status Value to be?
; What is the last valid table name?

write ";*********************************************************************"
write "; Step 6: Verify TBL Validate Table Command (Command Code 4)          "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 6.1: Load table #1 with the table file that will fail "
write "; validation."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

; Dump the Active table for Table #1
s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","ACTIVE","tbl_01_61active","CPU3","0FE0")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

; Modify the first byte so that it fails validation
SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE = x'36'

; Write the file
s write_file ("CPU3",4032,"Invalid Table load", "tbl_01_invalid","TST_TBL.deflt_tbl_01", "SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE", "SCX_CPU3_TST_TBL_TABLE1.EIGHTHBYTE")

; Setup the counters to look for
num_free_buffers = SCX_CPU3_TBL_numFreeshrBuf - 1
success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

; Load Table #1 with the file written above
start load_table ("tbl_01_invalid", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

; Verify that the # of available shared buffers is one less than before
ut_tlmwait SCX_CPU3_TBL_NumFreeshrBuf, {num_free_buffers} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Free shared buffers = ", num_free_buffers
else
  write "<!> Error: Expected ", num_free_buffers, " counter = ", SCX_CPU3_TBL_NumFreeshrBuf
endif

wait 10

; Dump the table registry
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

; Dump the inactive table to verify its contents
s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","INACTIVE","tbl_01_61inactive","CPU3","0FE0")

; Check if the first byte is what we set it to
if (SCX_CPU3_TST_TBL_TABLE1.FIRSTBYTE = x'36') then
  Write "<*> Success: Inactive buffer contains our load file."
else
  Write "<!> Error: Inactive buffer does not contain the specified load file."
endif

wait 10

write ";*********************************************************************"
write "; Step 6.2: Validate the Inactive table for table #1. This should     "
write "; fail validation."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 16, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.deflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: InActive Table 1 validate command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table 1 validation failed."
endif

; Look for the Validation failure event
if (SCX_CPU3_EVS_EVENTID <> 96) then
  ut_setupevt "SCX", "CPU3", "CFE_TBL", 96, "INFO"

  wait 20

  if (SCX_CPU3_num_found_messages = 1) then
    write "<*> Success: Inactive Table 1 failed validation."
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Inactive Table 1 Passed validation when it should have failed."
    Write "<!> Error: Expected Event Msg not received for validation command."
  endif
else
  write "<*> Success: Inactive Table 1 failed validation."
  write "<*> Success: Event Msg ",SCX_CPU3_evs_eventid," Found!"
endif

wait 10

write ";*********************************************************************"
write "; Step 6.3: Load table #2 with the table file that will pass "
write "; validation."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

; Dump the Active table for Table #2
s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_02","ACTIVE","tbl_02_63active","CPU3","0FE1")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1
error_cmd_ctr = SCX_CPU3_TBL_CMDEC
num_free_buffers = SCX_CPU3_TBL_numFreeshrBuf - 1
table2_loaded = 0
wait 3 

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

; Load the valid table 2
start load_table ("tbl_02_63active", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
    table2_loaded = 1
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

;;; Since Table 2 is a double-buffered table, it does not use the shared buffers
;;; Thus, the code below can be removed.
;if (table2_loaded = 1) then
;  ; Verify that the # of available shared buffers is one less than before
;  ut_tlmwait SCX_CPU3_TBL_NumFreeshrBuf, {num_free_buffers} 
;  if (UT_TW_Status = UT_Success) then
;    write "<*> Success: Free shared buffers = ", num_free_buffers
;  else
;    write "<!> Error: Expected ", num_free_buffers, " counter = ", SCX_CPU3_TBL_NumFreeshrBuf
;  endif
;endif

wait 10

; Dump the Table Registry
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

wait 10

write ";*********************************************************************"
write "; Step 6.4: Validate the Inactive table for table #2. This should pass"
write "; validation."
write ";*********************************************************************"
; If table #2 loaded successfully, validate the buffer
if (table2_loaded = 1) then
  ut_setupevt "SCX", "CPU3", "CFE_TBL", 16, "DEBUG"

  ut_sendcmd "SCX_CPU3_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.deflt_tbl_02"""
  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Success: InActive Table 2 validate command sent."
    if (SCX_CPU3_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
    else
      Write "<!> Error: Event Message not received for Validate command."
    endif
  else
    write "<!> Error: InActive Table 2 validation failed."
  endif

  ; Look for the Validation success event
  ut_setupevt "SCX", "CPU3", "CFE_TBL", 36, "INFO"

  wait 20

  if (SCX_CPU3_num_found_messages = 1) then
    write "<*> Success: Inactive Table 2 validated successfully."
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Inactive Table 2 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
  write "*** Inactive Table #2 validation skipped because the file load command failed in the previous step."
endif

wait 10

write ";*********************************************************************"
write "; Step 6.5: Validate the Active table for table #1. This should pass"
write "; validation."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 16, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_VALIDATE ACTIVE VTABLENAME=""TST_TBL.deflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Active Table 1 validate command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: InActive Table 1 validation failed."
endif

; Look for the Validation success event
if (SCX_CPU3_EVS_EVENTID <> 36) then
  ut_setupevt "SCX", "CPU3", "CFE_TBL", 36, "INFO"

  wait 20

  if (SCX_CPU3_num_found_messages = 1) then
    write "<*> Success: Active Table #1 validated."
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Active Table #1 validation failed."
    Write "<!> Error: Event Message not received for validation command."
  endif
else
  write "<*> Success: Active Table #1 validated."
  write "<*> Success: Event Msg ",SCX_CPU3_evs_eventid," Found!"
endif

wait 10

write ";*********************************************************************"
write "; Step 6.6: Validate the Active table for table #2. This should pass"
write "; validation."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 16, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_VALIDATE ACTIVE VTABLENAME=""TST_TBL.deflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Active Table 2 validate command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Validate command."
  endif
else
  write "<!> Error: Active Table 2 validation failed."
endif

; Look for the Validation success event
ut_setupevt "SCX", "CPU3", "CFE_TBL", 36, "INFO"

wait 20

if (SCX_CPU3_num_found_messages = 1) then
  write "<*> Success: Active Table 2 validated successfully."
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  write "<!> Error: Active Table 2 validation failed."
  Write "<!> Error: Event Message not received for validation command."
endif

wait 10

write ";*********************************************************************"
write "; Step 7: Verify TBL Activate Table Command (Command Code 5)          "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 7.1: Activate Table #1 and verify that the table was activated.  "
write "; Dump Active Table #1 and verify that the loaded file is being used. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table 1 activated."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table 1 activation failed."
endif

wait 10

; Dump the Table Registry
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

wait 10

; Dump Table #1 Active Table -- WHY????
; Wait until the load has been processed
write "Waiting for table #1 to load....."
wait until (p@SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADPENDING = "FALSE")

; verify that the loaded file is now being used 
if (SCX_CPU3_RF[1].SCX_CPU3_TBL_LASTFILELOADED = "RAM:/tbl_01_invalid") then
  write "<*> Success: Activation loaded the correct file into table #1."
else
  write "<!> Error: Activation failed to load the correct file into table #1."
endif

wait 10

write ";*********************************************************************"
write "; Step 7.2: Activate Table #2 and verify that the table was activated."
write "; Dump Active Table #2 and verify that the loaded file is being used. "
write ";*********************************************************************"
; Only activate Table #2 if the load from step 6.3 worked
if (table2_loaded = 1) then
  ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

  ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_02"""

  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Success: Table #2 Activate command sent."
    if (SCX_CPU3_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
    else
      Write "<!> Error: Event Message not received for Activate command."
    endif
  else
    write "<!> Error: Table #2 activation command failed."
  endif
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
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_WRITEREG2FILE DUMPFILENAME"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Registry successfully dumped."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Table Registry command."
  endif
else
  Write "<!> Error: Table Registry Dump command failed."
endif

wait 10
 
;  Verify that all of the test app tables are there
if (SCX_CPU3_RF[1].SCX_CPU3_TBL_TABLENAME = "TST_TBL.deflt_tbl_01" AND  ;;
    SCX_CPU3_RF[2].SCX_CPU3_TBL_TABLENAME = "TST_TBL.deflt_tbl_02" AND  ;;
    SCX_CPU3_RF[3].SCX_CPU3_TBL_TABLENAME = "TST_TBL.deflt_tbl_03" AND  ;;
    SCX_CPU3_RF[4].SCX_CPU3_TBL_TABLENAME = "TST_TBL.deflt_tbl_04") then
  write "<*> Success: Table Registry contains the correct tables."
else
  write "<!> Error: Table Registry verification failed. The correct tables are not loaded."
endif

wait 10

write ";*********************************************************************"
write "; Step 9: Verify TBL Telemeter Table Registry Entry Command "
write "; (Command  Code 7) Send command to telemeter the entry for Table #2  "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 18, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_TLM_REG RTTABLENAME=""TST_TBL.deflt_tbl_02"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Telemeter Registry command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  Write "<!> Error: Table Telementer Registry command failed."
endif

wait 10

write ";*********************************************************************"
write "; Step 10: Verify Command to Modify the Table Dump Only Option        "
write "; command (Command Code 8) "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 10.1: Send command to set Table #1 to Dump-Only. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 20, "INFO"

ut_sendcmd "SCX_CPU3_TBL_MOD_DONLY DUMP_ONLY RMTABLENAME=""TST_TBL.deflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Modify Dump_Only command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Table Modify command."
  endif
else
  Write "<!> Error: Table Modify Dump_Only command failed."
endif

wait 10

; Dump the Table Registry in order to verify the flag
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10
 
if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

wait 10

write "Waiting for the DumpOnly flag to get set to TRUE...."
wait until (p@SCX_CPU3_RF[1].SCX_CPU3_TBL_DUMPONLY = "TRUE")
write "<*> Success: DumpOnly = TRUE"

wait 10

write ";*********************************************************************"
write "; Step 10.2: Send command to load Table #1. Verify that this command  "
write "; gets rejected."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 14, "INFO"

; Dump the Active table for Table #1
s get_tbl_to_cvt ("RAM:","TST_TBL.deflt_tbl_01","ACTIVE","tbl_01_10_2active","CPU3","0FE0")

if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  if (SCX_CPU3_evs_eventid = 13) then
    Write "<*> Success: Expected Event Msg 14 but rcv'd Event Msg ",SCX_CPU3_evs_eventid," which means the dump file was overwritten."
  else
    Write "<!> Error: Expected Event Msg not received for Dump command."
  endif
endif

; When does the failure happen? On the load or on the activate?
;;;;;;;;;; Load is successful!!!! 
success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

start load_table ("tbl_01_10_2active", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

; try the activate -- Should this fail ???
;;;;;;;;;;;;;; Activate works ALSO!!!
;ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_01"""

;if (UT_SC_Status = UT_SC_CmdFailure) then
if (UT_SC_Status = UT_SC_Success) then
  write "<!> Error: Table #1 activation was successful when the DUMP_ONLY flag was set."
;  if (SCX_CPU3_num_found_messages = 1) then
;    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
;  else
;    Write "<!> Error: Event Message not received for Activate command."
;  endif
else
  write "<*> Success: Table #1 activation failed."
endif

wait 10
;  Verify that the load was rejected

write ";*********************************************************************"
write "; Step 10.3: Send command to set Table #1 to Load & Dump. "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 19, "INFO"

ut_sendcmd "SCX_CPU3_TBL_MOD_DONLY LOAD_DUMP RMTABLENAME=""TST_TBL.deflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table Modify Load & Dump command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Table Modify Load & Dump command."
  endif
else
  Write "<!> Error: Table Modify Load & Dump command failed."
endif

wait 5

; Dump the Table Registry in order to verify the flag
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10
 
if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

write "Waiting for the DumpOnly flag to get set to FALSE...."
wait until (p@SCX_CPU3_RF[1].SCX_CPU3_TBL_DUMPONLY = "FALSE")
write "<*> Success: DumpOnly = FALSE"

wait 10

write ";*********************************************************************"
write "; Step 10.4: Send command to load Table #1. Verify that this command  "
write "; is successful."
write ";*********************************************************************"
success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

start load_table ("tbl_01_10_2active", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

; Activate the load
ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table #1 activation command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table #1 activation failed."
endif

wait 10
;  Verify that the load was successful

write ";*********************************************************************"
write "; Step 11: Verify Command to Abort a Load (Command Code 9)            "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 11.1: Load Table #1 using the file dumped in Step 10.2.        "
write ";*********************************************************************"
success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

start load_table ("tbl_01_10_2active", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table Load command."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table Load command Failed!"
endif

wait 10

write ";*********************************************************************"
write "; Step 11.2: Dump the Table Registry to verify a load is pending.     "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10
 
if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

; Verify the Load IP field
write "Waiting for Load In Progress field to change......"
wait until (p@SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADINPROGRESS <> -1)
write "<*> Success: Load In Progress field changed"

wait 10

write ";*********************************************************************"
write "; Step 11.3: Send the Load Abort command for table #1."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 21, "INFO"

ut_sendcmd "SCX_CPU3_TBL_LOAD_ABORT ABTABLENAME=""TST_TBL.deflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Load Abort command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load Abort command."
  endif
else
  write "<!> Error: Load Abort for Table #1 failed."
endif

wait 10 

write ";*********************************************************************"
write "; Step 11.4: Dump the Table Registry to verify the load has aborted.  "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 15, "DEBUG"

s get_file_to_cvt ("RAM:", "tbl_reg.log", "scx_cpu3_tbl_reg.log", "CPU3")
wait 10
 
if (SCX_CPU3_num_found_messages = 1) then
  Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
else
  Write "<!> Error: Event Message not received for Registry Dump command."
endif

; Verify the Load IP field
write "Waiting for Load In Progress field to change......"
wait until (p@SCX_CPU3_RF[1].SCX_CPU3_TBL_LOADINPROGRESS = -1)
write "<*> Success: Load In Progress field changed"

wait 10

write ";*********************************************************************"
write "; Step 11.5: Dump the inactive table for table #1 and verify that the "
write "; inactive buffer is empty.          " 
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 66, "ERROR"

ut_sendcmd "SCX_CPU3_TBL_DUMP INACTIVE DTABLENAME=""TST_TBL.deflt_tbl_01"" DFILENAME=""RAM:/tbl_01_inactive"""

if (UT_SC_Status = UT_SC_CmdFailure) then
  write "<*> Success: Inactive Table 1 dump generated the expected error."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    write "<!> Error: Expected Error Event not received."
  endif
else
  write "<!> Error: Inactive Table 1 dump succeeded when a failure was expected."
endif

wait 10 

;  verify the # of shared buffers available is 4
if (SCX_CPU3_TBL_NumFreeshrBuf = 4) then
  write "<*> Success: There are 4 available shared buffers."
else
  write "<!> Error: There are ", SCX_CPU3_TBL_NumFreeshrBuf, " available shared buffers. Expected 4 buffers."
endif

wait 10 

write ";*********************************************************************"
write "; Step 12: Verify control access of a shared table. Here we want to   "
write "; make sure that a table cannot be updated if another application is  "
write "; using the shared table. "
write "; Table #4 has been registered and loaded by TST_TBL app. "
write ";*********************************************************************"
write ";*********************************************************************"
write "; Step 12.1: Send the command from TST_TBL2 to share Table #4."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "TST_TBL2", 25, "INFO"

/SCX_CPU3_TST_TBL2_SHARETBL

wait 10

if (SCX_CPU3_num_found_messages = 1) then
  write "<*> Success: Table share command from TST_TBL2 sent."
else
  write "<!> Error: Table share command from TST_TBL2 failed."
endif

wait 5

;;;;;;;;;;;;;;
;;; May not need to do this step????
;;;;;;;;;;;;;;
write ";*********************************************************************"
write "; Step 12.2: Send the command from TST_TBL2 to get the address of     "
write "; Table #4."
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "TST_TBL2", 17, "INFO"

/SCX_CPU3_TST_TBL2_GETTBLADDR

wait 10

if (SCX_CPU3_num_found_messages = 1) then
  write "<*> Success: Get Table Address command from TST_TBL2 sent."
else
  write "<!> Error: Get Table Address command from TST_TBL2 failed."
endif

wait 10

write ";*********************************************************************"
write "; Step 12.3: Load a different file into the working buffer for "
write "; Table #4."
write ";*********************************************************************"
;  Try to activate the table
success_cmd_ctr = SCX_CPU3_TBL_CMDPC + 1

ut_setupevt "SCX", "CPU3", "CFE_TBL", 12, "INFO"

; write a file to load
start load_table ("deflt_tbl_04_full", "CPU3")

ut_tlmwait SCX_CPU3_TBL_CMDPC, {success_cmd_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Success: Table #4 Load command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Load command."
  endif
else
  write "<!> Error: Table #4 Load command Failed!"
endif

wait 10

write ";*********************************************************************"
write "; Step 12.4: Activate the new load for Table #4. This should fail.    "
write ";*********************************************************************"
ut_setupevt "SCX", "CPU3", "CFE_TBL", 17, "DEBUG"

ut_sendcmd "SCX_CPU3_TBL_ACTIVATE ATABLENAME=""TST_TBL.deflt_tbl_04"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Success: Table #4 activate command sent."
  if (SCX_CPU3_num_found_messages = 1) then
    Write "<*> Success: Event Msg ",SCX_CPU3_find_evs_eventid," Found!"
  else
    Write "<!> Error: Event Message not received for Activate command."
  endif
else
  write "<!> Error: Table #4 activation failed."
endif

wait 10

;  Verify that the update did not occur
;  Tell Application B to "unlock" the table
;  Veify that the update did occur
 
     Write "Test complete"
ENDPROC
