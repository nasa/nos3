PROC $sc_$cpu_md_symtab
;*******************************************************************************
;  Test Name:  MD_SymTab
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to verify the Memory Dwell (MD) Symbol Table
;   functionality of the Core Flight System (CFS). Symbol Table support is
;   optional and thus provided in a separate test. If the mission provides
;   Symbol Table support, this test can be used to verify its functionality.
;   
;
;  Requirements Tested
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
;    MD3000     During each memory dwell cycle, MD shall collect data specified
;               in each enabled memory dwell table which contains the
;               following:
;                       a) Table ID
;                       b) <Optional> signature
;                       c) For each desired sample up to <PLATFORM_DEFINED>
;                          entries:
;                               1) address
;                               2) number of bytes
;                               3) delay between samples
;    MD3000.1   <OPTIONAL> Symbol Name and offset can be used in lieu of an
;		absolute address.
;    MD3001     When MD collects all of the data specified in a memory dwell
;               table (as defined in MD3000.2), MD shall issue a memory dwell
;               message containing the following:
;                       a) Table ID
;                       b) <OPTIONAL> Signature
;                       c) Number of bytes sampled
;                       d) Data
;    MD3002     Upon receipt of a Table Load, MD shall verify the contents of
;               the table and if the table is invalid, reject the table.
;    MD3002.2   If any address fails validation, MD shall reject the table.
;               Validation includes:
;                       a) If a symbolic address is specified, Symbol Table is
;                          present and symbolic address is contained in the
;                          Symbol Table.
;                       b) resolved address (numerical value of symbolic address;                          + offset address) is within valid range
;                       c) if resolved address is specified for a 2-byte dwell,
;                          address is an even value,
;                       d) if resolved address is specified for a 4-byte dwell,
;                          address is an integral multiple of 4
;    MD3002.4   <OPTIONAL> Symbol Name and offset can be used in lieu of an
;		absolute address.
;    MD4000     Upon receipt of a Jam Dwell command, MD shall update the
;               command-specified memory dwell table with the command-specified
;               information:
;                       a) Dwell Table Index
;                       b) Address
;                       c) Number of bytes (0,1,2 or 4)
;                       d) Delay Between Samples
;    MD4000.2   If the command-specified address fails validation, MD shall
;               reject the command. Validation includes:
;                       a) If a symbolic address is specified, Symbol Table is
;                          present and symbolic address is contained in the
;                          Symbol Table,
;                       b) resolved address (numerical value of symbolic address;                          if present + offset address) is within valid range
;                       c) if resolved address is specified for a 2-byte dwell,
;                          address is an even value,
;                       d) if resolved address is specified for a 4-byte dwell,
;                          address is a multiple integral of 4
;    MD4000.4   <OPTIONAL> Symbol Name and offset can be used in lieu of an
;		absolute address.
;    MD8000     MD shall generate a housekeeping message containing the
;               following:
;                       a) Valid Command Counter
;                       b) Command Rejected Counter
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
;               (in support of a cFE Processor Reset or Application Reset):
;                       a) Enable/Disable Status for each Dwell
;                       b) <OPTIONAL> signature for each dwell
;                       c) Contents of each Dwell Table
;    MD9004     On a cFE Processor Reset or a MD Application Reset, MD shall
;               restore the information specified in MD9003.
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
;   12/07/09      W. Moleski	Added requirements to this prolog and also
;				turned logging off around code that did not
;				provide any significant benefit of logging
;   01/23/12      W. Moleski    Added variable names for the app, table names
;                               and ram disk
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
#define MD_1005     1
#define MD_2000     2
#define MD_3000     3
#define MD_3000_1   4
#define MD_3001     5
#define MD_3002     6
#define MD_3002_2   7
#define MD_3002_4   8
#define MD_4000     9
#define MD_4000_2   10
#define MD_4000_4   11
#define MD_8000     12
#define MD_9000     13
#define MD_9001     14
#define MD_9002     15
#define MD_9003     16
#define MD_9004     17

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 17
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1004", "MD_1005", "MD_2000", "MD_3000", "MD_3000_1", "MD_3001", "MD_3002", "MD_3002_2", "MD_3002_4", "MD_4000", "MD_4000_2", "MD_4000_4", "MD_8000", "MD_9000", "MD_9001", "MD_9002", "MD_9003", "MD_9004"]

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

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

clear the local dwell data array
for i = 1 to 30 do
  for j = 1 to 62 do
    dwelldata[i,j] = 0
  enddo
enddo

%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test.. "
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
write ";  Step 1.7: Setup initial dwell tables"
write ";*********************************************************************"
write "; Setup the initial dwell tables"
s $sc_$cpu_md_deftables
wait 5

write ";*********************************************************************"
write ";  Step 2.0:  Symbol Table Tests - Valid Cases"
write ";*********************************************************************"
write ";  Step 2.1:  Load Table With Valid Symbols & Offsets and Start It  "
write ";********************************************************************"
write ";  Step 2.1.1:  Load a dwell table into FIRST slot, that contains"
write ";  entries with valid symbols, some with offsets  "
write ";********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       4       0      TestData+0
;   2       2       0      TestData+4
;   3       2       0      TestData+6
;   4       2       0      TestData+8
;   5       2       0      TestData+10
;   6       4       0      TestData+12
;   7       2       0      TestData+16
;   8       2       0      TestData+18
;   9       2       0      TestData+20
;   10      2       0      TestData+22
;   11      4       0      TestData+24
;   12      2       4      TestData+28
;   13      0       0      0
;
;  Entries = 12;  Size = 30;  Total Delay = 4

; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = "TestData"

; Entry #2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = "TestData"

