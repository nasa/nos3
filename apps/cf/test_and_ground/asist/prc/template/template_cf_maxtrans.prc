PROC $sc_$cpu_cf_maxtrans
;*******************************************************************************
;  Test Name:  cf_maxtrans
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application properly handles the case when the maximum number of
;	simultaneous transactions is reached.
;
;	NOTE: This test requires an alternate build of the CF application with
;	      the CF_MAX_SIMULTANEOUS_TRANSACTIONS configuration parameter set
;	      to 5.
;
;  Requirements Tested
;    CF1003	If CF accepts any command as valid, CF shall execute the
;		command, increment the CF Valid Command Counter and issue an
;		event message.
;    CF3000	The CF Application shall, upon command, playback a
;		command-specified file from the command-specified directory
;		using CFDP Unacknowledged Mode (Class 1 Service) or CFDP
;		Acknowledged Mode (Class 2 Service) using a command-specified
;		playback priority, playback channel, and peer entity ID.
;    CF5013	The CF application shall allow a maximum of <PLATFORM_DEFINED>
;		simultaneous transactions to occur at once.
;    CF5015	The CF application shall, upon command, cancel all transactions
;		in progress.
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
;	08/13/10	Walt Moleski	Original implementation.
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

%liv (log_procedure) = logging

#define CF_1003		0
#define CF_3000		1
#define CF_5013		2
#define CF_5015		3
#define CF_6000		4
#define CF_7000		5

global ut_req_array_size = 5
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1003", "CF_3000", "CF_5013", "CF_5015", "CF_6000", "CF_7000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL cmdCtr, errcnt
LOCAL workDir = %env("WORK")
LOCAL destDir = workDir & "/image/"
local CFAppName = "CF"

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
local actvTransId

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
write ";  Step 1.5: Find an enabled Class 1 and Class 2 Polling Directory in "
write ";  the Configuration table to use throughout this test. "
write ";***********************************************************************"
local class1Channel = 99
local class1Dir = 99
local class2Channel = 99
local class2Dir = 99

for i = 0 to CF_MAX_PLAYBACK_CHANNELS-1 do
  if (p@$SC_$CPU_CF_TBLPBCHAN[i].EntryInUse = "Yes") then
    ;; Need to look at PollDirs for Class 1
    for j = 0 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
      ;; CLASS 1
      if (p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse = "Yes") AND ;;
         ($SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class = 1) AND ;;
         (class1Channel = 99) and (i <> class2Channel) then
        class1Channel = i
        class1Dir = j
      endif

      ;; CLASS 2
      if (p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse = "Yes") AND ;;
         ($SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class = 2) AND ;;
         (class2Channel = 99) AND (i <> class1Channel) then
        class2Channel = i
        class2Dir = j
      endif
    enddo
  endif
enddo

write "==> Found class 1 channel = ",class1Channel,"; and dir = ",class1Dir
write "==> Found class 2 channel = ",class2Channel,"; and dir = ",class2Dir

;; Setup the channel values to use for commands with unions
local class1ChannelVal, class2ChannelVal

if (class1Channel = 0) then
  class1ChannelVal = "Chan_0"
elseif (class1Channel = 1) then
  class1ChannelVal = "Chan_1"
endif

if (class2Channel = 0) then
  class2ChannelVal = "Chan_0"
elseif (class2Channel = 1) then
  class2ChannelVal = "Chan_1"
endif

write ";***********************************************************************"
write ";  Step 1.6: Using the TST_CF application, create the directories needed"
write ";  for this test procedure and upload files to these directories. "
write ";***********************************************************************"
;; If the channels were found above, create the required directories
;; Otherwise, create the default directories
local class1DirName = "/ram/class1"
local class2DirName = "/ram/class2"

if (class1Channel <> 99) AND (class1Dir <> 99) then
  class1DirName = $SC_$CPU_CF_TBLPBCHAN[class1Channel].PollDir[class1Dir].SrcPath
endif

if (class2Channel <> 99) AND (class2Dir <> 99) then
  class2DirName = $SC_$CPU_CF_TBLPBCHAN[class2Channel].PollDir[class2Dir].SrcPath
endif

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class1 directory
/$SC_$CPU_TST_CF_DirCreate DirName=class1DirName

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class2 directory
/$SC_$CPU_TST_CF_DirCreate DirName=class2DirName

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

;; Use the CFDP Application to upload these files
;; Upload files used in the playback commands to $CPU
local class1Filename = class1DirName & "cf_largefile.dat"
local class2Filename = class2DirName & "cf_largefile.dat"

s ftp_file("RAM:0/class1","cf_largefile.dat","cf_largefile.dat","$CPU","P")
;;cfdp_dir "put -class1 cf_largefile.dat 0.24 {class1Filename}"
wait 2

s ftp_file("RAM:0/class2","cf_largefile.dat","cf_largefile2.dat","$CPU","P")
;;cfdp_dir "put -class1 cf_largefile.dat 0.24 {class2Filename}"

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Maximum Transaction Test "
write ";***********************************************************************"
write ";  Step 2.1: Send the Playback File command using CFDP Unacknowledged "
write ";  Mode (Class 1) and a large file. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=2 Keep_File PeerEntityId="0.23" SrcFilename="/ram/class1/cf_largefile.dat" DestFilename="cf_largefile.dat"

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

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the Playback File command using Class 1 on the other "
write ";  channel in order to have multiple transactions occurring. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class2ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/cf_largefile2.dat" DestFilename="cf_largefile2.dat"

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

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send uplink commands until the maximum transactions are "
write ";  executing. "
write ";***********************************************************************"
local newFileName = ""
local uplinkCmd = ""
local i

;; Start as many uplink transactions as necessary to exceed the maximum
for i = 3 to CF_MAX_SIMULTANEOUS_TRANSACTIONS do
  newFileName = "file" & i & ".dat"
  uplinkCmd = "put -class1 class1file.dat 0.24 /ram/" & newFileName
  cfdp_dir {uplinkCmd}
  wait 2
enddo

write ";***********************************************************************"
write ";  Step 2.4: Send another uplink command. This should generate an error."
write ";***********************************************************************"
;; Setup for the Error events
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PDU_RCV_ERR3_EID, "ERROR", 2

;; Start another uplink transaction - This should cause errors
cfdp_dir "put -class1 class1file.dat 0.24 /ram/file4.dat"

;; Check for the Command event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5013) - Maximum simuiltaneous transactions error message was rcv'd"
  ut_setrequirements CF_5013, "P"
else
  write "<!> Failed (5013) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CFDP_ENGINE_ERR_EID,"."
  ut_setrequirements CF_5013, "F"
endif

wait 5

step3:
write ";***********************************************************************"
write ";  Step 3.0: Clean-up - Send the Power-On Reset command. "
write ";***********************************************************************"
write ";  Step 3.1: Send the Cancel All Playbacks command. "
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

write ";***********************************************************************"
write ";  Step 3.2: Send the command to cancel all uplink transactions."
write ";***********************************************************************"
cfdp_dir "cancel all"

wait 10

write ";***********************************************************************"
write ";  Step 3.3: Send the Power-On Reset command. "
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
write ";  End procedure $SC_$CPU_cf_maxtrans"
write ";*********************************************************************"
ENDPROC
