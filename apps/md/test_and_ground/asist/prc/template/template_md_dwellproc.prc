PROC $sc_$cpu_md_dwellproc
;*******************************************************************************
;  Test Name:  MD_DwellProc
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to verify that Memory Dwell (MD) dwell
;   tables are processed properly.
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
;    MD2001     Upon receipt of a Stop Dwell command, MD shall identify the
;               command-specified memory dwell tables as DISABLED and stop
;               processing the command-specified memory dwell tables.
;    MD3000     During each memory dwell cycle, MD shall collect data specified
;		in each enabled memory dwell table which contains the
;		following:
;			a) Table ID
;			b) <Optional> signature
;			c) For each desired sample up to <PLATFORM_DEFINED>
;			   entries:
;				1) address
;				2) number of bytes
;				3) delay between samples
;    MD3000.2   The collection shall be done for each entry in an active Memory
;		Dwell Table, starting with the first entry, until one of the
;		following:
;			a) it reaches an entry that has a zero value for the
;			   Number of Bytes parameter or
;			b) until it has processed the last entry in a Dwell
;			   Table 
;    MD3000.3	Data collection occurs only when a Dwell Table in both ENABLED
;		and has a non-zero dwell rate 
;    MD3001	When MD collects all of the data specified in a memory dwell
;		table (as defined in MD3000.2), MD shall issue a memory dwell
;		message containing the following:
;			a) Table ID
;			b) <OPTIONAL> Signature
;			c) Number of bytes sampled
;			d) Data 
;    MD3002	Upon receipt of a Table Load, MD shall verify the contents of
;		the table and if the table is invalid, reject the table. 
;    MD3002.2	If any address fails validation, MD shall reject the table.
;		Validation includes:
;			a) If a symbolic address is specified, Symbol Table is
;			   present and symbolic address is contained in the
;			   Symbol Table. 
;			b) resolved address (numerical value of symbolic address
;			   + offset address) is within valid range
;			c) if resolved address is specified for a 2-byte dwell,
;			   address is an even value,
;			d) if resolved address is specified for a 4-byte dwell,
;			   address is an integral multiple of 4
;    MD3002.5	If the Number of Bytes is not 0,1,2 or 4, MD shall reject the
;		table. 
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
;    MD9000     Upon any initialization of the MD Application (cFE Power On, cFE
;               Processor Reset or MD Application Reset), MD shall initialize
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
;   Date          Name          Description
;   09/9/08       S. Jonke	Original Procedure
;   12/04/09      W. Moleski    Added requirements to this prolog and also
;                               turned logging off around code that did not
;                               provide any significant benefit of logging
;   04/28/11      W. Moleski    Added variables for the App and table names 
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
#define MD_2001     2
#define MD_3000     3
#define MD_3000_2   4
#define MD_3000_3   5
#define MD_3001     6
#define MD_3002     7
#define MD_3002_2   8
#define MD_3002_5   9
#define MD_4000     10
#define MD_8000     11
#define MD_9000     12
#define MD_9001     13
#define MD_9002     14

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 14
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1004", "MD_2000", "MD_2001", "MD_3000", "MD_3000_2", "MD_3000_3", "MD_3001", "MD_3002", "MD_3002_2", "MD_3002_5", "MD_4000", "MD_8000", "MD_9000", "MD_9001", "MD_9002"]

local i, j
local cmdcnt, errcnt
local rawcmd
local stream1, dwell1, dwell2, dwell3, dwell4
local passed
LOCAL dwell_tbl1_load_pkt, dwell_tbl2_load_pkt, dwell_tbl3_load_pkt, dwell_tbl4_load_pkt
LOCAL dwell_tbl1_load_appid, dwell_tbl2_load_appid, dwell_tbl3_load_appid, dwell_tbl4_load_appid
local dwl_tbl_1_index, dwl_tbl_2_index, dwl_tbl_3_index, dwl_tbl_4_index
local testdata_addr, addr, workaddress
local errcnt
local dwell_pkt1_appid, dwell_pkt2_appid, dwell_pkt3_appid, dwell_pkt4_appid
local oldval, nextval, passed
local cnt

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
dwell_pkt1_appid = "091"
dwell_pkt2_appid = "092"
dwell_pkt3_appid = "093"
dwell_pkt4_appid = "094"

if ("$CPU" = "CPU2") then
  dwell_tbl1_load_pkt = "0FC6"
  dwell_tbl2_load_pkt = "0FC7"
  dwell_tbl3_load_pkt = "0FC8"
  dwell_tbl4_load_pkt = "0FC9"
  dwell_tbl1_load_appid = 4038
  dwell_tbl2_load_appid = 4039
  dwell_tbl3_load_appid = 4040
  dwell_tbl4_load_appid = 4041
  dwell_pkt1_appid = "191"
  dwell_pkt2_appid = "192"
  dwell_pkt3_appid = "193"
  dwell_pkt4_appid = "194"
elseif ("$CPU" = "CPU3") then
  dwell_tbl1_load_pkt = "0FE6"
  dwell_tbl2_load_pkt = "0FE7"
  dwell_tbl3_load_pkt = "0FE8"
  dwell_tbl4_load_pkt = "0FE9"
  dwell_tbl1_load_appid = 4070
  dwell_tbl2_load_appid = 4071
  dwell_tbl3_load_appid = 4072
  dwell_tbl4_load_appid = 4073
  dwell_pkt1_appid = "291"
  dwell_pkt2_appid = "292"
  dwell_pkt3_appid = "293"
  dwell_pkt4_appid = "294"
endif

; Local dwell data storage holding 30 entries of up to 62 bytes each
local dwelldata[30,62]
local dataScnt[30]
local seqDiff

;; Turn off logging for the initialization
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

clear the local dwell data array
for i = 1 to 30 do
  for j = 1 to 62 do
    dwelldata[i,j] = 0
  enddo
enddo

%liv (log_procedure) = logging

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

s load_start_app ("TST_MD","$CPU", "TST_MD_AppMain")

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

; APID for TST_MD HK is not set in stone yet, currently using x'B27'

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
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

wait 10

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR
 
write "Test Data Address = ", testdata_addr

write ";*********************************************************************"
write ";  Step 1.2:  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 2

s load_start_app (MDAppName,"$CPU", "MD_AppMain")

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
;;; CPU1 is the default
local hkPktId
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
write ";  Step 2.0:  Memory Dwell Processing - Valid Tables"
write ";*********************************************************************"
write ";  Step 2.1:  Table Containing One Entry  "
write ";********************************************************************"
write ";  Step 2.1.1:  Load a dwell table that contains only one entry"
write ";  followed by a terminator entry with 0 for the number of bytes"
write ";********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       1       1      testdata_addr+0
;   2       0       0      0
;
;  Entries 1;  Size = 1;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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
    write "<!> Failed - Inactive Dwell Table #1 validation failed. Event Message not received."
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

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1004) - Activate command sent properly."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Activate command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002) - Dwell Table #1 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
else
  write "<!> Failed (3002) - Dwell Table #1 activation. Activation Success Event Message not received!"
  ut_setrequirements MD_3002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Load pending event message received."
else
  write "<!> Failed - Load pending eent message ",CFE_TBL_LOAD_PEND_REQ_INF_EID," not received."
endif

wait 5

write ";********************************************************************"
write ";  Step 2.1.2:  Send Start Dwell command, enabling the dwell table"
write ";********************************************************************"
write "Opening Dwell Packet 1 Page"
page $sc_$cpu_md_dwell_pkt1

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

; Start the test data incrementing
ut_setupevents "$SC", "$CPU", "TST_MD", TST_MD_STARTDATA_INF_EID, "INFO", 1

/$SC_$CPU_TST_MD_StartData
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_MD_STARTDATA_INF_EID, "."
endif

ut_tlmwait  $SC_$CPU_TST_MD_DATASTATE, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Test data started"
else
  write "<!> Failed - Test data was not started!"
endif

write ";********************************************************************"
write ";  Step 2.1.3:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging 
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt1_appid & "scnt"}
while ({"p" & dwell_pkt1_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt1TableId = 1) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 1"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 1!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt1NumAddresses = 1) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt1PktDataSize = 1) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt1Rate = 1) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging 
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 1
; Clear the local data
for i = 1 to 30 do
  dwelldata[i,1] = 0
  dwelldata[i,2] = 0
  dwelldata[i,3] = 0
  dwelldata[i,4] = 0
  dwelldata[i,5] = 0
  dwelldata[i,6] = 0
  dataScnt[i] = 0
enddo

; Collect data 30 times
for i = 1 to 30 do
  ; Wait up to about 5 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt1_appid & "scnt"}

  while ({"p" & dwell_pkt1_appid & "scnt"} = oldval) and (cnt < 20) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ;; Store the sequence count
  dataScnt[i] = {"p" & dwell_pkt1_appid & "scnt"}

  ; Collect the data from the packet
  dwelldata[i,1] = $SC_$CPU_MD_DwlPkt1DwellData[1]
  dwelldata[i,2] = $SC_$CPU_MD_DwlPkt1DwellData[2]
  dwelldata[i,3] = $SC_$CPU_MD_DwlPkt1DwellData[3]
  dwelldata[i,4] = $SC_$CPU_MD_DwlPkt1DwellData[4]
  dwelldata[i,5] = $SC_$CPU_MD_DwlPkt1DwellData[5]
  dwelldata[i,6] = $SC_$CPU_MD_DwlPkt1DwellData[6]
enddo

%liv (log_procedure) = logging

; Check the data - since the delay for this dwell table is 1 and the values
; are incrementing once per second, each collected data point should be 1 more
; than the previous one. The data in bytes 2 through 6 should remain 0
; throughout as we only have one entry of 1 bytes length in the dwell table.
passed = 1
write "1,1: ", dwelldata[1,1], "; 1,2: ", dwelldata[1,2], "; 1,3: ", dwelldata[1,3], "; 1,4: ", dwelldata[1,4], "; 1,5: ", dwelldata[1,5], "; 1,6: ", dwelldata[1,6]
if (dwelldata[1,2] <> 0) or (dwelldata[1,3] <> 0) or (dwelldata[1,4] <> 0) or (dwelldata[1,5] <> 0) or (dwelldata[1,6] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 30 do
  write i, ",1: ", dwelldata[i,1], "; ", i, ",2: ", dwelldata[i,2], "; ", i, ",3: ", dwelldata[i,3], "; ", i, ",4: ", dwelldata[i,4], "; ", i, ",5: ", dwelldata[i,5], "; ", i, ",6: ", dwelldata[i,6]
  j = 1
  seqDiff = dataScnt[i] - dataScnt[i-1]

  if (dwelldata[i-1,j] = 255) then
    if (dwelldata[i,j] <> seqDiff) then
      write "<!> FAILED at (", i, ",", j, ")!!"
      write "Previous value was: ", dwelldata[i-1,j]
      write "This value was: ", dwelldata[i,j]
      passed = 0
    endif
  else
;;    if (dwelldata[i,j] <> (dwelldata[i-1,j] + 1)) THEN
    if (dwelldata[i,j] <> (dwelldata[i-1,j] + seqDiff)) THEN
      write "<!> FAILED at (", i, ",", j, ")!!"
      write "Previous value was: ", dwelldata[i-1,j]
      write "This value was: ", dwelldata[i,j]
      passed = 0
    endif
  endif

  if (dwelldata[i,2] <> 0) or (dwelldata[i,3] <> 0) or (dwelldata[i,4] <> 0) or (dwelldata[i,5] <> 0) or (dwelldata[i,6] <> 0) then
    write "<!> FAILED at entry #", i, " - end of data not 0!"
    passed = 0
  endif
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Data was as expected and received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Data was NOT as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.2:  Table Half Filled With Entries  "
write ";********************************************************************"
write ";  Step 2.2.1:  Load a dwell table that contains only one entry"
write ";  followed by a terminator entry with 0 for the number of bytes"
write ";********************************************************************"
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
;   12      2       4      testdata_addr+28
;   13      0       0      0
;
;  Entries = 12;  Size = 30;  Total Delay = 4

; Set up dwell load table 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Signature="Table with signature"

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

; Entry #12 - with a delay of 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Delay = 4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_Offset = testdata_addr+28
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[12].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13].MD_TLE_SymName = ""

; Create a load file for dwell table #2 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl2_load_appid,"Dwell Table #2 Load", "md_dwl_ld_sg_tbl2",MDTblName2, "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[13]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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
    write "<!> Failed - Inactive Dwell Table #2 validation failed. Event Message not received."
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

if (ut_sc_status = UT_SC_Success) then
	write "<*> Passed (1004) - Activate command sent properly."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Activate command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002) - Dwell Table #2 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
else
  write "<!> Failed (3002) - Dwell Table #2 activation. Event Message not received!"
  ut_setrequirements MD_3002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
	write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";********************************************************************"
write ";  Step 2.2.2:  Send Start Dwell command, enabling the dwell table"
write ";********************************************************************"
write "Opening Dwell Packet 2 Page"
page $sc_$cpu_md_dwell_pkt2

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
  write "<*> Passed (2000) - Second Dwell Table was started, First Dwell Table still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";********************************************************************"
write ";  Step 2.2.3:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain the correct number of entries"
write ";********************************************************************"
;; Turn off logging for the includes
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt2_appid & "scnt"}
while ({"p" & dwell_pkt2_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt2TableId = 2) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 2"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 2!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt2NumAddresses = 12) THEN
  write "<*> Passed (3001) - Dwell packet for table 2 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 2 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt2PktDataSize = 30) THEN
  write "<*> Passed (3001) - Dwell packet for table 2 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 2 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt2Rate = 4) THEN
  write "<*> Passed (3001) - Dwell packet for table 2 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 2 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

ut_tlmwait  $SC_$CPU_MD_DwlPkt2TableId, 2

; Verify table id of the packet
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 2"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 2!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging for the includes
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Clear the local data
for i = 1 to 15 do
  for j = 1 to 34 do
    dwelldata[i,j] = 0
  enddo
  dataScnt[i] = 0
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt2_appid & "scnt"}
  while ({"p" & dwell_pkt2_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  dataScnt[i] = {"p" & dwell_pkt2_appid & "scnt"}
  
  ; Collect the data from the packet
  for j = 1 to 34 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt2DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 34 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 4 and the values
; are incrementing once per second, each collected data point should be 4 more
; than the previous one. The data in bytes 31 through 34 should remain 0
; throughout.
passed = 1
if (dwelldata[1,31] <> 0) or (dwelldata[1,32] <> 0) or (dwelldata[1,33] <> 0) or (dwelldata[1,34] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 30 do
    if (dwelldata[i-1,j] >= 252) then
      if (dwelldata[i,j] <> dwelldata[i-1,j] - 252) then
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] <> (dwelldata[i-1,j] + 4)) THEN
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  if (dwelldata[i,31] <> 0) or (dwelldata[i,32] <> 0) or (dwelldata[i,33] <> 0) or (dwelldata[i,34] <> 0) then
    write "<!> FAILED at entry #", i, " - end of data not 0!"
    passed = 0
  endif
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Data was as expected and received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Data was NOT as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

; Check the sequence of the data. In this case every entry should be in
; sequence.

passed = 1
for i = 1 to 15 do
  for j = 2 to 30 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Data was sequenced as expected."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Data was NOT sequenced as expected!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3:  Table With Maximum Number of Entries  "
write ";********************************************************************"
write ";  Step 2.3.1:  Load a dwell table that contains the maximum number"
write ";  of entries (and no terminator entry)"
write ";********************************************************************"
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
;   25      2       0      testdata_addr+254
;
;  Entries = 25;  Size = 58;  Total Delay = 5


; Delays in the data collection (from 0 base)
;   bytes 0 - 8:    +0
;   bytes 9 - 28:   +1    (1 second delay after 8th byte)
;   bytes 29 - 42:  +3    (2 second delay after 28th byte)
;   bytes 43 - 44:  +4    (1 second delay after 42nd byte)
;   bytes 45 - 58:  +5    (1 second delay after 44th byte)

;;; Set up dwell load table 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Signature="Table with signature"

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
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25].MD_TLE_Delay = 0
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
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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

