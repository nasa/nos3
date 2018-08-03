PROC $sc_$cpu_cf_tbl2
;*******************************************************************************
;  Test Name:  cf_tbl2
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates the CFS CCSDS File Delivery Protocol (CF)
;	Configuration Table load image file to be used by the Playback test
;	procedure. This table has a Class 1 and a Class 2 enabled directory to
;	allow file playback under both modes.
;	
;  Prerequisite Conditions
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	04/06/10	Walt Moleski	Inital implemetation.
;	11/02/10	Walt Moleski	Updated according to new table structure
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
write ";  Step 1.0: Define the CF Configuration Table."
write ";***********************************************************************"

;; Setup the appid based upon the CPU being used
local cfgAppid = 0x0F78

if ("$CPU" = "CPU2") then
  cfgAppid = 0x0F86
elseif ("$CPU" = "CPU3") then
  cfgAppid = 0x0F97
endif

;; Setup the Destination File Table
$SC_$CPU_CF_TBLID = "CF Playback Table"
$SC_$CPU_CF_TBLVersion = 2
$SC_$CPU_CF_TBLCyclesPerWakeup = 4 
$SC_$CPU_CF_TBLWakeupsPerQ = 2
$SC_$CPU_CF_TBLWakeupsPollDir = 4
$SC_$CPU_CF_TBLUpHistQDepth = 100
$SC_$CPU_CF_TBLAckTimeout = "30"
$SC_$CPU_CF_TBLAckLimit = "2"
$SC_$CPU_CF_TBLNakTimeout = "30"
$SC_$CPU_CF_TBLNakLimit = "3"
$SC_$CPU_CF_TBLInactTimeout = "40"
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
$SC_$CPU_CF_TBLPBCHAN[0].PendingQDepth = 10
$SC_$CPU_CF_TBLPBCHAN[0].HistoryQDepth = 100
$SC_$CPU_CF_TBLPBCHAN[0].ChanName = "TOPBOutputChan0"
$SC_$CPU_CF_TBLPBCHAN[0].SemName = "CFTOSemId0"

;; Polling Directory 1 (0-based array)
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].CF_Class = 1
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].Preserve = CF_DELETE_FILE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].SrcPath = "/ram/class1/"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].DstPath = "/ram/"
;;$SC_$CPU_CF_TBLPBCHAN[0].PollDir[0].TmpPath = "/ram/tmp/"

;; Polling Directory 2
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].CF_Class = 2
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].Preserve = CF_KEEP_FILE
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].SrcPath = "/ram/class2/"
$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].DstPath = "/ram/"
;;$SC_$CPU_CF_TBLPBCHAN[0].PollDir[1].TmpPath = "/ram/tmp/"

;; Zero out the remaining Polling Directories if they exist
if (CF_MAX_POLLING_DIRS_PER_CHAN > 2) then
  for i = 2 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].EntryInUse = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].EnableState = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].CF_Class = 1
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].Priority = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].Preserve = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].PeerEntityID = ""
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].SrcPath = ""
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].DstPath = ""
  enddo
endif

;; Playback Channel 1
$SC_$CPU_CF_TBLPBCHAN[1].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[1].DeqEnable = CF_ENABLED
$SC_$CPU_CF_TBLPBCHAN[1].DnlinkPDUMsgID = CF_SPACE_TO_GND_PDU_MID
$SC_$CPU_CF_TBLPBCHAN[1].PendingQDepth = 5
$SC_$CPU_CF_TBLPBCHAN[1].HistoryQDepth = 100
$SC_$CPU_CF_TBLPBCHAN[1].ChanName = "TOPBOutputChan1"
$SC_$CPU_CF_TBLPBCHAN[1].SemName = "CFTOSemId1"

;; Polling Directory 1
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].EntryInUse = CF_ENTRY_IN_USE
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].CF_Class = 2
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].Preserve = CF_DELETE_FILE
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].SrcPath = "/ram/class2/"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].DstPath = "/ram/"
;;$SC_$CPU_CF_TBLPBCHAN[1].PollDir[0].TmpPath = "/ram/tmp/"

;; Polling Directory 2
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].EntryInUse = CF_ENTRY_UNUSED
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].EnableState = CF_DISABLED
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].CF_Class = 1
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].Priority = 0
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].Preserve = CF_DELETE_FILE
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].PeerEntityID = "0.23"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].SrcPath = "/ram/class1/"
$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].DstPath = "/ram/"
;;$SC_$CPU_CF_TBLPBCHAN[1].PollDir[1].TmpPath = "/ram/tmp/"

;; Zero out the remaining Polling Directories
if (CF_MAX_POLLING_DIRS_PER_CHAN > 2) then
  for i = 2 to CF_MAX_POLLING_DIRS_PER_CHAN-1 do
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].EntryInUse = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].EnableState = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].CF_Class = 2
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].Priority = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].Preserve = CF_ENTRY_UNUSED
    $SC_$CPU_CF_TBLPBCHAN[0].PollDir[i].PeerEntityID = ""
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].SrcPath = ""
    $SC_$CPU_CF_TBLPBCHAN[1].PollDir[i].DstPath = ""
  enddo
endif

local maxChannels = CF_MAX_PLAYBACK_CHANNELS - 1
local maxDirs = CF_MAX_POLLING_DIRS_PER_CHAN - 1
local endMnemonic = "$SC_$CPU_CF_TBLPBCHAN[" & maxChannels & "].PollDir["
endMnemonic = endMnemonic & maxDirs & "].DstPath[" & OS_MAX_PATH_LEN & "]"

s create_tbl_file_from_cvt("$CPU", cfgAppid, "CF Playback Configuration Table 2", "$cpu_cf_cfg2.tbl", cfgTblName, "$SC_$CPU_CF_TBLID[1]", endMnemonic)

%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cf_tbl2"
write ";*********************************************************************"
ENDPROC
