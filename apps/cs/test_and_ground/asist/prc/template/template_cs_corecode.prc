PROC $sc_$cpu_cs_corecode
;*******************************************************************************
;  Test Name:  cs_corecode
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) OS and cFE Core code
;	segment checksumming commands function and handle anomolies properly.
;
;  Requirements Tested
;    CS1002	For all CS commands, if the length contained in the message
;		header is not equal to the expected length, CS shall reject the
;		command and issue an event message.
;    CS1003	If CS accepts any command as valid, CS shall execute the
;		command, increment the CS Valid Command Counter and issue an
;		event message.
;    CS1004	If CS rejects any command, CS shall abort the command execution,
;		increment the CS Command Rejected Counter and issue an event
;		message.
;    CS3000	Checksum shall calculate CRC for the OS code segment and
;		compare them against the corresponding baseline OS code segment
;		CRC if:
;			a) Checksumming (as a whole) is Enabled
;			b) OS segment checksumming is Enabled
;    CS3000.1	If the OS code segment CRC is not equal to the baseline OS code
;		segment CRC, CS shall increment the OS Code Segment CRC
;		Miscompare Counter and send an event message.
;    CS3002	Upon receipt of an Enable OS Code Segment command, CS shall 
;		enable checksumming of the OS Code segment.
;    CS3003	Upon receipt of a Disable OS Code Segment command, CS shall 
;		disable checksumming of the OS Code segment.
;    CS3004	Upon receipt of a Recompute OS Code Segment CRC, CS shall:
;			a) Recompute the baseline CRC for the OS Code segment
;			b) Set the Recompute In Progress Flag to TRUE
;    CS3004.1	Once the baseline CRC is computed, CS shall:
;			a) Generate an event message containing the baseline CRC
;			b) Set the Recompute In Progress Flag to FALSE
;    CS3004.2	If CS is already processing a Recompute CRC command or a One
;		Shot CRC command, CS shall reject the command.
;    CS3005	Upon receipt of a Report OS Code Segment CRC, CS shall send an
;		event message containing the baseline OS code segment CRC.
;    CS3006	Checksum shall calculate CRC for the cFE code segment and
;		compare them against the corresponding baseline cFE code segment
;		CRC if:
;			a) Checksumming (as a whole) is Enabled
;			b) cFE segment checksumming is Enabled
;    CS3006.1	If the cFE code segment CRC is not equal to the baseline cFE
;		code segment CRC, CS shall increment the cFE Code Segment CRC
;		Miscompare Counter and send an event message.
;    CS3007	Upon receipt of an Enable cFE code segment command, CS shall
;		enable checksumming of the cFE code segment.
;    CS3008	Upon receipt of a Disable cFE code segment command, CS shall
;		disable checksumming of the cFE code segment.
;    CS3009	Upon receipt of a Recompute cFE Code Segment CRC command, CS 
;		shall:
;			a) Recompute the baseline checksum for the cFE Code
;			   segment.
;			b) Set the Recompute In Progress Flag to TRUE
;    CS3009.1	Once the baseline CRC is computed, CS shall:
;			a) Generate an event message containing the baseline CRC
;			b) Set the Recompute In Progress Flag to FALSE
;    CS3009.2	If CS is already processing a Recompute CRC command or a One
;		Shot CRC command, CS shall reject the command.
;    CS3010	Upon receipt of a Report cFE Code Segment CRC command, CS shall 
;		send an event message containing the baseline cFE code segment
;		CRC.
;    CS8000	Upon receipt of an Enable Checksum command, CS shall start
;		calculating CRCs and compare them against the baseline CRCs.
;    CS8001	Upon receipt of a Disable Checksum command, CS shall stop
;		calculating CRCs and comparing them against the baseline CRCs.
;    CS9000	CS shall generate a housekeeping message containing the
;		following:
;			a) Valid Ground Command Counter
;			b) Ground Command Rejected Counter
;			c) Overall CRC enable/disable status
;			d) Total Non-Volatile Baseline CRC
;			e) OS code segment Baseline CRC
;			f) cFE code segment Baseline CRC
;			g) Non-Volatile CRC Miscompare Counter
;			h) OS Code Segment CRC Miscompare Counter
;			i) cFE Code Segment CRC Miscompare Counter
;			j) Application CRC Miscompare Counter
;			k) Table CRC Miscompare Counter
;			l) User-Defined Memory CRC Miscompare Counter
;			m) Last One Shot Address
;			n) Last One Shot Size
;			o) Last One Shot Checksum
;			p) Checksum Pass Counter (number of passes thru all of
;			   the checksum areas)
;			q) Current Checksum Region (Non-Volatile, OS code
;			 ) segment, cFE Code Segment etc)
;			r) Non-Volatile CRC enable/disable status
;			s) OS Code Segment CRC enable/disable status
;			t) cFE Code Segment CRC enable/disable status
;			u) Application CRC enable/disable status
;			v) Table CRC enable/disable status
;			w) User-Defined Memory CRC enable/disable status
;			x) Last One Shot Rate
;			y) Recompute In Progress Flag
;			z) One Shot In Progress Flag
;    CS9001     Upon any initialization of the CS Application (cFE Power On, cFE
;               Processor Reset or CS Application Reset), CS shall initialize
;               the following data to Zero:
;			a) Valid Ground Command Counter
;			b) Ground Command Rejected Counter
;			c) Non-Volatile CRC Miscompare Counter
;			d) OS Code Segment CRC Miscompare Counter
;			e) cFE Code Segment CRC Miscompare Counter
;			f) Application CRC Miscompare Counter
;			g) Table CRC Miscompare Counter
;			h) User-Defined Memory CRC Miscompare Counter
;			i) Recompute In Progress Flag
;			j) One Shot In Progress Flag
;
;  Prerequisite Conditions
;	The CFS is up and running and ready to accept commands.
;	The CS commands and telemetry items exist in the GSE database.
;	The display pages exist for the CS Housekeeping and the dump-only
;		Application Code Segment Result Table.
;	The Application Code Segment definition table exists defining the
;		segments to checksum.
;	A CS Test application (TST_CS) exists in order to fully test the CS
;		Application.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	08/25/08	Walt Moleski	Original Procedure.
;       09/22/10        Walt Moleski    Updated to use variables for the CFS
;                                       application name. Replaced all setupevt
;					instances with setupevents
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait	Wait for a specified telemetry point to update to a
;			specified value. 
;       ut_pfindicate	Print the pass fail status of a particular requirement
;			number.
;       ut_setupevents	Performs setup to verify that a particular event
;			message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			requirements array.
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
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "tst_cs_events.h"
#include "tst_tbl_events.h"

