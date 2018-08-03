PROC $sc_$cpu_mm_symtab
;*******************************************************************************
;  Test Name:  mm_symtab
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Memory Manager (MM) Symbol Table 
;	functionality works properly and that the MM application handles 
;	anomalies appropriately. This support is optional and thus provided in
;	a separate test.
;
;  Requirements Tested
;    MM1009	If MM accepts any command as valid, MM shall execute the 
;		command, increment the MM Valid Command Counter and issue an
;		event message.
;    MM1010	If MM rejects any command, MM shall abort the command execution,
;		increment the MM Command Rejected Counter and issue an error
;		event message.
;    MM1011	<OPTIONAL> Symbol Name and offset can be used in lieu of an
;		absolute address in any RAM command.
;    MM1012	<OPTIONAL> Symbol Name and offset can be used in lieu of an
;		absolute address in any EEPROM command.
;    MM1013	The MM application shall generate an error event if symbol table
;		operations are initiated but not supported in the current target
;		environment.
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
;		CRC matches the computed CRC of the data.
;    MM2004	Upon receipt of a Read command, MM shall read the 
;		command-specified number of consecutive bytes from the 
;		command-specified RAM memory address and generate and event
;		message containing the data.
;    MM2100	Upon receipt of a Load From File command, MM shall load RAM,
;		with interrupts enabled during the actual load, based on the
;		following information contained in the command-specified file: 
;			a. Destination Address
;			b. Destination Memory Type
;			c. <MISSION_DEFINED> CRC (data only)
;			d. Number of Bytes to Load
;    MM2104	Upon receipt of a Dump to File command, MM shall write the data
;		associated with the command-specified RAM address, 
;		command-specified number of bytes and calculated 
;		<MISSION_DEFINED> CRC to the command-specified file.
;    MM2300	Upon receipt of a Fill command, MM shall fill RAM with the
;		contents based on the following command-specified parameters:
;			a. Destination Address
;			b. Destination Memory Type
;			c. Number of Bytes to fill
;			d. 32-bit Fill Pattern
;    MM3000	Upon receipt of a Poke command, MM shall write 8,16, or 32 bits
;               of data to the command-specified EEPROM address.
;    MM3000.1	MM shall confirm a write to the EEPROM address by issuing an
;               event message which includes:
;                       a. address written
;                       b. length of data written
;                       c. value of the data written
;    MM3001	Upon receipt of a Peek command, MM shall read 8,16, or 32 bits
;               of data from the command-specified EEPROM address and generate
;               an event message containing the following data:
;                       a. address read
;                       b. length of data read
;                       c. value of the data read
;    MM3002	Upon receipt of a Read command, MM shall read the
;               command-specified number of consecutive bytes from the
;               command-specified EEPROM memory address and generate and event
;               message containing the data.
;    MM3100	Upon receipt of a Load From File command, MM shall load EEPROM,
;               with interrupts enabled during the actual load, based on the
;               following information contained in the command-specified file:
;                       a. Destination Address
;                       b. Destination Memory Type
;                       c. <MISSION_DEFINED> CRC (data only)
;                       d. Number of Bytes to Load
;    MM3104	Upon receipt of a Dump to File command, MM shall write the data
;               associated with the command-specified EEPROM address,
;               command-specified number of bytes and calculated
;               <MISSION_DEFINED> CRC to the command-specified file.
;    MM3200	Upon receipt of a Fill command, MM shall fill EEPROM with the
;               contents based on the following command-specified parameters:
;                       a. Destination Address
;                       b. Destination Memory Type
;                       c. Number of Bytes to fill
;                       d. 32-bit Fill Pattern
;    MM3400     Upon receipt of an Enable EEPROM command, MM shall enable the
;               command specified bank of EEPROM for writing.
;    MM3500     Upon receipt of a Disable EEPROM command, MM shall disable/lock
;               the command specified bank of EEPROM from being written to.
;    MM7001	Upon receipt of a Write Symbol Table command, MM shall save the
;		system symbol table to an onboard data file.
;    MM7002	Upon receipt of a Symbol-to-Address command, MM shall report the
;		resolved address in telemetry for the command-specified symbol.
;    MM7004	The MM application shall generate an error event and abort the
;		current operation if any symbolic name argument cannot be 
;		resolved to a valid address.
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
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	04/18/08	Walt Moleski	Original Procedure.
;       10/25/10        Walt Moleski    Replaced setupevt with setupevents and
;                                       added a variable for the app name
;       04/16/15        Walt Moleski    Updated to use the DataValue mnemonic
;                                       in place of FillPattern and updated the
;                                       requirements to add 3400 and 3500 and
;                                       the wording changes for 8000 and 9000
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
#include "mm_msgdefs.h"

