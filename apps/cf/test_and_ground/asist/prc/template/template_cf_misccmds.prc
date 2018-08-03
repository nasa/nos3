PROC $sc_$cpu_cf_misccmds
;*******************************************************************************
;  Test Name:  cf_misccmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application miscellaneous commands function properly. Both valid and
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
;    CF3001	The CF Application shall, upon command, playback all files from
;		the command-specified directory, excluding subdirectories, using
;		the command-specified playback priority, playback channel, and
;		peer entity ID.
;    CF5000	The CF Application shall provide a command to freeze all
;		transactions in progress.
;    CF5001	The CF Application shall provide a command to thaw all frozen
;		transactions.
;    CF5002	The CF Application shall support the following CFDP protocol
;		timer configurations via commands:
;			a. Modify the following protocol timer settings:
;			1.1 Ack Timer Value (seconds)
;			1.2 Nak Timer Value (seconds)
;			1.3 Inactivity Timer Value (seconds)
;			b. Modify the following protocol counter settings:
;			2.1 Max number of Ack Timeouts
;			2.2 Max number of Nak Timeouts
;    CF5002.1	The CF Application shall allow default timer/counter values to
;		be set via table parameters
;    CF5003	Upon command, CF shall issue an event message containing the
;		current value of the command-specified timer/counter setting.
;    CF5003.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified configuration parameter is
;		not defined.
;    CF5004	Upon command, CF shall send all configuration settings in a
;		telemetry message to the ground.
;    CF5006	The CF Application shall provide a command to abandon a 
;		a command-specified transaction.
;    CF5006.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified transaction is not 
;		currently in progress. 
;    CF5007	The CF Application shall provide a command to abandon all
;		transactions.
;    CF5007.1	The CF Application shall issue an error event message and reject
;		the command if there are currently no transactions in progress.
;    CF5008	The CF Application shall provide a command to send a diagnostics
;		packet for a command-specified transaction.
;    CF5008.1	The CF Application shall issue an error event message and reject
;		the command if the command-specified transaction is not found in
;		the pending, active or history queue. 
;    CF5009	The CF Application shall provide a command to write all active
;		transactions to a file.
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
;	06/02/10	Walt Moleski	Original implementation.
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

#define CF_1002		0
#define CF_1003		1
#define CF_1004		2
#define CF_3000		3
#define CF_3001		4
#define CF_5000		5
#define CF_5001		6
#define CF_5002		7
#define CF_50021	8
#define CF_5003		9
#define CF_50031	10
#define CF_5004		11
#define CF_5006		12
#define CF_50061	13
#define CF_5007		14
#define CF_50071	15
#define CF_5008		16
#define CF_50081	17
#define CF_5009		18
#define CF_6000		19
#define CF_7000		20

global ut_req_array_size = 20
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1002", "CF_1003", "CF_1004", "CF_3000", "CF_3001", "CF_5000", "CF_5001", "CF_5002", "CF_5002.1", "CF_5003", "CF_5003.1", "CF_5004", "CF_5006", "CF_5006.1", "CF_5007", "CF_5007.1", "CF_5008", "CF_5008.1", "CF_5009", "CF_6000", "CF_7000" ]

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
page $SC_$CPU_CF_CONFIG_TBL
page $SC_$CPU_CF_CFG_PARAMS
page $SC_$CPU_CF_TRANS_DIAG
page $SC_$CPU_CF_ACTIVE_TRANS
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
queId = "0F0D"
actvTransId = "0F0E"

if ("$CPU" = "CPU2") then
  hkPktId = "p1B0"
  queId = "0F2D"
  actvTransId = "0F2E"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2B0"
  queId = "0F4D"
  actvTransId = "0F4E"
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
local Filename1 = class1DirName & "class1file.dat"
local Filename2 = class1DirName & "file2.dat"
local Filename3 = class1DirName & "file3.txt"
local Filename4 = class2DirName & "class2file.txt"

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_OK_EID, "INFO", 1

;; Upload files used in the playback commands to $CPU
;;s ftp_file("RAM:0/class1", "$cpu_cf_defcfg.tbl", "class1file.dat", "$CPU", "P")
cfdp_dir "put -class1 $cpu_cf_defcfg.tbl 0.24 {Filename1}"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Upload successful for '",Filename1,"'"
else
  write "<!> Failed - Upload of '",Filename1,"'"
