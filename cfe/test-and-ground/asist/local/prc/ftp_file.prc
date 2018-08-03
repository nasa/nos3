proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the upload or download of 
;          a data or log file to/from the cFE spacecraft to/from the ASSIST
;          hardware.
;
; History:
;
; 18NOV05        Initial development of this proc.            SMA
; 23MAR06        Change proc name from proc_ftp to ftp_file.  SMA
; 04MAY06        Added support for perl script return code.   SMA
; 26SEP06        Changed perl script location to global/tools SMA
;
#include "cfe_utils.h"

ON ERROR RESUME

local supress = 1
local ipaddress
local perl_command
local xfermode="binary"
local cpu
local account = %env("ACCOUNT")
local toolsDir = %env("TOOLS")
local search_cmd
local dest_file_found
global ftp_file_rc

if (%nargs < 5) then
  error "USAGE : FTP_FILE remote_directory, filename, dest_filename, cpu, getorput"
  return
endif

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

perl_command = "perl " & toolsDir & "/ftp_file.pl"
perl_command = perl_command & " " & remote_directory
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & dest_filename
perl_command = perl_command & " " & xfermode
perl_command = perl_command & " " & ipaddress
perl_command = perl_command & " " & getorput

if (supress = "0") then
  write "The perl command is: ", perl_command
endif

ftp_file_rc = %native (perl_command)
write "Return code from ftp_file.pl: ", ftp_file_rc

ERROR_RETURN:
%liv (log_procedure) = logging
ENDPROC
