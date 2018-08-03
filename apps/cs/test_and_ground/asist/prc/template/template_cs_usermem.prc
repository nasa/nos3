PROC $sc_$cpu_cs_usermem
;*******************************************************************************
;  Test Name:  cs_usermem
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) User-Defined Memory
;	checksumming commands function properly and that the CS application
;	handles anomolies properly.
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
;    CS2003	Upon receipt of a Disable Non-volatile Checksumming command,
;               CS shall disable non-volatile checksumming.
;    CS3003	Upon receipt of a Disable OS Checksumming command, CS shall 
;		disable checksumming of the OS Code segment.
;    CS3008	Upon receipt of a Disable cFE code segment command, CS shall
;		disable checksumming of the cFE code segment.
;    CS4002	Upon receipt of a Disable Application checksumming command, CS
;               shall disable checksumming of all Application code segments.
;    CS5002	Upon receipt of a Disable Table Checksumming command, CS shall
;               disable checksumming of all tables.
;    CS6000	Checksum shall calculate CRCs for each Table-Defined 
;		User-Defined Memory and compare them against the corresponding
;		baseline CRCs if: 
;                       a) Checksumming (as a whole) is Enabled 
;                       b) User-Defined Memory checksumming is Enabled
;                       c) Checksumming of the individual Memory segments
;                          is Enabled
;    CS6000.1	If the User-Defined Memory's CRC is not equal to the 
;               corresponding baseline CRC, CS shall increment the User-Defined
;               Memory CRC Miscompare Counter and send an event message.
;    CS6000.2	If the table-defined Memory is invalid, CS shall send an event
;		message.
;    CS6001	Upon receipt of an Enable User-Defined Memory checksumming
;		command, CS shall enable checksumming of all User-Defined
;		Memory.
;    CS6002	Upon receipt of a Disable User-Defined Memory checksumming
;		command, CS shall disable checksumming of all User-Defined
;		Memory.
;    CS6003	Upon receipt of an Enable User-Defined Memory Item command, CS
;		shall enable checksumming of command-specified Memory.
;    CS6004	Upon receipt of a Disable User-Defined Memory Item command, CS
;		shall disable checksumming of command-specified Memory.
;    CS6005	Upon receipt of a Recompute User-Defined Memory checksum
;		command, CS shall:
;			a) Recompute the baseline checksum for the
;			   command-specified User-Defined Memory
;			b) Set the Recompute In Progress Flag to TRUE
;    CS6005.1	Once the baseline CRC is computed, CS shall:
;			a) Generate an event message containing the baseline CRC
;			b) Set the Recompute In Progress Flag to TRUE
;    CS6005.2	If CS is already processing a Recompute CRC command or a One
;		Shot CRC command, CS shall reject the command.
;    CS6006	Upon receipt of a Report User-Defined Memory CRC command, CS
;		shall send an event message containing the baseline User-Defined
;		Memory CRC.
;    CS6007	If the command-specified User-Defined Memory is invalid (for any
;		of the User-Defined Memory commands where the memory ID is a
;		command argument), CS shall reject the command and send an event
;		message.
;    CS6008	CS shall provide the ability to dump the baseline CRCs and
;		status for all User-Defined Memory via a dump-only table.
;    CS6009	Upon receipt of a Get User-Defined Memory Entry ID command, CS
;		shall send an informational message containing the User-Defined
;		Memory Table Entry ID for the command-specified Memory Address.
;    CS6009.1	If the command-specified Memory Address cannot be found within
;		the User-Defined Memory Table, CS shall send an informational
;		event message.
;    CS7000	The CS software shall limit the amount of bytes processed during
;               each of its execution cycles to a maximum of <PLATFORM_DEFINED>
;               bytes.
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
;	10/16/08	Walt Moleski	Original Procedure.
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
#include "osconfig.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_tbldefs.h"
#include "tst_cs_events.h"
#include "tst_cs_msgdefs.h"

%liv (log_procedure) = logging

