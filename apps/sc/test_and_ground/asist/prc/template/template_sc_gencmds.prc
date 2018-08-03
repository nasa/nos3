PROC $sc_$cpu_sc_gencmds
;*******************************************************************************
;  Test Name:  sc_gencmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Stored Command (SC) general commands
;	function properly. The NOOP and Reset Counters commands will be tested.
;	Invalid versions of these commands will also be tested to ensure that
;	the SC application hanled these properly.
;
;  Requirements Tested
;    SC1000	Upon receipt of a No-Op command, SC shall increment the SC
;		Valid Command Counter and generate an event message.
;    SC1001	Upon receipt of a Reset command, SC shall reset the following
;		housekeeping variables to a value of zero:
;		  a. Valid Ground Command Counter
;		  b. Ground Command Rejected Counter
;		  c. ATS Command Counter
;		  d. ATS Command Error Counter
;		  e. RTS Command Counter
;		  f. RTS Command Error Counter
;		  g. Number of RTS Started Counter
;		  h. Number of RTS Started Error Counter
;    SC1002	For all SC commands, if the length contained in the message
;		header is not equal to the expected length, SC shall reject the
;		command and issue an event message.
;    SC1004	If SC accepts any command as valid, SC shall execute the
;		command, increment the SC Valid Command Counter and issue an
;		event message.
;    SC1005	If SC rejects any command, SC shall abort the command execution,
;		increment the SC Command Rejected Counter and issue an event
;		message.
;    SC8000	SC shall generate a housekeeping message containing the
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
;    SC9000	Upon a power-on or processor reset, SC shall initialize the
;		the following data to Zero:
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
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The SC commands and telemetry items exist in the GSE database.
;	The display pages exist for the SC Housekeeping.
;	A SC Test application (TST_SC) exists in order to fully test the SC
;		Application.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	01/12/09	Walt Moleski	Original Procedure.
;	01/25/11	Walt Moleski	Updated for SC 2.1.0.0
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
#include "to_lab_events.h"
#include "sc_platform_cfg.h"
#include "sc_events.h"
#include "cfe_tbl_events.h"
#include "tst_sc_events.h"

%liv (log_procedure) = logging

#define SC_1000		0
#define SC_1001		1
#define SC_1002		2
#define SC_1004		3
#define SC_1005		4
#define SC_8000		5
#define SC_9000		6

global ut_req_array_size = 6
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SC_1000", "SC_1001", "SC_1002", "SC_1004", "SC_1005", "SC_8000", "SC_9000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
local SCAppName = "SC"

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

write ";***********************************************************************"
write ";  Step 1.3: Create and upload the table load file for RTS #1.  "
write ";***********************************************************************"
s $sc_$cpu_sc_loadrts1()
wait 5

write ";***********************************************************************"
write ";  Step 1.4:  Start the Stored Command (SC) and Test Applications.     "
write ";***********************************************************************"
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
    write "RTS Exe Status[",i,"] = ",%hex($SC_$CPU_SC_RTSExeStatus[i].RTSStatusEntry.AllStatus,4)
    write "RTS Dis Status[",i,"] = ", %hex($SC_$CPU_SC_RTSDisableStatus[i].RTSStatusEntry.AllStatus,4)
  enddo

  ut_setrequirements SC_9000, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the SC and CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=SCAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";***********************************************************************"
write ";  Step 2.0: Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Send the NO-OP command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_NOOP_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the NO-OP Command
/$SC_$CPU_SC_NOOP

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1004) - SC NO-OP command sent properly."
  ut_setrequirements SC_1000, "P"
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1000;1004) - SC NO-OP command did not increment CMDPC."
  ut_setrequirements SC_1000, "F"
  ut_setrequirements SC_1004, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1004) - Expected Event Msg ",SC_NOOP_INF_EID," rcv'd."
  ut_setrequirements SC_1000, "P"
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1000;1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_NOOP_INF_EID,"."
  ut_setrequirements SC_1000, "F"
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the NO-OP command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_SC_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18A9c000000200B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000200B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000200B0"
endif

