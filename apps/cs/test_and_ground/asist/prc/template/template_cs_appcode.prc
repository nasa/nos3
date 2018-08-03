PROC $sc_$cpu_cs_appcode
;*******************************************************************************
;  Test Name:  cs_appcode
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) application code segment
;	checksumming commands function properly and handles anomolies properly.
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
;    CS4000	Checksum shall calculate CRCs for each Table-Defined
;		Application's code segment and compare them against the
;		corresponding Application's baseline code segment CRC if:
;			a. Checksumming (as a whole) is Enabled
;			b. App code segment checksumming is Enabled
;			c. Checksumming of the individual Application Code
;			   segment is Enabled
;    CS4000.1	If the Application's code segment CRC is not equal to the
;		corresponding Application's baseline code segment CRC, CS shall
;		increment the Application Code Segment CRC Miscompare Counter
;		and send an event message.	
;    CS4000.2	If the table-defined Application code segment is invalid, CS
;		shall send an event message and skip that Application code
;		segment.
;    CS4001	Upon receipt of an Enable Application checksumming command, CS
;		shall enable checksumming of all Application code segments.
;    CS4002	Upon receipt of a Disable Application checksumming command, CS
;		shall disable checksumming of all Application code segments.
;    CS4003	Upon receipt of an Enable Application code segment command, CS
;		shall enable checksumming of the command-specified Application.
;    CS4004	Upon receipt of a Disable Application code segment command, CS
;		shall disable checksumming of the command-specified Application.
;    CS4005	Upon receipt of a Recompute Application Code Segment CRC
;		command, CS shall:
;			a) Recompute the baseline CRC for the Application
;			b) Set the Recompute In Progress Flag to TRUE
;		Application.
;    CS4005.1	Once the baseline CRC is computed, CS shall;
;			a) Generate an event message containing the baseline CRC
;			b) Set the Recompute In Progress Flag to FALSE
;    CS4005.2	If CS is already processing a Recompute CRC command, CS shall
;		reject the command.
;    CS4006	Upon receipt of a Report Application Code Segment CRC command, 
;		CS shall send an event message containing the baseline
;		Application code segment CRC.
;    CS4007	If the command-specified Application is invalid (for any
;		Application Code Segment command where the Application is a
;		command argument), CS shall reject the command and send an
;		event message.
;    CS4008	CS shall provide the ability to dump the baseline CRCs and
;		status for the Application code segment memory segments via a
;		dump-only table.
;    CS7000	The CS software shall limit the amount of bytes processed during
;		each of its execution cycles to a maximum of <PLATFORM_DEFINED>
;		bytes.
;    CS8000	Upon receipt of an Enable Checksum command, CS shall start
;		calculating CRCs and compare them against the baseline CRCs.
;    CS8001	Upon receipt of a Disable Checksum command, CS shall stop
;		calculating CRCs and comparing them against the baseline CRCs.
;    CS9000	CS shall generate a housekeeping message containing the
;		following:
;			a. Valid Ground Command Counter
;			b. Ground Command Rejected Counter
;			c. Overall CRC enable/disable status
;			d. Total Non-Volatile Baseline CRC
;			e. OS code segment Baseline CRC
;			f. cFE code segment Baseline CRC
;			g. Non-Volatile CRC Miscompare Counter
;			h. OS Code Segment CRC Miscompare Counter
;			i. cFE Code Segment CRC Miscompare Counter
;			j. Application CRC Miscompare Counter
;			k. Table CRC Miscompare Counter
;			l. User-Defined Memory CRC Miscompare Counter
;			m. Last One Shot Address
;			n. Last One Shot Size
;			o. Last One Shot Checksum
;			p. Checksum Pass Counter (number of passes thru all of
;			   the checksum areas)
;			q. Current Checksum Region (Non-Volatile, OS code
;			   segment, cFE Code Segment etc)
;			r. Non-Volatile CRC enable/disable status
;			s. OS Code Segment CRC enable/disable status
;			t. cFE Code Segment CRC enable/disable status
;			u. Application CRC enable/disable status
;			v. Table CRC enable/disable status
;			w. User-Defined Memory CRC enable/disable status
;			x. Last One Shot Rate
;			y) Recompute In Progress Flag
;			z) One Shot In Progress Flag
;    CS9001	Upon initialization of the CS Application(CE Power On, cFE
;		Processor Reset, or CS Application Reset), CS shall initialize
;		the following data to Zero:
;			a. Valid Ground Command Counter
;			b. Ground Command Rejected Counter
;			c. Non-Volatile CRC Miscompare Counter
;			d. OS Code Segment CRC Miscompare Counter
;			e. cFE Code Segment CRC Miscompare Counter
;			f. Application CRC Miscompare Counter
;			g. Table CRC Miscompare Counter
;			h. User-Defined Memory CRC Miscompare Counter
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
;	07/18/08	Walt Moleski	Original Procedure.
;	03/11/10	Walt Moleski	Updated to use the telemetry arrays
;					that start with zero (0).
;       09/22/10        Walt Moleski    Updated to use variables for the CFS
;                                       application name and ram disk. Replaced
;					all setupevt instances with setupevents
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
#define CS_4000		3
#define CS_40001	4
#define CS_40002	5
#define CS_4001		6
#define CS_4002		7
#define CS_4003		8
#define CS_4004		9
#define CS_4005		10
#define CS_40051	11
#define CS_40052	12
#define CS_4006		13
#define CS_4007		14
#define CS_4008		15
#define CS_7000		16
#define CS_8000		17
#define CS_8001		18
#define CS_9000		19
#define CS_9001		20

