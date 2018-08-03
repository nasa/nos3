PROC $sc_$cpu_cf_filercv
;*******************************************************************************
;  Test Name:  cf_filercv
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application adheres to its file receiving requirements.
;
;  Requirements Tested
;    CF1003	If CF accepts any command as valid, CF shall execute the 
;		command, increment the CF Valid Command Counter and issue an
;		event message.
;    CF2000	The CF application shall receive files from the ground using
;		CFDP Unacknowledged Mode (Class 1 Service) or CFDP Acknowledged
;		Mode (Class 2 Service).
;    CF2001	The CF application shall extract uplinked CFDP PDUs from SB
;		messages.
;    CF2002	The CF application shall extract file data from File Data PDUs
;		and construct a complete file in the meta-data specified
;		directory identical to the original transmitted file.
;    CF5012	The CF application shall support a maximum uplink PDU size of
;		<PLATFOR_DEFINED> bytes.
;    CF5020	The CF application shall provide a command to purge a command-
;		specified playback pending or history queue.
;    CF5021	The CF application shall provide a command to write the command-
;		specified queue contents to the command-specified file.
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
;	06/07/10	Walt Moleski	Original Procedure.
;       11/02/10        Walt Moleski    Modified to use a variable for the app
;					name, updated the requirements that
;					changed, and only enabled DEBUG events
;					for the appropriate apps.
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

%liv (log_procedure) = logging

#define CF_1003		0
#define CF_2000		1
#define CF_2001		2
#define CF_2002		3
#define CF_5012		4
#define CF_5020		5
#define CF_5021		6
#define CF_6000		7
#define CF_7000		8

global ut_req_array_size = 8
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_1003", "CF_2000", "CF_2001", "CF_2002", "CF_5012", "CF_5020", "CF_5021", "CF_6000", "CF_7000" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, cmdctr
local CFAppName = "CF"
local ramDir = "RAM:0"

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

write "==> Parsed default Config Table filename = '",tableFileName, "'"

;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_defcfg.tbl", tableFileName, "$CPU", "P")

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
write ";  Step 1.4: Enable DEBUG Event Messages for the appropriate apps. "
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
write ";  Step 2.0: File Receiving Tests."
write ";***********************************************************************"
write ";  Step 2.1: Uplink a file using CFDP Unacknowledged Mode (Class 1) "
write ";  using the default uplink PDU size."
write ";***********************************************************************"
;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_OK_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 3

;; Issue the cfdp_dir directive with the appropriate arguments
cfdp_dir "put -class1 class1file.dat 0.24 /ram/class1file.dat"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 1 file uplink completed successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 1 file uplink did not complete."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

wait 5

;; Check the number of PDUs Rcvd by CF. This whould equal the number of 
;; CF_CFDP_ENGINE_DEB_EID messages generated
if ($SC_$CPU_find_event[3].num_found_messages = $SC_$CPU_CF_PDUsRcvd) then
  write "<*> Passed (2002) - The correct number of PDUs were received and the file constructed."
  ut_setrequirements CF_2002, "P"
else
  write "<!> Failed (2002) - Did not receive the expected number of PDUs on file uplink."
  ut_setrequirements CF_2002, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.2: Change the uplink PDU size to the maximum defined by the "
write ";  platform configuration parameter (CF_INCOMING_PDU_BUF_SIZE). "
write ";***********************************************************************"
;; Need to save the current size in order to reset it
local currentSize = outgoing_file_chunk_size

;; Set the size of the uplink buffer
local pduDataSize = CF_INCOMING_PDU_BUF_SIZE
pduDataSize = pduDataSize - CF_PDU_HDR_BYTES - 4
local cfdpCmd = "outgoing_file_chunk_size=" & pduDataSize

;; Send the command to the ground engine
cfdp_dir {cfdpCmd}

;; Verify the PDU size was equal to the max (CF_5012)
ut_setrequirements CF_5012, "A"