if (ut_sc_status = UT_SC_Success) then
	write "<*> Passed (1004) - Activate command sent properly."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Activate command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002) - Dwell Table #4 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
else
  write "<!> Failed (3002) - Dwell Table #4 activation. Event Message not received!"
  ut_setrequirements MD_3002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
	write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";********************************************************************"
write ";  Step 2.3.2:  Send Start Dwell command, enabling the dwell table"
write ";********************************************************************"
write "Opening Dwell Packet 4 Page"
page $sc_$cpu_md_dwell_pkt4

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
if ($SC_$CPU_MD_EnableMask = b'1011') then
  write "<*> Passed (2000) - Last Dwell Table was started, First & Second Dwell Tables still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";********************************************************************"
write ";  Step 2.3.3:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain the maximum number of entries"
write ";********************************************************************"
;; Turn off logging for the includes
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt4_appid & "scnt"}
while ({"p" & dwell_pkt4_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt4TableId = 4) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 4"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 4!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt4NumAddresses = 25) THEN
  write "<*> Passed (3001) - Dwell packet for table 4 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 4 does NOT have correct number of addresses! Expected 25. Table has ",$SC_$CPU_MD_DwlPkt4NumAddresses
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt4PktDataSize = 58) THEN
  write "<*> Passed (3001) - Dwell packet for table 4 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 4 does NOT have correct data size! Expected 58. Table has ",$sc_$cpu_MD_DwlPkt4PktDataSize 
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt4Rate = 5) THEN
  write "<*> Passed (3001) - Dwell packet for table 4 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 4 does NOT have correct data rate! Expected 5. Table has ", $sc_$cpu_MD_DwlPkt4Rate 
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging for the includes
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Clear the local data
for i = 1 to 15 do
  for j = 1 to 62 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