%liv (log_procedure) = logging

#define CS_1002		0
#define CS_1003		1
#define CS_1004		2
#define CS_3000		3
#define CS_30001	4
#define CS_3002		5
#define CS_3003		6
#define CS_3004 	7
#define CS_30041	8
#define CS_30042	9
#define CS_3005		10
#define CS_3006		11
#define CS_30061	12
#define CS_3007		13
#define CS_3008		14
#define CS_3009 	15
#define CS_30091 	16
#define CS_30092 	17
#define CS_3010		18
#define CS_8000		19
#define CS_8001		20
#define CS_9000		21
#define CS_9001		22

global ut_req_array_size = 22
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1002", "CS_1003", "CS_1004", "CS_3000", "CS_3000.1", "CS_3002", "CS_3003", "CS_3004", "CS_3004.1", "CS_3004.2", "CS_3005", "CS_3006", "CS_3006.1", "CS_3007", "CS_3008", "CS_3009", "CS_3009.1", "CS_3009.2", "CS_3010", "CS_8000", "CS_8001", "CS_9000", "CS_9001" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream
local CSAppName = "CS"
local hostCPU = "$CPU"

write ";*********************************************************************"
write ";  Step 1.0: Checksum Table Test Setup."
write ";*********************************************************************"
write ";  Step 1.1: Command a Power-on Reset on $CPU."
write ";**********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";**********************************************************************"
page $SC_$CPU_CS_HK
page $SC_$CPU_TST_CS_HK

