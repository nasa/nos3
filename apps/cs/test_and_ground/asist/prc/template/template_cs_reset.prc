PROC $sc_$cpu_cs_reset
;*******************************************************************************
;  Test Name:  cs_reset
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) application initializes
;	the appropriate data items when any initialization occurs (Application
;	Reset, Power-On Reset, or Processor Reset).
;
;	This test will only be executed if the <PLATFORM_DEFINED>
;	PRESERVE_STATES_ON_PROCESSOR_RESET Flag is set to True. 
;
;  Requirements Tested
;    CS2010    CS shall provide the ability to dump the baseline CRCs and
;		status for the non-volatile memory segments via a dump-only
;		table.
;    CS4008	CS shall provide the ability to dump the baseline CRCs and
;		status for the Application code segment memory segments via a
;		dump-only table.
;    CS5008	CS shall provide the ability to dump the baseline CRCs and
;		status for the tables via a dump-only table.
;    CS6008	CS shall provide the ability to dump the baseline CRCs and
;		status for all User-Defined Memory via a dump-only table.
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
;    CS9001	Upon initialization of the CS Application (cFE Power On, cFE
;		Processor Reset or CS Application Reset), CS shall initialize
;		the following data to Zero:
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
;    CS9002	Upon a cFE Power On Reset, if the segment's <PLATFORM_DEFINED>
;		Power-On initialization state is set to Enabled, CS shall
;		compute the baseline CRCs for the following regions:
;			a) OS code segment
;			b) cFE code segment
;    CS9003	Upon a cFE Power On Reset, if the Non-Volatile
;		<PLATFORM_DEFINED> Power-On initialization state is set to
;		Enabled, CS shall compute the baseline CRCs for Non-volatile
;		segments based on the corresponding table definition for up to
;		<PLATFORM_DEFINED> segments.
;    CS9003.1	If the address range for any of the Non-volatile segments is
;		invalid, CS shall send an event message and disable Non-volatile
;		Checksumming.
;    CS9003.2	CS shall send an event message and disable Non-volatile
;		Checksumming if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9004	Upon a cFE Power On Reset, if the Non-Volatile
;		<PLATFORM_DEFINED> Power-On initialization state is set to
;		Enabled, CS shall compute the baseline CRC for the total of all
;		of non-volatile segments.
;    CS9005	Upon a cFE Power On Reset, if the Application <PLATFORM_DEFINED>
;		Power-On initialization state is set to Enabled, CS shall
;		compute baseline CRCs for the Application code segments region
;		based on the corresponding table definition for up to
;		<PLATFORM_DEFINED> Applications.
;    CS9005.1	CS shall send an event message and disable Application code
;		segment Checksumming if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9006	Upon a cFE Power On Reset, if the Tables <PLATFORM_DEFINED>
;		Power-On initialization state is set to Enabled, CS shall
;		compute baseline CRCs for the tables specified in the
;		corresponding table definition for up to <PLATFORM_DEFINED>
;		tables.
;    CS9006.1	CS shall send an event message and disable table Checksumming
;		if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9007	Upon a cFE Power On Reset, if the User-Defined Memory
;		<PLATFORM_DEFINED> Power-on initialization state is set to
;		Enabled, CS shall compute baseline CRCs for the User-defined
;		memory region based on the corresponding table definition for up
;		to <PLATFORM_DEFINED> memory segments.
;    CS9007.1	If the address range for any of the User-defined Memory is
;		invalid, CS shall send an event message and disable User-defined
;		Memory Checksumming.
;    CS9007.2	CS shall send an event message and disable Checksumming of the
;		User-defined Memory if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9008	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True, CS shall preserve the following:
;			a) OS Code Segment Checksumming State
;			b) cFE Code Segment Checksumming State
;			c) Non-volatile Checksumming State
;			d) Application Code Segment Checksumming State
;			e) Table Checksumming State
;			f) User-Defined Memory Checksumming State
;    CS9010	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True and the segment's state is set to Enabled, CS shall
;		compute baseline CRCs for the following regions:
;			a) OS code segment
;			b) cFE code segment
;    CS9011	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True and the Non-volatile Checksumming State is Enabled,
;		CS shall compute baseline CRCs for Non-volatile segments based
;		on the corresponding table definition for up to
;		<PLATFORM_DEFINED> segments.
;    CS9011.1	If the address range for any of the Non-volatile segments is
;		invalid, CS shall send an event message and disable Non-volatile
;		Checksumming.
;    CS9011.2	CS shall send an event message and disable Non-volatile
;		Checksumming, if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9012	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True and the Non-volatile Checksumming State is Enabled,
;		CS shall compute the baseline CRC for the total of all
;		Non-volatile segments.
;    CS9013	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True and the Application Code Segment Checksumming State
;		is Enabled, CS shall compute baseline CRCs for the Appication
;		code segments region on the corresponding table definition for
;		up to <PLATFORM_DEFIONED> Applications.
;    CS9013.1	CS shall send an event message and disable Application code
;		segment Checksumming, if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9014	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True and the Table Checksumming State is Enabled, CS
;		shall compute baseline CRCs for the tables specified in the
;		corresponding table definition for up to <PLATFORM_DEFIONED>
;		tables.
;    CS9014.1	CS shall send an event message and disable Table Checksumming,
;		if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
;    CS9015	Upon a cFE Processor Reset or CS Application Reset, if the
;		<PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;		set to True and the User-Defined Memory Checksumming State is
;		Enabled, CS shall compute baseline CRCs for User-Defined memory
;		region based on the corresponding table definition for up to
;		<PLATFORM_DEFINED> memory segments.
;    CS9015.1	If the address range for any of the User-Defined Memory is
;		invalid, CS shall send an event message and disable User-Defined
;		Memory Checksumming.
;    CS9015.2	CS shall send an event message and disable Checksumming of the
;		User-Defined Memory, if the state is not one of the following:
;			a) enabled
;			b) disabled
;			c) empty
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
;	10/20/08	Walt Moleski	Original Procedure.
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
#include "osconfig.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_tbldefs.h"
#include "tst_cs_events.h"
#include "tst_cs_msgdefs.h"
#include "tst_tbl_events.h"

%liv (log_procedure) = logging

#define CS_2010		0
#define CS_4008		1
#define CS_5008		2
#define CS_6008		3
#define CS_9000		4
#define CS_9001		5
#define CS_9002		6
#define CS_9003		7
#define CS_90031	8
#define CS_90032	9
#define CS_9004		10
#define CS_9005		11
#define CS_90051	12
#define CS_9006		13
#define CS_90061	14
#define CS_9007		15
#define CS_90071	16
#define CS_90072	17
#define CS_9008		18
#define CS_9010		19
#define CS_9011		20
#define CS_90111	21
#define CS_90112	22
#define CS_9012		23
#define CS_9013		24
#define CS_90131	25
#define CS_9014		26
#define CS_90141	27
#define CS_9015		28
#define CS_90151	29
#define CS_90152	30


global ut_req_array_size = 30
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_2010","CS_4008","CS_5008","CS_6008","CS_9000","CS_9001","CS_9002","CS_9003","CS_9003.1","CS_9003.2","CS_9004","CS_9005","CS_9005.1","CS_9006","CS_9006.1","CS_9007","CS_9007.1","CS_9007.2","CS_9008","CS_9010","CS_9011","CS_9011.1","CS_9011.2","CS_9012","CS_9013","CS_9013.1","CS_9014","CS_9014.1","CS_9015","CS_9015.1","CS_9015.2" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL appDefTblId, appDefPktId, appResTblId, appResPktId
LOCAL tblDefTblId, tblDefPktId, tblResTblId, tblResPktId
LOCAL eeDefTblId, eeDefPktId, eeResTblId, eeResPktId
LOCAL usrDefTblId, usrDefPktId, usrResTblId, usrResPktId
local i, cmdCtr, stream
local osCRC, cFECRC, eepromCRC
local appCRCs[0 .. CS_MAX_NUM_APP_TABLE_ENTRIES-1]
local tblCRCs[0 .. CS_MAX_NUM_TABLES_TABLE_ENTRIES-1]
local eeCRCs[0 .. CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1]
local usrCRCs[0 .. CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1]
local CSAppName = "CS" 
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME
local appResTblName = CSAppName & "." & CS_RESULTS_APP_TABLE_NAME
local eeDefTblName = CSAppName & "." & CS_DEF_EEPROM_TABLE_NAME
local eeResTblName = CSAppName & "." & CS_RESULTS_EEPROM_TABLE_NAME
local tblDefTblName = CSAppName & "." & CS_DEF_TABLES_TABLE_NAME
local tblResTblName = CSAppName & "." & CS_RESULTS_TABLES_TABLE_NAME
local memDefTblName = CSAppName & "." & CS_DEF_MEMORY_TABLE_NAME
local memResTblName = CSAppName & "." & CS_RESULTS_MEMORY_TABLE_NAME

