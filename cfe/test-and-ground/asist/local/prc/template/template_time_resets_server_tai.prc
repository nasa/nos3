PROC $SC_$CPU_time_resets_server_tai
;**********************************************************************
;  Test Name:  $SC_$CPU_time_reset_server_tai
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify the processor reset requirements  
;  for the Core Flight Execuive (cFE) Time Services (TIME). 
;
;  Requirements Tested
;         cTIME 2005
;         cTIME 2006
;         cTIME 2012
;         cTIME 2306
;         cTIME 2307
;         cTIME 2308
;         cTIME 2500
;         cTIME 2501 
;         cTIME 2502
;         cTIME 2700
;
;
;  Prerequisite Conditions
;       Time is configured with TAI as the default time on the server. Time
;       is configured as the server on the CPU being tested.
; 
;       Configuration of clock source and default values of various parameters
;       are contained in the cfe_platform_cfg.h and cfe_mission_cfg.h files.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		   Name		Description
;	07/18/05        J. Heavener	Original Procedure from
;                                       scenario.
;       07/22/05        J. Heavener     Updates pre test results review.
;       08/22/05        J. Heavener     Incorporate updates from test
;                                       results review.
;       03/01/07        W. Moleski      Updates for cFE 4.0
;       02/08/12        W. Moleski      Replaced ut_setupevt with ut_setupevents
;
;  Arguments
;	 None 
;
;
;  Procedures Called
;	
;	Name			Description
;	ut_pfindicate           Record the pass / fail results of the test requirements
; 
;  Expected Test Results and Analysis
;	This test procedure is self checking, all requirements are tested
;       and verified within the procedure. No external validation is 
;       required.
;**********************************************************************

#include "ut_statusdefs.h"
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "cfe_time_events.h"
#include "tst_time_events.h"

;**********************************************************************
;  Define variables
;**********************************************************************

LOCAL RUN_TIME
LOCAL START_TIME
LOCAL CMD_TIME_1
LOCAL CMD_TIME_2
LOCAL DELTA_MAX = 2
LOCAL DELTA_Seconds = 0
LOCAL DELTA_SubSeconds = 0
;
; The following defaults are listed here for completeness. Some or all of them
; may be included in a future version of one or more of the configuration files.
;
#define TIME_API_STATE_DEFAULT  "INVALID"
#define TIME_CLOCK_SET_STATE_FLAG  "NOT SET"
#define TIME_CLOCK_FLYWHEEL_STATE_FLAG  "NOFLY"
#define TIME_SERVER_FLYWHEEL_STATE_FLAG  "NOFLY"
#define TIME_CMD_TO_FLYWHEEL_STATE_FLAG  "NO"
#define CFE_TIME_DEF_LEAPS 32
#define CFE_TIME_DEF_STCF_SECS 1000000
#define CFE_TIME_DEF_STCF_SUBS 0

LOCAL Time_DMETSeconds
LOCAL Time_DMETSSeconds
LOCAL Time_DSTCFSeconds
LOCAL Time_DSTCFSSeconds
LOCAL Time_DTAISeconds
LOCAL Time_DTAISSeconds
LOCAL Time_DLeapSeconds
LOCAL Time_DUTCSeconds
LOCAL Time_DUTCSSeconds
LOCAL Time_DelaySeconds
LOCAL Time_DelaySSeconds
LOCAL Time_ClockSignal


;**********************************************************************
; Set the local values 
;**********************************************************************
#define cTIME2005  0
#define cTIME2006  1
#define cTIME2012  2
#define cTIME2306  3
#define cTIME2307  4
#define cTIME2308  5
#define cTIME2500  6
#define cTIME2501  7
#define cTIME2502  8
#define cTIME2700  9

global ut_req_array_size = 9
global ut_requirement[0 .. ut_req_array_size]
                                                                                
FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

local cfe_requirements[0 .. ut_req_array_size] = ["cTIME2005", "cTIME2006", "cTIME2012", "cTIME2306", "cTIME2307","cTIME2308",  "cTIME2500", "cTIME2501", "cTIME2502", "cTIME2700"]

write ";**********************************************************************"
write "; Step 1.0: Test setup."
write ";**********************************************************************"
write "; Step 1.1: Perform a Power-On Reset of $CPU."
write ";**********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";**********************************************************************"
write "; Step 1.2: Load the Time Test Application "
write ";**********************************************************************"
;; Setup for the expected events
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_START_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_INIT_INF_EID, INFO, 2

s load_start_app ("TST_TIME", "$CPU")
wait 10

