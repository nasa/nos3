PROC $sc_$cpu_tbl_cmding
;*******************************************************************************
;  Test Name:  $sc_$cpu_tbl_cmding
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify the Table Services commands. 
;
;  Requirements Tested
;    cTBL6000	Upon receipt of Command the cFE shall Load an Inactive Table
;		Image with the contents of the Command specified File.
;    cTBL6000.1 If the Command specified files header indicates that the file
;		contains only a portion of the Table, the cFE shall first load
;		an Inactive Table Image with the contents of the Active Table
;		Image and then load the contents of the Command specified File.
;    cTBL6000.2 If the number of data bytes contained in the file is greater
;		than the maximum size of the table then the load shall be
;		aborted and an event message shall be generated.
;    cTBL6000.3	If the number of data bytes specified in the file's header is
;		not equal to the number of data bytes contained in the file then
;		the load shall be aborted and an event message generated.
;    cTBL6000.4	The Inactive Table Image shall only be loaded with the contents
;		of the Active Table if the Inactive Table Image has not been
;		initialized.
;    cTBL6001	Upon receipt of Command the cFE shall dump the Command specified
;		Active or Inactive Table contents to a Command specified File. 
;    cTBL6003	Upon receipt of Command the cFE shall make an Inactive Table
;		Image of the Command specified Table the Active Table Image.
;    cTBL6007	Upon receipt of Command the cFE shall generate a NO-OP event
;		message.
;    cTBL6008	Upon receipt of Command the cFE shall set to zero all counters
;		reported in telemetry.   
;    cTBL6009	Upon receipt of valid command, the cFE shall increment the valid
;		command counter.
;    cTBL6010	Upon receipt of an invalid command, the cFE shall in increment
;		the invalid command counter. 
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
;
;  Prerequisite Conditions
;       None 
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	11/18/05	R. Ferrer	Started procedure.
;	12/20/05	R. Ferrer	Updated .prc as per procedure
;					walkthrough comments.
;	03/03/06	W. Moleski	Updated proc to use new table structure
;					and utility procs.
;	03/17/06	W. Moleski	Updated proc according to the comments
;					made at the walkthrough.
;	06/14/06	W. Moleski	Updated proc to work with cFE3.3.
;	08/10/06	W. Moleski	Modified the proc to work with the
;					default tables located anywhere in the
;					Table Registry. Also, made it work if
;					the $CPU agrument is not specified.
;	11/20/06	W. Moleski	Modified the proc for cFE 4.0
;	07/19/07	W. Moleski	Modified the proc for cFE 4.2 changes.
;	02/08/12	W. Moleski	Added variable for ram disk and replaced
;					ut_setupevt with ut_setupevents
;	05/03/16	W. Moleski	Added variable for CPU to connect and 
;					retrieve files from (hostCPU) since this
;					may be different from the command CPU.
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
;   $sc_$cpu_tst_tbl_apps_start Procedure that checks if the TST_TBL and 
;				TST_TBL2 applications are running and starts
;				them if they are not.
;	get_tbl_to_cvt		Procedure that dumps the specified table from
;				the processor and loads it into the cvt
;	ut_tlmwait		Directive that waits for the specified telemetry
;				condition to be met
;	create_tbl_file_from_cvt Procedure that creates a load file from the
;				specified arguments and cvt
;	load_table		Procedure that takes the specified file and
;				transfers the file to the specified processor
;				and then issues a TBL_LOAD command using the
;				file.
;	get_file_to_cvt		Procedure to load the specified file into the
;				specified cvt
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_evs_events.h"
#include "cfe_tbl_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************
LOCAL i

#define TS_6000		0
#define TS_60001	1
#define TS_60002	2
#define TS_60003	3
#define TS_60004	4
#define TS_6001		5
#define TS_6003		6
#define TS_6007		7
#define TS_6008		8
#define TS_6009		9
#define TS_6010		10
#define TS_6011a	11
#define TS_6011b	12
#define TS_6011d	13
#define TS_6011e	14
#define TS_6011f	15
#define TS_6011g	16

global ut_req_array_size = 16
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL cmdcnt, errcnt, num_free_buffers, rawcmd
LOCAL tbl_1_pkt, tbl_2_pkt
LOCAL tbl_1_appid, tbl_2_appid
LOCAL valsuccesscnt, valfailedcnt, valnumreqcnt, valrespcnt
LOCAL tbl_1_index, tbl_2_index

local ramDir = "RAM:0"
local hostCPU = "$CPU"

;**********************************************************************
; Set the local values 
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["TS_6000", "TS_6000.1", "TS_6000.2", "TS_6000.3", "TS_6000.4", "TS_6001", "TS_6003", "TS_6007", "TS_6008", "TS_6009", "TS_6010", "TS_6011a", "TS_6011b", "TS_6011d", "TS_6011e", "TS_6011f", "TS_6011g"]

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

write ";*********************************************************************"
write "; Step 1.0: Proc setup. "
write ";*********************************************************************"
write "; Step 1.1: Command a Power-On Reset on $CPU "
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
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
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
  elseif ($SC_$CPU_RF[i].$SC_$CPU_TBL_NAME = "TST_TBL.dflt_tbl_02") then
    tbl_2_index = i
  endif

  write "Table #",i," Name = ", $SC_$CPU_RF[i].$SC_$CPU_TBL_NAME 
enddo

write "Default Table #1 is at index #" & tbl_1_index
write "Default Table #2 is at index #" & tbl_2_index

write ";**********************************************************************"
write "; Step 2.0 : Partial table load test (double buffer table = Table #2)  "
write ";**********************************************************************"
write "; Step 2.1 : Dump the active and inactive buffers prior to commanding  "
write "; the first table load to Table #2. The buffers should contain the     "
write "; following values: "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  = -5                  element[1]  =  0 "
write "; element[2]  = -4                  element[2]  =  0 "
write "; element[3]  = -3                  element[3]  =  0 "
write "; element[4]  = -2                  element[4]  =  0 "
write "; element[5]  = -1                  element[5]  =  0 "
write "; element[6]  =  0                  element[6]  =  0 "
write "; element[7]  =  1                  element[7]  =  0 "
write "; element[8]  =  2                  element[8]  =  0 "
write "; element[9]  =  3                  element[9]  =  0 "
write "; element[10] =  5                  element[10] =  0 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Inactive Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Inactive Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "; Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Passed (6001) - Expected Event Msg ", CFE_TBL_WRITE_DUMP_INF_EID, " but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
    ut_setrequirements TS_6001, "P"
  else
    write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
    ut_setrequirements TS_6001, "F"
  endif
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Active Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Active Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