#define CS_1002		0
#define CS_1003		1
#define CS_1004		2
#define CS_2003		3
#define CS_3003		4
#define CS_3008		5
#define CS_4002		6
#define CS_5002		7
#define CS_6000		8
#define CS_60001	9
#define CS_60002	10
#define CS_6001		11
#define CS_6002		12
#define CS_6003		13
#define CS_6004		14
#define CS_6005		15
#define CS_60051	16
#define CS_60052	17
#define CS_6006		18
#define CS_6007		19
#define CS_6008		20
#define CS_6009		21
#define CS_60091	22
#define CS_7000		23
#define CS_8000		24
#define CS_8001		25
#define CS_9000		26
#define CS_9001		27

global ut_req_array_size = 27
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1002", "CS_1003", "CS_1004", "CS_2003", "CS_3003", "CS_3008", "CS_4002", "CS_5002", "CS_6000", "CS_6000.1", "CS_6000.2", "CS_6001", "CS_6002", "CS_6003", "CS_6004", "CS_6005", "CS_6005.1", "CS_6005.2", "CS_6006", "CS_6007", "CS_6008", "CS_6009", "CS_6009.1", "CS_7000", "CS_8000", "CS_8001", "CS_9000", "CS_9001" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream
LOCAL defTblId, defPktId, resTblId, resPktId
local i,segIndex,foundSeg,ramAddress
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local memDefTblName = CSAppName & "." & CS_DEF_MEMORY_TABLE_NAME
local memResTblName = CSAppName & "." & CS_RESULTS_MEMORY_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAD"
resTblId = "0FB1"
defPktId = 4013
resPktId = 4017

write ";*********************************************************************"
write ";  Step 1.0: Checksum Non-Volatile Memory Test Setup."
write ";*********************************************************************"
write ";  Step 1.1: Command a Power-on Reset on $CPU."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 1.2: Download the default Memory Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_MEMORY_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table filename
local tableFileName = CS_DEF_MEMORY_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")

;; loop until all slashes are found
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo

write "==> Default Application Code Segment Table filename = '",tableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",tableFileName,"cs_mem_orig_tbl.tbl",hostCPU,"G")

write ";**********************************************************************"
write ";  Step 1.3: Display the Housekeeping pages "
write ";**********************************************************************"
page $SC_$CPU_CS_HK
page $SC_$CPU_TST_CS_HK
page $SC_$CPU_CS_MEM_DEF_TABLE
page $SC_$CPU_CS_MEM_RESULTS_TBL

write ";*********************************************************************"
write ";  Step 1.4: Start the TST_CS_MemTbl application in order to setup   "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_CS_MEMTBL", 1, "INFO", 2

s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")

;;  Wait for app startup events
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
write ";  Step 1.5: Start the Checksum (CS) and TST_CS applications.         "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("1.5")
wait 5

;; Verify the Housekeeping Packet is being generated
;; Set the HK packet ID based upon the cpu being used
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

write ";*********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages for the applications needed "
write ";*********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the CS and CFE_TBL applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=CSAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 1.7: Verify that the CS Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Check the HK tlm items to see if they are 0 or NULL
;; the TST_CS application sends its HK packet
if ($SC_$CPU_CS_CMDPC = 0) AND ($SC_$CPU_CS_CMDEC = 0) AND ;;
   ($SC_$CPU_CS_EepromEC = 0) AND ($SC_$CPU_CS_MemoryEC = 0) AND ;;
   ($SC_$CPU_CS_TableEC = 0) AND ($SC_$CPU_CS_AppEC = 0) AND ;;
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
write ";  Step 1.8: Create & load the Memory Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_mdt1
wait 5

;; Load the file created above
s load_table ("usrmem_def_ld_1", hostCPU)
wait 5

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 2

local cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Memory Definition Table validate command sent."
else
  write "<!> Failed - Memory Definition Table validation failed."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_VALIDATION_INF_EID, "."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Memory Definition Table command sent properly."
else
  write "<!> Failed - Activate Memory Definition Table command."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Memory Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Memory Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.9: Dump the User-defined Memory Definition Table."
write ";*********************************************************************"
s get_tbl_to_cvt (ramDir,memDefTblName,"A","$cpu_usrdeftbl1_9",hostCPU,defTblId)
wait 5

write ";*********************************************************************"
write ";  Step 1.10: Disable all background checksumming except for "
write ";  User-defined memory. "
write ";*********************************************************************"
;; Send the Disable OS Checksumming Command
/$SC_$CPU_CS_DisableOS
wait 5

