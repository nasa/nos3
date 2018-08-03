PROC $sc_$cpu_sc_stress
;*******************************************************************************
;  Test Name:  sc_stress
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Stored Command (SC) application can 
;	support the execution of the maximum allowable Relative Time Sequences
;	(RTS) and the maximum number of commands contained in an Absolute Time
;	Sequences (ATS) or RTS.
;
;  Requirements Tested
;   SC1004	If SC accepts any command as valid, SC shall execute the
;		command, increment the SC Valid Command Counter and issue an
;		event message.
;   SC2000.2	SC shall accept maximum of <PLATFORM_DEFINED> commands per ATS.
;   SC2000.3	SC shall accept a variable numner of variable length commands
;		from each ATS. Each ATS command shall contain:
;		  a) A command number.
;		  b) A time tag denoting the time at which to execute the
;		     command.
;		  c) A variable length command.
;   SC2001	Upon receipt of a table update indication for an ATS table, SC
;		shall validate the following for the ATS table:
;		  a) Duplicate command numbers
;		  b) Invalid command lengths
;		  c) Commands that run off the end of the table
;   SC2002	SC shall allocate <PLATFORM_DEFINED> Relative Time-tagged
;		Sequences (RTSs) with each capable of storing <PLATFORM_DEFINED>
;		bytes of stored command data.
;   SC2002.2	SC shall accept variable length packed RTS commands within the
;		<PLATFORM_DEFINED> byte relative time-tagged sequences.
;   SC2002.3	Each individual command within the sequence shall consist of:
;		  a) A time tag with one second resolution.
;		  b) A variable length command, with a maximum length of
;		     <PLATFORM_DEFINED> bytes.
;   SC2003	Upon receipt of a table update indication for an RTS table, SC
;		shall set the RTS status to DISABLED.
;   SC2005	SC shall execute no more than <PLATFORM_DEFINED> commands per
;		second from all currently executing RTS tables and/or ATS tables
;   SC2005.1    SC shall defer execution of pending RTS commands, when the
;		combined execution count, of ATS and RTS, exceeds the command
;		per second limit.
;   SC2005.2    SC shall allow up to the maximum number of defined RTSs to be
;		active concurrently.
;   SC3000	Upon receipt of a Start ATS command, SC shall start the command-
;		specified ATS provided all of the following conditions are
;		satisfied:
;		  a) The command-specified ATS table identification is valid.
;		  b) The ATS table contains at least one command.
;		  c) Neither of the two ATS tables is currently executing.
;   SC3001	Upon receipt of a Stop ATS command, SC shall:
;		  a) Stop processing the currently executing ATS
;		  b) Set the state of that ATS to IDLE
;   SC4000	Upon receipt of a Start RTS command, SC shall start the
;		command-specified RTS provided all of the following conditions
;		are met:
;		  a) The command-specified RTS is not currently executing.
;		  b) The RTS table is enabled.
;		  c) The RTS table has been Loaded.
;   SC4000.1	If conditions are met, SC shall issue an event message
;		indicating the RTS started if the RTS number is less than
;		<PLATFORM_DEFINED> RTS number.
;   SC4003	Upon receipt of a Stop RTS command, SC shall terminate the
;		execution of an RTS table.
;   SC4004	Upon receipt of an Enable RTS command, SC shall set the status
;		of the command-specified RTS to Enabled.
;   SC8000	SC shall generate a housekeeping message containing the
;		following:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. Total count of commands dispatched from ATSs
;		  d. Total count of commands dispatched from RTSs
;		  e. Total count of commands which failed dispatch from ATSs
;		  f. Total count of commands which failed dispatch from RTSs
;		  g. ATS Table #1 free byte count
;		  h. ATS Table #2 free byte count
;		  i. Absolute Time Command Processing State
;		  j. Identifier of the active ATS table
;		  k. Number of the next ATS Command pending execution
;		  l. ATS switch pending flag
;		  m. Time the next ATS command is due to be dispatched
;		  n. The identifier of the ATS table for which the most recent
;		     ATS command failed to dispatch
;		  o. The identifier of the most recent ATS command which failed
;		     to dispatch from the ATS tables
;		  p. RTS table activation count
;		  q. RTS table activation error count
;		  r. Number of active RTSs 
;		  s. Identifier of the next RTS table to dispatch a command
;		  t. Time the next RTS command is due to be dispatched
;		  u. Execution status for each RTS table
;		  v. Enable status for each RTS table
;		  w. Identifier of the RTS table for which the most recent RTC
;		     command dispatch error occurred
;		  x. The word offset within the RTS for the most recent RTS
;		     command which failed to dispatch
;		  y. ATS Continue-On-Failure status
;                 z. The last append ATS 
;                aa. The last ATS Append Table command count
;                bb. The last appended count
;   SC9000	Upon a power-on or processor reset, SC shall initialize the
;		following data to Zero:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. Total count of commands dispatched from ATSs
;		  d. Total count of commands dispatched from RTSs
;		  e. Total count of commands which failed dispatch from ATSs
;		  f. Total count of commands which failed dispatch from RTSs
;		  g. ATS Table #1 free byte count
;		  h. ATS Table #2 free byte count
;		  i. Absolute Time Command Processing State - DISABLED
;		  j. Identifier of the active ATS table
;		  k. Number of the next ATS Command pending execution
;		  l. ATS switch pending flag
;		  m. Time the next ATS command is due to be dispatched
;		  n. The identifier of the ATS table for which the most recent
;		     ATS command failed to dispatch
;		  o. The identifier of the most recent ATS command which failed
;		     to dispatch from the ATS tables
;		  p. RTS table activation count
;		  q. RTS table activation error count
;		  r. Number of active RTSs 
;		  s. Identifier of the next RTS table to dispatch a command
;		  t. Time the next RTS command is due to be dispatched
;		  u. Execution status for each RTS table - IDLE
;		  v. Enable status for each RTS table - DISABLED
;		  w. Identifier of the RTS table for which the most recent RTC
;		     command dispatch error occurred
;		  x. The word offset within the RTS for the most recent RTS
;		     command which failed to dispatch
;		  y. ATS Continue-On-Failure status - DISABLED
;                 z. The last append ATS 
;                aa. The last ATS Append Table command count
;                bb. The last appended count
;   SC9004	Upon any reset, SC shall start RTS #1.
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The SC commands and telemetry items exist in the GSE database.
;	The display pages exist for the SC Housekeeping.
;	The SC Test application (TST_SC) exists.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	01/21/09	Walt Moleski	Original Procedure.
;       01/24/11        Walt Moleski    Updated for SC 2.1.0.0
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
#include "cfe_time_events.h"
#include "to_lab_events.h"
#include "sc_platform_cfg.h"
#include "sc_msgdefs.h"
#include "sc_events.h"
#include "tst_sc_events.h"

%liv (log_procedure) = logging

#define SC_1004		0
#define SC_20002	1
#define SC_20003	2
#define SC_2001		3
#define SC_2002		4
#define SC_20022	5
#define SC_20023	6
#define SC_2003		7
#define SC_2005		8
#define SC_20051	9
#define SC_20052	10
#define SC_3000		11
#define SC_3001 	12
#define SC_4000		13
#define SC_40001	14
#define SC_4003		15
#define SC_4004		16
#define SC_8000		17
#define SC_9000		18
#define SC_9004		19

global ut_req_array_size = 19
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SC_1004", "SC_2000.2", "SC_2000.3", "SC_2001", "SC_2002", "SC_2002.2", "SC_2002.3", "SC_2003", "SC_2005", "SC_2005.1", "SC_2005.2", "SC_3000", "SC_3001", "SC_4000", "SC_4000.1", "SC_4003", "SC_4004", "SC_8000", "SC_9000", "SC_9004" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
LOCAL cmdCtr, errcnt
LOCAL atsPktId,atsAppId
LOCAL rtsPktId,rtsAppId
local disableStatus
local bit5Mask = 16
local SCAppName = "SC"
local ramDir = "RAM:0"
local ATSATblName = SCAppName & "." & SC_ATS_TABLE_NAME & "1"
local ATSBTblName = SCAppName & "." & SC_ATS_TABLE_NAME & "2"
local RTS2TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "002"
local RTS5TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "005"