; Check for event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) -  Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Passed (6001) - Expected Event Msg ", CFE_TBL_WRITE_DUMP_INF_EID, " but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
    ut_setrequirements TS_6001, "P"
  else
    write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
    ut_setrequirements TS_6001, "F"
  endif
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 2.2 : Perform the first partial load to Table #2. Elements 2 and"
write "; 6 are changed to 2 and 4 respectively. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 2nd & 6th byte of the table
$SC_$CPU_TST_TBL_TABLE2.element[2] = 2
$SC_$CPU_TST_TBL_TABLE2.element[6] = 4

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 1", "dbl_tbl_ld_1","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[2]", "$SC_$CPU_TST_TBL_TABLE2.element[6]")

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
  write "<*> Passed (6000;6009) - Partial Load command sent."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Partial Load command failed."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.3 : Dump the active and inactive buffers for Table #2. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  = -5                  element[1]  =   -5* "
write "; element[2]  = -4                  element[2]  =    2* "
write "; element[3]  = -3                  element[3]  =   -3* "
write "; element[4]  = -2                  element[4]  =   -2* "
write "; element[5]  = -1                  element[5]  =   -1* "
write "; element[6]  =  0                  element[6]  =    4* "
write "; element[7]  =  1                  element[7]  =    1* "
write "; element[8]  =  2                  element[8]  =    2* "
write "; element[9]  =  3                  element[9]  =    3* "
write "; element[10] =  5                  element[10] =    5* "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Inactive Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Inactive Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE2.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[3] = (-3) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[4] = (-2) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = (-1) AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 4) then
  write "<*> Passed (6000;6000.1) - Inactive buffer contains changes made."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_60001, "P"
else
  write "<!> Failed (6000;6000.1) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_60001, "1"
endif

write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the Active buffer to verify that it did not change
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Active Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Active Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 2.4 : Validate the inactive buffer for Table #2. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID,"INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Failed - InActive Table #2 validation failed."
endif
                                                                                
; Look for the Validation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Inactive Table #2 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Inactive Table #2 validation failed."
  write "<!> Failed - Event Message not received for validation command."
endif

;; Look at the LastValBuf to see if it iindicates INACTIVE
if (p@$SC_$CPU_TBL_LASTVALBUF = "Inactive") then
  write "<*> Passed - TBL HK indicates the Inactive buffer was validated."
else
  write "<!> Failed - TBL HK does not indicate the Inactive buffer was validated."
endif

wait 10

write ";**********************************************************************"
write "; Step 2.5 : Activate the load for Table #2."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed (6003;6009) - Activate command not sent properly (", ut_sc_status, ")."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003;6009) - Activate command sent properly." 
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6003, "P"
endif

; Look for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
  write "<!> Failed - Event Message not received for Activate command."
endif

;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed (6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

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
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/dbl_tbl_ld_1."
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
write "; Step 2.6 : Dump both buffers to verify the update was completed. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =  -5 "
write "; element[2]  =   2*                element[2]  =  -4* "
write "; element[3]  =  -3                 element[3]  =  -3 "
write "; element[4]  =  -2                 element[4]  =  -2 "
write "; element[5]  =  -1                 element[5]  =  -1 "
write "; element[6]  =   4*                element[6]  =   0* "
write "; element[7]  =   1                 element[7]  =   1 "
write "; element[8]  =   2                 element[8]  =   2 "
write "; element[9]  =   3                 element[9]  =   3 "
write "; element[10] =   5                 element[10] =   5 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Inactive Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Inactive Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) ; Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) ; Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

; Dump the Active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Active Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Active Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; verify dump of the active table (should have been modified)
if ($SC_$CPU_TST_TBL_TABLE2.element[2] = 2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[6] = 4) then
  write "<*> Passed (6003) - Active buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Active buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 2.7 : Perform the second partial load to Table #2. Elements 3   "
write "; and 5 are modified to 3 and 5 respectively. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 3rd & 5th elements of the table
$SC_$CPU_TST_TBL_TABLE2.element[3] = 3
$SC_$CPU_TST_TBL_TABLE2.element[5] = 5

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 2", "dbl_tbl_ld_2","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[3]", "$SC_$CPU_TST_TBL_TABLE2.element[5]")

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
  write "<*> Passed (6000;6009) - Partial Load command sent."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Partial Load command failed."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error counter remained the same."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_LASTFILELOADED = "/ram/dbl_tbl_ld_2") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 2.8 : Dump the buffers again for Table #2. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =  -5  "
write "; element[2]  =   2                 element[2]  =   2* "
write "; element[3]  =  -3                 element[3]  =   3* "
write "; element[4]  =  -2                 element[4]  =  -2  "
write "; element[5]  =  -1                 element[5]  =   5* "
write "; element[6]  =   4                 element[6]  =   4* "
write "; element[7]  =   1                 element[7]  =   1  "
write "; element[8]  =   2                 element[8]  =   2  "
write "; element[9]  =   3                 element[9]  =   3  "
write "; element[10] =   5                 element[10] =   5  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Inactive Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Inactive Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 5) then
  write "<*> Passed (6000) - Inactive buffer contains changes made."
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6000, "F"
endif

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Active Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Active Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 2.9 : Validate the inactive buffer for Table #2. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Failed - InActive Table #2 validation failed."
endif
                                                                                
; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Active Table #2 validated."
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Active Table #2 validation failed."
    write "<!> Failed - Event Message not received for validation command."
  endif
else
  write "<*> Passed - Active Table #2 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 10

write ";**********************************************************************"
write "; Step 2.10 : Activate the second load for Table #2. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed (6003;6009) - Activate command not sent properly."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003;6009) - Activate command sent properly." 
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6003, "P"
endif

; Look for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
  write "<!> Failed - Event Message not received for Activate command."
endif


