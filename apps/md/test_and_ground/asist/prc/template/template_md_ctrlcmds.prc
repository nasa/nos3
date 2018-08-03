PROC $sc_$cpu_md_ctrlcmds
;*******************************************************************************
;  Test Name:  MD_CtrlCmds
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;    The purpose of this test is to verify that the Memory Dwell (MD)
;    Start and Stop control commands function properly.
;
;  Requirements Tested
;    MD1003	If Dwell Table ID specified in any MD command exceeds the
;		<PLATFORM_DEFINED> maximum number of allowable memory dwells,
;		MD shall reject the commans and issue an event message.
;    MD1004	If MD accepts any command as valid, MD shall execute the
;		command, increment the MD Valid Command Counter and issue an
;		event message.
;    MD1005	If MD rejects any command, MD shall abort the command execution,
;		increment the MD Command Rejected Counter and issue an event
;		message.
;    MD2000	Upon receipt of a Start Dwell command, MD shall identify the
;		command-specified tables as ENABLED and start processing the
;		command-specified memory dwell tables, starting with the first
;		entry, until one of the following:
;			a) an entry that has a zero value for the Number of
;			    Bytes field
;			b) until it has processed the last entry in a Dwell
;			   Table
;    MD2000.2	If the sum of all the 'delay between samples' for any memory
;		dwell table being commanded to start equals 0, MD shall send an
;		event to notify the user that no processing will occur in the
;		dwell table's current state.
;    MD2001	Upon receipt of a Stop Dwell command, MD shall identify the
;		command-specified memory dwell tables as DISABLED and stop
;		processing the command-specified memory dwell tables.
;    MD2001.1	The following items shall be set to zero for each Dwell:
;			1) Current Dwell Packet Index
;			2) Current Entry in the Dwell Table
;			3) Current Countdown counter
;    MD4000	Upon receipt of a Jam Dwell command, MD shall update the
;		command-specified memory dwell table with the command-specified
;		information:
;			a) Dwell Table Index
;			b) Address
;			c) Number of bytes (0,1,2 or 4)
;			d) Delay Between Samples
;    MD8000	MD shall generate a housekeeping message containing the
;		following:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;			c) For Each Dwell:
;				1. Enable/Disable Status
;				2. Number of Dwell Addresses
;				3. Dwell Rate
;				4. Number of Bytes
;				5. Current Dwell Packet Index
;				6. Current Entry in the Dwell Table
;				7. Current Countdown counter
;    MD9000	Upon any initialization of the MD Application (cFE Power On, cFE
;		Processor Reset or MD Application Reset), MD shall initialize
;		the following data to Zero
;			a) Valid Command Counter
;			b) Command Rejected Counter
;    MD9001	Upon cFE Power-on Reset, MD shall initialize each Memory Dwell
;		table status to DISABLED.
;    MD9002	Upon cFE Power-on Reset, MD shall initialize each Memory Dwell
;		table to zero.
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
;   Date         Name        Description
;   06/20/08     S. Jonke    Original Procedure
;   02/24/09     W. Moleski  Updated for MD 1.1.0.0
;   12/04/09     W. Moleski  Added requirements to this prolog and also turned 
;			     logging off around code that did not provide any
;			     significant benefit of logging
;   04/28/11     W. Moleski  Added a variable for the app name and replaced the
;			     hard-coded app name with the variable
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

#define MD_1003     0
#define MD_1004     1
#define MD_1005     2
#define MD_2000     3
#define MD_2000_2   4
#define MD_2001     5
#define MD_2001_1   6
#define MD_4000     7
#define MD_8000     8
#define MD_9000     9
#define MD_9001     10
#define MD_9002     11

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 11
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["MD_1003", "MD_1004", "MD_1005", "MD_2000", "MD_2000_2", "MD_2001", "MD_2001_1", "MD_4000", "MD_8000", "MD_9000", "MD_9001", "MD_9002"]

