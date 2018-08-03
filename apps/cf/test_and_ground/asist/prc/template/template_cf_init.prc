PROC $sc_$cpu_cf_init
;*******************************************************************************
;  Test Name:  cf_init
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS CCSDS File Delivery Protocol (CF)
;	application handles initialization properly for each of the possible
;	reset types (Power-On, Processor and Application).
;
;  Requirements Tested
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
;    CF7001	Upon any initialization, CF shall load the Configuration Table.
;    CF7001.1	If the Configuration Table fails validation, CF shall issue an
;		event message and exit.
;    CF7001.1.1	CF shall validate the following values in the Configuration
;		Table:
;			a. Outgoing File Chunk Size
;			b. Flight and Ground Entity Ids
;			c. Message IDs
;			d. CFDP Class 1 or 2
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The CF commands and telemetry items exist in the GSE database.
;	The display pages exist for the CF Housekeeping.
;	A CF Test application (TST_CF) exists in order to fully test the CF
;	Application.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	03/25/10	Walt Moleski	Original Procedure.
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
#include "cf_platform_cfg.h"
#include "cf_events.h"
#include "tst_cf_events.h"

%liv (log_procedure) = logging

#define CF_6000		0
#define CF_7000		1
#define CF_7001		2
#define CF_70011	3
#define CF_700111	4

global ut_req_array_size = 4
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CF_6000", "CF_7000", "CF_7001", "CF_7001.1", "CF_7001.1.1" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
local CFAppName = "CF"

write ";***********************************************************************"
write ";  Step 1.0: CFDP Test Setup."
write ";***********************************************************************"
write ";  Step 1.1: Perform a Power-On Reset "
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";***********************************************************************"
write ";  Step 1.2: Create and upload the table load file for this test.  "
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
write ";  Step 1.3: Display the Housekeeping pages "
write ";***********************************************************************"
page $SC_$CPU_CF_HK
page $SC_$CPU_CF_CONFIG_TBL
page $SC_$CPU_TST_CF_HK

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
;;write "--expected HK Cnt = ",expectedSCnt

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
   (p@$SC_$CPU_CF_PartnersFrozen = "False") AND ;;
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
  write "<!> Failed (7000) - Housekeeping telemetry NOT initialized properly at startup."
  ut_setrequirements CF_7000, "F"
  write "CMDPC           = ", $SC_$CPU_CF_CMDPC
  write "CMDEC           = ", $SC_$CPU_CF_CMDEC
  write "TotalInProgTrans = ", $SC_$CPU_CF_TotalInProgTrans
  write "TotalCompleteTrans = ", $SC_$CPU_CF_TotalCompleteTrans
  write "TotalFailedTrans = ", $SC_$CPU_CF_TotalFailedTrans
  write "PDUsRcvd = ", $SC_$CPU_CF_PDUsRcvd
  write "PDUsRejected = ", $SC_$CPU_CF_PDUsRejected
  write "ActiveQFileCnt = ", $SC_$CPU_CF_ActiveQFileCnt
  write "GoodUplinkCtr = ", $SC_$CPU_CF_GoodUplinkCtr
  write "BadUplinkCtr = ", $SC_$CPU_CF_BadUplinkCtr
  write "LastFileUplinked = '", $SC_$CPU_CF_LastFileUplinked, "'"
  write "PosAckNum = ", $SC_$CPU_CF_PosAckNum
  write "FileStoreRejNum = ", $SC_$CPU_CF_FileStoreRejNum
  write "FileChecksumNum = ", $SC_$CPU_CF_FileChecksumNum
  write "FileSizeNum = ", $SC_$CPU_CF_FileSizeNum
  write "NakLimitNum = ", $SC_$CPU_CF_NakLimitNum
  write "InactiveNum = ", $SC_$CPU_CF_InactiveNum
  write "CancelNum = ", $SC_$CPU_CF_CancelNum
  write "NumFrozen = ", $SC_$CPU_CF_NumFrozen
  write "PartnersFrozen = ", p@$SC_$CPU_CF_PartnersFrozen
  write "Frozen state = ", frozenState
  write "Chan 0 Pending Q state = ", pb0QState
  write "Chan 1 Pending Q state = ", pb1QState
  write "Chan 0 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].PendingQFileCnt
  write "Chan 1 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].PendingQFileCnt
  write "Chan 0 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].ActiveQFileCnt
  write "Chan 1 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].ActiveQFileCnt
  write "Chan 0 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
  write "Chan 1 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
  write "Chan 0 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
  write "Chan 1 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
  write "Chan 0 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
  write "Chan 1 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
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
write ";  Step 1.6: Utilizing the TST_CF application, send the command to set "
write ";  the housekeeping counters to values other than zero (0). "
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
write ";  Step 1.7: Perform a Processor Reset "
write ";***********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5


