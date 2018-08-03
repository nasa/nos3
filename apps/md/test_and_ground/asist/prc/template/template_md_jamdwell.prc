PROC $sc_$cpu_md_jamdwell
;*******************************************************************************
;  Test Name:  MD_JamDwell
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to verify that the Memory Dwell (MD) Jam
;   Dwell command functions properly.
;   
;
;  Requirements Tested
;    MD1003     If Dwell Table ID specified in any MD command exceeds the
;               <PLATFORM_DEFINED> maximum number of allowable memory dwells,
;               MD shall reject the commans and issue an event message.
;    MD1004     If MD accepts any command as valid, MD shall execute the
;               command, increment the MD Valid Command Counter and issue an
;               event message.
;    MD1005     If MD rejects any command, MD shall abort the command execution,;               increment the MD Command Rejected Counter and issue an event
;               message.
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
;    MD4000.1	If the Dwell Table index is greater than <PLATFORM_DEFINED>
;		maximum then MD shall reject the command
;    MD4000.2	If the command-specified address fails validation, MD shall
;		reject the command. Validation includes:
;			a) If a symbolic address is specified, Symbol Table is
;			   present and symbolic address is contained in the
;			   Symbol Table,
;			b) resolved address (numerical value of symbolic address
;			   if present + offset address) is within valid range
;			c) if resolved address is specified for a 2-byte dwell,
;			   address is an even value,
;			d) if resolved address is specified for a 4-byte dwell,
;			   address is a multiple integral of 4
;    MD4000.3	If the memory Dwell table being jammed is enabled and the sum of
;		all the 'delay between samples' for the memory dwell table
;		equals 0, then MD shall issue an event message informing that
;		the table will not be processing dwell packets in its current
;		state.
;    MD4000.5	If the command-specified Number of Bytes is not 0,1,2 or 4, MD
;		shall reject the command.
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
;
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
;   Date          Name      	Description
;   09/30/08      S. Jonke  	Original Procedure
;   12/04/09      W. Moleski    Turned logging off around code that did not
;                               provide any significant benefit of logging
;   01/23/12      W. Moleski    Added variable names for the app, table names
;				and ram disk
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

#define MD_1003     0
#define MD_1004     1
#define MD_1005     2
#define MD_2000     3
#define MD_4000     4
#define MD_4000_1   5
#define MD_4000_2   6
#define MD_4000_3   7
#define MD_4000_5   8
#define MD_8000     9
#define MD_9000     10
#define MD_9001     11
#define MD_9002     12

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 12
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1003", "MD_1004", "MD_1005", "MD_2000", "MD_4000", "MD_4000_1", "MD_4000_2", "MD_4000_3", "MD_4000_5", "MD_8000", "MD_9000", "MD_9001", "MD_9002"]

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

s load_start_app ("MD","$CPU","MD_AppMain")

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

write ";*********************************************************************"
write ";  Step 1.7: Setup initial dwell tables"
write ";*********************************************************************"
write ";  Setup initial dwell table 1"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+0
;   2       2       0      testdata_addr+2
;   3       0       0      0
;
;  Entries = 2;  Size = 4;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table #1 with signature"

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command failed."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #1."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName1"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #1 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #1 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #1 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #1. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName1"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #1 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #1 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write "; Setup initial dwell table 2"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       4       0      testdata_addr+0
;   2       2       0      testdata_addr+4
;   3       2       0      testdata_addr+6
;   4       2       0      testdata_addr+8
;   5       2       0      testdata_addr+10
;   6       4       0      testdata_addr+12
;   7       2       0      testdata_addr+16
;   8       2       0      testdata_addr+18
;   9       2       0      testdata_addr+20
;   10      2       0      testdata_addr+22
;   11      4       0      testdata_addr+24
;   12      2       0      testdata_addr+28
;   13      2       0      testdata_addr+30
;   14      4       0      testdata_addr+32
;   15      2       4      testdata_addr+36
;   16      0       0      0
;
;  Entries = 15;  Size = 38;  Total Delay = 4

; Set up dwell load table 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Signature="Table #2 with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Entry #2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Entry #3
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Offset = testdata_addr+6
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Entry #4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_Offset = testdata_addr+8
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_SymName = ""

; Entry #5
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[5].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[5].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[5].MD_TLE_Offset = testdata_addr+10
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[5].MD_TLE_SymName = ""

