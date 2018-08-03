proc move_file(remote_directory, filename, dest_filename, cpu, getorput, subdir)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the upload, download 
;          or deletion of a data or log file to/from the cFE spacecraft 
;          to/from the ASSIST hardware.
;
; History:
;
; 18NOV05        Initial development of this proc.            SMA
; 23MAR06        Change proc name from proc_ftp to ftp_file.  SMA
; 04MAY06        Added support for perl script return code.   SMA
; 26SEP06        Changed perl script location to global/tools SMA
; 22MAR07        Added CFDP capability. Renamed from ftp_file SMA
;                to move_file.                                
; 07JUN07        Added support for different dirs & sub-dirs  SMA
; 24JUL07        Modify when and how ramDir is built          SMA     
; 30JUL07        Added test for file existence before upload. SMA
; 07SEP07        Mod case conversion for RAM:0 directory.     SMA
; 25SEP07        Added test for file existence before dnload. SMA
; 10JAN08        Modify to control uplink/dnlink mode via     SMA
;                global variable(s) up_link & dn_link. 
; 06MAY08        Fix problem w/ ftp & subdirectories.         SMA
; 13JUN08        Fix ENTITY_ID Issues.                        SMA
; 

#include "cfe_utils.h"

ON ERROR RESUME

local supress = 0
local ipaddress
local perl_command
local xfermode="binary"
local cpu
local account = %env("ACCOUNT")
local toolsDir = %env("TOOLS")
local search_cmd
local dest_file_found
global ftp_file_rc
global filename_found
global fsw_filename_found
global up_link
global dn_link
global del_link
global rad750

local cfdp_status
local tsr = total_successful_receivers
local tss = total_successful_senders
local tus = total_unsuccessful_senders
local tur = total_unsuccessful_receivers
local wait_time

if (%nargs < 5) then
  error "USAGE : MOVE_FILE remote_directory, filename, dest_filename, cpu, getorput, subdir"
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

; ===========================================================
; Check for CFDP status
; ===========================================================
if (%select(packet_valid("my_entity_id"),1,0)) then
  cfdp_status = 1
else
  cfdp_status = 0
endif

write "CFDP Status = ", cfdp_status

; ===========================================================
; Set up directory definitions
; ===========================================================
local ramDir

; switch case if directory

if (remote_directory = "CF:1") or ;;
   (%substring(remote_directory,1,3) = "EEP") then
  write
  write "*** ALERT *** This is NOT permitted."
  write
  goto ERROR_RETURN
endif

; if ((remote_directory = "RAM:0")  or (remote_directory = "RAM:")) then
;  and (cfdp_status = 1) then
if (remote_directory = "RAM:0") then
  remote_directory = %lower(remote_directory)
elseif (remote_directory = "RAM:") then
  remote_directory = %lower(remote_directory) & "0"
elseif (remote_directory = "ram:") then
  remote_directory = remote_directory & "0"
endif

; build directory name
if (up_link = 1) then
  ramDir = "/" & %substring(remote_directory,1,%locate(remote_directory,":")-1) & "/"
else 
;  ramDir = %upper(remote_directory) & "/"
  ramDir = %upper(remote_directory)
endif

if (%length(ramDir) = 2) then
  write
  write "*** ALERT *** Invalid remote directory specified: ", ramDir
  write
  goto ERROR_RETURN
endif

if (%length(subdir) <> 0) then
  ramDir = ramDir & subdir & "/"
endif

; write "CFDP_STATUS: ", cfdp_status
; write "Direction: ", getorput
if (cfdp_status = 1) then
;   write "UP_LINK: ", up_link
;   write "DN_LINK: ", dn_link
  if (%upper(getorput)= "P")  then 
;     write "Here 1"
    if (up_link = 1) then
      goto CFDP_DETECTED
    else
      goto PERL
    endif
  endif
  if (%upper(getorput)= "G")  then 
;     write "Here 2"
    if (dn_link = 1) then
      goto CFDP_DETECTED
    else
      goto PERL
    endif
  endif
  if (%upper(getorput)= "R")  then 
;     write "Here 3"
    if (del_link = 1) then
      goto CFDP_DETECTED
    else
      goto PERL
    endif
  endif
endif

PERL:
; ===========================================================
; Set up perl command
; ===========================================================
if (%upper(getorput) = "P") then
    s does_file_exist(filename)
    if filename_found then
      goto ERROR_RETURN
    endif 
    goto START_PERL
