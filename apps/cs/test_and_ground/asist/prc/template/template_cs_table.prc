PROC $sc_$cpu_cs_table
;*******************************************************************************
;  Test Name:  cs_table
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) Table checksumming 
;	commands function properly and handles anomolies properly.
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
;		CS shall disable non-volatile checksumming.
;    CS3003	Upon receipt of a Disable OS Checksumming command, CS shall 
;		disable checksumming of the OS Code segment.
;    CS3008	Upon receipt of a Disable cFE code segment command, CS shall
;		disable checksumming of the cFE code segment.
;    CS4002	Upon receipt of a Disable Application checksumming command, CS
;		shall disable checksumming of all Application code segments.
;    CS5000	Checksum shall calculate CRCs for each Table-Defined Table and 
;		compare them against the corresponding Table's baseline CRC if:
;			a) Checksumming (as a whole) is Enabled
;			b) Table checksumming is Enabled
;			c) Checksumming of the Individual Table is Enabled
;    CS5000.1	If the Table's CRC is not equal to the corresponding Table's
;		baseline CRC and the table has not been modified (thru a table
;		load), CS shall increment the Table CRC Miscompare Counter and
;		send an event message.
;    CS5000.2	If the Table's CRC is not equal to the corresponding Table's
;		baseline CRC and the table has been modified (thru a table
;		load), CS shall recompute the table baseline CRC.
;    CS5000.3	If the table-defined Table is invalid, CS shall send an event
;		message and skip that Table.
;    CS5001	Upon receipt of a Enable Table Checksumming command, CS shall
;		enable checksumming of all tables.
;    CS5002	Upon receipt of a Disable Table Checksumming command, CS shall
;		disable checksumming of all tables.
;    CS5003	Upon receipt of a Enable Table Name command, CS shall enable 
;		checksumming of the command-specified Table.
;    CS5004	Upon receipt of a Disable Table Name command, CS shall disable 
;		checksumming of the command-specified Table.
;    CS5005	Upon receipt of a Recompute Table CRC command, CS shall:
;			a) Recompute the baseline checksum for the
;			   command-specified table
;			b) Set the Recompute In Progress Flag to TRUE
;    CS5005.1	Once the baseline CRC is computed, CS shall:
;			a) Generate an event message containing the baseline CRC
;			b) Set the Recompute In Progress Flag to FALSE
;    CS5005.2	If CS is already processing a Recompute CRC command or a One
;		Shot CRC command, CS shall reject the command.
;    CS5006	Upon receipt of a Report Table CRC command, CS shall send an
;		event message containing the baseline Table CRC for the
;		command-specified table.
;    CS5007	If the command-specified Table is invalid (for any CS Table
;		command where a table name is a command argument), CS shall
;		reject the command and send an event message.
;    CS5008	CS shall provide the ability to dump the baseline CRCs and
;		status for the tables via a dump-only table.
;    CS6002	Upon receipt of a Disable User-Defined Memory Checksumming
;		command, CS shall disable checksumming of all User-Defined
;		Memory.
;    CS7000	The CS software shall limit the amount of bytes processed during
;		each of its execution cycles to a maximum of <PLATFORM_DEFINED>
;		bytes.
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
;                       x) Last One Shot Rate
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
;	08/11/08	Walt Moleski	Original Procedure.
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
#include "tst_tbl_events.h"

%liv (log_procedure) = logging

#define CS_1002		0
#define CS_1003		1
#define CS_1004		2
#define CS_2003		3
#define CS_3003		4
#define CS_3008		5
#define CS_4002		6
#define CS_5000 	7
#define CS_50001	8
#define CS_50002	9
#define CS_50003	10
#define CS_5001		11
#define CS_5002		12
#define CS_5003		13
#define CS_5004 	14
#define CS_5005		15
#define CS_50051	16
#define CS_50052	17
#define CS_5006		18
#define CS_5007		19
#define CS_5008		20
#define CS_6002		21
#define CS_7000		22
#define CS_8000		23
#define CS_8001		24
#define CS_9000		25
#define CS_9001		26

global ut_req_array_size = 26
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1002", "CS_1003", "CS_1004", "CS_2003", "CS_3003", "CS_3008", "CS_4002", "CS_5000", "CS_5000.1", "CS_5000.2", "CS_5000.3", "CS_5001", "CS_5002", "CS_5003", "CS_5004", "CS_5005", "CS_5005.1", "CS_5005.2", "CS_5006", "CS_5007", "CS_5008", "CS_6002", "CS_7000", "CS_8000", "CS_8001", "CS_9000", "CS_9001" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream
LOCAL defTblId, defPktId, resTblId, resPktId
local i,tblIndex,tblName,foundTbl
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local tblDefTblName = CSAppName & "." & CS_DEF_TABLES_TABLE_NAME
local tblResTblName = CSAppName & "." & CS_RESULTS_TABLES_TABLE_NAME
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME
local appResTblName = CSAppName & "." & CS_RESULTS_APP_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAE"
resTblId = "0FB2"
defPktId = 4014
resPktId = 4018

write ";*********************************************************************"
write ";  Step 1.0:  Checksum Table Test Setup."
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
write ";  Step 1.2: Download the default Table Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_TABLES_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local tableFileName = CS_DEF_TABLES_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")

;; loop until all slashes are found
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo

write "==> Default Table Definition Table filename = '",tableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",tableFileName,"cs_tablesorig.tbl",hostCPU,"G")
wait 5

write ";**********************************************************************"
write ";  Step 1.3: Display the Housekeeping pages "
write ";**********************************************************************"
page $SC_$CPU_CS_HK
page $SC_$CPU_TST_CS_HK
page $SC_$CPU_CS_TBL_DEF_TABLE
page $SC_$CPU_CS_TBL_RESULTS_TBL

write ";*********************************************************************"
write ";  Step 1.4: Start the TST_CS_MemTbl application in order to setup   "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_CS_MEMTBL", 1, "INFO", 2

s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")

;; Wait for app startup events
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

write ";*********************************************************************"
write ";  Step 1.6: Create & load the Tables Definition Table file to be used "
write ";  during this test."
write ";********************************************************************"
s $sc_$cpu_cs_tdt1
wait 5

;; Load the Tables file created above
s load_table("tbl_def_tbl_ld_1",hostCPU)
wait 5

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 2

local cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Tables Definition Table validate command sent."
else
  write "<!> Failed - Tables Definition Table validation failed."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_VALIDATION_INF_EID, "."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Tables Definition Table command sent properly."
else
  write "<!> Failed - Activate Tables Definition Table command."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Tables Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Tables Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";**********************************************************************"
write ";  Step 1.7: Start the cFE Table Test Application (TST_TBL).          "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_TBL",hostCPU)

; Wait for app startup events
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_TBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_TBL not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_TBL Application start Event Message not received."
endif

write ";*********************************************************************"
write ";  Step 1.8: Enable DEBUG Event Messages "
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
write ";  Step 1.9: Verify that the CS Housekeeping telemetry items are "
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
write ";  Step 1.10: Dump the Tables Definition Table."
write ";*********************************************************************"
s get_tbl_to_cvt (ramDir,tblDefTblName,"A","$cpu_deftbl1_10",hostCPU,defTblId)
wait 5

write ";*********************************************************************"
write ";  Step 2.0: Valid Command and Functionality Test."
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

;; Send the Disable OS Checksumming Command
/$SC_$CPU_CS_DisableOS
wait 1

;; Send the Disable CFE Core Checksumming Command
/$SC_$CPU_CS_DisableCFECore
wait 1

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

;; Disable Application Checksumming if it is enabled
if (p@$SC_$CPU_CS_AppState = "Enabled") then
  /$SC_$CPU_CS_DisableApps
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send the Enable Tables Checksum command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Tables Command
/$SC_$CPU_CS_EnableTables

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - CS EnableTables command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5001, "P"
else
  write "<!> Failed (1003;5001) - CS EnableTables command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - Expected Event Msg ",CS_ENABLE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5001, "P"
else
  write "<!> Failed (1003;5001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Dump the Table Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_3",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4: Verify that Tables are being checksummed."
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
local keepDumpingResults=TRUE
local dumpFileName = "$cpu_tblrestbl2_4"

while (keepDumpingResults = TRUE) do
  s get_tbl_to_cvt (ramDir,tblResTblName,"A",dumpFileName,hostCPU,resTblId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
    ;; If the CRC has been computed AND the CRC is not zero -> Stop
    if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].ComputedYet = "TRUE") AND ;;
       ($SC_$CPU_CS_TBL_RESULT_TABLE[i].BaselineCRC <> 0) AND ;;
       (keepDumpingResults = TRUE) then
      keepDumpingResults = FALSE
    endif
  enddo
enddo

;; Will not get here if checksumming is not being calculated
write "<*> Passed (5000) - Table Checksumming is occurring."
ut_setrequirements CS_5000, "P"

write ";*********************************************************************"
write ";  Step 2.5: Send the Disable Table command for a valid Enabled table. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that Table's name in the DisableTableName command
;; Make sure this is not the DefTablesTbl.
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") AND ;;
     ($SC_$CPU_CS_TBL_RESULT_TABLE[i].Name <> tblDefTblName) AND ;;
     (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Enabled table '",tblName, "' found at index ", tblIndex
else
  tblName = appDefTblName
  write "; There were no Enabled tables found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Table Command
/$SC_$CPU_CS_DisableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - CS DisableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - CS DisableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

;; Dump the definition table to verify the entry's state was modified
;; This verifies DCR #18559
s get_tbl_to_cvt (ramDir,tblDefTblName,"A","$cpu_deftbl2_5",hostCPU,defTblId)
wait 5

if (p@$SC_$CPU_CS_TBL_DEF_TABLE[tblindex].State = "Disabled") then
  write "<*> Passed - Definition Table entry changed to Disabled"
else
  write "<!> Failed - Definition Table entry was not changed"
endif

wait 5

step2_6:
write ";*********************************************************************"
write ";  Step 2.6: Using the TST_CS application, manipulate the disabled    "
write ";  table's CRC. "
write ";*********************************************************************"
;; Send a TST_CS command to do this using tblName as the argument
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"INFO", 1

/$SC_$CPU_TST_CS_CorruptTblCRC TableName=tblName

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Send the Enable Table Name command for the table disabled"
write ";  in Step 2.5 above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_TABLES_NAME_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_TABLES_MISCOMPARE_ERR_EID,"ERROR", 2

write "*** Table Miscompare Ctr = ",$SC_$CPU_CS_TableEC

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check that the Table miscompare event message was rcvd
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000.1) - Expected Event Msg ",CS_TABLES_MISCOMPARE_ERR_EID," rcv'd."
  ut_setrequirements CS_50001, "P"