; Entry #6
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[6].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[6].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[6].MD_TLE_Offset = testdata_addr+12
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[6].MD_TLE_SymName = ""

; Entry #7
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[7].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[7].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[7].MD_TLE_Offset = testdata_addr+16
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[7].MD_TLE_SymName = ""

; Entry #8
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[8].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[8].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[8].MD_TLE_Offset = testdata_addr+18
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[8].MD_TLE_SymName = ""

; Entry #9
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[9].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[9].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[9].MD_TLE_Offset = testdata_addr+20
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[9].MD_TLE_SymName = ""

; Entry #10
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[10].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[10].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[10].MD_TLE_Offset = testdata_addr+22
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[10].MD_TLE_SymName = ""

; Entry #11
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[11].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[11].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[11].MD_TLE_Offset = testdata_addr+24
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[11].MD_TLE_SymName = ""

; Entry #12
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Offset = testdata_addr+28
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_SymName = ""

; Entry #13
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_Offset = testdata_addr+30
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_SymName = ""

; Entry #14
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Offset = testdata_addr+32
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_SymName = ""

; Entry #15 - with a delay of 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[15].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[15].MD_TLE_Delay = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[15].MD_TLE_Offset = testdata_addr+36
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[15].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[16].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[16].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[16].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[16].MD_TLE_SymName = ""

; Create a load file for dwell table #2 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl2_load_appid,"Dwell Table #2 Load", "md_dwl_ld_sg_tbl2",MDTblName2, "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[16]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command failed."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #2."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName2"

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

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #2 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #2. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName2"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #2 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #2 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write "; Setup initial dwell table 3"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+6
;   2       4       0      testdata_addr+8
;   3       0       0      0
;
;  Entries = 2;  Size = 6;  Total Delay = 1

;;; Set up dwell load table 3
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Signature="Table #3 with signature"

$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+6
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+8
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #3 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl3_load_appid,"Dwell Table #3 Load", "md_dwl_ld_sg_tbl3",MDTblName3, "$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl3", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command failed."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #3."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName3"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #3 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #3 Validate command."
  endif
else
  write "<!> Failed - InActive Table #3 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #3 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #3 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #3 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #3. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName3"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #3 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #3 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write "; Setup initial dwell table 4"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       0      testdata_addr+80
;   2       2       0      testdata_addr+82
;   3       4       1      testdata_addr+84
;   4       2       0      testdata_addr+88
;   5       2       0      testdata_addr+90
;   6       2       0      testdata_addr+200
;   7       2       0      testdata_addr+202
;   8       2       0      testdata_addr+204
;   9       2       0      testdata_addr+206
;   10      2       0      testdata_addr+208
;   11      2       0      testdata_addr+210
;   12      4       2      testdata_addr+212
;   13      2       0      testdata_addr+216
;   14      2       0      testdata_addr+218
;   15      4       0      testdata_addr+0
;   16      2       0      testdata_addr+4
;   17      2       0      testdata_addr+6
;   18      2       1      testdata_addr+8
;   19      2       1      testdata_addr+10
;   20      4       0      testdata_addr+12
;   21      2       0      testdata_addr+16
;   22      2       0      testdata_addr+248
;   23      2       0      testdata_addr+250
;   24      2       0      testdata_addr+252
;   25      2       1      testdata_addr+254
;
;  Entries = 25;  Size = 58;  Total Delay = 6

; Delays in the data collection (from 0 base)
;   bytes 0 - 8:    +0
;   bytes 9 - 28:   +1    (1 second delay after 8th byte)
;   bytes 29 - 42:  +3    (2 second delay after 28th byte)
;   bytes 43 - 44:  +4    (1 second delay after 42nd byte)
;   bytes 45 - 58:  +5    (1 second delay after 44th byte)

;;; Set up dwell load table 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Signature="Table #4 with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+80
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Entry #2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+82
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Entry #3 with delay 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = testdata_addr+84
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Entry #4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset = testdata_addr+88
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_SymName = ""

; Entry #5
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_Offset = testdata_addr+90
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_SymName = ""

; Entry #6
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_Offset = testdata_addr+200
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_SymName = ""

; Entry #7
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[7].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[7].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[7].MD_TLE_Offset = testdata_addr+202
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[7].MD_TLE_SymName = ""

