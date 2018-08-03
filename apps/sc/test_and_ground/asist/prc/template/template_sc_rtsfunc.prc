PROC $sc_$cpu_sc_rtsfunc
;*******************************************************************************
;  Test Name:  sc_rtsfunc
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Stored Command (SC) application supports
;	the execution of Relative Time Sequences (RTS) as well as detecting
;	errors with the commands contained in the RTS. Invalid versions of these
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
;   SC2002	SC shall allocate <PLATFORM_DEFINED> Relative Time-tagged
;		command sequences (RTSs) with each capable of storing
;		<PLATFORM_DEFINED> bytes of stored command data.
;   SC2002.1	SC shall resolve time to 1 second for Relative Time Command
;		Sequences (RTS).
;   SC2002.2	SC shall accept variable length packed RTS commands within the
;		<PLATFORM_DEFINED> byte relative time-tagged sequences.
;   SC2002.3	Each individual command within the sequence shall consist of:
;		a) A time tag with one second resolution.
;		b) A variable length command, with a maximum length of
;		    <PLATFORM_DEFINED> bytes.
;   SC2003	Upon receipt of a table update indication for an RTS table, SC
;		shall set the RTS status to DISABLED.
;   SC2005	SC shall execute no more than <PLATFORM_DEFINED> commands per
;		second from all currently executing RTS tables and/or ATS tables
;   SC2006	SC shall execute the RTSs in priority order based on the RTS
;		number where RTS #1 has the highest priority.
;   SC4000	Upon receipt of a Start RTS command, SC shall start the command-
;		specified RTS, or range of RTS, provided all of the following
;		conditions are met:
;		a) The command-specified RTS, or range of RTS, is not currently
;		   executing
;		b) The RTS table is enabled
;		c) The RTS table has been loaded
;   SC4000.1	If the conditions are met, SC shall issue an event message
;		indicating the RTS started if the RST number is less than
;		<PLATFORM_DEFINED> RTS number.
;   SC4000.2	If the conditions are not met, SC shall reject the command and
;		send an event message.
;   SC4001	SC shall dispatch commands whithin the RTS table, in position
;		order, as the relative time-tag specified in the RTS command
;		expires.
;   SC4001.1	The time-tag shall be interpreted as the number of seconds to
;		delay relative to the previous RTS command dispatched from the
;		RTS table.
;   SC4001.2	For the first command in an RTS table, the delay shall be
;		relative to the receipt of the RTS Start command.
;   SC4001.3	Prior to the dispatch of each individual RTS command, SC shall
;		verify the validity of the following command parameters:
;		a) RTS command length
;		b) Embedded command Data Integrity Check Value.
;   SC4001.3.1	In the event an RTS command fails validation checks, SC shall:
;		a) Discard the invalid RTS command
;		b) Generate an event message
;		c) ABORT the execution of that specific RTS
;   SC4001.4	Upon completion of the execution of an RTS, SC shall send an
;		event message indicating that the RTS completed.
;   SC4002	SC shall terminate the execution of an RTS table upon detection
;		of:
;		a) A Stop RTS command within the RTS command table
;		b) Null data
;		c) the physical end of the RTS table
;   SC4003	Upon receipt of a Stop RTS Command, SC shall terminate the
;		execution of the command-specified RTS table, or range of RTS
;		table.
;   SC4004	Upon receipt of an Enable RTS Command, SC shall set the status
;		of the command-specified RTS to Enabled.
;   SC4005	Upon receipt of a Disable RTS Command, SC shall set the status
;		of the command-specified RTS to Disabled.
;   SC4005.1	If the RTS is currently executing when the Disable RTS Command
;		is received, the current execution of this RTS table shall:
;		a) Be executed until completion
;		b) Set the RTS State to Disabled, preventing it from
;		   future execution.
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
;   SC9004	Upon a power-on reset, SC shall start RTS #1.
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
;	01/12/09	Walt Moleski	Original Procedure.
;       01/25/11        Walt Moleski    Updated for SC 2.1.0.0
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
#include "sc_events.h"
#include "tst_sc_events.h"

%liv (log_procedure) = logging

#define SC_1002		0
#define SC_1004		1
#define SC_1005		2
#define SC_2002		3
#define SC_20021	4
#define SC_20022	5
#define SC_20023	6
#define SC_2003		7
#define SC_2005		8
#define SC_2006		9
#define SC_4000		10
#define SC_40001	11
#define SC_40002	12
#define SC_4001		13
#define SC_40011	14
#define SC_40012	15
#define SC_40013	16
#define SC_400131	17
#define SC_40014	18
#define SC_4002		19
#define SC_4003		20
#define SC_4004		21
#define SC_4005		22
#define SC_40051	23
#define SC_8000		24
#define SC_9000		25
#define SC_9004		26

global ut_req_array_size = 26
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SC_1002", "SC_1004", "SC_1005", "SC_2002", "SC_2002.1", "SC_2002.2", "SC_2002.3", "SC_2003", "SC_2005", "SC_2006", "SC_4000", "SC_4000.1", "SC_4000.2", "SC_4001", "SC_4001.1", "SC_4001.2", "SC_4001.3", "SC_4001.3.1", "SC_4001.4", "SC_4002", "SC_4003", "SC_4004", "SC_4005", "SC_4005.1",  "SC_8000", "SC_9000", "SC_9004" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
LOCAL rtsPktId,rtsAppId
local disableStatus
local bit2Mask = 2;
local bit3Mask = 4;
local bit4Mask = 8;
local bit5Mask = 16;
local bit6Mask = 32;
local bit7Mask = 64;
local bit8Mask = 128;
local bit9Mask = 256;
local bit10Mask = 512;
local bit11Mask = 1024;
local SCAppName = "SC"
local ramDir = "RAM:0"
local ATSATblName = SCAppName & "." & SC_ATS_TABLE_NAME & "1"
local RTS2TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "002"
local RTS3TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "003"
local RTS4TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "004"
local RTS5TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "005"
local RTS6TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "006"
local RTS7TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "007"
local RTS8TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "008"
local RTS9TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "009"
local RTS10TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "010"
local RTS15TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "015"