;  ut_tlmupdate {"p" & dwell_pkt4_appid & "scnt"}, 10

  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt4_appid & "scnt"}
  while ({"p" & dwell_pkt4_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 62 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt4DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 62 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 5 and the values
; are incrementing once per second, each collected data point should be 5 more
; than the previous one. The data in bytes 59 through 62 should remain 0
; throughout.

passed = 1
if (dwelldata[1,59] <> 0) or (dwelldata[1,60] <> 0) or (dwelldata[1,61] <> 0) or (dwelldata[1,62] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 58 do
    if (dwelldata[i-1,j] >= 251) then
      if (dwelldata[i,j] <> dwelldata[i-1,j] - 251) then
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] <> (dwelldata[i-1,j] + 5)) THEN
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  if (dwelldata[i,59] <> 0) or (dwelldata[i,60] <> 0) or (dwelldata[i,61] <> 0) or (dwelldata[i,62] <> 0) then
    write "<!> FAILED at entry #", i, " - end of data not 0!"
    passed = 0
  endif
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Data was as expected and received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Data was NOT as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

; Check the sequence of the data. This is dependent on position and length of
; delays as well as changes in the address (offset) of the entries.

passed = 1
for i = 1 to 15 do
  for j = 2 to 8 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo

  if (dwelldata[i,9] <= 1) then
    if (dwelldata[i,9] - dwelldata[i,8] <> -254) then
      write "<!> FAILED at (", i, ",9): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,8]
      write "This byte was: ", dwelldata[i,9]
      passed = 0
    endif
  else
    if (dwelldata[i,9] - dwelldata[i,8] <> 2) THEN
      write "<!> FAILED at (", i, ",9): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,8]
      write "This byte was: ", dwelldata[i,9]
      passed = 0
    endif
  endif
  
  for j = 10 to 12 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
    
  if (dwelldata[i,13] <= 108) then
    if (dwelldata[i,13] - dwelldata[i,12] <> -147) then
      write "<!> FAILED at (", i, ",13)!!"
      write "Previous byte was: ", dwelldata[i,12]
      write "This byte was: ", dwelldata[i,13]
      passed = 0
    endif
  else
    if (dwelldata[i,13] - dwelldata[i,12] <> 109) then
      write "<!> FAILED at (", i, ",13)!!"
      write "Previous byte was: ", dwelldata[i,12]
      write "This byte was: ", dwelldata[i,13]
      passed = 0
    endif
  endif
    
  for j = 14 to 28 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  
  if (dwelldata[i,29] <= 2) then
    if (dwelldata[i,29] - dwelldata[i,28] <> -253) then
      write "<!> FAILED at (", i, ",29): it should have been delayed 2 (3 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,28]
      write "This byte was: ", dwelldata[i,29]
      passed = 0
    endif
  else
    if (dwelldata[i,29] - dwelldata[i,28] <> 3) THEN
      write "<!> FAILED at (", i, ",29): it should have been delayed 2 (3 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,28]
      write "This byte was: ", dwelldata[i,29]
      passed = 0
    endif
  endif

  for j = 30 to 32 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
    
  if (dwelldata[i,32] <= 218) then
    if (dwelldata[i,32] - dwelldata[i,33] <> -37) then
      write "<!> FAILED at (", i, ",33)!!"
      write "Previous byte was: ", dwelldata[i,32]
      write "This byte was: ", dwelldata[i,33]
      passed = 0
    endif
  else
    if (dwelldata[i,32] - dwelldata[i,33] <> 219) then
      write "<!> FAILED at (", i, ",33)!!"
      write "Previous byte was: ", dwelldata[i,32]
      write "This byte was: ", dwelldata[i,33]
      passed = 0
    endif
  endif

  for j = 34 to 42 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo

  if (dwelldata[i,43] <= 1) then
    if (dwelldata[i,43] - dwelldata[i,42] <> -254) then
      write "<!> FAILED at (", i, ",43): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,42]
      write "This byte was: ", dwelldata[i,43]
      passed = 0
    endif
  else
    if (dwelldata[i,43] - dwelldata[i,42] <> 2) THEN
      write "<!> FAILED at (", i, ",43): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,42]
      write "This byte was: ", dwelldata[i,43]
      passed = 0
    endif
  endif

  for j = 44 to 44 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo

  if (dwelldata[i,45] <= 1) then
    if (dwelldata[i,45] - dwelldata[i,44] <> -254) then
      write "<!> FAILED at (", i, ",45): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,44]
      write "This byte was: ", dwelldata[i,45]
      passed = 0
    endif
  else
    if (dwelldata[i,45] - dwelldata[i,44] <> 2) THEN
      write "<!> FAILED at (", i, ",45): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,44]
      write "This byte was: ", dwelldata[i,45]
      passed = 0
    endif
  endif

  for j = 46 to 50 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
    
  if (dwelldata[i,51] <= 230) then
    if (dwelldata[i,51] - dwelldata[i,50] <> -25) then
      write "<!> FAILED at (", i, ",51)!!"
      write "Previous byte was: ", dwelldata[i,50]
      write "This byte was: ", dwelldata[i,51]
      passed = 0
    endif
  else
    if (dwelldata[i,51] - dwelldata[i,50] <> 231) then
      write "<!> FAILED at (", i, ",51)!!"
      write "Previous byte was: ", dwelldata[i,50]
      write "This byte was: ", dwelldata[i,51]
      passed = 0
    endif
  endif

  for j = 52 to 58 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Data was sequenced as expected."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Data was NOT sequenced as expected!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4:  Table With Total Delay of 0  "
write ";********************************************************************"
write ";  Step 2.4.1:  Load a dwell table that contains several entries"
write ";  and a total delay of 0"
write ";********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       4       0      testdata_addr+0
;   2       4       0      testdata_addr+4
;   3       2       0      testdata_addr+8
;   4       0       0      0
;
;  Entries = 3;  Size = 10;  Total Delay = 0

;;; Set up dwell load table 3
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Entry #2
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+4
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Entry #3
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Offset = testdata_addr+8
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[4].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[4].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[4].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[4].MD_TLE_SymName = ""

; Create a load file for dwell table #3 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl3_load_appid,"Dwell Table #3 Load", "md_dwl_ld_sg_tbl3",MDTblName3, "$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[4]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl3", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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
  goto step2_5
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1

  wait 5

  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #3 validation. Event Message not received."
    ut_setrequirements MD_3002_2, "F"
    goto step2_5
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

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1004) - Activate command sent properly."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Activate command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  goto step2_5
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002) - Dwell Table #3 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
else
  write "<!> Failed (3002) - Dwell Table #3 activation. Event Message not received!"
  ut_setrequirements MD_3002, "F"
  goto step2_5
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";********************************************************************"
write ";  Step 2.4.2:  Send Start Dwell command, enabling the dwell table"
write ";********************************************************************"
write "Opening Dwell Packet 3 Page"
page $sc_$cpu_md_dwell_pkt3

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
if ($SC_$CPU_MD_EnableMask = b'1111') then
  write "<*> Passed (2000) - Third Dwell Table was started. First, Second and Fourth still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";********************************************************************"
write ";  Step 2.4.3:  Verify that no memory dwell packets are received"
write ";  because the total delay is 0"
write ";********************************************************************"
; Wait up to 30 seconds for dwell packet sequence count to change
ut_tlmupdate {"p" & dwell_pkt3_appid & "scnt"}, 30

if (UT_TU_Status = UT_TU_TlmNotUpdating) then
  write "<*> Passed (3000.3) - No memory dwell packets were sent."
  ut_setrequirements MD_3000_3, "P"
else
  write "<!> Failed (3000.3) - Memory dwell packets were sent and they should not have been!"
  ut_setrequirements MD_3000_3, "F"
endif

wait 5

step2_5:
write ";*********************************************************************"
write ";  Step 2.5:  Table With No Entries  "
write ";********************************************************************"
write ";  Step 2.5.1:  Send Stop Dwell comamnd, disabling ALL dwell tables"
write ";********************************************************************"
; Send stop dwell command, disabling ALL dwell tables
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'1111'"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;2001) - Stop Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (1004;2001) - Stop Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_2001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_STOP_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0000') then
  write "<*> Passed (2001) - All Dwell Tables stopped."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

