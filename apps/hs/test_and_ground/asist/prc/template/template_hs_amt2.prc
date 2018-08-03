PROC $sc_$cpu_hs_amt2
;*******************************************************************************
;  Test Name:  hs_amt2
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate an Application Monitoring
;	Table that contains a cFE Core Application that triggers a cFE Processor
;	Reset when it fails to check-in. The CFE_TBL application is used with a
;	Cycle count set to 2 which will cause HS to think it is not running.
;	
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
;       6/22/09		W. Moleski	Initial release
;       12/09/10        W. Moleski      Added variable for table name and
;                                       dynamically created the endmnemonic
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
local apid = 0xf72
local HSAppName = "HS"
local amtTblName = HSAppName & "." & HS_AMT_TABLENAME

if ("$CPU" = "CPU2") then
  apid =  0xf80
elseif ("$CPU" = "CPU3") then
  apid = 0xf91
endif 

write ";*********************************************************************"
write ";  Define the Application Monitoring Table "
write ";*********************************************************************"
; Entry 1
$SC_$CPU_HS_AMT[1].AppName = "CFE_ES"
$SC_$CPU_HS_AMT[1].NullTerm = 0
$SC_$CPU_HS_AMT[1].CycleCnt = 10
$SC_$CPU_HS_AMT[1].ActionType = HS_AMT_ACT_NOACT

;; Entry 2
$SC_$CPU_HS_AMT[2].AppName = "CFE_EVS"
$SC_$CPU_HS_AMT[2].NullTerm = 0
$SC_$CPU_HS_AMT[2].CycleCnt = 10
$SC_$CPU_HS_AMT[2].ActionType = HS_AMT_ACT_NOACT

;; Entry 3
$SC_$CPU_HS_AMT[3].AppName = "CFE_TIME"
$SC_$CPU_HS_AMT[3].NullTerm = 0
$SC_$CPU_HS_AMT[3].CycleCnt = 10
$SC_$CPU_HS_AMT[3].ActionType = HS_AMT_ACT_NOACT

;; Entry 4
$SC_$CPU_HS_AMT[4].AppName = "CFE_TBL"
$SC_$CPU_HS_AMT[4].NullTerm = 0
$SC_$CPU_HS_AMT[4].CycleCnt = 2
$SC_$CPU_HS_AMT[4].ActionType = HS_AMT_ACT_PROC_RESET

;; Entry 5
$SC_$CPU_HS_AMT[5].AppName = "CFE_SB"
$SC_$CPU_HS_AMT[5].NullTerm = 0
$SC_$CPU_HS_AMT[5].CycleCnt = 10
$SC_$CPU_HS_AMT[5].ActionType = HS_AMT_ACT_NOACT

;; Make the rest of the entries empty
for index = 6 to HS_MAX_MONITORED_APPS do
  $SC_$CPU_HS_AMT[index].AppName = ""
  $SC_$CPU_HS_AMT[index].NullTerm = 0
  $SC_$CPU_HS_AMT[index].CycleCnt = 0
  $SC_$CPU_HS_AMT[index].ActionType = HS_AMT_ACT_NOACT
enddo

local endmnemonic = "$SC_$CPU_HS_AMT[" & HS_MAX_MONITORED_APPS & "].ActionType"

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",apid,"App Monitoring Table Load 2","hs_def_amt2",amtTblName,"$SC_$CPU_HS_AMT[1].AppName",endmnemonic)

;; Restore procedure logging
%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_amt2 "
write ";*********************************************************************"
ENDPROC