else
  write "<!> Failed (5000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_TABLES_MISCOMPARE_ERR_EID,"."
  ut_setrequirements CS_50001, "F"
endif

write "*** Table Miscompare Ctr = ",$SC_$CPU_CS_TableEC

wait 5

write ";*********************************************************************"
write ";  Step 2.8: Dump the Table Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_8",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.9: Send the Recompute Table CRC commands        "
write ";*********************************************************************"
write ";  Step 2.9.1: Send the Recompute Table CRC command for the table       "
write ";  specified in Step 2.5 above in order to stop the miscompares. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_TABLES_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_TABLES_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - CS RecomputeTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (1003;5005) - CS RecomputeTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (1003;5005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5005, "F"
endif

;; Check for the Recompute finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (1003;5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_50051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9.2: Send the Recompute Table CRC command for the results "
write ";  table in order to verify the flag states. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_TABLES_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_TABLES_INF_EID, "INFO", 2

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
wait 3
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeTableName TableName="CS.ResTablesTbl"

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to TRUE (5005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (5005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (5005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_5005, "F"
endif

;; Check for the Recompute finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (1003;5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_50051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (5005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (5005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (5005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_50051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Send the Report Table CRC command for the table specified"
write ";  in Step 2.5 above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.11.0: Verify the CS Table recomputes CRC upon load."
write ";*********************************************************************"
write ";  Step 2.11.1: Dump the Table Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_11_1",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.11.2: Create and load a table file for one of the CS tables "
write ";  being checksummed. Verify that the CRC is recomputed after the load "
write ";  has been activated. "
write ";*********************************************************************"
;; NOTE: This assumes that the appDefTblName is 'Enabled' and the CRC has been
;;       computed already. If this is not true, this step will fail
local appDefCRC=0,appDefIndex=CS_MAX_NUM_TABLES_TABLE_ENTRIES

;; Loop for the App Definition Table entry in the results table
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if ($SC_$CPU_CS_TBL_RESULT_TABLE[i].Name = appDefTblName) then
    appDefCRC = $SC_$CPU_CS_TBL_RESULT_TABLE[i].BaselineCRC
    appDefIndex = i
    write "'",appDefTblName,"' found at index ",appDefIndex,"; Current CRC = ",%hex(appDefCRC,8)
    break
  endif
enddo

;; Create the load file
s $sc_$cpu_cs_adt2

;; Load the table
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

start load_table ("app_def_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command executed successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

;; Validate the load
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Definition Table validation failed."
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

;; Activate the load
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command sent properly."
else
  write "<!> Failed - Activate command not sent properly."
endif

;**** If the event was generated, the Activate occurred!!!
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif 

;; Wait for the CRC to get recomputed
wait 30

;; Dump the results table
s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_11_2",hostCPU,resTblId)
wait 5

;; Verify that the table's CRC has been recalculated
if ($SC_$CPU_CS_TBL_RESULT_TABLE[appDefIndex].BaselineCRC <> appDefCRC) then
  write "<*> Passed (5000.2) - ",appDefTblName, "'s CRC has been recomputed on table load."
  ut_setrequirements CS_50002, "P"
else
  write "<!> Failed (5000.2) - ",appDefTblName,"'s CRC was not recomputed on table load."
  ut_setrequirements CS_50002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.12: Stop the TST_TBL application. Verify that an event is    "
write ";  generated indicating the TST_TBL.dflt_tbl_01 was skipped."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID,"ERROR", 1

/$SC_$CPU_ES_DELETEAPP Application="TST_TBL"
wait 5

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5000.3) - Expected Event Msg ",CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID," rcv'd."
  ut_setrequirements CS_50003, "P"
else
  write "<!> Failed (5000.3) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID,"."
  ut_setrequirements CS_50003, "F"
endif

write ";*********************************************************************"
write ";  Step 2.13: Start the TST_TBL application. Verify that checksumming  "
write ";  is again occuring on the TST_TBL.dflt_tbl_01 table."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_TBL",hostCPU)

; Wait for app startup events
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_TBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_TBL not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_TBL Application start Event Message not received."
endif

;; CPU1 is the default
stream = x'904'

;; Subscribe to the Housekeeping packet
/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

wait 10

write ";*********************************************************************"
write ";  Step 2.14: Using the TST_TBL application, register 2 new tables. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tableA" TBLOPTS=X'0' TBLSIZE=X'28'
wait 5
/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tableB" TBLOPTS=X'0' TBLSIZE=X'28'
wait 5

;; Check for the Table Create events
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Created new single buffer tables 'tableA' and 'tableB'"
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Table create command. Expected 2 events of ID", TST_TBL_TBLREGISTERPASS_EID, ". Rcv'd ",$sc_$cpu_find_event[1].num_found_messages
endif

;; Check for the Table Register event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - All tables have been registered."
else
  write "<!> Failed - Table register command. Expected 2 events of ID",TST_TBL_TBLREGISTER_EID, ". Rcv'd ",$sc_$cpu_find_event[2].num_found_messages
endif

write ";*********************************************************************"
write ";  Step 2.15: Create a new Table Definition load file for the new "
write ";  tables registered above."
write ";*********************************************************************"
;; Initial tableA values
$SC_$CPU_TST_TBL_TABLE1.element[1] = 1
$SC_$CPU_TST_TBL_TABLE1.element[2] = 2
$SC_$CPU_TST_TBL_TABLE1.element[3] = 3
$SC_$CPU_TST_TBL_TABLE1.element[4] = 4
$SC_$CPU_TST_TBL_TABLE1.element[5] = 5
$SC_$CPU_TST_TBL_TABLE1.element[6] = 6
$SC_$CPU_TST_TBL_TABLE1.element[7] = 7
$SC_$CPU_TST_TBL_TABLE1.element[8] = 8
$SC_$CPU_TST_TBL_TABLE1.element[9] = 9
$SC_$CPU_TST_TBL_TABLE1.element[10] = 10

;; Create the load file
s create_tbl_file_from_cvt (hostCPU,4002,"TableA initial load", "tst_tbl_a_ld","TST_TBL.tableA", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

;; Initial tableB values
$SC_$CPU_TST_TBL_TABLE1.element[1] = 10
$SC_$CPU_TST_TBL_TABLE1.element[2] = 9
$SC_$CPU_TST_TBL_TABLE1.element[3] = 8
$SC_$CPU_TST_TBL_TABLE1.element[4] = 7
$SC_$CPU_TST_TBL_TABLE1.element[5] = 6
$SC_$CPU_TST_TBL_TABLE1.element[6] = 5
$SC_$CPU_TST_TBL_TABLE1.element[7] = 4
$SC_$CPU_TST_TBL_TABLE1.element[8] = 3
$SC_$CPU_TST_TBL_TABLE1.element[9] = 2
$SC_$CPU_TST_TBL_TABLE1.element[10] = 1

s create_tbl_file_from_cvt (hostCPU,4002,"TableB initial load", "tst_tbl_b_ld","TST_TBL.tableB", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

write ";*********************************************************************"
write ";  Step 2.16: Load the files above and send the Activate commands."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 2

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

start load_table ("tst_tbl_a_ld", hostCPU)
start load_table ("tst_tbl_b_ld", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for tables A and B executed successfully."
else
  write "<!> Failed - Load command for tables A and B did not execute successfully."
endif

;; Check for the events
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Table Load Event messages received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

;; Activate the loads
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.tableA"
/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.tableB"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate commands sent properly."
else
  write "<!> Failed - Activate commands not sent properly."
endif

wait 5

;**** If the event was generated, the Activate occurred!!!
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif 

write ";*********************************************************************"
write ";  Step 2.17: Enable checksumming on the 2 tables registered above."
write ";*********************************************************************"
;; Send the EnableTableName command
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 2
;; Send the Enable Table Name Commands
/$SC_$CPU_CS_EnableTableName TableName="TST_TBL.tableA"
wait 2
/$SC_$CPU_CS_EnableTableName TableName="TST_TBL.tableB"

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName commands sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName commands did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd twice."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Make sure that the tables above are being checksummed by the CS app
wait 30

;; Dump the Results table
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_17",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.18: Unregister the 2 new tables."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGSFREETBL_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_EID, INFO, 2

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="tableA"
wait 10

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="tableB"
wait 10

;; Should have 2 messages found for each event
if ($SC_$CPU_find_event[1].num_found_messages = 2 AND $SC_$CPU_find_event[2].num_found_messages = 2) then
  write "<*> Passed - Expected Event Messages ",$SC_$CPU_find_event[1].eventid, " and ", $SC_$CPU_find_event[2].eventid, " received."
else
  write "<!> Failed - Expected Event Messages ",TST_TBL_UNREGSFREETBL_EID," and ",TST_TBL_UNREGISTERTBL_EID," not received."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.19: Dump the Results table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_19",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.20: Re-register and load the tables unregistered above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tableA" TBLOPTS=X'0' TBLSIZE=X'28'
wait 5
/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tableB" TBLOPTS=X'0' TBLSIZE=X'28'
wait 5

;; Check for the Table Create events
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Created new single buffer tables 'tableA' and 'tableB'"
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Table create command. Expected 2 events of ID", TST_TBL_TBLREGISTERPASS_EID, ". Rcv'd ",$sc_$cpu_find_event[1].num_found_messages
endif

;; Check for the Table Register event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - All tables have been registered."
else
  write "<!> Failed - Table register command. Expected 2 events of ID",TST_TBL_TBLREGISTER_EID, ". Rcv'd ",$sc_$cpu_find_event[2].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.21: Load and Activate the tables registered above."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 2

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

start load_table ("tst_tbl_a_ld", hostCPU)
start load_table ("tst_tbl_b_ld", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for tables A and B executed successfully."
else
  write "<!> Failed - Load command for tables A and B did not execute successfully."
endif

;; Check for the event messages
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

;; Activate the loads
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.tableA"
/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.tableB"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate commands sent properly."
else
  write "<!> Failed - Activate commands not sent properly."
endif

;**** If the event was generated, the Activate occurred!!!
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif 

wait 5

write ";*********************************************************************"
write ";  Step 2.22: Enable checksumming on the 2 tables registered above."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 2
;; Send the Enable Table Name Commands
/$SC_$CPU_CS_EnableTableName TableName="TST_TBL.tableA"
/$SC_$CPU_CS_EnableTableName TableName="TST_TBL.tableB"

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName commands sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName commands did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd twice."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

wait 5

;; Dump the Results table
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_22",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

;; Make sure that the tables above are being checksummed by the CS app
wait 30

step2_23:
write ";*********************************************************************"
write ";  Step 2.23: Corrupt the one of the tables used above."
write ";*********************************************************************"
;; Send a TST_CS command to corrupt the tables
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_TABLES_MISCOMPARE_ERR_EID,"ERROR", 2

/$SC_$CPU_TST_CS_CorruptTblCRC TableName="TST_TBL.tableA"

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
endif

;; Check for the Miscompare event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.24: Load and activate the table corrupted above."
write ";*********************************************************************"
;; Set the table's CRC to the "corrupted" value
local tableABadCRC = 0x12D687

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

start load_table ("tst_tbl_a_ld", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for table A executed successfully."
else
  write "<!> Failed - Load command for table A did not execute successfully."
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

;; Activate the load
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.tableA"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command sent properly."
else
  write "<!> Failed - Activate command not sent properly."
endif

;**** If the event was generated, the Activate occurred!!!
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_UPDATE_SUCCESS_INF_EID, "."
endif 

;; Wait for the CS app to refresh the results
wait 20

write ";*********************************************************************"
write ";  Step 2.25: Dump the results table and verify that the CRC for the "
write ";  table used in the above steps has been recomputed. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_25",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

;; Verify that tableA's CRC has been recalculated
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if ($SC_$CPU_CS_TBL_RESULT_TABLE[i].Name = "TST_TBL.tableA") then
    if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].ComputedYet = "TRUE") AND ;;
       ($SC_$CPU_CS_TBL_RESULT_TABLE[i].BaselineCRC <> tableABadCRC) then
      write "<*> Passed (5000.2) - Table A's CRC has been recomputed on update."
      ut_setrequirements CS_50002, "P"
    else
      write "<!> Failed (5000.2) - Table A's CRC was not recomputed on update."
      ut_setrequirements CS_50002, "F"
    endif
  endif
enddo

wait 5

write ";*********************************************************************"
write ";  Step 2.26: Send the Recompute Table CRC command for the Definition "
write ";  Tables table. This verifies DCR #22897."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_TABLES_INF_EID,"INFO", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_TABLES_MISCOMPARE_ERR_EID,"ERROR", 3

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeTableName TableName=tblDefTblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - CS RecomputeTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (1003;5005) - CS RecomputeTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (1003;5005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5005, "F"
endif

;; Check for the Recompute finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (1003;5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_50051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (5005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (5005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (5005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_50051, "F"
endif

wait 20

;; Verify that the Miscompare Error event was not generated
if ($SC_$CPU_find_event[3].num_found_messages <> 0) then
  write "<!> Failed - Miscompare Error Event message rcv'd when not expected."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.27: Send the Recompute Table CRC command for the Table Results"
write ";  table. This verifies DCR #22897."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_TABLES_INF_EID,"INFO", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_TABLES_MISCOMPARE_ERR_EID,"ERROR", 3

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeTableName TableName=tblResTblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - CS RecomputeTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (1003;5005) - CS RecomputeTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (1003;5005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5005, "F"
endif

;; Check for the Recompute finished event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (1003;5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_50051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (5005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (5005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (5005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_50051, "F"
endif

wait 20

;; Verify that the Miscompare Error event was not generated
if ($SC_$CPU_find_event[3].num_found_messages <> 0) then
  write "<!> Failed - Miscompare Error Event message rcv'd when not expected."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Invalid Command Test."
write ";*********************************************************************"
write ";  Step 3.1: Send the Enable Table Checksumming command with an     "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000021c99"

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
write ";  Step 3.2: Send the Disable Table Checksumming command with an    "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000021D98"

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
write ";  Step 3.3: Send the Enable Table Name command with an invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000282066"

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
write ";  Step 3.4: Send the Enable Table Name command with an invalid table."
write ";*********************************************************************"
write ";  Step 3.4.1: Send the command with a null table name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName=""

ut_tlmwait  $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS EnableTableName with Null Table name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS EnableTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4.2: Send the command with a table name that is not currently"
write ";  defined in the definition table. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS EnableTableName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS EnableTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5
 
write ";*********************************************************************"
write ";  Step 3.5: Send the Disable Table Name command with an invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000282199"

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
write ";  Step 3.6: Send the Disable Table Name command with an invalid table."
write ";*********************************************************************"
write ";  Step 3.6.1: Send the command with a null Table name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Disable Table Name Command
/$SC_$CPU_CS_DisableTableName TableName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS DisableTableName with Null Table name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS DisableTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.2: Send the command with a table name that is not currently"
write ";  defined in the definition table. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Disable Table Name Command
/$SC_$CPU_CS_DisableTableName TableName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS DisableTableTableName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS DisableTableTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7: Send the Report Table CRC command with an invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000281E75"

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
write ";  Step 3.8: Send the Report Table CRC command with an invalid table."
write ";*********************************************************************"
write ";  Step 3.8.1: Send the command with a null table name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_INVALID_NAME_TABLES_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Report Table Name Command
/$SC_$CPU_CS_ReportTableName TableName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS ReportTableName with Null Tablename sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS ReportTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_BASELINE_INVALID_NAME_TABLES_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_INVALID_NAME_TABLES_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.8.2: Send the command with an application name that is not "
write ";  currently executing. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_INVALID_NAME_TABLES_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Report Table Name Command
/$SC_$CPU_CS_ReportTableName TableName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS ReportTableName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS ReportTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_BASELINE_INVALID_NAME_TABLES_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_INVALID_NAME_TABLES_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.9: Send the Recompute Table CRC command with an invalid "
write ";  length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000281F88"

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
write ";  Step 3.10: Send the Recompute Table CRC command with an invalid "
write ";  table name."
write ";*********************************************************************"
write ";  Step 3.10.1: Send the command with a null table name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeTableName TableName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS RecomputeTableName with Null Tablename sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS RecomputeTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.10.2: Send the command with an application name that is not "
write ";  currently executing. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeTableName TableName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - CS RecomputeTableName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - CS RecomputeTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5007) - Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_5007, "P"
else
  write "<!> Failed (1004;5007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_5007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.11: Send a valid Recompute command.  "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_TABLES_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_TABLES_INF_EID, "INFO", 2

;; Send the Command
/$SC_$CPU_CS_RecomputeTableName TableName=tblResTblName

write ";*********************************************************************"
write ";  Step 3.12: Send the Recompute command again. This should fail since "
write ";  a Recompute is already executing."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeTableName TableName="TST_TBL.tableA"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5005.2) - CS RecomputeTableName failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_50052, "P"
else
  write "<!> Failed (1004;5005.2) - CS RecomputeTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_50052, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5005.2) - Expected Event Msg ",CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_50052, "P"
else
  write "<!> Failed (1004;5005.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_50052, "F"
endif

write ";*********************************************************************"
write ";  Step 3.13: Send the One Shot CRC command. This should fail since "
write ";  a Recompute is already executing."
write ";*********************************************************************"
;; Check if the recompute finished. If yes, start another before the One Shot
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  /$SC_$CPU_CS_RecomputeTableName TableName=tblResTblName
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5005.2) - One Shot CRC command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_50052, "P"
else
  write "<!> Failed (1004;5005.2) - One Shot CRC command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_50052, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5005.2) - Expected Event Msg ",CS_ONESHOT_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_50052, "P"
else
  write "<!> Failed (1004;5005.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ONESHOT_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_50052, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.14: Dump the Table Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl3_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.0: Disabled Checksum Test."
write ";*********************************************************************"
write ";  Step 4.1: Send the Disable Checksum command.                       "
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
write ";  Step 4.2: Send the Disable Table Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable Tables Command
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
write ";  Step 4.3: Send the Enable Table Name command with a table whose "
write ";  status is DISABLED."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that name in the EnableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Disabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Disabled table '",tblName, "' found at index ", tblIndex
else
  tblName = tblDefTblName
  tblIndex = 1
  write "; There were no Disabled tables found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName=tblName

ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

wait 5

step4_4:
write ";*********************************************************************"
write ";  Step 4.4: Dump the Table Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_4",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

;;;;;; Need to make sure that tblName has been computed before continuing
if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].ComputedYet = "FALSE") then
  ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"DEBUG", 1
  ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_TABLES_INF_EID,"INFO", 2

  cmdCtr = $SC_$CPU_CS_CMDPC + 1
  ;; Send the Recompute Command
  /$SC_$CPU_CS_RecomputeTableName TableName=tblName

  ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1003;5005) - CS RecomputeTableName command sent properly."
    ut_setrequirements CS_1003, "P"
    ut_setrequirements CS_5005, "P"
  else
    write "<!> Failed (1003;5005) - CS RecomputeTableName command did not increment CMDPC."
    ut_setrequirements CS_1003, "F"
    ut_setrequirements CS_5005, "F"
  endif

  ;; Check for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1003;5005) - Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID," rcv'd."
    ut_setrequirements CS_1003, "P"
    ut_setrequirements CS_5005, "P"
  else
    write "<!> Failed (1003;5005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_TABLES_STARTED_DBG_EID,"."
    ut_setrequirements CS_1003, "F"
    ut_setrequirements CS_5005, "F"
  endif

  ;; Check for the Recompute finished event message
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 100
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1003;5005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID," rcv'd."
    ut_setrequirements CS_1003, "P"
    ut_setrequirements CS_50051, "P"
  else
    write "<!> Failed (1003;5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID,"."
    ut_setrequirements CS_1003, "F"
    ut_setrequirements CS_50051, "F"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5: Send the Report Table CRC command for the Table specified"
write ";  above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6: Manipulate the table's CRC in order to force a miscompare."
write ";*********************************************************************"
;; Send a TST_CS command to do this using tblName as the argument
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"INFO", 1

/$SC_$CPU_TST_CS_CorruptTblCRC TableName=tblName

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Send the Disable Table Name command with a table whose "
write ";  status is ENABLED."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that name in the DisableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Enabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Enabled tables found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Table Name Command
/$SC_$CPU_CS_DisableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - CS DisableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - CS DisableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

wait 5

step4_8:
write ";*********************************************************************"
write ";  Step 4.8: Dump the Table Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_8",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.9: Send the Report Table CRC command for the table specified"
write ";  above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.10: Send the Enable Table Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Tables Command
/$SC_$CPU_CS_EnableTables

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - CS EnableTables command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5001, "P"
else
  write "<!> Failed (1003;5001) - CS EnableTables command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - Expected Event Msg ",CS_ENABLE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5001, "P"
else
  write "<!> Failed (1003;5001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.11: Send the Enable Table Name command with a table whose   "
write ";  status is DISABLED.                              "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that name in the EnableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Disabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Disabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Disabled tables found in the results table"
endif

;; Send the EnableTableName command
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_TABLES_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

wait 5

step4_12:
write ";*********************************************************************"
write ";  Step 4.12: Dump the Table Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_12",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.13: Send the Report Table CRC command for the table specified"
write ";  above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.14: Send the Disable Table Name command with a table whose "
write ";  status is ENABLED.                            "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that name in the DisableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Enabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Enabled tables found in the results table"
endif

;; Send the DisableTableName command
ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable TableName Command
/$SC_$CPU_CS_DisableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - CS DisableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - CS DisableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

wait 5

step4_15:
write ";*********************************************************************"
write ";  Step 4.15: Dump the Table Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_15",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.16: Send the Report Table CRC command for the table specified"
write ";  above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.17: Send the Enable Checksum command.                       "
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

write ";*********************************************************************"
write ";  Step 4.18: Send the Disable Table Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable Tables Command
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
write ";  Step 4.19: Send the Enable Table Name command with a table whose "
write ";  status is DISABLED."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that name in the EnableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Disabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Disabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Disabled tables found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

wait 5

step4_20:
write ";*********************************************************************"
write ";  Step 4.20: Dump the Table Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_20",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.21: Send the Report Table CRC command for the Table specified"
write ";  above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.22: Manipulate the table's CRC in order to force a miscompare."
write ";*********************************************************************"
;; Send a TST_CS command to do this using tblName as the argument
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"INFO", 1

/$SC_$CPU_TST_CS_CorruptTblCRC TableName=tblName

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.23: Send the Disable Table Name command with a table whose "
write ";  status is ENABLED."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that name in the DisableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Enabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Enabled tables found in the results table"
endif

;; Send the DisableTableName command
ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Table Name Command
/$SC_$CPU_CS_DisableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - CS DisableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - CS DisableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

wait 5

step4_24:
write ";*********************************************************************"
write ";  Step 4.24: Dump the Table Results table. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_24",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.25: Send the Report Table CRC command for the table specifie"
write ";  above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.26: Send the Enable Table Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Tables Command
/$SC_$CPU_CS_EnableTables

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - CS EnableTables command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5001, "P"
else
  write "<!> Failed (1003;5001) - CS EnableTables command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - Expected Event Msg ",CS_ENABLE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5001, "P"
else
  write "<!> Failed (1003;5001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.27: Send the Enable Table Name command with a table whose   "
write ";  status is DISABLED.                              "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that name in the EnableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Disabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Disabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Disabled tables found in the results table"
endif

;; Send the EnableTableName command
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Table Name Command
/$SC_$CPU_CS_EnableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - CS EnableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - CS EnableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5003, "P"
else
  write "<!> Failed (1003;5003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5003, "F"
endif

wait 5

step4_28:
write ";*********************************************************************"
write ";  Step 4.28: Dump the Table Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_28",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.29: Send the Report Table CRC command for the table specified"
write ";  above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.30: Send the Disable Table Name command with a table whose "
write ";  status is ENABLED.                            "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that name in the DisableTableName command
foundTbl=FALSE

for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") AND (foundTbl = FALSE) then
    tblIndex = i
    foundTbl = TRUE
  endif
enddo

if (foundTbl = TRUE) then
  tblName = $SC_$CPU_CS_TBL_RESULT_TABLE[tblIndex].NAME
  write "; Enabled table '",tblName, "' found at index ", tblIndex
else
  tblName = "TST_CS"
  write "; There were no Enabled tables found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_TABLES_NAME_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable TableName Command
/$SC_$CPU_CS_DisableTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - CS DisableTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - CS DisableTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5004, "P"
else
  write "<!> Failed (1003;5004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5004, "F"
endif

wait 5

step4_31:
write ";*********************************************************************"
write ";  Step 4.31: Dump the Table Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_31",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.32: Send the Report Table CRC command for the table specified"
write ";  above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_TABLES_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportTableName Command
/$SC_$CPU_CS_ReportTableName TableName=tblName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - CS ReportTableName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - CS ReportTableName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Expected Event Msg ",CS_BASELINE_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_5006, "P"
else
  write "<!> Failed (1003;5006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_5006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Definition Table Update Test."
write ";*********************************************************************"
write ";  Step 5.1: Create a Table Definition table load file that contains  "
write ";  all empty items."
write ";*********************************************************************"
s $sc_$cpu_cs_tdt4
wait 5

write ";*********************************************************************"
write ";  Step 5.2: Send the command to load the file created above.         "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("tbldefemptytable", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CFE_TBL_FILE_LOADED_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3: Send the command to validate the file loaded in Step 5.2."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Table Definition Table validation failed."
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validation Success Event Msg rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID,"."
endif

write ";*********************************************************************"
write ";  Step 5.4: Send the command to Activate the file loaded in Step 5.2."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Table Definition Table command sent properly."
else
  write "<!> Failed - Activate Table Definition Table command."
endif

;; Check for event messages
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Check for event messages
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5: Dump the Tables Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl5_5",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 5.6: Create a Table Definition table load file containing empty"
write ";  entries in between valid entries and a load file containing an "
write ";  entry with an invalid state. "
write ";*********************************************************************"
s $sc_$cpu_cs_tdt2
wait 5

s $sc_$cpu_cs_tdt3
wait 5

write ";*********************************************************************"
write ";  Step 5.7: Send the command to load the invalid file created above."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("tbl_def_tbl_invalid", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8: Send the command to validate the file loaded in Step 5.7"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Table Definition Table validate command failed."
endif

;; Wait for the Validation Error event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validation failed with an invalid state as expected."
else
  write "<!> Failed - Table Definition Table validation was successful with an invalid state entry."
endif

write ";*********************************************************************"
write ";  Step 5.9: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=tblDefTblName

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
write ";  Step 5.10: Send the command to load the file with valid entries.   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("tbl_def_tbl_ld_2", hostCPU)

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
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.11: Send the command to validate the file loaded in Step 5.10"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Table Definition Table validation failed."
endif

;; Look for the validation event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validation event '", $SC_$CPU_find_event[2].eventid,"' found!"
else
  write "<!> Failed - Table Definition Table validation event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

write ";*********************************************************************"
write ";  Step 5.12: Send the command to Activate the file loaded in Step 5.10"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=tblDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Table Definition Table command sent properly."
else
  write "<!> Failed - Activate Table Definition Table command."
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.13: Dump the Table Definition and Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 2

;; Dump the Definition Table
s get_tbl_to_cvt (ramDir,tblDefTblName,"A","$cpu_deftbl5_13",hostCPU,defTblId)
wait 5

;; Dump the Definition Table
s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl5_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
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
write ";  Step 6.2: Send the Disable OS Code Segment command.                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_OS_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable OS Checksumming Command
/$SC_$CPU_CS_DisableOS

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3003) - CS DisableOS command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (1003;8003) - CS DisableOS command did not increment CMDPC."
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

write ";*********************************************************************"
write ";  Step 6.3: Send the Disable cFE Code Segment command.               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_CFECORE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable CFE Core Checksumming Command
/$SC_$CPU_CS_DisableCFECore

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3008) - CS DisableCFECORE command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (1003;3008) - CS DisableCFECORE command did not increment CMDPC."
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

write ";*********************************************************************"
write ";  Step 6.4: Send the Disable Application Checksumming command. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable CFE Core Checksumming Command
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
write ";  Step 6.5: Send the Disable User-Defined Memory Checksumming command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_MEMORY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable CFE Core Checksumming Command
/$SC_$CPU_CS_DisableMemory

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;6002) - CS DisableMemory command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_6002, "P"
else
  write "<!> Failed (1003;6002) - CS DisableMemory command did not increment CMDPC."
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
write ";  Step 6.6: Dump the Table Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl6_6",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 6.7: Determine the size of the largest table in the Results   "
write ";  table and divide that by 2. Set the max bytes per cycle to the     "
write ";  calculated value.  "
write ";*********************************************************************"
local largestSize=0, halfSize

;; Loop for each enabled entry in the results table
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  ;; If the entry is valid and the Offset or tempCRC are not zero -> Stop
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (largestSize < $SC_$CPU_CS_TBL_RESULT_TABLE[i].NumBytes) then
      largestSize = $SC_$CPU_CS_TBL_RESULT_TABLE[i].NumBytes
    endif
  endif
enddo

halfSize = largestSize / 2

ut_setupevents "$SC","$CPU","TST_CS",TST_CS_SET_BYTES_PER_CYCLE_INF_EID,"INFO",1

;; Send the command to set the bytes
/$SC_$CPU_TST_CS_SetMaxBytes numBytes=halfSize
wait 5

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SetMaxBytes command."
else
  write "<!> Failed - SetMaxBytes command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.8: Constantly Dump the Table Results table to determine if "
write ";  the CS application is segmenting the CRC calculation each cycle. "
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
keepDumpingResults=FALSE
local loopCtr = 1
local segmentedCRC=FALSE

while (keepDumpingResults = FALSE) do
  s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl6_8",hostCPU,resTblId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
    ;; If the entry is valid and the Offset or tempCRC are not zero -> Stop
    if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State <> "Empty") AND ;;
       ($SC_$CPU_CS_TBL_RESULT_TABLE[i].ByteOffset <> 0) OR ;;
       ($SC_$CPU_CS_TBL_RESULT_TABLE[i].TempCRC <> 0) AND ;;
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
  write "<*> Passed (7000) -  Segmenting has occurred for Tables."
  ut_setrequirements CS_7000, "P"
else
  write "<!> Failed (7000) - Table Checksumming is not segmenting."
  ut_setrequirements CS_7000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.0: Definition Table Initialization Test."
write ";*********************************************************************"
write ";  Step 7.1: Send the command to stop the CS and TST_CS applications. "
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP Application="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CSAppName
wait 5

write ";*********************************************************************"
write ";  Step 7.2: Delete the Table Definition table default load file from "
write ";  $CPU. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","na",tableFileName,hostCPU,"R")

write ";*********************************************************************"
write ";  Step 7.3: Start the CS Application. "
write ";*********************************************************************"
s $sc_$cpu_cs_start_apps("7.3")

write ";*********************************************************************"
write ";  Step 7.4: Dump the Table Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl7_4",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "P"
endif

write ";*********************************************************************"
write ";  Step 8.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 8.1: Upload the default Tables Definition table downloaded in "
write ";       step 1.1. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","cs_tablestbl.tblORIG",tableFileName,hostCPU,"P")

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
write ";  End procedure $SC_$CPU_cs_table"
write ";*********************************************************************"
ENDPROC
