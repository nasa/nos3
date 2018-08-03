PROC $sc_$cpu_cfe_altimage
;*******************************************************************************
;  Test Name:  $sc_$cpu_cfe_altimage
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify four (4) cFE requirements that
;	require a modification to the cFE Core software. The following changes
;	were made to the fsw:
;	    cfe_es_start.c - Modified CFE_ES_InitializeFileSystems to force the
;			failure of the volatile file system. I did this by
;			commenting out the OS_initfs call and added code to set
;			the return status to OS_FS_ERROR. 
;	    cfe_es_task.c - Modified the CFE_ES_NoopCmd function to perform a
;			floating point divide by zero in order to cause an
;			exception to be generated in the CORE FSW.
;	    cfe_time_utils.c - Modified the CFE_TIME_QueryResetVars function to
;			set the DataStoreStatus for the reset area to BAD.
;
;	NOTE: An alternate cFE Core image is required for this test. This means
;	      that there is code contained in the cFE Core to generate 
;	      conditions that are not contained in the "delivered" cFE Core fsw.
;
;  Requirements Tested
;    cES1517	Upon Processor Reset, the cFE shall check and mount the volatile
;		file system.
;    cES1517.1	If the volatile file system check fails, the cFE shall format
;		the volatile file system and create a system log entry.
;    cES1702	The cFE shall detect all unmasked CPU exceptions.
;    cES1702.3	If the CPU exception was caused by the Operating System or the
;		cFE Core then the cFE shall initiate a <PLATFORM_DEFINED>
;		response.
;    cES1703	The cFE shall detect all unmasked processor Floating Point
;		Exceptions.
;    cES1702.3	If the Floating Point exception was caused by the OS or cFE Core
;		then the cFE shall initiate a <PLATFORM_DEFINED> response.
;    cTIME2502	Upon a Processor Reset the cFE shall verify the Critical Data
;		Store used to store time values.
;   cTIME2502.1	If the critical data store is not valid, all of the time
;		elements shall be initialized in the same fashion as following a
;		power-on reset.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name		Description
;	09/04/07	W. Moleski	Initial development.
;	02/08/12	W. Moleski	Replaced ut_setupevt with ut_setupevents
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
;	ut_sendcmd     		Directive to send EVS commands Verifies command
;				processed and command error counters.
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "ut_statusdefs.h"
#include "cfe_es_events.h"
#include "cfe_es_resetTypes.h"
#include "cfe_evs_events.h"
#include "cfe_time_events.h"

%liv (log_procedure) = logging

;**********************************************************************
; Setup requirements checking
;**********************************************************************

#define ES15171		0
#define ES17023		1
#define ES17033		2
#define TIME25021	3

global ut_req_array_size = 3
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
local cfe_requirements[0 .. ut_req_array_size] = ["ES1517.1", "ES1702.3", "ES1703.3", "TIME2502.1" ]

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

local metSecsDef = $SC_$CPU_TIME_METSECS
local metSubSecsDef = $SC_$CPU_TIME_METSUBSECS
local stcfSecsDef = $SC_$CPU_TIME_STCFSECS
local stcfSubSecsDef = $SC_$CPU_TIME_STCFSUBSECS
local leapSecsDef = $SC_$CPU_TIME_LEAPSECS
local clkSigDef = $SC_$CPU_TIME_FLAGPRI

write "Time variables after Power-On:"
write "    MET Secs   = ", metSecsDef
write "    MET SSecs  = ", metSubSecsDef
write "    STCF Secs  = ", stcfSecsDef
write "    STCF SSecs = ", stcfSubSecsDef 
write "    LeapSecs   = ", leapSecsDef 
write "    Clock Sig  = ", clkSigDef 

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
write "; Step 2.1: Upload a file to the spacecraft in order to modify the"
write ";           contents of the volatile file system. This step is performed"
write ";           here because Step 2.4 should cause a Processor Reset."
write ";*********************************************************************"
;; ftp a test file to the volatile file system
s ftp_file (ramDir,"es_testfile_1","es_testfile_1",hostCPU,"P")
wait 5

write ";*********************************************************************"
write "; Step 2.2: Send a command to adjust the STCF time."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TIME", CFE_TIME_1HZ_EID, "INFO", 1

/$SC_$CPU_TIME_ADD1HZSTCF SECONDS=10 SUBSECS=5
wait 10

