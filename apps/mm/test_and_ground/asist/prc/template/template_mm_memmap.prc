PROC $sc_$cpu_mm_memmap
;*******************************************************************************
;  Test Name:  mm_memmap
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Memory Manager (MM) Memnory Mapped I/O
;	commands function properly and that the MM application handles 
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
;    MM5000	Upon receipt of a Poke command, MM shall write
;		<PLATFORM_DEFINED> bytes of data to the command-specified 
;		Memory Mapped I/O address.
;    MM5000.1	MM shall confirm a write to the Memory Mapped I/O address by
;		issuing an event message which includes:
;			a. address written
;			b. length of data written
;			c. value of the data written
;    MM5002	Upon receipt of a Peek command, MM shall read <PLATFORM_DEFINED>
;		bytes of data from the command-specified Memory Mapped I/O
;		address and generate an event message containing the following
;		data:
;			a. address read
;			b. length of data read
;			c. value of the data read
;    MM5004	Upon receipt of a Read command, MM shall read the 
;		command-specified number of consecutive bytes from the 
;		command-specified Memory Mapped I/O memory address and generate
;		an event message containing the data.
;    MM5004.1	If the number of bytes exceeds the maximum event message size
;		then the command shall be rejected.
;    MM5100	Upon receipt of a Load From File command, MM shall load Memory
;		mapped I/O, with interrupts enabled during the actual load,
;		based on the following information contained in the
;		command-specified file: 
;			a. Destination Address
;			b. Destination Memory Type
;			c. <MISSION_DEFINED> CRC (data only)
;			d. Number of Bytes to Load
;    MM5100.1	If the CRC contained in the file fails validation, MM shall
;		reject the command.
;    MM5100.2	If the number of bytes exceeds <PLATFORM_DEFINED, TBD> then
;		the command shall be rejected.
;    MM5104	Upon receipt of a Dump to File command, MM shall write the data
;               associated with the command-specified Memory Mapped I/O address,
;               command-specified number of bytes and calculated 
;               <MISSION_DEFINED> CRC to the command-specified file.
;    MM5104.1	If the command-specified number of bytes exeeds 
;                 <PLATFORM_DEFINED, TBD> then the command shall be rejected.
;    MM5300	Upon receipt of a Fill command, MM shall fill Memory Mapped I/O
;		with the contents based on the following command-specified
;		parameters:
;			a. Destination Address
;			b. Destination Memory Type
;			c. Number of Bytes to fill
;			d. 32-bit Fill Pattern
;    MM5300.1	If the number of bytes exceeds <PLATFORM_DEFINED, TBD> then
;		the command shall be rejected.
;    MM8000	MM shall generate a housekeeping message containing the
;		following:
;			a. Valid Command Counter
;			b. Command Rejected Counter
;			c. Last command executed
;			d. Address for last command
;			e. Memory Type for last command
;			f. Number of bytes specified by last command
;			g. Filename used in last command
;                       h. Data Value for last command (may be fill pattern or
;                          peek/poke value)
;MM9000	Upon initialization of the MM Application, MM shall initialize
;		the following data to zero::
;			a. Valid Command Counter
;			b. Command Rejected Counter
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
;	The MM fsw must contain the <OPTIONAL> Memory Mapped I/O support in
;	order for this test procedure to work properly. 
;	The MM commands and telemetry items exist in the GSE database.
;	A display page exists for the MM Housekeeping telemetry packet.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	03/17/08	Walt Moleski	Original Procedure.
;       10/25/10        Walt Moleski    Replaced setupevt with setupevents and
;                                       added a variable for the app name and
;					ram disk. Also, updated the commands to
;					use the MemType union defined in rdl
;	04/16/15	Walt Moleski	Updated the requirements for MM 2.4.0.0
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
;       load_start_app      Uploads and starts the specified application.
;       get_mm_file_to_cvt  Issues the MM_DumpFile command and transfers the
;                           dump file to the ground.
;       ftp_file            In this proc, the ftp_file procedure is used to
;                           download MM load files that are used more than once
;                           and delete large onboard files in order to not run
;                           out of disk space while executing this test.
;       create_mm_file_from_cvt  Uses the MM_LOADDUMP cvt to create a MM load
;                                file based upon the values in the cvt.
;       load_memory         Uploads the specified MM load file and issues an
;                           MM_LoadFile command using the filename.
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
#include "mm_platform_cfg.h"
#include "mm_events.h"
#include "mm_msgdefs.h"
#include "tst_mm_events.h"

%liv (log_procedure) = logging

#define MM_1006		0
#define MM_1007		1
#define MM_1008		2
#define MM_1009		3
#define MM_1010		4
#define MM_5000		5
#define MM_50001	6
#define MM_5002		7
#define MM_5004 	8
#define MM_50041	9
#define MM_5100 	10
#define MM_51001	11
#define MM_51002	12
#define MM_5104  	13
#define MM_51041	14
#define MM_5300 	15
#define MM_53001	16
#define MM_8000		17
#define MM_9000		18

