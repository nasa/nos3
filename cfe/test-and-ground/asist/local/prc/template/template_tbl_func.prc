PROC $sc_$cpu_tbl_func
;*******************************************************************************
;  Test Name:  $sc_$cpu_tbl_func
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify the functionality of the cFE
;	Table Services commands. 
;
;  Requirements Tested
;    cTBL6000	Upon receipt of Command the cFE shall Load an Inactive Table
;		Image with the contents of the Command specified File.
;    cTBL6000.5	If the specified table is defined as Dump Only then the command
;		shall be rejected and an event message be generated.
;    cTBL6001	Upon receipt of Command the cFE shall dump the Command specified
;		Active or Inactive Table contents to a Command specified File. 
;    cTBL6002 	Upon receipt of Command the cFE shall determine the validity of
;		the contents of either the Active or Inactive Table Image of the
;		Command specified Table.
;    cTBL6002.1	The cFE shall compute a Data Integrity Check Value on the
;		contents of either the Active or Inactive Table	Image of the
;		Command specified Table and report the result in telemetry. 
;    cTBL6002.2	The cFE shall Request an Application to validate the contents of
;		either the Active or Inactive Table Image of the Command
;		specified Table and report the result in telemetry. 
;    cTBL6003	Upon receipt of Command the cFE shall make an Inactive Table
;		Image of the Command specified Table the Active Table Image.
;    cTBL6005 	Upon receipt of Command the cFE shall write the contents of the
;		Table Registry to a file. 
;    cTBL6005.1	If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename.
;    cTBL6006 	Upon receipt of Command the cFE shall telemeter the contents of
;		the Table Registry associated with a Command specified Table. 
;    cTBL6011	The cFE shall provide the following Table Service data items in
;		telemetry (SB Messages):
;			a. Valid Command Counter
;			b. Invalid Command Counter
;			c. Number of Tables Currently Registered
;			d. Table Identifier of Last Table Modified
;			e. Time of Last Table Modification
;			f. Source Filename of Last Table Load
;			g. Destination Filename of Last Table Dump
;			h. Table Identifier of Commanded Data Integrity Check
;			i. Commanded Data Integrity Check Value
;			j. Commanded Table Verification Function result
;			k. Number of unused Shared Buffers
;    cTBL6012 	Upon receipt or a command the cFE shall abort the loading of the
;		specified table.
;    cTBL6012.1	If the Table buffering characteristics for the specified Table
;		indicate that it is a Single-buffered Table, then the allocated
;		shared buffer shall be released.
;    cTBL6012.2	If the Table buffering characteristics for the specified Table
;		indicate that it is a Double-buffered Table, then the inactive
;		buffer shall be marked as uninitialized.
;    cTBL6012.3	The Table Registry shall indicate that there are no loads
;		pending for the specified Table.
;    cTBL6300 	Upon receipt of Request, the cFE shall create a zero filled
;		Table Image with the Request specified name, size, buffering
;		characteristics, read-only, integrity check and sharable
;		preferences and Table Validation Function address.
;    cTBL6300.1	The cFE shall allow an Application to specify an address as the
;		one and only buffer for a dump-only Table.
;    cTBL6301 	Upon receipt of Request, the cFE shall free the resources
;		associated the Request specified Application. 
;    cTBL6302 	Upon receipt of Request, the cFE shall initialize the contents
;		of the Request specified Table Image with the contents of the
;		Request specified File. 
;    cTBL6302.1	If the Request specified File contains more data than the size
;		of the Request specified Table, the Table Image will not be
;		initialized and an Event Message shall be generated. 
;    cTBL6302.2	If the Request specified File contains less data than the size
;		of the Request specified Table, the first portion of the Table
;		Image will be initialized with the contents of the File and an
;		Event Message shall be generated. 
;    cTBL6303 	Upon receipt of Request, the cFE shall provide the calling
;		Application with a unique identifier of an existing Table Image.
;    cTBL6304 	Upon receipt of Request, the cFE shall free resources allocated
;		for the Request specified Table. 
;    cTBL6305	Upon receipt of Request, the cFE shall provide the calling
;		Application with the address of the Request specified Table data
;    cTBL6305.1	Upon providing a calling Application with the address of a 
;		Table's data, the cFE shall lock the contents of the Table to
;		prevent modification.
;    cTBL6305.2	If a Table has been modified since the last Table address
;		request, the cFE shall notify the calling Application that the
;		Table has been modified.
;    cTBL6306 	Upon receipt of Request, the cFE shall unlock the contents of
;		the Request specified Table.
;    cTBL6308 	Upon receipt of Request, the cFE shall update the request
;		specified Table if a load is pending and it is unlocked.
;    cTBL6308.1	If a Table is locked when an update Request is made, an
;		appropriate error code shall be returned to the calling
;		Application and the update shall not occur.
;    cTBL6309	Upon receipt of Request, the cFE shall provide the following
;		information to the calling Application for the specified Table:
;			a. Size of the table
;			b. Number of Users with access to the table (sharing)
;			c. Filename of the last file used to modify the table
;			d. File creation time of the last file used to modify
;			   the table
;			e. Time of last modification
;			f. Flag indicating if the Table has been initialized
;			g. Flag indicating if the Table is dump only
;			h. Flag indicating if the Table has a dedicated buffer
;			i. Flag indicating if the table is maintained in the
;			   Critical Data Store
;    cTBL6310	Upon receipt of Request, the cFE shall indicate if the specified
;		table has a validation, update, or dump pending.
;    cTBL6311	Upon receipt of Request, the cFE shall provide the calling
;		Application with the addresses of the data for the tables
;		requested if more than one table is needed.
;    cTBL6311.1	Upon providing a calling Application with the addresses of a
;		Tables' data, the cFE shall lock the contents of the Tables to
;		prevent modification.
;    cTBL6311.2	If at least one Table has been modified since the last Table
;		address request, the cFE shall notify the calling Application
;		that a Table has been modified.
;    cTBL6312	Upon receipt of Request, the cFE shall unlock the contents of
;		the Request specified Tables.
;    cTBL6700	The cFE shall support <Platform_defined, 128> Tables.
;    cTBL6701	The cFE shall support <Platform_defined, 4> Single-Buffer Table
;		Loads and Application-Defined-Address-Table dumps simultaneously
;
;  Prerequisite Conditions
;       None 
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	03/13/06	W. Moleski	Initial development.
;	11/21/06	W. Moleski	Modified for cFE 4.0.
;	02/21/07	W. Moleski	Modified based upon Build Verification
;					tests.
;	02/08/12	W. Moleski	Added variable for ram disk and replaced
;					ut_setupevt with ut_setupevents
;       05/31/16        W. Moleski      Added variable for CPU to connect and
;                                       retrieve files from (hostCPU) since this
;                                       may be different from the command CPU.
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;  	ut_pfindicate		Directive to print the pass fail status
;				of a particular requirement number.
;  	ut_setupevents		Directive to look for a particular event
;				and increment a value in the CVT to
;				indicate receipt.
;  	ut_setrequirements	Directive to status cfe requirements.
;	ut_sendcmd     		Directive to send EVS commands Verifies command
;				processed and command error counters.
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#include "ut_statusdefs.h"
#include "cfe_tbl_events.h"
#include "cfe_evs_events.h"
#include "tst_tbl_events.h"
#include "tst_tbl2_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************
LOCAL i,j

#define TS_6000		0
#define TS_60005	1
#define TS_6001		2
#define TS_6002		3
#define TS_60021	4
#define TS_60022	5
#define TS_6003		6
#define TS_60031	7
#define TS_600311	8
#define TS_600312	9
#define TS_6005		10
#define TS_60051	11
#define TS_6006		12
#define TS_6011c	13
#define TS_6011d	14
#define TS_6011e	15
#define TS_6011f	16
#define TS_6011g	17
#define TS_6011h	18
#define TS_6011i	19
#define TS_6011j	20
#define TS_6011k	21
#define TS_6012		22
#define TS_60121	23
#define TS_60122	24
#define TS_60123	25
#define TS_6300		26
#define TS_63001	27
#define TS_6301		28
#define TS_6302		29
#define TS_63021	30
#define TS_63022	31
#define TS_6303		32
#define TS_6304		33
#define TS_6305		34
#define TS_63051	35
#define TS_63052	36
#define TS_6306		37
#define TS_6308		38
#define TS_63081	39
#define TS_6309		40
#define TS_6310		41
#define TS_6311		42
#define TS_63111	43
#define TS_63112	44
#define TS_6312		45
#define TS_6700		46
#define TS_6701		47

global ut_req_array_size = 47
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL cmdcnt, errcnt, num_free_buffers, rawcmd, numTables
LOCAL tbl_1_pkt, tbl_2_pkt
LOCAL tbl_1_appid, tbl_2_appid
LOCAL last_data_integrity_value, tblcnt
LOCAL newTableName, zeroTableFlag, expectedNumUsers, newFileName
LOCAL tbl7Index, tbl8Index, tbl7_Found, tbl8_Found
LOCAL tbl_1_index, tbl_2_index

local ramDir = "RAM:0"
local hostCPU = "$CPU"

;**********************************************************************
; Set the local values 
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["TS_6000","TS_6000.5","TS_6001","TS_6002","TS_6002.1","TS_6002.2","TS_6003","TS_6003.1","TS_6003.1.1","TS_6003.1.2","TS_6005","TS_6005.1","TS_6006","TS_6011c","TS_6011d","TS_6011e","TS_6011f","TS_6011g","TS_6011h","TS_6011i", "TS_6011j", "TS_6011k", "TS_6012", "TS_6012.1", "TS_6012.2", "TS_6012.3", "TS_6300", "TS_6300.1", "TS_6301", "TS_6302", "TS_6302.1", "TS_6302.2", "TS_6303", "TS_6304", "TS_6305", "TS_6305.1", "TS_6305.2", "TS_6306", "TS_6308", "TS_6308.1", "TS_6309", "TS_6310", "TS_6311", "TS_6311.1", "TS_6311.2", "TS_6312", "TS_6700", "TS_6701"]

;; CPU1 is the default
tbl_1_pkt = "0FA2"
tbl_2_pkt = "0FA3"
tbl_1_appid = 4002
tbl_2_appid = 4003

;;if ("$CPU" = "CPU2") then
;;  tbl_1_pkt = "0FC0"
;;  tbl_2_pkt = "0FC1"
;;  tbl_1_appid = 4032
;;  tbl_2_appid = 4033
;;elseif ("$CPU" = "CPU3") then
;;  tbl_1_pkt = "0FE0"
;;  tbl_2_pkt = "0FE1"
;;  tbl_1_appid = 4064
;;  tbl_2_appid = 4065
;;endif

write ";**********************************************************************"
write "; Step 1.0:  Initialize the CPU for this test."
write ";**********************************************************************"
write "; Step 1.1: Command a Power-On Reset on $CPU"
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

write ";*********************************************************************"
write "; Step 1.2: Load the Test Applications (if necessary). "
write ";*********************************************************************"
write "BEFORE loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"
                                                                                
if ("$CPU" = "") then
  start tst_tbl_apps_start ("1.2")
else
  start $sc_$cpu_tst_tbl_apps_start ("1.2")
endif
 
Write "AFTER loading, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"

write ";*********************************************************************"
write "; Step 1.3: Search the Table Registry for the locations of the default"
write "; TST_TBL application tables."
write ";*********************************************************************"
for i = 1 to $SC_$CPU_TBL_NUMTABLES do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_01") then
    tbl_1_index = i
  endif
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
    tbl_2_index = i
  endif
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.AppDef_tbl_03") then
    ut_setrequirements TS_63001, "P"
  endif
enddo

write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index

write ";**********************************************************************"
write "; Step 2.0: Table Validation Tests "
write ";**********************************************************************"
write "; Step 2.1: Validate the default active tables (Table 1 & 2)"
write ";**********************************************************************"
;; Set the last data integrity check value
last_data_integrity_value = 0

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE ACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Active Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - Active Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Look for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Active Table 2 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Active Table 2 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1; 6011I) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  ut_setrequirements TS_6011i, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1; 6011I) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
  ut_setrequirements TS_6011i, "F"
endif

;;; Verify the Last Table Name validated
if ($SC_$CPU_TBL_LASTVALTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed (6011H) - Correct Table Identifier reported in TLM."
  ut_setrequirements TS_6011h, "P"
else
  write "<!> Failed (6011H) - Table Identifier reported in TLM does not match table validated."
  ut_setrequirements TS_6011h, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

;;; Verify the Last Buffer Validated indicates ACTIVE
if (p@$SC_$CPU_TBL_LASTVALBUF = "Active") then
  write "<*> Passed: TBL HK indicates the Active buffer was validated."
else
  write "<!> Failed: TBL HK does not indicate the Active buffer was validated."
endif

wait 5

;;; Validate the Active buffer of Table #1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE ACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Active Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - Active Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Look for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Active Table 1 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Active Table 1 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1; 6011I) - Data Integrity Check Value is correct."
  ut_setrequirements TS_60021, "P"
  ut_setrequirements TS_6011i, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1; 6011I) - Data Integrity Check Value is not correct."
  ut_setrequirements TS_60021, "F"
  ut_setrequirements TS_6011i, "F"
endif

;;; Verify the Last Table Name validated
if ($SC_$CPU_TBL_LASTVALTBLNAME = "TST_TBL.dflt_tbl_01") then
  write "<*> Passed (6011H) - Correct Table Identifier reported in TLM."
  ut_setrequirements TS_6011h, "P"