;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed (6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

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
if ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/dbl_tbl_ld_2") then
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/dbl_tbl_ld_2."
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
write "; Step 2.11 : Dump the buffers again in order to verify the load. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =  -5 "
write "; element[2]  =   2                 element[2]  =   2* "
write "; element[3]  =   3*                element[3]  =  -3* "
write "; element[4]  =  -2                 element[4]  =  -2 "
write "; element[5]  =   5*                element[5]  =  -1* "
write "; element[6]  =   4                 element[6]  =   4* "
write "; element[7]  =   1                 element[7]  =   1 "
write "; element[8]  =   2                 element[8]  =   2 "
write "; element[9]  =   3                 element[9]  =   3 "
write "; element[10] =   5                 element[10] =   5 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Inactive Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Inactive Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the active buffer and verify the load changes
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Active Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Active Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; verify dump of the active table (should have been modified)
;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE2.element[3] = 3 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[5] = 5) then
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 2.12 : Activate Table #2 without loading. This should fail. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_ACTIVATE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (ut_sc_status <> UT_SC_Success) then
  write "<*> Passed (6010) - Activate command failed as expected."
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6010) - Activate command sent properly when it was expected to fail." 
  ut_setrequirements TS_6010, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_ACTIVATE_ERR_EID, "."
  ut_setrequirements TS_6010, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 2.13 : Perform a final dump. The buffers should remain unchanged"
write "; from the previous dump in Step 2.11. They should contain the following"
write "; (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =  -5 "
write "; element[2]  =   2                 element[2]  =   2 "
write "; element[3]  =   3                 element[3]  =  -3 "
write "; element[4]  =  -2                 element[4]  =  -2 "
write "; element[5]  =   5                 element[5]  =  -1 "
write "; element[6]  =   4                 element[6]  =   4 "
write "; element[7]  =   1                 element[7]  =   1 "
write "; element[8]  =   2                 element[8]  =   2 "
write "; element[9]  =   3                 element[9]  =   3 "
write "; element[10] =   5                 element[10] =   5 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Inactive Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Inactive Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the Active Buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Active Dump command sent."
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001;6009) - Active Dump command failed."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6001, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error counter remained the same."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) ; Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 3.0 : Partial table load test (no activation between loads)"
write ";**********************************************************************"
write "; Step 3.1 : Perform the first load in a series of 3 (overall Load #3) "
write "; to Table #2. Elements 4 and 7 are modified to 4 and 0. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 4th & 7th element of the table
$SC_$CPU_TST_TBL_TABLE2.element[4] = 4
$SC_$CPU_TST_TBL_TABLE2.element[7] = 0

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 3", "dbl_tbl_ld_3","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[4]", "$SC_$CPU_TST_TBL_TABLE2.element[7]")

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
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_3") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "Waiting for LoadInProgress flag to change for table #2......"
wait until ($SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LOADBUFFERID <> -1)
write "<*> Passed - LoadInProgress flag indicates a load is pending."

wait 5

write ";**********************************************************************"
write "; Step 3.2 : Dump the active and inactive buffers for Table #2. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =   -5 "
write "; element[2]  =   2                 element[2]  =    2 "
write "; element[3]  =   3                 element[3]  =    3 "
write "; element[4]  =  -2                 element[4]  =    4*"
write "; element[5]  =   5                 element[5]  =    5 "
write "; element[6]  =   4                 element[6]  =    4 "
write "; element[7]  =   1                 element[7]  =    0*"
write "; element[8]  =   2                 element[8]  =    2 "
write "; element[9]  =   3                 element[9]  =    3 "
write "; element[10] =   5                 element[10] =    5 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) ; Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE2.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 0) then
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; print the dumped table elements
write "Table #2 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 3.3 : Perform the second load (4th overall) to Table #2. Element"
write "; 10 is modified to -1. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 8th byte of the table
$SC_$CPU_TST_TBL_TABLE2.element[10] = -1

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 4", "dbl_tbl_ld_4","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[10]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

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
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_4") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.4 : Dump the active and inactive buffers for Table #2. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =   -5 "
write "; element[2]  =   2                 element[2]  =    2 "
write "; element[3]  =   3                 element[3]  =    3 "
write "; element[4]  =  -2                 element[4]  =    4 "
write "; element[5]  =   5                 element[5]  =    5 "
write "; element[6]  =   4                 element[6]  =    4 "
write "; element[7]  =   1                 element[7]  =    0 "
write "; element[8]  =   2                 element[8]  =    2 "
write "; element[9]  =   3                 element[9]  =    3 "
write "; element[10] =   5                 element[10] =   -1*"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE2.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = -1) then
  write "<*> Passed (6003;6000.4) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
  ut_setrequirements TS_60004, "P"
else
  write "<!> Failed (6003;6000.4) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
  ut_setrequirements TS_60004, "F"
endif

; print the dumped table elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the Active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 3.5 : Perform the third load (5th overall) to Table #2. Element "
write ";  9 is modified to -2. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 9th element of the table
$SC_$CPU_TST_TBL_TABLE2.element[9] = -2

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 5", "dbl_tbl_ld_5","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[9]", "$SC_$CPU_TST_TBL_TABLE2.element[9]")

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
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_5") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 3.6 : Dump the active and inactive buffers for Table #2. The "
write "; buffers should contain the following (where * = changed values): "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  =   -5 "
write "; element[2]  =   2                 element[2]  =    2 "
write "; element[3]  =   3                 element[3]  =    3 "
write "; element[4]  =  -2                 element[4]  =    4 "
write "; element[5]  =   5                 element[5]  =    5 "
write "; element[6]  =   4                 element[6]  =    4 "
write "; element[7]  =   1                 element[7]  =    0 "
write "; element[8]  =   2                 element[8]  =    2 "
write "; element[9]  =   3                 element[9]  =   -2*"
write "; element[10] =   5                 element[10] =   -1 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE2.element[4] = 4 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[7] = 0 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[9] = -2 AND ;;
    $SC_$CPU_TST_TBL_TABLE2.element[10] = -1) then
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #2 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE2.element[i]
enddo

wait 5

;;; Dump the active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 3.7 : Validate the inactive buffer for Table #2. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Failed - InActive Table #2 validation failed."
endif
                                                                                
; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Inactive Table #2 validated."
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Inactive Table #2 validation failed."
    write "<!> Failed - Event Message not received for validation command."
  endif
else
  write "<*> Passed - Active Table #2 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 10

write ";**********************************************************************"
write "; Step 3.8 : Activate all of the loads at once for Table #2. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_02"""
if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed (6003;6009) - Activate command not sent properly."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003;6009) - Activate command sent properly." 
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6003, "P"
endif

; Look for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
  write "<!> Failed - Event Message not received for Activate command."
endif


;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_02") then
  write "<*> Passed (6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

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
  write "<*> Passed - The correct file was loaded into Table #2."
