PROC $sc_$cpu_hs_mat1
;*******************************************************************************
;  Test Name:  hs_mat1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the default Message Actions
;	Table for the Health and Safety Application
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
local apid = 0xf74
local HSAppName = "HS"
local matTblName = HSAppName & "." & HS_MAT_TABLENAME

if ("$CPU" = "CPU2") then
  apid =  0xf82
elseif ("$CPU" = "CPU3") then
  apid = 0xf93
endif 

write ";*********************************************************************"
write ";  Define the Application Monitoring Table "
write ";*********************************************************************"
; Entry 1 - HS_NOOP Command
$SC_$CPU_HS_MAT[1].EnableState = HS_MAT_STATE_ENABLED
$SC_$CPU_HS_MAT[1].Cooldown= 10
$SC_$CPU_HS_MAT[1].MessageData[1] = 0x18
$SC_$CPU_HS_MAT[1].MessageData[8] = 0x88
if ("$CPU" = "CPU2") then
  $SC_$CPU_HS_MAT[1].MessageData[1] = 0x19
  $SC_$CPU_HS_MAT[1].MessageData[8] = 0x89
elseif ("$CPU" = "CPU3") then
  $SC_$CPU_HS_MAT[1].MessageData[1] = 0x1A
  $SC_$CPU_HS_MAT[1].MessageData[8] = 0x8A
endif
$SC_$CPU_HS_MAT[1].MessageData[2] = 0xAE
$SC_$CPU_HS_MAT[1].MessageData[3] = 0xC0
$SC_$CPU_HS_MAT[1].MessageData[4] = 0
$SC_$CPU_HS_MAT[1].MessageData[5] = 0
$SC_$CPU_HS_MAT[1].MessageData[6] = 1
$SC_$CPU_HS_MAT[1].MessageData[7] = 0

; Entry 2 - TST_HS NOOP Command
$SC_$CPU_HS_MAT[2].EnableState = HS_MAT_STATE_ENABLED
$SC_$CPU_HS_MAT[2].Cooldown= 10
$SC_$CPU_HS_MAT[2].MessageData[1] = 0x19
$SC_$CPU_HS_MAT[2].MessageData[8] = 0x1C
if ("$CPU" = "CPU2") then
  $SC_$CPU_HS_MAT[2].MessageData[1] = 0x1A
  $SC_$CPU_HS_MAT[2].MessageData[8] = 0x1F
elseif ("$CPU" = "CPU3") then
  $SC_$CPU_HS_MAT[2].MessageData[1] = 0x1B
  $SC_$CPU_HS_MAT[2].MessageData[8] = 0x1E
endif
$SC_$CPU_HS_MAT[2].MessageData[2] = 0x3B
$SC_$CPU_HS_MAT[2].MessageData[3] = 0xC0
$SC_$CPU_HS_MAT[2].MessageData[4] = 0
$SC_$CPU_HS_MAT[2].MessageData[5] = 0
$SC_$CPU_HS_MAT[2].MessageData[6] = 1
$SC_$CPU_HS_MAT[2].MessageData[7] = 0

for index = 3 to HS_MAX_MSG_ACT_TYPES do
  $SC_$CPU_HS_MAT[index].EnableState = HS_MAT_STATE_DISABLED
  $SC_$CPU_HS_MAT[index].Cooldown= 10
  $SC_$CPU_HS_MAT[index].MessageData = 0
enddo

;; Restore procedure logging
%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_HS_MAT[" & HS_MAX_MSG_ACT_TYPES & "].MessageData"

;; Create the Table Load file
s create_tbl_file_from_cvt ("$CPU",apid,"Message Actions Table Load 1","hs_def_mat1",matTblName,"$SC_$CPU_HS_MAT[1].EnableState",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hs_mat1 "
write ";*********************************************************************"
ENDPROC
