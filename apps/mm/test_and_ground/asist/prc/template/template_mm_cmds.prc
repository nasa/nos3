PROC $sc_$cpu_mm_cmds
;*******************************************************************************
;  Test Name:  mm_cmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Memory Manager general commands function
;	properly. The MM_NOOP and MM_ResetCtrs commands will be tested as well
;	as invalid commands to see if the MM application handles these properly.
;
;  Requirements Tested
;     MM1000	Upon receipt of a No-Op command, MM shall increment the MM
;		Valid Command Counter and generate an event message.
;     MM1001	Upon receipt of a Reset command, MM shall reset the following
;		housekeeping variables to a value of zero:
;			a. MM Valid Command Counter
;			b. MM Command Rejected Counter
;     MM1006	For all MM commands, if the length contained in the message
;		header is not equal to the expected length,  MM shall reject the
;		command.
;     MM1009	If MM accepts any command as valid, MM shall execute the 
;		command, increment the MM Valid Command Counter and issue an
;		event message.
;     MM1010	If MM rejects any command, MM shall abort the command execution,
;		increment the MM Command Rejected Counter and issue an error
;		event message.
;     MM1013	The MM application shall send an error event if symbol table 
;		operations are initiated but not supported in the current target
;		environment.
;     MM7001	Upon receipt of a Write Symbol Table command, MM shall save the
;		system symbol table to an onboard data file.
;     MM8000	MM shall generate a housekeeping message containing the
;		following:
;		   a. Valid Command Counter
;		   b. Command Rejected Counter
;		   c. Last command executed
;		   d. Address for last command
;		   e. Memory Type for last command
;		   f. Number of bytes specified by last command
;		   g. Filename used in last command
;		   h. Data Value for last command (may be fill pattern or
;		      peek/poke value)
;     MM9000	Upon initialization of the MM Application, MM shall initialize
;		the following data to zero::
;		   a. Valid Command Counter
;		   b. Command Rejected Counter
;		   c. Last command executed
;		   d. Address for last command
;		   e. Memory Type for last command
;		   f. Number of bytes specified by last command
;		   g. Filename used in last command
;		   h. Data Value for last command (may be fill pattern or
;		      peek/poke value)
;
;  Prerequisite Conditions
;	The CFS is up and running and ready to accept commands.
;	The MM commands and telemetry items exist in the GSE database.
;	A display page exists for the MM Housekeeping telemetry packet.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	03/05/08	Walt Moleski	Original Procedure.
;	10/25/10	Walt Moleski	Replaced setupevt with setupevents and
;					added a variable for the app name
;	07/25/11	Walt Moleski	Modified Step 2.6 since the Dump Symbol
;					Table command is now implemented.
;	04/16/15	Walt Moleski	Updated the requirements for MM 2.4.0.0
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait      Wait for a specified telemetry point to update to
;                         a specified value. 
;       ut_sendcmd      Send commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_sendrawcmd   Send raw commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_pfindicate   Print the pass fail status of a particular requirement
;                         number.
;       ut_setupevents  Performs setup to verify that a particular event
;                         message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			 requirements array.
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "to_lab_events.h"
#include "mm_platform_cfg.h"
#include "mm_events.h"
#include "tst_mm_events.h"
#include "mm_msgdefs.h"

%liv (log_procedure) = logging

#define MM_1000		0
#define MM_1001		1
#define MM_1006		2
#define MM_1009		3
#define MM_1010		4
#define MM_1013		5
#define MM_7001		6
#define MM_8000		7
#define MM_9000		8

global ut_req_array_size = 8
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["MM_1000", "MM_1001", "MM_1006", "MM_1009", "MM_1010", "MM_1013", "MM_7001", "MM_8000", "MM_9000"]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1,symTabPktID
local MMAppName = "MM"
local ramDir = "RAM:0"

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
write ";  Step 1.1:  Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";**********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";**********************************************************************"
page $SC_$CPU_MM_HK
page $SC_$CPU_MM_SYMBOL_TBL
page $SC_$CPU_TST_MM_HK

write ";*********************************************************************"
write ";  Step 1.3:  Start the Memory Manager (MM) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_INIT_INF_EID, "INFO", 2

s load_start_app (MMAppName,"$CPU","MM_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - MM Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for MM not received."
  endif
else
  write "<!> Failed - MM Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'0887'
symTabPktID = "P0F0F"

if ("$CPU" = "CPU2") then
  stream1 = x'0987'
  symTabPktID = "P0F2F"
elseif ("$CPU" = "CPU3") then
  stream1 = x'0A87'
  symTabPktID = "P0F4F"
endif

write "Sending command to add subscription for MM HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 1.4:  Start the Memory Manager Test Application (TST_MM) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_INIT_INF_EID, "INFO", 2
                                                                                
s load_start_app ("TST_MM","$CPU","TST_MM_AppMain")
                                                                                
; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_MM Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_MM not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_MM Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'0924'

if ("$CPU" = "CPU2") then
  stream1 = x'0A24'
elseif ("$CPU" = "CPU3") then
  stream1 = x'0B24'
endif
                                                                                
write "Sending command to add subscription for TST_MM HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 1.5: Verify that the MM Housekeeping packet is being generated "
write ";  and the telemetry items are initialized to zero (0). "
write ";*********************************************************************"
;; Verify the Housekeeping Packet is being generated
local hkPktId

;; Set the HK packet ID based upon the cpu being used
hkPktId = "p087"

if ("$CPU" = "CPU2") then
  hkPktId = "p187"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p287"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements MM_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements MM_8000, "F"
endif

;; Check the HK tlm items to see if they are 0 or NULL
;; the TST_MM application sends its HK packet
if ($SC_$CPU_MM_CMDPC = 0) AND ($SC_$CPU_MM_CMDEC = 0) AND ;;
   ($SC_$CPU_MM_LastActn = 0) AND ($SC_$CPU_MM_MemType = 0) AND ;;
   ($SC_$CPU_MM_Address = 0) AND ($SC_$CPU_MM_DataValue = 0) AND ;;
   ($SC_$CPU_MM_BytesProc = 0) AND ($SC_$CPU_MM_LastFile = "") THEN
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements MM_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_MM_CMDPC
  write "  CMDEC           = ",$SC_$CPU_MM_CMDEC
  write "  Last Action     = ",$SC_$CPU_MM_LastActn
  write "  MemType         = ",$SC_$CPU_MM_MemType
  write "  Address         = ",$SC_$CPU_MM_Address
  write "  Data Value      = ",$SC_$CPU_MM_DataValue
  write "  Bytes processed = ", $SC_$CPU_MM_BytesProc
  write "  Filename        = ",$SC_$CPU_MM_LastFile
  ut_setrequirements MM_9000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC

;; Enable DEBUG events for the CS and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=MMAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0: MM General Commanding tests."
write ";*********************************************************************"
write ";  Step 2.1: Send a valid No-Op command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_NOOP_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_MM_NOOP"
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1000;1009) - NOOP command sent properly."
  ut_setrequirements MM_1000, "P"
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1000;1009) - NOOP command not sent properly."
  ut_setrequirements MM_1000, "F"
  ut_setrequirements MM_1009, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1000;1009) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1000, "P"
  ut_setrequirements MM_1009, "P"

  ;; Verify the HK fields
  if ($SC_$CPU_MM_LastActn = MM_NOOP) AND ($SC_$CPU_MM_MemType = 0) AND ;;
     ($SC_$CPU_MM_Address = 0) AND ($SC_$CPU_MM_DataValue = 0) AND ;;
     ($SC_$CPU_MM_BytesProc = 0) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected"
    ut_setrequirements MM_8000, "P"
  else
    write "<!> Failed (8000) - Last Command HK NOT correct."
    write "  Last Action     = ",p@$SC_$CPU_MM_LastActn
    write "  MemType         = ",p@$SC_$CPU_MM_MemType
    write "  Address         = ",%hex($SC_$CPU_MM_Address,8)
    write "  Data Value      = ",%hex($SC_$CPU_MM_DataValue,2)
    write "  Bytes processed = ",$SC_$CPU_MM_BytesProc
    write "  Filename        = ",$SC_$CPU_MM_LastFile
    ut_setrequirements MM_8000, "F"
  endif
else
  write "<!> Failed (1000;1009) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_NOOP_INF_EID, "."
  ut_setrequirements MM_1000, "F"
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send a No-Op command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_MM_CMDEC + 1
rawcmd = ""

;;; CPU1 is the default
rawcmd = "1888c000000200AC"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c00000020054"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c00000020054"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Send a valid ResetCtrs command."
write ";*********************************************************************"
;; Check that the counters are not 0
if ($SC_$CPU_MM_CMDPC = 0) then
  ;; Send a NOOP command
  /$SC_$CPU_MM_NOOP
  wait 5
endif

if ($SC_$CPU_MM_CMDEC = 0) then
  /raw {rawcmd}
  wait 5
endif

;; Setup for the expected event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_RESET_DBG_EID, "DEBUG", 1

/$SC_$CPU_MM_RESETCTRS
wait 5

