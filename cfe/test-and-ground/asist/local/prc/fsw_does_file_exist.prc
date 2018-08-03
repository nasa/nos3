proc fsw_does_file_exist(filename, cpu)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
;
;==============================================================================
;
; Purpose: 
;
; History:
;    11/2/07  IEA  Got tired of waiting for the full 55 seconds - ut_tlmupdate 
;                  doesn't seem to work when the value doesn't actually change. 
;                  Switched it to a wait until on the filename instead.
;    11/13/07 IEA  Changed the hard wait on the filename to a ut_tlmwait instead.
;    01/04/08 SMA  Reversed IEA change -- FM_FSFilename can equal filename at
;                  start command. Works faster in original design.

#include "cfe_utils.h"
#include "ut_statusdefs.h"

global fsw_filename_found = 0

ON ERROR RESUME
; ===================================
; Translate from cpu'x' to 'x'
; ===================================
if (%length(cpu) = "0") then
   cpu = 1
endif
if (%length(cpu) = "4") then
   cpu = %substring(cpu,4,4)
endif

; ===========================================================
; Set up base command prefix
; ===========================================================
local base_cmd_expr_front, base_tlm_expr_front
if (SC <> "") then
  base_cmd_expr_front = "/" & SC
  base_tlm_expr_front = SC
else
  base_cmd_expr_front = "/"
endif

;; Add the CPU definition
base_cmd_expr_front = base_cmd_expr_front & CPU_CFG
base_tlm_expr_front = base_tlm_expr_front & CPU_CFG

if (numCPUs > 1) then
  base_cmd_expr_front = base_cmd_expr_front & (cpu) & "_"
  base_tlm_expr_front = base_tlm_expr_front & (cpu) & "_"
else
  base_cmd_expr_front = base_cmd_expr_front & "_"
  base_tlm_expr_front = base_tlm_expr_front & "_"
endif

local pktct, tlmPktId
;; CPU1 is the default
tlmPktId = "P08B"

if (cpu = "2") then
  tlmPktId = "P18B"
elseif (cpu = "3") then
  tlmPktId = "P28B"
endif

local tlmItem = tlmPktId & "SCNT"
pktct = {tlmItem}

local dummy_cmd_expr, info_cmd_expr, stat_tlm_expr
;; Setup the command and telemetry expressions
dummy_cmd_expr = base_cmd_expr_front & "FM_FILEINFO FILE=""" 
dummy_cmd_expr = dummy_cmd_expr & "/ram/dummy" & """"

;;write "Dummy command = '", dummy_cmd_expr, "'"
; dummy request to init pkt ctr
; needs to be done on restart
%cmd(dummy_cmd_expr)
wait 2

;; Setup the actual info request command
info_cmd_expr = base_cmd_expr_front & "FM_FILEINFO FILE="""
info_cmd_expr = info_cmd_expr & (filename) & """"

stat_tlm_expr = base_tlm_expr_front & "FM_FILESTATUS"
{stat_tlm_expr} = 0
write "tlm status item  = '", stat_tlm_expr, "'"

write "Info command = '", info_cmd_expr, "'"
%cmd(info_cmd_expr)

;;wait until (P08BSCNT = pktct+1) OR (P08BSCNT <> pktct) timeout 7
wait until ({tlmItem} = pktct+1) OR ({tlmItem} <> pktct) timeout 7

; ut_tlmwait FM_FSFilename filename 55
; ut_tlmupdate fm_fsstate 55

stat_tlm_expr = base_tlm_expr_front & "FM_FILESTATUS"
write "tlm status item  = '", stat_tlm_expr, "'"

;;IF (gc_fm_filestatus = 0) THEN
IF ({stat_tlm_expr} = 0) THEN
  fsw_filename_found = 1
  write
  write "*** File '",filename, "' does not exist !!! ***"
  write
ELSE
  fsw_filename_found = 0
  write
  write "*** File '",filename, "' exists !!! ***"
  write
ENDIF

RETURN:
%liv (log_procedure) = logging

ENDPROC