else
  write "<!> Failed - Table #2 was loaded with ", $SC_$CPU_RF[tbl_2_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/dbl_tbl_ld_5."
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
write "; Step 3.9 : Perform a final dump of the active and inactive buffers   "
write "; for Table #2. They should contain the following (where * = changed) "
write "; values: "
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  -5                 element[1]  = -5 "
write "; element[2]  =   2                 element[2]  =  2 "
write "; element[3]  =   3                 element[3]  =  3 "
write "; element[4]  =   4*                element[4]  = -2*"
write "; element[5]  =   5                 element[5]  =  5 "
write "; element[6]  =   4                 element[6]  =  4 "
write "; element[7]  =   0*                element[7]  =  1*"
write "; element[8]  =   2                 element[8]  =  2 "
write "; element[9]  =  -2*                element[9]  =  3*"
write "; element[10] =  -1*                element[10] =  5*"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","I","inactive_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

;;; Dumpt the active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_02","A","active_tbl_2_dmp",hostCPU,tbl_2_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_2_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 4.0 : Partial table load test (single buffer table (Table #1)   "
write ";**********************************************************************"
write ";**********************************************************************"
write "; Step 4.1 : Dump the active table prior to commanding the table load. "
write "; Only the Active buffer exists for a single buffer table unless a "
write "; load command was executed. The expected values are:"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  =  n/a"
write "; element[2]  =   2                 element[2]  =  n/a"
write "; element[3]  =   3                 element[3]  =  n/a"
write "; element[4]  =   4                 element[4]  =  n/a"
write "; element[5]  =   5                 element[5]  =  n/a"
write "; element[6]  =   6                 element[6]  =  n/a"
write "; element[7]  =   7                 element[7]  =  n/a"
write "; element[8]  =   8                 element[8]  =  n/a"
write "; element[9]  =  10                 element[9]  =  n/a"
write "; element[10] =   9                 element[10] =  n/a"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Passed (6001) - Expected Event Msg ", CFE_TBL_WRITE_DUMP_INF_EID, " but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
    ut_setrequirements TS_6001, "P"
  else
    write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
    ut_setrequirements TS_6001, "F"
  endif
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 4.2 : Perform the first partial load to Table #1. Element 2 and "
write "; 6 are modified to 6 and 2 respectively. "
write ";**********************************************************************"
;;; Change the 2nd & 6th element of the table
$SC_$CPU_TST_TBL_TABLE1.element[2] = 6
$SC_$CPU_TST_TBL_TABLE1.element[6] = 2

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Partial Table #1 load 1", "sngl_tbl_ld_1","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[2]", "$SC_$CPU_TST_TBL_TABLE1.element[6]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 

start load_table ("sngl_tbl_ld_1", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " free shared buffers. Counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/sngl_tbl_ld_1") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "; Step 4.3 : Dump the inactive buffer for Table #1 to verify the load. "
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  =   1* "
write "; element[2]  =   2                 element[2]  =   6* "
write "; element[3]  =   3                 element[3]  =   3* "
write "; element[4]  =   4                 element[4]  =   4* "
write "; element[5]  =   5                 element[5]  =   5* "
write "; element[6]  =   6                 element[6]  =   2* "
write "; element[7]  =   7                 element[7]  =   7* "
write "; element[8]  =   8                 element[8]  =   8* "
write "; element[9]  =  10                 element[9]  =  10* "
write "; element[10] =   9                 element[10] =   9* "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_WRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  if ($SC_$CPU_evs_eventid = CFE_TBL_OVERWRITE_DUMP_INF_EID) then
    write "<*> Passed (6001) - Expected Event Msg ", CFE_TBL_WRITE_DUMP_INF_EID, " but rcv'd Event Msg ",$SC_$CPU_evs_eventid," which means the dump file was overwritten."
    ut_setrequirements TS_6001, "P"
  else
    write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_WRITE_DUMP_INF_EID, "."
    ut_setrequirements TS_6001, "F"
  endif
endif

; verify dump of the table
;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE1.element[2] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 2) then 
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 4.4 : Validate the inactive buffer for Table #1. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation failed."
endif
                                                                                
; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Inactive Table #1 validated."
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Inactive Table #1 validation failed."
    write "<!> Failed - Event Message not received for validation command."
  endif
else
  write "<*> Passed - Active Table #1 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 10

write ";**********************************************************************"
write "; Step 4.5 : Activate the load for Table #1. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed (6003;6009) - Activate command not sent properly."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003;6009) - Activate command sent properly." 
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6003, "P"
endif

; Look for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
  write "<!> Failed - Event Message not received for Activate command."
endif


if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 5

;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_01") then
  write "<*> Passed (6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)
wait 10

; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_1") then
  write "<*> Passed - The correct file was loaded into Table #1."
else
  write "<!> Failed - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/sngl_tbl_ld_1."
endif

;  verify the # of shared buffers available is 4
if ($SC_$CPU_TBL_NumFreeshrBuf = 4) then
  write "<*> Passed - There are 4 available shared buffers."
else
  write "<!> Failed - There are ", $SC_$CPU_TBL_NumFreeshrBuf, " available shared buffers. Expected 4 buffers."
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
write "; Step 4.6 : Dump the active buffer to verify the update was completed."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  = n/a "
write "; element[2]  =   6*                element[2]  = n/a "
write "; element[3]  =   3                 element[3]  = n/a "
write "; element[4]  =   4                 element[4]  = n/a "
write "; element[5]  =   5                 element[5]  = n/a "
write "; element[6]  =   2*                element[6]  = n/a "
write "; element[7]  =   7                 element[7]  = n/a "
write "; element[8]  =   8                 element[8]  = n/a "
write "; element[9]  =  10                 element[9]  = n/a "
write "; element[10] =   9                 element[10] = n/a "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; verify dump of the table
;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE1.element[2] = 6 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[6] = 2) then 
  write "<*> Passed (6003) - Active buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Active buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #1 Active Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 4.7 : Perform the second partial load of Table #1. Elements 3 & "
write "; 5 are modified to 5 and 3 respectively. "
write ";**********************************************************************"
;;; Change the 3rd & 5th byte of the table
$SC_$CPU_TST_TBL_TABLE1.element[3] = 5
$SC_$CPU_TST_TBL_TABLE1.element[5] = 3

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Partial Table #1 load 2", "sngl_tbl_ld_2","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[3]", "$SC_$CPU_TST_TBL_TABLE1.element[5]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 

