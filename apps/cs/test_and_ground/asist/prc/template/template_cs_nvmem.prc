PROC $sc_$cpu_cs_nvmem
;*******************************************************************************
;  Test Name:  cs_nvmem
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) Non-Volatile Memory
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
;    CS2001	The Checksum App shall calculate CRCs for each Table-Defined
;		Non-volatile segment and compare them against the corresponding
;		baseline Non-volatile segment CRCs if:
;			a. Checksumming (as a whole) is Enabled
;			b. Non-volatile segment checksumming is Enabled
;			c. Checksumming of the individual Non-volatile segment
;			   is Enabled
;    CS2001.1	If the Non-volatile segment CRC is not equal to the 
;		corresponding baseline CRC, CS shall increment the Non-volatile
;		CRC Miscompare Counter and send an event message.
;    CS2001.2	If the table-defined segment is invalid, CE shall send an error
;		event message. 
;    CS2002	Upon receipt of an Enable Non-volatile Checksumming command,
;		CS shall enable non-volatile checksumming.
;    CS2003	Upon receipt of a Disable Non-volatile Checksumming command,
;		CS shall disable non-volatile checksumming.
;    CS2004	Upon receipt of an Enable Non-volatile Segment command, CS shall
;		enable checksumming of the command-specified non-volatile
;		segment.
;    CS2005	Upon receipt of a Disable Non-volatile Segment command, CS shall
;		disable checksumming of the command-specified non-volatile
;		segment.
;    CS2006	Upon receipt of a Recompute Non-volatile Checksum Segment
;		command, CS shall:
;			a) Recompute the baseline checksum for the 
;			   command-specified non-volatile segment.
;			b) Set the Recompute In Progress Flag to TRUE
;    CS2006.1	If CS is already processing a Recompute CRC command or a One
;		Shot command, CS shall reject the command.
;    CS2006.2	Once the baseline CRC is computed, CS shall:
;			a) Generate an informational event message containing
;			   the baseline CRC
;			b) Set the Recompute In Progress Flag to FALSE
;    CS2007	Upon receipt of a Report Non-volatile Checksum Segment command,
;		CS shall send an event message containing the baseline
;		checksum for the command-specified non-volatile segment.
;    CS2008	Upon receipt of a Get Non-volatile Checksum Segment command, CS
;		shall send an event message containing the segment number for
;		the command-specified non-volatile address.
;    CS2009	If a command-specified segment is invalid (for any of the
;		non-volatile memory commands where segment is a command
;		argument), CS shall reject the command and send an event
;		message.
;    CS2010	CS shall provide the ability to dump the baseline CRCs and
;		status for the non-volatile memory segments via a dump-only
;		table.
;    CS3003	Upon receipt of a Disable OS Checksumming command, CS shall 
;		disable checksumming of the OS Code segment.
;    CS3008	Upon receipt of a Disable cFE code segment command, CS shall
;		disable checksumming of the cFE code segment.
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
;	10/09/08	Walt Moleski	Original Procedure.
;       09/22/10        Walt Moleski    Updated to use variables for the CFS
;                                       application name and ram disk. Replaced
;					all setupevt instances with setupevents
;       09/19/12        Walt Moleski    Added write of new HK items and added a
;					define of the OS_MEM_TABLE_SIZE that
;					was removed from osconfig.h in 3.5.0.0
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address. Changed define
;                                       of OS_MEM_TABLE_SIZE to MEM_TABLE_SIZE.
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

#define MEM_TABLE_SIZE	10

#define CS_1002		0
#define CS_1003		1
#define CS_1004		2
#define CS_2001		3
#define CS_20011	4
#define CS_20012	5
#define CS_2002		6
#define CS_2003		7
#define CS_2004		8
#define CS_2005		9
#define CS_2006		10
#define CS_20061	11
#define CS_20062	12
#define CS_2007		13
#define CS_2008		14
#define CS_2009		15
#define CS_2010		16
#define CS_3003		17
#define CS_3008		18
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
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1002","CS_1003","CS_1004","CS_2001","CS_2001.1","CS_2001.2","CS_2002","CS_2003","CS_2004","CS_2005","CS_2006","CS_2006.1","CS_2006.2","CS_2007","CS_2008","CS_2009","CS_2010","CS_3003","CS_3008","CS_8000","CS_8001","CS_9000","CS_9001" ]

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
local eeDefTblName = CSAppName & "." & CS_DEF_EEPROM_TABLE_NAME
local eeResTblName = CSAppName & "." & CS_RESULTS_EEPROM_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAC"
resTblId = "0FB0"
defPktId = 4012
resPktId = 4016

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