;; Set the pkt and app Ids for the appropriate CPU
;; CPU1 is the default
rtsPktId = "0FBC"
rtsAppId = 4028

if ("$CPU" = "CPU2") then
  rtsPktId = "0FDA"
  rtsAppId = 4058
elseif ("$CPU" = "CPU3") then
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
page $SC_$CPU_SC_RTS_TBL
page $SC_$CPU_TBL_REGISTRY

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

;; Check the HK tlm items to see if they are 0
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
write ";  Step 1.7: Verify that RTS #1 has been started.                       "
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
write ";  Step 1.8: Verify that all RTS tables have been allocated.            "
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
write ";  Step 2.0: RTS Valid Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Send the Enable RTS command for RTS #2."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=2

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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16->1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS2Status
write "--Bit 2 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #2 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #2 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Create and load a valid table to RTS #2.                   "
write ";***********************************************************************"
local work = %env("WORK")
;; Need to check if the .scs file used below exists. If not, end the proc
local filename = work & "/image/$sc_$cpu_rts2_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_8
endif

;; Using the SCP utilities, compile and build the RTS 3 load file
compile_rts "$sc_$cpu_rts2_load" 2
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts2_load","$cpu_rts002_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts002_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #2 sent successfully."
else
  write "<!> Failed - Load command for RTS #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send the Table Services Validate command for RTS #2.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS2TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #2 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #2 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #2 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #2 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Send the Table Services Activate command for RTS #2.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS2TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #2 Table command sent properly."
else
  write "<!> Failed - Activate RTS #2 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

;; Wait for the Update Success event
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #2 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #2 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #2 status indicates Disabled
write "--RTS Disable Status for 1-16 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS2Status
write "--Bit 2 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #2 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #2 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS2TblName,"A","$cpu_rts2_tbl2_4","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Enable command for RTS #2.                        "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=2

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

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 1-16 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS2Status
write "--Bit 2 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #2 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #2 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send the Start command for RTS #2.                         "
write ";***********************************************************************"
;; Setup the events for the next 2 steps. EID 1 => 2.6 EID 2 => 2.7
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_NOOP_INF_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_NOOP_INF_EID, "INFO", 4
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_NOOP_EID, "INFO", 5

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=2

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.7: Verify that RTS #2 is being processed correctly.           "
write ";  Wait until RTS #2 completes execution.                               "
write ";***********************************************************************"
;; Verifies rqmts 2002.1 - 1 second resolution
;; 4001 - dispatch in position order 1=SC_NOOP 2=CFE_TBL_NOOP 3=CFE_EVS_NOOP
;; 4001.1 - delay in seconds = 5 - 1 - 1
;; 4001.2 - delay for 1st command 5
;; These requirements can be verified by analyzing the log file
ut_setrequirements SC_20021, "A"
ut_setrequirements SC_4001, "A"
ut_setrequirements SC_40011, "A"
ut_setrequirements SC_40012, "A"

;; 4001.4 - Completion of RTS
;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4001.4) - Expected Event Msg ",SC_RTS_COMPL_INF_EID," rcv'd."
  ut_setrequirements SC_40014, "P"
else
  write "<!> Failed (4001.4) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_COMPL_INF_EID,"."
  ut_setrequirements SC_40014, "F"
endif

;; 4001.3 - command validity - All are valid
;; if the command executed, it was valid
if ($SC_$CPU_find_event[3].num_found_messages = 1) AND ;;
   ($SC_$CPU_find_event[4].num_found_messages = 1) AND ;;
   ($SC_$CPU_find_event[5].num_found_messages = 1) then
  write "<*> Passed (4001.3) - Expected Event Msgs rcv'd for RTS #2 commands."
  ut_setrequirements SC_40013, "P"
else
  write "<!> Failed (4001.3) - At least 1 Event msg was not rcv'd for RTS #2 commands."
  ut_setrequirements SC_40013, "F"
endif

;; Test the TST_SC_GetCRC command
local testData[1 .. 256]

;; populate testData with a command
testData[1] = x'18'
testData[2] = x'A9'
testData[3] = x'C0'
testData[4] = x'00'
testData[5] = x'00'
testData[6] = x'01'
testData[7] = x'00'
testData[8] = x'8D'

/$SC_$CPU_TST_SC_GetCRC DataSize=7 dataArray=testData
wait 10
write "Command Checksum for 7 bytes of data = ",$SC_$CPU_TST_SC_CmdCRC

/$SC_$CPU_TST_SC_GetCRC DataSize=8 dataArray=testData
wait 10
write "Command Checksum for 8 bytes of data = ",$SC_$CPU_TST_SC_CmdCRC

step2_8:
write ";***********************************************************************"
write ";  Step 2.8: Create table load for RTS #3 that contains 75% of the     "
write ";  commands that can execute in any 1 second. Repeat these commands for "
write ";  5 seconds and make sure that the delay for the first command is 5 "
write ";  seconds. "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts3_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_21
endif

;; Using the SCP utilities, compile and build the RTS 3 load file
compile_rts "$sc_$cpu_rts3_load" 3
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts3_load","$cpu_rts003_load")

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Create table load for RTS #8 that contains 50% of the     "
write ";  commands that can execute in any 1 second. Repeat these commands for "
write ";  10 seconds and make sure that the delay for the first command is 5 "
write ";  seconds. "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts8_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_21
endif

