proc get_file_to_cvt (source_directory, filename, dest_filename, cpu, filetype)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_utils.h"
#include "cfe_platform_cfg.h"
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the download of a data
;          or log file from the cFE spacecraft to the ASSIST hardware.
;
; History:
;
; 28APR05          Initial development of this proc.          SMA
; 05MAY05          Enhanced version ... using upgraded perl   SMA
;                  script                                     
; 17MAY05          Added %liv (log_procedure) = TRUE          MOB
; 18MAY05          Added support for ftp file transfer mode   SMA
; 19MAY05          Added support for multiple cpus, pages and SMA
;                  ip addressses.
; 31MAY05          Added support for CPU3                     SMA 
; 01JUN05          Modified cpu parameter to support cpu      SMA
;                  number only or string 'cpux' where x is
;                  the cpu number
; 13JUN05          Add supress option to cancel printing to   SMA
;                  log. 
; 28JUN05          Add support to capture file name and time. SMA
; 29JUN05          Remove support to capture file name and    SMA
;                  time. Added to file_to_cvt_beta directive. SMA
; 25AUG05          Add support for cFE Build 3 new SB files   SMA
;                  and new ES file. 
; 29AUG05          Changes to new ES file code.               SMA 
; 14NOV05          Prepend ES filenames with "cfe_"           SMA
; 15NOV05          Add support for Table Registry             SMA 
; 17NOV05          Mod to support standard write file         SMA
;                  mnemonics                        
; 09FEB06          Add support for null CPU parameter         SMA
; 27FEB06          Corrected IP address for CPU1 (someone     MOB
;                  changed to .1.8 instead of 1.4)
; 18MAY06          Modify to allow user to name file written  SMA
;                  to FSW RAM: drive.
; 13JUN06          Added ES Logic Analyzer file to filetypes  MOB
; 16NOV06          Fixed the SB	file processing		      WFM
; 22NOV06          Added logic for the ES ER Log              WFM
; 15MAY07          Added support for the ES CDS Registry      WFM
; 13JUN07          Modified code so that the default filename WFM
;                  actually gets the file to the ground. This
;		   problem is due to the fact that the default
;		   filenames contain the full path specification
;		   which the ftp perl script doesn't like.
; 24SEP07          Added support for the ES TASK Info Log     WFM
; 08JAN08          Removed the _BETA from the Get_File_to_CVT WFM
; 		   directive call since these are not BETA 
;		   routines in the new ASIST version.
;  FUTURE          Added call to utility to zero out CVT for  SMA
;                  designated APID.
;    
;
ON ERROR RESUME

local supress
local appid, appidkey
local ipaddress
local perl_command, vutl_command
local xfermode
local cpu
local filetype
local account = %env("ACCOUNT")
local toolsDir = %env("TOOLS")
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

local cfe_files = 10
local filetypes [0..cfe_files]=["SB_ROUTE","SB_PIPE","SB_MSGMAP","EVS_APP","EVS_LOG","ES_INFO","TBL_REG","ES_LA","ES_ERLOG","ES_CDSREG","ES_TASKINFO"]
local filenames [0..cfe_files]=["cfe_sb_route.dat", "cfe_sb_pipe.dat", "cfe_sb_msgmap.dat", "cfe_evs_app.dat", "cfe_evs.log", "cfe_es_app_info.log", "cfe_tbl_reg.log","cfe_es_perf.dat","cfe_erlog.log","cfe_cds_reg.log","cfe_es_task_info.log"]

local sb_route_cmd_expr, sb_pipe_cmd_expr, sb_map_cmd_expr, evs_app_cmd_expr, evs_log_cmd_expr, es_task_log_cmd_expr, es_la_cmd_expr, es_erlog_cmd_expr
local tbl_dmp_reg_cmd_expr, cds_dump_cmd_expr, es_app_log_cmd_expr
local filenameLen
local cfdp_cmd_expr

supress = 0
appid=0

xfermode=""

; Determine if proc was called with minimum # of parameters
if (%nargs < 4) then
  error "USAGE : GET_FILE_TO_CVT source_directory, filename, dest_filename, cpu, <filetype>"
  goto ERROR_RETURN
endif

