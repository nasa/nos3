PROC $sc_$cpu_cf_faultdet
;*******************************************************************************
;  Test Name:  cf_faultdet
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application detects the faults listed in its requirements.
;
;
;  Requirements Tested
;    CF2000	The CF application shall receive files from the ground using
;		CFDP Unacknowledged Mode (Class 1 Service) or CFDP Acknowledged
;		Mode (Class 2 Service).
;    CF2001	The CF application shall extract uplinked CFDP PDUs from SB
;		messages.
;    CF2002	The CF application shall extract file data from File Data PDUs
;		and construct a complete file in the meta-data specified
;		directory identical to the original transmitted file.
;    CF2002.1	CF shall cancel the transaction and issue an event message if
;		the engine detects that a "Fault" has occurred.
;    CF2002.1.2	The following faults shall be detected:
;			a. Positive Ack Limit Reached
;			b. deleted
;			c. deleted
;			d. Filestore Rejection
;			e. File Checksum Failure
;			f. File Size Error
;			g. NAK Limit Reached
;			h. Inactivity Detected
;			i. deleted
;			j. Suspend Request Received
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
;		       aa. Memory pool allocation remaining
;		       bb. Number of transactions abandoned
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
;		       bb. Number of transactions abandoned
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The CF commands and telemetry items exist in the GSE database.
;	The display pages exist for the CF Housekeeping.
;	A CF Test application (TST_CF) exists in order to fully test the CF
;		Application.
;
;  Assumptions and Constraints
;	This test uses CI commands that will capture and inject errors into
;	the PDUs that are uplinked by the CFDP ground engine. These commands
;	must be defined in order to run this test procedure.
;
;  Change History
;
;	Date		   Name		Description
;	08/24/10	Walt Moleski	Original Procedure.
;	11/02/10	Walt Moleski	Modified to use a variable for the app
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
#include "cf_platform_cfg.h"
#include "cf_events.h"
#include "cf_msgids.h"
#include "tst_cf_events.h"
#include "cf_defs.h"
#include "ci_lab_events.h"

%liv (log_procedure) = logging

