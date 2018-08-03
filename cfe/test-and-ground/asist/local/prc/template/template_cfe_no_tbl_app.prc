proc $sc_$cpu_cfe_no_tbl_app
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Test Name:  $sc_$cpu_es_appctrl
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to startup the cFE and verify that
;       the table services (CFE_TBL) is not executing.
;
;  Requirements Tested  
;	None
;
;  Prerequisite Conditions
;       The cFE is configured with Table Services decoupled. This means that
;	table services (CFE_TBL) will not execute when the cFE starts up.
;                       
;  Assumptions and Constraints 
;                       
;  Change History       
;       Date            Name                    Description
;       08/25/14        W. Moleski      Initial development.
;                       
;  Arguments
;       None            
;                       
;  Procedures Called    
;       Name                                    Description
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"

%liv (log_procedure) = logging

local app_info_file_index
local found_app

local ramDir = "RAM:0"

write ";*********************************************************************"
write "; Step 1.0: Table Services Decoupled Test "
write ";*********************************************************************"
write "; Step 1.1: Display the App Info page."
write ";*********************************************************************"
page $sc_$cpu_es_app_info

write ";*********************************************************************"
write "; Step 1.2: Determine if the CFE_TBL application is running."
write ";*********************************************************************"
start get_file_to_cvt (ramDir, "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")

found_app = FALSE
;Loop thru the table looking for the CFE_TBL app
for app_info_file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "CFE_TBL") then
    found_app = TRUE
  endif
enddo

if (found_app <> TRUE) then
  write "<*> Passed - The Table Services (CFE_TBL) application is not running."
else
  write "<!> Failed - CFE_TBL is executing when not expected to be."
endif

wait 10

endproc
