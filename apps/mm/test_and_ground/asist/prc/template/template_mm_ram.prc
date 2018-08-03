PROC $sc_$cpu_mm_ram
;*******************************************************************************
;  Test Name:  mm_ram
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Memory Manager (MM) Random Access Memory
;	(RAM) commands function properly and that the MM application handles 
;	anomalies appropriately.
;
;  Requirements Tested
;    MM1006	For all MM commands, if the length contained in the message
;		header is not equal to the expected length,  MM shall reject the
;		command.
;    MM1007	If the address specified in any MM command fails validation, MM
;		shall reject the command.
;    MM1008	If the filename specified in any MM command is not valid, MM
;		shall reject the command.
;    MM1009	If MM accepts any command as valid, MM shall execute the 
;		command, increment the MM Valid Command Counter and issue an
;		event message.
;    MM1010	If MM rejects any command, MM shall abort the command execution,
;		increment the MM Command Rejected Counter and issue an error
;		event message.
;    MM2000	Upon receipt of a Poke command, MM shall write 8,16, or 32 bits
;		of data to the command-specified RAM address.
;    MM2000.1	MM shall confirm a write to the RAM address by issuing an 
;		event message which includes:
;			a. address written
;			b. length of data written
;			c. value of the data written
;    MM2002	Upon receipt of a Peek command, MM shall read 8,16, or 32 bits
;		of data from the command-specified RAM address and generate an
;		event message containing the following data:
;			a. address read
;			b. length of data read
;			c. value of the data read
;    MM2003	Upon receipt of a Write With Intterupts Disable command, MM 
;		shall write up to <PLATFORM_DEFINED, TBD> bytes to the 
;		command-specified RAM memory address with interrupts disabled.
;    MM2003.1	MM shall verify that the command-specified <MISSION_DEFINED>
;		  CRC matches the computed CRC of the data.
;    MM2003.2	If the command-specified CRC fails validation, MM shall reject
;		the command.
;    MM2004	Upon receipt of a Read command, MM shall read the 
;		command-specified number of consecutive bytes from the 
;		command-specified RAM memory address and generate and event
;		message containing the data.
;    MM2004.1	If the number of bytes exceeds the maximum event message size
;		then the command shall be rejected.
;    MM2100	Upon receipt of a Load From File command, MM shall load RAM,
;		with interrupts enabled during the actual load, based on the
;		following information contained in the command-specified file: 
;			a. Destination Address
;			b. Destination Memory Type
;			c. <MISSION_DEFINED> CRC (data only)
;			d. Number of Bytes to Load
;    MM2100.1	If the CRC contained in the file fails validation, MM shall
;		reject the command.
;    MM2100.2	If the number of bytes exceeds <PLATFORM_DEFINED, TBD> then
;		the command shall be rejected.
;    MM2104	Upon receipt of a Dump to File command, MM shall write the data
;		associated with the command-specified RAM address, 
;		command-specified number of bytes and calculated 
;		<MISSION_DEFINED> CRC to the command-specified file.
;    MM2104.1	If the command-specified number of bytes exeeds 
;		<PLATFORM_DEFINED, TBD> then the command shall be rejected.
;    MM2300	Upon receipt of a Fill command, MM shall fill RAM with the
;		contents based on the following command-specified parameters:
;			a. Destination Address
;			b. Destination Memory Type
;			c. Number of Bytes to fill
;			d. 32-bit Fill Pattern
;    MM2300.1 If the number of bytes exceeds <PLATFORM_DEFINED, TBD> then
;		  the command shall be rejected.
;    MM2500	When writing data to RAM memory, MM shall write a maximum of
;		<PLATFORM_DEFINED, TBD> bytes per execution cycle.
;    MM2501	When writing data to a file, MM shall write a maximum of
;		<PLATFORM_DEFINED, TBD> bytes per execution cycle.
;    MM8000	MM shall generate a housekeeping message containing the
;		following:
;			a. MM Valid Command Counter
;			b. MM Command Rejected Counter
;			c. Last command executed
;			d. Address for last command
;			e. Memory Type for last command
;			f. Number of bytes specified by last command
;			g. Filename used in last command
;                       h. Data Value for last command (may be fill pattern or
;                          peek/poke value)
;    MM9000	Upon initialization of the MM Application, MM shall initialize
;		the following data to zero::
;			a. MM Valid Command Counter
;			b. MM Command Rejected Counter
;			c. Last command executed
;			d. Address for last command
;			e. Memory Type for last command
;			f. Number of bytes specified by last command
;			g. Filename used in last command
;                       h. Data Value for last command (may be fill pattern or
;                          peek/poke value)
;
;  Prerequisite Conditions
;	The CFS is up and running and ready to accept commands.
;	The MM commands and telemetry items exist in the GSE database.
;	A display page exists for the MM Housekeeping telemetry packet.
;
;  Assumptions and Constraints
;	The following Memory Manager configuration parameters must be set lower
;	than the the RAM disk partition size. The RAM disk size is controlled by
;	the CFE_ES_RAM_DISK_NUM_SECTORS define in the cfe_platform_cfg.h file.
;		MM_MAX_LOAD_FILE_DATA_RAM 	
;		MM_MAX_DUMP_FILE_DATA_RAM
;		MM_MAX_FILL_DATA_RAM
;
;  Change History
;
;	Date		   Name		Description
;	03/07/08	Walt Moleski	Original Procedure.
;	05/23/08	Walt Moleski	Added text to the Assumptions and
;					Constraints section above.
;       10/25/10        Walt Moleski    Replaced setupevt with setupevents and
;                                       added a variable for the app name
;       04/16/15        Walt Moleski    Updated the requirements for MM 2.4.0.0
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			    Description
;       ut_tlmwait          Wait for a specified telemetry point to update to
;                           a specified value. 
;       ut_sendcmd          Send commands to the spacecraft. Verifies command
;                           processed and command error counters.
;       ut_sendrawcmd       Send raw commands to the spacecraft. Verifies 
;                           command
;			    processed and command error counters.
;       ut_pfindicate       Print the pass fail status of a particular 
;			    requirement
;                           number.
;       ut_setupevents      Performs setup to verify that a particular event
;                           message was received by ASIST.
;	ut_setrequirements  A directive to set the status of the cFE
;			    requirements array.
;	load_start_app      Uploads and starts the specified application.
;	get_mm_file_to_cvt  Issues the MM_DumpFile command and transfers the
;			    dump file to the ground.
;	ftp_file	    In this proc, the ftp_file procedure is used to
;			    download MM load files that are used more than once
;			    and delete large onboard files in order to not run
;			    out of disk space while executing this test.
;	create_mm_file_from_cvt	 Uses the MM_LOADDUMP cvt to create a MM load
;				 file based upon the values in the cvt.
;	load_memory	    Uploads the specified MM load file and issues an
;			    MM_LoadFile command using the filename.
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_mission_cfg.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "to_lab_events.h"
#include "osconfig.h"
#include "mm_platform_cfg.h"
#include "mm_msgdefs.h"
#include "mm_events.h"
#include "tst_mm_events.h"