;; Check the CS HK parameter to ensure that OS Checkcumming is disabled
if (p@$SC_$CPU_CS_OSSTATE = "Disabled") then
  write "<*> Passed (3003) - OS Checksumming disabled."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - OS Checksumming did not get set to disabled as expected."
  ut_setrequirements CS_3003, "F"
endif

;; Send the Disable CFE Core Checksumming Command
/$SC_$CPU_CS_DisableCFECore
wait 5

if (p@$SC_$CPU_CS_CFECORESTATE = "Disabled") then
  write "<*> Passed (3008) - cFE Checksumming disabled."
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (3008) - cFE Checksumming did not get set to disabled as expected."
  ut_setrequirements CS_3008, "F"
endif

;; Send the Disable Non-volatile (EEPROM) Checksumming Command
/$SC_$CPU_CS_DisableEeprom
wait 5

if (p@$SC_$CPU_CS_EEPROMSTATE = "Disabled") then
  write "<*> Passed (2003) - Non-volatile Memory (EEPROM) Checksumming disabled."
  ut_setrequirements CS_2003, "P"
else
  write "<!> Failed (2003) - Non-volatile Memory (EEPROM) Checksumming did not get set to disabled as expected."
  ut_setrequirements CS_2003, "F"
endif

;; Send the Disable Application Checksumming Command
/$SC_$CPU_CS_DisableApps
wait 5

if (p@$SC_$CPU_CS_AppSTATE = "Disabled") then
  write "<*> Passed (4002) - Application Checksumming disabled."
  ut_setrequirements CS_4002, "P"
else
  write "<!> Failed (4002) - Application Checksumming did not get set to disabled as expected."
  ut_setrequirements CS_4002, "F"
endif

;; Send the Disable Table Checksumming Command
/$SC_$CPU_CS_DisableTables
wait 5

if (p@$SC_$CPU_CS_TableSTATE = "Disabled") then
  write "<*> Passed (5002) - Table Checksumming disabled."
  ut_setrequirements CS_5002, "P"
else
  write "<!> Failed (5002) - Table Checksumming did not get set to disabled as expected."
  ut_setrequirements CS_5002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.0: Valid Command Test."
write ";*********************************************************************"
write ";  Step 2.1: Send the command to dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_1",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.2: Send the Enable Checksum command."
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

write ";*********************************************************************"
write ";  Step 2.3: Send the Enable User-Defined Memory Checksumming command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Memory Command
/$SC_$CPU_CS_EnableMemory

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6001) - CS Enable User-defined Memory command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6001, "P"
else
  write "<!> Failed (1003;6001) - CS Enable User-defined Memory command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6001) - Expected Event Msg ",CS_ENABLE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6001, "P"
else
  write "<!> Failed (1003;6001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_4",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-Defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-Defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5: Verify that Memory Items are being checksummed."
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
local keepDumpingResults=TRUE
local DumpingResults=FALSE
local loopCount=0
local dumpFileName = "$cpu_usrrestbl2_5"

while (keepDumpingResults = TRUE) do
  s get_tbl_to_cvt (ramDir,memResTblName,"A",dumpFileName,hostCPU,resTblId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
    ;; If the CRC has been computed AND the CRC is not zero -> Stop
    if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].ComputedYet = "TRUE") AND ;;
       ($SC_$CPU_CS_MEM_RESULT_TABLE[i].BaselineCRC <> 0) AND ;;
       (keepDumpingResults = TRUE) then
      keepDumpingResults = FALSE
    endif
  enddo
enddo

if (keepDumpingResults = FALSE) then
  write "<*> Passed (6000) - User-defined Memory Checksumming is occurring."
  ut_setrequirements CS_6000, "P"
else
  write "<!> Failed (6000) - User-defined Memory Checksumming is not being calculated."
  ut_setrequirements CS_6000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6: Send the Disable Entry command for a valid enabled entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that entry's ID in the Disable command
foundSeg = FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled Memory segment found at index ", segIndex
else
  write "; There were no Enabled User-defined Memory segments found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_MEMORY_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - CS DisableMemoryEntry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - CS DisableMemoryEntry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

