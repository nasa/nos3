PROC $sc_$cpu_hs_xct3
;*******************************************************************************
;  Test Name:  hs_xct3
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate an Execution Counter Table
;	that contains the maximum number of entries.
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
;       06/24/09	W. Moleski	Initial release
;       01/13/11        W. Moleski      Added variables for app and table names
;                                       and dynamically created the endmnemonic
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
local apid = 0xf75
local HSAppName = "HS"
local xctTblName = HSAppName & "." & HS_XCT_TABLENAME

if ("$CPU" = "CPU2") then
  apid =  0xf83
elseif ("$CPU" = "CPU3") then
  apid = 0xf94
endif 

write ";*********************************************************************"
write ";  Define the Application Monitoring Table "
write ";*********************************************************************"
;; NOTE: If HS_MAX_EXEC_CNT_SLOTS is changed from the default of 32, the number
;;       of ResourceNames set and the last argument on the 
;;	 create_tbl_file_from_cvt proc will need to change

; Setup the ResourceNames for each Entry
$SC_$CPU_HS_XCT[1].ResourceName = "CFE_ES"
$SC_$CPU_HS_XCT[2].ResourceName = "CFE_EVS"
$SC_$CPU_HS_XCT[3].ResourceName = "CFE_TIME"
$SC_$CPU_HS_XCT[4].ResourceName = "CFE_TBL"
$SC_$CPU_HS_XCT[5].ResourceName = "CFE_SB"
$SC_$CPU_HS_XCT[6].ResourceName = "HS"
$SC_$CPU_HS_XCT[7].ResourceName = "TST_HS"
$SC_$CPU_HS_XCT[8].ResourceName = "TST_EVS"
$SC_$CPU_HS_XCT[9].ResourceName = "TST_TBL"
$SC_$CPU_HS_XCT[10].ResourceName = "TST_TBL2"
$SC_$CPU_HS_XCT[11].ResourceName = "TST_ES20"
$SC_$CPU_HS_XCT[12].ResourceName = "TST_ES2"
$SC_$CPU_HS_XCT[13].ResourceName = "TST_ES3"
$SC_$CPU_HS_XCT[14].ResourceName = "TST_ES4"
$SC_$CPU_HS_XCT[15].ResourceName = "TST_ES5"
$SC_$CPU_HS_XCT[16].ResourceName = "TST_ES6"
$SC_$CPU_HS_XCT[17].ResourceName = "TST_ES7"
$SC_$CPU_HS_XCT[18].ResourceName = "TST_ES8"
$SC_$CPU_HS_XCT[19].ResourceName = "TST_ES9"
$SC_$CPU_HS_XCT[20].ResourceName = "TST_ES10"
$SC_$CPU_HS_XCT[21].ResourceName = "TST_ES11"
$SC_$CPU_HS_XCT[22].ResourceName = "TST_ES12"
$SC_$CPU_HS_XCT[23].ResourceName = "TST_ES13"
$SC_$CPU_HS_XCT[24].ResourceName = "TST_ES14"
$SC_$CPU_HS_XCT[25].ResourceName = "TST_ES15"
$SC_$CPU_HS_XCT[26].ResourceName = "TST_ES16"
$SC_$CPU_HS_XCT[27].ResourceName = "TST_ES17"
$SC_$CPU_HS_XCT[28].ResourceName = "TIME_TONE_TASK"
$SC_$CPU_HS_XCT[29].ResourceName = "TIME_1HZ_TASK"
$SC_$CPU_HS_XCT[30].ResourceName = "CI_LAB_APP"
$SC_$CPU_HS_XCT[31].ResourceName = "TO_LAB_APP"
$SC_$CPU_HS_XCT[32].ResourceName = "SCH_LAB_APP"

for index = 1 to HS_MAX_EXEC_CNT_SLOTS do
  $SC_$CPU_HS_XCT[index].NullTerm = 0
  $SC_$CPU_HS_XCT[index].ResourceType = HS_XCT_TYPE_APP_MAIN
enddo

$SC_$CPU_HS_XCT[28].ResourceType = HS_XCT_TYPE_APP_CHILD
$SC_$CPU_HS_XCT[29].ResourceType = HS_XCT_TYPE_APP_CHILD

;; Restore procedure logging
%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_HS_XCT[" & HS_MAX_EXEC_CNT_SLOTS & "].ResourceType"

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",apid,"Execution Counter Table Load 3","hs_def_xct3",xctTblName,"$SC_$CPU_HS_XCT[1].ResourceName",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_xct3 "
write ";*********************************************************************"
ENDPROC
