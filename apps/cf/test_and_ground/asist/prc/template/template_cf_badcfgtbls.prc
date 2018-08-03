PROC $sc_$cpu_cf_badcfgtbls
;*******************************************************************************
;  Test Name:  cf_badcfgtbls
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates seven (7) CFS CCSDS File Delivery Protocol (CF)
;	Configuration Table load image files. These files contain
;		1. An invalid Class
;		2. An invalid Outgoing File Chunk Size
;		3. An invalid Flight Entity ID (value too large)
;		4. An invalid Ground Entity ID (value too large)
;		5. An invalid Ground Entity ID (invalid format)
;		6. An invalid Incoming Message ID
;		7. An invalid Downlink Message ID
;	
;  Prerequisite Conditions
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	03/25/10	Walt Moleski	Inital implementation.
;       11/02/10        Walt Moleski    Updated according to new table structure
;       11/16/10        Walt Moleski    Added local variables for Application
;                                       and table names
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
write ";  Step 1.0: Define the default CF Configuration  Table."
write ";***********************************************************************"

;; Setup the appid based upon the CPU being used
local cfgAppid = 0x0F78

if ("$CPU" = "CPU2") then
  cfgAppid = 0x0F86
elseif ("$CPU" = "CPU3") then
  cfgAppid = 0x0F97
endif

;; Setup the Destination File Table
$SC_$CPU_CF_TBLID = "CF Bad Class Table"
$SC_$CPU_CF_TBLVersion = 2
$SC_$CPU_CF_TBLCyclesPerWakeup = 4 
$SC_$CPU_CF_TBLWakeupsPerQ = 2
$SC_$CPU_CF_TBLWakeupsPollDir = 4
$SC_$CPU_CF_TBLUpHistQDepth = 100
$SC_$CPU_CF_TBLAckTimeout = "10"
$SC_$CPU_CF_TBLAckLimit = "2"
$SC_$CPU_CF_TBLNakTimeout = "5"
$SC_$CPU_CF_TBLNakLimit = "3"
$SC_$CPU_CF_TBLInactTimeout = "20"
$SC_$CPU_CF_TBLOutChunkSize = "200"
$SC_$CPU_CF_TBLSaveIncomplete = "no"
$SC_$CPU_CF_TBLFlightEntityID = "0.24"

;; Uplink Channel 0
$SC_$CPU_CF_TBLUPCHAN[0].IncomingPduMID = CF_INCOMING_PDU_MID
$SC_$CPU_CF_TBLUPCHAN[0].Class2ResponseChan = 0

;; Playback Channel 0
$SC_$CPU_CF_TBLPBCHAN[0].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[0].DeqEnable = CF_ENABLED
$SC_$CPU_CF_TBLPBCHAN[0].DnlinkPDUMsgID = CF_SPACE_TO_GND_PDU_MID
$SC_$CPU_CF_TBLPBCHAN[0].PendingQDepth = 100
$SC_$CPU_CF_TBLPBCHAN[0].HistoryQDepth = 100
$SC_$CPU_CF_TBLPBCHAN[0].ChanName = "TOPBOutputChan0"
$SC_$CPU_CF_TBLPBCHAN[0].SemName = "CFTOSemId"

;; Polling Directory 0 (0-based array)
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].CF_Class = 3
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Preserve = CF_DELETE_FILE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].SrcPath = "/cf/hot/"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].DstPath = "/ram/cftesting/"
;;$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].TmpPath = "/ram/"

;; Polling Directory 1
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].CF_Class = 1
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].Preserve = CF_KEEP_FILE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].SrcPath = "/cf/cold/"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].DstPath = "/gnd/"
;;$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].TmpPath = "/ram/"

;; Zero out the remaining Polling Directories
if (CF_MAX_POLLING_DIRS_PER_CHAN > 2) then
  for i = 2 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].EntryInUse = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].EnableState = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].CF_Class = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].Priority = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].Preserve = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].PeerEntityID = ""
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].SrcPath = ""
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].DstPath = ""
  enddo
endif

