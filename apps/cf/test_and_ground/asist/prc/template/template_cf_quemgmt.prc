PROC $sc_$cpu_cf_quemgmt
;*******************************************************************************
;  Test Name:  cf_quemgmt
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF) Queue
;	Management commands function properly and requirements are implemented
;	properly. Invalid commands will be tested to verify the CF application
;	handles anomalies appropriately.
;
;  Requirements Tested
;    CF1002	For all CF commands, if the length contained in the message
;		header is not equal to the expected length, CF shall reject the
;		command and issue an event message.
;    CF1003	If CF accepts any command as valid, CF shall execute the
;		command, increment the CF Valid Command Counter and issue an
;		event message.
;    CF1004	If CF rejects any command, CF shall abort the command execution,
;		increment the CF Command Rejected Counter and issue an event
;		message.
;    CF4000	The CF Application shall allow a table-defined number of 
;		playback channels.
;    CF4000.1	CF utilizes a Configuration Table to define these playback
;		channels
;    CF4000.1.1	The Configuration Table provides the following configuration
;		items:
;		    A. Playback Channel parameters (repeated for each channel)
;			a. Dequeue Enable 
;			b. deleted
;			c. Downlink PDU MsgId
;			d. Pending Queue Depth
;			e. History Queue Depth
;			f. deleted
;			g. Channel Name
;			h. Handshake Semaphore Name
;			i. Polling Directory Parameters (repeated for each
;			   polling directory)
;			    1. Enable State
;			    2. Class
;			    3. Priority
;			    4. Source Path
;			    5. Destination Path
;			    6. deleted
;			    7. Preserve file
;			    8. Peer Entity ID
;		    B. Input Channel parameters (repeated for each channel)
;			a. Input PDU MsgId
;			b. Class 2 Uplink Response Channel
;    CF4000.2	The CF application shall create a dedicated playback pending
;		queue for each playback channel.
;    CF4000.3	The CF application shall allow a unique message ID for the
;		output PDU on each playback channel.
;    CF4001	The CF application shall provide a command to enable or disable
;		poll directory processing.
;    CF4001.1	Polling directories will be polled for files at the table
;		defined rate.
;    CF4001.2.1	The CF application shall place all files found in the polling
;		directory on the corresponding playback pending queue at the
;		specified priority level.
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
;	06/07/10	Walt Moleski	Original implementation.
;	08/19/10	Walt Moleski	Updated to reflect requirement changes.
;       11/03/10        Walt Moleski    Modified to use a variable for the app
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
;       ut_setupevents	Allows the user to setup multiple events for capture
;			in order to verify multiple events from a single cmd
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
#include "cf_platform_cfg.h"
#include "cf_events.h"
#include "tst_cf_events.h"
#include "cfe_tbl_events.h"
#include "cf_defs.h"

%liv (log_procedure) = logging

#define CF_1002		0
#define CF_1003		1
#define CF_1004		2
#define CF_4000		3
#define CF_40001	4
#define CF_400011	5
#define CF_40002	6
#define CF_40003	7
#define CF_4001		8
#define CF_40011	9
#define CF_400121	10
#define CF_6000		11
#define CF_7000		12

global ut_req_array_size = 12
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1002", "CF_1003", "CF_1004", "CF_4000", "CF_4000.1", "CF_4000.1.1", "CF_4000.2", "CF_4000.3", "CF_4001", "CF_4001.1", "CF_4001.2.1", "CF_6000", "CF_7000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL cmdCtr, errcnt
LOCAL workDir = %env("WORK")
LOCAL destDir = workDir & "/image/"
local CFAppName = "CF"
local ramDir = "RAM:0"