endif

;;s ftp_file("RAM:0/class1", "$cpu_cf_cfg2.tbl", "file2.dat", "$CPU", "P")
cfdp_dir "put -class1 $cpu_cf_cfg2.tbl 0.24 {Filename2}"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 2, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Upload successful for '",Filename2,"'"
else
  write "<!> Failed - Upload of '",Filename2,"'"
endif

;;s ftp_file("RAM:0/class1", "$cpu_cf_defcfg.tbl", "file3.txt", "$CPU", "P")
cfdp_dir "put -class1 $cpu_cf_defcfg.tbl 0.24 {Filename3}"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 3, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Upload successful for '",Filename3,"'"
else
  write "<!> Failed - Upload of '",Filename3,"'"
endif

;; Upload files used in the playback commands to $CPU
;;s ftp_file("RAM:0/class2", "$cpu_cf_defcfg.tbl", "class2file.txt", "$CPU", "P")
cfdp_dir "put -class1 $cpu_cf_defcfg.tbl 0.24 {Filename4}"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 4, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Upload successful for '",Filename4,"'"
else
  write "<!> Failed - Upload of '",Filename4,"'"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Miscellaneous Command Testing "
write ";***********************************************************************"
write ";  Step 2.1: Send the playback directory command using a directory that "
write ";  contains multiple files. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_DIR_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackDir Class_1 {class1ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcPath="/ram/class1/" DstPath=destDir

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

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the Freeze command to pause all transactions. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_FREEZE_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Freeze

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5000) - Freeze command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5000, "P"
else
  write "<!> Failed (1003;5000) - Freeze command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5000, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Freeze command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Freeze command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

;; Check the HK Telemetry items indicates that transactions are Frozen
if ($SC_$CPU_CF_NumFrozen = 1) AND (p@$SC_$CPU_CF_PartnersFrozen = "True") then
  write "<*> Passed (5000) - HK values indicate transactions are Frozen"
  ut_setrequirements CF_5000, "P"
else
  write "<!> Failed (5000) - Freeze command HK values are not set properly"
  ut_setrequirements CF_5000, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send the Freeze command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000020491"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000020490"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000020493"
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
write ";  Step 2.4: Send the Thaw command to resume all transactions. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_THAW_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Thaw

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5001) - Thaw command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5001, "P"
else
  write "<!> Failed (1003;5000) - Thaw command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5001, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Thaw command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Thaw command event message was not rcv'd"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Thaw command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000020590"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000020591"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000020592"
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
write ";  Step 2.6: Send the Get Ack Limit command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetAckLimit

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Ack Limit command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Ack Limit command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Ack Limit command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Ack Limit command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
local eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the default table
if (eventText = $SC_$CPU_CF_TBLAckLimit) then
  write "<*> Passed (5002.1) - Ack Limit retrieved is equal to Table value"
  ut_setrequirements CF_50021, "P"
else
  write "<!> Failed (5002.1) - Ack Limit retrieved is NOT equal to Table value"
  ut_setrequirements CF_50021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7: Send the Get Ack Timeout command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetAckTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Ack Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Ack Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Ack Timeout command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Ack Timeout command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the default table
if (eventText = $SC_$CPU_CF_TBLAckTimeout) then
  write "<*> Passed (5002.1) - Ack Timeout retrieved is equal to Table value"
  ut_setrequirements CF_50021, "P"
else
  write "<!> Failed (5002.1) - Ack Timeout retrieved is NOT equal to Table value"
  ut_setrequirements CF_50021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Send the Get Inactivity Timeout command to retrieve the "
write ";  current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetInactivTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Inactivity Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Inactivity Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Inactivity Timeout command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Inactivity Timeout command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the default table
if (eventText = $SC_$CPU_CF_TBLInactTimeout) then
  write "<*> Passed (5002.1) - Inactivity Timeout retrieved is equal to Table value"
  ut_setrequirements CF_50021, "P"
else
  write "<!> Failed (5002.1) - Inactivity Timeout retrieved is NOT equal to Table value"
  ut_setrequirements CF_50021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Send the Get Nak Limit command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetNakLimit

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Nak Limit command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Nak Limit command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Nak Limit command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Nak Limit command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the default table
if (eventText = $SC_$CPU_CF_TBLNakLimit) then
  write "<*> Passed (5002.1) - Nak Limit retrieved is equal to Table value"
  ut_setrequirements CF_50021, "P"
