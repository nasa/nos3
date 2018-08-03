proc send_that_to_command
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
ON ERROR RESUME
;
; ======================================================================
;
; Purpose: The purpose of this procedure is to send 'that TO command'
;          automatically when a cpu is selected.
;
; History:
;
; 03JUN05                Initial development of this proc.      SMA
; 06JUN05                Add flag to remember that to command   SMA
;                        has been sent.
; 21OCT09                Updated this proc to use definitions   WFM
;			 and variables in the cfe_utils.h file
;			 to generically build and send the cmd
;
#include "cfe_utils.h"

local hostname
local that_to_command_expr
local what_machine 
global to_cmd_sent

hostname = %liv(LOCALHOST) ; get hostname from ASIST STOL local internal variable

; strip off 'gpm-' from rainman hostname
if (%locate(hostname,"-") <> 0) then
  hostname = %substring(hostname,%locate(hostname,"-")+1,%length(hostname))
  hostname = %upper(hostname)
endif
;;write ".. hostname = ", hostname

; translate machine name to uppercase and convert to 'cpu' nomenclature
what_machine = %upper(gsescstatval14)
;;write ".. machine before if = ", what_machine

if (what_machine = "CDH") then
  what_machine = "CPU1"
elseif (what_machine = "PSE") then
  what_machine = "CPU2"
elseif (what_machine = "ACE") then
  what_machine = "CPU3"
endif
;;write ".. machine after if = ", what_machine

to_cmd_sent = %substring(what_machine,4,4)
; to_cmd_sent = 0 ; send T0 command always

; ===========================================================
; Set up base command prefix
; ===========================================================
local base_cmd_expr_front
if (SC <> "") then
  base_cmd_expr_front = "/" & SC
else
  base_cmd_expr_front = "/"
endif

;; Add the CPU definition
base_cmd_expr_front = base_cmd_expr_front & CPU_CFG

if (numCPUs > 1) then
  base_cmd_expr_front = base_cmd_expr_front & to_cmd_sent & "_"
else
  base_cmd_expr_front = base_cmd_expr_front & "_"
endif

  
; build command
that_to_command_expr = base_cmd_expr_front & "TO_OUTPUT_ENA "
;;that_to_command_expr = "/"&"SCX_"&(what_machine)&"_TO_OUTPUT_ENA "
that_to_command_expr = that_to_command_expr&(hostname)

write "Sending: ",that_to_command_expr
%cmd(that_to_command_expr)

%liv (log_procedure) = logging

endproc