%liv (log_procedure) = logging

#define MM_1009		0
#define MM_1010		1
#define MM_1011		2
#define MM_1012		3
#define MM_1013		4
#define MM_2000		5
#define MM_20001	6
#define MM_2002		7
#define MM_2003		8
#define MM_20031	9
#define MM_2004		10
#define MM_2100 	11
#define MM_2104		12
#define MM_2300		13
#define MM_3000		14
#define MM_30001	15
#define MM_3001		16
#define MM_3002		17
#define MM_3100		18
#define MM_3104		19
#define MM_3200		20
#define MM_3400		21
#define MM_3500		22
#define MM_7001		23
#define MM_7002		24
#define MM_7004		25
#define MM_8000		26
#define MM_9000		27

global ut_req_array_size = 27
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["MM_1009", "MM_1010", "MM_1011", "MM_1012", "MM_1013", "MM_2000", "MM_2000.1", "MM_2002", "MM_2003", "MM_2003.1", "MM_2004", "MM_2100", "MM_2104", "MM_2300", "MM_3000", "MM_3000.1", "MM_3001", "MM_3002", "MM_3100", "MM_3104", "MM_3200", "MM_3400", "MM_3500", "MM_7001", "MM_7002", "MM_7004", "MM_8000", "MM_9000"]

;**********************************************************************
; Define local variables
;**********************************************************************
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
LOCAL validCRC
LOCAL rawcmd
LOCAL MMAppName = "MM"
LOCAL ramDir = "RAM:0"

;; This define is calculated in the mm_dump.c file and could not be used
;; directly from the MM source. Thus, if this size changes, this calculation
;; must be changed here
#define MM_MAX_DUMP_INEVENT_BYTES ((CFE_EVS_MAX_MESSAGE_LENGTH - (13 + 25)) / 5)

;; Determine the Packet IDs for the MM Load/Dump and Symbol Table file CVTs
;; CPU1 is the default
local varPktID = "PF0B"
local appPktID = "P0F0B"
local hexAppID = x'0F0B'
local symTabPktID = "P0F0F"

if ("$CPU" = "CPU2") then
  varPktID = "PF2B"
  appPktID = "P0F2B"
  hexAppID = x'0F2B'
  symTabPktID = "P0F2F"
elseif ("$CPU" = "CPU3") then
  varPktID = "PF4B"
  appPktID = "P0F4B"
  hexAppID = x'0F4B'
  symTabPktID = "P0F4F"
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
page $SC_$CPU_MM_SYMBOL_TBL
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
  write "  Data Value    = ",$SC_$CPU_MM_DataValue
  write "  Bytes processed = ",$SC_$CPU_MM_BytesProc
  write "  Filename        = ",$SC_$CPU_MM_LastFile
  ut_setrequirements MM_9000, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.0: Valid Symbol tests."
write ";**********************************************************************"
write ";  Step 2.1: Send the Write Symbol Table command."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMTBL_TO_FILE_INF_EID,"INFO",1
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMTBL_TO_FILE_FAIL_ERR_EID,"ERROR",2

cmdCtr = $SC_$CPU_MM_CMDPC + 1