; Entry #8
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[8].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[8].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[8].MD_TLE_Offset = testdata_addr+204
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[8].MD_TLE_SymName = ""

; Entry #9
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[9].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[9].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[9].MD_TLE_Offset = testdata_addr+206
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[9].MD_TLE_SymName = ""

; Entry #10
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[10].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[10].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[10].MD_TLE_Offset = testdata_addr+208
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[10].MD_TLE_SymName = ""

; Entry #11
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[11].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[11].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[11].MD_TLE_Offset = testdata_addr+210
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[11].MD_TLE_SymName = ""

; Entry #12 - with a delay of 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[12].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[12].MD_TLE_Delay = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[12].MD_TLE_Offset = testdata_addr+212
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[12].MD_TLE_SymName = ""

; Entry #13
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[13].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[13].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[13].MD_TLE_Offset = testdata_addr+216
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[13].MD_TLE_SymName = ""

; Entry #14
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[14].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[14].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[14].MD_TLE_Offset = testdata_addr+218
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[14].MD_TLE_SymName = ""

; Entry #15
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[15].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[15].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[15].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[15].MD_TLE_SymName = ""

; Entry #16
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[16].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[16].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[16].MD_TLE_Offset = testdata_addr+4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[16].MD_TLE_SymName = ""

; Entry #17
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[17].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[17].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[17].MD_TLE_Offset = testdata_addr+6
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[17].MD_TLE_SymName = ""

; Entry #18 with delay of 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[18].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[18].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[18].MD_TLE_Offset = testdata_addr+8
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[18].MD_TLE_SymName = ""

; Entry #19 with delay of 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[19].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[19].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[19].MD_TLE_Offset = testdata_addr+10
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[19].MD_TLE_SymName = ""

; Entry #20
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[20].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[20].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[20].MD_TLE_Offset = testdata_addr+12
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[20].MD_TLE_SymName = ""

; Entry #21
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[21].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[21].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[21].MD_TLE_Offset = testdata_addr+16
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[21].MD_TLE_SymName = ""

; Entry #22
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[22].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[22].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[22].MD_TLE_Offset = testdata_addr+248
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[22].MD_TLE_SymName = ""

; Entry #23
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[23].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[23].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[23].MD_TLE_Offset = testdata_addr+250
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[23].MD_TLE_SymName = ""

; Entry #24
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[24].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[24].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[24].MD_TLE_Offset = testdata_addr+252
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[24].MD_TLE_SymName = ""

; Entry #25
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Offset = testdata_addr+254
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_SymName = ""

; No terminator entry since whole table is used

; Create a load file for dwell table #4 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl4_load_appid,"Dwell Table #4 Load", "md_dwl_ld_sg_tbl4",MDTblName4, "$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl4", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command failed."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #4."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName4"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #4 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #4 Validate command."
  endif
else
  write "<!> Failed - InActive Table #4 validation failed."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #4 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #4 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #4 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #4. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName4"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #4 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #4 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")

wait 10

write ";*********************************************************************"
write ";  Step 2.0:  Table Disabled - Valid Cases"
write ";*********************************************************************"
write ";  Step 2.1:  Jam Dwell on FIRST table, FIRST entry  "
write ";********************************************************************"
write ";  Step 2.1.1:  Send Jam Dwell command, modifying FIRST table, FIRST"
write ";  entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+100

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=1 FieldLength=4 DwellDelay=2 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 2.1.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Display the Dwell Table #1 page
page $SC_$CPU_MD_DWELL_LOAD_TBL1

; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_jam212","$CPU",dwell_tbl1_load_pkt)

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
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 2 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2:  Jam Dwell on SECOND table, MIDDLE entry  "
write ";********************************************************************"
write ";  Step 2.2.1:  Send Jam Dwell command, modifying SECOND table,"
write ";  MIDDLE entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+104

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=12 FieldLength=1 DwellDelay=1 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 2.2.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Display the Dwell Table #2 page
page $SC_$CPU_MD_DWELL_LOAD_TBL2

; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl1_jam222","$CPU",dwell_tbl2_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3:  Jam Dwell on LAST table, LAST entry  "
write ";********************************************************************"
write ";  Step 2.3.1:  Send Jam Dwell command, modifying LAST table, LAST"
write ";  entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+108

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=25 FieldLength=4 DwellDelay=0 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 2.3.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Display the Dwell Table #4 page
page $SC_$CPU_MD_DWELL_LOAD_TBL4

; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl1_jam232","$CPU",dwell_tbl4_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Length
  write "- Expected Delay of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4:  Jam Dwell on FIRST table, past end of entries  "
write ";********************************************************************"
write ";  Step 2.4.1:  Send Jam Dwell command, modifying FIRST table, past"
write ";  end of entries"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+40

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=4 FieldLength=4 DwellDelay=2 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 2.4.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_jam242","$CPU",dwell_tbl1_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Delay = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Length
  write "- Expected Delay of 2 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Table Disabled - Invalid Cases"
write ";*********************************************************************"
write ";  Step 3.1:  Jam Dwell with Invalid Table Index  "
write ";********************************************************************"
write ";  Step 3.1.1:  Send Jam Dwell command specifying FIFTH (invalid)"
write ";  Dwell Table"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_JAM_TABLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

;;;/$SC_$CPU_MD_JAMDWELL TableId=5 EntryId=1 FieldLength=2 DwellDelay=0 Offset=0 SymName=""

;; CPU1 is the default
rawcmd = "1890C000004D04FA00050001000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000004D04FA00050001000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000004D04FA00050001000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
endif

ut_sendrawcmd "SCX_CPU1_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;1005;4000.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1003, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_1, "P"
else
  write "<!> Failed (1003;1005;4000.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_INVALID_JAM_TABLE_ERR_EID, "."
  ut_setrequirements MD_1003, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_1, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2:  Jam Dwell with Invalid Address  "
write ";********************************************************************"
write ";  Step 3.2.1:  Send Jam Dwell command for FIRST table, but"
write ";  specifying invalid address"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_JAM_ADDR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=2 FieldLength=2 DwellDelay=0 Offset=x'8000002' SymName=""

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_INVALID_JAM_ADDR_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3:  Jam Dwell with Invalid # of Bytes  "
write ";********************************************************************"
write ";  Step 3.3.1:  Send Jam Dwell command for SECOND table, FIRST"
write ";  entry, but # of bytes 3 specified (invalid)"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_LEN_ARG_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=1 FieldLength=3 DwellDelay=0 Offset=x'10' SymName=""

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.5) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_5, "P"
else
  write "<!> Failed (1005;4000.5) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_INVALID_LEN_ARG_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_5, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4:  Jam Dwell with # of bytes 2, address not word aligned  "
write ";********************************************************************"
write ";  Step 3.4.1:  Send Jam Dwell command for THIRD table, MIDDLE"
write ";  entry, # of bytes 2, but not word aligned"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_ADDR_NOT_16BIT_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=11 FieldLength=2 DwellDelay=0 Offset=x'21' SymName=""

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_JAM_ADDR_NOT_16BIT_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Jam Dwell with # of bytes 4, address not long word aligned."
write ";********************************************************************"
write ";  Step 3.5.1: Send Jam Dwell command for THIRD table, MIDDLE"
write ";  entry, # of bytes 4, but not long word aligned"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_ADDR_NOT_32BIT_ERR_EID,"ERROR", 1
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_DWELL_INF_EID, "INFO", 2

cmdcnt = $SC_$CPU_MD_CMDPC + 1
errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=13 FieldLength=4 DwellDelay=0 Offset=x'22' SymName=""

if (MD_ENFORCE_DWORD_ALIGN = 1) then
  ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1005) - Command Rejected Counter incremented."
    ut_setrequirements MD_1005, "P"
  else
    write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
    ut_setrequirements MD_1005, "F"
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements MD_1005, "P"
    ut_setrequirements MD_4000_2, "P"
  else
    write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_JAM_ADDR_NOT_32BIT_ERR_EID, "."
    ut_setrequirements MD_1005, "F"
    ut_setrequirements MD_4000_2, "F"
  endif
else
  ut_tlmwait $SC_$CPU_MD_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1005) - Command Accepted Counter incremented."
    ut_setrequirements MD_1005, "P"
  else
    write "<!> Failed (1005) - Command Accepted Counter did not increment as expected."
    ut_setrequirements MD_1005, "F"
  endif

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[2].eventid, " received"
    ut_setrequirements MD_1005, "P"
    ut_setrequirements MD_4000_2, "P"
  else
    write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_JAM_DWELL_INF_EID, "."
    ut_setrequirements MD_1005, "F"
    ut_setrequirements MD_4000_2, "F"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: Jam Dwell with Invalid Symbol"