local cmdcnt, errcnt
local rawcmd, hkPktId
local stream1, dwell1, dwell2, dwell3, dwell4
local passed
LOCAL dwell_tbl1_load_pkt, dwell_tbl2_load_pkt, dwell_tbl3_load_pkt, dwell_tbl4_load_pkt
LOCAL dwell_tbl1_load_appid, dwell_tbl2_load_appid, dwell_tbl3_load_appid, dwell_tbl4_load_appid
local testdata_addr

local MDAppName = "MD"

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

;;; Need to set the stream based upon the cpu being used
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

;;; Need to set the stream based upon the cpu being used
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

wait 5

write ";*********************************************************************"
write ";  Step 1.3: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdcnt = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the MD application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MDAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 1.4: Verify that the MD Housekeeping packet is being generated "
write ";  and the telemetry items are initialized to zero (0). "
write ";*********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the 
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

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
  write "  EnableMask = ",$SC_$CPU_MD_EnableMask
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
write ";  Step 2.0: Setup initial dwell tables "
write ";*********************************************************************"
write ";  Step 2.1:  Send Jam Dwell commands on FIRST table, creating an"
write ";             entry with delay 1.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=1 FieldLength=1 DwellDelay=1 Offset=0 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) -Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Add another entry with delay 0
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=2 FieldLength=1 DwellDelay=0 Offset=1 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Write a null entry to end the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_NULL_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=1 EntryId=3 FieldLength=0 DwellDelay=0 Offset=0 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_NULL_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

write ";*********************************************************************"
write ";  Step 2.2:  Send Jam Dwell commands on SECOND table, creating an"
write ";             entry with delay 1.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=1 FieldLength=2 DwellDelay=1 Offset=2 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Add another entry with delay 0
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=2 FieldLength=2 DwellDelay=0 Offset=2 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Write a null entry to end the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_NULL_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=3 FieldLength=0 DwellDelay=0 Offset=0 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_NULL_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

write ";*********************************************************************"
write ";  Step 2.3:  Send Jam Dwell commands on THIRD table, creating an"
write ";             entry with delay 1.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=1 FieldLength=4 DwellDelay=1 Offset=4 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Add another entry with delay 0
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=2 FieldLength=4 DwellDelay=0 Offset=8 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Write a null entry to end the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_NULL_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=3 EntryId=3 FieldLength=0 DwellDelay=0 Offset=0 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_NULL_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

write ";*********************************************************************"
write ";  Step 2.4:  Send Jam Dwell commands on LAST table, creating an"
write ";             entry with delay 1.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=1 FieldLength=1 DwellDelay=1 Offset=8 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Add another entry with delay 0
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=2 FieldLength=1 DwellDelay=0 Offset=9 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

; Write a null entry to end the dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_NULL_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=4 EntryId=3 FieldLength=0 DwellDelay=0 Offset=0 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_NULL_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

write ";*********************************************************************"
write ";  Step 3.0:  Start & Stop Command - Single Table Cases"
write ";*********************************************************************"
write ";  Step 3.1:  Start FIRST table  "
write ";********************************************************************"
; Send start dwell command, enabling FIRST dwell table
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

write ";*********************************************************************"
write ";  Step 3.2:  Start SECOND table  "
write ";********************************************************************"
; Send start dwell command, enabling SECOND dwell table
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
  write "<*> Passed (2000) - Second Dwell Table was started, FIRST Dwell Table still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3:  Start LAST table  "
write ";********************************************************************"
; Send start dwell command, enabling LAST dwell table
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
  write "<*> Passed (2000) - Last Dwell Table was started, FIRST & SECOND Dwell Tables still running.."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.4:  Stop FIRST table  "
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[1] = 0) or ($SC_$CPU_MD_DwTblEntry[1] = 0) or ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<!> Failed - Dwell data for FIRST table is already reset!"
endif

; Send stop dwell command, disabling FIRST dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'0001'"
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
if ($SC_$CPU_MD_EnableMask = b'1010') then
  write "<*> Passed (2001) - FIRST Dwell Table was stopped, SECOND & LAST Dwell Tables still running."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown
if ($SC_$CPU_MD_DwPktOffset[1] = 0) and ($SC_$CPU_MD_DwTblEntry[1] = 0) and ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<*> Passed (2001.1) - FIRST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - FIRST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 3.5:  Stop SECOND table  "
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[2] = 0) or ($SC_$CPU_MD_DwTblEntry[2] = 0) or ($SC_$CPU_MD_CountDown[2] = 0) then
  write "<!> Failed - Dwell data for SECOND table is already reset!"
endif

; Send stop dwell command, disabling SECOND dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'0010'"
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
if ($SC_$CPU_MD_EnableMask = b'1000') then
  write "<*> Passed (2001) - SECOND Dwell Table was stopped, LAST Dwell Table still running."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown
if ($SC_$CPU_MD_DwPktOffset[2] = 0) and ($SC_$CPU_MD_DwTblEntry[2] = 0) and ($SC_$CPU_MD_CountDown[2] = 0) then
  write "<*> Passed (2001.1) - SECOND Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - SECOND Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 3.6:  Stop LAST table  "
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[4] = 0) or ($SC_$CPU_MD_DwTblEntry[4] = 0) or ($SC_$CPU_MD_CountDown[4] = 0) then
  write "<!> Failed - Dwell data for LAST table is already reset!"
endif

; Send stop dwell command, disabling LAST dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'1000'"
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
  write "<*> Passed (2001) - LAST Dwell Table was stopped."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown
if ($SC_$CPU_MD_DwPktOffset[4] = 0) and ($SC_$CPU_MD_DwTblEntry[4] = 0) and ($SC_$CPU_MD_CountDown[4] = 0) then
  write "<*> Passed (2001.1) - LAST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - LAST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 4.0:  Start & Stop Command - Multiple Table Cases"
write ";*********************************************************************"
write ";  Step 4.1:  Start all tables  "
write ";********************************************************************"
; Send start dwell command, enabling all dwell tables
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
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 4.2:  Stop FIRST & LAST tables  "
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[1] = 0) or ($SC_$CPU_MD_DwTblEntry[1] = 0) or ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<!> Failed - Dwell data for FIRST table is already reset!"
endif

if ($SC_$CPU_MD_DwPktOffset[4] = 0) or ($SC_$CPU_MD_DwTblEntry[4] = 0) or ($SC_$CPU_MD_CountDown[4] = 0) then
  write "<!> Failed - Dwell data for LAST table is already reset!"
endif

; Send stop dwell command, disabling FIRST and LAST dwell tables
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'1001'"
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
if ($SC_$CPU_MD_EnableMask = b'0110') then
  write "<*> Passed (2000) - FIRST & LAST Tables were stopped, SECOND & THIRD tables still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown for FIRST table
if ($SC_$CPU_MD_DwPktOffset[1] = 0) and ($SC_$CPU_MD_DwTblEntry[1] = 0) and ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<*> Passed (2001.1) - FIRST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - FIRST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown for LAST table
if ($SC_$CPU_MD_DwPktOffset[4] = 0) and ($SC_$CPU_MD_DwTblEntry[4] = 0) and ($SC_$CPU_MD_CountDown[4] = 0) then
  write "<*> Passed (2001.1) - LAST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - LAST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 4.3:  Start FIRST & LAST tables  "
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
if ($SC_$CPU_MD_EnableMask = b'1111') then
  write "<*> Passed (2000) - FIRST & LAST Dwell Tables were started, SECOND & THIRD tables still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 4.4:  Stop ALL tables  "
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[1] = 0) or ($SC_$CPU_MD_DwTblEntry[1] = 0) or ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<!> Failed - Dwell data for FIRST table is already reset!"
endif

if ($SC_$CPU_MD_DwPktOffset[2] = 0) or ($SC_$CPU_MD_DwTblEntry[2] = 0) or ($SC_$CPU_MD_CountDown[2] = 0) then
  write "<!> Failed - Dwell data for SECOND table is already reset!"
endif

if ($SC_$CPU_MD_DwPktOffset[3] = 0) or ($SC_$CPU_MD_DwTblEntry[3] = 0) or ($SC_$CPU_MD_CountDown[3] = 0) then
  write "<!> Failed - Dwell data for THIRD table is already reset!"
