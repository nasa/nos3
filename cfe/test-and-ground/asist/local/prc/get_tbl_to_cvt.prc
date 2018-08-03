proc get_tbl_to_cvt (source_directory, tblname, tblstate, filename, cpu, apid)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
#include "ut_statusdefs.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the dump of a table to a 
; file on the cFE spacecraft and then to download the file to the ASSIST 
; hardware for loading and display on a telemetry page.
;
; History:
;
; 03FEB06       Initial development of this proc.                       SMA
; 08FEB06       Fixed error with tblname if logic.                      WFM
; 08FEB06       Append 'P' to passed apid                               SMA
; 09FEB06       Add support for null CPU parameter                      SMA
; 20NOV06       Modified to support cFE 4.0                             WFM
; 08JAN08       Removed the _BETA specification from the                WFM
;		Get_File_to_CVT directives since they are
;		not BETA routines in the new ASIST version. 
; 29OCT08	Added code to wait until the TBL_DUMP event message is  WFM
;		captured before ftp'ing the file to the ground.
;
ON ERROR RESUME

local supress=0
local ipaddress
local perl_command, vutl_command, cfdp_command
local xfermode="binary"
local cpu
local tbl_dmp_cmd_expr
local tblstate
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

if (%nargs < 6) then
  error "USAGE : source_directory, tblname, tblstate, filename, cpu, apid"
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

; ===========================================================
; Set tblstate based on parameter passed in call
; ===========================================================
; ============================================
; Translate from letter and validate parameter
; ============================================
if (%length(tblstate) = "1") then
  if (tblstate = "A") or (tblstate="a") then 
     tblstate="ACTIVE"
  elseif (tblstate = "I") or (tblstate="i") then 
     tblstate="INACTIVE"
  else
     error tblstate & " is not VALID !!!"
     goto ERROR_RETURN
  endif
elseif (tblstate <> "ACTIVE") AND (tblstate <> "INACTIVE") AND  ;;
       (tblstate <> "active") AND (tblstate <> "inactive") then 
  error tblstate & " is not VALID !!!"
  goto ERROR_RETURN
endif

;============================================================
; Append 'P' to apid
;============================================================
apid = "P" & apid

; ===========================================================
; Set up base command and telemetry prefix
; ===========================================================
;;if (SC = "") and (CPU = "1") then
;;  local base_cmd_expr_front = "/"
;;else
;;  local base_cmd_expr_front = "/"&(SC)&"CPU"&(cpu)&"_"
;;endif
local base_cmd_expr_front
local base_tlm_expr_front
if (SC <> "") then
  base_cmd_expr_front = "/" & SC
  base_tlm_expr_front = SC
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

;; Add the CPU spec to the telemetry db prefix
base_tlm_expr_front = base_tlm_expr_front & cpuName

; ============================================================
; Send appropriate STOL command based on cpu number passed
; ============================================================
tbl_dmp_cmd_expr = base_cmd_expr_front &"TBL_DUMP " & tblstate & " DTABLENAME="""
tbl_dmp_cmd_expr = tbl_dmp_cmd_expr & tblname &  """"
tbl_dmp_cmd_expr = tbl_dmp_cmd_expr & " DFILENAME=""" 
;;;tbl_dmp_cmd_expr = tbl_dmp_cmd_expr & source_directory & "/" 
tbl_dmp_cmd_expr = tbl_dmp_cmd_expr & ramDir 
tbl_dmp_cmd_expr = tbl_dmp_cmd_expr & filename & """"

;;write "SCName = ",scName,"; CPUName = ",cpuName 
;;
;;local event1_exp = base_tlm_expr_front & "_" & "find_event[7].num_found_messages"
;;local event2_exp = base_tlm_expr_front & "_" & "find_event[8].num_found_messages"
;;
;;ut_setupevents {scName}, {cpuName}, "CFE_TBL", 14, "INFO", 7
;;ut_setupevents {scName}, {cpuName}, "CFE_TBL", 13, "INFO", 8
;;
;;write "BEFORE: Event 1 expr = ", event1_exp, " = ", {event1_exp}
;;write "BEFORE: Event 2 expr = ", event2_exp, " = ", {event2_exp}
;;
if (supress = "0") then
  write "Sending Command: ", tbl_dmp_cmd_expr
endif
%cmd (tbl_dmp_cmd_expr)

;;ut_tlmwait {event1_exp}, 1
;;if (UT_TW_Status <> UT_Success) then
;;  if ({event2_exp} <> 1) then
;;    write ";; Error dumping file. Expected event not found!!!"
;;    goto ERROR_RETURN
;;  else
;;    write ";; Found event 2"
;;  endif
;;else
;;  write ";; Found event 1"
;;endif
;;wait 5
;;write "AFTER: Event 1 expr = ", event1_exp, " = ", {event1_exp}
;;write "AFTER: Event 2 expr = ", event2_exp, " = ", {event2_exp}
wait wait_time

if (supress = "0") then
  write
  write "   The TBLNAME is: ", tblname
  write "      The APID is: ", apid
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

; setup cfdp command
cfdp_command = base_cmd_expr_front & "CF_PLAYBACKFILE VIRTUALCHANNEL=1 FILEPRIORITY=1"
IF (rad750 = 1) THEN
  cfdp_command = cfdp_command & " CFCLASS=2 ENTITYID=[0,26] "
ELSE
  cfdp_command = cfdp_command & " CFCLASS=2 ENTITYID=[0,24] "
ENDIF
cfdp_command = cfdp_command & "SOURCEFILENAME="
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

; vutl_command = "vutl_reinit" & ", " & appid
; write vutl_command
; native vutl_command 

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