;; Using the SCP utilities, compile and build the RTS 3 load file
compile_rts "$sc_$cpu_rts8_load" 8
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts8_load","$cpu_rts008_load")

wait 5

write ";***********************************************************************"
write ";  Step 2.10: Send the Table Services Load command to load the image    "
write ";  created above for RTS #3. "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts003_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #3 sent successfully."
else
  write "<!> Failed - Load command for RTS #3 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.11: Send the Table Services Validate command for RTS #3.      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS3TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #3 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #3 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #3 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Send the Table Services Activate command for RTS #3.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS3TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #3 Table command sent properly."
else
  write "<!> Failed - Activate RTS #3 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #3 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #3 status indicates Disabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #3 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #3 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS3TblName,"A","$cpu_rts3_tbl2_12","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.13: Send the Enable command for RTS #3.                       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=3

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

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #3 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #3 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.14: Send the Table Services Load command to load the image    "
write ";  created above for RTS #8. "
write ";***********************************************************************"
;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts008_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #8 sent successfully."
else
  write "<!> Failed - Load command for RTS #8 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.15: Send the Table Services Validate command for RTS #8.      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS8TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #8 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #8 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #8 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #8 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.16: Send the Table Services Activate command for RTS #8.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS8TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #8 Table command sent properly."
else
  write "<!> Failed - Activate RTS #8 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #8 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #8 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #8 status indicates Disabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS8Status
write "--Bit 8 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #8 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #8 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS8TblName,"A","$cpu_rts8_tbl2_16","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.17: Send the Enable command for RTS #8.                       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=8

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

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS8Status
write "--Bit 8 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #8 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #8 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.18: Send the Start command for RTS #3 & #8.                   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 2
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=3
/$SC_$CPU_SC_StartRTS RTSID=8

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
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

write ";***********************************************************************"
write ";  Step 2.19: Verify that RTS #3 & #8 are executing properly.           "
write ";***********************************************************************"
;; Verify that only SC_MAX_CMDS_PER_SEC commands execute each second (2005)
;; Verify that RTS #3 commands are being executed first before RTS #8 (2006)
;; RTS #3 executes 6 commands each second after an initial 5 second wait
;; RTS #8 executes 4 commands each second after an initial 5 second wait
;; RTS #3 contains 3 SC_NOOP followed by 3 ES_NOOP cmds each second for 2 secs
;; RTS #8 contains 4 TBL_NOOP cmds each second for 3 seconds
;; These requirements can be verified by analyzing the log file
ut_setrequirements SC_2005, "A"
ut_setrequirements SC_2006, "A"

write ";***********************************************************************"
write ";  Step 2.20: Send the Disable command for RTS #3.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISABLE_RTS_DEB_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_DisableRTS RTSID=3
wait 5

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_DISABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4005, "F"
endif

;; Wait until both RTSs complete executing then check the status 
;; Check for the completion event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4005) - Expected Event Msg ",SC_RTS_COMPL_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4005, "P"
else
  write "<!> Failed (1004;4005) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_COMPL_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4005, "F"
endif

;; Make sure that the status indicates Disabled
write "--RTS Disable Status for 16->1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (4005.1) - Disable Status indicates RTS #3 is disabled."
  ut_setrequirements SC_40051, "P"
else
  write "<!> Failed (4005.1) - Disable Status for RTS #3 still indicates enabled."
  ut_setrequirements SC_40051, "F"
endif

wait 10

step2_21:
write ";***********************************************************************"
write ";  Step 2.21: Create and load a table for RTS #5 that contains several  "
write ";  commands, an RTS Stop command, and several more commands.            "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts5_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_27
endif

;; Using the SCP utilities, compile and build the RTS 3 load file
compile_rts "$sc_$cpu_rts5_load" 5
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts5_load","$cpu_rts005_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts005_load", "$CPU")

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

wait 5

write ";***********************************************************************"
write ";  Step 2.22: Send the Table Services Validate command for RTS #5.      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS5TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #5 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #5 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #5 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #5 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.23: Send the Table Services Activate command for RTS #5.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS5TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #5 Table command sent properly."
else
  write "<!> Failed - Activate RTS #5 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #5 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #5 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #5 status indicates Disabled
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

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS5TblName,"A","$cpu_rts5_tbl2_23","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.24: Send the Enable command for RTS #5.                       "
write ";***********************************************************************"
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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS5Status
write "--Bit 5 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #5 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #5 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.25: Send the Start command for RTS #5.                        "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTS_CMD_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.26: Verify that RTS #5 terminates after dispatching the Stop  "
write ";  command and that the commands after the Stop are not executed."
write ";***********************************************************************"
;; 4001.4 and 4002
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4001.4;4002) - Stop RTS Event Msg rcv'd."
  ut_setrequirements SC_40014, "P"
  ut_setrequirements SC_4002, "P"
else
  write "<!> Failed (4001.4;4002) - Stop RTS Event Msg was not rcv'd."
  ut_setrequirements SC_40014, "F"
  ut_setrequirements SC_4002, "F"
endif

step2_27:
write ";***********************************************************************"
write ";  Step 2.27: Create and load a table for RTS #4 that contains several  "
write ";  commands followed by null data.   "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts4_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step2_35
endif

;; Using the SCP utilities, compile and build the RTS 4 load file
compile_rts "$sc_$cpu_rts4_load" 4
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts4_load","$cpu_rts004_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts004_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #4 sent successfully."
else
  write "<!> Failed - Load command for RTS #4 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.28: Send the Table Services Validate command for RTS #4.      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS4TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #4 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #4 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #4 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #4 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.29: Send the Table Services Activate command for RTS #4  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS4TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #4 Table command sent properly."