write ";**********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";**********************************************************************"
page $SC_$CPU_CS_HK
page $SC_$CPU_TST_CS_HK
page $SC_$CPU_CS_EEPROM_DEF_TABLE
page $SC_$CPU_CS_EEPROM_RESULTS_TBL

write ";*********************************************************************"
write ";  Step 1.3: Start the TST_CS_MemTbl application in order to setup    "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_CS_MEMTBL", 1, "INFO", 2
                                                                                
s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")
                                                                                
;;  Wait for app startup event
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
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
write ";  Step 1.4: Create the EEPROM Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_edt1
wait 5

write ";*********************************************************************"
write ";  Step 1.5: Start the applications in order for the load file created"
write ";  above to successfully pass validation and load. "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("1.5")
wait 5

;; Verify the Housekeeping Packet is being generated
;; Set the DS HK packet ID based upon the cpu being used
local hkPktId = "p0A4"

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

write ";**********************************************************************"
write ";  Step 1.6: Load the Definition file created above.       "
write ";**********************************************************************"
start load_table ("eeprom_def_ld_1", hostCPU)
wait 5

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 2

local cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table validate command sent."
else
  write "<!> Failed - Application Definition Table validation failed."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_VALIDATION_INF_EID, "."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Application Definition Table command sent properly."
else
  write "<!> Failed - Activate Application Definition Table command."
endif

write ";*********************************************************************"
write ";  Step 1.7: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_EVS_CMDPC + 2

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
write ";  Step 1.8: Verify that the CS Housekeeping telemetry items are "
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
write ";  Step 1.9: Dump the EEPROM Definition Table."
write ";*********************************************************************"
s get_tbl_to_cvt (ramDir,eeDefTblName,"A","$cpu_eedeftbl1_9",hostCPU,defTblId)
wait 5

write ";*********************************************************************"
write ";  Step 2.0: Valid Command Test."
write ";*********************************************************************"
write ";  Step 2.1: Send the command to dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_1",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of Eeprom Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of Eeprom Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
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

;; Send the Disable OS Checksumming Command
/$SC_$CPU_CS_DisableOS
wait 1

;; Send the Disable CFE Core Checksumming Command
/$SC_$CPU_CS_DisableCFECore
wait 5

;; Test the CS HK parameters to ensure that OS and CFE Checksumming is disabled
if (p@$SC_$CPU_CS_OSSTATE = "Disabled") then
  write "<*> Passed (3003) - OS Checksumming disabled."
  ut_setrequirements CS_3003, "P"
else
  write "<!> Failed (3003) - OS Checksumming not disabled as expected."
  ut_setrequirements CS_3003, "F"
endif

if (p@$SC_$CPU_CS_CFECORESTATE = "Disabled") then
  write "<*> Passed (3008) - cFE Core Checksumming disabled."
  ut_setrequirements CS_3008, "P"
else
  write "<!> Failed (3008) - cFE Core Checksumming not disabled as expected."
  ut_setrequirements CS_3008, "F"
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

;; Disable Application Checksumming if it is enabled
if (p@$SC_$CPU_CS_AppState = "Enabled") then
  /$SC_$CPU_CS_DisableApps
  wait 1
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Send the Enable Non-Volatile Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable EEPROM Command
/$SC_$CPU_CS_EnableEeprom

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - CS Enable EEPROM command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2002, "P"
else
  write "<!> Failed (1003;2002) - CS Enable EEPROM command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - Expected Event Msg ",CS_ENABLE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2002, "P"
else
  write "<!> Failed (1003;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Dump the EEPROM Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_4",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5: Verify that Segments are being checksummed."
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
local keepDumpingResults=TRUE
local DumpingResults=FALSE
local loopCount=0
local dumpFileName = "$cpu_eerestbl2_5"