;; Send the command
/$SC_$CPU_MM_SymTbl2File FileName="/ram/symbolFile.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;7001) - Write Symbol Table command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_7001, "P"
else
  write "<!> Failed (1009;7001) - Write Symbol Table command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_7001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_SYMTBL_TO_FILE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMTBL_TO_FILE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (1013) - Symbol Table operations are not supported on this platform."
  ut_setrequirements MM_1013, "P"
else
  write "<*> Requirement 1013 could not be tested since Symbol Operations are supported on this platform."
  ;; Download the file
  s ftp_file (ramDir,"symbolFile.dat","symbolFile.dat","$CPU","G")
  wait 10

  FILE_TO_CVT %name("symbolFile.dat") %name(symTabPktID)

endif

wait 5

write ";**********************************************************************"
write ";  Step 2.2: Send an 8-bit Peek command using a valid RAM symbol name."
write ";**********************************************************************"
LOCAL textValueRead

;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 RAM SymName="validRAM" Offset=1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2002;7002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2002, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2002;7002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2002, "F"
  ut_setrequirements MM_7002, "F"
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
/$SC_$CPU_MM_DumpInEvent RAM DataSize=3 SymName="validRAM" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2004;7002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2004, "A"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2004;7002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2004, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.3: Determine the pattern to poke based upon the value returned"
write ";  from the peek command above."
write ";**********************************************************************"
LOCAL pokeValue;

if (textValueRead <> poke8Pattern1) then
  pokeValue = poke8Value1
else
  pokeValue = poke8Value2
endif

write ";**********************************************************************"
write ";  Step 2.4: Send an 8-bit Poke command using the symbol name specified"
write ";  in Step 2.2 above."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_POKE_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_Poke DataSize=8 RAM Data=pokeValue SymName="validRAM" Offset=1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2000;7002) - 8 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2000, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;101l;2000;7002) - 8 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2000, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2000.1) - Expected Event Msg ",MM_POKE_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_20001, "P"
else
  write "<!> Failed (1009;2000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_20001, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.5: Send an 8-bit Peek command to verify the Poke command above"
write ";  wrote the correct data. Check the byte before & after"
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 RAM SymName="validRAM" Offset=1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2002;7002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2002, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2002;7002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2002, "F"
  ut_setrequirements MM_7002, "F"
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
;; Compare the value read from the event with the pattern written
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
/$SC_$CPU_MM_DumpInEvent RAM DataSize=3 SymName="validRAM" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2004;7002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2004, "A"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2004;7002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2004, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.6: Send a write with interrupts disabled command using a valid"
write ";  RAM symbol name. "
write ";**********************************************************************"
;; Setup the data array to load
local widDataArray[1 .. MM_MAX_UNINTERRUPTABLE_DATA]
local testDataArray[1 .. 256]
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
/$SC_$CPU_MM_LoadWID DataSize=dataToSend CRC=validCRC SymName="validRAM" Offset=0 Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2003;2003.1;7002) - LoadWID command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2003, "P"
  ut_setrequirements MM_20031, "P"
  ut_setrequirements MM_7002, "A"
else
  write "<!> Failed (1009;1011;2003;2003.1;7002) - LoadWID command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2003, "F"
  ut_setrequirements MM_20031, "F"
  ut_setrequirements MM_7002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_LOAD_WID_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_LOAD_WID_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.7: Send a Load from File command with a valid RAM load file "
write ";  containing a valid RAM symbol name. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

dataToSend = 1024

;; Send the TST_MM command to create the file - will need to add Symbolname
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=0 Pattern=x'AF' RAM SymbolName="validRAM" Filename="/ram/validramload.dat"
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
  write "<*> Passed (1009;1011;2100;7002) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2100, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2100;7002) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2100, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.8: Send a Dump to file command with the RAM symbol name and "
write ";  number bytes specified in the step above in order to verify the load."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