;; Set the pkt and app IDs for the tables based upon the cpu being used
;; There are 4 sets of tables for Applications, Tables, EEPROM and User-defined
;; memory
;; CPU1 is the default
appDefTblId = "0FAF"
appResTblId = "0FB3"
appDefPktId = 4015
appResPktId = 4019
tblDefTblId = "0FAE"
tblResTblId = "0FB2"
tblDefPktId = 4014
tblResPktId = 4018
eeDefTblId = "0FAC"
eeResTblId = "0FB0"
eeDefPktId = 4012
eeResPktId = 4016
usrDefTblId = "0FAD"
usrResTblId = "0FB1"
usrDefPktId = 4013
usrResPktId = 4017

write ";*********************************************************************"
write ";  Step 1.0: Checksum Reset Test Setup."
write ";*********************************************************************"
;; Check to see if the PRESERVE State is true. If not, end the test.
if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET = 0) then
  write "** Preserve State is False **"
  write "** State must be TRUE to execute this test. Ending test. **"
  goto procterm
endif

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
write ";  Step 1.2: Download the default Application Code Segment Definition "
write ";  Table file in order to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; Parse the filename configuration parameter for the default table
local appTableFileName = CS_DEF_APP_TABLE_FILENAME
local slashLoc = %locate(appTableFileName,"/")
write "==> Default Application Code Segment Table filename config param = '",appTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  appTableFileName = %substring(appTableFileName,slashLoc+1,%length(appTableFileName))
  slashLoc = %locate(appTableFileName,"/")
enddo
write "==> Table filename ONLY = '",appTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",appTableFileName,"cs_app_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 1.3: Download the default Table Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_TABLES_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local tblTableFileName = CS_DEF_TABLES_TABLE_FILENAME
slashLoc = %locate(tblTableFileName,"/")
write "==> Default Table Definition Table filename config param = '",tblTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  tblTableFileName = %substring(tblTableFileName,slashLoc+1,%length(tblTableFileName))
  slashLoc = %locate(tblTableFileName,"/")
enddo
write "==> Table filename ONLY = '",tblTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",tblTableFileName,"cs_tbl_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 1.4: Download the default EEPROM Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_EEPROM_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local eeTableFileName = CS_DEF_EEPROM_TABLE_FILENAME
slashLoc = %locate(eeTableFileName,"/")
write "==> Default EEPROM Definition Table filename config param = '",eeTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  eeTableFileName = %substring(eeTableFileName,slashLoc+1,%length(eeTableFileName))
  slashLoc = %locate(eeTableFileName,"/")
enddo
write "==> Table filename ONLY = '",eeTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",eeTableFileName,"cs_eeprom_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 1.5: Download the default Memory Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_MEMORY_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local memTableFileName = CS_DEF_MEMORY_TABLE_FILENAME
slashLoc = %locate(memTableFileName,"/")
write "==> Default Memory Definition Table filename config param = '",memTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  memTableFileName = %substring(memTableFileName,slashLoc+1,%length(memTableFileName))
  slashLoc = %locate(memTableFileName,"/")
enddo
write "==> Table filename ONLY = '",memTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",memTableFileName,"cs_mem_orig_tbl.tbl",hostCPU,"G")

write ";**********************************************************************"
write ";  Step 1.6: Display the Housekeeping and Table Telemetry pages.       "
write ";**********************************************************************"
;; The Definition table pages are not really needed and are commented out
;; below. If you wish to view these pages, just uncomment them.
page $SC_$CPU_CS_HK
page $SC_$CPU_TST_CS_HK
;;page $SC_$CPU_CS_APP_DEF_TABLE
page $SC_$CPU_CS_APP_RESULTS_TBL
;;page $SC_$CPU_CS_TBL_DEF_TABLE
page $SC_$CPU_CS_TBL_RESULTS_TBL
;;page $SC_$CPU_CS_EEPROM_DEF_TABLE
page $SC_$CPU_CS_EEPROM_RESULTS_TBL
;;page $SC_$CPU_CS_MEM_DEF_TABLE
page $SC_$CPU_CS_MEM_RESULTS_TBL

write ";*********************************************************************"
write ";  Step 1.7: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 1.8: Create & upload the EEPROM Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
;;s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.9: Create & upload the Application Code Segment Definition "
write ";  Table file to be used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_adt1
wait 5

