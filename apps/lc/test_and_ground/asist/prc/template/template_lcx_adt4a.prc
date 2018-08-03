PROC $sc_$cpu_lcx_adt4a
;*******************************************************************************
;  Test Name:  lcx_adt4a
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate an ActionPoint Definition
;	Table (ADT) that contains a single AP that relies on a watchpoint that
;       evaluates to LC_WATCH_ERROR. 
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
;	None
;
;  Change History
;
;	Date		   Name		Description
;      09/27/12		W. Moleski	Initial release for LCX
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

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"

write ";*********************************************************************"
write ";  define local variables "
write ";*********************************************************************"

LOCAL entry
LOCAL i
LOCAL appid
local ADTTblName = LC_APP_NAME & ".LC_ADT"

;; Set up the default values to be CPU1
appid = 0xfb6

if ("$CPU" = "CPU2") then
   appid = 0xfd4
elseif ("$CPU" = "CPU3") then
   appid = 0xff4
endif 

write ";*********************************************************************"
write ";  Define the Action Point Definition Table. "
write ";*********************************************************************"

; Entry 1
$SC_$CPU_LC_ADT[0].DefaultState = LC_APSTATE_DISABLED
$SC_$CPU_LC_ADT[0].RTSId = 10
$SC_$CPU_LC_ADT[0].MaxPassiveEvents  = 1
$SC_$CPU_LC_ADT[0].MaxPassFailEvents = 1
$SC_$CPU_LC_ADT[0].MaxFailPassEvents = 1
$SC_$CPU_LC_ADT[0].MaxFailsBefRTS = 1
$SC_$CPU_LC_ADT[0].RPNEquation[1] = 0
$SC_$CPU_LC_ADT[0].RPNEquation[2] = LC_RPN_EQUAL

for i = 3 to LC_MAX_RPN_EQU_SIZE do
  $SC_$CPU_LC_ADT[0].RPNEquation[i] = 0
enddo

$SC_$CPU_LC_ADT[0].EventType = 2
$SC_$CPU_LC_ADT[0].EventId = LC_BASE_AP_EID + entry
$SC_$CPU_LC_ADT[0].EventText = "AP Fired RTS"

;; zero put the rest of the entries
for entry = 1 to LC_MAX_ACTIONPOINTS-1 do
  $SC_$CPU_LC_ADT[entry].DefaultState = LC_ACTION_NOT_USED
  $SC_$CPU_LC_ADT[entry].RTSId = 0
  $SC_$CPU_LC_ADT[entry].MaxPassiveEvents  = 0
  $SC_$CPU_LC_ADT[entry].MaxPassFailEvents = 0
  $SC_$CPU_LC_ADT[entry].MaxFailPassEvents = 0
  $SC_$CPU_LC_ADT[entry].MaxFailsBefRTS = 0

  for i = 1 to LC_MAX_RPN_EQU_SIZE do
    $SC_$CPU_LC_ADT[entry].RPNEquation[i] = 0
  enddo

  $SC_$CPU_LC_ADT[entry].EventType = 0
  $SC_$CPU_LC_ADT[entry].EventId = LC_BASE_AP_EID
  $SC_$CPU_LC_ADT[entry].EventText = " "
enddo
;; Restore procedure logging
%liv (log_procedure) = logging

local maxAPIndex = LC_MAX_ACTIONPOINTS - 1
local startMnemonic = "$SC_$CPU_LC_ADT[0]"
local endMnemonic = "$SC_$CPU_LC_ADT[" & maxAPIndex & "]"
 
s create_tbl_file_from_cvt("$CPU",appid,"ADTTable4a","lc_def_adt4a.tbl",ADTTblName,startMnemonic,endMnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_lcx_adt4a                                    "
write ";*********************************************************************"
ENDPROC