else
  write "<!> Failed - Activate RTS #4 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #4 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #4 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #4 status indicates Disabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--Bit 4 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #4 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #4 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS4TblName,"A","$cpu_rts4_tbl2_29","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.30: Send the Enable command for RTS #4.                       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--Bit 4 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #4 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #4 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.31: Send the Start command for RTS #4.                        "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "CFE_TIME", CFE_TIME_NOOP_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=4

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.32: Verify that RTS #4 terminates when the null data is       "
write ";  encountered."
write ";***********************************************************************"
if ($SC_$CPU_find_event[2].num_found_messages = 3) then
  write "<*> Passed (4002) - RTS #4 stopped when null data was encountered."
  ut_setrequirements SC_4002, "P"
else
  write "<!> Failed (4002) - RTS #4 stopped prematurely. Expected 3 event msgs and rcv'd ",$SC_$CPU_find_event[2].num_found_messages
  ut_setrequirements SC_4002, "F"
endif

write ";***********************************************************************"
write ";  Step 2.33: Send the Start command for RTS #4 again.                 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=4

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

wait 2

write ";***********************************************************************"
write ";  Step 2.34: Send the Stop command for RTS #4.                 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTS_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StopRTS RTSID=4

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4003) - SC Stop RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4003, "P"
else
  write "<!> Failed (1004;4003) - SC Stop RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4003, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STOPRTS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

step2_35:
write ";***********************************************************************"
write ";  Step 2.35: Create and load a table for RTS #7 that contains the  "
write ";  maximim number of commands that can fit into an RTS.  "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts7_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step3_0
endif

;; Using the SCP utilities, compile and build the RTS 7 load file
compile_rts "$sc_$cpu_rts7_load" 7
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts7_load","$cpu_rts007_load")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts007_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #7 sent successfully."
else
  write "<!> Failed - Load command for RTS #7 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.36: Send the Table Services Validate command for RTS #7.      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS7TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #7 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #7 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #7 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #7 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.37: Send the Table Services Activate command for RTS #7. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS7TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #7 Table command sent properly."
else
  write "<!> Failed - Activate RTS #7 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #7 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #7 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #7 status indicates Disabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS7Status
write "--Bit 7 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #7 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #7 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS7TblName,"A","$cpu_rts7_tbl2_37","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.38: Send the Enable command for RTS #7.                       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=7

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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS7Status
write "--Bit 7 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #7 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #7 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.39: Send the Start command for RTS #7.                        "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=7

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4000.1) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (4000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_COMPL_INF_EID,"."
  ut_setrequirements SC_40001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.40: Verify that RTS #7 terminates when the end of the table is"
write ";  encountered."
write ";***********************************************************************"
;; Check for the RTS Completed event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4001.4;4002) - Expected Event Msg ",SC_RTS_COMPL_INF_EID," rcv'd."
  ut_setrequirements SC_40014, "P"
  ut_setrequirements SC_4002, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_40014, "F"
  ut_setrequirements SC_4002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.41: Create and load RTS #3 with odd-byte commands in order to "
write ";  verify that odd-byte commands are acceptable."
write ";***********************************************************************"
;; Check if the load file exists before creating it
filename = work & "/image/$cpu_rts3oddbyteld"
if NOT file_exists(filename) then
  s $sc_$cpu_sc_rtsoddbyte
  wait 5
endif

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts3oddbyteld", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #3 sent successfully."
else
  write "<!> Failed - Load command for RTS #3 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.42: Send the Table Services Validate command for RTS #3.      "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS3TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #3 Table validation failed."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #3 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #3 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.43: Send the Table Services Activate command for RTS #3.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS3TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #3 Table command sent properly."
else
  write "<!> Failed - Activate RTS #3 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #3 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #3 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #3 status indicates Disabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #3 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #3 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS3TblName,"A","$cpu_rts3_tbl2_12","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 2.44: Send the Enable command for RTS #3.                       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=3

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

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #3 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #3 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.45: Send the Start command for RTS #3.                        "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_COMPL_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=3

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4000.1) - Expected Event Msg ",SC_RTS_COMPL_INF_EID," rcv'd."
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (4000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_COMPL_INF_EID,"."
  ut_setrequirements SC_40001, "F"
endif

;; Wait for the RTS Completed event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4001.4;4002) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_40014, "P"
  ut_setrequirements SC_4002, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_40014, "F"
  ut_setrequirements SC_4002, "F"
endif

wait 5

if (SC_ENABLE_GROUP_COMMANDS = FALSE) then
  write "; ** Group commands are not enabled. Skipping to Step 3.0"
  goto step3_0
endif

write ";***********************************************************************"
write ";  Step 2.46: Send the Disable Group command for RTSs #2 thru #5.       "
write ";***********************************************************************"
;; Make sure that the status of each RTS indicates Enabled
write "--RTS Disable Status for 16->1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
if ($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus <> x'FF20') then
  ;; Send the Enable Group command for RTSs 2 through #5
  /$SC_$CPU_SC_EnableRTSGroup FirstRTSID=2 LastRTSID=5
  wait 5
endif

if ($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus = x'FF20') then
  write ";=> All RTSs Enabled (2-5) "
endif

;; Setup event to capture
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTSGRP_CMD_INF_EID, "INFO", 1

;; Send the Disable RTS Group Command
cmdCtr = $SC_$CPU_SC_CMDPC

/$SC_$CPU_SC_DisableRTSGroup FirstRTSID=2 LastRTSID=5
wait 5

if ($SC_$CPU_SC_CMDPC > cmdCtr) then
  write "<*> Passed (1004) - SC Disable RTS Group command sent properly."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - SC Disable RTS Group command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_DISRTSGRP_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTSGRP_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check that each RTS is disabled
;; RTS #2
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS2Status
write "--Bit 2 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed - Disable Status indicates RTS #2 is disabled."
else
  write "<!> Failed - Disable Status for RTS #2 indicates enabled."