;; Upload the file created above as the default
;; Application Definition Table load file
;;s ftp_file ("CF:0/apps","app_def_tbl_ld_1","cs_apptbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","app_def_tbl_ld_1",appTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.10: Create & upload the Tables Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_tdt5
wait 5

;; Tables Definition Table load file
;;s ftp_file ("CF:0/apps","tbl_def_tbl_ld_3","cs_tablestbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","tbl_def_tbl_ld_3",tblTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.11: Create & upload the Memory Definition Table file to be  "
write ";  used during this test."
write ";********************************************************************"
s $sc_$cpu_cs_mdt5
wait 5

;; Upload the file created above as the default 
;;s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.12: Start the applications in order for the load files created"
write ";  above to successfully pass validation and load. "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("1.12")
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

write ";*********************************************************************"
write ";  Step 1.13: Start the other applications required for this test. "
write ";********************************************************************"
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
write ";  Step 1.14: Verify that the CS Housekeeping telemetry items are "
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
write ";  Step 1.15: Check the Checksum States to verify they are correct. "
write ";*********************************************************************"
;; Check the POWERON States are set
;; OS State
if ((p@$SC_$CPU_CS_OSState = "Enabled") AND (CS_OSCS_CHECKSUM_STATE = 1)) then
  write "<*> Passed - OS State as expected after Power-On reset."
elseif ((p@$SC_$CPU_CS_OSState = "Disabled") AND ;;
	(CS_OSCS_CHECKSUM_STATE = 2)) then
  write "<*> Passed - OS State as expected after Power-On reset."
else
  write "<!> Failed - OS State not set as expected after Power-On reset."
endif

;; cFE Core State
if ((p@$SC_$CPU_CS_CFECoreState = "Enabled") AND ;;
    (CS_CFECORE_CHECKSUM_STATE = 1)) then
  write "<*> Passed - cFE Core State as expected after Power-On reset."
elseif ((p@$SC_$CPU_CS_CFECoreState = "Disabled") AND ;;
    (CS_CFECORE_CHECKSUM_STATE = 2)) then
  write "<*> Passed - cFE Core State as expected after Power-On reset."
else
  write "<!> Failed - cFE Core State not set as expected after Power-On reset."
endif

;; EEPROM State
if ((p@$SC_$CPU_CS_EepromState = "Enabled") AND ;;
    (CS_EEPROM_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - EEPROM State as expected after Power-On reset."
elseif ((p@$SC_$CPU_CS_EepromState = "Disabled") AND ;;
    (CS_EEPROM_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - EEPROM State as expected after Power-On reset."
else
  write "<!> Failed - EEPROM State not set as expected after Power-On reset."
endif

;; User-Defined Memory State
if ((p@$SC_$CPU_CS_MemoryState = "Enabled") AND ;;
    (CS_MEMORY_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - User-Defined Memory State as expected after Power-On reset."
elseif ((p@$SC_$CPU_CS_MemoryState = "Disabled") AND ;;
    (CS_MEMORY_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - User-Defined Memory State as expected after Power-On reset."
else
  write "<!> Failed - User-Defined Memory State not set as expected after Power-On reset."
endif

;; Applications State
if ((p@$SC_$CPU_CS_AppState = "Enabled") AND ;;
    (CS_APPS_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - Application State as expected after Power-On reset."
elseif ((p@$SC_$CPU_CS_AppState = "Disabled") AND ;;
    (CS_APPS_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - Application State as expected after Power-On reset."
else
  write "<!> Failed - Application State not set as expected after Power-On reset."
endif

;; Tables State
if ((p@$SC_$CPU_CS_TableState = "Enabled") AND ;;
    (CS_TABLES_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - Tables State as expected after Power-On reset."
elseif ((p@$SC_$CPU_CS_TableState = "Disabled") AND ;;
    (CS_TABLES_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - Tables State as expected after Power-On reset."
else
  write "<!> Failed - Tables State not set as expected after Power-On reset."
endif

write ";*********************************************************************"
write ";  Step 1.16: Wait until the Pass Counter indicates that it has made a"
write ";  complete pass through the checksum tables. If this takes longer than"
write ";  300 seconds, then time-out. "
write ";*********************************************************************"
if ($SC_$CPU_CS_PASSCTR = 0) then
  ut_tlmwait $SC_$CPU_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif

write ";*********************************************************************"
write ";  Step 2.0: Power-On Reset Test."
write ";*********************************************************************"
write ";  Step 2.1: Modify the OS and cFE code segment baseline CRCs."
write ";*********************************************************************"
;; OS State
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  write ";** Skipping because OS State is Disabled."
  goto check_cFE_State
endif

;; Use the TST_CS app to corrupt the OS CRC
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_OS_CRC_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_OS_MISCOMPARE_ERR_EID, "ERROR", 2

/$SC_$CPU_TST_CS_CorruptOSCRC
wait 5

;; Check for OS the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

;; Check for the OS event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - OS Miscompare Event ID=",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - OS Miscompare Event was not received. Time-out occurred."
endif

check_cFE_State:
;; cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  write ";** Skipping because cFE Core State is Disabled."
  goto step_2_2
endif

;; Use the TST_CS app to corrupt the cFE CRC
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 4

/$SC_$CPU_TST_CS_CorruptCFECRC
wait 5

;; Check for the cFE event message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

;; Check for the CFE event message
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - cFE Core Miscompare Event ID=",CS_CFECORE_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - cFE Core Miscompare Event was not received. Time-out occurred."
endif

wait 5

step_2_2:
write ";*********************************************************************"
write ";  Step 2.2: Dump the Non-Volatile Code Segment Results Table."
write ";*********************************************************************"
;; Non-Volatile Memory State
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  write ";** Skipping tests because Non-Volatile Memory State is Disabled."
  goto step_2_5
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_2",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.3: Corrupt the Non-volatile Baseline CRCs in order to       "
write ";  determine if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
;; Loop for each valid entry in the results table
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.4: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_4",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

step_2_5:
write ";*********************************************************************"
write ";  Step 2.5: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
;; Application State
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  write ";** Skipping tests because Application State is Disabled."
  goto step_2_8
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl2_5",hostCPU,appResTblId)
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
write ";  Step 2.6: Corrupt the Application CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_APP_CRC_INF_EID,"INFO",1

    /$SC_$CPU_TST_CS_CorruptAppCRC AppName=$SC_$CPU_CS_APP_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.7: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl2_7",hostCPU,appResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

step_2_8:
write ";*********************************************************************"
write ";  Step 2.8: Dump the Table Results Table."
write ";*********************************************************************"
;; Table State
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  write ";** Skipping tests because Table State is Disabled."
  goto step_2_11
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_8",hostCPU,tblResTblId)
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
write ";  Step 2.9: Corrupt the Table CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_TABLE_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptTblCRC TABLEName=$SC_$CPU_CS_TBL_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.10: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_10",hostCPU,tblResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

step_2_11:
write ";*********************************************************************"
write ";  Step 2.11: Dump the User-defined Memory Results Table."
write ";*********************************************************************"
;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping tests because User-Defined Memory State is Disabled."
  goto step_2_14
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_11",hostCPU,usrResTblId)
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
write ";  Step 2.12: Corrupt the User-defined Memory CRCs in order to determine"
write ";  if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.13: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_13",hostCPU,usrResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

step_2_14:
write ";*********************************************************************"
write ";  Step 2.14: Save the CRCs so that they can be checked after the reset."
write ";*********************************************************************"
osCRC = $SC_$CPU_CS_OSBASELINE
cFECRC = $SC_$CPU_CS_CFECOREBASELINE

;; Loop and store the Non-Volatile Memory CRCs
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    eeCRCs[i] = $SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    appCRCs[i] = $SC_$CPU_CS_APP_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    tblCRCs[i] = $SC_$CPU_CS_TBL_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    usrCRCs[i] = $SC_$CPU_CS_MEM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.15: Send the Power-On reset command."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 2.16: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 2.17: Create & upload the EEPROM Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Power-On Reset. "
write ";********************************************************************"
s $sc_$cpu_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
;;s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 2.18: Create & upload the Memory Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Power-On Reset. "
write ";********************************************************************"
s $sc_$cpu_cs_mdt5
wait 5

;; Upload the file created above as the default 
;;s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 2.19:  Start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("2.19")

write ";*********************************************************************"
write ";  Step 2.20:  Start the other applications required for this test. "
write ";********************************************************************"
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

;; Set the eepromCRC variable to the initial value after app startup
eepromCRC = $SC_$CPU_CS_EEPROMBASELINE

write ";*********************************************************************"
write ";  Step 2.21: Wait until the CRCs have been recalculated.    "
write ";*********************************************************************"
if ($SC_$CPU_CS_PASSCTR = 0) then
  ut_tlmwait $SC_$CPU_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif

write ";*********************************************************************"
write ";  Step 2.22: Verify that the CRCs contained in the Housekeeping packet"
write ";  have been recalculated. "
write ";*********************************************************************"
;; OS State
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  write ";** Skipping OS CRC recalculation check because OS State is Disabled."
  goto check_cFE_CRC
endif

;; Check the OS CRC
if (osCRC <> $SC_$CPU_CS_OSBASELINE) then
  write "<*> Passed (9002) - OS CRC has been recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "P"
else
  write "<!> Failed (9002) - OS CRC was not recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "F"
endif

check_cFE_CRC:
;; cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  write ";** Skipping CRC recalculation check because cFE Core State is Disabled."
  goto check_EEPROM_CRC
endif

;; Check the cFE CRC
if (cFECRC <> $SC_$CPU_CS_CFECOREBASELINE) then
  write "<*> Passed (9002) - cFE CRC has been recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "P"
else
  write "<!> Failed (9002) - cFE CRC was not recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "F"
endif

check_EEPROM_CRC:
;; Non-Volatile Memory State
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  write ";** Skipping CRC recalculation check because Non-Volatile Memory State is Disabled."
  goto step_2_24
endif

;; Check the overall EEPROM CRC
if (eepromCRC <> $SC_$CPU_CS_EEPROMBASELINE) then
  write "<*> Passed (9004) - Overall EEPROM CRC has been recalculated on a Power-On Reset."
  ut_setrequirements CS_9004, "P"
else
  write "<!> Failed (9004) - Overall EEPROM CRC was not recalculated on a Power-On Reset."
  ut_setrequirements CS_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.23: Dump the Non-Volatile Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl2_23",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

;; Check the enabled EEPROM Table entries
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    if (eeCRCs[i] <> $SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9003) - EEPROM entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9003, "P"
    else
      write "<!> Failed (9003) - EEPROM entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9003, "F"
    endif
  endif
enddo

step_2_24:
write ";*********************************************************************"
write ";  Step 2.24: Dump the Application Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
;; Application State
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  write ";** Skipping CRC recalculation check because Application State is Disabled."
  goto step_2_25
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl2_24",hostCPU,appResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    if (appCRCs[i] <> $SC_$CPU_CS_APP_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9005) - App entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9005, "P"
    else
      write "<!> Failed (9005) - App entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9005, "F"
    endif
  endif
enddo

step_2_25:
write ";*********************************************************************"
write ";  Step 2.25: Dump the Table Results Table and verify the CRCs have "
write ";  been recalculated. "
write ";*********************************************************************"
;; Table State
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  write ";** Skipping CRC recalculation check because Table State is Disabled."
  goto step_2_26
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl2_25",hostCPU,tblResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif
;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (tblCRCs[i] <> $SC_$CPU_CS_TBL_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9006) - Table entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9006, "P"
    else
      write "<!> Failed (9006) - Table entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9006, "F"
    endif
  endif
enddo

step_2_26:
write ";*********************************************************************"
write ";  Step 2.26: Dump the User-defined Memory Results Table and verify the"
write ";  CRCs have been recalculated. "
write ";*********************************************************************"
;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_3_0
endif

;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_3_0
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl2_26",hostCPU,usrResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif
;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    if (usrCRCs[i] <> $SC_$CPU_CS_MEM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9007) - User-defined Memory entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9007, "P"
    else
      write "<!> Failed (9007) - User-defined Memory entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9007, "F"
    endif
  endif
enddo

step_3_0:
write ";*********************************************************************"
write ";  Step 3.0: Processor Reset Test."
write ";*********************************************************************"
write ";  Step 3.1: Modify the OS and cFE code segment baseline CRCs."
write ";*********************************************************************"
;; OS State
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  write ";** Skipping because OS State is Disabled."
  goto cFE_ProcReset_Check
endif

;; Use the TST_CS app to corrupt the OS and cFE CRCs
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_OS_CRC_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_OS_MISCOMPARE_ERR_EID,"ERROR", 2

/$SC_$CPU_TST_CS_CorruptOSCRC
wait 5

;; Check for OS the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

;; Check for the Miscompare event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - OS Miscompare Event ID=",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - OS Miscompare Event was not received. Time-out occurred."
endif

cFE_ProcReset_Check:
;; cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  write ";** Skipping because cFE Core State is Disabled."
  goto step_3_2
endif

;; Use the TST_CS app to corrupt the cFE CRC
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 4

/$SC_$CPU_TST_CS_CorruptCFECRC
wait 5

;; Check for the cFE event message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - cFE Core Miscompare Event ID=",CS_CFECORE_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - cFE Core Miscompare Event was not received. Time-out occurred."
endif

wait 5

step_3_2:
write ";*********************************************************************"
write ";  Step 3.2: Dump the Non-Volatile Code Segment Results Table."
write ";*********************************************************************"
;; Non-Volatile Memory State
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  write ";** Skipping tests because Non-Volatile Memory State is Disabled."
  goto step_3_5
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl3_2",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3: Corrupt the Non-volatile Baseline CRCs in order to       "
write ";  determine if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
;; Loop for each valid entry in the results table
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.4: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl3_4",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

step_3_5:
write ";*********************************************************************"
write ";  Step 3.5: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
;; Application State
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  write ";** Skipping tests because Application State is Disabled."
  goto step_3_8
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl3_5",hostCPU,appResTblId)
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
write ";  Step 3.6: Corrupt the Application CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_APP_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptAppCRC AppName=$SC_$CPU_CS_APP_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.7: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl3_7",hostCPU,appResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

step_3_8:
write ";*********************************************************************"
write ";  Step 3.8: Dump the Table Results Table."
write ";*********************************************************************"
;; Table State
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  write ";** Skipping tests because Table State is Disabled."
  goto step_3_11
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl3_8",hostCPU,tblResTblId)
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
write ";  Step 3.9: Corrupt the Table CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_TABLE_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptTblCRC TABLEName=$SC_$CPU_CS_TBL_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.10: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl3_10",hostCPU,tblResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

step_3_11:
write ";*********************************************************************"
write ";  Step 3.11: Dump the User-defined Memory Results Table."
write ";*********************************************************************"
;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping tests because User-Defined Memory State is Disabled."
  goto step_3_14
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl3_11",hostCPU,usrResTblId)
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
write ";  Step 3.12: Corrupt the User-defined Memory CRCs in order to determine"
write ";  if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.13: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl3_13",hostCPU,usrResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

step_3_14:
write ";*********************************************************************"
write ";  Step 3.14: Save the CRCs so that they can be checked after the reset."
write ";*********************************************************************"
osCRC = $SC_$CPU_CS_OSBASELINE
cFECRC = $SC_$CPU_CS_CFECOREBASELINE

;; Loop and store the Non-Volatile Memory CRCs
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    eeCRCs[i] = $SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    appCRCs[i] = $SC_$CPU_CS_APP_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    tblCRCs[i] = $SC_$CPU_CS_TBL_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    usrCRCs[i] = $SC_$CPU_CS_MEM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.15: Send the Processor reset command."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 3.16: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 3.17: Create & upload the EEPROM Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Processor Reset. "
write ";********************************************************************"
s $sc_$cpu_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
;;s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 3.18: Create & upload the Memory Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Processor Reset. "
write ";********************************************************************"
s $sc_$cpu_cs_mdt5
wait 5

;; Upload the file created above as the default 
;;s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 3.19:  Start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("3.19")

write ";*********************************************************************"
write ";  Step 3.20: Start the other applications required for this test. "
write ";********************************************************************"
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

;; Set the eepromCRC variable to the initial value after app startup
eepromCRC = $SC_$CPU_CS_EEPROMBASELINE

write ";*********************************************************************"
write ";  Step 3.21: Wait until the CRCs have been recalculated.    "
write ";*********************************************************************"
if ($SC_$CPU_CS_PASSCTR = 0) then
  ut_tlmwait $SC_$CPU_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif

write ";*********************************************************************"
write ";  Step 3.22: Verify that the CRCs contained in the Housekeeping packet"
write ";  have been recalculated. "
write ";*********************************************************************"
;; OS State
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  write ";** Skipping OS CRC recalculation check because OS State is Disabled."
  goto check_cFE_CRC2
endif

;; Check the OS CRC
if (osCRC <> $SC_$CPU_CS_OSBASELINE) then
  write "<*> Passed (9010) - OS CRC has been recalculated on a Processor Reset."
  ut_setrequirements CS_9010, "P"
else
  write "<!> Failed (9010) - OS CRC was not recalculated on a Processor Reset."
  ut_setrequirements CS_9010, "F"
endif

check_cFE_CRC2:
;; Check the cFE CRC
;; cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  write ";** Skipping CRC recalculation check because cFE Core State is Disabled."
  goto check_EEPROM_CRC2
endif

if (cFECRC <> $SC_$CPU_CS_CFECOREBASELINE) then
  write "<*> Passed (9010) - cFE CRC has been recalculated on a Processor Reset."
  ut_setrequirements CS_9010, "P"
else
  write "<!> Failed (9010) - cFE CRC was not recalculated on a Processor Reset."
  ut_setrequirements CS_9010, "F"
endif

check_EEPROM_CRC2:
;; Non-Volatile Memory State
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  write ";** Skipping CRC recalculation check because Non-Volatile Memory State is Disabled."
  goto step_3_24
endif

;; Check the overall EEPROM CRC
if (eepromCRC <> $SC_$CPU_CS_EEPROMBASELINE) then
  write "<*> Passed (9012) - Overall EEPROM CRC has been recalculated on a Processor Reset."
  ut_setrequirements CS_9012, "P"
else
  write "<!> Failed (9012) - Overall EEPROM CRC was not recalculated on a Processor Reset."
  ut_setrequirements CS_9012, "F"
endif

write ";*********************************************************************"
write ";  Step 3.23: Dump the Non-Volatile Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl3_23",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

;; Check the enabled EEPROM Table entries
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    if (eeCRCs[i] <> $SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9011) - EEPROM entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9011, "P"
    else
      write "<!> Failed (9011) - EEPROM entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9011, "F"
    endif
  endif
enddo

step_3_24:
write ";*********************************************************************"
write ";  Step 3.24: Dump the Application Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
;; Application State
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  write ";** Skipping CRC recalculation check because Application State is Disabled."
  goto step_3_25
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl3_24",hostCPU,appResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    if (appCRCs[i] <> $SC_$CPU_CS_APP_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9013) - App entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9013, "P"
    else
      write "<!> Failed (9013) - App entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9013, "F"
    endif
  endif
enddo

step_3_25:
write ";*********************************************************************"
write ";  Step 3.25: Dump the Table Results Table and verify the CRCs have "
write ";  been recalculated. "
write ";*********************************************************************"
;; Table State
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  write ";** Skipping CRC recalculation check because Table State is Disabled."
  goto step_3_26
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl3_25",hostCPU,tblResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif
;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (tblCRCs[i] <> $SC_$CPU_CS_TBL_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9014) - Table entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9014, "P"
    else
      write "<!> Failed (9014) - Table entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9014, "F"
    endif
  endif
enddo

step_3_26:
write ";*********************************************************************"
write ";  Step 3.26: Dump the User-defined Memory Results Table and verify the"
write ";  CRCs have been recalculated. "
write ";*********************************************************************"
;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_3_27
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl3_26",hostCPU,usrResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif
;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    if (usrCRCs[i] <> $SC_$CPU_CS_MEM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9015) - User-defined Memory entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9015, "P"
    else
      write "<!> Failed (9015) - User-defined Memory entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9015, "F"
    endif
  endif
enddo

step_3_27:
write ";*********************************************************************"
write ";  Step 3.27: Change the region states and verify that the states are"
write ";  preserved after a Processor Reset. "
write ";*********************************************************************"
write ";  Step 3.27.1: Toggle the region states."
write ";*********************************************************************"
;; Variables for the expected states
local expOSState
local expCFEState
local expEepromState
local expMemoryState
local expAppState
local expTableState

;; Change the OS State
if (p@$SC_$CPU_CS_OSState = "Enabled") then
  /$SC_$CPU_CS_DisableOS
  wait 1
  expOSState = "Disabled"
else
  /$SC_$CPU_CS_EnableOS
  wait 1
  expOSState = "Enabled"
endif

;; Change the cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Enabled") then
  /$SC_$CPU_CS_DisableCFECore
  wait 1
  expCFEState = "Disabled"
else
  /$SC_$CPU_CS_EnableCFECore
  wait 1
  expCFEState = "Enabled"
endif

;; Change the EEPROM State
if (p@$SC_$CPU_CS_EepromState = "Enabled") then
  /$SC_$CPU_CS_DisableEeprom
  wait 1
  expEepromState = "Disabled"
else
  /$SC_$CPU_CS_EnableEeprom
  wait 1
  expEepromState = "Enabled"
endif

;; Change the User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Enabled") then
  /$SC_$CPU_CS_DisableMemory
  wait 1
  expMemoryState = "Disabled"
else
  /$SC_$CPU_CS_EnableMemory
  wait 1
  expMemoryState = "Enabled"
endif

;; Change the Applications State
if (p@$SC_$CPU_CS_AppState = "Enabled") then
  /$SC_$CPU_CS_DisableApps
  wait 1
  expAppState = "Disabled"
else
  /$SC_$CPU_CS_EnableApps
  wait 1
  expAppState = "Enabled"
endif

;; Change the Tables State
if (p@$SC_$CPU_CS_TableState = "Enabled") then
  /$SC_$CPU_CS_DisableTables
  wait 1
  expTableState = "Disabled"
else
  /$SC_$CPU_CS_EnableTables
  wait 1
  expTableState = "Enabled"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.27.2: Perform a Processor Reset."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 3.27.3: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 3.27.4: Reload Memory Tables. "
write ";********************************************************************"
s $sc_$cpu_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 5

s $sc_$cpu_cs_mdt5
wait 5

;; Upload the file created above as the default
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 3.27.5: Start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("3.27.5")

write ";*********************************************************************"
write ";  Step 3.27.6: Check the states and verify they are as expected.  "
write ";********************************************************************"
;; Check the OS State
if (p@$SC_$CPU_CS_OSState = expOSState) then
  write "<*> Passed (9008) - OS State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - OS State not set as expected after reset. Expected '",expOSState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = expCFEState) then
  write "<*> Passed (9008) - cFE State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - cFE State not set as expected after reset. Expected '",expCFEState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the EEPROM State
if (p@$SC_$CPU_CS_EepromState = expEepromState) then
  write "<*> Passed (9008) - Eeprom State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - EEPROM State not set as expected after reset. Expected '",expEepromState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = expMemoryState) then
  write "<*> Passed (9008) - Memory State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - Memory State not set as expected after reset. Expected '",expMemoryState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the Applications State
if (p@$SC_$CPU_CS_AppState = expAppState) then
  write "<*> Passed (9008) - Application State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - Application State not set as expected after reset. Expected '",expAppState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the Tables State
if (p@$SC_$CPU_CS_TableState = expTableState) then
  write "<*> Passed (9008) - Tables State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - Tables State not set as expected after reset. Expected '",expTableState,"'."
  ut_setrequirements CS_9008, "F"
endif

write ";*********************************************************************"
write ";  Step 3.27.7: Restore the initial States prior to the Application "
write ";  Reset Test. "
write ";********************************************************************"
;; Enable the OS State if it is not enabled
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  /$SC_$CPU_CS_EnableOS
  wait 1
else
  /$SC_$CPU_CS_DisableOS
  wait 1
endif

;; Enable the cFE State if it is not enabled
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  /$SC_$CPU_CS_EnableCFECore
  wait 1
else
  /$SC_$CPU_CS_DisableCFECore
  wait 1
endif

;; Enable the Eeprom State if it is not enabled
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  /$SC_$CPU_CS_EnableEeprom
  wait 1
else
  /$SC_$CPU_CS_DisableEeprom
  wait 1
endif

;; Enable the Memory State if it is not enabled
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  /$SC_$CPU_CS_EnableMemory
  wait 1
else
  /$SC_$CPU_CS_DisableMemory
  wait 1
endif

;; Enable the Application State if it is not enabled
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  /$SC_$CPU_CS_EnableApps
  wait 1
else
  /$SC_$CPU_CS_DisableApps
  wait 1
endif

;; Enable the Tables State if it is not enabled
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  /$SC_$CPU_CS_EnableTables
  wait 1
else
  /$SC_$CPU_CS_DisableTables
  wait 1
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.27.8: Start the other applications required for this test. "
write ";********************************************************************"
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
write ";  Step 3.28: Wait until all the Checksums are recalculated. "
write ";********************************************************************"
local nextPass = $SC_$CPU_CS_PASSCTR + 2
ut_tlmwait $SC_$CPU_CS_PASSCTR, {nextPass}, 700

write ";*********************************************************************"
write ";  Step 4.0: Application Reset Test."
write ";*********************************************************************"
write ";  Step 4.1: Modify the OS and cFE code segment baseline CRCs."
write ";*********************************************************************"
;; cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  write ";** Skipping because cFE Core State is Disabled."
  goto os_AppReset_Check
endif

;; Use the TST_CS app to corrupt the cFE CRC
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 2

/$SC_$CPU_TST_CS_CorruptCFECRC
wait 5

;; Check for the cFE event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - cFE Core Miscompare Event ID=",CS_CFECORE_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - cFE Core Miscompare Event was not received. Time-out occurred."
endif

os_AppReset_Check:
;; OS State
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  write ";** Skipping because OS State is Disabled."
  goto step_4_2
endif

;; Use the TST_CS app to corrupt the OS and cFE CRCs
ut_setupevents "$SC","$CPU","TST_CS",TST_CS_CORRUPT_OS_CRC_INF_EID, "INFO", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_OS_MISCOMPARE_ERR_EID, "ERROR", 4

/$SC_$CPU_TST_CS_CorruptOSCRC
wait 5

;; Check for OS the event message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

;; Check for the Miscompare event message
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - OS Miscompare Event ID=",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - OS Miscompare Event was not received. Time-out occurred."
endif

cFE_AppReset_Check:
wait 5

step_4_2:
write ";*********************************************************************"
write ";  Step 4.2: Dump the Non-Volatile Code Segment Results Table."
write ";*********************************************************************"
;; Non-Volatile Memory State
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  write ";** Skipping tests because Non-Volatile Memory State is Disabled."
  goto step_4_5
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_2",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.3: Corrupt the Non-volatile Baseline CRCs in order to       "
write ";  determine if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
;; Loop for each valid entry in the results table
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.4: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_4",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

step_4_5:
write ";*********************************************************************"
write ";  Step 4.5: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
;; Application State
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  write ";** Skipping tests because Application State is Disabled."
  goto step_4_8
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_5",hostCPU,appResTblId)
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
write ";  Step 4.6: Corrupt the Application CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_APP_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptAppCRC AppName=$SC_$CPU_CS_APP_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.7: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_7",hostCPU,appResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

step_4_8:
write ";*********************************************************************"
write ";  Step 4.8: Dump the Table Results Table."
write ";*********************************************************************"
;; Table State
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  write ";** Skipping tests because Table State is Disabled."
  goto step_4_11
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_8",hostCPU,tblResTblId)
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
write ";  Step 4.9: Corrupt the Table CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_TABLE_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptTblCRC TABLEName=$SC_$CPU_CS_TBL_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.10: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_10",hostCPU,tblResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

step_4_11:
write ";*********************************************************************"
write ";  Step 4.11: Dump the User-defined Memory Results Table."
write ";*********************************************************************"
;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping tests because User-Defined Memory State is Disabled."
  goto step_4_14
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_11",hostCPU,usrResTblId)
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
write ";  Step 4.12: Corrupt the User-defined Memory CRCs in order to determine"
write ";  if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "$SC", "$CPU", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /$SC_$CPU_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.13: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_13",hostCPU,usrResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

step_4_14:
write ";*********************************************************************"
write ";  Step 4.14: Save the CRCs so that they can be checked after the reset."
write ";*********************************************************************"
osCRC = $SC_$CPU_CS_OSBASELINE
cFECRC = $SC_$CPU_CS_CFECOREBASELINE

;; Loop and store the Non-Volatile Memory CRCs
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    eeCRCs[i] = $SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    appCRCs[i] = $SC_$CPU_CS_APP_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    tblCRCs[i] = $SC_$CPU_CS_TBL_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    usrCRCs[i] = $SC_$CPU_CS_MEM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.15: Stop the CS and TST_CS applications."
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_CS"
wait 5

;; Setup event to capture on CS Application stop (DCR #146120)
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_STOP_INF_EID, "INFO", 1

/$SC_$CPU_ES_DELETEAPP APPLICATION=CSAppName
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - CS generated proper event message upon normal exit."
else
  write "<!> Failed - CS did not generate proper event message on normal exit."
endif

write ";*********************************************************************"
write ";  Step 4.16: Re-start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("4.16")

;; Set the eepromCRC variable to the initial value after app startup
eepromCRC = $SC_$CPU_CS_EEPROMBASELINE

write ";*********************************************************************"
write ";  Step 4.17: Wait until the CRCs have been recalculated.    "
write ";*********************************************************************"
if ($SC_$CPU_CS_PASSCTR = 0) then
  ut_tlmwait $SC_$CPU_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif

write ";*********************************************************************"
write ";  Step 4.18: Verify that the CRCs contained in the Housekeeping packet"
write ";  have been recalculated. "
write ";*********************************************************************"
;; OS State
if (p@$SC_$CPU_CS_OSState = "Disabled") then
  write ";** Skipping OS CRC recalculation check because OS State is Disabled."
  goto check_cFE_CRC3
endif

;; Check the OS CRC
if (osCRC <> $SC_$CPU_CS_OSBASELINE) then
  write "<*> Passed (9010) - OS CRC has been recalculated on an Application Reset."
  ut_setrequirements CS_9010, "P"
else
  write "<!> Failed (9010) - OS CRC was not recalculated on an Application Reset."
  ut_setrequirements CS_9010, "F"
endif

check_cFE_CRC3:
;; Check the cFE CRC
;; cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Disabled") then
  write ";** Skipping CRC recalculation check because cFE Core State is Disabled."
  goto check_EEPROM_CRC3
endif

if (cFECRC <> $SC_$CPU_CS_CFECOREBASELINE) then
  write "<*> Passed (9010) - cFE CRC has been recalculated on an Application Reset."
  ut_setrequirements CS_9010, "P"
else
  write "<!> Failed (9010) - cFE CRC was not recalculated on an Application Reset."
  ut_setrequirements CS_9010, "F"
endif

check_EEPROM_CRC3:
;; Non-Volatile Memory State
if (p@$SC_$CPU_CS_EepromState = "Disabled") then
  write ";** Skipping CRC recalculation check because Non-Volatile Memory State is Disabled."
  goto step_4_20
endif

;; Check the overall EEPROM CRC
if (eepromCRC <> $SC_$CPU_CS_EEPROMBASELINE) then
  write "<*> Passed (9012) - Overall EEPROM CRC has been recalculated on an Application Reset."
  ut_setrequirements CS_9012, "P"
else
  write "<!> Failed (9012) - Overall EEPROM CRC was not recalculated on an Application Reset."
  ut_setrequirements CS_9012, "F"
endif

write ";*********************************************************************"
write ";  Step 4.19: Dump the Non-Volatile Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","$cpu_eerestbl4_19",hostCPU,eeResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

;; Check the enabled EEPROM Table entries
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    if (eeCRCs[i] <> $SC_$CPU_CS_EEPROM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9011) - EEPROM entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9011, "P"
    else
      write "<!> Failed (9011) - EEPROM entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9011, "F"
    endif
  endif
enddo

step_4_20:
write ";*********************************************************************"
write ";  Step 4.20: Dump the Application Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
;; Application State
if (p@$SC_$CPU_CS_AppState = "Disabled") then
  write ";** Skipping CRC recalculation check because Application State is Disabled."
  goto step_4_21
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","$cpu_apprestbl4_20",hostCPU,appResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    if (appCRCs[i] <> $SC_$CPU_CS_APP_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9013) - App entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9013, "P"
    else
      write "<!> Failed (9013) - App entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9013, "F"
    endif
  endif
enddo

step_4_21:
write ";*********************************************************************"
write ";  Step 4.21: Dump the Table Results Table and verify the CRCs have "
write ";  been recalculated. "
write ";*********************************************************************"
;; Table State
if (p@$SC_$CPU_CS_TableState = "Disabled") then
  write ";** Skipping CRC recalculation check because Table State is Disabled."
  goto step_4_22
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","$cpu_tblrestbl4_21",hostCPU,tblResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif
;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (tblCRCs[i] <> $SC_$CPU_CS_TBL_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9014) - Table entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9014, "P"
    else
      write "<!> Failed (9014) - Table entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9014, "F"
    endif
  endif
enddo

step_4_22:
write ";*********************************************************************"
write ";  Step 4.22: Dump the User-defined Memory Results Table and verify the"
write ";  CRCs have been recalculated. "
write ";*********************************************************************"
;; User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_4_23
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","$cpu_usrrestbl4_22",hostCPU,usrResTblId)
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif
;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@$SC_$CPU_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    if (usrCRCs[i] <> $SC_$CPU_CS_MEM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9015) - User-defined Memory entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9015, "P"
    else
      write "<!> Failed (9015) - User-defined Memory entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9015, "F"
    endif
  endif
enddo

step_4_23:
write ";*********************************************************************"
write ";  Step 4.23: Change the region states and verify that the states are"
write ";  preserved after a Processor Reset. "
write ";*********************************************************************"
write ";  Step 4.23.1: Toggle the region states."
write ";*********************************************************************"
;; Change the OS State
if (p@$SC_$CPU_CS_OSState = "Enabled") then
  /$SC_$CPU_CS_DisableOS
  wait 1
  expOSState = "Disabled"
else
  /$SC_$CPU_CS_EnableOS
  wait 1
  expOSState = "Enabled"
endif

;; Change the cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = "Enabled") then
  /$SC_$CPU_CS_DisableCFECore
  wait 1
  expCFEState = "Disabled"
else
  /$SC_$CPU_CS_EnableCFECore
  wait 1
  expCFEState = "Enabled"
endif

;; Change the EEPROM State
if (p@$SC_$CPU_CS_EepromState = "Enabled") then
  /$SC_$CPU_CS_DisableEeprom
  wait 1
  expEepromState = "Disabled"
else
  /$SC_$CPU_CS_EnableEeprom
  wait 1
  expEepromState = "Enabled"
endif

;; Change the User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = "Enabled") then
  /$SC_$CPU_CS_DisableMemory
  wait 1
  expMemoryState = "Disabled"
else
  /$SC_$CPU_CS_EnableMemory
  wait 1
  expMemoryState = "Enabled"
endif

;; Change the Applications State
if (p@$SC_$CPU_CS_AppState = "Enabled") then
  /$SC_$CPU_CS_DisableApps
  wait 1
  expAppState = "Disabled"
else
  /$SC_$CPU_CS_EnableApps
  wait 1
  expAppState = "Enabled"
endif

;; Change the Tables State
if (p@$SC_$CPU_CS_TableState = "Enabled") then
  /$SC_$CPU_CS_DisableTables
  wait 1
  expTableState = "Disabled"
else
  /$SC_$CPU_CS_EnableTables
  wait 1
  expTableState = "Enabled"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.23.2: Perform an Application Reset."
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP APPLICATION=CSAppName
wait 5

write ";*********************************************************************"
write ";  Step 4.23.3: Start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s $sc_$cpu_cs_start_apps("4.23.3")

write ";*********************************************************************"
write ";  Step 4.23.4: Check the states and verify they are as expected.  "
write ";********************************************************************"
;; Check the OS State
if (p@$SC_$CPU_CS_OSState = expOSState) then
  write "<*> Passed (9008) - OS State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - OS State not set as expected after reset. Expected '",expOSState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the cFE Core State
if (p@$SC_$CPU_CS_CFECoreState = expCFEState) then
  write "<*> Passed (9008) - cFE State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - cFE State not set as expected after reset. Expected '",expCFEState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the EEPROM State
if (p@$SC_$CPU_CS_EepromState = expEepromState) then
  write "<*> Passed (9008) - Eeprom State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - EEPROM State not set as expected after reset. Expected '",expEepromState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the User-Defined Memory State
if (p@$SC_$CPU_CS_MemoryState = expMemoryState) then
  write "<*> Passed (9008) - Memory State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - Memory State not set as expected after reset. Expected '",expMemoryState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the Applications State
if (p@$SC_$CPU_CS_AppState = expAppState) then
  write "<*> Passed (9008) - Application State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - Application State not set as expected after reset. Expected '",expAppState,"'."
  ut_setrequirements CS_9008, "F"
endif

;; Check the Tables State
if (p@$SC_$CPU_CS_TableState = expTableState) then
  write "<*> Passed (9008) - Tables State as expected after reset."
  ut_setrequirements CS_9008, "P"
else
  write "<!> Failed (9008) - Tables State not set as expected after reset. Expected '",expTableState,"'."
  ut_setrequirements CS_9008, "F"
endif

write ";*********************************************************************"
write ";  Step 5.0: Table-defined Anomoly Tests."
write ";*********************************************************************"
write ";  Step 5.1: Create a Non-Volatile Segment Definition table load file "
write ";  that contains an invalid segment and an invalid state. "
write ";*********************************************************************"
s $sc_$cpu_cs_edt2
wait 5

write ";*********************************************************************"
write ";  Step 5.2: Upload the invalid file created above."
write ";*********************************************************************"
;;s ftp_file ("CF:0/apps","eeprom_def_invalid","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_invalid",eeTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 5.3: Create an Application Code Segment Definition table load "
write ";  file containing valid entries along with an entry that contains an "
write ";  invalid state. "
write ";*********************************************************************"
s $sc_$cpu_cs_adt3
wait 5

write ";*********************************************************************"
write ";  Step 5.4: Upload the invalid file created above."
write ";*********************************************************************"
;;s ftp_file ("CF:0/apps","app_def_tbl_invalid","cs_apptbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","app_def_tbl_invalid",appTableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 5.5: Create a Table Definition table load file containing empty"
write ";  entries in between valid entries and an entry with an invalid state."
write ";*********************************************************************"
s $sc_$cpu_cs_tdt3
wait 5

write ";*********************************************************************"
write ";  Step 5.6: Upload the invalid file created above."
write ";*********************************************************************"
;;s ftp_file ("CF:0/apps","tbl_def_tbl_invalid","cs_tablestbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","tbl_def_tbl_invalid",tblTableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 5.7: Create a User-defined Memory Definition table load file "
write ";  containing several valid entries, an entry that contains an invalid "
write ";  address, an entry that contains an invalid range and an entry with an"
write ";  invalid state."
write ";*********************************************************************"
s $sc_$cpu_cs_mdt2
wait 5

write ";*********************************************************************"
write ";  Step 5.8: Upload the invalid file created above."
write ";*********************************************************************"
;;s ftp_file ("CF:0/apps","usrmem_def_invalid","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_invalid",memTableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 5.9: Send the Power-On reset command."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 5.10: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 5.11: Start the applications in order for the load files created"
write ";  above to successfully pass validation and load. "
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_RANGE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_RANGE_ERR_EID, "ERROR", 4
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_TABLES_STATE_ERR_EID, "ERROR", 5
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_APP_STATE_ERR_EID, "ERROR", 6
s $sc_$cpu_cs_start_apps("5.11")

;; 3/8/17: Change to add these events prior to call to start_apps above.
;;         There are 8 event slots. start_app uses 1 and 2. If the events are
;;         captured, the appropriate requirements can be set to 'P' and 'F'
;;         rather than 'A'
;write ";*********************************************************************"
;write ";  The event messages indicating the validation failures on"
;write ";  CS startup cannot be captured since they occur in a different "
;write ";  procedure. However, they will be contained in the log file and can "
;write ";  be verified by analyzing this step after this procedure completes. "
;write ";********************************************************************"
;ut_setrequirements CS_90031, "A" ;; EID 102
;;  Check for validation failure events
;; EEPROM Table
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9003.1) - EEPROM Validation Failure event generated."
  ut_setrequirements CS_90031, "P"
else
  write "<!> Failed (9003.1) - Expected EEPROM Validation Failure event was not received."
  ut_setrequirements CS_90031, "F"
endif

;ut_setrequirements CS_90051, "A" ;; EID 106
;; Application Table
ut_tlmwait $SC_$CPU_find_event[6].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9005.1) - Application State Validation Failure event generated."
  ut_setrequirements CS_90051, "P"
else
  write "<!> Failed (9005.1) - Expected Application State Validation Failure event was not received."
  ut_setrequirements CS_90051, "F"
endif

;ut_setrequirements CS_90061, "A" ;; EID 105
;; Tables Table
ut_tlmwait $SC_$CPU_find_event[5].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9006.1) - Table State Validation Failure event generated."
  ut_setrequirements CS_90061, "P"
else
  write "<!> Failed (9006.1) - Expected Table State Validation Failure event was not received."
  ut_setrequirements CS_90061, "F"
endif

;ut_setrequirements CS_90071, "A" ;; EID 104
;; User-Defined Memory Table
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9007.1) - User-Defined Memory Validation Failure event generated."
  ut_setrequirements CS_90071, "P"
else
  write "<!> Failed (9007.1) - Expected User-Defined Memory Validation Failure event was not received."
  ut_setrequirements CS_90071, "F"
endif

write ";*********************************************************************"
write ";  Step 5.12: Send the Processor reset command."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 5.13: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 5.14: Start the applications in order for the load files created"
write ";  above to attempt to be loaded. "
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_RANGE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_RANGE_ERR_EID, "ERROR", 4
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_TABLES_STATE_ERR_EID, "ERROR", 5
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_APP_STATE_ERR_EID, "ERROR", 6
s $sc_$cpu_cs_start_apps("5.14")

;write ";*********************************************************************"
;write ";  The event messages indicating the validation failures on"
;write ";  CS startup cannot be captured since they occur in a different "
;write ";  procedure. However, they will be contained in the log file and can "
;write ";  be verified by analyzing the log after this procedure completes. "
;write ";********************************************************************"
;;  Check for validation failure events
;; EEPROM Table
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9011.1) - EEPROM Validation Failure event generated."
  ut_setrequirements CS_90111, "P"
else
  write "<!> Failed (9011.1) - Expected EEPROM Validation Failure event was not received."
  ut_setrequirements CS_90111, "F"
endif

;; Application Table
ut_tlmwait $SC_$CPU_find_event[6].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9013.1) - Application State Validation Failure event generated."
  ut_setrequirements CS_90131, "P"
else
  write "<!> Failed (9013.1) - Expected Application State Validation Failure event was not received."
  ut_setrequirements CS_90131, "F"
endif

;; Tables Table
ut_tlmwait $SC_$CPU_find_event[5].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9014.1) - Table State Validation Failure event generated."
  ut_setrequirements CS_90141, "P"
else
  write "<!> Failed (9014.1) - Expected Table State Validation Failure event was not received."
  ut_setrequirements CS_90141, "F"
endif

;; User-Defined Memory Table
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9015.1) - User-Defined Memory Validation Failure event generated."
  ut_setrequirements CS_90151, "P"
else
  write "<!> Failed (9015.1) - Expected User-Defined Memory Validation Failure event was not received."
  ut_setrequirements CS_90151, "F"
endif

write ";*********************************************************************"
write ";  Step 5.15: Stop and restart the CS and TST_CS applications."
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP APPLICATION=CSAppName
wait 5

ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_RANGE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_RANGE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_RANGE_ERR_EID, "ERROR", 4
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_TABLES_STATE_ERR_EID, "ERROR", 5
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_APP_STATE_ERR_EID, "ERROR", 6
s $sc_$cpu_cs_start_apps("5.15")

;write ";*********************************************************************"
;write ";  The event messages indicating the validation failures on"
;write ";  CS startup cannot be captured since they occur in a different "
;write ";  procedure. However, they will be contained in the log file and can "
;write ";  be verified by analyzing the log after this procedure completes. "
;write ";********************************************************************"
;;  Check for validation failure events
;; EEPROM Table
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9011.1) - EEPROM Validation Failure event generated."
  ut_setrequirements CS_90111, "P"
else
  write "<!> Failed (9011.1) - Expected EEPROM Validation Failure event was not received."
  ut_setrequirements CS_90111, "F"
endif

;; Application Table
ut_tlmwait $SC_$CPU_find_event[6].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9013.1) - Application State Validation Failure event generated."
  ut_setrequirements CS_90131, "P"
else
  write "<!> Failed (9013.1) - Expected Application State Validation Failure event was not received."
  ut_setrequirements CS_90131, "F"
endif

;; Tables Table
ut_tlmwait $SC_$CPU_find_event[5].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9014.1) - Table State Validation Failure event generated."
  ut_setrequirements CS_90141, "P"
else
  write "<!> Failed (9014.1) - Expected Table State Validation Failure event was not received."
  ut_setrequirements CS_90141, "F"
endif

;; User-Defined Memory Table
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9015.1) - User-Defined Memory Validation Failure event generated."
  ut_setrequirements CS_90151, "P"
else
  write "<!> Failed (9015.1) - Expected User-Defined Memory Validation Failure event was not received."
  ut_setrequirements CS_90151, "F"
endif

write ";*********************************************************************"
write ";  Step 5.16: Create a Non-Volatile Segment Definition table load file "
write ";  that contains an an invalid state. "
write ";*********************************************************************"
s $sc_$cpu_cs_edt5
wait 5

write ";*********************************************************************"
write ";  Step 5.17: Upload the invalid file created above."
write ";*********************************************************************"
;;s ftp_file ("CF:0/apps","eeprom_bad_state","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_bad_state",eeTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 5.18: Create a User-defined Memory Definition table load file "
write ";  that contains an entry with an invalid state."
write ";*********************************************************************"
s $sc_$cpu_cs_mdt2
wait 5

write ";*********************************************************************"
write ";  Step 5.19: Upload the invalid file created above."
write ";*********************************************************************"
;;s ftp_file ("CF:0/apps","usrmem_def_invalid3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_invalid3",memTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 5.20: Send the Power-On reset command."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 5.21: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 5.22: Start the applications in order for the load files created"
write ";  above to successfully pass validation and load. "
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_STATE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_STATE_ERR_EID, "ERROR", 4
s $sc_$cpu_cs_start_apps("5.22")

;;write ";*********************************************************************"
;;write ";  The event messages indicating the validation failures on"
;;write ";  CS startup cannot be captured since they occur in a different "
;;write ";  procedure. However, they will be contained in the log file and can "
;;write ";  be verified by analyzing this step after this procedure completes. "
;;write ";********************************************************************"
;;ut_setrequirements CS_90032, "A" ; EID 101
;;ut_setrequirements CS_90072, "A" ; EID 103
;;  Check for validation failure events
;; EEPROM Table
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9003.2) - EEPROM Validation Failure event generated."
  ut_setrequirements CS_90032, "P"
else
  write "<!> Failed (9003.2) - Expected EEPROM Validation Failure event was not received."
  ut_setrequirements CS_90032, "F"
endif

;; User-Defined Memory Table
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9007.2) - User-Defined Memory Validation Failure event generated."
  ut_setrequirements CS_90072, "P"
else
  write "<!> Failed (9007.2) - Expected User-Defined Memory Validation Failure event was not received."
  ut_setrequirements CS_90072, "F"
endif

write ";*********************************************************************"
write ";  Step 5.23: Send the Processor reset command."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 5.24: Start the TST_CS_MemTbl application in order to setup   "
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
wait 10

write ";*********************************************************************"
write ";  Step 5.25: Start the applications in order for the load files created"
write ";  above to attempt to be loaded. "
write ";********************************************************************"
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_STATE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_STATE_ERR_EID, "ERROR", 4
s $sc_$cpu_cs_start_apps("5.25")

;;write ";*********************************************************************"
;;write ";  The event messages indicating the validation failures on"
;;write ";  CS startup cannot be captured since they occur in a different "
;;write ";  procedure. However, they will be contained in the log file and can "
;;write ";  be verified by analyzing the log after this procedure completes. "
;;write ";********************************************************************"
;;  Check for validation failure events
;; EEPROM Table
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9011.2) - EEPROM Validation Failure event generated."
  ut_setrequirements CS_90112, "P"
else
  write "<!> Failed (9011.2) - Expected EEPROM Validation Failure event was not received."
  ut_setrequirements CS_90112, "F"
endif

;; User-Defined Memory Table
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9015.2) - User-Defined Memory Validation Failure event generated."
  ut_setrequirements CS_90152, "P"
else
  write "<!> Failed (9015.2) - Expected User-Defined Memory Validation Failure event was not received."
  ut_setrequirements CS_90152, "F"
endif

write ";*********************************************************************"
write ";  Step 5.26: Stop and restart the CS and TST_CS applications."
write ";*********************************************************************"
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_CS"
wait 5
/$SC_$CPU_ES_DELETEAPP APPLICATION=CSAppName
wait 5

ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_EEPROM_STATE_ERR_EID, "ERROR", 3
ut_setupevents "$SC","$CPU",{CSAppName},CS_VAL_MEMORY_STATE_ERR_EID, "ERROR", 4
s $sc_$cpu_cs_start_apps("5.26")

;;write ";*********************************************************************"
;;write ";  The event messages indicating the validation failures on"
;;write ";  CS startup cannot be captured since they occur in a different "
;;write ";  procedure. However, they will be contained in the log file and can "
;;write ";  be verified by analyzing the log after this procedure completes. "
;;write ";********************************************************************"
;;  Check for validation failure events
;; EEPROM Table
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9011.2) - EEPROM Validation Failure event generated."
  ut_setrequirements CS_90112, "P"
else
  write "<!> Failed (9011.2) - Expected EEPROM Validation Failure event was not received."
  ut_setrequirements CS_90112, "F"
endif

;; User-Defined Memory Table
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (9015.2) - User-Defined Memory Validation Failure event generated."
  ut_setrequirements CS_90152, "P"
else
  write "<!> Failed (9015.2) - Expected User-Defined Memory Validation Failure event was not received."
  ut_setrequirements CS_90152, "F"
endif

write ";*********************************************************************"
write ";  Step 6.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 6.1: Upload the default Definition files downloaded in step 1.1."
write ";*********************************************************************"
s ftp_file ("CF:0/apps","cs_mem_orig_tbl.tbl",memTableFileName,hostCPU,"P")
s ftp_file ("CF:0/apps","cs_app_orig_tbl.tbl",appTableFileName,hostCPU,"P")
s ftp_file ("CF:0/apps","cs_tbl_orig_tbl.tbl",tblTableFileName,hostCPU,"P")
s ftp_file ("CF:0/apps","cs_eeprom_orig_tbl.tbl",eeTableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 6.2: Send the Power-On Reset command. "
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

procterm:

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_reset"
write ";*********************************************************************"
ENDPROC