write ";*********************************************************************"
write ";  Step 1.3: Start the TST_CS_MemTbl application in order to setup    "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_CS_MEMTBL", 1, "INFO", 2

s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")

;;  Wait for app startup event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_CS_MEMTBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_CS_MEMTBL not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_CS_MEMTBL Application start Event Message not received."
endif

;; These are the TST_CS HK Packet IDs since this app sends this packet
;; CPU1 is the default
stream = x'0930'

if ("$CPU" = "CPU2") then
  stream = x'0A30'
elseif ("$CPU" = "CPU3") then
  stream = x'0B30'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5

write ";*********************************************************************"
write ";  Step 1.4: Start the Checksum (CS) and TST_CS applications.        "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("1.4")
wait 5

;; Verify the Housekeeping Packet is being generated
local hkPktId

;; Set the DS HK packet ID based upon the cpu being used
hkPktId = "p0A4"

if ("$CPU" = "CPU2") then
  hkPktId = "p1A4"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2A4"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9000) - Housekeeping packet is being generated."
  ut_setrequirements CS_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements CS_9000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.5: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the CS and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=CSAppName DEBUG
wait 2

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 1.6: Verify that the CS Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Check the HK tlm items to see if they are 0 or NULL
;; the TST_CS application sends its HK packet
if ($SC_$CPU_CS_CMDPC = 0) AND ($SC_$CPU_CS_CMDEC = 0) AND ;;
   ($SC_$CPU_CS_EepromEC = 0) AND ($SC_$CPU_CS_MemoryEC = 0) AND ;;
   ($SC_$CPU_CS_TableEC = 0) AND ($SC_$CPU_CS_AppEC = 0) AND ;;
   ($SC_$CPU_CS_RecomputeInProgress = 0) AND ;;
   ($SC_$CPU_CS_OneShotInProgress = 0) AND ;;
   ($SC_$CPU_CS_CFECoreEC = 0) AND ($SC_$CPU_CS_OSEC = 0) THEN
  write "<*> Passed (9001) - Housekeeping telemetry initialized properly."
  ut_setrequirements CS_9001, "P"
else
  write "<!> Failed (9001) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC       = ",$SC_$CPU_CS_CMDPC
  write "  CMDEC       = ",$SC_$CPU_CS_CMDEC
  write "  EEPROM MC   = ",$SC_$CPU_CS_EEPROMEC
  write "  Memory MC   = ",$SC_$CPU_CS_MemoryEC
  write "  Table MC    = ",$SC_$CPU_CS_TABLEEC
  write "  App MC      = ",$SC_$CPU_CS_AppEC
  write "  cFE Core MC = ",$SC_$CPU_CS_CFECOREEC
  write "  OS MC       = ",$SC_$CPU_CS_OSEC
  ut_setrequirements CS_9001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.0: OS Code Segment Test."
write ";*********************************************************************"
write ";  Step 2.1: Send the Enable Checksum command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_ALL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable All Command
/$SC_$CPU_CS_EnableAll

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8000) - CS EnableALL command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (1003;8000) - CS EnableALL command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8000, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8000) - Expected Event Msg ",CS_ENABLE_ALL_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (1003;8000) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_ALL_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8000, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_State = "Enabled") then
  write "<*> Passed (8000) - Overall CS State set to 'Enabled'."
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (8000) - CS HK did not update the Overall State to 'Enabled'."
  ut_setrequirements CS_8000, "F"
endif

;; Disable all other checksummiing
;; Disable Application Checksumming if it is enabled
if (p@$SC_$CPU_CS_AppState = "Enabled") then
  /$SC_$CPU_CS_DisableApps
  wait 1
endif

;; Disable Eeprom Checksumming if it is enabled
if (p@$SC_$CPU_CS_EepromState = "Enabled") then
  /$SC_$CPU_CS_DisableEeprom
  wait 1
endif

;; Disable User-defined Memory Checksumming if it is enabled
if (p@$SC_$CPU_CS_MemoryState = "Enabled") then
  /$SC_$CPU_CS_DisableMemory
  wait 1
endif

