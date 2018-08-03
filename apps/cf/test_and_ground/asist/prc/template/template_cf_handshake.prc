PROC $sc_$cpu_cf_handshake
;*******************************************************************************
;  Test Name:  cf_handshake
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application can interface with another application to control the
;	downlink PDU data rate.
;
;  Requirements Tested
;    CF1003	If CF accepts any command as valid, CF shall execute the
;		command, increment the CF Valid Command Counter and issue an
;		event message.
;    CF3000	The CF Application shall, upon command, playback a command-
;		specified file from the command-specified directory using CFDP
;		Unacknowledged Mode (Class 1 Service) or CFDP Acknowledged Mode
;		(Class 2 Service) using a command-specified playback priority,
;		playback channel, and peer entity ID.
;    CF3006	CF shall allow another application to control the PDU output
;		rate.
;    CF5005	The CF application shall provide a command to cancel a command-
;		specified transaction.
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
;	08/10/10	Walt Moleski	Original Procedure.
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
#include "tst_cf_events.h"
#include "tst_cf2_events.h"

%liv (log_procedure) = logging

#define CF_1003		0
#define CF_3000		1
#define CF_3006		2
#define CF_5005		3
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
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1003", "CF_3000", "CF_3006", "CF_5005", "CF_6000", "CF_7000" ]

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
s $sc_$cpu_cf_tbl2

;; Parse the filename configuration parameter for the default table
local tableFileName = CF_CONFIG_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")

;; loop until all slashes are found
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo

write "==> Parsed default Config Table filename = '",tableFileName, "'"

;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_cfg2.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_CF_HK
page $SC_$CPU_CF_CONFIG_TBL
page $SC_$CPU_TST_CF_HK
page $SC_$CPU_TST_CF2_HK

write ";***********************************************************************"
write ";  Step 1.3: Start the Handshake application. "
write ";***********************************************************************"
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
write ";  Step 1.4: Start the CFDP (CF) and Test (TST_CF) Applications and "
write ";  verify that the housekeeping packet is being generated and the HK "
write ";  data is initialized properly. "
write ";***********************************************************************"
s $sc_$cpu_cf_start_apps("1.4")
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
write ";  Step 1.5: Enable DEBUG Event Messages "
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
write ";  Step 1.6: Find an enabled Class 1 and Class 2 Polling Directory in "
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
         (i <> class2Channel) AND ;;
         (class1Channel = 99) then
        class1Channel = i
        class1Dir = j
      endif

      ;; CLASS 2
      if (p@$SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].EntryInUse = "Yes") AND ;;
         ($SC_$CPU_CF_TBLPBCHAN[i].PollDir[j].CF_Class = 2) AND ;;
         (i <> class1Channel) AND ;;
         (class2Channel = 99) then
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
write ";  Step 1.7: Using the TST_CF application, create the directories needed"
write ";  for this test procedure and upload files to these directories. "
write ";***********************************************************************"
;; If the channels were found above, create the required directories
;; Otherwise, create the default directories
local chan1DirName = "/ram/class1"
local chan2DirName = "/ram/class2"

if (class1Channel <> 99) AND (class1Dir <> 99) then
  chan1DirName = $SC_$CPU_CF_TBLPBCHAN[class1Channel].PollDir[class1Dir].SrcPath
endif

if (class2Channel <> 99) AND (class2Dir <> 99) then
  chan2DirName = $SC_$CPU_CF_TBLPBCHAN[class2Channel].PollDir[class2Dir].SrcPath
endif

local cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class1 directory
/$SC_$CPU_TST_CF_DirCreate DirName=chan1DirName

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

cmdCtr = $SC_$CPU_TST_CF_CMDPC + 1

;; Send the TST_CF command to create the class2 directory
/$SC_$CPU_TST_CF_DirCreate DirName=chan2DirName

ut_tlmwait  $SC_$CPU_TST_CF_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_CF Create Directory command sent properly."
else
  write "<!> Failed - TST_CF Create Directory command did not increment CMDPC."
endif

;; Upload files used in the playback commands to $CPU
s ftp_file("RAM:0/class1", "$cpu_cf_defcfg.tbl", "class1file.dat", "$CPU", "P")
wait 2