else
  write "<!> Failed (6011H) - Table Identifier reported in TLM does not match table validated."
  ut_setrequirements TS_6011h, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.2 : Load the default double buffer table (Table #2) with a full"
write "; load."
write ";**********************************************************************"
;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = 0
$SC_$CPU_TST_TBL_TABLE2.element[2] = 1
$SC_$CPU_TST_TBL_TABLE2.element[3] = 2
$SC_$CPU_TST_TBL_TABLE2.element[4] = 3
$SC_$CPU_TST_TBL_TABLE2.element[5] = 4
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[7] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[8] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[9] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 1", "dbl_tbl_ld_1","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

;*** Check to see if the TBL HK telemetry reported the correct file
if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed (TS_6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (TS_6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
endif

; check for the successful load event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LoadBufferID <> -1)
write "<*> Passed - Load Buffer flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.3: Load the default single buffer table (Table #1)."
write ";**********************************************************************"
;;; Change the elements of the table
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

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load 1", "sngl_tbl_ld_1","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6011K) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6011k, "P"
else
  write "<!> Failed (6000;6011K) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6011k, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.4: Validate the default inactive double buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 2 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 2 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.5: Validate the default inactive single buffer table "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 1 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 1 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.6: Activate table #2."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6003) - Table 2 Updated successfully."
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Table 2 update failed."
  Write "<!> Failed - Event Message not received for activate command."
  ut_setrequirements TS_6003, "F"
endif

wait 10
;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed (TS_6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (TS_6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #2."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

;;; Here we should compare the TBL HK value with that in the Table Registry.
;;; If they are equal, then pass the display requirement
if ($SC_$CPU_TBL_LastUpdTime.$SC_$CPU_TBL_SECONDS = $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_SYSTIME.$SC_$CPU_TBL_ST_SECONDS) then
  write "<*> Passed (6011E) - Table HK and Table Registry seconds are the same."
  ut_setrequirements TS_6011e, "P"
else
  write "<!> Failed (6011E) - Table HK and Table Registry seconds are different."
  ut_setrequirements TS_6011e, "F"
endif
                                                                                
if ($SC_$CPU_TBL_LastUpdTime.$SC_$CPU_TBL_SUBSECONDS = $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_SYSTIME.$SC_$CPU_TBL_ST_SUBSECONDS) then
  write "<*> Passed (6011E) - Table HK and Table Registry sub-seconds are the same."
  ut_setrequirements TS_6011e, "P"
else
  write "<!> Failed (6011E) - Table HK and Table Registry sub-seconds are different."
  ut_setrequirements TS_6011e, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.7: Activate table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
endif

wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 10

;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_01") then
  write "<*> Passed (TS_6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (TS_6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_1") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #1."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

;;; Here we should compare the TBL HK value with that in the Table Registry.
;;; If they are equal, then pass the display requirement
if ($SC_$CPU_TBL_LastUpdTime.$SC_$CPU_TBL_SECONDS = $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_SYSTIME.$SC_$CPU_TBL_ST_SECONDS) then
  write "<*> Passed (6011E) - Table HK and Table Registry seconds are the same."
  ut_setrequirements TS_6011e, "P"
else
  write "<!> Failed (6011E) - Table HK and Table Registry seconds are different."
  ut_setrequirements TS_6011e, "F"
endif
                                                                                
if ($SC_$CPU_TBL_LastUpdTime.$SC_$CPU_TBL_SUBSECONDS = $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_SYSTIME.$SC_$CPU_TBL_ST_SUBSECONDS) then
  write "<*> Passed (6011E) - Table HK and Table Registry sub-seconds are the same."
  ut_setrequirements TS_6011e, "P"
else
  write "<!> Failed (6011E) - Table HK and Table Registry sub-seconds are different."
  ut_setrequirements TS_6011e, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.8: Dump the active and inactive buffers for table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (TS_6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (TS_6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains the default active buffer values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain the default active buffer values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (TS_6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (TS_6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed (6000) - Table #2 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.9: Dump the table #1 active buffer. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Load command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Load command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed (6000) - Table #1 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.0: Multiple load test (no activation between loads)"
write ";**********************************************************************"
write "; Step 3.1: Perform 2 full loads to Table #1."
write ";**********************************************************************"
;;; Change the elements of the table
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

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Full Table #1 load 2", "sngl_tbl_ld_2","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

start load_table ("sngl_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6701, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

;;; Load the table a second time
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 5
$SC_$CPU_TST_TBL_TABLE1.element[2] = 3
$SC_$CPU_TST_TBL_TABLE1.element[3] = 1
$SC_$CPU_TST_TBL_TABLE1.element[4] = 2
$SC_$CPU_TST_TBL_TABLE1.element[5] = 4
$SC_$CPU_TST_TBL_TABLE1.element[6] = 7
$SC_$CPU_TST_TBL_TABLE1.element[7] = 6
$SC_$CPU_TST_TBL_TABLE1.element[8] = 9
$SC_$CPU_TST_TBL_TABLE1.element[9] = 10
$SC_$CPU_TST_TBL_TABLE1.element[10] = 9

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Full Table #1 load 3", "sngl_tbl_ld_3","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
;;; Multiple loads to the same table DOES NOT decrement the Free Buffer counter
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

start load_table ("sngl_tbl_ld_3", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is the same as before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6701, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 3.2 : Validate the inactive buffer for Table #1."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Look for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 1 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 1 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.3 : Activate Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_3") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #1."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.4 : Dump and verify Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Load command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Load command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
   ut_setrequirements TS_6001, "P"
else
   write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
   ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 9) then
  write "<*> Passed (6000) - Table #1 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.5 : Perform two full loads to Table #2."
write ";**********************************************************************"
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = (-5)
$SC_$CPU_TST_TBL_TABLE2.element[2] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[3] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[4] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[5] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[6] = 0
$SC_$CPU_TST_TBL_TABLE2.element[7] = 1
$SC_$CPU_TST_TBL_TABLE2.element[8] = 2
$SC_$CPU_TST_TBL_TABLE2.element[9] = 4
$SC_$CPU_TST_TBL_TABLE2.element[10] = 5

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 2", "dbl_tbl_ld_2","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

;;; Perform the second load of Table #2
;;; Change the elements of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[2] = 4
$SC_$CPU_TST_TBL_TABLE2.element[3] = 3
$SC_$CPU_TST_TBL_TABLE2.element[4] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[5] = 1
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[7] = 2
$SC_$CPU_TST_TBL_TABLE2.element[8] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[9] = 1
$SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 3", "dbl_tbl_ld_3","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_3", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.6 : Validate inactive buffer for Table #2."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Look for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 2 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 2 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.7 : Activate Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_3") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #2."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.8: Dump and verify Table #2 Load"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains the values from the active buffer."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain the values from the active buffer."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.0: Validation Error Tests "
write ";**********************************************************************"
write "; Step 4.1: Send an Validate command with an invalid table name."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME="TST_TBL.invalid_tbl_02"

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002) - Validate command with an invalid table name failed as expected."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - Validate command with an invalid table name worked."
  ut_setrequirements TS_6002, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.2: Send an Validate command with an invalid buffer flag."
write ";**********************************************************************"
; Not sure what event this test will generate
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC + 1
rawcmd = ""
;; CPU1 is the default
;;;;;  Buffer flag is     xxxx
rawcmd = "1804c000002B043611115453545F54424C2E64666C745F74626C5F303100000000000000000000000000000000000000000000"

;;if ("$CPU" = "CPU2") then
;;;;;;;  Buffer flag is     xxxx
;;  rawcmd = "1824c000002B043611115453545F54424C2E64666C745F74626C5F303100000000000000000000000000000000000000000000"
;;elseif ("$CPU" = "CPU3") then
;;;;;;;  Buffer flag is     xxxx
;;  rawcmd = "1844c000002B043611115453545F54424C2E64666C745F74626C5F303100000000000000000000000000000000000000000000"
;;endif

ut_sendrawcmd "$SC_$CPU_TBL", (rawcmd)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002) - Validate command with an invalid buffer flag failed as expected."
  ut_setrequirements TS_6002, "P"
else
  write "<!> Failed (6002) - Validate command with an invalid buffer flag worked when failure was expected."
  ut_setrequirements TS_6002, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Validate command with invalid buffer flag."
endif 

wait 5

write ";**********************************************************************"
write "; Step 4.3: Send an Validate command with the table name missing."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "ERROR", 1

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME="""""
if (UT_SC_Status <> UT_SC_Success) then
   write "<*> Passed (6002) - Validate command missing the table name failed as expected."
   if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - Validate command missing the table name worked."
  ut_setrequirements TS_6002, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.4: Load the single buffer table with a load that will fail    "
write "; validation. The default single buffer table expects all values to be "
write "; between 1 and 10. "
write ";**********************************************************************"
;;; The second element is outside the valid value range
$SC_$CPU_TST_TBL_TABLE1.element[1] = 1
$SC_$CPU_TST_TBL_TABLE1.element[2] = 0
$SC_$CPU_TST_TBL_TABLE1.element[3] = 3
$SC_$CPU_TST_TBL_TABLE1.element[4] = 4
$SC_$CPU_TST_TBL_TABLE1.element[5] = 5
$SC_$CPU_TST_TBL_TABLE1.element[6] = 6
$SC_$CPU_TST_TBL_TABLE1.element[7] = 7
$SC_$CPU_TST_TBL_TABLE1.element[8] = 8
$SC_$CPU_TST_TBL_TABLE1.element[9] = 9
$SC_$CPU_TST_TBL_TABLE1.element[10] = 10

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load that fails validation", "sngl_tbl_ld_fail","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1

start load_table ("sngl_tbl_ld_fail", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is one less than before
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6701, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 4.5: Validate the table loaded in the previous step. Verify that"
write "; the validation failed event is generated."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 1 failed validation as expected."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 1 passed validation when failure was expected."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 1) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.6: Load the double buffer table with a load that will fail    "
write "; validation. The default double buffer table expects all values to be "
write "; between -5 and 5. "
write ";**********************************************************************"

;;; The 6th element is outside the valid value range
$SC_$CPU_TST_TBL_TABLE2.element[1] = 0
$SC_$CPU_TST_TBL_TABLE2.element[2] = 1
$SC_$CPU_TST_TBL_TABLE2.element[3] = 2
$SC_$CPU_TST_TBL_TABLE2.element[4] = 3
$SC_$CPU_TST_TBL_TABLE2.element[5] = 4
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-10)
$SC_$CPU_TST_TBL_TABLE2.element[7] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[8] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[9] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Table #2 load that fails validation", "dbl_tbl_ld_fail","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_fail", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 4.7: Validate the table loaded in the previous step. Verify that"
write "; the validation failed event is generated."
write ";**********************************************************************"
; Look for the Validation events
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

;; Look for the error message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 2 failed validation as expected."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 2 passed validation when failure was expected."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = -1) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.8: Activate the two previous loads. The scenario expects these"
write "; to fail. To verify that the Activate fails, the active buffer should "
write "; not change and an error event should be generated."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UNVALIDATED_ERR_EID, ERROR, 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.dflt_tbl_01"
;;ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
else   
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
endif

wait 5

;; If the event message was received, the Activate failed
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003.1;6003.1.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_60031, "P"
  ut_setrequirements TS_600311, "P"
else
  write "<!> Failed (6003.1;6003.1.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_UNVALIDATED_ERR_EID, "."
  ut_setrequirements TS_60031, "F"
  ut_setrequirements TS_600311, "F"
endif

;;; Activate Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UNVALIDATED_ERR_EID, ERROR, 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.dflt_tbl_02"
;;ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
else   
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
endif

wait 5

;; If the event message was received, the Activate failed
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003.1;6003.1.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_60031, "P"
  ut_setrequirements TS_600311, "P"
else
  write "<!> Failed (6003.1;6003.1.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_UNVALIDATED_ERR_EID, "."
  ut_setrequirements TS_60031, "F"
  ut_setrequirements TS_600311, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 4.9: Abort the Loads to the default tables so that the following"
write "; 	   tests do not fail."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Table #1 Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load Abort command."
  endif
else
  write "<!> Failed - Table #1 Load Abort command failed."
endif

;; Abort Table #2
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Table #2 Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load Abort command."
  endif
else
  write "<!> Failed - Table #2 Load Abort command failed."
endif

wait 10
; Dump the table registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Validate the Load in progress tlm point
write "Waiting for LoadInProgress flag to change for Table #1......"
ut_tlmwait $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Load In Progress field indicates there is not a load pending for Table #1."
else
   write "<!> Failed - Load In Progress field indicates a load is pending for Table #1."
endif

write "Waiting for LoadInProgress flag to change for Table #2......"
ut_tlmwait $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Load In Progress field indicates there is not a load pending for Table #2."
else
   write "<!> Failed - Load In Progress field indicates a load is pending for Table #2."
endif

wait 5

write ";**********************************************************************"
write "; Step 5.0: Load Error Tests."
write ";**********************************************************************"
write "; Step 5.1: Send a Load command with a file containing more data than  "
write "; the table can hold. The table size is 40 bytes (10 integers). The    "
write "; File Header specifies 41 bytes of data and contains 41 bytes of data."
write "; NOTE: This file must already exist before executing this proc. The   "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, ERROR, 1

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

;;; Load the file that has more data than needed
start load_table ("too_much_data_load2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6302.1) - File Load generated an error as expected."
  ut_setrequirements TS_63021, "P"
else
  write "<!> Failed (6302.1) - File Load worked when failure was expected."
  ut_setrequirements TS_63021, "F"
endif

if ($SC_$CPU_TBL_CMDPC = cmdcnt) then
  write "<*> Passed (6302.1) - File Load did not increment CMDPC as expected."
  ut_setrequirements TS_63021, "P"
else
  write "<!> Failed (6302.1) - File Load incremented CMDPC."
  ut_setrequirements TS_63021, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6302.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_63021, "P"
else
  write "<!> Failed (6302.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, "."
  ut_setrequirements TS_63021, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 5.2: Send a Load command without a filename specified."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_ACCESS_ERR_EID, ERROR, 1

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_LOAD LFILENAME=""

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command without a filename generated an error." 
  ut_setrequirements TS_6000, "P"
else   
  write "<!> Failed (6000) - Load command worked when it should have failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_ACCESS_ERR_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 5.3: Send a Load command with an invalid tablename contained in "
write "; the load file."
write ";**********************************************************************"
;; Create the file with an invalid table name
s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table load with invalid table name", "invalid_tbl_name_ld","TST_TBL.invalid_table_name", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_SUCH_TABLE_ERR_EID, ERROR, 1

start load_table ("invalid_tbl_name_ld", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDPC = cmdcnt) then
  write "<*> Passed (6000) - Load command failed as expected."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command succeeded when failure was ecpected."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 5.4: Exhaust the number of Free Shared buffers."
write ";**********************************************************************"
write "; Step 5.4.1: Register 4 additional single buffer tables."
write ";**********************************************************************"
;; Use the Send command to do these
for i = 1 to 4 do
  tblcnt = $SC_$CPU_TBL_NUMTABLES+1
  newTableName = "tst_tbl_0"&i

  ut_setupevents "$SC","$CPU","TST_TBL",TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
  ut_setupevents "$SC","$CPU","TST_TBL",TST_TBL_TBLREGISTER_EID, "INFO", 2

  /$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME=newTableName TBLOPTS=X'0' TBLSIZE=X'28'
  wait 10

  ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6011C) - Table counter incremented."
    ut_setrequirements TS_6011c, "P"
  else
    write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
    ut_setrequirements TS_6011c, "P"
  endif

  ;; Check for the Table Create event
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6300) - Created new single buffer table '", newTableName, "'" 
    write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements TS_6300, "P"
  else
    write "<!> Failed (6300) - Table create command for '", newTableName, "'"
    write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_TBL_TBLREGISTERPASS_EID, "."
    ut_setrequirements TS_6300, "F"
  endif

  ;; Check for the Table Register event
  if ($SC_$CPU_find_event[2].num_found_messages = 1) then
    write "<*> Passed (6300) - table '", newTableName, "' has been registered." 
    write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[2].eventid, " received"
    ut_setrequirements TS_6300, "P"
  else
    write "<!> Failed (6300) - Table Register command for '", newTableName, "'"
    write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_TBL_TBLREGISTER_EID, "."
    ut_setrequirements TS_6300, "F"
  endif
enddo

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 5.4.2: Perform the initial Load to each of the 4 tables created "
write "; in the step above. This is needed to load the default table contents."
write ";**********************************************************************"
;;; Change the elements of the table
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

;;;; Create the load files
s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Test Table #1 load", "tst_tbl1_ld_1","TST_TBL.tst_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Test Table #2 load", "tst_tbl2_ld_1","TST_TBL.tst_tbl_02", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Test Table #3 load", "tst_tbl3_ld_1","TST_TBL.tst_tbl_03", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Test Table #4 load", "tst_tbl4_ld_1","TST_TBL.tst_tbl_04", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

;;; Loop for the 4 new tables created in Step 5.4.1
for i = 1 to 4 do
  cmdcnt = $SC_$CPU_TBL_CMDPC+1
  errcnt = $SC_$CPU_TBL_CMDEC
  newTableName = "TST_TBL.tst_tbl_0"&i

  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

  start load_table ("tst_tbl"&i&"_ld_1", hostCPU)

  ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6000) - Load command for ", newTableName, " executed successfully."
    ut_setrequirements TS_6000, "P"
  else
    write "<!> Failed (6000) - Load command for ", newTableName, " did not execute successfully."
    ut_setrequirements TS_6000, "F"
  endif

  ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6000) - Error count incremented."
    ut_setrequirements TS_6000, "P"
  else
    write "<!> Failed (6000) - error count did not increment."
    ut_setrequirements TS_6000, "F"
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements TS_6000, "P"
  else
    write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
    ut_setrequirements TS_6000, "F"
  endif

  wait 5

  ;; Activate this load
  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

  ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=newTableName"

  if (UT_SC_Status <> UT_SC_Success) then
    write "<!> Failed (6003) - Activate command not sent properly."
    ut_setrequirements TS_6003, "F"
  else   
    write "<*> Passed (6003) - Activate command sent properly." 
    ut_setrequirements TS_6003, "P"
  endif

  wait 5

  ;**** If the event was generated, the Activate occurred!!!
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6003;6003.1.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements TS_6003, "P"
    ut_setrequirements TS_600312, "P"
  else
    write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
    ut_setrequirements TS_6003, "F"
    ut_setrequirements TS_600312, "F"
  endif

  wait 10
  ;;;; Dump the Table Registry
  s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
  wait 10

  ;; Get the Table Telemetry Info Packet for this table
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_TLM_REG_CMD_INF_EID, "DEBUG", 1

  ut_sendcmd "$SC_$CPU_TBL_TLMREG RTTABLENAME=newTableName"

  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Passed (6006) - Table Telemeter Registry command sent successfully."
    if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      write "<*> Passed (6006) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      ut_setrequirements TS_6006, "P"
    else
      write "<!> Failed (6006) - Event Message not received for Activate command."
      ut_setrequirements TS_6006, "F"
    endif
  else
    write "<!> Failed (6006) - Table Telemeter Registry command failed."
    ut_setrequirements TS_6006, "F"
  endif

  ;; Write the contents of the tbl tlm info packet
  write "; Table Telemetry Info Packet contains:"
  write ";    Name = ",$SC_$CPU_TBL_NAME
  write ";    Size = ",$SC_$CPU_TBL_SIZE
  write ";    Active Buffer Address = ",$SC_$CPU_TBL_ActBufAdd
  write ";    Inactive Buffer Address = ",$SC_$CPU_TBL_IActBufAdd
  write ";    Validation Fn Pointer = ",$SC_$CPU_TBL_VALFuncPtr
  write ";    Seconds of last update  = ",$SC_$CPU_TBL_TLUSECONDS
  write ";    SubSeconds of last update  = ",$SC_$CPU_TBL_TLUSUBSECONDS
  write ";    Loaded Once Flag = ",$SC_$CPU_TBL_LoadedOnce
  write ";    Load Pending Flag = ",$SC_$CPU_TBL_UpdatePndng
  write ";    Dump-Only Flag = ",$SC_$CPU_TBL_DumpOnly
  write ";    Double Buffer Flag = ",$SC_$CPU_TBL_DblBuffered
  write ";    Last File Loaded = ",$SC_$CPU_TBL_LASTFILEUPD
  write ";    Owner Application Name = ",$SC_$CPU_TBL_OWNERAPP
  write ";    File Create Seconds = ",$SC_$CPU_TBL_FILECSECONDS
  write ";    File Create SubSeconds = ",$SC_$CPU_TBL_FILECSUBSECONDS

  ;; Verify the tbl tlm info packet
  if ($SC_$CPU_TBL_NAME = newTableName AND ;;
      $SC_$CPU_TBL_OWNERAPP = "TST_TBL" AND ;;
      $SC_$CPU_TBL_SIZE = 40 AND $SC_$CPU_TBL_DBLBUFFERED = 0) then
    write "<*> Passed (6300) - Table Telemetry Info Packet contains the correct data."
    ut_setrequirements TS_6300, "P"
  else
    write "<!> Failed (6300) - Table Telemetry info packet does not contain the correct data."
    ut_setrequirements TS_6300, "F"
  endif

  wait 5
enddo

write ";**********************************************************************"
write "; Step 5.4.3: Perform Loads to Table #1 and the 4 tables registered in "
write "; the step above. Verify that an error event is generated when the 5th "
write "; load is performed because there are no available buffers. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_NumFreeshrBuf - 1

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command to default Table #1 sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6701, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6701) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6701, "F"
endif

wait 5

;;; Loop for the 4 new tables created in Step 5.4.1
for i = 1 to 4 do
  newTableName = "TST_TBL.tst_tbl_0"&i

  num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
  write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

  ;; Check the free buffers to determine when a failure should occur
  if (num_free_buffers = (-1)) then
    cmdcnt = $SC_$CPU_TBL_CMDPC
    errcnt = $SC_$CPU_TBL_CMDEC+1
    num_free_buffers = 0
    ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_WORK_BUFFERS_ERR_EID, ERROR, 1
  else
    cmdcnt = $SC_$CPU_TBL_CMDPC+1
    errcnt = $SC_$CPU_TBL_CMDEC
    ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
  endif

  start load_table ("tst_tbl"&i&"_ld_1", hostCPU)

  ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6000) - Load command to Table '",newTableName,"' worked as expected."
    ut_setrequirements TS_6000, "P"
  else
    write "<!> Failed (6000) - Load command did not work as expected."
    ut_setrequirements TS_6000, "F"
  endif

  ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6000) - Error counter contains the expected value."
    ut_setrequirements TS_6000, "P"
  else
    write "<!> Failed (6000) - Error counter does not contain the expected value."
    ut_setrequirements TS_6000, "F"
  endif

  ; Verify that the # of available shared buffers is one less than before
  write "Waiting for free shared buffers to = ", num_free_buffers
  ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
    ut_setrequirements TS_6701, "P"
  else
    write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
    ut_setrequirements TS_6701, "F"
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements TS_6000, "P"
  else
    write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
    ut_setrequirements TS_6000, "F"
  endif

  wait 5
enddo

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 5.5: Send the Load Abort command for the 3 test tables loaded in"
write "; the previous step. "
write ";**********************************************************************"
local tbl_index=0;
for i = 1 to 3 do
  num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf + 1
  newTableName = "TST_TBL.tst_tbl_0"&i

  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

  ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=newTableName"

  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Passed - Table '",newTableName,"' Load Abort command sent."
    if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      ut_setrequirements TS_6012, "P"
    else
      write "<!> Failed - Event Message not received for Load Abort command."
      ut_setrequirements TS_6012, "F"
    endif
  else
    write "<!> Failed - Table '",newTableName, "' Load Abort command failed."
    ut_setrequirements TS_6012, "F"
  endif

  ; Verify that the # of available shared buffers is one less than before
  write "Waiting for free shared buffers to = ", num_free_buffers
  ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6012.1) - Free shared buffers = ", num_free_buffers
    ut_setrequirements TS_60121, "P"
  else
    write "<!> Failed (6012.1) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
    ut_setrequirements TS_60121, "F"
  endif

  wait 10
  ; Dump the table registry
  s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
  wait 10

  ; add validation for the Load in progress tlm point
  write "Waiting for LoadInProgress flag to reset for Table '",newTableName, "' ......"
  tbl_index = 0
  for j = tbl_2_index to $SC_$CPU_TBL_NUMTABLES do
    if ($SC_$CPU_RF[j].$SC_$CPU_TBL_NAME = newTableName) then
        tbl_index = j
    endif
  enddo
  ut_tlmwait $SC_$CPU_RF[tbl_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (6012.3) - Load In Progress field indicates there is not a load pending for Table  '",newTableName, "'."
    ut_setrequirements TS_60123, "P"
  else
    write "<!> Failed (6012.3) - Load In Progress field indicates a load is still pending for Table  '",newTableName, "'."
    ut_setrequirements TS_60123, "F"
  endif
enddo

wait 5

write ";**********************************************************************"
write "; Step 5.6: Send the Validate command for a table that does not have a "
write "; 	   Validation function."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_ASSUMED_VALID_INF_EID, INFO, 1

;; Find a table without a validation function
for i = tbl_2_index to $SC_$CPU_TBL_NUMTABLES do
  if (p@$SC_$CPU_RF[i].$SC_$CPU_TBL_VALFUNCPRESENT = "FALSE") then
    newTableName = $SC_$CPU_RF[i].$SC_$CPU_TBL_NAME
  endif
enddo

write "Sending the Validate command for table '", newTableName, "' since it does not have a validation fn."

cmdcnt = $SC_$CPU_TBL_CMDPC+1

/$SC_$CPU_TBL_VALIDATE ACTIVE VTABLENAME=newTableName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Validate command for table '", newTableName, "'."
else
  write "<!> Failed - Table Validate command for table '", newTableName, "'."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_ASSUMED_VALID_INF_EID, "."
endif

write ";**********************************************************************"
write "; Step 6.0: Load Abort Tests."
write ";**********************************************************************"
write "; Step 6.1: Dump the active and inactive buffers for Tables 1 & 2. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-10) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Active buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Inactive buffer for Table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 10) then
  write "<*> Passed (6000) - Table #1 Inactive buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Inactive buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Active buffer for Table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 9) then
  write "<*> Passed (6000) - Table #1 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 6.2: Validate the inactive buffer for Table #1."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table #1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

;; Wait for the Validation message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table #1 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table #1 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 6.3: Send the Load Abort command for Table #1. Verify that the  "
write "; load was not performed. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Table #1 Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load Abort command."
  endif
else
  write "<!> Failed - Table #1 Load Abort command failed."
endif

wait 10
; Dump the table registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; add validation for the Load in progress tlm point
write "Waiting for LoadInProgress flag to change for Table #1......"
ut_tlmwait $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
write "<*> Passed - Load In Progress field indicates there is not a load pending for Table #1."

wait 5

write ";**********************************************************************"
write "; Step 6.4: Perform a load to Table #2."
write ";**********************************************************************"
;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = 0
$SC_$CPU_TST_TBL_TABLE2.element[2] = 1
$SC_$CPU_TST_TBL_TABLE2.element[3] = 2
$SC_$CPU_TST_TBL_TABLE2.element[4] = 3
$SC_$CPU_TST_TBL_TABLE2.element[5] = 4
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[7] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[8] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[9] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 4", "dbl_tbl_ld_4","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_4", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 6.5: Validate the inactive buffer for Table #2.                 "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table #2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

;; Wait for the Validation message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table #2 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table #2 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <>  $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 6.6: Send the Load Abort command for Table #2. Verify that the  "
write "; load was not performed. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Table #2 Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6012) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6012, "P"
  else
    write "<!> Failed (6012) - Event Message not received for Load Abort command."
    ut_setrequirements TS_6012, "F"
  endif
else
  write "<!> Failed (6012) - Table #2 Load Abort command failed."
  ut_setrequirements TS_6012, "F"
endif

wait 10
; Dump the table registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Validate the Load in progress tlm point
write "Waiting for LoadInProgress flag to change for Table #2......"
ut_tlmwait $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (60123) - Load In Progress field indicates there is not a load pending for Table #2."
  ut_setrequirements TS_60123, "P"
else
  write "<*> Passed (60123) - Load In Progress field indicates a load is pending for Table #2."
  ut_setrequirements TS_60123, "F"
endif

;; Add code to test TS_6012.2 which states that the inactive buffer should be
;; marked as uninitialized
;; Dump the Inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. They should match the load of 6.4
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed - Table #2 Inactive buffer contains the correct values."
else
  write "<!> Failed - Table #2 Inactive buffer does not contain the correct values."
endif

;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[5] = 3
$SC_$CPU_TST_TBL_TABLE2.element[6] = 5

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 2", "dbl_tbl_ld_7","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[5]", "$SC_$CPU_TST_TBL_TABLE2.element[6]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_7", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

;; Dump the Inactive buffer again
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. Since the buffer was marked as 
;;; uninitialized, the contents of the active buffer should appear in locations
;;; not loaded by the partial load (5 & 6). If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6012.2) - Table #2 Inactive buffer contains the expected values."
  ut_setrequirements TS_60122, "P"
else
  write "<!> Failed (60122) - Table #2 Inactive buffer does not contain the expected values."
  ut_setrequirements TS_60122, "F"
endif
wait 5

write ";**********************************************************************"
write "; Step 7.0: Activate Error Tests."
write ";**********************************************************************"
write "; Step 7.1: Send an Activate command with an invalid table name.       "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_SUCH_TABLE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.invalid_name"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed (6003) - Activate command generated an error as expected."
  ut_setrequirements TS_6003, "P"
else   
  write "<!> Failed (6003) - Activate command sent properly when an error was expected." 
  ut_setrequirements TS_6003, "F"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.2: Send an Activate command without a table name.       "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_SUCH_TABLE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName="""""

if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed (6003) - Activate without a table name failed as expected."
  ut_setrequirements TS_6003, "P"
else   
  write "<!> Failed (6003) - Activate command without a table name succeeded." 
  ut_setrequirements TS_6003, "F"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.3: Send two consecutive Activate commands on a single buffer  "
write "; table.       "
write ";**********************************************************************"
write "; Step 7.3.1: Dump the active buffer for Table #1."
write ";**********************************************************************"
;;; Dump the Active buffer for Table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
   ut_setrequirements TS_6001, "P"
else
   write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
   ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 9) then
  write "<*> Passed (6000) - Table #1 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.3.2: Perform a Load to Table #1."
write ";**********************************************************************"
;;; Change the elements of the table
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

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load 4", "sngl_tbl_ld_4","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

start load_table ("sngl_tbl_ld_4", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6701, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 7.3.3: Validate the inactive buffer for Table #1."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation Event to get generated
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 1 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 1 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.3.4: Activate the inactive buffer for Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 10
;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_4") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #1."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.3.5: Dump and verify the active buffer for Table #1."
write ";**********************************************************************"
;;; Dump the Active buffer for Table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
   ut_setrequirements TS_6001, "P"
else
   write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
   ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 1) then
  write "<*> Passed (6000) - Table #1 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.3.6: Activate a second time without performing a load. This   "
write "; should fail."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_ACTIVATE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed (6003) - Activate command failed because a load was not performed."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Activate command succeeded when a load was not performed."
  ut_setrequirements TS_6003, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_ACTIVATE_ERR_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.4: Send two consecutive Activate commands for a double buffer "
write "; table.       "
write ";**********************************************************************"
write "; Step 7.4.1: Dump the active and inactive buffers for Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Active buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.4.2: Perform a Load to Table #2."
write ";**********************************************************************"
;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[2] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[3] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[4] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[5] = (-5)
$SC_$CPU_TST_TBL_TABLE2.element[6] = 0
$SC_$CPU_TST_TBL_TABLE2.element[7] = 1
$SC_$CPU_TST_TBL_TABLE2.element[8] = 2
$SC_$CPU_TST_TBL_TABLE2.element[9] = 3
$SC_$CPU_TST_TBL_TABLE2.element[10] = 4

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 5", "dbl_tbl_ld_5","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_5", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 7.4.3: Validate the inactive buffer for Table #2."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table #2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table #2 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table #2 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.4.4: Activate the inactive buffer for Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6003) - Activate command not sent properly."
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003) - Activate command sent properly." 
  ut_setrequirements TS_6003, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_5") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #2."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.4.5: Dump and verify the active and inactive buffers for      "
write "; Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 4) then
  write "<*> Passed (6000) - Table #2 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.4.6: Activate a second time without performing a load. This   "
write "; should fail."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_ACTIVATE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed (6003) - Activate command failed because a load was not performed."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Activate command succeeded when a load was not performed."
  ut_setrequirements TS_6003, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_ACTIVATE_ERR_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.4.7: Dump and verify the active and inactive buffers for      "
write "; Table #2. Make sure that they did not change because of Step 7.4.6.  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed(6001)  - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 4) then
  write "<*> Passed (6000) - Table #2 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the inactive buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error count did not increment."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains the correct values."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain the correct values."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.5: Activation Tests without validation."
write ";**********************************************************************"
write "; Step 7.5.1: Load single-buffer table (Table #1)."
write ";**********************************************************************"
;;; Change the elements of table #1
$SC_$CPU_TST_TBL_TABLE1.element[1] = 1
$SC_$CPU_TST_TBL_TABLE1.element[2] = 9
$SC_$CPU_TST_TBL_TABLE1.element[3] = 8
$SC_$CPU_TST_TBL_TABLE1.element[4] = 7
$SC_$CPU_TST_TBL_TABLE1.element[5] = 6
$SC_$CPU_TST_TBL_TABLE1.element[6] = 5
$SC_$CPU_TST_TBL_TABLE1.element[7] = 4
$SC_$CPU_TST_TBL_TABLE1.element[8] = 3
$SC_$CPU_TST_TBL_TABLE1.element[9] = 2
$SC_$CPU_TST_TBL_TABLE1.element[10] = 10

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load 5", "sngl_tbl_ld_5","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

start load_table ("sngl_tbl_ld_5", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error count did not increment."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6701) - Free shared buffers = ", num_free_buffers
  ut_setrequirements TS_6701, "P"
else
  write "<!> Failed (6701) - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
  ut_setrequirements TS_6701, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 7.5.2: Activate the inactive buffer for Table #1. This should "
write "; 	     generate an error event indicating the table needs to be"
write "; 	     validated. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UNVALIDATED_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed (6003.1.1) - Activate command sent properly." 
  ut_setrequirements TS_600311, "P"
else   
  write "<!> Failed (6003.1.1) - Activate command not sent properly."
  ut_setrequirements TS_600311, "F"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003.1.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_600311, "P"
else
  write "<!> Failed (6003.1.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_600311, "F"
endif

wait 10
;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 7.5.3: Perform a Load to Table #2."
write ";**********************************************************************"
;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[1] = 0
$SC_$CPU_TST_TBL_TABLE2.element[2] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[3] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[4] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[5] = (-5)
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[7] = 1
$SC_$CPU_TST_TBL_TABLE2.element[8] = 2
$SC_$CPU_TST_TBL_TABLE2.element[9] = 3
$SC_$CPU_TST_TBL_TABLE2.element[10] = 4

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Full Table #2 load 6", "dbl_tbl_ld_6","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_6", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 7.5.4: Activate the inactive buffer for Table #2. This should "
write "; 	     generate an error event indicating the table needs to be"
write "; 	     validated. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UNVALIDATED_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status <> UT_SC_Success) then
  write "<*> Passed (6003.1.1) - Activate command sent properly." 
  ut_setrequirements TS_600311, "P"
else   
  write "<!> Failed (6003.1.1) - Activate command not sent properly."
  ut_setrequirements TS_600311, "F"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003.1.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_600311, "P"
else
  write "<!> Failed (6003.1.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_600311, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 7.6: Send an Activate on a Dump-Only Table. Verify that the "
write "; 	   error event is generated. "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.AppDef_tbl_03"

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate on a Dump-Only table incremented the error count as expected."
else
  write "<!> Failed - Activate on a Dump-Only table did not increment the error count."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID, "."
endif

write ";**********************************************************************"
write "; Step 7.7: Abort the Loads to the default tables so that the following"
write "; 	   tests do not fail."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_01"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Table #1 Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load Abort command."
  endif
else
  write "<!> Failed - Table #1 Load Abort command failed."
endif

;; Abort Table #2
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Table #2 Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load Abort command."
  endif
else
  write "<!> Failed - Table #2 Load Abort command failed."
endif

wait 10
; Dump the table registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Validate the Load in progress tlm point
write "Waiting for LoadInProgress flag to change for Table #1......"
ut_tlmwait $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load In Progress field indicates there is not a load pending for Table #1."
else
  write "<!> Failed - Load In Progress field indicates a load is pending for Table #1."
endif

write "Waiting for LoadInProgress flag to change for Table #2......"
ut_tlmwait $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load In Progress field indicates there is not a load pending for Table #2."
else
  write "<!> Failed - Load In Progress field indicates a load is pending for Table #2."
endif

wait 5

write ";**********************************************************************"
write "; Step 8.0: Dump Error tests."
write ";*********************************************************************"
write "; Step 8.1:  Send a Table Dump command for the inactive buffer of a   "
write "; single buffer table when a load does not exist for that table.      "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID,"ERROR", 1

ut_sendcmd "$SC_$CPU_TBL_DUMP INACTIVE DTABLENAME=""TST_TBL.dflt_tbl_01"" DFILENAME=""/ram/inactive_tbl_1_dmp"""

if (UT_SC_Status = UT_SC_CmdFailure) then
  write "<*> Passed (6001) - Inactive Table #1 dump generated the expected error."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6001) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6001, "P"
  else
    write "<!> Failed (6001) - Expected Error Event not received."
    ut_setrequirements TS_6001, "F"
  endif
else
  write "<!> Failed (6001) - Inactive Table #1 dump succeeded when a failure was expected."
  ut_setrequirements TS_6001, "F"
endif

wait 5

write ";*********************************************************************"
write "; Step 8.2:  Send a Table Dump command with an invalid table name.    "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_DUMP ACTIVE DTABLENAME="""TST_TBL.bad_name""" DFILENAME="""/ram/inactive_tbl_?_dmp"""

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Table dump with an invalid table name generated the expected error."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6001) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6001, "P"
  else
    write "<!> Failed (6001) - Expected Error Event not received."
    ut_setrequirements TS_6001, "F"
  endif
else
  write "<!> Failed (6001) - Table dump with invalid table name succeeded when a failure was expected."
  ut_setrequirements TS_6001, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 9.0: Table Registry Tests."
write ";**********************************************************************"
write "; Step 9.1: Send the Dump Registry command with a different filename."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_WRITE_REG_DUMP_INF_EID,"DEBUG", 1

;;; Refresh the table registry page with this file using get_file_to_cvt
wait 10
s get_file_to_cvt (ramDir, "walts_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU, "TBL_REG")
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6005) - Dump Registry command with a different filename sent."
  write "<*> Passed (6005) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6005) - Dump Registry command with a different filename failed."
  write "<!> Failed (6005) - Expected Event not received."
  ut_setrequirements TS_6005, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 9.2: Send the Table Telemeter Registry command with an invalid  "
write "; table name."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_NO_SUCH_TABLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_TLMREG RTTABLENAME="""TST_TBL.invalid_tbl"""

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6006) - Table Telemeter command with a missing filename generated an error."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6006) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6006, "P"
  else
    write "<!> Failed (6006) - Expected Event Message not received."
    ut_setrequirements TS_6006, "F"
  endif
else
  write "<!> Failed (6006) - Table Telemeter Registry command with a bad table name succeeded when failure was expected"
  ut_setrequirements TS_6006, "F"
endif
  
wait 5

write ";**********************************************************************"
write "; Step 9.3: Send the Dump Table Registry command without a filename. "
write "; 	   Verify that the default file is generated."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_OVERWRITE_REG_DUMP_INF_EID,"DEBUG", 1

ut_sendcmd "$SC_$CPU_TBL_WRITEREG2FILE DUMPFILENAME="""""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6005.1) - Dump Registry command sent."
  ut_setrequirements TS_60051, "P"
else
  write "<!> Failed (6005.1) - Dump Registry command with default filename."
  ut_setrequirements TS_60051, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6005.1) - Expected Event Msg received."
  ut_setrequirements TS_60051, "P"
else
  write "<!> Failed (6005.1) - Expected Event Msg ",CFE_TBL_OVERWRITE_REG_DUMP_INF_EID," not received."
  ut_setrequirements TS_60051, "F"
endif

write ";**********************************************************************"
write "; Step 9.4: Send the WRITEREG2FILE command without a path specification"
write "; 	   for the filename."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_CREATING_DUMP_FILE_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC+1
 
/$SC_$CPU_TBL_WRITEREG2FILE DUMPFILENAME="nopathname"
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Write Registry command failed as expected."
else
  write "<!> Failed (6005.1) - Write Registry command did not increment CMDEC."
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Expected Event message ",$SC_$CPU_evs_eventid," received."
else
  write "<!> Failed - Expected Event Msg ",CFE_TBL_CREATING_DUMP_FILE_ERR_EID," not received."
endif

write ";**********************************************************************"
write "; Step 10.0: Attempt to load a DUMP_ONLY table. This should fail. "
write ";**********************************************************************"
;; Need to create a table file for the DUMP_ONLY table
;; then try to load this file
;;; Change the elements of the table
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

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #3 load 1", "dump_only_tbl3_ld","TST_TBL.AppDef_tbl_03", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID, ERROR, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1
 
start load_table ("dump_only_tbl3_ld", hostCPU)
 
ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (TS_6000.5) - Load command to DUMP_ONLY table failed as expected."
  ut_setrequirements TS_60005, "P"
else
  write "<!> Failed (TS_6000.5) - Load command worked to a DUMP_ONLY table."
  ut_setrequirements TS_60005, "F"
endif
 
;; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000.5) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_60005, "P"
else
  write "<!> Failed (6000.5) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_60005, "F"
endif
 
wait 10
;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10
 
; Verify the LoadinProgress Flag
write "Checking the LoadInProgress flag for table #3......"
ut_tlmwait $SC_$CPU_RF[tbl_1_index+2].$SC_$CPU_TBL_LOADBUFFERID, (-1)
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - LoadInProgress flag indicates there is not a load pending."
else 
  write "<!> Failed - LoadInProgress flag indicates a load is pending on a DUMP_ONLY table."
endif

wait 5

write ";**********************************************************************"
write "; Step 11.0: Table Creation tests."
write ";**********************************************************************"
write "; Step 11.1: Send the Table Create Request without any parameters."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_REGISTER_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_INVALIDNAME_ERR_EID, "ERROR", 2

;; Need to send a raw command for this one
rawcmd = ""
;; CPU1 is the default
rawcmd = "190BC00000310288"

;;if ("$CPU" = "CPU2") then
;;  rawcmd = "1A0BC00000310288"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1B0BC00000310288"
;;endif

;; Set the expected table count (This should not change)
tblcnt = $SC_$CPU_TBL_NUMTABLES

ut_sendrawcmd "$SC_$CPU_TST_TBL", (rawcmd)

;; Check for the Table Create failure events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Table Create command without a buffer parameter failed as expected."
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table Create command without a buffer parameter worked when failure was expected."
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",CFE_TBL_REGISTER_ERR_EID," and ",TST_TBL_INVALIDNAME_ERR_EID,"."
  ut_setrequirements TS_6300, "F"
endif

ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter did not increment."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter incremented. Expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 11.2: Create a new single and a new double buffer table.        "
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES+2

ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tst_tbl_05" TBLOPTS=X'0' TBLSIZE=X'28'
wait 10

;; Check for the Table Create event
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Created new single buffer table 'tst_tbl_05'" 
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table create command for 'tst_tbl_05'"
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",TST_TBL_TBLREGISTERPASS_EID," and ",TST_TBL_TBLREGISTER_EID,"."
  ut_setrequirements TS_6300, "F"
endif

wait 5

;; Create a new Double Buffer Table
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tst_tbl_06" TBLOPTS=X'1' TBLSIZE=X'28'
wait 10

;; Check for the Table Create event
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Created new single buffer table 'tst_tbl_06'" 
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table create command for 'tst_tbl_06'"
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",TST_TBL_TBLREGISTERPASS_EID," and ",TST_TBL_TBLREGISTER_EID,"."
  ut_setrequirements TS_6300, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

; Verify the two new tables are in the registry
for i = tbl_1_index to $SC_$CPU_TBL_NumTables do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_05") then
    write "<*> Passed - Table Registry contains tst_tbl_05."
    ;; Check the DblBuffered flag
    if (p@$SC_$CPU_RF[i].$SC_$CPU_TBL_DBLBUFFERED = "FALSE") then
      write "<*> Passed - Table Registry indicates 'tst_tbl_05' is a single buffer table."
    else
      write "<!> Failed - Table Registry indicates 'tst_tbl_05' is NOT a single buffer table when it should be."
    endif
  endif
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_06") then
    write "<*> Passed - Table Registry contains tst_tbl_06."
    ;; Check the DblBuffered flag
    if (p@$SC_$CPU_RF[i].$SC_$CPU_TBL_DBLBUFFERED = "TRUE") then
      write "<*> Passed - Table Registry indicates 'tst_tbl_06' is a double-buffered table."
    else
      write "<!> Failed - Table Registry indicates 'tst_tbl_06' is NOT a double-buffered table when it should be."
    endif
  endif
enddo

wait 5

write ";**********************************************************************"
write "; Step 11.3: Request Table Registry info for the single buffer table.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_TLM_REG_CMD_INF_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_TBL_TLMREG RTTABLENAME=""TST_TBL.tst_tbl_05"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6006) - Table Telemeter Registry command sent successfully."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6006) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6006, "P"
  else
    write "<!> Failed (6006) - Event Message not received for Activate command."
    ut_setrequirements TS_6006, "F"
  endif
else
  write "<!> Failed (6006) - Table Telemeter Registry command failed."
  ut_setrequirements TS_6006, "F"
endif

;; Write the contents of the tbl tlm info packet
write "; Table Telemetry Info Packet contains:"
  write ";    Name = ",$SC_$CPU_TBL_NAME
  write ";    Size = ",$SC_$CPU_TBL_SIZE
  write ";    Active Buffer Address = ",$SC_$CPU_TBL_ActBufAdd
  write ";    Inactive Buffer Address = ",$SC_$CPU_TBL_IActBufAdd
  write ";    Validation Fn Pointer = ",$SC_$CPU_TBL_VALFuncPtr
  write ";    Seconds of last update  = ",$SC_$CPU_TBL_TLUSECONDS
  write ";    SubSeconds of last update  = ",$SC_$CPU_TBL_TLUSUBSECONDS
  write ";    Loaded Once Flag = ",$SC_$CPU_TBL_LoadedOnce
  write ";    Load Pending Flag = ",$SC_$CPU_TBL_UpdatePndng
  write ";    Dump-Only Flag = ",$SC_$CPU_TBL_DumpOnly
  write ";    Double Buffer Flag = ",$SC_$CPU_TBL_DblBuffered
  write ";    Last File Loaded = ",$SC_$CPU_TBL_LASTFILEUPD
  write ";    Owner Application Name = ",$SC_$CPU_TBL_OWNERAPP

;; Verify the tbl tlm info packet
if ($SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_05" AND ;;
    $SC_$CPU_TBL_OWNERAPP = "TST_TBL" AND ;;
    $SC_$CPU_TBL_SIZE = 40 AND $SC_$CPU_TBL_DBLBUFFERED = 0) then
  write "<*> Passed (6300) - Table Telemetry Info Packet contains the correct data."
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table Telemetry info packet does not contain the correct data."
  ut_setrequirements TS_6300, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 11.4: Request Table Registry info for the double buffer table.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_TLM_REG_CMD_INF_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_TBL_TLMREG RTTABLENAME=""TST_TBL.tst_tbl_06"""

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6006) - Table Telemeter Registry command sent successfully."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6006) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6006, "P"
  else
    write "<!> Failed (6006) - Event Message not received for Activate command."
    ut_setrequirements TS_6006, "F"
  endif
else
  write "<!> Failed (6006) - Table Telemeter Registry command failed."
  ut_setrequirements TS_6006, "F"
endif

;; Write the contents of the tbl tlm info packet
write "; Table Telemetry Info Packet contains:"
write ";    Name = ",$SC_$CPU_TBL_NAME
write ";    Size = ",$SC_$CPU_TBL_SIZE
write ";    Active Buffer Address = ",$SC_$CPU_TBL_ActBufAdd
write ";    Inactive Buffer Address = ",$SC_$CPU_TBL_IActBufAdd
write ";    Validation Fn Pointer = ",$SC_$CPU_TBL_VALFuncPtr
write ";    Seconds of last update  = ",$SC_$CPU_TBL_TLUSECONDS
write ";    SubSeconds of last update  = ",$SC_$CPU_TBL_TLUSUBSECONDS
write ";    Loaded Once Flag = ",$SC_$CPU_TBL_LoadedOnce
write ";    Load Pending Flag = ",$SC_$CPU_TBL_UpdatePndng
write ";    Dump-Only Flag = ",$SC_$CPU_TBL_DumpOnly
write ";    Double Buffer Flag = ",$SC_$CPU_TBL_DblBuffered
write ";    Last File Loaded = ",$SC_$CPU_TBL_LASTFILEUPD
write ";    Owner Application Name = ",$SC_$CPU_TBL_OWNERAPP

;; Verify the tbl tlm info packet
if ($SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_06" AND ;;
    $SC_$CPU_TBL_OWNERAPP = "TST_TBL" AND ;;
    $SC_$CPU_TBL_SIZE = 40 AND $SC_$CPU_TBL_DBLBUFFERED = 1) then
  write "<*> Passed (6300) - Table Telemetry Info Packet contains the correct data."
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table Telemetry info packet does not contain the correct data."
  ut_setrequirements TS_6300, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 11.5: Send the Table Create Request without a table name.       "
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_REGISTER_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_INVALIDNAME_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 3

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="" TBLOPTS=X'0' TBLSIZE=X'28'
wait 5

;; Check for the Table Create failure events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6300) - Table Create without a table name failed as expected."
  write "<*> Passed (6300) - Expected Event messages were received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table Create without a table name worked when failure was expected."
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",CFE_TBL_REGISTER_ERR_EID,",",TST_TBL_TBLREGISTER_EID," and ",TST_TBL_INVALIDNAME_ERR_EID,"."
  ut_setrequirements TS_6300, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 11.6: Send the Table Create Request with a size of zero (0). "
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_REGISTER_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_INVALIDSIZE_ERR_EID, "ERROR", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tst_tbl_10" TBLOPTS=X'0' TBLSIZE=X'0'
wait 5

;; Check for the Table Create failure events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Table Create with a size of zero (0) failed as expected."
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table Create with a size of zero (0) worked when failure was expected."
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",CFE_TBL_REGISTER_ERR_EID," and ",TST_TBL_INVALIDSIZE_ERR_EID,"."
  ut_setrequirements TS_6300, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 12.0: Test the second application commands.                     "
write ";**********************************************************************"
write "; Step 12.1: Send the request to share a single buffer table. The table"
write "; used will be the default single buffer table tst_tbl_01. "
write ";**********************************************************************"
;;; Find the index of 'tst_tbl_01'
LOCAL tst_tbl_01_index = 0
for i = 1 to $SC_$CPU_TBL_NUMTABLES do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_01") then
    tst_tbl_01_index = i
  endif
enddo

write "Found tst_tbl_01 at index #", tst_tbl_01_index
expectedNumUsers = $SC_$CPU_RF[tst_tbl_01_index].$SC_$CPU_TBL_NUMUSERS + 1

ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_TBLSHARE_EID, INFO, 1

/$SC_$CPU_TST_TBL2_ShareTbl STABLENAME="TST_TBL.tst_tbl_01" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6303, "P"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message TST_TBL2_TBLSHARE_EID."
  ut_setrequirements TS_6303, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the Number of Users Flag incremented