;; Disable Table Checksumming if it is enabled
if (p@$SC_$CPU_CS_TableState = "Enabled") then
  /$SC_$CPU_CS_DisableTables
  wait 1
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send the Enable OS Code Segment command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable OS Command
/$SC_$CPU_CS_EnableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - CS EnableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - CS EnableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - Expected Event Msg ",CS_ENABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_OSState = "Enabled") then
  write "<*> Passed (3002) - OS State set to 'Enabled'."
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (3002) - CS HK did not update the OS State to 'Enabled'."
  ut_setrequirements CS_3002, "F"
endif

write ";*********************************************************************"
write ";  Step 2.3: Verify that CRC calculations are happening."
write ";*********************************************************************"
;; Wait until at least one complete Background Checksum pass has occurred
ut_tlmwait $SC_$CPU_CS_PASSCTR, 1, 300

if ($SC_$CPU_CS_OSBASELINE <> 0) then
  write "<*> Passed (3000) - Checksumming is occurring."
  ut_setrequirements CS_3000, "P"
else
  write "<!> Failed (3000) - OS Checksum calculation timed-out."
  ut_setrequirements CS_3000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4: Send the Disable OS Code Segment command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable OS Command
/$SC_$CPU_CS_DisableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - CS DisableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - CS DisableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - Expected Event Msg ",CS_DISABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_OSState = "Disabled") then
  write "<*> Passed (3003) - OS State set to 'Disabled'."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - CS HK did not update the OS State to 'Disabled'."
  ut_setrequirements CS_3003, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5: Using the TST_CS application, manipulate the OS CRC.     "
write ";*********************************************************************"
;; Send a TST_CS command to do this using tblName as the argument
ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_OS_CRC_INF_EID, "INFO", 1

/$SC_$CPU_TST_CS_CorruptOSCRC

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send the Enable OS command.                                "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable OS Command
/$SC_$CPU_CS_EnableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - CS EnableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - CS EnableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - Expected Event Msg ",CS_ENABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_OSState = "Enabled") then
  write "<*> Passed (3002) - OS State set to 'Enabled'."
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (3002) - CS HK did not update the OS State to 'Enabled'."
  ut_setrequirements CS_3002, "F"
endif

;; wait until an OS Miscompare error message appears
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_OS_MISCOMPARE_ERR_EID, "ERROR", 1

;; Wait for the OS Miscompare event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg ",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
  ut_setrequirements CS_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_OS_MISCOMPARE_ERR_EID,"."
  ut_setrequirements CS_30001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.7: Send the Disable OS Code Segment command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable OS Command
/$SC_$CPU_CS_DisableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - CS DisableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - CS DisableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - Expected Event Msg ",CS_DISABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_OSState = "Disabled") then
  write "<*> Passed (3003) - OS State set to 'Disabled'."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - CS HK did not update the OS State to 'Disabled'."
  ut_setrequirements CS_3003, "F"
endif

write ";*********************************************************************"
write ";  Step 2.8: Send the Recompute OS Code Segment command.              "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_OS_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - CS RecomputeOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - CS RecomputeOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Verify the telemetry flag is set to TRUE (3004)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3004) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (3004) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #2
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3004.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30041, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3004.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30041, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Send the Report OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportOS Command
/$SC_$CPU_CS_ReportOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - CS ReportOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - CS ReportOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - Expected Event Msg ",CS_BASELINE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: cFE Code Segment Test."
write ";*********************************************************************"
write ";  Step 3.1: Send the Enable Checksum command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_ALL_INF_EID, "INFO", 1

local cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable All Command
/$SC_$CPU_CS_EnableAll

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8000) - CS EnableALL command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (1003;8000) - CS EnableALL command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8000, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8000) - Expected Event Msg ",CS_ENABLE_ALL_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (1003;8000) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_ALL_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8000, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_State = "Enabled") then
  write "<*> Passed (8000) - Overall CS State set to 'Enabled'."
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (8000) - CS HK did not update the Overall State to 'Enabled'."
  ut_setrequirements CS_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.2: Send the Enable cFE Code Segment command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable cFE Core Command