;; Set the pkt and app Ids for the appropriate CPU
;; CPU1 is the default
atsPktId = "0FBB"
atsAppId = 4027
rtsPktId = "0FBC"
rtsAppId = 4028

if ("$CPU" = "CPU2") then
  atsPktId = "0FD9"
  atsAppId = 4057
  rtsPktId = "0FDA"
  rtsAppId = 4058
elseif ("$CPU" = "CPU3") then
  atsPktId = "0FF9"
  atsAppId = 4089
  rtsPktId = "0FFA"
  rtsAppId = 4090
endif

write ";***********************************************************************"
write ";  Step 1.0: Stored Command Test Setup."
write ";***********************************************************************"
write ";  Step 1.1: Command a Power-on Reset on $CPU."
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_SC_HK
page $SC_$CPU_TST_SC_HK
page $SC_$CPU_SC_ATS_TBL
page $SC_$CPU_SC_RTS_TBL

write ";***********************************************************************"
write ";  Step 1.3: Start the Stored Command (SC) and Test Applications. "
write ";***********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("1.3")
wait 5

write ";***********************************************************************"
write ";  Step 1.4: Verify that the SC Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Add the HK message receipt test
local hkPktId

;; Set the SC HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0AA"

if ("$CPU" = "CPU2") then
  hkPktId = "p1AA"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2AA"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements SC_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements SC_8000, "F"
endif

;; Check the HK tlm items to see if they are initialized properly
local hkGood = FALSE
local freeByteCount = SC_ATS_BUFF_SIZE * 2
local RTSStatusSize = SC_NUMBER_OF_RTS/16

if (rts001_started = TRUE) then
  if ($SC_$CPU_SC_CMDPC = 1) AND ($SC_$CPU_SC_CMDEC = 0) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 1) AND ($SC_$CPU_SC_RTSActvErr = 0) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 3) AND ($SC_$CPU_SC_RTSErrCtr = 0) then
    hkGood = TRUE
  endif
else
  if ($SC_$CPU_SC_CMDPC = 0) AND ($SC_$CPU_SC_CMDEC = 1) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 0) AND ($SC_$CPU_SC_RTSActvErr = 1) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 0) AND ($SC_$CPU_SC_RTSErrCtr = 0) then
    hkGood = TRUE
  endif
endif

;; Check the default value of the continue on ATS Failure flag
;; if the above values passed
if (hkGood = TRUE) then
  if (SC_CONT_ON_FAILURE_START = TRUE) then
    if (p@$SC_$CPU_SC_ContATSFlag = "Disabled") then
      hkGood = FALSE
    endif
  else
    if (p@$SC_$CPU_SC_ContATSFlag = "Enabled") then
      hkGood = FALSE
    endif
  endif
endif

;; Check the RTS Execution and RTS Disabled Status fields
;; set the disable status for the first value
local disableStatusVal = x'FFFE'
if (rts001_started = FALSE) then
  disableStatusVal = x'FFFF'
endif

if (hkGood = TRUE) then
  for i = 1 to RTSStatusSize do
    if (i > 1) then
      disableStatusVal = x'FFFF'
    endif

    if ($SC_$CPU_SC_RTSExeStatus[i].RTSStatusEntry.AllStatus <> 0) then
      hkGood = FALSE
    endif

    if ($SC_$CPU_SC_RTSDisableStatus[i].RTSStatusEntry.AllStatus <> disableStatusVal) then
      hkGood = FALSE
    endif
  enddo
endif