global ut_req_array_size = 20
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1002", "CS_1003", "CS_1004", "CS_4000", "CS_4000.1", "CS_4000.2", "CS_4001", "CS_4002", "CS_4003", "CS_4004", "CS_4005", "CS_4005.1", "CS_4005.2", "CS_4006", "CS_4007", "CS_4008", "CS_7000", "CS_8000", "CS_8001", "CS_9000", "CS_9001" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream
LOCAL defAppId, defPktId, resAppId, resPktId
local i,appIndex,appName,foundApp
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME
local appResTblName = CSAppName & "." & CS_RESULTS_APP_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defAppId = "0FAF"
resAppId = "0FB3"
defPktId = 4015
resPktId = 4019

write ";*********************************************************************"
write ";  Step 1.0: Checksum Application Code Segment Test Setup."
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
page $SC_$CPU_CS_APP_DEF_TABLE
page $SC_$CPU_CS_APP_RESULTS_TBL

write ";*********************************************************************"
write ";  Step 1.3: Create & upload the Application Code Segment Definition "
write ";  Table file to be used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_adt1
wait 5

write ";*********************************************************************"
write ";  Step 1.4: Start the TST_CS_MemTbl application in order to setup    "
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

write ";**********************************************************************"
write ";  Step 1.6: Load the Application Definition file created above.       "
write ";**********************************************************************"
start load_table ("app_def_tbl_ld_1", hostCPU)
wait 5

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 2

local cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=appDefTblName

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

/$SC_$CPU_TBL_ACTIVATE ATableName=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Application Definition Table command sent properly."
else
  write "<!> Failed - Activate Application Definition Table command."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Application Definition Table update failed."
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

wait 5

write ";*********************************************************************"
write ";  Step 1.8: Enable DEBUG Event Messages "
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
write ";  Step 1.9: Verify that the CS Housekeeping telemetry items are "
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
write ";  Step 1.10: Dump the Application Code Segment Definition Table."
write ";*********************************************************************"
s get_tbl_to_cvt (ramDir,appDefTblName,"A","$cpu_appdeftbl1_10",hostCPU,defAppId)
wait 5

write ";*********************************************************************"
write ";  Step 2.0: Application Code Segment Test."
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

;; Disable Table Checksumming if it is enabled
if (p@$SC_$CPU_CS_TableState = "Enabled") then
  /$SC_$CPU_CS_DisableTables
  wait 1
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send the Enable Application Checksum command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Apps Command
/$SC_$CPU_CS_EnableApps

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - CS EnableApps command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4001, "P"
else
  write "<!> Failed (1003;4001) - CS EnableApps command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Expected Event Msg ",CS_ENABLE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4001, "P"
else
  write "<!> Failed (1003;4001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl2_3",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.4: Verify that Applications are being checksummed."
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
local keepDumpingResults=FALSE