if ($SC_$CPU_RF[tst_tbl_01_index].$SC_$CPU_TBL_NUMUSERS = expectedNumUsers) then
  write "<*> Passed - Number of Users incremented."
else
  write "<!> Failed - Number of Users remained the same."
endif

wait 5

write ";**********************************************************************"
write "; Step 12.2: Send the Get Table Address request for the single buffer  "
write "; table shared in the step above. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDRWARNINFO_EID, INFO, 2

/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="TST_TBL.tst_tbl_01" 
wait 5

;;;
;; Add code to test for EID #2
;;;
if ($SC_$CPU_find_event[1].eventid = TST_TBL_GETTBLADDR_EID AND ;;
    $SC_$CPU_find_event[2].eventid = TST_TBL2_GETTBLADDRWARNINFO_EID) then
  write "<*> Passed (6305) - Expected Event Msgs Found!"
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ", TST_TBL2_GETTBLADDR_EID, " and ", TST_TBL2_GETTBLADDRWARNINFO_EID, " were not received."
  ut_setrequirements TS_6305, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 12.3: Send the Get Table Address request without a table name.  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_INVALIDTBLADDR_ERR_EID, INFO, 2

/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="" 
wait 10

;; Check for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6305) - Expected Event Msgs received."
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ", TST_TBL2_GETTBLADDR_EID, " and ", TST_TBL2_GETTBLADDRNONAME_EID, " were not received."
  ut_setrequirements TS_6305, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 12.4: Send the requests to release sharing of table #1.  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDRPASS_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELTBLADDR RATABLENAME="TST_TBL.tst_tbl_01" 