;; Send the Dump command and transfer the file to ground
s get_mm_file_to_cvt(ramDir,"mm_st2_8_dump.dat","$CPU",appPktID,"RAM",dataToSend,"validRAM",0)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2104;7002) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2104, "A"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2104;7002) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2104, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.9: Send a Fill command using a valid RAM symbol name."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill RAM DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFAAEEBB' SymName="validRAM" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1011;2300;7002) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1011, "P"
  ut_setrequirements MM_2300, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1011;2300;7002) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1011, "F"
  ut_setrequirements MM_2300, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.10: Send a Peek command using a valid EEPROM symbol name."
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 EEPROM SymName="validEEPROM" Offset=1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1012;3001;7002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3001, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3001;7002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3001, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;2002) - Expected Event Msg ",MM_PEEK_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3001, "P"
;; Get the result (if possible)
;; Peek Command: Addr = 0x00000000 Size = 8 bits Data = 0x07
;;	Will have to parse the event msg that gets returned
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,57)
  write "; Parsed text value = '", textValueRead
else
  write "<!> Failed (1009;3001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3001, "F"
endif

;; Send the DumpInEvent command with the address -1 for 3 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent EEPROM DataSize=3 SymName="validEEPROM" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1012;3002;7002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3002, "A"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3002;7002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3002, "F"
  ut_setrequirements MM_7002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3002, "A"
else
  write "<!> Failed (1009;3002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3002, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.11: Determine the pattern to poke based upon the value "
write ";  returned from the peek command above."
write ";**********************************************************************"
if (textValueRead <> poke8Pattern1) then
  pokeValue = poke8Value1
else
  pokeValue = poke8Value2
endif

write ";**********************************************************************"
write ";  Step 2.12: Perform an 8-bit Poke command using the valid EEPROM "
write ";  symbol name specified in Step 2.10 above."
write ";**********************************************************************"
write ";  Step 2.12.1: Send the Enable EEPROM Write command."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_EEPROM_WRITE_ENA_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_EnableEEWrite Bank=0

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3400) - EEPROM Write Enable command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3400, "P"
else
  write "<!> Failed (1009;3400) - EEPROM Write Enable command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3400, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_EEPROM_WRITE_ENA_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_EEPROM_WRITE_ENA_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.12.2: Send the 8-bit Poke command with an offset of 1. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_POKE_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_Poke DataSize=8 EEPROM Data=pokeValue SymName="validEEPROM" Offset=1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1012;3000;7002) - 8 bit Poke command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3000, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3000;7002) - 8 bit Poke command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3000, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3000.1) - Expected Event Msg ",MM_POKE_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_30001, "P"
else
  write "<!> Failed (1009;3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_POKE_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_30001, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.12.3: Send the Disable EEPROM Write command."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_EEPROM_WRITE_DIS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_DisableEEWrite Bank=0

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3500) - EEPROM Write Disable command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3500, "P"
else
  write "<!> Failed (1009;3500) - EEPROM Write Disable command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3500, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_EEPROM_WRITE_DIS_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_EEPROM_WRITE_DIS_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.13: Send an 8-bit Peek command to verify the Poke command "
write ";  above wrote the correct data. Check the byte before & after"
write ";**********************************************************************"
;; Setup for the Peek Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_PEEK_BYTE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the Peek
/$SC_$CPU_MM_Peek DataSize=8 EEPROM SymName="validEEPROM" Offset=1

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1012;3001;7002) - 8 bit Peek command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3001, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3001;7002) - 8 bit Peek command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3001, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3001) - Expected Event Msg ",MM_PEEK_BYTE_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3001, "P"
;; Get the result (if possible)
;;	Will have to parse the event msg that gets returned
;; Value read should be = to pokeValue
  write "==== Event Text = '", $SC_$CPU_find_event[1].event_txt, "'"
  textValueRead = %substring($SC_$CPU_find_event[1].event_txt,56,57)
  write "; Parsed text value = '", textValueRead
;; Compare the value read from the event with the pattern written
else
  write "<!> Failed (1009;3001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_PEEK_BYTE_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3001, "F"
endif

;; Send the DumpInEvent command with the address -1 for 3 bytes
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DUMP_INEVENT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_DumpInEvent EEPROM DataSize=3 SymName="validEEPROM" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3002;7002) -  Dump In Event command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3002, "A"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3002;7002) - Dump In Event command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3002, "F"
  ut_setrequirements MM_7002, "F"