;; Check for expected events
if ($SC_$CPU_find_event[1].num_found_messages = 1 AND ;;
    $SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Started TST_TIME application."
endif

write ";**********************************************************************"
write "; Step 1.3: Add telemetry packets for Time Test App.                   "
write ";**********************************************************************"

LOCAL startStream = 0
LOCAL buffLimit = 4

/$SC_$CPU_TIME_REQUESTDIAG
wait 2

write " ; Add packet for TST_TIME_HK_TLM_MSG "

;; CPU1 is the default
startStream = x'913'

if ("$CPU" = "CPU2") then
  startStream = x'A13'
elseif ("$CPU" = "CPU3") then
  startStream = x'B13'
endif
                                                                                
for i = 1 to 16 do
  if (i > 1) then
    buffLimit = x'32'
  endif

  /$sc_$cpu_TO_ADDPACKET STREAM=startStream PKT_SIZE=x'0' PRIORITY=x'0' RELIABILITY=x'0' BUFLIMIT=buffLimit
  WAIT 2
  write " ; Add packet for stream ", startStream
  ;;; Increment the stream
  startStream = startStream + 1
enddo

/$SC_$CPU_TST_TIME_SEND_HK
wait 5

write ";**********************************************************************"
write "; Step 2.0:  Check default values after a Power-On Reset."
write ";**********************************************************************"

;; Write out the Default Values
write "Default Leap Seconds = ", CFE_TIME_DEF_LEAPS
write "Default STCF Seconds = ", CFE_TIME_DEF_STCF_SECS
write "Default STCF SubSeconds = ", CFE_TIME_DEF_STCF_SUBS
write "Default Clock State = ", TIME_API_STATE_DEFAULT
write "default Clock Set State Flag = ", TIME_CLOCK_SET_STATE_FLAG
write "default Clock Flywheel State Flag = ", TIME_CLOCK_FLYWHEEL_STATE_FLAG
write "default Time Server Flywheel State Flag = ", TIME_SERVER_FLYWHEEL_STATE_FLAG
write "default Command-to Flywheel State Flag = ", TIME_CMD_TO_FLYWHEEL_STATE_FLAG

;; Write out Telemetry Values
write "Leap Seconds (diag) = ", $SC_$CPU_TIME_DLEAPS
write "Leap Seconds (hk)   = ", $SC_$CPU_TIME_LEAPSecs
write "STCF Seconds (diag) = ", $SC_$CPU_TIME_DSTCFS
write "STCF Seconds (hk)   = ", $SC_$CPU_TIME_STCFSECS
write "TLM STCF SubSeconds (diag) = ", $SC_$CPU_TIME_DSTCFSS
write "TLM STCF SubSeconds (hk)   = ", $SC_$CPU_TIME_STCFSUBSECS
write "TLM Clock State (diag) = ", P@$SC_$CPU_TIME_DAPISTATE
write "TLM Clock State (hk)   = ", P@$SC_$CPU_TIME_APISTATE
write "TLM Clock Set State Flag (diag) = ", P@$SC_$CPU_TIME_DFLAGSET
write "TLM Clock Set State Flag (hk)   = ", P@$SC_$CPU_TIME_FLAGSET
write "TLM Clock Flywheel State Flag (diag) = ", P@$SC_$CPU_TIME_DFLAGFLY
write "TLM Clock Flywheel State Flag (hk)   = ", P@$SC_$CPU_TIME_FLAGFLY
write "TLM Time Serever Flywheel State Flag (diag) = ", P@$SC_$CPU_TIME_DFLAGSFLY
write "TLM Time Serever Flywheel State Flag (hk)   = ", P@$SC_$CPU_TIME_FLAGSFLY
write "TLM Command-to Flywheel State Flag (diag) = ", P@$SC_$CPU_TIME_DFLAGCFLY
write "TLM Command-to Flywheel State Flag (hk)   = ", P@$SC_$CPU_TIME_FLAGCFLY

IF ($SC_$CPU_TIME_DLEAPS = CFE_TIME_DEF_LEAPS) THEN
  write "<*> Passed (2500) - Leap Seconds set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Leap Seconds set to ",$SC_$CPU_TIME_DLEAPS, ";expected ",CFE_TIME_DEF_LEAPS
  ut_setrequirements cTIME2500, "F"
ENDIF

IF ($SC_$CPU_TIME_DSTCFS = CFE_TIME_DEF_STCF_SECS) THEN
  write "<*> Passed (2500) - STCF Seconds set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - STCF Seconds set to ",$SC_$CPU_TIME_DSTCFS, "; expected ",CFE_TIME_DEF_STCF_SECS
  ut_setrequirements cTIME2500, "F"
ENDIF

IF ($SC_$CPU_TIME_DSTCFSS = CFE_TIME_DEF_STCF_SUBS) THEN
  write "<*> Passed (2500) - STCF SubSeconds set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - STCF SubSeconds set to ",$SC_$CPU_TIME_DSTCFSS, "; expected ",CFE_TIME_DEF_STCF_SUBS
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DAPISTATE = TIME_API_STATE_DEFAULT) THEN
  write "<*> Passed (2500) - Clock State set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock State set to ",$SC_$CPU_TIME_DAPISTATE, "; expected ",TIME_API_STATE_DEFAULT
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGSET = TIME_CLOCK_SET_STATE_FLAG) THEN
  write "<*> Passed (2500) - Clock Set State Flag set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock Set State Flag set to ",P@$SC_$CPU_TIME_DFLAGSET, "; expected ",TIME_CLOCK_SET_STATE_FLAG
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGFLY = TIME_CLOCK_FLYWHEEL_STATE_FLAG) THEN
  write "<*> Passed (2500) - Clock Flywheel State Flag set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock Flywheel State Flag set to ",P@$SC_$CPU_TIME_DFLAGFLY, "; expected ",TIME_CLOCK_FLYWHEEL_STATE_FLAG
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGSFLY = TIME_SERVER_FLYWHEEL_STATE_FLAG) THEN
  write "<*> Passed (2500) - Time Server Flywheel State Flag set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Time Server Flywheel State Flag set to ",P@$SC_$CPU_TIME_DFLAGSFLY, "; expected ",TIME_SERVER_FLYWHEEL_STATE_FLAG
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGCFLY = TIME_CMD_TO_FLYWHEEL_STATE_FLAG) THEN
  write "<*> Passed (2500) - Command-to Flywheel State Flag set to Default."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Command-to Flywheel State Flag set to ",P@$SC_$CPU_TIME_DFLAGCFLY, "; expected ",TIME_CMD_TO_FLYWHEEL_STATE_FLAG
  ut_setrequirements cTIME2500, "F"
