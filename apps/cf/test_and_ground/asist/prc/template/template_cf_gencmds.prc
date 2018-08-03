PROC $sc_$cpu_cf_gencmds
;*******************************************************************************
;  Test Name:  cf_gencmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application general commands function properly. The NOOP and Reset
;	Counters commands will be tested. Invalid versions of these commands
;	will also be tested to ensure that the CF application handles these
;	properly.
;
;  Requirements Tested
;    CF1000	Upon receipt of a No-Op command, CF shall increment the CF
;		Valid Command Counter and generate an event message.
;    CF1001	Upon receipt of a Reset Counters command, CF shall reset the
;		command-specified housekeeping variables to a value of zero.
;    CF1002	For all CF commands, if the length contained in the message
;		header is not equal to the expected length, CF shall reject the
;		command and issue an event message.
;    CF1003	If CF accepts any command as valid, CF shall execute the
;		command, increment the CF Valid Command Counter and issue an
;		event message.
;    CF1004	If CF rejects any command, CF shall abort the command execution,
;		increment the CF Command Rejected Counter and issue an event
;		message.
;    CF6000	CF shall generate a housekeeping message containing the
;		following:
;			a. Ground Command Error Counter
;			b. Ground Command Counter
;			c. Playback Pending Queue State
;			d. Files on Playback Pending Queue
;			e. Frozen Status
;			f. Transactions in Progress
;			g. Completed Transactions
;			h. Failed Transactions
;			i. Uplink PDUs Received
;			j. Uplink Transactions In Progress
;			k. Uplink Transactions Succeeded
;			l. Uplink Transactions Failed
;			m. deleted
;			n. Filename of Last Successful Uplink
;			o. Downlink Transactions In Progress
;			p. Downlink Transactions Succeeded
;			q. Downlink Transactions Failed
;			r. deleted
;			s. Downlink PDUs Sent
;			t. Ack Timer Limit Faults
;			u. Filestore Rejection Faults
;			v. Checksum Failure Faults
;			w. Filesize Error Faults
;			x. NAK Limit Faults
;			y. Inactivity Timer Limit Faults
;			z. Cancel Request Faults
;                      aa. Memory pool allocation remaining
;                      bb. Number of transactions abandoned
;    CF7000	Upon cFE Power-On, CF shall initialize the following
;		Housekeeping data to Zero (or the value specified for the item
;		below):
;			a. Ground Command Error Counter
;			b. Ground Command Counter
;			c. Playback Pending Queue State - enabled
;			d. Files on Playback Pending Queue
;			e. Frozen Status - thawed
;			f. Transactions in Progress
;			g. Completed Transactions
;			h. Failed Transactions
;			i. Uplink PDUs Received
;			j. Uplink Transactions In Progress
;			k. Uplink Transactions Succeeded
;			l. Uplink Transactions Failed
;			m. deleted
;			n. Filename of Last Successful Uplink
;			o. Downlink Transactions In Progress
;			p. Downlink Transactions Succeeded
;			q. Downlink Transactions Failed
;			r. deleted
;			s. Downlink PDUs Sent
;			t. Ack Timer Limit Faults
;			u. Filestore Rejection Faults
;			v. Checksum Failure Faults
;			w. Filesize Error Faults
;			x. NAK Limit Faults
;			y. Inactivity Timer Limit Faults
;			z. Cancel Request Faults
;                      bb. Number of transactions abandoned
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The CF commands and telemetry items exist in the GSE database.
;	The display pages exist for the CF Housekeeping.
;	A CF Test application (TST_CF) exists in order to fully test the CF
;		Application.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	03/22/10	Walt Moleski	Original Procedure.
;       11/02/10        Walt Moleski    Modified to use a variable for the app
;                                       name, updated the requirements that
;                                       changed, and only enabled DEBUG events
;                                       for the appropriate apps.
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
#include "cf_platform_cfg.h"
#include "cf_events.h"
#include "cfe_tbl_events.h"
#include "tst_cf_events.h"
#include "tst_cf2_events.h"

%liv (log_procedure) = logging

#define CF_1000		0
#define CF_1001		1
#define CF_1002		2
#define CF_1003		3
#define CF_1004		4
#define CF_6000		5
#define CF_7000		6

