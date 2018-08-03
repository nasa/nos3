proc load_memory (filename, cpu)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
;;
;==============================================================================
;
; Purpose: The purpose of this procedure is to transfer the supplied file to
;	the supplied cpu and issue the MM_LoadFile command.
;
; History:
;
; 20MAR08  WFM     Initial development copied from load_table procedure.
;
;==============================================================================
ON ERROR RESUME

local ipaddress
local perl_command
local mm_cmd_expr
local directory
local toolsDir = %env("TOOLS")

directory = "RAM:0"
local ramDir = "/ram/"

if (%nargs < 2) then
  error "USAGE : LOAD_MEMORY filename, cpu"
  return
endif

; ==========================================================
; Convert table name to lowercase 
; ==========================================================
filename = %lower(filename)
write "Filename: ", filename

; ===================================
; Translate from cpu'x' to 'x'
; ===================================
if (%length(cpu) = "4") then
   cpu = %substring(cpu,4,4)
endif

; ===========================================================
; Set ip address based on cpu passed in call
; ===========================================================
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
; Build and make call to ftp_file.pl perl script
; ============================================================
perl_command = "perl " & toolsDir & "/table_ftp.pl"
perl_command = perl_command & " " & ipaddress
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & directory
perl_command = perl_command & " " & cpu

write "The perl command is ", perl_command
native perl_command

; ===========================================================
; Set up base command front & rear
; ===========================================================
if (SC = "") and (CPU = "1") then
  local base_cmd_expr_front = "/"
else
  local base_cmd_expr_front = "/"&(SC)&"CPU"&(cpu)&"_"
endif

; ============================================================
; Send appropriate STOL command based on cpu number passed
; ============================================================
mm_cmd_expr = base_cmd_expr_front & "MM_LOADFILE FILENAME="""
mm_cmd_expr = mm_cmd_expr & ramDir &(filename)&""""

write "Sending Command: ", mm_cmd_expr
%cmd(mm_cmd_expr)
 
%liv (log_procedure) = logging

ENDPROC