; Entry #3
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Offset = 6
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_SymName = "TestData"

; Entry #4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_Offset = 8
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[4].MD_TLE_SymName = "TestData"

; Entry #5
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[5].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[5].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[5].MD_TLE_Offset = 10
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[5].MD_TLE_SymName = "TestData"

; Entry #6
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[6].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[6].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[6].MD_TLE_Offset = 12
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[6].MD_TLE_SymName = "TestData"

; Entry #7
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[7].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[7].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[7].MD_TLE_Offset = 16
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[7].MD_TLE_SymName = "TestData"

; Entry #8
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[8].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[8].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[8].MD_TLE_Offset = 18
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[8].MD_TLE_SymName = "TestData"

; Entry #9
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[9].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[9].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[9].MD_TLE_Offset = 20
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[9].MD_TLE_SymName = "TestData"

; Entry #10
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[10].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[10].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[10].MD_TLE_Offset = 22
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[10].MD_TLE_SymName = "TestData"

; Entry #11
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[11].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[11].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[11].MD_TLE_Offset = 24
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[11].MD_TLE_SymName = "TestData"

; Entry #12 - with a delay of 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[12].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[12].MD_TLE_Delay = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[12].MD_TLE_Offset = 28
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[12].MD_TLE_SymName = "TestData"

; Terminator entry
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[13].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[13].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[13].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[13].MD_TLE_SymName = "TestData"

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load", "md_dwl_ld_sg_tbl1",MDTblName1, "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled", "$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[13]")

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
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
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
  write "<*> Passed (3002;3002.4) - Dwell Table #1 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_3002, "P"
  ut_setrequirements MD_3002_4, "P"
else
  write "<!> Failed (3002;3002.4) - Dwell Table #1 activation. Event Message not received!"
  ut_setrequirements MD_3002, "F"
  ut_setrequirements MD_3002_4, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
	write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
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
  write "<*> Passed (2000) - Second Dwell Table was started, First Dwell Table still running."
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
write ";  the proper rate and contain the correct number of entries"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
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
IF ($SC_$CPU_MD_DwlPkt1NumAddresses = 12) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt1PktDataSize = 30) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt1Rate = 4) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

ut_tlmwait  $SC_$CPU_MD_DwlPkt1TableId, 1

; Verify table id of the packet
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 1"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 1!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Clear the local data
for i = 1 to 15 do
  for j = 1 to 34 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt1_appid & "scnt"}
  while ({"p" & dwell_pkt1_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 34 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt1DwellData[j]
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
  write "<*> Passed (3000;3000.1;3001) - Data was as expected and received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_1, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.1;3001) - Data was NOT as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_1, "F"
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
  write "<*> Passed (3000;3000.1;3001) - Data was sequenced as expected."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_1, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.1;3001) - Data was NOT sequenced as expected!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_1, "F"
  ut_setrequirements MD_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2:  Jam Dwell With Valid Symbol, 0 Offset, and Start"
write ";  The Table  "
write ";********************************************************************"
write ";  Step 2.2.1:  Send Jam Dwell command, modifying an entry in the"
write ";  SECOND table using a valid symbol and a 0 offset"
write ";********************************************************************"
; Table summary (initial)
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+2
;   2       2       0      testdata_addr+4
;   3       0       0      0
;
;  Entries = 2;  Size = 4;  Total Delay = 1

; Table summary (after jam dwell)
;   Entry#  Length  Delay  Offset
;   1       4       4      TestData
;   2       2       0      testdata_addr+4
;   3       0       0      0
;
;  Entries = 2;  Size = 6;  Total Delay = 4

ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+104

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=1 FieldLength=4 DwellDelay=4 Offset=0 SymName=""TestData"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004,4000,4000.4) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_4000, "P"
  ut_setrequirements MD_4000_4, "P"
else
  write "<!> Failed (1004,4000,4000.4) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_4000, "F"
  ut_setrequirements MD_4000_4, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

wait 5

write ";********************************************************************"
write ";  Step 2.2.2:  Dump the table to verify that the entry has been"
write ";  updated"
write ";********************************************************************"
; Display the Dwell Table #2 page
page $SC_$CPU_MD_DWELL_LOAD_TBL2

; Dump the current active dwell load table 2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName2,"A","$cpu_symdump222","$CPU",dwell_tbl2_load_pkt)

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
if ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay = 4) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset = 0) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName = "TestData") then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 4 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 2 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of 0 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset
  write "- Expected SymName of 'TestData' - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";********************************************************************"
write ";  Step 2.2.3:  Send Start Dwell command, enabling SECOND dwell table. "
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
write ";  Step 2.2.4:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
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
IF ($SC_$CPU_MD_DwlPkt2NumAddresses = 2) THEN
  write "<*> Passed (3001) - Dwell packet for table 2 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 2 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt2PktDataSize = 6) THEN
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

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 2
; Clear the local data
for i = 1 to 15 do
  for j = 1 to 10 do
    dwelldata[i,j] = 0
  enddo
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
  
  ; Collect the data from the packet
  for j = 1 to 10 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt2DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 10 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 4 and the values