ut_sendrawcmd "$SC_$CPU_SC", (rawcmd)

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005) - Command Rejected Counter incremented."
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Utilizing the TST_SC application, send the command that  "
write ";  will set all the counters that get reset to zero (0) by the Reset  "
write ";  command to a non-zero value."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_SC", TST_SC_SET_COUNTERS_INF_EID, "INFO", 1

/$SC_$CPU_TST_SC_SetCounters

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_SC_SET_COUNTERS_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_SC_SET_COUNTERS_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Verify that all the counters are non-zero and send the   "
write ";  Reset command if so.                                               "
write ";***********************************************************************"
;; Check the HK telemetry
if ($SC_$CPU_SC_CMDPC > 0) AND ($SC_$CPU_SC_CMDEC > 0) AND ;;
   ($SC_$CPU_SC_RTSActvCtr > 0) AND ($SC_$CPU_SC_RTSActvErr > 0) AND ;;
   ($SC_$CPU_SC_ATSCmdCtr > 0) AND ($SC_$CPU_SC_ATSErrCtr > 0) AND ;;
   ($SC_$CPU_SC_RTSCmdCtr > 0) AND ($SC_$CPU_SC_RTSErrCtr > 0) THEN
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RESET_DEB_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_SC_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_SC_ResetCtrs
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1004) - Expected Event Msg ",SC_RESET_DEB_EID," rcv'd."
    ut_setrequirements SC_1001, "P"
    ut_setrequirements SC_1004, "P"
  else
    write "<!> Failed (1001;1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RESET_DEB_EID,"."
    ut_setrequirements SC_1001, "F"
    ut_setrequirements SC_1004, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_SC_CMDPC = 0) AND ($SC_$CPU_SC_CMDEC = 0) AND ;;
     ($SC_$CPU_SC_RTSActvCtr = 0) AND ($SC_$CPU_SC_RTSActvErr = 0) AND ;;
     ($SC_$CPU_SC_ATSCmdCtr = 0) AND ($SC_$CPU_SC_ATSErrCtr = 0) AND ;;
     ($SC_$CPU_SC_RTSCmdCtr = 0) AND ($SC_$CPU_SC_RTSErrCtr = 0) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements SC_1001, "P"
  else
    write "<!> Failed (1001) - Counters did not reset to zero."
    ut_setrequirements SC_1001, "F"
  endif
else
  write "<!> Reset command not sent because at least 1 counter is set to 0."
endif

;; Write out the counters for verification
write "CMDPC           = ", $SC_$CPU_SC_CMDPC
write "CMDEC           = ", $SC_$CPU_SC_CMDEC
write "ATS Command Ctr = ", $SC_$CPU_SC_ATSCmdCtr
write "ATS Command EC  = ", $SC_$CPU_SC_ATSErrCtr
write "RTS Command Ctr = ", $SC_$CPU_SC_RTSCmdCtr
write "RTS Command EC  = ", $SC_$CPU_SC_RTSErrCtr
write "RTSs Started    = ", $SC_$CPU_SC_RTSActvCtr
write "RTSs Started EC = ", $SC_$CPU_SC_RTSActvErr

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Reset command with an invalid length.             "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_SC_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18A9c000000201B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000201B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000201B0"
endif

ut_sendrawcmd "$SC_$CPU_SC", (rawcmd)

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1005) - Command Rejected Counter incremented."
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send an invalid command.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_INVLD_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18A9c0000001AA"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c0000001AA"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c0000001AA"
endif

ut_sendrawcmd "$SC_$CPU_SC", (rawcmd)

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements SC_1005, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_INVLD_CMD_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7: Send the SC Housekeeping request with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_SC_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18AAc000000201B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19AAc000000201B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AAAc000000201B0"
endif

ut_sendrawcmd "$SC_$CPU_SC", (rawcmd)

;;NOTE: May have to replace 1005 with 1002 below.
;;      Not sure if the CMDEC increments either since this is a REQUEST not a command
ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Command Rejected Counter incremented."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements SC_1005, "F"
endif

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Clean-up - Send the Processor Reset command.             "
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

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_gencmds"
write ";*********************************************************************"
ENDPROC