ENDIF

wait 5
 
write ";**********************************************************************"
write ";  Step 3.0:  Set clock state to a non-default value"  
write ";**********************************************************************"
write ";  Step 3.1:  Request the value of the clock state before the change"  
write ";  "
write ";  Requirement cTIME 2308 "
write ";**********************************************************************"

write "Write out the current value of the Clock State prior to changing it "
write "The current value of Clock State is ", P@$SC_$CPU_TIME_DAPISTATE
write "The Current value of Clock Set State Flag = ", P@$SC_$CPU_TIME_DFLAGSET
write "The Current value of Clock Flywheel State Flag = ", P@$SC_$CPU_TIME_DFLAGFLY
write "The Current value of Time Server Flywheel State Flag = ", P@$SC_$CPU_TIME_DFLAGSFLY
write "The Current value of Command-to Flywheel State = ", P@$SC_$CPU_TIME_DFLAGCFLY

;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_CKLSTATE_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TST_TIME_CLKSTATE

write "Request $SC_$CPU_TST_CLKSTATE sent at ", CMD_TIME_1
wait 5 

write "Current Clock State is $SC_$CPU_TIME_DAPISTATE = ", P@$SC_$CPU_Time_DAPIState
write "Returned Clock State is $SC_$CPU_TT_CLKST = ", P@$SC_$CPU_TT_CLKST

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2308) - Expected event msg received."
  ut_setrequirements cTIME2308, "P"
ELSE
  write "<!> Failed (2308) - Expected Event message ",TST_TIME_CKLSTATE_INF_EID, " not received."
  ut_setrequirements cTIME2308, "F"
endif

IF ($SC_$CPU_TT_CLKST = $SC_$CPU_Time_DAPIState) THEN
  write "<*> Passed (2308) - Requested Clock State is correct."
  ut_setrequirements cTIME2308, "P"
ELSE
  write "<!> Failed (2308) - Current Clock State set to ",P@$SC_$CPU_TIME_DAPISTATE, "; TST_TIME returned ",P@$SC_$CPU_TT_CLKST
  ut_setrequirements cTIME2308, "F"
ENDIF

write ";**********************************************************************"
write ";  Step 3.2:  Change the Clock State to VALID"  
write ";  "
write ";  Requirement cTIME 2500"
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STATE_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TIME_SetState VALID
write "Command $SC_$CPU_TIME_SETSTATE = VALID sent at ", CMD_TIME_1
wait 10

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2012) - Expected event msg received."
  ut_setrequirements cTIME2012, "P"
