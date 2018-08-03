PROC $sc_$cpu_md_signatures
;*******************************************************************************
;  Test Name:  MD_Signatures
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to verify that the Memory Dwell (MD) Set
;   Dwell Table Signature command functions properly. Memory Dwell Table
;   Signature support is optional and thus provided in a separate test.
;   If the mission provides Memory Dwell Table Signature support, this
;   test can be used to verify its functionality.
;
;  Requirements Tested
;    MD1002	For all MD commands, if the length contained in the message
;		header is not equal to the expected length, MD shall reject
;		the command and issue an event message.
;    MD1004	If MD accepts any command as valid, MD shall execute the
;		command, increment the MD Valid Command Counter and issue an
;		event message
;    MD1005	If MD rejects any command, MD shall abort the command execution,
;		increment the MD Command Rejected Counter and issue an error
;		event message
;    MD2000     Upon receipt of a Start Dwell command, MD shall identify the
;               command-specified tables as ENABLED and start processing the
;               command-specified memory dwell tables, starting with the first
;               entry, until one of the following:
;			a) an entry that has a zero value for the Number of
;			   Bytes field
;			b) until it has processed the last entry in a Dwell
;			   Table
;    MD3001     When MD collects all of the data specified in a memory dwell
;               table (as defined in MD3000.2), MD shall issue a memory dwell
;               message containing the following:
;			a) Table ID
;			b) <OPTIONAL> Signature
;			c) Number of bytes sampled
;			d) Data
;    MD3002     Upon receipt of a Table Load, MD shall verify the contents of
;               the table and if the table is invalid, reject the table.
;    MD5000	<OPTIONAL> Upon receipt of a Set Dwell Table Signature Command,
;		the signature field for the specified Dwell Table shall be set
;		to the command-specified string.
;    MD5000.1	If the command-specified signature exceeds <PLATFORM_DEFINED>
;		maximum length then the command shall be rejected. Note that
;		the signature must be 32-bit aligned.
;    MD8000	MD shall generate a housekeeping message containing the
;		following:
;			a)  Valid Command Counter
;			b)  Command Rejected Counter
;			c)  For each Dwell:
;				1.  Enable/Disable Status
;				2.  Number of Dwell Addresses
;				3.  Dwell Rate
;				4.  Number of Bytes
;				5.  Current Dwell Packet Index
;				6.  Current Entry in the Dwell Table
;				7.  Current Countdown counter
;    MD9000	Upon any Initialization of the MD Application (cFE Power On, cFE;		Processor Reset or MD Application Reset),  MD shall initialize
;		the following data to Zero
;			a)  Valid Command Counter
;			b)  Command Rejected Counter
;    MD9001	Upon cFE Power-on Reset, MD shall initialize each Memory Dwell
;		table status to DISABLED
;    MD9002	Upon cFE Power-on Reset, MD shall initialize each Memory Dwell
;		table to zero
;    MD9003     MD shall store the following information whenever it changes
;               (in support of a cFE Processor Reset or Application Reset):
;                       a) Enable/Disable Status for each Dwell
;                       b) <OPTIONAL> signature for each dwell
;                       c) Contents of each Dwell Table
;    MD9004     On a cFE Processor Reset or a MD Application Reset, MD shall
;               restore the information specified in MD9003.
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
;   08/8/08       S. Jonke 	Original Procedure
;   12/07/09      W. Moleski    Added requirements to this prolog and also
;                               turned logging off around code that did not
;                               provide any significant benefit of logging
;   01/23/12      W. Moleski    Added variable names for the app, table names
;                               and ram disk
;
;  Arguments
;   None
;
;  Procedures Called
;   None 
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

%liv (log_procedure) = logging

#define MD_1002     0
#define MD_1004     1
#define MD_1005     2
#define MD_2000     3
#define MD_3001     4
#define MD_3002     5
#define MD_5000     6
#define MD_5000_1   7
#define MD_8000     8
#define MD_9000     9
#define MD_9001     10
#define MD_9002     11
#define MD_9003     12
#define MD_9004     13

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 13
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1002", "MD_1004", "MD_1005", "MD_2000", "MD_3001", "MD_3002", "MD_5000", "MD_5000_1", "MD_8000", "MD_9000", "MD_9001", "MD_9002", "MD_9003", "MD_9004"]

