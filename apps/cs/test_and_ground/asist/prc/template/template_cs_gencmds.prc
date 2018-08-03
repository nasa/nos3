PROC $sc_$cpu_cs_gencmds
;*******************************************************************************
;  Test Name:  cs_gencmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) general commands function
;	properly. The NOOP, Reset Counters, Enable/Disable Checksum, and One
;	Shot commands will be tested. Invalid versions of these commands will
;	also be tested to ensure that the CS application handled these properly.
;
;  Requirements Tested
;    CS1000	Upon receipt of a No-Op command, CS shall increment the CS
;		Valid Command Counter and generate an event message.
;    CS1001	Upon receipt of a Reset command, CS shall reset the following
;		housekeeping variables to a value of zero:
;			a) Valid Ground Command Counter
;			b) Ground Command Rejected Counter
;			c) Non-volatile CRC Miscompare Counter
;			d) OS Code Segment CRC Miscompare Counter
;			e) cFE Code Segment CRC Miscompare Counter
;			f) Application CRC Miscompare Counter
;			g) Table CRC Miscompare Counter
;			h) User-defined Memory CRC Miscompare Counter
;			i) Checksum Pass Counter (number of passes through all
;			   of the checksum areas)
;    CS1002	For all CS commands, if the length contained in the message
;		header is not equal to the expected length, CS shall reject the
;		command and issue an event message.
;    CS1003	If CS accepts any command as valid, CS shall execute the
;		command, increment the CS Valid Command Counter and issue an
;		event message.
;    CS1004	If CS rejects any command, CS shall abort the command execution,
;		increment the CS Command Rejected Counter and issue an event
;		message.
;    CS1005	CS shall use the <PLATFORM_DEFINED> CRC algorithm to compute
;		the CRCs for any segment.
;    CS8000	Upon receipt of an Enable Checksum command, CS shall start
;		calculating CRCs and compare them against the baseline CRCs.
;    CS8001	Upon receipt of a Disable Checksum command, CS shall stop
;		calculating CRCs and comparing them against the baseline CRCs.
;    CS8002	Upon receipt of a One Shot command, CS shall:
;			a) Calculate the CRC starting at the command-specified
;			   address for the command-specified bytes at the
;			   command-specified rate (Max Bytes Per Cycle).
;			b) Set the One Shot In Progress Flag to TRUE
;    CS8002.1	Once the CRC is computed, CS shall:
;			a) Issue an event message containing the CRC
;			b) Set the One Shot In Progress Flag to FALSE
;    CS8002.2	If CS is already processing a One Shot CRC command or a
;		Recompute CRC command, CS shall reject the command.
;    CS8002.3	If the command-specified rate is zero, CS shall calculate the
;		CRC at the <PLATFORM_DEFINED> rate (Max Bytes Per Cycle).
;    CS8003	Upon receipt of a Cancel One Shot command, CS shall stop the
;		current One Shot calculation.
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
;			   segment, cFE Code Segment etc)
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
;	Date		   Name		Description
;	08/27/08	Walt Moleski	Original Procedure.
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
#include "cs_tbldefs.h"
#include "tst_cs_events.h"

%liv (log_procedure) = logging

#define CS_1000		0
#define CS_1001		1
#define CS_1002		2
#define CS_1003		3
#define CS_1004		4
#define CS_1005		5
#define CS_8000		6
#define CS_8001		7
#define CS_8002		8
#define CS_80021	9
#define CS_80022	10
#define CS_80023	11
#define CS_8003		12
#define CS_9000		13
#define CS_9001		14

global ut_req_array_size = 14
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1000", "CS_1001", "CS_1002", "CS_1003", "CS_1004", "CS_1005", "CS_8000", "CS_8001", "CS_8002", "CS_8002.1", "CS_8002.2", "CS_8002.3", "CS_8003", "CS_9000", "CS_9001" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
local CSAppName = "CS"
local hostCPU = "$CPU"

write ";***********************************************************************"
write ";  Step 1.0: Checksum Table Test Setup."
write ";***********************************************************************"
write ";  Step 1.1: Command a Power-on Reset on $CPU."
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
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

/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 5

write ";*********************************************************************"
write ";  Step 1.4: Create & upload the EEPROM Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.5: Create & upload the Memory Definition Table file to be  "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_mdt5
wait 5

;; Upload the file created above as the default
s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
wait 10


write ";***********************************************************************"
write ";  Step 1.6:  Start the Checksum (CS) and Test Applications.            "
write ";***********************************************************************"
s $sc_$cpu_cs_start_apps("1.6")
wait 5

