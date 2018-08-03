PROC scx_cpu3_cf_tbl4
;*******************************************************************************
;  Test Name:  cf_tbl1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates a CFS CCSDS File Delivery Protocol (CF)
;	Configuration Table load image file needed to test DCR #14534. This file
;	contains 2 uplink channels.
;	
;  Prerequisite Conditions
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	05/31/11	Walt Moleski	Inital implemetation.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "osconfig.h"
#include "cf_platform_cfg.h"
#include "cf_events.h"
#include "cf_defs.h"
#include "cf_msgids.h"

;***********************************************************************"
; Define local variables
;***********************************************************************"
local CFAppName = "CF"
local cfgTblName = CFAppName & "." & CF_CONFIG_TABLE_NAME

write ";***********************************************************************"
write ";  Step 1.0: Define the CF Configuration Table."
write ";***********************************************************************"

;; Setup the appid based upon the CPU being used
local cfgAppid = 0x0F78

if ("CPU3" = "CPU2") then
  cfgAppid = 0x0F86
elseif ("CPU3" = "CPU3") then
  cfgAppid = 0x0F97
endif

;; Setup the Destination File Table
SCX_CPU3_CF_TBLID = "CF Table 2 Uplinks "
SCX_CPU3_CF_TBLVersion = 4
SCX_CPU3_CF_TBLCyclesPerWakeup = 4 
SCX_CPU3_CF_TBLWakeupsPerQ = 2
SCX_CPU3_CF_TBLWakeupsPollDir = 4
SCX_CPU3_CF_TBLUpHistQDepth = 100
SCX_CPU3_CF_TBLAckTimeout = "10"
SCX_CPU3_CF_TBLAckLimit = "2"
SCX_CPU3_CF_TBLNakTimeout = "5"
SCX_CPU3_CF_TBLNakLimit = "3"
SCX_CPU3_CF_TBLInactTimeout = "20"
SCX_CPU3_CF_TBLOutChunkSize = "200"
SCX_CPU3_CF_TBLSaveIncomplete = "no"
SCX_CPU3_CF_TBLFlightEntityID = "0.24"

;; Uplink Channel 0
SCX_CPU3_CF_TBLUPCHAN[0].IncomingPduMID = CF_INCOMING_PDU_MID
SCX_CPU3_CF_TBLUPCHAN[0].Class2ResponseChan = 0

;; Uplink Channel 1
SCX_CPU3_CF_TBLUPCHAN[1].IncomingPduMID = 0x1FFC
SCX_CPU3_CF_TBLUPCHAN[1].Class2ResponseChan = 0

;; Playback Channel 0
SCX_CPU3_CF_TBLPBCHAN[0].EntryInUse = CF_ENTRY_IN_USE
SCX_CPU3_CF_TBLPBCHAN[0].DeqEnable = CF_ENABLED
SCX_CPU3_CF_TBLPBCHAN[0].DnlinkPDUMsgID = CF_SPACE_TO_GND_PDU_MID
SCX_CPU3_CF_TBLPBCHAN[0].PendingQDepth = 100
SCX_CPU3_CF_TBLPBCHAN[0].HistoryQDepth = 100
SCX_CPU3_CF_TBLPBCHAN[0].ChanName = "TOPBOutputChan0"
SCX_CPU3_CF_TBLPBCHAN[0].SemName = "CFTOSemId0"

;; Polling Directory 1 - (0-based array)
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].EntryInUse = CF_ENTRY_IN_USE
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].EnableState = CF_DISABLED
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].CF_Class = 1
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].Priority = 0
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].Preserve = CF_DELETE_FILE
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].PeerEntityID = "0.23"
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].SrcPath = "/cf/hot/"
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].DstPath = "/ram/cftesting/"
;;SCX_CPU3_CF_TBLPBCHAN[0].PollDir[0].TmpPath = "/ram/"

