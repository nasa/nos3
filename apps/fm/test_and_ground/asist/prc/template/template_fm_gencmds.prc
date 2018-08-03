PROC $sc_$cpu_fm_gencmds
;*******************************************************************************
;  Test Name:  FM_GenCmds
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM)
;   general commands function properly. The FM_NoOp and FM_Reset
;   commands will be tested as well as invalid commands and an
;   application reset to see if the FM application behaves
;   appropriately.
;
;  Requirements Tested
;    FM1000	Upon receipt of a No-Op command, FM shall increment the Valid
;		Command Counter and generate an event message.
;    FM1001	Upon receipt of a Reset Counters command, FM shall reset
;		the following housekeeping variables to a value of zero:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;    FM1002	If the computed length of any FM command is not equal to
;		the length contained in the message header, FM shall reject
;		the command
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command
;		execution, increment the FM Command Rejected Counter and
;		issue an error event message
;    FM1009	Upon receipt of the Set Table Entry State Command, FM will set
;		the enable/disable state for the specified entry in the File
;		System Free Space Table.
;    FM1009.1	If the File System Free Space table has not been loaded, FM will
;		reject the command and send an event message
;    FM1009.2	If the command-specified entry in the File System Free Space
;		table is invalid, FM will reject the command and send an event
;		message
;    FM1009.3	If the command-specified state is invalid, FM will reject the
;		command and send an event message
;    FM1009.4	If the command-specified entry in the File System Free Space
;		table is unused, FM will reject the command and send an event
;		message
;    FM4000	FM shall generate a housekeeping message containing the
;		following:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;			c) For each file system: Total number of open files 
;    FM4001	Upon receipt of a Report Device Free Space command, FM shall
;		generate a message containing for each enabled device in the FM
;		device table the amount of available free space.
;    FM5000	Upon initialization of the FM Application, FM shall
;		initialize the following data to Zero
;			a) Valid Command Counter
;			b) Command Rejected Counter
;
;
;  Prerequisite Conditions
;    The cFE & FM are up and running and ready to accept commands. 
;    The FM commands and TLM items exist in the GSE database. 
;    The display page for the FM Housekeeping exists. 
;
;  Assumptions and Constraints
;	None
;
;  Change History
;
;	Date	   Name		Description
;	04/04/08   D. Stewart	Original Procedure
;	12/10/08   W. Moleski	General cleanup
;	03/04/10   W. Moleski	Added test of GetFreeSpace and SetTblState
;				commands
;       03/01/11   W. Moleski   Added variables for App name and ram directory
;       08/17/11   W. Moleski   Added Requirement 1009 to Steps 2.7 and 2.9
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG except
;				for the NOOP command.
;
;  Arguments
;	None
;
;  Procedures Called
;	Name			Description
; 
;  Required Post-Test Analysis
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_tbl_events.h"
#include "fm_events.h"
#include "fm_platform_cfg.h"
#include "fm_defs.h"

%liv (log_procedure) = logging

#define FM_1000     0
#define FM_1001     1
#define FM_1002     2
#define FM_1003     3
#define FM_1004     4
#define FM_1009     5
#define FM_10091    6
#define FM_10092    7
#define FM_10093    8
#define FM_10094    9
#define FM_4000     10
#define FM_4001     11
#define FM_5000     12

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 12
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1000","FM_1001","FM_1002","FM_1003","FM_1004","FM_1009","FM_1009.1","FM_1009.2","FM_1009.3","FM_1009.4","FM_4000", "FM_4001", "FM_5000"]

local rawcmd

local FMAppName = FM_APP_NAME
local ramDirPhys = "RAM:0" 
local freeSpaceTblName = FMAppName & "." & FM_TABLE_CFE_NAME

local cmdCtr, errcnt

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
write ";  Step 1.1:  Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 1.2: Create the FreeSpace table load image, upload this image, "
write ";  and start the File Manager (FM) and Test (TST_FM) Applications. "
write ";********************************************************************"
s $sc_$cpu_fm_tableloadfile
wait 5
    
s ftp_file ("CF:0/apps", "$cpu_fmdevtbl_ld_1", FM_TABLE_FILENAME, "$CPU", "P")
wait 5

s $sc_$cpu_fm_startfmapps
wait 5

write ";*********************************************************************"
write ";  Step 1.3: Verify that the FM Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
local hkPktId, fsindex

;; Set the FM HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p08A"
fsindex = "0FBA"

