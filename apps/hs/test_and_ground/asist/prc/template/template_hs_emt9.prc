PROC $sc_$cpu_hs_emt9
;*******************************************************************************
;  Test Name:  hs_emt9
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure generates a Critical Event Table image that contains 
;	multiple entries for the TST_HS_NOOP event with different actions for
;	each entry.
;
;  Requirements Tested
;       None
;
;  Prerequisite Conditions
;	None
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		Name		Description
;       01/14/11	W. Moleski	Initial release
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			 Description
;       create_tbl_file_from_cvt Procedure that creates a load file from
;                                the specified arguments and cvt
;
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "hs_platform_cfg.h"
#include "hs_tbldefs.h"

write ";*********************************************************************"
write ";  define local variables "
write ";*********************************************************************"
;; CPU1 is the default
local apid = 0xf73
local HSAppName = "HS"                  
local emtTblName = HSAppName & "." & HS_EMT_TABLENAME

if ("$CPU" = "CPU2") then
  apid =  0xf81
elseif ("$CPU" = "CPU3") then
  apid = 0xf92
endif 

write ";*********************************************************************"
write ";  Define the Application Monitoring Table "
write ";*********************************************************************"
; Entry 1
$SC_$CPU_HS_EMT[1].AppName = "CFE_ES"
$SC_$CPU_HS_EMT[1].NullTerm = 0
$SC_$CPU_HS_EMT[1].EventID = 10
$SC_$CPU_HS_EMT[1].ActionType = HS_EMT_ACT_NOACT

;; Entry 2
$SC_$CPU_HS_EMT[2].AppName = "CFE_EVS"
$SC_$CPU_HS_EMT[2].NullTerm = 0
$SC_$CPU_HS_EMT[2].EventID = 10
$SC_$CPU_HS_EMT[2].ActionType = HS_EMT_ACT_NOACT

;; Entry 3
$SC_$CPU_HS_EMT[3].AppName = "CFE_TIME"
$SC_$CPU_HS_EMT[3].NullTerm = 0
$SC_$CPU_HS_EMT[3].EventID = 10
$SC_$CPU_HS_EMT[3].ActionType = HS_EMT_ACT_NOACT

;; Entry 4
$SC_$CPU_HS_EMT[4].AppName = "CFE_TBL"
$SC_$CPU_HS_EMT[4].NullTerm = 0
$SC_$CPU_HS_EMT[4].EventID = 10
$SC_$CPU_HS_EMT[4].ActionType = HS_EMT_ACT_NOACT

;; Entry 5
$SC_$CPU_HS_EMT[5].AppName = "CFE_SB"
$SC_$CPU_HS_EMT[5].NullTerm = 0
$SC_$CPU_HS_EMT[5].EventID = 10
$SC_$CPU_HS_EMT[5].ActionType = HS_EMT_ACT_NOACT

;; Entry 6
$SC_$CPU_HS_EMT[6].AppName = "TST_HS"
$SC_$CPU_HS_EMT[6].NullTerm = 0
$SC_$CPU_HS_EMT[6].EventID = 2
$SC_$CPU_HS_EMT[6].ActionType = HS_EMT_ACT_LAST_NONMSG+1

;; Entry 7
$SC_$CPU_HS_EMT[7].AppName = "TST_HS"
$SC_$CPU_HS_EMT[7].NullTerm = 0
$SC_$CPU_HS_EMT[7].EventID = 2
$SC_$CPU_HS_EMT[7].ActionType = HS_EMT_ACT_APP_RESTART

for index = 8 to HS_MAX_MONITORED_EVENTS do
  $SC_$CPU_HS_EMT[index].AppName = ""
  $SC_$CPU_HS_EMT[index].NullTerm = 0
  $SC_$CPU_HS_EMT[index].EventID = 0
  $SC_$CPU_HS_EMT[index].ActionType = HS_EMT_ACT_NOACT
enddo

;; Restore procedure logging
%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_HS_EMT[" & HS_MAX_MONITORED_EVENTS & "].ActionType"

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",apid,"Event Monitoring Table Load 9","hs_def_emt9",emtTblName,"$SC_$CPU_HS_EMT[1].AppName",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_emt4 "
write ";*********************************************************************"
ENDPROC