endif

;; Check if the correct event message was generated
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3002) - Expected Event Msg ",MM_DUMP_INEVENT_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3002, "A"
else
  write "<!> Failed (1009;3002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_DUMP_INEVENT_INF_EID,"."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3002, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.14: Send a Load from File command with an EEPROM load file "
write ";  containing a valid EEPROM symbol name. "
write ";**********************************************************************"
write ";  Step 2.14.1: Send a Load from File command with a valid EEPROM load "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

dataToSend = 1024

;; Send the TST_MM command to create the file
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=0 Pattern=x'AF' EEPROM SymbolName="validEEPROM" Filename="/ram/valideeload.dat"
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
write ";  Step 2.14.2: Send the Enable EEPROM Write command."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_EEPROM_WRITE_ENA_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_EnableEEWrite Bank=0

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3400) - EEPROM Write Enable command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3400, "P"
else
  write "<!> Failed (1009;3400) - EEPROM Write Enable command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3400, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_EEPROM_WRITE_ENA_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_EEPROM_WRITE_ENA_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.14.3: Send a Load from File command with a valid EEPROM load "
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LD_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
/$SC_$CPU_MM_LoadFile Filename="/ram/valideeload.dat"

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3100) - LoadFile command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3100, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;3100) - LoadFile command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3100, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.14.4: Send the Disable EEPROM Write command."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_EEPROM_WRITE_DIS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_DisableEEWrite Bank=0

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3500) - EEPROM Write Disable command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3500, "P"
else
  write "<!> Failed (1009;3500) - EEPROM Write Disable command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3500, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_EEPROM_WRITE_DIS_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_EEPROM_WRITE_DIS_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.15: Send a Dump to file command with the EEPROM symbol name "
write ";  and number bytes specified in the step above to verify the load."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_DMP_MEM_FILE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

;; Send the Dump command and transfer the file to ground
s get_mm_file_to_cvt(ramDir,"mm_st2_15_dump.dat","$CPU",appPktID,"EEPROM",dataToSend,"validEEPROM",0)

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1012;3104;7002) - Dump2File command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3104, "A"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3104;7002) - Dump2File command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3104, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.16: Perform a Fill command using a valid EEPROM symbol name."
write ";**********************************************************************"
write ";  Step 2.16.1: Send the Enable EEPROM Write command."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_EEPROM_WRITE_ENA_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_EnableEEWrite Bank=0

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3400) - EEPROM Write Enable command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3400, "P"
else
  write "<!> Failed (1009;3400) - EEPROM Write Enable command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3400, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_EEPROM_WRITE_ENA_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_EEPROM_WRITE_ENA_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.16.2: Send the Fill command."
write ";**********************************************************************"
;; Setup for the Event message
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_FILL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1
;; Send the command
/$SC_$CPU_MM_Fill EEPROM DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFAAEEBB' SymName="validEEPROM" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;1012;3200;7002) - Fill command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_1012, "P"
  ut_setrequirements MM_3200, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1009;1012;3200;7002) - Fill command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_1012, "F"
  ut_setrequirements MM_3200, "F"
  ut_setrequirements MM_7002, "F"
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
write ";  Step 2.16.3: Send the Disable EEPROM Write command."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_EEPROM_WRITE_DIS_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_DisableEEWrite Bank=0

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;3500) - EEPROM Write Disable command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_3500, "P"
else
  write "<!> Failed (1009;3500) - EEPROM Write Disable command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_3500, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_EEPROM_WRITE_DIS_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_EEPROM_WRITE_DIS_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 2.17: Send the Lookup Symbol command with a valid symbol."
write ";**********************************************************************"
;; Set the validRam Address in order to check it in HK
validAddr = $SC_$CPU_TST_MM_RAMAddress

ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYM_LOOKUP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_MM_CMDPC + 1

/$SC_$CPU_MM_LookupSymbol SymName="validRAM"