while (keepDumpingResults = TRUE) do
  s get_tbl_to_cvt (ramDir,eeResTblName,"A",dumpFileName,hostCPU,resTblId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
    ;; If the CRC has been computed AND the CRC is not zero -> Stop
    if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].ComputedYet = "TRUE") AND ;;
       ($SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BaselineCRC <> 0) AND ;;
       (keepDumpingResults = TRUE) then
      keepDumpingResults = FALSE
    endif
  enddo
enddo

if (keepDumpingResults = FALSE) then
  write "<*> Passed (2001) - EEPROM Checksumming is occurring."
  ut_setrequirements CS_2001, "P"
else
  write "<!> Failed (2001) - EEPROM Checksumming is not being calculated."
  ut_setrequirements CS_2001, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6: Send the Disable EEPROM Segment command for a valid      "
write ";  Enabled segment. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that entry's ID in the DisableTableName command
foundSeg = FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled segment found at index ", segIndex
else
  write "; There were no Enabled EEPROM segments found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_EEPROM_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - CS DisableEepromEntry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - CS DisableEepromEntry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

;; Dump the definition table to verify the entry's state was modified
;; This verifies DCR #18559
s get_tbl_to_cvt (ramDir,eeDefTblName,"A","$cpu_eedeftbl2_6",hostCPU,defTblId)
wait 5

if (p@$SC_$CPU_CS_EEPROM_DEF_TABLE[segindex].State = "Disabled") then
  write "<*> Passed - Definition Table entry changed to Disabled"
else
  write "<!> Failed - Definition Table entry was not changed"
endif

wait 5

step2_7:
write ";*********************************************************************"
write ";  Step 2.7: Dump the results table to ensure that the above entry was"
write ";  disabled. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_7",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of Eeprom Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of Eeprom Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.8: Using the TST_CS application, manipulate the disabled    "
write ";  entry's CRC. "
write ";*********************************************************************"
;; Send a TST_CS command to do this using the entry index as the argument
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"INFO",1

/$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=segIndex

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Send the Enable Segment command for the segment disabled"
write ";  in Step 2.6 above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_EEPROM_ENTRY_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_EEPROM_MISCOMPARE_ERR_EID,"ERROR", 2

write "*** Eeprom Segment Miscompare Ctr = ",$SC_$CPU_CS_EepromEC

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - CS Enable Eeprom Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - CS Enable Eeprom Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

;; Check that the Table miscompare counter incremented
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001.1) - Expected Event Msg ",CS_EEPROM_MISCOMPARE_ERR_EID," rcv'd."
  ut_setrequirements CS_20011, "P"
else
  write "<!> Failed (2001.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_EEPROM_MISCOMPARE_ERR_EID,"."
  ut_setrequirements CS_20011, "F"
endif

write "*** Eeprom Miscompare Ctr = ",$SC_$CPU_CS_EepromEC

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_10",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of Eeprom Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of Eeprom Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.11: Send the Recompute Eeprom Segment command for the segment"
write ";  specified in Steps above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Entry Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the completed message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2006.2) - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_20062, "P"
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.12: Send the Report EEPROM Segment command for the specified"
write ";  entry used in Steps above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.13: Dump the Results table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of Eeprom Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of Eeprom Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.14: Send the Get EEPROM Segment ID command using an entry in"
write ";  the results table. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Invalid Command Test."
write ";*********************************************************************"
write ";  Step 3.1: Send the Enable EEPROM Checksumming command with an     "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000020E99"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000020E99"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000020E99"
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
write ";  Step 3.2: Send the Disable EEPROM Checksumming command with an    "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000020F98"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000020F98"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000020F98"
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
write ";  Step 3.3: Send the Enable EEPROM Segment command with an invalid   "
write ";  length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041266"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000041226"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000041226"
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

write ";**********************************************************************"
write ";  Step 3.4: Send the Enable EEPROM Segment command with an invalid ID."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableEepromEntry EntryID=CS_MAX_NUM_EEPROM_TABLE_ENTRIES

ut_tlmwait  $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - CS Enable EEPROM Entry with ID=0 sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - CS Enable EEPROM Entry with ID=0 command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - Expected Event Msg ",CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Send the Disable EEPROM Segment command with an invalid  "
write ";  length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041377"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000041377"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000041377"
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
write ";  Step 3.6: Send the Disable EEPROM Segment command with an invalid ID."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableEepromEntry EntryID=CS_MAX_NUM_EEPROM_TABLE_ENTRIES

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - CS DisableTableName with Null Table name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - CS DisableTableName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - Expected Event Msg ",CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7: Send the Recompute EEPROM Segment command with invalid   "
write ";  length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041155"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000041155"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000041155"
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
write ";  Step 3.8: Send the Recompute EEPROM Segment command with an invalid "
write ";  ID. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom entryID=CS_MAX_NUM_EEPROM_TABLE_ENTRIES