endif

if (%upper(getorput) = "G") then
    ramDir = "/" & %substring(remote_directory,1,%locate(remote_directory,":")-1) & "/"
    local get_filename = ramDir & filename
    s fsw_does_file_exist(get_filename, cpu)
    if fsw_filename_found then
      goto ERROR_RETURN
    else
      ramDir =  %upper(remote_directory)
    endif
    goto START_PERL
endif

if (%upper(getorput) = "R") then
      ramDir = "/" & %substring(remote_directory,1,%locate(remote_directory,":")-1) & "/"
    local del_filename = ramDir & dest_filename
    s fsw_does_file_exist(del_filename, cpu)
    if fsw_filename_found then
      goto ERROR_RETURN
    else
      ramDir =  %upper(remote_directory)
    endif 
endif

START_PERL:
perl_command = "perl " & toolsDir & "/ftp_file.pl"
; perl_command = perl_command & " " & %upper(remote_directory)
perl_command = perl_command & " " & ramDir 
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
goto ERROR_RETURN

CFDP_DETECTED:
; write "Here 3"
; *****************************************************************
; Setup to capture event id(s) to determine dl and deletion failure
; *****************************************************************

; ut_setupevents "","","CF_APP", 300, ERROR, 8
; ut_setupevents "","","FM_APP", 18, INFO, 2 

; ************************************************************
; setup file commands
; ************************************************************

local put_command = """" & "put " & filename
put_command = put_command & " 0.23 " & ramDir & dest_filename & """"
local put_filename = ramDir & dest_filename
 
local get_command = base_cmd_expr_front & "CF_PLAYBACKFILE "
get_command = get_command & " VIRTUALCHANNEL=1 FILEPRIORITY=1"
IF (rad750 = 1 ) THEN
  get_command = get_command & " CFCLASS=2 ENTITYID=[0,26] "  
ELSE
  get_command = get_command & " CFCLASS=2 ENTITYID=[0,24] "
ENDIF
get_command = get_command & "SOURCEFILENAME=" 
get_command = get_command & """"
get_command = get_command & ramDir & filename & """"
get_command = get_command & " DESTFILENAME="
get_command = get_command & """" & dest_filename & """"
local get_filename = ramDir & filename

local del_command = base_cmd_expr_front & "FM_DELETEFILE "
del_command = del_command & "PATHNAME=" & """"
del_command = del_command & ramDir & dest_filename & """"
local del_filename = ramDir & dest_filename

if (%upper(getorput) = "P") then
    s does_file_exist(filename)
    if filename_found then
      goto ERROR_RETURN
    endif 
    if (supress = "0") then
       write "The command is: cfdp_dir ", put_command
       write
    endif

    s fsw_does_file_exist(put_filename, cpu)
    if fsw_filename_found then
      goto FILE_DOES_NOT_EXIST
    endif 
    s file_attr_del(del_filename, cpu)
FILE_DOES_NOT_EXIST:
    cfdp_dir {put_command}
    s file_attr_put(put_filename)

    ; set return code

; temp NOOP
;    if (fm_fsarchive = 0) then
;      ftp_file_rc = 0
;    else
;      ftp_file_rc = 1
;    endif
    wait 10
; write "Return code: ", ftp_file_rc
endif

; ####################
; Get File
; ####################
if (%upper(getorput) = "G") then
    s fsw_does_file_exist(get_filename, cpu)
    if fsw_filename_found then
      goto ERROR_RETURN
    endif
    if (supress = "0") then
       write "The command is: ", get_command
       write
    endif
%cmd (get_command)

s file_attr_get(get_filename)

; set return code

; temp NOOP
;if (fm_fsarchive = 2) then
;    ftp_file_rc = 0
; else
;    ftp_file_rc = 1
;endif
wait 10
; write "Return code: ", ftp_file_rc
endif

; ##########################
; Remove file 
; ##########################

if (%upper(getorput)= "R")  then
    s fsw_does_file_exist(del_filename, cpu)
    if fsw_filename_found then
      goto ERROR_RETURN
    endif
    if (supress = "0") then
       write "The command is: ", del_command
       write
    endif
%cmd (del_command)
wait 10
write
; wait 5
; if (FIND_EVENT[2].num_found_messages = 1) then
;  ftp_file_rc = 1
; else
;  ftp_file_rc = 0
; endif
; write "Return code: ", ftp_file_rc 
endif

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC

