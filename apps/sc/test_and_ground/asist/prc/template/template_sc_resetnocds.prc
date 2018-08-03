PROC $sc_$cpu_sc_resetnocds
;*******************************************************************************
;  Test Name:  sc_resetnocds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Stored Command (SC) application 
;	initializes the appropriate data items based upon the type of
;	initialization that occurs (Application, Processor, or Power-On). This
;	test also verifies that the proper notifications occur if anomalies
;	exist with the data items states in the requirements.
;
;	NOTE: This test should NOT be executed of the configuration parameter
;	indicating Save Critical Data is set to YES by the mission.
;
;  Requirements Tested
;   SC1004	If SC accepts any command as valid, SC shall execute the
;		command, increment the SC Valid Command Counter and issue an
;		event message.
;   SC2000.2	SC shall accept maximum of <PLATFORM_DEFINED> commands per ATS.
;   SC2000.3	SC shall accept a variable numner of variable length commands
;		from each ATS. Each ATS command shall contain:
;		a) A command number.
;		b) A time tag denoting the time at which to execute the
;		   command.
;		c) A variable length command.
;   SC2001	Upon receipt of a table update indication for an ATS table, SC
;		shall validate the following for the ATS table:
;		a) Duplicate command numbers
;		b) Invalid command lengths
;		c) Commands that run off the end of the table
;   SC2002.2	SC shall accept variable length packed RTS commands within the
;		<PLATFORM_DEFINED> byte relative time-tagged sequences.
;   SC2002.3	Each individual command within the sequence shall consist of:
;		a) A time tag with one second resolution.
;		b) A variable length command, with a maximum length of
;		   <PLATFORM_DEFINED> bytes.
;   SC2003	Upon receipt of a table update indication for an RTS table, SC
;		shall set the RTS status to DISABLED.
;   SC3000	Upon receipt of a Start ATS command, SC shall start the command-
;		specified ATS provided all of the following conditions are
;		satisfied:
;		a) The command-specified ATS table identification is
;		   valid.
;		b) The ATS table contains at least one command.
;		c) Neither of the two ATS tables is currently executing.
;   SC4000	Upon receipt of a Start RTS command, SC shall start the
;		command-specified RTS provided all of the following conditions
;		are met:
;		a) The command-specified RTS is not currently executing.
;		b) The RTS table is enabled.
;		c) The RTS table has been Loaded.
;   SC4000.1	If conditions are met, SC shall issue an event message
;		indicating the RTS started if the RTS number is less than
;		<PLATFORM_DEFINED> RTS number.
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
;		  x. The word offset within the RTS for the most recen RTS
;		     command which failed to dispatch
;		  y. ATS Continue-On-Failure status - DISABLED
;                 z. The last append ATS 
;                aa. The last ATS Append Table command count
;                bb. The last appended count
;   SC9004	Upon a power-on reset, SC shall start RTS #1.
;   SC9005	Upon a processor reset, SC shall start RTS #2.
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The SC commands and telemetry items exist in the GSE database.
;	The display pages exist for the SC Housekeeping.
;	The SC Test application (TST_SC) exists.
;	The SC Application is compiled with the Save Critical Data parameter
;	set to NO.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	02/04/09	Walt Moleski	Original Procedure.
;       01/24/11        Walt Moleski    Updated for SC 2.1.0.0
;       08/31/11        Walt Moleski    Updated for SC 2.2.0.0
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
#define SC_20022	4
#define SC_20023	5
#define SC_2003		6
#define SC_3000		7
#define SC_4000		8
#define SC_40001	9
#define SC_4004		10
#define SC_8000		11
#define SC_9000		12
#define SC_9004		13
#define SC_9005		14

global ut_req_array_size = 14
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SC_1004", "SC_2000.2", "SC_2000.3", "SC_2001", "SC_2002.2", "SC_2002.3", "SC_2003", "SC_3000", "SC_4000", "SC_4000.1", "SC_4004", "SC_8000", "SC_9000", "SC_9004", "SC_9005" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
LOCAL cmdCtr, errcnt
LOCAL atsPktId,atsAppId
LOCAL rtsPktId,rtsAppId
local SCAppName = "SC"
local ramDir = "RAM:0"
local ATSATblName = SCAppName & "." & SC_ATS_TABLE_NAME & "1"
local RTS3TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "003"
local RTS4TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "004"

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
write ";  Step 1.3: Create and upload the table load file for RTS #1 and #2.  "
write ";***********************************************************************"
s $sc_$cpu_sc_loadrts1()
wait 2