write ";***********************************************************************"
write ";  Step 1.8: Start the CFDP (CF) and Test (TST_CF) Applications and "
write ";  verify that the housekeeping packet is being generated and the HK "
write ";  data is initialized properly. "
write ";***********************************************************************"
s $sc_$cpu_cf_start_apps("1.8")
wait 5

currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2
;;write "--expected HK Cnt = ",expectedSCnt

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;7001) - Housekeeping packet is being generated."
  ut_setrequirements CF_6000, "P"
  ut_setrequirements CF_7001, "P"
else
  write "<!> Failed (6000;7001) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements CF_6000, "F"
  ut_setrequirements CF_7001, "F"
endif

;; Extract bit 0 of the engine flags and downlink flags
frozenState = %and($SC_$CPU_CF_EngineFlags,1)
pb0QState = %and($SC_$CPU_CF_DownlinkChan[0].DownlinkFlags,1)
pb1QState = %and($SC_$CPU_CF_DownlinkChan[1].DownlinkFlags,1)

;;write "--frozen state = ",frozenState
;;write "--Chan 0 Pending Q state = ",pb0QState
;;write "--Chan 1 Pending Q state = ",pb1QState

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
   (p@$SC_$CPU_CF_PartnersFrozen = "False") AND ;;
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
  write "<!> Failed (7000) - Housekeeping telemetry NOT initialized properly at startup."
  ut_setrequirements CF_7000, "F"
  write "CMDPC           = ", $SC_$CPU_CF_CMDPC
  write "CMDEC           = ", $SC_$CPU_CF_CMDEC
  write "TotalInProgTrans = ", $SC_$CPU_CF_TotalInProgTrans
  write "TotalCompleteTrans = ", $SC_$CPU_CF_TotalCompleteTrans
  write "TotalFailedTrans = ", $SC_$CPU_CF_TotalFailedTrans
  write "PDUsRcvd = ", $SC_$CPU_CF_PDUsRcvd
  write "PDUsRejected = ", $SC_$CPU_CF_PDUsRejected
  write "ActiveQFileCnt = ", $SC_$CPU_CF_ActiveQFileCnt
  write "GoodUplinkCtr = ", $SC_$CPU_CF_GoodUplinkCtr
  write "BadUplinkCtr = ", $SC_$CPU_CF_BadUplinkCtr
  write "LastFileUplinked = '", $SC_$CPU_CF_LastFileUplinked, "'"
  write "PosAckNum = ", $SC_$CPU_CF_PosAckNum
  write "FileStoreRejNum = ", $SC_$CPU_CF_FileStoreRejNum
  write "FileChecksumNum = ", $SC_$CPU_CF_FileChecksumNum
  write "FileSizeNum = ", $SC_$CPU_CF_FileSizeNum
  write "NakLimitNum = ", $SC_$CPU_CF_NakLimitNum
  write "InactiveNum = ", $SC_$CPU_CF_InactiveNum
  write "CancelNum = ", $SC_$CPU_CF_CancelNum
  write "NumFrozen = ", $SC_$CPU_CF_NumFrozen
  write "PartnersFrozen = ", p@$SC_$CPU_CF_PartnersFrozen
  write "Chan 0 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].PendingQFileCnt
  write "Chan 1 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].PendingQFileCnt
  write "Chan 0 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].ActiveQFileCnt
  write "Chan 1 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].ActiveQFileCnt
  write "Chan 0 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
  write "Chan 1 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
  write "Chan 0 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
  write "Chan 1 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
  write "Chan 0 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
  write "Chan 1 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
endif

wait 5

