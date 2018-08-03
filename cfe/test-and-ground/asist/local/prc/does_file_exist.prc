proc does_file_exist(filename)
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

local file_search_cmd

global filename_found = 0

ON ERROR RESUME

; file_search_cmd = "cd $WORK/image ; find "
; file_search_cmd =  file_search_cmd & filename

; filename_found = %native(file_search_cmd)
;IF filename_found THEN
IF (file_exists(%env("WORK") & "/image/" & filename) = 0 ) THEN
  write
  write "*** File ", %env("STOL_IMAGE"),"/", filename, " does not exist !!! ***"
  write
  filename_found = 1
ELSE
  write "*** File ", %env("STOL_IMAGE"),"/", filename, " exists!!! ***"
ENDIF

RETURN:
%liv (log_procedure) = logging

ENDPROC