;; Verify the Housekeeping Packet is being generated
;; Set the DS HK packet ID based upon the cpu being used
local hkPktId = "p0A4"

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

write ";***********************************************************************"
write ";  Step 1.7: Enable DEBUG Event Messages "
write ";***********************************************************************"
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

write ";***********************************************************************"
write ";  Step 1.8: Verify that the CS Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";***********************************************************************"
;; Check the HK tlm items to see if they are 0
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

write ";***********************************************************************"
write ";  Step 1.9: Set Requirement 1005 to Analysis since the CRC algorithm   "
write ";  cannot be determined by this procedure. "
write ";***********************************************************************"
ut_setrequirements CS_1005, "A"

write ";***********************************************************************"
write ";  Step 2.0: Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Send the NO-OP command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_NOOP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the NO-OP Command
/$SC_$CPU_CS_NOOP

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - CS NO-OP command sent properly."
  ut_setrequirements CS_1000, "P"
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1000;1003) - CS NO-OP command did not increment CMDPC."
  ut_setrequirements CS_1000, "F"
  ut_setrequirements CS_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000) - Expected Event Msg ",CS_NOOP_INF_EID," rcv'd."
  ut_setrequirements CS_1000, "P"
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1000) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_NOOP_INF_EID,"."
  ut_setrequirements CS_1000, "F"
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the NO-OP command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc000000200B0"

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

write ";***********************************************************************"
write ";  Step 2.3: Utilizing the TST_CS application, send the command that  "
write ";  will set all the counters that get reset to zero (0) by the Reset  "
write ";  command to a non-zero value."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_SET_COUNTERS_INF_EID, "INFO", 1

/$SC_$CPU_TST_CS_SetCounters

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_SET_COUNTERS_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_SET_COUNTERS_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Verify that all the counters are non-zero and send the   "
write ";  Reset command if so.                                               "
write ";***********************************************************************"
;; Check the HK telemetry
if ($SC_$CPU_CS_CMDPC > 0) AND ($SC_$CPU_CS_CMDEC > 0) AND ;;
   ($SC_$CPU_CS_EepromEC > 0) AND ($SC_$CPU_CS_MemoryEC > 0) AND ;;
   ($SC_$CPU_CS_TableEC > 0) AND ($SC_$CPU_CS_AppEC > 0) AND ;;
   ($SC_$CPU_CS_CFECoreEC > 0) AND ($SC_$CPU_CS_OSEC > 0) AND ;;
   ($SC_$CPU_CS_PASSCTR > 0) THEN
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RESET_DBG_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_CS_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_CS_ResetCtrs
  wait 5

  ;; Check for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",CS_RESET_DBG_EID," rcv'd."
    ut_setrequirements CS_1001, "P"
    ut_setrequirements CS_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RESET_DBG_EID,"."
    ut_setrequirements CS_1001, "F"
    ut_setrequirements CS_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_CS_CMDPC = 0) AND ($SC_$CPU_CS_CMDEC = 0) AND ;;
     ($SC_$CPU_CS_EepromEC = 0) AND ($SC_$CPU_CS_MemoryEC = 0) AND ;;
     ($SC_$CPU_CS_TableEC = 0) AND ($SC_$CPU_CS_AppEC = 0) AND ;;
     ($SC_$CPU_CS_CFECoreEC = 0) AND ($SC_$CPU_CS_OSEC = 0) AND ;;
     ($SC_$CPU_CS_PASSCTR = 0) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements CS_1001, "P"
  else
    write "<!> Failed (1001) - Counters did not reset to zero."
    ut_setrequirements CS_1001, "F"
  endif
else
  write "<!> Reset command not sent because at least 1 counter is set to 0."
endif

;; Write out the counters for verification
write "CMDPC     = ", $SC_$CPU_CS_CMDPC
write "CMDEC     = ", $SC_$CPU_CS_CMDEC
write "EEPROMEC  = ", $SC_$CPU_CS_EepromEC
write "MemoryEC  = ", $SC_$CPU_CS_MemoryEC
write "TableEC   = ", $SC_$CPU_CS_TableEC
write "AppEC     = ", $SC_$CPU_CS_AppEC
write "CFECoreEC = ", $SC_$CPU_CS_CFECoreEC
write "OSEC      = ", $SC_$CPU_CS_OSEC
write "PassCtr   = ", $SC_$CPU_CS_PASSCTR

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Reset command with an invalid length.             "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc000000201B0"

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