;; Dump the definition table to verify the entry's state was modified
;; This verifies DCR #18559
s get_tbl_to_cvt (ramDir,memDefTblName,"A","$cpu_usrdeftbl2_6",hostCPU,defTblId)
wait 5

if (p@$SC_$CPU_CS_MEM_DEF_TABLE[segindex].State = "Disabled") then
  write "<*> Passed - Definition Table entry changed to Disabled"
else
  write "<!> Failed - Definition Table entry was not changed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Dump the results table to ensure that the above entry was"
write ";  disabled. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_7",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.8: Using the TST_CS application, manipulate the disabled    "
write ";  entry's CRC. "
write ";*********************************************************************"
;; Send a TST_CS command to do this using the entry index as the argument
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"INFO", 1

/$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=segIndex

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Send the Enable Entry command for the Entry disabled"
write ";  in Step 2.6 above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_MEMORY_ENTRY_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_MEMORY_MISCOMPARE_ERR_EID,"ERROR", 2

write "*** Memory Segment Miscompare Ctr = ",$SC_$CPU_CS_MemoryEC

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - CS Enable Memory Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - CS Enable Memory Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check that the miscompare event message was rcvd
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000.1) - Expected Event Msg ",CS_MEMORY_MISCOMPARE_ERR_EID," rcv'd."
  ut_setrequirements CS_60001, "P"
else
  write "<!> Failed (6000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_MEMORY_MISCOMPARE_ERR_EID,"."
  ut_setrequirements CS_60001, "F"
endif

