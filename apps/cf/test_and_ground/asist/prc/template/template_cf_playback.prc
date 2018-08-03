PROC $sc_$cpu_cf_playback
;*******************************************************************************
;  Test Name:  cf_playback
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application file playback commands function properly. Both valid and
;	invalid commands will be tested to ensure that the CF application
;	handles these properly.
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
;    CF3000	The CF Application shall, upon command, playback a
;		command-specified file from the command-specified directory
;		using CFDP Unacknowledged Mode (Class 1 Service) or CFDP
;		Acknowledged Mode (Class 2 Service) using a command-specified
;		playback priority, playback channel, and peer entity ID.
;    CF3000.1	CF shall issue an error event message and reject a playback file
;		command if command-specified file is currently open.
;    CF3000.2	CF shall issue an error event message if the command-specified
;		file is currently being played back.
;    CF3000.3	CF shall issue an error event message if the command-specified
;		file is not found.
;    CF3000.4	CF shall issue an error event message and reject a playback file
;		command if the playback pending queue is full.
;    CF3000.5	CF shall issue an error event message and reject a playback file
;		command if the command-specified playback channel number is not
;		valid.
;    CF3000.6	CF shall issue an error event message and reject a playback file
;		command if the command-specified class number is not valid.
;    CF3001	The CF Application shall, upon command, playback all files from
;		the command-specified directory, excluding subdirectories, using
;		the command-specified playback priority, playback channel, and
;		peer entity ID.
;    CF3001.1	The CF Application shall skip playback of files that are open.
;    CF3001.3	The CF Application shall issue an error event if the
;		command-specified directory is not found.
;    CF3001.5	The CF Application shall issue an error event and reject a
;		playback directory command if the playback pending queue is full
;    CF3001.6	The CF Application shall issue and error event and reject a
;		playback directory command if the command-specified playback
;		channel number is not valid.
;    CF3001.7	The CF Application shall issue and error event and reject a
;		playback directory command if the command-specified class
;		number is not valid.
;    CF3005	The CF Application shall encapsulate all playback CFDP PDUs in
;		SB messages.
;    CF3005.1	The CF Application shall assign the Message ID given in the
;		configuration table.
;    CF3007	The CF Application shall, if there is a series of files to be
;		played back, wait until the engine indicates "EOF Sent" on the
;		file currently being played back before starting playback for
;		the next file in the playback pending queue on the corresponding
;		playback channel.
;    CF3008	If there is no playback in progress, CF shall check the playback
;		pending queue for a file at the Table_Defined rate.
;    CF3009	The CF Application shall provide a call-back routine that
;		executes each time a file or group of files is placed on the
;		playback pending queue.
;    CF5005	The CF Application shall provide a command to cancel a
;		command-specified transaction.
;    CF5005.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified transaction is currently
;		not in progress. 
;    CF5014	The CF Application shall support a maximum downlink PDU size of
;		<PLATFORM_DEFINED> bytes.
;    CF5015	The CF Application shall provide a command to cancel all
;		transactions in progress.
;    CF5015.1	The CF Application shall issue an error event message if there
;		are no transactions in progress.
;    CF5016	The CF Application shall provide a command to suspend a
;		command-specified transaction.
;    CF5016.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified transaction is not in
;		progress.
;    CF5017	The CF Application shall provide a command to suspend all
;		transactions in progress.
;    CF5017.1	The CF Application shall issue an error event message and reject
;		the command if there are no transactions in progress.
;    CF5018	The CF Application shall provide a command to resume a
;		command-specified paused transaction.
;    CF5018.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified transaction is not 
;		currently suspended.
;    CF5019	The CF Application shall provide a command to resume all paused
;		transactions.
;    CF5019.1	The CF Application shall issue an error event message and reject
;		the command if there are no suspended transactions.
;    CF5020	The CF Application shall provide a command to purge a
;		command-specified playback pending or history queue.
;    CF5020.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified queue is not defined.
;    CF5021	The CF Application shall provide a command to write the
;		command-specified queue contents to a command-specified file.
;    CF5021.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified queue is not defined.
;    CF5022	The CF Application shall provide a command to dequeue a
;		command-specified file on the playback pending or history queue.
;    CF5022.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified file is not found.
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
;	This step uses only two (2) channels to playback files. The channel
;	array is searched for an enabled Class1 and Class2 channel. If these do
;	not exist, the test will fail.
;
;  Change History
;
;	Date		   Name		Description
;	04/06/10	Walt Moleski	Original Procedure.
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
;	Step 4.12 requires the tester to review the log file to verify CF_3007
;
;*******************************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cf_platform_cfg.h"
#include "cf_events.h"
#include "cf_defs.h"
#include "tst_cf_events.h"

%liv (log_procedure) = logging

#define CF_1002		0
#define CF_1003		1
#define CF_1004		2
#define CF_3000		3
#define CF_30001	4
#define CF_30002	5
#define CF_30003	6
#define CF_30004	7
#define CF_30005	8
#define CF_30006	9
#define CF_3001		10
#define CF_30011	11
#define CF_30013	12
#define CF_30015	13
#define CF_30016	14
#define CF_30017	15
#define CF_3005		16
#define CF_30051	17
#define CF_3007		18
#define CF_3008		19
#define CF_3009		20
#define CF_5005		21
#define CF_50051	22
#define CF_5014		23
#define CF_5015		24
#define CF_50151	25
#define CF_5016		26
#define CF_50161	27
#define CF_5017		28
#define CF_50171	29
#define CF_5018		30
#define CF_50181	31
#define CF_5019		32
#define CF_50191	33
#define CF_5020		34
#define CF_50201	35
#define CF_5021		36
#define CF_50211	37
#define CF_5022		38
#define CF_50221	39
#define CF_6000		40
#define CF_7000		41

global ut_req_array_size = 41
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1002", "CF_1003", "CF_1004", "CF_3000", "CF_3000.1", "CF_3000.2", "CF_3000.3", "CF_3000.4","CF_3000.5","CF_3000.6","CF_3001", "CF_3001.1", "CF_3001.3", "CF_3001.5", "CF_3001.6", "CF_3001.7", "CF_3005", "CF_3005.1", "CF_3007", "CF_3008", "CF_3009", "CF_5005", "CF_5005.1", "CF_5014", "CF_5015", "CF_5015.1", "CF_5016", "CF_5016.1", "CF_5017", "CF_5017.1", "CF_5018", "CF_5018.1", "CF_5019", "CF_5019.1", "CF_5020", "CF_5020.1", "CF_5021", "CF_5021.1", "CF_5022", "CF_5022.1", "CF_6000", "CF_7000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
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

;; Set the CF HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0B0"
queId = "0F0D"