ut_tlmwait $SC_$CPU_MM_CMDPC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;1009) - Valid Command Counter was reset."
  ut_setrequirements MM_1001, "P"
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1001;1009) - Valid Command Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements MM_1001, "F"
  ut_setrequirements MM_1009, "F"
endif

ut_tlmwait $SC_$CPU_MM_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001) - Command Rejected Counter was reset."
  ut_setrequirements MM_1001, "P"
else
  write "<!> Failed (1001) - Command Rejected Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements MM_1001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1001, "P"
  ;; Verify the HK fields
  if ($SC_$CPU_MM_LastActn = MM_RESET) AND ($SC_$CPU_MM_MemType = 0) AND ;;
     ($SC_$CPU_MM_Address = 0) AND ($SC_$CPU_MM_DataValue = 0) AND ;;
     ($SC_$CPU_MM_BytesProc = 0) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected"
    ut_setrequirements MM_8000, "P"
  else
    write "<!> Failed (8000) - Last Command HK NOT correct."
    write "  Last Action     = ",p@$SC_$CPU_MM_LastActn
    write "  MemType         = ",p@$SC_$CPU_MM_MemType
    write "  Address         = ",%hex($SC_$CPU_MM_Address,8)
    write "  Data Value      = ",%hex($SC_$CPU_MM_DataValue,2)
    write "  Bytes processed = ",$SC_$CPU_MM_BytesProc
    write "  Filename        = ",$SC_$CPU_MM_LastFile
    ut_setrequirements MM_8000, "F"
  endif
else
  write "<!> Failed (1001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_RESET_DBG_EID, "."
  ut_setrequirements MM_1001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Send a ResetCtrs command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MM_CMDEC + 1

;;; CPU1 is the default
rawcmd = "1888c000000401AD"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000000401AD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000000401AD"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.5: Send an invalid MM command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_CC1_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_MM_CMDEC + 1

;;; CPU1 is the default
rawcmd = "1888c0000001aa"
if ("$CPU" = "CPU2") then
  rawcmd = "1988c0000001aa"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c0000001aa"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_CC1_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6: Send the Write Symbol Table command."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMTBL_TO_FILE_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMTBL_TO_FILE_FAIL_ERR_EID,"ERROR",2

cmdCtr = $SC_$CPU_EVS_CMDPC

;; Send the Write Symbol Table command
/$SC_$CPU_MM_SymTbl2File FileName="/ram/mm_symbol_tbl.dat"

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;7001) - Symbol Table to File Command sent successfully."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_7001, "P"
else
  write "<!> Failed (1009;7001) - Symbol Table to File command did not increment the CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_7001, "F"
endif

;; Check if the error event was issued
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (1013) - Symbol Table operations are not supported on this platform."
  ut_setrequirements MM_1013, "P"
else
  write "<*> Requirement 1013 could not be tested since Symbol Operations are supported on this platform."
  ;; Download the file
  s ftp_file (ramDir,"mm_symbol_tbl.dat","mm_symbol_tbl.dat","$CPU","G")
  wait 10

  FILE_TO_CVT %name("mm_symbol_tbl.dat") %name(symTabPktID)
endif

;; If the first event message was rcv'd, Symbol write command executed
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  ;; Verify the HK fields
  if ($SC_$CPU_MM_LastActn = MM_SYMTBL_SAVE) AND ;;
     ($SC_$CPU_MM_MemType = 0) AND ;;
     ($SC_$CPU_MM_Address = 0) AND ($SC_$CPU_MM_DataValue = 0) AND ;;
     ($SC_$CPU_MM_BytesProc = 0) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/mm_symbol_tbl.dat") THEN
    write "<*> Passed (8000) - Last Command HK items as expected"
    ut_setrequirements MM_8000, "P"
  else
    write "<!> Failed (8000) - Last COmmand HK NOT correct."
    write "  Last Action     = ",p@$SC_$CPU_MM_LastActn
    write "  MemType         = ",p@$SC_$CPU_MM_MemType
    write "  Address         = ",%hex($SC_$CPU_MM_Address,8)
    write "  Data Value      = ",%hex($SC_$CPU_MM_DataValue,2)
    write "  Bytes processed = ",$SC_$CPU_MM_BytesProc
    write "  Filename        = ",$SC_$CPU_MM_LastFile
    ut_setrequirements MM_8000, "F"
  endif
endif

write ";*********************************************************************"
write ";  Step 3.0:  Perform a Power-on Reset to clean-up from this test."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

;; Remove the display pages from the screen
clear $SC_$CPU_MM_HK
clear $SC_$CPU_TST_MM_HK

write "**** Requirements Status Reporting"

write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_mm_cmds                                    "
write ";*********************************************************************"
ENDPROC