%liv (log_procedure) = logging

#define MM_1006		0
#define MM_1007		1
#define MM_1008		2
#define MM_1009		3
#define MM_1010		4
#define MM_2000		5
#define MM_20001	6
#define MM_2002		7
#define MM_2003		8
#define MM_20031	9
#define MM_20032	10
#define MM_2004		11
#define MM_20041	12
#define MM_2100 	13
#define MM_21001	14
#define MM_21002	15
#define MM_2104		16
#define MM_21041	17
#define MM_2300		18
#define MM_23001	19
#define MM_2500		20
#define MM_2501		21
#define MM_8000		22
#define MM_9000		23

global ut_req_array_size = 23
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["MM_1006", "MM_1007", "MM_1008", "MM_1009", "MM_1010", "MM_2000", "MM_2000.1", "MM_2002", "MM_2003", "MM_2003.1", "MM_2003.2", "MM_2004", "MM_2004.1", "MM_2100", "MM_2100.1", "MM_2100.2", "MM_2104", "MM_2104.1", "MM_2300", "MM_2300.1", "MM_2500", "MM_2501", "MM_8000", "MM_9000"]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL poke8Pattern1 = "FF"
LOCAL poke8Pattern2 = "AA"
LOCAL poke8Value1 = x'FF'
LOCAL poke8Value2 = x'AA'
LOCAL poke16Pattern1 = "AABB"
LOCAL poke16Pattern2 = "FFFF"
LOCAL poke16Value1 = x'AABB'
LOCAL poke16Value2 = x'FFFF'
LOCAL poke32Pattern1 = "FFFFFFFF"
LOCAL poke32Pattern2 = "AAAAAAAA"
LOCAL poke32Value1 = x'FFFFFFFF'
LOCAL poke32Value2 = x'AAAAAAAA'
LOCAL cmdCtr
LOCAL errCtr
LOCAL validAddr
LOCAL invalidAddr
LOCAL MMAppName = "MM"
LOCAL ramDir = "RAM:0"

;; This define is calculated in the mm_dump.c file and could not be used
;; directly from the MM source. Thus, if this size changes, this calculation
;; must be changed here
#define MM_MAX_DUMP_INEVENT_BYTES ((CFE_EVS_MAX_MESSAGE_LENGTH - (13 + 25)) / 5)

;; Determine the Packet IDs for the MM Load/Dump file CVT
local varPktID
local appPktID
local hexAppID

;; CPU1 is the default
varPktID = "PF0B"
appPktID = "P0F0B"
hexAppID = x'0F0B'

