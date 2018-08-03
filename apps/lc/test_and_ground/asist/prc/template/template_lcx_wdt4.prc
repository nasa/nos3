PROC $sc_$cpu_lcx_wdt4
;*******************************************************************************
;  Test Name:  lcx_wdt4
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the WatchPoint Definition
;	Table (WDT) containing the maximpum number of WatchPoints that will all
;       trigger.
;
;	Note that the message ids used are borrowed from the other CFS
;	applications (MM, FM, MD, and SCH).    
;
;  WDT1:  Used by the Stress monitoring procedure
;
;  Requirements Tested
;       None
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.	
;       The LC commands and TLM items exist in the GSE database. 
;	A display page exists for the LC Housekeeping telemetry packet
;       LC Test application loaded and running;
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;      08/10/12         W. Moleski      Initial release for LCX
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			 Description
;       create_tbl_file_from_cvt Procedure that creates a load file from
;                                the specified arguments and cvt
;                                
;
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"

write ";*********************************************************************"
write ";  define local variables "
write ";*********************************************************************"

LOCAL entry
LOCAL apid
LOCAL MessageID

;; CPU1 is the default
apid = 0xfb7
;; Use CPU2 Message IDs
MessageID = 0x989

if ("$CPU" = "CPU2") then
  apid =  0xfd5
  ;; Use CPU3 Message IDs
  MessageID = 0xa89
elseif ("$CPU" = "CPU3") then
  apid = 0xff5
  ;; Use CPU1 Message IDs
  MessageID = 0x889
endif 

write ";*********************************************************************"
write ";  Define the Watch Point Definition Table "
write ";*********************************************************************"

; Make all the entries the same so that they all fail
for entry = 0 to LC_MAX_WATCHPOINTS-1 do
  $SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_BE
  $SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
  $SC_$CPU_LC_WDT[entry].MessageID = MessageID
  $SC_$CPU_LC_WDT[entry].WPOffset = 19
  $SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
  $SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00001345
  $SC_$CPU_LC_WDT[entry].CustFctArgument = 0
  $SC_$CPU_LC_WDT[entry].StaleAge = 0
enddo

;; Restore procedure logging
%liv (log_procedure) = logging
 
local wpIndex = LC_MAX_WATCHPOINTS - 1
local startMnemonic = "$SC_$CPU_LC_WDT[0]"
local endMnemonic = "$SC_$CPU_LC_WDT[" & wpIndex & "]"
local tableName = LC_APP_NAME & ".LC_WDT"

s create_tbl_file_from_cvt("$CPU",apid,"WDTTable4","lc_def_wdt4.tbl",tableName,startMnemonic,endMnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_lcx_wdt4                                     "
write ";*********************************************************************"
ENDPROC