while (keepDumpingResults = FALSE) do
  s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl2_4",hostCPU,resAppId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
    ;; If the CRC has been computed AND the CRC is not zero -> Stop
    if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].ComputedYet = "TRUE") AND ;;
       ($SC_$CPU_CS_APP_RESULT_TABLE[i].BaselineCRC <> 0) AND ;;
       (keepDumpingResults = FALSE) then
      keepDumpingResults = TRUE
    endif
  enddo
enddo

if (keepDumpingResults = TRUE) then
  write "<*> Passed (4000) - Application Checksumming is occurring."
  ut_setrequirements CS_4000, "P"
else
  write "<!> Failed (4000) - Application Checksumming is not being calculated."
  ut_setrequirements CS_4000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5: Send the Disable Application Code Segment command for a "
write ";  valid Enabled application. "
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; enabled. Once found, use that app's name in the DisableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Enabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Enabled apps found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_APP_NAME_INF_EID,"INFO",1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Disable App Command
/$SC_$CPU_CS_DisableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - CS DisableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - CS DisableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

;; Dump the definition table to verify the entry's state was modified
s get_tbl_to_cvt (ramDir,appDefTblName,"A","$cpu_appdeftbl2_5",hostCPU,defAppId)
wait 5

if (p@$SC_$CPU_CS_APP_DEF_TABLE[appindex].State = "Disabled") then
  write "<*> Passed - Definition Table entry changed to Disabled"
else
  write "<!> Failed - Definition Table entry was not changed"
endif

wait 5

step2_6:
write ";*********************************************************************"
write ";  Step 2.6: Using the TST_CS application, manipulate the disabled    "
write ";  application's CRC. "
write ";*********************************************************************"
;; Send a TST_CS command to do this using appName as the argument
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_APP_CRC_INF_EID,"INFO",1

/$SC_$CPU_TST_CS_CorruptAppCRC AppName=appName

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Send the Enable Application Code Segment command for the "
write ";  application disabled in Step 2.5 above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_NAME_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_APP_MISCOMPARE_ERR_EID, "ERROR", 2

write "*** App Miscompare Ctr = ",$SC_$CPU_CS_AppEC

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the Enable App Name Command
/$SC_$CPU_CS_EnableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - CS EnableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - CS EnableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

; Wait for the CS background Checksumming to occur
;; Check that the Application miscompare counter incremented
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4000.1) - Expected Event Msg ",CS_APP_MISCOMPARE_ERR_EID," rcv'd."
  ut_setrequirements CS_40001, "P"
else
  write "<!> Failed (4000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_APP_MISCOMPARE_ERR_EID,"."
  ut_setrequirements CS_40001, "F"
endif

write "*** App Miscompare Ctr = ",$SC_$CPU_CS_AppEC

wait 5

write ";*********************************************************************"
write ";  Step 2.8: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl2_8",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.9: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 2.5 above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO", 2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Send the Report Application Code Segment CRC command "
write ";  for the application specified in Step 2.5 above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.11: Stop the TST_TBL application. Verify that an event is    "
write ";  generated indicating the TST_TBL application was skipped."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_COMPUTE_APP_NOT_FOUND_ERR_EID,"ERROR",1

/$SC_$CPU_ES_DELETEAPP Application="TST_TBL"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 40
if (UT_TW_Status = UT_Success) then
;; Check for the event message
  write "<*> Passed (4000.2) - Expected Event Msg ",CS_COMPUTE_APP_NOT_FOUND_ERR_EID," rcv'd."
  ut_setrequirements CS_40002, "P"
else
  write "<!> Failed (4000.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_COMPUTE_APP_NOT_FOUND_ERR_EID,"."
  ut_setrequirements CS_40002, "F"
endif

write ";*********************************************************************"
write ";  Step 2.12: Start the TST_TBL application. Verify that checksumming  "
write ";  is again occuring on the TST_TBL application."
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

wait 5

write ";*********************************************************************"
write ";  Step 2.13: Send the Recompute Application Code Segment CRC command "
write ";  for the TST_TBL application. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName="TST_TBL"

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Invalid Command Test."
write ";*********************************************************************"
write ";  Step 3.1: Send the Enable Application Checksum command with an     "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000022299"

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
write ";  Step 3.2: Send the Disable Application Checksum command with an    "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc00000022398"

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
write ";  Step 3.3: Send the Enable Application Code Segment command with an "
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000002526"

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
write ";  Step 3.4: Send the Enable Application Code Segment command with an "
write ";  invalid application. "
write ";*********************************************************************"
write ";  Step 3.4.1: Send the command with a null application name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the EnableAppName Command
/$SC_$CPU_CS_EnableAppName AppName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS EnableAppName with Null Appname sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS EnableAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4.2: Send the command with an application name that is not "
write ";  currently executing. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the EnableAppName Command
/$SC_$CPU_CS_EnableAppName AppName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS EnableAppName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS EnableAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5
 