s $sc_$cpu_sc_loadrts2()
wait 2

write ";***********************************************************************"
write ";  Step 1.4: Start the Stored Command (SC) and Test Applications. "
write ";***********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("1.4")
wait 5

write ";***********************************************************************"
write ";  Step 1.5: Verify that the SC Housekeeping telemetry packet is being "
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
     ($SC_$CPU_SC_RTSCmdCtr = 0) AND ($SC_$CPU_SC_RTSErrCtr = 1) then
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
  disableStatusVal = x'FFFE'
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
write ";  Step 1.6: Enable DEBUG Event Messages "
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
write ";  Step 1.7: Verify that RTS #1 has been started.                       "
write ";***********************************************************************"
;; Check the global variable to indicate if RTS #1 started
if (rts001_started = TRUE) then
  write "<*> Passed (9004) - RTS #1 started."
  ut_setrequirements SC_9004, "P"
else
  write "<!> Failed (9004) - RTS #1 did not start on power-on reset."
  ut_setrequirements SC_9004, "F"
endif

write ";***********************************************************************"
write ";  Step 2.0: Processor Reset Test."
write ";***********************************************************************"
write ";  Step 2.1: Create and load a valid table into ATS A. "
write ";***********************************************************************"
;; Get the current MET
local TAITime = hkPktId & "ttime"
local UTCTime = hkPktId & "stime"
local currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
local firstCmdMET = currentMET + 240
local lastCmdMET = currentMET + 263

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

local work = %env("WORK")
;; Need to check if the .scs file used below exists. If not, end the proc
local filename = work & "/image/$sc_$cpu_atsa_load9.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step3_0
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load9" A {currentMET}
s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load9","$cpu_ats_a_load10")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load10", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
endif

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";***********************************************************************"
write ";  Step 2.2: Send the Validate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS A Table validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
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
write ";  Step 2.3: Send the Activate command for ATS A. "
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

ut_tlmwait $SC_$CPU_num_found_messages, 1
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

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_3","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Create and load a valid table into several RTS tables. " 
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts3_load2.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step3_0
endif

;; Using the SCP utilities, compile and build the RTS 3 load file
compile_rts "$sc_$cpu_rts3_load2" 3
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts3_load2","$cpu_rts003_load")

;; Using the SCP utilities, compile and build the RTS 4 load file
compile_rts "$sc_$cpu_rts4_load2" 4
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts4_load2","$cpu_rts004_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

start load_table ("$cpu_rts003_load", "$CPU")
start load_table ("$cpu_rts004_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #3 & 4 sent successfully."
else
  write "<!> Failed - Load command for RTS #3 & 4 did not execute successfully."
endif

if ($SC_$CPU_num_found_messages = 2) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";*********************************************************************"
write ";  Step 2.5: Validate RTS #3 & 4 loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS3TblName
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS4TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 & 4 validate command sent."
  if ($SC_$CPU_num_found_messages = 2) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #3 & 4 validation failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
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
write ";  Step 2.6: Send the Table Services Activate command.   "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS3TblName
/$SC_$CPU_TBL_ACTIVATE ATableName=RTS4TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #3 & 4 commands sent properly."
else
  write "<!> Failed - Activate RTS #3 & 4 commands."
endif

ut_tlmwait $SC_$CPU_num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 & 4 Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #3 & 4 update failed. Event Msg not received for activate command."
endif

;; Check the Disable Status for RTS #3 & 4
local bit3Mask = 4
local bit4Mask = 8
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
local rts3Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
local rts4Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
;;local rts4Status = %and($SC_$CPU_SC_RTSDISABLESTATUS[1],bit4Mask)
write "--RTS 3 status = ",rts3Status
write "--RTS 4 status = ",rts4Status
;;if ((rts3Status = bit3Mask) AND (rts4Status = bit4Mask)) then
if ((rts3Status = 1) AND (rts4Status = 1)) then
  write "<*> Passed (2003) - Disable Status indicates RTS #3 & 4 are disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #3 & 4 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS4TblName,"A","$cpu_rts4_tbl2_6","$CPU",rtsPktId)

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Enable RTS #3 & 4. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG",1

cmdCtr = $SC_$CPU_SC_CMDPC + 2
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=3
/$SC_$CPU_SC_EnableRTS RTSID=4

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
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check the Disable Status fields
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
rts3Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
rts4Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--RTS 3 status = ",rts3Status
write "--RTS 4 status = ",rts4Status
if ((rts3Status = 0) AND (rts4Status = 0)) then
  write "<*> Passed - Disable Status indicates RTS #3 & 4 are enabled."
else
  write "<!> Failed - Disable Status for RTS #3 & 4 indicates disabled after an enable command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Send the Start command for ATS A.                         "
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
write ";  Step 2.9: Send the Start command for RTS #3 & 4. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 2
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=3
wait 1
/$SC_$CPU_SC_StartRTS RTSID=4

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS commands sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS commands did not increment CMDPC properly."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd for each RTS."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," was not rcv'd for each RTS. Expected 2 msgs. Rcv'd ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

write ";**********************************************************************"
write ";  Step 2.10: Save the appropriate data items in order to determine if"
write ";  any data was saved across the reset. "
write ";**********************************************************************"
;; This step is not needed since the HK parameters should be set to their
;; default values. If any value is not the default, the test fails.
write "-- This test validates that nothing is saved across a reset."
write "-- Thus, if any of the HK items are not set to their default value, this test fails."

write ";*********************************************************************"
write ";  Step 2.11: - Send the Processor Reset command.             "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";***********************************************************************"
write ";  Step 2.12: Start the Stored Command (SC) and Test Applications. "
write ";***********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("2.12")
wait 5

write ";***********************************************************************"
write ";  Step 2.13: Enable DEBUG Event Messages "
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
write ";  Step 2.14: Verify that the SC Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements SC_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements SC_8000, "F"
endif

;; Check the HK tlm items to see if they are initialized properly
hkGood = FALSE
freeByteCount = SC_ATS_BUFF_SIZE * 2
RTSStatusSize = SC_NUMBER_OF_RTS/16

;; Note the rts001_started global variable is used for RTS #2 in this case
if (rts001_started = TRUE) then
  if ($SC_$CPU_SC_CMDPC = 2) AND ($SC_$CPU_SC_CMDEC = 0) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 1) AND ($SC_$CPU_SC_RTSActvErr = 0) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 3) AND ($SC_$CPU_SC_RTSErrCtr = 0) then
    hkGood = TRUE
  endif