ut_tlmwait  $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - CS Recompute EEPROM with ID=0 sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - CS Recompute EEPROM command with ID=0 did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.9: Send the Report EEPROM Segment command with an invalid   "
write ";  length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000041044"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000041044"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000041044"
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
write ";  Step 3.10: Send the Report EEPROM Segment command with an invalid ID."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Report EEPROM Command
/$SC_$CPU_CS_ReportEeprom entryID=CS_MAX_NUM_EEPROM_TABLE_ENTRIES

ut_tlmwait  $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - CS Report EEPROM command with ID=0 sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - CS Report EEPROM command with ID=0 did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - Expected Event Msg ",CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.11: Send the Get EEPROM Segment ID command with an invalid  "
write ";  length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000061422"

if ("$CPU" = "CPU2") then
  rawcmd = "199Fc00000061422"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A9Fc00000061422"
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
write ";  Step 3.12: Send the Get EEPROM Segment ID command with an invalid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID, "INFO", 1

local foundType=FALSE

for i = 1 to MEM_TABLE_SIZE DO
  if (p@$SC_$CPU_TST_CS_MemType[i] = "RAM") AND (foundType = FALSE) then
    ramAddress = $SC_$CPU_TST_CS_StartAddr[i] + 16
    foundType = TRUE
  endif
enddo

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry Command
/$SC_$CPU_CS_GetEepromEntryID Address=ramAddress

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}, 5
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - CS Get EEPROM ID command with invalid address sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - CS Get EEPROM ID command with invalid address did not increment CMDPC as expected."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_2009, "P"
else
  write "<!> Failed (1004;2009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.13: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl3_13",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 3.14: Send the Recompute EEPROM Segment command for an entry in"
write ";  the results table. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=1

write ";*********************************************************************"
write ";  Step 3.15: Send the Recompute EEPROM Segment command again to      "
write ";  verify that only 1 Recompute can occur at the same time. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID,"ERROR", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"INFO", 2

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=0

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2006.1) - CS Recompute EEPROM Entry command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_20061, "P"
else
  write "<!> Failed (1003;2006.1) - CS Recompute EEPROM Entry command did not increment CMDEC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_20061, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1003, "F"
endif

write ";*********************************************************************"
write ";  Step 3.16: Send the One Shot CRC command to verify that only 1 "
write ";  recompute or One Shot can occur at the same time. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_CHDTASK_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2006.1) - One Shot CRC command failed as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_20061, "P"
else
  write "<!> Failed (1003;2006.1) - One Shot CRC command did not increment CMDEC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_20061, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1003, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 180
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2006.2) - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_20062, "P"
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (2006.2)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (2006.2) - In Progress Flag set to False as expected."
  ut_setrequirements CS_20062, "P"
else
  write "<!> Failed (2006.2) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Disabled Checksum Test."
write ";*********************************************************************"
write ";  Step 4.1: Dump the Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_1",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.2: Send the Disable Checksum command.                       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_ALL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable All Command
/$SC_$CPU_CS_DisableAll

ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
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
write ";  Step 4.3: Send the Disable EEPROM Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable Eeprom Command
/$SC_$CPU_CS_DisableEeprom

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2003) - CS Disable EEPROM command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2003, "P"
else
  write "<!> Failed (1003;2003) - CS Disable EEPROM command did not increment CMDPC."
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
write ";  Step 4.4: Send the Disable EEPROM Segment command for an ENABLED  "
write ";  entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the DisableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_EEPROM_ENTRY_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - CS DisableEepromEntry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - CS DisableEepromEntry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

wait 5

step4_5:
write ";*********************************************************************"
write ";  Step 4.5: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_5",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.6: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.4 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 180
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2006.2) - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
  ut_setrequirements CS_20062, "P"
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.4 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.8: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9: Send the Enable EEPROM Segment command for a DISABLED "
write ";  entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the EnableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_EEPROM_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - CS Enable Eeprom Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - CS Enable Eeprom Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