; ==========================================================
; 1) Determine file type if not provided in proc call w/
;    default cFE RAM: file name.
; 2) Trap proc call w/ user defined RAM: file name and 
;    no file type provided.
; 3) Validate file type passed if provided in proc call.
; ==========================================================
if (%nargs = 4) then 
local filename_found = 0
  FOR i = 0 to cfe_files DO
    if (filename = filenames[i]) and (filename_found = 0) then
      filetype = filetypes[i]
      filename_found = 1
    endif
  ENDDO
  if filename_found = 0 then
    error "FILETYPE must be specified !!!" 
    goto ERROR_RETURN
  endif
endif

if (%nargs = 5) then
local filetype_found = 0
  FOR i = 0 to cfe_files DO
    if (filetype = filetypes[i]) and (filetype_found = 0) then
      filetype_found = 1
    endif
  ENDDO
  if filetype_found = 0 then
    error "Incorrect <filetype> specified: " &filetype 
    goto ERROR_RETURN
  endif
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
   appidkey = "0"
elseif (cpu = "2") then
  ipaddress = CPU2_IP
  appidkey = "2"
elseif (cpu = "3") then
  ipaddress = CPU3_IP
  appidkey = "4"  
else
  error "CPU"&cpu&" is not VALID !!!"
  goto ERROR_RETURN
endif

; ===================================
; Translate RAM disk
; ===================================
local ramDir
ramDir = {%substring(source_directory,1,3) & "_" & %substring(source_directory,5,5)}

; ===========================================================
; Set up base command prefix
; ===========================================================
;;if (SC = "") and (cpu = "1") then
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
sb_route_cmd_expr = base_cmd_expr_front & "SB_WRITEROUTING2FILE ROUTINFOFILENAME="
sb_pipe_cmd_expr = base_cmd_expr_front & "SB_WRITEPIPE2FILE PIPEINFOFILENAME="
sb_map_cmd_expr = base_cmd_expr_front & "SB_WRITEMAP2FILE MAPINFOFILENAME="
evs_app_cmd_expr = base_cmd_expr_front & "EVS_WRITEAPPDATA2FILE APPDATAFILENAME="
evs_log_cmd_expr = base_cmd_expr_front & "EVS_WRITELOG2FILE LOGFILENAME="
es_app_log_cmd_expr = base_cmd_expr_front & "ES_WRITEAPPINFO2FILE APPFILENAME="
tbl_dmp_reg_cmd_expr = base_cmd_expr_front & "TBL_WRITEREG2FILE DUMPFILENAME=" 
es_la_cmd_expr = base_cmd_expr_front & "ES_STOPLADATA DATAFILENAME="
es_erlog_cmd_expr = base_cmd_expr_front & "ES_WRITEERLOG2FILE ERLOGFILENAME="
cds_dump_cmd_expr = base_cmd_expr_front & "ES_WRITECDS2FILE CDSREGFILENAME="
es_task_log_cmd_expr = base_cmd_expr_front & "ES_WRITETASKINFO2FILE TASKFILENAME="

; setup cfdp cmd expression
cfdp_cmd_expr = base_cmd_expr_front & "CF_PLAYBACKFILE VIRTUALCHANNEL=1 FILEPRIORITY=1"
IF (rad750 = 1) THEN
  cfdp_cmd_expr = cfdp_cmd_expr & " CFCLASS=2 ENTITYID=[0,26] "
ELSE
  cfdp_cmd_expr = cfdp_cmd_expr & " CFCLASS=2 ENTITYID=[0,24] "
ENDIF
 
if (filetype = "SB_ROUTE") then
   ; Setup the command expression
   sb_route_cmd_expr = sb_route_cmd_expr & """"

   if (filename = "") then
     filename = CFE_SB_DEFAULT_ROUTING_FILENAME
     sb_route_cmd_expr = sb_route_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     sb_route_cmd_expr = sb_route_cmd_expr & ramDir & filename
   endif
   sb_route_cmd_expr = sb_route_cmd_expr & """"

   if (supress = "0") then
     write "Sending Command: ", sb_route_cmd_expr
   endif
   %cmd (sb_route_cmd_expr)
   appid = "P0F" & appidkey & "2"
   xfermode="binary"