write ";*********************************************************************"
write ";  Step 3.5: Send the Disable Application Code Segment command with an"
write ";  invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000002527"

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
write ";  Step 3.6: Send the Disable Application Code Segment command with an"
write ";  invalid application. "
write ";*********************************************************************"
write ";  Step 3.6.1: Send the command with a null application name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the DisableAppName Command
/$SC_$CPU_CS_DisableAppName AppName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS DisableAppName with Null Appname sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS DisableAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.2: Send the command with an application name that is not "
write ";  currently executing. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the DisableAppName Command
/$SC_$CPU_CS_DisableAppName AppName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS DisableAppName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS DisableAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7: Send the Recompute Application Code Segment CRC command  "
write ";  with an invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000002525"

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
write ";  Step 3.8: Send the Recompute Application Code Segment CRC command  "
write ";  with an invalid application. "
write ";*********************************************************************"
write ";  Step 3.8.1: Send the command with a null application name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS RecomputeAppName with Null Appname sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS RecomputeAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.8.2: Send the command with an application name that is not "
write ";  currently executing. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS RecomputeAppName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS RecomputeAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.9: Send the Report Application Code Segment CRC command with"
write ";  an invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CS_CMDEC + 1

;; CPU1 is the default
rawcmd = "189Fc000002524"

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
write ";  Step 3.10: Send the Report Application Code Segment CRC command    "
write ";  with an invalid application. "
write ";*********************************************************************"
write ";  Step 3.10.1: Send the command with a null application name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_INVALID_NAME_APP_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=""

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS ReportAppName with Null Appname sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS ReportAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_BASELINE_INVALID_NAME_APP_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_INVALID_NAME_APP_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.10.2: Send the command with an application name that is not "
write ";  currently executing. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_INVALID_NAME_APP_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName="CS_TST"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - CS ReportAppName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - CS ReportAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4007) - Expected Event Msg ",CS_BASELINE_INVALID_NAME_APP_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_4007, "P"
else
  write "<!> Failed (1004;4007) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_INVALID_NAME_APP_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.11: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl3_11",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 3.12: Send the Recompute Application Code Segment CRC command  "
write ";  for an enabled application. "
write ";*********************************************************************"
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=CSAppName

;; Verify the telemetry flag is set to TRUE (4005)
ut_tlmwait $SC_$CPU_CS_RecomputeInProgress, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

write ";*********************************************************************"
write ";  Step 3.13: Send the Recompute Application Code Segment CRC command "
write ";  for a different enabled application. Verify that this second "
write ";  is rejected since only 1 recompute command can occur at a time."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_RECOMPUTE_APP_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName="TST_CS"

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4005.2) - CS RecomputeAppName with invalid name sent properly."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_40052, "P"
else
  write "<!> Failed (1004;4005.2) - CS RecomputeAppName command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_40052, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_APP_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.14: Send the Recompute Application Code Segment CRC command  "
write ";  for an enabled application. "
write ";*********************************************************************"
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=CSAppName

write ";*********************************************************************"
write ";  Step 3.15: Send a One Shot CRC command. Verify that this command "
write ";  is rejected."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_ONESHOT_CHDTASK_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CS_CMDEC + 1
;; Send the One Shot Command
/$SC_$CPU_CS_OneShot Address=$SC_$CPU_TST_CS_StartAddr[1] RegionSize=2048 MaxBytes=2048

ut_tlmwait $SC_$CPU_CS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4005.2) - One Shot CRC command faild as expected."
  ut_setrequirements CS_1004, "P"
  ut_setrequirements CS_40052, "P"
else
  write "<!> Failed (1004;4005.2) - One Shot CRC command did not increment CMDEC."
  ut_setrequirements CS_1004, "F"
  ut_setrequirements CS_40052, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",CS_RECOMPUTE_APP_CHDTASK_ERR_EID," rcv'd."
  ut_setrequirements CS_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_CHDTASK_ERR_EID,"."
  ut_setrequirements CS_1004, "F"