;; Check the remaining HK parameters
if ($SC_$CPU_SC_ATSNumber = 0) AND ($SC_$CPU_SC_ATPState = 2) AND ;;
   ($SC_$CPU_SC_ATPCmdNumber = 65536) AND ($SC_$CPU_SC_SwitchPend = 0) AND ;;
   ($SC_$CPU_SC_ActiveRTSs = 0) AND ($SC_$CPU_SC_NextRTS = 0) AND ;;
   ($SC_$CPU_SC_ATSCmdCtr = 0) AND ($SC_$CPU_SC_ATSErrCtr = 0) AND ;;
   ($SC_$CPU_SC_LastATSErr = 0) AND ($SC_$CPU_SC_LastATSCmdErr = 0) AND ;;
   ($SC_$CPU_SC_LastRTSErr = 0) AND ($SC_$CPU_SC_LastRTSCmdErr = 0) AND ;;
   ($SC_$CPU_SC_FreeBytes[1] = freeByteCount) AND ;;
   ($SC_$CPU_SC_FreeBytes[2] = freeByteCount) AND ;;
   ($SC_$CPU_SC_NextRTSTime = x'FFFFFFFF') AND ;;
   ($SC_$CPU_SC_NextATSTime = x'FFFFFFFF') AND ;;
   (p@$SC_$CPU_SC_AppendAtsID = "None") AND ;;
   ($SC_$CPU_SC_AppendCount = 0) AND ($SC_$CPU_SC_AppendSize = 0) AND ;;
   (hkGood = TRUE) then
  write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
  ut_setrequirements SC_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized properly at startup."
  write "CMDPC           = ", $SC_$CPU_SC_CMDPC
  write "CMDEC           = ", $SC_$CPU_SC_CMDEC
  write "RTSs Started    = ", $SC_$CPU_SC_RTSActvCtr
  write "RTSs Started EC = ", $SC_$CPU_SC_RTSActvErr
  write "RTS Command Ctr = ", $SC_$CPU_SC_RTSCmdCtr
  write "RTS Command EC  = ", $SC_$CPU_SC_RTSErrCtr
  write "Cont ATS Flag   = ", p@$SC_$CPU_SC_ContATSFlag
  write "ATS Number      = ", $SC_$CPU_SC_ATSNumber
  write "ATP State       = ", p@$SC_$CPU_SC_ATPState
  write "ATP Command #   = ", $SC_$CPU_SC_ATPCmdNumber
  write "Switch Pending? = ", p@$SC_$CPU_SC_SwitchPend
  write "Active RTSs     = ", $SC_$CPU_SC_ActiveRTSs
  write "Next RTS to run = ", $SC_$CPU_SC_NextRTS
  write "ATS Command Ctr = ", $SC_$CPU_SC_ATSCmdCtr
  write "ATS Command EC  = ", $SC_$CPU_SC_ATSErrCtr
  write "Last ATS w/err  = ", $SC_$CPU_SC_LastATSErr
  write "Last ATS Cmd err= ", $SC_$CPU_SC_LastATSCmdErr
  write "Last RTS w/err  = ", $SC_$CPU_SC_LastRTSErr
  write "Last RTS Cmd err= ", $SC_$CPU_SC_LastRTSCmdErr
  write "ATS A Free Bytes= ", $SC_$CPU_SC_FreeBytes[1]
  write "ATS B Free Bytes= ", $SC_$CPU_SC_FreeBytes[2]
  write "Next RTS Time   = ", %hex($SC_$CPU_SC_NextRTSTime)
  write "Next ATS Time   = ", %hex($SC_$CPU_SC_NextATSTime)
  write "Append ATS ID   = ", p@$SC_$CPU_SC_AppendATSID
  write "Append Count    = ", p@$SC_$CPU_SC_AppendCount
  write "Append Size     = ", p@$SC_$CPU_SC_AppendSize

  for i = 1 to RTSStatusSize do
    write "RTS Exe Status[",i,"] = ", %hex($SC_$CPU_SC_RTSExeStatus[i].RTSStatusEntry.AllStatus,4)
    write "RTS Dis Status[",i,"] = ", %hex($SC_$CPU_SC_RTSDisableStatus[i].RTSStatusEntry.AllStatus,4)
  enddo

  ut_setrequirements SC_9000, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 1.5: Enable DEBUG Event Messages "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 2
  
;; Enable DEBUG events for the SC and CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=SCAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG
  
ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";***********************************************************************"
write ";  Step 1.6: Verify that RTS #1 has been started.                       "
write ";***********************************************************************"
;; Check the global variable to indicate if RTS #1 started
if (rts001_started = TRUE) then
  write "<*> Passed (9004) - RTS #1 started."
  ut_setrequirements SC_9004, "P"