write ";***********************************************************************"
write ";  Step 2.6: Send the Disable Checksum command.                         "
write ";***********************************************************************"
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
  write "<*> Passed (8001) - Overall CS State set to 'Disabled'."
  ut_setrequirements CS_8001, "P"
else
  write "<!> Failed (8001) - CS HK did not update the State to 'Disabled'."
  ut_setrequirements CS_8001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.7: Send the Disable Checksum command with an invalid length.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc000000205BE"

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

write ";***********************************************************************"
write ";  Step 2.8: Send a One Shot command with a valid address and size.     "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_FINISHED_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=32

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - CS One Shot command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - CS One Shot command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Verify the telemetry flag is set to TRUE (8002)
if (p@$SC_$CPU_CS_OneShotInProgress = "True") then
  write "<*> Passed (8002) - In Progress Flag set to True as expected."
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (8002) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_8002, "F"
endif

;; Check for the finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8002.1) - Expected Event Msg ",CS_ONESHOT_FINISHED_INF_EID," rcv'd."
  ut_setrequirements CS_80021, "P"
else
  write "<!> Failed (8002.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_FINISHED_INF_EID,"."
  ut_setrequirements CS_80021, "F"
endif

;; Wait for the next HK packet
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_OneShotInProgress = "False") then
  write "<*> Passed (8002.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_80021, "P"
else
  write "<!> Failed (8002.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_80021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Send the Enable Checksum command.                         "
write ";***********************************************************************"
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

write ";***********************************************************************"
write ";  Step 2.10: Send the Enable Checksum command with an invalid length.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc000000204BF"

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

write ";***********************************************************************"
write ";  Step 2.11: Send a One Shot command with a valid address and size.    "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_FINISHED_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_STARTADDR[1]+10 RegionSize=2048 MaxBytes=16

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - CS One Shot command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - CS One Shot command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Verify the telemetry flag is set to TRUE (8002)
if (p@$SC_$CPU_CS_OneShotInProgress = "True") then
  write "<*> Passed (8002) - In Progress Flag set to True as expected."
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (8002) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_8002, "F"
endif

;; Check for the finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8002.1) - Expected Event Msg ",CS_ONESHOT_FINISHED_INF_EID," rcv'd."
  ut_setrequirements CS_80021, "P"
else
  write "<!> Failed (8002.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_FINISHED_INF_EID,"."
  ut_setrequirements CS_80021, "F"
endif

;; Wait for the next HK packet
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_OneShotInProgress = "False") then
  write "<*> Passed (8002.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_80021, "P"
else
  write "<!> Failed (8002.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_80021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Send a One Shot command with an invalid length.           "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc000000A02B0"

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

write ";***********************************************************************"
write ";  Step 2.13: Send a One Shot command with arguments that go beyond the "
write ";  memory region. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_MEMVALIDATE_ERR_EID,"ERROR",1

local startAddr = $SC_$CPU_TST_CS_STARTADDR[1] + $SC_$CPU_TST_CS_SIZE[1]
startAddr = startAddr - 1000

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=startAddr RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - CS One Shot command sent properly."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - CS One Shot command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_ONESHOT_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.14: Send a One Shot command with a valid address and a very   "
write ";  large but valid size.    "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_STARTED_DBG_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_STARTADDR[1] RegionSize=$SC_$CPU_TST_CS_SIZE[1] MaxBytes=0

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - CS One Shot command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - CS One Shot command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Verify the telemetry flag is set to TRUE (8002)
if (p@$SC_$CPU_CS_OneShotInProgress = "True") then
  write "<*> Passed (8002) - In Progress Flag set to True as expected."
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (8002) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_8002, "F"
endif

;; Verify the Rate is set to the platform-defined value (8002.3)
if ($SC_$CPU_CS_LastOneShotRate = CS_DEFAULT_BYTES_PER_CYCLE) then
  write "<*> Passed (8002.3) - One shot rate is set as expected."
  ut_setrequirements CS_80023, "P"
else
  write "<!> Failed (8002.3) - One shot rate set to '",$SC_$CPU_CS_LastOneShotRate,"'. Expected '",CS_DEFAULT_BYTES_PER_CYCLE,"'"
  ut_setrequirements CS_80023, "F"
endif

write ";***********************************************************************"
write ";  Step 2.15: Before the above command completes, send a Cancel One Shot"
write ";  command.   "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_CANCELLED_INF_EID,"INFO", 1

local cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Cancel One Shot Command
/$SC_$CPU_CS_CancelOneShot
  
ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8003) - CS Cancel One Shot command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8003, "P"
else
  write "<!> Failed (1003;8003) - CS Cancel One Shot command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8003) - Expected Event Msg ",CS_ONESHOT_CANCELLED_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8003, "P"
