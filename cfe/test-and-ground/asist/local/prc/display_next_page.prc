proc display_next_page (max_page_size, max_lines_displayed, page_direction, page_id, page_type)
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

;
;==============================================================================
;
; Purpose: The purpose of this procedure is to allow an ASIST display page to
;	   contain a "PREV" and "NEXT" button to scroll through an array of
;	   telemetry mneumonics contained in either a file or telemetry packet.
;
; History:
;
; 17AUG06  SMA	Initial development of this proc.
; 17MAY07  WFM	Added page_type argument to be able to handle files as well
;		as telemetry packets. Added checks to determine which list
;		to use based upon the arguments passed in.
;
ON ERROR RESUME

local min_page_size = 1
local error_msg = "*** ERROR - Will exceed array bounds ***"
local lastPage_msg = "*** Warning - Final Page displayed ***"
local firstPage_msg = "*** Warning - First Page displayed ***"
local blank_msg = ""

; Determine if proc was called with minimum # of parameters
if (%nargs < 4) then
  error "USAGE : Display_Next_Page (max_page_size, max_lines_displayed, page_direction, page_id, <page_type>) "
  goto ERROR_RETURN
endif

; If only 4 arguments are passed, ASSUME FILE
IF (%nargs = 4) THEN
  page_type = "FILE"
ENDIF

IF (page_direction = "NEXT") THEN
  ; Check the page_type to determine which list to use
  IF (page_type = "FILE") THEN
    IF ((FILE_LIST[page_id].PAGEINDEX + max_lines_displayed)  > max_page_size) THEN
       FILE_LIST[page_id].PAGEERRORMSG = error_msg
    ELSEIF ((FILE_LIST[page_id].PAGEINDEX + (max_lines_displayed * 2)) > max_page_size) THEN
       FILE_LIST[page_id].PAGEINDEX = (max_page_size - max_lines_displayed)+1
       FILE_LIST[page_id].PAGEERRORMSG = lastPage_msg
    ELSE
       FILE_LIST[page_id].PAGEINDEX = FILE_LIST[page_id].PAGEINDEX + max_lines_displayed
       FILE_LIST[page_id].PAGEERRORMSG = blank_msg
    ENDIF
  ELSEIF (page_type = "TLM") THEN
    IF ((PAGE_LIST[page_id].PAGEINDEX + max_lines_displayed)  > max_page_size) THEN
       PAGE_LIST[page_id].PAGEERRORMSG = error_msg
    ELSEIF ((PAGE_LIST[page_id].PAGEINDEX + (max_lines_displayed * 2)) > max_page_size) THEN
       PAGE_LIST[page_id].PAGEINDEX = (max_page_size - max_lines_displayed)+1
       PAGE_LIST[page_id].PAGEERRORMSG = lastPage_msg
    ELSE
       PAGE_LIST[page_id].PAGEINDEX = PAGE_LIST[page_id].PAGEINDEX + max_lines_displayed
       PAGE_LIST[page_id].PAGEERRORMSG = blank_msg
    ENDIF
  ENDIF
ELSEIF (page_direction = "PREV") THEN
  ; Check the page_type to determine which list to use
  IF (page_type = "FILE") THEN
    IF ((FILE_LIST[page_id].PAGEINDEX - max_lines_displayed)  < min_page_size) THEN
      FILE_LIST[page_id].PAGEINDEX = 1
      FILE_LIST[page_id].PAGEERRORMSG = firstPage_msg
    ELSE
      FILE_LIST[page_id].PAGEINDEX = FILE_LIST[page_id].PAGEINDEX - max_lines_displayed
      FILE_LIST[page_id].PAGEERRORMSG = blank_msg
    ENDIF
  ELSEIF (page_type = "TLM") THEN
    IF ((PAGE_LIST[page_id].PAGEINDEX - max_lines_displayed)  < min_page_size) THEN
      PAGE_LIST[page_id].PAGEINDEX = 1
      PAGE_LIST[page_id].PAGEERRORMSG = firstPage_msg
    ELSE
      PAGE_LIST[page_id].PAGEINDEX = PAGE_LIST[page_id].PAGEINDEX - max_lines_displayed
      PAGE_LIST[page_id].PAGEERRORMSG = blank_msg
    ENDIF
  ENDIF
ENDIF

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC

