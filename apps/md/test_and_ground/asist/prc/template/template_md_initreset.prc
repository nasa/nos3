PROC $sc_$cpu_md_initreset
;*******************************************************************************
;  Test Name:  MD_InitReset
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to verify that the Memory Dwell (MD)
;   application behaves correctly when it is initialized or reset, storing
;   data in the CDS and restoring the data when a cFE Processor Reset or MD
;   application reset is performed.
;   
;
;  Requirements Tested
;    MD1004     If MD accepts any command as valid, MD shall execute the
;               command, increment the MD Valid Command Counter and issue an
;               event message.
;    MD2000     Upon receipt of a Start Dwell command, MD shall identify the
;               command-specified tables as ENABLED and start processing the
;               command-specified memory dwell tables, starting with the first
;               entry, until one of the following:
;                       a) an entry that has a zero value for the Number of
;                           Bytes field
;                       b) until it has processed the last entry in a Dwell
;                          Table
;    MD4000     Upon receipt of a Jam Dwell command, MD shall update the
;               command-specified memory dwell table with the command-specified
;               information:
;                       a) Dwell Table Index
;                       b) Address
;                       c) Number of bytes (0,1,2 or 4)
;                       d) Delay Between Samples
;    MD8000     MD shall generate a housekeeping message containing the
;               following:
;                       a) Valid Command Counter
;                       b) Command Rejected Counter
;                       c) For Each Dwell:
;                               1. Enable/Disable Status
;                               2. Number of Dwell Addresses
;                               3. Dwell Rate
;                               4. Number of Bytes
;                               5. Current Dwell Packet Index
;                               6. Current Entry in the Dwell Table
;                               7. Current Countdown counter
;    MD9000     Upon any initialization of the MD Application (cFE Power On, cFE;               Processor Reset or MD Application Reset), MD shall initialize
;               the following data to Zero
;                       a) Valid Command Counter
;                       b) Command Rejected Counter
;    MD9001     Upon cFE Power-on Reset, MD shall initialize each Memory Dwell
;               table status to DISABLED.
;    MD9002     Upon cFE Power-on Reset, MD shall initialize each Memory Dwell
;               table to zero.
;    MD9003     MD shall store the following information whenever it changes
;		(in support of a cFE Processor Reset or Application Reset):
;			a) Enable/Disable Status for each Dwell
;			b) <OPTIONAL> signature for each dwell
;			c) Contents of each Dwell Table
;    MD9004	On a cFE Processor Reset or a MD Application Reset, MD shall
;		restore the information specified in MD9003.
;    MD9004.1   MD shall validate the data and if any data is invalid, MD shall:
;			a) disable the invalid dwell table
;			b) initialize table contents with default values
;
;  Prerequisite Conditions
;    The CFS is up and running and ready to accept commands. The MD
;    commands and TLM items exist in the GSE database. The display page
;    for the MD Housekeeping exists. An MD test application exists
;    which contains known data to dwell on, loads the initial dwell
;    tables and sends wakeup calls to support testing of
;    supercommutation.
;
;  Assumptions and Constraints
;   None
;
;  Change History
;
;   Date          Name     	Description
;   09/3/08       S. Jonke	Original Procedure
;   12/04/09      W. Moleski    Turned logging off around code that did not
;                               provide any significant benefit of logging
;   01/23/12      W. Moleski    Updated proc to work with cFE 6.2.2.0 and added
;				variable names for the app, tables and ram disk
;
;  Arguments
;   None
;
;  Procedures Called
;   $sc_$cpu_md_deftables 
; 
;  Required Post-Test Analysis
;   None
;**********************************************************************

;; Turn off logging for the includes
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "tst_md_events.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "md_platform_cfg.h"
#include "md_events.h"
#include "cfe_tbl_events.h"
#include "cfe_platform_cfg.h"

%liv (log_procedure) = logging

#define MD_1004     0
#define MD_2000     1
#define MD_4000     2
#define MD_8000     3
#define MD_9000     4
#define MD_9001     5
#define MD_9002     6
#define MD_9003     7
#define MD_9004     8
#define MD_9004_1   9

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 9
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1004", "MD_2000", "MD_4000", "MD_8000", "MD_9000", "MD_9001", "MD_9002", "MD_9003", "MD_9004", "MD_9004_1"]

local cmdcnt, errcnt
local rawcmd
local stream1, dwell1, dwell2, dwell3, dwell4
local passed
LOCAL dwell_tbl1_load_pkt, dwell_tbl2_load_pkt, dwell_tbl3_load_pkt, dwell_tbl4_load_pkt
LOCAL dwell_tbl1_load_appid, dwell_tbl2_load_appid, dwell_tbl3_load_appid, dwell_tbl4_load_appid
local dwl_tbl_1_index, dwl_tbl_2_index, dwl_tbl_3_index, dwl_tbl_4_index
local testdata_addr, addr, workaddress
local errcnt

local MDAppName = "MD"
local ramDir = "RAM:0"
local MDTblName1 = MDAppName & ".DWELL_TABLE1"
local MDTblName2 = MDAppName & ".DWELL_TABLE2"
local MDTblName3 = MDAppName & ".DWELL_TABLE3"
local MDTblName4 = MDAppName & ".DWELL_TABLE4"

;; CPU1 is the default
dwell_tbl1_load_pkt = "0FA8"
dwell_tbl2_load_pkt = "0FA9"
dwell_tbl3_load_pkt = "0FAA"
dwell_tbl4_load_pkt = "0FAB"
dwell_tbl1_load_appid = 4008
dwell_tbl2_load_appid = 4009
dwell_tbl3_load_appid = 4010
dwell_tbl4_load_appid = 4011