/$SC_$CPU_CS_EnableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - CS EnableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - CS EnableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Enabled") then
  write "<*> Passed (3007) - cFE Core State set to 'Enabled'."
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (3007) - CS HK did not update the cFE Core State to 'Enabled'."
  ut_setrequirements CS_3007, "F"
endif

wait 5

;; Disable OS Code Segment checksumming
cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable OS Command
/$SC_$CPU_CS_DisableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - CS DisableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - CS DisableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3: Verify that CRC calculations are happening."
write ";*********************************************************************"
;; Wait until the cFE Baseline is calculated
local waitForCFECRC = TRUE

while (waitForCFECRC = TRUE) do
  if ($SC_$CPU_CS_CFECOREBASELINE <> 0) then
    waitForCFECRC = FALSE
  else
    wait 10
  endif
enddo

write "<*> Passed (3006) - Checksumming is occurring."
ut_setrequirements CS_3006, "P"

write ";*********************************************************************"
write ";  Step 3.4: Send the Disable cFE Code Segment command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable CFECore Command
/$SC_$CPU_CS_DisableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - CS DisableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - CS DisableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3008, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3008, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Disabled") then
  write "<*> Passed (3008) - cFE Core State set to 'Disabled'."
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (3008) - CS HK did not update the cFE Core State to 'Disabled'."
  ut_setrequirements CS_3008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Using the TST_CS application, manipulate the cFE CRC.     "
write ";*********************************************************************"
;; Send a TST_CS command to do this using tblName as the argument
ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 1

/$SC_$CPU_TST_CS_CorruptCFECRC

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.6: Send the Enable cFE command.                               "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable CFECore Command
/$SC_$CPU_CS_EnableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - CS EnableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - CS EnableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Enabled") then
  write "<*> Passed (3007) - cFE Core State set to 'Enabled'."
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (3007) - CS HK did not update the cFE Core State to 'Enabled'."
  ut_setrequirements CS_3007, "F"
endif

wait 5

;; wait until a cFE Miscompare error message appears
ut_setupevents "$SC","$CPU",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 1

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3006.1) - Expected Event Msg ",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
  ut_setrequirements CS_30061, "P"
else
  write "<!> Failed (3006.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_OS_MISCOMPARE_ERR_EID,"."
  ut_setrequirements CS_30061, "F"
endif

write ";*********************************************************************"
write ";  Step 3.7: Send the Recompute cFE Code Segment command.              "
write ";*********************************************************************"
;; Send the Disable CFECore Command
/$SC_$CPU_CS_DisableCFECore
wait 5

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - CS RecomputeCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - CS RecomputeCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Verify the telemetry flag is set to TRUE (3009)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3009) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (3009) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 230
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3009.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (1003;3009.1) - RecomputeCFECore failed to complete in the time allowed. Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30091, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3009.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3009.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.8: Send the Report cFE Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_BASELINE_CFECORE_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportCFECore Command
/$SC_$CPU_CS_ReportCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - CS ReportCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - CS ReportCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Invalid Command Test."
write ";*********************************************************************"
write ";  Step 4.1: Send the Enable OS Code Segment command with an invalid  "
write ";  length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000020AB0"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000020AB0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000020AB0"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Send the Disable OS Code Segment command with an invalid "
write ";  length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000020BB1"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000020BB1"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000020BB1"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: Send the Recompute OS Code Segment CRC command with an   "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000020DB5"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000020DB5"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000020DB5"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4: Send the Report OS Code Segment CRC command with an      "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000020CB8"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000020CB8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000020CB8"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5: Send the Enable cFE Code Segment command with an invalid "
write ";  length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000000206BC"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc000000206BC"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc000000206BC"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6: Send the Disable cFE Code Segment command with an invalid"
write ";  length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000000207BD"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc000000207BD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc000000207BD"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Send the Recompute cFE Code Segment CRC command with an  "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000000209B0"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc000000209B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc000000209B0"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.8: Send the Report cFE Code Segment CRC command with an      "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000000208B4"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc000000208B4"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc000000208B4"
endif

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CS_1002, "P"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CS_1002, "F"
  ut_setrequirements CS_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9: Send the Recompute OS Code Segment command.              "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_OS_STARTED_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - CS RecomputeOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - CS RecomputeOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Verify the telemetry flag is set to TRUE (3004)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3004) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (3004) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.10: Send the Recompute OS Code Segment command again. This  "