; are incrementing once per second, each collected data point should be 4 more
; than the previous one. The data in bytes 7 through 10 should remain 0
; throughout.
passed = 1
if (dwelldata[1,7] <> 0) or (dwelldata[1,8] <> 0) or (dwelldata[1,9] <> 0) or (dwelldata[1,10] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 6 do
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
  if (dwelldata[i,7] <> 0) or (dwelldata[i,8] <> 0) or (dwelldata[i,9] <> 0) or (dwelldata[i,10] <> 0) then
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
  for j = 2 to 4 do
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
    
  if (dwelldata[i,5] <= 4) then
    if (dwelldata[i,5] - dwelldata[i,4] <> -251) then
      write "<!> FAILED at (", i, ",5): it should have been delayed 4 (5 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,4]
      write "This byte was: ", dwelldata[i,5]
      passed = 0
    endif
  else
    if (dwelldata[i,5] - dwelldata[i,4] <> 5) THEN
      write "<!> FAILED at (", i, ",5): it should have been delayed 4 (5 more then previous byte)!!"
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
write ";  Step 2.3:  Jam Dwell Enabled Table With Valid Symbol, Non-Zero"
write ";  Offset"
write ";********************************************************************"
write ";  Step 2.3.1:  Send Start Dwell command, enabling THIRD dwell"
write ";  table"
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
if ($SC_$CPU_MD_EnableMask = b'0111') then
  write "<*> Passed (2000) - Third Dwell Table was started, First & Second Dwell Tables still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";********************************************************************"
write ";  Step 2.3.2:  Send Jam Dwell command, modifying an entry in the"
write ";  THIRD table using a valid symbol and a non-zero offset"
write ";********************************************************************"
; Table summary (initial)
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+6
;   2       4       0      testdata_addr+8
;   3       0       0      0
;
;  Entries = 2;  Size = 6;  Total Delay = 1

; Table summary (after jam dwell)
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+6
;   2       4       0      testdata_addr+8
;   3       2       1      TestData+12
;   4       0       0      0
;
;  Entries = 3;  Size = 8;  Total Delay = 2

ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

workaddress = testdata_addr+104

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=3 FieldLength=2 DwellDelay=1 Offset=12 SymName=""TestData"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1004,4000,4000.4) - Jam Dwell command sent properly."
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_4000, "P"
  ut_setrequirements MD_4000_4, "P"
else
  write "<!> Failed (1004,4000,4000.4) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_4000, "F"
  ut_setrequirements MD_4000_4, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_1004, "F"
endif

wait 5

write ";********************************************************************"
write ";  Step 2.3.3:  Dump the table to verify that the entry has been"
write ";  updated"
write ";********************************************************************"
; Display the Dwell Table #3 page
page $SC_$CPU_MD_DWELL_LOAD_TBL3

; Dump the current active dwell load table 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName3,"A","$cpu_symdump_233","$CPU",dwell_tbl3_load_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed: Dump command for Dwell Table 3."
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  if ($SC_$CPU_evs_eventid <> CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<!> Failed: Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  endif
endif

;; Check the table entry to see if it corresponds to what was jammed
if ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Length = 2) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Delay = 1) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Offset = 12) AND ;;
   ($SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_SymName = "TestData") then
  write "<*> Passed (4000) - The jammed entry has been found in the table."
  ut_setrequirements MD_4000, "P"
else
  write "<!> Failed (4000) - The table did not contain the jammed entry."
  write "- Expected Length of 2 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length
  write "- Expected Delay of 1 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay
  write "- Expected Offset of 12 - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset
  write "- Expected SymName of 'TestData' - table = ",$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName
  ut_setrequirements MD_4000, "F"
endif

wait 5

write ";********************************************************************"
write ";  Step 2.3.4:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt3_appid & "scnt"}
while ({"p" & dwell_pkt3_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt3TableId = 3) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 3"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 3!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt3NumAddresses = 3) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt3PktDataSize = 8) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt3Rate = 2) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 2
; Clear the local data
for i = 1 to 15 do
  for j = 1 to 12 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt3_appid & "scnt"}
  while ({"p" & dwell_pkt3_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 12 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt3DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 12 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 2 and the values
; are incrementing once per second, each collected data point should be 2 more
; than the previous one. The data in bytes 9 through 12 should remain 0
; throughout.
passed = 1
if (dwelldata[1,9] <> 0) or (dwelldata[1,10] <> 0) or (dwelldata[1,11] <> 0) or (dwelldata[1,12] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 6 do
    if (dwelldata[i-1,j] >= 254) then
      if (dwelldata[i,j] <> dwelldata[i-1,j] - 254) then
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] <> (dwelldata[i-1,j] + 2)) THEN
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  if (dwelldata[i,9] <> 0) or (dwelldata[i,10] <> 0) or (dwelldata[i,11] <> 0) or (dwelldata[i,12] <> 0) then
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
    
  if (dwelldata[i,3] <= 1) then
    if (dwelldata[i,3] - dwelldata[i,2] <> -254) then
      write "<!> FAILED at (", i, ",3): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,4]
      write "This byte was: ", dwelldata[i,5]
      passed = 0
    endif
  else
    if (dwelldata[i,3] - dwelldata[i,2] <> 2) THEN
      write "<!> FAILED at (", i, ",3): it should have been delayed 1 (2 more then previous byte)!!"
      write "Previous byte was: ", dwelldata[i,4]
      write "This byte was: ", dwelldata[i,5]
      passed = 0
    endif
  endif
  
  for j = 4 to 8 do
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
write ";  Step 3.0:  Symbol Table Tests - Invalid Cases"
write ";*********************************************************************"
write ";  Step 3.1:  Load Table With Invalid Symbol  "
write ";********************************************************************"
write ";  Step 3.1.1:  Load a dwell table that contains an entry with an"
write ";  invalid symbol"
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
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 50
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = "MD Invalid Symbol"

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
  write "<*> Passed (3002.2) - Validation failed  due to unresolved symbol. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Did not receive unresolved symbol Event Message!"
  ut_setrequirements MD_3002_2, "F"
endif
                                                                             
wait 5

write ";*********************************************************************"
write ";  Step 3.2:  Load Table With Valid Symbol, Invalid Offset  "
write ";********************************************************************"
write ";  Step 3.2.1:  Load a dwell table that contains an entry with a"
write ";  valid symbol, but an invalid offset"
write ";********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset   Symbol
;   1       2       1      8000002  TestData
;   2       0       0      0
;
;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = x'8000002'
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = "TestData"

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
            
; Look for MD message indicating failure due to range error
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3002.2) - Validation failed due to range error. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_2, "P"
else
  write "<!> Failed (3002;3002.2) - Did not receive range error Event Message!"
  ut_setrequirements MD_3002_2, "F"
