PROC $sc_$cpu_sc_atsfunc
;*******************************************************************************
;  Test Name:  sc_atsfunc
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Stored Command (SC) application supports
;	the execution of Absolute Time Sequences (ATS) as well as detecting
;	errors with the commands contained in the ATS. Invalid versions of 
;	commands will also be tested to ensure that the SC application handles
;	these properly.
;
;  Requirements Tested
;   SC1002	For all SC commands, if the length contained in the message
;		header is not equal to the expected length, SC shall reject the
;		command and issue an event message.
;		Valid Command Counter and generate an event message.
;   SC1004	If SC accepts any command as valid, SC shall execute the
;		command, increment the SC Valid Command Counter and issue an
;		event message.
;   SC1005	If SC rejects any command, SC shall abort the command execution,
;		increment the SC Command Rejected Counter and issue an event
;		message.
;   SC2000	SC shall allocate <PLATFORM_DEFINED> bytes of storage for each
;		of two (2) Absolute Time-tagged command sequence (ATS) tables.
;   SC2000.1	SC shall resolve time to 1 second for Absolute Time Command
;		Sequences (ATS).
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
;   SC2004	SC shall execute commands in the ATS table in ascending order,
;		based upon the time-tag of the commands, regardless of the order
;		in which the commands are stored in the ATS table.
;   SC2007	SC shall define <PLATFORM_DEFINED> bytes of storage for an ATS
;		Append Table
;   SC2007.1	The ATS Append Table format is the same as the ATS tables
;   SC2008	Upon receipt of an Apply ATS Append Table command, SC shall
;		append the ATS Append Table contents to the command-specified
;		ATS Table
;   SC2008.1	The Append command may be performed on either ATS table
;   SC2008.2	If the number of entries in the ATS and ATS Append Table exceed
;		the size of the ATS table, SC shall reject the Apply ATS Append
;		Table command.
;   SC2008.3	The Apply ATS Append Table command shall add all ATS Append
;		Table data to the end of the command-specified ATS table
;   SC2008.4	The Apply ATS Append Table command shall provide the ability to
;		add new commands to the ATS buffer
;   SC2008.5	The Apply ATS Append Table command shall provide the ability to
;		modify existing commands in the ATS buffer
;   SC2008.6	Upon completion of the Apply ATS Append Table command, SC shall
;		issue an info event indicating the number of commands that were
;		appended to the ATS
;   SC2008.7	Upon completion of the Apply ATS Append Table command, SC shall
;		recompute the command execution sequence
;   SC2009	Upon receit of a table update indication for an ATS Append
;		table, SC shall validate the following for the ATS table:
;		  a) Duplicate command numbers
;		  b) Invalid command lengths
;		  c) Commands that run off the end of the table
;		  d) Command number
;   SC3000	Upon receit of a Start ATS command, SC shall start the command-
;		specified ATS provided all of the following conditions are
;		satisfied:
;		  a) The command-specified ATS table identification is valid.
;		  b) The ATS table contains at least one command.
;		  c) Neither of the two ATS tables is currently executing.
;   SC3000.1	SC shall mark all ATS commands with time less-than the current
;		time as SKIPPED and an event message shall be generated.
;   SC3000.3	Prior to the dispatch of each individual ATS command, SC shall
;		verify the Data Integrity Check Value of the stored command.
;   SC3000.3.1	For any ATS command which fails the Data Integrity Check Value,
;		the following shall be performed:
;		  a) Discard the command
;		  b) Mark the command with DATA INTEGRITY CHECK VALUE
;		   VERIFICATION FAILED
;		  c) Issue an event message
;   SC3000.3.2	If the Continuation Execution of ATS On Error Flag is Disabled,
;		SC shall terminate the execution of the ATS.
;   SC3001	Upon receipt of a Stop ATS command, SC shall:
;		  a) Stop processing the currently executing ATS
;		  b) Set the state of that ATS to IDLE
;   SC3001.1	If no ATS is executing, SC shall increment the Valid Command
;		Counter.
;   SC3002	Upon receipt of a Switch ATS command, SC shall:
;		  a) Terminate the processing of the current ATS table after
;		     processing all of the commands within the current second.
;		  b) Start processing of the alternate ATS table.
;   SC3002.1	SC shall begin processing the first ATS command after the next
;		1 second occurs containing a time which is greater-than-or-
;		equal-to the current time.
;		event message indicating that the RTS completed.
;   SC3002.2	SC shall mark all ATS commands with time less-than the current
;		time as SKIPPED and an event message shall be generated.
;   SC3002.3	If the alternate ATS table has not been loaded, SC shall reject
;		the command.
;   SC3002.4	If the Switch command is located within an ATS, SC shall
;		immediately execute the switch command.
;		of the command-specified RTS to Enabled.
;   SC3003	Upon receipt of a Jump Command, SC shall transfer execution to
;		the command within the currently executing ATS table whose
;		time-tag is equal to a command specified time value.
;   SC3003.1	If no command exists that is equal to the command-specified jump
;		time, SC shall wait for the first command after the jump time.
;   SC3003.2	If the command-specified time value is less-than or equal-to the
;		current time, SC shall skip all of the commands in the past.
;   SC3003.2.1	The status of all ATS commands skipped over as a result of the
;		Jump command shall be marked as SKIPPPED and an event message
;		shall be generated.
;   SC3003.2.2	If all the commands in the ATS have been skipped, SC shall stop
;		the ATS and issue an event message.
;   SC3003.3	If neither of the two ATS tables are currently executing, SC
;		shall reject the Jump command.
;   SC3003.4	If multiple commands exist that satisfy the Jump condition, the
;		commands shall be executed in ascending command number order
;		(as they exist in the ATS table).
;   SC3004	Upon receipt of an Enable Continuation Execution of ATS On Error
;		Command, SC shall set the Continuation Execution of ATS On Error
;		Flag to Enabled.
;   SC3005	Upon receipt of a Disable Continuation Execution of ATS On Error
;		Command, SC shall set the Continuation Execution of ATS On Error
;		Flag to Disabled.
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
;		  z. The last append ATS
;	         aa. The last ATS Append Table command count
;	         bb. The last appended count
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
;		  z. The last append ATS
;	         aa. The last ATS Append Table command count
;	         bb. The last appended count
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The SC commands and telemetry items exist in the GSE database.
;	The display pages exist for the SC Housekeeping.
;	The SC Test application (TST_SC) exists.
;
;  Assumptions and Constraints
;	The SC application is not compiled using the CDS. If it is, Steps 4.27
;	and 4.28 will fail.
;
;  Change History
;
;	Date		   Name		Description
;	01/21/09	Walt Moleski	Original Procedure.
;	01/24/11	Walt Moleski	Updated for SC 2.1.0.0 which is mainly
;					the ATS Append command.
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
#include "cfe_sb_events.h"
#include "to_lab_events.h"
#include "sc_platform_cfg.h"
#include "sc_msgdefs.h"
#include "sc_events.h"
#include "tst_sc_events.h"

%liv (log_procedure) = logging

#define SC_1002		0
#define SC_1004		1
#define SC_1005		2
#define SC_2000		3
#define SC_20001	4
#define SC_20002	5
#define SC_20003	6
#define SC_20004	7
#define SC_2001		8
#define SC_2004		9
#define SC_2007		10
#define SC_20071	11
#define SC_2008		12
#define SC_20081	13
#define SC_20082	14
#define SC_20083	15
#define SC_20084	16
#define SC_20085	17
#define SC_20086	18
#define SC_20087	19
#define SC_2009		20
#define SC_3000		21
#define SC_30001	22
#define SC_30003	23
#define SC_300031	24
#define SC_300032	25
#define SC_3001 	26
#define SC_30011	27
#define SC_3002		28
#define SC_30021	29
#define SC_30022	30
#define SC_30023	31
#define SC_30024	32
#define SC_3003		33
#define SC_30031	34
#define SC_30032	35
#define SC_300321	36
#define SC_300322	37
#define SC_30033	38
#define SC_30034	39
#define SC_3004		40
#define SC_3005		41
#define SC_8000		42
#define SC_9000		43

global ut_req_array_size = 43
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SC_1002", "SC_1004", "SC_1005", "SC_2000", "SC_2000.1", "SC_2000.2", "SC_2000.3", "SC_2000.4", "SC_2001", "SC_2004", "SC_2007", "SC_2007.1", "SC_2008", "SC_2008.1", "SC_2008.2", "SC_2008.3", "SC_2008.4", "SC_2008.5", "SC_2008.6", "SC_2008.7", "SC_2009", "SC_3000", "SC_3000.1", "SC_3000.3", "SC_3000.3.1", "SC_3000.3.2", "SC_3001", "SC_3001.1", "SC_3002", "SC_3002.1", "SC_3002.2", "SC_3002.3", "SC_3002.4",  "SC_3003", "SC_3003.1", "SC_3003.2", "SC_3003.2.1", "SC_3003.2.2", "SC_3003.3", "SC_3003.4", "SC_3004", "SC_3005", "SC_8000", "SC_9000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
LOCAL cmdCtr, errcnt
LOCAL atsPktId,atsAppId
LOCAL appendPktId,appendAppId
LOCAL atsCmdPktId, atsCmdAppId
local SCAppName = "SC"
local ramDir = "RAM:0"
local ATSATblName = SCAppName & "." & SC_ATS_TABLE_NAME & "1"
local ATSBTblName = SCAppName & "." & SC_ATS_TABLE_NAME & "2"
local ATSACmdTblName = SCAppName & "." & SC_ATS_CMD_STAT_TABLE_NAME & "1"
local ATSBCmdTblName = SCAppName & "." & SC_ATS_CMD_STAT_TABLE_NAME & "2"
local ATSAppendTblName = SCAppName & "." & SC_APPEND_TABLE_NAME
local currentMET
local work = %env("WORK")
local filename
local firstCmdMET,lastCmdMET,waitTime
local atsBTimes[6]
local jumpTime
local highWord,lowWord