if ("$CPU" = "CPU2") then
  hkPktId = "p18A"
  fsindex = "0FD8"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p28A"
  fsindex = "0FF8"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4000) - Housekeeping packet is being generated."
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (4000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements FM_4000, "F"
endif

;; Check the HK tlm items to see if they are 0 or NULL
if ($SC_$CPU_FM_CMDPC = 0) AND ($SC_$CPU_FM_CMDEC = 0) THEN
  write "<*> Passed (5000) - Housekeeping telemetry initialized properly."
  ut_setrequirements FM_5000, "P"
else
  write "<!> Failed (5000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_FM_CMDPC
  write "  CMDEC           = ",$SC_$CPU_FM_CMDEC
  ut_setrequirements FM_5000, "F"
endif

;; Dump the Free Space Table
s get_tbl_to_cvt (ramDirPhys, freeSpaceTblName, "A", "$cpu_fmfreespacetbl","$CPU",fsindex)
wait 5

write ";***********************************************************************"
write ";  Step 1.4: Enable DEBUG Event Messages for the CFE_TBL application "
write ";***********************************************************************"
cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Debug events for the FM application are enabled in the startfmapps proc
;; Enable DEBUG events for the CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0: FM General Commanding tests."
write ";*********************************************************************"
write ";  Step 2.1: Send a valid No-Op command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_NOOP_CMD_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_FM_NOOP"
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1000;1003;4000) - NOOP command sent properly."
  ut_setrequirements FM_1000, "P"
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1000;1003;4000) - NOOP command not sent properly."
  ut_setrequirements FM_1000, "F"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1000;1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1000, "P"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1000;1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_NOOP_CMD_EID, "."
  ut_setrequirements FM_1000, "F"
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send No-Op commands with invalid command lengths."
write ";*********************************************************************"
write ";  Step 2.2.1: Too long."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_NOOP_PKT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000200A8"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000200A8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000200A8"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4000) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1002;1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_NOOP_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2.2: Too short."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_NOOP_PKT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000000A8"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000000A8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000000A8"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4000) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1002;1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_NOOP_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Send a valid ResetCtrs command."
write ";*********************************************************************"
;; Check that the counters are not 0
if ($SC_$CPU_FM_CMDPC = 0) then
  ;; Send a NOOP command
  /$SC_$CPU_FM_NOOP
  wait 5
endif

if ($SC_$CPU_FM_CMDEC = 0) then
  /raw {rawcmd}
  wait 5
endif

;; Setup for the expected event
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RESET_CMD_EID, "DEBUG", 1

/$SC_$CPU_FM_RESETCTRS
wait 5

ut_tlmwait $SC_$CPU_FM_CMDPC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;4000) - Valid Command Counter was reset."
  ut_setrequirements FM_1001, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1001;4000) - Valid Command Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements FM_1001, "F"
  ut_setrequirements FM_4000, "F"
endif

ut_tlmwait $SC_$CPU_FM_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;4000) - Command Rejected Counter was reset."
  ut_setrequirements FM_1001, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1001;4000) - Command Rejected Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements FM_1001, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1001, "P"
else
  write "<!> Failed (1001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_RESET_CMD_EID, "."
  ut_setrequirements FM_1001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Send ResetCtrs commands with invalid command lengths."
write ";*********************************************************************"
write ";  Step 2.4.1: Too long."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RESET_PKT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000201A9"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000201A9"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000201A9"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4000) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1002;1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_RESET_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4.2: Too short."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RESET_PKT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000001A9"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000001A9"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000001A9"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4000) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1002;1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_RESET_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

;; Display the Free Space page
page $SC_$CPU_FM_FREESPACE_TLM

write ";*********************************************************************"
write ";  Step 2.5: Send a valid Get Free Space command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FREE_SPACE_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_GetFreeSpace

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Get Free Space command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_4001, "P"
else
  write "<!> Failed (1003;4001) - Get Free Space command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_4001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FREE_SPACE_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

;; Check the tlm
for i = 0 to FM_TABLE_ENTRY_COUNT-1 do
  if (p@$SC_$CPU_FM_FreeSpaceTBL[i].State = "Enabled") then
    write "Free space on '",$SC_$CPU_FM_FreeSpacePkt[i].Name, "' is ", %hex($SC_$CPU_FM_FreeSpacePkt[i].Upper32)," and ",%hex($SC_$CPU_FM_FreeSpacePkt[i].Lower32)
  elseif (p@$SC_$CPU_FM_FreeSpaceTBL[i].State = "Disabled") then
    write "Table entry for '",$SC_$CPU_FM_FreeSpaceTBL[i].Name, "' is disabled."
  else
    write "Table entry #", i, " is not in use."
  endif