global ut_req_array_size = 6
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1000", "CF_1001", "CF_1002", "CF_1003", "CF_1004", "CF_6000", "CF_7000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
local CFAppName = "CF"

write ";***********************************************************************"
write ";  Step 1.0: CFDP Test Setup."
write ";***********************************************************************"
write ";  Step 1.1: Create and upload the table load file for this test.  "
write ";***********************************************************************"
s $sc_$cpu_cf_tbl1

;; Parse the filename configuration parameter for the default table
local tableFileName = CF_CONFIG_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")

;; loop until all slashes are found
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo

write "==> Parsed default Config Table filename = '",tableFileName

;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_defcfg.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_CF_HK
page $SC_$CPU_TST_CF_HK

write ";***********************************************************************"
write ";  Step 1.3: Start the CFDP (CF) and Test (TST_CF) Applications and "
write ";  verify that the housekeeping packet is being generated and the HK "
write ";  data is initialized properly. "
write ";***********************************************************************"
s $sc_$cpu_cf_start_apps("1.3")
wait 5

;; Add the HK message receipt test
local hkPktId

;; Set the CF HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0B0"

if ("$CPU" = "CPU2") then
  hkPktId = "p1B0"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2B0"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000) - Housekeeping packet is being generated."
  ut_setrequirements CF_6000, "P"
else
  write "<!> Failed (6000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements CF_6000, "F"
endif

;; Extract bit 0 of the engine flags and downlink flags
local frozenState = %and($SC_$CPU_CF_EngineFlags,1)
local pb0QState = %and($SC_$CPU_CF_DownlinkChan[0].DownlinkFlags,1)
local pb1QState = %and($SC_$CPU_CF_DownlinkChan[1].DownlinkFlags,1)

;; Check the HK tlm items to see if they are initialized properly
if ($SC_$CPU_CF_CMDPC = 0) AND ($SC_$CPU_CF_CMDEC = 0) AND ;;
   ($SC_$CPU_CF_TotalInProgTrans = 0) AND ;;
   ($SC_$CPU_CF_TotalCompleteTrans = 0) AND ;;
   ($SC_$CPU_CF_TotalFailedTrans = 0) AND ($SC_$CPU_CF_PDUsRcvd = 0) AND ;;
   ($SC_$CPU_CF_PDUsRejected = 0) AND ($SC_$CPU_CF_ActiveQFileCnt = 0) AND ;;
   ($SC_$CPU_CF_GoodUplinkCtr = 0) AND ($SC_$CPU_CF_BadUplinkCtr = 0) AND ;;
   ($SC_$CPU_CF_LastFileUplinked = "") AND ;;
   ($SC_$CPU_CF_PosAckNum = 0) AND ($SC_$CPU_CF_FileStoreRejNum = 0) AND ;;
   ($SC_$CPU_CF_FileChecksumNum = 0) AND ($SC_$CPU_CF_FileSizeNum = 0) AND ;;
   ($SC_$CPU_CF_NakLimitNum = 0) AND ($SC_$CPU_CF_InactiveNum = 0) AND ;;
   ($SC_$CPU_CF_CancelNum = 0) AND ($SC_$CPU_CF_NumFrozen = 0) AND ;;
   (p@$SC_$CPU_CF_PartnersFrozen = "False") AND (frozenState = 0) AND ;;
   (pb0QState = 0) AND (pb1QState = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PendingQFileCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PendingQFileCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].ActiveQFileCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].ActiveQFileCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PDUsSent = 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PDUsSent = 0) THEN
  write "<*> Passed (7000) - Housekeeping telemetry initialized properly."
  ut_setrequirements CF_7000, "P"