endif

if ($SC_$CPU_MD_DwPktOffset[4] = 0) or ($SC_$CPU_MD_DwTblEntry[4] = 0) or ($SC_$CPU_MD_CountDown[4] = 0) then
  write "<!> Failed - Dwell data for LAST table is already reset!"
endif

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
  write "<*> Passed (2001) - ALL Dwell Tables were stopped."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown for FIRST table
if ($SC_$CPU_MD_DwPktOffset[1] = 0) and ($SC_$CPU_MD_DwTblEntry[1] = 0) and ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<*> Passed (2001.1) - FIRST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - FIRST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown for SECOND table
if ($SC_$CPU_MD_DwPktOffset[2] = 0) and ($SC_$CPU_MD_DwTblEntry[2] = 0) and ($SC_$CPU_MD_CountDown[2] = 0) then
  write "<*> Passed (2001.1) - SECOND Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001.1) - SECOND Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown for THIRD table
if ($SC_$CPU_MD_DwPktOffset[3] = 0) and ($SC_$CPU_MD_DwTblEntry[3] = 0) and ($SC_$CPU_MD_CountDown[3] = 0) then
  write "<*> Passed (2001.1) - THIRD Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001.1) - THIRD Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown for LAST table
if ($SC_$CPU_MD_DwPktOffset[4] = 0) and ($SC_$CPU_MD_DwTblEntry[4] = 0) and ($SC_$CPU_MD_CountDown[4] = 0) then
  write "<*> Passed (2001.1) - LAST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001.1) - LAST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 5.0:  Start & Stop Command - Redundant Cases"
write ";*********************************************************************"
write ";  Step 5.1:  Start FIRST & THIRD tables with THIRD table already"
write ";             enabled"
write ";********************************************************************"
; Send start dwell command, enabling THIRD table
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
if ($SC_$CPU_MD_EnableMask = b'0100') then
  write "<*> Passed (2000) - THIRD Dwell Table was started."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

; Send start dwell command, enabling FIRST and THIRD dwell tables (THIRD table
; is already enabled)
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StartDwell TableMask=b'0101'"
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
if ($SC_$CPU_MD_EnableMask = b'0101') then
  write "<*> Passed (2000) - FIRST Dwell Table was started, THIRD Dwell Table is still running."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 5.2:  Stop FIRST Dwell Table when already stopped"
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[1] = 0) or ($SC_$CPU_MD_DwTblEntry[1] = 0) or ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<!> Failed - Dwell data for FIRST table is already reset!"
endif

; Send stop dwell command, disabling FIRST dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'0001'"
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
if ($SC_$CPU_MD_EnableMask = b'0100') then
  write "<*> Passed (2001) - FIRST Dwell Table was stopped, THIRD Dwell Table still running."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown
if ($SC_$CPU_MD_DwPktOffset[1] = 0) and ($SC_$CPU_MD_DwTblEntry[1] = 0) and ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<*> Passed (2001.1) - FIRST Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - FIRST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

; Send stop dwell command, disabling FIRST dwell table which is already stopped
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_STOP_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_StopDwell TableMask=b'0001'"
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
if ($SC_$CPU_MD_EnableMask = b'0100') then
  write "<*> Passed (2001) - FIRST Dwell Table still stopped, THIRD Dwell Table still running."
  ut_setrequirements MD_2001, "P"
else
  write "<!> Failed (2001) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2001, "F"
endif

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown
if ($SC_$CPU_MD_DwPktOffset[1] = 0) and ($SC_$CPU_MD_DwTblEntry[1] = 0) and ($SC_$CPU_MD_CountDown[1] = 0) then
  write "<*> Passed (2001.1) - FIRST Dwell Table dwell data is still reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - FIRST Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 5.2:  Stop ALL Dwell Tables with only THIRD table enabled"
write ";********************************************************************"
if ($SC_$CPU_MD_DwPktOffset[3] = 0) or ($SC_$CPU_MD_DwTblEntry[3] = 0) or ($SC_$CPU_MD_CountDown[3] = 0) then
  write "<!> Failed - Dwell data for THIRD table is already reset!"