endif

;; RTS #3
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed - Disable Status indicates RTS #3 is disabled."
else
  write "<!> Failed - Disable Status for RTS #3 indicates enabled."
endif

;; RTS #4
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--Bit 4 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed - Disable Status indicates RTS #4 is disabled."
else
  write "<!> Failed - Disable Status for RTS #4 indicates enabled."
endif

;; RTS #5
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS5Status
write "--Bit 5 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed - Disable Status indicates RTS #5 is disabled."
else
  write "<!> Failed - Disable Status for RTS #5 indicates enabled."
endif

write ";***********************************************************************"
write ";  Step 2.47: Send the Enable Group command for RTSs #2 thru #5.       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTSGRP_CMD_INF_EID, "INFO", 1

;; This step assumes that the previous step disabled all these RTSs
cmdCtr = $SC_$CPU_SC_CMDPC

/$SC_$CPU_SC_EnableRTSGroup FirstRTSID=2 LastRTSID=5
wait 5

if ($SC_$CPU_SC_CMDPC > cmdCtr) then
  write "<*> Passed (1004) - SC Enable RTS Group command sent properly."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - SC Enable RTS Group command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENARTSGRP_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTSGRP_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Check that each RTS is enabled
;; RTS #2
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS2Status
write "--Bit 2 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed - Disable Status indicates RTS #2 is enabled."
else
  write "<!> Failed - Disable Status for RTS #2 indicates disabled."
endif

;; RTS #3
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS3Status
write "--Bit 3 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed - Disable Status indicates RTS #3 is enabled."
else
  write "<!> Failed - Disable Status for RTS #3 indicates disabled."
endif

;; RTS #4
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS4Status
write "--Bit 4 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed - Disable Status indicates RTS #4 is enabled."
else
  write "<!> Failed - Disable Status for RTS #4 indicates disabled."
endif

;; RTS #5
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS5Status
write "--Bit 5 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed - Disable Status indicates RTS #5 is enabled."
else
  write "<!> Failed - Disable Status for RTS #5 indicates disabled."
endif

write ";***********************************************************************"
write ";  Step 2.48: Send the Start Group command for RTSs #2 thru #5.       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTSGRP_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC

/$SC_$CPU_SC_StartRTSGroup FirstRTSID=2 LastRTSID=5
wait 5

if ($SC_$CPU_SC_CMDPC > cmdCtr) then
  write "<*> Passed (1004) - SC Start RTS Group command sent properly."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - SC Start RTS Group command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STARTRTSGRP_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTSGRP_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

write ";***********************************************************************"
write ";  Step 2.49: Send the Stop Group command for RTSs #2 thru #5.       "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTSGRP_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC

/$SC_$CPU_SC_StopRTSGroup FirstRTSID=2 LastRTSID=5
wait 5

if ($SC_$CPU_SC_CMDPC > cmdCtr) then
  write "<*> Passed (1004) - SC Stop RTS Group command sent properly."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - SC Stop RTS Group command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
endif

;; Wait for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STOPRTSGRP_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTSGRP_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

step3_0:
write ";***********************************************************************"
write ";  Step 3.0: RTS Invalid Command Tests."
write ";***********************************************************************"
write ";  Step 3.1: Send the Start RTS command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c000000404B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000404B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000404B0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.2: Send the Start RTS command for an invalid RTS ID.          "
write ";***********************************************************************"
write ";  Step 3.2.1: ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_INVALID_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTS_CMD_INVALID_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTS_CMD_INVALID_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.2.2: ID = MAX + 1 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_INVALID_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTS_CMD_INVALID_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTS_CMD_INVALID_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.3: Send the Stop RTS command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c000000405B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000405B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000405B0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.4: Send the Stop RTS command for an invalid RTS ID.          "
write ";***********************************************************************"
write ";  Step 3.4.1: ID = 0  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTS_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTS RTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTS_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTS_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.4.2: ID = MAX + 1 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTS_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTS RTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTS_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTS_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.5: Send the Stop command for RTS #4 which is not executing. "
write ";  This should work like a valid command. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STOPRTS_CMD_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StopRTS RTSID=4

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4003) - SC Stop RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4003, "P"
else
  write "<!> Failed (1004;4003) - SC Stop RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_STOPRTS_CMD_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTS_CMD_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.6: Send the Enable RTS command with an invalid length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c000000407B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000407B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000407B0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.7: Send the Enable RTS command for an invalid RTS ID.         "
write ";***********************************************************************"
write ";  Step 3.7.1: ID = 0  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTS_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTS_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTS_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.7.2: ID = MAX + 1 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTS_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTS_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTS_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.8: Send the Enable RTS command for an RTS that is already     "
write ";  enabled. This should work like a valid command.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=2

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

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS2Status
write "--Bit 2 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #2 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #2 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.9: Send the Disable RTS command with an invalid length.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18A9c000000406B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000406B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000406B0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.10: Send the Disable RTS command for an invalid RTS ID.      "
write ";***********************************************************************"
write ";  Step 3.10.1: ID = 0  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTS_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTS RTSID=0

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTS_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTS_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.10.2: ID = MAX + 1 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTS_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTS RTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTS_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTS_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.11: Send the Disable RTS command for an RTS that is already   "
write ";  disabled. This should work like a valid command.   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_DisableRTS RTSID=3

ut_tlmwait $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4005) - SC Disable RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4005, "P"
else
  write "<!> Failed (1004;4005) - SC Disable RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_DISABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

if (SC_ENABLE_GROUP_COMMANDS = FALSE) then
  write "; ** Group commands are not enabled. Skipping to Step 4.0"
  goto step4_0
endif