if ("$CPU" = "CPU2") then
  hkPktId = "p1B0"
  queId = "0F2D"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2B0"
  queId = "0F4D"
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
write ";  Step 1.5: Monitor the CF application to determine if the Pending "
write ";  queues are being checked at the <TABLE_DEFINED> rate. "
write ";***********************************************************************"
local chan0QChecks = $SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr
local chan1QChecks = $SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr

wait $SC_$CPU_CF_TBLCyclesPerWakeup

;; Need to determine if the channels are Enabled
;; Check Channel 0
if ($SC_$CPU_CF_DownlinkChan[0].PendingQChkCtr > chan0QChecks) then
  write "<*> Passed (3008) - Playback Pending Queue for channel 0 is being checked"
  ut_setrequirements CF_3008, "P"
else
  write "<!> Failed (3008) - Playback Pending Queue Counter for channel 0 did not increment as expected."
  ut_setrequirements CF_3008, "F"
endif

;; Check Channel 1
if ($SC_$CPU_CF_DownlinkChan[1].PendingQChkCtr > chan1QChecks) then
  write "<*> Passed (3008) - Playback Pending Queue for channel 1 is being checked"
  ut_setrequirements CF_3008, "P"
else
  write "<!> Failed (3008) - Playback Pending Queue Counter for channel 1 did not increment as expected."
  ut_setrequirements CF_3008, "F"
endif

write ";***********************************************************************"
write ";  Step 1.6: Using the TST_CF application, create the directories needed"
write ";  for this test procedure and upload files to these directories. "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class1 directory
/$SC_$CPU_TST_CF_DirCreate DirName="/ram/class1"

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class2 directory
/$SC_$CPU_TST_CF_DirCreate DirName="/ram/class2"

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

;; Use the CFDP app to upload these files
;; Upload files used in the playback commands to $CPU
local class1Filename = "/ram/class1/class1file.dat"
local class2Filename = "/ram/class2/class2file.txt"

s ftp_file("RAM:0/class1","$cpu_cf_defcfg.tbl","class1file.dat","$CPU","P")
wait 2

;; Upload files used in the playback commands to $CPU
s ftp_file("RAM:0/class2","$cpu_cf_cfg2.tbl", "class2file.txt", "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Playback File Command Tests "
write ";***********************************************************************"
write ";  Step 2.1: Send the playback file command using CFDP Unacknowledged "
write ";  Mode (Class 1). "
write ";***********************************************************************"
;; Need to find an enabled Class 1 Polling Directory in the Config table
local class1Channel = 0
local class1Dir = 0
for i = 0 to CF_MAX_PLAYBACK_CHANNELS-1 do
  if (p@$SC_$CPU_CF_TBLPBCHAN[i].EntryInUse = "Yes") then
    ;; Need to look at PollDirs for Class 1
    for j = 0 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
      if (p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse = "Yes") AND ;;
         ($SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class = 1) then
        class1Channel = i
        class1Dir = j
        break
      endif
    enddo
  endif
enddo

write "==> Found class 1 channel = ",class1Channel,"; and dir = ",class1Dir

;; Setup the channel value to use for commands with unions
local class1ChannelVal

if (class1Channel = 0) then
  class1ChannelVal = "Chan_0"
elseif (class1Channel = 1) then
  class1ChannelVal = "Chan_1"
endif

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 4
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 5

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/class1file.dat" DestFilename="class1file.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the Command event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Wait for the completed(Deallocated) event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($SC_$CPU_find_event[2].num_found_messages = 1) then
    write "<*> Passed - Completed playback. All indication event messages rcv'd"
    write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[4].eventID
    write "<*> - Rcv'd ",$SC_$CPU_find_event[5].num_found_messages," msgs for EID=",$SC_$CPU_find_event[5].eventID
  else
    write "<*> Passed - Completed playback but did not rcv all indication event messages"
  endif
else
  write "<!> Failed - Did not rcv the expected completion event message"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the playback file command using CFDP Acknowledged "
write ";  Mode (Class 2). "
write ";***********************************************************************"
;; Need to find an enabled Class 2 Polling Directory in the Config table
local class2Channel = 0
local class2Dir = 0
for i = 0 to CF_MAX_PLAYBACK_CHANNELS-1 do
  if (p@$SC_$CPU_CF_TBLPBCHAN[i].EntryInUse = "Yes") then
    ;; Need to look at PollDirs for Class 2
    for j = 0 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
      if (p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse = "Yes") AND ;;
         ($SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class = 2) then
        class2Channel = i
        class2Dir = j
        break
      endif
    enddo
  endif
enddo

write "==> Found class 2 channel = ",class2Channel,"; and dir = ",class2Dir

;; Setup the channel value to use for commands with unions
local class2ChannelVal

if (class2Channel = 0) then
  class2ChannelVal = "Chan_0"
elseif (class2Channel = 1) then
  class2ChannelVal = "Chan_1"
endif

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 4

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Wait for the completed(Deallocated) event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Completed playback. All indication event messages rcv'd"
  write "<*> - Rcv'd ",$SC_$CPU_find_event[3].num_found_messages," msgs for EID=",$SC_$CPU_find_event[3].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[4].eventID
else
  write "<!> Failed - Did not rcv the expected completion event message"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send the Playback File command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C0000096026901000501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000096026801000501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000096026B01000501"
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
write ";  Step 2.4: Send the Playback File command for a file that is currently"
write ";  open. "
write ";***********************************************************************"
cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to open a file
/$SC_$CPU_TST_CF_Open Name="/ram/class1/class1file.dat"

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Open File command sent properly."
else
  write "<!> Failed - TST_CF Open File command did not increment CMDPC."
endif

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_FILE_ERR4_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/class1file.dat" DestFilename="class1file.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - CF Playback File command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1004;3000) - CF Playback File command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the error event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.1) - Expected Event Msg rcv'd."
  ut_setrequirements CF_30001, "P"
else
  write "<!> Failed (3000.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PB_FILE_ERR4_EID,"."
  ut_setrequirements CF_30001, "F"
endif

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to Close the file used above
/$SC_$CPU_TST_CF_Close Name="/ram/class1/class1file.dat"

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Close File command sent properly."
else
  write "<!> Failed - TST_CF Close File command did not increment CMDPC."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Playback File command for a file that is currently"
write ";  being played back. This should generate an error. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_FILE_ERR4_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 4
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 5

cmdCtr = $SC_$CPU_CF_CMDPC + 1
errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 
wait 2

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=5 Keep_file PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 

;; Check for the error event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000.2) - Open Error event message rcv'd for file currently being played back"
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_30002, "P"
else
  write "<!> Failed (1004;3000.2) -  Did not receive expected error event ",CF_PB_FILE_ERR4_EID,"."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_30002, "F"
endif

;; Check the CMDPC increment for the 1st command
if ($SC_$CPU_CF_CMDPC = cmdCtr) then
  write "<*> Passed (1003;3000) - 1st CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - 1st CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - 2nd CF Playback File command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1004;3000) - 2nd CF Playback File command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3000) - The correct number of Playback File commands event message rcv'd"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - Expected 1 Playback File command event messages; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements CF_3000, "F"
endif

;; Wait for the first playback to complete
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Completed playback. All indication event messages rcv'd"
  write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[5].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[5].num_found_messages," msgs for EID=",$SC_$CPU_find_event[6].eventID
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - 1st Playback File command did not complete successfully"
  ut_setrequirements CF_3000, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send the Playback File command for a file that does not "
write ";  exist. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_LOGIC_NAME_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_FAILED_EID, "ERROR", 3

;;NOTE: This error is detected by the engine. Thus, the CMDPC will increment
cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/filenotthere.dat" DestFilename="filenotthere.dat" 

ut_tlmwait $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - CF Playback File command sent."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1004;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3000) - Playback File command event message rcv'd"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - Playback File command event message was not rcv'd"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the error event messages
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.3) - File not found Error Event Msg rcv'd."
  ut_setrequirements CF_30003, "P"
else
  write "<!> Failed (3000.3) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_LOGIC_NAME_ERR_EID,"."
  ut_setrequirements CF_30003, "F"
endif

;; Check for the put request error event message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000.3) - Transaction error event message rcv'd"
  ut_setrequirements CF_30003, "P"
else
  write "<!> Failed (3000.3) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_OUT_TRANS_FAILED_EID,"."
  ut_setrequirements CF_30003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7: Send the Disable Dequeue command for the Class 2 playback "
write ";  channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_DisDeque {class2ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Disable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Disable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Disable Dequeue command event message rcv'd"
else
  write "<!> Failed - Disable Dequeue command event message was not rcv'd"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Send enough Playback File commands to fill up the playback "
write ";  pending queue stopped in Step 2.7 above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1

local expectedMsgs = $SC_$CPU_CF_TBLPBCHAN[class2Channel].PendingQDepth

;; Populate a directory with the max number of files that the queue can hold
local newFileName

;; Loop for the Max entries in the queue
for i = 1 to $SC_$CPU_CF_TBLPBCHAN[class2Channel].PendingQDepth do
  newFileName = "file" & i & ".dat"
  s ftp_file("RAM:0/class1", "class1file.dat", newFileName, "$CPU", "P")

  /$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/" & newFileName DestFilename=newFileName 
  wait 1

  ;; Remove the file from the directory
  s ftp_file("RAM:0/class1", "na", newFileName, "$CPU", "R")
enddo

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = expectedMsgs) then
  write "<*> Passed (3000) - Rcv'd the correct number of Playback File command event messages"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - Rcv'd ",$SC_$CPU_find_event[1].num_found_messages, " Playback File command event msgs; Expected ",expectedMsgs
  ut_setrequirements CF_3000, "F"
endif

;; Dump the Playback Pending Queue
local channelText = "pb" & class2Channel
local fileName = "$cpu_" & channelText & "pend"
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Send another Playback File command to the playback pending "
write ";  queue that was filled in the step above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_FILE_ERR3_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=1 keep_file PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - CF Playback File command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1004;3000) - CF Playback File command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the Queue Full Error Event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3000.4) - Pending Queue Full error event message rcv'd"
  ut_setrequirements CF_30004, "P"
else
  write "<!> Failed (3000.4) - Pending Queue Full error event message was not rcv'd"
  ut_setrequirements CF_30004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.10: Send the Purge Queue command for the playback pending "
write ";  queue used in the steps above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ2_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_PurgePendingQue {class2ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Purge Queue command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5020, "P"
else
  write "<!> Failed (1003) - CF Purge Queue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5020, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Purge Queue command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Purge Queue event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Dump the Playback Pending Queue
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 2.11: Send the Enable Dequeue command for the Class 2 playback "
write ";  channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_EnaDeque {class2ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Enable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Enable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Enable Dequeue command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Enable Dequeue command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Send the Playback File command with an invalid channel."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_FILE_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000095026901020501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000095026801020501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000095026B01020501"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - CF Playback File command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1004;3000) - CF Playback File command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (3000.5) - Expected Event Msg rcv'd."
  ut_setrequirements CF_30005, "P"
else
  write "<!> Failed (3000.5) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PB_FILE_ERR1_EID,"."
  ut_setrequirements CF_30005, "F"
endif

wait 5

step2_13:
write ";***********************************************************************"
write ";  Step 2.13: Send the Playback File command with an invalid class."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_FILE_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000095026900010501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000095026800010501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000095026B00010501"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - CF Playback File command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1004;3000) - CF Playback File command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the error event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3000.6) - Expected Event Msg rcv'd."
  ut_setrequirements CF_30006, "P"
else
  write "<!> Failed (3000.6) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PB_FILE_ERR1_EID,"."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_30006, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.14: Send a Playback File command with Auto Suspend enabled"
write ";***********************************************************************"
write ";  Step 2.14.1: Enable the Auto Suspend capability."
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
write ";  Step 2.14.2: Playback a file and look for the Suspend event after "
write ";  completion. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 4

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the Command event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Expected Event Msg rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Wait for the completed(Deallocated) event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Completed playback. All indication event messages rcv'd"
  write "<*> - Rcv'd ",$SC_$CPU_find_event[3].num_found_messages," msgs for EID=",$SC_$CPU_find_event[3].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[4].eventID
else
  write "<!> Failed - Did not rcv the expected completion event message"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.14.3: Disable the Auto Suspend capability."
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

step3_0:
write ";**********************************************************************"
write ";  Step 3.0: Playback Directory Command Tests "
write ";**********************************************************************"
write ";  Step 3.1: Send the Playback Directory Command using a directory that"
write ";  contains a single file. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 4
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 5

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir CLASS_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - Playback Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1003;3001) - Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback Directory command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Playback Directory command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Wait for the playback to complete
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Completed playback. All indication event messages rcv'd"
  write "<*> - Rcv'd ",$SC_$CPU_find_event[2].num_found_messages," msgs for EID=",$SC_$CPU_find_event[2].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[4].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[5].num_found_messages," msgs for EID=",$SC_$CPU_find_event[5].eventID
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (3001) - Playback Directory command did not complete successfully"
  ut_setrequirements CF_3001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.2: Send the Playback Directory command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C00000960369"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000960368"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000009603F4"
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
write ";  Step 3.3: Send the Playback Directory command for a directory that "
write ";  contains 2 open files. The directory contains 3 files. Thus, 1 should"
write ";  be played back. "
write ";***********************************************************************"
;; Upload 2 new files and open them via the TST_CF command
s ftp_file("RAM:0/class1", "class1file.dat", "file2.dat", "$CPU", "P")
s ftp_file("RAM:0/class1", "class2file.txt", "file3.txt", "$CPU", "P")