#define CF_2000		0
#define CF_2001		1
#define CF_2002		2
#define CF_20021	3
#define CF_200212	4
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
local cfe_requirements[0 .. ut_req_array_size] = ["CF_2000", "CF_2001", "CF_2002", "CF_2002.1", "CF_2002.1.2", "CF_6000", "CF_7000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
local eventText = ""
local transID = ""
local commaLoc = 0
local spaceLoc = 0
local cfdpCmd = ""
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

;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_defcfg.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 1.2: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_CF_HK
page $SC_$CPU_TST_CF_HK
page $SC_$CPU_CI_HK

write ";***********************************************************************"
write ";  Step 1.3:  Start the CFDP (CF) and Test (TST_CF) Applications and "
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
write ";  Step 1.4: Enable DEBUG Event Messages for the proper applications. "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the appropriate applications ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=CFAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CI_LAB_APP" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Fault Detection Tests."
write ";***********************************************************************"
write ";  Step 2.1: Positive Ack Limit Reached test "
write ";***********************************************************************"
;; Remove the TO Packet so that the Ack is unable to be rcv'd
local ackMID = CF_SPACE_TO_GND_PDU_MID
local expPosAckCtr = $SC_$CPU_CF_PosAckNum + 1

/$SC_$CPU_TO_RemovePacket Stream=ackMID
wait 5

;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_FAU_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 4

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put class2file.txt 0.24 /ram/class2file.txt"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  ;; Parse the event text for the transaction ID
  eventText = $SC_$CPU_find_event[1].event_txt
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

  write "<*> Passed (2000;2001) - Class 2 file uplink started successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 2 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event messages."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"

  ;; Issue the cfdp_dir to abandon the transaction
  cfdpCmd = "abandon " & transID
  cfdp_dir {cfdpCmd}
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd. Expected 3; Rcv'd ",$SC_$CPU_find_event[4].num_found_messages
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Check that the Positive Ack Counter incremented
ut_tlmwait $SC_$CPU_CF_PosAckNum, {expPosAckCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - Positive Ack Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - Positive Ack Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

;; Restore the Ack receipt
/$SC_$CPU_TO_ADDPACKET STREAM=ackMID PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'20'

wait 5

write ";***********************************************************************"
write ";  Step 2.2: File Store Rejection test "
write ";***********************************************************************"
local expFSRejCtr = $SC_$CPU_CF_FileStoreRejNum + 1

;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_FAU_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 4

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /boot/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink started successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 1 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Verify the counter incremented
ut_tlmwait $SC_$CPU_CF_FileStoreRejNum, {expFSRejCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - File Store Rejection Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - File Store Rejection Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.3: File Checksum Failure test"
write ";***********************************************************************"
local expChecksumCtr = $SC_$CPU_CF_FileChecksumNum + 1

;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", "CI_LAB_APP", CI_CAPTUREPDU_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", "CI_LAB_APP", CI_CORRUPT_CHECKSUM_CMD_EID, "DEBUG", 2

;; Issue the CI command to Start Capturing PDUs
/$SC_$CPU_CI_CapturePDUs MsgID=CF_INCOMING_PDU_MID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - PDU Capture is now on."
else
  write "<!> Failed - PDU Capture command event message was not rcv'd"
endif

;; Issue the CI commands to corrupt the checksum and Capture PDUs
/$SC_$CPU_CI_CorruptChecksum

ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Corrupt Checksum command sent properly."
else
  write "<!> Failed - Corrupt Checksum command event message was not rcv'd"
endif

;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_FAU_EID, "DEBUG", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink started successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 1 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 80
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Verify the counter incremented
ut_tlmwait $SC_$CPU_CF_FileChecksumNum, {expChecksumCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - File Checksum Fault Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - File Checksum Fault Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: File Size Error test"
write ";***********************************************************************"
local expFileSizeCtr = $SC_$CPU_CF_FileSizeNum + 1

ut_setupevents "$SC","$CPU","CI_LAB_APP",CI_MOD_PDU_FILESIZE_CMD_EID,"DEBUG",1

;; Issue the CI command to change the file size
/$SC_$CPU_CI_ModifyPDUFileSize Subtract Amount=100

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Modify PDU File Size command sent successfully."
else
  write "<!> Failed - Modify PDU File Size event message was not rcv'd"
endif

;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_FAU_EID, "DEBUG", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink started successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 1 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 80
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Verify the counter incremented
ut_tlmwait $SC_$CPU_CF_FileSizeNum, {expFileSizeCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - File Checksum Fault Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - File Checksum Fault Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.5: NAK Limit Reached test"
write ";***********************************************************************"
local expNakLimitCtr = $SC_$CPU_CF_NakLimitNum + 1

;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IND_XACT_FAU_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 4

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put class2file.txt 0.24 /ram/class2file.txt"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 2 file uplink started successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 2 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

;; Wait until the uplink is almost complete and issue the CI command to drop 
;; enough file data pdus such that the NAK limit is reached
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 13, 100
if (UT_TW_Status = UT_Success) then
  ;; Send the CI command to drop 10 File Data PDUs
  /$SC_$CPU_CI_DropPDUs FileData dropCnt=10
  wait 2
endif

;; Check that the Nak Limit Counter incremented
ut_tlmwait $SC_$CPU_CF_NakLimitNum, {expNakLimitCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - Nak Limit Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - Nak Limit Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Inactivity Detected test"
write ";***********************************************************************"
local expInactivityCtr = $SC_$CPU_CF_InactiveNum + 1

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink completed successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"

  ;; Parse the event text for the transaction ID
  eventText = $SC_$CPU_find_event[1].event_txt
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
  write "<!> Failed (2000;2001) - Class 1 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

;; Issue the cfdp_dir to abandon the transaction
cfdpCmd = "abandon " & transID
cfdp_dir {cfdpCmd}

;; Check for the transaction failed message
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Verify that the counter incremented
ut_tlmwait $SC_$CPU_CF_InactiveNum, {expInactivityCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - Inactivity Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - Inactivity Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.7: Suspend Request Received test "
write ";***********************************************************************"
local expSuspendCtr = $SC_$CPU_CF_SuspendNum + 1

ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink completed successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"

  ;; Parse the event text for the transaction ID
  eventText = $SC_$CPU_find_event[1].event_txt
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
  write "<!> Failed (2000;2001) - Class 1 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

;; Issue the cfdp_dir to suspend the transaction
cfdpCmd = "suspend " & transID
cfdp_dir {cfdpCmd}

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Verify that the counter incremented
ut_tlmwait $SC_$CPU_CF_SuspendNum, {expSuspendCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - Suspend Request Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - Suspend Request Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Cancel Request Received test "
write ";***********************************************************************"
local expCancelCtr = $SC_$CPU_CF_CancelNum + 1

;; Setup events
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_START_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink completed successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"

  ;; Parse the event text for the transaction ID
  eventText = $SC_$CPU_find_event[1].event_txt
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
  write "<!> Failed (2000;2001) - Class 1 file uplink did not start."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

;; Issue the cfdp_dir to cancel the transaction
cfdpCmd = "cancel " & transID
cfdp_dir {cfdpCmd}

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1;2002.1.2) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002;2002.1;2002.1.2) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
  ut_setrequirements CF_200212, "F"
endif

;; Verify that the counter incremented
ut_tlmwait $SC_$CPU_CF_CancelNum, {expCancelCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002.1.2) - Cancel Request Counter incremented as expected."
  ut_setrequirements CF_200212, "P"
else
  write "<!> Failed (2002.1.2) - Cancel Request Counter did not increment."
  ut_setrequirements CF_200212, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Corrupt the Meta Data test "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "CI_LAB_APP", CI_DROP_PDU_CMD_EID, "DEBUG", 1

;; Send the CI command to drop the meta data PDU
/$SC_$CPU_CI_DropPDUs MetaData dropCnt=1

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - CI Drop PDU command sent successfully."
else
  write "<!> Failed - CI Drop PDU command event message was not rcv'd."
endif

;; Setup events
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_FAILED_EID, "ERROR", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2002;2002.1) - Rcv'd expected error event message."
  ut_setrequirements CF_2002, "P"
  ut_setrequirements CF_20021, "P"
else
  write "<!> Failed (2002;2002.1) - Expected error event was NOT rcv'd."
  ut_setrequirements CF_2002, "F"
  ut_setrequirements CF_20021, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Clean-up - Send the Power-On Reset command.              "
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
write ";  End procedure $SC_$CPU_cf_faultdet "
write ";*********************************************************************"
ENDPROC