endif

wait 5

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
write ";  Step 4.2: Send the Disable Application Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable Apps Command
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
write ";  Step 4.3: Send the Enable Application Code Segment command for an "
write ";  application whose status is DISABLED."
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; disabled. Once found, use that app's name in the EnableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Disabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Disabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Disabled apps found in the results table"
endif

;; Send the EnableAppName command
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the EnableAppName Command
/$SC_$CPU_CS_EnableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - CS EnableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - CS EnableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

wait 5

step4_4:
write ";*********************************************************************"
write ";  Step 4.4: Dump the Application Code Segment Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_4",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.5: Send the Recompute Application Code Segment CRC command  "
write ";  for the application specified in Step 4.3 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6: Send the Report Application Code Segment CRC command for "
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Send the Disable Application Code Segment command with an"
write ";  application whose status is ENABLED."
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; enabled. Once found, use that app's name in the DisableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Enabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Enabled apps found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the DisableAppName command
/$SC_$CPU_CS_DisableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - CS DisableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - CS DisableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

wait 5

step4_8:
write ";*********************************************************************"
write ";  Step 4.8: Dump the Application Code Segment Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_8",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.9: Send the Recompute Application Code Segment CRC command  "
write ";  for the application specified in Step 4.7 above.                   "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.10: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.11: Send the Enable Application Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the EnableApps Command
/$SC_$CPU_CS_EnableApps

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - CS EnableApps command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4001, "P"
else
  write "<!> Failed (1003;4001) - CS EnableApps command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Expected Event Msg ",CS_ENABLE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4001, "P"
else
  write "<!> Failed (1003;4001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.12: Send the Enable Application Code Segment command with an"
write ";  application whose status is DISABLED.                              "
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; disabled. Once found, use that app's name in the EnableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Disabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Disabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Disabled apps found in the results table"
endif

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the EnableAppName Command
/$SC_$CPU_CS_EnableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - CS EnableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - CS EnableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

wait 5

step4_13:
write ";*********************************************************************"
write ";  Step 4.13: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_13",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.14: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 4.12 above.                  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.15: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.16: Send the Disable Application Code Segment command with  "
write ";  an application whose status is ENABLED.                            "
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; enabled. Once found, use that app's name in the DisableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Enabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Enabled apps found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the DisableAppName Command
/$SC_$CPU_CS_DisableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - CS DisableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - CS DisableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

wait 5

step4_17:
write ";*********************************************************************"
write ";  Step 4.17: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_17",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.18: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 4.16 above.                  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.19: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.20: Send the Enable Checksum command.                       "
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
write ";  Step 4.21: Send the Disable Application Checksumming command.      "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Disable Apps Command
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
write ";  Step 4.22: Send the Enable Application Code Segment command with an"
write ";  application whose status is DISABLED."
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; disabled. Once found, use that app's name in the EnableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Disabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Disabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Disabled apps found in the results table"
endif

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the EnableAppName command
/$SC_$CPU_CS_EnableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - CS EnableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - CS EnableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

wait 5

step4_23:
write ";*********************************************************************"
write ";  Step 4.23: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_23",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.24: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 4.22 above.                  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.25: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.26: Send the Disable Application Code Segment command with  "
write ";  an application whose status is ENABLED."
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; enabled. Once found, use that app's name in the DisableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Enabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Enabled apps found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the DisableAppName command
/$SC_$CPU_CS_DisableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - CS DisableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - CS DisableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

wait 5

step4_27:
write ";*********************************************************************"
write ";  Step 4.27: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_27",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.28: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 4.26 above.                  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.29: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.30: Send the Enable Application Checksumming command.       "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1

;; Send the Enable Apps Command
/$SC_$CPU_CS_EnableApps

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - CS EnableApps command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4001, "P"
else
  write "<!> Failed (1003;4001) - CS EnableApps command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Expected Event Msg ",CS_ENABLE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4001, "P"
else
  write "<!> Failed (1003;4001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.31: Send the Enable Application Code Segment command with an"
write ";  application whose status is DISABLED.                              "
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; disabled. Once found, use that app's name in the EnableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Disabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Disabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Disabled apps found in the results table"
endif

ut_setupevents "$SC", "$CPU", {CSAppName}, CS_ENABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the EnableAppName command
/$SC_$CPU_CS_EnableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - CS EnableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - CS EnableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4003) - Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_ENABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4003, "F"
endif