local cmdcnt, errcnt
local rawcmd
local stream1, dwell1, dwell2, dwell3, dwell4
local passed
LOCAL dwell_tbl1_load_pkt, dwell_tbl2_load_pkt, dwell_tbl3_load_pkt, dwell_tbl4_load_pkt
LOCAL dwell_tbl1_load_appid, dwell_tbl2_load_appid, dwell_tbl3_load_appid, dwell_tbl4_load_appid
local dwl_tbl_1_index, dwl_tbl_2_index, dwl_tbl_3_index, dwl_tbl_4_index
local testdata_addr, addr
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

write ";*********************************************************************"
write ";  Step 1.7: Setup initial dwell tables "
write ";*********************************************************************"
write ";  Setup initial dwell table 1"
write ";*********************************************************************"
; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","md_dwl_tbl1_a","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Active Dump command failed."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; Set up the table data
;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+1
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
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
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
    write "<!> Failed (3002) - Inactive Dwell Table #1 validation. Event Message not received."
    ut_setrequirements MD_3002, "F"
  else
    write "<*> Passed (3002) - Inactive Dwell Table #1 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
    ut_setrequirements MD_3002, "P"  
  endif
else
  write "<*> Passed (3002) - Inactive Dwell Table #1 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  ut_setrequirements MD_3002, "P"  
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
write ";  Setup initial dwell table 2"
write ";*********************************************************************"
; Dump the current active dwell load table 2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","md_dwl_tbl2_a","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Active Dump command failed."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; Set up the table data
;;; Set up dwell load table 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Signature="Table with signature"

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #2 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl2_load_appid,"Dwell Table #2 Load", "md_dwl_ld_sg_tbl2",MDTblName2, "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
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
    write "<!> Failed (3002) - Inactive Dwell Table #2 validation. Event Message not received."
    ut_setrequirements MD_3002, "F"
  else
    write "<*> Passed (3002) - Inactive Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
    ut_setrequirements MD_3002, "P"  
  endif
else
  write "<*> Passed (3002) - Inactive Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  ut_setrequirements MD_3002, "P"  
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
write ";  Setup initial dwell table 3"
write ";*********************************************************************"
; Dump the current active dwell load table 3
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName3,"A","md_dwl_tbl3_a","$CPU",dwell_tbl3_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; Set up the table data
;;; Set up dwell load table 3
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Signature="Table with signature"

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
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
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
    write "<!> Failed (3002) - Inactive Dwell Table #3 validation. Event Message not received."
    ut_setrequirements MD_3002, "F"
  else
    write "<*> Passed (3002) - Inactive Dwell Table #3 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
    ut_setrequirements MD_3002, "P"  
  endif
else
  write "<*> Passed (3002) - Inactive Dwell Table #3 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  ut_setrequirements MD_3002, "P"  
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
write ";  Setup initial dwell table 4"
write ";*********************************************************************"
; Dump the current active dwell load table 4
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","md_dwl_tbl4_a","$CPU",dwell_tbl4_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; Set up the table data
;;; Set up dwell load table 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Signature="Table with signature"

$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+12
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+13
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #4 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl4_load_appid,"Dwell Table #4 Load", "md_dwl_ld_sg_tbl4",MDTblName4, "$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl4", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
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
    write "<!> Failed (3002) - Inactive Dwell Table #4 validation. Event Message not received."
    ut_setrequirements MD_3002, "F"
  else
    write "<*> Passed (3002) - Inactive Dwell Table #4 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
    ut_setrequirements MD_3002, "P"  
  endif
