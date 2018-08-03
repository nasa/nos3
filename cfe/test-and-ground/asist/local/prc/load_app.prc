proc load_app (remote_directory, application, cpu)
;
local logging = %liv (log_procedure)
;%liv (log_procedure) = FALSE
#include "cfe_utils.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to load an application from the
;          ground to the specified remote_directory. 
;
;    NOTE: This procedure does not start the supplied application.
;
; History:
;
; 08JAN07          Initial development of this proc.          WFM
;    
;
ON ERROR RESUME

local ipaddress
local perl_command
local directory
local app_filename
local toolsDir = %env("TOOLS")

if (%nargs < 3) then
  error "USAGE : LOAD_APP remote_directory, application, cpu"
  return
endif

; ==========================================================
; Determine filename based on passed application name
; ==========================================================
app_filename = application
app_filename = %lower(app_filename)&".o"

write "Application Filename: ", app_filename

; ===========================================================
; Set ip address based on cpu passed in call
; ===========================================================
; ===================================
; Translate from cpu'x' to 'x'
; ===================================
if (%length(cpu) = "0") then
   cpu = 1
endif
if (%length(cpu) = "4") then
   cpu = %substring(cpu,4,4)
endif

if (cpu = "1") then
   ipaddress = CPU1_IP
elseif (cpu = "2") then
   ipaddress = CPU2_IP
elseif (cpu = "3") then
   ipaddress = CPU3_IP
else
  error "CPU"&cpu&" is not VALID !!!"
  return
endif

; ============================================================
; Build and make call to fsw_ftp.pl perl script
; ============================================================
perl_command = "perl " & toolsDir & "/fsw_ftp.pl"
perl_command = perl_command & " " & ipaddress
perl_command = perl_command & " " & app_filename
perl_command = perl_command & " " & remote_directory
perl_command = perl_command & " " & cpu

write "The perl command is ", perl_command
native perl_command

%liv (log_procedure) = logging

ENDPROC