write ";***********************************************************************"
write ";  Step 1.0: CFDP Test Setup."
write ";***********************************************************************"
write ";  Step 1.1: Create and upload the table load file for this test.  "
write ";***********************************************************************"
s $sc_$cpu_cf_tbl2

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
s ftp_file("CF:0", "$cpu_cf_cfg2.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_CF_HK
page $SC_$CPU_CF_QUEUE_INFO
page $SC_$CPU_CF_CONFIG_TBL
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
local queId
local tblId

;; Set the CF HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0B0"
queId = "0F0D"
tblId = "0F78"

if ("$CPU" = "CPU2") then
  hkPktId = "p1B0"
  queId = "0F2D"
  tblId = "0F86"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2B0"
  queId = "0F4D"
  tblId = "0F97"
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
cmdCtr = $SC_$CPU_EVS_CMDPC + 1

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
write ";  Step 1.5: Check the Configuration table requirements. "
write ";***********************************************************************"
;; Check the number of Playback Channels in use
for i = 0 to CF_MAX_PLAYBACK_CHANNELS-1 do
  write "            Playback Channel ", i
  write "=============================================="
  write "    Channel In Use = '",p@$SC_$CPU_CF_TBLPBCHAN[i].EntryInUse, "'"
  write "    Dequeue Enable = '",p@$SC_$CPU_CF_TBLPBCHAN[i].DeqEnable, "'"
  write "    Downlink PDU MsgID = '",p@$SC_$CPU_CF_TBLPBCHAN[i].DnlinkPDUMsgID,"'"
  write "    Pending Queue Depth = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PendingQDepth,"'"
  write "    History Queue Depth = '",p@$SC_$CPU_CF_TBLPBCHAN[i].HistoryQDepth,"'"
  write "    Channel Name = '",p@$SC_$CPU_CF_TBLPBCHAN[i].ChanName,"'"
  write "    Handshake Semaphore Name = '",p@$SC_$CPU_CF_TBLPBCHAN[i].SemName,"'"
  for j = 0 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    write "        Polling Directory ", j
    write "    =============================================="
    write "    Dir In Use = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse, "'"
    write "    Enable State = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EnableState, "'"
    write "    Class = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class, "'"
    write "    Priority = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].Priority, "'"
    write "    Preserve File = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].Preserve, "'"
    write "    Source Path = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].SrcPath, "'"
    write "    Destination Path = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].DstPath, "'"
  enddo
enddo

;; Set the requirements to Pass since this proc will not compile if the 
;; fields above did not exist
ut_setrequirements CF_4000, "P"
ut_setrequirements CF_40001, "P"
ut_setrequirements CF_400011, "P"

wait 5

write ";***********************************************************************"
write ";  Step 1.6: Using the TST_CF application, create the directories needed"
write ";  for this test procedure and upload files to these directories. "
write ";***********************************************************************"
cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Get the directory names from the Config Table entries
local chan1DirName = $SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].SrcPath

;; Send the TST_CF command to create the class1 directory
/$SC_$CPU_TST_CF_DirCreate DirName=chan1DirName

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

local chan2DirName = $SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].SrcPath

;; Send the TST_CF command to create the class2 directory
/$SC_$CPU_TST_CF_DirCreate DirName=chan2DirName

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Command Testing "
write ";***********************************************************************"
write ";  Step 2.1: Send the Enable Polling Directory command for the first "
write ";  directory of the first channel specified in the table. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_POLL_CMD2_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_EnaPoll Chan_0 PollDir_0

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Enable Polling Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1003;4001) - Enable Polling Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Enable Polling Directory command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Enable Polling Directory command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the Enable Polling Directory command with an invalid "
write ";  length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000612830000"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000612820000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000612810000"
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
write ";  Step 2.3: Send the Disable Polling Directory command for the "
write ";  directory enabled in Step 2.1. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_POLL_CMD2_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_DisPoll Chan_0 PollDir_0

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Disable Polling Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1003;4001) - Disable Polling Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Disable Polling Directory command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Disable Polling Directory command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Send the Disable Polling Directory command with an invalid "
write ";  length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000613820000"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000613830000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000613800000"
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
write ";  Step 2.5: Send the Enable Polling Directory command with an invalid "
write ";  channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_POLL_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000512830200"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000512820200"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000512810200"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4001) - Enable Polling Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1004;4001) - Enable Polling Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Enable Polling Directory command error event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Enable Polling Directory command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send the Enable Polling Directory command with an invalid "
write ";  directory. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_POLL_ERR2_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000512830008"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000512820008"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000512810008"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4001) - Enable Polling Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1004;4001) - Enable Polling Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Enable Polling Directory command error event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Enable Polling Directory command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

step2_7:
write ";***********************************************************************"
write ";  Step 2.7: Send the Disable Polling Directory command with an invalid "
write ";  channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_POLL_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000513820200"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000513830200"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000513800200"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4001) - Disable Polling Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1004;4001) - Disable Polling Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Disable Polling Directory command event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Disable Polling Directory command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

step2_8:
write ";***********************************************************************"
write ";  Step 2.8: Send the Disable Polling Directory command with an invalid "
write ";  directory. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_POLL_ERR2_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000000513820008"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000000513830008"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000000513800008"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;4001) - Disable Polling Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1004;4001) - Disable Polling Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Disable Polling Directory command event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Disable Polling Directory command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

step3:
write ";***********************************************************************"
write ";  Step 3.0: Directory Processing Tests "
write ";***********************************************************************"
write ";  Step 3.1: Uplink a file into each polling directory that will be "
write ";  enabled below."
write ";***********************************************************************"
local chan0cmd, chan1cmd
local uplinkCtr = $SC_$CPU_CF_GoodUplinkCtr + 1