enddo

wait 5

write ";*********************************************************************"
write ";  Step 2.6: Send Get Free Space command with invalid command lengths."
write ";*********************************************************************"
write ";  Step 2.6.1: Too long."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_FREE_SPACE_PKT_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000210BA"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000210BB"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000210B8"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4001) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4001, "P"
else
  write "<!> Failed (1002;1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FREE_SPACE_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6.2: Too short."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_FREE_SPACE_PKT_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000010BA"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000010BB"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000010B8"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4000) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1002;1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FREE_SPACE_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Send a valid Set Table State command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName},FM_SET_TABLE_STATE_CMD_EID, "INFO", 1

local newFSState = FM_TABLE_ENTRY_UNUSED

;; Use the first entry. Set its state to the opposite of what it is currently
if (p@$SC_$CPU_FM_FreeSpaceTBL[0].State = "Enabled") then
  newFSState = FM_TABLE_ENTRY_DISABLED
else
  newFSState = FM_TABLE_ENTRY_ENABLED
endif

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_SetTblState TBLEntry=0 NewState=newFSState

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;1009) - Set Table State command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_1009, "P"
else
  write "<!> Failed (1003;1009) - Set Table State command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1009, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

;; Dump the Free Space Table
s get_tbl_to_cvt (ramDirPhys, freeSpaceTblName, "A", "$cpu_fmfreespacetbl","$CPU",fsindex)
wait 5

;; Verify the new state was set
if ($SC_$CPU_FM_FreeSpaceTBL[0].State = newFSState) then
  write "<*> Passed - Free Space Table State was set properly"
else
  write "<!> Failed - Free Space Table State was not set as expected"
endif


write ";*********************************************************************"
write ";  Step 2.8: Send Set Table State command with invalid command lengths."
write ";*********************************************************************"
write ";  Step 2.8.1: Too long."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_SET_TABLE_STATE_PKT_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000A11B2"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000A11B3"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000A11B3"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.8.2: Too short."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_SET_TABLE_STATE_PKT_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC000000811B2"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000000811B3"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000000811B3"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_PKT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Send a Set Table State command with invalid arguments"
write ";*********************************************************************"
write ";  Step 2.9.1: Invalid Table Entry"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_SET_TABLE_STATE_ARG_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_SetTblState TBLEntry=FM_TABLE_ENTRY_COUNT NewState=FM_TABLE_ENTRY_ENABLED

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1009.2) - Set Table State command failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_10092, "P"
else
  write "<!> Failed (1004;1009.2) - Set Table State command did not increment the CMDEC."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_10092, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_ARG_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9.2: Try to enable an UNUSED entry"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_SET_TABLE_STATE_UNUSED_ERR_EID,"ERROR", 1

;; Find an UNUSED entry in the Free Space Table
local fsEntry=0
for i = 0 to FM_TABLE_ENTRY_COUNT-1 do
  if ($SC_$CPU_FM_FreeSpaceTBL[i].State = FM_TABLE_ENTRY_UNUSED) then
    fsEntry = i
    break
  endif
enddo

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_SetTblState TBLEntry=fsEntry NewState=FM_TABLE_ENTRY_ENABLED

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1009.4) - Set Table State command failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_10094, "P"
else
  write "<!> Failed (1004;1009.4) - Set Table State command did not increment the CMDEC."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_10094, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_ARG_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9.3: Invalid State"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_SET_TABLE_STATE_ARG_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_SetTblState TBLEntry=0 NewState=3

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1009.3) - Set Table State command failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_10093, "P"
else
  write "<!> Failed (1004;1009.3) - Set Table State command did not increment the CMDEC."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_10093, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_ARG_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9.4: Send the command when the table is not loaded."
write ";*********************************************************************"
write ";  Step 2.9.4.1: Stop the FM and TST_FM applications."
write ";*********************************************************************"
;; Delete the apps
/$SC_$CPU_ES_DeleteApp Application="TST_FM"
wait 2
/$SC_$CPU_ES_DeleteApp Application=FMAppName
wait 2

write ";*********************************************************************"
write ";  Step 2.9.4.2: Remove the default table and restart the apps."
write ";*********************************************************************"
;; Remove the "default" table
s ftp_file ("CF:0/apps", FM_TABLE_FILENAME, FM_TABLE_FILENAME, "$CPU", "R")
wait 5