wait 5

step4_10:
write ";*********************************************************************"
write ";  Step 4.10: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_10",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.11: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.9 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.12: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.9 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.13: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.14: Send the Enable EEPROM Checksumming command.            "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable EEPROM Command
/$SC_$CPU_CS_EnableEeprom

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - CS Enable EEPROM command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2002, "P"
else
  write "<!> Failed (1003;2002) - CS Enable EEPROM command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - Expected Event Msg ",CS_ENABLE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2002, "P"
else
  write "<!> Failed (1003;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.15: Send the Disable EEPROM Segment command for an ENABLED  "
write ";  entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the DisableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_EEPROM_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - CS DisableEepromEntry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - CS DisableEepromEntry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

wait 5

step4_16:
write ";*********************************************************************"
write ";  Step 4.16: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_16",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.17: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.15 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeEeprom Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.18: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.15 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.19: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Segment ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.20: Send the Enable EEPROM Segment command for a DISABLED "
write ";  entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the EnableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_EEPROM_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - CS Enable Eeprom Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - CS Enable Eeprom Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

wait 5

step4_21:
write ";*********************************************************************"
write ";  Step 4.21: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_21",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.22: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.20 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeEeprom Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.23: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.20 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.24: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
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
write ";  Step 4.26: Send the Disable EEPROM Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the DisableEeprom Command
/$SC_$CPU_CS_DisableEeprom

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2003) - CS Disable EEPROM command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2003, "P"
else
  write "<!> Failed (1003;2003) - CS Disable EEPROM command did not increment CMDPC."
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
write ";  Step 4.27: Send the Disable EEPROM Segment command for an ENABLED  "
write ";  entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the DisableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_EEPROM_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - CS DisableEepromEntry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - CS DisableEepromEntry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

wait 5

step4_28:
write ";*********************************************************************"
write ";  Step 4.28: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_28",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.29: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.27 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.30: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.27 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.31: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.32: Send the Enable EEPROM Segment command for a DISABLED "
write ";  entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the EnableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_EEPROM_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Entry Command
/$SC_$CPU_CS_EnableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - CS Enable Eeprom Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - CS Enable Eeprom Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

wait 5

step4_32:
write ";*********************************************************************"
write ";  Step 4.33: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_33",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.34: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.32 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.35: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.32 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Report Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.36: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.37: Send the Enable EEPROM Checksumming command.            "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable EEPROM Command
/$SC_$CPU_CS_EnableEeprom

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - CS Enable EEPROM command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2002, "P"
else
  write "<!> Failed (1003;2002) - CS Enable EEPROM command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - Expected Event Msg ",CS_ENABLE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2002, "P"
else
  write "<!> Failed (1003;2002) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.38: Send the Disable EEPROM Segment command for an ENABLED  "
write ";  entry."
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; enabled. Once found, use that ID in the DisableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Enabled entry found at index ", segIndex
else
  write "; There were no Enabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_EEPROM_ENTRY_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable Entry Command
/$SC_$CPU_CS_DisableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - CS DisableEepromEntry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - CS DisableEepromEntry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2005) - Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2005, "P"
else
  write "<!> Failed (1003;2005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2005, "F"
endif

wait 5

step4_39:
write ";*********************************************************************"
write ";  Step 4.39: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_39",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.40: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.38 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.41: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.38 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.42: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_GET_ENTRY_ID_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.43: Send the Enable EEPROM Segment command for a DISABLED "
write ";  entry. "
write ";*********************************************************************"
;; loop through the Results table until you find an entry whose state is
;; disabled. Once found, use that ID in the EnableEepromEntry command
foundSeg=FALSE

for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Disabled") AND (foundSeg = FALSE) then
    segIndex = i
    foundSeg = TRUE
  endif
enddo

if (foundSeg = TRUE) then
  write "; Disabled entry found at index ", segIndex
else
  write "; There were no Disabled entries found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_ENABLE_EEPROM_ENTRY_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable Segment Command
/$SC_$CPU_CS_EnableEepromEntry EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - CS Enable Eeprom Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - CS Enable Eeprom Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2004, "P"
else
  write "<!> Failed (1003;2004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_EEPROM_ENTRY_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2004, "F"