else
  write "<!> Failed (5002.1) - Nak Limit retrieved is NOT equal to Table value"
  ut_setrequirements CF_50021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.10: Send the Get Nak Timeout command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetNakTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Nak Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Nak Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Nak Timeout command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Nak Timeout command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the default table
if (eventText = $SC_$CPU_CF_TBLNakTimeout) then
  write "<*> Passed (5002.1) - Nak Timeout retrieved is equal to Table value"
  ut_setrequirements CF_50021, "P"
else
  write "<!> Failed (5002.1) - Nak Timeout retrieved is NOT equal to Table value"
  ut_setrequirements CF_50021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.11: Send the Save Incomplete Files command to retrieve the "
write ";  current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetSaveIncompFiles

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Save Incomplete Files command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Save Incomplete Files command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Save Incomplete Files command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Save Incomplete Files command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the default table
if (%lower(eventText) = $SC_$CPU_CF_TBLSaveIncomplete) then
  write "<*> Passed (5002.1) - Save Incomplete Files retrieved is equal to Table value"
  ut_setrequirements CF_50021, "P"
else
  write "<!> Failed (5002.1) - Save Incomplete Files retrieved is NOT equal to Table value"
  ut_setrequirements CF_50021, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Send the Get command for an undefined configuration "
write ";  parameter. This should generate a warning from the CFDP Engine."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_WARN_EID, "INFO", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_GetInvalidParam

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5003) - Invalid Get command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1004;5003) - Invalid Get command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003.1) - Invalid Get command event message rcv'd"
  ut_setrequirements CF_50031, "P"
else
  write "<!> Failed (5003.1) - Invalid Get command error event message was not rcv'd"
  ut_setrequirements CF_50031, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.13: Send the Get command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000220B69"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000220B68"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000220BF4"
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
write ";  Step 2.14: Send the Set Ack Limit command with a value that is "
write ";  different than the current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

local newLimit = "20"

;; Send the Command
/$SC_$CPU_CF_SetAckLimit NumTries=newLimit

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Set Ack Limit command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1003;5002) - Set Ack Limit command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Set Ack Limit command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Set Ack Limit command event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.15: Send the Get Ack Limit command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetAckLimit

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Ack Limit command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Ack Limit command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Ack Limit command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Ack Limit command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the value set in Step 2.14
if (eventText = newLimit) then
  write "<*> Passed (5002) - Ack Limit retrieved is equal to value set"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Ack Limit retrieved is NOT equal to value set"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.16: Send the Set Ack Timeout command with a value that is "
write ";  different than the current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

local newTimeout = "60"

;; Send the Command
/$SC_$CPU_CF_SetAckTimeout NumSeconds=newTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Set Ack Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1003;5002) - Set Ack Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Set Ack Timeout command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Set Ack Timeout command event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.17: Send the Get Ack Timeout command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetAckTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Ack Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Ack Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Ack Timeout command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Ack Timeout command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the value set in Step 2.16
if (eventText = newTimeout) then
  write "<*> Passed (5002) - Ack Timeout retrieved is equal to value set"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Ack Timeout retrieved is NOT equal to value set"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.18: Send the Set Inactivity Timeout command with a value that "
write ";  is different than the current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_SetInactivTimeout NumSeconds=newTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Set Inactivity Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1003;5002) - Set Inactivity Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Set Inactivity Timeout command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Set Inactivity Timeout command event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.19: Send the Get Inactivity Timeout command to retrieve the "
write ";  current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetInactivTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Inactivity Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Inactivity Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Inactivity Timeout command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Inactivity Timeout command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the value set in Step 2.18
if (eventText = newTimeout) then
  write "<*> Passed (5002) - Inactivity Timeout retrieved is equal to value set"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Inactivity Timeout retrieved is NOT equal to value set"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.20: Send the Set Nak Limit command with a value that is "
write ";  different than the current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_SetNakLimit NumTries=newLimit

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Set Nak Limit command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1003;5002) - Set Nak Limit command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Set Nak Limit command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Set Nak Limit command event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.21: Send the Get Nak Limit command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetNakLimit

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Nak Limit command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Nak Limit command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Nak Limit command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Nak Limit command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the value set in Step 2.20
if (eventText = newLimit) then
  write "<*> Passed (5002) - Nak Limit retrieved is equal to value set"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Nak Limit retrieved is NOT equal to value set"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.22: Send the Set Nak Timeout command with a value that is "