if ("$CPU" = "CPU2") then
  dwell_tbl1_load_pkt = "0FC6"
  dwell_tbl2_load_pkt = "0FC7"
  dwell_tbl3_load_pkt = "0FC8"
  dwell_tbl4_load_pkt = "0FC9"
  dwell_tbl1_load_appid = 4038
  dwell_tbl2_load_appid = 4039
  dwell_tbl3_load_appid = 4040
  dwell_tbl4_load_appid = 4041
elseif ("$CPU" = "CPU3") then
  dwell_tbl1_load_pkt = "0FE6"
  dwell_tbl2_load_pkt = "0FE7"
  dwell_tbl3_load_pkt = "0FE8"
  dwell_tbl4_load_pkt = "0FE9"
  dwell_tbl1_load_appid = 4070
  dwell_tbl2_load_appid = 4071
  dwell_tbl3_load_appid = 4072
  dwell_tbl4_load_appid = 4073
endif

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
write ";             Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 1.1:  Start the Memory Dwell Test (TST_MD) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MD", TST_MD_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MD","$CPU","TST_MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - TST_MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";*********************************************************************"
write ";  Step 1.2:  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 2

s load_start_app (MDAppName,"$CPU","MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

local hkPktId
;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'890'
hkPktId = "p090"
dwell1 = x'891'
dwell2 = x'892'
dwell3 = x'893'
dwell4 = x'894'

if ("$CPU" = "CPU2") then
  stream1 = x'990'
  hkPktId = "p190"
  dwell1 = x'991'
  dwell2 = x'992'
  dwell3 = x'993'
  dwell4 = x'994'
elseif ("$CPU" = "CPU3") then
  stream1 = x'A90'
  hkPktId = "p290"
  dwell1 = x'A91'
  dwell2 = x'A92'
  dwell3 = x'A93'
  dwell4 = x'A94'
endif

write "Sending commands to add subscriptions for MD HK and dwell packets."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 5
/$SC_$CPU_TO_ADDPACKET Stream=dwell3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write "Opening MD HK Page."
page $SC_$CPU_MD_HK


;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

; Search the Table Registry for the locations of the dwell tables

for i = 1 to $SC_$CPU_TBL_NUMTABLES do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = MDTblName1) then
    dwl_tbl_1_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = MDTblName2) then
    dwl_tbl_2_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = MDTblName3) then
    dwl_tbl_3_index = i
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = MDTblName4) then
    dwl_tbl_4_index = i
  endif

  write "Table #",i," Name = ", $SC_$CPU_RF[i].$SC_$CPU_TBL_NAME 
enddo

write "Dwell Table #1 is at index #" & dwl_tbl_1_index
write "Dwell Table #2 is at index #" & dwl_tbl_2_index
write "Dwell Table #3 is at index #" & dwl_tbl_3_index
write "Dwell Table #4 is at index #" & dwl_tbl_4_index

write ";*********************************************************************"
write ";  Step 1.3: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 1.4: Verify that the MD Housekeeping packet is being generated"
write ";  and the telemetry items are initialized to zero (0). "
write ";*********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currScnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements MD_8000, "F"
endif

;; Check the HK tlm items to see if they are 0 or NULL
if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

write ";*********************************************************************"
write ";  Step 1.5: Verify that each MD table is disabled. "
write ";*********************************************************************"
if ($SC_$CPU_MD_EnableMask = 0) THEN
  write "<*> Passed (9001) - Enable Mask initialized properly."
  ut_setrequirements MD_9001, "P"
else
  write "<!> Failed (9001) - Enable Mask was NOT initialized at startup."
  write "  EnableMask           = ",$SC_$CPU_MD_EnableMask
  ut_setrequirements MD_9001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.6: Verify that each MD table is initialized to 0. "