endif

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

; Check MD_DwellPktOffset, MD_DwellTblEntry & MD_CountDown
if ($SC_$CPU_MD_DwPktOffset[3] = 0) and ($SC_$CPU_MD_DwTblEntry[3] = 0) and ($SC_$CPU_MD_CountDown[3] = 0) then
  write "<*> Passed (2001.1) - THIRD Dwell Table dwell data was reset"
  ut_setrequirements MD_2001_1, "P"
else
  write "<!> Failed (2001) - THIRD Dwell Table dwell data was NOT reset!"
  ut_setrequirements MD_2001_1, "F"
endif

write ";*********************************************************************"
write ";  Step 6.0:  Start & Stop Command - Special Cases"
write ";*********************************************************************"
write ";  Step 6.1:  SECOND table has total delay of 0"
write ";********************************************************************"
; Send jam dwell command on SECOND table to create total delay of 0
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_JAM_DWELL_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MD_JAMDWELL TableId=2 EntryId=1 FieldLength=2 DwellDelay=0 Offset=2 SymName="""""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (4000) - Jam Dwell command not sent properly (", UT_SC_Status, ")."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell command sent properly."
  ut_setrequirements MD_4000, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) THEN
  write "<!> Failed (4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", MD_JAM_DWELL_INF_EID, "."
  ut_setrequirements MD_4000, "F"
else
  write "<*> Passed (4000) - Jam Dwell event message rcv'd."
  ut_setrequirements MD_4000, "P"
endif

; Send start dwell command, enabling SECOND dwell table
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_START_DWELL_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_ZERO_RATE_CMD_INF_EID, "INFO", 2

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

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (2000.2) - No packets will be sent message was received!"
  write "<*> Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements MD_2000_2, "P"
else
  write "<!> Failed (2000.2) - No packets will be sent message was NOT received!"
  ut_setrequirements MD_2000_2, "F"
endif

; Check MD_EnableMask
if ($SC_$CPU_MD_EnableMask = b'0010') then
  write "<*> Passed (2000) - Second Dwell Table was started."
  ut_setrequirements MD_2000, "P"
else
  write "<!> Failed (2000) - MD_EnableMask is not the expected value!"
  ut_setrequirements MD_2000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.0:  Start & Stop Command - Invalid Cases"
write ";*********************************************************************"
write ";  Step 7.1:  Start Dwell command with invalid mask"
write ";********************************************************************"
; Send a start dwell command with a dwell mask containing no valid dwell
; table IDs
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_EMPTY_TBLMASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_StartDwell TableMask=x'2000'

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1003, "P"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1003;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1003, "F"
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_EMPTY_TBLMASK_ERR_EID, "."
  ut_setrequirements MD_1003, "F"
endif

write ";*********************************************************************"
write ";  Step 7.2:  Stop Dwell command with invalid mask"
write ";********************************************************************"
; Send a stop dwell command with a dwell mask containing no valid dwell
; table IDs
ut_setupevents "$SC", "$CPU", {MDAppName}, MD_EMPTY_TBLMASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MD_CMDEC + 1

/$SC_$CPU_MD_StopDwell TableMask=x'2000'

ut_tlmwait $SC_$CPU_MD_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;1005) - Command Rejected Counter incremented."
  ut_setrequirements MD_1003, "P"
  ut_setrequirements MD_1005, "P"
else
  write "<!> Failed (1003;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MD_1003, "F"
  ut_setrequirements MD_1005, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MD_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MD_EMPTY_TBLMASK_ERR_EID, "."
  ut_setrequirements MD_1003, "F"
endif

write ";*********************************************************************"
write ";  Step 8.0:  Perform a Power-on Reset to clean-up from this test."
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

clear $SC_$CPU_MD_DWELL_PKT1
clear $SC_$CPU_MD_DWELL_PKT2
clear $SC_$CPU_MD_DWELL_PKT3
clear $SC_$CPU_MD_DWELL_PKT4

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_md_CtrlCmds                                 "
write ";*********************************************************************"
ENDPROC