write ";  different than the current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_SetNakTimeout NumSeconds=newTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Set Nak Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1003;5002) - Set Nak Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Set Nak Timeout command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Set Nak Timeout command event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.23: Send the Get Nak Timeout command to retrieve the current "
write ";  configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetNakTimeout

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Nak Timeout command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Nak Timeout command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Nak Timeout command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Nak Timeout command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the value set in Step 2.22
if (eventText = newTimeout) then
  write "<*> Passed (5002) - Nak Timeout retrieved is equal to value set"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Nak Timeout retrieved is NOT equal to value set"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.24: Send the Set Save Incomplete Files command with a value "
write ";  that is different than the current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

local newSaveFilesValue = "yes"

;; Send the Command
/$SC_$CPU_CF_SetSaveIncompFiles YesorNo=newSaveFilesValue

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5002) - Set Save Incomplete Files command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1003;5002) - Set Save Incomplete Files command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Set Save Incomplete Files command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Set Save Incomplete Files command event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.25: Send the Get Save Incomplete Files command to retrieve the"
write ";  current configuration parameter setting. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_GET_MIB_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_GetSaveIncompFiles

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5003) - Get Save Incomplete Files command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (1003;5003) - Get Save Incomplete Files command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5003, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5003) - Get Save Incomplete Files command event message rcv'd"
  ut_setrequirements CF_5003, "P"
else
  write "<!> Failed (5003) - Get Save Incomplete Files command event message was not rcv'd"
  ut_setrequirements CF_5003, "F"
endif

;; Parse the value out of the above event
eventText = $SC_$CPU_find_event[1].event_txt

;; loop until the last space
slashLoc = %locate(eventText," ")
while (slashLoc <> 0) do
  eventText = %substring(eventText,slashLoc+1,%length(eventText))
  slashLoc = %locate(eventText," ")
enddo

;; Compare the event's value with the value set in Step 2.22
if (%lower(eventText) = newSaveFilesValue) then
  write "<*> Passed (5002) - Save Incomplete Files value retrieved is equal to value set"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Save Incomplete Files value retrieved is NOT equal to value set"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.26: Send the Set Configuration Parameter command for an  "
write ";  undefined configuration parameter. This will generate an error from "
write ";  the CFDP Engine"
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_SetInvalidParam

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5002) - Invalid Set command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (1004;5002) - Invalid Set command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_5002, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5002) - Invalid Set command event message rcv'd"
  ut_setrequirements CF_5002, "P"
else
  write "<!> Failed (5002) - Invalid Set command error event message was not rcv'd"
  ut_setrequirements CF_5002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.27: Send the Set command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000320ADA"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000320ADB"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000320AD8"
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
write ";  Step 2.28: Send the Send Configuration command to generate a "
write ";  telemetry packet containing all the configuration settings. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SND_CFG_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_SendCfg

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5004) - Send Configuration command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5004, "P"
else
  write "<!> Failed (1003;5004) - Send Configuration command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5004, "F"
endif

;; Check for the command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5004) - Send Configuration command event message rcv'd"
  ut_setrequirements CF_5004, "P"
else
  write "<!> Failed (5004) - Send Configuration command event message was not rcv'd"
  ut_setrequirements CF_5004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.29: Send the Send Configuration command with invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000020E9B"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000020E9A"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000020E99"
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
write ";  Step 2.30: Send the Playback File command using CFDP Unacknowledged "
write ";  Mode (Class 1) and a large file. "
write ";***********************************************************************"
;; Upload the large file to use for this step
s ftp_file("RAM:0/class1", "cf_largefile.dat", "cf_largefile.dat", "$CPU", "P")

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1

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

wait 5

write ";***********************************************************************"
write ";  Step 2.31: Send the Abandon command for the transaction created in "
write ";  the step above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Abandon TransIdorFilename="/ram/class1/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5006) - Abandon command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5006, "P"
else
  write "<!> Failed (1003;5006) - Abandon command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5006, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Abandon command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.32: Send the Abandon command again for the transaction just "