write "*** Memory Miscompare Ctr = ",$SC_$CPU_CS_MemoryEC

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_10",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.11: Send the Recompute User-Defined Memory Entry command for"
write ";  the segment specified in Steps above. "
write ";*********************************************************************"
write ";  Step 2.11.1: Send the Recompute User-Defined Memory Entry command "
write ";  for the corrupted segment specified in above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - CS Recompute Memory Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute Memory Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the Completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.11.2: Send the Recompute User-Defined Memory Entry command "
write ";  for a larger segment in order to test the Flag states "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=1

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - CS Recompute Memory Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute Memory Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to TRUE (6005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (6005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (6005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_6005, "F"
endif

;; Check for the Completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (6005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (6005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (6005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.12: Send the Report User-Defined Memory Entry command for "
write ";  the specified entry used in Steps above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report Memory Entry CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report Memory Entry CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.13: Dump the Results table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.14: Sent the Get User-Defined Entry ID command using an   "
write ";  address of an entry in the results table. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Memory Entry ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Invalid Command Test."
write ";*********************************************************************"
write ";  Step 3.1: Send the Enable User-defined Memory Checksumming command "
write ";  with an invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1
rawcmd = ""

;; CPU1 is the default
rawcmd = "189Fc00000021599"

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
write ";  Step 3.2: Send the Disable User-defined Memory Checksumming command"
write ";  with an invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000021698"

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
write ";  Step 3.3: Send the Enable User-defined Memory Item command with an "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041966"

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

write ";**********************************************************************"
write ";  Step 3.4: Send the Enable User-defined Memory Item command with an "
write ";  invalid ID."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=CS_MAX_NUM_MEMORY_TABLE_ENTRIES

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - CS Enable User-defined Memory Entry with an invalid ID sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - CS Enable User-defined Memory Entry with an invalid ID command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - Expected Event Msg ",CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Send the Disable User-defined Memory Item command with an"
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041A77"

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
write ";  Step 3.6: Send the Disable User-defined Memory Item command with an"
write ";  invalid ID."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableMemoryEntry EntryID=CS_MAX_NUM_MEMORY_TABLE_ENTRIES

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - CS Disable User-defined Memory Item command with an invalid ID sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - CS Disable User-defined Memory Item command with an invalid ID did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - Expected Event Msg ",CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7: Send the Recompute User-defined Memory Item command with "
write ";  an invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041855"

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
write ";  Step 3.8: Send the Recompute User-defined Memory Item command with "
write ";  an invalid ID. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeMemory entryID=CS_MAX_NUM_MEMORY_TABLE_ENTRIES

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - CS Recompute User-defined Memory Item with an invalid ID sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - CS Recompute User-defined Memory command with an invalid ID did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - Expected Event Msg ",CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.9: Send the Report User-defined Memory Item command with an "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041744"

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
write ";  Step 3.10: Send the Report User-defined Memory Item command with an"
write ";  invalid ID."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory entryID=CS_MAX_NUM_MEMORY_TABLE_ENTRIES

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - CS Report User-defined Memory Item command with an invalid ID sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - CS Report User-defined Memory Item command with an invalid ID did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - Expected Event Msg ",CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.11: Send the Get User-defined Memory ID command with an "
write ";  invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000061B22"

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
write ";  Step 3.12: Send the Get User-defined Memory ID command with an "
write ";  invalid address. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[0].StartAddr-1000

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}, 5
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007;6009.1) - CS Get User-defined Memory ID command with invalid address sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
  ut_setrequirements CS_60091, "P"
else
  write "<!> Failed (1004;6007;6009.1) - CS Get User-defined Memory  ID command with invalid address did not increment CMDPC as expected."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
  ut_setrequirements CS_60091, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6007) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_6007, "P"
else
  write "<!> Failed (1004;6007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_6007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.13: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl3_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 3.14: Send the Recompute User-defined Memory Item command for "
write ";  an entry in the results table. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=1

write ";*********************************************************************"
write ";  Step 3.15: Send the Recompute User-defined Memory Item command again"
write ";  to verify that only 1 Recompute can occur at the same time. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=0

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6005.2) - CS Recompute User-defined Memory Item command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_60052, "P"
else
  write "<!> Failed (1004;6005.2) - CS Recompute User-defined Memory Item command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_60052, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

write ";*********************************************************************"
write ";  Step 3.16: Send the One Shot CRC command. Verify that this command "
write ";  fails since only 1 Recompute or One Shot can occur at the same time. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName}, CS_ONESHOT_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;6005.2) - One Shot CRC command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_60052, "P"
else
  write "<!> Failed (1004;6005.2) - One Shot CRC command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_60052, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

;; Check for the Completed event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (6005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (6005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (6005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Disabled Checksum Test."
write ";*********************************************************************"
write ";  Step 4.1: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_1",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.2: Send the Disable Checksum command.                       "
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

write ";*********************************************************************"
write ";  Step 4.3: Send the Disable User-defined Memory Checksumming command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable Memory Command
/$SC_$CPU_CS_DisableMemory

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6002) - CS Disable User-defined Memory command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6002, "P"
else
  write "<!> Failed (1003;6002) - CS Disable User-defined Memory command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6002) - Expected Event Msg ",CS_DISABLE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6002, "P"
else
  write "<!> Failed (1003;6002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4: Send the Disable User-defined Memory Item command for an "
write ";  ENABLED entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the Disable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_MEMORY_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - CS Disable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - CS Disable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_5",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.6: Send the Recompute User-defined Memory Item command for the"
write ";  entry used in Step 4.4 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.4 above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.8: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Memory ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9: Send the Enable User-defined Memory Item command for a "
write ";  DISABLED entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the Enable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
  segIndex = 1
endif

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_MEMORY_ENTRY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - CS Enable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - CS Enable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.10: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_10",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.11: Send the Recompute User-defined Memory command for the "
write ";  entry used in Step 4.9 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.12: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.9 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.13: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.14: Send the Enable User-defined Memory Checksumming command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable MEMORY Command
/$SC_$CPU_CS_EnableMemory

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6001) - CS Enable User-defined Memory Checksumming command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6001, "P"
else
  write "<!> Failed (1003;6001) - CS Enable User-defined Memory Checksumming command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6001) - Expected Event Msg ",CS_ENABLE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6001, "P"
else
  write "<!> Failed (1003;6001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.15: Send the Disable User-defined Memory Item command for an "
write ";  ENABLED entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the Disable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_MEMORY_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - CS Disable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - CS Disable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.16: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_16",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.17: Send the Recompute User-defined Memory Item command for "
write ";  the entry used in Step 4.15 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.18: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.15 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.19: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Memory ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.20: Send the Enable User-defined Memory Item command for a "
write ";  DISABLED entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the Enable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
  segIndex = 1
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_MEMORY_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - CS Enable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - CS Enable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.21: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_21",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.22: Send the Recompute User-defined Memory command for the "
write ";  entry used in Step 4.20 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.23: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.20 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.24: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.25: Send the Enable Checksum command.                       "
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

write ";*********************************************************************"
write ";  Step 4.26: Send the Disable User-defined Memory Checksumming command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Tables Command
/$SC_$CPU_CS_DisableMemory

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6002) - CS Disable User-defined Memory command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6002, "P"
else
  write "<!> Failed (1003;6002) - CS Disable User-defined Memory command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6002) - Expected Event Msg ",CS_DISABLE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6002, "P"
else
  write "<!> Failed (1003;6002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.27: Send the Disable User-defined Memory Item command for an "
write ";  ENABLED entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the Disable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_MEMORY_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - CS Disable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - CS Disable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.28: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_28",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.29: Send the Recompute User-defined Memory Item command for "
write ";  the entry used in Step 4.27 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.30: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.27 above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.31: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Memory ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.32: Send the Enable User-defined Memory Item command for a "
write ";  DISABLED entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the Enable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
  segIndex = 1
endif

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_MEMORY_ENTRY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - CS Enable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - CS Enable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.33: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_33",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.34: Send the Recompute User-defined Memory command for the "
write ";  entry used in Step 4.32 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.35: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.32 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.36: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.37: Send the Enable User-defined Memory Checksumming command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable MEMORY Command
/$SC_$CPU_CS_EnableMemory

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6001) - CS Enable User-defined Memory Checksumming command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6001, "P"
else
  write "<!> Failed (1003;6001) - CS Enable User-defined Memory Checksumming command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6001) - Expected Event Msg ",CS_ENABLE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6001, "P"
else
  write "<!> Failed (1003;6001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.38: Send the Disable User-defined Memory Item command for an "
write ";  ENABLED entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the Disable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_MEMORY_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - CS Disable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - CS Disable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6004) - Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6004, "P"
else
  write "<!> Failed (1003;6004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.39: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_39",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.40: Send the Recompute User-defined Memory Item command for "
write ";  the entry used in Step 4.38 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.41: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.38 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.42: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Memory ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.43: Send the Enable User-defined Memory Item command for a "
write ";  DISABLED entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the Enable command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
  segIndex = 1
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_MEMORY_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - CS Enable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - CS Enable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.44: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_44",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.45: Send the Recompute User-defined Memory command for the "
write ";  entry used in Step 4.43 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - CS Recompute User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - CS Recompute User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005) - Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6005, "P"
else
  write "<!> Failed (1003;6005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6005, "F"
endif

;; Check for the completed event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_60051, "P"
else
  write "<!> Failed (1003;6005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_60051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.46: Send the Report User-defined Memory Item command for the "
write ";  entry used in Step 4.43 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportMemory EntryID=segIndex

ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - CS Report User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - CS Report User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6006) - Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6006, "P"
else
  write "<!> Failed (1003;6006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.47: Send the Get User-defined Memory ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_MEMORY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry ID Command