ut_tlmwait $SC_$CPU_MM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009;7002) - Lookup Symbol command sent properly."
  ut_setrequirements MM_1009, "P"
  ut_setrequirements MM_7002, "A"
else
  write "<!> Failed (1009;7002) - Lookup Symbol command did not increment CMDPC."
  ut_setrequirements MM_1009, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1009) - Expected Event Msg ",MM_SYM_LOOKUP_INF_EID," rcv'd."
  ut_setrequirements MM_1009, "P"
  ;; Verify the HK fields
  if ($SC_$CPU_MM_LastActn = MM_SYM_LOOKUP) AND ;;
     ($SC_$CPU_MM_MemType = 0) AND ;;
     ($SC_$CPU_MM_Address = validAddr) AND ($SC_$CPU_MM_DataValue = 0) AND ;;
     ($SC_$CPU_MM_BytesProc = 0) AND ;;
     ($SC_$CPU_MM_LastFile = "") THEN
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
else
  write "<!> Failed (1009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYM_LOOKUP_INF_EID,"."
  ut_setrequirements MM_1009, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.0: Invalid Symbol tests."
write ";**********************************************************************"
write ";  Step 3.1: Send a Poke command with an invalid RAM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_Poke DataSize=32 RAM Data=pokeValue SymName="InvalidSymbolName" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.2: Send a Peek command with an invalid RAM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_Peek DataSize=8 RAM SymName="invalidRAMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.3: Send a Load with interrupts disabled command with an "
write ";  invalid RAM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_LoadWID DataSize=128 CRC=validCRC SymName="invalidRAMSymbol" Offset=0 Data=widDataArray

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5
write ";**********************************************************************"
write ";  Step 3.4: Send a Read command with an invalid RAM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_DumpInEvent RAM DataSize=3 SymName="invalidRAMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.5: Send a Load from File command with an invalid RAM symbol "
write ";  name."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

dataToSend = 1024

;; Send the TST_MM command to create the file - will need to add Symbolname
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=0 Pattern=x'AF' RAM SymbolName="invalidRAMSymbol" Filename="/ram/invalidramsymb.dat"
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

;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 
/$SC_$CPU_MM_LoadFile Filename="/ram/invalidramsymb.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.6: Send a Dump to File command with an invalid RAM symbol "
write ";  name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

s get_mm_file_to_cvt(ramDir,"mm_st3_6_dump.dat","$CPU",appPktID,"RAM",dataToSend,"invalidRAMSymbol",0)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.7: Send a Fill command with an invalid RAM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_Fill RAM DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFAAEEBB' SymName="invalidRAMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.8: Send a Poke command with an invalid EEPROM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_Poke DataSize=32 EEPROM Data=pokeValue SymName="InvalidEEPROMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.9: Send a Peek command with an invalid EEPROM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_Peek DataSize=8 EEPROM SymName="invalidEEPROMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.10: Send a Read command with an invalid EEPROM "
write ";  symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 

/$SC_$CPU_MM_DumpInEvent EEPROM DataSize=3 SymName="invalidEEPROMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.11: Send a Load from File command with an invalid EEPROM "
write ";  symbolname."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents "$SC", "$CPU", "TST_MM", TST_MM_CREATEFILE_INF_EID, "INFO", 1

dataToSend = 1024

;; Send the TST_MM command to create the file - will need to add Symbolname
cmdCtr = $SC_$CPU_TST_MM_CMDPC + 1
/$SC_$CPU_TST_MM_CreateFile DataSize=dataToSend Address=0 Pattern=x'AF' EEPROM SymbolName="invalidEEPROMSymbol" Filename="/ram/invalideesymb.dat"
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
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 
/$SC_$CPU_MM_LoadFile Filename="/ram/invalideesymb.dat"

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.12: Send a Dump to File command with an invalid EEPROM symbol"
write ";  name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 
s get_mm_file_to_cvt(ramDir,"mm_st3_12_dump.dat","$CPU",appPktID,"EEPROM",dataToSend,"invalidEEPROMSymbol",0)

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.13: Send a Fill command with an invalid EEPROM symbol name."
write ";**********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1 
/$SC_$CPU_MM_Fill EEPROM DataSize=MM_MAX_DUMP_INEVENT_BYTES Pattern=x'FFAAEEBB' SymName="invalidEEPROMSymbol" Offset=0