ELSE
  write "<!> Failed (2012) - Expected Event message ",TST_TIME_CKLSTATE_INF_EID, " not received."
  ut_setrequirements cTIME2012, "F"
endif

write "Modified Clock State is $SC_$CPU_TIME_DAPISTATE = ", P@$SC_$CPU_Time_DAPIState
write "Modified Clock Set State Flag = ", P@$SC_$CPU_TIME_DFLAGSET
write "Modified Clock Flywheel State Flag = ", P@$SC_$CPU_TIME_DFLAGFLY
write "Modified Time Server Flywheel State Flag = ", P@$SC_$CPU_TIME_DFLAGSFLY
write "Modified Command-to Flywheel State = ", P@$SC_$CPU_TIME_DFLAGCFLY

IF (P@$SC_$CPU_Time_DAPIState = "VALID") THEN
  write "<*> Passed (2500) - Clock State indicates VALID as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Set Clock State is not set to VALID."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGSET = "SET") THEN
  write "<*> Passed (2500) - Clock Set State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock Set State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGFLY = "NOFLY") THEN
  write "<*> Passed (2500) - Clock Flywheel State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock Flywheel State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGSFLY = "NOFLY") THEN
  write "<*> Passed (2500) - Time Server Flywheel State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Time Server Flywheel State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGCFLY = "NO") THEN
  write "<*> Passed (2500) - Command-to Flywheel State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Command-to Flywheel State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 3.3:  Change the Clock State to FLYWHEEL"  
write ";  "
write ";  Requirement cTIME 2500, 2502"
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STATE_EID, INFO, 1

/$SC_$CPU_TIME_SetState FLYWHEEL
write "Command $SC_$CPU_TIME_SETSTATE = FLYWHEEL sent at ", CMD_TIME_1
wait 10

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2012) - Expected event msg received."
  ut_setrequirements cTIME2012, "P"
ELSE
  write "<!> Failed (2012) - Expected Event message ",CFE_TIME_STATE_EID, " not received."
  ut_setrequirements cTIME2012, "F"
endif

write "Modified Clock State is $SC_$CPU_TIME_DAPISTATE = ", P@$SC_$CPU_Time_DAPIState
write "Modified Clock Set State Flag = ", P@$SC_$CPU_TIME_DFLAGSET
write "Modified Clock Flywheel State Flag = ", P@$SC_$CPU_TIME_DFLAGFLY
write "Modified Time Server Flywheel State Flag = ", P@$SC_$CPU_TIME_DFLAGSFLY
write "Modified Command-to Flywheel State = ", P@$SC_$CPU_TIME_DFLAGCFLY

IF (P@$SC_$CPU_Time_DAPIState = "FLYWHEEL") THEN
  write "<*> Passed (2500) - Clock State indicates FLYWHEEL as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Set Clock State is not set to VALID."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGSET = "SET") THEN
  write "<*> Passed (2500) - Clock Set State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock Set State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGFLY = "FLY") THEN
  write "<*> Passed (2500) - Clock Flywheel State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Clock Flywheel State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGSFLY = "FLY") THEN
  write "<*> Passed (2500) - Time Server Flywheel State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Time Server Flywheel State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

IF (P@$SC_$CPU_TIME_DFLAGCFLY = "YES") THEN
  write "<*> Passed (2500) - Command-to Flywheel State Flag set as expected."
  ut_setrequirements cTIME2500, "P"
ELSE
  write "<!> Failed (2500) - Command-to Flywheel State Flag is not set correctly."
  ut_setrequirements cTIME2500, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 3.4:  Request the new value of the clock state to verify change"
write ";  "
write ";  Requirement cTIME 2308 "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_CKLSTATE_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TST_TIME_CLKSTATE

write "Request $SC_$CPU_TST_CLKSTATE sent at ", CMD_TIME_1
wait 5 

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2308) - Expected event msg received."
  ut_setrequirements cTIME2308, "P"
ELSE
  write "<!> Failed (2308) - Expected Event message ",TST_TIME_CKLSTATE_INF_EID, " not received."
  ut_setrequirements cTIME2308, "F"
endif

write "Current Clock State is $SC_$CPU_TIME_DAPISTATE = ", P@$SC_$CPU_Time_DAPIState
write "Returned Clock State is $SC_$CPU_TT_CLKST = ", P@$SC_$CPU_TT_CLKST

IF ($SC_$CPU_TT_CLKST  = $SC_$CPU_Time_DAPIState) THEN
  write "<*> Passed (2308) - Requested Clock State is correct."
  ut_setrequirements cTIME2308, "P"