else
  write "<*> Passed (3002) - Inactive Dwell Table #4 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  ut_setrequirements MD_3002, "P"  
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
write ";  Step 2.0:  Set Signature Command - Valid Cases"
write ";*********************************************************************"
write ";  Step 2.1:  0 Character Signature  "
write ";********************************************************************"
; Send Set Signature command on FIRST dwell table with a null string signature
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_SET_SIGNATURE_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_SetSignature TableId=1 Signature="""""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;5000) - Set Signature command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (1004;2000) - Set Signature command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_5000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_SET_SIGNATURE_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;;***************************************************
; Dump the table to verify that the signature was set
;;***************************************************
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName1,"A","$cpu_tbl1_sig2.1","$CPU",dwell_tbl1_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Dwell Table #1 Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the Dwell Load Signature 
if ($SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature = "") then 
  write "<*> Passed (5000) - Signature was found in the table #1 dump."
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (5000) - Expected Signature ''; Found '",$SC_$CPU_MD_DWELL_LOAD_TBL1.MD_DTL_Signature,"'."
  ut_setrequirements MD_5000, "F"
endif

; Send Start Dwell command, enabling the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'0001'"
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
if ($SC_$CPU_MD_EnableMask = b'0001') then
  write "<*> Passed (2000) - First Dwell Table was started."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

page $sc_$cpu_md_dwell_pkt1

wait 5

; Verify that memory dwell messages are received and contain the signature
ut_tlmwait $SC_$CPU_MD_DwlPkt1Signature, ""
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Signature appears in the dwell packet."
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Signature does not appear correctly in the dwell packet."
  ut_setrequirements MD_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.2:  1 Character Signature  "
write ";********************************************************************"
; Send Set Signature command on SECOND dwell table with a 1 character signature
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_SET_SIGNATURE_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_SetSignature TableId=2 Signature=""1"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;5000) - Set Signature command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (1004;2000) - Set Signature command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_5000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_SET_SIGNATURE_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;; *************************************************
; Dump the table to verify that the signature was set
;; *************************************************
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_tbl2_sig22","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Dwell Table #2 Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the Dwell Load Signature 
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Signature = "1") then 
  write "<*> Passed (5000) - Signature was found in the table #2 dump."
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (5000) - Expected Signature '1'; Found '",$SC_$CPU_MD_DWELL_LOAD_TBL2.MD_DTL_Signature,"'."
  ut_setrequirements MD_5000, "F"
endif

; Send Start Dwell command, enabling the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'0010'"
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
if ($SC_$CPU_MD_EnableMask = b'0011') then
  write "<*> Passed (2000) - Second Dwell Table was started, first dwell table still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

page $sc_$cpu_md_dwell_pkt2

wait 5

; Verify that memory dwell messages are received and contain the signature
ut_tlmwait $SC_$CPU_MD_DwlPkt2Signature, "1"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Signature appears in the dwell packet."
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Signature does not appear correctly in the dwell packet."
  ut_setrequirements MD_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.3:  Half Maximum Length Signature  "
write ";********************************************************************"
; Send Set Signature command on THIRD dwell table with a signature of half the
; maximum length (max is 32, so signature is 16 characters)

ut_setupevents "$SC", "$CPU", {MDAppName}, MD_SET_SIGNATURE_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_SetSignature TableId=3 Signature=""1234567890123456"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;5000) - Set Signature command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (1004;2000) - Set Signature command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_5000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_SET_SIGNATURE_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;; ***********************************************
;; Dump the table to verify that the signature was set
;; ***********************************************
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName3,"A","$cpu_tbl3_sig23","$CPU",dwell_tbl3_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Dwell Table #3 Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the Dwell Load Signature 
if ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Signature = "1234567890123456") then 
  write "<*> Passed (5000) - Signature was found in the table #3 dump."
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (5000) - Expected Signature '1234567890123456'; Found '",$SC_$CPU_MD_DWELL_LOAD_TBL3.MD_DTL_Signature,"'."
  ut_setrequirements MD_5000, "F"
endif

; Send Start Dwell command, enabling the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'0100'"
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
if ($SC_$CPU_MD_EnableMask = b'0111') then
  write "<*> Passed (2000) - Third Dwell Table was started, first & second dwell tables still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

page $sc_$cpu_md_dwell_pkt3

wait 5

; Verify that memory dwell messages are received and contain the signature
ut_tlmwait $SC_$CPU_MD_DwlPkt3Signature, "1234567890123456"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Signature appears in the dwell packet."
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4:  Maximum Length Signature  "
write ";********************************************************************"
; Send Set Signature command on FOURTH dwell table with a signature of
; maximum length (max is 32, so 31 characters plus null character)

ut_setupevents "$SC", "$CPU", {MDAppName}, MD_SET_SIGNATURE_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_SetSignature TableId=4 Signature=""1234567890123456789012345678901"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;5000) - Set Signature command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (1004;2000) - Set Signature command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_5000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_SET_SIGNATURE_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

;; *****************************************
;; Dump the table to verify that the signature was set
;; *****************************************
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_tbl4_sig24","$CPU",dwell_tbl4_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Dwell Table #4 Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the Dwell Load Signature 
if ($SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Signature = "1234567890123456789012345678901") then 
  write "<*> Passed (5000) - Signature was found in the table #4 dump."
  ut_setrequirements MD_5000, "P"
else
  write "<!> Failed (5000) - Expected Signature '1234567890123456789012345678901'; Found '",$SC_$CPU_MD_DWELL_LOAD_TBL4.MD_DTL_Signature,"'."
  ut_setrequirements MD_5000, "F"
endif

; Send Start Dwell command, enabling the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'1000'"
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
  write "<*> Passed (2000) - Fourth Dwell Table was started, other dwell tables still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

page $sc_$cpu_md_dwell_pkt4

wait 5

; Verify that memory dwell messages are received and contain the signature
ut_tlmwait $SC_$CPU_MD_DwlPkt4Signature, "1234567890123456789012345678901"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Signature appears in the dwell packet."
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Signature does not appear correctly in the dwell packet."
  ut_setrequirements MD_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 3.0:  Set Signature Command - Reset Cases"
write ";*********************************************************************"
write ";  Step 3.1:  cFE Processor Reset  "
write ";*********************************************************************"
write ";  Step 3.1.1: Perform a processor reset  "
write ";********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

; Set the local signature values so we can see them be restored
$SC_$CPU_MD_DwlPkt1Signature = "Signature reset"
$SC_$CPU_MD_DwlPkt2Signature = "Signature reset"
$SC_$CPU_MD_DwlPkt3Signature = "Signature reset"
$SC_$CPU_MD_DwlPkt4Signature = "Signature reset"

write ";*********************************************************************"
write ";  Step 3.1.2: Start the Memory Dwell (MD) Application and "
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
write ";  Step 3.1.3:  Start the Memory Dwell Test (TST_MD) Application and "
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
write ";  Step 3.1.4: Enable DEBUG Event Messages "
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
write ";  Step 3.1.5: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'1111') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
  
  ; for purposes of the rest of the test, start up the dwell tables
  ; Send Start Dwell command, enabling all dwell tables
  /$SC_$CPU_MD_StartDwell TableMask=b'1111'

  ; Check MD_EnableMask
  ut_tlmwait $SC_$CPU_MD_EnableMask, b'1111'
  if (UT_TW_Status <> UT_Success) then
    write "<!> Failed - could not restart dwell tables after processor reset"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.1.6: Verify that the signatures are in the dwell packets "
write ";*********************************************************************"
; Verify that memory dwell messages are received and contain the signature
; Dwell table 1
ut_tlmwait $SC_$CPU_MD_DwlPkt1Signature, ""
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 1."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #1."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; Dwell table 2
ut_tlmwait $SC_$CPU_MD_DwlPkt2Signature, "1"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 2."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #2."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; Dwell table 3
ut_tlmwait $SC_$CPU_MD_DwlPkt3Signature, "1234567890123456"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 3."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #3."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; Dwell table 4
ut_tlmwait $SC_$CPU_MD_DwlPkt4Signature, "1234567890123456789012345678901"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 4."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #4."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2:  Application Reset  "
write ";*********************************************************************"
write ";  Step 3.2.1: Perform an application reset  "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 1

/$SC_$CPU_ES_RESTARTAPP APPLICATION=MDAppName
wait 5

; Set the local signature values so we can see them be restored
$SC_$CPU_MD_DwlPkt1Signature = "Signature reset"
$SC_$CPU_MD_DwlPkt2Signature = "Signature reset"
$SC_$CPU_MD_DwlPkt3Signature = "Signature reset"
$SC_$CPU_MD_DwlPkt4Signature = "Signature reset"

wait 5

write ";*********************************************************************"
write ";  Step 3.2.2: Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
if ($SC_$CPU_MD_CMDPC != 0) OR ($SC_$CPU_MD_CMDEC != 0) THEN
  write "<!> Failed - Housekeeping telemetry NOT re-initialized at restart."
  write "  CMDPC           = ",$SC_$CPU_MD_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MD_CMDEC
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

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";*********************************************************************"
write ";  Step 3.2.3: Enable DEBUG Event Messages "
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
write ";  Step 3.2.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'1111') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
  
  ; for purposes of the rest of the test, start up the dwell tables
  ; Send Start Dwell command, enabling all dwell tables
  /$SC_$CPU_MD_StartDwell TableMask=b'1111'

  ; Check MD_EnableMask
  ut_tlmwait $SC_$CPU_MD_EnableMask, b'1111'
  if (UT_TW_Status <> UT_Success) then
    write "<!> Failed - could not restart dwell tables after processor reset"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.5: Verify that the signatures are in the dwell packets "
write ";*********************************************************************"
; Verify that memory dwell messages are received and contain the signature
; Dwell table 1
ut_tlmwait $SC_$CPU_MD_DwlPkt1Signature, ""
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 1."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #1."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; Dwell table 2
ut_tlmwait $SC_$CPU_MD_DwlPkt2Signature, "1"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 2."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #2."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; Dwell table 3
ut_tlmwait $SC_$CPU_MD_DwlPkt3Signature, "1234567890123456"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 3."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #3."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; Dwell table 4
ut_tlmwait $SC_$CPU_MD_DwlPkt4Signature, "1234567890123456789012345678901"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9003,9004) - Signature appears in dwell packet 4."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - Signature does not appear correctly in the dwell packet for Tbl #4."
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3:  Power On Reset  "
write ";*********************************************************************"
write ";  Step 3.3.1: Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: Start the Memory Dwell (MD) Application and "
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
write ";  Step 3.3.3:  Start the Memory Dwell Test (TST_MD) Application and "
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
write ";  Step 3.3.4: Enable DEBUG Event Messages "
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
write ";  Step 3.3.5: Verify that the MD Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
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
write ";  Step 3.3.6: Verify that each MD table is disabled. "
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
write ";  Step 3.3.7: Verify that each MD table is initialized to 0. "
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
write ";  Step 3.3.8: Verify that each MD table has been returned to"
write ";  its initialized state including signatures. "
write ";*********************************************************************"
local tableName,dumpFileName
local loadPktId,dataName
;; Dump each Dwell Table
for i = 1 to MD_NUM_DWELL_TABLES do
  ; Dump the dwell table
  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

  tableName = MDAppName & ".DWELL_TABLE" & i
  dumpFileName = "$cpu_tbl" & i & "_md_sig338"
  if (i = 1) then
    loadPktId = dwell_tbl1_load_pkt
  elseif (i = 2) then
    loadPktId = dwell_tbl2_load_pkt
  elseif (i = 3) then
    loadPktId = dwell_tbl3_load_pkt
  else
    loadPktId = dwell_tbl4_load_pkt
  endif

  cmdcnt = $SC_$CPU_TBL_CMDPC+1

  s get_tbl_to_cvt (ramDir,tableName,"A",dumpFileName,"$CPU",loadPktId)

  ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
  if (UT_TW_Status <> UT_Success) then
    write "<!> Failed: Dump command for Dwell Table ",i,"."
  endif

  ; Check for event message
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
      write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
    endif
  endif

  ;; ********************************************
  ;; Check each load table for defaults
  ;; ********************************************
  passed = 1
  ;; Verify the Enabled Flag is 0
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Enabled"
  if ({dataName} <> 0) then
    passed = 0
    write "<!> Failed - Enable Flag for Tbl #",i," = ",{dataName},"; expected 0."
  endif
                                                                                
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Signature"
  if ({dataName} <> "") then
    passed = 0
    write "<!> Failed - Signature for Tbl #",i," = ",{dataName},"; Expected ''"
  endif

  ;; loop for each table entry
  for j = 1 to MD_DWELL_TABLE_SIZE do
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Length"
    if ({dataName} <> 0) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] length = ",{dataName},"; Expected 0."
    endif
                                                                                
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Delay"
    if ({dataName} <> 0) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] delay = ",{dataName},"; Expected 0."
    endif

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Offset"
    if ({dataName} <> 0) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] offset = ",{dataName},"; Expected 0."
    endif
                                                                                
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_SymName"
    if ({dataName} <> "") then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] symName = ",{dataName},"; Expected ''."
    endif
  enddo
enddo

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  Set Signature Command - Invalid Cases"
write ";*********************************************************************"
write ";  Step 4.1:  Greater Than Maximum Length Signature  "
write ";********************************************************************"
; Send Set Signature command on THIRD dwell table with a signature of greater
; than maximum length (max is 32, so 32 characters plus null character)
;;ut_setupevents "$SC", "$CPU", {MDAppName}, MD_SIGNATURE_TOO_LONG_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_SIGNATURE_LENGTH_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_SetSignature TableId=3 Signature="12345678901234567890123456789012"

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;5000.1) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_5000_1, "P"
else
  write "<!> Failed (1005;5000.1) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_5000_1, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<!> Passed (1005;5000.1) - Event message ",$SC_$CPU_find_event[1].eventid," received."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_5000_1, "P"
else
  write "<!> Failed (1005;5000.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_SIGNATURE_TOO_LONG_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_5000_1, "F"
endif

;;************************************************************
; Dump the table to verify that the signature was not changed
;;************************************************************
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName3,"A","$cpu_tbl3_sig41","$CPU",dwell_tbl3_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Active Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the Dwell Load Signature 
if ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Signature = "12345678901234567890123456789012") then 
  write "<!> Failed (5000) - 32 char signature was accepted when rejection was expected."
  ut_setrequirements MD_5000, "F"
else
  write "<*> Passed (5000) - 32-char Signature was not found in table #3 dump."
  ut_setrequirements MD_5000, "P"
endif

write ";*********************************************************************"
write ";  Step 4.2:  Invalid Command Length  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
rawcmd = ""

;; CPU1 is the default
rawcmd = "1890C000006F05A8000000000000000000000000000000000000000000000000000000000000000000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000006F05A8000000000000000000000000000000000000000000000000000000000000000000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000006F05A8000000000000000000000000000000000000000000000000000000000000000000000000"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1002;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_CMD_LEN_ERR_EID, "."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0:  Table Load"
write ";*********************************************************************"
write ";  Step 5.1:  Load Table With Signatures  "
write ";********************************************************************"
; Dump the current active dwell load table 2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","md_dwl_tbl2_a","$CPU",dwell_tbl2_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - Dwell Table #2 Dump command."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

; Set up the table data
;;; Set up dwell load table 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Signature="Table #2 with signature"

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+32
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+36
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Offset = testdata_addr+38
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4].MD_TLE_SymName = ""

; Create a load file for dwell table #2 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl2_load_appid,"Dwell Table #2 Load", "md_dwl_ld_sg_tbl2",MDTblName2, "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[4]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

wait 5
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 5

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for dwell table #2......"
wait until ($SC_$CPU_RF[dwl_tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

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
  write "<!> Failed - Dwell Table #2 validation."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed (3002) - Dwell Table #2 validation. Event Message not received."
    ut_setrequirements MD_3002, "F"
  else
    write "<*> Passed (3002) - Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
    ut_setrequirements MD_3002, "P"  
  endif
else
  write "<*> Passed (3002) - Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  ut_setrequirements MD_3002, "P"  
endif
                                                                                
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
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 5

/$SC_$CPU_MD_STARTDWELL TableMask=b'0010'
page $SC_$CPU_MD_DWELL_PKT2
wait 5

; Verify that memory dwell messages are received and contain the signature
ut_tlmwait $SC_$CPU_MD_DwlPkt2Signature, "Table #2 with signature"
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Signature appears in the dwell packet for Tbl #2."
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Signature in dwell packet is '",$SC_$CPU_MD_DwlPkt2Signature, "'. Expected 'Table #2 with signature'."
  ut_setrequirements MD_3001, "F"
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
write ";  End procedure $SC_$CPU_md_Signatures                                    "
write ";*********************************************************************"
ENDPROC