endif

wait 5

step4_44:
write ";*********************************************************************"
write ";  Step 4.44: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_44",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.45: Send the Recompute EEPROM Segment command for the entry  "
write ";  used in Step 4.43 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Recompute Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - CS Recompute EEPROM Entry command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - CS Recompute EEPROM Entry command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2006) - Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2006, "P"
else
  write "<!> Failed (1003;2006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2006, "F"
endif

;; Check for the Recompute Finished message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event msg ",CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID," rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.46: Send the Report EEPROM Segment command for the entry     "
write ";  used in Step 4.43 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_EEPROM_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportEeprom Command
/$SC_$CPU_CS_ReportEeprom EntryID=segIndex

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - CS Report EEPROM Segment CRC command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - CS Report EEPROM Segment CRC command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2007) - Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2007, "P"
else
  write "<!> Failed (1003;2007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.47: Send the Get EEPROM Segment ID command with a valid "
write ";  address. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_GET_ENTRY_ID_EEPROM_INF_EID,"INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Get EEPROM Entry ID Command
/$SC_$CPU_CS_GetEepromEntryID Address=$SC_$CPU_CS_EEPROM_RESULT_TABLE[1].StartAddr

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - CS Get EEPROM Entry ID command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - CS Get EEPROM Entry ID command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  write "<*> Passed (1003;2008) - Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_2008, "P"
else
  write "<!> Failed (1003;2008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_GET_ENTRY_ID_EEPROM_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_2008, "F"
endif

wait 5
write ";*********************************************************************"
write ";  Step 5.0: Definition Table Update Test."
write ";*********************************************************************"
write ";  Step 5.1: Create an EEPROM Definition table load file that contains "
write ";  all empty items."
write ";*********************************************************************"
s $sc_$cpu_cs_edt4
wait 5

write ";*********************************************************************"
write ";  Step 5.2: Send the command to load the file created above.         "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("eepromdefemptytable", hostCPU)

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

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - EEPROM Definition Table validation failed."
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

write ";*********************************************************************"
write ";  Step 5.4: Send the Recompute EEPROM Segment command for a valid   "
write ";  entry specified in the Results Table."
write ";*********************************************************************"
;; Send the Command
/$SC_$CPU_CS_RecomputeEeprom EntryID=1

write ";*********************************************************************"
write ";  Step 5.5: Send the command to Activate the file loaded in Step 5.2."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,"INFO", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate EEPROM Definition Table command sent properly."
else
  write "<!> Failed - Activate EEPROM Definition Table command."
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Need to wait until the Recompute from Step 5.4 completes in order for the
;; Activate to be performed.

;; Wait for the recompute finished event message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 180
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Recompute Finished Event Message rcv'd."
else
  write "<!> Failed (2006.2) - Recompute Finished Event msg not rcv'd as expected."
  ut_setrequirements CS_20062, "F"
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - EEPROM Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6: Dump the Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl5_6",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 5.7: Create an EEPROM Definition table load file containing "
write ";  several valid entries, an entry that contains an invalid address, an"
write ";  entry that contains an invalid range and an entry with an invalid "
write ";  state."
write ";*********************************************************************"
s $sc_$cpu_cs_edt2
wait 5

write ";*********************************************************************"
write ";  Step 5.8: Send the command to load the invalid file created above."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("eeprom_def_invalid", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.9: Send the command to validate the file loaded in Step 5.8"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_RANGE_ERR_EID,"ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - EEPROM Definition Table validate command failed."
endif

;; Wait for the Validation Error event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table validation failed with an invalid range."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - EEPROM Definition Table validation was successful with an invalid state entry."
endif

;; Wait for the Validation Error event message from CS
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001.2) - EEPROM Definition Table validation failed with an invalid range."
  ut_setrequirements CS_20012, "P"
else
  write "<!> Failed (2001.2) - EEPROM Definition Table validation was successful with an invalid state entry."
  ut_setrequirements CS_20012, "F"
endif

write ";*********************************************************************"
write ";  Step 5.10: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=eeDefTblName

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
write ";  Step 5.11: Send the command to load the invalid state table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("eeprom_def_invalid2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.12: Send the command to validate the file loaded in Step 5.11"
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_STATE_ERR_EID,"ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - EEPROM Definition Table validate command failed."
endif