if ("$CPU" = "CPU2") then
  varPktID = "PF2B"
  appPktID = "P0F2B"
  hexAppID = x'0F2B'
elseif ("$CPU" = "CPU3") then
  varPktID = "PF4B"
  appPktID = "P0F4B"
  hexAppID = x'0F4B'
endif

write ";**********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";**********************************************************************"
write ";  Step 1.1:  Command a Power-On Reset on $CPU. "
write ";**********************************************************************"
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
page $SC_$CPU_TST_MM_HK

write ";**********************************************************************"
write ";  Step 1.3: Start the Memory Manager (MM) Application and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_INIT_INF_EID, "INFO", 2

s load_start_app (MMAppName,"$CPU","MM_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - MM Application Started"
  else
    write "<!> Failed - MM Application start Event Message not received."
  endif
else
  write "<!> Failed - MM Application start Event Messages not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'887'

if ("$CPU" = "CPU2") then
  stream1 = x'987'
elseif ("$CPU" = "CPU3") then
  stream1 = x'A87'
endif

write "Sending command to add subscription for MM HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 1.4: Start the Memory Manager Test Application (TST_MM) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MM","$CPU","TST_MM_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - MM Application Started"
  else
    write "<!> Failed - MM Application start Event Message not received."
  endif
else
  write "<!> Failed - MM Application start Event Messages not received."
endif

;;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream1 = x'924'

if ("$CPU" = "CPU2") then
  stream1 = x'A24'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B24'
endif

write "Sending command to add subscription for MM HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 1.5: Verify that the MM Housekeeping packet is being generated "
write ";  and the telemetry items are initialized to zero (0). "
write ";**********************************************************************"
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

;; Need to send the HK Request to MM
;; Check the HK tlm items to see if they are 0 or NULL
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
  write "  Bytes processed = ",$SC_$CPU_MM_BytesProc
  write "  Filename        = ",$SC_$CPU_MM_LastFile
  ut_setrequirements MM_9000, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.0: MM Peek and Poke tests."
write ";**********************************************************************"
write ";  Step 2.1: Send an 8-bit Peek command."
write ";**********************************************************************"
;; Get the valid RAM address from the test application
validAddr = $SC_$CPU_TST_MM_RAMAddress + 1
LOCAL textValueRead

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
else
  write "<!> Failed (1009;2002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
;; Get the result (if possible)
;; Peek Command: Addr = 0x00000000 Size = 8 bits Data = 0x07
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,57)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = 1) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

;; Send the DumpInEvent command with the address -1 for 3 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=3 SymName="" Offset=validAddr-1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_INEVENT) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr-1) AND ;;
     ($SC_$CPU_MM_BytesProc = 3) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.2: Determine the pattern to poke based upon the value returned"
write ";  from the peek command above."
write ";**********************************************************************"
LOCAL pokeValue;

if (textValueRead <> poke8Pattern1) then
  pokeValue = poke8Value1
else
  pokeValue = poke8Value2
endif

write ";**********************************************************************"
write ";  Step 2.3: Send an 8-bit Poke command."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_POKE_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_Poke DataSize=8 RAM Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000) - 8 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2000, "P"
else
  write "<!> Failed (1009;2000) - 8 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2000, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000.1) - Expected Event Msg ",MM_POKE_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_20001, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_POKE) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = pokeValue) AND ;;
     ($SC_$CPU_MM_BytesProc = 1) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_20001, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.4: Send an 8-bit Peek command to verify the Poke command above"
write ";  wrote the correct data. Check the byte before & after"
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
else
  write "<!> Failed (1009;2002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
;; Value read should be = to pokeValue
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,57)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = pokeValue) AND ;;
     ($SC_$CPU_MM_BytesProc = 1) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

;; Send the DumpInEvent command with the address -1 for 3 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=3 SymName="" Offset=validAddr-1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.5: Send a 16-bit Peek command."
write ";**********************************************************************"
;; Determine a valid RAM address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 2

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_WORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=16 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - 16 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
else
  write "<!> Failed (1009;2002) - 16 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_WORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,57,60)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = 2) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_WORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

;; Send the DumpInEvent command with the address -2 for 6 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=6 SymName="" Offset=validAddr-2

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_INEVENT) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr-2) AND ;;
     ($SC_$CPU_MM_BytesProc = 6) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.6: Determine the pattern to poke based upon the value returned"
write ";  from the peek command above."
write ";**********************************************************************"
if (textValueRead <> poke16Pattern1) then
  pokeValue = poke16Value1
else
  pokeValue = poke16Value2
endif

write ";**********************************************************************"
write ";  Step 2.7: Send a 16-bit Poke command."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_POKE_WORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_Poke DataSize=16 RAM Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000) - 16 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2000, "P"
else
  write "<!> Failed (1009;2000) - 16 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2000, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000.1) - Expected Event Msg ",MM_POKE_WORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_20001, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_POKE) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = pokeValue) AND ;;
     ($SC_$CPU_MM_BytesProc = 2) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_WORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_20001, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.8: Send a 16-bit Peek command to verify the Poke command above"
