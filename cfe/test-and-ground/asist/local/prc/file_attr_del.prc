proc file_attr_del (filename_n_path, cpu)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
;
;==============================================================================
;
; Purpose: 
;
; History:
;
#include "cfe_utils.h"

ON ERROR RESUME

local wait_time

; set up wait time for file delete
wait_time = %gmt + 120

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

local delete_cmd_expr, info_cmd_expr, stat_tlm_expr

;; Setup the info command and tlm expressions
info_cmd_expr = base_cmd_expr_front & "FM_FILEINFO FILE="""
info_cmd_expr = info_cmd_expr & (filename_n_path) & """"
stat_tlm_expr = base_tlm_expr_front & "FM_FILESTATUS"

; delete file
delete_cmd_expr = base_cmd_expr_front & "FM_DELETE FILE="""
delete_cmd_expr = delete_cmd_expr & (filename_n_path) & """"
;;/GC_FM_DELETE FILE=filename_n_path

write "Sending Command: ", delete_cmd_expr
%cmd(delete_cmd_expr)

; loop until file is deleted 
write "Starting file delete monitoring ... "
%cmd(info_cmd_expr)
;;/GC_FM_FILEINFO FILE=filename_n_path
;;while (gc_fm_filestatus <> 0) and (%gmt < wait_time) do
while ({stat_tlm_expr} <> 0) and (%gmt < wait_time) do
   write "Waiting for file to delete ..."
   wait 3
   %cmd(info_cmd_expr)
;;   /GC_FM_FILEINFO FILE=filename_n_path
enddo

RETURN:
%liv (log_procedure) = logging

ENDPROC
