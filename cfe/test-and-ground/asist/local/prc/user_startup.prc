proc user_startup 
;@PROCEDURE WHICH DEFINES ASIST USER ENVIRONMENT FOR TEST USE.

GOTO PREAMB
; REV A  cFE  INITIAL     MOB     18AUG04
;  26APR05  Mike Tong    : TASK START cfe_cdh_sb_task 
;  25MAY05  Scott Applebee : Add call to cfe_fsw_utils. Setup checksum and cpu labels.
;  01JUN05  Scott Applebee : Add cFE and OS version labels. Add note about gse_file_to_cvt_options.
;  03JUN05  Scott Applebee : Move labels setup to new proc.
;  06JUN05  Scott Applebee : Add TO command sent flag.


PREAMB:
local logging = %liv (log_procedure)
%liv (log_procedure) = TRUE

#include "cfe_utils.h"

global gse_file_to_cvt_options
global to_cmd_sent

gse_file_to_cvt_options="-z" ; Sets to all zeros. -Z or x'fe' option writes all 'fe's to file
to_cmd_sent = "0"
;
;; ANY New Mission should replace GPM with the defined value in cfe_utils.h
if (MISSION = "CFE") then
  S cfe_data_center_interface   ;Defines open_tlm and open_cmd directives for UDP msging
elseif (MISSION = "GPM") then
  S gpm_data_center_interface   ;Defines open_tlm and open_cmd directives for UDP msging
endif
;

s cfe_fsw_utils

s new_file_dirs

s login_exit_dirs

; Defines the ats and rts compile directives
s cfs_ats_rts_utils

if (MISSION = "GPM") then
  s gds_dirs
  s setup_string_config

  s setup_cfdp_status
  s gpm_time

  wait until dpd_sammi_up
  page EASY_GPM
endif

%liv (log_procedure) = logging
;

;  The following statements start a task (PSTOL) that is always running 
;  (infinite loop). The task processes and sorts the SB routing info file.
; TASK START cfe_cdh_sb_task  PRIORITY 0 ID cfe_cdh_sb_task

; Set up Quick Buttons
QUICK1 = "CFE_CDH_ROUTING"
QUICK2 = "CFE_ACE_ROUTING"
QUICK3 = "CFE_PSE_ROUTING"
QUICK4 = "CFE_CI"
QUICK5 = "CFE_HS_TASKS_PRESENT"
QUICK6 = "CFE_CDH_SB"

if (MISSION = "GPM") then
  QUICK1 = "EASY_GPM"
  UPDATE_QUICK_BUTTONS
endif

endproc