;; Look for expected event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_TIME_1HZ_EID, " not received."
endif

write ";*********************************************************************"
write "; Step 2.3: Print out the TIME Reset Variables prior to Exception."
write ";*********************************************************************"
wait 30

local metSecsOld = $SC_$CPU_TIME_METSECS
local metSubSecsOld = $SC_$CPU_TIME_METSUBSECS
local stcfSecsOld = $SC_$CPU_TIME_STCFSECS
local stcfSubSecsOld = $SC_$CPU_TIME_STCFSUBSECS
local leapSecsOld = $SC_$CPU_TIME_LEAPSECS
local clkSigOld = $SC_$CPU_TIME_FLAGPRI

write "Time variables prior to Processor Reset:"
write "    MET Secs   = ", metSecsOld
write "    MET SSecs  = ", metSubSecsOld
write "    STCF Secs  = ", stcfSecsOld
write "    STCF SSecs = ", stcfSubSecsOld 
write "    LeapSecs   = ", leapSecsOld 
write "    Clock Sig  = ", clkSigOld 

write ";*********************************************************************"
write "; Step 2.4: Send the ES_NOOP Command to initiate an exception."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_NOOP_INF_EID, "INFO", 1

/$SC_$CPU_ES_NOOP
wait 10

;; Verify that the processor was reset due to an exception in CFE_ES
;; This can be found in the ES_ERLog
ut_setrequirements ES17023, "A"
ut_setrequirements ES17033, "A"

close_data_center
wait 75

cfe_startup $CPU
wait 5

local metSecs = $SC_$CPU_TIME_METSECS
local metSubSecs = $SC_$CPU_TIME_METSUBSECS
local stcfSecs = $SC_$CPU_TIME_STCFSECS
local stcfSubSecs = $SC_$CPU_TIME_STCFSUBSECS
local leapSecs = $SC_$CPU_TIME_LEAPSECS
local clkSig = $SC_$CPU_TIME_FLAGPRI

write "Time variables after Processor Reset:"
write "    MET Secs   = ", metSecs
write "    MET SSecs  = ", metSubSecs
write "    STCF Secs  = ", stcfSecs
write "    STCF SSecs = ", stcfSubSecs
write "    LeapSecs   = ", leapSecs
write "    Clock Sig  = ", clkSig

;; Dump the ERLog to verify that
s get_file_to_cvt(ramDir,"cfe_erlog.log","$sc_$cpu_cfe_erlog.log",hostCPU)
wait 10

write ";*********************************************************************"
write "; Step 3.0: Volatile File Sytem Check Failure Test"
write ";***********************************************************************"
write "; Step 3.1: Since the above step cause a Processor Reset, verify that an"
write ";           entry exists in the System Log indicating that the Volatile"
write ";           file system check failed and that it was reformatted."
write ";*********************************************************************"
;; ftp the file uploaded in Step 2.1 to see if it exists
s ftp_file (ramDir,"es_testfile_1","es_altimage_file1",hostCPU,"G")
wait 5

local work_dir = %env("WORK")
local filename = work_dir & "/image/es_altimage_file1"

if file_exists(filename) then
  write "<!> Failed (1517.1) - Test file 1 still exists in the Volatile File system."
  ut_setrequirements ES15171, "F"
else
  write "<*> Passed (1517.1) - Volatile file system did not contain test file 1."
  ut_setrequirements ES15171, "P"
endif

write ";*********************************************************************"
write "; Step 4.0: Time Element Initialization Test"
write ";***********************************************************************"
write "; Step 4.1: Verify that the time elements are set to the default values."
write ";*********************************************************************"
;; Check if the saved Time variables <= previous values 
;; NOTE: The metSubSecs will not be equal. Every other one should be
if ((metSecs <= metSecsOld) AND ;;
    (stcfSecs <= stcfSecsOld) AND (stcfSubSecs <= stcfSubSecsOld) AND ;;
    (leapSecs <= leapSecsOld) AND (clkSig <= clkSigOld)) then
  write "<*> Passed (2502.1) - Time variable set to defaults as expected."
  ut_setrequirements TIME25021, "P"
else
  write "<!> Failed (2502.1) - Time variables not set correctly."
  ut_setrequirements TIME25021, "F"
endif

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