;; Set the pkt and app Ids for the appropriate CPU
;; CPU1 is the default
atsPktId = "0FBB"
atsAppId = 4027
atsCmdPktId = "0F71"
atsCmdAppId = 3953
appendPktId = "0F79"
appendAppId = 3961

if ("$CPU" = "CPU2") then
  atsPktId = "0FD9"
  atsAppId = 4057
  atsCmdPktId = "0FDF"
  atsCmdAppId = 4063
  appendPktId = "0F87"
  appendAppId = 3975
elseif ("$CPU" = "CPU3") then
  atsPktId = "0FF9"
  atsAppId = 4089
  atsCmdPktId = "0F90"
  atsCmdAppId = 3984
  appendPktId = "0F99"
  appendAppId = 3993
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
page $SC_$CPU_SC_ATSCMD_TBL
page $SC_$CPU_SC_ATS_TBL
page $SC_$CPU_SC_ATS_APPEND_TBL

write ";***********************************************************************"
write ";  Step 1.3: Create and upload the table load file for RTS #1.  "
write ";***********************************************************************"
s $sc_$cpu_sc_loadrts1()

write ";***********************************************************************"
write ";  Step 1.4:  Start the Stored Command (SC) and Test Applications.      "
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

;; Now that the HK Packet ID is set, define the time variables
local TAITime = hkPktId & "ttime"
local UTCTime = hkPktId & "stime"

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
write ";  Step 2.0: ATS Valid Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Verify that both ATS tables indicate they are free.       "
write ";***********************************************************************"
if ($SC_$CPU_SC_FreeBytes[1] = freeByteCount) AND ;;
   ($SC_$CPU_SC_FreeBytes[2] = freeByteCount) then
  write "<*> Passed (2000) - Both ATS tables are empty and allocated to ",freeByteCount, " bytes."
  ut_setrequirements SC_2000, "P"
else
  write "<!> Failed (2000) - At least one ATS table is not empty or allocated to the desired size. ATS A has ",$SC_$CPU_SC_FREEBYTES[1], "bytes free and ATS B has ",$SC_$CPU_SC_FREEBYTES[2], " bytes free."
  ut_setrequirements SC_2000, "F"
endif

filename = work & "/image/ats_a_empty_ld"
if NOT file_exists(filename) then
  ;; Clear out the ATS Data (just in case)
  for i = 1 to SC_ATS_BUFF_SIZE do
    $SC_$CPU_SC_ATSDATA[i] = 0
  enddo

;; ******* Need to initialize a variable for the end data size
  ;; Create the empty ATS Table Load files
  s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Empty Table Load", "ats_a_empty_ld",ATSATblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[8000]")

  s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS B Empty Table Load", "ats_b_empty_ld",ATSBTblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[8000]")
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Create and load a valid table to ATS A.                   "
write ";***********************************************************************"
;; Get the current MET
write "-- TAITime => ", {TAITime}, " (hex) = ", %hex({TAITime},8)
write "-- UTCTime => ", {UTCTime}, " (hex) = ", %hex({UTCTime},8)

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_7
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load","$cpu_ats_a_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step2_7
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send the Table Services Validate command for ATS A.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_EID, "INFO", 3

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
  goto step2_7
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

;; Wait for the SC validation message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC ATS validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - SC ATS validation event Msg ",SC_VERIFY_ATS_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Send the Table Services Activate command for ATS A.  "
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
  goto step2_7
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

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_4","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Start command for ATS A.                         "
write ";***********************************************************************"
;; Setup the events for the next 2 steps. EID 1 => 2.6 EID 2 => 2.7
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_COMPL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_NOOP_INF_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_NOOP_INF_EID, "INFO", 4
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_NOOP_EID, "INFO", 5
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_ERR_SKP_DBG_EID, "DEBUG", 6

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
  goto step2_7
endif

;; Check for the command event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STARTATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check for the event message indicating the number of 
;; commands skipped when the ATS was started
ut_tlmwait $SC_$CPU_find_event[6].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID," rcv'd."
  ut_setrequirements SC_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID,"."
  ut_setrequirements SC_30001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.6: Verify that ATS A is being processed correctly.           "
write ";  Wait until execution completes.                               "
write ";***********************************************************************"
;; Verifies rqmts 2000.1 - 1 second resolution
;; 2004 - dispatch in ascending time order 1=CFE_TBL_NOOP 2=SC_NOOP 3=CFE_EVS_NOOP
;; These requirements can be verified by analyzing the log file
ut_setrequirements SC_20001, "A"
ut_setrequirements SC_2004, "A"

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

firstCmdMET = $SC_$CPU_SC_NEXTATSTIME
lastCmdMET = firstCmdMET + 10
write "-- LastCMDMET => ", lastCmdMET
waitTime = lastCmdMET - currentMET

;; Wait for the ATS Completed event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, {waitTime}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",SC_ATS_COMPL_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ATS_COMPL_INF_EID,"."
endif

;; if the commands executed, it was valid
if ($SC_$CPU_find_event[3].num_found_messages = 1) AND ;;
   ($SC_$CPU_find_event[4].num_found_messages = 1) AND ;;
   ($SC_$CPU_find_event[5].num_found_messages = 1) then
  write "<*> Passed - Expected Event Msgs rcv'd for ATS A commands."
else
  write "<!> Failed - At least 1 Event msg was not rcv'd for ATS A commands."
endif

wait 5

step2_7:
write ";***********************************************************************"
write ";  Step 2.7: Send the Disable Continuation of ATS On Error command.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_CONT_CMD_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_ContinueATS stop

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3005) - SC Continue Disable ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3005, "P"
else
  write "<!> Failed (1004;3005) - SC Continue Disable ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_CONT_CMD_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_CONT_CMD_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Send the Enable Continuation of ATS On Error command.      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_CONT_CMD_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_ContinueATS continue

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3004) - SC Continue Enable ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3004, "P"
else
  write "<!> Failed (1004;3004) - SC Continue Enable ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3004, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_CONT_CMD_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_CONT_CMD_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Create and load a new valid table to ATS A. This table just"
write ";  contains new times for the 3 original commands executed above. "
write ";***********************************************************************"
;; Get the current MET
currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load2.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_18
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load2" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load2","$cpu_ats_a_load2")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step2_18
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.10: Create and load a valid table to ATS B. Make sure that "
write ";  there are enough commands in this ATS in order to issue a Jump "
write ";  command to a valid location in ATS B. "
write ";***********************************************************************"
currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)," (dec) ", %dec(currentMET,10)

;; Set the times until each ATS B cmd executes
atsBTimes[1] = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs + 210
atsBTimes[2] = atsBTimes[1] + 5
atsBTimes[3] = atsBTimes[2] + 5
atsBTimes[4] = atsBTimes[3] + 50
atsBTimes[5] = atsBTimes[4] + 10
atsBTimes[6] = atsBTimes[5] + 10

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsb_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_18
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsb_load" B {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsb_load","$cpu_ats_b_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_b_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS B sent successfully."
else
  write "<!> Failed - Load command for ATS B did not execute successfully."
  goto step2_18
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

;; Print out the Cmd times for ATS B
for i = 1 to 6 do
  write "-- ATS B Cmd #", i, " MET = ", atsBTimes[i], " (hex) = ", %hex(atsBTimes[i],8)
enddo

wait 5

write ";***********************************************************************"
write ";  Step 2.11: Send the Table Services Validate command for ATS A & B.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_EID, "INFO", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSBTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Table validate commands sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 2) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Table validation commands failed."
  goto step2_18
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS Table Validations were successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS Table validations failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2001, "F"
endif

;; Wait for the SC validation message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC ATS validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - SC ATS validation event Msg ",SC_VERIFY_ATS_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Send the Table Services Activate command for ATS A & B. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSATblName
/$SC_$CPU_TBL_ACTIVATE ATableName=ATSBTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Table commands sent properly."
else
  write "<!> Failed - Activate ATS Table commands."
  goto step2_18
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2;2000.3) - ATS Tables Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
  ut_setrequirements SC_20003, "P"
else
  write "<!> Failed (2000.2;2000.3) - ATS Table updates failed. Event Msg not received for activate command."
  ut_setrequirements SC_20002, "F"
  ut_setrequirements SC_20003, "F"
endif

ut_tlmupdate $SC_$CPU_TBL_CMDPC

;; Check the Free Byte count for both tables
local atsAFreeBytes = freeByteCount - 42
local atsBFreeBytes = freeByteCount - 84
if ($SC_$CPU_SC_FreeBytes[1] = atsAFreeBytes) AND ;;
   ($SC_$CPU_SC_FreeBytes[2] = atsBFreeBytes) then
  write "<*> Passed (2000) - Both ATS tables report the proper free bytes."
  ut_setrequirements SC_2000, "P"
