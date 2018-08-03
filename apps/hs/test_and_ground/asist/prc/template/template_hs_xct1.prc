PROC $sc_$cpu_hs_xct1
;*******************************************************************************
;  Test Name:  hs_xct1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the default Execution
;	Counter Table for the Health and Safety Application
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
;       06/19/09	W. Moleski	Initial release
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
; Entry 1
$SC_$CPU_HS_XCT[1].ResourceName = "CFE_ES"
$SC_$CPU_HS_XCT[1].NullTerm = 0
$SC_$CPU_HS_XCT[1].ResourceType = HS_XCT_TYPE_APP_MAIN

;; Entry 2
$SC_$CPU_HS_XCT[2].ResourceName = "CFE_EVS"
$SC_$CPU_HS_XCT[2].NullTerm = 0
$SC_$CPU_HS_XCT[2].ResourceType = HS_XCT_TYPE_APP_MAIN

;; Entry 3
$SC_$CPU_HS_XCT[3].ResourceName = "CFE_TIME"
$SC_$CPU_HS_XCT[3].NullTerm = 0
$SC_$CPU_HS_XCT[3].ResourceType = HS_XCT_TYPE_APP_MAIN

;; Entry 4
$SC_$CPU_HS_XCT[4].ResourceName = "CFE_TBL"
$SC_$CPU_HS_XCT[4].NullTerm = 0
$SC_$CPU_HS_XCT[4].ResourceType = HS_XCT_TYPE_APP_MAIN

;; Entry 5
$SC_$CPU_HS_XCT[5].ResourceName = "CFE_SB"
$SC_$CPU_HS_XCT[5].NullTerm = 0
$SC_$CPU_HS_XCT[5].ResourceType = HS_XCT_TYPE_APP_MAIN

for index = 6 to HS_MAX_EXEC_CNT_SLOTS do
  $SC_$CPU_HS_XCT[index].ResourceName = ""
  $SC_$CPU_HS_XCT[index].NullTerm = 0
  $SC_$CPU_HS_XCT[index].ResourceType = HS_XCT_TYPE_NOTYPE
enddo

;; Restore procedure logging
%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_HS_XCT[" & HS_MAX_EXEC_CNT_SLOTS & "].ResourceType"

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",apid,"Execution Counter Table Load 1","hs_def_xct1",xctTblName,"$SC_$CPU_HS_XCT[1].ResourceName",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_xct1 "
write ";*********************************************************************"
ENDPROC