write ";  wrote the correct data. Check the byte before & after"
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_WORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=16 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - 16 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
else
  write "<!> Failed (1009;2002) - 16 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_WORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,57,60)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = pokeValue) AND ;;
     ($SC_$CPU_MM_BytesProc = 2) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_WORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

;; Send the DumpInEvent command with the address -2 for 6 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=6 SymName="" Offset=validAddr-2

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.9: Send a 32-bit Peek command."
write ";**********************************************************************"
;; Determine a valid RAM address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 4

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_DWORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=32 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - 32 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
else
  write "<!> Failed (1009;2002) - 32 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_DWORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,57,64)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = 4) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_DWORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

;; Send the DumpInEvent command with the address -4 for 12 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=12 SymName="" Offset=validAddr-4

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_INEVENT) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr-4) AND ;;
     ($SC_$CPU_MM_BytesProc = 12) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.10: Determine the pattern to poke based upon the value "
write ";  returned from the peek command above."
write ";**********************************************************************"
;; Verify that the value read is not equal to the pattern to be written
if (textValueRead <> poke32Pattern1) then
  pokeValue = poke32Value1
else
  pokeValue = poke32Value2

endif

write ";**********************************************************************"
write ";  Step 2.11: Send a 32-bit Poke command."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_POKE_DWORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_Poke DataSize=32 RAM Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000) - 32 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2000, "P"
else
  write "<!> Failed (1009;2000) - 32 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2000, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000.1) - Expected Event Msg ",MM_POKE_DWORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_20001, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_POKE) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = pokeValue) AND ;;
     ($SC_$CPU_MM_BytesProc = 4) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_DWORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_20001, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.12: Send a 32-bit Peek command to verify the Poke command "
write ";  above wrote the correct data. Check the byte before & after."
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_DWORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=32 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - 32 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
else
  write "<!> Failed (1009;2002) - 32 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_DWORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,57,64)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = pokeValue) AND ;;
     ($SC_$CPU_MM_BytesProc = 4) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_DWORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2002, "F"
endif

;; Send the DumpInEvent command with the address -4 for 12 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=12 SymName="" Offset=validAddr-4

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.13: Send a Poke command with an invalid RAM address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
invalidAddr = $SC_$CPU_TST_MM_EEPROMAddress+4

/$SC_$CPU_MM_Poke DataSize=32 RAM Data=pokeValue SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Poke command failed as expected."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.14: Send a Poke command with an invalid command length"
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004E033B"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004E033B"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004E033B"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.15: Send a Poke command with an invalid arguments."
write ";**********************************************************************"
write ";  Step 2.15.1: Send command with an invalid Memory Type."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_MEMTYPE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004D033B2000"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004D033B2000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004D033B2000"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Invalid Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.15.2: Send a Poke command with an invalid Data Size."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BITS_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_Poke DataSize=4 RAM Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Invalid Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BITS_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BITS_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.16: Send a Peek command with an invalid RAM address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_Peek DataSize=32 RAM SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Peek command failed as expected."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Invalid Peek command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.17: Send a Peek command with an invalid command length"
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004A0275"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004A0275"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004A0275"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.18: Send a Peek command with invalid arguments."
write ";**********************************************************************"
write ";  Step 2.18.1: Send a Peek command with an invalid Memory Type."
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_MEMTYPE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the Peek
;; CPU1 is the default
rawcmd = "1888c000004902750800"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004902750800"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004902750800"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Peek command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Peek command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.18.2: Send a Peek command with an invalid Data Size."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BITS_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_Peek DataSize=3 RAM SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Peek command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Peek command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BITS_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BITS_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.0: MM Read & Write tests."
write ";**********************************************************************"
write ";  Step 3.1: Send a write with interrupts disabled command to a valid"
write ";  RAM address. "
write ";**********************************************************************"
;; Setup the data array to load
local widDataArray[1 .. MM_MAX_UNINTERRUPTABLE_DATA]
local testDataArray[1 .. 256]
local validCRC
local dataToSend

;; Turn logging off for this data initialization
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

;; May want to turn logging off around this initialization
for i = 1 to MM_MAX_UNINTERRUPTABLE_DATA DO
  widDataArray[i] = x'FA'
  testDataArray[i] = x'FA'
enddo
if (MM_MAX_UNINTERRUPTABLE_DATA < 256) then
  for i = MM_MAX_UNINTERRUPTABLE_DATA+1 to 256 DO
    testDataArray[i] = x'FA'
  enddo
endif
%liv (log_procedure) = logging

;; Send half of the max data
dataToSend = MM_MAX_UNINTERRUPTABLE_DATA / 2

;; Setup for the Test App Event message
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_GETCRC_INF_EID, "INFO", 1