ELSE
  write "<!> Failed (2308) - Current Clock State set to ",P@$SC_$CPU_TIME_DAPISTATE, "; TST_TIME returned ",P@$SC_$CPU_TT_CLKST
  ut_setrequirements cTIME2308, "F"
ENDIF

write ";**********************************************************************"
write ";  Step 4.0: Set STCF to a non default value " 
write ";  "
write ";  Requirement cTIME 2006; 2306"
write ";**********************************************************************"
write ";  Step 4.1: Use the test application to request the current STCF."
write ";  "
write ";  Requirement cTIME 2306"
write ";**********************************************************************"
write "Write out the current value of the STCF prior to changing it "
write "Current value of STCF Seconds is = ", $SC_$CPU_Time_DSTCFS
write "Current value of STCF SubSeconds is = ", $SC_$CPU_Time_DSTCFSS

;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_STCF_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TST_TIME_STCF

Time_DSTCFSeconds = $SC_$CPU_Time_DSTCFS
Time_DSTCFSSeconds = $SC_$CPU_Time_DSTCFSS
wait 5

write "Request for $SC_$CPU_TST_TIME_STCF sent at ", CMD_TIME_1

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2306) - Expected event msg received."
  ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2306) - Expected Event message ",TST_TIME_CKLSTATE_INF_EID, " not received."
  ut_setrequirements cTIME2306, "F"
endif

write "Time Test app returned value for $SC_$CPU_TT_STCF_SECS = ",$SC_$CPU_TT_STCF_SECS
write "TIME value of $SC_$CPU_Time_DSTCFSeconds = ", Time_DSTCFSeconds 
write "Time Test app returned value for $SC_$CPU_TT_STCF_SUBS = ",$SC_$CPU_TT_STCF_SUBS
write "TIME value of $SC_$CPU_Time_DSTCFSSeconds = ", Time_DSTCFSSeconds 

IF ($SC_$CPU_TT_STCF_SECS < Time_DSTCFSeconds) THEN
  DELTA_Seconds = Time_DSTCFSeconds - $SC_$CPU_TT_STCF_SECS
ELSE
  DELTA_Seconds = $SC_$CPU_TT_STCF_SECS - Time_DSTCFSeconds
ENDIF

IF (DELTA_Seconds > DELTA_MAX) THEN
  write "<!> Failed (2306) - Requested STCF Time Seconds Delta is not in bounds."
  ut_setrequirements cTIME2306, "F"
ELSE
  write "<*> Passed (2306) - Requested STCF Time Seconds Delta is in bounds."
  ut_setrequirements cTIME2306, "P"
ENDIF

IF ($SC_$CPU_TT_STCF_SUBS < Time_DSTCFSSeconds) THEN
  DELTA_SubSeconds = Time_DSTCFSSeconds - $SC_$CPU_TT_STCF_SUBS
ELSE
  DELTA_SubSeconds = $SC_$CPU_TT_STCF_SUBS - Time_DSTCFSSeconds
ENDIF

IF (DELTA_SubSeconds > DELTA_MAX) THEN
  write "<!> Failed (2306) - Requested STCF Time Sub Seconds Delta is not in bounds."
  ut_setrequirements cTIME2306, "F"
ELSE
  write "<*> Passed (2306) - Requested STCF Time Sub Seconds Delta is in bounds."
  ut_setrequirements cTIME2306, "P"
ENDIF

write "The Delta seconds value is ", DELTA_Seconds
write "The Delta sub seconds value is ", DELTA_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 4.2: Command a new STCF value."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TIME_SetClockSTCF SECONDS=5000 MICROSECS=50000

write "Command to set $SC_$CPU_TIME_SetClockSTCF SECONDS = 5000 SUBSECONDS = 50000 to at ", CMD_TIME_1
wait 5

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2006) - Expected event msg received."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - Expected Event message ",CFE_TIME_STCF_EID, " not received."
  ut_setrequirements cTIME2006, "F"
endif

write "Modified value of STCF Seconds is = ", $SC_$CPU_Time_DSTCFS
write "Modified value of STCF SubSeconds is = ", $SC_$CPU_Time_DSTCFSS

IF ($SC_$CPU_Time_DSTCFS = 5000) THEN
  write "<*> Passed (2006) - Modification of STCF Seconds was successful"
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - Modification of STCF Seconds failed"
  ut_setrequirements cTIME2006, "F"
ENDIF

IF ($SC_$CPU_Time_DSTCFSS = x'CCCC000') THEN
  write "<*> Passed (2006) - Modification of STCF Sub Seconds was successful"
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - Modification of STCF Sub Seconds failed"
  ut_setrequirements cTIME2006, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 4.3: Use the test application to request the current STCF."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_STCF_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TST_TIME_STCF