write ";********************************************************************"
write ";  Step 3.6.1: Send Jam Dwell command for LAST table, LAST entry,"
write ";  # of bytes 2, specifying an invalid symbol"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_CANT_RESOLVE_JAM_ADDR_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=25 FieldLength=2 DwellDelay=0 Offset=x'30' SymName="InvalidSymbolName"

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

wait 5


write ";*********************************************************************"
write ";  Step 4.0:  Table Enabled - Valid Cases"
write ";*********************************************************************"
write ";  Step 4.1:  Jam Dwell on FIRST table, FIRST entry  "
write ";********************************************************************"
write ";  Step 4.1.1:  Send Start Dwell command enabling ALL tables"
write ";********************************************************************"
write "Opening Dwell Packet Pages"
page $sc_$cpu_md_dwell_pkt1
page $sc_$cpu_md_dwell_pkt2
page $sc_$cpu_md_dwell_pkt3
page $sc_$cpu_md_dwell_pkt4

ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'1111'"
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
if ($SC_$CPU_MD_EnableMask = b'1111') then
  write "<*> Passed (2000) - All Dwell Tables were started."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask wasn't the expected value!"
  write "$SC_$CPU_MD_EnableMask = ", $SC_$CPU_MD_EnableMask, " and should have been ", b'1111'
  ut_setrequirements MD_2000, "F"
endif

write ";********************************************************************"
write ";  Step 4.1.2:  Send Jam Dwell command, modifying FIRST table, FIRST"
write ";  entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+128

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=1 FieldLength=1 DwellDelay=3 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 4.1.3:  Verify that the entry has been updated"
write ";********************************************************************"
; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_jam413","$CPU",dwell_tbl1_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 3) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 3 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2:  Jam Dwell on SECOND table, MIDDLE entry  "
write ";********************************************************************"
write ";  Step 4.2.1:  Send Jam Dwell command, modifying SECOND table,"
write ";  MIDDLE entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+130

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=14 FieldLength=1 DwellDelay=1 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 4.2.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl2_jam422","$CPU",dwell_tbl2_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Length = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[14].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3:  Jam Dwell on LAST table, LAST entry  "
write ";********************************************************************"
write ";  Step 4.3.1:  Send Jam Dwell command, modifying LAST table, LAST"
write ";  entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+132

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=25 FieldLength=2 DwellDelay=2 Offset=workaddress SymName="""""
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

write ";********************************************************************"
write ";  Step 4.3.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Dump the current active dwell load table 4
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl4_jam432","$CPU",dwell_tbl4_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Length = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Delay = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 2 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Length
  write "- Expected Delay of 2 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4:  Jam Dwell on THIRD table, creating table with Total"
write ";  Delay of 0  "
write ";********************************************************************"
write ";  Step 4.4.1:  Send Jam Dwell commands modifying the table to have"
write ";  a total delay of 0"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_ZERO_RATE_CMD_INF_EID, "INFO", 2

workaddress = testdata_addr+116

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=1 FieldLength=4 DwellDelay=0 Offset=workaddress SymName="""""
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

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (4000.3) - No packets will be sent Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_4000_3, "P"
else
  write "<!> Failed (4000.3) - No packets will be sent message was NOT received!"
  ut_setrequirements MD_4000_3, "F"
endif

write ";********************************************************************"
write ";  Step 4.4.2:  Verify that the entry has been updated"
write ";********************************************************************"
; Display the Dwell Table #3 page
page $SC_$CPU_MD_DWELL_LOAD_TBL3

; Dump the current active dwell load table 3
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName3,"A","$cpu_tbl3_jam442","$CPU",dwell_tbl3_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Delay = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Offset = workaddress) then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of ",workaddress, " - table = ",$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Offset
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0:  Table Enabled - Invalid Cases"
write ";*********************************************************************"
write ";  Step 5.1:  Jam Dwell with Invalid Table Index  "
write ";********************************************************************"
write ";  Step 5.1.1:  Send Jam Dwell command specifying FIFTH (invalid)"
write ";  Dwell Table"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_JAM_TABLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