chan0cmd = "put -class1 class1file.dat 0.24 " & $SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].SrcPath & "class1file.dat"
cfdp_dir {chan0cmd}

;; Wait until the file has been successfully uplinked
ut_tlmwait $SC_$CPU_CF_GoodUplinkCtr, {uplinkCtr}, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - File 'class1file.dat' successfully uplinked."
else
  write "<!> Failed - File uplink failed for 'class1file.dat'."
endif

uplinkCtr = $SC_$CPU_CF_GoodUplinkCtr + 1

chan1cmd = "put -class1 class2file.txt 0.24 " & $SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].SrcPath & "class2file.txt"
cfdp_dir {chan1cmd}

;; Wait until the file has been successfully uplinked
ut_tlmwait $SC_$CPU_CF_GoodUplinkCtr, {uplinkCtr}, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - File 'class2file.txt' successfully uplinked."
else
  write "<!> Failed - File uplink failed for 'class2file.txt'."
endif

;; Disable playback on the channels above
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 2

;; Send the Commands
/$SC_$CPU_CF_DisDeque CHAN_0
wait 1
/$SC_$CPU_CF_DisDeque CHAN_1
wait 1

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Disable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Disable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Disable Dequeue command event messages rcv'd"
else
  write "<!> Failed - Did not rcv the correct number of Disable Dequeue command event messages"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.2: Send the Enable Polling Directory command for one directory"
write ";  on each playback channel specified in the table. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_POLL_CMD2_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_QDIR_ACTIVEFILE_EID, "DEBUG", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 2

;; Send the Commands
/$SC_$CPU_CF_EnaPoll CHAN_0 pollDir_0
wait 1

/$SC_$CPU_CF_EnaPoll CHAN_1 pollDir_0
wait 1

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Enable Polling Directory commands sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_4001, "P"
else
  write "<!> Failed (1003;4001) - Enable Polling Directory commands did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_4001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed (1003) - Enable Polling Directory commands event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Enable Polling Directory commands event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Two events should be generated each cycle 
;; since the pending queues are disabled
;; Wait for the table-defined rate
wait $SC_$CPU_CF_TBLWakeupsPollDir*2
write "==> Events generated = ",$SC_$CPU_find_event[2].num_found_messages

if ($SC_$CPU_find_event[2].num_found_messages > 3) then
  write "<*> Passed (4001.1) - Enabled directories are being polled properly"
  ut_setrequirements CF_40011, "P"
else
  write "<!> Failed (4001.1) - Event count incorrect for polled directories."
  ut_setrequirements CF_40011, "F"
endif

;; Verify that the playback Pending queues contain the correct number of files
;; Dump the pending queue for channel 0
s get_que_to_cvt(ramDir,"pending","pb0","$cpu_pb0pend","$CPU",queId)

;; Check the first filename in the queue
if ($SC_$CPU_CF_QInfo[1].FileName = "/ram/class1/class1file.dat") then
  write "<*> Passed (4001.2.1) - Channel 0 Queue contains the correct file"
  ut_setrequirements CF_400121, "P"
else
  write "<!> Failed (4001.2.1) - Pending Queue for channel 0 does not contain the correct file."
  ut_setrequirements CF_400121, "F"
endif

;; Save the status for checking rqmt 4000.2
local chan0Status = $SC_$CPU_CF_QInfo[1].TranStatus

;; Dump the pending queue for channel 1
s get_que_to_cvt(ramDir,"pending","pb1","$cpu_pb1pend","$CPU",queId)

;; Check the first filename in the queue
if ($SC_$CPU_CF_QInfo[1].FileName = "/ram/class2/class2file.txt") then
  write "<*> Passed (4001.2.1) - Channel 1 Queue contains the correct file"
  ut_setrequirements CF_400121, "P"
else
  write "<!> Failed (4001.2.1) - Pending Queue for channel 0 does not contain the correct file."
  ut_setrequirements CF_400121, "F"
endif

;; Save the status for checking rqmt 4000.2
local chan1Status = $SC_$CPU_CF_QInfo[1].TranStatus

;; Check 4000.2
if (chan0Status = CF_STAT_PENDING) AND (chan1Status = CF_STAT_PENDING) then
  write "<*> Passed (4000.2) - Pending Queues exist for each channel"
  ut_setrequirements CF_40002, "P"
else
  write "<!> Failed (4000.2) - The pending queue status is not correct for each channel."
  ut_setrequirements CF_40002, "F"
endif

;; Enable Dequeue for each playback channel
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 2

;; Send the Commands
/$SC_$CPU_CF_EnaDeque CHAN_0
wait 1
/$SC_$CPU_CF_EnaDeque CHAN_1
wait 1

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Enable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Enable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Enable Dequeue command event messages rcv'd"
else
  write "<!> Failed - Did not rcv the correct number of Enable Dequeue command event messages"
endif

local dnLinkCtr = $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt + 1

;; Wait until the files above have been downlinked at least 1 time
ut_tlmwait $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt, {dnlinkCtr}, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Channel 0 File successfully downlinked."
else
  write "<!> Failed - Channel 0 File downlink failed."
endif

dnLinkCtr = $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt + 1

;; Wait until the files above have been downlinked at least 1 time
ut_tlmwait $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt, {dnlinkCtr}, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Channel 1 File successfully downlinked."
else
  write "<!> Failed - Channel 1 File downlink failed."
endif

;; Send the Command
/$SC_$CPU_CF_DisPoll Chan_0 pollDir_0
wait 5

;; Send the Command
/$SC_$CPU_CF_DisPoll Chan_1 pollDir_0
wait 5

write ";***********************************************************************"
write ";  Step 3.3: Send the Set Polling Directory Parameter command to change "
write ";  several parameters. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_POLL_PARAM1_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 2

;; Change each parameter that can change
/$SC_$CPU_CF_SetPollParam Chan_0 PollDir_0 Class_2 Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/class2/" DstPath="/ram/"
wait 5

/$SC_$CPU_CF_SetPollParam Chan_1 PollDir_0 Class_1 Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath="/ram/"
wait 5

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Set Poll Parameter command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Set Poll Parameter command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Set Poll Param command event messages rcv'd"
else
  write "<!> Failed - Did not rcv the correct number of Set Poll Param command event messages"
endif

;; Dump the Config table
s get_tbl_to_cvt (ramDir,"CF.ConfigTable","A","$cpu_cf_tbldump","$CPU",tblId)
wait 20

;; Verify the changes Channel 0 Dir 0
if ($SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].CF_Class = 2) AND ;;
   ($SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Priority = 5) AND ;;
   (p@$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Preserve = "Keep") AND ;;
   ($SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].SrcPath = "/ram/class2/") AND ;;
   ($SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].DstPath = "/ram/") then
  write "<*> Passed - Channel 0 Dir 0 Parameters were set properly."
else
  write "<!> Failed - Channel 0 Dir 0 Parameters were not set to the values passed with the command."
  write "Class = '",$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].CF_Class, "'"
  write "Priority = '",$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Priority, "'"
  write "Preserve File = '",p@$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Preserve, "'"
  write "Source Path = '",$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].SrcPath, "'"
  write "Destination Path = '",$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].DstPath, "'"
endif

;; Verify the changes Channel 1 Dir 0
if ($SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].CF_Class = 1) AND ;;
   ($SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Priority = 5) AND ;;
   (p@$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Preserve = "Keep") AND ;;
   ($SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].SrcPath = "/ram/class1/") AND ;;
   ($SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].DstPath = "/ram/") then
  write "<*> Passed - Channel 1 Dir 0 Parameters were set properly."
else
  write "<!> Failed - Channel 1 Dir 0 Parameters were not set to the values passed with the command."
  write "Class = '",$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].CF_Class, "'"
  write "Priority = '",$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Priority, "'"
  write "Preserve File = '",p@$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Preserve, "'"
  write "Source Path = '",$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].SrcPath, "'"
  write "Destination Path = '",$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].DstPath, "'"
endif

write ";***********************************************************************"
write ";  Step 3.4: Send the Set Polling Directory Parameters command with an "
write ";  invalid channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_POLL_PARAM_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000990D820300010501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000990D830300010501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000990D690300010501"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Set Poll Param command failed as expected."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Set Poll Param command event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.5: Send the Set Polling Directory Parameters command with an "
write ";  invalid directory. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_POLL_PARAM_ERR2_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000990D820008010501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000990D830008010501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000990D660008010501"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Set Poll Param command failed as expected."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Set Poll Param command event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.6: Send the Set Polling Directory Parameters command with an "
write ";  invalid preserve value. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_POLL_PARAM_ERR4_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000990D820000010502"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000990D830000010502"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000990D660000010502"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Set Poll Param command failed as expected."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Set Poll Param command event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.7: Send the Set Polling Directory Parameters command with an "
write ";  invalid class. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_POLL_PARAM_ERR3_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000990D820000000501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000990D830000000501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000990D660000000501"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Set Poll Param command failed as expected."
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Set Poll Param command event message rcv'd"
  ut_setrequirements CF_1004, "P"