else
  write "<!> Failed (9004) - RTS #1 did not start on initialization."
  ut_setrequirements SC_9004, "F"
endif

write ";***********************************************************************"
write ";  Step 1.7: Verify that all RTS tables have been allocated.            "
write ";***********************************************************************"
;; Dump the table registry and count the number of RTS tables
s get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 10

local rtsTable,rtsTblCnt

rtsTblCnt = 0
;; Need to figure out the length of SCAppName and the RTS Table prefix
local rtsTblPrefix = SCAppName & "." & SC_RTS_TABLE_NAME
local rtsPrefixLen = %length(rtsTblPrefix)

;; Loop for the number of tables
for i = 1 to $SC_$CPU_TBL_NUMTABLES do
  rtsTable = %substring($SC_$CPU_RF[i].$SC_$CPU_TBL_Name,1,rtsPrefixLen)

  if (rtsTable = rtsTblPrefix) then
    rtsTblCnt = rtsTblCnt + 1
  endif
enddo

;; Verify that the appropriate RTS tables have been allocated
if (rtsTblCnt = SC_NUMBER_OF_RTS) then
  write "<*> Passed (2002) - All ",rtsTblCnt, " RTS Tables have been allocated."
  ut_setrequirements SC_2002, "P"
else
  write "<!> Failed (2002) - Allocated ",rtsTblCnt, " RTS Tables. Expected ",SC_NUMBER_OF_RTS, " tables to be allocated."
  ut_setrequirements SC_2002, "F"
endif

;; Need to dump an RTS table
s get_tbl_to_cvt (ramDir,RTS2TblName,"A","$cpu_rts2_tbl2_4","$CPU",rtsPktId)

;; Write each word of the dump table to fully verify that SC_RTS_BUFF_SIZE
;; word are stored in the RTS Table
for i = 1 to SC_RTS_BUFF_SIZE do
  write "RTSData[",i,"] = ",$SC_$CPU_SC_RTSDATA[i]
enddo

if (i = SC_RTS_BUFF_SIZE+1) then
  write "<*> Passed (2002) - Each RTS Table is capable of storing <",i,"> bytes of data."
  ut_setrequirements SC_2002, "P"
else
  write "<!> Failed (2002) - The RTS Table did not contain the expected bytes of data."
  ut_setrequirements SC_2002, "F"
endif

write ";***********************************************************************"
write ";  Step 2.0: ATS and RTS Execution Tests."
write ";***********************************************************************"
write ";  Step 2.1: Create, load, validate and activate a table image for each "
write ";  defined RTS. Since there are only a limited number of shared buffers "
write ";  to hold an inactive table load, these commands must be done "
write ";  equentially for each RTS."
write ";***********************************************************************"
;; need to loop for each each RTS in order to not use up all the shared buffers
local tableName, tableNum
local loadFileName, delayTime
local work = %env("WORK")
local filename = work & "/image/$sc_$cpu_rts_stres1.scs"

for i = 1 to SC_NUMBER_OF_RTS do
  ;; Set the first RTS command delay
  delayTime = SC_NUMBER_OF_RTS - i + 10

  ;; This is needed since STOL pads decimal numbers with blanks not zeros
  if (i < 10) then
    tableNum = "00" & i
  elseif (i < 100) then
    tableNum = "0" & i
  else
    tableNum = i
  endif

;;  tableName = "SC.RTS_TBL" & tableNum 
  tableName = SCAppName & "." & SC_RTS_TABLE_NAME & tableNum
  loadFileName = "$cpu_rts" & tableNum & "_load"

  write ";*********************************************************************"
  write ";  Step 2.1.1: Create and load the table image into the next RTS"
  write ";*********************************************************************"
  ;; Need to check if the .scs file used below exists. If not, end the proc
  if NOT file_exists(filename) then
    write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
    goto step2_9
  endif

  ;; Using the SCP utilities, compile and build the RTS load file
  compile_rts "$sc_$cpu_rts_stres1" {i}
  s $sc_$cpu_load_ats_rts("$sc_$cpu_rts_stres1",loadFileName,0)

  ;; Set the initial delay time for the first command
  $SC_$CPU_SC_RTSDATA[1] = delayTime

  s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS Stress Table Load",loadFileName,tableName,"$SC_$CPU_SC_RTSDATA[1]", "$SC_$CPU_SC_RTSDATA[10]")

  ;; Load the table
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_FILE_LOADED_INF_EID,"INFO", 1

  cmdCtr = $SC_$CPU_TBL_CMDPC + 1

  start load_table (loadFileName, "$CPU")

  ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Load command for RTS #",i," sent successfully."
  else
    write "<!> Failed - Load command for RTS #",i," did not execute successfully."
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Load command."
  endif

  write ";*********************************************************************"
  write ";  Step 2.1.2: Validate each RTS loaded above. "
  write ";*********************************************************************"
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

  cmdCtr = $SC_$CPU_TBL_CMDPC + 1

  /$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=tableName

  ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - RTS Table ",tableName," validate command sent."
    if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    else
      write "<!> Failed - Event Message not received for Validate command."
    endif
  else
    write "<!> Failed - RTS Table ",tableName," validation failed."
  endif

  ;; Wait for the validation message
  ;; If the message is rcv'd, then validation passed
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (2002.2;2002.3) - RTS Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
    ut_setrequirements SC_20022, "P"
    ut_setrequirements SC_20023, "P"
  else
    write "<!> Failed (2002.2;2002.3) - RTS Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
    ut_setrequirements SC_20022, "F"
    ut_setrequirements SC_20023, "F"
  endif

  write ";*********************************************************************"
  write ";  Step 2.1.3: Send the Table Services Activate command.   "
  write ";*********************************************************************"
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
  ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

  cmdCtr = $SC_$CPU_TBL_CMDPC + 1

  /$SC_$CPU_TBL_ACTIVATE ATableName=tableName

  ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Activate RTS Table ",tableName," command sent properly."
  else
    write "<!> Failed - Activate RTS Table ",tableName," command."
  endif

  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
  endif

  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (2002.2;2002.3) - RTS Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
    ut_setrequirements SC_20022, "P"
    ut_setrequirements SC_20023, "P"
  else
    write "<!> Failed (2002.2;2002.3) - RTS Table update failed. Event Msg not received for activate command."
    ut_setrequirements SC_20022, "F"
    ut_setrequirements SC_20023, "F"
  endif

  write ";*********************************************************************"
  write ";  Step 2.1.4: Enable this RTS. "
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_SC_CMDPC + 1
  ;; Send the Command
  /$SC_$CPU_SC_EnableRTS RTSID=i

  ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1004;4004) - SC Enable RTS command sent properly."
    ut_setrequirements SC_1004, "P"
    ut_setrequirements SC_4004, "P"
  else
    write "<!> Failed (1004;4004) - SC Enable RTS command did not increment CMDPC."
    ut_setrequirements SC_1004, "F"
    ut_setrequirements SC_4004, "F"
  endif

  ;; Check for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
    ut_setrequirements SC_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
    ut_setrequirements SC_1004, "F"
  endif
enddo

;; Make sure that the status indicates Enabled for ALL RTSs
local statusVars = SC_NUMBER_OF_RTS/16
local disableStatus
for i = 1 to statusVars do
  write "--RTS Disable Status for variable #",i, " = ", %bin($SC_$CPU_SC_RTSDisableStatus[i].RTSStatusEntry.AllStatus,16)
  disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[i].RTSStatusEntry.AllStatus
  if (disableStatus = 0) then
    write "<*> Passed (4004) - Disable Status indicates enabled."
    ut_setrequirements SC_4004, "P"
  else
    write "<!> Failed (4004) - Disable Status indicates disabled."
    ut_setrequirements SC_4004, "F"
  endif
enddo

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Create and load a valid table into ATS A. "
write ";***********************************************************************"
;; Get the current MET
local TAITime = hkPktId & "ttime"
local UTCTime = hkPktId & "stime"
local currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
local firstCmdMET = currentMET + 200
local lastCmdMET = currentMET + 223

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_stres1.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_9
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_stres1" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_stres1","$cpu_ats_a_load11",1,1) 

