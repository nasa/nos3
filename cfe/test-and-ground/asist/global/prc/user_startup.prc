proc user_startup 
;@PROCEDURE WHICH DEFINES ASIST USER ENVIRONMENT FOR TEST USE.

GOTO PREAMB
; REV A  cFE  INITIAL     MOB     18AUG04
;                         Mike Tong 12OCT04 TASK START S2_SB_TASK.
;                         Jeff Condron 18OCT04 Took them out (moved to pseudo)

;

PREAMB:
%liv (log_procedure) = FALSE

;
S data_center          ;Defines open_tlm and open_cmd directives for UDP msging
;

s new_file_dirs

%liv (log_procedure) = TRUE
;

;  The following statement starts a task that is always running.
;  It processes and sorts the SB routing packet.
;;TASK START S2_SB_TASK  PRIORITY 0 ID S2_TASK
;;TASK START sort_ace_routing  PRIORITY 0 ID sort_ACE_TASK
;;TASK START sort_pse_routing  PRIORITY 0 ID sort_PSE_TASK

; Set up Quick Buttons
QUICK1 = "CFE_CDH_ROUTING"
QUICK2 = "CFE_ACE_ROUTING"
QUICK3 = "CFE_PSE_ROUTING"
QUICK4 = "CFE_CI"
QUICK5 = "CFE_HS_TASKS_PRESENT"
QUICK6 = "CFE_CDH_SB"

endproc