elseif ($SC_$CPU_SC_FreeBytes[1] <> atsAFreeBytes) then
  write "<!> Failed (2000) - ATS A does not report the proper bytes free = ",$SC_$CPU_SC_FREEBYTES[1]
  ut_setrequirements SC_2000, "F"
elseif ($SC_$CPU_SC_FreeBytes[2] <> atsBFreeBytes) then
  write "<!> Failed (2000) - ATS B does not report the proper bytes free = ",$SC_$CPU_SC_FREEBYTES[2]
  ut_setrequirements SC_2000, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_12","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.13: Send the Start command for ATS A.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_ERR_SKP_DBG_EID, "DEBUG", 2

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
  goto step2_18
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

;; Check for the event message indicating the number of 
;; commands skipped when the ATS was started
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID," rcv'd."
  ut_setrequirements SC_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID,"."
  ut_setrequirements SC_30001, "F"
endif

;; Set the time to wait until ATS A cmd #1 executes
lastCmdMET = $SC_$CPU_SC_NEXTATSTIME
write "-- Last Cmd MET => ", lastCmdMET, " (hex) = ", %hex(lastCmdMET,8)

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
waitTime = lastCmdMET - currentMET
if (waitTime > 0) then
  wait waitTime
endif

write ";***********************************************************************"
write ";  Step 2.14: Send the Switch command for ATS B.                      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_SWITCH_ATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{SCAppName},SC_ATS_ERR_SKP_DBG_EID, "DEBUG", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_ATS_SERVICE_SWTCH_INF_EID, "INFO", 3

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET before SWITCH => ", currentMET, " (hex) = ", %hex(currentMET,8)

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_SwitchATS

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3002) - SC Switch ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3002, "P"
else
  write "<!> Failed (1004;3000) - SC Switch ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3002, "F"
  goto step2_18
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_SWITCH_ATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_SWITCH_ATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check for the ATS Started event
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3002) - Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID," rcv'd."
  ut_setrequirements SC_3002, "P"
else
  write "<!> Failed (3002) - Expected Event Msg were not received. Expected ",SC_ATS_ERR_SKP_DBG_EID
  ut_setrequirements SC_3002, "F"
endif

;; Check for the ATS Switched event
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3002) - Expected Event Msg ",SC_ATS_SERVICE_SWTCH_INF_EID," rcv'd."
  ut_setrequirements SC_3002, "P"
else
  write "<!> Failed (3002) - Expected Event Msgs were not received. Expected ",SC_ATS_SERVICE_SWTCH_INF_EID
  ut_setrequirements SC_3002, "F"
endif

write ";***********************************************************************"
write ";  Step 2.15: Verify ATS B is executing and the appropriate command is  "
write ";  next to execute.   "
write ";***********************************************************************"
;; ATS B should start with the 1st command
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET after SWITCH => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to dump the ATS command table to view each command's status
s get_tbl_to_cvt (ramDir,ATSBCmdTblName,"A","$cpu_atsb_cmd2_15","$CPU",atsCmdPktId)

;; Command #1 should not indicate "Skipped"
if (p@$SC_$CPU_SC_ATSCMDStatus[1] <> "Skipped") then
  write "<*> Passed (3002.1;3002.2) - ATS B Command #1 Status is correct and is waiting to execute."
  ut_setrequirements SC_30021, "P"
  ut_setrequirements SC_30022, "P"
else
  write "<!> Failed (3002.1;3002.2) - ATS B Command #1 Status indicates 'Skipped'. This should be the first command to execute after the Switch."
  ut_setrequirements SC_30021, "F"
  ut_setrequirements SC_30022, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSBTblName,"A","$cpu_atsb_tbl2_15","$CPU",atsPktId)

;; Wait until the first command executes
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENTMET => ", currentMET, " (hex) = ", %hex(currentMET,8)
waitTime = $SC_$CPU_SC_NEXTATSTIME - currentMET
if (waitTime > 0) then
  wait waitTime
endif

write ";***********************************************************************"
write ";  Step 2.16: Send the Jump command with an appropriate time for ATS B."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_JUMP_ATS_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{SCAppName},SC_JUMP_ATS_SKIPPED_DBG_EID, "DEBUG", 2

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET before JUMP => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Set the Jump time to ATS B Command #4
jumpTime = atsBTimes[4]
write "-- JUMPing to time => ", jumpTime, " (hex) = ", %hex(jumpTime,8)

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_JumpATS newTime=jumpTime

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3003) - Expected Event Msg ",SC_JUMP_ATS_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3003, "P"
else
  write "<!> Failed (1004;3003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMP_ATS_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3003.2.1) - SC Jump Command Skipped Event Msg ",SC_JUMP_ATS_SKIPPED_DBG_EID," rcv'd."
  ut_setrequirements SC_300321, "P"
else
  write "<!> Failed (3003.2.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMP_ATS_SKIPPED_DBG_EID,"."
  ut_setrequirements SC_300321, "F"
endif

;; Need to dump the ATS command table to view each command's status
s get_tbl_to_cvt (ramDir,ATSBCmdTblName,"A","$cpu_atsb_cmd2_16","$CPU",atsCmdPktId)

;; At least Command #3 should indicate "Skipped"
if (p@$SC_$CPU_SC_ATSCMDStatus[3] = "Skipped") then
  write "<*> Passed (3003.2;3003.2.1) - ATS B Command #3 Status indicates 'Skipped'."
  ut_setrequirements SC_30032, "P"
  ut_setrequirements SC_300321, "P"
else
  write "<!> Failed (3003.2) - ATS B Command #3 Status does not indicate 'Skipped' as expected."
  ut_setrequirements SC_30032, "F"
  ut_setrequirements SC_300321, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.17: Send the Stop command for ATS B.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPATS_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StopATS

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - SC Stop ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3001, "P"
else
  write "<!> Failed (1004;3001) - SC Stop ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STOPATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

step2_18:
write ";***********************************************************************"
write ";  Step 2.18: Create and load a valid table to ATS A. This table just "
write ";  contains new times for the 3 original commands executed previously. "
write ";***********************************************************************"
;; Get the current MET
currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load2.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_27
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load2" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load2","$cpu_ats_a_load2")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step2_27
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.19: Send the Table Services Validate command for ATS A."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_EID, "INFO", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Table validate command sent for ATS A."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Table validation commands failed."
  goto step2_27
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS Table validations failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2001, "F"
endif

;; Wait for the SC validation message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC ATS validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - SC ATS validation event Msg ",SC_VERIFY_ATS_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.20: Send the Table Services Activate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Table A command sent properly."
else
  write "<!> Failed - Activate ATS Table A command."
  goto step2_27
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
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

;; Check the Free Byte count for both tables
atsAFreeBytes = freeByteCount - 42
if ($SC_$CPU_SC_FreeBytes[1] = atsAFreeBytes) then
  write "<*> Passed (2000) - ATS A reports the proper free bytes."
  ut_setrequirements SC_2000, "P"
else
  write "<!> Failed (2000) - ATS A does not report the proper bytes free = ",$SC_$CPU_SC_FREEBYTES[1]
  ut_setrequirements SC_2000, "F"
endif

;; Dump the ATS A table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_20","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.21: Create and load a valid Append table for ATS A. This table"
write ";  replaces the last command in the original ATS and adds 2 additional "
write ";  commands. "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_append.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_27
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_append" A {currentMET}

s $sc_$cpu_load_ats_append("$sc_$cpu_atsa_append","$cpu_ats_a_append")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_append", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2007) - Load command for ATS Append sent successfully."
  ut_setrequirements SC_2007, "P"
else
  write "<!> Failed (2007) - Load command for ATS Append did not execute successfully."
  ut_setrequirements SC_2007, "F"
  goto step2_27
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.22: Send the Table Services Validate command for the ATS "
write ";  Append table. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_EID, "INFO", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Append Table validation command failed."
  goto step2_27
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2009) - ATS Append Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2009, "P"
else
  write "<!> Failed (2009) - ATS Append Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2009, "F"
endif

;; Wait for the SC validation message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC ATS validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - SC ATS validation event Msg ",SC_VERIFY_ATS_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.23: Send the Table Services Activate command for Append table."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Append Table command sent properly."
else
  write "<!> Failed - Activate ATS Append Table command."
  goto step2_27
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Wait for the success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - ATS Append Table update failed. Event Msg not received for activate command."
endif

;; Check the following telemetry items here
write "==> AppendCount (after) = ",$SC_$CPU_SC_AppendCount
write "==> AppendSize  (after) = ",$SC_$CPU_SC_AppendSize
write "==> AppendLoads (after) = ",$SC_$CPU_SC_AppendLoads

wait 5

write ";***********************************************************************"
write ";  Step 2.24: Send the Apply ATS Append Table command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_APPEND_CMD_INF_EID, "INFO", 1

local originalLastByte = freeByteCount - $SC_$CPU_SC_FreeBytes[1]

cmdCtr = $SC_$CPU_SC_CMDPC + 1

/$SC_$CPU_SC_AppendATS ATSID=1

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2008;2008.1) - ATS Append command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_2008, "P"
  ut_setrequirements SC_20081, "P"
else
  write "<!> Failed (1004;2008;2008.1) - ATS Append command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_2008, "F"
  ut_setrequirements SC_20081, "F"
  goto step2_27
endif

;; Verify event was generated
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2008.6) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd."
  ut_setrequirements SC_20086, "P"
else
  write "<!> Failed - Apply ATS Append Table command. Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd; Expected ",SC_APPEND_CMD_INF_EID
  ut_setrequirements SC_20086, "F"