;; Calculate CRC on Data by sending a TST_MM command
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_GetCRC DataSize=dataToSend dataArray=testDataArray

ut_tlmwait  $SC_$CPU_TST_MM_CMDPC, {cmdCtr}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_GetCRC command did not increment CMDPC."
endif

; Set the variable for the calculated CRC
validCRC = $SC_$CPU_TST_MM_ValidCRC

;; Setup for the Load_WID Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LOAD_WID_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Load with interrupts disabled command
/$SC_$CPU_MM_LoadWID DataSize=dataToSend CRC=validCRC SymName="" Offset=validAddr Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2003;2003.1) - LoadWID command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2003, "P"
  ut_setrequirements MM_20031, "P"
else
  write "<!> Failed (1009;2003;2003.1) - LoadWID command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2003, "F"
  ut_setrequirements MM_20031, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LOAD_WID_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_WID) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LOAD_WID_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.2: Send a read command with the RAM address and number of "
write ";  bytes specified in the step above in order to verify the write. "
write ";  The area written above is dumped to a file for verification. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

;; Send the Dump command and transfer the file to ground
s get_mm_file_to_cvt(ramDir,"mm_ram3_2_dump.dat","$CPU",appPktID,"RAM",dataToSend,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2104, "A"
else
  write "<!> Failed (1009;2104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Delete the onboard file
  s ftp_file (ramDir,"na","mm_ram3_2_dump.dat","$CPU","R")
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.3: Send a write with interrupts disabled command containing an"
write ";  invalid CRC for the specified data."
write ";**********************************************************************"
;; Setup for the Load_WID Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LOAD_WID_CRC_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the Load with interrupts disabled command
/$SC_$CPU_MM_LoadWID DataSize=dataToSend CRC=0 SymName="" Offset=validAddr Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;2003.2) - LoadWID command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_20032, "P"
else
  write "<!> Failed (1010;2003.2) - LoadWID command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_20032, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LOAD_WID_CRC_ERR_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LOAD_WID_CRC_ERR_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.4: Send a write with interrupts disabled command with an "
write ";  invalid RAM address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_LoadWID DataSize=dataToSend CRC=validCRC SymName="" Offset=invalidAddr Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - LoadWID command failed as expected."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Invalid LoadWID command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.5: Send a write with interrupts disabled command with an "
write ";  invalid command length."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000011604B2"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000011604B2"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000011604B2"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.6: Send a read command with the bytes to be read larger than "
write ";  the maximum event message size. The maximum data bytes that can be "
write ";  dumped in an event message is calculated at the top of this "
write ";  procedure. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

local eventBytes = MM_MAX_DUMP_INEVENT_BYTES+1 

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=eventBytes SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;2004.1) -  Dump In Event command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_20041, "P"
else
  write "<!> Failed (1010;2004.1) - Dump In Event command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_20041, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.7: Send a read command with the bytes to be read equal to the"
write ";  maximum event message size. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=MM_MAX_DUMP_INEVENT_BYTES SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.8: Send a read command to read a single byte."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=1 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.9: Send a read command with an invalid RAM address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_DumpInEvent RAM DataSize=10 SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Dump In Event command failed as expected."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Invalid Dump In Event command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.10: Send a read command with invalid arguments. "
write ";**********************************************************************"
write ";  Step 3.10.1: Send a read command with an invalid command length."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004A07BD"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004A07BD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004A07BD"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.10.2: Send command specifying 0 bytes."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_DATA_SIZE_BYTES_ERR_EID,"ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_DumpInEvent RAM DataSize=0 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Dump In Event command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Invalid Dump In Event command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.10.3: Send command with an invalid Memory Type."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_MEMTYPE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004907BD000A"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004907BD000A"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004907BD000A"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Invalid Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.11: Send a write with interrupts disabled command with invalid"
write ";  arguments. The only argument not tested above is the DataSize."
write ";**********************************************************************"
write ";  Step 3.11.1: Send command with DataSize > max."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

dataToSend = MM_MAX_UNINTERRUPTABLE_DATA + 1
errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_LoadWID DataSize=dataToSend CRC=validCRC SymName="" Offset=validAddr Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - LoadWID command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Invalid LoadWID command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.11.2: Send command with DataSize = 0."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_LoadWID DataSize=0 CRC=validCRC SymName="" Offset=validAddr Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - LoadWID command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Invalid LoadWID command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.0: MM File command tests."
write ";**********************************************************************"
write ";  Step 4.1: Send a Load from File command with a valid RAM load file. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress
dataToSend = 1024

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=validAddr Pattern=x'AF' RAM SymbolName="" Filename="/ram/validramload.dat"
wait 5

ut_tlmwait  $SC_$CPU_TST_MM_CMDPC, {cmdCtr}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateFile command did not increment CMDPC."
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_MM_CREATEFILE_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_MM_CREATEFILE_INF_EID,"."
endif

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/validramload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2100, "P"
else
  write "<!> Failed (1009;2100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/validramload.dat") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LD_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.2: Send a Dump to File command using the RAM address and "