endif
                                                                             
wait 5

write ";*********************************************************************"
write ";  Step 3.3: Load Table With Valid Symbol & Offset With Entry"
write ";  Specifying 2 Bytes, But Not Word Aligned  "
write ";********************************************************************"
write ";  Step 3.3.1: Load a dwell table that contains an entry with a valid "
write ";  symbol and offset, specifying 2 bytes, but not word aligned"
write ";********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset   Symbol
;   1       2       1      1       TestData
;   2       0       0      0
;
;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = "TestData"

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
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{MDAppName},MD_TBL_ALIGN_ERR_EID, "ERROR", 3

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
  goto step3_4
endif

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
            
;; Look for MD message indicating failure due to alignment error
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3002.2) - Validation failed due to alignment error. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
  ut_setrequirements MD_3002_2, "P"
else 
  write "<!> Failed (3002;3002.2) - Did not receive alignment error Event Message!"
  ut_setrequirements MD_3002_2, "F"
endif

wait 5

step3_4:
write ";*********************************************************************"
write ";  Step 3.4: Load Table With Valid Symbol & Offset With Entry"
write ";  Specifying 4 Bytes, But Not Long-Word Aligned  "
write ";********************************************************************"
write ";  Step 3.4.1: Load a dwell table that contains an entry with a valid "
write ";  symbol and offset, specifying 4 bytes, but not long-word aligned"
write ";********************************************************************"
; Set up the table data

; Table summary
;   Entry#  Length  Delay  Offset   Symbol
;   1       4       1      2       TestData
;   2       0       0      0
;
;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table with signature"

; Entry #1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = 2
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = "TestData"

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
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{MDAppName},MD_TBL_ALIGN_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 4
                                                                                
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

  ; Look for MD message indicating failure due to alignment error
  if ($SC_$CPU_find_event[3].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Validation failed due to alignment error. Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
    ut_setrequirements MD_3002_2, "P"
  else
    write "<!> Failed (3002;3002.2) - Did not receive alignment error Event Message!"
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
write ";  Step 3.5: Jam Dwell with Invalid Symbol"
write ";********************************************************************"
write ";  Step 3.5.1:  Send Jam Dwell command for FIRST table, specifying"
write ";  invalid symbol"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=2 FieldLength=2 DwellDelay=0 Offset=0 SymName="MD Invalid Symbol"

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
write ";  Step 3.6:  Jam Dwell with Valid Symbol, Invalid Offset"
write ";********************************************************************"
write ";  Step 3.6.1:  Send Jam Dwell command for SECOND table, specifying"
write ";  valid symbol, but and invalid offset"
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INVALID_JAM_ADDR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=1 FieldLength=2 DwellDelay=0 Offset=x'8000004' SymName="TestData"

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
write ";  Step 3.7: Jam Dwell with Valid Symbol & Offset, Specifying 2"
write ";  Bytes, but not Word Aligned"
write ";********************************************************************"
write ";  Step 3.7.1: Send Jam Dwell command for THIRD table, specifying a"
write ";  valid symbol & offset and 2 bytes, but not word aligned"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_ADDR_NOT_16BIT_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=3 FieldLength=2 DwellDelay=0 Offset=3 SymName="TestData"

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
write ";  Step 3.8: Jam Dwell with Valid Symbol & Offset, Specifying 4"
write ";  Bytes, but not Long-Word Aligned"
write ";********************************************************************"
write ";  Step 3.8.1: Dump Load table 4 in order to restore the jammed value "
write ";  if alignment is not bening enforced."
write ";********************************************************************"
; Dump the current active dwell load table 2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

s get_tbl_to_cvt (ramDir,MDTblName4,"A","$cpu_symdump381","$CPU",dwell_tbl4_load_pkt)

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

;; Save table entry #1 values
local savedLength = $SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Length
local savedDelay = $SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Delay 
local savedOffset = $SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Offset
local savedSymName = $SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_SymName

write ";********************************************************************"
write ";  Step 3.8.2: Send Jam Dwell command for LAST table, specifying a"
write ";  valid symbol & offset and 4 bytes, but not long-word aligned"
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_ADDR_NOT_32BIT_ERR_EID,"ERROR", 1
ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_DWELL_INF_EID, "INFO", 2

cmdcnt = $SC_$CPU_MD_CMDPC + 1
errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=1 FieldLength=4 DwellDelay=0 Offset=6 SymName="TestData"

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

;; Only execute the next step if Alignment is not being ENFORCED
if (MD_ENFORCE_DWORD_ALIGN = 0) then
  write ";********************************************************************"
  write ";  Step 3.8.3: Send Jam Dwell command to restore the original entry"
  write ";********************************************************************"
  ut_setupevents "$SC","$CPU",{MDAppName},MD_JAM_DWELL_INF_EID, "INFO", 2

  cmdcnt = $SC_$CPU_MD_CMDPC + 1

  /$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=1 FieldLength=savedLength DwellDelay=savedDelay Offset=savedOffset SymName=savedSymName

  ut_tlmwait $SC_$CPU_MD_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1005) - Command Accepted Counter incremented."
    ut_setrequirements MD_1005, "P"
  else
    write "<!> Failed (1005) - Command Accepted Counter did not increment as expected."
    ut_setrequirements MD_1005, "F"
  endif