write ";*********************************************************************"
passed = 1
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  if ($SC_$CPU_MD_AddrCnt[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_AddrCnt[", i, "] = ", $SC_$CPU_MD_AddrCnt[i]
  endif
  
  if ($SC_$CPU_MD_Rate[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_Rate[", i, "] = ", $SC_$CPU_MD_Rate[i]
  endif

  if ($SC_$CPU_MD_DataSize[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DataSize[", i, "] = ", $SC_$CPU_MD_DataSize[i]
  endif
  
  if ($SC_$CPU_MD_DwPktOffset[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  endif
  
  if ($SC_$CPU_MD_DwTblEntry[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwTblEntry[", i, "] = ", $SC_$CPU_MD_DwTblEntry[i]
  endif
ENDDO

if (passed = 1) THEN
  write "<*> Passed (9002) - Memory Dwell tables initialized properly."
  ut_setrequirements MD_9002, "P"
else
  write "<!> Failed (9002) - Memory Dwell tables NOT initialized at startup."
  ut_setrequirements MD_9002, "F"
endif

wait 5

write "; Setup the initial dwell tables"
s $sc_$cpu_md_deftables

wait 5

write ";*********************************************************************"
write ";  Step 2.0:  Data Collection & Reset Tests"
write ";*********************************************************************"
write ";  Step 2.1:  Modify FIRST table and Perform cFE Processor Reset  "
write ";*********************************************************************"
write ";  Step 2.1.1: Send Jam Dwell command modifying an entry in the FIRST "
write ";  table  "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+128
local tbl1JamAddr = workaddress

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=1 FieldLength=4 DwellDelay=1 Offset=workaddress SymName="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (1004;4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;; Dump the table to confirm that the Jam command above updated the table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_initreset211","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 1."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000;9003) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
  ut_setrequirements MD_9003, "P"
else
  write "<!> Failed (4000;9003) - The table did not contain the jammed entry."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
  ut_setrequirements MD_9003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.1.2: Perform a processor reset  "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";  Start the Memory Dwell Test (TST_MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MD", TST_MD_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MD","$CPU","TST_MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - TST_MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RECOVERED_TBL_VALID_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 3

s load_start_app (MDAppName,"$CPU","MD_AppMain")

; Wait for table recovery events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 4
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9004) - All MD Dwell Table recovery messages were received"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Not all of the MD Dwell Table recovery messages were received!"
  ut_setrequirements MD_9004, "F"
endif

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'890'
dwell1 = x'891'
dwell2 = x'892'
dwell3 = x'893'
dwell4 = x'894'

if ("$CPU" = "CPU2") then
  stream1 = x'990'
  dwell1 = x'991'
  dwell2 = x'992'
  dwell3 = x'993'
  dwell4 = x'994'
elseif ("$CPU" = "CPU3") then
  stream1 = x'A90'
  dwell1 = x'A91'
  dwell2 = x'A92'
  dwell3 = x'A93'
  dwell4 = x'A94'
endif

write "Sending commands to add subscriptions for MD HK and dwell packets."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 5
/$SC_$CPU_TO_ADDPACKET Stream=dwell3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write "Opening MD HK Page."
page $SC_$CPU_MD_HK

write ";  Enable DEBUG Event Messages "
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";  Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "

;; Check the MD tlm items to see if they are 0 or NULL

if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.1.3: Verify that each MD table data is initialized to 0. "
write ";*********************************************************************"
passed = 1
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  if ($SC_$CPU_MD_DwPktOffset[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  endif
  
  if ($SC_$CPU_MD_DwTblEntry[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwTblEntry[", i, "] = ", $SC_$CPU_MD_DwTblEntry[i]
  endif
ENDDO

if (passed = 1) THEN
  write "<*> Passed - Memory Dwell table data is initialized properly."
else
  write "<!> Failed - Memory Dwell table data is NOT initialized!"
endif

write ";*********************************************************************"
write ";  Step 2.1.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0000') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.1.5: Verify that the entries that were modified in the"
write ";  FIRST table were restored "
write ";*********************************************************************"
;; Dump table #1 to confirm that table was restored properly
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_initreset215","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 1."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = tbl1JamAddr) then
  write "<*> Passed (9004) - Dwell table #1 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #1 was not restored upon reset."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",tbl1JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; FIRST table should have address count 2, rate 1 and data size 5
if ($SC_$CPU_MD_AddrCnt[1] = 2) and ($SC_$CPU_MD_Rate[1] = 1) and ($SC_$CPU_MD_DataSize[1] = 5) THEN
  write "<*> Passed (9004) - FIRST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - FIRST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 5)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #2 verification
;;*******************************
; SECOND table should have address count 2, rate 1 and data size 4 - it was not modified
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 1) and ($SC_$CPU_MD_DataSize[2] = 4) THEN
  write "<*> Passed (9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 4)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #3 verification
;;*******************************
; THIRD table should have address count 2, rate 1 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[3] = 2) and ($SC_$CPU_MD_Rate[3] = 1) and ($SC_$CPU_MD_DataSize[3] = 6) THEN
  write "<*> Passed (9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 6)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #4 verification
;;*******************************
; LAST table should have address count 2, rate 1 and data size 2 - it was not modified
if ($SC_$CPU_MD_AddrCnt[4] = 2) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 2) THEN
  write "<*> Passed (9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 2)"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2:  Modify SECOND table and Perform MD Application Reset  "
write ";***********************************************************************"
write ";  Step 2.2.1: Send Jam Dwell command modifying an entry in the SECOND "
write ";  table  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+64
local tbl2JamAddr = workaddress

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=2 FieldLength=1 DwellDelay=1 Offset=workaddress SymName="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004,4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (1004,4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;; Dump the table to confirm that the Jam command above updated the table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_initreset221","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 2."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000;9003) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
  ut_setrequirements MD_9003, "P"
else
  write "<!> Failed (4000;9003) - The table did not contain the jammed entry."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
  ut_setrequirements MD_9003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2.2: Perform an MD application reset  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_RESTART_APP_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RECOVERED_TBL_VALID_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 3

/$SC_$CPU_ES_RESTARTAPP APPLICATION=MDAppName

; Wait for table recovery events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 4
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9004) - All MD Dwell Table recovery messages were received"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Not all of the MD Dwell Table recovery messages were received!"
  ut_setrequirements MD_9004, "F"
endif

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES restart app Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

wait 5

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

write ";  Enable DEBUG Event Messages "
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";  Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "

;; Check the MD tlm items to see if they are 0 or NULL
if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";*********************************************************************"
write ";  Step 2.2.3: Verify that each MD table data is initialized to 0. "
write ";*********************************************************************"
passed = 1
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  if ($SC_$CPU_MD_DwPktOffset[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  endif
  
  if ($SC_$CPU_MD_DwTblEntry[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwTblEntry[", i, "] = ", $SC_$CPU_MD_DwTblEntry[i]
  endif
ENDDO

if (passed = 1) THEN
  write "<*> Passed - Memory Dwell table data is initialized properly."
else
  write "<!> Failed - Memory Dwell table data is NOT initialized!"
endif

write ";*********************************************************************"
write ";  Step 2.2.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0000') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.2.5: Verify that the entries that were modified in the"
write ";  FIRST & SECOND tables were restored "
write ";*********************************************************************"
;; Dump table #1 to confirm that table was restored properly
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_reset225","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 1."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = tbl1JamAddr) then
  write "<*> Passed (9004) - Dwell table #1 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #1 was not restored upon reset."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",tbl1JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; FIRST table should have address count 2, rate 1 and data size 5
if ($SC_$CPU_MD_AddrCnt[1] = 2) and ($SC_$CPU_MD_Rate[1] = 1) and ($SC_$CPU_MD_DataSize[1] = 5) THEN
  write "<*> Passed (9004) - FIRST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - FIRST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 5)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #2 verification
;;*******************************
;; Dump the table to confirm that table was restored after the reset
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl2_reset225","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 2."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = tbl2JamAddr) then
  write "<*> Passed (9004) - Dwell table #2 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #2 was not restored upon reset."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay
  write "- Expected Offset of ",tbl2JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; SECOND table should have address count 2, rate 2 and data size 3
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 2) and ($SC_$CPU_MD_DataSize[2] = 3) THEN
  write "<*> Passed (9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 3)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #3 verification
;;*******************************
; THIRD table should have address count 2, rate 1 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[3] = 2) and ($SC_$CPU_MD_Rate[3] = 1) and ($SC_$CPU_MD_DataSize[3] = 6) THEN
  write "<*> Passed (9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 6)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #4 verification
;;*******************************
; LAST table should have address count 2, rate 1 and data size 2 - it was not modified
if ($SC_$CPU_MD_AddrCnt[4] = 2) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 2) THEN
  write "<*> Passed (9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 2)"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3:  Modify LAST table and Perform cFE Processor Reset  "
write ";********************************************************************"
write ";  Step 2.3.1: Send Jam Dwell command modifying an entry in the LAST "
write ";  table  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+34
local tbl4JamAddr = workaddress

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=3 FieldLength=2 DwellDelay=0 Offset=workaddress SymName="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004,4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004,4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

; add a new null entry to the end
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_NULL_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=4 FieldLength=0 DwellDelay=0 Offset=0 SymName="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004,4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (1004,4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_NULL_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;; Dump the table to confirm that the Jam commands above updated the table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl4_reset231","$CPU",dwell_tbl4_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 2."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = workaddress) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset = 0) then
  write "<*> Passed (4000;9003) - The jammed entries have been found in the table."
  ut_setrequirements MD_4000, "P"
  ut_setrequirements MD_9003, "P"
else
  write "<!> Failed (4000;9003) - The table did not contain the jammed entries."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset
  write "- Expected Length of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length
  write "- Expected Delay of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay
  write "- Expected Offset of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3.2: Perform a processor reset  "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";  Start the Memory Dwell Test (TST_MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MD", TST_MD_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MD","$CPU","TST_MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - TST_MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RECOVERED_TBL_VALID_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 3

s load_start_app (MDAppName,"$CPU","MD_AppMain")

; Wait for table recovery events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 4
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9004) - All MD Dwell Table recovery messages were received"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Not all of the MD Dwell Table recovery messages were received!"
  ut_setrequirements MD_9004, "F"
endif

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

; APID for MD HK is not set in stone yet, currently using x'A8A'

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'890'
dwell1 = x'891'
dwell2 = x'892'
dwell3 = x'893'
dwell4 = x'894'

if ("$CPU" = "CPU2") then
  stream1 = x'990'
  dwell1 = x'991'
  dwell2 = x'992'
  dwell3 = x'993'
  dwell4 = x'994'
elseif ("$CPU" = "CPU3") then
  stream1 = x'A90'
  dwell1 = x'A91'
  dwell2 = x'A92'
  dwell3 = x'A93'
  dwell4 = x'A94'
endif

write "Sending commands to add subscriptions for MD HK and dwell packets."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 5
/$SC_$CPU_TO_ADDPACKET Stream=dwell3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write "Opening MD HK Page."
page $SC_$CPU_MD_HK

write ";  Enable DEBUG Event Messages "
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";  Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "

;; Check the MD tlm items to see if they are 0 or NULL

if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3.3: Verify that each MD table data is initialized to 0. "
write ";*********************************************************************"
passed = 1
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  if ($SC_$CPU_MD_DwPktOffset[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  endif
  
  if ($SC_$CPU_MD_DwTblEntry[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwTblEntry[", i, "] = ", $SC_$CPU_MD_DwTblEntry[i]
  endif
ENDDO

if (passed = 1) THEN
  write "<*> Passed - Memory Dwell table data is initialized properly."
else
  write "<!> Failed - Memory Dwell table data is NOT initialized!"
endif

write ";*********************************************************************"
write ";  Step 2.3.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0000') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.3.5: Verify that the entries that were modified in the"
write ";  FIRST, SECOND and LAST tables were restored "
write ";*********************************************************************"
;; Dump table #1 to confirm that table was restored properly
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_reset235","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 1."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = tbl1JamAddr) then
  write "<*> Passed (9004) - Dwell table #1 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #1 was not restored upon reset."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",tbl1JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif
; FIRST table should have address count 2, rate 1 and data size 5
if ($SC_$CPU_MD_AddrCnt[1] = 2) and ($SC_$CPU_MD_Rate[1] = 1) and ($SC_$CPU_MD_DataSize[1] = 5) THEN
  write "<*> Passed (9004) - FIRST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - FIRST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 5)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #2 verification
;;*******************************
;; Dump the table to confirm that table was restored after the reset
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl2_reset235","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 2."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = tbl2JamAddr) then
  write "<*> Passed (9004) - Dwell table #2 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #2 was not restored upon reset."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay
  write "- Expected Offset of ",tbl2JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif
; SECOND table should have address count 2, rate 2 and data size 3
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 2) and ($SC_$CPU_MD_DataSize[2] = 3) THEN
  write "<*> Passed (9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 3)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #3 verification
;;*******************************
; THIRD table should have address count 2, rate 1 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[3] = 2) and ($SC_$CPU_MD_Rate[3] = 1) and ($SC_$CPU_MD_DataSize[3] = 6) THEN
  write "<*> Passed (9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 6)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #4 verification
;;*******************************
;; Dump the table to confirm that the Jam commands above updated the table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl4_reset235","$CPU",dwell_tbl4_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 4."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = tbl4JamAddr) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset = 0) then
  write "<*> Passed (9004) - The jammed entries have been found in the table."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - The table did not contain the jammed entries."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay
  write "- Expected Offset of ",tbl4JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset
  write "- Expected Length of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length
  write "- Expected Delay of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay
  write "- Expected Offset of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; LAST table should have address count 3, rate 1 and data size 4
if ($SC_$CPU_MD_AddrCnt[4] = 3) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 4) THEN
  write "<*> Passed (9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 3)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 4)"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4:  Start FIRST and LAST tables and Perform MD"
write ";  Application Reset  "
write ";********************************************************************"
write ";  Step 2.4.1:  Send Start Dwell command, enabling FIRST and LAST"
write ";  dwell tables"
write ";********************************************************************"
; Send start dwell command, enabling FIRST and LAST tables
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'1001'"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;2000) - Start Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (1004;2000) - Start Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_2000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_START_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'1001') then
  write "<*> Passed (2000) - FIRST & LAST Dwell Tables were started"
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

;; Print the table values prior to the reset
write " Dwell Table Values prior to an MD Application Reset"
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  write "  DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  write "  DwTblEntry[", i, "]  = ", $SC_$CPU_MD_DwTblEntry[i]
ENDDO

;; Set variables for each MD table to check after reset
;; NOTE: THIS CODE ASSUMES ONLY 4 TABLES which is the delivered DEFAULT
local offsetTbl1 = $SC_$CPU_MD_DwPktOffset[1]
local entryTbl1 = $SC_$CPU_MD_DwTblEntry[1]
local offsetTbl2 = $SC_$CPU_MD_DwPktOffset[2]
local entryTbl2 = $SC_$CPU_MD_DwTblEntry[2]
local offsetTbl3 = $SC_$CPU_MD_DwPktOffset[3]
local entryTbl3 = $SC_$CPU_MD_DwTblEntry[3]
local offsetTbl4 = $SC_$CPU_MD_DwPktOffset[4]
local entryTbl4 = $SC_$CPU_MD_DwTblEntry[4]

write ";*********************************************************************"
write ";  Step 2.4.2: Perform an MD application reset  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_RESTART_APP_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RECOVERED_TBL_VALID_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 3

/$SC_$CPU_ES_RESTARTAPP APPLICATION=MDAppName

; Wait for table recovery events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 4
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9004) - All MD Dwell Table recovery messages were received"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Not all of the MD Dwell Table recovery messages were received!"
  ut_setrequirements MD_9004, "F"
endif

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES restart app Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

wait 5

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

write ";  Enable DEBUG Event Messages "
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";  Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "

;; Check the MD tlm items to see if they are 0 or NULL

if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";*********************************************************************"
write ";  Step 2.4.3: Verify that each MD table data is initialized properly."
write ";*********************************************************************"
;; Since an Application Reset occurred, the tables should contain the values
;; they had prior to the reset.
;; Print the table values after the reset
write " Dwell Table Values after the MD Application Reset"
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  write "  DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  write "  DwTblEntry[", i, "]  = ", $SC_$CPU_MD_DwTblEntry[i]
ENDDO

passed = 1
;; Check the Offset values
if ($SC_$CPU_MD_DwPktOffset[1] != offsetTbl1) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #1 Offset was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwPktOffset[2] != offsetTbl2) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #2 Offset was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwPktOffset[3] != offsetTbl3) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #3 Offset was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwPktOffset[4] != offsetTbl4) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #4 Offset was not saved across an application reset!"
endif

;; Check the Entry values
if ($SC_$CPU_MD_DwTblEntry[1] != entryTbl1) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #1 Entry was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwTblEntry[2] != entryTbl2) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #2 Entry was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwTblEntry[3] != entryTbl3) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #3 Entry was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwTblEntry[4] != entryTbl4) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #4 Entry was not saved across an application reset!"
endif

if (passed = 1) then
  write "<*> Passed (9004) - Memory Dwell table data is initialized properly."
  ut_setrequirements MD_9004, "P"
else
  ut_setrequirements MD_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'1001') then
  write "<*> Passed (9003,9004) - FIRST & LAST Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4.5: Verify that the entries that were modified in the"
write ";  FIRST, SECOND & LAST tables were restored "
write ";*********************************************************************"
;; Dump table #1 to confirm that table was restored properly
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_reset245","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 1."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = tbl1JamAddr) then
  write "<*> Passed (9004) - Dwell table #1 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #1 was not restored upon reset."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",tbl1JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif
; FIRST table should have address count 2, rate 1 and data size 5
if ($SC_$CPU_MD_AddrCnt[1] = 2) and ($SC_$CPU_MD_Rate[1] = 1) and ($SC_$CPU_MD_DataSize[1] = 5) THEN
  write "<*> Passed (9004) - FIRST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - FIRST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 5)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #2 verification
;;*******************************
;; Dump the table to confirm that table was restored after the reset
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl2_reset245","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 2."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = tbl2JamAddr) then
  write "<*> Passed (9004) - Dwell table #2 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #2 was not restored upon reset."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay
  write "- Expected Offset of ",tbl2JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif
; SECOND table should have address count 2, rate 2 and data size 3
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 2) and ($SC_$CPU_MD_DataSize[2] = 3) THEN
  write "<*> Passed (9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
  ut_setrequirements MD_9004_1, "P"
else
  write "<!> Failed (9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 3)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #3 verification
;;*******************************
; THIRD table should have address count 2, rate 1 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[3] = 2) and ($SC_$CPU_MD_Rate[3] = 1) and ($SC_$CPU_MD_DataSize[3] = 6) THEN
  write "<*> Passed (9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
  ut_setrequirements MD_9004_1, "P"
else
  write "<!> Failed (9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 6)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #4 verification
;;*******************************
;; Dump the table to confirm that the Jam commands above updated the table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl4_reset245","$CPU",dwell_tbl4_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 4."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = tbl4JamAddr) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset = 0) then
  write "<*> Passed (9004) - The jammed entries have been found in the table."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - The table did not contain the jammed entries."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay
  write "- Expected Offset of ",tbl4JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset
  write "- Expected Length of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length
  write "- Expected Delay of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay
  write "- Expected Offset of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; LAST table should have address count 3, rate 1 and data size 4
if ($SC_$CPU_MD_AddrCnt[4] = 3) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 4) THEN
  write "<*> Passed (9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 3)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 4)"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.5:  cFE Processor Reset With Invalid Data"