else
  write "<!> Failed (1003;8003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_CANCELLED_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.16: Send a Cancel One Shot command with an invalid length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc000000203B0"

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

write ";***********************************************************************"
write ";  Step 2.17: Send a One Shot command with a valid address and a very   "
write ";  large but valid size.    "
write ";***********************************************************************"

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_STARTADDR[2] RegionSize=$SC_$CPU_TST_CS_SIZE[2] MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - CS One Shot command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - CS One Shot command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

write ";***********************************************************************"
write ";  Step 2.18: Send a One Shot command while a child task is already "
write ";  running. An error event should be generated "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_CHDTASK_ERR_EID, "ERROR", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_FINISHED_INF_EID, "INFO", 2

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_STARTADDR[1] RegionSize=$SC_$CPU_TST_CS_SIZE[1] MaxBytes=0

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;8002.2) - CS One Shot command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_80022, "P"
else
  write "<!> Failed (1004;8002.2) - CS One Shot command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_80022, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_ONESHOT_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1003;8002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

write ";***********************************************************************"
write ";  Step 2.19: Send a Recompute command while a child task is already "
write ";  running. An error event should be generated. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=CSAppName

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;8002.2) - CS One Shot command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_80022, "P"
else
  write "<!> Failed (1004;8002.2) - CS One Shot command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_80022, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_ONESHOT_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1003;8002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

;; Send the Cancel One Shot Command
/$SC_$CPU_CS_CancelOneShot
wait 5

write ";***********************************************************************"
write ";  Step 2.20: Send a Cancel One Shot command when there is no One Shot  "
write ";  command executing.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Cancel One Shot Command
/$SC_$CPU_CS_CancelOneShot
 
ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - CS Cancel One Shot command failed as expected."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - CS Cancel One Shot command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.21: Send an invalid command.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_CC1_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "189Fc0000001AA"

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
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_MID_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.22: Send the CS Housekeeping Request with an invalid length."
write ";  Since this is an internal command, the CMDEC SHOULD NOT increment."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC
  
;; CPU1 is the default
rawcmd = "18A0c00000020000"

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

;; Verify the CMDEC did not increment
if (errcnt = $SC_$CPU_CS_CMDEC) then
  write "<*> Passed - CMDEC remained the same."
else
  write "<!> Failed - CMDEC incremented when it was not expected."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.23: Send the CS Background Cycle Request with an invalid "
write ";  length. Since this is an internal command, the CMDEC SHOULD NOT "
write ";  increment."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC
  
;; CPU1 is the default
rawcmd = "18A1c00000020000"

ut_sendrawcmd "$SC_$CPU_CS", (rawcmd)

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CS_LEN_ERR_EID, "."
  ut_setrequirements CS_1004, "F"
endif

;; Verify the CMDEC did not increment
if (errcnt = $SC_$CPU_CS_CMDEC) then
  write "<*> Passed - CMDEC remained the same."
else
  write "<!> Failed - CMDEC incremented when it was not expected."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.24: Send a One Shot command with a valid address and a very   "
write ";  large but valid size and MaxBytes that is not 8 bit aligned.    "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_FINISHED_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_STARTADDR[2] RegionSize=2048 MaxBytes=6

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - CS One Shot command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - CS One Shot command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;8002) - Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (1003;8002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_8002, "F"
endif

;; Verify the telemetry flag is set to TRUE (8002)
if (p@$SC_$CPU_CS_OneShotInProgress = "True") then
  write "<*> Passed (8002) - In Progress Flag set to True as expected."
  ut_setrequirements CS_8002, "P"
else
  write "<!> Failed (8002) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_8002, "F"
endif

;; Check for the finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 400
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8002.1) - Expected Event Msg ",CS_ONESHOT_FINISHED_INF_EID," rcv'd."
  ut_setrequirements CS_80021, "P"
else
  write "<!> Failed (8002.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_FINISHED_INF_EID,"."
  ut_setrequirements CS_80021, "F"
endif

;; Wait for the next HK packet
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (3004.1)
if (p@$SC_$CPU_CS_OneShotInProgress = "False") then
  write "<*> Passed (8002.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_80021, "P"
else
  write "<!> Failed (8002.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_80021, "F"
endif

write ";*********************************************************************"
write ";  Step 3.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 3.1: Send the Power-On Reset command. "
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
write ";  End procedure $SC_$CPU_cs_gencmds"
write ";*********************************************************************"
ENDPROC