write ";  number of bytes specified in the file loaded above. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

;; Dump the file and transfer it to the ground
s get_mm_file_to_cvt(ramDir,"mm_ram4_2_dump.dat","$CPU",appPktID,"RAM",dataToSend,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2104, "A"
else
  write "<!> Failed (1009;2104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_TO_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/mm_ram4_2_dump.dat") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","mm_ram4_2_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.3: Send a Load from File command with an invalid CRC "
write ";  specified in the load file. "
write ";**********************************************************************"
;; Need to get a valid RAM address from test app
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Set the RAM memory address where the load starts
local hdrVar = varPktID & "ADDROFFSET"
{hdrVar} = validAddr
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the symbol name to NULL
hdrVar = varPktID & "SYMNAME"
{hdrVar} = ""
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the number of bytes to load
hdrVar = varPktID & "NUMBYTES"
{hdrVar} = 2048
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the CRC for the data being loaded
hdrVar = varPktID & "CRC"
{hdrVar} = 0
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the Memory Type
hdrVar = varPktID & "MEMTYPE"
{hdrVar} = MM_RAM
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Create the Load File
s create_mm_file_from_cvt("$CPU",varPktID,appPktID,hexAppID,"Invalid CRC RAM Load File","badramcrcload.dat")

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Transfer the file to s/c and issue the MM LoadFile command
s load_memory ("badramcrcload.dat", "$CPU")

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;2100.1) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_21001, "P"
else
  write "<!> Failed (1010;2100.1) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_21001, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_LOAD_FILE_CRC_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LOAD_FILE_CRC_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","badramcrcload.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.4: Send a Load from File command with a load file that exceeds"
write ";  the <PLATFORM_DEFINED> maximum number of bytes. "
write ";**********************************************************************"
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the error files one is used by this step
;; and the other two are used in Steps 4.10 and 4.11
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateErrorFiles Address=validAddr Pattern=x'FA' RAM
wait 5

ut_tlmwait  $SC_$CPU_TST_MM_CMDPC, {cmdCtr}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateFile command did not increment CMDPC."
endif

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/overmaxload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;2100.2) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_21002, "P"
else
  write "<!> Failed (1010;2100.2) - LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_21002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","overmaxload.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.5: Send a Load from File command with a load file that "
write ";  contains exactly the <PLATFORM_DEFINED> maximum number of bytes. "
write ";**********************************************************************"
write ";  Step 4.5.1: Create the load file using the test application. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=MM_MAX_LOAD_FILE_DATA_RAM Address=validAddr Pattern=x'AF' RAM SymbolName="" Filename="/ram/maxdataramload.dat"
wait 5

ut_tlmwait  $SC_$CPU_TST_MM_CMDPC, {cmdCtr}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateFile command did not increment CMDPC."
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_MM_CREATEFILE_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_MM_CREATEFILE_INF_EID,"."
endif

write ";**********************************************************************"
write ";  Step 4.5.2: Send the load file command. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/maxdataramload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2100, "P"
else
  write "<!> Failed (1009;2100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = MM_MAX_LOAD_FILE_DATA_RAM) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/maxdataramload.dat") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LD_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

write ";**********************************************************************"
write ";  Step 4.5.3: Download the load file in order to use it again later "
write ";  and delete the onboard file. "
write ";**********************************************************************"
;; Get the onboard file
s ftp_file (ramDir,"maxdataramload.dat","maxdataramload.dat","$CPU","G")

;; Delete the onboard file
s ftp_file (ramDir,"na","maxdataramload.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.6: Send a Load from File command with a load file that "
write ";  contains exactly one (1) byte of data. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress + 1

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=1 Address=validAddr Pattern=x'55' RAM SymbolName="" Filename="/ram/onebyteramload.dat"
wait 5

ut_tlmwait  $SC_$CPU_TST_MM_CMDPC, {cmdCtr}
if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateFile command did not increment CMDPC."
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_MM_CREATEFILE_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_MM_CREATEFILE_INF_EID,"."
endif

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/onebyteramload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2100, "P"
else
  write "<!> Failed (1009;2100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LD_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.7: Send a Load from File command with an invalid command "
write ";  length. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004205"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004205"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004205"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.8: Send a Load from File command with a load file that "
write ";  contains an invalid RAM destination address."
write ";**********************************************************************"
;; Create the Load File
;;s create_mm_file_from_cvt("$CPU",varPktID,appPktID,hexAppID,ramDirad File with invalid address",".dat")
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=invalidAddr Pattern=x'AF' RAM SymbolName="" Filename="/ram/badramaddrload.dat"

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/badramaddrload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - LoadFile command failed as expected."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.9: Send a Load from File command with an filename that does "
write ";  not exist."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_OPEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_LoadFile Filename="/ram/mmFileMissing.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - LoadFile command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1009;1010) - Invalid LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_OPEN_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_OPEN_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.10: Send a Load from File command with a load file whose file"
write ";  header contains a valid number of bytes but whose data portion "
write ";  contains more data than specified. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_FILE_SIZE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/toomuchdata.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_LD_FILE_SIZE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LD_FILE_SIZE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","toomuchdata.dat","$CPU","R")
wait 5

write ";**********************************************************************"
write ";  Step 4.11: Send a Load from File command with a load file whose file"
write ";  header contains a valid number of bytes but whose data portion "
write ";  contains less data than specified. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_FILE_SIZE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/notenoughdata.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_LD_FILE_SIZE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LD_FILE_SIZE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","notenoughdata.dat","$CPU","R")
wait 5

write ";**********************************************************************"
write ";  Step 4.12: Send a Load from File command with invalid arguments. "
write ";  The only argument is the filename. Send multiple commands that "
write ";  attempt to load invalid filenames. One test should use a NULL. "
write ";**********************************************************************"
write ";  Step 4.12.1: Send command with a NULL Filename. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_CMD_FNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_LoadFile Filename=""

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - LoadFile command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_CMD_FNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_CMD_FNAME_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.12.2: Send command with a Filename containing invalid "
write ";  characters. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_CMD_FNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_LoadFile Filename="/ram/ab(3)"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - LoadFile command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_CMD_FNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_CMD_FNAME_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5
write ";**********************************************************************"
write ";  Step 4.13: Send a Dump to File command with the number of bytes "
write ";  greater than the <PLATFORM_DEFINED> maximum number of bytes. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File RAM DataSize=MM_MAX_DUMP_FILE_DATA_RAM+1 SymName="" Offset=validAddr Filename="/ram/mm_ram4_13_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;2104.1) -  Dump2File command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_21041, "P"
else
  write "<!> Failed (1010;2104.1) - Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_21041, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.14: Send a Dump to File command with the number of bytes "
write ";  equal to the <PLATFORM_DEFINED> maximum number of bytes. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the dump command and transfer the file to the ground
s get_mm_file_to_cvt(ramDir,"mm_ram4_14_dump.dat","$CPU",appPktID,"RAM",MM_MAX_DUMP_FILE_DATA_RAM,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2104, "A"
else
  write "<!> Failed (1009;2104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","mm_ram4_14_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.15: Send a Dump to File command with the number of bytes "
write ";  equal to one (1) byte. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the dump command and transfer the file to the ground
s get_mm_file_to_cvt(ramDir,"mm_ram4_15_dump.dat","$CPU",appPktID,"RAM",1,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2104, "A"
else
  write "<!> Failed (1009;2104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.16: Send a Dump to File command with an invalid command "
write ";  length. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000008E06"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000008E06"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000008E06"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.17: Send a Dump to File command with an invalid RAM address. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File RAM DataSize=dataToSend SymName="" Offset=invalidAddr Filename="/ram/mm_ram4_17_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Dump2File command failed as expected."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.18: Send a Dump to File command with an invalid filename. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_CREAT_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File RAM DataSize=dataToSend SymName="" Offset=validAddr Filename="mm_invalid_dumpname.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) -  Dump2File command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_CREAT_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_CREAT_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.19: Send a Dump to File command with invalid arguments. "
write ";**********************************************************************"
write ";  Step 4.19.1: Send command with invalid memory type. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_MEMTYPE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
;; CPU1 is the default
rawcmd = "1888c000008D066400"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000008D066400"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000008D066400"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - Dump2File command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.19.2: Send command with data size = 0. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File RAM DataSize=0 SymName="" Offset=validAddr Filename="/ram/mm_dump_14_19_2.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - Dump2File command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.19.3: Send command with a NULL dump filename. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_CMD_FNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File RAM DataSize=100 SymName="" Offset=validAddr Filename=""

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - Dump2File command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_CMD_FNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_CMD_FNAME_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.0: MM Fill Command tests."
write ";**********************************************************************"
write ";  Step 5.1: Send a Fill command to a valid RAM address."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFFFFFFF' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2300, "P"
else
  write "<!> Failed (1009;2300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'FFFFFFFF') AND ;;
     ($SC_$CPU_MM_BytesProc = MM_MAX_DUMP_INEVENT_BYTES) AND ;;
     ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_FILL_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.2: Send a Read command with the address and number of bytes "
write ";  specified in the above step. Verify the Fill command worked. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=MM_MAX_DUMP_INEVENT_BYTES SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.3: Send a Fill command with the command-specified number of "
write ";  bytes greater than the <PLATFORM_DEFINED> maximum number of bytes."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=MM_MAX_FILL_DATA_RAM+1 Pattern=x'AAAAAAAA' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;2300.1) - Fill command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_23001, "P"
else
  write "<!> Failed (1009;2300.1) - Invalid Fill command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_23001, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DATA_SIZE_BYTES_ERR_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.4: Send a Fill command with the command-specified number of "
write ";  bytes equal to the <PLATFORM_DEFINED> maximum number of bytes."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=MM_MAX_FILL_DATA_RAM Pattern=x'55555555' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2300, "P"
else
  write "<!> Failed (1009;2300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_RAM) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'55555555') AND ;;
     ($SC_$CPU_MM_BytesProc = MM_MAX_FILL_DATA_RAM) AND ;;
     ($SC_$CPU_MM_LastFile = "") THEN
    write "<*> Passed (8000) - Last Command HK items as expected."
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
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_FILL_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.5: Send a Dump to File command using the arguments from the "
write ";  Fill command in the step above. Verify the Fill command worked."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
s get_mm_file_to_cvt(ramDir,"mm_ram5_5_dump.dat","$CPU",appPktID,"RAM",MM_MAX_FILL_DATA_RAM,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2104, "A"
else
  write "<!> Failed (1009;2104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

;; Delete the onboard file
s ftp_file (ramDir,"na","mm_ram5_5_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 5.6: Send a Fill command with the command-specified number of "
write ";  bytes equal to one (1) byte."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=1 Pattern=x'BBBBBBBB' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2300, "P"
else
  write "<!> Failed (1009;2300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_FILL_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.7: Send a Read command with the address and number of bytes "
write ";  specified in the above step. Verify the Fill command worked. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent RAM DataSize=3 SymName="" Offset=validAddr-1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2004, "A"
else
  write "<!> Failed (1009;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.8: Send a Fill command with an invalid command length. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000005208"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000005208"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000005208"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1006;1010) - Command Rejected Counter incremented."
  ut_setrequirements MM_1006, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1006;1010) - Command Rejected Counter did not increment as expected."
  ut_setrequirements MM_1006, "F"
  ut_setrequirements MM_1010, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.9: Send a Fill command with an invalid RAM address. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=10 Pattern=x'FFFFFFFF' SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Fill command sent properly."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Fill command did not increment CMDEC."
  ut_setrequirements MM_1007, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.10: Send a Fill command with invalid arguments. "
write ";**********************************************************************"
write ";  Step 5.10.1: Send command with invalid memory type. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_MEMTYPE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
;; CPU1 is the default
rawcmd = "1888c000005108F700"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000005108F700"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000005108F700"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)


ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) -  Fill command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid Fill command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.10.2: Send command with data size = 0. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=0 Pattern=x'FAFAFAFA' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1008;1010) - Fill command failed as expected."
  ut_setrequirements MM_1008, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1008;1010) - Invalid Fill command did not increment CMDEC."
  ut_setrequirements MM_1008, "F"
  ut_setrequirements MM_1010, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 6.0: MM Data Writing tests."