else
  if ($SC_$CPU_SC_CMDPC = 0) AND ($SC_$CPU_SC_CMDEC = 1) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 0) AND ($SC_$CPU_SC_RTSActvErr = 1) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 0) AND ($SC_$CPU_SC_RTSErrCtr = 1) then
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
disableStatusVal = x'FFFD'
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
write ";  Step 2.15: Verify that RTS #1 has been started.                   "
write ";***********************************************************************"
;; Check the global variable to indicate if RTS #1 started
if (rts001_started = TRUE) then
  write "<*> Passed (9005) - RTS #2 started."
  ut_setrequirements SC_9005, "P"
else
  write "<!> Failed (9005) - RTS #2 did not start on processor reset."
  ut_setrequirements SC_9005, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_15","$CPU",atsPktId)

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS4TblName,"A","$cpu_rts4_tbl2_15","$CPU",rtsPktId)

wait 5

step3_0:
write ";***********************************************************************"
write ";  Step 3.0: Application Reset Test."
write ";***********************************************************************"
write ";  Step 3.1: Create & Load a valid image into ATS A. The same commands  "
write ";  from Step 2.1 will be used. All that needs to be updated are the "
write ";  command times so that the are in the future. "
write ";***********************************************************************"
;; Get the current MET
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
firstCmdMET = currentMET + 240
lastCmdMET = currentMET + 263

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
local filename = work & "/image/$sc_$cpu_atsa_load9.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_0
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load9" A {currentMET}
s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load9","$cpu_ats_a_load10")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO",1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load10", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
endif

if ($SC_$CPU_num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";***********************************************************************"
write ";  Step 3.2: Send the Validate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS A Table validate command sent."
  if ($SC_$CPU_num_found_messages = 1) then
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
write ";  Step 3.3: Send the Activate command for ATS A. "
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

ut_tlmwait $SC_$CPU_num_found_messages, 1
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

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl3_3","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 3.4: Load the tables created in Step 2.4 into RTS #3 & 4."
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

start load_table ("$cpu_rts003_load", "$CPU")
start load_table ("$cpu_rts004_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #3 & 4 sent successfully."
else
  write "<!> Failed - Load command for RTS #3 & 4 did not execute successfully."
endif

if ($SC_$CPU_num_found_messages = 2) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

write ";*********************************************************************"
write ";  Step 3.5: Validate RTS #3 & 4 loaded above. "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS3TblName
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS4TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 & 4 validate command sent."
  if ($SC_$CPU_num_found_messages = 2) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #3 & 4 validation failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
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
write ";  Step 3.6: Send the Table Services Activate command.   "
write ";*********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS3TblName
/$SC_$CPU_TBL_ACTIVATE ATableName=RTS4TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #3 & 4 commands sent properly."
else
  write "<!> Failed - Activate RTS #3 & 4 commands."
endif

ut_tlmwait $SC_$CPU_num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 & 4 Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #3 & 4 update failed. Event Msg not received for activate command."
endif

;; Check the Disable Status for RTS #3 & 4
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
rts3Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
rts4Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--RTS 3 status = ",rts3Status
write "--RTS 4 status = ",rts4Status
if ((rts3Status = 1) AND (rts4Status = 1)) then
  write "<*> Passed (2003) - Disable Status indicates RTS #3 & 4 are disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #3 & 4 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,RTS3TblName,"A","$cpu_rts3_tbl3_7","$CPU",rtsPktId)

wait 5

write ";*********************************************************************"
write ";  Step 3.7: Enable RTS #3 & 4. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 2
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=3
/$SC_$CPU_SC_EnableRTS RTSID=4

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
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check the Disable Status for RTS #3 & 4
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
rts3Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
rts4Status = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--RTS 3 status = ",rts3Status
write "--RTS 4 status = ",rts4Status
if ((rts3Status = 0) AND (rts4Status = 0)) then
  write "<*> Passed - Disable Status indicates RTS #3 & 4 are enabled."
else
  write "<!> Failed - Disable Status for RTS #3 & 4 indicates disabled after an enable command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.8: Send the Start command for ATS A.                         "
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
write ";  Step 3.9: Send the Start command for RTS #3 & 4. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 2
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=3
wait 1
/$SC_$CPU_SC_StartRTS RTSID=4

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS commands sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS commands did not increment CMDPC properly."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd for each RTS."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," was not rcv'd for each RTS. Expected 2 msgs. Rcv'd ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

write ";**********************************************************************"
write ";  Step 3.10: Save the appropriate data items in order to determine if"
write ";  any data was saved across the reset. "
write ";**********************************************************************"
;; This step is not needed since the HK parameters should be set to their
;; default values. If any value is not the default, the test fails.
write "-- This test validates that nothing is saved across a reset."
write "-- Thus, if any of the HK items are not set to their default value, this test fails."

write ";*********************************************************************"
write ";  Step 3.11: - Send the commands to stop the SC and TST_SC apps. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_DELETEAPP Application="TST_SC"
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_SC app stop command sent properly."
else
  write "<!> Failed - TST_SC app stop command did not increment CMDPC."
endif

wait 5

cmdCtr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_DELETEAPP Application=SCAppName
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC app stop command sent properly."
else
  write "<!> Failed - SC app stop command did not increment CMDPC."
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.12: Start the Stored Command (SC) and Test Applications. "
write ";***********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("3.12")
wait 5

write ";***********************************************************************"
write ";  Step 3.13: Enable DEBUG Event Messages "
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
write ";  Step 3.14: Verify that the SC Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements SC_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements SC_8000, "F"
endif

;; Check the HK tlm items to see if they are initialized properly
hkGood = FALSE
freeByteCount = SC_ATS_BUFF_SIZE * 2
RTSStatusSize = SC_NUMBER_OF_RTS/16

;; Note the rts001_started global variable is used for RTS #2 in this case
if (rts001_started = TRUE) then
  if ($SC_$CPU_SC_CMDPC = 2) AND ($SC_$CPU_SC_CMDEC = 0) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 1) AND ($SC_$CPU_SC_RTSActvErr = 0) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 3) AND ($SC_$CPU_SC_RTSErrCtr = 0) then
    hkGood = TRUE
  endif
else
  if ($SC_$CPU_SC_CMDPC = 0) AND ($SC_$CPU_SC_CMDEC = 1) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 0) AND ($SC_$CPU_SC_RTSActvErr = 1) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 0) AND ($SC_$CPU_SC_RTSErrCtr = 1) then
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
disableStatusVal = x'FFFD'
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
write ";  Step 3.15: Verify that RTS #1 has been started.                   "
write ";***********************************************************************"
;; Check the global variable to indicate if RTS #1 started
if (rts001_started = TRUE) then
  write "<*> Passed (9004) - RTS #1 started."
  ut_setrequirements SC_9004, "P"
else
  write "<!> Failed (9004) - RTS #1 did not start on initialization."
  ut_setrequirements SC_9004, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl3_15","$CPU",atsPktId)

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS4TblName,"A","$cpu_rts4_tbl3_15","$CPU",rtsPktId)

wait 5

step4_0:
write ";*********************************************************************"
write ";  Step 4.0: SC App startup without RTS001 Load File Tests.             "
write ";*********************************************************************"
write ";  Step 4.1: Remove the table load file from $CPU. "
write ";*********************************************************************"
local rtsFileName = SC_RTS_FILE_NAME
local slashLoc = %locate(rtsFileName,"/")

;; loop until all slashes are found
while (slashLoc <> 0) do
  rtsFileName = %substring(rtsFileName,slashLoc+1,%length(rtsFileName))
  slashLoc = %locate(rtsFileName,"/")
enddo

;; Remove the Processor Reset file
local rts1FileName = %lower(rtsFileName) & "001.tbl"
local rts2FileName = %lower(rtsFileName) & "002.tbl"
write "==> RTS #1 Load file name = '",rts1FileName,"'"
write "==> RTS #2 Load file name = '",rts2FileName,"'"

;; Remove the files
s ftp_file ("CF:0/apps", rts1FileName, rts1FileName,"$CPU","R")
s ftp_file ("CF:0/apps", rts2FileName, rts2FileName,"$CPU","R")

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Send the Processor Reset command.             "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";***********************************************************************"
write ";  Step 4.3: Start the Stored Command (SC) and Test Applications. "
write ";***********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("4.3")
wait 5

write ";***********************************************************************"
write ";  Step 4.4: Enable DEBUG Event Messages "
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
write ";  Step 4.5: Verify that the SC Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements SC_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements SC_8000, "F"
endif

;; Check the HK tlm items to see if they are initialized properly
hkGood = FALSE
freeByteCount = SC_ATS_BUFF_SIZE * 2
RTSStatusSize = SC_NUMBER_OF_RTS/16

if (rts001_started = TRUE) then
  ;; set the disable status for the first value
  disableStatusVal = x'FFFE'
  if ($SC_$CPU_SC_CMDPC = 1) AND ($SC_$CPU_SC_CMDEC = 0) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 1) AND ($SC_$CPU_SC_RTSActvErr = 0) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 3) AND ($SC_$CPU_SC_RTSErrCtr = 0) then
    hkGood = TRUE
  endif
else
  ;; set the disable status for the first value
  disableStatusVal = x'FFFF'
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

write ";*********************************************************************"
write ";  Step 4.6: Send the commands to stop the SC and TST_SC apps. "
write ";*********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_DELETEAPP Application="TST_SC"
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_SC app stop command sent properly."
else
  write "<!> Failed - TST_SC app stop command did not increment CMDPC."
endif

wait 5

cmdCtr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_DELETEAPP Application=SCAppName
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC app stop command sent properly."
else
  write "<!> Failed - SC app stop command did not increment CMDPC."
endif

wait 10

write ";***********************************************************************"
write ";  Step 4.7: Start the Stored Command (SC) and Test Applications. "
write ";***********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("4.7")
wait 5

write ";***********************************************************************"
write ";  Step 4.8: Enable DEBUG Event Messages "
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
write ";  Step 4.9: Verify that the SC Housekeeping telemetry packet is being "
write ";  generated and the appropriate items are initialized to zero (0). "
write ";***********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements SC_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements SC_8000, "F"
endif

;; Check the HK tlm items to see if they are initialized properly
hkGood = FALSE
freeByteCount = SC_ATS_BUFF_SIZE * 2
RTSStatusSize = SC_NUMBER_OF_RTS/16

if (rts001_started = TRUE) then
  ;; set the disable status for the first value
  disableStatusVal = x'FFFE'
  if ($SC_$CPU_SC_CMDPC = 1) AND ($SC_$CPU_SC_CMDEC = 0) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 1) AND ($SC_$CPU_SC_RTSActvErr = 0) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 3) AND ($SC_$CPU_SC_RTSErrCtr = 0) then
    hkGood = TRUE
  endif
else
  ;; set the disable status for the first value
  disableStatusVal = x'FFFF'
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

write ";*********************************************************************"
write ";  Step 4.10: Send the file back up to $CPU. "
write ";*********************************************************************"
;; ftp the files back to the appropriate location
s ftp_file ("CF:0/apps", rts1FileName, rts1FileName,"$CPU","P")
s ftp_file ("CF:0/apps", rts2FileName, rts2FileName,"$CPU","P")

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Clean-up - Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_sc_resetnocds"
write ";*********************************************************************"
ENDPROC