global ut_req_array_size = 18
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["MM_1006", "MM_1007", "MM_1008", "MM_1009", "MM_1010", "MM_5000", "MM_5000.1", "MM_5002", "MM_5004", "MM_5004.1", "MM_5100", "MM_5100.1", "MM_5100.2", "MM_5104", "MM_5104.1", "MM_5300", "MM_5300.1", "MM_8000", "MM_9000"]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL poke8Pattern1 = "FF"
LOCAL poke8Pattern2 = "AA"
LOCAL poke8Value1 = x'FF'
LOCAL poke8Value2 = x'AA'
LOCAL poke16Pattern1 = "FFFF"
LOCAL poke16Pattern2 = "AAAA"
LOCAL poke16Value1 = x'FFFF'
LOCAL poke16Value2 = x'AAAA'
LOCAL poke32Pattern1 = "FFFFFFFF"
LOCAL poke32Pattern2 = "AAAAAAAA"
LOCAL poke32Value1 = x'FFFFFFFF'
LOCAL poke32Value2 = x'AAAAAAAA'
LOCAL validAddr
LOCAL invalidAddr
LOCAL cmdCtr
LOCAL errCtr
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
write ";  Step 1.3:  Start the Memory Manager (MM) Application and "
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
write ";  Step 1.4:  Start the Memory Manager Test Application (TST_MM) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_MM","$CPU","TST_MM_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - TST_MM Application Started"
  else
    write "<!> Failed - TST_MM Application start Event Message not received."
  endif
else
  write "<!> Failed - TST_MM Application start Event Messages not received."
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
LOCAL textValueRead

;; Get a valid address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 1

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 MEM8 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
else
  write "<!> Failed (1009;5002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_PEEK_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,57)
  write "; Parsed text value = '", textValueRead,"'"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Send the DumpInEvent command with the address -1 for 3 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM8 DataSize=3 SymName="" Offset=validAddr-1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"

  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_INEVENT) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
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

/$SC_$CPU_MM_Poke DataSize=8 MEM8 Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5000) - 8 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5000, "P"
else
  write "<!> Failed (1009;5000) - 8 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5000, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5000.1) - Expected Event Msg ",MM_POKE_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_50001, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_POKE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
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
  write "<!> Failed (1009;5000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_50001, "F"
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
/$SC_$CPU_MM_Peek DataSize=8 MEM8 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
else
  write "<!> Failed (1009;5002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_PEEK_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
;; Value read should be = to pokeValue
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,57)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Send the DumpInEvent command with the address -1 for 3 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM8 DataSize=3 SymName="" Offset=validAddr-1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.5: Send a 16-bit Peek command."
write ";**********************************************************************"
;; Get a valid address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 2

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_WORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=16 MEM16 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - 16 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
else
  write "<!> Failed (1009;5002) - 16 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_PEEK_WORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,60)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_WORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Send the DumpInEvent command with the address -2 for 6 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM16 DataSize=6 SymName="" Offset=validAddr-2

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_INEVENT) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
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

/$SC_$CPU_MM_Poke DataSize=16 MEM16 Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5000) - 16 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5000, "P"
else
  write "<!> Failed (1009;5000) - 16 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5000, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5000.1) - Expected Event Msg ",MM_POKE_WORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_50001, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_POKE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
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
  write "<!> Failed (1009;5000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_WORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_50001, "F"
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
/$SC_$CPU_MM_Peek DataSize=16 MEM16 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - 16 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
else
  write "<!> Failed (1009;5002) - 16 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_PEEK_WORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,60)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_WORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Send the DumpInEvent command with the address -2 for 6 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM16 DataSize=6 SymName="" Offset=validAddr-2

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.9: Send a 32-bit Peek command."
write ";**********************************************************************"
;; Determine a valid EEPROM address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 4

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_DWORD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=32 MEM32 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - 32 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
else
  write "<!> Failed (1009;5002) - 32 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_PEEK_DWORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,64)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_DWORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Send the DumpInEvent command with the address -4 for 12 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM32 DataSize=12 SymName="" Offset=validAddr-4

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_INEVENT) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.10: Determine the pattern to poke based upon the value "
write ";  returned from the peek command above."
write ";**********************************************************************"
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

/$SC_$CPU_MM_Poke DataSize=32 MEM32 Data=pokeValue SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5000) - 32 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5000, "P"
else
  write "<!> Failed (1009;5000) - 32 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5000, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5000.1) - Expected Event Msg ",MM_POKE_DWORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_50001, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_POKE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
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
  write "<!> Failed (1009;5000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_DWORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_50001, "F"
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
/$SC_$CPU_MM_Peek DataSize=32 MEM32 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - 32 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
else
  write "<!> Failed (1009;5002) - 32 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_PEEK_DWORD_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,64)
  write "; Parsed text value = '", textValueRead
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_PEEK) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
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
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_DWORD_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Send the DumpInEvent command with the address -4 for 12 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM32 DataSize=12 SymName="" Offset=validAddr-4

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5002, "A"
else
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5002, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.13: Send a Poke command with an invalid address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
invalidAddr = 12

/$SC_$CPU_MM_Poke DataSize=32 MEM32 Data=pokeValue SymName="" Offset=invalidAddr

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

/$SC_$CPU_MM_Poke DataSize=4 MEM8 Data=pokeValue SymName="" Offset=validAddr

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
write ";  Step 2.16: Send a Peek command with an invalid address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_Peek DataSize=32 MEM32 SymName="" Offset=invalidAddr

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
rawcmd = "1888c000004902752000"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004902752000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004902752000"
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

