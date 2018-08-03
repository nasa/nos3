PROC $sc_$cpu_cs_tdt1
;*******************************************************************************
;  Test Name:  cs_tdt1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the default Tables
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
;	02/24/15	Walt Moleski	Moved the DefTablesTbl entry lower in
;					the table and added the DefAppsTbl.
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
LOCAL defTblId, defPktId
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local tblDefTblName = CSAppName & "." & CS_DEF_TABLES_TABLE_NAME
local tblResTblName = CSAppName & "." & CS_RESULTS_TABLES_TABLE_NAME
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME
local appResTblName = CSAppName & "." & CS_RESULTS_APP_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAE"
defPktId = 4014

write ";*********************************************************************"
write ";  Define the Application Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED
$SC_$CPU_CS_TBL_DEF_TABLE[0].State = CS_STATE_ENABLED
$SC_$CPU_CS_TBL_DEF_TABLE[0].Name = appDefTblName
$SC_$CPU_CS_TBL_DEF_TABLE[1].State = CS_STATE_DISABLED
$SC_$CPU_CS_TBL_DEF_TABLE[1].Name = appResTblName
$SC_$CPU_CS_TBL_DEF_TABLE[2].State = CS_STATE_ENABLED
$SC_$CPU_CS_TBL_DEF_TABLE[2].Name = "TST_TBL.dflt_tbl_01"
$SC_$CPU_CS_TBL_DEF_TABLE[3].State = CS_STATE_DISABLED
$SC_$CPU_CS_TBL_DEF_TABLE[3].Name = "TST_TBL.tableA"
$SC_$CPU_CS_TBL_DEF_TABLE[4].State = CS_STATE_DISABLED
$SC_$CPU_CS_TBL_DEF_TABLE[4].Name = "TST_TBL.tableB"
$SC_$CPU_CS_TBL_DEF_TABLE[5].State = CS_STATE_ENABLED
$SC_$CPU_CS_TBL_DEF_TABLE[5].Name = tblDefTblName
$SC_$CPU_CS_TBL_DEF_TABLE[6].State = CS_STATE_DISABLED
$SC_$CPU_CS_TBL_DEF_TABLE[6].Name = tblResTblName

;; Clear out the remaining entries in the table
for i = 7 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 do
  $SC_$CPU_CS_TBL_DEF_TABLE[i].State = CS_STATE_EMPTY
  $SC_$CPU_CS_TBL_DEF_TABLE[i].Name = ""
enddo

local lastEntry = CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1
local endmnemonic = "$SC_$CPU_CS_TBL_DEF_TABLE[" & lastEntry & "].Name"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defTblId,"Table Definition Table Load 1","tbl_def_tbl_ld_1",tblDefTblName,"$SC_$CPU_CS_TBL_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_tdt1                              "
write ";*********************************************************************"
ENDPROC