wait 5

;; Check for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6306) - Expected Event Msgs received."
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Expected Event Messages ", TST_TBL2_RELEASETBLADDR_EID, " and ", TST_TBL2_RELEASETBLADDRPASS_EID, " were not received."
  ut_setrequirements TS_6306, "F"
endif

wait 5

;;; Release the share handle for Table #1
expectedNumUsers = $SC_$CPU_RF[tst_tbl_01_index].$SC_$CPU_TBL_NUMUSERS - 1

ut_setupevents $SC,$CPU,TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLE_EID, INFO, 1
ut_setupevents $SC,$CPU,TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLEOK_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELShareTbl RSTABLENAME="TST_TBL.tst_tbl_01" 
wait 5

;; Check for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6304) - Expected Event Msgs received."
  ut_setrequirements TS_6304, "P"
else
  write "<!> Failed (6304) - Expected Event Messages ", TST_TBL2_RELEASETBLSHAREHANDLE_EID, " and ", TST_TBL2_RELEASETBLSHAREHANDLEOK_EID, " were not received."
  ut_setrequirements TS_6304, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the Number of Users Flag incremented
if ($SC_$CPU_RF[tst_tbl_01_index].$SC_$CPU_TBL_NUMUSERS = expectedNumUsers) then
  write "<*> Passed - Number of Users decremented."
else
  write "<!> Failed - Number of Users remained the same."
endif

wait 5

write ";**********************************************************************"
write "; Step 13.0: Perform a Load from the Test Application.                 "
write ";**********************************************************************"
write "; Step 13.1: Create a new single buffer table.                         "
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES+1

ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tst_tbl_07" TBLOPTS=X'0' TBLSIZE=X'28'
wait 10

;; Check for the Table Create event
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Created new single buffer table 'tst_tbl_07'" 
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table create request for 'tst_tbl_07'"
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",TST_TBL_TBLREGISTERPASS_EID," and ",TST_TBL_TBLREGISTER_EID,"."
  ut_setrequirements TS_6300, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 13.2: Create a new double buffer table.                         "
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES+1

;; Create a new Double Buffer Table
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tst_tbl_08" TBLOPTS=X'1' TBLSIZE=X'28'
wait 10

;; Check for the Table Create event
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Created new double buffer table 'tst_tbl_08'" 
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table create request for 'tst_tbl_08'"
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",TST_TBL_TBLREGISTERPASS_EID," and ",TST_TBL_TBLREGISTER_EID,"."
  ut_setrequirements TS_6300, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 13.3: Create another new single buffer table.                   "
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES+1

ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTERPASS_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_TBL", TST_TBL_TBLREGISTER_EID, "INFO", 2

/$SC_$CPU_TST_TBL_TBLREGISTER RTABLENAME="tst_tbl_09" TBLOPTS=X'0' TBLSIZE=X'28'
wait 10

;; Check for the Table Create event
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6300) - Created new single buffer table 'tst_tbl_09'" 
  write "<*> Passed (6300) - Event message ",$SC_$CPU_find_event[1].eventid, " and ",$SC_$CPU_find_event[2].eventid, " received"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table create request for 'tst_tbl_09'"
  write "<!> Failed (6300) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",TST_TBL_TBLREGISTERPASS_EID," and ",TST_TBL_TBLREGISTER_EID,"."
  ut_setrequirements TS_6300, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 13.4: Dump the active buffers of the new tables. Verify that    "
write "; they are zero-filled. "
write ";**********************************************************************"
;;; Dump the Active buffer for each of the newly created tables
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.tst_tbl_07","A","active_tbl_7_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #7 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #7 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

write "Table #7 Active Buffer dumped:"
zeroTableFlag = "true"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
  ;; Check to see if each element is 0
  if ($SC_$CPU_TST_TBL_TABLE1.element[i] <> 0) then
    zeroTableFlag = "false"
  endif 
enddo

;; If the table contained all zeros, then pass the requirement
if (zeroTableFlag = "true") then
  write "<*> Passed (6300) - Table #7 contained all zeros"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table #7 did not contain all zeros"
  ut_setrequirements TS_6300, "P"
endif

wait 5

;; Dump Table #8
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.tst_tbl_08","A","active_tbl_8_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #8 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #8 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

write "Table #8 Active Buffer dumped:"
zeroTableFlag = "true"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
  ;; Check to see if each element is 0
  if ($SC_$CPU_TST_TBL_TABLE2.element[i] <> 0) then
    zeroTableFlag = "false"
  endif 
enddo

;; If the table contained all zeros, then pass the requirement
if (zeroTableFlag = "true") then
  write "<*> Passed (6300) - Table #8 contained all zeros"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table #8 did not contain all zeros"
  ut_setrequirements TS_6300, "P"
endif

wait 5

;; Dump Table #9
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 2

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.tst_tbl_09","A","active_tbl_9_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #9 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #9 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1 OR $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6001) - Dump File Write Event message ",$SC_$CPU_evs_eventid," received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID," or ",CFE_TBL_OVERWRITE_DUMP_INF_EID,"."
  ut_setrequirements TS_6001, "P"
endif

write "Table #9 Active Buffer dumped:"
zeroTableFlag = "true"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
  ;; Check to see if each element is 0
  if ($SC_$CPU_TST_TBL_TABLE1.element[i] <> 0) then
    zeroTableFlag = "false"
  endif 
enddo

;; If the table contained all zeros, then pass the requirement
if (zeroTableFlag = "true") then
  write "<*> Passed (6300) - Table #9 contained all zeros"
  ut_setrequirements TS_6300, "P"
else
  write "<!> Failed (6300) - Table #9 did not contain all zeros"
  ut_setrequirements TS_6300, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 13.5: Send a load request from Test Application #1 with a file  "
write "; that is too large for the table to handle. Verify that an error is   "
write "; generated. "
write "; NOTE: The file loaded has correct header information but more data in"
write "; the file than specified in the header. "
write ";**********************************************************************"
;; Need to ftp this file to the CPU
s ftp_file (ramDir,"too_much_data_load","too_much_data_load",hostCPU,"P")
wait 10

;;; Look for EID 93 from CFE_TBL and 28 & 18 from TST_TBL
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_ERR_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILE_ERR_EID, ERROR, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILE_EID, INFO, 3

;;; Load the file that has more data than needed "too_much_data_load"
/$SC_$CPU_TST_TBL_LoadFile LTABLENAME="dflt_tbl_01" LFILENAME="/ram/too_much_data_load"
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6302.1) - Event messages ",$SC_$CPU_find_event[1].eventid,",", $SC_$CPU_find_event[2].eventid," and ", $SC_$CPU_find_event[3].eventid, " received"
  ut_setrequirements TS_63021, "P"
else
  write "<!> Failed (6302.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",CFE_TBL_LOAD_ERR_EID, ",",TST_TBL_TBLLOADFILE_ERR_EID, " and ",TST_TBL_TBLLOADFILE_EID,"." 
  ut_setrequirements TS_63021, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 13.6: Send a Load request with a file containing more data than  "
write "; the table can hold. The table size is 40 bytes (10 integers). The    "
write "; File Header specifies 41 bytes of data and contains 41 bytes of data."
write ";**********************************************************************"
;; Need to ftp this file to the CPU
s ftp_file (ramDir,"too_much_data_load2","too_much_data_load2",hostCPU,"P")
wait 10

;;; Look for EID 93 from CFE_TBL and 28 & 18 from TST_TBL
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_ERR_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILE_ERR_EID, ERROR, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILE_EID, INFO, 3

;;; Load the file that has more data than needed "too_much_data_load2"
/$SC_$CPU_TST_TBL_LoadFile LTABLENAME="dflt_tbl_01" LFILENAME="/ram/too_much_data_load2"
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6302.1) - Event messages ",$SC_$CPU_find_event[1].eventid,",",$SC_$CPU_find_event[2].eventid," and ", $SC_$CPU_find_event[3].eventid, " received"
  ut_setrequirements TS_63021, "P"
else
  write "<!> Failed (6302.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",CFE_TBL_LOAD_ERR_EID,",", TST_TBL_TBLLOADFILE_ERR_EID, " and ",TST_TBL_TBLLOADFILE_EID,"." 
  ut_setrequirements TS_63021, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 13.7: Send a load request from Test Application #1 with a file  "
write "; that is smaller than the table size."
write ";**********************************************************************"
;;; Change the contents of the table
$SC_$CPU_TST_TBL_TABLE2.element[5] = 3
$SC_$CPU_TST_TBL_TABLE2.element[6] = 5

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 1", "dbl_tbl_ld_6","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[5]", "$SC_$CPU_TST_TBL_TABLE2.element[6]")

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

;; Need to ftp this file to the CPU
s ftp_file (ramDir,"dbl_tbl_ld_6","dbl_tbl_ld_6",hostCPU,"P")
wait 10

;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_SUCCESS_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILEPASS_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILE_EID, INFO, 3

/$SC_$CPU_TST_TBL_LoadFile LTABLENAME="dflt_tbl_02" LFILENAME="/ram/dbl_tbl_ld_6"
wait 5

; Look for the event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6302.1;6308) - Event messages ",$SC_$CPU_find_event[1].eventid,",",$SC_$CPU_find_event[2].eventid," and ", $SC_$CPU_find_event[3].eventid, " received"
  ut_setrequirements TS_63022, "P"
  ut_setrequirements TS_6308, "P"
else
  write "<!> Failed (6302.2;6308) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event messages ",CFE_TBL_LOAD_SUCCESS_INF_EID,",",TST_TBL_TBLLOADFILEPASS_EID," and ", TST_TBL_TBLLOADFILE_EID,"."
  ut_setrequirements TS_63022, "F"
  ut_setrequirements TS_6308, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 13.8: Send valid load requests from Test Application #1 for the "
write "; single and double buffer tables."
write ";**********************************************************************"
;;; Change the elements of the single buffer table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 5
$SC_$CPU_TST_TBL_TABLE1.element[2] = 6
$SC_$CPU_TST_TBL_TABLE1.element[3] = 7
$SC_$CPU_TST_TBL_TABLE1.element[4] = 8
$SC_$CPU_TST_TBL_TABLE1.element[5] = 9
$SC_$CPU_TST_TBL_TABLE1.element[6] = 10
$SC_$CPU_TST_TBL_TABLE1.element[7] = 4
$SC_$CPU_TST_TBL_TABLE1.element[8] = 3
$SC_$CPU_TST_TBL_TABLE1.element[9] = 2
$SC_$CPU_TST_TBL_TABLE1.element[10] = 1

;;; Create the Load files for tst_tbl_07, 8 & 9
s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #7 load 1", "tst_tbl7_ld","TST_TBL.tst_tbl_07", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

;;; Change the contents of the double buffer table
$SC_$CPU_TST_TBL_TABLE2.element[1] = 0
$SC_$CPU_TST_TBL_TABLE2.element[2] = 1
$SC_$CPU_TST_TBL_TABLE2.element[3] = 2
$SC_$CPU_TST_TBL_TABLE2.element[4] = 3
$SC_$CPU_TST_TBL_TABLE2.element[5] = 4
$SC_$CPU_TST_TBL_TABLE2.element[6] = (-1)
$SC_$CPU_TST_TBL_TABLE2.element[7] = (-2)
$SC_$CPU_TST_TBL_TABLE2.element[8] = (-3)
$SC_$CPU_TST_TBL_TABLE2.element[9] = (-4)
$SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Table #8 load 1", "tst_tbl8_ld","TST_TBL.tst_tbl_08", "$SC_$CPU_TST_TBL_TABLE2.element[1]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #9 load 1", "tst_tbl9_ld","TST_TBL.tst_tbl_09", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

;;; Load tables tst_tbl_07, 8 & 9 with appropriate contents
for i = 7 to 9 do
  newTableName = "tst_tbl_0"&i
  newFileName = "tst_tbl"&i&"_ld"

  ;; Need to ftp this file to the CPU
  s ftp_file (ramDir,newFileName,newFileName,hostCPU,"P")
  wait 10

  ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_SUCCESS_INF_EID, DEBUG, 1
  ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_TBLLOADFILE_EID, INFO, 2

  ;; Send the load request
  /$SC_$CPU_TST_TBL_LoadFile LTABLENAME=newTableName LFILENAME="/ram/"&newFileName
  wait 5

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (6302) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements TS_6302, "P"
  else
    write "<!> Failed (6302) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_SUCCESS_INF_EID, "."
    ut_setrequirements TS_6302, "F"
  endif

  wait 5
enddo

wait 5

write ";**********************************************************************"
write "; Step 13.9: Dump the active and inactive buffers of the three tables. "
write "; Verify that they each contain the appropriate contents."
write ";**********************************************************************"
;;; Dump the Active buffer for each of the tables loaded above
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.tst_tbl_07","A","active_tbl_7_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #7 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #7 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #7 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 1) then
  write "<*> Passed (6302) - Table #7 Active buffer contains changes made."
  ut_setrequirements TS_6302, "P"