write ";  command should fail since there is already a recompute active.     "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_OS_CHDTASK_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3004.2) - CS RecomputeOS command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_30042, "P"
else
  write "<!> Failed (1004;3004.2) - CS RecomputeOS command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_30042, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_OS_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.11: Send the Recompute cFE Code Segment command again. This "
write ";  command should fail since there is already a recompute active.     "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3009.2) - CS RecomputeCFECore command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_30092, "P"
else
  write "<!> Failed (1004;3009.2) - CS RecomputeCFECore command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_30092, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.12: Send the One Shot CRC command. This should fail since "
write ";  there is already a recompute active.     "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU",{CSAppName},CS_ONESHOT_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3009.2) - One Shot CRC command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_30092, "P"
else
  write "<!> Failed (1004;3009.2) - One Shot CRC command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_30092, "F"
endif

;; Need to wait until the RecomputeCFE task is completed.
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RecomputeCFE task completed."
else
  write "<!> Failed - RecomputeCFE failed to complete in the time allowed."
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3009.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3009.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30091, "F"
endif

write ";*********************************************************************"
write ";  Step 4.13: Send the Recompute cFE Code Segment command.              "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - CS RecomputeCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - CS RecomputeCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Verify the telemetry flag is set to TRUE (3009)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3009) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (3009) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3009, "F"
endif

write ";*********************************************************************"
write ";  Step 4.14: Send the Recompute cFE Code Segment command again. This "
write ";  command should fail since there is already a recompute active.     "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3009.2) - CS RecomputeCFECore command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_30092, "P"
else
  write "<!> Failed (1004;3009.2) - CS RecomputeCFECore command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_30092, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.15: Send the Recompute OS Code Segment command again. This  "
write ";  command should fail since there is already a recompute active.     "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_OS_CHDTASK_ERR_EID,"ERROR", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"INFO", 2

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3004.2) - CS RecomputeOS command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_30042, "P"
else
  write "<!> Failed (1004;3004.2) - CS RecomputeOS command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_30042, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_OS_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

write ";*********************************************************************"
write ";  Step 4.16: Send the One Shot CRC command. This should fail since "
write ";  there is already a recompute active.     "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3004.2) - One Shot CRC command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_30042, "P"
else
  write "<!> Failed (1004;3004.2) - One Shot CRC command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_30042, "F"
endif

;; Need to wait until the RecomputeOS task is completed.
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RecomputeOS task completed."
else
  write "<!> Failed - RecomputeOS failed to complete in the time allowed."
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3004.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30041, "F"
endif

write ";*********************************************************************"
write ";  Step 5.0: Disabled Checksum Test."
write ";*********************************************************************"
write ";  Step 5.1: Send the Disable Checksum command.                       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_ALL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable All Command
/$SC_$CPU_CS_DisableAll

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8001) - CS DisableALL command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8001, "P"
else
  write "<!> Failed (1003;8001) - CS DisableALL command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8001) - Expected Event Msg ",CS_DISABLE_ALL_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8001, "P"
else
  write "<!> Failed (1003;8001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_ALL_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8001, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_State = "Disabled") then
  write "<*> Passed (8000) - Overall CS State set to 'Disabled'."
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (8000) - CS HK did not update the State to 'Disabled'."
  ut_setrequirements CS_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 5.2: Send the Disable OS Code Segment command.                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable OS code segment Command
/$SC_$CPU_CS_DisableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - CS DisableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - CS DisableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - Expected Event Msg ",CS_DISABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_OSState = "Disabled") then
  write "<*> Passed (3003) - OS State set to 'Disabled'."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - CS HK did not update the OS State to 'Disabled'."
  ut_setrequirements CS_3003, "F"
endif
 