;; Playback Channel 1
$SC_$CPU_CF_TBLPBCHAN[1].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[1].DeqEnable = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[1].DnlinkPDUMsgID = CF_SPACE_TO_GND_PDU_MID
$SC_$CPU_CF_TBLPBCHAN[1].PendingQDepth = 100
$SC_$CPU_CF_TBLPBCHAN[1].HistoryQDepth = 100
$SC_$CPU_CF_TBLPBCHAN[1].ChanName = "TOPBOutputChan1"
$SC_$CPU_CF_TBLPBCHAN[1].SemName = "CFTOSemId"

;; Polling Directory 1
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].CF_Class = 1
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Preserve = CF_DELETE_FILE
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].SrcPath = "/cf/hot/"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].DstPath = "/ram/cftesting/"
;;$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].TmpPath = "/ram/"

;; Polling Directory 2
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].EntryInUse = CF_ENTRY_UNUSED
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].CF_Class = 1
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].Preserve = CF_DELETE_FILE
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].SrcPath = "/cf/cold/"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].DstPath = "/ram/cftesting/"
;;$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].TmpPath = "/ram/"

;; Zero out the remaining Polling Directories
if (CF_MAX_POLLING_DIRS_PER_CHAN > 2) then
  for i = 2 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].EntryInUse = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].EnableState = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].CF_Class = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].Priority = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].Preserve = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].PeerEntityID = ""
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].SrcPath = ""
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].DstPath = ""
;;    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].TmpPath = ""
  enddo
endif

local maxChannels = CF_MAX_PLAYBACK_CHANNELS - 1
local maxDirs = CF_MAX_POLLING_DIRS_PER_CHAN - 1
local endMnemonic = "$SC_$CPU_CF_TBLPBCHAN[" & maxChannels & "].PollDir["
endMnemonic = endMnemonic & maxDirs & "].DstPath[" & OS_MAX_PATH_LEN & "]"

s create_tbl_file_from_cvt("$CPU", cfgAppid, "CF Bad Class Table", "$cpu_cf_badclass.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

;; Set the invalid class back to valid
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].CF_Class = 1
$SC_$CPU_CF_TBLID = "CF Bad Chunk Size Table"
$SC_$CPU_CF_TBLOutChunkSize = "5000"

s create_tbl_file_from_cvt("$CPU", cfgAppid, "CF Bad Chunk Size", "$cpu_cf_badsize.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

;; Set the invalid Chunk size back to valid
$SC_$CPU_CF_TBLOutChunkSize = "200"
$SC_$CPU_CF_TBLFlightEntityID = "256.24"

s create_tbl_file_from_cvt("$CPU", cfgAppid, "Bad Flt Entity ID", "$cpu_cf_badfltid.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

;; Set the invalid Flight Entity ID back to valid
$SC_$CPU_CF_TBLFlightEntityID = "0.24"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].PeerEntityID = "0.256"

s create_tbl_file_from_cvt("$CPU", cfgAppid, "Bad Peer Entity ID1", "$cpu_cf_badpid1.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

;; Set the invalid Peer Entity ID back to valid
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].PeerEntityID = "0..23"

s create_tbl_file_from_cvt("$CPU", cfgAppid, "Bad Peer Entity ID2", "$cpu_cf_badpid2.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

;; Set the invalid Peer Entity ID back to valid
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLUPCHAN[0].IncomingPduMID = CFE_SB_HIGHEST_VALID_MSGID + 1

s create_tbl_file_from_cvt("$CPU", cfgAppid, "Bad Incoming MID", "$cpu_cf_badimid.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

;; Set the Incoming MID back to valid
$SC_$CPU_CF_TBLUPCHAN[0].IncomingPduMID = CF_INCOMING_PDU_MID
$SC_$CPU_CF_TBLPBCHAN[1].DnlinkPDUMsgID = CFE_SB_HIGHEST_VALID_MSGID + 1

s create_tbl_file_from_cvt("$CPU", cfgAppid, "Bad Downlink MID", "$cpu_cf_baddmid.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cf_badcfgtbls "
write ";*********************************************************************"
ENDPROC