else
  write "<!> Failed (6302) - Table #7 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6302, "F"
endif

wait 5

;; Dump Table #8
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.tst_tbl_08","A","active_tbl_8_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #8 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #8 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #8 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-5)) then
  write "<*> Passed (6302) - Table #8 Active buffer contains changes loaded."
  ut_setrequirements TS_6302, "P"
else
  write "<!> Failed (6302) - Table #8 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6302, "F"
endif

wait 5

;; Dump Table #9
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.tst_tbl_09","A","active_tbl_9_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #9 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #9 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #9 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 1) then
  write "<*> Passed (6302) - Table #9 Active buffer contains changes made."
  ut_setrequirements TS_6302, "P"
else
  write "<!> Failed (6302) - Table #9 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6302, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.0: Test table sharing between applications.                  "
write ";**********************************************************************"
write "; Step 14.1: Command Test Application #2 to share Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_TBLSHARE_EID, INFO, 1

expectedNumUsers = $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_NUMUSERS + 1

/$SC_$CPU_TST_TBL2_ShareTbl STABLENAME="TST_TBL.dflt_tbl_02" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6303, "P"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_TBL2_TBLSHARE_EID,"."
  ut_setrequirements TS_6303, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the Number of Users Flag incremented
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_NUMUSERS = expectedNumUsers) then
  write "<*> Passed - Number of Users incremented."
else
  write "<!> Failed - Number of Users remained the same."
endif

wait 5

write ";**********************************************************************"
write "; Step 14.2: Command Test Application #2 to get the address of Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDRWARNINFO_EID, INFO, 2

/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="TST_TBL.dflt_tbl_02" 
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6305) - Expected Event Msgs received."
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ",TST_TBL2_GETTBLADDR_EID," and ",TST_TBL2_GETTBLADDRWARNINFO_EID, " not received."
  ut_setrequirements TS_6305, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.3: Load the double buffer table (Table #2).                  "
write ";**********************************************************************"
;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 14.4: Validate the inactive buffer for Table #2.                "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 2 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 2 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.5: Dump the active and inactive buffers of Table #2.         "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed (6000) - Table #2 Inactive buffer contains changes loaded."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 4) then
  write "<*> Passed (6000) - Table #2 Active buffer did not change."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer contains changes."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.6: Send an Activate command for Table #2. This should succeed."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6305.1) - Activate command not sent properly."
  ut_setrequirements TS_63051, "F"
else   
  write "<*> Passed (6305.1) - Activate command sent properly." 
  ut_setrequirements TS_63051, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6305.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_63051, "P"
else
  write "<!> Failed (6305.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_63051, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
ut_tlmwait $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
write "<*> Passed - LoadInProgress flag indicates there is not a load pending."

wait 5

write ";**********************************************************************"
write "; Step 14.7: Dump the active buffer of Table #2 an verify that it has "
write "; the changes in the load. "
write ";**********************************************************************"
;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
	write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed (6000) - Table #2 Active buffer contains changes loaded."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.8: Load the double buffer table (Table #2) a second time.    "
write "; This should fail to get a buffer for the load. "
write ";**********************************************************************"
;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NO_WORK_BUFFERS_ERR_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command generated the expected error."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command worked when failure was expected."
  ut_setrequirements TS_6000, "F"
endif

if ($SC_$CPU_TBL_CMDPC = cmdcnt) then
  write "<*> Passed (6000) - command counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - command counter incremented."
  ut_setrequirements TS_6000, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_NO_WORK_BUFFERS_ERR_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
ut_tlmwait $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
write "<*> Passed - LoadInProgress flag does not indicate a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 14.9: Dump the active and inactive buffers of Table #2.         "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 4) then
  write "<*> Passed (6000) - Table #2 Inactive buffer did not change."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Inactive buffer contains changes."
  ut_setrequirements TS_6000, "F"
endif

wait 5

;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-5) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = 5) then
  write "<*> Passed (6000) - Table #2 Active buffer did not change."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer contains changes."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.10: Command Test Application #2 to Release the address of and"
write "; unshare Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDR_EID, INFO, 1

/$SC_$CPU_TST_TBL2_RELTBLADDR RATABLENAME="TST_TBL.dflt_tbl_02" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6306) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_TBL2_RELEASETBLADDR_EID,"."
  ut_setrequirements TS_6306, "F"
endif

;; Release the Share on Table #2
;;ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLE_EID, INFO, 1
;;
;;/$SC_$CPU_TST_TBL2_RELShareTbl RSTABLENAME="TST_TBL.dflt_tbl_02" 
;;wait 5
;;
;;if ($SC_$CPU_find_event[1].num_found_messages = 2) THEN
;;  write "<*> Passed (6304) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
;;  ut_setrequirements TS_6304, "P"
;;else
;;  write "<!> Failed (6304) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_TBL2_RELEASETBLSHAREHANDLE_EID,"."
;;  ut_setrequirements TS_6304, "F"
;;endif

wait 5

write ";**********************************************************************"
write "; Step 14.11: Command Test App #2 to get the address of Table #2 again."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDRWARNINFO_EID, INFO, 2

/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="TST_TBL.dflt_tbl_02" 
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6305) - Expected Event Msgs received."
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ",TST_TBL2_GETTBLADDR_EID," and ",TST_TBL2_GETTBLADDRWARNINFO_EID," not received."
  ut_setrequirements TS_6305, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.12: Load the double buffer table (Table #2) a third time.    "
write "; This should succeed. "
write ";**********************************************************************"
;;; Need to do a Load before you can activate
;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDR_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELEASETBLADDR_EID, INFO, 3

;; Need to get the table address and then release it in order to load
/$SC_$CPU_TST_TBL_GETTBLADDR GTABLENAME="dflt_tbl_02" 
wait 5

/$SC_$CPU_TST_TBL_RELTBLADDR RLTABLENAME="dflt_tbl_02" 
wait 5

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

start load_table ("dbl_tbl_ld_3", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6306) - Load command sent."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6000;6306) - Load command failed."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6306, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6306) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6000;6306) - error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6306, "F"
endif

; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000;6306) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write ";; Get Address event count = ", $SC_$CPU_find_event[2].num_found_messages
  write ";; Release Address event count = ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6000;6306) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6306, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 14.13: Command Test Application #2 to Release the address of and"
write "; unshare Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDRPASS_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELTBLADDR RATABLENAME="TST_TBL.dflt_tbl_02" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6306) - Expected Event Msgs received."
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Expected Event Messages ",TST_TBL2_RELEASETBLADDR_EID," and ",TST_TBL2_RELEASETBLADDRPASS_EID," not received."
  ut_setrequirements TS_6306, "F"
endif

;; Release the Share on Table #2
ut_setupevents $SC,$CPU,TST_TBL2,TST_TBL2_RELEASETBLSHAREHANDLE_EID, INFO, 1
ut_setupevents $SC,$CPU,TST_TBL2,TST_TBL2_RELEASETBLSHAREHANDLEOK_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELShareTbl RSTABLENAME="TST_TBL.dflt_tbl_02" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6304) - Expected Event Msgs received."
  ut_setrequirements TS_6304, "P"
else
  write "<!> Failed (6304) - Expected Event Messages ",TST_TBL2_RELEASETBLSHAREHANDLE_EID," and ",TST_TBL2_RELEASETBLSHAREHANDLEOK_EID," not received."
  ut_setrequirements TS_6304, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.14: Validate the inactive double buffer table (Table #2)"
write ";**********************************************************************"
;; Set the expected data integrity check value
last_data_integrity_value = 0

ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Active Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - Active Table #2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Look for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table #2 buffer validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table #2 buffer failed validation."
  write "<!> Failed (6002.2) - Event Message not received for validation Success."
  ut_setrequirements TS_60022, "F"
endif

;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1; 6011I) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  ut_setrequirements TS_6011i, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1; 6011I) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
  ut_setrequirements TS_6011i, "F"
endif

;;; Verify the Last Table Name validated
if ($SC_$CPU_TBL_LASTVALTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed (6011H) - Correct Table Identifier reported in TLM."
  ut_setrequirements TS_6011h, "P"
else
  write "<!> Failed (6011H) - Table Identifier reported in TLM does not match table validated."
  ut_setrequirements TS_6011h, "F"
endif

;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.15: Send an Activate command for Table #2. Verify that an     "
write "; event is generated indicating the activate command was successful. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6305.2) - Activate command not sent properly."
  ut_setrequirements TS_63052, "F"
else   
  write "<*> Passed (6305.2) - Activate command sent properly." 
  ut_setrequirements TS_63052, "P"
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6305.2) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_63052, "P"
else
  write "<!> Failed (6305.2) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_63052, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #2......"
ut_tlmwait $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID, (-1)
write "<*> Passed - LoadInProgress flag indicates that no load is pending."

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_3") then
  write "<*> Passed (6003;6005) - The correct file was loaded into Table #2."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_6005, "P"
else
  write "<!> Failed (6003;6005) - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_6005, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.16: Dump the active buffer of Table #2 an verify that it has "
write "; changed to the contents that were loaded in Step 14.12.         "
write ";**********************************************************************"
;;; Dump the Active buffer for Table #2
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #2 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #2 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE2.element[1] = (-4) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[2] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[8] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = (-1)) then
  write "<*> Passed (6000) - Table #2 Active buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Active buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.17: Command Test Application #2 to share Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_TBLSHARE_EID, INFO, 1

expectedNumUsers = $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_NUMUSERS + 1

/$SC_$CPU_TST_TBL2_ShareTbl STABLENAME="TST_TBL.dflt_tbl_01" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6303, "P"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_TBL2_TBLSHARE_EID,"."
  ut_setrequirements TS_6303, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the Number of Users Flag incremented
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_NUMUSERS = expectedNumUsers) then
  write "<*> Passed - Number of Users incremented."
else
  write "<!> Failed - Number of Users remained the same."
endif

wait 5

write ";**********************************************************************"
write "; Step 14.18: Command Test Application #2 to get the address of Table 1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDRWARNINFO_EID, INFO, 2
 
/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="TST_TBL.dflt_tbl_01"
wait 10
 
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6305) - Expected Event Msgs received."
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ",TST_TBL2_GETTBLADDR_EID," and ",TST_TBL2_GETTBLADDRWARNINFO_EID," not received."
  ut_setrequirements TS_6305, "F"
endif
 
wait 5
 
write ";**********************************************************************"
write "; Step 14.19: Load the single buffer table (Table #1).                  "
write ";**********************************************************************"
;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
 
start load_table ("sngl_tbl_ld_2", hostCPU)
 
ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif
 
if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif
 
; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif
 
wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10
 
; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."
 
wait 5
 
write ";**********************************************************************"
write "; Step 14.20: Validate the inactive buffer for Table #1.                "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
 
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif
else
  write "<!> Failed (6002) - InActive Table #1 validation failed."
  ut_setrequirements TS_6002, "F"
endif
 
; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table #1 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table #1 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif
 
;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif
 
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5
 
write ";**********************************************************************"
write "; Step 14.21: Dump the active and inactive buffers of Table #1.         "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
 
s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)
 
ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif
 
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo
 
;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 1) then
  write "<*> Passed (6000) - Table #1 Inactive buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5
 
;;; Dump the Active buffer for Table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
 
s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)
 
ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif
 
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo
 
;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 1) then
  write "<*> Passed (6000) - Table #1 Active buffer did not change."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer contains changes."
  ut_setrequirements TS_6000, "F"
endif

wait 5
 
write ";**********************************************************************"
write "; Step 14.22: Send an Activate command for Table #1. Verify that an    "
write "; event is generated indicating the table is locked. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
 
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6305.1) - Activate command not sent properly."
  ut_setrequirements TS_63051, "F"
else
  write "<*> Passed (6305.1) - Activate command sent properly."
  ut_setrequirements TS_63051, "P"
endif
 
wait 5
 
;; Do we look for event #2????
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6305.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_63051, "P"
else
  write "<!> Failed (6305.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_63051, "F"
endif
 
wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

;; Is there something in the Table Registry to verify here???
;; Need to check the UART log for a table locked message
ut_setrequirements TS_63081, "A"
 
write ";**********************************************************************"
write "; Step 14.23: Command Test Application #2 to Release the address of and"
write "; unshare Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDRPASS_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELTBLADDR RATABLENAME="TST_TBL.dflt_tbl_01" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6306) - Expected Event Msgs received."
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Expected Event Messages ",TST_TBL2_RELEASETBLADDR_EID," and ",TST_TBL2_RELEASETBLADDRPASS_EID," not received."
  ut_setrequirements TS_6306, "F"
endif

;; Release the Share on Table #1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLE_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLEOK_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELShareTbl RSTABLENAME="TST_TBL.dflt_tbl_01" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6304) - Expected Event Msgs received"
  ut_setrequirements TS_6304, "P"
else
  write "<!> Failed (6304) - Expected Event Messages ",TST_TBL2_RELEASETBLSHAREHANDLE_EID," and ",TST_TBL2_RELEASETBLSHAREHANDLEOK_EID," not received."
  ut_setrequirements TS_6304, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 14.24: Load the single buffer table again (Table #1).           "
write ";**********************************************************************"
;; Setup the array to look for all the events that get generated
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
 
start load_table ("sngl_tbl_ld_3", hostCPU)
 
ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command failed."
  ut_setrequirements TS_6000, "F"
endif
 
if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - error counter incremented."
  ut_setrequirements TS_6000, "F"
endif
 
; check for event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif
 
wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10
 
; Verify the LoadinProgress Flag
write "Waiting for LoadInProgress flag to change for table #1......"
wait until ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."
 
wait 5
 
write ";**********************************************************************"
write "; Step 14.25: Validate the inactive buffer for Table #1.                "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
 
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif
else
  write "<!> Failed (6002) - InActive Table #1 validation failed."
  ut_setrequirements TS_6002, "F"
endif
 
; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table #1 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table #1 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif
 
;; Verify the Data Integrity Check Value
write "Data Integrity Check Value = ",$SC_$CPU_TBL_LASTVALCRC 
if (last_data_integrity_value <> $SC_$CPU_TBL_LASTVALCRC) then
  write "<*> Passed (6002.1) - Data Integrity Check Value has changed."
  ut_setrequirements TS_60021, "P"
  last_data_integrity_value = $SC_$CPU_TBL_LASTVALCRC 
else
  write "<!> Failed (6002.1) - Data Integrity Check Value did not change."
  ut_setrequirements TS_60021, "F"
endif
 
;;; Verify the Commanded Table Verification Function Result
if ($SC_$CPU_TBL_LASTVALS = 0) then
  write "<*> Passed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates success."
  ut_setrequirements TS_6011j, "P"
else
  write "<!> Failed (6011J) - Verification Result reported '",$SC_$CPU_TBL_LASTVALS, "' in TLM which indicates failure."
  ut_setrequirements TS_6011j, "F"
endif

wait 5
 
write ";**********************************************************************"
write "; Step 14.26: Dump the active and inactive buffers of Table #1.         "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
 
s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)
 
ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Inactive Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Inactive Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "P"
endif
 
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo
 
;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 1 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 9) then
  write "<*> Passed (6000) - Table #1 Inactive buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

wait 5
 
;;; Dump the Active buffer for Table #1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1
 
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
 
s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)
 
ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001) - Active Table #1 Dump command sent."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Active Table #1 Dump command failed."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - error counter incremented."
  ut_setrequirements TS_6001, "F"
endif
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif
 
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo
 
;;; Verify the contents of the Dump. If they match, pass the requirement
if ($SC_$CPU_TST_TBL_TABLE1.element[1] = 10 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[2] = 9 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[3] = 8 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[9] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[10] = 1) then
  write "<*> Passed (6000) - Table #1 Active buffer did not change."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Active buffer contains changes."
  ut_setrequirements TS_6000, "F"
endif

wait 5
 