write ";***********************************************************************"
write ";  Step 1.9: Utilizing the TST_CF application, send the command to set "
write ";  the housekeeping counters to values other than zero (0). "
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
write ";  Step 1.10: Stop the CFDP and TST_CF applications"
write ";***********************************************************************"
/$SC_$CPU_ES_DELETEAPP Application="TST_CF"
wait 5
/$SC_$CPU_ES_DELETEAPP Application=CFAppName
wait 5

write ";***********************************************************************"
write ";  Step 1.11: Start the CFDP (CF) and Test (TST_CF) Applications and "
write ";  verify that the housekeeping packet is being generated and the HK "
write ";  data is initialized properly. "
write ";***********************************************************************"
s $sc_$cpu_cf_start_apps("1.11")
wait 5

currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2
;;write "--expected HK Cnt = ",expectedSCnt

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;7001) - Housekeeping packet is being generated."
  ut_setrequirements CF_6000, "P"
  ut_setrequirements CF_7001, "P"
else
  write "<!> Failed (6000;7001) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements CF_6000, "F"
  ut_setrequirements CF_7001, "F"
endif

;; Extract bit 0 of the engine flags and downlink flags
frozenState = %and($SC_$CPU_CF_EngineFlags,1)
pb0QState = %and($SC_$CPU_CF_DownlinkChan[0].DownlinkFlags,1)
pb1QState = %and($SC_$CPU_CF_DownlinkChan[1].DownlinkFlags,1)

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
  write "<!> Failed (7000) - Housekeeping telemetry NOT initialized properly at startup."
  ut_setrequirements CF_7000, "F"
  write "CMDPC           = ", $SC_$CPU_CF_CMDPC
  write "CMDEC           = ", $SC_$CPU_CF_CMDEC
  write "TotalInProgTrans = ", $SC_$CPU_CF_TotalInProgTrans
  write "TotalCompleteTrans = ", $SC_$CPU_CF_TotalCompleteTrans
  write "TotalFailedTrans = ", $SC_$CPU_CF_TotalFailedTrans
  write "PDUsRcvd = ", $SC_$CPU_CF_PDUsRcvd
  write "PDUsRejected = ", $SC_$CPU_CF_PDUsRejected
  write "ActiveQFileCnt = ", $SC_$CPU_CF_ActiveQFileCnt
  write "GoodUplinkCtr = ", $SC_$CPU_CF_GoodUplinkCtr
  write "BadUplinkCtr = ", $SC_$CPU_CF_BadUplinkCtr
  write "LastFileUplinked = '", $SC_$CPU_CF_LastFileUplinked, "'"
  write "PosAckNum = ", $SC_$CPU_CF_PosAckNum
  write "FileStoreRejNum = ", $SC_$CPU_CF_FileStoreRejNum
  write "FileChecksumNum = ", $SC_$CPU_CF_FileChecksumNum
  write "FileSizeNum = ", $SC_$CPU_CF_FileSizeNum
  write "NakLimitNum = ", $SC_$CPU_CF_NakLimitNum
  write "InactiveNum = ", $SC_$CPU_CF_InactiveNum
  write "CancelNum = ", $SC_$CPU_CF_CancelNum
  write "NumFrozen = ", $SC_$CPU_CF_NumFrozen
  write "PartnersFrozen = ", p@$SC_$CPU_CF_PartnersFrozen
  write "Frozen state = ", frozenState
  write "Chan 0 Pending Q state = ", pb0QState
  write "Chan 1 Pending Q state = ", pb1QState
  write "Chan 0 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].PendingQFileCnt
  write "Chan 1 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].PendingQFileCnt
  write "Chan 0 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].ActiveQFileCnt
  write "Chan 1 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].ActiveQFileCnt
  write "Chan 0 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
  write "Chan 1 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
  write "Chan 0 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
  write "Chan 1 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
  write "Chan 0 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
  write "Chan 1 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
endif

wait 5

write ";***********************************************************************"
write ";  Step 1.12: Utilizing the TST_CF application, send the command to set "
write ";  the housekeeping counters to values other than zero (0). "
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
write ";  Step 1.13: Send the command to Restart the CFDP and TST_CF apps"
write ";***********************************************************************"
/$SC_$CPU_ES_RESTARTAPP Application=CFAppName
wait 5
/$SC_$CPU_ES_RESTARTAPP Application="TST_CF"
wait 5