;; Start the fm Apps
s $sc_$cpu_fm_startfmapps
wait 5

write ";*********************************************************************"
write ";  Step 2.9.4.3: Send the Set State command."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_SET_TABLE_STATE_TBL_ERR_EID,"ERROR",1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_SetTblState TBLEntry=1 NewState=FM_TABLE_ENTRY_ENABLED

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1009.1) - Set Table State command failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_10091, "P"
else
  write "<!> Failed (1004;1009.1) - Set Table State command did not increment the CMDEC."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_10091, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_SET_TABLE_STATE_TBL_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Send an invalid FM function code."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000017FA8"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000017FA8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000017FA8"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000) - Command Rejected Counter incremented."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (1004;4000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CC_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Re-Initialization of the application."
write ";*********************************************************************"
write ";  Step 3.1:  Ensure Items are in non-initialized state."
write ";*********************************************************************"
;; Restore the default table load file that was removed in Step 2.9.4.2
s ftp_file ("CF:0/apps", "$cpu_fmdevtbl_ld_1", FM_TABLE_FILENAME, "$CPU", "P")
wait 5

;; Check that the counters are not 0
if ($SC_$CPU_FM_CMDPC = 0) then
  ;; Send a NOOP command
  /$SC_$CPU_FM_NOOP
  wait 5
endif

if ($SC_$CPU_FM_CMDEC = 0) then
  /raw {rawcmd}
  wait 5
endif

write "  CMDPC           = ",$SC_$CPU_FM_CMDPC
write "  CMDEC           = ",$SC_$CPU_FM_CMDEC

wait 5

write ";*********************************************************************"
write ";  Step 3.2:  Re-Initialize the FM app."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_STARTUP_EID, "INFO", 1

/$SC_$CPU_ES_RESTARTAPP APPLICATION=FMAppName
wait 5

write ";*********************************************************************"
write ";  Step 3.3: Verify that the FM Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Wait for telemetry to update
ut_tlmupdate $SC_$CPU_FM_CMDPC

;; Check the HK tlm items to see if they are 0 or NULL
if ($SC_$CPU_FM_CMDPC = 0) AND ($SC_$CPU_FM_CMDEC = 0) THEN
  write "<*> Passed (5000) - Housekeeping telemetry re-initialized properly."
  ut_setrequirements FM_5000, "P"
else
  write "<!> Failed (5000) - Housekeeping telemetry NOT re-initialized at restart."
  write "  CMDPC           = ",$SC_$CPU_FM_CMDPC
  write "  CMDEC           = ",$SC_$CPU_FM_CMDEC
  ut_setrequirements FM_5000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Create a FreeSpace table load image that will fail "
write ";  validation. "
write ";*********************************************************************"
s $sc_$cpu_fm_badtblloadfile
wait 5

write ";*********************************************************************"
write ";  Step 3.5: Load the table image created above to $CPU."
write ";*********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

s load_table("$cpu_fmbadtbl_ld_1","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command executed successfully."
else
  write "<!> Failed - Load command did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: Validate the table image loaded above."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID,"ERROR", 2
ut_setupevents "$SC","$CPU",{FMAppName}, FM_TABLE_VERIFY_EID, "INFO", 3
ut_setupevents "$SC","$CPU",{FMAppName}, FM_TABLE_VERIFY_ERR_EID, "ERROR", 4

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=freeSpaceTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - FreeSpace Table validate command sent."
else
  write "<!> Failed - TBL Validate command did not increment the TBL_CMDPC."
endif

;; Check if the event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message ",CFE_TBL_VAL_REQ_MADE_INF_EID," not received for Validate command."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validation Error Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Expected TBL Error Event Message ",CFE_TBL_VALIDATION_ERR_EID," not received."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - FM Validation Error Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
else
  write "<!> Failed - FM Validation Error Event Message ",FM_TABLE_VERIFY_EID," not received."
endif

if ($SC_$CPU_find_event[4].num_found_messages = 1) then
  write "<*> Passed - FM Validation Error Event Msg ",$SC_$CPU_find_event[4].eventid," Found!"
else
  write "<!> Failed - FM Validation Error Event Message ",FM_TABLE_VERIFY_ERR_EID," not received."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  Perform a Power-on Reset to clean-up from this test."
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

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $SC_$CPU_fm_GenCmds                                  "
write ";*********************************************************************"
ENDPROC