write ";***********************************************************************"
write ";  Step 3.12: Send the Start RTS Group command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c00000040DB0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c00000040DB0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c00000040DB0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.13: Send the Start RTS Group command with invalid arguments.  "
write ";***********************************************************************"
write ";  Step 3.13.1: First ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STARTRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTSGroup FirstRTSID=0 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS Group command with invalid first ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.13.2: First ID > Last ID"
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STARTRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTSGroup FirstRTSID=8 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.13.3: First ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STARTRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTSGroup FirstRTSID=SC_NUMBER_OF_RTS+1 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.13.4: Last ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STARTRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTSGroup FirstRTSID=2 LastRTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS Group command with invalid last ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS Group command with invalid last ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.13.5: Last ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STARTRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StartRTSGroup FirstRTSID=2 LastRTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Start RTS Group command with invalid Last ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Start RTS Group command with invalid Last ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.14: Send the Stop RTS Group command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c00000040EB0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c00000040EB0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c00000040EB0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.15: Send the Stop RTS Group command with invalid arguments.   "
write ";***********************************************************************"
write ";  Step 3.15.1: First ID = 0  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STOPRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTSGroup FirstRTSID=0 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS Group command with invalid first ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.15.2: First ID > Last ID"
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STOPRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTSGroup FirstRTSID=8 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS Group command with first ID > last ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS Group command with first ID > last ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.15.3: First ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STOPRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTSGroup FirstRTSID=SC_NUMBER_OF_RTS+1 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS Group command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.15.4: Last ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STOPRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTSGroup FirstRTSID=2 LastRTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS Group command with invalid last ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS Group command with invalid last ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.15.5: Last ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU",{SCAppName},SC_STOPRTSGRP_CMD_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_StopRTSGroup FirstRTSID=2 LastRTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Stop RTS Group command with invalid last ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Stop RTS Group command with invalid last ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STOPRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.16: Send the Enable RTS Group command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
  
;; CPU1 is the default
rawcmd = "18A9c000000410B0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c000000410B0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c000000410B0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.17: Send the Enable RTS Group command with invalid arguments. "
write ";***********************************************************************"
write ";  Step 3.17.1: First ID = 0  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTSGroup FirstRTSID=0 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS Group command with invalid first ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.17.2: First ID > Last ID "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTSGroup FirstRTSID=8 LastRTSID=5

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS Group command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS Group command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.17.3: First ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTSGroup FirstRTSID=SC_NUMBER_OF_RTS+1 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS Group command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS Group command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.17.4: Last ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTSGroup FirstRTSID=2 LastRTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS Group command with invalid first ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.17.5: Last ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENARTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_EnableRTSGroup FirstRTSID=2 LastRTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Enable RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Enable RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENARTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.18: Send the Disable RTS Group command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18A9c00000040FB0"

if ("$CPU" = "CPU2") then
  rawcmd = "19A9c00000040FB0"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AA9c00000040FB0"
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
  write "<*> Passed (1002;1005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements SC_1002, "P"
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1002;1005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",SC_LEN_ERR_EID, "."
  ut_setrequirements SC_1002, "F"
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.19: Send the Disable RTS Group command with invalid arguments."
write ";***********************************************************************"
write ";  Step 3.19.1: First ID = 0  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTSGroup FirstRTSID=0 LastRTSID=5

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS Group command with invalid first ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.19.2: First ID > Last ID "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTSGroup FirstRTSID=7 LastRTSID=5

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS Group command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS Group command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.19.3: First ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTSGroup FirstRTSID=SC_NUMBER_OF_RTS+1 LastRTSID=5

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS Group command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS Group command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.19.4: Last ID = 0 "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTSGroup FirstRTSID=2 LastRTSID=0

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS Group command with invalid first ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS Group command with invalid first ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

write ";***********************************************************************"
write ";  Step 3.19.5: Last ID > MAX "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_DISRTSGRP_CMD_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1

;; Send the Command
/$SC_$CPU_SC_DisableRTSGroup FirstRTSID=2 LastRTSID=SC_NUMBER_OF_RTS+1

ut_tlmwait $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - SC Disable RTS command with invalid ID failed as expected."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1005) - SC Disable RTS command with invalid ID did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005) - Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_DISRTSGRP_CMD_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
endif

wait 5

step4_0:
write ";***********************************************************************"
write ";  Step 4.0: RTS Anomoly Tests."
write ";***********************************************************************"
write ";  Step 4.1: Create and load a table to RTS #6 that contains a single   "
write ";  command that exceeds the SC_PACKET_MAX_SIZE configuration parameter. "
write ";***********************************************************************"
;; Create the command
$SC_$CPU_SC_RTSDATA[1] = 5
$SC_$CPU_SC_RTSDATA[2] = x'1B37'
$SC_$CPU_SC_RTSDATA[3] = x'C000'
;;$SC_$CPU_SC_RTSDATA[4] = x'0105'
$SC_$CPU_SC_RTSDATA[4] = SC_PACKET_MAX_SIZE + 2
$SC_$CPU_SC_RTSDATA[5] = x'03EE'
$SC_$CPU_SC_RTSDATA[6] = x'0000'
$SC_$CPU_SC_RTSDATA[7] = x'0007'
$SC_$CPU_SC_RTSDATA[8] = x'18A9'
$SC_$CPU_SC_RTSDATA[9] = x'C000'
$SC_$CPU_SC_RTSDATA[10] = x'0001'
$SC_$CPU_SC_RTSDATA[11] = x'008D'

;; zero out the rest of the table buffer
for i = 12 to SC_RTS_BUFF_SIZE DO
  $SC_$CPU_SC_RTSDATA[i] = x'0000'
enddo

local endmnemonic = "$SC_$CPU_SC_RTSDATA[" & SC_RTS_BUFF_SIZE & "]"