write ";***********************************************************************"
write ";  Step 2.3: Uplink a file using CFDP Acknowledged Mode (Class 2)."
write ";***********************************************************************"
;; Setup for the events to receive
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_IN_TRANS_OK_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_INFO_EID, "DEBUG", 2
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFDP_ENGINE_DEB_EID, "DEBUG", 3

;; Save the PDU Counter
local oldPDUsRcvdCnt = $SC_$CPU_CF_PDUsRcvd
write "==> Prior to upload, PDU Count = ",oldPDUsRcvdCnt

cfdp_dir "put class2file.txt 0.24 /ram/class2file.txt"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1, 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2000;2001) - Class 2 file uplink completed successfully."
  ut_setrequirements CF_2000, "P"
  ut_setrequirements CF_2001, "P"
else
  write "<!> Failed (2000;2001) - Class 2 file uplink did not complete."
  ut_setrequirements CF_2000, "F"
  ut_setrequirements CF_2001, "F"
endif

;; Wait for the next HK cycle to update the PDUs Rcvd Count
wait 5

;; Check the number of PDUs Rcvd by CF.
;; Since this is a class 2 uplink, there are 2 extra messages at end
local pduCnt = $SC_$CPU_CF_PDUsRcvd - oldPDUsRcvdCnt 
local msgCnt = $SC_$CPU_find_event[3].num_found_messages - 2
write "==> After upload, PDU Count = ",$SC_$CPU_CF_PDUsRcvd 
write "==> Debug PDU Msg Count = ",$SC_$CPU_find_event[3].num_found_messages

if (msgCnt = pduCnt) then
  write "<*> Passed (2002) - The correct number of PDUs were received and the file constructed."
  ut_setrequirements CF_2002, "P"
else
  write "<!> Failed (2002) - Expected ",pduCnt, " number of PDUs on file uplink. Rcv'd ",msgCnt
  ut_setrequirements CF_2002, "F"
  write "PDUs Rcvd Counter before class 2 uplink = ",oldPDUsRcvdCnt 
  write "PDUs Rcvd Counter after  class 2 uplink = ",$SC_$CPU_CF_PDUsRcvd
endif

;; Reset the outgoing size on the ground engine
cfdpCmd = "outgoing_file_chunk_size=" & currentSize

;; Send the command to the ground engine
cfdp_dir {cfdpCmd}

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Dump the Uplink History Queue"
write ";***********************************************************************"
;; Setup for event
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SND_Q_INFO_EID, "DEBUG", 1

;; Dump the History Queue
s get_que_to_cvt(ramDir,"history","up","$cpu_filercv_hist","$CPU",queId)

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5021) - Dump Queue command completed successfully."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5021, "P"
else
  write "<!> Failed (1003;5021) - Dump Queue command event message was not rcv'd."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5021, "F"
endif

wait 10

write ";***********************************************************************"
write ";  Step 2.5: Purge the Uplink History Queue"
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_PURGEQ1_EID, "INFO", 1

cmdCtr = $SC_$CPU_CF_CMDPC + 1

/$SC_$CPU_CF_PurgeInHistoryQue
wait 5

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

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Purge Queue command event message rcv'd"
  ut_setrequirements CF_1003, "P"
else
  write "<!> Failed (1003) - Did not rcv the Purge Queue command event message"
  ut_setrequirements CF_1003, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Dump the Uplink History Queue to verify it is empty "
write ";***********************************************************************"
;; Setup for events
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_SND_Q_INFO_EID, "DEBUG", 1

;; Dump the History Queue
s get_que_to_cvt(ramDir,"history","up","$cpu_filercv_hist","$CPU",queId)

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5021) - Dump Queue command completed successfully."
  ut_setrequirements CF_1003, "P"
  ut_setrequirements CF_5021, "P"
else
  write "<!> Failed (1003;5021) - Dump Queue command event message was not rcv'd."
  ut_setrequirements CF_1003, "F"
  ut_setrequirements CF_5021, "F"
endif

wait 10

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
write ";  End procedure $SC_$CPU_cf_filercv"
write ";*********************************************************************"
ENDPROC