write ";**********************************************************************"
write ";  Step 5.3: Send the Recompute OS Code Segment command.               "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_OS_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - CS RecomputeOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - CS RecomputeOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Verify the telemetry flag is set to TRUE (3004)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3004) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (3004) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3004.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (1003;3004.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30041, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3004.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30041, "F"
endif

write ";*********************************************************************"
write ";  Step 5.4: Send the Report OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportOS Command
/$SC_$CPU_CS_ReportOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - CS ReportOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - CS ReportOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - Expected Event Msg ",CS_BASELINE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5: Send the Enable OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable OS Command
/$SC_$CPU_CS_EnableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - CS EnableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - CS EnableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - Expected Event Msg ",CS_ENABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_OSState = "Enabled") then
  write "<*> Passed (3003) - OS State set to 'Enabled'."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - CS HK did not update the OS State to 'Enabled'."
  ut_setrequirements CS_3003, "F"
endif

write ";**********************************************************************"
write ";  Step 5.6: Send the Recompute OS Code Segment command.               "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_OS_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - CS RecomputeOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - CS RecomputeOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Verify the telemetry flag is set to TRUE (3004)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3004) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (3004) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3004.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (1003;3004.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30041, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3004.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30041, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.7: Send the Report OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportOS Command
/$SC_$CPU_CS_ReportOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - CS ReportOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - CS ReportOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - Expected Event Msg ",CS_BASELINE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8: Send the Disable cFE Code Segment command.                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable CFECore Command
/$SC_$CPU_CS_DisableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - CS DisableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - CS DisableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3008, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3008, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Disabled") then
  write "<*> Passed (3008) - cFE Core State set to 'Disabled'."
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (3008) - CS HK did not update the cFE Core State to 'Disabled'."
  ut_setrequirements CS_3008, "F"
endif

write ";**********************************************************************"
write ";  Step 5.9: Send the Recompute cFE Code Segment command.               "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - CS RecomputeCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - CS RecomputeCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Verify the telemetry flag is set to TRUE (3009)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3009) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (3009) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3009.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (1003;3009.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30091, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3009.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3009.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.10: Send the Report cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_BASELINE_CFECORE_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportCFECore Command
/$SC_$CPU_CS_ReportCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - CS ReportCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - CS ReportCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.11: Send the Enable cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable cFE Core Command
/$SC_$CPU_CS_EnableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - CS EnableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - CS EnableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Enabled") then
  write "<*> Passed (3007) - cFE Core State set to 'Enabled'."
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (3007) - CS HK did not update the cFE Core State to 'Enabled'."
  ut_setrequirements CS_3007, "F"
endif

write ";**********************************************************************"
write ";  Step 5.12: Send the Recompute cFE Code Segment command.             "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - CS RecomputeCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - CS RecomputeCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Verify the telemetry flag is set to TRUE (3009)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3009) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (3009) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3009.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (1003;3009.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30091, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3009.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3009.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.13: Send the Report cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_BASELINE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportCFECore Command
/$SC_$CPU_CS_ReportCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - CS ReportCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - CS ReportCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.14: Send the Enable Checksum command.                       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_ALL_INF_EID, "INFO", 1

local cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable All Command
/$SC_$CPU_CS_EnableAll

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8000) - CS EnableALL command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (1003;8000) - CS EnableALL command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8000, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8000) - Expected Event Msg ",CS_ENABLE_ALL_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (1003;8000) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_ALL_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8000, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_State = "Enabled") then
  write "<*> Passed (8000) - Overall State set to 'Enabled'."
  ut_setrequirements CS_8000, "P"
else
  write "<!> Failed (8000) - CS HK did not update the CS State to 'Enabled'."
  ut_setrequirements CS_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 5.15: Send the Disable OS Code Segment command.                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable OS code segment Command
/$SC_$CPU_CS_DisableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - CS DisableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - CS DisableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - Expected Event Msg ",CS_DISABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;3003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3003, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_OSState = "Disabled") then
  write "<*> Passed (3003) - OS State set to 'Disabled'."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - CS HK did not update the OS State to 'Disabled'."
  ut_setrequirements CS_3003, "F"
endif

