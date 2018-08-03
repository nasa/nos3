PROC $sc_$cpu_cfe_myeh
;*******************************************************************************
;  Test Name:  $sc_$cpu_cfe_myeh
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify two (2) cFE requirements that
;	require a modification to the cFE Core software. The following changes
;	were made to the fsw:
;	    cfe_es_task.c - Modified the CFE_ES_NoopCmd function to perform a
;			floating point divide by zero in order to cause an
;			exception to be generated in the CORE FSW.
;	    cfe_psp_exception.c - Added the CFE_PSP_MyExceptionHandler(..)
;			function to the bottom of this file.
;	    cfe_platform_cfg.h - Changed the CFE_ES_EXCEPTION_FUNCTION to
;			CFE_PSP_MyExceptionHandler.
;
;	NOTE: An alternate cFE Core image is required for this test. This means
;	      that there is code contained in the cFE Core to generate 
;	      conditions that are not contained in the "delivered" cFE Core fsw.
;	NOTE2: Requirements ES1702 and ES1703 are listed below for clarification
;	       purposes only. The sub-requirements are tested by this proc.
;
;  Requirements Tested
;    cES1702	The cFE shall detect all unmasked CPU exceptions. 
;    cES1702.3	If the CPU exception was caused by the Operating System or the
;		cFE Core then the cFE shall initiate a <PLATFORM_DEFINED>
;		response.
;    cES1703	The cFE shall detect all unmasked processor Floating Point
;		Exceptions.
;    cES1702.3	If the Floating Point exception was caused by the OS or cFE Core
;		then the cFE shall initiate a <PLATFORM_DEFINED> response.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name		Description
;	09/24/14	W. Moleski	Initial development.
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;  	ut_pfindicate		Directive to print the pass fail status
;				of a particular requirement number.
;  	ut_setupevents		Directive to look for a particular event
;				and increment a value in the CVT to
;				indicate receipt.
;  	ut_setrequirements	Directive to status cfe requirements.
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "ut_statusdefs.h"
#include "cfe_es_events.h"
#include "cfe_evs_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************

#define ES17023		0
#define ES17033		1

global ut_req_array_size = 1
global ut_requirement[0 .. ut_req_array_size]

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL i,j, numMaxErrors
LOCAL cmdcnt, errcnt
local ramDir = "RAM:0"
local hostCPU = "$CPU"

;**********************************************************************
; Initialize the global requirement values to "U" for Untested.
;**********************************************************************
FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

;**********************************************************************
; Set the local values 
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["ES1702.3", "ES1703.3" ]

write ";*********************************************************************"
write "; Step 1.0: cFE Image verification "
write ";*********************************************************************"
write "; Step 1.1: Load the test image and restart $CPU"
write ";*********************************************************************"

write "Verify that the correct image for this test is loaded on $CPU"
write "Enter 'go' or 'g' when the correct image is loaded."
wait

/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Step 1.2: Enable DEBUG Events "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  wait 5
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

write ";*********************************************************************"
write "; Step 2.0: cFE Core software Exception Test."
write ";*********************************************************************"
write "; Step 2.1: Send the ES_NOOP Command to initiate an exception."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_NOOP_INF_EID, "INFO", 1

/$SC_$CPU_ES_NOOP
wait 10

;; Verify that the User-defined exception handler executed after an exception
;; was generated in CFE_ES
ut_setrequirements ES17023, "A"
ut_setrequirements ES17033, "A"

;; Since the exception handler only prints out a message in the UART, we need to
;; reboot the processor in order to get CFE_ES running again. Since the Reset
;; commands are handled by CFE_ES, this needs to be done manually
write "Reset $CPU by hitting the reset button on the card. "
write "Enter 'go' or 'g' when completed."
wait

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Step 3.0: Non-Core Application Exception Test."
write ";*********************************************************************"
write "; Step 3.1: Start the TST_ES application"
write ";*********************************************************************"
s load_start_app ("TST_ES", hostCPU)
wait 5

;; Dump the App Info to verify TST_ES started
s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)
wait 5

;; Loop thru the table of apps again to verify that the TST_ES app is running
local found_app = FALSE
local stream
for app_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if (found_app = FALSE) then
    if ($SC_$CPU_ES_ALE[app_index].ES_AL_AppName = "TST_ES") then
      found_app = TRUE
    endif
  endif
enddo

if (found_app = TRUE) then
  write "<*> Passed - TST_ES app started"

  ;; Send commands to subscribe to the TST_ES HK packet
  ;; CPU1 is the default
  stream = x'90D'

;;  if ("$CPU" = "CPU2") then
;;    stream = x'A0D'
;;  elseif ("$CPU" = "CPU3") then
;;    stream = x'B0D'
;;  endif

  /$SC_$CPU_TO_ADDPACKET STREAM=stream PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

  ;; Add an Event Filter for the TST_ES HK Request Event in order to
  ;; only receive this event 1 time rather than every 4 seconds
  /$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
  wait 5
else
  write "<!> Failed - Failed to start the TST_ES app"
endif

wait 5

write ";*********************************************************************"
write "; Step 2.2: Non-Core Application Exception test. Send the TST_ES app a"
write ";           command that generates an exception. Verify that the CPU "
write ";           resets."
write ";*********************************************************************"
/$SC_$CPU_TST_ES_GenException
wait 10

;; Verify that the User-defined exception handler executed after an exception
;; was generated in TST_ES
wait 10

;; Send the command to delete the TST_ES application
/$SC_$CPU_ES_DeleteApp Application="TST_ES"
wait 10

write "**** Requirements Status Reporting"

write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