/$SC_$CPU_CS_GetMemoryEntryID Address=$SC_$CPU_CS_MEM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6009) - CS Get User-defined Memory ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6009, "P"
else
  write "<!> Failed (1003;6009) - CS Get User-defined Memory ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6009, "F"
endif

;; Check for at least one event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003) - Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_MEMORY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Definition Table Update Test."
write ";*********************************************************************"
write ";  Step 5.1: Create a User-defined Memory Definition table load file  "
write ";  that contains all empty items."
write ";*********************************************************************"
s $sc_$cpu_cs_mdt4
wait 5

write ";*********************************************************************"
write ";  Step 5.2: Send the command to load the file created above.         "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("usrmemdefemptytbl", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3: Send the command to validate the file loaded in Step 5.2."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - User-defined Memory Definition Table validation failed."
endif

;; Wait for the Validation Success event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

write ";*********************************************************************"
write ";  Step 5.4: Send the Recompute User-defined Memory Item command for a"
write ";  valid entry specified in the Results Table."
write ";*********************************************************************"
;; Send the Command
/$SC_$CPU_CS_RecomputeMemory EntryID=6

write ";*********************************************************************"
write ";  Step 5.5: Send the command to Activate the file loaded in Step 5.2."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"INFO",3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate User-defined Memory Definition Table command sent properly."
else
  write "<!> Failed - Activate User-defined Memory Definition Table command."
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Need to wait for the recompute to finish before the table will get updated
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table Updated successfully."
else
  write "<!> Failed - User-defined Memory Definition Table update failed."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl5_6",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 5.7: Create a User-defined Memory Definition table load file "
write ";  containing several valid entries, an entry that contains an invalid "
write ";  address, an entry that contains an invalid range and an entry with an"
write ";  invalid state."
write ";*********************************************************************"
s $sc_$cpu_cs_mdt2
wait 5

write ";*********************************************************************"
write ";  Step 5.8: Load the invalid files created above one at a time to "
write ";  generate the appropriate error event messages. "
write ";*********************************************************************"
write ";  Step 5.8.1: Send the command to load the invalid file."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("usrmem_def_invalid", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8.2: Send the command to validate the file loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_RANGE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_INF_EID, "INFO", 4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - User-defined Memory Definition Table validate command failed."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validation failed as expected."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - User-defined Memory Definition Table validation was successful when failure was expected."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6000.2) - Invalid Memory Range error detected as expected."
  ut_setrequirements CS_60002, "P"
else
  write "<!> Failed (6000.2) - Invalid Memory Range Error was not generated."
  ut_setrequirements CS_60002, "F"
endif

if ($SC_$CPU_find_event[4].num_found_messages = 1) then
  write "<*> Passed - Memory Table verification results message rcv'd."
else
  write "<!> Failed - Memory Table verification results not rcv'd."
endif

write ";*********************************************************************"
write ";  Step 5.8.3: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8.4: Send the command to load the second invalid file."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("usrmem_def_invalid2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8.5: Send the command to validate the file loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_RANGE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_INF_EID, "INFO", 4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - User-defined Memory Definition Table validate command failed."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validation failed as expected."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - User-defined Memory Definition Table validation was successful when failure was expected."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6000.2) - Invalid Memory Range error detected as expected."
  ut_setrequirements CS_60002, "P"
else
  write "<!> Failed (6000.2) - Invalid Memory Range Error was not generated."
  ut_setrequirements CS_60002, "F"
endif

if ($SC_$CPU_find_event[4].num_found_messages = 1) then
  write "<*> Passed - Memory Table verification results message rcv'd."
else
  write "<!> Failed - Memory Table verification results not rcv'd."
endif

write ";*********************************************************************"
write ";  Step 5.8.6: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8.7: Send the command to load the third invalid file."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("usrmem_def_invalid3", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8.8: Send the command to validate the file loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_STATE_ERR_EID,"ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_INF_EID, "INFO", 4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - User-defined Memory Definition Table validate command failed."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validation failed as expected."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - User-defined Memory Definition Table validation was successful when failure was expected."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6000.2) - Invalid State entry error detected as expected."
  ut_setrequirements CS_60002, "P"
else
  write "<!> Failed (6000.2) - Invalid State entry message was not generated."
  ut_setrequirements CS_60002, "F"
endif

if ($SC_$CPU_find_event[4].num_found_messages = 1) then
  write "<*> Passed - Memory Table verification results message rcv'd."
else
  write "<!> Failed - Memory Table verification results not rcv'd."
endif

write ";*********************************************************************"
write ";  Step 5.8.9: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.9: Create a User-defined Memory Definition table load file "
write ";  containing entries that overlap and empty entries in between valid "
write ";  entries. "
write ";*********************************************************************"
s $sc_$cpu_cs_mdt3
wait 5

write ";*********************************************************************"
write ";  Step 5.10: Send the command to load the file with valid entries.   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("usrmem_def_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.11: Send the command to validate the file loaded in Step 5.9"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - User-defined Memory Definition Table validation failed."
endif

;; Look for the validation event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table validation event '", $SC_$CPU_find_event[2].eventid,"' found!"
else
  write "<!> Failed - User-defined Memory Definition Table validation event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_VALIDATION_INF_EID, "."
endif

write ";*********************************************************************"
write ";  Step 5.12: Send the command to Activate the file loaded in Step 5.9"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=memDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate User-defined Memory Definition Table command sent properly."
else
  write "<!> Failed - Activate User-defined Memory Definition Table command."
endif

;; Look for the Load Pending event
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Look for the Activate success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - User-defined Memory Definition Table Updated successfully."
else
  write "<!> Failed - User-defined Memory Definition Table update failed."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.13: Dump the Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl5_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 6.0: Processing Limit Test."
write ";*********************************************************************"
write ";  Step 6.1: Send the Disable Non-Volatile (Eeprom) Checksumming      "
write ";  command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Eeprom Checksumming Command
/$SC_$CPU_CS_DisableEeprom

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2003) - CS DisableEeprom command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2003, "P"
else
  write "<!> Failed (1003;2003) - CS DisableEeprom command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2003) - Expected Event Msg ",CS_DISABLE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2003, "P"
else
  write "<!> Failed (1003;2003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2: Send the Disable Table Checksumming command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Table Checksumming Command
/$SC_$CPU_CS_DisableTables

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - CS DisableTables command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5002, "P"
else
  write "<!> Failed (1003;5002) - CS DisableTables command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Expected Event Msg ",CS_DISABLE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5002, "P"
else
  write "<!> Failed (1003;5002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3: Send the Disable Application Checksumming command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Application Checksumming Command
/$SC_$CPU_CS_DisableApps

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4002) - CS DisableApps command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4002, "P"
else
  write "<!> Failed (1003;4002) - CS DisableApps command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4002) - Expected Event Msg ",CS_DISABLE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4002, "P"