write ";*********************************************************************"
write ";  Step 2.5.1: Reset the ES Processor Reset Count in order for the CDS"
write ";  to be restored. The prior steps issued 2 Processor Resets and the "
write ";  default ES Processor Reset count is 2 before a Power-on reset occurs."
write ";*********************************************************************"
/$SC_$CPU_ES_ResetPRCnt
wait 5

write ";*********************************************************************"
write ";  Step 2.5.2: Perform a processor reset  "
write ";*********************************************************************"
;; Print the table values prior to the reset
write " Dwell Table Values prior to a Processor Reset"
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  write "  DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  write "  DwTblEntry[", i, "]  = ", $SC_$CPU_MD_DwTblEntry[i]
ENDDO

;; Set variables for each MD table to check after reset
;; NOTE: THIS CODE ASSUMES ONLY 4 TABLES which is the delivered DEFAULT
offsetTbl1 = $SC_$CPU_MD_DwPktOffset[1]
entryTbl1 = $SC_$CPU_MD_DwTblEntry[1]
offsetTbl2 = $SC_$CPU_MD_DwPktOffset[2]
entryTbl2 = $SC_$CPU_MD_DwTblEntry[2]
offsetTbl3 = $SC_$CPU_MD_DwPktOffset[3]
entryTbl3 = $SC_$CPU_MD_DwTblEntry[3]
offsetTbl4 = $SC_$CPU_MD_DwPktOffset[4]
entryTbl4 = $SC_$CPU_MD_DwTblEntry[4]

