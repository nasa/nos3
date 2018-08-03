proc load_start_app (application, cpu, entry_point)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to load and start a user 
;          application.
;
; History:
;
; 24MAY05          Initial development of this proc.          SMA
; 01JUN05          Remove "_APP" processing. Add CPU          SMA
;                  processing for apps directory
; 13SEP05          Add new parameter - entry_point - for B3   SMA
; 14NOV05          Added automatic fill when entry_point      SMA
;                  parameter is null
; 01FEB06          Modified for LRO                           SMA
; 09FEB06          Allow for null CPU parameter               SMA
; 26SEP07          Modified stack size for build 5.0          MOB
;                   must be min 8129 decimal   
; 28AUG08   WFM    Removed LoadAddress argument from ES_StartAPP
;
ON ERROR RESUME

global ftp_file_rc
local ipaddress
local perl_command
local app_cmd_expr
local directory
local app_filename
local toolsDir = %env("TOOLS")

directory = "RAM:0"
local ramDir = "/ram/" 

if (%nargs < 2) then
  error "USAGE : LOAD_START_APP application, cpu, entry_point"
  goto ERROR_RETURN
endif

if (%nargs = 2) then
  entry_point = application & "_TaskMain"
endif

; ==========================================================
; Set filename based on passed application name
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
  goto ERROR_RETURN
endif

; ============================================================
; Build and make call to fsw_ftp.pl perl script
; ============================================================
perl_command = "perl " & toolsDir & "/fsw_ftp.pl"
perl_command = perl_command & " " & ipaddress
perl_command = perl_command & " " & app_filename
perl_command = perl_command & " " & directory
perl_command = perl_command & " " & cpu

write "The perl command is ", perl_command
ftp_file_rc = %native (perl_command)
write "Return code from ftp_file.pl: ", ftp_file_rc

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
  base_cmd_expr_front = base_cmd_expr_front & (cpu) & "_"
else
  base_cmd_expr_front = base_cmd_expr_front & "_"
endif

; ============================================================
; Send appropriate STOL command based on cpu number passed
; ============================================================
app_cmd_expr = base_cmd_expr_front & "ES_STARTAPP APPLICATION="""
; app_cmd_expr = "/"&"ES_STARTAPP APPLICATION="""
app_cmd_expr = app_cmd_expr &(application)&""""& " APP_ENTRY_PT="""
app_cmd_expr = app_cmd_expr &(entry_point) &""""& " APP_FILE_NAME="""
app_cmd_expr = app_cmd_expr & ramDir &(app_filename)& """"&" STACKSIZE=X'2000' "
app_cmd_expr = app_cmd_expr & "PRIORITY=X'c8' RESTARTCPU " 

write "Sending Command: ", app_cmd_expr
%cmd(app_cmd_expr)

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
