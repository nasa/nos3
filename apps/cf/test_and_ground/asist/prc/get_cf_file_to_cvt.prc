proc get_cf_file_to_cvt (filename, dst_filename, cpu, appid, qtype, pbChan)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
#include "cfe_utils.h"
#include "ut_statusdefs.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to send the CF_WriteActiveTrans
; command to generate the specified file on the spacecraft and then download it
; to the ASIST hardware for loading and display on a telemetry page.
;
; History:
;
; 08/12/10   WFM    Initial version.
;
;==============================================================================
ON ERROR RESUME

local supress=0
local ipaddress
local perl_command
local cfdp_command
local cfdp_status
local xfermode="binary"
local cpu
local dmp_cmd_expr
local toolsDir = %env("TOOLS")

if (%nargs < 5) then
  error "USAGE : filename, cpu, appid, qtype, pbChan"
  return
endif

;; Verify qtype
qtype = %lower(qtype)

if (qtype <> "all") AND (qtype <> "incoming") AND (qtype <> "outgoing") then
  error "qtype must be All, Incoming, or Outgoing"
  return
endif

if (%select(packet_valid("my_entity_id"),1,0)) then
  cfdp_status = 1
else
  error "CF Application is not running. Cannot download file"
  return
endif

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
;============================================================
appid = "P" & appid

; ===========================================================
; Set up base command front & rear
; ===========================================================
if (SC = "") and (CPU = "1") then
  local base_cmd_expr_front = "/"
else
  local base_cmd_expr_front = "/"&(SC)&"CPU"&(cpu)&"_"
  local base_tlm_expr_front = (SC)&"CPU"&(cpu)&"_"
endif

; ============================================================
; Send CF command to write the file
; ============================================================
dmp_cmd_expr = base_cmd_expr_front & "CF_WriteActiveTrans " & qtype
dmp_cmd_expr = dmp_cmd_expr & " FileName=""" & filename & """"

if (supress = "0") then
  write "Sending Command: ", dmp_cmd_expr
endif
%cmd (dmp_cmd_expr)

if (supress = "0") then
  write
  write "   The Filename is: ", filename
  write "      The APID is: ", appid
  write "       The CPU is: CPU",cpu
;;  write "The IP Address is: ", ipaddress
endif

;;perl_command = "perl " & toolsDir & "/ftp.pl"
;;perl_command = perl_command & " " & source_directory
;;perl_command = perl_command & " " & dst_filename
;;perl_command = perl_command & " " & dst_filename
;;perl_command = perl_command & " " & xfermode
;;perl_command = perl_command & " " & ipaddress
;;
;;if (supress = "0") then
;;  write "The perl command is: ", perl_command
;;endif
;;native perl_command

local tlmSpec = base_tlm_expr_front & "CF_DownlinkChan[" & pbChan
tlmSpec = tlmSpec & "].GoodDownlinkCnt"

local goodDnlinkCtr = {tlmSpec} + 1

; ============================================================
; Send CF_playback command to get the file to the ground
; ============================================================
cfdp_command = base_cmd_expr_front & "CF_PlaybackFile Class_1 "
if (pbChan = 0) then
  cfdp_command = cfdp_command & "Chan_0 Priority=5 Keep_file"
else
  cfdp_command = cfdp_command & "Chan_1 Priority=5 Keep_file"
endif
cfdp_command = cfdp_command & " PeerEntityID=""0.23"""
cfdp_command = cfdp_command & " SrcFileName=""" & filename & """"
cfdp_command = cfdp_command & " DestFileName=""" & dst_filename & """"

%cmd (cfdp_command)

;; Wait until the file has been successfully downloaded
ut_tlmwait {tlmSpec}, {goodDnlinkCtr}, 60
if (UT_TW_Status = UT_Success) then
  write "File was rcv'd"
else
  write "Playback not successful"
endif

FILE_TO_CVT %name(dst_filename) %name(appid)

local appid_number
local the_command,where,the_date_command

where=%env("WORK") & "/image"

local appid_number = telemetry_attr(appid,"APID")
file_list[appid_number].file_write_name = %lower(dst_filename)
the_date_command = "cvt -ws file_list[" & appid_number
the_date_command = the_date_command  & "].file_write_time "
the_date_command = the_date_command & """`date +%y-%j-%T -r "
the_date_command = the_date_command & where  & "/"
the_date_command = the_date_command & %lower(dst_filename) & "`"""
native the_date_command


ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