else
  write "<!> Failed (7000) - All Housekeeping telemetry NOT initialized properly at startup."
  ut_setrequirements CF_7000, "F"
  write "CMDPC                  = ", $SC_$CPU_CF_CMDPC
  write "CMDEC                  = ", $SC_$CPU_CF_CMDEC
  write "TotalInProgTrans       = ", $SC_$CPU_CF_TotalInProgTrans
  write "TotalCompleteTrans     = ", $SC_$CPU_CF_TotalCompleteTrans
  write "TotalFailedTrans       = ", $SC_$CPU_CF_TotalFailedTrans
  write "PDUsRcvd               = ", $SC_$CPU_CF_PDUsRcvd
  write "PDUsRejected           = ", $SC_$CPU_CF_PDUsRejected
  write "ActiveQFileCnt         = ", $SC_$CPU_CF_ActiveQFileCnt
  write "GoodUplinkCtr          = ", $SC_$CPU_CF_GoodUplinkCtr
  write "BadUplinkCtr           = ", $SC_$CPU_CF_BadUplinkCtr
  write "LastFileUplinked       = '", $SC_$CPU_CF_LastFileUplinked, "'"
  write "PosAckNum              = ", $SC_$CPU_CF_PosAckNum
  write "FileStoreRejNum        = ", $SC_$CPU_CF_FileStoreRejNum
  write "FileChecksumNum        = ", $SC_$CPU_CF_FileChecksumNum
  write "FileSizeNum            = ", $SC_$CPU_CF_FileSizeNum
  write "NakLimitNum            = ", $SC_$CPU_CF_NakLimitNum
  write "InactiveNum            = ", $SC_$CPU_CF_InactiveNum
  write "CancelNum              = ", $SC_$CPU_CF_CancelNum
  write "NumFrozen              = ", $SC_$CPU_CF_NumFrozen
  write "PartnersFrozen         = ", p@$SC_$CPU_CF_PartnersFrozen
  write "Frozen state           = ",frozenState
  write "Chan 0 Pending Q state = ",pb0QState
  write "Chan 1 Pending Q state = ",pb1QState
  write "Chan 0 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].PendingQFileCnt
  write "Chan 1 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].PendingQFileCnt
  write "Chan 0 ActiveQFileCnt  = ", $SC_$CPU_CF_DownlinkChan[0].ActiveQFileCnt
  write "Chan 1 ActiveQFileCnt  = ", $SC_$CPU_CF_DownlinkChan[1].ActiveQFileCnt
  write "Chan 0 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
  write "Chan 1 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
  write "Chan 0 BadDownlinkCnt  = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
  write "Chan 1 BadDownlinkCnt  = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
  write "Chan 0 PDUsSent        = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
  write "Chan 1 PDUsSent        = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
endif

wait 5

write ";***********************************************************************"
write ";  Step 1.4: Enable DEBUG Event Messages "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the appropriate applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=CFAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Commanding Test."
write ";***********************************************************************"
write ";  Step 2.1: Send the NO-OP command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_NOOP_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1
;; Send the NO-OP Command
/$SC_$CPU_CF_NOOP

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - CF NO-OP command sent properly."
  ut_setrequirements CF_1000, "P"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1000;1003) - CF NO-OP command did not increment CMDPC."
  ut_setrequirements CF_1000, "F"
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1000;1003) - Expected Event Msg ",CF_NOOP_CMD_EID," rcv'd."
  ut_setrequirements CF_1000, "P"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1000;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_NOOP_CMD_EID,"."
  ut_setrequirements CF_1000, "F"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the NO-OP command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C00000020095"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000020094"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000020097"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1002, "P"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1002, "F"
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CMD_LEN_ERR_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Utilizing the TST_CF application, send the command that  "
write ";  will set all the counters that get reset to zero (0) by the Reset  "
write ";  command to a non-zero value."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_CF", TST_CF_SET_COUNTERS_INF_EID, "INFO", 1

/$SC_$CPU_TST_CF_SetCounters

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CF_SET_COUNTERS_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF_SET_COUNTERS_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Send the Reset Counters command."
write ";  There are 5 sets of counters that can be reset. The following steps  "
write ";  send the Reset command for each set."
write ";***********************************************************************"
write ";  Step 2.4.1: Send the Reset command to reset All Counters (0).  "
write ";***********************************************************************"
;; Value=0 -> resets all counters
local pollDirCtr0 = $SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr
local pendingQCtr0 = $SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr
local pollDirCtr1 = $SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr
local pendingQCtr1 = $SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr

;; Check the HK telemetry
if ($SC_$CPU_CF_CMDPC > 0) AND ($SC_$CPU_CF_CMDEC > 0) AND ;;
   ($SC_$CPU_CF_PosAckNum > 0) AND ($SC_$CPU_CF_FileStoreRejNum > 0) AND ;;
   ($SC_$CPU_CF_FileChecksumNum > 0) AND ($SC_$CPU_CF_FileSizeNum > 0) AND ;;
   ($SC_$CPU_CF_NakLimitNum > 0) AND ($SC_$CPU_CF_InactiveNum > 0) AND ;;
   ($SC_$CPU_CF_CancelNum > 0) AND ($SC_$CPU_CF_PDUsRcvd > 0) AND ;;
   ($SC_$CPU_CF_PDUsRejected > 0) AND ($SC_$CPU_CF_MetaCount > 0) AND ;;
   ($SC_$CPU_CF_GoodUplinkCtr > 0) AND ($SC_$CPU_CF_BadUplinkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PDUsSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].FilesSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].RedLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].GreenLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PDUsSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].FilesSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].RedLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].GreenLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr > 0) THEN
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {CFAppName}, CF_RESET_CMD_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_CF_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_CF_ResetCtrs All
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",CF_RESET_CMD_EID," rcv'd."
    ut_setrequirements CF_1001, "P"
    ut_setrequirements CF_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_RESET_CMD_EID,"."
    ut_setrequirements CF_1001, "F"
    ut_setrequirements CF_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_CF_CMDPC = 0) AND ($SC_$CPU_CF_CMDEC = 0) AND ;;
     ($SC_$CPU_CF_PosAckNum = 0) AND ($SC_$CPU_CF_FileStoreRejNum = 0) AND ;;
     ($SC_$CPU_CF_FileChecksumNum = 0) AND ($SC_$CPU_CF_FileSizeNum = 0) AND ;;
     ($SC_$CPU_CF_NakLimitNum = 0) AND ($SC_$CPU_CF_InactiveNum = 0) AND ;;
     ($SC_$CPU_CF_CancelNum = 0) AND ($SC_$CPU_CF_PDUsRcvd = 0) AND ;;
     ($SC_$CPU_CF_PDUsRejected = 0) AND ($SC_$CPU_CF_MetaCount = 0) AND ;;
     ($SC_$CPU_CF_GoodUplinkCtr = 0) AND ($SC_$CPU_CF_BadUplinkCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].PDUsSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].FilesSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].RedLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].GreenLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr < pollDirCtr0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr < pendingQCtr0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].PDUsSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].FilesSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].RedLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].GreenLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr < pollDirCtr1) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr < pendingQCtr1) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements CF_1001, "P"
  else
    write "<!> Failed (1001) - All Counters did not reset to zero."
    ut_setrequirements CF_1001, "F"
  endif
else
  write "<!> Failed (1001) - Reset command not sent because at least 1 counter is set to 0."
  ut_setrequirements CF_1001, "F"
endif

;; Write out the counters for verification
write "CMDPC                    = ", $SC_$CPU_CF_CMDPC
write "CMDEC                    = ", $SC_$CPU_CF_CMDEC
write "PosAckNum                = ", $SC_$CPU_CF_PosAckNum
write "FileStoreRejNum          = ", $SC_$CPU_CF_FileStoreRejNum
write "FileChecksumNum          = ", $SC_$CPU_CF_FileChecksumNum
write "FileSizeNum              = ", $SC_$CPU_CF_FileSizeNum
write "NakLimitNum              = ", $SC_$CPU_CF_NakLimitNum
write "InavtiveNum              = ", $SC_$CPU_CF_InactiveNum
write "CancelNum                = ", $SC_$CPU_CF_CancelNum
write "PDUsReceived             = ", $SC_$CPU_CF_PDUsRcvd
write "PDUsRejected             = ", $SC_$CPU_CF_PDUsRejected
write "MetaCount                = ", $SC_$CPU_CF_MetaCount
write "GoodUplinkCtr            = ", $SC_$CPU_CF_GoodUplinkCtr
write "BadUplinkCtr             = ", $SC_$CPU_CF_BadUplinkCtr
write "PB Chan 0 PDUsSent       = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
write "PB Chan 0 FilesSent      = ", $SC_$CPU_CF_DownlinkChan[0].FilesSent
write "PB Chan 0 GoodCnt        = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
write "PB Chan 0 BadCnt         = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
write "PB Chan 0 RedLightCtr    = ", $SC_$CPU_CF_DownlinkChan[0].RedLightCtr
write "PB Chan 0 GreenLightCtr  = ", $SC_$CPU_CF_DownlinkChan[0].GreenLightCtr
write "PB Chan 0 PollDirsChkCtr = ", $SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr
write "PB Chan 0 PendingQChkCtr = ", $SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr
write "PB Chan 1 PDUsSent       = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
write "PB Chan 1 FilesSent      = ", $SC_$CPU_CF_DownlinkChan[1].FilesSent
write "PB Chan 1 GoodCnt        = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
write "PB Chan 1 BadCnt         = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
write "PB Chan 1 RedLightCtr    = ", $SC_$CPU_CF_DownlinkChan[1].RedLightCtr
write "PB Chan 1 GreenLightCtr  = ", $SC_$CPU_CF_DownlinkChan[1].GreenLightCtr
write "PB Chan 1 PollDirsChkCtr = ", $SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr
write "PB Chan 1 PendingQChkCtr = ", $SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr

wait 5

write ";***********************************************************************"
write ";  Step 2.4.2: Utilizing the TST_CF application, send the set counters "
write ";  command to set all the CF counters to a non-zero value."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_CF", TST_CF_SET_COUNTERS_INF_EID, "INFO", 1

/$SC_$CPU_TST_CF_SetCounters

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CF_SET_COUNTERS_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF_SET_COUNTERS_INF_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4.3: Send the Reset command to reset the Command Counters (1)."
write ";***********************************************************************"
;; Value=1 -> resets CMDPC and CMDEC counters

;; Check the HK telemetry
if ($SC_$CPU_CF_CMDPC > 0) AND ($SC_$CPU_CF_CMDEC > 0) THEN
  write "<*> Command Counters are non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {CFAppName}, CF_RESET_CMD_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_CF_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_CF_ResetCtrs Command
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",CF_RESET_CMD_EID," rcv'd."
    ut_setrequirements CF_1001, "P"
    ut_setrequirements CF_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_RESET_CMD_EID,"."
    ut_setrequirements CF_1001, "F"
    ut_setrequirements CF_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_CF_CMDPC = 0) AND ($SC_$CPU_CF_CMDEC = 0) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements CF_1001, "P"
  else
    write "<!> Failed (1001) - All Counters did not reset to zero."
    ut_setrequirements CF_1001, "F"
  endif
else
  write "<!> Failed (1001) - Reset command not sent because at least 1 counter is set to 0."
  ut_setrequirements CF_1001, "F"
endif

;; Write out the counters for verification
write "CMDPC  = ", $SC_$CPU_CF_CMDPC
write "CMDEC  = ", $SC_$CPU_CF_CMDEC

wait 5

write ";***********************************************************************"
write ";  Step 2.4.4: Send the Reset command to reset the Fault Counters (2).  "
write ";***********************************************************************"
;; Value=2 -> resets Fault counters

;; Check the HK telemetry
if ($SC_$CPU_CF_PosAckNum > 0) AND ($SC_$CPU_CF_FileStoreRejNum > 0) AND ;;
   ($SC_$CPU_CF_FileChecksumNum > 0) AND ($SC_$CPU_CF_FileSizeNum > 0) AND ;;
   ($SC_$CPU_CF_NakLimitNum > 0) AND ($SC_$CPU_CF_InactiveNum > 0) AND ;;
   ($SC_$CPU_CF_CancelNum > 0) THEN
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {CFAppName}, CF_RESET_CMD_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_CF_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_CF_ResetCtrs Fault
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",CF_RESET_CMD_EID," rcv'd."
    ut_setrequirements CF_1001, "P"
    ut_setrequirements CF_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_RESET_CMD_EID,"."
    ut_setrequirements CF_1001, "F"
    ut_setrequirements CF_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_CF_PosAckNum = 0) AND ($SC_$CPU_CF_FileStoreRejNum = 0) AND ;;
     ($SC_$CPU_CF_FileChecksumNum = 0) AND ($SC_$CPU_CF_FileSizeNum = 0) AND ;;
     ($SC_$CPU_CF_NakLimitNum = 0) AND ($SC_$CPU_CF_InactiveNum = 0) AND ;;
     ($SC_$CPU_CF_CancelNum = 0) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements CF_1001, "P"
  else
    write "<!> Failed (1001) - All Counters did not reset to zero."
    ut_setrequirements CF_1001, "F"
  endif
else
  write "<!> Failed (1001) - Reset command not sent because at least 1 counter is set to 0."
  ut_setrequirements CF_1001, "F"
endif

;; Write out the counters for verification
write "PosAckNum       = ", $SC_$CPU_CF_PosAckNum
write "FileStoreRejNum = ", $SC_$CPU_CF_FileStoreRejNum
write "FileChecksumNum = ", $SC_$CPU_CF_FileChecksumNum
write "FileSizeNum     = ", $SC_$CPU_CF_FileSizeNum
write "NakLimitNum     = ", $SC_$CPU_CF_NakLimitNum
write "InactiveNum     = ", $SC_$CPU_CF_InactiveNum
write "CancelNum       = ", $SC_$CPU_CF_CancelNum

wait 5

write ";***********************************************************************"
write ";  Step 2.4.5: Send the Reset command to reset the Uplink Counters (3). "
write ";***********************************************************************"
;; Value=3 -> resets Uplink counters

;; Check the HK telemetry
if ($SC_$CPU_CF_PDUsRcvd > 0) AND ($SC_$CPU_CF_PDUsRejected > 0) AND ;;
   ($SC_$CPU_CF_MetaCount > 0) AND ($SC_$CPU_CF_GoodUplinkCtr > 0) AND ;;
   ($SC_$CPU_CF_BadUplinkCtr > 0) THEN
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {CFAppName}, CF_RESET_CMD_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_CF_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_CF_ResetCtrs Incoming
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",CF_RESET_CMD_EID," rcv'd."
    ut_setrequirements CF_1001, "P"
    ut_setrequirements CF_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_RESET_CMD_EID,"."
    ut_setrequirements CF_1001, "F"
    ut_setrequirements CF_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_CF_PDUsRcvd = 0) AND ($SC_$CPU_CF_PDUsRejected = 0) AND ;;
     ($SC_$CPU_CF_MetaCount = 0) AND ($SC_$CPU_CF_GoodUplinkCtr = 0) AND ;;
     ($SC_$CPU_CF_BadUplinkCtr = 0) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements CF_1001, "P"
  else
    write "<!> Failed (1001) - All Counters did not reset to zero."
    ut_setrequirements CF_1001, "F"
  endif
else
  write "<!> Failed (1001) - Reset command not sent because at least 1 counter is set to 0."
  ut_setrequirements CF_1001, "F"
endif

;; Write out the counters for verification
write "PDUsRcvd      = ", $SC_$CPU_CF_PDUsRcvd
write "PDUsRejected  = ", $SC_$CPU_CF_PDUsRejected
write "MetaCount     = ", $SC_$CPU_CF_MetaCount
write "GoodUplinkCtr = ", $SC_$CPU_CF_GoodUplinkCtr
write "BadUplinkCtr  = ", $SC_$CPU_CF_BadUplinkCtr

wait 5

write ";***********************************************************************"
write ";  Step 2.4.6: Send the Reset command to reset the Downlink Counters (4)"
write ";***********************************************************************"
;; Value=4 -> resets Downlink counters
pollDirCtr0 = $SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr
pendingQCtr0 = $SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr
pollDirCtr1 = $SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr
pendingQCtr1 = $SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr

;; Check the Downlink HK telemetry
;; NOTE: This assumes only 2 Downlink channels
if ($SC_$CPU_CF_DownlinkChan[0].PDUsSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].FilesSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].RedLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].GreenLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PDUsSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].FilesSent > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].RedLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].GreenLightCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr > 0) AND ;;
   ($SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr > 0) THEN
  write "<*> Counters are all non-zero. Sending reset command."

  ;; Send the reset command
  ut_setupevents "$SC", "$CPU", {CFAppName}, CF_RESET_CMD_EID, "DEBUG", 1

  cmdCtr = $SC_$CPU_CF_CMDPC + 1

  ;; Send the Reset Command
  /$SC_$CPU_CF_ResetCtrs Outgoing
  wait 5

  ;; Check for the event message
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Expected Event Msg ",CF_RESET_CMD_EID," rcv'd."
    ut_setrequirements CF_1001, "P"
    ut_setrequirements CF_1003, "P"
  else
    write "<!> Failed (1001;1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_RESET_CMD_EID,"."
    ut_setrequirements CF_1001, "F"
    ut_setrequirements CF_1003, "F"
  endif

  ;; Check to see if the counters were reset
  if ($SC_$CPU_CF_DownlinkChan[0].PDUsSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].FilesSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].RedLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].GreenLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr < pollDirCtr0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr < pendingQCtr0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].PDUsSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].FilesSent = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].RedLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].GreenLightCtr = 0) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr < pollDirCtr1) AND ;;
     ($SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr < pendingQCtr1) THEN
    write "<*> Passed (1001) - Counters all reset to zero."
    ut_setrequirements CF_1001, "P"
  else
    write "<!> Failed (1001) - All Counters did not reset to zero."
    ut_setrequirements CF_1001, "F"
  endif
else
  write "<!> Failed (1001) - Reset command not sent because at least 1 counter is set to 0."
  ut_setrequirements CF_1001, "F"
endif

;; Write out the counters for verification
write "PB Chan 0 PDUsSent       = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
write "PB Chan 0 FilesSent      = ", $SC_$CPU_CF_DownlinkChan[0].FilesSent
write "PB Chan 0 GoodCnt        = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
write "PB Chan 0 BadCnt         = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
write "PB Chan 0 RedLightCtr    = ", $SC_$CPU_CF_DownlinkChan[0].RedLightCtr
write "PB Chan 0 GreenLightCtr  = ", $SC_$CPU_CF_DownlinkChan[0].GreenLightCtr
write "PB Chan 0 PollDirsChkCtr = ", $SC_$CPU_CF_DownlinkChan[0].PollDirsChkCtr
write "PB Chan 0 PendingQChkCtr = ", $SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr
write "PB Chan 1 PDUsSent       = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
write "PB Chan 1 FilesSent      = ", $SC_$CPU_CF_DownlinkChan[1].FilesSent
write "PB Chan 1 GoodCnt        = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
write "PB Chan 1 BadCnt         = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
write "PB Chan 1 RedLightCtr    = ", $SC_$CPU_CF_DownlinkChan[1].RedLightCtr
write "PB Chan 1 GreenLightCtr  = ", $SC_$CPU_CF_DownlinkChan[1].GreenLightCtr
write "PB Chan 1 PollDirsChkCtr = ", $SC_$CPU_CF_DownlinkChan[1].PollDirsChkCtr
write "PB Chan 1 PendingQChkCtr = ", $SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Reset command with an invalid length.             "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C00000060190"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000060191"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000060192"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1002, "P"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1002, "F"
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CMD_LEN_ERR_EID, "."
  ut_setrequirements CF_1005, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send an invalid command.    "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000001AA"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000001AA"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000001AA"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CC_ERR_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7: Channel Semaphore Action command tests.    "
write ";***********************************************************************"
write ";  Step 2.7.1: Send valid Channel Semaphore Action commands. Since the"
write ";  handshake application is not running, these commands should fail. "
write ";***********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GIVETAKE_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

/$SC_$CPU_CF_ChanSemAction Chan_0 Give

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Channel Semaphore Action failed as expected."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Channel Semaphore Action command did not increment CMDEC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg ",CF_GIVETAKE_ERR1_EID," rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_GIVETAKE_ERR1_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GIVETAKE_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