else
  write "<!> Failed (1003;4002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.4: Send the Enable Memory Entry Checksumming command for a "
write ";  large segment in order for Step 6.5 to work properly."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_MEMORY_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableMemoryEntry EntryID=2

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - CS Enable User-defined Memory Item command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - CS Enable User-defined Memory Item command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6003) - Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6003, "P"
else
  write "<!> Failed (1003;6003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_MEMORY_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_6003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.5: Constantly Dump the User-defined Memory Results table to "
write ";  determine if the CS application is segmenting the CRC calculation  "
write ";  each cycle. "
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
keepDumpingResults=FALSE
local loopCtr = 1
local segmentedCRC=FALSE

while (keepDumpingResults = FALSE) do
  s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl6_5",hostCPU,resTblId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
    ;; If the entry is valid and the Offset or tempCRC are not zero -> Stop
    if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State <> "Empty") AND ;;
       ($SC_$CPU_CS_MEM_RESULT_TABLE[i].ByteOffset <> 0) OR ;;
       ($SC_$CPU_CS_MEM_RESULT_TABLE[i].TempCRC <> 0) AND ;;
       (keepDumpingResults = FALSE) then
      keepDumpingResults = TRUE
      segmentedCRC = TRUE
    endif
  enddo

  if (loopCtr > 15) then
    keepDumpingResults = TRUE
  else
    loopCtr = loopCtr + 1
  endif