/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";********************************************************************"
write ";  Step 2.5.3:  Corrupt the CDS such that it has an entry with an"
write ";  invalid length in the first table"
write ";********************************************************************"
local addval

addval = 16 + CFE_ES_RESET_AREA_SIZE + (CFE_ES_RAM_DISK_SECTOR_SIZE * CFE_ES_RAM_DISK_NUM_SECTORS) + 20 + 4

write " Corrupt the first dwell table Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Click the Refresh button on the $SC_$CPU_ES_CDS_REGISTRY page"
write "     4. Note the CDS ""Handle"" for ",MDTblName1," on that page"
write "     5. Add the CDS Handle of ",MDTblName1," to the sum calculated in Step 2."
write "     6. m <value calculated in 5>,2"
write "     7. Enter 6 and hit the enter or return key"
write "     8. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
page $SC_$CPU_ES_CDS_REGISTRY
wait

write ";  Start the Memory Dwell Test (TST_MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MD", TST_MD_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MD","$CPU","TST_MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - TST_MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU","CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU",{MDAppName},MD_TBL_INIT_INF_EID,"INFO", 2
ut_setupevents "$SC", "$CPU",{MDAppName},MD_RECOVERED_TBL_VALID_INF_EID, "INFO", 3
ut_setupevents "$SC", "$CPU",{MDAppName},MD_INIT_INF_EID, "INFO", 4