write ";********************************************************************"
write ";  Step 2.5.2:  Send Jam Dwell command on a table, creating a table"
write ";  with no entries, only a terminator entry"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_NULL_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=1 FieldLength=0 DwellDelay=0 Offset=0 SymName="""""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004;4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (1004;2001) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
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

; Table summary
;   Entry#  Length  Delay  Offset
;   1       0       0      0
;
;  Entries = 0;  Size = 0;  Total Delay = 0

wait 5

write ";********************************************************************"
write ";  Step 2.5.3:  Send Start Dwell command, enabling the dwell table"
write ";********************************************************************"
write "Opening Dwell Packet 1 Page"
page $sc_$cpu_md_dwell_pkt1

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

wait 5

write ";********************************************************************"
write ";  Step 2.5.4:  Verify that no memory dwell packets are received"
write ";********************************************************************"
; Wait up to 30 seconds for dwell packet sequence count to change
ut_tlmupdate {"p" & dwell_pkt1_appid & "scnt"}, 30

if (UT_TU_Status = UT_TU_TlmNotUpdating) then
  write "<*> Passed - No memory dwell packets were sent."
  ut_setrequirements MD_3000_3, "P"
else
  write "<!> Failed - Memory dwell packets were sent and they should not have been!"
  ut_setrequirements MD_3000_3, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6:  Table With Supercommutation  "
write ";********************************************************************"
write ";  Step 2.6.1:  Load a dwell table that contains several entries"
write ";  including multiple entries for the same address with different"
write ";  delays (supercommutation)"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+0
;   2       2       3      testdata_addr+0
;   3       2       0      testdata_addr+2
;   4       2       1      testdata_addr+0
;   5       2       1      testdata_addr+0
;   6       0       0      0
;
;  Entries = 5; Size = 10;  Total Delay = 6

;;; Set up dwell load table 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Entry #2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Delay = 3
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Entry #3
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = testdata_addr+2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Entry #4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_Offset = testdata_addr+0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[4].MD_TLE_SymName = ""

; Entry #5
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_Offset = testdata_addr+0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[5].MD_TLE_SymName = ""

; Entry #6 - Terminator Entry
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[6].MD_TLE_SymName = ""

; Create a load file for dwell table #4 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl4_load_appid,"Dwell Table #4 Load", "md_dwl_ld_sg_tbl4",MDTblName4, "$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[25]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl4", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1004) - Activate command sent properly."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Activate command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002) - Dwell Table #4 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
else
  write "<!> Failed (3002) - Dwell Table #4 activation. Event Message not received!"
  ut_setrequirements MD_3002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
	write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";********************************************************************"
write ";  Step 2.6.2:  Send Start Dwell command, enabling the dwell table"
write ";********************************************************************"
write "Opening Dwell Packet 4 Page"
page $sc_$cpu_md_dwell_pkt4

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
if ($SC_$CPU_MD_EnableMask = b'1001') then
  write "<*> Passed (2000) - Last Dwell Table was started, First Table still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

wait 5

write ";********************************************************************"
write ";  Step 2.6.3:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain the maximum number of entries"
write ";********************************************************************"
;; Turn off logging for the includes
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt1_appid & "scnt"}
while ({"p" & dwell_pkt1_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt4TableId = 4) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 4"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 4!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt4NumAddresses = 5) THEN
  write "<*> Passed (3001) - Dwell packet for table 4 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 4 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt4PktDataSize = 10) THEN
  write "<*> Passed (3001) - Dwell packet for table 4 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 4 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt4Rate = 6) THEN
  write "<*> Passed (3001) - Dwell packet for table 4 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 4 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging for the includes
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Clear the local data
for i = 1 to 15 do
  for j = 1 to 14 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 12 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt4_appid & "scnt"}
  while ({"p" & dwell_pkt4_appid & "scnt"} = oldval) and (cnt < 48) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 14 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt4DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 14 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 6 and the values
; are incrementing once per second, each collected data point should be 5 more
; than the previous one. The data in bytes 11 through 14 should remain 0
; throughout.

passed = 1
if (dwelldata[1,11] <> 0) or (dwelldata[1,12] <> 0) or (dwelldata[1,13] <> 0) or (dwelldata[1,14] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 10 do
    if (dwelldata[i-1,j] >= 250) then
      if (dwelldata[i,j] <> dwelldata[i-1,j] - 250) then
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] <> (dwelldata[i-1,j] + 6)) THEN
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  if (dwelldata[i,11] <> 0) or (dwelldata[i,12] <> 0) or (dwelldata[i,13] <> 0) or (dwelldata[i,14] <> 0) then
    write "<!> FAILED at entry #", i, " - end of data not 0!"
    passed = 0
  endif
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Values incremented by 6 as expected and were received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Values didn't increment as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

; Check the sequence of the data. This is dependent on position and length of
; delays as well as changes in the address (offset) of the entries. Also we
; have supercommutation in this case.

passed = 1
for i = 1 to 15 do
  for j = 2 to 2 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo

  if (dwelldata[i,3] = 0) then
    if (dwelldata[i,1] <> 255) then
      write "<!> FAILED at (", i, ",3): Supercommutation - it should have been 1 more then byte 1!!"
      write "Byte 1 was: ", dwelldata[i,1]
      write "This byte was: ", dwelldata[i,3]
      passed = 0
    endif
  else
    if (dwelldata[i,3] - dwelldata[i,1] <> 1) THEN
      write "<!> FAILED at (", i, ",3): Supercommutation - it should have been 1 more then byte 1!!"
      write "Byte 1 was: ", dwelldata[i,1]
      write "This byte was: ", dwelldata[i,3]
      passed = 0
    endif
  endif
  
  for j = 4 to 4 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): Supercommutation - it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): Supercommutation - it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
    
  if (dwelldata[i,5] <= 3) then
    if (dwelldata[i,5] - dwelldata[i,4] <> -252) then
      write "<!> FAILED at (", i, ",4): it should have been 4 more then previous byte!!"
      write "Previous byte was: ", dwelldata[i,4]
      write "This byte was: ", dwelldata[i,5]
      passed = 0
    endif
  else
    if (dwelldata[i,5] - dwelldata[i,4] <> 4) THEN
      write "<!> FAILED at (", i, ",4): it should have been 4 more then previous byte!!"
      write "Previous byte was: ", dwelldata[i,4]
      write "This byte was: ", dwelldata[i,5]
      passed = 0
    endif
  endif
    
  for j = 6 to 6 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  
  if (dwelldata[i,7] <= 3) then
    if (dwelldata[i,7] - dwelldata[i,1] <> -252) then
      write "<!> FAILED at (", i, ",7): Supercommutation - it should have been 4 more then byte 1!!"
      write "Byte 1 was: ", dwelldata[i,1]
      write "This byte was: ", dwelldata[i,7]
      passed = 0
    endif
  else
    if (dwelldata[i,7] - dwelldata[i,1] <> 4) THEN
      write "<!> FAILED at (", i, ",7): Supercommutation - it should have been 4 more then byte 1!!"
      write "Byte 1 was: ", dwelldata[i,1]
      write "This byte was: ", dwelldata[i,7]
      passed = 0
    endif
  endif
  
  for j = 8 to 8 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): Supercommutation - it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): Supercommutation - it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  
  if (dwelldata[i,9] <= 4) then
    if (dwelldata[i,9] - dwelldata[i,1] <> -251) then
      write "<!> FAILED at (", i, ",9): Supercommutation - it should have been 5 more then byte 1!!"
      write "Byte 1 was: ", dwelldata[i,1]
      write "This byte was: ", dwelldata[i,9]
      passed = 0
    endif
  else
    if (dwelldata[i,9] - dwelldata[i,1] <> 5) THEN
      write "<!> FAILED at (", i, ",9): Supercommutation - it should have been 5 more then byte 1!!"
      write "Byte 1 was: ", dwelldata[i,1]
      write "This byte was: ", dwelldata[i,9]
      passed = 0
    endif
  endif
  
  for j = 10 to 10 do
    if (dwelldata[i,j] = 0) then
      if (dwelldata[i,j-1] <> 255) then
        write "<!> FAILED at (", i, ",", j, "): Supercommutation - it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] - dwelldata[i,j-1] <> 1) THEN
        write "<!> FAILED at (", i, ",", j, "): Supercommutation - it should have been in sequence!!"
        write "Previous byte was: ", dwelldata[i,j-1]
        write "This byte was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
enddo

if (passed = 1) then
  write "<*> Passed (3000;3000.2;3001) - Data was sequenced as expected."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_2, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.2;3001) - Data was NOT sequenced as expected!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_2, "F"
  ut_setrequirements MD_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Memory Dwell Processing - Invalid Tables  "
write ";*********************************************************************"
write ";  Step 3.1:  Table Containing a Symbol Not in Symbol Table (or"
write ";  Symbol Table not preset"
write ";********************************************************************"
write ";  Step 3.1.1:  Load a dwell table with one entry, that specifies an"
write ";  invalid symbol name"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset   Symbol
;   1       2       1      50       Invalid
;   2       0       0      0
;
;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 50
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = "Invalid"

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RESOLVE_ERR_EID, "ERROR", 3
                                                                                
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

; Look for the Validation failed event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002;3002.2) - Inactive Dwell Table 1 failed validation as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Validation failure Event Message not received!"
  ut_setrequirements MD_3002, "F"
  ut_setrequirements MD_3002_2, "F"
endif
            
; Look for MD message indicating failure due to unresolved symbol
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3002.2) - Validation failed due to unresolved symbol. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Did not receive unresolved symbol Event Message!"
  ut_setrequirements MD_3002_2, "F"
endif
                                                                             
wait 5

write ";*********************************************************************"
write ";  Step 3.2:  Table Containing an invalid address"
write ";********************************************************************"
write ";  Step 3.2.1:  Load a dwell table that contains an entry specifying"
write ";  an invalid address"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      x'800000F'
;   2       0       0      0
;
;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = x'800000F'
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
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
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RANGE_ERR_EID, "ERROR", 3
                                                                                
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

; Look for the Validation failed event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002;3002.2) - Inactive Dwell Table 1 failed validation as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Validation failure Event Message not received!"
  ut_setrequirements MD_3002, "F"
  ut_setrequirements MD_3002_2, "F"
endif
            
; Look for MD message indicating failure due to invalid address
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3002.2) - Validation failed due to invalid address. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Did not receive invalid address Event Message!"
  ut_setrequirements MD_3002_2, "F"
endif
                                                                                
wait 5

write ";*********************************************************************"
write ";  Step 3.3:  Table contains entry with # of bytes 2, but address is"
write ";  not word aligned"
write ";********************************************************************"
write ";  Step 3.3.1: Load a dwell table that contains an entry with # of "
write ";  bytes 2, but address is not word aligned"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      1
;   2       0       0      0
;
;  Size = 2;  Total Delay = 1

;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.3.2: Validate the inactive buffer for Dwell Table #1. "
write ";  The result depends upon whether alignment is enforced or not."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID,"ERROR", 2
ut_setupevents "$SC","$CPU",{MDAppName},MD_TBL_ALIGN_ERR_EID,"ERROR", 3

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName1"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation command failed."
  goto step3_4
endif

; Look for the Validation failed event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002;3002.2) - Inactive Dwell Table 1 failed validation as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Validation failure Event Message not received as expected!"
  ut_setrequirements MD_3002, "F"
  ut_setrequirements MD_3002_2, "F"
endif

; Look for MD message indicating failure due to unaligned address
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3002.2) - Validation failed due to address unaligned. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Did not receive address unaligned Event Message!"
  ut_setrequirements MD_3002_2, "F"
endif

wait 5

step3_4:
write ";*********************************************************************"
write ";  Step 3.4: Table contains entry with # of bytes 4, but address is"
write ";  not long-word aligned"
write ";********************************************************************"
write ";  Step 3.4.1: Load a dwell table that contains an entry with # of"
write ";  bytes 4, but address is not long-word aligned"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset
;   1       4       1      2
;   2       0       0      0
;
;  Size = 4;  Total Delay = 1

;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.4.2: Validate the inactive buffer for Dwell Table #1. "
write ";  The result depends upon whether alignment is enforced or not."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID,"ERROR", 2
ut_setupevents "$SC","$CPU",{MDAppName},MD_TBL_ALIGN_ERR_EID,"ERROR", 3
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 4
                                                                                
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
  goto step3_5
endif

if (MD_ENFORCE_DWORD_ALIGN = 1) then
  ;; Look for the Validation failed event
  if ($SC_$CPU_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3002;3002.2) - Inactive Dwell Table 1 failed validation as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
    ut_setrequirements MD_3002, "P"
    ut_setrequirements MD_3002_2, "P"
  else
    write "<!> Failed (3002;3002.2) - Validation failure Event Message not received!"
    ut_setrequirements MD_3002, "F"
    ut_setrequirements MD_3002_2, "F"
  endif
            
  ; Look for MD message indicating failure due to unaligned address
  if ($SC_$CPU_find_event[3].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Validation failed due to address unaligned. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
    ut_setrequirements MD_3002_2, "P"
  else
    write "<!> Failed (3002;3002.2) - Did not receive address unaligned Event Message!"
    ut_setrequirements MD_3002_2, "F"
  endif
else
  ; Look for the Validation success event
  if ($SC_$CPU_find_event[4].num_found_messages = 1) then
    write "<*> Passed (3002;3002.2) - Inactive Dwell Table 1 passed validation with an unaligned entry as expected. Event Msg ",$SC_$CPU_find_event[4].eventid," Found!"
    ut_setrequirements MD_3002, "P"
    ut_setrequirements MD_3002_2, "P"
  else
    write "<!> Failed (3002;3002.2) - Validation success Event Message not received as expected!"
    ut_setrequirements MD_3002, "F"
    ut_setrequirements MD_3002_2, "F"
  endif
endif

wait 5

step3_5:
write ";*********************************************************************"
write ";  Step 3.5: Table contains entry with # of bytes 3"
write ";********************************************************************"
write ";  Step 3.5.1: Load a dwell table that contains an entry with # of"
write ";  bytes 3"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset
;   1       3       1      0
;   2       0       0      0
;
;  Size = 3;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0

;; Set the table signature
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 3
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1,"$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Table load command sent."
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Table load command failed."
  ut_setrequirements MD_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004): Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004): Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.5.2: Validate the inactive buffer for Dwell Table #1."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL", CFE_TBL_VALIDATION_ERR_EID,"ERROR", 2
ut_setupevents "$SC","$CPU",{MDAppName}, MD_TBL_HAS_LEN_ERR_EID,"ERROR", 3
                                                                                
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

; Look for the Validation failed event
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002;3002.2) - Inactive Dwell Table 1 failed validation as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
  ut_setrequirements MD_3002_5, "P"
else
  write "<!> Failed (3002;3002.2) - Validation failure Event Message not received!"
  ut_setrequirements MD_3002, "F"
  ut_setrequirements MD_3002_5, "F"
endif
            
; Look for MD message indicating failure due to invalid length
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3002.2) - Validation failed due to invalid length. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_5, "P"
else
  write "<!> Failed (3002;3002.2) - Did not receive invalid length Event Message!"
  ut_setrequirements MD_3002_5, "F"
endif
                                                                                
wait 5

step_4:
write ";*********************************************************************"
write ";  Step 4.0: Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $SC_$CPU_md_DwellProc                                "
write ";*********************************************************************"
ENDPROC
