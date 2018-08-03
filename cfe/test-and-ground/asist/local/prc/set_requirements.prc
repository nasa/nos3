proc set_requirements (REQNUM, STATE)

;
; ******************** SET REQUIREMENTS  **********************
;

IF (%nargs <> 2) THEN
   write "<!> Invalid number of arguments"
ELSE
   global ut_req_array_size
   global ut_requirement[0 .. ut_req_array_size]
   global ut_setreq_debug
   global ut_requirement_text[0 .. ut_req_array_size]

if ((ut_requirement[reqnum] = "U") OR (state = "F")) then
   ut_requirement[reqnum] = state
endif

write "*** ", ut_requirement_text[reqnum], ": ",ut_requirement[reqnum], " ***" 

if (ut_setreq_debug = "1") then
  write "Requirement Index: ", reqnum
  write "Requirement State: ", ut_requirement[reqnum]
endif

ENDIF

endproc