/$SC_$CPU_MM_Peek DataSize=3 MEM16 SymName="" Offset=validAddr

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
write ";  Step 3.0: MM Read tests."
write ";**********************************************************************"
write ";  Step 3.1: Send a Load from file command to a valid Memory Mapped "
write ";  address. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress + 1
LOCAL dataToSend = 1024

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=validAddr Pattern=x'AF' MEM8 SymbolName="" Filename="/ram/validmm8load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/validmm8load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;; 
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/validmm8load.dat") THEN
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
write ";  Step 3.2: Send a Dump to file command with the address and number of"
write ";  bytes specified in the step above in order to verify the write. "
write ";  The area written above is dumped to a file for verification. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_mm8_32_dump.dat", "$CPU", appPktID, MM_MEM8, dataToSend, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_mm8_32_dump.dat","$CPU",appPktID,"MEM8",dataToSend,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_TO_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/mm_mm8_32_dump.dat") THEN
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
s ftp_file (ramDir,"na","mm_mm8_32_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 3.3: Send a read command with the bytes to be read larger than "
write ";  the maximum event message size. The maximum data bytes that can be "
write ";  dumped in an event message is calculated at the top of this "
write ";  procedure. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

local eventBytes = MM_MAX_DUMP_INEVENT_BYTES+1 

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM32 DataSize=eventBytes SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5004.1) - Dump In Event command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_50041, "P"
else
  write "<!> Failed (1010;5004.1) - Dump In Event command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_50041, "F"
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
write ";  Step 3.4: Send a read command with the bytes to be read equal to the"
write ";  maximum event message size. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM16 DataSize=MM_MAX_DUMP_INEVENT_BYTES SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.5: Send a read command to read a single byte."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM8 DataSize=1 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.6: Send a read command with an invalid address."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_DumpInEvent MEM32 DataSize=10 SymName="" Offset=invalidAddr

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
write ";  Step 3.7: Send a read command with an invalid command length."
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
write ";  Step 3.8: Send a read command with invalid arguments. "
write ";**********************************************************************"
write ";  Step 3.8.1: Send command with an invalid Memory Type."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_MEMTYPE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;; CPU1 is the default
rawcmd = "1888c000004907BD00"

if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004907BD00"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004907BD00"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

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
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.8.2: Send command specifying 0 bytes."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_DumpInEvent MEM16 DataSize=0 SymName="" Offset=validAddr

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
write ";  Step 4.0: MM File command tests."
write ";**********************************************************************"
write ";  Step 4.1: Send a Load from File command with a valid load file for "
write ";  the MEM16 memory type."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress
dataToSend = 1024

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=validAddr Pattern=x'AF' MEM16 SymbolName="" Filename="/ram/validmm16load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/validmm16load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;; 
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/validmm16load.dat") THEN
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
write ";  Step 4.2: Send a Dump to File command using the address and number "
write ";  of bytes specified in the file loaded above. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_mm16_42_dump.dat", "$CPU", appPktID, MM_MEM16, dataToSend, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_mm16_42_dump.dat","$CPU",appPktID,"MEM16",dataToSend,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_TO_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/mm_mm16_42_dump.dat") THEN
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
s ftp_file (ramDir,"na","mm_mm16_42_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.3: Send a Load from File command with an invalid CRC "
write ";  specified in the load file for the MEM16 memory type."
write ";**********************************************************************"
;; Get a valid MEM32 Address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 2

;; Set the EEPROM memory address where the load starts
LOCAL hdrVar = varPktID & "ADDROFFSET"
{hdrVar} = validAddr
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the symbol name to NULL
hdrVar = varPktID & "SYMNAME"
{hdrVar} = ""
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the number of bytes to load
dataToSend = 2048
hdrVar = varPktID & "NUMBYTES"
{hdrVar} = dataToSend
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the invalid CRC for the data being loaded
hdrVar = varPktID & "CRC"
{hdrVar} = 1234
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Set the Memory Type
hdrVar = varPktID & "MEMTYPE"
{hdrVar} = MM_MEM16
write "Step 4.3: ", hdrVar, " = ", {hdrVar}

;; Create the Load File
s create_mm_file_from_cvt("$CPU",varPktID,appPktID,hexAppID,"MEM16 Load File with an invalid CRC","badmem16crcload.dat")

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LOAD_FILE_CRC_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Transfer the file to s/c and issue the MM LoadFile command
s load_memory ("badmem16crcload.dat", "$CPU")

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5100.1) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51001, "P"
else
  write "<!> Failed (1010;5100.1) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51001, "F"
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
s ftp_file (ramDir,"na","badmem16crcload.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.4: Send a Load from File command with a load file that exceeds"
write ";  the <PLATFORM_DEFINED> maximum number of bytes for MEM16. "
write ";**********************************************************************"
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the error files one is used by this step
;; and the other two are used in Steps 4.8 and 4.9
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateErrorFiles Address=validAddr Pattern=x'FA' MEM16
wait 5

if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateFile command did not increment CMDPC."
endif

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/overmaxload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5100.2) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51002, "P"
else
  write "<!> Failed (1010;5100.2) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51002, "F"
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
write ";  contains exactly the <PLATFORM_DEFINED> maximum number of bytes for"
write ";  the MEM16 memory type."
write ";**********************************************************************"
write ";  Step 4.5.1: Create the load file using the test application. "
write ";**********************************************************************";
; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=MM_MAX_LOAD_FILE_DATA_MEM16 Address=validAddr Pattern=x'AF' MEM16 SymbolName="" Filename="/ram/maxdatamm16load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/maxdatamm16load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;; 
     ($SC_$CPU_MM_BytesProc = MM_MAX_LOAD_FILE_DATA_MEM16) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/maxdatamm16load.dat") THEN
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
write ";**********************************************************************";
; Get the onboard file to the ground
s ftp_file (ramDir,"maxdatamm16load.dat","maxdatamm16load.dat","$CPU","G")