/$SC_$CPU_CF_ChanSemAction Chan_1 Take

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Channel Semaphore Action failed as expected."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Channel Semaphore Action command did not increment CMDEC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg ",CF_GIVETAKE_ERR1_EID," rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_GIVETAKE_ERR1_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7.2: Send a Channel Semaphore Action command with an invalid "
write ";  length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000419"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000419"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000419"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CC_ERR_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7.3: Send a Channel Semaphore Action command with an invalid "
write ";  channel. "
write ";***********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GIVETAKE_ERR2_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000003198F0200"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000003198E0200"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000003198D0200"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CC_ERR_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7.4: Stop the CF and TST_CF apps and start the TST_CF2. "
write ";***********************************************************************"
/$SC_$CPU_ES_DELETEAPP Application="TST_CF"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CFAppName
wait 5

write ";***********************************************************************"
write ";  Step 2.7.5: Start the Handshake application. "
write ";***********************************************************************"
;; Open the Housekeeping page for the Handshake application
page $SC_$CPU_TST_CF2_HK

ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_CF2","$CPU","TST_CF2_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 70
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      write "<*> Passed - TST_CF2 Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_CF2 not received."
  endif
else
  write "<!> Failed - TST_CF2 Application start Event Message not received."
endif

;; CPU1 is the default
stream = x'0945'

if ("$CPU" = "CPU2") then
  stream = x'0A45'
elseif ("$CPU" = "CPU3") then
  stream = x'0B45'
endif

;; Send the TO command to enable the receipt of the TST_CF2 Housekeeping packet
/$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

wait 5

write ";***********************************************************************"
write ";  Step 2.7.6: Startup the CFDP and TSTCF applications. "
write ";***********************************************************************"
s $sc_$cpu_cf_start_apps("2.7.6")
wait 5

;; Enable DEBUG events for the CFDP application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=CFAppName DEBUG
wait 5

write ";***********************************************************************"
write ";  Step 2.7.7: Send valid Channel Semaphore Action commands. "
write ";***********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GIVETAKE_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_ChanSemAction Chan_0 Give

ut_tlmwait $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Channel Semaphore Action command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Channel Semaphore Action command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg ",CF_GIVETAKE_CMD_EID," rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_GIVETAKE_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GIVETAKE_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_ChanSemAction Chan_0 Take

ut_tlmwait $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Channel Semaphore Action command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Channel Semaphore Action command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg ",CF_GIVETAKE_CMD_EID," rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_GIVETAKE_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7.8: Send a Channel Semaphore Action command with an invalid "
write ";  action. "
write ";***********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GIVETAKE_ERR3_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000003198E0002"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000003198F0002"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000003198D0002"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_GIVETAKE_ERR3_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Auto Suspend command tests.    "
write ";***********************************************************************"
write ";  Step 2.8.1: Send the Enable Auto Suspend command.    "
write ";***********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENDIS_AUTO_SUS_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_AutoSuspend Enable

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Auto Suspend command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Auto Suspend command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check the HK to verify that it matches the command
if (p@$SC_$CPU_CF_AutoSuspendFlag = "Enabled") then
  write "<*> Passed (1003) - CF Auto Suspend flag indicates Enabled."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Auto Suspend command = ",p@$SC_$CPU_CF_AutoSuspendFlag,"; Expected 'Enabled'"
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg ",CF_ENDIS_AUTO_SUS_CMD_EID," rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_ENDIS_AUTO_SUS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8.2: Send the Disable Auto Suspend command.    "
write ";***********************************************************************"
;; Setup for the event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENDIS_AUTO_SUS_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_AutoSuspend Disable

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Auto Suspend command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Auto Suspend command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check the HK to verify that it matches the command
if (p@$SC_$CPU_CF_AutoSuspendFlag = "Disabled") then
  write "<*> Passed (1003) - CF Auto Suspend flag indicates Disabled."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Auto Suspend command = ",p@$SC_$CPU_CF_AutoSuspendFlag,"; Expected 'Disabled'"
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg ",CF_ENDIS_AUTO_SUS_CMD_EID," rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_ENDIS_AUTO_SUS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8.3: Send the Auto Suspend command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000061A"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000061A"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000061A"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CC_ERR_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8.4: Send Auto Suspend command with an invalid state. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000061A8A00000002"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000061A8B00000002"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000051A8900000002"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Command Rejected Counter incremented."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements CF_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_CC_ERR_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Clean-up - Send the Power-On Reset command.             "
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
write ";  End procedure $SC_$CPU_cf_gencmds"
write ";*********************************************************************"
ENDPROC