wait 5
Time_DSTCFSeconds = $SC_$CPU_Time_DSTCFS
Time_DSTCFSSeconds = $SC_$CPU_Time_DSTCFSS

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2306) - Expected event msg received."
  ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2306) - Expected Event message ",TST_TIME_CKLSTATE_INF_EID, " not received."
  ut_setrequirements cTIME2306, "F"
endif

write "Request for $SC_$CPU_TST_TIME_STCF sent at ", CMD_TIME_1
write "Time Test app returned value for $SC_$CPU_TT_STCF_SECS = ",$SC_$CPU_TT_STCF_SECS
write "TIME value of $SC_$CPU_Time_DSTCFSeconds = ", Time_DSTCFSeconds 
write "Time Test app returned value for $SC_$CPU_TT_STCF_SUBS = ",$SC_$CPU_TT_STCF_SUBS
write "TIME value of $SC_$CPU_Time_DSTCFSSeconds = ", Time_DSTCFSSeconds 

IF ($SC_$CPU_TT_STCF_SECS < Time_DSTCFSeconds) THEN
  DELTA_Seconds = Time_DSTCFSeconds - $SC_$CPU_TT_STCF_SECS
ELSE
  DELTA_Seconds = $SC_$CPU_TT_STCF_SECS - Time_DSTCFSeconds
ENDIF

IF (DELTA_Seconds > DELTA_MAX) THEN
  write "<!> Failed (2306) - Requested STCF Time Seconds Delta is not in bounds."
  ut_setrequirements cTIME2306, "F"
ELSE
  write "<*> Passed (2306) - Requested STCF Time Seconds Delta is in bounds."
  ut_setrequirements cTIME2306, "P"
ENDIF

IF ($SC_$CPU_TT_STCF_SUBS < Time_DSTCFSSeconds) THEN
  DELTA_SubSeconds = Time_DSTCFSSeconds - $SC_$CPU_TT_STCF_SUBS
ELSE
  DELTA_SubSeconds = $SC_$CPU_TT_STCF_SUBS - Time_DSTCFSSeconds
ENDIF

IF (DELTA_SubSeconds > DELTA_MAX) THEN
  write "<!> Failed (2306) - Requested STCF Time Sub Seconds Delta is not in bounds."
  ut_setrequirements cTIME2306, "F"
ELSE
  write "<*> Passed (2306) - Requested STCF Time Sub Seconds Delta is in bounds."
  ut_setrequirements cTIME2306, "P"
ENDIF

write "The Delta seconds value is ", DELTA_Seconds
write "The Delta sub seconds value is ", DELTA_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 5.0: Set Leap Seconds to a non-default value "
write ";**********************************************************************"
write "; Step 5.1: Request the current Leap Seconds from TST_TIME."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_LEAPSEC_INF_EID, INFO, 1

write "Write out the current value of the Leap Seconds prior to changing it "
write "Current value of Leap Seconds is = ", $SC_$CPU_TIME_DLEAPS

CMD_TIME_1 =%GMT

/$SC_$CPU_TST_TIME_LEAPSEC

write "Command $SC_$CPU_TST_TIME_LEAPSEC sent at ", CMD_TIME_1
wait 5

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2307) - Expected event msg received."
  ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - Expected Event message ",TST_TIME_LEAPSEC_INF_EID, " not received."
  ut_setrequirements cTIME2307, "F"
endif

write "The TIME Leap seconds from telemetry = ", $SC_$CPU_Time_DLeapS
write "The Leap Seconds value from the Time Test APP = ", $SC_$CPU_TT_LPSEC_SECS

wait 5

write ";**********************************************************************"
write ";  Step 5.2: Change the Leap Seconds."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_LEAPS_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TIME_SetClockLeap LEAPS=500

write "Command $SC_$CPU_TIME_SetClockLeap = to 500 at ", CMD_TIME_1
wait 5

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2005) - Expected event msg received."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2005) - Expected Event message ",TST_TIME_CKLSTATE_INF_EID, " not received."
  ut_setrequirements cTIME2005, "F"
endif

write "TIME value of $SC_$CPU_Time_DLeapS = ", $SC_$CPU_TIME_DLEAPS

IF ($SC_$CPU_Time_DLeapS = 500) THEN
  write "<*> Passed (2005) - Commanded Leap Seconds set correctly."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2005) - Commanded Leap Seconds."
  ut_setrequirements cTIME2005, "F"
ENDIF
wait 5