endif

write ";*********************************************************************"
write ";  Step 4.0:  Reset Tests"
write ";*********************************************************************"
write ";  Step 4.1:  cFE Processor Reset  "
write ";********************************************************************"
write ";  Step 4.1.1: Dump the Load Tables before performing a Processor Reset "
write ";  in order to verify the tables were restored."
write ";********************************************************************"
local tableName,dumpFileName
local loadPktId
local tbl_enaFlag[MD_NUM_DWELL_TABLES]
local tbl_signature[MD_NUM_DWELL_TABLES]
local tbl_length[MD_DWELL_TABLE_SIZE,MD_DWELL_TABLE_SIZE]
local tbl_delay[MD_DWELL_TABLE_SIZE,MD_DWELL_TABLE_SIZE]
local tbl_offset[MD_NUM_DWELL_TABLES,MD_DWELL_TABLE_SIZE]
local tbl_symName[MD_NUM_DWELL_TABLES,MD_DWELL_TABLE_SIZE]
local dataName

;; Display the pages (2 & 3 should already be displayed)
page $SC_$CPU_MD_DWELL_LOAD_TBL1
page $SC_$CPU_MD_DWELL_LOAD_TBL4

;; Dump each Dwell Table
for i = 1 to MD_NUM_DWELL_TABLES do
  ; Dump the dwell table
  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

  tableName = MDAppName & ".DWELL_TABLE" & i
  dumpFileName = "$cpu_tbl" & i & "symdmp411"
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

  ;; Store the table data
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Enabled"
  tbl_enaFlag[i] = {dataName}
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Signature"
  tbl_signature[i] = {dataName}

  ;; loop for each table entry
  for j = 1 to MD_DWELL_TABLE_SIZE do
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Length"
    tbl_length[i,j] = {dataName}
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Delay"
    tbl_delay[i,j] = {dataName}
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Offset"
    tbl_offset[i,j] = {dataName}
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_SymName"
    tbl_symName[i,j] = {dataName}
  enddo
enddo

write ";********************************************************************"
write ";  Step 4.1.2:  Send cFE Processor Reset command"
write ";********************************************************************"
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

write ";  Start the Memory Dwell (MD) Application and "
write ";  add any required subscriptions.  "

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RECOVERED_TBL_VALID_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_INIT_INF_EID, "INFO", 3

s load_start_app (MDAppName,"$CPU","MD_AppMain")

; Wait for table recovery events
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 4
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9004) - All MD Dwell Table recovery messages were received"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9004) - Not all of the MD Dwell Table recovery messages were received!"
  ut_setrequirements MD_9004, "F"
endif

; Wait for app startup events
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
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

; get the address of the test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

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