;; Send the Open command from the TST_CF application
cmdCtr = $SC_$CPU_TST_CF_CMDPC + 2

;; Send the TST_CF command to open a file
/$SC_$CPU_TST_CF_Open Name="/ram/class1/file2.dat"
wait 5
/$SC_$CPU_TST_CF_Open Name="/ram/class1/file3.txt"
wait 5

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Open File commands sent properly."
else
  write "<!> Failed - TST_CF Open File commands did not increment CMDPC properly."
endif

;; Setup for the Playback Directory Command
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 4

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=8 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - CF Playback Directory command sent properly."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1004;3001) - CF Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3001) - Playback Directory command event message rcv'd"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (3001) - Playback Directory command event message was not rcv'd"
  ut_setrequirements CF_3001, "F"
endif

;; Check that the 2 open files were skipped by verifying only 1 file was
;; played back
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001.1) - Successful Playback event message rcv'd once as expected."
  write "<*> - Rcv'd ",$SC_$CPU_find_event[3].num_found_messages," msgs for EID=",$SC_$CPU_find_event[3].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[4].eventID
  ut_setrequirements CF_30011, "P"
else
  write "<!> Failed (3001.1) - Expected 1 successful playback event message. Rcv'd ",$SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements CF_30011, "F"
endif

wait 5

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 2

;; Send the TST_CF command to Close the file used above
/$SC_$CPU_TST_CF_Close Name="/ram/class1/file2.dat"
wait 5
/$SC_$CPU_TST_CF_Close Name="/ram/class1/file3.txt"
wait 5

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Close File command sent properly."
else
  write "<!> Failed - TST_CF Close File command did not increment CMDPC."
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.4: Send the Playback Directory command for a directory "
write ";  containing a number of files. Three files will be played back."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 4

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=7 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - Playback Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1003;3001) - Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Playback Directory command event message rcv'd"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (3001) - Playback Directory command event message was not rcv'd"
  ut_setrequirements CF_3001, "F"
endif

;; Wait until all 3 files are played back
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 3
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - All Successful Playback event messages were rcv'd."
  write "<*> - Rcv'd ",$SC_$CPU_find_event[3].num_found_messages," msgs for EID=",$SC_$CPU_find_event[3].eventID
  write "<*> - Rcv'd ",$SC_$CPU_find_event[4].num_found_messages," msgs for EID=",$SC_$CPU_find_event[4].eventID
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (3001) - Expected 3 successful playback event message. Rcv'd ",$SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements CF_3001, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.5: Send the Playback Directory command for a directory that "
write ";  does not exist. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OPEN_DIR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=4 Keep_File PeerEntityID="0.23" SrcPath="/boot/apps/" DstPath=destDir

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - CF Playback Directory command sent."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1004;3001) - CF Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the Expected Error event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3001.3) - Playback Directory Error event message rcv'd"
  ut_setrequirements CF_30013, "P"
else
  write "<!> Failed (3001.3) - Playback Directory Error event message was not rcv'd"
  ut_setrequirements CF_30013, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.6: Send the Disable Dequeue command for the Class 1 playback "
write ";  channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_DisDeque {class1ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Disable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Disable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Disable Dequeue command event message rcv'd"
else
  write "<!> Failed - Disable Dequeue command event message was not rcv'd"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.7: Send the Playback Directory command using a directory that "
write ";  contains enough files to fill the pending queue stopped above. "
write ";***********************************************************************"
cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class2 directory
/$SC_$CPU_TST_CF_DirCreate DirName="/ram/fullDir"

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

;; Populate a directory with the max number of files that the queue can hold
local newFileName

;; Loop for the Max entries in the queue
for i = 1 to $SC_$CPU_CF_TBLPBCHAN[class1Channel].PendingQDepth do
  newFileName = "file" & i & ".dat"
  s ftp_file("RAM:0/fullDir", "class2file.txt", newFileName, "$CPU", "P")
enddo

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1

/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/fullDir/" DstPath=destDir

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3001) - Rcv'd Playback Dir command event message"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (3001) - Did not rcv the expected Playback Dir command msg"
  ut_setrequirements CF_3001, "F"
endif

;; Dump the Playback Pending Queue
channelText = "pb" & class1Channel
fileName = "$cpu_" & channelText & "pend"
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 3.8: Send the Playback Directory command again. This should "
write ";  generate an event indicating the queue is full. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_QDIR_PQFUL_EID, "ERROR", 2

errcnt = $SC_$CPU_CF_CMDEC + 1

/$SC_$CPU_CF_PlaybackDir CLASS_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/class2/" DstPath=destDir

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - Playback Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1004;3001) - Playback Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3001.5) - Playback Pending Queue Full event rcv'd"
  ut_setrequirements CF_30015, "P"
else
  write "<!> Failed (3001.5) - Pending Queue Full error event message was not rcv'd"
  ut_setrequirements CF_30015, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.9: Send the Purge Queue command for the playback pending queue"
write ";  used in the above steps. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ2_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_PurgePendingQue {class1ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Purge Queue command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5020, "P"
else
  write "<!> Failed (1003) - CF Purge Queue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5020, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Purge Queue command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Purge Queue event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Dump the Playback Pending Queue
channelText = "pb" & class1Channel
fileName = "$cpu_" & channelText & "pend"
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 3.10: Send the Playback Directory and File commands in order to "
write ";  test the default callback function execution. "
write ";***********************************************************************"
s ftp_file(ramDir, "class2file.txt", "lastfile.txt", "$CPU", "P")

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 2

;; Send the PlaybackDir command for the class1 directory
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

;; Dump the Playback Pending Queue
;; There should be 3 files on the pending queue
channelText = "pb" & class1Channel
fileName = "$cpu_" & channelText & "pend"
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

;; Send the PlaybackFile command using a different priority
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="" 

;; Dump the Playback Pending Queue
;; There should be 4 files on the pending queue
;; The class2file.txt should be 1st in the list
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

;; Send the PlaybackFile command using a different priority
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=8 Keep_File PeerEntityID="0.23" SrcFilename="/ram/lastfile.txt" DestFilename="" 

;; Dump the Playback Pending Queue
;; There should be 5 files on the pending queue
;; The class2file.txt should be last in the list
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

