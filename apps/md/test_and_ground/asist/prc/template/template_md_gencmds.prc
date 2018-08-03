PROC $sc_$cpu_md_gencmds
;*******************************************************************************
;  Test Name:  MD_GenCmds
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to verify that the Memory Dwell (MD)
;   general commands function properly. MD Initialization and the
;   MD_NOOP and MD_Reset commands will be tested as well as invalid
;   commands.
;
;  Requirements Tested
;   MD1000  Upon receipt of a No-Op command, MD shall increment the MD
;           Valid Command Counter and generate an event message.
;   MD1001  Upon receipt of a Reset command,  MD shall reset the
;           following housekeeping variables to a value of zero:
;              a.  MD Valid Command Counter
;              b.  MD Command Rejected Counter
;   MD1002  For all MD commands, if the length contained in the message
;           header is not equal to the expected length, MD shall reject
;           the command and issue an event message.
;   MD1003  If dwell table ID specified in any MD command exceeds the
;           <PLATFORM_DEFINED> maximum number of allowable memory dwells,
;           MD shall reject the command and issue an event message
;   MD1004  If MD accepts any command as valid, MD shall execute the
;           command, increment the MD Valid Command Counter and issue an
;           event message
;   MD1005  If MD rejects any command, MD shall abort the command
;           execution, increment the MD Command Rejected Counter and
;           issue an error event message
;   MD1006  The MD application shall generate an error event message if
;           symbol table operations are initiated but not supported in
;           the current target environment.
;   MD8000  MD shall generate a housekeeping message containing the
;           following:
;              a)  Valid Command Counter
;              b)  Command Rejected Counter
;              c)  For each Dwell:
;                  1.  Enable/Disable Status
;                  2.  Number of Dwell Addresses
;                  3.  Dwell Rate
;                  4.  Number of Bytes
;                  5.  Current Dwell Packet Index
;                  6.  Current Entry in the Dwell Table
;                  7.  Current Countdown counter
;   MD9000  Upon any Initialization of the MD Application (cFE Power
;           On, cFE Processor Reset or MD Application Reset),  MD shall
;           initialize the following data to Zero
;              a)  Valid Command Counter
;              b)  Command Rejected Counter
;   MD9001  Upon cFE Power-on Reset, MD shall initialize each Memory
;           Dwell table status to DISABLED
;   MD9002  Upon cFE Power-on Reset, MD shall initialize each Memory
;           Dwell table to zero
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
;   Date          Name     	Description
;   05/20/08      S. Jonke	Original Procedure
;   12/04/09      W. Moleski    Turned logging off around code that did not
;                               provide any significant benefit of logging
;   04/28/11      W. Moleski    Added variables for the App and table names
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

%liv (log_procedure) = logging

#define MD_1000     0
#define MD_1001     1
#define MD_1002     2
#define MD_1003     3
#define MD_1004     4
#define MD_1005     5
#define MD_1006     6
#define MD_8000     7
#define MD_9000     8
#define MD_9001     9
#define MD_9002     10

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 10
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1000", "MD_1001", "MD_1002", "MD_1003", "MD_1004", "MD_1005", "MD_1006", "MD_8000", "MD_9000", "MD_9001", "MD_9002"]

local rawcmd
local stream1, dwell1, dwell2, dwell3, dwell4
local passed
local testdata_addr

local errcnt

local MDAppName = "MD"

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
;;; CPU1 is the default
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

; write "Opening MD Dwell Pages"
page $SC_$CPU_MD_DWELL_PKT1
page $SC_$CPU_MD_DWELL_PKT2
page $SC_$CPU_MD_DWELL_PKT3
page $SC_$CPU_MD_DWELL_PKT4

write ";*********************************************************************"
write ";  Step 1.3: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the MD and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 1.4: Verify that the MD Housekeeping packet is being generated"
write ";  and telemetry items are initialized to zero (0). "
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
write ";  Step 2.0: Command Testing "
write ";*********************************************************************"
write ";  Step 2.1: Send a NO-OP command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_NOOP_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_NOOP"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (1000;1004;8000) - NOOP command sent properly."
  ut_setrequirements MD_1000, "P"
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1000;1004;8000) - NOOP command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_1000, "F"
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1000;1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1000, "P"
  ut_setrequirements MD_1004, "P"