write ";*********************************************************************"
write ";  Step 4.1.3: Verify that each MD dwell load table was restored.  "
write ";*********************************************************************"
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  ; Dump the dwell table
  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

  tableName = MDAppName & ".DWELL_TABLE" & i
  dumpFileName = "$cpu_tbl" & i & "symdmp413"
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

  passed = 1
  ;; Compare the table Enabled Flag
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Enabled"
  if (tbl_enaFlag[i] <> {dataName}) then
    passed = 0
    write "<!> Failed - Enable Flag expected for Tbl #",i," = ",tbl_enaFlag[i],"; Tbl value = ",{dataName}
  endif 

  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Signature"
  if (tbl_signature[i] <> {dataName}) then
    passed = 0
    write "<!> Failed - Signature expected for Tbl #",i," = ",tbl_signature[i],"; Tbl value = ",{dataName}
  endif 

  ;; loop for each table entry
  for j = 1 to MD_DWELL_TABLE_SIZE do
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Length"
    if (tbl_length[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected length = ",tbl_length[i,j],"; Tbl value = ",{dataName}
    endif 

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Delay"
    if (tbl_delay[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected delay = ",tbl_delay[i,j],"; Tbl value = ",{dataName}
    endif 

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Offset"
    if (tbl_offset[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected offset = ",tbl_offset[i,j],"; Tbl value = ",{dataName}
    endif 

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_SymName"
    if (tbl_symName[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected symName = ",tbl_symName[i,j],"; Tbl value = ",{dataName}
    endif 
  enddo

  if (passed = 1) then
    write "<*> Passed - Load Tbl #",i," was restored properly after reset."
  else
    write "<!> Failed - Load Tbl #",i," was not preserved across the reset."
  endif
ENDDO

write ";*********************************************************************"
write ";  Step 4.1.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0111') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"

  ; Start the tables so we can finish the test
  /$SC_$CPU_MD_StartDwell TableMask=b'0111'
  wait 5
endif

write ";*********************************************************************"
write ";  Step 4.1.5: Verify that the HK entries were restored for each tbl."
write ";*********************************************************************"
; FIRST table should have address count 12, rate 4 and data size 30
if ($SC_$CPU_MD_AddrCnt[1] = 12) and ($SC_$CPU_MD_Rate[1] = 4) and ($SC_$CPU_MD_DataSize[1] = 30) THEN
  write "<*> Passed (9003,9004) - FIRST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - FIRST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 12)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 4)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 30)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; SECOND table should have address count 2, rate 4 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 4) and ($SC_$CPU_MD_DataSize[2] = 6) THEN
  write "<*> Passed (9003,9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 6)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; THIRD table should have address count 3, rate 2 and data size 8
if ($SC_$CPU_MD_AddrCnt[3] = 3) and ($SC_$CPU_MD_Rate[3] = 2) and ($SC_$CPU_MD_DataSize[3] = 8) THEN
  write "<*> Passed (9003,9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 3)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 8)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; LAST table should have address count 2, rate 1 and data size 2
if ($SC_$CPU_MD_AddrCnt[4] = 2) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 2) THEN
  write "<*> Passed (9003,9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 2)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.1.6: Verify that memory dwell messages are received at the"
write ";  proper rate and with the correct number of entries "
write ";*********************************************************************"
write ";  Table 1: Verify that memory dwell messages are received at"
write ";  the proper rate and contain the correct number of entries"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
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
IF ($SC_$CPU_MD_DwlPkt1NumAddresses = 12) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt1PktDataSize = 30) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt1Rate = 4) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Clear the local data
for i = 1 to 15 do
  for j = 1 to 34 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt1_appid & "scnt"}
  while ({"p" & dwell_pkt1_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 34 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt1DwellData[j]
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
  write "<*> Passed (3000;3000.1;3001) - Data was as expected and received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_1, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.1;3001) - Data was NOT as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_1, "F"
  ut_setrequirements MD_3001, "F"
endif

wait 5

write ";********************************************************************"
write ";  Table 2: Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
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
IF ($SC_$CPU_MD_DwlPkt2NumAddresses = 2) THEN
  write "<*> Passed (3001) - Dwell packet for table 2 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 2 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt2PktDataSize = 6) THEN
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

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 2
; Clear the local data
for i = 1 to 15 do
  for j = 1 to 10 do
    dwelldata[i,j] = 0
  enddo
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
  
  ; Collect the data from the packet
  for j = 1 to 10 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt2DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 10 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 4 and the values
; are incrementing once per second, each collected data point should be 4 more
; than the previous one. The data in bytes 7 through 10 should remain 0
; throughout.
passed = 1
if (dwelldata[1,7] <> 0) or (dwelldata[1,8] <> 0) or (dwelldata[1,9] <> 0) or (dwelldata[1,10] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 6 do
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
  if (dwelldata[i,7] <> 0) or (dwelldata[i,8] <> 0) or (dwelldata[i,9] <> 0) or (dwelldata[i,10] <> 0) then
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

wait 5

write ";********************************************************************"
write ";  Table 3:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt3_appid & "scnt"}
while ({"p" & dwell_pkt3_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt3TableId = 3) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 3"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 3!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt3NumAddresses = 3) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt3PktDataSize = 8) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt3Rate = 2) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 2
; Clear the local data
for i = 1 to 15 do
  for j = 1 to 12 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt3_appid & "scnt"}
  while ({"p" & dwell_pkt3_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 12 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt3DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 12 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 2 and the values
; are incrementing once per second, each collected data point should be 2 more
; than the previous one. The data in bytes 9 through 12 should remain 0
; throughout.
passed = 1
if (dwelldata[1,9] <> 0) or (dwelldata[1,10] <> 0) or (dwelldata[1,11] <> 0) or (dwelldata[1,12] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 6 do
    if (dwelldata[i-1,j] >= 254) then
      if (dwelldata[i,j] <> dwelldata[i-1,j] - 254) then
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] <> (dwelldata[i-1,j] + 2)) THEN
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  if (dwelldata[i,9] <> 0) or (dwelldata[i,10] <> 0) or (dwelldata[i,11] <> 0) or (dwelldata[i,12] <> 0) then
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

wait 5

write ";*********************************************************************"
write ";  Step 4.2:  MD Application Reset  "
write ";********************************************************************"
write ";  Step 4.2.1: Dump the Load Tables before performing an Application "
write ";  Reset in order to verify the tables were restored."
write ";********************************************************************"
;; Dump each Dwell Table
for i = 1 to MD_NUM_DWELL_TABLES do
  ; Dump the dwell table
  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

  tableName = MDAppName & ".DWELL_TABLE" & i
  dumpFileName = "$cpu_tbl" & i & "symdmp421"
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

  ;; Store the table data
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Enabled"
  tbl_enaFlag[i] = {dataName}
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Signature"
  tbl_signature[i] = {dataName}

  ;; loop for each table entry
  for j = 1 to MD_DWELL_TABLE_SIZE do
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Length"
    tbl_length[i,j] = {dataName}
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Delay"
    tbl_delay[i,j] = {dataName}
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Offset"
    tbl_offset[i,j] = {dataName}
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_SymName"
    tbl_symName[i,j] = {dataName}
  enddo
enddo

write ";********************************************************************"
write ";  Step 4.2.2:  Send MD Application Reset command"
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
write ";  Step 4.2.3: Verify that each MD dwell load table was restored.  "
write ";*********************************************************************"
FOR i = 1 to MD_NUM_DWELL_TABLES DO
  ; Dump the dwell table
  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

  tableName = MDAppName & ".DWELL_TABLE" & i
  dumpFileName = "$cpu_tbl" & i & "symdmp423"
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

  passed = 1
  ;; Compare the table Enabled Flag
  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Enabled"
  if (tbl_enaFlag[i] <> {dataName}) then
    passed = 0
    write "<!> Failed - Enable Flag expected for Tbl #",i," = ",tbl_enaFlag[i],"; Tbl value = ",{dataName}
  endif 

  dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Signature"
  if (tbl_signature[i] <> {dataName}) then
    passed = 0
    write "<!> Failed - Signature expected for Tbl #",i," = ",tbl_signature[i],"; Tbl value = ",{dataName}
  endif 

  ;; loop for each table entry
  for j = 1 to MD_DWELL_TABLE_SIZE do
    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Length"
    if (tbl_length[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected length = ",tbl_length[i,j],"; Tbl value = ",{dataName}
    endif 

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Delay"
    if (tbl_delay[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected delay = ",tbl_delay[i,j],"; Tbl value = ",{dataName}
    endif 

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_Offset"
    if (tbl_offset[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected offset = ",tbl_offset[i,j],"; Tbl value = ",{dataName}
    endif 

    dataName = "$SC_$CPU_MD_Dwell_Load_Tbl" & i & ".MD_DTL_Entry[" & j & "].MD_TLE_SymName"
    if (tbl_symName[i,j] <> {dataName}) then
      passed = 0
      write "<!> Failed - entry[",i,",",j,"] expected symName = ",tbl_symName[i,j],"; Tbl value = ",{dataName}
    endif 
  enddo

  if (passed = 1) then
    write "<*> Passed - Load Tbl #",i," was restored properly after reset."
  else
    write "<!> Failed - Load Tbl #",i," was not preserved across the reset."
  endif
ENDDO

write ";*********************************************************************"
write ";  Step 4.2.4: Verify that enabled state was restored for each dwell "
write ";*********************************************************************"
; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0111') then
  write "<*> Passed (9003,9004) - Dwell Tables were restarted."
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"

  ; Start the tables so we can finish the test
  /$SC_$CPU_MD_StartDwell TableMask=b'0111'
  wait 5
endif

/$SC_$CPU_MD_StartDwell TableMask=b'0111'
wait 10

write ";*********************************************************************"
write ";  Step 4.2.5: Verify that the HK entries were restored for each tbl."
write ";*********************************************************************"
; FIRST table should have address count 12, rate 4 and data size 30
if ($SC_$CPU_MD_AddrCnt[1] = 12) and ($SC_$CPU_MD_Rate[1] = 4) and ($SC_$CPU_MD_DataSize[1] = 30) THEN
  write "<*> Passed (9003,9004) - FIRST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - FIRST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[1] = ", $SC_$CPU_MD_AddrCnt[1], " (Expected 12)"
  write "$SC_$CPU_MD_Rate[1] = ", $SC_$CPU_MD_Rate[1], " (Expected 4)"
  write "$SC_$CPU_MD_DataSize[1] = ", $SC_$CPU_MD_DataSize[1], " (Expected 30)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; SECOND table should have address count 2, rate 4 and data size 6 - it was not modified
if ($SC_$CPU_MD_AddrCnt[2] = 2) and ($SC_$CPU_MD_Rate[2] = 4) and ($SC_$CPU_MD_DataSize[2] = 6) THEN
  write "<*> Passed (9003,9004) - SECOND Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - SECOND Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[2] = ", $SC_$CPU_MD_AddrCnt[2], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[2] = ", $SC_$CPU_MD_Rate[2], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[2] = ", $SC_$CPU_MD_DataSize[2], " (Expected 6)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; THIRD table should have address count 3, rate 2 and data size 8
if ($SC_$CPU_MD_AddrCnt[3] = 3) and ($SC_$CPU_MD_Rate[3] = 2) and ($SC_$CPU_MD_DataSize[3] = 8) THEN
  write "<*> Passed (9003,9004) - THIRD Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - THIRD Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[3] = ", $SC_$CPU_MD_AddrCnt[3], " (Expected 3)"
  write "$SC_$CPU_MD_Rate[3] = ", $SC_$CPU_MD_Rate[3], " (Expected 2)"
  write "$SC_$CPU_MD_DataSize[3] = ", $SC_$CPU_MD_DataSize[3], " (Expected 8)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

; LAST table should have address count 2, rate 1 and data size 2
if ($SC_$CPU_MD_AddrCnt[4] = 2) and ($SC_$CPU_MD_Rate[4] = 1) and ($SC_$CPU_MD_DataSize[4] = 2) THEN
  write "<*> Passed (9003,9004) - LAST Dwell Table has correct address count, rate and data size!"
  ut_setrequirements MD_9003, "P"
  ut_setrequirements MD_9004, "P"
else
  write "<!> Failed (9003,9004) - LAST Dwell Table does not have correct address count, rate and data size!"
  write "$SC_$CPU_MD_AddrCnt[4] = ", $SC_$CPU_MD_AddrCnt[4], " (Expected 2)"
  write "$SC_$CPU_MD_Rate[4] = ", $SC_$CPU_MD_Rate[4], " (Expected 1)"
  write "$SC_$CPU_MD_DataSize[4] = ", $SC_$CPU_MD_DataSize[4], " (Expected 2)"
  ut_setrequirements MD_9003, "F"
  ut_setrequirements MD_9004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2.6: Verify that memory dwell messages are received at the"
write ";  proper rate and with the correct number of entries "
write ";*********************************************************************"
write ";  Table 1: Verify that memory dwell messages are received at"
write ";  the proper rate and contain the correct number of entries"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
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
IF ($SC_$CPU_MD_DwlPkt1NumAddresses = 12) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt1PktDataSize = 30) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt1Rate = 4) THEN
  write "<*> Passed (3001) - Dwell packet for table 1 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 1 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Clear the local data
for i = 1 to 15 do
  for j = 1 to 34 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt1_appid & "scnt"}
  while ({"p" & dwell_pkt1_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 34 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt1DwellData[j]
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
  write "<*> Passed (3000;3000.1;3001) - Data was as expected and received at the correct rate."
  ut_setrequirements MD_3000, "P"
  ut_setrequirements MD_3000_1, "P"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3000;3000.1;3001) - Data was NOT as expected and/or not received at the correct rate!"
  ut_setrequirements MD_3000, "F"
  ut_setrequirements MD_3000_1, "F"
  ut_setrequirements MD_3001, "F"
endif

wait 5

write ";********************************************************************"
write ";  Table 2: Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
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
IF ($SC_$CPU_MD_DwlPkt2NumAddresses = 2) THEN
  write "<*> Passed (3001) - Dwell packet for table 2 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 2 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt2PktDataSize = 6) THEN
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

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 2
; Clear the local data
for i = 1 to 15 do
  for j = 1 to 10 do
    dwelldata[i,j] = 0
  enddo
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
  
  ; Collect the data from the packet
  for j = 1 to 10 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt2DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 10 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 4 and the values
; are incrementing once per second, each collected data point should be 4 more
; than the previous one. The data in bytes 7 through 10 should remain 0
; throughout.
passed = 1
if (dwelldata[1,7] <> 0) or (dwelldata[1,8] <> 0) or (dwelldata[1,9] <> 0) or (dwelldata[1,10] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 6 do
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
  if (dwelldata[i,7] <> 0) or (dwelldata[i,8] <> 0) or (dwelldata[i,9] <> 0) or (dwelldata[i,10] <> 0) then
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

wait 5

write ";********************************************************************"
write ";  Table 3:  Verify that memory dwell messages are received at"
write ";  the proper rate and contain only one entry each"
write ";********************************************************************"
;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Wait up to about 10 seconds for dwell packet sequence count to change
cnt = 0
oldval = {"p" & dwell_pkt3_appid & "scnt"}
while ({"p" & dwell_pkt3_appid & "scnt"} = oldval) and (cnt < 40) do
  cnt = cnt + 1
  wait 0.25
enddo

%liv (log_procedure) = logging

; Verify table id of the packet
IF ($SC_$CPU_MD_DwlPkt3TableId = 3) THEN
  write "<*> Passed (3001) - Dwell packet contains ID for table 3"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet not received or packet does not contain ID for table 3!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of addresses of the packet
IF ($SC_$CPU_MD_DwlPkt3NumAddresses = 3) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct number of addresses"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct number of addresses!"
  ut_setrequirements MD_3001, "F"
endif

; Verify number of bytes of data of the packet
IF ($sc_$cpu_MD_DwlPkt3PktDataSize = 8) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct data size"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct data size!"
  ut_setrequirements MD_3001, "F"
endif

; Verify rate of the packet
IF ($sc_$cpu_MD_DwlPkt3Rate = 2) THEN
  write "<*> Passed (3001) - Dwell packet for table 3 has correct data rate"
  ut_setrequirements MD_3001, "P"
else
  write "<!> Failed (3001) - Dwell packet for table 3 does NOT have correct data rate!"
  ut_setrequirements MD_3001, "F"
endif

;; Turn off logging
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

; Verify that the data increments by 2
; Clear the local data
for i = 1 to 15 do
  for j = 1 to 12 do
    dwelldata[i,j] = 0
  enddo
enddo

; Collect data 15 times
for i = 1 to 15 do
  ; Wait up to 10 seconds for dwell packet sequence count to change
  cnt = 0
  oldval = {"p" & dwell_pkt3_appid & "scnt"}
  while ({"p" & dwell_pkt3_appid & "scnt"} = oldval) and (cnt < 40) do
    cnt = cnt + 1
    wait 0.25
  enddo
  
  ; Collect the data from the packet
  for j = 1 to 12 do
    dwelldata[i,j] = $SC_$CPU_MD_DwlPkt3DwellData[j]
  enddo
enddo

%liv (log_procedure) = logging

for j = 1 to 12 do
    write "Byte ", j, "; 1:", dwelldata[1,j], " 2:", dwelldata[2,j], " 3:", dwelldata[3,j], " 4:", dwelldata[4,j], " 5:", dwelldata[5,j], " 6:", dwelldata[6,j], " 7:", dwelldata[7,j], " 8:", dwelldata[8,j], " 9:", dwelldata[9,j], " 10:", dwelldata[10,j], " 11:", dwelldata[11,j], " 12:", dwelldata[12,j], " 13:", dwelldata[13,j], " 14:", dwelldata[14,j], " 15:", dwelldata[15,j]
enddo

; Check the data - since the delay for this dwell table is 2 and the values
; are incrementing once per second, each collected data point should be 2 more
; than the previous one. The data in bytes 9 through 12 should remain 0
; throughout.
passed = 1
if (dwelldata[1,9] <> 0) or (dwelldata[1,10] <> 0) or (dwelldata[1,11] <> 0) or (dwelldata[1,12] <> 0) then
  write "<!> FAILED at start - end of data not 0!"
  passed = 0
endif
for i = 2 to 15 do
  for j = 1 to 6 do
    if (dwelldata[i-1,j] >= 254) then
      if (dwelldata[i,j] <> dwelldata[i-1,j] - 254) then
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    else
      if (dwelldata[i,j] <> (dwelldata[i-1,j] + 2)) THEN
        write "<!> FAILED at (", i, ",", j, ")!!"
        write "Previous value was: ", dwelldata[i-1,j]
        write "This value was: ", dwelldata[i,j]
        passed = 0
      endif
    endif
  enddo
  if (dwelldata[i,9] <> 0) or (dwelldata[i,10] <> 0) or (dwelldata[i,11] <> 0) or (dwelldata[i,12] <> 0) then
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

wait 5

write ";*********************************************************************"
write ";  Step 4.3:  Power On Reset  "
write ";*********************************************************************"
write ";  Step 4.3.1: Send Power On Reset command"
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
write ";  Step 4.3.2: Verify that each MD table is disabled. "
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
write ";  Step 4.3.3: Verify that each MD table is initialized to 0. "
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
write ";  Step 4.3.4: Verify that each MD table has been returned to"
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
write ";  Step 5.0:  Clean-up from this test."
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
write ";  End procedure $SC_$CPU_md_SymTab                                 "
write ";*********************************************************************"
ENDPROC