;; Verify the filenames are in the correct order
if ($SC_$CPU_CF_QINFO[1].Filename = "/ram/class2/class2file.txt") AND ;;
   ($SC_$CPU_CF_QINFO[2].Filename = "/ram/class1/class1file.dat") AND ;;
   ($SC_$CPU_CF_QINFO[3].Filename = "/ram/class1/file2.dat") AND ;;
   ($SC_$CPU_CF_QINFO[4].Filename = "/ram/class1/file3.txt") AND ;;
   ($SC_$CPU_CF_QINFO[5].Filename = "/ram/lastfile.txt") then
  write "<*> Passed (3009;5021) - Callback routine placed the files onto the queue in priority order."
  ut_setrequirements CF_3009, "P"
  ut_setrequirements CF_5021, "P"
else
  write "<!> Failed (3009;5021) - Files are not listed on the queue properly."
  ut_setrequirements CF_3009, "F"
  ut_setrequirements CF_5021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.11: Send the Purge Queue command for the playback pending "
write ";  queue used in the above steps. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ2_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_PurgePendingQue {class1ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Purge Queue command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5020, "P"
else
  write "<!> Failed (1003) - CF Purge Queue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5020, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Purge Queue command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Purge Queue event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Dump the Playback Pending Queue
channelText = "pb" & class1Channel
fileName = "$cpu_" & channelText & "pend"
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 3.12: Send the Enable Dequeue command for the playback pending "
write ";  queue used in the above steps. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_ENA_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_EnaDeque {class1ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Enable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Enable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Enable Dequeue command event message rcv'd"
else
  write "<!> Failed - Enable Dequeue command event message was not rcv'd"
endif

wait 5

write ";***********************************************************************"
write ";  Step 3.13: Send the Playback Directory command w/ an invalid channel."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_DIR_ERR1_EID, "ERROR", 2

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000095036902020501302E32330000000000000000000000002F72616D2F636C617373312F000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002F732F6F70722F6163636F756E74732F6366735F746573742F696D6167652F"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000095036802020501302E32330000000000000000000000002F72616D2F636C617373312F000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002F732F6F70722F6163636F756E74732F6366735F746573742F696D6167652F"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000095034F02020501302E32330000000000000000000000002F72616D2F636C617373312F000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002F732F6F70722F6163636F756E74732F6366735F746573742F696D6167652F"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - Playback Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1004;3001) - Playback Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3001.6) - Playback Directory error event rcv'd"
  ut_setrequirements CF_30016, "P"
else
  write "<!> Failed (3001.6) - Pending Directory error event message was not rcv'd"
  ut_setrequirements CF_30016, "F"
endif

wait 5

step3_14:
write ";***********************************************************************"
write ";  Step 3.14: Send the Playback Directory command with an invalid class."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PB_DIR_ERR1_EID, "ERROR", 2

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000095036900030501"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000095036800030501"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000095034F00030501"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001) - Playback Directory command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1004;3001) - Playback Directory command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (3001.7) - Playback Directory error event rcv'd"
  ut_setrequirements CF_30017, "P"
else
  write "<!> Failed (3001.7) - Playback Directory error event message was not rcv'd"
  ut_setrequirements CF_30017, "F"
endif

wait 5

step4_0:
write ";***********************************************************************"
write ";  Step 4.0: Other Command Tests "
write ";***********************************************************************"
write ";  Step 4.1: Send the playback file command using CFDP Unacknowledged "
write ";  Mode (Class 1) and a large file. "
write ";***********************************************************************"
;; Upload the large file to use for this step
s ftp_file("RAM:0/class1", "cf_largefile.dat", "cf_largefile.dat", "$CPU", "P")

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/cf_largefile.dat" DestFilename="cf_largefile.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback File command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the Command event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Playback File started indication rcv'd."
else
;;  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_OUT_TRANS_START_EID,"."
  write "<!> Failed - Expected Event message not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.2: Send the cancel playback command for the file used above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_FAILED_EID, "ERROR", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Cancel TransIdorFilename="/ram/class1/cf_largefile.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Cancel command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5005, "P"
else
  write "<!> Failed (1003;5005) - Cancel command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5005, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Cancel command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the Termination event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Machine deallocated event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_OUT_TRANS_FAILED_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.3: Send the playback file command using CFDP Unacknowledged "
write ";  Mode (Class 1) and a large file. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/cf_largefile.dat" DestFilename="cf_largefile.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback File command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

local eventText = ""
local transID = ""
local commaLoc = 0
local spaceLoc = 0

;; Check for the Transaction Start event message
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  eventText = $SC_$CPU_find_event[2].event_txt
  commaLoc = %locate(eventText,",")
  eventText = %substring(eventText,1,commaLoc-1)
  ;; Parse out the spaces since the Transaction ID is right before the comma
  spaceLoc = %locate(eventText," ")
  while (spaceLoc <> 0) do
    eventText = %substring(eventText,spaceLoc+1,%length(eventText))
    spaceLoc = %locate(eventText," ")
  enddo
  transID = eventText 
  write "==> Parsed transaction ID = '", transID, "' from start event."
else
  write "<!> Failed - Did not rcv Transaction Start Event message. Could not parse Transaction ID."
  transID = "/ram/class1/cf_largefile.dat" 
endif
wait 5

write ";***********************************************************************"
write ";  Step 4.4: Send the cancel playback command for the file used above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_FAILED_EID, "ERROR", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Cancel TransIdorFilename=transID 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Cancel command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5005, "P"
else
  write "<!> Failed (1003;5005) - Cancel command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5005, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Cancel command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the Termination event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Machine deallocated event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_OUT_TRANS_FAILED_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.5: Send the cancel playback command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C000004208A2"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000004208A3"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004208A0"
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
write ";  Step 4.6: Send the cancel playback command sent in Step 4.2. This "
write ";  command should fail since the transaction is no longer in progress."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_Cancel TransIdorFilename="/ram/class1/cf_largefile.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Cancel command failed as expected."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5005, "P"
else
  write "<!> Failed (1003;5005) - Cancel command did not increment CMDEC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5005, "F"
endif

;; Check for the Error event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5005.1) - Cancel command Error event message rcv'd."
  ut_setrequirements CF_50051, "P"
else
  write "<!> Failed (5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_ERR1_EID,"."
  ut_setrequirements CF_50051, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.7: Send the cancel playback command sent in Step 4.4. This "
write ";  command should fail since the transaction is no longer in progress."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_Cancel TransIdorFilename=transID

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005) - Cancel command failed as expected."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5005, "P"
else
  write "<!> Failed (1003;5005) - Cancel command did not increment CMDEC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5005, "F"
endif

;; Check for the Error event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5005.1) - Cancel command Error event message rcv'd."
  ut_setrequirements CF_50051, "P"
else
  write "<!> Failed (5005.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CFDP_ENGINE_ERR_EID,"."
  ut_setrequirements CF_50051, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.8: Send several playback file commands for files that will "
