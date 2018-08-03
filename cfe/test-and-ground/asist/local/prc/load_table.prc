proc load_table (tablename, cpu, mult)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
;;
;==============================================================================
;
; Purpose: The purpose of this procedure is to load and start a user 
;          application.
;
; History:
;
; 16NOV05          Initial development of this proc.          SMA
; 21OCT09          Added logging variable to reset it to the  WFM
;		   state before this proc was called. Also,
;		   added cfdp command statements
;
ON ERROR RESUME

local ipaddress
local perl_command, cfdp_command
local tbl_cmd_expr
local directory
local toolsDir = %env("TOOLS")
local cfdp_status
local tss = total_successful_senders
local supress = 0
local wait_time = 5

global up_link
global ftp_file_rc

global fsw_filename_found
global filename_found

if (%length(mult) = 0) then
  mult = 6
ENDIF

if (%select(packet_valid("my_entity_id"),1,0)) then
  cfdp_status = 1
else
  cfdp_status = 0
endif

; patch for global variable uplink/dnlink mode control
if (up_link = "0") then
  cfdp_status = 0
endif

directory = "RAM:0"
local ramDir = "/ram/"

if (%nargs < 2) then
  error "USAGE : LOAD_TABLE tablename, cpu"
  goto ERROR_RETURN
endif

; ==========================================================
; Convert table name to lowercase 
; ==========================================================
tablename = %lower(tablename)
write "Table Filename: ", tablename

; ===========================================================
; Set ip address based on cpu passed in call
; ===========================================================
; ===================================
; Translate from cpu'x' to 'x'
; ===================================
if (%length(cpu) = "0") then
   cpu = 1
elseif (%length(cpu) = "4") then
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
; Build and make call to table_ftp.pl perl script
; ============================================================
;;;perl_command = "perl /s/opr/accounts/" & account & "/prc/table_ftp.pl"
perl_command = "perl " & toolsDir & "/table_ftp.pl"
perl_command = perl_command & " " & ipaddress
perl_command = perl_command & " " & tablename
perl_command = perl_command & " " & directory
perl_command = perl_command & " " & cpu

; ============================================================
; Build CFDP call
; ============================================================
cfdp_command = """" & "put " & tablename
cfdp_command = cfdp_command & " 0.23 " & ramDir & tablename & """"
local path_n_file =  ramDir & tablename

if cfdp_status then
    if (supress = "0") then
       write "The CFDP command is: cfdp_dir ", cfdp_command
       write
    endif

;;    goto TEMP_BYPASS
    s fsw_does_file_exist(path_n_file, cpu)
    if fsw_filename_found then
      goto FILE_DOES_NOT_EXIST
    endif
TEMP_BYPASS:
    s file_attr_del(path_n_file, cpu)

FILE_DOES_NOT_EXIST:
   s does_file_exist(tablename)
   if filename_found then
     goto ERROR_RETURN
   endif
   write "About to send cfdp command <" & cfdp_command & ">"
   cfdp_dir {cfdp_command}
   s does_file_exist(tablename)
   if filename_found then
      goto ERROR_RETURN
   endif
   s file_attr_put (path_n_file)
else
   if (supress = "0") then
      write "The perl command is: ", perl_command
      write
   endif

   ftp_file_rc = %native (perl_command)
   write "Return code from ftp_file.pl: ", ftp_file_rc

   wait wait_time
   write
endif

; ===========================================================
; Set up base command prefix
; ===========================================================
;;if (SC = "") and (CPU = "1") then
;;  local base_cmd_expr_front = "/"
;;else
;;  local base_cmd_expr_front = "/"&(SC)&"CPU"&(cpu)&"_"
;;endif
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
tbl_cmd_expr = base_cmd_expr_front & "TBL_LOAD LFILENAME="""
;;;tbl_cmd_expr = tbl_cmd_expr &(directory)&"/"&(tablename)&""""
tbl_cmd_expr = tbl_cmd_expr & ramDir &(tablename)&""""

write "Sending Command: ", tbl_cmd_expr
%cmd(tbl_cmd_expr)
  
wait wait_time*mult

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