write ";**********************************************************************"
write ";  Step 6.1: Start the Performance Analyzer data collection in order to"
write ";  determine if the cycle segmentation is happening."
write ";**********************************************************************"
;; Send the ES command
/$SC_$CPU_ES_STARTPERF TriggerStart

write ";**********************************************************************"
write ";  Step 6.2: Send a Load from File command to a valid RAM address with"
write ";  a large amount of data to load. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command to load the maximum number of bytes
s load_memory ("maxdataramload.dat", "$CPU")

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2100, "P"
else
  write "<!> Failed (1009;2100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LD_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 6.3: Set the requirement for the above step to Analysis and    "
write ";  delete the onboard file to free up some space."
write ";**********************************************************************"
ut_setrequirements MM_2500, "A"

;; Delete the onboard file
s ftp_file (ramDir,"na","maxdataramload.dat","$CPU","R")

write ";**********************************************************************"
write ";  Step 6.4: Send a Dump to File command to a valid RAM address and a "
write ";  large number of bytes specified. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
s get_mm_file_to_cvt(ramDir,"mm_ram6_4_dump.dat","$CPU",appPktID,"RAM",MM_MAX_DUMP_FILE_DATA_RAM,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_2104, "A"
else
  write "<!> Failed (1009;2104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_2104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 6.5: Set the requirement for the above step to Analysis and    "
write ";  stop the Performance Analyzer data collection. "
write ";**********************************************************************"
ut_setrequirements MM_2501, "A"

/$SC_$CPU_ES_StopPerf DataFileName="/ram/mm_ram_perf.dat"
wait 30
                                                                                
;; May have to wait until the data file finishes before doing the reset
s ftp_file (ramDir,"mm_ram_perf.dat","mm_ram_perf.dat","$CPU","G")

write ";**********************************************************************"
write ";  Step 7.0: Perform a Power-on Reset to clean-up from this test."
write ";**********************************************************************"
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
write ";  End procedure $SC_$CPU_mm_ram                                      "
write ";*********************************************************************"
ENDPROC