write ";  abandoned. This should generate an error."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_Abandon TransIdorFilename="/ram/class1/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5006) - Abandon command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_5006, "P"
else
  write "<!> Failed (1004;5006) - Abandon command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_5006, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5006.1) - Abandon error event message rcv'd."
  ut_setrequirements CF_50061, "P"
else
  write "<!> Failed (5006.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_ERR1_EID,"."
  ut_setrequirements CF_50061, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.33: Send the Playback Directory and File commands to have "
write ";  multiple transactions occurring. "
write ";***********************************************************************"
;; Upload the second large file to use for this step
s ftp_file("RAM:0/class2", "cf_largefile.dat", "cf_largefile2.dat", "$CPU", "P")

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class2ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/cf_largefile2.dat" DestFilename="cf_largefile2.dat"

wait 5

;; Start an uplink 
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

wait 5

write ";***********************************************************************"
write ";  Step 2.34: Send the Write Active Transaction Information command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_WRACT_TRANS_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Get All transaction info
s get_cf_file_to_cvt("/ram/$cpu_activeinfo","$cpu_activeinfo","$CPU",actvTransId,"All",class1Channel)

;; Check for the Command event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5009) - Write Active Transaction Information command event message rcv'd."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5009, "P"
else
  write "<!> Failed (1003;5009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_WRACT_TRANS_EID,"."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5009, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.35: Send the Abandon All Transactions command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Abandon TransIdorFilename="All"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5007) - Abandon All command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5007, "P"
else
  write "<!> Failed (1003;5007) - Abandon All command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5007, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Abandon All command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

;; Mark Requirement 5007.1 Failed since the requirement 
;; was not deleted as expected
write "<!> Failed (5007.1) - Abandon All command with no active transactions did not generate an error event."
ut_setrequirements CF_50071, "F"

wait 5

write ";***********************************************************************"
write ";  Step 2.36: Send the Abandon command with invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000004209AC"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000004209AD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004209AE"
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
write ";  Step 2.37: Send the Playback File command using CFDP Unacknowledged "
write ";  Mode (Class 1) and a large file. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class2ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/cf_largefile.dat" DestFilename="cf_largefile.dat"

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
write ";  Step 2.38: Send the Send Transaction Diagnostics command for the "
write ";  transaction above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SND_TRANS_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_SendTransDiag TransIdorFilename="/ram/class1/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5008) - Send Transaction Diagnostics command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5008, "P"
else
  write "<!> Failed (1003;5008) - Send Transaction Diagnostics command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5008, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5008) - Send Transaction Diagnostics command event message rcv'd."
  ut_setrequirements CF_5008, "P"
else
  write "<!> Failed (5008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_SND_TRANS_CMD_EID,"."
  ut_setrequirements CF_5008, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.39: Send the Send Transaction Diagnostics command for a "
write ";  transaction that does not exist. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SND_TRANS_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; Send the Command
/$SC_$CPU_CF_SendTransDiag TransIdorFilename="/boot/apps/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;5008) - Send Transaction Diagnostics command failed as expected."
  ut_setrequirements CF_1004, "P"
  ut_setrequirements CF_5008, "P"
else
  write "<!> Failed (1004;5008) - Send Transaction Diagnostics command did not increment CMDEC."
  ut_setrequirements CF_1004, "F"
  ut_setrequirements CF_5008, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5008.1) - Send Transaction Diagnostics command event message rcv'd."
  ut_setrequirements CF_50081, "P"
else
  write "<!> Failed (5008.1) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_SND_TRANS_CMD_EID,"."
  ut_setrequirements CF_50081, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.40: Send the Send Transaction Diagnostics command with an "
write ";  invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000420CA9"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000420CAA"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000420CAB"
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
write ";  Step 2.41: Send the Write Active Transaction Information command."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_WRACT_TRANS_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
s get_cf_file_to_cvt("/ram/$cpu_activeinfo2","$cpu_activeinfo2","$CPU",actvTransId,"Outgoing",class1Channel)

;; Check for the Command event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  ut_setrequirements CF_1003, "P"
  write "<*> Passed (1003;5009) - Write Active Transaction Information command event message rcv'd."
  ut_setrequirements CF_5009, "P"
else
  write "<!> Failed (1003;5009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_WRACT_TRANS_EID,"."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5009, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.42: Send the Abandon command for the first transaction listed "
write ";  in the Active Transactions display page. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