;; Create the RTS Table Load file
s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS #6 Table Load","$cpu_rts006_load",RTS6TblName,"$SC_$CPU_SC_RTSDATA[1]",endmnemonic)

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts006_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #6 sent successfully."
else
  write "<!> Failed - Load command for RTS #6 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS6TblName,"I","$cpu_rts6_tbl4_1","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.2: Send the Table Services Validate command for RTS #6. Since "
write ";  the load contains invalid command lengths, the validation is expected"
write ";  to fail.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2
ut_setupevents "$SC","$CPU",{SCAppName},SC_RTS_LEN_ERR_EID, "ERROR", 3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS6TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #6 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #6 Table validation command was not sent properly."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.3) - RTS #6 Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.3) - RTS #6 Table validation did not rcv the expected Event Msg ",CFE_TBL_VALIDATION_ERR_EID
  ut_setrequirements SC_20023, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.3) - RTS #6 Invalid command length Msg ",$SC_$CPU_find_event[3].eventid," rcv'd."
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.3) - RTS #6 Table validation did not rcv the expected Event Msg ",SC_RTS_LEN_ERR_EID
  ut_setrequirements SC_20023, "F"
endif

;; Sent the TBL_LoadABORT command to release the buffer used by RTS #6
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_LOAD_ABORT_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_LOADABORT ABTABLENAME=RTS6TblName

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

write ";***********************************************************************"
write ";  Step 4.3: Create and load a table to RTS #9 that contains several    "
write ";  valid commands followed by several commands that contain invalid Data"
write ";  Integrity Check values. "
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
filename = work & "/image/$sc_$cpu_rts9_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step4_9
endif

;; Using the SCP utilities, compile and build the RTS 9 load file
compile_rts "$sc_$cpu_rts9_load" 9
s $sc_$cpu_load_ats_rts("$sc_$cpu_rts9_load","$cpu_rts009_load",0,1)

;; Setup the invalid Data Integrity checks
$SC_$CPU_SC_RTSDATA[15] = x'0026'
$SC_$CPU_SC_RTSDATA[20] = x'0120'

;; Create the RTS Table Load file
s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS #9 Table Load","$cpu_rts009_load",RTS9TblName,"$SC_$CPU_SC_RTSDATA[1]",endmnemonic)

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts009_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #9 sent successfully."
else
  write "<!> Failed - Load command for RTS #9 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.4: Send the Table Services Validate command for RTS #9.       "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS9TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #9 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #9 Table validation command."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #9 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #9 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.5: Send the Table Services Activate command for RTS #9.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS9TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #9 Table command sent properly."
else
  write "<!> Failed - Activate RTS #9 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #9 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #9 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #9 status indicates Disabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS9Status
write "--Bit 9 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #9 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #9 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS9TblName,"A","$cpu_rts9_tbl4_5","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.6: Send the Enable command for RTS #9.                        "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=9

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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16-1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS9Status
write "--Bit 9 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #9 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #9 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.7: Send the Start command for RTS #9.                         "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_CHKSUM_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=9

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.8: Verify that RTS #9 aborts when the first command containing"
write ";  an invalid Data Integrity Check value executes. "
write ";***********************************************************************"
;;; May be able to detect this by waiting for the event message that is
;;; generated when the command fails dispatch (4001.3.1)
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4001.3.1) - Expected Event Msg ",SC_RTS_CHKSUM_ERR_EID," rcv'd."
  ut_setrequirements SC_400131, "P"
else
  write "<!> Failed (4001.3.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_CHKSUM_ERR_EID,"."
  ut_setrequirements SC_400131, "F"
endif

step4_9:
write ";***********************************************************************"
write ";  Step 4.9: Send the Start command for RTS #2.                         "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=2

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4000;4000.1) - SC Start RTS command sent properly."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004;4000;4000.1) - SC Start RTS command did not increment CMDPC."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40001, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_RTS_START_INF_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
  ut_setrequirements SC_40001, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_RTS_START_INF_EID,"."
  ut_setrequirements SC_1004, "F"
  ut_setrequirements SC_40001, "F"
endif

write ";***********************************************************************"
write ";  Step 4.10: Send the Start command for RTS #2 again. This command     "
write ";  should be rejected since the RTS is already executing. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_NOT_LDED_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=2

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000;4000.2) - SC Start RTS command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000;4000.2) - SC Start RTS command did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Expected Event Msg ",SC_STARTRTS_CMD_NOT_LDED_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTS_CMD_NOT_LDED_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_40002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.11: Send the Start command for a disabled RTS. This command   "
write ";  should be rejected. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_DISABLED_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=3

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000;4000.2) - SC Start RTS command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000;4000.2) - SC Start RTS command did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Expected Event Msg ",SC_STARTRTS_CMD_DISABLED_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTS_CMD_DISABLED_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_40002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.12: Send the Start command for an RTS that has not been "
write ";  loaded. This command should be rejected. "
write ";***********************************************************************"
write ";  Step 4.12.1: Send the Enable command for RTS #11.                   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=11

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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16->1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS11Status
write "--Bit 11 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #11 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #11 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.12.2: Send the Start command for RTS #11.                   "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_NOT_LDED_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=11

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000;4000.2) - SC Start RTS command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000;4000.2) - SC Start RTS command did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Expected Event Msg ",SC_STARTRTS_CMD_NOT_LDED_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTS_CMD_NOT_LDED_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_40002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.13: Create and load an empty table to RTS #10.                "
write ";***********************************************************************"
;; Clear out the RDL
for i = 1 to SC_RTS_BUFF_SIZE DO
  $SC_$CPU_SC_RTSDATA[i] = 0
enddo

;; Create the RTS Table Load file
s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS #10 Table Load", "$cpu_rts010_load",RTS10TblName, "$SC_$CPU_SC_RTSDATA[1]",endmnemonic)

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts010_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #10 sent successfully."
else
  write "<!> Failed - Load command for RTS #10 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5
