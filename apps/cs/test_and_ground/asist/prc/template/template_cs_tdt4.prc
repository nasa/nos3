PROC $sc_$cpu_cs_tdt4
;*******************************************************************************
;  Test Name:  cs_tdt4
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate a Tables Definition Table
;	for the Checksum Application that contains all empty entries.
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
LOCAL defTblId, defPktId
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local tblDefTblName = CSAppName & "." & CS_DEF_TABLES_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAE"
defPktId = 4014

write ";*********************************************************************"
write ";  Define the Application Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 do
  $SC_$CPU_CS_TBL_DEF_TABLE[i].State = CS_STATE_EMPTY
  $SC_$CPU_CS_TBL_DEF_TABLE[i].Name = ""
enddo

local lastEntry = CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1
local endmnemonic = "$SC_$CPU_CS_TBL_DEF_TABLE[" & lastEntry & "].Name"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defTblId,"Table Definition Empty Table Load","tbldefemptytable",tblDefTblName,"$SC_$CPU_CS_TBL_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_tdt4                              "
write ";*********************************************************************"
ENDPROC
