PROC $sc_$cpu_evs_dflt_chk
; Sub-procedure to set up requirements
; upon initial power on default values verification
; Called by evs reset test
; Written on May-30-2006 by Eva I. Stattel
  
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"       
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h" 

%liv (log_procedure) = logging

;; NOTE: CFE_EVS_PORT_DEFAULT define is not recognized by STOL 
;;       as a valid hex value resulting in a syntax error
;;  if ($sc_$cpu_EVS_OUTPUTPORT = CFE_EVS_PORT_DEFAULT) then
  if ($sc_$cpu_EVS_OUTPUTPORT = 1) then
    ut_setrequirements cEVS3200, "P"
  else
    ut_setrequirements cEVS3200, "F"
    write "<!> Failed (3200) - Default value for evt port enabled"
  endif
  write " Output Ports Enabled ", %bin($sc_$cpu_EVS_OUTPUTPORT, 4) 
;
  if (p@$sc_$cpu_EVS_LOGFULL = "FALSE") then 
    ut_setrequirements cEVS3202, "P"
  endif
  write " Local event log full flag = ", p@$sc_$cpu_EVS_LOGFULL 
;
  if (p@$sc_$cpu_EVS_MSGFMTMODE = "LONG") then
    ut_setrequirements cEVS3201, "P"
  else
    ut_setrequirements cEVS3201, "F"
    write "<!> Failed (3201) - Default Message Format mode is not set to LONG"
  endif
  write " Event Msg Format = ", p@$sc_$cpu_EVS_MSGFMTMODE
;
  if ($sc_$cpu_EVS_LOGMODE = CFE_EVS_DEFAULT_LOG_MODE) then
    ut_setrequirements cEVS3203, "P"
  else
    ut_setrequirements cEVS3203, "F"
    write "<!> Failed (3203) - Default value for logging mode is not correct"
  endif
  write " Logging Mode = ", $sc_$cpu_EVS_LOGMODE 
;
ENDPROC