write ";**********************************************************************"
write ";  Step 5.3: Request the current Leap Seconds from TST_TIME."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_LEAPSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$SC_$CPU_TST_TIME_LEAPSEC

write "Command $SC_$CPU_TST_TIME_LEAPSEC sent at ", CMD_TIME_1
wait 5

;; Verify the receipt of the expected event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2307) - Expected event msg received."
  ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - Expected Event message ", TST_TIME_LEAPSEC_INF_EID, " not received."
  ut_setrequirements cTIME2307, "F"
endif

write "The TIME Leap seconds from telemetry = ", $SC_$CPU_Time_DLeapS
write "The Leap Seconds value from the Time Test APP = ", $SC_$CPU_TT_LPSEC_SECS

IF ($SC_$CPU_TT_LPSEC_SECS = $SC_$CPU_Time_DLeapS) THEN
   write "Requested Leap seconds equals tlm value"
ELSE
   write "Requested Leap seconds not equal to tlm value"
ENDIF

wait 5

;;;;; Print the Time Status Data, STCF, and LeapSeconds in order to
;;;;; verify they are preserved on a Processor Reset.
Time_DLeapSeconds = $SC_$CPU_Time_DLeapS
Time_DSTCFSeconds = $SC_$CPU_Time_DSTCFS
Time_DSTCFSSeconds = $SC_$CPU_Time_DSTCFSS
Time_DMETSeconds = $SC_$CPU_Time_DMETS
Time_DMETSSeconds = $SC_$CPU_Time_DMETSS
Time_DelaySeconds = $SC_$CPU_Time_DLatentS
Time_DelaySSeconds = $SC_$CPU_Time_DLatentSS
Time_ClockSignal = P@$sc_$cpu_Time_DFlagPri

write "Clock Signal before reset = ", Time_ClockSignal
write "Leap seconds before reset = ", Time_DLeapSeconds
write "STCF Seconds before reset = ", Time_DSTCFSeconds
write "STCF SubSecs before reset = ", Time_DSTCFSSeconds
write "MET Seconds before reset = ", Time_DMETSeconds
write "MET SubSecs before reset = ", Time_DMETSSeconds
write "Delay Seconds before reset = ", Time_DelaySeconds
write "Delay SubSecs before reset = ", Time_DelaySSeconds

write ";**********************************************************************"
write ";  Step 6.0: Command a Processor Reset"
write ";**********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";**********************************************************************"
write ";  Step 6.1: Verify Time Status Data, STCF and leap seconds reflect the"
write ";            preserved values and not default values. If any of these "
write "; 	    values fail verification, the <MISSION_DEFINED> default "
write "; 	    value should be set. "
write "; "
write ";  Requirement # cTIME 2501 & 2502"
write ";**********************************************************************"
local New_DLeapSeconds = $SC_$CPU_Time_DLeapS
local New_DSTCFSeconds = $SC_$CPU_Time_DSTCFS
local New_DSTCFSSeconds = $SC_$CPU_Time_DSTCFSS
local New_DMETSeconds = $SC_$CPU_Time_DMETS
local New_DMETSSeconds = $SC_$CPU_Time_DMETSS
local New_DelaySeconds = $SC_$CPU_Time_DLatentS
local New_DelaySSeconds = $SC_$CPU_Time_DLatentSS
local New_ClockSignal = P@$sc_$cpu_Time_DFlagPri

write "Clock Signal after reset = ", New_ClockSignal
write "Leap seconds after reset = ", New_DLeapSeconds
write "STCF Seconds after reset = ", New_DSTCFSeconds
write "STCF SubSecs after reset = ", New_DSTCFSSeconds
write "MET Seconds after reset = ", New_DMETSeconds
write "MET SubSecs after reset = ", New_DMETSSeconds
write "Delay Seconds after reset = ", New_DelaySeconds
write "Delay SubSecs after reset = ", New_DelaySSeconds

IF (Time_DLeapSeconds = New_DLeapSeconds) THEN
  write "<*> Passed (2501;2700) - Leap Seconds preserved."
  ut_setrequirements cTIME2501, "P"
  ut_setrequirements cTIME2700, "P"
ELSE
  write "<!> Failed (2501;2700) - Leap Seconds not equal to value BEFORE reset."
  ut_setrequirements cTIME2501, "F"
  ut_setrequirements cTIME2700, "F"
ENDIF

IF (Time_DSTCFSeconds = New_DSTCFSeconds) THEN
  write "<*> Passed (2501;2700) - STCF Seconds preserved."
  ut_setrequirements cTIME2501, "P"
  ut_setrequirements cTIME2700, "P"