ut_tlmwait  $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7004) - Poke command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (1010;7004) - Poke command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7004, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (7004) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_7004, "P"
else
  write "<!> Failed (7004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_7004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.14: Send the Write Symbol Table command with an invalid length"
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;;; CPU1 is the default
rawcmd = "1888c00000420ABD"
if ("$CPU" = "CPU2") then
  rawcmd = "1988c00000420ABD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c00000420ABD"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
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
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.15: Send the Lookup Symbol command with an invalid length"
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_LEN_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

;;; CPU1 is the default
rawcmd = "1888c000004209E7"
if ("$CPU" = "CPU2") then
  rawcmd = "1988c000004209E7"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A88c000004209E7"
endif

ut_sendrawcmd "$SC_$CPU_MM", (rawcmd)

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
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
  write "<!> Failed (1010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",MM_LEN_ERR_EID, "."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.16: Send the Lookup Symbol command with an invalid symbol."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_LookupSymbol SymName="invalidSymbolName"

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7002) - Lookup Symbol command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1010;7002) - Lookup Symbol command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_SYMNAME_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.17: Send the Lookup Symbol command with a NULL symbol."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {MMAppName}, MM_SYMNAME_NUL_ERR_EID, "ERROR", 1

errCtr = $SC_$CPU_MM_CMDEC + 1

/$SC_$CPU_MM_LookupSymbol SymName=""

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7002) - Lookup Symbol command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7002, "P"
else
  write "<!> Failed (1010;7002) - Lookup Symbol command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7002, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_SYMNAME_NUL_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMNAME_NUL_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.18: Send the Write Symbol Table command with a NULL filename."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMFILENAME_NUL_ERR_EID,"ERROR",1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_SymTbl2File FileName=""

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7001) - Write Symbol Table command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7001, "P"
else
  write "<!> Failed (1010;7001) - Write Symbol Table command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_SYMFILENAME_NUL_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMFILENAME_NUL_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.19: Send the Write Symbol Table command with a filename that "
write ";  does not exist." 
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMTBL_TO_FILE_FAIL_ERR_EID,"ERROR",1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_SymTbl2File FileName="/boot/symfile.dat"

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7001) - Write Symbol Table command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7001, "P"
else
  write "<!> Failed (1010;7001) - Write Symbol Table command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_SYMTBL_TO_FILE_FAIL_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMTBL_TO_FILE_FAIL_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 3.20: Send the Write Symbol Table command with a filename that "
write ";  contains invalid characters." 
write ";**********************************************************************"
ut_setupevents "$SC","$CPU",{MMAppName},MM_SYMTBL_TO_FILE_INVALID_ERR_EID,"ERROR",1

errCtr = $SC_$CPU_MM_CMDEC + 1
;; Send the command
/$SC_$CPU_MM_SymTbl2File FileName="/boot/sym(file).dat"

ut_tlmwait $SC_$CPU_MM_CMDEC, {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010;7001) - Write Symbol Table command failed as expected."
  ut_setrequirements MM_1010, "P"
  ut_setrequirements MM_7001, "P"
else
  write "<!> Failed (1010;7001) - Write Symbol Table command did not increment CMDEC."
  ut_setrequirements MM_1010, "F"
  ut_setrequirements MM_7001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1010) - Expected Event Msg ",MM_SYMTBL_TO_FILE_INVALID_ERR_EID," rcv'd."
  ut_setrequirements MM_1010, "P"
else
  write "<!> Failed (1010) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",MM_SYMTBL_TO_FILE_INVALID_ERR_EID,"."
  ut_setrequirements MM_1010, "F"
endif

write ";**********************************************************************"
write ";  Step 4.0: Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $SC_$CPU_mm_symtab                                   "
write ";*********************************************************************"
ENDPROC
