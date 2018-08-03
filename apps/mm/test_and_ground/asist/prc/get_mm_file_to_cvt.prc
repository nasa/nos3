proc get_mm_file_to_cvt (source_directory, filename, cpu, appid, mem_type, data_size, sym_name, offset)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform a memory dump to a 
; file on the cFE spacecraft and then to download the file to the ASIST 
; hardware for loading and display on a telemetry page.
;
; History:
;
; 18MAR08   WFM    Initial development copied from get_tbl_to_cvt.
; 10JAN11   WFM	   Changed the command to use mem_type argument as a string
;
;==============================================================================
ON ERROR RESUME

local supress=0
local ipaddress
local perl_command
local xfermode="binary"
local cpu
local dmp_cmd_expr
local toolsDir = %env("TOOLS")

if (%nargs < 8) then
  error "USAGE : source_directory, filename, cpu, appid, mem_type, data_size, sym_name, offset"
  return
endif

; ===================================
; Translate RAM disk
; ===================================
local ramDir

if (%length(source_directory) = 4) then
  source_directory = source_directory & "0"
endif
ramDir = {%substring(source_directory,1,3) & "_" & %substring(source_directory,5,5)}

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

;============================================================
; Append 'P' to appid
; This is not needed since we pass in the appid as "Pxxxx"
;============================================================
;;appid = "P" & appid

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
;;dmp_cmd_expr = base_cmd_expr_front & "MM_DUMP2File MemType=" & mem_type
dmp_cmd_expr = base_cmd_expr_front & "MM_DUMP2File " & mem_type
dmp_cmd_expr = dmp_cmd_expr & " DataSize=" & data_size
dmp_cmd_expr = dmp_cmd_expr & " SymName=""" & sym_name & """"
dmp_cmd_expr = dmp_cmd_expr & " Offset=" & offset
dmp_cmd_expr = dmp_cmd_expr & " FileName=""" & ramDir
dmp_cmd_expr = dmp_cmd_expr & filename & """"

if (supress = "0") then
  write "Sending Command: ", dmp_cmd_expr
endif
%cmd (dmp_cmd_expr)

if (supress = "0") then
  write
  write "   The Filename is: ", filename
  write "      The APID is: ", appid
  write "       The CPU is: CPU",cpu
  write "The IP Address is: ", ipaddress
endif

;;;perl_command = "perl /s/opr/accounts/" & account & "/prc/ftp.pl"
perl_command = "perl " & toolsDir & "/ftp.pl"
perl_command = perl_command & " " & source_directory
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & xfermode
perl_command = perl_command & " " & ipaddress

if (supress = "0") then
  write "The perl command is: ", perl_command
endif
native perl_command

FILE_TO_CVT %name(filename) %name(appid)

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