enddo

if (segmentedCRC = TRUE) then
  write "<*> Passed (7000) -  Segmenting has occurred for User-defined Memory."
  ut_setrequirements CS_7000, "P"
else
  write "<!> Failed (7000) - User-defined Memory Checksumming is not segmenting."
  ut_setrequirements CS_7000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.0: Definition Table Initialization Test."
write ";*********************************************************************"
write ";  Step 7.1: Send the command to stop the CS Application and the "
write ";  TST_CS Application. "
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP Application="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CSAppName
wait 5

write ";*********************************************************************"
write ";  Step 7.2: Delete the User-defined Memory Definition table default "
write ";  load file from $CPU. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","na",tableFileName,hostCPU,"R")

write ";*********************************************************************"
write ";  Step 7.3: Start the CS and TST_CS applications. "
write ";*********************************************************************"
s $sc_$cpu_cs_start_apps("7.3")

write ";*********************************************************************"
write ";  Step 7.4: Dump the Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl7_4",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 8.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 8.1: Upload the default User-defined Memory Definition file "
write ";  downloaded in step 1.1. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","cs_mem_orig_tbl.tbl",tableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 8.2: Send the Power-On Reset command. "
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
write ";  End procedure $SC_$CPU_cs_usermem"
write ";*********************************************************************"
ENDPROC