elseif (filetype = "SB_PIPE") then
   ; Setup the command expression
   sb_pipe_cmd_expr = sb_pipe_cmd_expr & """"

   if (filename = "") then
     filename = CFE_SB_DEFAULT_PIPE_FILENAME
     sb_pipe_cmd_expr = sb_pipe_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     sb_pipe_cmd_expr = sb_pipe_cmd_expr & ramDir & filename
   endif
   sb_pipe_cmd_expr = sb_pipe_cmd_expr & """"

   if (supress = "0") then
     write "Sending Command: ", sb_pipe_cmd_expr
   endif
   %cmd (sb_pipe_cmd_expr)
   appid = "P0F" & appidkey & "3"
   xfermode="binary"
elseif (filetype = "SB_MSGMAP") then
   ; Setup the command expression
   sb_map_cmd_expr = sb_map_cmd_expr & """"

   if (filename = "") then
     filename = CFE_SB_DEFAULT_MAP_FILENAME
     sb_map_cmd_expr = sb_map_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     sb_map_cmd_expr = sb_map_cmd_expr & ramDir & filename
   endif
   sb_map_cmd_expr = sb_map_cmd_expr & """"

   if (supress = "0") then
     write "Sending Command: ", sb_map_cmd_expr
   endif
   %cmd (sb_map_cmd_expr)
   appid = "P0F" & appidkey & "4"
   xfermode="binary"
elseif (filetype = "EVS_APP") then
   ; Setup the command expression
   evs_app_cmd_expr = evs_app_cmd_expr & """"

   if (filename = "") then
     filename = CFE_EVS_DEFAULT_APP_DATA_FILE
     evs_app_cmd_expr = evs_app_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     evs_app_cmd_expr = evs_app_cmd_expr & ramDir & filename
   endif
   evs_app_cmd_expr = evs_app_cmd_expr & """"

   if (supress = "0") then
     write "Sending Command: ", evs_app_cmd_expr
   endif

   %cmd (evs_app_cmd_expr)
   appid = "P0F" & appidkey & "1"
   xfermode="binary"
elseif (filetype = "EVS_LOG") then
   ; Setup the command expression
   evs_log_cmd_expr = evs_log_cmd_expr & """"

   if (filename = "") then
     filename = CFE_EVS_DEFAULT_LOG_FILE
     evs_log_cmd_expr = evs_log_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     evs_log_cmd_expr = evs_log_cmd_expr & ramDir & filename
   endif
   evs_log_cmd_expr = evs_log_cmd_expr & """"

   if (supress = "0") then   
     write "Sending Command: ", evs_log_cmd_expr
   endif

   %cmd (evs_log_cmd_expr) 
   appid = "P0F" & appidkey & "0"  
   xfermode="binary"
elseif (filetype = "ES_INFO") then
   ; Setup the command expression
   es_app_log_cmd_expr = es_app_log_cmd_expr & """"

   if (filename = "") then
     filename = CFE_ES_DEFAULT_TASK_LOG_FILE
     es_app_log_cmd_expr = es_app_log_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     es_app_log_cmd_expr = es_app_log_cmd_expr & ramDir & filename
   endif
   es_app_log_cmd_expr = es_app_log_cmd_expr & """"

   if (supress = "0") then   
     write "Sending Command: ", es_app_log_cmd_expr
   endif
   %cmd (es_app_log_cmd_expr) 
   appid = "P0F" & appidkey & "5"  
   xfermode="binary"
elseif (filetype = "TBL_REG") then
   ; Setup the command expression
   tbl_dmp_reg_cmd_expr = tbl_dmp_reg_cmd_expr & """"

   if (filename = "") then
     filename = CFE_TBL_DEFAULT_REG_DUMP_FILE
     tbl_dmp_reg_cmd_expr = tbl_dmp_reg_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     tbl_dmp_reg_cmd_expr = tbl_dmp_reg_cmd_expr & ramDir & filename
   endif
   tbl_dmp_reg_cmd_expr = tbl_dmp_reg_cmd_expr & """"

   if (supress = "0") then
     write "Sending Command: ", tbl_dmp_reg_cmd_expr
   endif
   %cmd (tbl_dmp_reg_cmd_expr) 
   appid = "P0F" & appidkey & "6"  
   xfermode="binary"
elseif (filetype = "ES_LA") then
   ; Setup the command expression
   es_la_cmd_expr = es_la_cmd_expr & """"

   if (filename = "") then
     filename = CFE_ES_DEFAULT_PERF_DUMP_FILENAME
     es_la_cmd_expr = es_la_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     es_la_cmd_expr = es_la_cmd_expr & ramDir & filename
   endif
   es_la_cmd_expr = es_la_cmd_expr & """"

   if (supress = "0") then
     write "Sending Command: ", es_la_cmd_expr
   endif
   %cmd (es_la_cmd_expr) 
   appid = "P0F" & appidkey & "7"  
   xfermode="binary"