wait 5

step4_32:
write ";*********************************************************************"
write ";  Step 4.32: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_32",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.33: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 4.31 above.                  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.34: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.35: Send the Disable Application Code Segment command with  "
write ";  an application whose status is ENABLED.                            "
write ";*********************************************************************"
;; loop through the APP Results table until you find an entry whose state is
;; enabled. Once found, use that app's name in the DisableAppName command
foundApp=FALSE

for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") AND (foundApp = FALSE) then
    appIndex = i
    foundApp = TRUE
  endif
enddo

if (foundApp = TRUE) then
  appName = $SC_$CPU_CS_APP_RESULT_TABLE[appIndex].NAME
  write "; Enabled app '",appName, "' found at index ", appIndex
else
  appName = "TST_CS"
  write "; There were no Enabled apps found in the results table"
endif

ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_APP_NAME_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the DisableAppName command
/$SC_$CPU_CS_DisableAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - CS DisableAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - CS DisableAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4004, "P"
else
  write "<!> Failed (1003;4004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_APP_NAME_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4004, "F"
endif

wait 5

step4_36:
write ";*********************************************************************"
write ";  Step 4.36: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_36",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 4.37: Send the Recompute Application Code Segment CRC command "
write ";  for the application specified in Step 4.35 above.                  "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID," rcv'd."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_APP_INF_EID,"."
  ut_setrequirements CS_40051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (4005.1)
if (p@$SC_$CPU_CS_RecomputeInProgress = "False") then
  write "<*> Passed (4005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_40051, "P"
else
  write "<!> Failed (4005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_40051, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.38: Send the Report Application Code Segment CRC command for"
write ";  for the application specified above.                               "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {CSAppName}, CS_BASELINE_APP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the ReportAppName Command
/$SC_$CPU_CS_ReportAppName AppName=appName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - CS ReportAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - CS ReportAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4006) - Expected Event Msg ",CS_BASELINE_APP_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_BASELINE_APP_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Definition Table Update Test."
write ";*********************************************************************"
write ";  Step 5.1: Create an Application Code Segment Definition table load "
write ";  file that contains all empty items."
write ";*********************************************************************"
s $sc_$cpu_cs_adt4
wait 5

write ";*********************************************************************"
write ";  Step 5.2: Send the command to load the file created above.         "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("appdefemptytable", hostCPU)

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
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Application Definition Table validation failed."
endif

;; Wait for the Validation Success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

write ";*********************************************************************"
write ";  Step 5.4: Send the Recompute Application Code Segment CRC command "
write ";  for an application specified in the Results table. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_APP_STARTED_DBG_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_RECOMPUTE_FINISH_APP_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_CS_CMDPC + 1
;; Send the RecomputeAppName Command
/$SC_$CPU_CS_RecomputeAppName AppName=CSAppName

ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - CS RecomputeAppName command sent properly."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - CS RecomputeAppName command did not increment CMDPC."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4005) - Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_APP_STARTED_DBG_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_4005, "F"
endif

;; Verify the telemetry flag is set to TRUE (4005)
if (p@$SC_$CPU_CS_RecomputeInProgress = "True") then
  write "<*> Passed (4005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_4005, "P"
else
  write "<!> Failed (4005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_4005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5: Send the command to Activate the file loaded in Step 5.2."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Application Definition Table command sent properly."
else
  write "<!> Failed - Activate Application Definition Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Application Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6: Dump the Application Code Segment Results table.         "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl5_6",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 5.7: Create an Application Code Segment Definition table load "
write ";  file containing empty entries in between valid entries.            "
write ";*********************************************************************"
s $sc_$cpu_cs_adt2

;; Create a table load file with an invalid state (> 3)
s $sc_$cpu_cs_adt3