start load_table ("sngl_tbl_ld_2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/sngl_tbl_ld_2") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "; Step 4.8 : Dump the inactive buffer for Table #1 to verify the load."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  =   1* "
write "; element[2]  =   6                 element[2]  =   6* "
write "; element[3]  =   3                 element[3]  =   5* "
write "; element[4]  =   4                 element[4]  =   4* "
write "; element[5]  =   5                 element[5]  =   3* "
write "; element[6]  =   2                 element[6]  =   2* "
write "; element[7]  =   7                 element[7]  =   7* "
write "; element[8]  =   8                 element[8]  =   8* "
write "; element[9]  =  10                 element[9]  =  10* "
write "; element[10] =   9                 element[10] =   9* "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

; verify dump of the table
;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE1.element[3] = 5 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[5] = 3) then 
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 4.9 : Validate the inactive buffer for Table #1. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation failed."
endif
                                                                                
; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Inactive Table #1 validated."
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Inactive Table #1 validation failed."
    write "<!> Failed - Event Message not received for validation command."
  endif
else
  write "<*> Passed - Active Table #1 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 10

write ";**********************************************************************"
write "; Step 4.10 : Activate the second load for Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed (6003;6009) - Activate command not sent properly."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6003, "F"
else   
  write "<*> Passed (6003;6009) - Activate command sent properly." 
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6003, "P"
endif

; Look for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
  write "<!> Failed - Event Message not received for Activate command."
endif

;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_01") then
  write "<*> Passed (6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

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
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_2") then
  write "<*> Passed - The correct file was loaded into Table #1."
else
  write "<!> Failed - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/sngl_tbl_ld_2."
endif

;  verify the # of shared buffers available is 4
if ($SC_$CPU_TBL_NumFreeshrBuf = 4) then
  write "<*> Passed - There are 4 available shared buffers."
else
  write "<!> Failed - There are ", $SC_$CPU_TBL_NumFreeshrBuf, " available shared buffers. Expected 4 buffers."
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
write "; Step 4.11 : Dump the active buffer to verify the table was updated."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  = n/a "
write "; element[2]  =   6                 element[2]  = n/a "
write "; element[3]  =   5*                element[3]  = n/a "
write "; element[4]  =   4                 element[4]  = n/a "
write "; element[5]  =   3*                element[5]  = n/a "
write "; element[6]  =   2                 element[6]  = n/a "
write "; element[7]  =   7                 element[7]  = n/a "
write "; element[8]  =   8                 element[8]  = n/a "
write "; element[9]  =  10                 element[9]  = n/a "
write "; element[10] =   9                 element[10] = n/a "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 4.12 : Activate without performing a load. This should fail."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_ACTIVATE_ERR_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
if (ut_sc_status <> UT_SC_Success) then
  write "<*> Passed (6010) - Activate command failed because a load was not performed."
  ut_setrequirements TS_6010, "P"
else   
  write "<!> Failed (6010) - Activate command succeeded when a load was not performed." 
  ut_setrequirements TS_6010, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_ACTIVATE_ERR_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 4.13 : Perform a final dump of the Table #1 active buffer."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  = n/a "
write "; element[2]  =   6                 element[2]  = n/a "
write "; element[3]  =   5                 element[3]  = n/a "
write "; element[4]  =   4                 element[4]  = n/a "
write "; element[5]  =   3                 element[5]  = n/a "
write "; element[6]  =   2                 element[6]  = n/a "
write "; element[7]  =   7                 element[7]  = n/a "
write "; element[8]  =   8                 element[8]  = n/a "
write "; element[9]  =  10                 element[9]  = n/a "
write "; element[10] =   9                 element[10] = n/a "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 5.0 : Partial table load test (single buffer table)"
write ";            (with no activations in between loads)"
write ";**********************************************************************"
write "; Step 5.1 : Perform the first partial load (3rd overall) to Table #1. "
write "; Elements 4 and 7 are modified to 7 and 4 respectively. "
write ";**********************************************************************"
;;; Change the 4th & 7th byte of the table
$SC_$CPU_TST_TBL_TABLE1.element[4] = 7
$SC_$CPU_TST_TBL_TABLE1.element[7] = 4

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Partial Table #1 load 3", "sngl_tbl_ld_3","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[4]", "$SC_$CPU_TST_TBL_TABLE1.element[7]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 

start load_table ("sngl_tbl_ld_3", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/sngl_tbl_ld_3") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "; Step 5.2 : Dump the inactive buffer for Table #1 to verify the load."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  =   1* "
write "; element[2]  =   6                 element[2]  =   6* "
write "; element[3]  =   5                 element[3]  =   5* "
write "; element[4]  =   4                 element[4]  =   7* "
write "; element[5]  =   3                 element[5]  =   3* "
write "; element[6]  =   2                 element[6]  =   2* "
write "; element[7]  =   7                 element[7]  =   4* "
write "; element[8]  =   8                 element[8]  =   8* "
write "; element[9]  =  10                 element[9]  =  10* "
write "; element[10] =   9                 element[10] =   9* "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4) then 
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 5.3 : Perform the second partial load (4th overall) to Table #1."
write "; Element 8 is modified to 1."
write ";**********************************************************************"
;;; Change the 8th element of the table
$SC_$CPU_TST_TBL_TABLE1.element[8] = 1

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Partial Table #1 load 4", "sngl_tbl_ld_4","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[8]", "$SC_$CPU_TST_TBL_TABLE1.element[8]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 

start load_table ("sngl_tbl_ld_4", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

; Verify that the # of available shared buffers did not change
write "Free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 
if (num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf) then
  write "<*> Passed - Free shared buffers remained the same"
else
  write "<!> Failed - Expected ", num_free_buffers, " free shared buffers. Actual = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/sngl_tbl_ld_4") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 5.4 : Dump the inactive buffer again to verify the load."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  =   1  "
write "; element[2]  =   4                 element[2]  =   6  "
write "; element[3]  =   1                 element[3]  =   5  "
write "; element[4]  =   4                 element[4]  =   7  "
write "; element[5]  =   5                 element[5]  =   3  "
write "; element[6]  =   2                 element[6]  =   2  "
write "; element[7]  =   7                 element[7]  =   4  "
write "; element[8]  =   8                 element[8]  =   1* "
write "; element[9]  =  10                 element[9]  =  10  "
write "; element[10] =   9                 element[10] =   9  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;; 
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 1) then 
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

write ";**********************************************************************"
write "; Step 5.5 : Perform the third partial load (5th overall) to Table #1. "
write "; Element 1 is modified to 8."
write ";**********************************************************************"
;;; Change the first element of the table
$SC_$CPU_TST_TBL_TABLE1.element[1] = 8

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Partial Table #1 load 5", "sngl_tbl_ld_5","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[1]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 

