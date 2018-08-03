proc file_attr_put (filename_n_path)
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
local tsr = total_successful_receivers
local tss = total_successful_senders
local tus = total_unsuccessful_senders
local tur = total_unsuccessful_receivers

; set up wait time for file xfer

wait_time = %gmt + 900

; loop until archive bit clears, tss increments or time is expired

write "Starting file monitoring ... "
; /FM_REQFILESTAT PATHNAME=filename_n_path
; while (fm_fsarchive = 0) and 
;   while (%gmt < wait_time) ;;
;   and (total_successful_senders <> tss + 1) do
   while (total_successful_senders <> tss + 1) do
   write "Waiting for file to be created ..."
   wait 3
;   /FM_REQFILESTAT PATHNAME=filename_n_path
enddo

RETURN:
%liv (log_procedure) = logging

ENDPROC