endif

;; Check the HK items
if (p@$SC_$CPU_SC_AppendAtsID = "ATS A") then
  write "<*> Passed - Append ATS ID indicates 'A'."
else
  write "<!> Failed - Append ATS ID indicates '",p@$SC_$CPU_SC_AppendAtsID,"'. Expected 'ATS A'."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.25: Send the Start command for ATS A.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
;; setup this event for the last command in the ATS Append table
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_NOOP_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_CMD0_RCVD_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_COMPL_INF_EID, "INFO", 4
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_ERR_SKP_DBG_EID, "DEBUG", 5

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
  goto step2_27
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

;; Check for the event message indicating the number of 
;; commands skipped when the ATS was started
ut_tlmwait $SC_$CPU_find_event[5].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID," rcv'd."
  ut_setrequirements SC_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID,"."
  ut_setrequirements SC_30001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.26: Dump the ATS A table to verify its contents."
write ";***********************************************************************"
;; Dump the ATS Append table
s get_tbl_to_cvt (ramDir,ATSAppendTblName,"A","$cpu_append_tbl2_26","$CPU",appendPktId)

;; Dump the ATS A table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_26","$CPU",atsPktId)

;; Need to parse the ATS Data and Append Data to verify the Append Data was
;; added to the end of the ATS A table
local ATSStartWord = originalLastByte / 2
local wordCnt = $SC_$CPU_SC_AppendSize / 2
local dataGood = TRUE
write "==> Checking the ATS Data starting at index = ",ATSStartWord
write "==> Need to check ",wordCnt, " words "
;; Loop for 1 to wordCnt. If data is not = fail rqmt
for i = 1 to wordCnt do
  if ($SC_$CPU_SC_ATSDATA[wordCnt+i] <> $SC_$CPU_SC_ATSAppendData[i]) then
    dataGood = FALSE
    break
  endif
enddo

if (dataGood = TRUE) then
  write "<*> Passed (2007.1;2008.3) - Append data was placed at the end of the ATS buffer."
  ut_setrequirements SC_20071, "P"
  ut_setrequirements SC_20083, "P"
else
  write "<!> Failed (2007.1;2008.3) - Data appended to ATS is different from what is in Append Table."
  ut_setrequirements SC_20071, "F"
  ut_setrequirements SC_20083, "F"
endif

;; Wait the the ATS Completion event message
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS A completed execution."
else
  write "<!> Failed - Did not rcv the ATS Completion message."
endif

;; This command was in the Append table as a replacement for the EVS NOOP
;; Also, if this event is rcvd, the execution sequence was recomputed
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (2008.5;2008.7) - SB NOOP Event Msg rcv'd."
  ut_setrequirements SC_20085, "P"
  ut_setrequirements SC_20087, "P"
else
  write "<!> Failed (2008.5;2008.7) - Did not rcv SB NOOP Event Msg as expected."
  ut_setrequirements SC_20085, "F"
  ut_setrequirements SC_20087, "F"
endif

;; Wait until the SC NOOP command executes
;; If the event is found, the Append command added commands
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (2008.4) - SC NOOP Event Msg rcv'd."
  ut_setrequirements SC_20084, "P"
else
  write "<!> Failed (2008.4) - Did not rcv SC NOOP Event Msg as expected."
  ut_setrequirements SC_20084, "F"
endif

wait 5

step2_27:
write ";***********************************************************************"
write ";  Step 2.27: Create and load the ATS with odd-byte commands in order to"
write ";  verify that odd-byte commands are acceptable."
write ";***********************************************************************"
;; Check if the load file exists before creating it
filename = work & "/image/$cpu_ats_oddbyteld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_atsoddbyte
  wait 5
endif

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_oddbyteld", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 2.28: Send the Table Services Validate command for ATS A."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_EID, "INFO", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Table validate command sent for ATS A."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Table validation commands failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS Table validations failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2001, "F"
endif

;; Wait for the SC validation message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC ATS validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - SC ATS validation event Msg ",SC_VERIFY_ATS_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.29: Send the Table Services Activate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Table A command sent properly."
else
  write "<!> Failed - Activate ATS Table A command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
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
write ";  Step 2.30: Send the Start command for ATS A.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_COMPL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_NOOP_INF_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_ERR_SKP_DBG_EID, "DEBUG", 4

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

;; Check for the event message indicating the number of 
;; commands skipped when the ATS was started
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID," rcv'd."
  ut_setrequirements SC_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID,"."
  ut_setrequirements SC_30001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.31: Dump the ATS A table to verify its contents."
write ";***********************************************************************"
;; Dump the ATS A table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl2_31","$CPU",atsPktId)
wait 5

;; Wait the the ATS Completion event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 200
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS A completed execution."
else
  write "<!> Failed - Did not rcv the ATS Completion message."
endif

;; Verify the SC NOOP command executed
;; This is the last command in the ATS
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - SC NOOP Event Msg rcv'd."
else
  write "<!> Failed - Did not rcv SC NOOP Event Msg as expected."
endif

wait 5

step3_0:
write ";***********************************************************************"
write ";  Step 3.0: ATS Invalid Command Tests."
write ";***********************************************************************"
write ";  Step 3.1: Send the Start ATS command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
rawcmd = ""
  
;; CPU1 is the default
rawcmd = "18A9c000000402B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000402B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000402B0"
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

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.2: Send the Start ATS command for an invalid ATS ID. "
write ";***********************************************************************"
write ";  Step 3.2.1: ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INVLD_ID_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartATS ATSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start ATS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start ATS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTATS_CMD_INVLD_ID_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTATS_CMD_INVLD_ID_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.2.2: ID = 3 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INVLD_ID_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartATS ATSID=3

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start ATS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start ATS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTATS_CMD_INVLD_ID_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTATS_CMD_INVLD_ID_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.3: Send the Stop ATS command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18A9c000000403B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000403B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000403B0"
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

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.4: Send the Switch ATS command with an invalid length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c000000408B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000408B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000408B0"
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

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.5: Send the Jump ATS command with an invalid length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c000000609B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000609B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000609B0"
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

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.6: Send a valid Jump ATS command. This command should be  "
write ";  rejected since neither ATS is currently executing. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_JUMPATS_CMD_NOT_ACT_ERR_EID, "ERROR", 1

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENTMET => ", currentMET, " (hex) = ", %hex(currentMET,8)
jumpTime = currentMET + 60

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_JumpATS newTime=jumpTime

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;3003.3) - SC Jump ATS command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_30033, "P"
else
  write "<!> Failed (1005;3003.3) - SC Jump ATS command did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_30033, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_JUMPATS_CMD_NOT_ACT_ERR_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMPATS_CMD_NOT_ACT_ERR_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.7: Send the Continuation Execution of ATS On Error command "
write ";  with an invalid length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18A9c00000040AB0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c00000040AB0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c00000040AB0"
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

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.8: Send the Apply ATS Append Table command with an invalid "
write ";  length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18A9c00000040B85"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c00000040B85"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c00000040B85"
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

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.9: Send the Append ATS command for an invalid ATS ID. "
write ";***********************************************************************"
write ";  Step 3.9.1: ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_APPEND_CMD_ARG_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_AppendATS ATSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Append ATS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Append ATS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_APPEND_CMD_ARG_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_APPEND_CMD_ARG_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.9.2: ID = 3 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_APPEND_CMD_ARG_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_AppendATS ATSID=3

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Append ATS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Append ATS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_APPEND_CMD_ARG_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_APPEND_CMD_ARG_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

step4_0:
write ";***********************************************************************"
write ";  Step 4.0: ATS Anomoly Tests."
write ";***********************************************************************"
write ";  Step 4.1: Create an ATS load that contains duplicate command numbers,"
write ";  invalid command lengths, and an incomplete command at the end.  "
write ";***********************************************************************"
;; Get the current MET
currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)," (dec) ", %dec(currentMET,10)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load3.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_5
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load3" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load3","$cpu_ats_a_load3",0)

;; Set the locations in the ATS Data with the invalid checksum values
$SC_$CPU_SC_ATSDATA[14] = x'0002'
$SC_$CPU_SC_ATSDATA[28] = x'0002'

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Table Load", "$cpu_ats_a_load3",ATSATblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[42]")

write ";***********************************************************************"
write ";  Step 4.2: Send the Table Services Load command with the above file.  "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load3", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step4_5
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"I","$cpu_atsa_tbl4_2","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.3: Send the Table Services Validate command for ATS A.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_DUP_ERR_EID, "ERROR", 3

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
  write "<!> Failed - ATS A Table validation command failed."
  goto step4_4
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS A Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS A Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
  ut_setrequirements SC_2001, "F"
endif

;; Check for the invalid command number event message
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - Duplicate command number detected in ATS load file. Event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - Did not rcv the expected Event Msg for duplicate command number - ",SC_VERIFY_ATS_DUP_ERR_EID
endif

wait 5

step4_4:
write ";***********************************************************************"
write ";  Step 4.4: Send the Table Services Load Abort command.               "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_5:
write ";***********************************************************************"
write ";  Step 4.5: Send a Stop command when there are no ATSs executing.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPATS_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StopATS

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001.1) - SC Stop ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_30011, "P"
else
  write "<!> Failed (1004;3001.1) - SC Stop ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_30011, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STOPATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.6: Send a Switch command when there are no ATSs executing.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_SWITCH_ATS_CMD_IDLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_SwitchATS

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3002) - SC Switch ATS command failed as expected."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3002, "P"
else
  write "<!> Failed (1004;3002) - SC Switch ATS command did not increment CMDEC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_SWITCH_ATS_CMD_IDLE_ERR_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_SWITCH_ATS_CMD_IDLE_ERR_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.7: Create an ATS containing MAX + 1 commands for ATS A. "