write ";*********************************************************************"
write ";  Step 5.8: Send the command to load the invalid file created above."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("app_def_tbl_invalid", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.9: Send the command to validate the file loaded in Step 5.8 "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Application Definition Table validate command failed."
endif

;; Wait for the Validation Error event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table validation failed with an invalid state."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Application Definition Table validation was successful with an invalid state entry."
endif

write ";*********************************************************************"
write ";  Step 5.10: Send the command to abort the invalid load.             "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.11: Send the command to load the file with valid entries.   "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("app_def_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command sent successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

;; Check for the Event message generation
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
wait 5

write ";*********************************************************************"
write ";  Step 5.12: Send the command to validate the file loaded in Step 5.11"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Application Definition Table validation failed."
endif

;; Look for the Validation Event Message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table validation event '", $SC_$CPU_find_event[2].eventid,"' found!"
else
  write "<!> Failed - Application Definition Table validation event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

write ";*********************************************************************"
write ";  Step 5.13: Send the command to Activate the file loaded in Step 5.11"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=appDefTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate Application Definition Table command sent properly."
else
  write "<!> Failed - Activate Application Definition Table command."
endif

;; Look for the Load pending Event Message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Look for the Update Event Message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Application Definition Table Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Application Definition Table update failed."
  Write "<!> Failed - Event Message not received for activate command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.14: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl5_14",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 6.0: Processing Limit Test."
write ";*********************************************************************"
write ";  Step 6.1: Send the Disable Non-Volatile (Eeprom) Checksumming      "
write ";  command if it is Enabled. "
write ";*********************************************************************"
;; Disable Eeprom Checksumming if it is enabled
if (p@$SC_$CPU_CS_EepromState = "Enabled") then
  ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_EEPROM_INF_EID,"INFO", 1

  cmdCtr = $SC_$CPU_CS_CMDPC + 1
  ;; Send the Disable Eeprom Checksumming Command
  /$SC_$CPU_CS_DisableEeprom

  ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - CS DisableEeprom command sent properly."
  else
    write "<!> Failed - CS DisableEeprom command did not increment CMDPC."
  endif

  ;; Check for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Expected Event Msg ",CS_DISABLE_EEPROM_INF_EID," rcv'd."
  else
    write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_EEPROM_INF_EID,"."
  endif
else
  write "=> Eeprom Checksumming is already disabled."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2: Send the Disable OS Code Segment command if necessary.   "
write ";*********************************************************************"
;; Disable OS Checksumming if it is enabled
if (p@$SC_$CPU_CS_OSState = "Enabled") then
  ut_setupevents "$SC", "$CPU", {CSAppName}, CS_DISABLE_OS_INF_EID, "INFO", 1

  cmdCtr = $SC_$CPU_CS_CMDPC + 1
  ;; Send the Disable OS Checksumming Command
  /$SC_$CPU_CS_DisableOS

  ut_tlmwait $SC_$CPU_CS_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - CS DisableOS command sent properly."
  else
    write "<!> Failed - CS DisableOS command did not increment CMDPC."
  endif

  ;; Check for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Expected Event Msg ",CS_DISABLE_OS_INF_EID," rcv'd."
  else
    write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_OS_INF_EID,"."
  endif
else
  write "=> OS Code Segment Checksumming is already disabled."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3: Send the Disable cFE Code Segment command if necessary. "
write ";*********************************************************************"
;; Disable CFE Checksumming if it is enabled
if (p@$SC_$CPU_CS_CFECoreState = "Enabled") then
  ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_CFECORE_INF_EID,"INFO", 1

  cmdCtr = $SC_$CPU_CS_CMDPC + 1
  ;; Send the Disable CFE Core Checksumming Command
  /$SC_$CPU_CS_DisableCFECore

  ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - CS DisableCFECORE command sent properly."
  else
    write "<!> Failed - CS DisableCFECORE command did not increment CMDPC."
  endif

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID," rcv'd."
  else
    write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_CFECORE_INF_EID,"."
  endif
else
  write "=> CFE Code Segment Checksumming is already disabled."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.4: Send the Disable Table Checksumming command if necessary."
write ";*********************************************************************"
;; Disable Table Checksumming if it is enabled
if (p@$SC_$CPU_CS_TableState = "Enabled") then
  ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_TABLES_INF_EID,"INFO", 1

  cmdCtr = $SC_$CPU_CS_CMDPC + 1
  ;; Send the Disable CFE Core Checksumming Command
  /$SC_$CPU_CS_DisableTables

  ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - CS DisableTables command sent properly."
  else
    write "<!> Failed - CS DisableTables command did not increment CMDPC."
  endif

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Expected Event Msg ",CS_DISABLE_TABLES_INF_EID," rcv'd."
  else
    write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_TABLES_INF_EID,"."
  endif
else
  write "=> Table Checksumming is already disabled."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.5: Send the Disable User-Defined Memory Checksumming command."
write ";*********************************************************************"
;; Disable Memory Checksumming if it is enabled
if (p@$SC_$CPU_CS_MemoryState = "Enabled") then
  ut_setupevents "$SC","$CPU",{CSAppName},CS_DISABLE_MEMORY_INF_EID,"INFO", 1

  cmdCtr = $SC_$CPU_CS_CMDPC + 1
  ;; Send the Disable CFE Core Checksumming Command
  /$SC_$CPU_CS_DisableMemory

  ut_tlmwait  $SC_$CPU_CS_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - CS DisableMemory command sent properly."
  else
    write "<!> Failed - CS DisableMemory command did not increment CMDPC."
  endif

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Expected Event Msg ",CS_DISABLE_MEMORY_INF_EID," rcv'd."
  else
    write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CS_DISABLE_MEMORY_INF_EID,"."
  endif
else
  write "=> User-Defined Memory Checksumming is already disabled."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.6: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl6_6",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "P"
endif

write ";*********************************************************************"
write ";  Step 6.7: Constantly Dump the Application Code Segment Results     "
write ";  table to determine if the CS application is segmenting the CRC     "
write ";  calculation each cycle. "
write ";*********************************************************************"
;; In order to do this, I must constantly dump the Results table and monitor
;; the Baseline CRC values
keepDumpingResults=FALSE
local loopCtr = 1
local segmentedCRC=FALSE

while (keepDumpingResults = FALSE) do
  s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl6_7",hostCPU,resAppId)
  wait 3

  ;; Loop for each valid entry in the results table
  for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
    ;; If the entry is valid and the Offset or tempCRC are not zero -> Stop
    if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State <> "Empty") AND ;;
       ($SC_$CPU_CS_APP_RESULT_TABLE[i].ByteOffset <> 0) OR ;;
       ($SC_$CPU_CS_APP_RESULT_TABLE[i].TempCRC <> 0) AND ;;
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
  write "<*> Passed (7000) -  Segmenting has occurred for Applications."
  ut_setrequirements CS_7000, "P"