;; Check that HK packet is being generated
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6000;7001) - Housekeeping packet is being generated."
  ut_setrequirements CF_6000, "P"
  ut_setrequirements CF_7001, "P"
else
  write "<!> Failed (6000;7001) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements CF_6000, "F"
  ut_setrequirements CF_7001, "F"
endif

;; Extract bit 0 of the engine flags and downlink flags
frozenState = %and($SC_$CPU_CF_EngineFlags,1)
pb0QState = %and($SC_$CPU_CF_DownlinkChan[0].DownlinkFlags,1)
pb1QState = %and($SC_$CPU_CF_DownlinkChan[1].DownlinkFlags,1)

;;write "--frozen state = ",frozenState
;;write "--Chan 0 Pending Q state = ",pb0QState
;;write "--Chan 1 Pending Q state = ",pb1QState

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
   (p@$SC_$CPU_CF_PartnersFrozen = "False") AND ;;
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
  write "<!> Failed (7000) - Housekeeping telemetry NOT initialized properly at startup."
  ut_setrequirements CF_7000, "F"
  write "CMDPC           = ", $SC_$CPU_CF_CMDPC
  write "CMDEC           = ", $SC_$CPU_CF_CMDEC
  write "TotalInProgTrans = ", $SC_$CPU_CF_TotalInProgTrans
  write "TotalCompleteTrans = ", $SC_$CPU_CF_TotalCompleteTrans
  write "TotalFailedTrans = ", $SC_$CPU_CF_TotalFailedTrans
  write "PDUsRcvd = ", $SC_$CPU_CF_PDUsRcvd
  write "PDUsRejected = ", $SC_$CPU_CF_PDUsRejected
  write "ActiveQFileCnt = ", $SC_$CPU_CF_ActiveQFileCnt
  write "GoodUplinkCtr = ", $SC_$CPU_CF_GoodUplinkCtr
  write "BadUplinkCtr = ", $SC_$CPU_CF_BadUplinkCtr
  write "LastFileUplinked = '", $SC_$CPU_CF_LastFileUplinked, "'"
  write "PosAckNum = ", $SC_$CPU_CF_PosAckNum
  write "FileStoreRejNum = ", $SC_$CPU_CF_FileStoreRejNum
  write "FileChecksumNum = ", $SC_$CPU_CF_FileChecksumNum
  write "FileSizeNum = ", $SC_$CPU_CF_FileSizeNum
  write "NakLimitNum = ", $SC_$CPU_CF_NakLimitNum
  write "InactiveNum = ", $SC_$CPU_CF_InactiveNum
  write "CancelNum = ", $SC_$CPU_CF_CancelNum
  write "NumFrozen = ", $SC_$CPU_CF_NumFrozen
  write "PartnersFrozen = ", p@$SC_$CPU_CF_PartnersFrozen
  write "Chan 0 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].PendingQFileCnt
  write "Chan 1 PendingQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].PendingQFileCnt
  write "Chan 0 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[0].ActiveQFileCnt
  write "Chan 1 ActiveQFileCnt = ", $SC_$CPU_CF_DownlinkChan[1].ActiveQFileCnt
  write "Chan 0 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].GoodDownlinkCnt
  write "Chan 1 GoodDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].GoodDownlinkCnt
  write "Chan 0 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[0].BadDownlinkCnt
  write "Chan 1 BadDownlinkCnt = ", $SC_$CPU_CF_DownlinkChan[1].BadDownlinkCnt
  write "Chan 0 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[0].PDUsSent
  write "Chan 1 PDUsSent = ", $SC_$CPU_CF_DownlinkChan[1].PDUsSent
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.0: Initialization Error Tests "
write ";***********************************************************************"
write ";  Step 2.1: Remove the default Configuration Table file from $CPU."
write ";***********************************************************************"
s ftp_file("CF:0", "na", tableFileName, "$CPU", "R")
wait 5

write ";***********************************************************************"
write ";  Step 2.2: Perform a Power-On Reset "
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";***********************************************************************"
write ";  Step 2.3: Attempt to start the CF Application. This should fail since"
write ";  the Configuration Table load file does not exist. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_70011, "P"
else
  write "<!> Failed (7001.1) - CF Application started when the Configuration Table failed to load."
  ut_setrequirements CF_70011, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.4: Perform a Processor Reset "