write ";***********************************************************************"
;; Check if the max cmds load file exists before creating it
filename = work & "/image/$cpu_ats_maxcmd_ld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_maxcmdats
  wait 5
endif

write ";***********************************************************************"
write ";  Step 4.8: Load the image created above. "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_maxcmd_ld", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step4_11
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.9: Send the Table Services Validate command for ATS A.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_NUM_ERR_EID, "ERROR", 3

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
  write "<!> Failed - ATS A Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation failed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2) - ATS A Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
else
  write "<!> Failed (2000.2) - ATS A Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
  ut_setrequirements SC_20002, "F"
endif

;; Wait for the Invalid command number error event
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000.2) - ATS A Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
else
  write "<!> Failed (2000.2) - ATS A Table validation did not rcv the expected Event Msg ",SC_VERIFY_ATS_NUM_ERR_EID
  ut_setrequirements SC_20002, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"I","$cpu_atsa_tbl4_9","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.10: Send the Table Services Load Abort command.               "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_11:
write ";***********************************************************************"
write ";  Step 4.11: Create an ATS load that contains commands that will get  "
write ";  skipped on startup along with commands that contain invalid Data "
write ";  Integrity Check Values. "
write ";***********************************************************************"
;; Get the current MET
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
lastCmdMET = currentMET + 150
write "-- Last Cmd MET => ", lastCmdMET, " (hex) = ", %hex(lastCmdMET,8)

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)," (dec) ", %dec(currentMET,10)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load4.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_19
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load4" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load4","$cpu_ats_a_load4",0)

;; Set the location in the ATS Data with an invalid checksum value
$SC_$CPU_SC_ATSDATA[28] = x'0080'

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Table Load", "$cpu_ats_a_load4",ATSATblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[42]")

write ";***********************************************************************"
write ";  Step 4.12: Send the Table Services Load command with the above file."
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load4", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step4_19
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.13: Send the Table Services Validate command for ATS A.       "
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
  goto step4_19
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS A Table Validation successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS A Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID," not rcv'd."
  ut_setrequirements SC_2001, "F"
  goto step4_19
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.14: Send the Table Services Activate command for ATS A.  "
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
  goto step4_19
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

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl4_14","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.15: Send the Disable Continuation of ATS On Error command.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_CONT_CMD_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_ContinueATS stop

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3005) - SC Continue Disable ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3005, "P"
else
  write "<!> Failed (1004;3005) - SC Continue Disable ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_CONT_CMD_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_CONT_CMD_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Wait until ATS A command #3's execution time has passed
if (SC_TIME_TO_USE = SC_USE_UTC) then
  wait 37
else
  wait 5
endif

write ";***********************************************************************"
write ";  Step 4.16: Send the Start command for ATS A.                         "
write ";***********************************************************************"
;; Setup the events
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1

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
write ";  Step 4.17: Verify that the first 3 commands are marked 'Skipped'.  "
write ";***********************************************************************"
;; Need to dump the ATS command table to view each command's status
s get_tbl_to_cvt (ramDir,ATSACmdTblName,"A","$cpu_atsa_cmd4_17","$CPU",atsCmdPktId)

;; Commands #1-3 should indicate "Skipped"
if (p@$SC_$CPU_SC_ATSCMDStatus[1] = "Skipped") AND ;;
   (p@$SC_$CPU_SC_ATSCMDStatus[2] = "Skipped") AND ;;
   (p@$SC_$CPU_SC_ATSCMDStatus[3] = "Skipped") then
  write "<*> Passed (3000.1) - The first 3 commands of ATS A are marked skipped as expected."
  ut_setrequirements SC_30001, "P"
else
  write "<!> Failed (3000.1) - One of ATS A's first 3 commands has an incorrect Status."
  ut_setrequirements SC_30001, "F"
endif

write ";***********************************************************************"
write ";  Step 4.18: Verify that ATS A is being processed correctly. Command "
write ";  #4 should fail the Data Integrity Check validation and ATS A should "
write ";  abort execution since the Continue-On-Error flag is disabled.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_ABT_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_CHKSUM_ERR_EID, "ERROR", 2

;; Wait until ATS A Command #4 is dispatched
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
waitTime = $SC_$CPU_SC_NEXTATSTIME - currentMET
if (waitTime > 0) then
  wait waitTime
endif

;; Check to see if the Checksum Error event was rcv'd
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3000.3.1) - Checksum Error Event message rcv'd."
  ut_setrequirements SC_300031, "P"
else
  write "<!> Failed (3000.3.1) - Checksum error Event message was not rcv'd"
  ut_setrequirements SC_300031, "F"
endif

;; Check to see if the ATS Aborted event message was rcv'd
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3000.3.2) - ATS A stopped after encountering an error."
  ut_setrequirements SC_300032, "P"
else
  write "<!> Failed (3000.3.2) - ATS Aborted Event message was not rcv'd"
  ut_setrequirements SC_300032, "F"
endif

;; Need to dump the ATS command table to view command #4's status
s get_tbl_to_cvt (ramDir,ATSACmdTblName,"A","$cpu_atsa_cmd4_18","$CPU",atsCmdPktId)

;; Command #4 should indicate "Failed Checksum"
if (p@$SC_$CPU_SC_ATSCMDStatus[4] = "Failed Checksum") then
  write "<*> Passed (3000.3;3000.3.1) - ATS A Command #4 Status is correct."
  ut_setrequirements SC_30003, "P"
  ut_setrequirements SC_300031, "P"
else
  write "<!> Failed (3000.3;3000.3.1) - ATS A Command #4 Status does not indicate failure."
  ut_setrequirements SC_30003, "F"
  ut_setrequirements SC_300031, "F"
endif

;; Check to see if the HK telemetry indicates Command #4 
;; as the last ATS command with an error
if ($SC_$CPU_SC_LastATSCmdErr = 4) then
  write "<*> Passed (3000.3.1) - Housekeeping indicates Cmd #4 was discarded."
  ut_setrequirements SC_300031, "P"
else
  write "<!> Failed (3000.3.1) - Housekeeping does not indicate that Cmd #4 failed."
  ut_setrequirements SC_300031, "F"
endif

wait 5

step4_19:
write ";***********************************************************************"
write ";  Step 4.19: Restart the SC and TST_SC applications in order to clear "
write ";  out the ATS buffers. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_ES_CMDPC + 2

/$SC_$CPU_ES_RESTARTAPP Application=SCAppName
/$SC_$CPU_ES_RESTARTAPP Application="TST_SC"
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC and TST_SC apps restart commands sent properly."
else
  write "<!> Failed - App restart commands did not increment CMDPC."
endif

;; Wait until RTS 1 completes after startup
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

wait 5

;; Re-enable DEBUG events
cmdCtr = $SC_$CPU_EVS_CMDPC + 2

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

wait 5

write ";***********************************************************************"
write ";  Step 4.20: Create an ATS load that contains valid commands, a command"
write ";  that contains an invalid Data Integrity Check Value, a Switch "
write ";  command, and a Jump command. The Jump command will jump to a time "
write ";  after the end of the ATS. "
write ";***********************************************************************"
;; Get the current MET
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
lastCmdMET = currentMET + 140
local switchCmdTime = currentMET + 155
write "-- Last Cmd MET => ", lastCmdMET, " (hex) = ", %hex(lastCmdMET,8)
write "-- Switch Time => ", switchCmdTime, " (hex) = ", %hex(switchCmdTime,8)

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)," (dec) ", %dec(currentMET,10)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load5.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_29
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load5" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load5","$cpu_ats_a_load5",0)

;; Set the data words to cause an invalid checksum for cmd #3
$SC_$CPU_SC_ATSDATA[21] = x'0068'

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Table Load", "$cpu_ats_a_load5",ATSATblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[44]")
wait 1

write ";***********************************************************************"
write ";  Step 4.21: Send the Table Services Load command with the above file."
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load5", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS A sent successfully."
else
  write "<!> Failed - Load command for ATS A did not execute successfully."
  goto step4_29
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.22: Send the Table Services Validate command for ATS A.       "
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
  goto step4_29
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
  goto step4_29
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.23: Send the Table Services Activate command for ATS A.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSATblName
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS A Table command sent properly."
else
  write "<!> Failed - Activate ATS A Table command."
  goto step4_29
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (2000.2;2000.3) - ATS A Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
  ut_setrequirements SC_20003, "P"
else
  write "<!> Failed (2000.2;2000.3) - ATS A Table update failed. Event Msg not received for activate command."
  ut_setrequirements SC_20002, "F"
  ut_setrequirements SC_20003, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSATblName,"A","$cpu_atsa_tbl4_23","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.24: Send the Enable Continuation of ATS On Error command.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_CONT_CMD_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_ContinueATS continue

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3004) - SC Continue Enable ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3004, "P"
else
  write "<!> Failed (1004;3004) - SC Continue Enable ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3004, "F"
  goto step4_29
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_CONT_CMD_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_CONT_CMD_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.25: Send the Start command for ATS A.                         "
write ";***********************************************************************"
;; Setup the events
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1

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
  goto step4_29
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
write ";  Step 4.26: Verify that the command with the checksum error is "
write ";  discarded and marked appropriately and the ATS continues to execute. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_CHKSUM_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_INLINE_SWTCH_NOT_LDED_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_JUMPATS_CMD_STOPPED_ERR_EID, "ERROR", 3