elseif (filetype = "ES_ERLOG") then
   ; Setup the command expression
   es_erlog_cmd_expr = es_erlog_cmd_expr & """"

   if (filename = "") then
     filename = CFE_ES_DEFAULT_ER_LOG_FILE
     es_erlog_cmd_expr = es_erlog_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     es_erlog_cmd_expr = es_erlog_cmd_expr & ramDir & filename
   endif
   es_erlog_cmd_expr = es_erlog_cmd_expr & """"

   if (supress = "0") then   
     write "Sending Command: ", es_erlog_cmd_expr
   endif
   %cmd (es_erlog_cmd_expr) 
   appid = "P0F" & appidkey & "8"  
   xfermode="binary"
elseif (filetype = "ES_CDSREG") then
   ; Setup the command expression
   cds_dump_cmd_expr = cds_dump_cmd_expr & """"

   if (filename = "") then
     filename = CFE_ES_DEFAULT_CDS_REG_DUMP_FILE
     cds_dump_cmd_expr = cds_dump_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     cds_dump_cmd_expr = cds_dump_cmd_expr & ramDir & filename
   endif
   cds_dump_cmd_expr = cds_dump_cmd_expr & """"

   if (supress = "0") then   
     write "Sending Command: ", cds_dump_cmd_expr
   endif
   %cmd (cds_dump_cmd_expr) 
   appid = "P0F" & appidkey & "9"  
   xfermode="binary"
elseif (filetype = "ES_TASKINFO") then
   ; Setup the command expression
   es_task_log_cmd_expr = es_task_log_cmd_expr & """"

   if (filename = "") then
     filename = CFE_ES_DEFAULT_TASK_LOG_FILE
     es_task_log_cmd_expr = es_task_log_cmd_expr & filename
     filenameLen = %length(filename)
     filename = %substring(filename,6,filenameLen)
   else
     es_task_log_cmd_expr = es_task_log_cmd_expr & ramDir & filename
   endif
   es_task_log_cmd_expr = es_task_log_cmd_expr & """"

   if (supress = "0") then   
     write "Sending Command: ", es_task_log_cmd_expr
   endif
   %cmd (es_task_log_cmd_expr) 
   appid = "P0F" & appidkey & "A"  
   xfermode="binary"
endif

wait wait_time
if (supress = "0") then
  write "The APID is: ", appid
  write "The CPU is: CPU",cpu
  write "The IP Address is: ", ipaddress
endif

;;;perl_command = "perl /s/opr/accounts/" & account & "/prc/ftp.pl"
perl_command = "perl " & toolsDir & "/ftp.pl"
perl_command = perl_command & " " & source_directory
perl_command = perl_command & " " & filename
perl_command = perl_command & " " & dest_filename
perl_command = perl_command & " " & xfermode
perl_command = perl_command & " " & ipaddress

cfdp_cmd_expr = cfdp_cmd_expr & "SOURCEFILENAME="
cfdp_cmd_expr = cfdp_cmd_expr & """"
cfdp_cmd_expr = cfdp_cmd_expr & ramDir & filename & """"
cfdp_cmd_expr = cfdp_cmd_expr & " DESTFILENAME="
cfdp_cmd_expr = cfdp_cmd_expr & """" & dest_filename & """"
local path_n_file =  ramDir & filename

if cfdp_status then
    if (supress = "0") then
       write "The CFDP command is: ", cfdp_cmd_expr
       write
    endif
%cmd (cfdp_cmd_expr)
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

FILE_TO_CVT %name(dest_filename) %name(appid)

local appid_number
local the_command,where,the_date_command

where=%env("WORK") & "/image"

appid_number = telemetry_attr(appid,"APID")
file_list[appid_number].file_write_name = %lower(dest_filename)
the_date_command = "cvt -ws file_list[" & appid_number
the_date_command = the_date_command  & "].file_write_time "
the_date_command = the_date_command & """`date +%y-%j-%T -r "
the_date_command = the_date_command & where  & "/"
the_date_command = the_date_command & %lower(dest_filename) & "`"""
native the_date_command
if (supress = "0") then
  write "The unix command is ", the_date_command
endif

wait 5

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
