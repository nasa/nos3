proc get_que_to_cvt (source_directory, quename, channel, filename, cpu, apid)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
#include "ut_statusdefs.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to dump the specified CFDP Queue
; information to a file on the cFE spacecraft and then to download the file to
; the ASSIST hardware for loading and display on a telemetry page.
;
; History:
;
; 20NOV09       Initial development of this proc.                       WFM
; 03MAY10       Added code to force the use of ftp by this proc since   WFM
;		using cfdp was causing this procedure to hang while
;		other cfdp playback requests were being processed
;
ON ERROR RESUME

local supress=0
local ipaddress
local perl_command, vutl_command, cfdp_command
local xfermode="binary"
local cpu
local dmp_cmd_expr
local cmd_expr
local apid
local toolsDir = %env("TOOLS")
local scName
local cfdp_status
local tsr = total_successful_receivers
local wait_time
global dn_link
global ftp_file_rc
global rad750

if (%select(packet_valid("my_entity_id"),1,0)) then
  cfdp_status = 1
  wait_time = 10
else
  cfdp_status = 0
  wait_time = 15
endif

; patch for global variable uplink/dnlink mode control
if (dn_link = "0") then
  cfdp_status = 0
  wait_time = 15
endif

;; Added this to force ftp - WFM
cfdp_status = 0

if (%nargs < 6) then
  error "USAGE : source_directory, quename, channel, filename, cpu, apid"
  goto ERROR_RETURN
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
   goto ERROR_RETURN
endif

;============================================================
; Append 'P' to apid
;============================================================
apid = "P" & apid

; ===========================================================
; Set up base command and telemetry prefix
; ===========================================================
local base_cmd_expr_front
if (SC <> "") then
  base_cmd_expr_front = "/" & SC
  if (%length(SC) = "4") then
    scName = %substring(SC,1,3)
  endif
else
  base_cmd_expr_front = "/"
endif

;; Add the CPU definition
base_cmd_expr_front = base_cmd_expr_front & CPU_CFG
local cpuName = CPU_CFG

if (numCPUs > 1) then
  base_cmd_expr_front = base_cmd_expr_front & (cpu) & "_"
  cpuName = cpuName & (cpu)
else
  base_cmd_expr_front = base_cmd_expr_front & "_"
endif

; ============================================================================
; Send appropriate STOL command based on the quename and channel supplied 
; ============================================================================
quename = %lower(quename)
channel = %upper(channel)
if (channel = "UP") then
  if (quename = "active") then
    cmd_expr = "CF_WRITEUPACTVINFO"
  elseif (quename = "history") then
    cmd_expr = "CF_WRITEUPHISTINFO"
  endif
else
  if (quename = "active") then
    cmd_expr = "CF_WRITE" & channel & "ACTVINFO"
  elseif (quename = "history") then
    cmd_expr = "CF_WRITE" & channel & "HISTINFO"
  elseif (quename = "pending") then
    cmd_expr = "CF_WRITE" & channel & "PENDINFO"
  endif
endif

dmp_cmd_expr = base_cmd_expr_front & cmd_expr & " FILENAME=""" 
dmp_cmd_expr = dmp_cmd_expr & ramDir 
dmp_cmd_expr = dmp_cmd_expr & filename & """"

if (supress = "0") then
  write "Sending Command: ", dmp_cmd_expr
endif
%cmd (dmp_cmd_expr)

wait wait_time

if (supress = "0") then
  write
  write "   The QUENAME is: ", quename
  write "      The APID is: ", apid
  write "       The CPU is: CPU",cpu
  write "The IP Address is: ", ipaddress
endif

perl_command = "perl " & toolsDir & "/ftp.pl"
perl_command = perl_command & " " & source_directory
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & xfermode
perl_command = perl_command & " " & ipaddress

; setup cfdp command
cfdp_command = base_cmd_expr_front & "CF_PLAYBACKFILE CHAN_0 PRIORITY=1 CLASS_1 Keep_File"
cfdp_command = cfdp_command & " SRCFILENAME="
cfdp_command = cfdp_command & """"
cfdp_command = cfdp_command & ramDir & filename & """"
cfdp_command = cfdp_command & " DESTFILENAME="
cfdp_command = cfdp_command & """" & filename & """"
local path_n_file =  ramDir & filename

if cfdp_status then
  if (supress = "0") then
     write "The CFDP command is: ", cfdp_command
     write
  endif
  %cmd (cfdp_command)
  ; wait until (total_successful_receivers = tsr + 1)
  s file_attr_get (path_n_file)
else
  if (supress = "0") then
    write "The perl command is: ", perl_command
  endif
  ftp_file_rc = %native (perl_command)
  write "Return code from ftp_file.pl: ", ftp_file_rc

  wait wait_time
  write
endif

FILE_TO_CVT %name(filename) %name(apid)

local appid_number
local the_command,where,the_date_command

where=%env("WORK") & "/image"

appid_number = telemetry_attr(apid,"APID")
file_list[appid_number].file_write_name = %lower(filename)
the_date_command = "cvt -ws file_list[" & appid_number
the_date_command = the_date_command  & "].file_write_time "
the_date_command = the_date_command & """`date +%y-%j-%T -r "
the_date_command = the_date_command & where  & "/"
the_date_command = the_date_command & %lower(filename) & "`"""
native the_date_command
if (supress = "0") then
  write "The unix command is ", the_date_command
endif

wait 5

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