write ";***********************************************************************"
write ";  Step 4.14: Send the Table Services Validate command for RTS #10.     "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS10TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #10 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #10 Table validation command."
endif

;; If the message is rcv'd, then validation passed
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #10 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #10 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.15: Send the Table Services Activate command for RTS #10.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName=RTS10TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #10 Table command sent properly."
else
  write "<!> Failed - Activate RTS #10 Table command."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #10 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #10 Table update failed. Event Msg not received for activate command."
endif

;; Verify that RTS #10 status indicates Disabled
write "--RTS Disable Status for 16->1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS10Status
write "--Bit 10 value = ",disableStatus
if (disableStatus = 1) then
  write "<*> Passed (2003) - Disable Status indicates RTS #10 is disabled after a table update."
  ut_setrequirements SC_2003, "P"
else
  write "<!> Failed (2003) - Disable Status for RTS #10 indicates enabled after a table update."
  ut_setrequirements SC_2003, "F"
endif

;; Need to dump the RTS table
s get_tbl_to_cvt (ramDir,RTS10TblName,"A","$cpu_rts10_tbl4_15","$CPU",rtsPktId)

wait 5

write ";***********************************************************************"
write ";  Step 4.16: Send the Enable command for RTS #10.                      "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=10

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
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
  ut_setrequirements SC_1004, "P"
else
  write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
  ut_setrequirements SC_1004, "F"
endif

;; Make sure that the status indicates Enabled
write "--RTS Disable Status for 16->1 = ", %bin($SC_$CPU_SC_RTSDisableStatus[1].RTSStatusEntry.AllStatus,16)
disableStatus = $SC_$CPU_SC_RTSDISABLESTATUS[1].RTSStatusEntry.RTS10Status
write "--Bit 10 value = ",disableStatus
if (disableStatus = 0) then
  write "<*> Passed (4004) - Disable Status indicates RTS #10 is enabled."
  ut_setrequirements SC_4004, "P"
else
  write "<!> Failed (4004) - Disable Status for RTS #10 indicates disabled."
  ut_setrequirements SC_4004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.17: Send the Start command for RTS #10. Since the table is    "
write ";  empty, this command should be rejected. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {SCAppName}, SC_STARTRTS_CMD_INVLD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_SC_CMDEC + 1
;; Send the Command
/$SC_$CPU_SC_StartRTS RTSID=10

ut_tlmwait  $SC_$CPU_SC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000;4000.2) - SC Start RTS command failed as expected."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_4000, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000;4000.2) - SC Start RTS command did not increment CMDEC."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_4000, "F"
  ut_setrequirements SC_40002, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;4000.2) - Expected Event Msg ",SC_STARTRTS_CMD_INVLD_LEN_ERR_EID," rcv'd."
  ut_setrequirements SC_1005, "P"
  ut_setrequirements SC_40002, "P"
else
  write "<!> Failed (1005;4000.2) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_STARTRTS_CMD_INVLD_LEN_ERR_EID,"."
  ut_setrequirements SC_1005, "F"
  ut_setrequirements SC_40002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.18: Create and load an RTS table (#15) that contains the  "
write ";  maximum number of commands with the last command running off the end."
write ";***********************************************************************"
;; Need to check if the .scs file used below exists. If not, end the proc
;;filename = work & "/image/$sc_$cpu_rts15_load.scs"
;;if NOT file_exists(filename) then
;;  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
;;  goto step5_0
;;endif
;;
;; Using the SCP utilities, compile and build the RTS 9 load file
;;compile_rts "$sc_$cpu_rts15_load" 15
;;s $sc_$cpu_load_ats_rts("$sc_$cpu_rts15_load","$cpu_rts015_load",0,1)
;;
;;;; Load the last command so that it runs off the end of the buffer
;;$SC_$CPU_SC_RTSDATA[141] = x'192B'
;;$SC_$CPU_SC_RTSDATA[142] = x'C000'
;;$SC_$CPU_SC_RTSDATA[143] = x'0027'
;;$SC_$CPU_SC_RTSDATA[144] = x'0365'
;;$SC_$CPU_SC_RTSDATA[145] = x'5343'
;;$SC_$CPU_SC_RTSDATA[146] = x'2E41'
;;$SC_$CPU_SC_RTSDATA[147] = x'5453'
;;$SC_$CPU_SC_RTSDATA[148] = x'5F54'
;;$SC_$CPU_SC_RTSDATA[149] = x'424C'
;;$SC_$CPU_SC_RTSDATA[150] = x'3100'
;;
;;;; Create the RTS Table Load file
;;s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS #15 Table Load", "$cpu_rts015_load",RTS15TblName, "$SC_$CPU_SC_RTSDATA[1]",endmnemonic)

;; Create the load file
s $sc_$cpu_sc_rtsoffend

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts015_load", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #15 sent successfully."
else
  write "<!> Failed - Load command for RTS #15 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.19: Send the Table Services Validate command for RTS #15.   "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_ERR_EID, "ERROR", 2

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=RTS15TblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #15 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #15 Table validation failed."
endif

;; If the message is rcv'd, then validation failed
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.2;2002.3) - RTS #15 Table Validation failed as expected. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
  ut_setrequirements SC_20022, "P"
  ut_setrequirements SC_20023, "P"
else
  write "<!> Failed (2002.2;2002.3) - RTS #15 Table validation passed when failure was expected. Event Msg ",CFE_TBL_VALIDATION_ERR_EID, " not rcv'd."
  ut_setrequirements SC_20022, "F"
  ut_setrequirements SC_20023, "F"
endif

wait 5

step5_0:
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
write ";  End procedure $SC_$CPU_sc_rtsfunc"
write ";*********************************************************************"
ENDPROC