ELSE
  write "<!> Failed (2501;2700) - STCF Seconds not equal to value BEFORE reset."
  ut_setrequirements cTIME2501, "F"
  ut_setrequirements cTIME2700, "F"
ENDIF

IF (Time_DMETSeconds <= New_DMETSeconds) THEN
  write "<*> Passed (2501;2700) - MET Seconds preserved."
  ut_setrequirements cTIME2501, "P"
  ut_setrequirements cTIME2700, "P"
ELSE
  write "<!> Failed (2501;2700) - MET Seconds not equal to value BEFORE reset."
  ut_setrequirements cTIME2501, "F"
  ut_setrequirements cTIME2700, "F"
ENDIF

IF (Time_DelaySeconds = New_DelaySeconds) THEN
  write "<*> Passed (2501;2700) - Delay Seconds preserved."
  ut_setrequirements cTIME2501, "P"
  ut_setrequirements cTIME2700, "P"
ELSE
  write "<!> Failed (2501;2700) - Delay Seconds not equal to value BEFORE reset."
  ut_setrequirements cTIME2501, "F"
  ut_setrequirements cTIME2700, "F"
ENDIF

IF (Time_ClockSignal = New_ClockSignal) THEN
  write "<*> Passed (2501;2700) - Clock Signal preserved."
  ut_setrequirements cTIME2501, "P"
  ut_setrequirements cTIME2700, "P"
ELSE
  write "<!> Failed (2501;2700) - Clock Signal not equal to value BEFORE reset."
  ut_setrequirements cTIME2501, "F"
  ut_setrequirements cTIME2700, "F"
ENDIF

;;; Verify the other HK Telemetry is set to the DEFAULT
IF (P@$SC_$CPU_TIME_APISTATE = TIME_API_STATE_DEFAULT) THEN
  write "<*> Passed (2502) - Clock State set to Default."
  ut_setrequirements cTIME2502, "P"
ELSE
  write "<!> Failed (2502) - Clock State set to ",$SC_$CPU_TIME_APISTATE, "; expected ",TIME_API_STATE_DEFAULT
  ut_setrequirements cTIME2502, "F"
ENDIF

IF (P@$SC_$CPU_TIME_FLAGSET = TIME_CLOCK_SET_STATE_FLAG) THEN
  write "<*> Passed (2502) - Clock Set State Flag set to Default."
  ut_setrequirements cTIME2502, "P"
ELSE
  write "<!> Failed (2502) - Clock Set State Flag set to ",P@$SC_$CPU_TIME_FLAGSET, "; expected ",TIME_CLOCK_SET_STATE_FLAG
  ut_setrequirements cTIME2502, "F"
ENDIF

IF (P@$SC_$CPU_TIME_FLAGFLY = TIME_CLOCK_FLYWHEEL_STATE_FLAG) THEN
  write "<*> Passed (2502) - Clock Flywheel State Flag set to Default."
  ut_setrequirements cTIME2502, "P"
ELSE
  write "<!> Failed (2502) - Clock Flywheel State Flag set to ",P@$SC_$CPU_TIME_FLAGFLY, "; expected ",TIME_CLOCK_FLYWHEEL_STATE_FLAG
  ut_setrequirements cTIME2502, "F"
ENDIF

IF (P@$SC_$CPU_TIME_FLAGSFLY = TIME_SERVER_FLYWHEEL_STATE_FLAG) THEN
  write "<*> Passed (2502) - Time Server Flywheel State Flag set to Default."
  ut_setrequirements cTIME2502, "P"
ELSE
  write "<!> Failed (2502) - Time Server Flywheel State Flag set to ",P@$SC_$CPU_TIME_FLAGSFLY, "; expected ",TIME_SERVER_FLYWHEEL_STATE_FLAG
  ut_setrequirements cTIME2502, "F"
ENDIF

IF (P@$SC_$CPU_TIME_FLAGCFLY = TIME_CMD_TO_FLYWHEEL_STATE_FLAG) THEN
  write "<*> Passed (2502) - Command-to Flywheel State Flag set to Default."
  ut_setrequirements cTIME2502, "P"
ELSE
  write "<!> Failed (2500) - Command-to Flywheel State Flag set to ",P@$SC_$CPU_TIME_FLAGCFLY, "; expected ",TIME_CMD_TO_FLYWHEEL_STATE_FLAG
  ut_setrequirements cTIME2500, "F"
ENDIF

write "**** Requirements Status Reporting"
                                                                                
write
write "Requirement(s) Report"
write "---------------------"
                                                                                
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
                                                                                
ENDDO
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
