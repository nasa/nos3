PROC $sc_$cpu_hs_amt12
;*******************************************************************************
;  Test Name:  hs_amt12
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate an Application Monitoring
;	Table that contains the maximum number of entries with different actions
;	in order to perform some stress tests.
;	NOTE: Some of these applications may not exist in the Mission setting
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
;       7/14/09		W. Moleski	Initial release
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
; Set each entry's Application Name
$SC_$CPU_HS_AMT[1].AppName = "CFE_ES"
$SC_$CPU_HS_AMT[2].AppName = "CFE_EVS"
$SC_$CPU_HS_AMT[3].AppName = "CFE_TIME"
$SC_$CPU_HS_AMT[4].AppName = "CFE_TBL"
$SC_$CPU_HS_AMT[5].AppName = "CFE_SB"
$SC_$CPU_HS_AMT[6].AppName = "HS"
$SC_$CPU_HS_AMT[7].AppName = "TST_HS"
$SC_$CPU_HS_AMT[8].AppName = "SC"
$SC_$CPU_HS_AMT[9].AppName = "TST_SC"
$SC_$CPU_HS_AMT[10].AppName = "MM"
$SC_$CPU_HS_AMT[11].AppName = "TST_MM"
$SC_$CPU_HS_AMT[12].AppName = "MD"
$SC_$CPU_HS_AMT[13].AppName = "TST_MD"
$SC_$CPU_HS_AMT[14].AppName = "CS"
$SC_$CPU_HS_AMT[15].AppName = "TST_CS"
$SC_$CPU_HS_AMT[16].AppName = "HK"
$SC_$CPU_HS_AMT[17].AppName = "TST_HK"
$SC_$CPU_HS_AMT[18].AppName = "FM"
$SC_$CPU_HS_AMT[19].AppName = "TST_FM"
$SC_$CPU_HS_AMT[20].AppName = "LC"
$SC_$CPU_HS_AMT[21].AppName = "TST_LC"
$SC_$CPU_HS_AMT[22].AppName = "SCH"
$SC_$CPU_HS_AMT[23].AppName = "TST_SCH"
$SC_$CPU_HS_AMT[24].AppName = "TST_ES"
$SC_$CPU_HS_AMT[25].AppName = "TST_EVS"
$SC_$CPU_HS_AMT[26].AppName = "TST_TIME"
$SC_$CPU_HS_AMT[27].AppName = "TST_TBL"
$SC_$CPU_HS_AMT[28].AppName = "TST_TBL2"
$SC_$CPU_HS_AMT[29].AppName = "TST_SB"
$SC_$CPU_HS_AMT[30].AppName = "TST_ES2"
$SC_$CPU_HS_AMT[31].AppName = "TST_ES3"
$SC_$CPU_HS_AMT[32].AppName = "TST_ES4"

;; Set the common fields in each entry
for index = 1 to HS_MAX_MONITORED_APPS do
  $SC_$CPU_HS_AMT[index].NullTerm = 0
  $SC_$CPU_HS_AMT[index].CycleCnt = 10
  $SC_$CPU_HS_AMT[index].ActionType = HS_AMT_ACT_EVENT
enddo

;; Set the TST_ES2 Action to be Application Restart
$SC_$CPU_HS_AMT[30].ActionType = HS_AMT_ACT_APP_RESTART

local endmnemonic = "$SC_$CPU_HS_AMT[" & HS_MAX_MONITORED_APPS & "].ActionType"

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",apid,"App Monitoring Table Load 12","hs_def_amt12",amtTblName,"$SC_$CPU_HS_AMT[1].AppName",endmnemonic)

;; Restore procedure logging
%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_amt12 "
write ";*********************************************************************"
ENDPROC