else
  write "<!> Failed (1004) - Set Poll Param command event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
endif

wait 5

step4:
write ";***********************************************************************"
write ";  Step 4.0: Configuration Table Change Tests "
write ";***********************************************************************"
write ";  Step 4.1: Generate a CF Configuration table with a different output "
write ";  PDU message ID for each channel. "
write ";***********************************************************************"
s $sc_$cpu_cf_tbl3

wait 5

write ";***********************************************************************"
write ";  Step 4.2: Stop the CF and TST_CF applications and copy the above "
write ";  table to the default table file location. "
write ";***********************************************************************"
;; Stop the Applications
/$SC_$CPU_ES_DELETEAPP Application="TST_CF"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CFAppName
wait 5

;; Upload the configuration file created above to $CPU
s ftp_file("CF:0", "$cpu_cf_cfg3.tbl", tableFileName, "$CPU", "P")
wait 5

;; Restart the apps
s $sc_$cpu_cf_start_apps("4.2")
wait 5

;; If the CF application starts, CF4000.3 is verified since table 3 contains 
;; 2 different msg IDs for each playback channel
if ($SC_$CPU_CF_TBLPBCHAN[0].DnlinkPDUMsgID <> $SC_$CPU_CF_TBLPBCHAN[1].DnlinkPDUMsgID) then
  write "<*> Passed (4000.3) - CF allowed unique message ids for each playback channel."
  ut_setrequirements CF_40003, "P"
else
  write "<!> Failed (4000.3) - CF did not allow different message ids on the playback channels."
  ut_setrequirements CF_40003, "F"
endif 

;; Check the number of Playback Channels in use
for i = 0 to CF_MAX_PLAYBACK_CHANNELS-1 do
  write "            Playback Channel ", i
  write "=============================================="
  write "    Channel In Use = '",p@$SC_$CPU_CF_TBLPBCHAN[i].EntryInUse, "'"
  write "    Dequeue Enable = '",p@$SC_$CPU_CF_TBLPBCHAN[i].DeqEnable, "'"
  write "    Downlink PDU MsgID = '",p@$SC_$CPU_CF_TBLPBCHAN[i].DnlinkPDUMsgID,"'"
  write "    Pending Queue Depth = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PendingQDepth,"'"
  write "    History Queue Depth = '",p@$SC_$CPU_CF_TBLPBCHAN[i].HistoryQDepth,"'"
  write "    Channel Name = '",p@$SC_$CPU_CF_TBLPBCHAN[i].ChanName,"'"
  write "    Handshake Semaphore Name = '",p@$SC_$CPU_CF_TBLPBCHAN[i].SemName,"'"
  for j = 0 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    write "        Polling Directory ", j
    write "    =============================================="
    write "    Dir In Use = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse, "'"
    write "    Enable State = '",p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EnableState, "'"
    write "    Class = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class, "'"
    write "    Priority = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].Priority, "'"
    write "    Preserve File = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].Preserve, "'"
    write "    Source Path = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].SrcPath, "'"
    write "    Destination Path = '",$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].DstPath, "'"
  enddo
enddo

wait 5

write ";***********************************************************************"
write ";  Step 4.3: Send the Table Services commands to update the CF "
write ";  Configuration Table with the load file created above. This should "
write ";  fail since the table cannot be updated while CF is running. "
write ";***********************************************************************"
cmdCtr = $SC_$CPU_TBL_CMDPC + 1

;; Load the table
s load_table("$cpu_cf_cfg3.tbl", "$CPU",7)

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table Load command sent successfully."
else
  write "<!> Failed - Table Load command did not execute successfully."
endif

;; Validate the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate table
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME="CF.ConfigTable"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command sent successfully."
else
  write "<!> Failed - Validate command did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validation successful message rcv'd"
else
  write "<!> Failed - Failed Validation"
endif

;; Setup for the CF Error event for a table load attempt
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_TBL_LD_ATTEMPT_EID, "ERROR", 1

;; Send the command to activate the table
cmdctr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME="CF.ConfigTable"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command sent successfully."
else
  write "<!> Failed - Activate command did not execute successfully."
endif

ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate failed as expected. Error message rcv'd"
else
  write "<!> Failed - Activate error message was not rcv'd"
endif

wait 5

step5:
write ";***********************************************************************"
write ";  Step 5.0: Clean-up - Send the Power-On Reset command. "
write ";***********************************************************************"
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
write ";  End procedure $SC_$CPU_cf_quemgmt"
write ";*********************************************************************"
ENDPROC
