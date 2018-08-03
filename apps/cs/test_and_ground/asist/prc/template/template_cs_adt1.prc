PROC $sc_$cpu_cs_adt1
;*******************************************************************************
;  Test Name:  cs_adt1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the default Application
;	Definition Table for the Checksum Application.
;
;  Requirements Tested:
;	None
;
;  Prerequisite Conditions
;	None
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	07/18/11	Walt Moleski	Initial release.
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;      create_tbl_file_from_cvt Procedure that creates a load file from
;                               the specified arguments and cvt
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_tbldefs.h"

%liv (log_procedure) = logging

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL defAppId, defPktId
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defAppId = "0FAF"
defPktId = 4015

write ";*********************************************************************"
write ";  Define the Application Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED
$SC_$CPU_CS_APP_DEF_TABLE[0].State = CS_STATE_ENABLED
$SC_$CPU_CS_APP_DEF_TABLE[0].Name = CSAppName
$SC_$CPU_CS_APP_DEF_TABLE[1].State = CS_STATE_DISABLED
$SC_$CPU_CS_APP_DEF_TABLE[1].Name = "TST_CS"
$SC_$CPU_CS_APP_DEF_TABLE[2].State = CS_STATE_ENABLED
$SC_$CPU_CS_APP_DEF_TABLE[2].Name = "TST_TBL"

local maxEntry = CS_MAX_NUM_APP_TABLE_ENTRIES - 1

;; Clear the remainder of the table
for i = 3 to maxEntry do
  $SC_$CPU_CS_APP_DEF_TABLE[i].State = 0
  $SC_$CPU_CS_APP_DEF_TABLE[i].Name = ""
enddo

local endmnemonic = "$SC_$CPU_CS_APP_DEF_TABLE[" & maxEntry & "].Name"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defAppId,"App Definition Table Load 1","app_def_tbl_ld_1",appDefTblName,"$SC_$CPU_CS_APP_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_adt1                              "
write ";*********************************************************************"
ENDPROC