s load_start_app (MDAppName,"$CPU","MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

;; Check that the proper number of events were rcv'd
if ($SC_$CPU_find_event[2].num_found_messages = 1) AND ;;
   ($SC_$CPU_find_event[3].num_found_messages = 3) then
  write "<*> Passed (9004;9004.1) - Three MD Dwell Table recovery messages were received as expected."
  ut_setrequirements MD_9004, "P"
  ut_setrequirements MD_9004_1, "P"
else
  write "<!> Failed (9004;9004.1) - Did not receive correct number of MD Dwell Table recovery messages!"
  ut_setrequirements MD_9004, "F"
  ut_setrequirements MD_9004_1, "F"
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'890'
dwell1 = x'891'
dwell2 = x'892'
dwell3 = x'893'
dwell4 = x'894'

if ("$CPU" = "CPU2") then
  stream1 = x'990'
  dwell1 = x'991'
  dwell2 = x'992'
  dwell3 = x'993'
  dwell4 = x'994'
elseif ("$CPU" = "CPU3") then
  stream1 = x'A90'
  dwell1 = x'A91'
  dwell2 = x'A92'
  dwell3 = x'A93'
  dwell4 = x'A94'
endif

write "Sending commands to add subscriptions for MD HK and dwell packets."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 5
/$SC_$CPU_TO_ADDPACKET Stream=dwell3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write "Opening MD HK Page."
page $SC_$CPU_MD_HK

write ";  Enable DEBUG Event Messages "
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";  Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "

;; Check the MD tlm items to see if they are 0 or NULL
if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.5.4: Verify that each MD table's stats are initialized to 0. "
write ";*********************************************************************"
;; Since a Processor Reset occurred, the tables should contain the values they
;; had prior to the reset with the exception of Table #1 which was corrupted.

;; Print the table values after the reset
write " Dwell Table Values after the Processor Reset"
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  write "  DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  write "  DwTblEntry[", i, "]  = ", $SC_$CPU_MD_DwTblEntry[i]
ENDDO

passed = 1
;; Check the Offset values
if ($SC_$CPU_MD_DwPktOffset[1] != 0) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #1 Offset was not reset to 0 !"
endif
if ($SC_$CPU_MD_DwPktOffset[2] != offsetTbl2) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #2 Offset was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwPktOffset[3] != offsetTbl3) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #3 Offset was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwPktOffset[4] != offsetTbl4) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #4 Offset was not saved across an application reset!"
endif

;; Check the Entry values
if ($SC_$CPU_MD_DwTblEntry[1] != 0) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #1 Entry was not reset to 0!"
endif
if ($SC_$CPU_MD_DwTblEntry[2] != entryTbl2) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #2 Entry was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwTblEntry[3] != entryTbl3) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #3 Entry was not saved across an application reset!"
endif
if ($SC_$CPU_MD_DwTblEntry[4] != entryTbl4) then
  passed = 0;
  write "<!> Failed (9004) - Memory Dwell table #4 Entry was not saved across an application reset!"
endif

if (passed = 1) then
  write "<*> Passed (9004) - Memory Dwell table data is initialized properly."
  ut_setrequirements MD_9004, "P"
else
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.5.5: Verify that enabled state was restored for the LAST"
write ";  table, but the FIRST table and the other tables are disabled."
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'1000') then
  write "<*> Passed (9003,9004,9004.1) - LAST Dwell Table was restarted, FIRST was not."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
  ut_setrequirements MD_9004_1, "P"