start load_table ("sngl_tbl_ld_5", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
if (num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf) then
  write "<*> Passed - Free shared buffers remained the same"
else
  write "<!> Failed - Expected ", num_free_buffers, " free shared buffers. Actual = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/sngl_tbl_ld_5") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 5.6 : Dump the active and inactive buffers again for Table #1."
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =   1                 element[1]  =  8* "
write "; element[2]  =   6                 element[2]  =  6  "
write "; element[3]  =   5                 element[3]  =  5  "
write "; element[4]  =   4                 element[4]  =  7  "
write "; element[5]  =   3                 element[5]  =  3  "
write "; element[6]  =   2                 element[6]  =  2  "
write "; element[7]  =   7                 element[7]  =  4  "
write "; element[8]  =   8                 element[8]  =  1 "
write "; element[9]  =  10                 element[9]  = 10  "
write "; element[10] =   9                 element[10] =  9  "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","I","inactive_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/inactive_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6001, "P"
else
  write "<!> Failed (6001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6001, "F"
endif

;;;; Verify that the elements that were changed are reflected in the dump
if ($SC_$CPU_TST_TBL_TABLE1.element[4] = 7 AND ;;
    $SC_$CPU_TST_TBL_TABLE1.element[7] = 4 AND ;; 
    $SC_$CPU_TST_TBL_TABLE1.element[1] = 8 AND ;; 
    $SC_$CPU_TST_TBL_TABLE1.element[8] = 1) then 
  write "<*> Passed (6003) - Inactive buffer contains changes made."
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Inactive buffer does not contain changes loaded."
  ut_setrequirements TS_6003, "F"
endif

; print the dumped elements
write "Table #1 Inactive Buffer dumped:"
for i = 1 to 10 do
  write " element[",i,"] = ", $SC_$CPU_TST_TBL_TABLE1.element[i]
enddo

wait 5

;;; Dump the active buffer
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 5.7 : Validate the inactive buffer for Table #1. This must be   "
write "; done in cFE 4.0 in order to activate a load."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation failed."
endif
                                                                                
; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Inactive Table #1 validated."
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Inactive Table #1 validation failed."
    write "<!> Failed - Event Message not received for validation command."
  endif