else
  write "<!> Failed (7000) - Application Checksumming is not segmenting."
  ut_setrequirements CS_7000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.0: Definition Table Initialization Test."
write ";*********************************************************************"
write ";  Step 7.1: Send the command to stop the CS Application. "
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP Application="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CSAppName
wait 5

write ";*********************************************************************"
write ";  Step 7.2: Download the default Application Code Segment Definition "
write ";  Table file in order to restore it during cleanup."
write ";********************************************************************"
;;NOTE: If CFDP is used, you do not need to parse the filename from the path.
;;      You can just used the config paramater
;; use ftp utilities to get the file
;; CS_DEF_APP_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table filename
local tableFileName = CS_DEF_APP_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")
local lastSlashLoc = 0
local pathSpec = tableFileName
write "==> Default Application Code Segment Table filename config paramter = '",tableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  lastSlashLoc = lastSlashLoc + slashLoc
  slashLoc = %locate(tableFileName,"/")
enddo
write "==> Table filename ONLY = '",tableFileName,"'"

pathSpec = %substring(pathSpec,1,lastSlashLoc)
write "==> last Slash found at ",lastSlashLoc
write "==> Default path spec = '",pathSpec,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",tableFileName,"cs_apptbl.tblORIG",hostCPU,"G")
wait 5

write ";*********************************************************************"
write ";  Step 7.3: Delete the Application Code Segment Definition table "
write ";  default load file from the $CPU. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","na",tableFileName,hostCPU,"R")

write ";*********************************************************************"
write ";  Step 7.4: Start the CS Application. "
write ";*********************************************************************"
s $sc_$cpu_cs_start_apps("7.4")

write ";*********************************************************************"
write ";  Step 7.5: Dump the Application Code Segment Results table.        "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl7_5",hostCPU,resAppId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

write ";*********************************************************************"
write ";  Step 8.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 8.1: Upload the default Application Code Segment Definition   "
write ";  table downloaded in step 7.2. "
write ";*********************************************************************"
s ftp_file ("CF:0/apps","cs_apptbl.tblORIG",tableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 8.2: Send the Power-On Reset command. "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

reqReport:
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
write ";  End procedure $SC_$CPU_cs_appcode                                  "
write ";*********************************************************************"
ENDPROC