write ";***********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";***********************************************************************"
write ";  Step 2.5: Attempt to start the CF Application. This should fail since"
write ";  the Configuration Table load file does not exist. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_70011, "P"
else
  write "<!> Failed (7001.1) - CF Application started when the default Configuration Table file did not exist."
  ut_setrequirements CF_70011, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.6: Create all invalid table load files and upload the one "
write ";  containing the invalid CFDP Class. "
write ";***********************************************************************"
s $sc_$cpu_cf_badcfgtbls

;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_badclass.tbl", tableFileName, "$CPU", "P")
wait 5

write ";***********************************************************************"
write ";  Step 2.7: Attempt to start the CF Application. This should fail since"
write ";  the Configuration Table load file contains an invalid class. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001;7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_7001, "P"
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing a bad class."
  ut_setrequirements CF_700111, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.8: Upload the Configuration Table load file containing the "
write ";  invalid Outgoing File Chunk Size. "
write ";***********************************************************************"
;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_badsize.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.9: Attempt to start the CF Application. This should fail since"
write ";  the Configuration Table load file contains an invalid chunk size. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing an invalid chunk size."
  ut_setrequirements CF_700111, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.10: Upload the Configuration Table load file containing the "
write ";  invalid Flight Entity ID. "
write ";***********************************************************************"
;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_badfltid.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.11: Attempt to start the CF Application. This should fail "
write ";  since the Configuration Table load file contains an invalid flight "
write ";  entity ID."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing an invalid Flight Entity ID."
  ut_setrequirements CF_700111, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.12: Upload the Configuration Table load file containing the "
write ";  invalid Peer Entity ID. "
write ";***********************************************************************"
;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_badpid1.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.13: Attempt to start the CF Application. This should fail "
write ";  since the Configuration Table load file contains an invalid peer "
write ";  entity ID."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing an invalid Peer Entity ID."
  ut_setrequirements CF_700111, "F"

  ;; Stop the CF application
  /$SC_$CPU_ES_DELETEAPP Application=CFAppName
  wait 5
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.14: Upload the Configuration Table load file containing the "
write ";  invalid Peer Entity ID format. "
write ";***********************************************************************"
;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_badpid2.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.15: Attempt to start the CF Application. This should fail "
write ";  since the Configuration Table load file contains an invalid peer "
write ";  entity ID format."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing an invalid Peer Entity ID."
  ut_setrequirements CF_700111, "F"

  ;; Stop the CF application
  /$SC_$CPU_ES_DELETEAPP Application=CFAppName
  wait 5
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.16: Upload the Configuration Table load file containing the "
write ";  invalid Incoming PDU Message ID. "
write ";***********************************************************************"
;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_badimid.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.17: Attempt to start the CF Application. This should fail "
write ";  since the Configuration Table load file contains an invalid incoming "
write ";  PDU Message ID."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing an invalid incoming PDU MID."
  ut_setrequirements CF_700111, "F"
endif

wait 5

write ";***********************************************************************"
write ";  Step 2.18: Upload the Configuration Table load file containing the "
write ";  invalid Downlink PDU Message ID. "
write ";***********************************************************************"
;; Upload the default configuration file to $CPU
s ftp_file("CF:0", "$cpu_cf_baddmid.tbl", tableFileName, "$CPU", "P")

wait 5

write ";***********************************************************************"
write ";  Step 2.19: Attempt to start the CF Application. This should fail "
write ";  since the Configuration Table load file contains an invalid downlink "
write ";  PDU Message ID."
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", {CFAppName}, CF_CFGTBL_LD_ERR_EID, "ERROR", 1

s load_start_app (CFAppName,"$CPU","CF_AppMain")

;; Wait for app startup Error Event message 
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (7001.1.1) - CF Application Failed to start as expected."
  ut_setrequirements CF_700111, "P"
else
  write "<!> Failed (7001.1.1) - CF Application started with a Configuration Table containing an invalid downlink PDU MID."
  ut_setrequirements CF_700111, "F"
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
write ";  End procedure $SC_$CPU_cf_init"
write ";*********************************************************************"
ENDPROC