;; Wait for the Validation Error event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table validation failed with an invalid state."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - EEPROM Definition Table validation was successful with an invalid state entry."
endif

;; Wait for the Validation Error event message from CS
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001.2) - EEPROM Definition Table validation failed with an invalid state."
  ut_setrequirements CS_20012, "P"
else
  write "<!> Failed (2001.2) - EEPROM Definition Table validation was successful with an invalid state entry."
  ut_setrequirements CS_20012, "F"
endif

write ";*********************************************************************"
write ";  Step 5.13: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=eeDefTblName

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
write ";  Step 5.14: Create an EEPROM Definition table load file containing "
write ";  entries that overlap and empty entries in between valid entries.  "
write ";*********************************************************************"
s $sc_$cpu_cs_edt3
wait 5

write ";*********************************************************************"
write ";  Step 5.15: Send the command to load the file with valid entries.   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("eeprom_def_ld_2", hostCPU)

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
write ";  Step 5.16: Send the command to validate the file loaded in Step 5.15"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - EEPROM Definition Table validation failed."
endif

;; Look for the validation event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table validation event '", $SC_$CPU_find_event[2].eventid,"' found!"
else
  write "<!> Failed - EEPROM Definition Table validation event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_VALIDATION_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.17: Send the command to Activate the file loaded in Step 5.15"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=eeDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate EEPROM Definition Table command sent properly."
else
  write "<!> Failed - Activate EEPROM Definition Table command."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - EEPROM Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - EEPROM Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.18: Dump the Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl5_15",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of Eeprom Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of Eeprom Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 5.19: Corrupt simulated EEPROM using the TST_CS application.  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_EEPROM_MEM_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_EEPROM_MISCOMPARE_ERR_EID,"ERROR", 2

;; Set the expected EEPROM Miscompare Counter before corrupting the memory
local eepromMiscompareCtr = $SC_$CPU_CS_EepromEC

/$SC_$CPU_TST_CS_CorruptEeprom
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - TST_CS Corrupt EEPROM command sent successfully."
else
  write "<!> Failed - TST_CS Corrupt EEPROM command was not successful."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.20: Monitor the EEPROM Miscompare Counter to verify that   "
write ";  miscompares are occurring. "
write ";*********************************************************************"
;; Wait for the CS application to attempt to recalculate the checksums
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 120

if ($SC_$CPU_CS_EepromEC > eepromMiscompareCtr) then
  write "<*> Passed (2001.1) - Eeprom Miscompare Counter incremented after memory corruption."
  ut_setrequirements CS_20011, "P"
else
  write "<!> Failed (2001.1) - Eeprom Miscompare counter did not increment after memory corruption."
  ut_setrequirements CS_20011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0: Definition Table Initialization Test."
write ";*********************************************************************"
write ";  Step 6.1: Send the command to stop the CS & TST_CS Applications. "
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CSAppName
wait 5

write ";********************************************************************"
write ";  Step 6.2: Download the default Memory Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_EEPROM_TABLE_FILENAME -> full path file spec.
;; This define is set to "/cf/apps/cs_eepromtbl.tbl" in cs_platform_cfg.h
;; Parse the filename configuration parameter for the default table filename
local tableFileName = CS_DEF_EEPROM_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")
write "==> Default EEPROM Code Segment Table filename config param = '",tableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo
write "==> Table filename ONLY = '",tableFileName,"'"

;; Download the table
s ftp_file ("CF:0/apps",tableFileName,"cs_eeprom_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 6.2: Delete the EEPROM Definition table default load file from "
write ";  $CPU. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","na",tableFileName,hostCPU,"R")

write ";*********************************************************************"
write ";  Step 6.3: Start the CS and TST_CS Applications. "
write ";*********************************************************************"
s $sc_$cpu_cs_start_apps("6.3")

write ";*********************************************************************"
write ";  Step 6.4: Dump the Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl6_4",hostCPU,resTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of Eeprom Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of Eeprom Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 7.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 7.1: Upload the default Application Code Segment Definition   "
write ";  table downloaded in step 1.1. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","cs_eeprom_orig_tbl.tbl",tableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 7.2: Send the Power-On Reset command. "
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
write ";  End procedure $SC_$CPU_cs_nvmem"
write ";*********************************************************************"
ENDPROC