else
  write "<!> Failed (9003,9004,9004.1) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
  ut_setrequirements MD_9004_1, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5.6: Verify that the FIRST table has been initialized with"
write ";  default values, while the modified entries in the SECOND and LAST"
write ";  tables were restored"
write ";*********************************************************************"
;; Dump table #1 to confirm that table was restored properly
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_reset255","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 1."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; FIRST table should now be empty.
if ($SC_$CPU_MD_AddrCnt[1] = 0) and ($SC_$CPU_MD_Rate[1] = 0) and ($SC_$CPU_MD_DataSize[1] = 0) THEN
  write "<*> Passed (9004,9004.1) - FIRST Dwell Table was cleared as expected!"
  ut_setrequirements MD_9004, "P"
  ut_setrequirements MD_9004_1, "P"
else
  write "<!> Failed (9004,9004.1) - FIRST Dwell Table was not cleared!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 0)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 0)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 0)"
  ut_setrequirements MD_9004, "F"
  ut_setrequirements MD_9004_1, "F"
endif

;;*******************************
;; Dwell Table #2 verification
;;*******************************
;; Dump the table to confirm that table was restored after the reset
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl2_reset255","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 2."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = tbl2JamAddr) then
  write "<*> Passed (9004) - Dwell table #2 was restored after the reset."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Dwell table #2 was not restored upon reset."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay
  write "- Expected Offset of ",tbl2JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; SECOND table should have address count 2, rate 2 and data size 3
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 2) and ($SC_$CPU_MD_DataSize[2] = 3) THEN
  write "<*> Passed (9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 3)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #3 verification
;;*******************************
; THIRD table should have address count 2, rate 1 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[3] = 2) and ($SC_$CPU_MD_Rate[3] = 1) and ($SC_$CPU_MD_DataSize[3] = 6) THEN
  write "<*> Passed (9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 6)"
  ut_setrequirements MD_9004, "F"
endif

;;*******************************
;; Dwell Table #4 verification
;;*******************************
;; Dump the table to confirm that the Jam commands above updated the table
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl4_reset255","$CPU",dwell_tbl4_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 4."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = tbl4JamAddr) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset = 0) then
  write "<*> Passed (9004) - The jammed entries have been found in the table."
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - The table did not contain the jammed entries."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay
  write "- Expected Offset of ",tbl4JamAddr, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset
  write "- Expected Length of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length
  write "- Expected Delay of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay
  write "- Expected Offset of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset
  ut_setrequirements MD_9004, "F"