write ";**********************************************************************"
write ";  Step 5.16: Send the Recompute OS Code Segment command.               "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_OS_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - CS RecomputeOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - CS RecomputeOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Verify the telemetry flag is set to TRUE (3004)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3004) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (3004) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3004.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30041, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3004.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30041, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.17: Send the Report OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportOS Command
/$SC_$CPU_CS_ReportOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - CS ReportOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - CS ReportOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - Expected Event Msg ",CS_BASELINE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.18: Send the Enable OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable OS Command
/$SC_$CPU_CS_EnableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - CS EnableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - CS EnableOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3002) - Expected Event Msg ",CS_ENABLE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3002, "P"
else
  write "<!> Failed (1003;3002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3002, "F"
endif

wait 5

;; Check the HK State 
if (p@$sc_$cpu_CS_OSState = "Enabled") then
  write "<*> Passed (3003) - OS State set to 'Enabled'."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - CS HK did not update the OS State to 'Enabled'."
  ut_setrequirements CS_3003, "F"
endif

write ";**********************************************************************"
write ";  Step 5.19: Send the Recompute OS Code Segment command.               "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_OS_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - CS RecomputeOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - CS RecomputeOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3004) - Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (1003;3004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_OS_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3004, "F"
endif

;; Verify the telemetry flag is set to TRUE (3004)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3004) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3004, "P"
else
  write "<!> Failed (3004) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3004, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3004.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30041, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3004.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30041, "P"
else
  write "<!> Failed (3004.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30041, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.20: Send the Report OS Code Segment command.                 "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportOS Command
/$SC_$CPU_CS_ReportOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - CS ReportOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - CS ReportOS command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3005) - Expected Event Msg ",CS_BASELINE_OS_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3005, "P"
else
  write "<!> Failed (1003;3005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_OS_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.21: Send the Disable cFE Code Segment command.                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable CFECore Command
/$SC_$CPU_CS_DisableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - CS DisableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - CS DisableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3008, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3008, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Disabled") then
  write "<*> Passed (3008) - cFE Core State set to 'Disabled'."
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (3008) - CS HK did not update the cFE Core State to 'Disabled'."
  ut_setrequirements CS_3008, "F"
endif

write ";**********************************************************************"
write ";  Step 5.22: Send the Recompute cFE Code Segment command.             "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - CS RecomputeCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - CS RecomputeCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Verify the telemetry flag is set to TRUE (3009)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3009) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (3009) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3009.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (1003;3009.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30091, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3009.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3009.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.23: Send the Report cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportCFECore Command
/$SC_$CPU_CS_ReportCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - CS ReportCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - CS ReportCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.24: Send the Enable cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable cFE Core Command
/$SC_$CPU_CS_EnableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - CS EnableCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - CS EnableCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3007) - Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (1003;3007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3007, "F"
endif

wait 5

;; Check the HK status field
if (p@$sc_$cpu_CS_CFECoreState = "Enabled") then
  write "<*> Passed (3007) - cFE Core State set to 'Enabled'."
  ut_setrequirements CS_3007, "P"
else
  write "<!> Failed (3007) - CS HK did not update the cFE Core State to 'Enabled'."
  ut_setrequirements CS_3007, "F"
endif

write ";**********************************************************************"
write ";  Step 5.25: Send the Recompute cFE Code Segment command.             "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - CS RecomputeCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - CS RecomputeCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #1
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3009) - Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (1003;3009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3009, "F"
endif

;; Verify the telemetry flag is set to TRUE (3009)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (3009) - In Progress Flag set to True as expected."
  ut_setrequirements CS_3009, "P"
else
  write "<!> Failed (3009) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_3009, "F"
endif

;; Check for the event message #2
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3009.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_30091, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3009.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (3009.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_30091, "P"
else
  write "<!> Failed (3009.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_30091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.26: Send the Report cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_BASELINE_CFECORE_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportCFECore Command
/$SC_$CPU_CS_ReportCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - CS ReportCFECore command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - CS ReportCFECore command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3010) - Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3010, "P"
else
  write "<!> Failed (1003;3010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_CFECORE_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_3010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 6.1: Send the Power-On Reset command. "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
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

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_corecode"
write ";*********************************************************************"
ENDPROC