;; CPU1 is the default
rawcmd = "1890C000004D04FA00050001000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000004D04FA00050001000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000004D04FA00050001000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
endif

ut_sendrawcmd "SCX_CPU1_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;1005;4000.1) - Command Rejected Counter incremented."
  ut_setrequirements MD_1003, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_1, "P"
else
  write "<!> Failed (1003;1005;4000.1) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1003, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_1, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;1005;4000.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1003, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_1, "P"
else
  write "<!> Failed (1003;1005;4000.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_INVALID_JAM_TABLE_ERR_EID, "."
  ut_setrequirements MD_1003, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_1, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.2:  Jam Dwell with Invalid Address  "
write ";********************************************************************"
write ";  Step 5.2.1:  Send Jam Dwell command for FIRST table, but"
write ";  specifying invalid address"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_JAM_ADDR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=2 FieldLength=2 DwellDelay=0 Offset=x'8000002' SymName=""

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_INVALID_JAM_ADDR_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3:  Jam Dwell with Invalid # of Bytes  "
write ";********************************************************************"
write ";  Step 5.3.1:  Send Jam Dwell command for SECOND table, FIRST"
write ";  entry, but # of bytes 3 specified (invalid)"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_LEN_ARG_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=1 FieldLength=3 DwellDelay=0 Offset=x'10' SymName=""

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.5) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_5, "P"
else
  write "<!> Failed (1005;4000.5) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_5, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.5) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_5, "P"
else
  write "<!> Failed (1005;4000.5) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_INVALID_LEN_ARG_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_5, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4:  Jam Dwell with # of bytes 2, address not word aligned  "
write ";********************************************************************"
write ";  Step 5.4.1:  Send Jam Dwell command for THIRD table, MIDDLE"
write ";  entry, # of bytes 2, but not word aligned"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_ADDR_NOT_16BIT_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=11 FieldLength=2 DwellDelay=0 Offset=x'21' SymName=""

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_JAM_ADDR_NOT_16BIT_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5: Jam Dwell with # of bytes 4, address not long word aligned."
write ";********************************************************************"
write ";  Step 5.5.1: Send Jam Dwell command for THIRD table, MIDDLE"
write ";  entry, # of bytes 4, but not long word aligned"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_ADDR_NOT_32BIT_ERR_EID,"ERROR", 1
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_DWELL_INF_EID, "INFO", 2

cmdcnt = $SC_$CPU_MD_CMDPC + 1
errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=13 FieldLength=4 DwellDelay=0 Offset=x'22' SymName=""

if (MD_ENFORCE_DWORD_ALIGN = 1) then
  ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1005;4000.2) - Command Rejected Counter incremented."
    ut_setrequirements MD_1005, "P"
    ut_setrequirements MD_4000_2, "P"
  else
    write "<!> Failed (1005;4000.2) - Command Rejected Counter did not increment as expected."
    ut_setrequirements MD_1005, "F"
    ut_setrequirements MD_4000_2, "F"
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements MD_1005, "P"
    ut_setrequirements MD_4000_2, "P"
  else
    write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_JAM_ADDR_NOT_32BIT_ERR_EID, "."
    ut_setrequirements MD_1005, "F"
    ut_setrequirements MD_4000_2, "F"
  endif
else
  ut_tlmwait $SC_$CPU_MD_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1005) - Command Accepted Counter incremented."
    ut_setrequirements MD_1005, "P"
  else
    write "<!> Failed (1005) - Command Accepted Counter did not increment as expected."
    ut_setrequirements MD_1005, "F"
  endif

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[2].eventid, " received"
    ut_setrequirements MD_1005, "P"
    ut_setrequirements MD_4000_2, "P"
  else
    write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_JAM_DWELL_INF_EID, "."
    ut_setrequirements MD_1005, "F"
    ut_setrequirements MD_4000_2, "F"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6:  Jam Dwell with Invalid Symbol"
write ";********************************************************************"
write ";  Step 5.6.1:  Send Jam Dwell command for LAST table, LAST entry,"
write ";  # of bytes 2, specifying an invalid symbol"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=25 FieldLength=2 DwellDelay=0 Offset=x'30' SymName="InvalidSymbolName"

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005;4000.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_4000_2, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_4000_2, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0:  Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $SC_$CPU_md_JamDwell                                 "
write ";*********************************************************************"
ENDPROC