endif

; LAST table should have address count 3, rate 1 and data size 4
if ($SC_$CPU_MD_AddrCnt[4] = 3) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 4) THEN
  write "<*> Passed (9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 3)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 4)"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6:  Power On Reset  "
write ";*********************************************************************"
write ";  Step 2.6.1: Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";  Start the Memory Dwell Test (TST_MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MD", TST_MD_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MD","$CPU","TST_MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - TST_MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'92D'

if ("$CPU" = "CPU2") then
  stream1 = x'A2D'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B2D'
endif

write "Sending command to add subscription for TST_MD HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 2

write "Opening TST MD HK Page."
page $SC_$CPU_TST_MD_HK

wait 5

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 2

s load_start_app (MDAppName,"$CPU","MD_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MD Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MD not received."
  endif
else
  write "<!> Failed - MD Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'890'
dwell1 = x'891'
dwell2 = x'892'
dwell3 = x'893'
dwell4 = x'894'

if ("$CPU" = "CPU2") then
  stream1 = x'990'
  dwell1 = x'991'
  dwell2 = x'992'
  dwell3 = x'993'
  dwell4 = x'994'
elseif ("$CPU" = "CPU3") then
  stream1 = x'A90'
  dwell1 = x'A91'
  dwell2 = x'A92'
  dwell3 = x'A93'
  dwell4 = x'A94'
endif

write "Sending commands to add subscriptions for MD HK and dwell packets."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 5
/$SC_$CPU_TO_ADDPACKET Stream=dwell3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=dwell4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write "Opening MD HK Page."
page $SC_$CPU_MD_HK

write ";  Enable DEBUG Event Messages "
cmdcnt = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";  Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "

;; Check the HK tlm items to see if they are 0 or NULL
if ($SC_$CPU_MD_CMDPC = 0) AND ($SC_$CPU_MD_CMDEC = 0) THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MD_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
  ut_setrequirements MD_9000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6.2: Verify that each MD table is disabled. "
write ";*********************************************************************"
if ($SC_$CPU_MD_EnableMask = 0) THEN
  write "<*> Passed (9001) - Enable Mask initialized properly."
  ut_setrequirements MD_9001, "P"
else
  write "<!> Failed (9001) - Enable Mask was NOT initialized at startup."
  write "  EnableMask           = ",$SC_$CPU_MD_EnableMask
  ut_setrequirements MD_9001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6.3: Verify that each MD table is initialized to 0. "
write ";*********************************************************************"
passed = 1
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  if ($SC_$CPU_MD_AddrCnt[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_AddrCnt[", i, "] = ", $SC_$CPU_MD_AddrCnt[i]
  endif
  
  if ($SC_$CPU_MD_Rate[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_Rate[", i, "] = ", $SC_$CPU_MD_Rate[i]
  endif

  if ($SC_$CPU_MD_DataSize[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DataSize[", i, "] = ", $SC_$CPU_MD_DataSize[i]
  endif
  
  if ($SC_$CPU_MD_DwPktOffset[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwPktOffset[", i, "] = ", $SC_$CPU_MD_DwPktOffset[i]
  endif
  
  if ($SC_$CPU_MD_DwTblEntry[i] != 0) THEN
    passed = 0
    write "$SC_$CPU_MD_DwTblEntry[", i, "] = ", $SC_$CPU_MD_DwTblEntry[i]
  endif
ENDDO

if (passed = 1) THEN
  write "<*> Passed (9002) - Memory Dwell tables initialized properly."
  ut_setrequirements MD_9002, "P"
else
  write "<!> Failed (9002) - Memory Dwell tables NOT initialized at startup."
  ut_setrequirements MD_9002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6.4: Verify that each MD table has been returned to"
write ";  its initialized state"
write ";*********************************************************************"
; Every table should now be empty
passed = 1
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  if ($SC_$CPU_MD_AddrCnt[i] = 0) and ($SC_$CPU_MD_Rate[i] = 0) and ($SC_$CPU_MD_DataSize[i] = 0) THEN
    write "<*> Passed (9002) - Dwell Table ", i, " was cleared as expected!"
  else
    passed = 0
    write "<!> Failed (9002) - Dwell Table ", i, " was not cleared!"
    write "$SC_$CPU_MD_AddrCnt[",i,"] = ", $SC_$CPU_MD_AddrCnt[i], " (Expected 0)"
    write "$SC_$CPU_MD_Rate[",i,"] = ", $SC_$CPU_MD_Rate[i], " (Expected 0)"
    write "$SC_$CPU_MD_DataSize[",i,"] = ", $SC_$CPU_MD_DataSize[i], " (Expected 0)"
  endif
ENDDO

if (passed = 1) THEN
  write "<*> Passed (9002) - Memory Dwell tables initialized properly."
  ut_setrequirements MD_9002, "P"
else
  write "<!> Failed (9002) - Memory Dwell tables NOT initialized at startup."
  ut_setrequirements MD_9002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Clean-up from this test."
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

write "Closing MD HK Page."
clear $SC_$CPU_MD_HK

write "Closing TST MD HK Page."
clear $SC_$CPU_TST_MD_HK

write "Closing Dwell Packet Pages."
clear $sc_$cpu_md_dwell_pkt1
clear $sc_$cpu_md_dwell_pkt2
clear $sc_$cpu_md_dwell_pkt3
clear $sc_$cpu_md_dwell_pkt4

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_md_InitReset  "
write ";*********************************************************************"
ENDPROC