;; Create the ATS Table Load file
;;s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Stress Table Load", "$cpu_ats_a_load11",ATSATblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[192]")
;;
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load11", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";***********************************************************************"
write ";  Step 2.3: Send the Validate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS A Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS A Table validation failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS A Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS A Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.4: Send the Activate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS A Table command sent properly."
else
  write "<!> Failed - Activate ATS A Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2;2000.3) - ATS A Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
  ut_setrequirements SC_20003, "P"
else
  write "<!> Failed (2000.2;2000.3) - ATS A Table update failed. Event Msg not received for activate command."
  ut_setrequirements SC_20002, "F"
  ut_setrequirements SC_20003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Start command for ATS A.                         "
write ";***********************************************************************"
;; Setup the events
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_COMPL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartATS ATSID=1

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - SC Start ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3000, "P"
else
  write "<!> Failed (1004;3000) - SC Start ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STARTATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

write ";***********************************************************************"
write ";  Step 2.6: Send the Start command for each RTS. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_DBG_EID, "DEBUG", 2
local msgCnt1 = SC_LAST_RTS_WITH_EVENTS
local msgCnt2 = SC_NUMBER_OF_RTS - SC_LAST_RTS_WITH_EVENTS

for i = 1 to SC_NUMBER_OF_RTS do
  ;; Send the Command
  /$SC_$CPU_SC_StartRTS RTSID=i
  wait 1
enddo

;; Check for the event message 1
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, {msgCnt1}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2005.2;4000;4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd for each RTS whose ID is less than ",msgCnt1,"."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_20052, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;2005.2;4000;4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," was not rcv'd for each RTS. Expected ",msgCnt1, " msgs. Rcv'd ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_20052, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message 2
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, {msgCnt2}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2005.2;4000;4000.1) - Expected Event Msg ",SC_STARTRTS_CMD_DBG_EID," rcv'd for each RTS whose ID is > ",msgCnt1,"."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_20052, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;2005.2;4000;4000.1) - Expected Event Msg ",SC_STARTRTS_CMD_DBG_EID," was not rcv'd for each RTS. Expected ",msgCnt2, " msgs. Rcv'd ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_20052, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.7: Send the Stop command for ATS A.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPATS_CMD_INF_EID, "INFO", 1

;; Send the Command
/$SC_$CPU_SC_StopATS

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - Expected Event Msg ",SC_STOPATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3001, "P"
else
  write "<!> Failed (1004;3001) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg$",SC_STOPATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.8: Send the Stop command for each RTS.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTS_CMD_INF_EID, "INFO", 1

for i = 1 to SC_NUMBER_OF_RTS do
  ;; Send the Command
  /$SC_$CPU_SC_StopRTS RTSID=i
  wait 1
enddo

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = SC_NUMBER_OF_RTS) then
  write "<*> Passed (1004;4003) - Expected Event Msg ",SC_STOPRTS_CMD_INF_EID," rcv'd for each RTS."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4003, "P"
else
  write "<!> Failed (1004;4003) - Expected Event Msg ",SC_STOPRTS_CMD_INF_EID," was not rcv'd for each RTS. Expected ",SC_NUMBER_OF_RTS, " msgs. Rcv'd ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4003, "F"
endif

wait 5

step2_9:
write ";***********************************************************************"
write ";  Step 2.9: Create and load a valid table into ATS B containing 6    "
write ";  seconds of the maximum number of commands per second. "
write ";***********************************************************************"
;; Get the current MET
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
firstCmdMET = currentMET + 200
lastCmdMET = currentMET + 206

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsb_stres1.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step3_0
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsb_stres1" B {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsb_stres1","$cpu_ats_b_load3") 

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1
if (cmdCtr = 256) then
  cmdCtr = 0
endif

start load_table ("$cpu_ats_b_load3", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS B sent successfully."
else
  write "<!> Failed - Load command for ATS B did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";***********************************************************************"
write ";  Step 2.10: Send the Validate command for ATS B. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSBTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS B Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - Table validation did not increment the CMDPC correctly. Expected ",cmdCtr, "; TLM = ", $SC_$CPU_TBL_CMDPC
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS B Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS B Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.11: Send the Activate command for ATS B. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSBTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS B Table command sent properly."
else
  write "<!> Failed - Activate ATS B did not increment the CMDPC correctly. Expected ",cmdCtr, "; TLM = ", $SC_$CPU_TBL_CMDPC
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages , 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2;2000.3) - ATS B Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
  ut_setrequirements SC_20003, "P"
else
  write "<!> Failed (2000.2;2000.3) - ATS B Table update failed. Event Msg not received for activate command."
  ut_setrequirements SC_20002, "F"
  ut_setrequirements SC_20003, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSBTblName,"A","$cpu_atsb_tbl2_11","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Create and load a valid table into RTS #5 containing 10  " 
write ";  seconds of the multiple commands that will get deferred because of  "
write ";  the execution of ATS B."
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts5_load2.scs"
if NOT file_exists(filename) then
    write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
    goto step3_0
  endif

  ;; Using the SCP utilities, compile and build the RTS load file
  compile_rts "$sc_$cpu_rts5_load2" 5
  s $sc_$cpu_load_ats_rts("$sc_$cpu_rts5_load2","$cpu_rts005_load2")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts005_load2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #5 sent successfully."
else
  write "<!> Failed - Load command for RTS #5 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";*********************************************************************"
write ";  Step 2.13: Validate RTS #5 loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS5TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS Table ",tableName," validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS Table ",tableName," validation failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2003) - RTS Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - RTS Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2003, "F"
endif

write ";*********************************************************************"
write ";  Step 2.14: Send the Table Services Activate command.   "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS5TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #5 command sent properly."
else
  write "<!> Failed - Activate RTS #5 command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #5 Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #5 update failed. Event Msg not received for activate command."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,RTS5TblName,"A","$cpu_rts5_tbl2_14","$CPU",rtsPktId)

;; Check to make sure RTS #5 Status is disabled after the update
write "--RTS Disable Status for 1-16 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS5Status
write "--Bit 5 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #5 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #5 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif


write ";*********************************************************************"
write ";  Step 2.15: Enable RTS #5. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4004) - SC Enable RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (1004;4004) - SC Enable RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

write ";***********************************************************************"
write ";  Step 2.16: Send the Start command for ATS B.                         "
write ";***********************************************************************"
;; Setup the events
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_COMPL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartATS ATSID=2

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - SC Start ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3000, "P"
else
  write "<!> Failed (1004;3000) - SC Start ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STARTATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

write ";***********************************************************************"
write ";  Step 2.17: Wait until ATS B and RTS #5 complete execution. Verify the"
write ";  ATS commands executed first followed by the RTS commands. " 
write ";***********************************************************************"
ut_setrequirements SC_2005, "A"
ut_setrequirements SC_20051, "A"

;; Setup for the completion events
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_COMPL_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 2

lastCmdMET = $SC_$CPU_SC_NEXTATSTIME + 8 
;; Wait until ATS B and RTS #5 complete executing
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
local waitTime = lastCmdMET - currentMET
if (waitTime > 0) then
  wait waitTime
endif

;; Wait for the ATS Completion event
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Completion Event Msg ",SC_ATS_COMPL_INF_EID," rcv'd for each RTS."
else
  write "<!> Failed - Expected ATS Completion Event Msg ",SC_ATS_COMPL_INF_EID," was not rcv'd."
endif

;; Wait for the RTS Completion event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS Completion Event Msg ",SC_RTS_COMPL_INF_EID," rcv'd for each RTS."
else
  write "<!> Failed - Expected RTS Completion Event Msg ",SC_RTS_COMPL_INF_EID," was not rcv'd."
endif

step3_0:
write ";*********************************************************************"
write ";  Step 3.0: Clean-up - Send the Power-On Reset command.             "
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5


endTest:
write "**** Requirements Status Reporting"
                                                                                
write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables
drop rts001_started

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_stress"
write ";*********************************************************************"
ENDPROC