;; Delete the onboard file
s ftp_file (ramDir,"na","maxdatamm16load.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.6: Send a Load from File command with a load file that "
write ";  contains the minimum amount of data for MEM16. "
write ";**********************************************************************"
write ";  Step 4.6.1: Create and Load a file that contains one (1) byte of "
write ";  data for MEM16. This should fail since the minimum load for MEM16 "
write ";  should be 2 bytes. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=1 Address=validAddr Pattern=x'55' MEM16 SymbolName="" Filename="/ram/onebytemm16load.dat"
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
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_ALIGN16_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/onebytemm16load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command failed as expected."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_ALIGN16_ERR_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_ALIGN16_ERR_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.6.2: Create and load file that contains two (2) bytes of data"
write ";  for MEM16. This should work. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress + 2

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=2 Address=validAddr Pattern=x'AA' MEM16 SymbolName="" Filename="/ram/twobytemm16load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/twobytemm16load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
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
write ";  Step 4.7: Send a Load from File command with a load file that "
write ";  contains an invalid MEM16 destination address."
write ";**********************************************************************"
;; Create the Load File
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=invalidAddr Pattern=x'AF' MEM8 SymbolName="" Filename="/ram/badmm16addrload.dat"

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Transfer the file to s/c and issue the MM LoadFile command
s load_memory ("badmm16addrload.dat", "$CPU")

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
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.8: Send a Load from File command with a load file whose file"
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
write ";  Step 4.9: Send a Load from File command with a load file whose file"
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
write ";  Step 4.10: Send a Load from File command with a valid load file for "
write ";  the MEM8 memory type. The load file was created in Step 3.1 above.  "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/validmm8load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
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
write ";  Step 4.11: Send a Dump to File command using the address and number "
write ";  of bytes specified in the file loaded above. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_mm8_411_dump.dat", "$CPU", appPktID, MM_MEM8, dataToSend, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_mm8_411_dump.dat","$CPU",appPktID,"MEM8",dataToSend,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_mm8_411_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.12: Send a Load from File command with an invalid CRC "
write ";  specified in the load file for the MEM8 memory type."
write ";**********************************************************************"
;; Get a valid MEM8 Address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 2

;; Set the EEPROM memory address where the load starts
hdrVar = varPktID & "ADDROFFSET"
{hdrVar} = validAddr
write "Step 4.12: ", hdrVar, " = ", {hdrVar}

;; Set the symbol name to NULL
hdrVar = varPktID & "SYMNAME"
{hdrVar} = ""
write "Step 4.12: ", hdrVar, " = ", {hdrVar}

;; Set the number of bytes to load
dataToSend = 2048
hdrVar = varPktID & "NUMBYTES"
{hdrVar} = dataToSend
write "Step 4.12: ", hdrVar, " = ", {hdrVar}

;; Set the invalid CRC for the data being loaded
hdrVar = varPktID & "CRC"
{hdrVar} = 1234
write "Step 4.12: ", hdrVar, " = ", {hdrVar}

;; Set the Memory Type
hdrVar = varPktID & "MEMTYPE"
{hdrVar} = MM_MEM8
write "Step 4.12: ", hdrVar, " = ", {hdrVar}

;; Create the Load File
s create_mm_file_from_cvt("$CPU",varPktID,appPktID,hexAppID,"MEM8 Load File with an invalid CRC","badmem8crcload.dat")

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LOAD_FILE_CRC_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Transfer the file to s/c and issue the MM LoadFile command
s load_memory ("badmem8crcload.dat", "$CPU")

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5100.1) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51001, "P"
else
  write "<!> Failed (1010;5100.1) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51001, "F"
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
s ftp_file (ramDir,"na","badmem8crcload.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.13: Send a Load from File command with a load file that "
write ";  exceeds the <PLATFORM_DEFINED> maximum number of bytes for MEM8. "
write ";**********************************************************************"
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the error files one is used by this step
;; and the other two are used in Steps 4.17 and 4.18
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateErrorFiles Address=validAddr Pattern=x'FA' MEM8
wait 5

if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateErrorFiles command did not increment CMDPC."
endif

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/overmaxload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5100.2) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51002, "P"
else
  write "<!> Failed (1010;5100.2) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51002, "F"
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
write ";  Step 4.14: Send a Load from File command with a load file that "
write ";  contains exactly the <PLATFORM_DEFINED> maximum number of bytes for"
write ";  the MEM8 memory type."
write ";**********************************************************************"
write ";  Step 4.14.1: Create the load file using the test application. "
write ";**********************************************************************";
; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=MM_MAX_LOAD_FILE_DATA_MEM8 Address=validAddr Pattern=x'AF' MEM8 SymbolName="" Filename="/ram/maxdatamm8load.dat"
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
write ";  Step 4.14.2: Send the load file command. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/maxdatamm8load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;; 
     ($SC_$CPU_MM_BytesProc = MM_MAX_LOAD_FILE_DATA_MEM8) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/maxdatamm8load.dat") THEN
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
write ";  Step 4.14.3: Download the load file in order to use it again later "
write ";  and delete the onboard file. "
write ";**********************************************************************";
; Get the onboard file
s ftp_file (ramDir,"maxdatamm8load.dat","maxdatamm8load.dat","$CPU","G")