else
  write "<!> Failed (1000;1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_NOOP_INF_EID, "."
  ut_setrequirements MD_1000, "F"
  ut_setrequirements MD_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send a No-Op command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
rawcmd = ""

;;; CPU1 is the default
rawcmd = "1890C000000200A8"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000000200A8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000000200A8"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1002;1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
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
write ";  Step 2.3: Send a Start Dwell command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
rawcmd = ""

;;; CPU1 is the default
rawcmd = "1890C000000402A80000"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000000402A80000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000000402A80000"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1002;1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
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
write ";  Step 2.4: Send a Stop Dwell command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
rawcmd = ""

;;; CPU1 is the default
rawcmd = "1890C000000403A80000"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000000403A80000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000000403A80000"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1002;1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
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
write ";  Step 2.5: Send a Jam Dwell command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
rawcmd = ""

;;; CPU1 is the default
rawcmd = "1890C000005F04A800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000005F04A800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000005F04A800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1002;1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
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
write ";  Step 2.6: Send a Reset command. "
write ";*********************************************************************"
;; Check that the counters are not 0
if ($SC_$CPU_MD_CMDPC = 0) then
  ;; Send a NOOP command
  /$SC_$CPU_MD_NOOP
  wait 5
endif

if ($SC_$CPU_MD_CMDEC = 0) then
  /raw {rawcmd}
  wait 5
endif

;; Setup for the expected event
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_RESET_CNTRS_DBG_EID, "DEBUG", 1

/$SC_$CPU_MD_RESETCTRS
wait 5

ut_tlmwait $SC_$CPU_MD_CMDPC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;1004;8000) - Valid Command Counter was reset."
  ut_setrequirements MD_1001, "P"
  ut_setrequirements MD_1004, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1001;1004;8000) - Valid Command Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements MD_1001, "F"
  ut_setrequirements MD_1004, "F"
  ut_setrequirements MD_8000, "F"
endif

ut_tlmwait $SC_$CPU_MD_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;8000) - Command Rejected Counter was reset."
  ut_setrequirements MD_1001, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1001;8000) - Command Rejected Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements MD_1001, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1001, "P"
else
  write "<!> Failed (1001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_RESET_CNTRS_DBG_EID, "."
  ut_setrequirements MD_1001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Send a ResetCtrs command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
rawcmd = ""
;;; CPU1 is the default
rawcmd = "1890C000000401AD"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C000000401AD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C000000401AD"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1002, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1002;1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1002, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_CMD_LEN_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.8: Send an invalid MD command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CC_NOT_IN_TBL_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1
;;; CPU1 is the default
rawcmd = "1890C0000001aa"

if ("$CPU" = "CPU2") then
  rawcmd = "1990C0000001aa"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A90C0000001aa"
endif

ut_sendrawcmd "$SC_$CPU_MD", (rawcmd)

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_CC_NOT_IN_TBL_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Send a Start Dwell command with a dwell mask containing"
write ";            no valid dwell IDs."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_EMPTY_TBLMASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_STARTDWELL TableMask=x'1000'

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_EMPTY_TBLMASK_ERR_EID, "."
  ut_setrequirements MD_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Send a Stop Dwell command with a dwell mask containing"
write ";            no valid dwell IDs."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_EMPTY_TBLMASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_STOPDWELL TableMask=x'1000'

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;1005;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1003, "P"
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1003;1005;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1003, "F"
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_EMPTY_TBLMASK_ERR_EID, "."
  ut_setrequirements MD_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.11: Send a Jam Dwell command specifying an invalid symbol and offset."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=1 FieldLength=1 DwellDelay=1 Offset=x'FF' SymName="BogusSymbolMDGC"

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;1006;8000) - Command Rejected Counter incremented."
  ut_setrequirements MD_1005, "P"
  ut_setrequirements MD_1006, "P"
  ut_setrequirements MD_8000, "P"
else
  write "<!> Failed (1005;1006;8000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1005, "F"
  ut_setrequirements MD_1006, "F"
  ut_setrequirements MD_8000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1006) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1006, "P"
else
  write "<!> Failed (1006) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, "."
  ut_setrequirements MD_1006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Perform a Power-on Reset to clean-up from this test."
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


write ";*********************************************************************"
write ";  End procedure $SC_$CPU_md_GenCmds                                    "
write ";*********************************************************************"
ENDPROC