lastCmdMET = $SC_$CPU_SC_NEXTATSTIME + 15
switchCmdTime = $SC_$CPU_SC_NEXTATSTIME + 30
write "-- LastCmdMET => ", lastCmdMET, " (hex) = ", %hex(lastCmdMET,8)
;; Wait until ATS A Command #3 is dispatched
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
waitTime = lastCmdMET - currentMET
if (waitTime > 0) then
  wait waitTime
endif

;; Check to see if the Checksum Error event was rcv'd
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3000.3.1) - Checksum Error Event message rcv'd."
  ut_setrequirements SC_300031, "P"
else
  write "<!> Failed (3000.3.1) - Checksum error Event message was not rcv'd"
  ut_setrequirements SC_300031, "F"
endif

;; Check to see if the ATS is still executing
if (p@$SC_$CPU_SC_ATPState = "Executing") then
  write "<*> Passed (3000.3.2) - ATS A is still executing after error."
  ut_setrequirements SC_300032, "P"
else
  write "<!> Failed (3000.3.2) - HK indicates ATS A is not executing after checksum error."
  ut_setrequirements SC_300032, "F"
endif

;; Need to dump the ATS command table to view command #4's status
s get_tbl_to_cvt (ramDir,ATSACmdTblName,"A","$cpu_atsa_cmd4_26","$CPU",atsCmdPktId)

;; Command #3 should indicate "Failed Checksum"
if (p@$SC_$CPU_SC_ATSCMDStatus[3] = "Failed Checksum") then
  write "<*> Passed (3000.3;3000.3.1) - ATS A Command #3 Status is correct."
  ut_setrequirements SC_30003, "P"
  ut_setrequirements SC_300031, "P"
else
  write "<!> Failed (3000.3;3000.3.1) - ATS A Command #3 Status does not indicate checksum error."
  ut_setrequirements SC_30003, "F"
  ut_setrequirements SC_300031, "F"
endif

write ";***********************************************************************"
write ";  Step 4.27: Verify that the Switch command is dispatched and rejected."
write ";***********************************************************************"
;; If I can detect when the Switch is scheduled, I may be able to verify this
;; requirement via demonstration rather than Analysis
ut_setrequirements SC_30024, "A"

;; Wait until the switch command is dispatched
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif

;; Check the event time
write "-- SwitchCmd MET => ",switchCmdTime, " (hex) = ", %hex(switchCmdTime,8)
write "-- NextATSCmd MET=> ",$SC_$CPU_SC_NEXTATSTIME
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

waitTime = switchCmdTime - currentMET
if (waitTime > 0) then
  wait waitTime
endif

;; Check to see if the Switch Command Error event was rcv'd
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3002.3) - Switch command failure Event message rcv'd."
  ut_setrequirements SC_30023, "P"
else
  write "<!> Failed (3002.3) - The expected Switch command failure Event message was not rcv'd"
  ut_setrequirements SC_30023, "F"
endif

write ";***********************************************************************"
write ";  Step 4.28: Verify that the Jump command is dispatched and the ATS "
write ";  stops executing. "
write ";***********************************************************************"
;; Wait until the jump command is dispatched
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif

lastCmdMET = switchCmdTime + 5
;; Check the event time
write "-- JumpCmd MET => ",lastCmdMET, " (hex) = ", %hex(lastCmdMET,8)
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

waitTime = lastCmdMET - currentMET
if (waitTime > 0) then
  wait waitTime
endif

;; Check to see if the Jump command Error event was rcv'd
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed (3003.2.2) - Jump command failure Event message rcv'd."
  ut_setrequirements SC_300322, "P"
else
  write "<!> Failed (3003.2.2) - The expected Jump command failure Event message was not rcv'd"
  ut_setrequirements SC_300322, "F"
endif

wait 5

step4_29:
write ";***********************************************************************"
write ";  Step 4.29: Create a valid table for ATS B containing a significant "
write ";  number of commands separated by various amounts of time."
write ";***********************************************************************"
;; Get the current MET
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
atsBTimes[1] = currentMET + 140
atsBTimes[2] = currentMET + 145
atsBTimes[3] = currentMET + 150
atsBTimes[4] = currentMET + 200
atsBTimes[5] = currentMET + 210
atsBTimes[6] = currentMET + 210

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)," (dec) ", %dec(currentMET,10)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsb_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_38
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsb_load" B {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsb_load","$cpu_ats_b_load2")

write ";***********************************************************************"
write ";  Step 4.30: Send the Table Services Load command with the above load."
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_b_load2", "$CPU")

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

;; Print out the Cmd times for ATS B
for i = 1 to 6 do
  write "-- ATS B Cmd #", i, " MET = ", atsBTimes[i], " (hex) = ", %hex(atsBTimes[i],8)
enddo

wait 5

write ";***********************************************************************"
write ";  Step 4.31: Send the Table Services Validate command for ATS B.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_EID, "INFO", 3

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
  write "<!> Failed - ATS B Table validation failed."
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

;; Wait for the SC validation message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC ATS validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - SC ATS validation event Msg ",SC_VERIFY_ATS_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.32: Send the Table Services Activate command for ATS B.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSBTblName
wait 5

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS B Table command sent properly."
else
  write "<!> Failed - Activate ATS B Table command."
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (2000.2;2000.3) - ATS B Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20002, "P"
  ut_setrequirements SC_20003, "P"
else
  write "<!> Failed (2000.2;2000.3) - ATS B Table update failed. Event Msg not received for activate command."
  ut_setrequirements SC_20002, "F"
  ut_setrequirements SC_20003, "F"
endif

;; Need to dump the ATS table
s get_tbl_to_cvt (ramDir,ATSBTblName,"A","$cpu_atsb_tbl4_32","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.33: Send the Start command for ATS B.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ATS_ERR_SKP_DBG_EID, "DEBUG", 2

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

;; Check for the event message indicating the number of 
;; commands skipped when the ATS was started
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID," rcv'd."
  ut_setrequirements SC_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ATS_ERR_SKP_DBG_EID,"."
  ut_setrequirements SC_30001, "F"
endif

write ";***********************************************************************"
write ";  Step 4.34: Send the Jump command with a time not in ATS B."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_JUMP_ATS_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{SCAppName},SC_JUMP_ATS_SKIPPED_DBG_EID, "DEBUG", 2

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET before JUMP => ", currentMET, " (hex) = ", %hex(currentMET,8)

atsBTimes[1] = $SC_$CPU_SC_NEXTATSTIME
atsBTimes[2] = atsBTimes[1] + 5
atsBTimes[3] = atsBTimes[1] + 10
atsBTimes[4] = atsBTimes[1] + 60
atsBTimes[5] = atsBTimes[1] + 70
atsBTimes[6] = atsBTimes[1] + 70

;; Set the Jump time to be between Commands #3 & 4
jumpTime = atsBTimes[3] + 20
write "-- JUMPing to time => ", jumpTime, " (hex) = ", %hex(jumpTime,8)

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_JumpATS newTime=jumpTime

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3003) - SC Jump ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3003, "P"
else
  write "<!> Failed (1004;3003) - SC Jump ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3003, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_JUMP_ATS_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMP_ATS_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3003.2.1) - SC Jump Command Skipped Event Msg ",SC_JUMP_ATS_SKIPPED_DBG_EID," rcv'd."
  ut_setrequirements SC_300321, "P"
else
  write "<!> Failed (3003.2.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMP_ATS_SKIPPED_DBG_EID,"."
  ut_setrequirements SC_300321, "F"
endif

wait 5

;; Verify that the next ATS Command to execute is Command #4
if ($SC_$CPU_SC_ATPCmdNumber = 4) then
  write "<*> Passed (3003.1) - Housekeeping indicates that Command #4 is next to execute."
  ut_setrequirements SC_30031, "P"
else
  write "<!> Failed (3003.1) - The next expected command is not correct."
  ut_setrequirements SC_30031, "F"
endif

write ";***********************************************************************"
write ";  Step 4.35: Send the Jump command with a time in ATS B that has "
write ";  multiple commands to execute. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_JUMP_ATS_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU",{SCAppName},SC_JUMP_ATS_SKIPPED_DBG_EID, "DEBUG", 2

currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET before JUMP => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Set the Jump time to be Commands #5 & 6
jumpTime = atsBTimes[5]
write "-- JUMPing to time => ", jumpTime, " (hex) = ", %hex(jumpTime,8)

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_JumpATS newTime=jumpTime

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3003) - SC Jump ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3003, "P"
else
  write "<!> Failed (1004;3003) - SC Jump ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3003, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_JUMP_ATS_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMP_ATS_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3003.2.1) - SC Jump Command Skipped Event Msg ",SC_JUMP_ATS_SKIPPED_DBG_EID," rcv'd."
  ut_setrequirements SC_300321, "P"
else
  write "<!> Failed (3003.2.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_JUMP_ATS_SKIPPED_DBG_EID,"."
  ut_setrequirements SC_300321, "F"
endif

wait 5

;; Verify that the next ATS Command to execute is #5
if ($SC_$CPU_SC_ATPCmdNumber = 5) then
  write "<*> Passed (3003.4) - Housekeeping indicates that Command #5 is next to execute."
  ut_setrequirements SC_30034, "P"
else
  write "<!> Failed (3003.4) - The next expected command is not correct. Expected 5; Next Cmd = ",$SC_$CPU_SC_ATPCmdNumber
  ut_setrequirements SC_30034, "F"
endif

write ";***********************************************************************"
write ";  Step 4.36: Send the Start command for ATS A.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTATS_CMD_NOT_IDLE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_StartATS ATSID=1

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;3000) - SC Start ATS A command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_3000, "P"
else
  write "<!> Failed (1005;3000) - SC Start ATS A command did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTATS_CMD_INF_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 4.37: Stop the execution of ATS B.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPATS_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StopATS

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - SC Stop ATS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_3001, "P"
else
  write "<!> Failed (1004;3001) - SC Stop ATS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_3001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STOPATS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPATS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

step4_38:
write ";***********************************************************************"
write ";  Step 4.38: Send the Table Services Load command with the empty ATS A"
write ";  load file. "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("ats_a_empty_ld", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 4.39: Send the Table Services Validate command for ATS A.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_MPT_ERR_EID, "ERROR", 3

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
;; If the message is rcv'd, then validation failed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - ATS A Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_3000, "P"
else
  write "<!> Failed (3000) - ATS A Table validation passed with an empty table."
  ut_setrequirements SC_3000, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS A Table Table empty event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
else
  write "<!> Failed - Did not rcv the ATS A Table empty event message."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.40: Send the Table Services Load Abort command.               "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_41:
write ";***********************************************************************"
write ";  Step 4.41: Create an ATS containing MAX commands with the last "
write ";  command running off the end of the buffer for ATS A. "
write ";***********************************************************************"
;; Check if the load file exists before creating it
filename = work & "/image/$cpu_cmdoffend_ld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_atsoffend
  wait 5
endif

write ";***********************************************************************"
write ";  Step 4.42: Load the image created above. "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_cmdoffend_ld", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 4.43: Send the Table Services Validate command for ATS A.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_BUF_ERR_EID, "ERROR", 3

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
  write "<!> Failed - ATS A Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS A Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS A Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
  ut_setrequirements SC_2001, "F"
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - SC ATS Validation event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - Did not rcv the expected SC ATS validation event Msg ",SC_VERIFY_ATS_END_ERR_EID
  ut_setrequirements SC_2001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.44: Send the Table Services Load Abort command. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_45:
write ";***********************************************************************"
write ";  Step 4.45: Create an ATS load file that contains a single command "
write ";  that exceeds the SC_PACKET_MAX_SIZE configuration parameter. "
write ";  NOTE: This step does not utilize the SCP utilities since the content "
write ";  does not have to be a valid command just too long for a single cmd."
write ";***********************************************************************"
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif

;; Extract the High and Low words of the time in order to set the ATS buffer
highWord = %ashiftr(currentMET,16)
lowWord = %and(currentMET,x'0000FFFF')
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
write "-- High word   => ", highWord, " (hex) = ", %hex(highWord,4)
write "-- Low word    => ", lowWord, " (hex) = ", %hex(lowWord,4)

;; 1st command => TST_SC_GetCRC with a length > SC_PACKET_MAX_SIZE
$SC_$CPU_SC_ATSDATA[1] = 1
;; The next 2 values are the absolute time-tag
$SC_$CPU_SC_ATSDATA[2] = highWord
$SC_$CPU_SC_ATSDATA[3] = lowWord + 100
$SC_$CPU_SC_ATSDATA[4] = x'1B37'
$SC_$CPU_SC_ATSDATA[5] = x'C000'
$SC_$CPU_SC_ATSDATA[6] = SC_PACKET_MAX_SIZE + 2
;;$SC_$CPU_SC_ATSDATA[6] = x'015'
$SC_$CPU_SC_ATSDATA[7] = x'03EE'
$SC_$CPU_SC_ATSDATA[8] = x'0000'
$SC_$CPU_SC_ATSDATA[9] = x'0007'
$SC_$CPU_SC_ATSDATA[10] = x'18A9'
$SC_$CPU_SC_ATSDATA[11] = x'C000'
$SC_$CPU_SC_ATSDATA[12] = x'0001'
$SC_$CPU_SC_ATSDATA[13] = x'008D'

;; Turn logging off for this
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

;; Zero out the rest of the table buffer
for i = 14 to SC_ATS_BUFF_SIZE DO
  $SC_$CPU_SC_ATSDATA[i] = 0
enddo

;; Turn logging back on
%liv (log_procedure) = logging

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Time Table Load", "$cpu_ats_a_load6",ATSATblName, "$SC_$CPU_SC_ATSDATA[1]", "$SC_$CPU_SC_ATSDATA[192]")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load6", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 4.46: Send the Table Services Validate command for ATS A.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_PKT_ERR_EID, "ERROR", 3

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
  write "<!> Failed - ATS A Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation failed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS A Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS A Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
  ut_setrequirements SC_2001, "F"
endif

;; Wait for the SC error message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - Invalid Packet Length error Event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - Did not rcv the expected Error Event Msg ",SC_VERIFY_ATS_PKT_ERR_EID
  ut_setrequirements SC_2001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.47: Send the Table Services Load Abort command.               "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_48:
write ";***********************************************************************"
write ";  Step 4.48: Create an ATS Append Table load file that contains a "
write ";  single command that exceeds the SC_PACKET_MAX_SIZE configuration "
write ";  parameter. "
write ";  NOTE: This step does not utilize the SCP utilities since the content "
write ";  does not have to be a valid command just too long for a single cmd."
write ";***********************************************************************"
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
highWord = %ashiftr(currentMET,16)
lowWord = %and(currentMET,x'0000FFFF')
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
write "-- High word   => ", highWord, " (hex) = ", %hex(highWord,4)
write "-- Low word    => ", lowWord, " (hex) = ", %hex(lowWord,4)

;; 1st command => TST_SC_GetCRC with a length > SC_PACKET_MAX_SIZE
$SC_$CPU_SC_ATSAPPENDDATA[1] = 1
;; The next 2 values are the absolute time-tag
$SC_$CPU_SC_ATSAPPENDDATA[2] = highWord
$SC_$CPU_SC_ATSAPPENDDATA[3] = lowWord + 100
$SC_$CPU_SC_ATSAPPENDDATA[4] = x'1B37'
$SC_$CPU_SC_ATSAPPENDDATA[5] = x'C000'
$SC_$CPU_SC_ATSAPPENDDATA[6] = SC_PACKET_MAX_SIZE + 2
;;$SC_$CPU_SC_ATSAPPENDDATA[6] = x'0105'
$SC_$CPU_SC_ATSAPPENDDATA[7] = x'03EE'
$SC_$CPU_SC_ATSAPPENDDATA[8] = x'0000'
$SC_$CPU_SC_ATSAPPENDDATA[9] = x'0007'
$SC_$CPU_SC_ATSAPPENDDATA[10] = x'18A9'
$SC_$CPU_SC_ATSAPPENDDATA[11] = x'C000'
$SC_$CPU_SC_ATSAPPENDDATA[12] = x'0001'
$SC_$CPU_SC_ATSAPPENDDATA[13] = x'008D'

;; Turn logging off for this
logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

;; Zero out the rest of the table buffer
for i = 14 to SC_APPEND_BUFF_SIZE DO
  $SC_$CPU_SC_ATSAPPENDDATA[i] = 0
enddo

;; Turn logging back on
%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_SC_ATSAPPENDDATA[" & SC_APPEND_BUFF_SIZE & "]"

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS Append Cmd To Big Error","$cpu_append_load3",ATSAppendTblName,"$SC_$CPU_SC_ATSAPPENDDATA[1]",endmnemonic)

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_append_load3", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS Append table sent successfully."
else
  write "<!> Failed - Load command for ATS Append table did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.49: Send the Table Services Validate command for the ATS "
write ";  Append table. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_VERIFY_ATS_PKT_ERR_EID, "ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Append Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation failed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - ATS Append Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
endif

;; Wait for the SC error message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - Invalid Packet Length error Event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - Did not rcv the expected Error Event Msg ",SC_VERIFY_ATS_PKT_ERR_EID
  ut_setrequirements SC_2001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.50: Send the Table Services Load Abort command.          "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_51:
write ";***********************************************************************"
write ";  Step 4.51: Create an Append table containing as many commands that "
write ";  will fit with the last command running off the end of the buffer. "
write ";***********************************************************************"
;; Check if the load file exists before creating it
filename = work & "/image/$cpu_appoffend_ld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_appoffend
  wait 5
endif

write ";***********************************************************************"
write ";  Step 4.52: Load the image created above. "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_appoffend_ld", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ATS Append Table sent successfully."
else
  write "<!> Failed - Load command for ATS Append Table did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.53: Send the Table Services Validate command for Append table."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Append Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation failed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - ATS Append Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.54: Send the Table Services Load Abort command.          "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load abort command sent successfully."
else
  write "<!> Failed - Load abort command did not execute successfully."
endif

;; Check for the Event message generation
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load Abort command."
endif

wait 5

step4_55:
write ";***********************************************************************"
write ";  Step 4.55: Create an ATS table containing valid commands.  "
write ";***********************************************************************"
;; Check if the load file exists before creating it
filename = work & "/image/$cpu_ats_700cmd_ld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_700cmdats
  wait 5
endif

write ";***********************************************************************"
write ";  Step 4.56: Load the ATS B table with the image created above.  "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_700cmd_ld", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 4.57: Send the Table Services Validate command for ATS B.       "
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
  write "<!> Failed - ATS B Table validation failed."
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

wait 5

write ";***********************************************************************"
write ";  Step 4.58: Send the Table Services Activate command for ATS B.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSBTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS B Table command sent properly."
else
  write "<!> Failed - Activate ATS B Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
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
s get_tbl_to_cvt (ramDir,ATSBTblName,"A","$cpu_atsb_tbl4_58","$CPU",atsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.59: Create an Append table containing valid commands.  "
write ";***********************************************************************"
;; Check if the load file exists before creating it
filename = work & "/image/$cpu_appfull_ld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_appendfull
  wait 5
endif

write ";***********************************************************************"
write ";  Step 4.60: Load the Append table with the image created above.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_appfull_ld", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2007) - Load command for ATS Append sent successfully."
  ut_setrequirements SC_2007, "P"
else
  write "<!> Failed (2007) - Load command for ATS Append did not execute successfully."
  ut_setrequirements SC_2007, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.61: Send the Table Services Validate command for the ATS "
write ";  Append table. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Append Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2009) - ATS Append Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2009, "P"
else
  write "<!> Failed (2009) - ATS Append Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2009, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.62: Send the Table Services Activate command for Append table."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2
ut_setupevents "$SC","$CPU",{SCAppName},SC_UPDATE_APPEND_EID,"INFO",3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Append Table command sent properly."
else
  write "<!> Failed - Activate ATS Append Table command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Wait for the success event from CFE_TBL
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - ATS Append Table update failed. CFE_TBL Event Msg not received for activate command."
endif

;; Wait for the success event from SC
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2009) - ATS Append Table Updated successfully. Event Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
  ut_setrequirements SC_2009, "P"
