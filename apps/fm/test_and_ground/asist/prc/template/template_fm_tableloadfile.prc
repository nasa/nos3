PROC $sc_$cpu_fm_tableloadfile
;*******************************************************************************
;  Name:  fm_tableloadfile
;
;  Test Description
;	This procedure populates the CFS File Manager free space table and 
;	generates a table load image for that table. This table image can be
;	loaded via the CFE_TBL_LOAD command.
;
;  Change History
;
;	Date	   Name		Description
;	12/10/08   W. Moleski	Original Procedure.
;	02/01/10   W. Moleski	Updated for FM 2.1.0.0
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
;; States are 0=Disabled; 1=Enabled;
$SC_$CPU_FM_FreeSpaceTBL[0].State = 1
$SC_$CPU_FM_FreeSpaceTBL[0].Name = "/ram"

$SC_$CPU_FM_FreeSpaceTBL[1].State = 1
$SC_$CPU_FM_FreeSpaceTBL[1].Name = "/cf"

$SC_$CPU_FM_FreeSpaceTBL[2].State = 0
$SC_$CPU_FM_FreeSpaceTBL[2].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[3].State = 0
$SC_$CPU_FM_FreeSpaceTBL[3].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[4].State = 0
$SC_$CPU_FM_FreeSpaceTBL[4].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[5].State = 0
$SC_$CPU_FM_FreeSpaceTBL[5].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[6].State = 0
$SC_$CPU_FM_FreeSpaceTBL[6].Name = ""

$SC_$CPU_FM_FreeSpaceTBL[7].State = 0
$SC_$CPU_FM_FreeSpaceTBL[7].Name = ""

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",tblAppId,"FM FreeSpace Table Load 1", "$cpu_fmdevtbl_ld_1","FM.FreeSpace", "$SC_$CPU_FM_FreeSpaceTBL[0].State", "$SC_$CPU_FM_FreeSpaceTBL[7].Name")

ENDPROC
