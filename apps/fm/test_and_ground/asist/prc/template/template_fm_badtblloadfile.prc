PROC $sc_$cpu_fm_badtblloadfile
;*******************************************************************************
;  Name:  fm_badtblloadfile
;
;  Test Description
;	This procedure populates the CFS File Manager free space table and 
;	generates a table load image for that table. This table image containd
;	all possible validation errors. This image can be loaded via the
;	CFE_TBL_LOAD command.
;
;  Change History
;
;	Date	   Name		Description
;	05/11/10   W. Moleski	Initial creation
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "fm_platform_cfg.h"
#include "fm_events.h"
#include "fm_defs.h"

%liv (log_procedure) = logging

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL tblAppId, tblPktId

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;;; Right now, the pktIDs are not used
;; CPU1 is the default
tblAppId = "0FBA"
tblPktId = 4026

if ("$CPU" = "CPU2") then
  tblAppId = "0FD8"
  tblPktId = 4056
elseif ("$CPU" = "CPU3") then
  tblAppId = "0FF8"
  tblPktId = 4088
endif

write ";*********************************************************************"
write ";  Create & upload the Free Space Table file."
write ";********************************************************************"
;; States are 0=Unused; 1=Enabled; 2=Disabled
$SC_$CPU_FM_FreeSpaceTBL[0].State =FM_TABLE_ENTRY_ENABLED 
$SC_$CPU_FM_FreeSpaceTBL[0].Name = "/ram"

$SC_$CPU_FM_FreeSpaceTBL[1].State = FM_TABLE_ENTRY_ENABLED
$SC_$CPU_FM_FreeSpaceTBL[1].Name = "/cf"

$SC_$CPU_FM_FreeSpaceTBL[2].State = FM_TABLE_ENTRY_DISABLED
$SC_$CPU_FM_FreeSpaceTBL[2].Name = "/cf/apps"

$SC_$CPU_FM_FreeSpaceTBL[3].State = FM_TABLE_ENTRY_DISABLED
$SC_$CPU_FM_FreeSpaceTBL[3].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[4].State = 3
$SC_$CPU_FM_FreeSpaceTBL[4].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[5].State = FM_TABLE_ENTRY_UNUSED
$SC_$CPU_FM_FreeSpaceTBL[5].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[6].State = FM_TABLE_ENTRY_UNUSED
$SC_$CPU_FM_FreeSpaceTBL[6].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[7].State = FM_TABLE_ENTRY_UNUSED
$SC_$CPU_FM_FreeSpaceTBL[7].Name = ""

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",tblAppId,"FM FreeSpace Table Invalid Load 1", "$cpu_fmbadtbl_ld_1","FM.FreeSpace", "$SC_$CPU_FM_FreeSpaceTBL[0].State", "$SC_$CPU_FM_FreeSpaceTBL[7].Name")

ENDPROC