;; Upload files used in the playback commands to $CPU
s ftp_file("RAM:0/class2", "$cpu_cf_defcfg.tbl", "class2file.txt", "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Handshake Test "
write ";***********************************************************************"
write ";  Step 2.1: Send the Playback File command using Class 1. Verify that "
write ";  the playback does not start and the Red Light counter is incrementing"
write ";  on the channel that the playback was requested. "
write ";***********************************************************************"
local eventText = ""
local transID = ""
local commaLoc = 0
local spaceLoc = 0

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_1 {class1ChannelVal} Priority=2 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class1/class1file.dat" DestFilename="class1file.dat"

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
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Send the Handshake Application command to allow 5 PDUs to  "
write ";  be processed. Verify that the counters are set properly. "
write ";***********************************************************************"
;; Set the expected Green Light Counter value
local expGreenLightCtr = $SC_$CPU_CF_DownlinkChan[class1Channel].GreenLightCtr + 5

ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_PROCESS_PDU_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TST_CF2_CMDPC + 1

;; Send the Command
/$SC_$CPU_TST_CF2_ProcessPDUs NumPDUs=5 Channel=class1Channel

ut_tlmwait  $SC_$CPU_TST_CF2_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Process PDUs command sent properly."
else
  write "<!> Failed - Process PDUs command did not increment CMDPC."
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Process PDUs command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF2_PROCESS_PDU_INF_EID,"."
endif

;; Check the GreenLight counter
ut_tlmwait $SC_$CPU_CF_DownlinkChan[class1Channel].GreenLightCtr, {expGreenLightCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3006) - Green Light counter incremented as expected."
  ut_setrequirements CF_3006, "P"
else
  write "<!> Failed (3006) - Green Light Counter = ",$SC_$CPU_CF_DownlinkChan[class1Channel].GreenLightCtr,"; Expected ",expGreenLightCtr
  ut_setrequirements CF_3006, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: Send the Handshake Application command to allow 4 more PDUs"
write ";  to be processed. Verify that the counters are set properly. "
write ";***********************************************************************"
;; Set the expected Green Light Counter value
expGreenLightCtr = $SC_$CPU_CF_DownlinkChan[class1Channel].GreenLightCtr + 4

ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_PROCESS_PDU_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TST_CF2_CMDPC + 1

;; Send the Command
/$SC_$CPU_TST_CF2_ProcessPDUs NumPDUs=4 Channel=class1Channel

ut_tlmwait  $SC_$CPU_TST_CF2_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Process PDUs command sent properly."
else
  write "<!> Failed - Process PDUs command did not increment CMDPC."
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Process PDUs command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF2_PROCESS_PDU_INF_EID,"."
endif

;; Check the GreenLight counter
ut_tlmwait $SC_$CPU_CF_DownlinkChan[class1Channel].GreenLightCtr, {expGreenLightCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3006) - Green Light counter incremented as expected."
  ut_setrequirements CF_3006, "P"
else
  write "<!> Failed (3006) - Green Light Counter = ",$SC_$CPU_CF_DownlinkChan[class1Channel].GreenLightCtr,"; Expected ",expGreenLightCtr
  ut_setrequirements CF_3006, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Send the Cancel Playback command for the playback started  "
write ";  above in Step 2.1. In order for the Cancel to work, a PDU must be "
write ";  processed by the Handshake application (TST_CF2)."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_FAILED_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_PROCESS_PDU_INF_EID, "INFO", 3

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

cmdCtr = $SC_$CPU_TST_CF2_CMDPC + 1

;; Send the Command
/$SC_$CPU_TST_CF2_ProcessPDUs NumPDUs=1 Channel=class1Channel

ut_tlmwait  $SC_$CPU_TST_CF2_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Process PDUs command sent properly."
else
  write "<!> Failed - Process PDUs command did not increment CMDPC."
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - Process PDUs command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF2_PROCESS_PDU_INF_EID,"."
endif

;; Check for the Termination event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Transaction Cancelled event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_OUT_TRANS_FAILED_EID,"."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.5: Send the Playback File command using Class 2. Verify that "
write ";  the playback does not start and the Red Light counter is incrementing"
write ";  on the channel that the playback was requested. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PLAYBACK_FILE_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_START_EID, "INFO", 2

cmdCtr = $SC_$CPU_CF_CMDPC + 1

;; Send the Command
/$SC_$CPU_CF_PlaybackFile Class_2 {class2ChannelVal} Priority=5 Keep_File PeerEntityID="0.23" SrcFilename="/ram/class2/class2file.txt" DestFilename=""

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
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Send the Handshake Application command to allow 5 PDUs to  "
write ";  be processed. Verify that the counters are set properly. "
write ";***********************************************************************"
;; Set the expected Green Light Counter value
expGreenLightCtr = $SC_$CPU_CF_DownlinkChan[class2Channel].GreenLightCtr + 5

ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_PROCESS_PDU_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TST_CF2_CMDPC + 1

;; Send the Command
/$SC_$CPU_TST_CF2_ProcessPDUs NumPDUs=5 Channel=class2Channel

ut_tlmwait  $SC_$CPU_TST_CF2_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Process PDUs command sent properly."
else
  write "<!> Failed - Process PDUs command did not increment CMDPC."
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Process PDUs command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF2_PROCESS_PDU_INF_EID,"."
endif

;; Check the GreenLight counter
ut_tlmwait $SC_$CPU_CF_DownlinkChan[class2Channel].GreenLightCtr, {expGreenLightCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3006) - Green Light counter incremented as expected."
  ut_setrequirements CF_3006, "P"
else
  write "<!> Failed (3006) - Green Light Counter = ",$SC_$CPU_CF_DownlinkChan[class2Channel].GreenLightCtr,"; Expected ",expGreenLightCtr
  ut_setrequirements CF_3006, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7: Send the Handshake Application command to allow 4 more PDUs"
write ";  to be processed. Verify that the counters are set properly. "
write ";***********************************************************************"
;; Set the expected Green Light Counter value
expGreenLightCtr = $SC_$CPU_CF_DownlinkChan[class2Channel].GreenLightCtr + 4

ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_PROCESS_PDU_INF_EID, "INFO", 1

cmdCtr = $SC_$CPU_TST_CF2_CMDPC + 1

;; Send the Command
/$SC_$CPU_TST_CF2_ProcessPDUs NumPDUs=4 Channel=class2Channel

ut_tlmwait  $SC_$CPU_TST_CF2_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Process PDUs command sent properly."
else
  write "<!> Failed - Process PDUs command did not increment CMDPC."
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Process PDUs command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF2_PROCESS_PDU_INF_EID,"."
endif

;; Check the GreenLight counter
ut_tlmwait $SC_$CPU_CF_DownlinkChan[class2Channel].GreenLightCtr, {expGreenLightCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3006) - Green Light counter incremented as expected."
  ut_setrequirements CF_3006, "P"
else
  write "<!> Failed (3006) - Green Light Counter = ",$SC_$CPU_CF_DownlinkChan[class2Channel].GreenLightCtr,"; Expected ",expGreenLightCtr
  ut_setrequirements CF_3006, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Send the Cancel Playback command for the playback started  "
write ";  above in Step 2.5. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CARS_CMD_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_OUT_TRANS_FAILED_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", "TST_CF2", TST_CF2_PROCESS_PDU_INF_EID, "INFO", 3

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

cmdCtr = $SC_$CPU_TST_CF2_CMDPC + 1

;; Send the Command
/$SC_$CPU_TST_CF2_ProcessPDUs NumPDUs=1 Channel=class2Channel

ut_tlmwait  $SC_$CPU_TST_CF2_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Process PDUs command sent properly."
else
  write "<!> Failed - Process PDUs command did not increment CMDPC."
endif

;; Check for the Command event message
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - Process PDUs command event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",TST_CF2_PROCESS_PDU_INF_EID,"."
endif

;; Check for the Termination event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Transaction Cancelled event message rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",CF_OUT_TRANS_FAILED_EID,"."
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
write ";  End procedure $SC_$CPU_cf_handshake"
write ";*********************************************************************"
ENDPROC