else
  write "<*> Passed - Active Table #1 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Step 5.8 : Activate the set of loads for Table #1."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName = ""TST_TBL.dflt_tbl_01"""
if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed (6003;6009) - Activate command not sent properly."
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6003, "F"
else
  write "<*> Passed (6003;6009) - Activate command sent properly." 
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6003, "P"
endif

; Look for the Activation success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Table #2 activation failed."
  write "<!> Failed - Event Message not received for Activate command."
endif

;; *** Check the TBL HK values
if ($SC_$CPU_TBL_LASTUPDTBLNAME = "TST_TBL.dflt_tbl_01") then
  write "<*> Passed (6011D) - the correct table was specified in HK"
  ut_setrequirements TS_6011d, "P"
else
  write "<!> Failed (6011D) - HK specifies the incorrect Table has been updated."
  ut_setrequirements TS_6011d, "F"
endif

; Check to see if the Event was rcv'd
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (6003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6003, "P"
else
  write "<!> Failed (6003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_OVERWRITE_DUMP_INF_EID, "."
  ut_setrequirements TS_6003, "F"
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", hostCPU)

wait 10

;  Verify #available shared buffers is 4
if ($SC_$CPU_TBL_NumFreeshrBuf = 4) then
  write "<*> Passed - There are 4 available shared buffers."
else
  write "<!> Failed - There are ", $SC_$CPU_TBL_NumFreeshrBuf, " available shared buffers."
endif
                                                                                
; Check to see if the last file loaded specified the correct file
if ($SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD = "/ram/sngl_tbl_ld_5") then
  write "<*> Passed - The correct file was loaded into Table #1."
else
  write "<!> Failed - Table #1 was loaded with ", $SC_$CPU_RF[tbl_1_index].$SC_$CPU_TBL_LASTFILEUPD, ". Expected /ram/sngl_tbl_ld_5."
endif

;  verify the # of shared buffers available is 4
if ($SC_$CPU_TBL_NumFreeshrBuf = 4) then
  write "<*> Passed - There are 4 available shared buffers."
else
  write "<!> Failed - There are ", $SC_$CPU_TBL_NumFreeshrBuf, " available shared buffers. Expected 4 buffers."
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
write "; Step 5.9 : Perform a final dump of the active buffer for Table #1. "
write "; The expected values are (where * = changed values):"
write ";       Active Buffer                     Inactive Buffer "
write "; element[1]  =  8*                element[1]  = n/a "
write "; element[2]  =  6                 element[2]  = n/a "
write "; element[3]  =  5                 element[3]  = n/a "
write "; element[4]  =  7*                element[4]  = n/a "
write "; element[5]  =  3                 element[5]  = n/a "
write "; element[6]  =  2                 element[6]  = n/a "
write "; element[7]  =  4*                element[7]  = n/a "
write "; element[8]  =  1*                element[8]  = n/a "
write "; element[9]  = 10                 element[9]  = n/a "
write "; element[10] =  9                 element[10] = n/a "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_OVERWRITE_DUMP_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC

s get_tbl_to_cvt (ramDir,"TST_TBL.dflt_tbl_01","A","active_tbl_1_dmp",hostCPU,tbl_1_pkt)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6001;6009) - Dump command sent successfully."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6001;6009) - Dump command did not execute successfully."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6001;6010) - error count did not increment."
  ut_setrequirements TS_6001, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6001;6010) - Error counter incremented."
  ut_setrequirements TS_6001, "F"
  ut_setrequirements TS_6010, "F"
endif

wait 10

;****** Check the dump file name in the TBL HK
if ($SC_$CPU_TBL_LASTFILEDUMPED = "/ram/active_tbl_1_dmp") then
  write "<*> Passed (6011G) - Dump filename is correct."
  ut_setrequirements TS_6011g, "P"
else
  write "<!> Failed (6011G) - Dump filename not the expected name."
  ut_setrequirements TS_6011g, "F"
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

wait 5

write ";**********************************************************************"
write "; Step 6.0 : Verify cFE generates a NOOP event message upon command"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NOOP_INF_EID, INFO, 1

ut_sendcmd "$SC_$CPU_TBL_NOOP"
if (ut_sc_status <> UT_SC_Success) then
   write "<!> Failed (6007;6009) - NOOP command not sent properly (", ut_sc_status, ")."
   ut_setrequirements TS_6009, "F"
   ut_setrequirements TS_6007, "F"
else   
   write "<*> Passed (6007;6009) - NOOP command sent properly." 
   ut_setrequirements TS_6009, "P"
   ut_setrequirements TS_6007, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6007) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6007, "P"
else
  write "<!> Failed (6007) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_NOOP_INF_EID, "."
  ut_setrequirements TS_6007, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.0 : Reset the Table Services Counters."
write ";**********************************************************************"
write "; Step 7.1:  Sending a command with an error so that the error counter"
write "; increments. A command with an invalid function code = hex 'aa'  "
write "; (42 decimal) is sent. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_CC1_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_TBL_CMDEC + 1
rawcmd = ""
;; CPU1 is the default
rawcmd = "1804c0000001aa"

;;if ("$CPU" = "CPU2") then
;;  rawcmd = "1824c0000001aa"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1844c0000001aa"
;;endif

ut_sendrawcmd "$SC_$CPU_TBL", (rawcmd) 

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Error counter incremented!"
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (6010;6011B)- Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements TS_6010, "P"
    ut_setrequirements TS_6011b, "P"
  else
    write "<!> Failed (6010;6011B) Event Message not received for Invalid command."
    ut_setrequirements TS_6010, "F"
    ut_setrequirements TS_6011b, "F"
  endif
else
  write "<!> Failed - Error counter did not increment!"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.2 : Perform a load to Table #2 that will fail validation. "
write ";  Element 9 is modified to 6 and element 10 modified to -6. "
write ";  The valid values for each element are -5 to 5."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

;;; Change the 9th element of the table
$SC_$CPU_TST_TBL_TABLE2.element[9] = 6
$SC_$CPU_TST_TBL_TABLE2.element[10] = -6

s create_tbl_file_from_cvt ("$CPU",tbl_2_appid,"Partial Table #2 load 6", "dbl_tbl_ld_6","TST_TBL.dflt_tbl_02", "$SC_$CPU_TST_TBL_TABLE2.element[9]", "$SC_$CPU_TST_TBL_TABLE2.element[10]")

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
  write "<*> Passed (6000;6009;6011A) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
  ut_setrequirements TS_6011a, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
  ut_setrequirements TS_6011a, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/dbl_tbl_ld_6") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6000, "P"
else
  write "<!> Failed (6000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
  ut_setrequirements TS_6000, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 7.3 : Validate the active buffer for Table #2 in order to "
write "; increment the TBL_FAILEDVALCTR counter. Failure is expected. "
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_02"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Active Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Failed - Active Table #2 validation command failed."
endif

; Look for the Validation failure event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table #2 Activated."
  write "<*> Passed - Active Table #2 failed validation as expected."
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for validation command."
endif

wait 10

write ";**********************************************************************"
write "; Step 7.3 : Load Table #1 with a load that will pass validation. To "
write "; fail validation, a value in the table must be outside the range of "
write "; 1 - 10. "
write ";**********************************************************************"
;;; Change the 1st element of the table
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

s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table #1 load that passes validation", "sngl_tbl_ld_good","TST_TBL.dflt_tbl_01", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

cmdcnt = $SC_$CPU_TBL_CMDPC+1
errcnt = $SC_$CPU_TBL_CMDEC
num_free_buffers = $SC_$CPU_TBL_numFreeshrBuf - 1
write "Current free shared buffers available = ",$SC_$CPU_TBL_numFreeshrBuf 

start load_table ("sngl_tbl_ld_good", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6009) - Load command sent successfully."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Load command did not execute successfully."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

if ($SC_$CPU_TBL_CMDEC = errcnt) then
  write "<*> Passed (6000;6010) - error count did not increment."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - Error counter incremented."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

; Verify that the # of available shared buffers is one less than before
write "Waiting for free shared buffers to = ", num_free_buffers
ut_tlmwait $SC_$CPU_TBL_NumFreeshrBuf, {num_free_buffers}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Free shared buffers = ", num_free_buffers
else
  write "<!> Failed - Expected ", num_free_buffers, " counter = ", $SC_$CPU_TBL_NumFreeshrBuf
endif

if ($SC_$CPU_TBL_LastFileLoaded = "/ram/sngl_tbl_ld_good") then
  write "<*> Passed (6011F) - the correct file was loaded."
  ut_setrequirements TS_6011f, "P"
else
  write "<!> Failed (6011F) - the expected file was not loaded."
  ut_setrequirements TS_6011f, "F"
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
write "; Step 7.4 : Validate the inactive buffer for Table #1. Success is "
write "; expected in order to increment the SUCCESSVALCTR counter."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation failed."
endif
                                                                                
; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Active Table #1 validated."
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Active Table #1 validation failed."
    write "<!> Failed - Event Message not received for validation command."
  endif
else
  write "<*> Passed - Active Table #1 validated."
  write "<*> Passed - Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Step 7.5 : Send the Reset Counter command."
write ";**********************************************************************"
;;; Print out the values of all the counters that should reset
cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC
valsuccesscnt = $SC_$CPU_TBL_VALSUCCESSCTR
valfailedcnt = $SC_$CPU_TBL_VALFAILEDCTR
valnumreqcnt = $SC_$CPU_TBL_VALREQCTR
valrespcnt = $SC_$CPU_TBL_VALCOMPLTDCTR

write "Command Ctr = ", cmdcnt
write "Error Ctr = ", errcnt
write "Validation Success Ctr = ", valsuccesscnt
write "Validation Failed Ctr = ", valfailedcnt
write "Validation Request Ctr = ", valnumreqcnt
write "Validation Response Ctr = ", valrespcnt

;;; Add code to fail the requirement if all the counters have not incremented
if (cmdcnt > 0) AND (errcnt > 0) AND (valsuccesscnt > 0) AND ;;
   (valfailedcnt > 0) AND (valnumreqcnt > 0) AND (valrespcnt > 0) then
  write "<*> Passed (6008) - All counters are non-zero."
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - One of the counters is zero. Failing the requirement TS_6008."
  ut_setrequirements TS_6008, "F"
endif

ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_RESET_INF_EID, DEBUG, 1

/$SC_$CPU_TBL_RESETCTRS

wait 5

;;; Print out the values of all the counters that should reset
cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC
valsuccesscnt = $SC_$CPU_TBL_VALSUCCESSCTR
valfailedcnt = $SC_$CPU_TBL_VALFAILEDCTR
valnumreqcnt = $SC_$CPU_TBL_VALREQCTR
valrespcnt = $SC_$CPU_TBL_VALCOMPLTDCTR

write "Command Ctr = ", cmdcnt
write "Error Ctr = ", errcnt
write "Validation Success Ctr = ", valsuccesscnt
write "Validation Failed Ctr = ", valfailedcnt
write "Validation Request Ctr = ", valnumreqcnt
write "Validation Response Ctr = ", valrespcnt

; Check if the CMDPC counter reset
ut_tlmwait $SC_$CPU_TBL_CMDPC, 0

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Command counter was reset!"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Command counter did not reset!"
  ut_setrequirements TS_6008, "F"
endif

ut_tlmwait $SC_$CPU_TBL_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Error counter was reset!"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Error counter did not reset!"
  ut_setrequirements TS_6008, "F"
endif

ut_tlmwait $SC_$CPU_TBL_VALSUCCESSCTR, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Validation Success counter was reset!"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Validation Success counter did not reset!"
  ut_setrequirements TS_6008, "F"
endif

ut_tlmwait $SC_$CPU_TBL_VALFAILEDCTR, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Validation Failed counter was reset!"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Validation Failed counter did not reset!"
  ut_setrequirements TS_6008, "F"
endif

ut_tlmwait $SC_$CPU_TBL_VALREQCTR, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Validation Requests counter was reset!"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Validation Requests counter did not reset!"
  ut_setrequirements TS_6008, "F"
endif

ut_tlmwait $SC_$CPU_TBL_VALCOMPLTDCTR, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Validation Response counter was reset!"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Validation Response counter did not reset!"
  ut_setrequirements TS_6008, "F"
endif

; Check for the expected event ID
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6008) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6008, "P"
else
  write "<!> Failed (6008) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_RESET_INF_EID, "."
  ut_setrequirements TS_6008, "F"
endif

wait 5

write ";**********************************************************************"
write "; Step 8.0 : Table Services Error condition tests."
write ";**********************************************************************"
write "; Step 8.1:  Send a Table Load command with an invalid table name."
write ";*********************************************************************"
s create_tbl_file_from_cvt ("$CPU",tbl_1_appid,"Table load with invalid table name", "invalid_tbl_name_load","TST_TBL.invalid_table_name", "$SC_$CPU_TST_TBL_TABLE1.element[1]", "$SC_$CPU_TST_TBL_TABLE1.element[10]")

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

start load_table ("invalid_tbl_name_load", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;6010) - error counter incremented."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6000;6010) - error counter remained the same."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6010, "F"
endif

if ($SC_$CPU_TBL_CMDPC = cmdcnt) then
  write "<*> Passed (6000;6009) - Load command with invalid table name generated an error as expected."
  ut_setrequirements TS_6000, "P"
  ut_setrequirements TS_6009, "P"
else
  write "<!> Failed (6000;6009) - Partial Load command failed."
  ut_setrequirements TS_6000, "F"
  ut_setrequirements TS_6009, "F"
endif

wait 5

write ";*********************************************************************"
write "; Step 8.2:  Send a Table Load command with a file that has more data "
write "; than the table can store. I.E., a file that is too large for the    "
write "; table to handle. This file contains valid header information for the"
write "; table load but contains more data than specified by the header.     "
write "; NOTE: This file needs to exist prior to executing this procedure.   "
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_TOO_BIG_ERR_EID, ERROR, 1

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

;;; Load the file that has more data than needed
start load_table ("too_much_data_load", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - File Load generated an error as expected."
else
  write "<!> Failed - File Load worked when failure was expected."
endif

if ($SC_$CPU_TBL_CMDPC = cmdcnt) then
  write "<*> Passed - File Load did not increment CMDPC as expected."
else
  write "<!> Failed - File Load incremented CMDPC."
endif

; Check for the expected event ID
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (60003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_60003, "P"
else
  write "<!> Failed (60003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_ACCESS_ERR_EID, "."
  ut_setrequirements TS_60003, "F"
endif

wait 5

write ";*********************************************************************"
write "; Step 8.3:  Send a Table Load command without a filename."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_ACCESS_ERR_EID, ERROR, 1

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

/$SC_$CPU_TBL_LOAD LFILENAME=""

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6010) - Load command without a filename generated an error." 
  ut_setrequirements TS_6010, "P"
else   
  write "<!> Failed (6010) - Load command worked when it should have failed."
  ut_setrequirements TS_6010, "F"
endif

; Check for the expected event ID
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (6010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_6010, "P"
else
  write "<!> Failed (6010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_ACCESS_ERR_EID, "."
  ut_setrequirements TS_6010, "F"
endif

wait 5

write ";*********************************************************************"
write "; Step 8.4:  Send a Table Load command with a file that has more data "
write "; than the table can store. The table size is 40 bytes (10 integers). "
write "; The file header specifies 41 bytes of data and contains 41 bytes of "
write "; data. "
write "; NOTE: This file needs to exist prior to executing this procedure.   "
write ";*********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, ERROR, 1

cmdcnt = $SC_$CPU_TBL_CMDPC
errcnt = $SC_$CPU_TBL_CMDEC+1

;;; Load the file that has more data than needed
start load_table ("too_much_data_load2", hostCPU)

ut_tlmwait $SC_$CPU_TBL_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - File Load generated an error as expected."
else
  write "<!> Failed - File Load worked when failure was expected."
endif

if ($SC_$CPU_TBL_CMDPC = cmdcnt) then
  write "<*> Passed - File Load did not increment CMDPC as expected."
else
  write "<!> Failed - File Load incremented CMDPC."
endif

; Check for the expected event ID
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (60002) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements TS_60002, "P"
else
  write "<!> Failed (60002) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, "."
  ut_setrequirements TS_60002, "F"
endif

wait 5

write ";*********************************************************************"
write "; Step 9.0: Test clean-up"
write ";*********************************************************************"
write "; Step 9.1: Send the Load Abort command for Table #1 in order to clean"
write "; up after the ResetCtrs and Error tests. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_TBL_LOADABORT ABTABLENAME=""TST_TBL.dflt_tbl_01"""
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Load Abort command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load Abort command."
  endif
else
  write "<!> Failed - Load Abort for Table #1 failed."
endif

wait 5

write ";*********************************************************************"
write "; Step 9.2: Command a Power-On Reset on $CPU "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 60
                                                                                
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