;; Delete the onboard file
s ftp_file (ramDir,"na","maxdatamm8load.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.15: Send a Load from File command with a load file that "
write ";  contains exactly one (1) byte of data for MEM8. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress + 1

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=1 Address=validAddr Pattern=x'55' MEM8 SymbolName="" Filename="/ram/onebytemm8load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/onebytemm8load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
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
write ";  Step 4.16: Send a Load from File command with a load file that "
write ";  contains an invalid MEM8 destination address."
write ";**********************************************************************"
;; Create the Load File
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=invalidAddr Pattern=x'AF' MEM8 SymbolName="" Filename="/ram/badmm8addrload.dat"

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/badmm8addrload.dat"

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
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.17: Send a Load from File command with a load file whose file"
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
write ";  Step 4.18: Send a Load from File command with a load file whose file"
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
write ";  Step 4.19: Send a Load from File command with a valid load file for "
write ";  the MEM32 memory type."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress
dataToSend = 1024

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=validAddr Pattern=x'AF' MEM32 SymbolName="" Filename="/ram/validmm32load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/validmm32load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;; 
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/validmm32load.dat") THEN
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
write ";  Step 4.20: Send a Dump to File command using the address and number "
write ";  of bytes specified in the file loaded above. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_m32_420_dump.dat", "$CPU", appPktID, MM_MEM32, dataToSend, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_m32_420_dump.dat","$CPU",appPktID,"MEM32",dataToSend,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_DMP_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_DUMP_TO_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_BytesProc = dataToSend) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/mm_m32_420_dump.dat") THEN
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
s ftp_file (ramDir,"na","mm_m32_420_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.21: Send a Load from File command with an invalid CRC "
write ";  specified in the load file for the MEM32 memory type."
write ";**********************************************************************"
;; Get a valid MEM32 Address
validAddr = $SC_$CPU_TST_MM_RAMAddress + 4

;; Set the EEPROM memory address where the load starts
hdrVar = varPktID & "ADDROFFSET"
{hdrVar} = validAddr
write "Step 4.21: ", hdrVar, " = ", {hdrVar}

;; Set the symbol name to NULL
hdrVar = varPktID & "SYMNAME"
{hdrVar} = ""
write "Step 4.21: ", hdrVar, " = ", {hdrVar}

;; Set the number of bytes to load
dataToSend = 2048
hdrVar = varPktID & "NUMBYTES"
{hdrVar} = dataToSend
write "Step 4.21: ", hdrVar, " = ", {hdrVar}

;; Set the invalid CRC for the data being loaded
hdrVar = varPktID & "CRC"
{hdrVar} = 1234
write "Step 4.21: ", hdrVar, " = ", {hdrVar}

;; Set the Memory Type
hdrVar = varPktID & "MEMTYPE"
{hdrVar} = MM_MEM32
write "Step 4.21: ", hdrVar, " = ", {hdrVar}

;; Create the Load File
s create_mm_file_from_cvt("$CPU",varPktID,appPktID,hexAppID,"MEM32 Load File with an invalid CRC","badmem32crcload.dat")

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LOAD_FILE_CRC_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Transfer the file to s/c and issue the MM LoadFile command
s load_memory ("badmem32crcload.dat", "$CPU")

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5100.1) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51001, "P"
else
  write "<!> Failed (1010;5100.1) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51001, "F"
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
s ftp_file (ramDir,"na","badmem32crcload.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.22: Send a Load from File command with a load file that "
write ";  exceeds the <PLATFORM_DEFINED> maximum number of bytes for MEM32. "
write ";**********************************************************************"
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the error files one is used by this step
;; and the other two are used in Steps 4.26 and 4.27
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateErrorFiles Address=validAddr Pattern=x'FA' MEM32
wait 5

if (UT_TW_Status <> UT_Success) then
  write "<!> Failed - TST_MM_CreateFile command did not increment CMDPC."
endif

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/overmaxload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5100.2) - LoadFile command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51002, "P"
else
  write "<!> Failed (1010;5100.2) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51002, "F"
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
write ";  Step 4.23: Send a Load from File command with a load file that "
write ";  contains exactly the <PLATFORM_DEFINED> maximum number of bytes for"
write ";  the MEM8 memory type."
write ";**********************************************************************"
write ";  Step 4.23.1: Create the load file using the test application. "
write ";**********************************************************************";
; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1
validAddr = $SC_$CPU_TST_MM_RAMAddress

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=MM_MAX_LOAD_FILE_DATA_MEM32 Address=validAddr Pattern=x'AF' MEM32 SymbolName="" Filename="/ram/maxdatamm32load.dat"
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
write ";  Step 4.23.2: Send the load file command. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/maxdatamm32load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LD_MEM_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_LOAD_FROM_FILE) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;; 
     ($SC_$CPU_MM_BytesProc = MM_MAX_LOAD_FILE_DATA_MEM32) AND ;;
     ($SC_$CPU_MM_LastFile = "/ram/maxdatamm32load.dat") THEN
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
write ";  Step 4.23.3: Download the load file in order to use it again later "
write ";  and delete the onboard file. "
write ";**********************************************************************";
; Get the onboard file
s ftp_file (ramDir,"maxdatamm32load.dat","maxdatamm32load.dat","$CPU","G")

;; Delete the onboard file
s ftp_file (ramDir,"na","maxdatamm32load.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.24: Send a Load from File command with a load file that "
write ";  contains the minimum amount of data for MEM32. "
write ";**********************************************************************"
write ";  Step 4.24.1: Create and load a file that contains one (1) byte of "
write ";  data for MEM32. This step should fail since the minimum load for "
write ";  MEM32 is 4 bytes."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress + 4

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=1 Address=validAddr Pattern=x'55' MEM32 SymbolName="" Filename="/ram/onebytemm32load.dat"
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
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_ALIGN32_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/onebytemm32load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command failed as expected."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDEC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_ALIGN32_ERR_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_ALIGN32_ERR_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.24.2: Create and load a file that contains four (4) bytes of "
write ";  data for MEM32. This step should work. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress + 4

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=4 Address=validAddr Pattern=x'BA' MEM32 SymbolName="" Filename="/ram/minmem32load.dat"
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
/$SC_$CPU_MM_LoadFile Filename="/ram/minmem32load.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5100, "P"
else
  write "<!> Failed (1009;5100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5100, "F"
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
write ";  Step 4.25: Send a Load from File command with a load file that "
write ";  contains an invalid MEM32 destination address."
write ";**********************************************************************"
;; Create the Load File
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=invalidAddr Pattern=x'AF' MEM32 SymbolName="" Filename="/ram/badmm32addrload.dat"

;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/badmm32addrload.dat"

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
write ";  Step 4.26: Send a Load from File command with a load file whose file"
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
write ";  Step 4.27: Send a Load from File command with a load file whose file"
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
write ";  Step 4.28: Send a Load from File command with an invalid command "
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
write ";  Step 4.29: Send a Load from File command with an filename that does "
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
  write "<!> Failed (1009;2104) - Invalid LoadFile command did not increment CMDEC."
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
write ";  Step 4.30: Send a Load from File command with invalid arguments. "
write ";  The only argument is the filename. Send multiple commands that "
write ";  attempt to load invalid filenames. One test should use a NULL. "
write ";**********************************************************************"
write ";  Step 4.30.1: Send command with a NULL Filename. "
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
  write "<!> Failed (1008;2104) - Invalid LoadFile command did not increment CMDEC."
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
write ";  Step 4.30.2: Send command with a Filename containing invalid "
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
  write "<!> Failed (1008;2104) - Invalid LoadFile command did not increment CMDEC."
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
write ";  Step 4.31: Send a Dump to File command with the number of bytes "
write ";  greater than the <PLATFORM_DEFINED> maximum number of bytes for the "
write ";  MEM16 memory type."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM16 DataSize=MM_MAX_DUMP_FILE_DATA_MEM16+1 SymName="" Offset=validAddr Filename="/ram/mm_m16_431_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5104.1) -  Dump2File command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51041, "P"
else
  write "<!> Failed (1010;5104.1) - Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51041, "F"
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
write ";  Step 4.32: Send a Dump to File command with the number of bytes "
write ";  equal to the <PLATFORM_DEFINED> maximum number of bytes for MEM16. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_m16_432_dump.dat", "$CPU", appPktID, MM_MEM16, MM_MAX_DUMP_FILE_DATA_MEM16, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_m16_432_dump.dat","$CPU",appPktID,"MEM16",MM_MAX_DUMP_FILE_DATA_MEM16,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) -  Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_m16_432_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.33: Minimum Dump to File commands for MEM16."
write ";**********************************************************************"
write ";  Step 4.33.1: Send a Dump to File command with the number of bytes "
write ";  equal to one (1) byte for MEM16. This should fail since the minimum "
write ";  dump for MEM16 is 2 bytes. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_ALIGN16_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm16_4331_dump.dat", "$CPU", appPktID, MM_MEM16, 1, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm16_4331_dump.dat","$CPU",appPktID,"MEM16",1,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command failed as expected."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_ALIGN16_ERR_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_ALIGN16_ERR_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.33.2: Send a Dump to File command with the number of bytes "
write ";  equal to two (2) bytes for MEM16. This should work. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm16_4332_dump.dat", "$CPU", appPktID, MM_MEM16, 2, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm16_4332_dump.dat","$CPU",appPktID,"MEM16",2,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
write ";  Step 4.34: Send a Dump to File command with an invalid MEM16 addr. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM16 DataSize=dataToSend SymName="" Offset=invalidAddr Filename="/ram/mm_m16_434_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) -  Dump2File command failed as expected."
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
write ";  Step 4.35: Send a Dump to File command with the number of bytes "
write ";  greater than the <PLATFORM_DEFINED> maximum number of bytes for the "
write ";  MEM8 memory type."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM8 DataSize=MM_MAX_DUMP_FILE_DATA_MEM8+1 SymName="" Offset=validAddr Filename="/ram/mm_m8_435_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5104.1) -  Dump2File command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51041, "P"
else
  write "<!> Failed (1010;5104.1) - Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51041, "F"
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
s ftp_file (ramDir,"na","mm_m8_435_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.36: Send a Dump to File command with the number of bytes "
write ";  equal to the <PLATFORM_DEFINED> maximum number of bytes for MEM8. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_m8_436_dump.dat", "$CPU", appPktID, MM_MEM8, MM_MAX_DUMP_FILE_DATA_MEM8, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_m8_436_dump.dat","$CPU",appPktID,"MEM8",MM_MAX_DUMP_FILE_DATA_MEM8,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) -  Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_m8_436_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.37: Send a Dump to File command with the number of bytes "
write ";  equal to one (1) byte for MEM8. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_m8_437_dump.dat", "$CPU", appPktID, MM_MEM8, 1, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_m8_437_dump.dat","$CPU",appPktID,"MEM8",1,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
write ";  Step 4.38: Send a Dump to File command with an invalid MEM8 addr. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM8 DataSize=dataToSend SymName="" Offset=invalidAddr Filename="/ram/mm_m8_438_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) -  Dump2File command failed as expected."
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
write ";  Step 4.39: Send a Dump to File command with the number of bytes "
write ";  greater than the <PLATFORM_DEFINED> maximum number of bytes for the "
write ";  MEM32 memory type."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM32 DataSize=MM_MAX_DUMP_FILE_DATA_MEM32+1 SymName="" Offset=validAddr Filename="/ram/mm_m32_439_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5104.1) -  Dump2File command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_51041, "P"
else
  write "<!> Failed (1010;5104.1) - Dump2File command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_51041, "F"
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
write ";  Step 4.40: Send a Dump to File command with the number of bytes "
write ";  equal to the <PLATFORM_DEFINED> maximum number of bytes for MEM32. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_m32_440_dump.dat", "$CPU", appPktID, MM_MEM32, MM_MAX_DUMP_FILE_DATA_MEM32, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_m32_440_dump.dat","$CPU",appPktID,"MEM32",MM_MAX_DUMP_FILE_DATA_MEM32,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) -  Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_m32_440_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 4.41: Minimum Dump to File commands for MEM32."
write ";**********************************************************************"
write ";  Step 4.41.1: Send a Dump to File command with the number of bytes "
write ";  equal to one (1) byte for MEM32. This should fail since the minimum "
write ";  number of bytes is 4."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_ALIGN32_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm32_4411_dump.dat", "$CPU", appPktID, MM_MEM32, 1, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm32_4411_dump.dat","$CPU",appPktID,"MEM32",1,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command failed as expected."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_ALIGN32_ERR_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_ALIGN32_ERR_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.41.2: Send a Dump to File command with the minimum number of "
write ";  bytes for MEM32 which is 4. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm32_4412_dump.dat", "$CPU", appPktID, MM_MEM32, 4, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm32_4412_dump.dat","$CPU",appPktID,"MEM32",4,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
write ";  Step 4.42: Send a Dump to File command with an invalid MEM32 addr. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM32 DataSize=dataToSend SymName="" Offset=invalidAddr Filename="/ram/mm_m32_442_dump.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) -  Dump2File command failed as expected."
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
write ";  Step 4.43: Send a Dump to File command with an invalid command "
write ";  length. "
write ";**********************************************************************"
write "; Invalid address determination not implemented in cFE yet!!!"
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
write ";  Step 4.44: Send a Dump to File command with an invalid filename. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_CREAT_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM16 DataSize=dataToSend SymName="" Offset=validAddr Filename="mm_bad_dumpname.dat"

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
write ";  Step 4.45: Send a Dump to File command with invalid arguments. "
write ";**********************************************************************"
write ";  Step 4.45.1: Send command with invalid memory type. "
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
  write "<*> Passed (1010) - Expected Event Msg ",MM_MEMTYPE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_MEMTYPE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.45.2: Send command with data size = 0. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM32 DataSize=0 SymName="" Offset=validAddr Filename="/ram/mm_dump_4_452.dat"

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
  write "<*> Passed (1010) - Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_OS_MEMVALIDATE_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 4.45.3: Send command with a NULL dump filename. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_CMD_FNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Dump2File MEM8 DataSize=100 SymName="" Offset=validAddr Filename=""

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
write ";  Step 5.1: Send a Fill command to a valid MEM8 address."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM8 DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFFFFFFF' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'FF') AND ;;
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
/$SC_$CPU_MM_DumpInEvent MEM8 DataSize=MM_MAX_DUMP_INEVENT_BYTES SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.3: Send a Fill command with the command-specified number of "
write ";  bytes greater than the <PLATFORM_DEFINED> maximum number of bytes "
write ";  for the MEM8 memory type. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM8 DataSize=MM_MAX_FILL_DATA_MEM8+1 Pattern=x'AAAAAAAA' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5300.1) - Fill command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_53001, "P"
else
  write "<!> Failed (1009;5300.1) - Invalid Fill command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_53001, "F"
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
/$SC_$CPU_MM_Fill MEM8 DataSize=MM_MAX_FILL_DATA_MEM8 Pattern=x'55555555' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM8) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'55') AND ;;
     ($SC_$CPU_MM_BytesProc = MM_MAX_FILL_DATA_MEM8) AND ;;
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
;;s get_mm_file_to_cvt(ramDir, "mm_mm8_55_dump.dat", "$CPU", appPktID, MM_MEM8, MM_MAX_FILL_DATA_MEM8, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_mm8_55_dump.dat","$CPU",appPktID,"MEM8",MM_MAX_FILL_DATA_MEM8,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_mm8_55_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 5.6: Send a Fill command with the command-specified number of "
write ";  bytes equal to one (1) byte for MEM8 memory type."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM8 DataSize=1 Pattern=x'BBBBBBBB' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
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
/$SC_$CPU_MM_DumpInEvent MEM8 DataSize=3 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.8: Send a Fill command with an invalid MEM8 address. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM8 DataSize=10 Pattern=x'FFFFFFFF' SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Fill command sent properly."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Fill command did not increment CMDPC."
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
write ";  Step 5.9: Send a Fill command to a valid MEM16 address."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM16 DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFFFFFFF' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'FFFF') AND ;;
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
write ";  Step 5.10: Send a Read command with the address and number of bytes "
write ";  specified in the above step. Verify the Fill command worked. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM16 DataSize=MM_MAX_DUMP_INEVENT_BYTES SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.11: Send a Fill command with the command-specified number of "
write ";  bytes greater than the <PLATFORM_DEFINED> maximum number of bytes "
write ";  for the MEM16 memory type. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM16 DataSize=MM_MAX_FILL_DATA_MEM16+1 Pattern=x'AAAAAAAA' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5300.1) - Fill command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_53001, "P"
else
  write "<!> Failed (1009;5300.1) - Invalid Fill command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_53001, "F"
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
write ";  Step 5.12: Send a Fill command with the command-specified number of "
write ";  bytes equal to the <PLATFORM_DEFINED> maximum number of bytes."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM16 DataSize=MM_MAX_FILL_DATA_MEM16 Pattern=x'55555555' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM16) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'5555') AND ;;
     ($SC_$CPU_MM_BytesProc = MM_MAX_FILL_DATA_MEM16) AND ;;
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
write ";  Step 5.13: Send a Dump to File command using the arguments from the "
write ";  Fill command in the step above. Verify the Fill command worked."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_mm16_513_dump.dat", "$CPU", appPktID, MM_MEM16, MM_MAX_FILL_DATA_MEM16, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_mm16_513_dump.dat","$CPU",appPktID,"MEM16",MM_MAX_FILL_DATA_MEM16,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_mm16_513_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 5.14: Send a Fill command with the command-specified number of "
write ";  bytes equal to the minimum two (2) bytes for MEM16."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM16 DataSize=2 Pattern=x'BBBBBBBB' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
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
write ";  Step 5.15: Send a Read command with the address and number of bytes "
write ";  specified in the above step. Verify the Fill command worked. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM16 DataSize=6 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.16: Send a Fill command with an invalid MEM16 address. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM16 DataSize=10 Pattern=x'FFFFFFFF' SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Fill command sent properly."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Fill command did not increment CMDPC."
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
write ";  Step 5.17: Send a Fill command to a valid MEM32 address."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

validAddr = $SC_$CPU_TST_MM_RAMAddress

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM32 DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFFFFFFF' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
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
write ";  Step 5.18: Send a Read command with the address and number of bytes "
write ";  specified in the above step. Verify the Fill command worked. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM32 DataSize=MM_MAX_DUMP_INEVENT_BYTES SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.19: Send a Fill command with the command-specified number of "
write ";  bytes greater than the <PLATFORM_DEFINED> maximum number of bytes "
write ";  for the MEM32 memory type. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DATA_SIZE_BYTES_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM32 DataSize=MM_MAX_FILL_DATA_MEM32+1 Pattern=x'AAAAAAAA' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;5300.1) - Fill command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_53001, "P"
else
  write "<!> Failed (1009;5300.1) - Invalid Fill command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_53001, "F"
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
write ";  Step 5.20: Send a Fill command with the command-specified number of "
write ";  bytes equal to the <PLATFORM_DEFINED> maximum number of bytes."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM32 DataSize=MM_MAX_FILL_DATA_MEM32 Pattern=x'55555555' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_FILL_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the Last Command HK items
  if ($SC_$CPU_MM_LastActn = MM_FILL) AND ;;
     ($SC_$CPU_MM_MemType = MM_MEM32) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ;;
     ($SC_$CPU_MM_DataValue = x'55555555') AND ;;
     ($SC_$CPU_MM_BytesProc = MM_MAX_FILL_DATA_MEM32) AND ;;
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
write ";  Step 5.21: Send a Dump to File command using the arguments from the "
write ";  Fill command in the step above. Verify the Fill command worked."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Dump command and transfer the file to ground
;;s get_mm_file_to_cvt(ramDir, "mm_mm32_521_dump.dat", "$CPU", appPktID, MM_MEM32, MM_MAX_FILL_DATA_MEM32, "", validAddr)
s get_mm_file_to_cvt(ramDir,"mm_mm32_521_dump.dat","$CPU",appPktID,"MEM32",MM_MAX_FILL_DATA_MEM32,"",validAddr)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5104) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5104, "A"
else
  write "<!> Failed (1009;5104) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5104, "F"
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
s ftp_file (ramDir,"na","mm_mm32_521_dump.dat","$CPU","R")

wait 5

write ";**********************************************************************"
write ";  Step 5.22: Send a Fill command with the command-specified number of "
write ";  bytes equal to the minimum four (4) bytes for MEM32."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM32 DataSize=4 Pattern=x'BBBBBBBB' SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5300) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5300, "P"
else
  write "<!> Failed (1009;5300) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5300, "F"
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
write ";  Step 5.23: Send a Read command with the address and number of bytes "
write ";  specified in the above step. Verify the Fill command worked. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent MEM32 DataSize=12 SymName="" Offset=validAddr

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;5004) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_5004, "A"
else
  write "<!> Failed (1009;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_5004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.24: Send a Fill command with an invalid MEM32 address. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM32 DataSize=10 Pattern=x'FFFFFFFF' SymName="" Offset=invalidAddr

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1007;1010) - Fill command sent properly."
  ut_setrequirements MM_1007, "P"
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1007;1010) - Fill command did not increment CMDPC."
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
write ";  Step 5.25: Send a Fill command with an invalid command length. "
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
write ";  Step 5.26: Send a Fill command with invalid arguments. "
write ";**********************************************************************"
write ";  Step 5.26.1: Send command with invalid memory type. "
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
write ";  Step 5.26.2: Send command with data size = 0. "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_OS_MEMVALIDATE_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_Fill MEM8 DataSize=0 Pattern=x'FAFAFAFA' SymName="" Offset=validAddr

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
write ";  Step 6.0: Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $SC_$CPU_mm_memmap                                  "
write ";*********************************************************************"
ENDPROC