write ";  take a bit of time to download and that get placed on more than one "
write ";  pending queue."
write ";***********************************************************************"
;; Upload the large file to use for this step
s ftp_file(ramDir, "cf_largefile.dat", "cf_largefile1.dat", "$CPU", "P")
wait 5

s ftp_file(ramDir, "cf_largefile.dat", "cf_largefile2.dat", "$CPU", "P")
wait 5

/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=8 Keep_File PeerEntityID="0.23" SrcFilename="/ram/cf_largefile1.dat" DestFilename="cf_largefile1.dat" 
wait 5

/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=8 Keep_File PeerEntityID="0.23" SrcFilename="/ram/cf_largefile2.dat" DestFilename="cf_largefile2.dat" 

wait 5

step4_6:
write ";***********************************************************************"
write ";  Step 4.9: Send the Cancel All Playbacks Command. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_FAILED_EID, "ERROR", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Cancel TransIdorFilename="All" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5015) - Cancel command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5015, "P"
else
  write "<!> Failed (1003;5015) - Cancel command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5015, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Cancel command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Wait until you get 2 Machine Deallocated indication events
;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5015) - Cancel command successful. Both transactions were terminated."
  ut_setrequirements CF_5015, "P"
else
  write "<!> Failed (5015) - Did not rcv the expected Event messages. Expected 2 of ",CF_OUT_TRANS_FAILED_EID,". Only rcv'd ",$SC_$CPU_find_event[2].num_found_messages
  ut_setrequirements CF_5015, "F"
endif

;; Mark Requirement 5015.1 Failed since the requirement
;; was not deleted as expected
write "<!> Failed (5015.1) - Cancel All command with no active transactions did not generate an error event."
ut_setrequirements CF_50151, "F"

wait 5

write ";***********************************************************************"
write ";  Step 4.10: Send the Playback File Command using CFDP Class 2 with the"
write ";  file deletion parameter set."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Delete_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Playback File command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the playback completion indication message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Playback File completed."
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - Did not rcv the machine deallocated indication."
  ut_setrequirements CF_3000, "F"
endif

;; Need to verify that the file no longer exists on $CPU
;; Send the command again. If the file was deleted, it should fail
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_LOGIC_NAME_ERR_EID, "ERROR", 2

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename="class2file.txt" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Playback File command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the error event message since the file does not exist
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Playback File command failed as expected. The file was deleted by the previous command."
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - Playback File command did not generate the expected error. File must still exist."
  ut_setrequirements CF_3000, "F"
endif

;; replace the file into the class2 directory
s ftp_file("RAM:0/class2", "class2file.txt", "class2file2.txt", "$CPU", "P")
wait 5

write ";***********************************************************************"
write ";  Step 4.11: Send the Playback File Command using CFDP Class 1. This "
write ";  step verifies that the downlinked PDUs are contained in SB messages."
write ";***********************************************************************"
local currentPDUSize, eventText

;; Get the current setting in order to reset it after this step
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "DEBUG", 1

/$SC_$CPU_CF_GetOutgoingSize
wait 5

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  ;; Parse the value out of the above event
  eventText = $SC_$CPU_find_event[1].event_txt

  ;; loop until the last space
  slashLoc = %locate(eventText," ")
  while (slashLoc <> 0) do
    eventText = %substring(eventText,slashLoc+1,%length(eventText))
    slashLoc = %locate(eventText," ")
  enddo

  ;; Set the current PDU size
  currentPDUSize = eventText
else
  ;; Set the value to the Table specified value
  currentPDUSize = $SC_$CPU_CF_TBLOutChunkSize
endif

local maxPDUSize = CF_MAX_OUTGOING_CHUNK_SIZE - 16
local maxPDUSizeTxt = "" & maxPDUSize & ""
;; Send the command to set the outgoing PDU size to its maximum
/$SC_$CPU_CF_SetOutgoingSize NumBytes=maxPDUSizeTxt
wait 5

;; Send a TST_CF command prior to the playback in order to capture
;; the downlinked PDUs
ut_setupevents "$SC", "$CPU", "TST_CF", TST_CF_CAPTUREPDU_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

/$SC_$CPU_TST_CF_CapturePDUs MsgID=$SC_$CPU_CF_TBLPBChan[class1Channel].DnlinkPDUMsgID

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF PDU Capture command sent properly."
else
  write "<!> Failed - TST_CF PDU Capture command did not increment CMDPC."
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - PDU Capture command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF_CAPTUREPDU_INF_EID,"."
endif

;; Setup for the Playback
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/class1file.dat" DestFilename="class1file.dat" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - CF Playback File command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (1003;3000) - CF Playback File command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3000, "F"
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Playback File command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_FILE_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the Successful Transaction event message
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Playback File completed successfully."
  ut_setrequirements CF_3000, "P"
else
  write "<!> Failed (3000) - File playback success message not rcv'd."
  ut_setrequirements CF_3000, "F"
endif

;; Need to verify that the PDUs were contained in SB messages 
;; with the proper MID
;; 3005 & 3005.1
if ($SC_$CPU_TST_CF_PDUsRcvd > 0) then
  write "<*> Passed (3005;3005.1) - PDUs were captured by TST_CF"
  ut_setrequirements CF_3005, "P"
  ut_setrequirements CF_30051, "P"
else
  write "<!> Failed (3005;3005.1) - PDUs were not captured properly."
  ut_setrequirements CF_3005, "F"
  ut_setrequirements CF_30051, "F"
endif

;; Send the command to Stop capturing PDUs
ut_setupevents "$SC", "$CPU", "TST_CF", TST_CF_STOPPDUCAPTURE_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

/$SC_$CPU_TST_CF_StopPDUCapture

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Stop PDU Capture command sent properly."
else
  write "<!> Failed - TST_CF Stop PDU Capture command did not increment CMDPC."
endif

;; Check whether the CF application allowed the MAX PDU Size
;; Note the size reported contains the CCSDS header which is 12 bytes
if ($SC_$CPU_TST_CF_MaxPDUSize = CF_MAX_OUTGOING_CHUNK_SIZE+12) then
  write "<*> Passed (5014) - The MAX PDU Size was captured"
  ut_setrequirements CF_5014, "P"
else
  write "<!> Failed (5014) - The MAX PDU Size captured = ",$SC_$CPU_TST_CF_MaxPDUSize, "; Expected ",CF_OUTGOING_PDU_BUF_SIZE+12
  ut_setrequirements CF_5014, "F"
endif

;; Reset the Outgoing PDU Size
/$SC_$CPU_CF_SetOutgoingSize NumBytes=currentPDUSize

wait 10

write ";***********************************************************************"
write ";  Step 4.12: Send the Playback Directory Command using a directory "
write ";  containing multiple files. This step verifies that the files are "
write ";  played back sequentially such that the next file is started after the"
write ";  EOF_Sent indication is received from the CFDP engine. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_OK_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 3

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=7 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - CF Playback Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1003;3001) - CF Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback Directory command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_DIR_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Wait for 4 Machine Deallocated event messages
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 3, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3007) - Rcv'd 3 of 4 Machine Deallocated messages. Analyze the log file to verify this requirement."
  ut_setrequirements CF_3007, "A"
else
  write "<!> Failed (3007) - Rcv'd ",$SC_$CPU_find_event[2].num_found_messages, " for EID = ",CF_OUT_TRANS_OK_EID," Expected 3."
  ut_setrequirements CF_3007, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.13: Extract the transaction ID from the last start event."
write ";***********************************************************************"
;; Get the Transaction Start event message for the last transaction
eventText = $SC_$CPU_find_event[3].event_txt

commaLoc = %locate(eventText,",")
eventText = %substring(eventText,1,commaLoc-1)

;; Parse out the spaces since the Transaction ID is right before the comma
spaceLoc = %locate(eventText," ")
while (spaceLoc <> 0) do
  eventText = %substring(eventText,spaceLoc+1,%length(eventText))
  spaceLoc = %locate(eventText," ")
enddo

transID = eventText 
write "==> Parsed transaction ID = '", transID, "' from start event."

wait 5

write ";***********************************************************************"
write ";  Step 4.14: Send the Suspend Playback Command for the current file "
write ";  being played back using the transaction ID parsed above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_SUS_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Suspend TransIdorFilename=transID

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5016) - Suspend command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5016, "P"
else
  write "<!> Failed (1003;5016) - Suspend command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5016, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Suspend command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (5016) - Transaction suspended event message rcv'd."
  ut_setrequirements CF_5016, "P"
else
  write "<!> Failed (5016) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_IND_XACT_SUS_EID,"."
  ut_setrequirements CF_5016, "F"
endif

;; Mark Requirement 5016.1 Failed since the requirement
;; was not deleted as expected
write "<!> Failed (5016.1) - Suspend command with a non-active transaction did not generate an error event."
ut_setrequirements CF_50161, "F"

wait 5

write ";***********************************************************************"
write ";  Step 4.15: Send the Suspend Playback Command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C000004206AD"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000004206AC"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004206AF"
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
write ";  Step 4.16: Send the Resume Playback Command for the file suspended in"
write ";  Step 4.14 above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Resume TransIdorFilename=transID

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5018) - Resume command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5018, "P"
else
  write "<!> Failed (1003;5018) - Resume command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5018, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Resume command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Mark Requirement 5018.1 Failed since the requirement
;; was not deleted as expected
write "<!> Failed (5018.1) - Resume command with a transaction that is not suspended did not generate an error event."
ut_setrequirements CF_50181, "F"

wait 5

write ";***********************************************************************"
write ";  Step 4.17: Send the Resume Playback Command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C000004207AC"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000004207AD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004207AE"
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
write ";  Step 4.18: Send the Suspend Playback Command for the current file "
write ";  being played back."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_SUS_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Suspend TransIdorFilename="/ram/class1/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5016) - Suspend command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5016, "P"
else
  write "<!> Failed (1003;5016) - Suspend command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5016, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Suspend command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed (5016) - Transaction suspended event message rcv'd."
  ut_setrequirements CF_5016, "P"
else
  write "<!> Failed (5016) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_IND_XACT_SUS_EID,"."
  ut_setrequirements CF_5016, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.19: Send the Resume Playback Command for the file suspended in"
write ";  Step 4.18 above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Resume TransIdorFilename="/ram/class1/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5018) - Resume command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5018, "P"
else
  write "<!> Failed (1003;5018) - Resume command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5018, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Resume command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.20: Send several Playback Directory Commands that will queue "
write ";  files on more than one pending queue."
write ";***********************************************************************"
;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=7 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir
wait 5

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_2 {class2ChannelVal} Priority=7 Keep_File PeerEntityID="0.23" SrcPath="/ram/class2/" DstPath=destDir

wait 5

write ";***********************************************************************"
write ";  Step 4.21: Send the Suspend All Playbacks Command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_SUS_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Suspend TransIdorFilename="All" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5017) - CF Suspend All command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5017, "P"
else
  write "<!> Failed (1003;5017) - CF Suspend All command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5017, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Suspend command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Wait for the 2 Suspended indication events
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5017) - Rcv'd the expected number of Suspended events."
  ut_setrequirements CF_5017, "P"
else
  write "<!> Failed (5017) - Expected 2 Suspended indications; Rcv'd ",$SC_$CPU_find_event[2].num_found_messages
  ut_setrequirements CF_5017, "F"
endif

;; Mark Requirement 5017.1 Failed since the requirement
;; was not deleted as expected
write "<!> Failed (5017.1) - Suspend All command with no active transactions did not generate an error event."
ut_setrequirements CF_50171, "F"

wait 5

write ";***********************************************************************"
write ";  Step 4.22: Send the Resume All Playbacks Command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_RES_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Resume TransIdorFilename="All" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5019) - Resume All command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5019, "P"
else
  write "<!> Failed (1003;5019) - Resume All command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5019, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Resume command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the Transaction Resumed indications
if ($SC_$CPU_find_event[2].num_found_messages > 0) then
  write "<*> Passed (5019) - Transaction Resumed indication event message rcv'd."
  ut_setrequirements CF_5019, "P"
else
  write "<!> Failed (5019) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_IND_XACT_RES_EID,"."
  ut_setrequirements CF_5019, "F"
endif

;; Mark Requirement 5019.1 Failed since the requirement
;; was not deleted as expected
write "<!> Failed (5019.1) - Resume All command with no suspended transactions did not generate an error event."
ut_setrequirements CF_50191, "F"

wait 5

write ";***********************************************************************"
write ";  Step 4.23: Send the Playback Directory Command using a directory that"
write ";  contains multiple files."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_2 {class2ChannelVal} Priority=7 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - CF Playback Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1003;3001) - CF Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback Directory command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_DIR_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.24: Send the Purge Queue command for the playback pending "
write ";  queue used in the steps above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ2_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_PurgePendingQue {class2ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Purge Queue command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5020, "P"
else
  write "<!> Failed (1003) - CF Purge Queue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5020, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Purge Queue command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Purge Queue event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.25: Send the Purge Queue command for a pending queue that does"
write ";  not exist. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ_ERR4_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000005158702010300"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000005158602010300"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000005158502010300"
endif

ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5020.1) - Purge Queue command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_50201, "P"
else
  write "<!> Failed (1004;5020.1) - Purge Queue command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_50201, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5020.1) - Purge Queue command event message rcv'd"
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_50201, "P"
else
  write "<!> Failed (1004;5020.1) - Purge Queue event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_50201, "F"
endif

wait 5

step4_26:
write ";***********************************************************************"
write ";  Step 4.26: Send the Purge Queue command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C0000006158702010000"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000006158602010000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C0000006158502010000"
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
write ";  Step 4.27: Send the Playback Directory Command using a directory that"
write ";  contains multiple files."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_2 {class2ChannelVal} Priority=7 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - CF Playback Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1003;3001) - CF Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback Directory command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_DIR_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.28: Send the Dump Queue Command for all defined queues. "
write ";***********************************************************************"
write ";  Step 4.28.1: Dump the Active, Pending, and History Queues for each "
write ";  Playback Channel defined. "
write ";***********************************************************************"
for i = 0 to CF_MAX_PLAYBACK_CHANNELS-1 do
  if (p@$SC_$CPU_CF_TBLPBCHAN[i].EntryInUse = "Yes") then
    channelText = "pb" & i
    fileName = "$cpu_" & channelText & "pend"
    ;; Dump the Pending Queue
    s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

    fileName = "$cpu_" & channelText & "actv"
    ;; Dump the Active Queue
    s get_que_to_cvt(ramDir,"active",channelText,fileName,"$CPU",queId)

    fileName = "$cpu_" & channelText & "hist"
    ;; Dump the Pending Queue
    s get_que_to_cvt(ramDir,"history",channelText,fileName,"$CPU",queId)
  endif
enddo

wait 5

write ";***********************************************************************"
write ";  Step 4.28.2: Dump the Active and History Queues for Uplink Channel."
write ";***********************************************************************"
;; Dump the Active Queue
s get_que_to_cvt(ramDir,"active","up","$cpu_upactv","$CPU",queId)

fileName = "$cpu_" & channelText & "hist"
;; Dump the History Queue
s get_que_to_cvt(ramDir,"history","up","$cpu_uphist","$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 4.29: Send the Dump Queue Command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C00000460F97"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000460F97"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000460F54"
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
write ";  Step 4.30: Send the Dump Queue Command for a queue that doesn't exist"
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_WR_CMD_ERR2_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000450FDF020003"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000450FDE020003"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000450F54020003"
endif

;; Send the Command
ut_sendrawcmd "$SC_$CPU_CF", (rawcmd)

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5021.1) - Dump Queue command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_50211, "P"
else
  write "<!> Failed (1004;5021.1) - Dump Queue command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_50211, "F"
endif

;; Check for the error event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004;5021.1) - Dump Queue error event message rcv'd"
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_50211, "P"
else
  write "<!> Failed (1004;5021.1) - Dump Queue error event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_50211, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.31: Send the Disable Dequeue command for class 1 playback "
write ";  channel."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DIS_DQ_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_DisDeque {class1ChannelVal}

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Disable Dequeue command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - CF Disable Dequeue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Disable Dequeue command event message rcv'd"
else
  write "<!> Failed - Disable Dequeue command event message was not rcv'd"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.32: Send the Playback Directory Command using a directory that"
write ";  contains multiple files."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=6 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - CF Playback Directory command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_3001, "P"
else
  write "<!> Failed (1003;3001) - CF Playback Directory command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_3001, "F"
endif

;; Check for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Playback Directory command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_PLAYBACK_DIR_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Dump the queue contents
channelText = "pb" & class1Channel
fileName = "$cpu_" & channelText & "pend"
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 4.33: Send the Deque Node Command for an entry contained on the "
write ";  queue populated above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DEQ_NODE2_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_DequeNode TransIdorFileName="/ram/class1/class1file.dat"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5022) - Dequeue Node command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5022, "P"
else
  write "<!> Failed (1003;5022) - Dequeue Node command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5022, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Dequeue Node command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Dequeue Node command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Dump the Queue again to verify the file is no longer on the queue
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 4.34: Send the Deque Node Command for a file that is not on the "
write ";  queue populated above."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DEQ_NODE_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_DequeNode TransIdorFileName="/ram/class1/filenotthere.dat"

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5022;5022.1) - Dequeue Node command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_5022, "P"
  ut_setrequirements CF_50221, "P"
else
  write "<!> Failed (1004;5022;5022.1) - Dequeue Node command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_5022, "F"
  ut_setrequirements CF_50221, "F"
endif

;; Check for the error event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004;5022.1) - Dequeue Node error event message rcv'd"
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_50221, "P"
else
  write "<!> Failed (1004;5022.1) - Dequeue Node error event message was not rcv'd"
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_50221, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.35: Send the Deque Node Command for the second entry contained"
write ";  on the queue populated above using the transaction ID."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_DEQ_NODE2_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

transID = $SC_$CPU_CF_QInfo[2].SrcEntityID & "_" & $SC_$CPU_CF_QInfo[2].TransNum

;; Send the Command
/$SC_$CPU_CF_DequeNode TransIdorFileName=transID

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5022) - Dequeue Node command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5022, "P"
else
  write "<!> Failed (1003;5022) - Dequeue Node command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5022, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Dequeue Node command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Dequeue Node command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Dump the Queue again to verify the file is no longer on the queue
s get_que_to_cvt(ramDir,"pending",channelText,fileName,"$CPU",queId)

wait 5

write ";***********************************************************************"
write ";  Step 4.36: Send the Deque Node Command with an invalid length."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1
 
;; CPU1 is the default
rawcmd = "18B3C00000461497020000002F72616D2F636C617373312F636C6173733166696C652E646174"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000461497020000002F72616D2F636C617373312F636C6173733166696C652E646174"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004614CD020000002F72616D2F636C617373312F636C6173733166696C652E646174"
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
write ";  Step 4.37: Send the Purge History Queue command for each playback "
write ";  channel. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ2_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 2

/$SC_$CPU_CF_PurgeOutHistoryQue Chan_0
wait 2
/$SC_$CPU_CF_PurgeOutHistoryQue Chan_1

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - CF Purge Queue command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5020, "P"
else
  write "<!> Failed (1003) - CF Purge Queue command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5020, "F"
endif

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Purge Queue command event messages rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - The correct number of Purge Queue event messages were not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

wait 5

step5:
write ";***********************************************************************"
write ";  Step 5.0: Clean-up - Send the Power-On Reset command. "
write ";***********************************************************************"
write ";  Step 5.1: Send the Cancel All Transactions command. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Cancel TransIdorFilename="All" 

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5015) - Cancel command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5015, "P"
else
  write "<!> Failed (1003;5015) - Cancel command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5015, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Cancel command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 5.2: Send the Power-On Reset command. "
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
write ";  End procedure $SC_$CPU_cf_playback"
write ";*********************************************************************"
ENDPROC