;; Polling Directory 2
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].EntryInUse = CF_ENTRY_IN_USE
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].EnableState = CF_DISABLED
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].CF_Class = 1
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].Priority = 0
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].Preserve = CF_KEEP_FILE
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].PeerEntityID = "0.23"
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].SrcPath = "/cf/cold/"
SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].DstPath = "/gnd/"
;;SCX_CPU3_CF_TBLPBCHAN[0].PollDir[1].TmpPath = "/ram/"

;; Zero out the remaining Polling Directories if they exist
if (CF_MAX_POLLING_DIRS_PER_CHAN > 2) then
  for i = 2 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].EntryInUse = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].EnableState = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].CF_Class = 1
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].Priority = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].Preserve = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].PeerEntityID = ""
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].SrcPath = ""
    SCX_CPU3_CF_TBLPBCHAN[0].PollDir[i].DstPath = ""
  enddo
endif

;; Playback Channel 1
SCX_CPU3_CF_TBLPBCHAN[1].EntryInUse = CF_ENTRY_UNUSED
SCX_CPU3_CF_TBLPBCHAN[1].DeqEnable = CF_DISABLED
SCX_CPU3_CF_TBLPBCHAN[1].DnlinkPDUMsgID = CF_SPACE_TO_GND_PDU_MID
SCX_CPU3_CF_TBLPBCHAN[1].PendingQDepth = 100
SCX_CPU3_CF_TBLPBCHAN[1].HistoryQDepth = 100
SCX_CPU3_CF_TBLPBCHAN[1].ChanName = "TOPBOutputChan1"
SCX_CPU3_CF_TBLPBCHAN[1].SemName = "CFTOSemId1"

;; Polling Directory 1 (0-based array)
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].EntryInUse = CF_ENTRY_IN_USE
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].EnableState = CF_DISABLED
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].CF_Class = 2
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].Priority = 0
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].Preserve = CF_DELETE_FILE
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].PeerEntityID = "0.23"
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].SrcPath = "/cf/hot/"
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].DstPath = "/ram/cftesting/"
;;SCX_CPU3_CF_TBLPBCHAN[1].PollDir[0].TmpPath = "/ram/"

;; Polling Directory 2
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].EntryInUse = CF_ENTRY_UNUSED
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].EnableState = CF_DISABLED
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].CF_Class = 2
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].Priority = 0
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].Preserve = CF_DELETE_FILE
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].PeerEntityID = "0.23"
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].SrcPath = "/cf/cold/"
SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].DstPath = "/ram/cftesting/"
;;SCX_CPU3_CF_TBLPBCHAN[1].PollDir[1].TmpPath = "/ram/"

;; Zero out the remaining Polling Directories
if (CF_MAX_POLLING_DIRS_PER_CHAN > 2) then
  for i = 2 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].EntryInUse = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].EnableState = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].CF_Class = 2
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].Priority = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].Preserve = CF_ENTRY_UNUSED
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].PeerEntityID = ""
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].SrcPath = ""
    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].DstPath = ""
;;    SCX_CPU3_CF_TBLPBCHAN[1].PollDir[i].TmpPath = ""
  enddo
endif

local maxChannels = CF_MAX_PLAYBACK_CHANNELS - 1
local maxDirs = CF_MAX_POLLING_DIRS_PER_CHAN - 1
local endMnemonic = "SCX_CPU3_CF_TBLPBCHAN[" & maxChannels & "].PollDir["
endMnemonic = endMnemonic & maxDirs & "].DstPath[" & OS_MAX_PATH_LEN & "]"

s create_tbl_file_from_cvt("CPU3", cfgAppid, "CF 2 Uplink Channels Table", "cpu3_cf_2upcfg.tbl", cfgTblName, "SCX_CPU3_CF_TBLID[1]", endMnemonic)

%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  End procedure SCX_CPU3_cf_tbl4"
write ";*********************************************************************"
ENDPROC