write ";**********************************************************************"
write "; Step 14.27: Send an Activate command for Table #1. Verify that this  "
write "; succeeds. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
;; Look for the Update Event (ID=37)
 
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
 
if (UT_SC_Status <> UT_SC_Success) then
  write "<!> Failed (6305.1) - Activate command not sent properly."
  ut_setrequirements TS_63051, "F"
else
  write "<*> Passed (6305.1) - Activate command sent properly."
  ut_setrequirements TS_63051, "P"
endif
 
wait 5
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6305.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_63051, "P"
else
  write "<!> Failed (6305.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_63051, "F"
endif
 
wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10
 
write ";**********************************************************************"
write "; Step 15.0: Free Table Resources tests."
write ";**********************************************************************"
write "; Step 15.1: Send the Table Registry command for the selected tables.  "
write ";**********************************************************************"
for i = 7 to 9 do
  newTableName = "TST_TBL.tst_tbl_0"&i

  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_TLM_REG_CMD_INF_EID, "DEBUG", 1

  ut_sendcmd "$SC_$CPU_TBL_TLMREG RTTABLENAME=newTableName"

  if (UT_SC_Status = UT_SC_Success) then
    write "<*> Passed - Table '", newTableName, "' Telemeter Registry command sent successfully."
    if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      ut_setrequirements TS_6006, "P"
    else
      write "<!> Failed - Event Message not received for Table Telemeter Registry command."
      ut_setrequirements TS_6006, "F"
    endif
  else
    write "<!> Failed - Table '", newTableName, "' Telemeter Registry command failed."
    ut_setrequirements TS_6006, "F"
  endif

  ;; Write the contents of the tbl tlm info packet
  write "; Table Telemetry Info Packet contains:"
  write ";    Name = ",$SC_$CPU_TBL_NAME
  write ";    Size = ",$SC_$CPU_TBL_SIZE
  write ";    Active Buffer Address = ",$SC_$CPU_TBL_ActBufAdd
  write ";    Inactive Buffer Address = ",$SC_$CPU_TBL_IActBufAdd
  write ";    Validation Fn Pointer = ",$SC_$CPU_TBL_VALFuncPtr
  write ";    Seconds of last update  = ",$SC_$CPU_TBL_TLUSECONDS
  write ";    SubSeconds of last update  = ",$SC_$CPU_TBL_TLUSUBSECONDS
  write ";    Loaded Once Flag = ",$SC_$CPU_TBL_LoadedOnce
  write ";    Load Pending Flag = ",$SC_$CPU_TBL_UpdatePndng
  write ";    Dump-Only Flag = ",$SC_$CPU_TBL_DumpOnly
  write ";    Double Buffer Flag = ",$SC_$CPU_TBL_DblBuffered
  write ";    Last File Loaded = ",$SC_$CPU_TBL_LASTFILEUPD
  write ";    Owner Application Name = ",$SC_$CPU_TBL_OWNERAPP

  ;; Verify the tbl tlm info packet
  if ($SC_$CPU_TBL_NAME = newTableName AND ;;
      $SC_$CPU_TBL_OWNERAPP = "TST_TBL" AND ;;
      $SC_$CPU_TBL_SIZE = 40) then
    write "<*> Passed (6300) - Table Telemetry Info Packet contains the correct data."
    ut_setrequirements TS_6300, "P"
  else
    write "<!> Failed (6300) - Table Telemetry info packet does not contain the correct data."
    ut_setrequirements TS_6300, "F"
  endif

  wait 10
enddo

wait 5

write ";**********************************************************************"
write "; Step 15.2: Command Test Application #2 to get the table address of a "
write "; single buffer table. This step uses tst_tbl_07."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_TBLSHARE_EID, INFO, 1

; Find the index of tst_tbl_07 in the registry
for i = tbl_1_index to $SC_$CPU_TBL_NumTables do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_07") then
    tbl7Index = i
  endif
enddo

expectedNumUsers = $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_NUMUSERS + 1

/$SC_$CPU_TST_TBL2_ShareTbl STABLENAME="TST_TBL.tst_tbl_07" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6303, "P"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message TST_TBL2_TBLSHARE_EID."
  ut_setrequirements TS_6303, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the Number of Users Flag incremented
if ($SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_NUMUSERS = expectedNumUsers) then
  write "<*> Passed - Number of Users incremented."
else
  write "<!> Failed - Number of Users remained the same."
endif

wait 5

;;; Get the address of table #7
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDRWARNINFO_EID, INFO, 2

/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="TST_TBL.tst_tbl_07" 
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6305) - Expected Event Msgs received."
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ",TST_TBL2_GETTBLADDR_EID," and ",TST_TBL2_GETTBLADDRWARNINFO_EID," not received."
  ut_setrequirements TS_6305, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 15.3: Command Test Application #2 to get the table address of a "
write "; double buffer table. This test uses tst_tbl_08."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_TBLSHARE_EID, INFO, 1

; Find the index of tst_tbl_08 in the registry
for i = tbl_1_index to $SC_$CPU_TBL_NumTables do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.tst_tbl_08") then
    tbl8Index = i
  endif
enddo

expectedNumUsers = $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_NUMUSERS + 1

/$SC_$CPU_TST_TBL2_ShareTbl STABLENAME="TST_TBL.tst_tbl_08" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6303, "P"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message TST_TBL2_TBLSHARE_EID."
  ut_setrequirements TS_6303, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the Number of Users Flag incremented
if ($SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_NUMUSERS = expectedNumUsers) then
  write "<*> Passed - Number of Users incremented."
else
  write "<!> Failed - Number of Users remained the same."
endif

wait 5

ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_GETTBLADDRWARNINFO_EID, INFO, 2

/$SC_$CPU_TST_TBL2_GETTBLADDR GTABLENAME="TST_TBL.tst_tbl_08" 
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6305) - Expected Event Msgs received."
  ut_setrequirements TS_6305, "P"
else
  write "<!> Failed (6305) - Expected Event Messages ",TST_TBL2_GETTBLADDR_EID," and ",TST_TBL2_GETTBLADDRWARNINFO_EID," not received."
  ut_setrequirements TS_6305, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 15.4: Send a request from Test Application #1 to unregister the "
write "; tables used in Step 15.2 & 15.3."
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES-3

ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGSFREETBL_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_EID, INFO, 2

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="tst_tbl_07" 
wait 10

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="tst_tbl_08" 
wait 10

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="tst_tbl_09" 
wait 10

;;;; Should have 3 messages found for each event
if ($SC_$CPU_find_event[1].num_found_messages = 3 AND $SC_$CPU_find_event[2].num_found_messages = 3) then
  write "<*> Passed - Expected Event Messages ",$SC_$CPU_find_event[1].eventid, " and ", $SC_$CPU_find_event[2].eventid, " received."
else
  write "<!> Failed - Expected Event Messages ",TST_TBL_UNREGSFREETBL_EID," and ",TST_TBL_UNREGISTERTBL_EID," not received."
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 15.5: Dump the Table Registry and verify that the 2 tables still"
write "; exist. NOTE: Currently there is no way to do this. There is a DCR    "
write "; written to provide some type of notification for this situation.     "
write ";**********************************************************************"
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Verify the two new tables are in the registry
if ($SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_NAME = "" AND ;;
    $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_LOADBUFFERID = -1 AND ;;
    $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_LOADEDONCE = 1 AND ;;
    $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_SIZE = 40 AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_NAME = "" AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_LOADBUFFERID = -1 AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_LOADEDONCE = 1 AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_SIZE = 40) then
  write "<*> Passed - Table Registry still contains the unregistered tables."
endif

wait 5

write ";**********************************************************************"
write "; Step 15.6: Command Test Application #2 to release the table addresses"
write "; and shares for the selected tables."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDRPASS_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELTBLADDR RATABLENAME="TST_TBL.tst_tbl_07" 
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6306) - Expected Event Msgs received."
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Expected Event Messages ",TST_TBL2_RELEASETBLADDR_EID," and ",TST_TBL2_RELEASETBLADDRPASS_EID," not received."
  ut_setrequirements TS_6306, "F"
endif

wait 5

ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLADDRPASS_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELTBLADDR RATABLENAME="TST_TBL.tst_tbl_08" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6306) - Expected Event Msgs received."
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Expected Event Messages ",TST_TBL2_RELEASETBLADDR_EID," and ",TST_TBL2_RELEASETBLADDRPASS_EID," not received."
  ut_setrequirements TS_6306, "F"
endif

;; Release the Share on Table #7
ut_setupevents $SC,$CPU,TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLE_EID, INFO, 1
ut_setupevents $SC,$CPU,TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLEOK_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELShareTbl RSTABLENAME="TST_TBL.tst_tbl_07" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6304) - Expected Event Msgs received."
  ut_setrequirements TS_6304, "P"
else
  write "<!> Failed (6304) - Expected Event Messages ",TST_TBL2_RELEASETBLSHAREHANDLE_EID," and ",TST_TBL2_RELEASETBLSHAREHANDLEOK_EID," not received."
  ut_setrequirements TS_6304, "F"
endif

;; Release the Share on Table #8
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLE_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL2, TST_TBL2_RELEASETBLSHAREHANDLEOK_EID, INFO, 2

/$SC_$CPU_TST_TBL2_RELShareTbl RSTABLENAME="TST_TBL.tst_tbl_08" 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6304) - Expected Event Msgs received."
  ut_setrequirements TS_6304, "P"
else
  write "<!> Failed (6304) - Expected Event Messages ",TST_TBL2_RELEASETBLSHAREHANDLE_EID," and ",TST_TBL2_RELEASETBLSHAREHANDLEOK_EID," not received."
  ut_setrequirements TS_6304, "F"
endif

wait 10

write ";**********************************************************************"
write "; Step 15.7: Dump the Table Registry and verify that the 2 tables were "
write "; unregistered."
write ";**********************************************************************"
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

if ($SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_NAME = "" AND ;;
    $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_LOADBUFFERID = 0 AND ;;
    $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_LOADEDONCE = 0 AND ;;
    $SC_$CPU_RF[tbl7Index].$SC_$CPU_TBL_SIZE = 0 AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_NAME = "" AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_LOADBUFFERID = 0 AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_LOADEDONCE = 0 AND ;;
    $SC_$CPU_RF[tbl8Index].$SC_$CPU_TBL_SIZE = 0) then
  write "<*> Passed - Table Registry does not contain 'tst_tbl_07' or 'tst_tbl_08'."
else
  write "<!> Failed - Table Registry still contains 'tst_tbl_07' and/or 'tst_tbl_08'."
endif

wait 5

write ";**********************************************************************"
write "; Step 15.8: Send the unregister table request with an invalid table   "
write "; name. EID 11 and 19 are expected from TST_TBL app."
write ";**********************************************************************"
;; Set the variable for the expected table count
tblcnt = $SC_$CPU_TBL_NUMTABLES

ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_ERR_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_EID, INFO, 2

/$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME="invalid_tbl_name" 
wait 10

;; Check for the events set above
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6306) - Expected Event Msgs received"
  ut_setrequirements TS_6306, "P"
else
  write "<!> Failed (6306) - Expected Event messages ",TST_TBL_UNREGISTERTBL_ERR_EID, " and ",TST_TBL_UNREGISTERTBL_EID," not received."
  ut_setrequirements TS_6306, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tblcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - Table counter = ", tblcnt, " as expected."
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected '", tblcnt, "'; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "P"
endif

wait 5

write ";**********************************************************************"
write "; Step 15.9: Send the Request to delete the TST_TBL application. In "
write "; order to do this, the TST_ES application needs to be started."
write ";**********************************************************************"
;; Start the TST_ES app
write "; Starting the TST_ES application. "
s load_start_app ("TST_ES", hostCPU)
wait 10

;; Add an Event Filter for the TST_ES HK Request Event in order to
;; only receive this event 1 time rather than every 4 seconds
/$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'

;  Dump ES apps
start get_file_to_cvt (ramDir, "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", hostCPU)
                                                                                
local tst_tblIndex = 0, app_info_file_index 
;Loop thru the table looking for the TST_TBL apps (TST_TBL and TST_TBL2)
for app_info_file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_TBL") then
    tst_tblIndex = app_info_file_index 
  endif
enddo

/$SC_$CPU_TST_ES_DELETEAPP APPID=$SC_$CPU_ES_ALE[tst_tblIndex].ES_AL_APPID
wait 10

;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_NAME = "" AND ;;
    $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADBUFFERID = 0 AND ;;
    $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LOADEDONCE = 0 AND ;;
    $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_SIZE = 0 AND ;;
    $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_NAME = "" AND ;;
    $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID = 0 AND ;;
    $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADEDONCE = 0 AND ;;
    $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_SIZE = 0) then
  write "<*> Passed (6301) - Table Registry does not contain 'dflt_tbl_01' or 'dflt_tbl_02'."
  ut_setrequirements TS_6301, "P"
else
  write "<!> Failed (6301)- Table Registry stil contains 'tst_tbl_07' and/or 'tst_tbl_08'."
  ut_setrequirements TS_6301, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 15.10: Send the Commands to delete the TST_TBL2 and TST_ES "
write "; applications. The TST_TBL & TST_TBL2 applications need to be restarted"
write "; in order to complete this test."
write ";**********************************************************************"
cmdcnt = $SC_$CPU_ES_CMDPC + 1

; Delete the TST_ES application
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_ES"
wait 10
                                                                                
ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - DeleteApp command for TST_ES executed properly."
else
  write "<!> Failed - DeleteApp command for TST_ES failed."
endif
cmdcnt = $SC_$CPU_ES_CMDPC + 1

; Delete the TST_TBL2 application
/$SC_$CPU_ES_DELETEAPP APPLICATION="TST_TBL2"
wait 10
                                                                                
ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - DeleteApp command for TST_TBL2 executed properly."
else
  write "<!> Failed - DeleteApp command for TST_TBL2 failed."
endif

write ";*********************************************************************"
write "; Step 15.11: Load the Test Table Applications. "
write ";*********************************************************************"
if ("$CPU" = "") then
  start tst_tbl_apps_start ("15.11")
else
  start $sc_$cpu_tst_tbl_apps_start ("15.11")
endif

write ";**********************************************************************"
write "; Step 16.0: Register for the Maximum number of tables."
write ";**********************************************************************"
write "; Step 16.1: Restart the TST_TBL application "
write ";**********************************************************************"
write "; Skipped this step because the next step will register the max number"
write "; of tables regardless of how many are actually registered. "
;;write ";**********************************************************************"
;;write "; Step 16.1: UnRegister all the non-default tables registered by the   "
;;write "; TST_TBL application."
;;write ";**********************************************************************"
;;numTables = $SC_$CPU_TBL_NUMTABLES
;;
;;if (numTables > tbl_2_index) then
;;  j = 0
;;  for i = tbl_2_index+3 to numTables do
;;    j = j + 1
;;    newTableName = "tst_tbl_0"&j
;;    ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGSFREETBL_EID, INFO, 1
;;    ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_UNREGISTERTBL_EID, INFO, 2
;;
;;    /$SC_$CPU_TST_TBL_UNREGTBL UTABLENAME=newTableName
;;    wait 10
;;
;;    if ($SC_$CPU_find_event[1].num_found_messages = 1 AND $SC_$CPU_find_event[2].num_found_messages = 1) then
;;      write "<*> Passed (6306) - Expected Event Msgs received."
;;      ut_setrequirements TS_6306, "P"
;;    else
;;      write "<!> Failed (6306) - Expected Event messages ",TST_TBL_UNREGSFREETBL_EID, " and ",TST_TBL_UNREGISTERTBL_EID," not received."
;;      ut_setrequirements TS_6306, "F"
;;    endif
;;
;;    wait 5
;;  enddo
;;endif

;;; Check the number of tables counter
;;ut_tlmwait $SC_$CPU_TBL_NUMTABLES, {tbl_2_index+2}
;;if (UT_TW_Status = UT_Success) then
;;  write "<*> Passed (6011C) - Table counter = ",$SC_$CPU_TBL_NUMTABLES," as expected."
;;  ut_setrequirements TS_6011c, "P"
;;else
;;  write "<!> Failed (6011C) - Table counter expected 4; rcv'd '", $SC_$CPU_TBL_NUMTABLES,"'."
;;  ut_setrequirements TS_6011c, "F"
;;endif

