PROC $sc_$cpu_cfe_osobjfailure
;*******************************************************************************
;  Test Name:  $sc_$cpu_cfe_osobjfailure
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify cFE requirement ES1515.1. This
;	test requires modified cFE Core software in order to verify. The
;	following changes were made to the fsw:
;
;	    cfe_es_objtab.c - Modified the CFE_ES entry to contain a stacksize
;			that was smaller than required. This will cause the an
;			object creation failure.
;
;	The tricky part of this is the recovery from the constant reboot of the
;	cpu. This is achieved by having the good cFE Core image ready to reload
;	as soon as the cpu reboots.
;
;	NOTE: An alternate cFE Core image is required for this test. This means
;	      that there is code contained in the cFE Core to generate a
;	      condition for testing this requirement. This code is not contained
;	      in the "delivered" cFE Core fsw.
;
;  Requirements Tested
;    cES1515	Upon Processor Reset, the cFE shall create all operating system
;		objects required by the cFE.
;    cES1515.1	If the creation of the operating system object fails, the cFE
;		shall perform a power-on reset.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name			Description
;	09/05/07	W. Moleski	Initial development.
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;  	ut_pfindicate		Directive to print the pass fail status
;				of a particular requirement number.
;  	ut_setrequirements	Directive to status cfe requirements.
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#include "ut_statusdefs.h"
#include "cfe_es_events.h"
#include "cfe_es_resetTypes.h"
#include "cfe_evs_events.h"
#include "cfe_time_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************

#define ES15151		0

global ut_req_array_size = 0
global ut_requirement[0 .. ut_req_array_size]

;**********************************************************************
;  Define local variables
;**********************************************************************
LOCAL i,j, numMaxErrors
LOCAL cmdcnt, errcnt

;**********************************************************************
; Initialize the global requirement values to "U" for Untested.
;**********************************************************************
FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

;**********************************************************************
; Set the local values 
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["ES1515.1"]

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

;; Set the requirement to 'A' 
ut_setrequirements ES15151, "A"

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