local transID = $SC_$CPU_CF_TransInfo[1].SrcEntityID & "_" & $SC_$CPU_CF_TransInfo[1].TransNum

;; Send the Command
/$SC_$CPU_CF_Abandon TransIdorFilename=transID

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5007) - Abandon All command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5007, "P"
else
  write "<!> Failed (1003;5007) - Abandon All command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5007, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Abandon All command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.43: Start an uplink and send the Write Active Transactions "
write ";  command for the incoming files only."
write ";***********************************************************************"
;; Start an uplink 
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

wait 5

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_WRACT_TRANS_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
s get_cf_file_to_cvt("/ram/$cpu_activeinfo3","$cpu_activeinfo3","$CPU",actvTransId,"Incoming",class1Channel)

;; Check for the Command event message
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  ut_setrequirements CF_1003, "P"
  write "<*> Passed (1003;5009) - Write Active Transaction Information command event message rcv'd."
  ut_setrequirements CF_5009, "P"
else
  write "<!> Failed (1003;5009) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_WRACT_TRANS_EID,"."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5009, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.44: Send the Send Transaction Diagnostics command for the "
write ";  uplink transaction started above. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SND_TRANS_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

transID = $SC_$CPU_CF_TransInfo[1].SrcEntityID & "_" & $SC_$CPU_CF_TransInfo[1].TransNum

;; Send the Command
/$SC_$CPU_CF_SendTransDiag TransIdorFilename=transID

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5008) - Send Transaction Diagnostics command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5008, "P"
else
  write "<!> Failed (1003;5008) - Send Transaction Diagnostics command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5008, "F"
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (5008) - Send Transaction Diagnostics command event message rcv'd."
  ut_setrequirements CF_5008, "P"
else
  write "<!> Failed (5008) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_SND_TRANS_CMD_EID,"."
  ut_setrequirements CF_5008, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.45: Send the Write Active Transactions command with an "
write ";  invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000004416A9"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000004416AA"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004416AB"
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
write ";  Step 2.46: Send the Write Active Transactions command with an "
write ";  invalid queue type argument. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_WRACT_ERR1_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C000004316A903"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C000004316AA03"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C000004316AB03"
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
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CF_WRACT_ERR1_EID, "."
  ut_setrequirements CF_1004, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.47: Send the KickStart command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_KICKSTART_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_KickStart Chan_0

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - KickStart command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - KickStart command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

wait 10

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - KickStart command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_KICKSTART_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.48: Send the KickStart command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C00000061786"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C00000061787"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000061784"
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
write ";  Step 2.49: Send the Playback File command using CFDP Unacknowledged "
write ";  Mode (Class 1) and a large file. "
write ";***********************************************************************"
local commaLoc = 0
local spaceLoc = 0

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class2ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/cf_largefile.dat" DestFilename="cf_largefile.dat"

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

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  ;; Parse the event text for the transaction ID
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
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.50: Send the Quick Status command with a transaction ID.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_QUICK_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_QuickStatus TransIdorFilename=transID

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Quick Status command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Quick Status command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

wait 5

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - KickStart command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_QUICK_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.51: Send the Quick Status command with a filename.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_QUICK_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_QuickStatus TransIdorFilename="/ram/class1/cf_largefile.dat"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Quick Status command sent properly."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Quick Status command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
endif

wait 5

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - KickStart command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_QUICK_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.52: Send the Quick Status command with an invalid length. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_CF_CMDEC + 1

;; CPU1 is the default
rawcmd = "18B3C0000042188B"

if ("$CPU" = "CPU2") then
  rawcmd = "19B3C0000042188A"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1AB3C00000421889"
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
write ";  Step 2.53: Send the Abandon All Transactions command.  "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_Abandon TransIdorFilename="All"

ut_tlmwait  $SC_$CPU_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5007) - Abandon All command sent properly."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5007, "P"
else
  write "<!> Failed (1003;5007) - Abandon All command did not increment CMDPC."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5007, "F"
endif

wait 10

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1003) - Abandon All command event message rcv'd."
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_CARS_CMD_EID,"."
  ut_setrequirements CF_1003, "F"
endif

wait 5

step3:
write ";***********************************************************************"
write ";  Step 3.0: Clean-up - Send the Power-On Reset command. "
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
write ";  End procedure $SC_$CPU_cf_misccmds"
write ";*********************************************************************"
ENDPROC