;;wait 10
;;;; Dump the Table Registry
;;s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
;;wait 10

write ";**********************************************************************"
write "; Step 16.2: Register the maximum number of tables (128). An error event"
write "; should get generated on the 129th and 130th tables."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_REGMAXTABLES_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_REGISTER_ERR_EID, ERROR, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_REGMAXTABLES_ERR_EID, ERROR, 3

/$SC_$CPU_TST_TBL_REGMAXTBLS
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6700) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  write "<*> Passed (6700) - Event message ",$SC_$CPU_find_event[2].eventid, " received ",$SC_$CPU_find_event[2].num_found_messages, " times."
  write "<*> Passed (6700) - Event message ",$SC_$CPU_find_event[3].eventid, " received ",$SC_$CPU_find_event[3].num_found_messages, " times."
  ut_setrequirements TS_6700, "P"
else
    write "<!> Failed (6700) - The proper Event messages were not received (42;90;7)."
    ut_setrequirements TS_6700, "F"
endif

;;; Check the number of tables counter
ut_tlmwait $SC_$CPU_TBL_NUMTABLES, CFE_TBL_MAX_NUM_TABLES
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6011C) - As expected, the table counter = ",CFE_TBL_MAX_NUM_TABLES
  ut_setrequirements TS_6011c, "P"
else
  write "<!> Failed (6011C) - Table counter expected ",CFE_TBL_MAX_NUM_TABLES,"; telemetry reports '", $SC_$CPU_TBL_NUMTABLES,"'."
  ut_setrequirements TS_6011c, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

write ";**********************************************************************"
write "; Step 17.0: Table Info and Get Status Request Tests"
write ";**********************************************************************"
write "; Step 17.1: Send the Table Info Request for Table #1."
write ";**********************************************************************"
local stream1
;; Send the request for the info packet
;; CPU1 is the default
stream1 = x'906'

;;if ("$CPU" = "CPU2") then
;;   stream1 = x'A06'
;;elseif ("$CPU" = "CPU3") then
;;   stream1 = x'B06'
;;endif
                                                                                
/$SC_$CPU_TO_ADDPACKET STREAM=stream1 PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 10

;; setup for events to be generated
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLINFO_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLINFOPASS_EID, INFO, 2

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_GETINFO TABLENAME="TST_TBL.dflt_tbl_01"

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6309) - Get Info request."
  ut_setrequirements TS_6309, "P"
else
  write "<!> Failed (6309) - Get Info request."
  ut_setrequirements TS_6309, "F"
endif

; Look for the expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6309) - Expected Event Msgs received."
  ut_setrequirements TS_6309, "P"
else
  write "<!> Failed (6309) - Expected Event Messages ",TST_TBL_GETTBLINFO_EID," and ",TST_TBL_GETTBLINFOPASS_EID," not received."
  ut_setrequirements TS_6309, "F"
endif

write ";**********************************************************************"
write "; Step 17.2: Send the Table Status Request for Table #1. This should "
write "; return an event indicating there is nothing pending."
write ";**********************************************************************"
;; setup for events to be generated
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUS_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUSPASS_EID, INFO, 2

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_GETSTATUS TABLENAME="dflt_tbl_01"

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6310) - Get Status request."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Get Status request."
  ut_setrequirements TS_6310, "F"
endif

; Look for the expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6310) - Expected Event Msgs received."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Expected Event Messages ",TST_TBL_GETSTATUS_EID," and ",TST_TBL_GETSTATUSPASS_EID," not received."
  ut_setrequirements TS_6310, "F"
endif

write ";**********************************************************************"
write "; Step 17.3: Send the command to dump the Dump-Only Table and "
write "; immediately send the GetStatus request. This should return an event "
write "; indicating there is a dump pending."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUS_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUSDUMPPEND_EID, INFO, 2

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

;; Send the dump command 
/$SC_$CPU_TBL_DUMP ACTIVE DTABLENAME="TST_TBL.AppDef_tbl_03" DFILENAME="/ram/tbl_3_dmp"

; Send the GetStatus command
/$SC_$CPU_TST_TBL_GETSTATUS TABLENAME="AppDef_tbl_03"

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6310) - Get Status request."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Get Status request."
  ut_setrequirements TS_6310, "F"
endif

; Look for the expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6310) - Expected Event Msgs received."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Expected Event Messages ",TST_TBL_GETSTATUS_EID," and ",TST_TBL_GETSTATUSDUMPPEND_EID," not received."
  ut_setrequirements TS_6310, "F"
endif

write ";**********************************************************************"
write "; Step 17.4: Send the command to validate Table #1's active buffer and "
write "; immediately send the GetStatus request. This should return an event "
write "; indicating there is a validation pending."
write ";**********************************************************************"
;;; Validate the Active buffer of Table #1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUS_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUSVALIDATIONPEND_EID, INFO, 2

/$SC_$CPU_TBL_VALIDATE ACTIVE VTABLENAME="TST_TBL.dflt_tbl_01"

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_GETSTATUS TABLENAME="dflt_tbl_01"

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6310) - Get Status request."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Get Status request."
  ut_setrequirements TS_6310, "F"
endif

; Look for the expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6310) - Expected Event Msgs received."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Expected Event Messages ",TST_TBL_GETSTATUS_EID," and ",TST_TBL_GETSTATUSVALIDATIONPEND_EID," not received."
  ut_setrequirements TS_6310, "F"
endif

write ";**********************************************************************"
write "; Step 17.5: Send the command to load, validate & activate Table #1 and"
write "; immediately send the GetStatus request. This should return an event "
write "; indicating there is an update pending."
write ";**********************************************************************"
;; Load Table #1
cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

;; Validate Table #1
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME="TST_TBL.dflt_tbl_01"
wait 20

ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUS_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETSTATUSUPDATEPEND_EID, INFO, 2

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

;; Activate Table #1
/$SC_$CPU_TBL_ACTIVATE ATableName="TST_TBL.dflt_tbl_01"

/$SC_$CPU_TST_TBL_GETSTATUS TABLENAME="dflt_tbl_01"

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6310) - Get Status request."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Get Status request."
  ut_setrequirements TS_6310, "F"
endif

; Look for the expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6310) - Expected Event Msgs received."
  ut_setrequirements TS_6310, "P"
else
  write "<!> Failed (6310) - Expected Event Messages ",TST_TBL_GETSTATUS_EID," and ",TST_TBL_GETSTATUSVALIDATIONPEND_EID," not received."
  ut_setrequirements TS_6310, "F"
endif

write ";**********************************************************************"
write "; Step 17.6: Send the command to Telemeter the Critical Table. Verify "
write "; that the critical flag indicates TRUE."
write ";**********************************************************************"
;; Get the Table Telemetry Info Packet for this table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_TLM_REG_CMD_INF_EID, "DEBUG", 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1

/$SC_$CPU_TBL_TLMREG RTTABLENAME="TST_TBL.CriticalTbl_04"
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TLMREG Command for the Critical Table."
else
  write "<!> Failed - TLMREG Command for the Critical table."
endif

; Look for the expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Expected Event Msg received."
else
  write "<!> Failed - Expected Event Message ",CFE_TBL_TLM_REG_CMD_INF_EID," not received."
endif

;; Check the Critical Flag TLM
if (p@$SC_$CPU_TBL_CRITFLAG = "TRUE") then
  write "<*> Passed - Critical Flag tlm is correct."
else
  write "<!> Failed - Critical Flag tlm does not indicate this table is a Critical Table."
endif

; Verify the Critical table flag in the registry
for i = tbl_1_index to $SC_$CPU_TBL_NumTables do
  if ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.CriticalTbl_04") then
    ;; Check the Critical flag
    if (p@$SC_$CPU_RF[i].$SC_$CPU_TBL_CRITICAL = "TRUE") then
      write "<*> Passed - Table Registry indicates a critical table."
    else
      write "<!> Failed - Table Registry indicates 'CriticalTbl_04' is NOT a crtical table when it should be."
    endif
  endif
enddo

write ";**********************************************************************"
write "; Step 18.0: Multiple Table Address Tests"
write ";**********************************************************************"
write "; Step 18.1: Get the addresses of the 2 default tables."
write ";**********************************************************************"
;; Setup for the expected events
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETMULTITBLADDR_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDRSPASS_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDRSUPDATE_EID, INFO, 3

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_GETADDRS NUMTABLES=2 TABLENAME1="dflt_tbl_01" TABLENAME2="dflt_tbl_02" TABLENAME3="" TABLENAME4="" TABLENAME5=""

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6311) - Get Address request."
  ut_setrequirements TS_6311, "P"
else
  write "<!> Failed (6311) - Get Address request."
  ut_setrequirements TS_6311, "F"
endif

;; Look for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
   ($SC_$CPU_find_event[2].num_found_messages = 1 OR ;;
   $SC_$CPU_find_event[3].num_found_messages = 1)) then
  write "<*> Passed (6311) - Expected event Msgs received."
  ut_setrequirements TS_6311, "P"
else 
  write "<!> Failed (6311) - Did not receive the expected events."
  ut_setrequirements TS_6311, "F"
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6311.2) - Table Update Event received."
  ut_setrequirements TS_63112, "P"
endif

write ";**********************************************************************"
write "; Step 18.2: Load, Validate and Activate each of the default tables.   "
write "; For Table #1, the Activate succeeds. However, the Update does not    "
write "; occur until the table is released. The locked message appears in the "
write "; uart window. "
write "; For Table #2, the first Activate succeeds. However, the subsequent   "
write "; load will generate an error event indicating there are no working    "
write "; buffers available for the load."
write ";**********************************************************************"
;; Load Table #1
cmdcnt = $SC_$CPU_TBL_CMDPC+1

num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Table #1 Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

;; Validate the Inactive buffer of Table #1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
wait 20

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (6002.2) - Inactive Table 1 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 1 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Activate Table #1
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
wait 10

ut_setrequirements TS_63111, "A"

;; Load Table #2
cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Table #2 Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

;; Validate the Inactive buffer of Table #2
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 2 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 2 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Activate the Inactive buffer of Table #2
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
wait 10

errcnt = $SC_$CPU_TBL_CMDEC+1

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6311.1) - Table #2 Load command failed as expected."
  ut_setrequirements TS_63111, "P"
else
  write "<!> Failed (6311.1) - Table #2 Load command succeeded when failure was expected."
  ut_setrequirements TS_63111, "F"
endif

write ";**********************************************************************"
write "; Step 18.3: Release the addresses of the 2 default tables."
write ";**********************************************************************"
;; Setup for the expected events
ut_setupevents $SC,$CPU,TST_TBL,TST_TBL_RELEASEMULTITBLADDR_EID, INFO, 1
ut_setupevents $SC,$CPU,TST_TBL,TST_TBL_RELEASETBLADDRSPASS_EID, INFO, 2
ut_setupevents $SC,$CPU,TST_TBL,TST_TBL_RELEASETBLADDRSINFOUPDATE_EID, INFO, 3

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_RELADDRS NUMTABLES=2 TABLENAME1="dflt_tbl_01" TABLENAME2="dflt_tbl_02" TABLENAME3="" TABLENAME4="" TABLENAME5=""

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6312) - Release Address request."
  ut_setrequirements TS_6312, "P"
else
  write "<!> Failed (6312) - Release Address request."
  ut_setrequirements TS_6312, "F"
endif

;; Look for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
   ($SC_$CPU_find_event[2].num_found_messages = 1 OR ;;
   $SC_$CPU_find_event[3].num_found_messages = 1)) then
  write "<*> Passed (6312) - Expected event Msgs received."
  ut_setrequirements TS_6312, "P"
else 
  write "<!> Failed (6312) - Did not receive the expected events."
  ut_setrequirements TS_6312, "F"
endif

write ";**********************************************************************"
write "; Step 18.4: Get the addresses of the first 4 tables."
write ";**********************************************************************"
;; Setup for the expected events
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETMULTITBLADDRANGE_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDRSRANGEPASS_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_GETTBLADDRSRANGEUPDATE_EID, INFO, 3

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_GETADDRS2 NUMTABLES=4 STARTINDEX=0

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6311) - Get Address request #2."
  ut_setrequirements TS_6311, "P"
else
  write "<!> Failed (6311) - Get Address request #2."
  ut_setrequirements TS_6311, "F"
endif

;; Look for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
   ($SC_$CPU_find_event[2].num_found_messages = 4 OR ;;
   $SC_$CPU_find_event[3].num_found_messages = 1)) then
  write "<*> Passed (6311) - Expected event Msgs received."
  ut_setrequirements TS_6311, "P"
else 
  write "<!> Failed (6311) - Did not receive the expected events."
  ut_setrequirements TS_6311, "F"
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (6311.2) - Table Update Event received."
  ut_setrequirements TS_63112, "P"
endif

write ";**********************************************************************"
write "; Step 18.5: Load, Validate and Activate each of the default tables.   "
write ";            The Activate should fail since the contents of the tables "
write ";            should be locked. "
write ";**********************************************************************"
;; Load Table #1
cmdcnt = $SC_$CPU_TBL_CMDPC+1

num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Table #1 Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #1 Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

;; Validate the Inactive buffer of Table #1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed (6002) - InActive Table 1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    Write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 1 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 1 validated successfully."
  Write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 1 validation failed."
  Write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Activate Table #1
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
wait 10

ut_setrequirements TS_63111, "A"

;; Load Table #2
cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Table #2 Load command sent successfully."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Table #2 Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
endif

;; Validate the Inactive buffer of Table #2
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - InActive Table 2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6002) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6002, "P"
  else
    write "<!> Failed (6002) - Event Message not received for Validate command."
    ut_setrequirements TS_6002, "F"
  endif 
else
  write "<!> Failed (6002) - InActive Table 2 validation failed."
  ut_setrequirements TS_6002, "F"
endif

; Wait for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6002.2) - Inactive Table 2 validated successfully."
  write "<*> Passed (6002.2) - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
  ut_setrequirements TS_60022, "P"
else
  write "<!> Failed (6002.2) - Inactive Table 2 validation failed."
  write "<!> Failed (6002.2) - Event Message not received for validation command."
  ut_setrequirements TS_60022, "F"
endif

;; Activate the Inactive buffer of Table #2
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
wait 10

;; Load Table #2
errcnt = $SC_$CPU_TBL_CMDEC+1

start load_table ("dbl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6311.1) - Second double-buffered Load command failed as expected."
  ut_setrequirements TS_63111, "P"
else
  write "<!> Failed (6311.1) - Second double-buffered Load command succeeded when failure was expected."
  ut_setrequirements TS_63111, "F"
endif

write ";**********************************************************************"
write "; Step 18.6: Release the addresses of the first 4 tables."
write ";**********************************************************************"
;; Setup for the expected events
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELMULTITBLADDRANGE_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELTBLADDRSRANGEPASS_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_TBL, TST_TBL_RELTBLADDRSRANGEUPDATE_EID, INFO, 3

cmdcnt = $SC_$CPU_TST_TBL_CMDPC+1

/$SC_$CPU_TST_TBL_RELADDRS2 NUMTABLES=4 STARTINDEX=0

ut_tlmwait $SC_$CPU_TST_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6312) - Release Address request #2."
  ut_setrequirements TS_6312, "P"
else
  write "<!> Failed (6312) - Release Address request #2."
  ut_setrequirements TS_6312, "F"
endif

;; Look for the expected event messages
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
   ($SC_$CPU_find_event[2].num_found_messages = 1 OR ;;
   $SC_$CPU_find_event[3].num_found_messages = 1)) then
  write "<*> Passed (6312) - Expected event Msgs received."
  ut_setrequirements TS_6312, "P"
else 
  write "<!> Failed (6312) - Did not receive the expected events."
  ut_setrequirements TS_6312, "F"
endif

write ";**********************************************************************"
write "; Step 19.0: Command a Power-On Reset on $CPU in order to clean up. "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
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

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