else
  write "<!> Failed (2009) - ATS Append Table update failed. SC Event Msg not received."
  ut_setrequirements SC_2009, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.63: Send the Apply ATS Append Table command for ATS B. This "
write ";  command should fail because the contents of the Append Table do not "
write ";  fit into the ATS B buffer. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_APPEND_CMD_FIT_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

/$SC_$CPU_SC_AppendATS ATSID=2

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;2008.2) - ATS Append command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_20082, "P"
else
  write "<!> Failed (1005;2008.2) - ATS Append command did not increment CMDEC as expected."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_20082, "F"
endif

;; Verify event was generated
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Apply ATS Append Table command. Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd; Expected ",SC_APPEND_CMD_FIT_ERR_EID
  ut_setrequirements SC_1005, "F"
endif

wait 5

step4_64:
write ";***********************************************************************"
write ";  Step 4.64: Restart the SC and TST_SC applications in order to clear "
write ";  out the ATS buffers. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_ES_CMDPC + 2

/$SC_$CPU_ES_RESTARTAPP Application=SCAppName
/$SC_$CPU_ES_RESTARTAPP Application="TST_SC"
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC and TST_SC apps restart commands sent properly."
else
  write "<!> Failed - App restart commands did not increment CMDPC."
endif

;; Wait until RTS 1 completes after startup
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

wait 5

;; Re-enable DEBUG events
cmdCtr = $SC_$CPU_EVS_CMDPC + 2

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

wait 5

write ";***********************************************************************"
write ";  Step 4.65: Create and load a valid Append table. "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_append.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_70
endif

;; Get the current MET
currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_append" A {currentMET}

s $sc_$cpu_load_ats_append("$sc_$cpu_atsa_append","$cpu_ats_a_append")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_append", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2007) - Load command for ATS Append sent successfully."
  ut_setrequirements SC_2007, "P"
else
  write "<!> Failed (2007) - Load command for ATS Append did not execute successfully."
  ut_setrequirements SC_2007, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.66: Send the Table Services Validate command for the ATS "
write ";  Append table. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Append Table validation command failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2009) - ATS Append Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2009, "P"
else
  write "<!> Failed (2009) - ATS Append Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2009, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.67: Send the Table Services Activate command for Append table."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSAppendTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Append Table command sent properly."
else
  write "<!> Failed - Activate ATS Append Table command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Wait for the success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Append Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - ATS Append Table update failed. Event Msg not received for activate command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.68: Send the Apply ATS Append Table command for ATS A. This "
write ";  command should fail because ATS A is empty. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_APPEND_CMD_TGT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

/$SC_$CPU_SC_AppendATS ATSID=1

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - ATS Append command failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - ATS Append command did not increment CMDEC as expected."
  ut_setrequirements SC_1005, "F"
endif

;; Verify event was generated
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Apply ATS Append Table command. Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd; Expected ",SC_APPEND_CMD_FIT_ERR_EID
  ut_setrequirements SC_1005, "F"
endif

wait 5

step4_69:
write ";***********************************************************************"
write ";  Step 4.69: Restart the SC and TST_SC applications in order to clear "
write ";  out the ATS buffers. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_ES_CMDPC + 2

/$SC_$CPU_ES_RESTARTAPP Application=SCAppName
/$SC_$CPU_ES_RESTARTAPP Application="TST_SC"
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC and TST_SC apps restart commands sent properly."
else
  write "<!> Failed - App restart commands did not increment CMDPC."
endif

wait 5

;; Wait until RTS 1 completes after startup
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
wait 5

;; Re-enable DEBUG events
cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the SC and CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=SCAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

wait 5

step4_70:
write ";***********************************************************************"
write ";  Step 4.70: Create and load a valid table to ATS A. "
write ";***********************************************************************"
;; Get the current MET
currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load2.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step5_0
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load2" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load2","$cpu_ats_a_load2")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load2", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 4.71: Send the Table Services Validate command for ATS A."
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ATS Table validate command sent for ATS A."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - ATS Table validation commands failed."
endif

;; Wait for the validation message
;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2001) - ATS Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_2001, "P"
else
  write "<!> Failed (2001) - ATS Table validations failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_2001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.72: Send the Table Services Activate command for ATS A. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=ATSATblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate ATS Table A command sent properly."
else
  write "<!> Failed - Activate ATS Table A command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
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
write ";  Step 4.73: Send the Apply ATS Append Table command for ATS A. This "
write ";  command should fail because ATS A is empty. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_APPEND_CMD_SRC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

/$SC_$CPU_SC_AppendATS ATSID=1

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - ATS Append command failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - ATS Append command did not increment CMDEC as expected."
  ut_setrequirements SC_1005, "F"
endif

;; Verify event was generated
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - Apply ATS Append Table command. Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd; Expected ",SC_APPEND_CMD_FIT_ERR_EID
  ut_setrequirements SC_1005, "F"
endif

wait 5

step5_0:
write ";*********************************************************************"
write ";  Step 5.0: Time Format Test "
write ";*********************************************************************"
write ";  Step 5.1: Create and load a table image for ATS A that contains "
write ";  commands that will execute every second consecutively. This ATS "
write ";  runs for 24 consecutive seconds."
write ";*********************************************************************"
;; Get the current MET
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif

currentMET = {TAITime}
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = {UTCTime}
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)," (dec) ", %dec(currentMET,10)

;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_atsa_load7.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step6_0
endif

;; Using the SCP utilities, compile and build the ATS load file
compile_ats "$sc_$cpu_atsa_load7" A {currentMET}

s $sc_$cpu_load_ats_rts("$sc_$cpu_atsa_load7","$cpu_ats_a_load7")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_ats_a_load7", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 5.2: Send the Table Services Validate command for ATS A.       "
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

wait 5

write ";***********************************************************************"
write ";  Step 5.3: Send the Table Services Activate command for ATS A.  "
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

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 10

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
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
write ";  Step 5.4: Send the Start command for ATS A.                      "
write ";***********************************************************************"
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

;; Set the first command MET equal to that reported in SC HK
firstCmdMET = $SC_$CPU_SC_NEXTATSTIME

;; Wait until the 1st command in ATS A executes
currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)
waitTime = firstCmdMET - currentMET
if (waitTime > 0) then
  wait waitTime
endif

write ";*********************************************************************"
write ";  Step 5.5: Send a command to jam the Time. "
write ";*********************************************************************"
local leapSeconds = $SC_$CPU_TIME_LEAPSECS - 5

;; Send the command to set the leap seconds
/$SC_$CPU_TIME_SetClockLeap Leaps=leapSeconds

write ";*********************************************************************"
write ";  Step 5.6: Depending upon what Time Format the SC Application is "
write ";  using, verify that SC handles the jam command issued above properly."
write ";  If the format is UTC, the commands for the missed seconds should be "
write ";  executed in the same second. If the format is TAI, the jam command "
write ";  should have no affect on the ATS execution."
write ";*********************************************************************"
;; SC_TIME_TO_USE is the config parameter that specifies the Time Format
;; The options are SC_USE_CFE_TIME; SC_USE_TAI; SC_USE_UTC
;; TAI is the default CFE_TIME format
ut_setrequirements SC_20004, "A"

if (SC_TIME_TO_USE = SC_USE_UTC) then
  write "-- Jam should affect the execution of commands in that several should execute in the same second after the JAM command"
else
  write "-- Jam should have no affect on the execution of commands"
endif

;; Wait for the ATS A completed event messag
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

wait 5

step6_0:
write ";*********************************************************************"
write ";  Step 6.0: Clean-up - Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_sc_atsfunc"
write ";*********************************************************************"
ENDPROC
