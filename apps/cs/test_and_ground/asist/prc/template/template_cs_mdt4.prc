PROC $sc_$cpu_cs_mdt4
;*******************************************************************************
;  Test Name:  cs_mdt4
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;       The purpose of this procedure is to generate a User-defined Memory
;       Definition Table for the Checksum Application that contains all empty
;       entries.
;
;  Requirements Tested:
;	None
;
;  Prerequisite Conditions
;	The TST_CS_MemTbl application must be executing for this procedure to
;	generate the appropriate EEPROM Definition Table
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	07/19/11	Walt Moleski	Initial release.
;       09/19/12        Walt Moleski    Added write of new HK items and added a
;                                       define of the OS_MEM_TABLE_SIZE that
;                                       was removed from osconfig.h in 3.5.0.0
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address. Removed the
;                                       OS_MEM_TABLE_SIZE - not used.
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
local memDefTblName = CSAppName & "." & CS_DEF_MEMORY_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAD"
defPktId = 4013

write ";*********************************************************************"
write ";  Define the Memory Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED

;; Clear out all the entries in the table
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 do
  $SC_$CPU_CS_MEM_DEF_TABLE[i].State = CS_STATE_EMPTY
  $SC_$CPU_CS_MEM_DEF_TABLE[i].StartAddr = 0
  $SC_$CPU_CS_MEM_DEF_TABLE[i].NumBytes = 0
enddo

local lastEntry = CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1
local endmnemonic = "$SC_$CPU_CS_MEM_DEF_TABLE[" & lastEntry & "].NumBytes"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defTblId,"Memory Definition Empty Table Load","usrmemdefemptytbl",memDefTblName,"$SC_$CPU_CS_MEM_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_cs_mdt4                              "
write ";*********************************************************************"
ENDPROC
