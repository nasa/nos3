PROC $sc_$cpu_time_command_server_tai
;**********************************************************************
;  Test Name:  $sc_$cpu_time_command_server_tai
;  Test Level: Build Verification 
;  Test Type:  Functional
; 
;  Test Description
;	The purpose of this test is to verify the ground commands, telemetry, 
;  and current time and clock state requests for the Core Flight Execuive 
;  (cFE) Time Services (TIME). 
;
;  Requirements Tested
;       Based on cFE SRS Ver 1.2 Apr 14, 2005
;
;         cTime 2000, cTIME 2001, cTIME 2002, cTIME 2003, cTIME 2004, 
;	  cTIME 2005, cTIME 2006, cTIME 2007, cTIME 2008, cTIME 2009, 
;         cTIME 2010, cTIME 2300, cTIME 2301, cTIME 2302, cTIME 2303,
;	  cTIME 2304, cTIME 2305, cTIME 2306, cTIME 2307, cTIME 2309,
;	  cTIME 2310, cTIME 2311, cTIME 2312, cTIME 2313
;
;
;  Prerequisite Conditions
;       Time is configured with TAI as the default time on the server. Time
;       is configured as the server on the CPU being tested. 
;       Configuration of clock source and default values of various parameters
;       are contained in the configuration file cfe_platform_cfg.h and is 
;	referenced by this procedure.	
;
;  Assumptions and Constraints
;       For the current configuration the command to set the clock source to INTERNAL/EXTERNAL
;       is invalid. The test step 2.3 will be commented out.
;
;
;  Change History
;	Date		   Name		Description
;	05/25/05        J. Heavener	Original Procedure from
;                                       scenario.
;       06/17/05        J. Heavener     Complete updates to run in cfe lab.
;
;       06/22/05        J. Heavener     Updates due to problems in first run, 
;                                       change proc name.
;       07/01/05        J. Heavener     Convert to CPU2 from CPU1.
;       07/08/05        J. Heavener     Update to check for acceptable range 
;                                       when testing test app returned values.
;       07/11/05        J. Heavener     Add checking STCF adjustment telemetry
;                                       to steps 7, 8, and 9.
;       08/08/05        J. Heavener     Incorporate changes from review.
;       10/04/05        J. Heavener     Incorporate changes from test results
;					review.
;       11/02/05        J. Heavener	Additional changes from review.
;       11/14/05        J. Heavener     Produce template from cpu2 version.
;       11/17/05        J. Heavener     Incorporate changes to correct problems
;					found in review. Steps 7.1, 7.2, 10.1.1,
;					10.2.1, & 13.
;       01/03/06        J. Heavener     Make changes necessary for build 3.1
;       01/27/06        J. Heavener     Additional changes from results review.
;       02/02/06        J. Heavener     Change Step 3 test, command to Set 
;					Signal not valid in current 
;					configuration.
;       03/07/06        J. Heavener     Change checking scheme per test results
;					review.
;	10/10/06	W. Moleski	Changed the proc into a template.
;	12/05/06	W. Moleski	Changed the expected subseconds returned
;					to be 'FFFFF000' not 'FFFFE000'.
;	03/01/07	W. Moleski	Updated for cFE 4.0
;	08/13/09	W. Moleski	Modified the xxx1HZSTCF commands to take
;					Sub-seconds rather than MicroSeconds as
;					an argument.
;	12/06/10	W. Moleski	Added tests for cTIME 2000
;	02/08/12	W. Moleski	Replaced ut_setupevt with ut_setupevents
;
;  Arguments
;	 None 
;
;
;  Procedures Called
;      ut_pfindicate       Directive to print the pass fail status of a 
;                          particular requirement number.
;      ut_setupevents      Directive to look for a particular event & increment
;			   a value in the CVT to indicate receipt.  
;      load_start_app      Procedure to load and start a user application from 
;                          the /s/opr/accounts/cfebx/apps/cpux directory.
;      ut_setrequirements  Directive to status cfe requirements.
; 
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "cfe_time_events.h"
#include "tst_time_events.h"

%liv (log_procedure) = logging

;**********************************************************************
;  Define variables
;**********************************************************************

#define TRUE 1
#define FALSE 0

LOCAL RUN_TIME
LOCAL START_TIME
LOCAL CMD_TIME_1
LOCAL CMD_TIME_2
LOCAL CLOCKSTATE_DEF
LOCAL TOTALSTCF_DEF
LOCAL TOTALSTCF_SUBS_DEF
LOCAL NEW_VLD_CMD_CNT
LOCAL NEW_INVAL_CMD_CNT
LOCAL INVLD_CMD_CNT
LOCAL DELTA_Seconds = 0
LOCAL DELTA_SubSeconds = 0
LOCAL MET_DELTA_Seconds = 0
LOCAL MET_DELTA_SubSeconds = 0
LOCAL rawCmd
LOCAL New_DMETSeconds
LOCAL New_DMETSSeconds
LOCAL New_DSTCFSeconds
LOCAL New_DSTCFSSeconds
LOCAL New_DTAISeconds
LOCAL New_DTAISSeconds
LOCAL New_DLeapSeconds
LOCAL New_DUTCSeconds
LOCAL New_DUTCSSeconds
LOCAL Expected_Subseconds
LOCAL Expected_Seconds

LOCAL TIME_LEAP_SECS_DEFAULT
LOCAL TIME_STCF_SECS_DEFAULT
LOCAL TIME_STCF_SUBS_DEFAULT
LOCAL TIME_API_STATE_DEFAULT
LOCAL TIME_CLOCK_STATE_DEFAULT
LOCAL TIME_CLOCK_SET_STATE_FLAG
LOCAL TIME_CLOCK_FLYWHEEL_STATE_FLAG
LOCAL TIME_SERVER_FLYWHEEL_STATE_FLAG
LOCAL TIME_CMD_TO_FLYWHEEL_STATE_FLAG

; Local variables used in step 3 and 4 testing

LOCAL Time_DMETSeconds
LOCAL Time_DMETSSeconds
LOCAL Time_DSTCFSeconds
LOCAL Time_DSTCFSSeconds
LOCAL Time_DTAISeconds
LOCAL Time_DTAISSeconds
LOCAL Time_DLeapSeconds
LOCAL Time_DUTCSeconds
LOCAL Time_DUTCSSeconds

LOCAL Compute_Time_DSTCFSeconds
LOCAL Compute_Time_DSTCFSSeconds
LOCAL Compute_Time_DTAISeconds
LOCAL Compute_Time_DTAISSeconds
LOCAL Compute_Time_DUTCSeconds
LOCAL Compute_Time_DUTCSSeconds

LOCAL MAX_TIME = 4294967295               ; x'FFFFFFFF'
LOCAL MAX_MICRO_2_SSEC = 4294963200       ; x'FFFFF000'
LOCAL MAX_LEAP_SEC = 32767
LOCAL MIN_LEAP_SEC = -32768
LOCAL MAX_MICROSEC = 999999

LOCAL ADD_SECONDS_RESULT = 16796876 
LOCAL ADD_SUBSECONDS_RESULT = 536870912

LOCAL ONE_MICROSEC_AS_SSEC = 4096

#define cTIME2000  0
#define cTIME2001  1
#define cTIME2002  2
#define cTIME2003  3
#define cTIME2004  4
#define cTIME2005  5
#define cTIME2006  6
#define cTIME2007  7
#define cTIME2008  8
#define cTIME2009  9
#define cTIME2010  10
#define cTIME2011  11
#define cTIME2012  12
#define cTIME20121 13
#define cTIME2013  14
#define cTIME2014  15
#define cTIME2300  16
#define cTIME2301  17
#define cTIME2302  18
#define cTIME2303  19
#define cTIME2304  20
#define cTIME2305  21
#define cTIME2306  22
#define cTIME2307  23
#define cTIME2309  24
#define cTIME2310  25
#define cTIME2311  26
#define cTIME2312  27
#define cTIME2313  28
#define cTIME2314  29

global ut_req_array_size = 29
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

; The following define is not in the configuration file yet.
#define INTERN_FLAG TRUE
#ifdef CFE_TIME_CFG_DEFAULT_TAI
#define CFG_DEFAULT_TAI TRUE
#define CFG_DEFAULT_UTC FALSE
#else
#define CFG_DEFAULT_TAI FALSE
#define CFG_DEFAULT_UTC TRUE
#endif

TIME_LEAP_SECS_DEFAULT = 32
TIME_STCF_SECS_DEFAULT = 1000000
TIME_STCF_SUBS_DEFAULT = 0
TIME_CLOCK_STATE_DEFAULT = -1
TIME_CLOCK_SET_STATE_FLAG = "NOT SET"
TIME_CLOCK_FLYWHEEL_STATE_FLAG = "NOFLY"
TIME_SERVER_FLYWHEEL_STATE_FLAG = "NOFLY"
TIME_CMD_TO_FLYWHEEL_STATE_FLAG = "NO"

local cfe_requirements[0 .. ut_req_array_size] = ["cTIME2000", "cTIME2001", "cTIME2002", "cTIME2003", "cTIME2004", "cTIME2005", "cTIME2006", "cTIME2007", "cTIME2008", "cTIME2009", "cTIME2010", "cTIME2011", "cTIME2012", "cTIME2012.1", "cTIME2013", "cTIME2014", "cTIME2300", "cTIME2301", "cTIME2302", "cTIME2303", "cTIME2304", "cTIME2305", "cTIME2306", "cTIME2307", "cTIME2309", "cTIME2310", "cTIME2311", "cTIME2312", "cTIME2313", "cTIME2314"]

write ";**********************************************************************"
write "Starting a sequential print to capture the time"
write ";**********************************************************************"

seq print on ;;
     p@$sc_$cpu_TIME_FLAG1HZD ;;
     $sc_$cpu_TIME_METSECS using %8x ;;
     $sc_$cpu_TIME_METSUBSECS using %8x ;;
     $sc_$cpu_TIME_STCFSECS using %8x  ;;
     $sc_$cpu_TIME_STCFSUBSECS using %8x ;;
     $sc_$cpu_TIME_1HZADJSECS using %8x ;;
     $sc_$cpu_TIME_1HZADJSSECS using %8x to cfe_$cpu_time_cmd_srvr_tai.out


write ";**********************************************************************"
write ";  Step 1.0: Time Command Tests. "
write ";            These tests are done before starting the TST_TIME app "
write ";            in order to verify the command and error counters."
write ";            The TST_TIME app issues commands to request the HK and "
write ";            Diagnostic information which makes it difficult to verify"
write ";            exact values of the counters."
write ";**********************************************************************"
write ";  Step 1.1: Test the NOOP command. "
write ";"
write ";  Requirement cTIME 2001, cTIME 2003"
write ";**********************************************************************"

NEW_VLD_CMD_CNT = $sc_$cpu_TIME_CMDPC + 1
INVLD_CMD_CNT = $sc_$cpu_TIME_CMDEC

write "Value of TIME Command Processed Counter is ",$sc_$cpu_TIME_CMDPC
write "Value of TIME Command Error Counter is ",$sc_$cpu_TIME_CMDEC
write "Value of LOCAL NEW_VLD_CMD_CNT is ",NEW_VLD_CMD_CNT

ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1

/$sc_$cpu_TIME_NOOP
wait 5

ut_tlmwait $sc_$cpu_TIME_CMDPC, {NEW_VLD_CMD_CNT}
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (2001;2003) - NO-OP Command succeeded."
   ut_setrequirements cTIME2001, "P" 
   ut_setrequirements cTIME2003, "P"
else
  write "<!> Failed (2001;2003) - NO-OP Command did not increment CMDPC."
   ut_setrequirements cTIME2001, "F"
   ut_setrequirements cTIME2003, "F"
endif

write "Value of TIME Command Processed Counter is ",$sc_$cpu_TIME_CMDPC
write "Value of TIME Command Error Counter is ",$sc_$cpu_TIME_CMDEC
write "Value of LOCAL NEW_VLD_CMD_CNT is ",NEW_VLD_CMD_CNT

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN 
  write "<*> Passed (2001;2003) - Event Message found "
  ut_setrequirements cTIME2001, "P" 
  ut_setrequirements cTIME2003, "P"
ELSE
  write "<!> Failed (2001;2003) - Expected event message ", CFE_TIME_NOOP_EID, " not received."
  ut_setrequirements cTIME2001, "F"
  ut_setrequirements cTIME2003, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 1.2: Send an invalid command to Test the command error counter"
write "; "
write ";  Requirement cTIME 2004"
write ";**********************************************************************"

NEW_INVAL_CMD_CNT = $sc_$cpu_TIME_CMDEC + 2

write "Value of TIME Command Error Counter is ",$sc_$cpu_TIME_CMDEC
write "Value of LOCAL NEW_INVAL_CMD_CNT is ",NEW_INVAL_CMD_CNT

write " Send a raw command incorrectly which will cause the error count to increment "
write " The raw command used is Set Clock Source with an invalid source command"

ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SOURCE_ERR_EID, ERROR, 1

rawCmd = ""
;; CPU1 is the default
rawCmd = "1805C000000303600005"

if ("$CPU" = "CPU2") then
  rawCmd = "1825C000000303600005"
elseif ("$CPU" = "CPU3") then
  rawCmd = "1845C000000303600005"
endif

ut_sendrawcmd "$sc_$cpu_TIME", {rawCmd}
wait 3

ut_sendrawcmd "$sc_$cpu_TIME", {rawCmd}
IF (ut_rcs_status <> UT_RCS_CmdFailure) THEN
  write "<!> Failed (2004) - Command succeeded when failure was expected"
  ut_setrequirements cTIME2004, "F"
ELSE
  write "<*> Passed (2004) - Error counter incremented as expected"
  ut_setrequirements cTIME2004, "P"
ENDIF

IF ($sc_$cpu_find_event[1].num_found_messages = 2) THEN 
  write "<*> Passed (2004) - Expected Event Msgs received."
  ut_setrequirements cTIME2004, "P"
ELSE
  write "<!> Failed (2004) - Expected event message ", CFE_TIME_SOURCE_ERR_EID, " not received 2 times."
  ut_setrequirements cTIME2004, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 1.3: Send the command to request diagnostic information."
write ";"
write ";  Requirement cTIME 2011"
write ";**********************************************************************"
write " Enable DEBUG messages"
/$SC_$CPU_EVS_ENAEVENTTYPE DEBUG
wait 5

NEW_VLD_CMD_CNT = $sc_$cpu_TIME_CMDPC + 1
local oldMETAtTone = $SC_$CPU_TIME_DTMETS

ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DIAG_EID, DEBUG, 1

/$sc_$cpu_TIME_RequestDiag
wait 5

ut_tlmwait $sc_$cpu_TIME_CMDPC, {NEW_VLD_CMD_CNT}
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (2011) - Request Diagnostic Info Command succeeded."
   ut_setrequirements cTIME2011, "P" 
else
  write "<!> Failed (2011) - Request Diagnostic Info Command did not increment CMDPC."
   ut_setrequirements cTIME2011, "F"
endif

;; Check for the event message
IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN 
  write "<*> Passed (2011) - Event Message found "
  ut_setrequirements cTIME2011, "P" 
ELSE
  write "<!> Failed (2011) - Expected event message ", CFE_TIME_DIAG_EID, " not received."
  ut_setrequirements cTIME2011, "F"
ENDIF

;; Check if the MET at Tone changed from previous value
if (oldMETAtTone <> $SC_$CPU_TIME_DTMETS) then
  write "<*> Passed (2011) - MET at Prev Tone changed as expected."
  ut_setrequirements cTIME2011, "P" 
else
  write "<!> Failed (2011) - MET at Prev Tone did not change."
  ut_setrequirements cTIME2011, "F"
endif

write " Disable DEBUG messages "
/$SC_$CPU_EVS_DISEVENTTYPE DEBUG 

;; Set 2314 to 'A'
;; The ES System log should be inspected to verify this rqmt
;; since CFE_ES_WriteToSysLog calls the CFE_TIME_Print API
;; Should we dump the System Log to a file at this point???
ut_setrequirements cTIME2314, "A"

wait 5

write ";**********************************************************************"
write "; Step 2.0: Load the Test Application (TST_TIME). "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, 1, INFO, 1

start load_start_app ("TST_TIME", "$CPU")
wait 10

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
   write "<*> Passed - Test Application TST_TIME started."
ELSE
   write "<!> Failed - Test Application TST_TIME did not start."
ENDIF

write ";**********************************************************************"
write "; Step 2.1: Add telemetry packets for Time Test App.                   "
write ";**********************************************************************"
LOCAL startStream = 0
LOCAL buffLimit = 4

;;/$sc_$cpu_TIME_RequestDiag
;;wait 2
;;
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
  WAIT 3
  write "Sent Add packet command for stream ", startStream

  ;;; Increment the stream
  startStream = startStream + 1
enddo

;;/$sc_$cpu_TST_TIME_SEND_HK
;;
/$SC_$CPU_EVS_ADDEVTFLTR APPLICATION="TST_TIME" EVENT_ID=X'15' EVENT_MASK=X'ffff' 
wait 5

write ";**********************************************************************"
write ";  Step 3.0: Verify the command to Set Clock Source and Clock Signal, "
write ";            it is dependent on the mission configuration. The "
write ";	    cfe_platform_cfg.h contains this definition, "
write ";            the test will check the configuration and test accordingly."
write ";"
write ";  Requirement # cTIME 2010  "
write ";**********************************************************************"

write "Current value of Clock Source is ", P@$sc_$cpu_Time_DFlagSrc
write "Current value of Clock Primary/Redundant is ", P@$sc_$cpu_Time_DFlagPri

IF (INTERN_FLAG = TRUE) THEN 
   write "The configuration is set to Internal. Expect the to commands External and Internal to fail "

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SOURCE_CFG_EID, ERROR, 1
   wait 5
   CMD_TIME_1 = %GMT
   /$sc_$cpu_TIME_SetSource EXTERNAL
   write "Command $sc_$cpu_TIME_SetSource to EXTERNAL sent at ", CMD_TIME_1
   wait 5

   write "Number of found messages is =", $sc_$cpu_find_event[1].num_found_messages

   IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
      write "<*> Passed (2010)- Expected Event Message received."
      ut_setrequirements cTIME2010, "P"
   ELSE
     write "<!> Failed (2010) - Expected event message ", CFE_TIME_SOURCE_CFG_EID, " not received."
      ut_setrequirements cTIME2010, "F"
   ENDIF

   IF (P@$sc_$cpu_Time_DFlagSrc = "INT") THEN
      write "<*> Passed (2010) - Clock source remained Internal as expected"
      ut_setrequirements cTIME2010, "P"
   ELSE
      write "<!> Failed (2010) - Clock source was not Internal as expected"
      ut_setrequirements cTIME2010, "F"
   ENDIF

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SOURCE_CFG_EID, ERROR, 1
   wait 5
   CMD_TIME_1 = %GMT
   /$sc_$cpu_TIME_SetSource INTERNAL
   write "Command $sc_$cpu_TIME_SetSource to INTERNAL sent at ", CMD_TIME_1
   wait 5

   IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
      write "<*> Passed (2010)- Expected Event Message received."
      ut_setrequirements cTIME2010, "P"
   ELSE
     write "<!> Failed (2010) - Expected event message ", CFE_TIME_SOURCE_CFG_EID, " not received."
     ut_setrequirements cTIME2010, "F"
   ENDIF

   IF (P@$sc_$cpu_Time_DFlagSrc = "INT") THEN
      write "<*> Passed (2010) - Clock source remained Internal as expected"
      ut_setrequirements cTIME2010, "P"
   ELSE
      write "<!> Failed (2010) - Clock source was not Internal as expected"
      ut_setrequirements cTIME2010, "F"
   ENDIF

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SIGNAL_CFG_EID, ERROR, 1
   wait 5
   CMD_TIME_1 = %GMT
   /$sc_$cpu_TIME_SetSignal REDUNDANT
   write "Command $sc_$cpu_TIME_SetSignal to REDUNDANT sent at ", CMD_TIME_1
   write " "
   wait 5
   IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
      write "<*> Passed (2010)- Expected Event Message received."
      ut_setrequirements cTIME2010, "P"
   ELSE
     write "<!> Failed (2010) - Expected event message ", CFE_TIME_SIGNAL_CFG_EID, " not received."
      ut_setrequirements cTIME2010, "F"
   ENDIF
ELSE 
   write "The configuration set to EXTERNAL. Expect the commands to External and Internal to pass "
   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SOURCE_EID, INFO, 1
   wait 5
   CMD_TIME_1 = %GMT

   /$sc_$cpu_TIME_SetSource EXTERNAL
   write "Command $sc_$cpu_TIME_SetSource to EXTERNAL sent at ", CMD_TIME_1
   wait 5

   IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
      write "<*> Passed (2010)- Expected Event Message received."
      ut_setrequirements cTIME2010, "P"
   ELSE
     write "<!> Failed (2010) - Expected event message ", CFE_TIME_SOURCE_EID, " not received."
      ut_setrequirements cTIME2010, "F"
   ENDIF

   IF (P@$sc_$cpu_Time_DFlagSrc = "EXT") THEN
      write "<*> Passed (2010) - Clock source set to EXTERNAL as expected"
      ut_setrequirements cTIME2010, "P"
   ELSE
      write "<!> Failed (2010) - Clock source was not EXTERNAL as expected"
      ut_setrequirements cTIME2010, "F"
   ENDIF

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SOURCE_EID, INFO, 1
   wait 5

   CMD_TIME_1 = %GMT
   /$sc_$cpu_TIME_SetSource INTERNAL
   write "Command $sc_$cpu_TIME_SetSource to INTERNAL sent at ", CMD_TIME_1
   wait 5

   IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
     write "<*> Passed (2010)- Expected Event Message received."
     ut_setrequirements cTIME2010, "P"
   ELSE
     write "<!> Failed (2010) - Expected event message ", CFE_TIME_SOURCE_EID, " not received."
     ut_setrequirements cTIME2010, "F"
   ENDIF

   IF (P@$sc_$cpu_Time_DFlagSrc = "INT") THEN
      write "<*> Passed (2010) - Clock source set to Internal as expected"
      ut_setrequirements cTIME2010, "P"
   ELSE
      write "<!> Failed (2010) - Clock source was not Internal as expected"
      ut_setrequirements cTIME2010, "F"
   ENDIF

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SIGNAL_EID, INFO, 1
   wait 5

   CMD_TIME_1 = %GMT
   /$sc_$cpu_TIME_SetSignal REDUNDANT
   write "Command $sc_$cpu_TIME_SetSignal to REDUNDANT sent at ", CMD_TIME_1
   wait 5

   IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
     write "<*> Passed (2010)- Expected Event Message received."
     ut_setrequirements cTIME2010, "P"
   ELSE
     write "<!> Failed (2010) - Expected event message ", CFE_TIME_SIGNAL_EID, " not received."
     ut_setrequirements cTIME2010, "F"
   ENDIF

   IF (P@$sc_$cpu_Time_DFlagPri = "RED" ) THEN
      write "<*> Passed (2010) - Clock Signal set to REDUNDANT as expected."
      ut_setrequirements cTIME2010, "P"
   ELSE
      write "<!> Failed (2010) - Clock signal does not specify REDUNDANT!!!"
      ut_setrequirements cTIME2010, "F"
   ENDIF
ENDIF

write " Set the Clock Signal back to Primary "

ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_SIGNAL_CFG_EID, ERROR, 1
wait 5

CMD_TIME_1 = %GMT
/$sc_$cpu_TIME_SetSignal PRIMARY
write "Command $sc_$cpu_TIME_SetSignal to PRIMARY sent at ", CMD_TIME_1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2010)- Expected Event Message received."
  ut_setrequirements cTIME2010, "P"
ELSE
  write "<!> Failed (2010) - Expected event message ", CFE_TIME_SIGNAL_CFG_EID, " not received."
  ut_setrequirements cTIME2010, "F"
ENDIF

IF (P@$sc_$cpu_Time_DFlagPri = "PRI" ) THEN
  write "<*> Passed (2010) - Clock Signal Telemetry for clock source PRIMARY is correct"
  ut_setrequirements cTIME2010, "P"
ELSE
  write "<!> Failed (2010) - Clock signal does not specify PRIMARY!!!"
  ut_setrequirements cTIME2010, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 4.0: Test the command to set the mission elapsed timer (MET) "
write ";            Note the following requirements are for requests for "
write ";            various time formats, there is no requirement to set the "
write ";            MET in the SRS. "
write ";"
write ";  Requirement # cTIME 2300, cTIME 2301, cTIME 2302, cTIME 2303," 
write ";  cTIME 2304, cTIME 2305"
write ";********************************************************************** "
write ";  Step 4.1: Test command parameter maximum value by setting the "
write ";            Seconds = 0xFFFFFFFF and Microseconds = 999999 "
write ";********************************************************************** "
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_MET_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_SetClockMET SECONDS=MAX_TIME, MICROSECS=MAX_MICROSEC
wait 5
IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message received."
ELSE
  write "<!> Failed - Expected event message ", CFE_TIME_MET_EID, " not received."
ENDIF

write "TIME value of $sc_$cpu_Time_DMETS = ", $sc_$cpu_Time_DMETS 
write "TIME value of $sc_$cpu_Time_DMETSS = ", $sc_$cpu_Time_DMETSS
write " "
write "Command $sc_$cpu_TIME_SetClockMET to max value at ", CMD_TIME_1
write " "

write ";********************************************************************** "
write ";  Step 4.1.1: Use the test application to request the current MET. "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_MET_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_MET

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
write " Save the current value of the MET at the same time as the request is sent"
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2303) - Expected Event Message received."
  ut_setrequirements cTIME2303, "P"
ELSE
  write "<!> Failed (2303) - Expected event message ", TST_TIME_MET_INF_EID, " not received."
  ut_setrequirements cTIME2303, "F"
ENDIF

write "Request for $sc_$cpu_TST_TIME_MET sent at ", CMD_TIME_1
write "Time Test app returned value for $sc_$cpu_TT_MET_SECS = ",$sc_$cpu_TT_MET_SECS
write "TIME value of $sc_$cpu_Time_DMETS = ", Time_DMETSeconds 
write " "
write "Time Test app returned value for $sc_$cpu_TT_MET_SUBS = ",$sc_$cpu_TT_MET_SUBS
write "TIME value of $sc_$cpu_Time_DMETSS = ", Time_DMETSSeconds 
write " "

IF (%unsigned_cmp($sc_$cpu_TT_MET_SECS, Time_DMETSeconds) = -1 ) THEN
    DELTA_Seconds = Time_DMETSeconds - $sc_$cpu_TT_MET_SECS
ELSE
    DELTA_Seconds = $sc_$cpu_TT_MET_SECS - Time_DMETSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2303) - Requested MET Time Delta is out of bounds."
  ut_setrequirements cTIME2303, "F"
ELSE
  write "<*> Passed (2303) - Requested MET Time Delta is in bounds."
  ut_setrequirements cTIME2303, "P"
ENDIF

write "The MET Delta seconds value is ", DELTA_Seconds
wait 5

write ";********************************************************************** "
write ";  Step 4.1.2: Use the test application to request the current MET "
write ";              seconds. "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_METSEC_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

write " Save the current value of the MET at the same time as the request is sent"

/$sc_$cpu_TST_TIME_METSEC
Time_DMETSeconds = $sc_$cpu_Time_DMETS
write "Request for $sc_$cpu_TST_TIME_METSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_MET_SECS = ",$sc_$cpu_TT_METSEC
write "TIME value of $sc_$cpu_Time_DMETS = ", Time_DMETSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2304) - Expected Event Message received."
   ut_setrequirements cTIME2304, "P"
ELSE
  write "<!> Failed (2304) - Expected event message ", TST_TIME_METSEC_INF_EID, " not received."
   ut_setrequirements cTIME2304, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_METSEC, Time_DMETSeconds) = -1 ) THEN
     DELTA_Seconds = Time_DMETSeconds - $sc_$cpu_TT_METSEC
ELSE
     DELTA_Seconds = $sc_$cpu_TT_METSEC - Time_DMETSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2304) - Requested MET Time Delta is greater than allowed."
  ut_setrequirements cTIME2304, "F"
ELSE
  write "<*> Passed (2304) - Requested MET Time Delta is in bounds."
  ut_setrequirements cTIME2304, "P"
ENDIF

write "The MET Delta seconds value is ", DELTA_Seconds
wait 5

write ";********************************************************************** "
write ";  Step 4.1.3: Use the test application to request the current MET "
write ";              subseconds. "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_METSUBSEC_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DMETSS 5

write " Save the current value of the MET at the same time as the request is sent"

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_METSUBSEC
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
write "Request for $sc_$cpu_TST_TIME_METSUBSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_METSSUB = ",$sc_$cpu_TT_METSSUB
write "TIME value of $sc_$cpu_Time_DMETSS = ", Time_DMETSSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2305) - Expected Event Message received."
  ut_setrequirements cTIME2305, "P"
ELSE
  write "<!> Failed (2305) - Expected event message ", TST_TIME_METSUBSEC_INF_EID, " not received."
  ut_setrequirements cTIME2305, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_METSSUB, Time_DMETSSeconds) = -1 ) THEN
     DELTA_SubSeconds = Time_DMETSSeconds - $sc_$cpu_TT_METSSUB
ELSE
     DELTA_SubSeconds = $sc_$cpu_TT_METSSUB - Time_DMETSSeconds
ENDIF

;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
;; tends to return 0 for SubSeconds more frequently than not.
;; Thus, this validation is commented out
;;IF (DELTA_SubSeconds > 2) THEN
;;  write "<!> Failed (2305) - Requested MET Time Sub Seconds Delta is out of bounds."
;;  ut_setrequirements cTIME2305, "F"
;;ELSE
;;  write "<*> Passed (2305) - Requested MET Time Sub Seconds Delta is in bounds."
;;  ut_setrequirements cTIME2305, "P"
;;ENDIF

write "The MET Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
wait 5

write ";********************************************************************** "
write ";  Step 4.1.4: Use the test application to request the current time in "
write ";              TAI format. "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_TAI_INF_EID, INFO, 1

write " Save the current value of the TAI at the same time as the request is sent"
write " "

ut_tlmupdate $sc_$cpu_Time_DTAIS 5

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_TAI
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS

write "Request for $sc_$cpu_TST_TIME_TAI sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_TAI_SECS = ",$sc_$cpu_TT_TAI_SECS
write "TIME value of $sc_$cpu_Time_DTAIS = ", Time_DTAISeconds 
write " "
write "Time Test app returned value for $sc_$cpu_TT_TAI_SUBS = ",$sc_$cpu_TT_TAI_SUBS
write "TIME value of $sc_$cpu_Time_DTAISS = ", Time_DTAISSeconds 
write " "

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2300) - Expected Event Message received."
  ut_setrequirements cTIME2300, "P"
ELSE
  write "<!> Failed (2300) - Expected event message ", TST_TIME_TAI_INF_EID, " not received."
   ut_setrequirements cTIME2300, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_TAI_SECS, Time_DTAISeconds) = -1 ) THEN
    DELTA_Seconds = Time_DTAISeconds - $sc_$cpu_TT_TAI_SECS
ELSE
    DELTA_Seconds = $sc_$cpu_TT_TAI_SECS - Time_DTAISeconds
ENDIF

IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2300) - Requested TAI Time Seconds Delta is out of bounds."
  ut_setrequirements cTIME2300, "F"
ELSE
  write "<*> Passed (2300) - Requested TAI Time Seconds Delta is in bounds."
  ut_setrequirements cTIME2300, "P"
ENDIF

write "The TAI Delta seconds value is ", DELTA_Seconds
wait 5

write ";**********************************************************************"
write ";  Step 4.1.5: Use the test application to request the current time in "
write ";              UTC format. "
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_UTC_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DUTCS 5

write " Save the current value of the UTC at the same time as the request is sent"

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_UTC

Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Request for $sc_$cpu_TST_TIME_UTC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_UTC_SECS = ",$sc_$cpu_TT_UTC_SECS
write "TIME value of $sc_$cpu_Time_DUTCS = ", Time_DUTCSeconds 
write " "
write "Time Test app returned value for $sc_$cpu_TT_UTC_SUBS = ",$sc_$cpu_TT_UTC_SUBS
write "TIME value of $sc_$cpu_Time_DUTCSS = ", Time_DUTCSSeconds 
write " "

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2301) - Expected Event Message received."
  ut_setrequirements cTIME2301, "P"
ELSE
  write "<!> Failed (2301) - Expected event message ", TST_TIME_UTC_INF_EID, " not received."
  ut_setrequirements cTIME2301, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_UTC_SECS, Time_DUTCSeconds) = -1 ) THEN
    DELTA_Seconds = Time_DUTCSeconds - $sc_$cpu_TT_UTC_SECS
ELSE
    DELTA_Seconds = $sc_$cpu_TT_UTC_SECS - Time_DUTCSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2301) - Requested UTC Time Seconds Delta is out of bounds."
  ut_setrequirements cTIME2301, "P"
ELSE
  write "<*> Passed (2301) - Requested UTC Time Seconds Delta is in bounds."
  ut_setrequirements cTIME2301, "P"
ENDIF

write "The UTC Delta seconds value is ", DELTA_Seconds
wait 5

write ";********************************************************************** "
write ";  Step 4.1.6: Use the test application to request the current time in "
write ";              default format. "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SPACE_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DTAIS 5

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_SPACETIME

Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Request $sc_$cpu_TST_TIME_SPACETIME sent at ", CMD_TIME_1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2302) - Expected Event Message received."
  ut_setrequirements cTIME2302, "P"
ELSE
  write "<!> Failed (2302) - Expected event message ", TST_TIME_SPACE_INF_EID, " not received."
  ut_setrequirements cTIME2302, "F"
ENDIF

IF CFG_DEFAULT_TAI THEN
   write "The configuration is set to TAI time format "
   write "Time Test app returned value for $sc_$cpu_TT_ST_SECS = ",$sc_$cpu_TT_ST_SECS
   write "TIME value of $sc_$cpu_Time_DTAIS = ", Time_DTAISeconds 
   write "Time Test app returned value for $sc_$cpu_TT_ST_SUBS = ",$sc_$cpu_TT_ST_SUBS
   write "TIME value of $sc_$cpu_Time_DTAISS = ", Time_DTAISSeconds 

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SECS, Time_DTAISeconds) = -1 ) THEN
        DELTA_Seconds = Time_DTAISeconds - $sc_$cpu_TT_ST_SECS
   ELSE
        DELTA_Seconds = $sc_$cpu_TT_ST_SECS - Time_DTAISeconds
   ENDIF

   IF (DELTA_Seconds > 2) THEN
     write "<!> Failed (2302) - Requested TAI Time Seconds Delta is out of bounds."
     ut_setrequirements cTIME2302, "F"
   ELSE
     write "<*> Passed (2302) - Requested TAI Time Seconds Delta is in bounds."
     ut_setrequirements cTIME2302, "P"
   ENDIF

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SUBS, Time_DTAISSeconds) = -1 ) THEN
        DELTA_SubSeconds = Time_DTAISSeconds - $sc_$cpu_TT_ST_SUBS
   ELSE
        DELTA_SubSeconds = $sc_$cpu_TT_ST_SUBS - Time_DTAISSeconds
   ENDIF

   ;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
   ;; tends to return 0 for SubSeconds more frequently than not.
   ;; Thus, this validation is commented out
   ;;IF (DELTA_SubSeconds > 2) THEN
   ;;  write "<!> Failed (2302) - Requested TAI Time Sub Seconds Delta is out of bounds."
   ;;  ut_setrequirements cTIME2302, "F"
   ;;ELSE
   ;;  write "<*> Passed (2302) - Requested TAI Time Sub Seconds Delta is in bounds."
   ;;  ut_setrequirements cTIME2302, "P"
   ;;ENDIF

   write "The TAI Delta seconds value is ", DELTA_Seconds
   write "The TAI Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
ENDIF

IF CFG_DEFAULT_UTC THEN
   write "The configuration is set to UTC time format "
   write "Time Test app returned value for $sc_$cpu_TT_ST_SECS = ",$sc_$cpu_TT_ST_SECS
   write "TIME value of $sc_$cpu_Time_DUTCS = ", Time_DUTCSeconds 
   write "Time Test app returned value for $sc_$cpu_TT_ST_SUBS = ",$sc_$cpu_TT_ST_SUBS
   write "TIME value of $sc_$cpu_Time_DUTCSS = ", Time_DUTCSSeconds 

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SECS, Time_DUTCSeconds) = -1 ) THEN
        DELTA_Seconds = Time_DUTCSeconds - $sc_$cpu_TT_ST_SECS
   ELSE
        DELTA_Seconds = $sc_$cpu_TT_ST_SECS - Time_DUTCSeconds
   ENDIF
   IF (DELTA_Seconds > 2) THEN
     write "<!> Failed (2302) - Requested UTC Time Seconds Delta is out of bounds."
     ut_setrequirements cTIME2302, "F"
   ELSE
     write "<*> Passed (2302) - Requested UTC Time Seconds Delta is in bounds."
     ut_setrequirements cTIME2302, "P"
   ENDIF

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SUBS, Time_DUTCSSeconds) = -1 ) THEN
        DELTA_SubSeconds = Time_DUTCSSeconds - $sc_$cpu_TT_ST_SUBS
   ELSE
        DELTA_SubSeconds = $sc_$cpu_TT_ST_SUBS - Time_DUTCSSeconds
   ENDIF

   ;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
   ;; tends to return 0 for SubSeconds more frequently than not.
   ;; Thus, this validation is commented out
   ;;IF (DELTA_SubSeconds > 2) THEN
   ;;  write "<!> Failed (2302) - Requested UTC Time Sub Seconds Delta is out of bounds."
   ;;    ut_setrequirements cTIME2302, "F"
   ;;ELSE
   ;;  write "<*> Passed (2302) - Requested UTC Time Sub Seconds Delta is in bounds."
   ;;  ut_setrequirements cTIME2302, "P"
   ;;ENDIF
   write "The UTC Delta seconds value is ", DELTA_Seconds
   write "The UTC Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
ENDIF
wait 5

write ";********************************************************************** "
write ";  Step 4.2: Test command parameter minimum value "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_MET_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_SetClockMET SECONDS=0,MICROSECS=0
wait 5

write "TIME value of $sc_$cpu_Time_DMETS = ", $sc_$cpu_Time_DMETS 
write "TIME value of $sc_$cpu_Time_DMETSS = ", $sc_$cpu_Time_DMETSS
write "Command $sc_$cpu_TIME_SetClockMET to min value at ", CMD_TIME_1

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message received."
ELSE
  write "<!> Failed - Expected event message ", CFE_TIME_MET_EID, " not received."
ENDIF

wait 5

write ";********************************************************************** "
write ";  Step 4.2.1: Use the test application to request the current MET. "
write ";********************************************************************** "
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_MET_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

write " Save the current value of the MET at the same time as the request is sent"

/$sc_$cpu_TST_TIME_MET
Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
write "Request for $sc_$cpu_TST_TIME_MET sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_MET_SECS = ",$sc_$cpu_TT_MET_SECS
write "TIME value of $sc_$cpu_Time_DMETS = ", Time_DMETSeconds 
write "Time Test app returned value for $sc_$cpu_TT_MET_SUBS = ",$sc_$cpu_TT_MET_SUBS
write "TIME value of $sc_$cpu_Time_DMETSS = ", Time_DMETSSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2303) - Expected Event Message received."
  ut_setrequirements cTIME2303, "P"
ELSE
  write "<!> Failed (2303) - Expected event message ", TST_TIME_MET_INF_EID, " not received."
   ut_setrequirements cTIME2303, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_MET_SECS, Time_DMETSeconds) = -1 ) THEN
    DELTA_Seconds = Time_DMETSeconds - $sc_$cpu_TT_MET_SECS
ELSE
    DELTA_Seconds = $sc_$cpu_TT_MET_SECS - Time_DMETSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2303) - Requested MET Time Delta is out of bounds."
  ut_setrequirements cTIME2303, "F"
ELSE
  write "<*> Passed (2303) - Requested MET Time Delta is in bounds."
  ut_setrequirements cTIME2303, "P"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_MET_SUBS, Time_DMETSSeconds) = -1 ) THEN
    DELTA_SubSeconds = Time_DMETSSeconds - $sc_$cpu_TT_MET_SUBS
ELSE
    DELTA_SubSeconds = $sc_$cpu_TT_MET_SUBS - Time_DMETSSeconds
ENDIF

;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
;; tends to return 0 for SubSeconds more frequently than not.
;; Thus, this validation is commented out
;;IF (DELTA_SubSeconds > 2) THEN
;;  write "<!> Failed (2303) - Requested MET Time Sub Seconds Delta is out of bounds."
;;  ut_setrequirements cTIME2303, "F"
;;ELSE
;;  write "<*> Passed (2303) - Requested MET Time Sub Seconds Delta is in bounds."
;;  ut_setrequirements cTIME2303, "P"
;;ENDIF

write "The MET Delta seconds value is ", DELTA_Seconds
write "The MET Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
wait 5

write ";**********************************************************************"
write ";  Step 4.2.2: Use the test application to request the current MET"
write ";              seconds."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_METSEC_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

write " Save the current value of the MET at the same time as the request is sent"

/$sc_$cpu_TST_TIME_METSEC

Time_DMETSeconds = $sc_$cpu_Time_DMETS
write "Request for $sc_$cpu_TST_TIME_METSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_MET_SECS = ",$sc_$cpu_TT_METSEC
write "TIME value of $sc_$cpu_Time_DMETS = ", Time_DMETSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2304) - Expected Event Message received."
  ut_setrequirements cTIME2304, "P"
ELSE
  write "<!> Failed (2304) - Expected event message ", TST_TIME_MET_INF_EID, " not received."
  ut_setrequirements cTIME2304, "F"
ENDIF


IF (%unsigned_cmp($sc_$cpu_TT_METSEC, Time_DMETSeconds) = -1 ) THEN
     DELTA_Seconds = Time_DMETSeconds - $sc_$cpu_TT_METSEC
ELSE
     DELTA_Seconds = $sc_$cpu_TT_METSEC - Time_DMETSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2304) - Requested MET Time Seconds Delta is out of bounds."
  ut_setrequirements cTIME2304, "F"
ELSE
  write "<*> Passed (2304) - Requested MET Time Seconds Delta is in bounds."
  ut_setrequirements cTIME2304, "P"
ENDIF

write "The MET Delta seconds value is ", DELTA_Seconds
wait 5

write ";**********************************************************************"
write "; Step 4.2.3: Use the test application to request the current MET "
write ";              subseconds"
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_METSUBSEC_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DMETSS 5

write " Save the current value of the MET at the same time as the request is sent"

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_METSUBSEC

Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
write "Request for $sc_$cpu_TST_TIME_METSUBSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_METSSUB = ",$sc_$cpu_TT_METSSUB
write "TIME value of $sc_$cpu_Time_DMETSS = ", Time_DMETSSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2305) - Expected Event Message received."
  ut_setrequirements cTIME2305, "P"
ELSE
  write "<!> Failed (2305) - Expected event message ", TST_TIME_MET_INF_EID, " not received."
  ut_setrequirements cTIME2305, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_METSSUB, Time_DMETSSeconds) = -1 ) THEN
     DELTA_SubSeconds = Time_DMETSSeconds - $sc_$cpu_TT_METSSUB
ELSE
     DELTA_SubSeconds = $sc_$cpu_TT_METSSUB - Time_DMETSSeconds
ENDIF

;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
;; tends to return 0 for SubSeconds more frequently than not.
;; Thus, this validation is commented out
;;IF (DELTA_SubSeconds > 2) THEN
;;  write "<!> Failed (2305) - Requested MET Time Sub Seconds Delta is out of bounds."
;;  ut_setrequirements cTIME2305, "F"
;;ELSE
;;  write "<*> Passed (2305) - Requested MET Time Sub Seconds Delta is in bounds."
;;  ut_setrequirements cTIME2305, "P"
;;ENDIF

write "The MET Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
wait 5

write ";**********************************************************************"
write ";  Step 4.2.4: Use the test application to request the current time in "
write ";              TAI format."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_TAI_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DTAIS 5

write " Save the current value of the TAI at the same time as the request is sent"

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_TAI
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS

write "Request for $sc_$cpu_TST_TIME_TAI sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_TAI_SECS = ",$sc_$cpu_TT_TAI_SECS
write "TIME value of $sc_$cpu_Time_DTAIS = ", Time_DTAISeconds 
write "Time Test app returned value for $sc_$cpu_TT_TAI_SUBS = ",$sc_$cpu_TT_TAI_SUBS
write "TIME value of $sc_$cpu_Time_DTAISSeconds = ", Time_DTAISSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2300) - Expected Event Message received."
  ut_setrequirements cTIME2300, "P"
ELSE
  write "<!> Failed (2300) - Expected event message ", TST_TIME_MET_INF_EID, " not received."
  ut_setrequirements cTIME2300, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_TAI_SECS, Time_DTAISeconds) = -1 ) THEN
    DELTA_Seconds = Time_DTAISeconds - $sc_$cpu_TT_TAI_SECS
ELSE
    DELTA_Seconds = $sc_$cpu_TT_TAI_SECS - Time_DTAISeconds
ENDIF

IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2300) - Requested TAI Time Seconds Delta is out of bounds."
  ut_setrequirements cTIME2300, "F"
ELSE
  write "<*> Passed (2300) - Requested TAI Time Seconds Delta is in bounds."
    write "Requested TAI Time Delta is in bounds, request verified"
    write " "
    ut_setrequirements cTIME2300, "P"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_TAI_SUBS, Time_DTAISSeconds) = -1 ) THEN
    DELTA_SubSeconds = Time_DTAISSeconds - $sc_$cpu_TT_TAI_SUBS
ELSE
    DELTA_SubSeconds = $sc_$cpu_TT_TAI_SUBS - Time_DTAISSeconds
ENDIF

;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
;; tends to return 0 for SubSeconds more frequently than not.
;; Thus, this validation is commented out
;;IF (DELTA_SubSeconds > 2) THEN
;;  write "<!> Failed (2300) - Requested TAI Time Sub Seconds Delta is out of bounds."
;;  ut_setrequirements cTIME2300, "F"
;;ELSE
;;  write "<*> Passed (2300) - Requested TAI Time Sub Seconds Delta is in bounds."
;;  ut_setrequirements cTIME2300, "P"
;;ENDIF

write "The TAI Delta seconds value is ", DELTA_Seconds
write "The TAI Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
wait 5

write ";**********************************************************************"
write ";  Step 4.2.5: Use the test application to request the current time in "
write ";              UTC format."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_UTC_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DUTCS 5

write " Save the current value of the UTC at the same time as the request is sent"

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_UTC

Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Request for $sc_$cpu_TST_TIME_UTC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_UTC_SECS = ",$sc_$cpu_TT_UTC_SECS
write "TIME value of $sc_$cpu_Time_DUTCS = ", Time_DUTCSeconds 
write "Time Test app returned value for $sc_$cpu_TT_UTC_SUBS = ",$sc_$cpu_TT_UTC_SUBS
write "TIME value of $sc_$cpu_Time_DUTCSS = ", Time_DUTCSSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2301) - Expected Event Message received."
  ut_setrequirements cTIME2301, "P"
ELSE
  write "<!> Failed (2301) - Expected event message ", TST_TIME_UTC_INF_EID, " not received."
  ut_setrequirements cTIME2301, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_UTC_SECS, Time_DUTCSeconds) = -1 ) THEN
    DELTA_Seconds = Time_DUTCSeconds - $sc_$cpu_TT_UTC_SECS
ELSE
    DELTA_Seconds = $sc_$cpu_TT_UTC_SECS - Time_DUTCSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2301) - Requested UTC Time Seconds Delta is out of bounds."
  ut_setrequirements cTIME2301, "P"
ELSE
  write "<*> Passed (2301) - Requested UTC Time Seconds Delta is in bounds."
  ut_setrequirements cTIME2301, "P"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_UTC_SUBS, Time_DUTCSSeconds) = -1 ) THEN
    DELTA_SubSeconds = Time_DUTCSSeconds - $sc_$cpu_TT_UTC_SUBS
ELSE
    DELTA_SubSeconds = $sc_$cpu_TT_UTC_SUBS - Time_DUTCSSeconds
ENDIF

;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
;; tends to return 0 for SubSeconds more frequently than not.
;; Thus, this validation is commented out
;;IF (DELTA_SubSeconds > 2) THEN
;;  write "<!> Failed (2301) - Requested UTC Time Sub Seconds Delta is out of bounds."
;;  ut_setrequirements cTIME2301, "P"
;;ELSE
;;  write "<*> Passed (2301) - Requested UTC Time Sub Seconds Delta is in bounds."
;;  ut_setrequirements cTIME2301, "P"
;;ENDIF

write "The UTC Delta seconds value is ", DELTA_Seconds
write "The UTC Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
wait 5

write ";**********************************************************************"
write ";  Step 4.2.6: Use the test application to request the current time in "
write ";              default format."
write ";**********************************************************************"
;; Setup for the expected event
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SPACE_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_Time_DTAIS 5

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_SPACETIME

Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Request $sc_$cpu_TST_TIME_SPACETIME sent at ", CMD_TIME_1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2302) - Expected Event Message received."
  ut_setrequirements cTIME2302, "P"
ELSE
  write "<!> Failed (2302) - Expected event message ", TST_TIME_SPACE_INF_EID, " not received."
  ut_setrequirements cTIME2302, "F"
ENDIF


IF CFG_DEFAULT_TAI THEN
   write "The configuration is set to TAI time format "
   write "Time Test app returned value for $sc_$cpu_TT_ST_SECS = ",$sc_$cpu_TT_ST_SECS
   write "TIME value of $sc_$cpu_Time_DTAIS = ", Time_DTAISeconds 
   write "Time Test app returned value for $sc_$cpu_TT_ST_SUBS = ",$sc_$cpu_TT_ST_SUBS
   write "TIME value of $sc_$cpu_Time_DTAISS = ", Time_DTAISSeconds 

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SECS, Time_DTAISeconds) = -1 ) THEN
        DELTA_Seconds = Time_DTAISeconds - $sc_$cpu_TT_ST_SECS
   ELSE
        DELTA_Seconds = $sc_$cpu_TT_ST_SECS - Time_DTAISeconds
   ENDIF
   IF (DELTA_Seconds > 2) THEN
     write "<!> Failed (2302) - Requested TAI Time Seconds Delta is out of bounds."
     ut_setrequirements cTIME2302, "f"
   ELSE
     write "<*> Passed (2302) - Requested TAI Time Seconds Delta is in bounds."
     ut_setrequirements cTIME2302, "P"
   ENDIF

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SUBS, Time_DTAISSeconds) = -1 ) THEN
        DELTA_SubSeconds = Time_DTAISSeconds - $sc_$cpu_TT_ST_SUBS
   ELSE
        DELTA_SubSeconds = $sc_$cpu_TT_ST_SUBS - Time_DTAISSeconds
   ENDIF

;; Subseconds are not very reliable since the cFE is using a Fake Tone which 
;; tends to return 0 for SubSeconds more frequently than not.
;; Thus, this validation is commented out
;;   IF (DELTA_SubSeconds > 2) THEN
;;     write "<!> Failed (2302) - Requested TAI Time Sub Seconds Delta is out of bounds."
;;     ut_setrequirements cTIME2302, "F"
;;   ELSE
;;     write "<*> Passed (2302) - Requested TAI Time Sub Seconds Delta is in bounds."
;;     ut_setrequirements cTIME2302, "P"
;;   ENDIF

   write "The TAI Delta seconds value is ", DELTA_Seconds
   write "The TAI Delta sub seconds value is ", DELTA_SubSeconds
ENDIF

IF CFG_DEFAULT_UTC THEN
   write "The configuration is set to UTC time format "
   write "Time Test app returned value for $sc_$cpu_TT_ST_SECS = ",$sc_$cpu_TT_ST_SECS
   write "TIME value of $sc_$cpu_Time_DUTCS = ", Time_DUTCSeconds 
   write "Time Test app returned value for $sc_$cpu_TT_ST_SUBS = ",$sc_$cpu_TT_ST_SUBS
   write "TIME value of $sc_$cpu_Time_DUTCSS = ", Time_DUTCSSeconds 

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SECS, Time_DUTCSeconds) = -1 ) THEN
        DELTA_Seconds = Time_DUTCSeconds - $sc_$cpu_TT_ST_SECS
   ELSE
        DELTA_Seconds = $sc_$cpu_TT_ST_SECS - Time_DUTCSeconds
   ENDIF

   IF (DELTA_Seconds > 2) THEN
     write "<!> Failed (2302) - Requested UTC Time Seconds Delta is out of bounds."
     ut_setrequirements cTIME2302, "F"
   ELSE
     write "<*> Passed (2302) - Requested UTC Time Seconds Delta is in bounds."
     ut_setrequirements cTIME2302, "P"
   ENDIF

   IF (%unsigned_cmp($sc_$cpu_TT_ST_SUBS, Time_DUTCSSeconds) = -1 ) THEN
        DELTA_SubSeconds = Time_DUTCSSeconds - $sc_$cpu_TT_ST_SUBS
   ELSE
        DELTA_SubSeconds = $sc_$cpu_TT_ST_SUBS - Time_DUTCSSeconds
   ENDIF

   IF (DELTA_SubSeconds > 2) THEN
     write "<!> Failed (2302) - Requested UTC Time Sub Seconds Delta is out of bounds."
     ut_setrequirements cTIME2302, "F"
   ELSE
     write "<*> Passed (2302) - Requested UTC Time Sub Seconds Delta is in bounds."
     ut_setrequirements cTIME2302, "P"
   ENDIF

   write "The UTC Delta seconds value is ", DELTA_Seconds
   write "The UTC Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds
ENDIF
wait 5

write ";**********************************************************************"
write ";  Step 5.0: Test the command to set the leap seconds "
write ";"
write ";  Requirements tested:" 
write ";     cTIME 2005"
write ";     cTIME 2307"
write ";"
write "; The following applies to the time values listed below:"
write ";     The MET is uncorrected/adjusted time for the system " 
write ";     The TAI is = MET + STCF "
write ";     The UTC is = TAI - Leap Seconds"
write ";     The STCF is = commanded time value - MET"
write ";**********************************************************************"
write "Print out the current values of the TAI, MET, UTC and LeapSeconds before we start messing with them"
write "Current TIME value of $sc_$cpu_Time_DTAIS = ",  $sc_$cpu_Time_DTAIS
write "Current TIME value of $sc_$cpu_Time_DTAISS = ",  $sc_$cpu_Time_DTAISS
write "Current TIME value of $sc_$cpu_Time_DMETS = ",  $sc_$cpu_Time_DMETS
write "Current TIME value of $sc_$cpu_Time_DMETSS = ",  $sc_$cpu_Time_DMETSS
write "Current TIME value of $sc_$cpu_Time_DUTCS = ",  $sc_$cpu_Time_DUTCS
write "Current TIME value of $sc_$cpu_Time_DUTCSS = ",  $sc_$cpu_Time_DUTCSS
write "Current TIME value of $sc_$cpu_TIME_DSTCFS = ",  $sc_$cpu_TIME_DSTCFS
write "Current TIME value of $sc_$cpu_Time_DSTCFSS = ",  $sc_$cpu_Time_DSTCFSS
write "Current TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS 

write ";**********************************************************************"
write ";  Step 5.1: Test command parameter maximum value"
write ";**********************************************************************"
; Set up to capture the event message of changing the Leap Seconds
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_LEAPS_EID, INFO, 1

/$sc_$cpu_TIME_SetClockLeap LEAPS=MAX_LEAP_SEC
wait 5
write "TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2305) - Expected Event Message received."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2305) - Expected event message ", CFE_TIME_LEAPS_EID, " not received."
  ut_setrequirements cTIME2005, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_Time_DLeapS, MAX_LEAP_SEC) = 0 ) THEN
  write "<*> Passed (2305) - Commanded Leap Seconds."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2305) - Commanded Leap Seconds not set."
  ut_setrequirements cTIME2005, "F"
ENDIF

if ($SC_$CPU_TIME_LEAPSECS = MAX_LEAP_SEC) then
  write "<*> Passed (2000) - LeapSeconds TLM reflects value set."
  ut_setrequirements cTIME2000, "P"
else
  write "<!> Failed (2000) - LeapSeconds TLM was not set to commanded value."
  ut_setrequirements cTIME2000, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.1.1: Use the test application to request the current Leap "
write ";              Seconds."
write ";**********************************************************************"
; Set up to capture the event message of changing the Leap Seconds
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_LEAPSEC_INF_EID, INFO, 1

CMD_TIME_1 = %GMT

/$sc_$cpu_TST_TIME_LEAPSEC

write "Request $sc_$cpu_TST_TIME_LEAPSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_LPSEC_SECS = ",$sc_$cpu_TT_LPSEC_SECS
write "TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2307) - Expected Event Message received."
  ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - Expected event message ", TST_TIME_LEAPSEC_INF_EID, " not received."
   ut_setrequirements cTIME2307, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_LPSEC_SECS, MAX_LEAP_SEC) = 0 ) THEN
  write "<*> Passed (2307) - Expected Leap Seconds reported by TST_TIME."
  ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - TST_TIME Leap Seconds are not correct."
  ut_setrequirements cTIME2307, "F"
ENDIF
wait 5

write ";**********************************************************************"
write ";  Step 5.1.2: Compute the new values of the time formats"
write "; "
write ";  NOTE: There is no requirement for the difference between the computed"
write ";        values for TAI and UTC seconds & sub seconds. This was done as "
write ";        a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 10

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  Time_DSTCFSeconds
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", Time_DLeapSeconds

write "; Compute the expected values of the TAI and UTC" 

Compute_Time_DTAISeconds = Time_DMETSeconds + Time_DSTCFSeconds

IF (%unsigned_cmp((Time_DMETSSeconds + Time_DSTCFSSeconds), MAX_TIME)) = 1 THEN
    write "MET + STCF Sub Seconds overflowed, subtract MAX_TIME and add one to Seconds"
    Compute_Time_DTAISSeconds = (Time_DMETSSeconds + Time_DSTCFSSeconds) - MAX_TIME
    Compute_Time_DTAISeconds = Compute_Time_DTAISeconds + 1    
ELSE
    write "MET + STCF Sub Seconds OK, add the values"
    Compute_Time_DTAISSeconds = Time_DMETSSeconds + Time_DSTCFSSeconds 
ENDIF

Compute_Time_DUTCSeconds = Compute_Time_DTAISeconds - Time_DLeapSeconds
Compute_Time_DUTCSSeconds = Compute_Time_DTAISSeconds 

write "Computed value of TAI Seconds is = ", Compute_Time_DTAISeconds
write "Computed value of TAI Sub Seconds is = ", Compute_Time_DTAISSeconds
write "Computed value of UTC Seconds is = ", Compute_Time_DUTCSeconds
write "Computed value of UTC Sub Seconds is = ", Compute_Time_DUTCSSeconds
write " Verify the new values are expected based on the modified STCF "

IF (%unsigned_cmp(Compute_Time_DTAISeconds, Time_DTAISeconds) = -1 ) THEN
    DELTA_Seconds = Time_DTAISeconds - Compute_Time_DTAISeconds
ELSE
    DELTA_Seconds = Compute_Time_DTAISeconds - Time_DTAISeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
    write "<!> Failed - Computed TAI Time Seconds Delta is out of bounds."
ELSE
    write "<*> Passed - Computed TAI Time Seconds Delta is in bounds."
ENDIF

write "The TAI Delta seconds value is ", DELTA_Seconds

IF (%unsigned_cmp(Compute_Time_DUTCSeconds, Time_DUTCSeconds) = -1 ) THEN
    DELTA_Seconds = Time_DUTCSeconds - Compute_Time_DUTCSeconds
ELSE
    DELTA_Seconds = Compute_Time_DUTCSeconds - Time_DUTCSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
    write "<!> Failed - Computed UTC Time Seconds Delta is out of bounds."
ELSE
    write "<*> Passed - Computed UTC Time Seconds Delta is in bounds."
ENDIF

write "The UTC Delta seconds value is ", DELTA_Seconds
wait 5

write ";**********************************************************************"
write ";  Step 5.2: Test command parameter minimum value for Leap Seconds"
write ";**********************************************************************"
; Set up to capture the event message of changing the Leap Seconds
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_LEAPS_EID, INFO, 1

/$sc_$cpu_TIME_SetClockLeap LEAPS=MIN_LEAP_SEC
wait 5

write "TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2305) - Expected Event Message received."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2305) - Expected event message ", CFE_TIME_LEAPS_EID, " not received."
   ut_setrequirements cTIME2005, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_Time_DLeapS, MIN_LEAP_SEC) = 0 ) THEN
  write "<*> Passed (2005) - Commanded Leap Seconds were set correctly."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2005) - Commanded Leap Seconds were not as expected."
  ut_setrequirements cTIME2005, "F"
ENDIF

if ($SC_$CPU_TIME_LEAPSECS = MIN_LEAP_SEC) then
  write "<*> Passed (2000) - LeapSeconds TLM reflects value set."
  ut_setrequirements cTIME2000, "P"
else
  write "<!> Failed (2000) - LeapSeconds TLM was not set to commanded value."
  ut_setrequirements cTIME2000, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.2.1: Use the test application to request the current Leap "
write ";              Seconds."
write ";**********************************************************************"
; Set up to capture the event message of changing the Leap Seconds
ut_setupevents $SC, $CPU, tst_time, TST_TIME_LEAPSEC_INF_EID, INFO, 1

CMD_TIME_1 = %GMT

/$sc_$cpu_TST_TIME_LEAPSEC

write "Request $sc_$cpu_TST_TIME_LEAPSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_LPSEC_SECS = ",$sc_$cpu_TT_LPSEC_SECS
write "TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2307) - Expected Event Message received."
   ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - Expected event message ", TST_TIME_LEAPSEC_INF_EID, " not received."
   ut_setrequirements cTIME2307, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_LPSEC_SECS, MIN_LEAP_SEC) = 0 ) THEN
  write "<*> Passed (2307) - Expected Leap Seconds reported by TST_TIME."
  ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - TST_TIME Leap Seconds are not correct."
  ut_setrequirements cTIME2307, "F"
ENDIF
wait 5

write ";**********************************************************************"
write ";  Step 5.2.2: Compute the new values of the time formats"
write ";"
write ";  NOTE: There is no requirement for difference between the computed "
write ";        values for TAI and UTC seconds & sub seconds. This was done as "
write ";        a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 10

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS


write "Print out the Saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  Time_DSTCFSeconds
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", Time_DLeapSeconds

write "; Compute the expected values of the TAI and UTC" 

Compute_Time_DTAISeconds = Time_DMETSeconds + Time_DSTCFSeconds

IF (%unsigned_cmp((Time_DMETSSeconds + Time_DSTCFSSeconds), MAX_TIME)) = 1 THEN
    write "MET + STCF Sub Seconds overflowed, subtract MAX_TIME and add one to Seconds"
    Compute_Time_DTAISSeconds = (Time_DMETSSeconds + Time_DSTCFSSeconds) - MAX_TIME
    Compute_Time_DTAISeconds = Compute_Time_DTAISeconds + 1    
ELSE
    write "MET + STCF Sub Seconds OK, add the values"
    Compute_Time_DTAISSeconds = Time_DMETSSeconds + Time_DSTCFSSeconds 
ENDIF

Compute_Time_DUTCSeconds = Compute_Time_DTAISeconds - Time_DLeapSeconds
Compute_Time_DUTCSSeconds = Compute_Time_DTAISSeconds 

write "Computed value of TAI Seconds is = ", Compute_Time_DTAISeconds
write "Computed value of TAI Sub Seconds is = ", Compute_Time_DTAISSeconds
write "Computed value of UTC Seconds is = ", Compute_Time_DUTCSeconds
write "Computed value of UTC Sub Seconds is = ", Compute_Time_DUTCSSeconds
write " Verify the new values are expected based on the modified STCF "

IF (%unsigned_cmp(Compute_Time_DTAISeconds, Time_DTAISeconds) = -1 ) THEN
    DELTA_Seconds = Time_DTAISeconds - Compute_Time_DTAISeconds
ELSE
    DELTA_Seconds = Compute_Time_DTAISeconds - Time_DTAISeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
    write "<!> Failed - Computed TAI Time Seconds Delta is out of bounds."
ELSE
    write "<*> Passed - Computed TAI Time Seconds Delta is in bounds."
ENDIF

write "The TAI Delta seconds value is ", DELTA_Seconds

IF (%unsigned_cmp(Compute_Time_DUTCSeconds, Time_DUTCSeconds) = -1 ) THEN
    DELTA_Seconds = Time_DUTCSeconds - Compute_Time_DUTCSeconds
ELSE
    DELTA_Seconds = Compute_Time_DUTCSeconds - Time_DUTCSeconds
ENDIF
IF (DELTA_Seconds > 2) THEN
    write "<!> Failed - Computed UTC Time Seconds Delta is out of bounds."
ELSE
    write "<*> Passed - Computed UTC Time Seconds Delta is in bounds."
ENDIF

write "The UTC Delta seconds value is ", DELTA_Seconds
wait 5

write ";**********************************************************************"
write ";  Step 5.3: Return the Leap Seconds to its nominal value (32)."
write ";**********************************************************************"
; Set up to capture the event message of changing the Leap Seconds
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_LEAPS_EID, INFO, 1

/$sc_$cpu_TIME_SetClockLeap LEAPS=32
wait 5
write "TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2005) - Expected Event Message received."
   ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2005) - Expected event message ", CFE_TIME_LEAPS_EID, " not received."
   ut_setrequirements cTIME2005, "F"
ENDIF

IF ($sc_$cpu_Time_DLeapS =  32) THEN
  write "<*> Passed (2005) - Leap Seconds set as expected."
  ut_setrequirements cTIME2005, "P"
ELSE
  write "<!> Failed (2005) - Leap Seconds did not get set."
  ut_setrequirements cTIME2005, "F"
ENDIF

if ($SC_$CPU_TIME_LEAPSECS = 32) then
  write "<*> Passed (2000) - LeapSeconds TLM reflects value set."
  ut_setrequirements cTIME2000, "P"
else
  write "<!> Failed (2000) - LeapSeconds TLM was not set to commanded value."
  ut_setrequirements cTIME2000, "F"
endif

wait 5

write ";**********************************************************************"
write ";  Step 5.3.1: Verify the Leap Seconds returned to its nominal value. "
write ";**********************************************************************"
; Set up to capture the event message of changing the Leap Seconds
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_LEAPSEC_INF_EID, INFO, 1

CMD_TIME_1 = %GMT

/$sc_$cpu_TST_TIME_LEAPSEC

write "Request $sc_$cpu_TST_TIME_LEAPSEC sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_LPSEC_SECS = ",$sc_$cpu_TT_LPSEC_SECS
write "TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLEAPS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2307) - Expected Event Message received."
   ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - Expected event message ", TST_TIME_LEAPSEC_INF_EID, " not received."
   ut_setrequirements cTIME2307, "F"
ENDIF

IF ($sc_$cpu_TT_LPSEC_SECS = 32) THEN
  write "<*> Passed (2307) - Leap Seconds set as expected."
    ut_setrequirements cTIME2307, "P"
ELSE
  write "<!> Failed (2307) - Leap Seconds did not get set."
    ut_setrequirements cTIME2307, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 6.0: Test the command to set the spacecraft time correlation "
write ";            factor (STCF)"
write ";"
write ";  Requirements tested: "
write ";     cTIME 2006"
write ";     cTIME 2306"
write ";**********************************************************************"
write ";  Step 6.1: Test command parameter maximum value for seconds and sub "
write ";            seconds"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_SetClockSTCF SECONDS=MAX_TIME MICROSECS=MAX_MICROSEC
write "Command $sc_$cpu_TIME_SetClockSTCF Seconds = to FFFFFFFF, MICROSECONDS = to 999999 at ", CMD_TIME_1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2006) - Expected Event Message received."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - Expected event message ", CFE_TIME_STCF_EID, " not received."
  ut_setrequirements cTIME2006, "F"
ENDIF

write ";;; Comparing STCF Seconds = ",$sc_$cpu_TIME_DSTCFS," to ", MAX_TIME

IF (%unsigned_cmp($sc_$cpu_TIME_DSTCFS, MAX_TIME) = 0) THEN
  write "<*> Passed (2006) - STCF Seconds set as expected."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - STCF Seconds did not get set properly."
  ut_setrequirements cTIME2006, "F"
ENDIF

write ";;; Comparing STCF Sub Seconds = ",$sc_$cpu_TIME_DSTCFSS," to ", MAX_MICRO_2_SSEC

IF (%unsigned_cmp($sc_$cpu_Time_DSTCFSS, MAX_MICRO_2_SSEC) = 0) THEN
  write "<*> Passed (2000;2006) - STCF Sub Seconds set as expected."
  ut_setrequirements cTIME2000, "P"
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2000;2006) - STCF Sub Seconds did not get set properly."
  ut_setrequirements cTIME2000, "F"
  ut_setrequirements cTIME2006, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 6.1.1: Use the test application to request the current STCF."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_STCF_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_TIME_DSTCFS 5

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_STCF

Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Request for $sc_$cpu_TST_TIME_STCF sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_STCF_SECS = ",$sc_$cpu_TT_STCF_SECS
write "TIME value of $sc_$cpu_TIME_DSTCFS = ", Time_DSTCFS
write "Time Test app returned value for $sc_$cpu_TT_STCF_SUBS = ",$sc_$cpu_TT_STCF_SUBS
write "TIME value of $sc_$cpu_Time_DSTCFSS = ", Time_DSTCFSSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2306) - Expected Event Message received."
   ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2306) - Expected event message ", TST_TIME_STCF_INF_EID, " not received."
   ut_setrequirements cTIME2306, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_STCF_SECS, Time_DSTCFSeconds) = 0 ) THEN
  write "<*> Passed (2306) - STCF Seconds set as expected."
  ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2306) - STCF Seconds did not get set properly."
  ut_setrequirements cTIME2306, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_STCF_SUBS, Time_DSTCFSSeconds) = 0 ) THEN
  write "<*> Passed (2306) - STCF Sub Seconds set as expected."
  ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2306) - STCF Sub Seconds did not get set properly."
  ut_setrequirements cTIME2306, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 6.1.2: Verify the new values of the time formats"
write ";"
write ";  NOTE: There is no requirement for difference between the computed "
write ";        values for TAI and UTC seconds & sub seconds. This was done as"
write ";        a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSeconds
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSeconds

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds Delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds Delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds Delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds Delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds

write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds Delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds Delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds Delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds Delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 6.2: Test command parameter minimum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_SetClockSTCF SECONDS=0 MICROSECS=0
write "Command $sc_$cpu_TIME_SetClockSTCF Seconds = to 0, MICROSECONDS = to 0 at ", CMD_TIME_1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2006) - Expected Event Message received."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - Expected event message ", CFE_TIME_STCF_EID, " not received."
  ut_setrequirements cTIME2006, "F"
ENDIF

IF ($sc_$cpu_TIME_DSTCFS = 0 ) THEN
  write "<*> Passed (2006) - STCF Seconds set as expected."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - STCF Seconds did not get set properly."
  ut_setrequirements cTIME2006, "F"
ENDIF

IF ($sc_$cpu_Time_DSTCFSS = 0 ) THEN
  write "<*> Passed (2000;2006) - STCF Sub Seconds set as expected."
  ut_setrequirements cTIME2000, "P"
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2000;2006) - STCF Sub Seconds did not get set properly."
  ut_setrequirements cTIME2000, "F"
  ut_setrequirements cTIME2006, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 6.2.1: Use the test application to request the current STCF."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_STCF_INF_EID, INFO, 1

ut_tlmupdate $sc_$cpu_TIME_DSTCFS 10

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_STCF

Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Request for $sc_$cpu_TST_TIME_STCF sent at ", CMD_TIME_1
wait 5

write "Time Test app returned value for $sc_$cpu_TT_STCF_SECS = ",$sc_$cpu_TT_STCF_SECS
write "TIME value of $sc_$cpu_TIME_DSTCFS = ", Time_DSTCFSeconds 
write "Time Test app returned value for $sc_$cpu_TT_STCF_SUBS = ",$sc_$cpu_TT_STCF_SUBS
write "TIME value of $sc_$cpu_Time_DSTCFSS = ", Time_DSTCFSSeconds 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2306) - Expected Event Message received."
   ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2306) - Expected event message ", TST_TIME_STCF_INF_EID, " not received."
   ut_setrequirements cTIME2306, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TT_STCF_SECS, Time_DSTCFSeconds) = 0 ) THEN
  write "<*> Passed (2006) - STCF Seconds set as expected."
  ut_setrequirements cTIME2306, "P"
ELSE
  write "<!> Failed (2006) - STCF Seconds did not get set properly."
  ut_setrequirements cTIME2306, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 6.2.2: Verify the new values of the time formats"
write ";"
write ";  NOTE: There is no requirement for difference between the computed "
write ";        values for TAI and UTC seconds & sub seconds. This was done as"
write ";        a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 7.0: Test the command to calculate the STCF given current time"
write ";"
write "; Requirements tested:"
write ";    cTIME2007"
write ";    cTIME2300"
write ";    cTIME2301"
write ";    cTIME2302"
write ";    cTIME2306"
write ";**********************************************************************"
write ";  Step 7.1: Test command parameter maximum value"
write ";"
write "; Requirement cTIME 2007 "
write ";    The STCF is = commanded time value - MET"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_TIME_EID, INFO, 1

/$sc_$cpu_TIME_SetClock SECONDS=MAX_TIME MICROSECS=MAX_MICROSEC

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS

wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2007) - Expected Event Message received."
  ut_setrequirements cTIME2007, "P"
ELSE
  write "<!> Failed (2007) - Expected event message ", CFE_TIME_TIME_EID, " not received."
  ut_setrequirements cTIME2007, "F"
ENDIF

write "Save the value of the MET (Seconds and Sub Seconds) "
write "Then compute the expected value of STCF based on the commanded time setting "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  Time_DSTCFSeconds
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds

Compute_Time_DSTCFSSeconds = MAX_MICRO_2_SSEC - Time_DMETSSeconds
Compute_Time_DSTCFSeconds = MAX_TIME - Time_DMETSeconds

write "Computed value of STCF Seconds is = ", Compute_Time_DSTCFSeconds
write "Computed value of STCF Sub Seconds is = ", Compute_Time_DSTCFSSeconds

write "Print out the current values of the TAI, MET, UTC, STCF and LeapSeconds"
write "Current TIME value of $sc_$cpu_Time_DTAIS = ",  $sc_$cpu_Time_DTAIS
write "Current TIME value of $sc_$cpu_Time_DTAISS = ",  $sc_$cpu_Time_DTAISS
write "Current TIME value of $sc_$cpu_Time_DMETS = ",  $sc_$cpu_Time_DMETS
write "Current TIME value of $sc_$cpu_Time_DMETSS = ",  $sc_$cpu_Time_DMETSS
write "Current TIME value of $sc_$cpu_Time_DUTCS = ",  $sc_$cpu_Time_DUTCS
write "Current TIME value of $sc_$cpu_Time_DUTCSS = ",  $sc_$cpu_Time_DUTCSS
write "Current TIME value of $sc_$cpu_TIME_DSTCFS = ",  $sc_$cpu_TIME_DSTCFS
write "Current TIME value of $sc_$cpu_Time_DSTCFSS = ",  $sc_$cpu_Time_DSTCFSS
write "Current TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLeapS 

write " Verify the new values are expected based on the modified STCF"

IF (%unsigned_cmp(Compute_Time_DSTCFSeconds, TIME_DSTCFSECONDS) = -1 ) THEN
    DELTA_Seconds = TIME_DSTCFSECONDS - Compute_Time_DSTCFSeconds
ELSE
    DELTA_Seconds = Compute_Time_DSTCFSeconds - TIME_DSTCFSECONDS
ENDIF

IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2007) - STCF Seconds delta is out of bounds."
  ut_setrequirements cTIME2007, "F"
ELSE
  write "<*> Passed (2007) - STCF Seconds delta is in bounds."
  ut_setrequirements cTIME2007, "P"
ENDIF

write "The STCF Delta seconds value is ", DELTA_Seconds

IF (%unsigned_cmp(Compute_Time_DSTCFSSeconds, TIME_DSTCFSSECONDS) = -1 ) THEN
    DELTA_SubSeconds = TIME_DSTCFSSECONDS - Compute_Time_DSTCFSSeconds
ELSE
    DELTA_SubSeconds = Compute_Time_DSTCFSeconds - TIME_DSTCFSSECONDS
ENDIF
IF (DELTA_Seconds > 8192) .OR. (DELTA_Seconds < -8192) THEN
  write "<!> Failed (2007) - STCF Sub Seconds delta is out of bounds."
  ut_setrequirements cTIME2007, "F"
ELSE
  write "<*> Passed (2007) - STCF Sub Seconds delta is in bounds."
  ut_setrequirements cTIME2007, "P"
ENDIF

write "The STCF Delta sub seconds (4096 dec = 1 Microsecond) value is ", DELTA_SubSeconds

wait 5

write ";**********************************************************************"
write "; Step 7.1.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write "; The STCF is = commanded time value - MET"
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1

write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1

write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 7.2: Test command parameter minimum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_TIME_EID, INFO, 1

/$sc_$cpu_TIME_SetClock SECONDS=0 MICROSECS=0

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS

wait 5
IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2007) - Expected Event Message received."
   ut_setrequirements cTIME2007, "P"
ELSE
  write "<!> Failed (2007) - Expected event message ", CFE_TIME_TIME_EID, " not received."
   ut_setrequirements cTIME2007, "F"
ENDIF

write "Save the value of the MET (Seconds and Sub Seconds) "
write "Then compute the expected value of STCF based on the commanded time setting "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds

Compute_Time_DSTCFSSeconds = 0 - Time_DMETSSeconds
Compute_Time_DSTCFSeconds = 0 - Time_DMETSeconds

write "Computed value of STCF Seconds is = ", Compute_Time_DSTCFSeconds
write "Computed value of STCF Sub Seconds (4096 dec = 1 Microsecond) is = ", Compute_Time_DSTCFSSeconds

write "Print out the current values of the TAI, MET, UTC, STCF and LeapS"
write "Current TIME value of $sc_$cpu_Time_DTAIS = ",  $sc_$cpu_Time_DTAIS
write "Current TIME value of $sc_$cpu_Time_DTAISS = ",  $sc_$cpu_Time_DTAISS
write "Current TIME value of $sc_$cpu_Time_DMETS = ",  $sc_$cpu_Time_DMETS
write "Current TIME value of $sc_$cpu_Time_DMETSS = ",  $sc_$cpu_Time_DMETSS
write "Current TIME value of $sc_$cpu_Time_DUTCS = ",  $sc_$cpu_Time_DUTCS
write "Current TIME value of $sc_$cpu_Time_DUTCSS = ",  $sc_$cpu_Time_DUTCSS
write "Current TIME value of $sc_$cpu_TIME_DSTCFS = ",  $sc_$cpu_TIME_DSTCFS
write "Current TIME value of $sc_$cpu_Time_DSTCFSS = ",  $sc_$cpu_Time_DSTCFSS
write "Current TIME value of $sc_$cpu_Time_DLeapS = ", $sc_$cpu_TIME_DLeapS 

write " Verify the new values are expected based on the modified STCF"

IF (%unsigned_cmp(Compute_Time_DSTCFSeconds, TIME_DSTCFSECONDS) = -1 ) THEN
    DELTA_Seconds = TIME_DSTCFSECONDS - Compute_Time_DSTCFSeconds
ELSE
    DELTA_Seconds = Compute_Time_DSTCFSeconds - TIME_DSTCFSECONDS
ENDIF

IF (DELTA_Seconds > 2) THEN
  write "<!> Failed (2007) - Computed STCF Seconds delta is out of bounds."
  ut_setrequirements cTIME2007, "F"
ELSE
  write "<*> Passed (2007) - Computed STCF Seconds delta is withing bounds."
  ut_setrequirements cTIME2007, "P"
ENDIF

write "The STCF Delta seconds value is ", DELTA_Seconds

IF (%unsigned_cmp(Compute_Time_DSTCFSSeconds, TIME_DSTCFSSECONDS) = -1 ) THEN
    DELTA_SubSeconds = TIME_DSTCFSSECONDS - Compute_Time_DSTCFSSeconds
ELSE
    DELTA_SubSeconds = Compute_Time_DSTCFSSeconds - TIME_DSTCFSSECONDS
ENDIF

IF (DELTA_Seconds > 8192) .OR. (DELTA_Seconds < -8192) THEN
  write "<!> Failed (2007) - STCF Sub Seconds delta is out of bounds."
  ut_setrequirements cTIME2007, "F"
ELSE
  write "<*> Passed (2007) - STCF Sub Seconds delta is in bounds."
  ut_setrequirements cTIME2007, "P"
ENDIF

write "The STCF Delta sub seconds value (4096 dec = 1 Microsecond) is ", DELTA_SubSeconds

wait 5

write ";**********************************************************************"
write "; Step 7.2.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFSECONDS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 7.3: Set the STCF back to the default setting. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO, 1

/$sc_$cpu_TIME_SetClockSTCF SECONDS=TIME_STCF_SECS_DEFAULT MICROSECS=0
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2006) - Expected Event Message received."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - Expected event message ", CFE_TIME_STCF_EID, " not received."
  ut_setrequirements cTIME2006, "F"
ENDIF

IF ($sc_$cpu_TIME_DSTCFS = 1000000 ) THEN
  write "<*> Passed (2006) - STCF Seconds set as expected."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - STCF Seconds did not get set properly."
  ut_setrequirements cTIME2006, "F"
ENDIF

IF ($sc_$cpu_Time_DSTCFSS = 0 ) THEN
  write "<*> Passed (2006) - STCF Sub Seconds set as expected."
  ut_setrequirements cTIME2006, "P"
ELSE
  write "<!> Failed (2006) - STCF Sub Seconds did not get set properly."
  ut_setrequirements cTIME2006, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 8.0: Test the command to add a 1 time adjustment to the STCF"
write ";"
write "; Requirements Tested:"
write ";    cTIME2008"
write ";    cTIME2306"
write ";**********************************************************************"
write ";  Step 8.1: Test command parameter maximum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO, 1

/$sc_$cpu_TIME_AddSTCFAdj SECONDS=MAX_TIME MICROSECS=MAX_MICROSEC
wait 5

write "Direction of one time STCF adjustment $sc_$cpu_TIME_DADJUSTDIR = ", $sc_$cpu_TIME_DADJUSTDIR
write "Value of one time STCF adjustment seconds $sc_$cpu_TIME_DADJUSTS = ", $sc_$cpu_TIME_DADJUSTS
write "Value of one time STCF adjustment subseconds $sc_$cpu_TIME_DADJUSTSS = ", $sc_$cpu_TIME_DADJUSTSS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2008) - Expected Event Message received."
   ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - Expected event message ", CFE_TIME_DELTA_EID, " not received."
   ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTDIR = 1) THEN
  write "<*> Passed (2008) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2006) - STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_DADJUSTS, MAX_TIME) = 0 )THEN
  write "<*> Passed (2008) - One Time STCF adjustment Seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment Seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

write ";; Comparing ", $sc_$cpu_TIME_DADJUSTSS, " with ", MAX_MICRO_2_SSEC

IF (%unsigned_cmp($sc_$cpu_TIME_DADJUSTSS, MAX_MICRO_2_SSEC) = 0 ) THEN
  write "<*> Passed (2008) - One Time STCF adjustment Sub Seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment Sub Seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

wait 5

write ";**********************************************************************"
write "; Step 8.1.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;
Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

local secondsDiff = Delta_Seconds - MET_Delta_Seconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSEIF (secondsDiff = 1) OR (secondsDiff = -1) THEN
  write "<*> Passed - TAI Seconds delta within threshold."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds(4096 dec = 1 Microsecond)  value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

secondsDiff = Delta_Seconds - MET_Delta_Seconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSEIF (secondsDiff = 1) OR (secondsDiff = -1) THEN
  write "<*> Passed - TAI Seconds delta within threshold."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 8.2: Test command parameter smallest positive value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO, 1

/$sc_$cpu_TIME_AddSTCFAdj SECONDS=1 MICROSECS=1
wait 5

write "Direction of one time STCF adjustment $sc_$cpu_TIME_DADJUSTDIR = ", $sc_$cpu_TIME_DADJUSTDIR
write "Value of one time STCF adjustment seconds $sc_$cpu_TIME_DADJUSTS = ", $sc_$cpu_TIME_DADJUSTS
write "Value of one time STCF adjustment subseconds $sc_$cpu_TIME_DADJUSTSS = ", $sc_$cpu_TIME_DADJUSTSS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2008) - Expected Event Message received."
   ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - Expected event message ", CFE_TIME_DELTA_EID, " not received."
   ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTDIR = 1) THEN
  write "<*> Passed (2008) - One Time STCF adjustment direction set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTS = 1) THEN
  write "<*> Passed (2008) - One Time STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_DADJUSTSS, ONE_MICROSEC_AS_SSEC) = 0 ) THEN
  write "<*> Passed (2008) - One Time STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

wait 5

write ";**********************************************************************"
write "; Step 8.2.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 8.3: Test command parameter minimum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO, 1

/$sc_$cpu_TIME_AddSTCFAdj SECONDS=0 MICROSECS=0
wait 5

write "Direction of one time STCF adjustment $sc_$cpu_TIME_DADJUSTDIR = ", $sc_$cpu_TIME_DADJUSTDIR
write "Value of one time STCF adjustment seconds $sc_$cpu_TIME_DADJUSTS = ", $sc_$cpu_TIME_DADJUSTS
write "Value of one time STCF adjustment subseconds $sc_$cpu_TIME_DADJUSTSS = ", $sc_$cpu_TIME_DADJUSTSS
write " "

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2008) - Expected Event Message received."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - Expected event message ", CFE_TIME_DELTA_EID, " not received."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTDIR = 1) THEN
  write "<*> Passed (2008) - One Time STCF adjustment direction set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTS = 0) THEN
  write "<*> Passed (2008) - One Time STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTSS = 0) THEN
  write "<*> Passed (2008) - One Time STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

wait 5

write ";**********************************************************************"
write "; Step 8.3.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF


IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 8.4: Send the commands to set the Clock Latency. "
write ";  NOTE: If the cFE is configured as a Time Server, these commands will"
write ";  	generate an error event."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELAY_CFG_EID, ERROR, 1 

/$SC_$CPU_TIME_AddClockLat SECONDS=1 MICROSECS=1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected error event generated."
else
  write "<!> Failed - Expected error event was not generated."
endif

ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELAY_CFG_EID, ERROR, 1

/$SC_$CPU_TIME_SubClockLat SECONDS=1 MICROSECS=1
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected error event generated."
else
  write "<!> Failed - Expected error event was not generated."
endif
wait 5

write ";**********************************************************************"
write ";  Step 9.0: Test the command to subtract a 1 time adjustment from the "
write ";            STCF"
write ";"
write "; Requirements tested:"
write ";    cTIME2008"
write ";    cTIME2306"
write ";**********************************************************************"
write ";  Step 9.1: Test command parameter maximum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO, 1

/$sc_$cpu_TIME_SubSTCFAdj SECONDS=MAX_TIME MICROSECS=MAX_MICROSEC
wait 5

write "Direction of one time STCF adjustment $sc_$cpu_TIME_DADJUSTDIR = ", $sc_$cpu_TIME_DADJUSTDIR
write "Value of one time STCF adjustment seconds $sc_$cpu_TIME_DADJUSTS = ", $sc_$cpu_TIME_DADJUSTS
write "Value of one time STCF adjustment subseconds $sc_$cpu_TIME_DADJUSTSS = ", $sc_$cpu_TIME_DADJUSTSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2008) - Expected Event Message received."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - Expected event message ", CFE_TIME_DELTA_EID, " not received."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTDIR = 2) THEN
  write "<*> Passed (2008) - One Time STCF adjustment direction set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_DADJUSTS, MAX_TIME) = 0 ) THEN
  write "<*> Passed (2008) - One Time STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_DADJUSTSS, MAX_MICRO_2_SSEC) = 0 ) THEN
  write "<*> Passed (2008) - One Time STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

wait 5

write ";**********************************************************************"
write "; Step 9.1.1: Verify the new values of the time formats"
write "; "
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 9.2: Test command parameter smallest positive value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO, 1

/$sc_$cpu_TIME_SubSTCFAdj SECONDS=1 MICROSECS=1
wait 5

write "Direction of one time STCF adjustment $sc_$cpu_TIME_DADJUSTDIR = ", $sc_$cpu_TIME_DADJUSTDIR
write "Value of one time STCF adjustment seconds$sc_$cpu_TIME_DADJUSTS = ", $sc_$cpu_TIME_DADJUSTS
write "Value of one time STCF adjustment subseconds $sc_$cpu_TIME_DADJUSTSS = ", $sc_$cpu_TIME_DADJUSTSS
write " "

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2008) - Expected Event Message received."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - Expected event message ", CFE_TIME_DELTA_EID, " not received."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTDIR = 2) THEN
  write "<*> Passed (2008) - One Time STCF adjustment direction set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTS = 1) THEN
  write "<*> Passed (2008) - One Time STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTSS = 4096) THEN
  write "<*> Passed (2008) - One Time STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 9.2.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 9.3: Test command parameter minimum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO, 1

/$sc_$cpu_TIME_SubSTCFAdj SECONDS=0 MICROSECS=0
wait 5

write "Direction of one time STCF adjustment $sc_$cpu_TIME_DADJUSTDIR = ", $sc_$cpu_TIME_DADJUSTDIR
write "Value of one time STCF adjustment seconds $sc_$cpu_TIME_DADJUSTS = ", $sc_$cpu_TIME_DADJUSTS
write "Value of one time STCF adjustment subseconds $sc_$cpu_TIME_DADJUSTSS = ", $sc_$cpu_TIME_DADJUSTSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2008) - Expected Event Message received."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - Expected event message ", CFE_TIME_DELTA_EID, " not received."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTDIR = 2) THEN
  write "<*> Passed (2008) - One Time STCF adjustment direction set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTS = 0) THEN
  write "<*> Passed (2008) - One Time STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

IF ($sc_$cpu_TIME_DADJUSTSS = 0) THEN
  write "<*> Passed (2008) - One Time STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2008, "P"
ELSE
  write "<!> Failed (2008) - One Time STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2008, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 9.3.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "Print out the saved values of the TAI, MET, UTC and LeapSeconds "
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond)  Value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds

IF (Delta_Seconds = MET_Delta_Seconds) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write "; Step 10.0: Test the command to add a continuous adjustment to the STCF"
write ";"
write ";  Requirements tested:"
write ";    cTIME 2009"
write ";    cTIME 2306"
write ";**********************************************************************"
write ";  Step 10.1: Test command parameter maximum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_Add1HzSTCF SECONDS=MAX_TIME SUBSECS=MAX_TIME
wait 5

write "Command $sc_$cpu_TIME_Add1HzSTCF Seconds = FFFFFFFF (-1), Subseconds = FFFFFFFF at ", CMD_TIME_1
write "Direction of 1 Hz STCF adjustment $sc_$cpu_TIME_D1HZADJDIR = ", $sc_$cpu_TIME_D1HZADJDIR
write "Value of 1 Hz STCF adjustment seconds $sc_$cpu_TIME_D1HZADJS = ", $sc_$cpu_TIME_D1HZADJS
write "Value of 1 Hz STCF adjustment subseconds $sc_$cpu_TIME_D1HZADJSS = ", $sc_$cpu_TIME_D1HZADJSS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", CFE_TIME_1HZ_EID, " not received."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJDIR = 1) THEN
  write "<*> Passed (2009) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_D1HZADJS, MAX_TIME) = 0 ) THEN
  write "<*> Passed (2009) - STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_D1HZADJSS, MAX_TIME) = 0 ) THEN
  write "<*> Passed (2009) - STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 10.1.1: Verify the new values of the STCF"
write ";**********************************************************************"
write "; wait for new STCF Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Current values of STCF and MET collected at ", CMD_TIME_1
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "New values of STCF collected at ", CMD_TIME_1
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write " For this computation the expected value for the STCF Seconds would not change due to "
write " the 1 HZ change for seconds to be equivalent to -1 per second. The net change in seconds "
write " would be zero (no change). "

IF (Time_DSTCFSeconds != New_DSTCFSeconds) THEN
  write "<!> Failed (2009) - The two values of STCF Seconds do not match."
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - The two values of STCF Seconds match."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " The subseconds field has been commanded to increase by -1 per second. "
write " This changes along with the -1 second commanded for seconds will have the net effect of making the "
write " subseconds to decrement by one subsecond each second. "

write " Compute the elapsed time"

DELTA_Seconds = New_DMETSeconds - Time_DMETSeconds

write " Compute the expected STCF SubSeconds "

Expected_subseconds = (Time_DSTCFSSeconds - New_DSTCFSSeconds) / DELTA_Seconds

IF (Expected_subseconds != 1) THEN
  write "<!> Failed (2009) - The STCF Sub Seconds are not as expected."
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - The STCF Sub Seconds match the expected result."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " #### The value computed for the Delta Seconds is ", DELTA_Seconds
write " ###### The value computed for the expected STCF Sub Seconds is ", Expected_subseconds

wait 5

write ";**********************************************************************"
write ";  Step 10.1.2: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
; 
;

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds
;;MET_Delta_Subseconds = (New_DMETSSeconds - Time_DMETSSeconds) / MET_Delta_Seconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds
;;Delta_Subseconds = (Time_DTAISSeconds - New_DTAISSeconds) / Delta_Seconds 
local Delta_Seconds_diff
if (MET_Delta_Seconds > Delta_Seconds) then
  Delta_Seconds_diff = MET_Delta_Seconds - Delta_Seconds
else
  Delta_Seconds_diff = Delta_Seconds - MET_Delta_Seconds
endif

IF (Delta_Seconds_diff < 2) THEN
  write "<*> Passed - TAI Seconds delta is in bounds."
ELSE
  write "<!> Failed - TAI Seconds delta is out of bounds."
ENDIF

;;IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
;;IF (Delta_SubSeconds = ONE_MICROSEC_AS_SSEC) THEN
;;  write "<*> Passed - TAI Sub Seconds delta set as expected."
;;ELSE
;;  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
;;ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds
;;Delta_Subseconds = (Time_DUTCSSeconds - New_DUTCSSeconds) / Delta_Seconds

if (MET_Delta_Seconds > Delta_Seconds) then
  Delta_Seconds_diff = MET_Delta_Seconds - Delta_Seconds
else
  Delta_Seconds_diff = Delta_Seconds - MET_Delta_Seconds
endif

IF (Delta_Seconds_diff < 2) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

;;IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
;;IF (Delta_SubSeconds = ONE_MICROSEC_AS_SSEC) THEN
;;  write "<*> Passed - UTC Sub Seconds delta set as expected."
;;ELSE
;;  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
;;ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "The MET Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds (4096 dec = 1 Microsecond) value is ", Delta_SubSeconds

wait 5

write ";**********************************************************************"
write ";  Step 10.2: Test command parameter minimum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_ADD1HZSTCF SECONDS=1 SUBSECS=1
wait 5
write "Command $sc_$cpu_TIME_Add1HzSTCF, Seconds = 1, Subseconds = 1 at ", CMD_TIME_1
write "Direction of 1 Hz STCF adjustment $sc_$cpu_TIME_D1HZADJDIR = ", P@$sc_$cpu_TIME_D1HZADJDIR
write "Value of 1 Hz STCF adjustment seconds $sc_$cpu_TIME_D1HZADJS = ", $sc_$cpu_TIME_D1HZADJS
write "Value of 1 Hz STCF adjustment subseconds $sc_$cpu_TIME_D1HZADJSS = ", $sc_$cpu_TIME_D1HZADJSS
write " "

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", CFE_TIME_1HZ_EID, " not received."
   ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJDIR = 1) THEN
  write "<*> Passed (2009) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJS = 1) THEN
  write "<*> Passed (2009) - STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_D1HZADJSS, 1) = 0 ) THEN
  write "<*> Passed (2009) - STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 10.2.1: Verify the new values of the STCF"
write ";**********************************************************************"
write "; wait for new STCF Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Current values of STCF and MET collected at ", CMD_TIME_1
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "New values of STCF collected at ", CMD_TIME_1
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds

write " For this computation the expected value for the STCF Seconds would increase by 1 each second  "

write " Compute the elapsed time"

DELTA_Seconds = New_DMETSeconds - Time_DMETSeconds

write " Compute the expected STCF Seconds "

Expected_Seconds = Time_DSTCFSeconds + DELTA_Seconds

IF (Expected_Seconds != New_DSTCFSeconds) THEN
  write "<!> Failed (2009) - The two values of STCF Seconds do not match."
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - The two values of STCF Seconds match."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " The subseconds field has been commanded to increase by 1 per second. "

Expected_subseconds = (New_DSTCFSSeconds - Time_DSTCFSSeconds) / DELTA_Seconds

IF (Expected_subseconds != 1) THEN
  write "<!> Failed (2009) - STCF Sub Seconds do not match the expected results."
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - STCF Sub Seconds match the expected result."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " #### The value computed for the Delta Seconds is ", DELTA_Seconds
write " #### The value computed for the STCF Seconds is ", Expected_Seconds
write " ###### The value computed for the expected STCF Sub Seconds is ", Expected_subseconds

wait 5

write ";**********************************************************************"
write ";  Step 10.2.2: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new MET Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DMETSSeconds = $sc_$cpu_Time_DMETSS
Time_DTAISeconds = $sc_$cpu_Time_DTAIS
Time_DTAISSeconds = $sc_$cpu_Time_DTAISS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
Time_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
Time_DUTCSeconds = $sc_$cpu_Time_DUTCS
Time_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "Current values of time formats collected at ", CMD_TIME_1
write "Saved TIME value of $sc_$cpu_Time_DTAIS = ",  Time_DTAISeconds
write "Saved TIME value of $sc_$cpu_Time_DTAISS = ",  Time_DTAISSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_Time_DMETSS = ",  Time_DMETSSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCS = ",  Time_DUTCSeconds
write "Saved TIME value of $sc_$cpu_Time_DUTCSS = ",  Time_DUTCSSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds
write "Saved TIME value of $sc_$cpu_Time_DLeapS = ", TIME_DLEAPSECONDS 

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DMETSSeconds = $sc_$cpu_Time_DMETSS
New_DTAISeconds = $sc_$cpu_Time_DTAIS
New_DTAISSeconds = $sc_$cpu_Time_DTAISS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS
New_DLeapSeconds = $sc_$cpu_TIME_DLEAPS
New_DUTCSeconds = $sc_$cpu_Time_DUTCS
New_DUTCSSeconds = $sc_$cpu_Time_DUTCSS

write "New values of time formats collected at ", CMD_TIME_1
write "New TIME value of $sc_$cpu_Time_DTAIS = ",  New_DTAISeconds
write "New TIME value of $sc_$cpu_Time_DTAISS = ",  New_DTAISSeconds
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_Time_DMETSS = ",  New_DMETSSeconds
write "New TIME value of $sc_$cpu_Time_DUTCS = ",  New_DUTCSeconds
write "New TIME value of $sc_$cpu_Time_DUTCSS = ",  New_DUTCSSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds
write "New TIME value of $sc_$cpu_Time_DLeapS = ", New_DLEAPSECONDS 

;
;  Compute the MET delta seconds and subseconds values 
; 

MET_Delta_Seconds = New_DMETSeconds - Time_DMETSeconds 
MET_Delta_Subseconds = New_DMETSSeconds - Time_DMETSSeconds

;
;  Compute the delta seconds and subseconds values for TAI 
;

Delta_Seconds = New_DTAISeconds - Time_DTAISeconds
local Expected_seconds = (Delta_Seconds - MET_Delta_Seconds) / MET_Delta_Seconds
Delta_Subseconds = New_DTAISSeconds - Time_DTAISSeconds
Expected_subseconds = (Delta_Subseconds - MET_Delta_Subseconds) / MET_Delta_Seconds

;;IF (Delta_Seconds = MET_Delta_Seconds) THEN
IF (Expected_seconds = 1) THEN
  write "<*> Passed - TAI Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Seconds delta did not get set properly."
ENDIF

IF (Expected_subSeconds = 1) THEN
  write "<*> Passed - TAI Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - TAI Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The TAI Delta Seconds value is ", Delta_Seconds
write "Expected seconds Value is ", Expected_seconds
write "The MET Delta Sub Seconds Value is ", MET_Delta_SubSeconds
write "The TAI Delta Sub Seconds Value is ", Delta_SubSeconds
write "Expected Subseconds Value is ", Expected_subseconds

;
;  Compute the delta seconds and subseconds values for UTC 
;

Delta_Seconds = New_DUTCSeconds - Time_DUTCSeconds
Expected_seconds = (Delta_Seconds - MET_Delta_Seconds) / MET_Delta_Seconds
Delta_Subseconds = New_DUTCSSeconds - Time_DUTCSSeconds
Expected_subseconds = (Delta_Subseconds - MET_Delta_Subseconds) / MET_Delta_Seconds

IF (Expected_seconds = 1) THEN
  write "<*> Passed - UTC Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Seconds delta did not get set properly."
ENDIF

;;IF (Delta_SubSeconds = MET_Delta_SubSeconds) THEN
IF (Expected_subSeconds = 1) THEN
  write "<*> Passed - UTC Sub Seconds delta set as expected."
ELSE
  write "<!> Failed - UTC Sub Seconds delta did not get set properly."
ENDIF

write "The MET Delta seconds value is ", MET_Delta_Seconds
write "The UTC Delta Seconds value is ", Delta_Seconds
write "Expected seconds Value is ", Expected_seconds
write "The MET Delta Sub Seconds Value is ", MET_Delta_SubSeconds
write "The UTC Delta Sub Seconds Value is ", Delta_SubSeconds
write "Expected Subseconds Value is ", Expected_subseconds

wait 5

write ";**********************************************************************"
write "; Step 10.3: Test command to stop the continuous adjustment to the "
write ";            STCF (add)"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_StopAdd1Hz
write "Command $sc_$cpu_TIME_StopAdd1Hz sent at ", CMD_TIME_1
wait 5

write "Direction of 1 Hz STCF adjustment $sc_$cpu_TIME_D1HZADJDIR = ", $sc_$cpu_TIME_D1HZADJDIR
write "Value of 1 Hz STCF adjustment seconds $sc_$cpu_TIME_D1HZADJS = ", $sc_$cpu_TIME_D1HZADJS
write "Value of 1 Hz STCF adjustment subseconds $sc_$cpu_TIME_D1HZADJSS = ", $sc_$cpu_TIME_D1HZADJSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", CFE_TIME_1HZ_EID, " not received."
   ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJDIR = 1) THEN
  write "<*> Passed (2009) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJS = 0) THEN
  write "<*> Passed (2009) - STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2008) - STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJSS = 0) THEN
  write "<*> Passed (2009) - STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2008) - STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

wait 5

write ";**********************************************************************"
write "; Step 11.0: Test the command to subtract a continuous adjustment from "
write ";            the STCF"
write ";**********************************************************************"
write ";  Step 11.1: Test command parameter maximum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_Sub1HzSTCF SECONDS=MAX_TIME SUBSECS=MAX_TIME
wait 5

write "Command $sc_$cpu_TIME_Sub1HzSTCF Seconds = FFFFFFFF, Subseconds = FFFFFFFF at ", CMD_TIME_1
write "Direction of 1 Hz STCF adjustment $sc_$cpu_TIME_D1HZADJDIR = ", $sc_$cpu_TIME_D1HZADJDIR
write "Value of 1 Hz STCF adjustment seconds $sc_$cpu_TIME_D1HZADJS = ", $sc_$cpu_TIME_D1HZADJS
write "Value of 1 Hz STCF adjustment subseconds $sc_$cpu_TIME_D1HZADJSS = ", $sc_$cpu_TIME_D1HZADJSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", CFE_TIME_1HZ_EID, " not received."
ENDIF

IF ($sc_$cpu_TIME_D1HZADJDIR = 2) THEN
  write "<*> Passed (2009) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_D1HZADJS, MAX_TIME) = 0 ) THEN
  write "<*> Passed (2009) - STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF (%unsigned_cmp($sc_$cpu_TIME_D1HZADJSS, MAX_TIME) = 0 ) THEN
  write "<*> Passed (2009) - STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 11.1.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new STCF Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Current values of STCF and MET collected at ", CMD_TIME_1
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "New values of STCF collected at ", CMD_TIME_1
write "Print out the 2nd saved values of the STCF and MET "
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds

write " For this computation the expected value for the STCF Seconds would not change due to "
write " the 1 HZ change for seconds to be equivalent to +1 per second. The net change in seconds "
write " would be zero (no change). "

IF (Time_DSTCFSeconds != New_DSTCFSeconds) THEN
  write "<!> Failed (2009) - The two values of STCF Seconds do not match."
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - The two values of STCF Seconds match."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " The subseconds field has been commanded to decrease by 1 per second. "
write " This changes along with the -1 second commanded for seconds will have the net effect of making the "
write " subseconds to increment by one each second. "

write " Compute the elapsed time"

DELTA_Seconds = New_DMETSeconds - Time_DMETSeconds

write "The elapsed time is ", DELTA_Seconds
write " Compute the expected STCF SubSeconds "

Expected_subseconds = (New_DSTCFSSeconds - Time_DSTCFSSeconds) / DELTA_Seconds

IF (Expected_subseconds != 1) THEN
  write "<!> Failed (2009) - STCF Sub Seconds does not match the expected result"
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - STCF Sub Seconds matches the expected result."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " #### The value computed for the Delta Seconds is ", DELTA_Seconds
write " #### The value computed for the STCF Seconds is ", Expected_Seconds
write " ###### The value computed for the STCF Sub Seconds is ", Expected_subseconds

wait 5

write ";**********************************************************************"
write ";  Step 11.2: Test command parameter minimum value"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO, 1

CMD_TIME_1 =%GMT
/$sc_$cpu_TIME_Sub1HzSTCF SECONDS=1 SUBSECS=1
wait 5

write "Command $sc_$cpu_TIME_Sub1HzSTCF Seconds = 1, Subseconds = 1 at ", CMD_TIME_1
write "Direction of 1 Hz STCF adjustment $sc_$cpu_TIME_D1HZADJDIR = ", $sc_$cpu_TIME_D1HZADJDIR
write "Value of 1 Hz STCF adjustment seconds $sc_$cpu_TIME_D1HZADJS = ", $sc_$cpu_TIME_D1HZADJS
write "Value of 1 Hz STCF adjustment subseconds $sc_$cpu_TIME_D1HZADJSS = ", $sc_$cpu_TIME_D1HZADJSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", CFE_TIME_1HZ_EID, " not received."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJDIR = 2) THEN
  write "<*> Passed (2009) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJS = 1) THEN
  write "<*> Passed (2009) - STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJSS = 1 ) THEN
  write "<*> Passed (2009) - STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 11.2.1: Verify the new values of the time formats"
write ";"
write "; NOTE: There is no requirement for difference between the computed "
write ";       values for TAI and UTC seconds & sub seconds. This was done as "
write ";       a verification that the TIME computed the values as expected. "
write ";**********************************************************************"
write "; wait for new STCF Telemetry "

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

Time_DMETSeconds = $sc_$cpu_Time_DMETS
Time_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
Time_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "Current values of STCF and MET collected at ", CMD_TIME_1
write "Saved TIME value of $sc_$cpu_Time_DMETS = ",  Time_DMETSeconds
write "Saved TIME value of $sc_$cpu_TIME_DSTCFS = ",  TIME_DSTCFSECONDS
write "Saved TIME value of $sc_$cpu_Time_DSTCFSS = ",  Time_DSTCFSSeconds

write " Wait for approximately 20 seconds and then compare the values to those saved earlier"

wait 18

ut_tlmupdate $sc_$cpu_Time_DMETS 5

CMD_TIME_1 =%GMT

New_DMETSeconds = $sc_$cpu_Time_DMETS
New_DSTCFSeconds = $sc_$cpu_TIME_DSTCFS
New_DSTCFSSeconds = $sc_$cpu_Time_DSTCFSS

write "New values of STCF collected at ", CMD_TIME_1
write "Print out the 2nd saved values of the STCF and MET "
write "New TIME value of $sc_$cpu_Time_DMETS = ",  New_DMETSeconds
write "New TIME value of $sc_$cpu_TIME_DSTCFS = ",  New_DSTCFSECONDS
write "New TIME value of $sc_$cpu_Time_DSTCFSS = ",  New_DSTCFSSeconds

write " Compute the elapsed time"

DELTA_Seconds = New_DMETSeconds - Time_DMETSeconds

write " Compute the expected STCF Seconds "
write " For this computation the expected value for the STCF Seconds would decrease by 1 each second  "

Expected_Seconds = Time_DSTCFSeconds - New_DSTCFSeconds 
;;;Expected_Seconds = New_DSTCFSeconds - Time_DSTCFSeconds

IF (Expected_Seconds != DELTA_Seconds) THEN
  write "<!> Failed (2009) - The two values of STCF Seconds do not match."
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - The two values of STCF Seconds match."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " The subseconds field has been commanded to increase by 1 per second. "

Expected_subseconds = (Time_DSTCFSSeconds - New_DSTCFSSeconds) / DELTA_Seconds

IF (Expected_subseconds != 1) THEN
  write "<!> Failed (2009) - STCF Sub Seconds does not match the expected result"
  ut_setrequirements cTIME2009, "F"
ELSE
  write "<*> Passed (2009) - STCF Sub Seconds matches the expected result."
  ut_setrequirements cTIME2009, "P"
ENDIF

write " #### The value computed for the Delta Seconds is ", DELTA_Seconds
write " #### The value computed for the STCF Seconds is ", Expected_Seconds
write " ###### The value computed for the STCF Sub Seconds is ", Expected_subseconds

wait 5

write ";**********************************************************************"
write "; Step 11.3: Test command to stop the continuous adjustment to the "
write ";            STCF (subtract)"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TIME_StopSub1Hz
wait 5

write "Command $sc_$cpu_TIME_StopSub1Hz at ", CMD_TIME_1
write "Direction of 1 Hz STCF adjustment $sc_$cpu_TIME_D1HZADJDIR = ", $sc_$cpu_TIME_D1HZADJDIR
write "Value of 1 Hz STCF adjustment seconds $sc_$cpu_TIME_D1HZADJS = ", $sc_$cpu_TIME_D1HZADJS
write "Value of 1 Hz STCF adjustment subseconds $sc_$cpu_TIME_D1HZADJSS = ", $sc_$cpu_TIME_D1HZADJSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", CFE_TIME_1HZ_EID, " not received."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJDIR = 2) THEN
  write "<*> Passed (2009) - STCF adjustment direction set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - One Time STCF adjustment direction was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJS = 0) THEN
  write "<*> Passed (2009) - STCF adjustment seconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - STCF adjustment seconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

IF ($sc_$cpu_TIME_D1HZADJSS = 0) THEN
  write "<*> Passed (2009) - STCF adjustment subseconds set as expected."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2008) - STCF adjustment subseconds was not set properly."
  ut_setrequirements cTIME2009, "F"
ENDIF

wait 5

write " Enable all DEBUG message types from EVT Services "
/$SC_$CPU_EVS_ENAEVENTTYPEMASK BITMASK=x'1'

write ";**********************************************************************"
write ";  Step 12.0: Test the reset counters command"
write ";"
write "; Requirement cTIME 2002 (Partial)"
write ";"
write "; NOTE: Only the Command Processed and Command Error counters are tested"
write "; in this step"
write ";**********************************************************************"
write ";  Step 12.1: Verify all counters are non zero"
write ";**********************************************************************"
write "Counter value for $sc_$cpu_Time_CMDPC = ", $sc_$cpu_Time_CMDPC
write "Counter value for $sc_$cpu_Time_CMDEC = ", $sc_$cpu_Time_CMDEC
write "Counter value for $sc_$cpu_Time_DVerifyCNT = ", $sc_$cpu_Time_DVerifyCNT
write "Counter value for $sc_$cpu_Time_DVerifyER = ", $sc_$cpu_Time_DVerifyER
write "Counter value for $sc_$cpu_Time_DTSDetectCNT = ", $sc_$cpu_Time_DTSDetCNT
write "Counter value for $sc_$cpu_Time_DTatTSBCNT = ", $sc_$cpu_Time_DTatTCNT
write "Counter value for $sc_$cpu_Time_DTsTaskCNT = ", $sc_$cpu_Time_DTsTaskCNT
write "Counter value for $sc_$cpu_Time_DVersionCNT = ", $sc_$cpu_Time_DVersionCNT
write "Counter value for $sc_$cpu_Time_D1HzISRCNT = ", $sc_$cpu_Time_D1HzISRCNT
write "Counter value for $sc_$cpu_Time_D1HzTaskCNT = ", $sc_$cpu_Time_D1HzTaskCNT
write " "
wait 5

IF ($sc_$cpu_Time_CMDPC <> 0) THEN
   write "$sc_$cpu_Time_CMDPC is non-zero"
ELSE
   write "$sc_$cpu_Time_CMDPC is zero, sending a command to cause it to increment"
   /$sc_$cpu_TIME_NOOP
   wait 5
   write "$sc_$cpu_Time_CMDPC = ", $sc_$cpu_Time_CMDPC
ENDIF

IF ($sc_$cpu_Time_CMDEC <> 0 ) THEN
   write "$sc_$cpu_Time_CMDEC is non-zero"
ELSE
   write  "$sc_$cpu_Time_CMDEC is zero, sending a command to cause it to increment "
   ut_sendrawcmd "$sc_$cpu_TIME", {rawCmd}
   wait 5
   write "$sc_$cpu_Time_CMDEC = ", $sc_$cpu_Time_CMDEC
ENDIF

IF ($sc_$cpu_Time_DVerifyCNT <> 0 ) THEN
   write "$sc_$cpu_Time_DVerifyCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DVerifyCNT is zero"
ENDIF

IF ($sc_$cpu_Time_DVerifyER <> 0 ) THEN
   write "$sc_$cpu_Time_DVerifyER is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DVerifyER is zero"
ENDIF

IF ($sc_$cpu_Time_DTSDetCNT  <> 0 ) THEN
   write "$sc_$cpu_Time_DTSDetCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DTSDetCNT is zero"
ENDIF

IF ($sc_$cpu_Time_DTatTCNT <> 0 ) THEN
   write "$sc_$cpu_Time_DTatBCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DTatTSBCNT is zero"
ENDIF

IF ($sc_$cpu_Time_DTsISRCNT <> 0 ) THEN
   write "$sc_$cpu_Time_DTsISRCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DTsISRCNT is zero"
ENDIF

IF ( $sc_$cpu_Time_DTsTaskCNT <> 0) THEN
   write "$sc_$cpu_Time_DTsTaskCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DTsTaskCNT is zero"
ENDIF

IF ( $sc_$cpu_Time_DVersionCNT <> 0) THEN
   write "$sc_$cpu_Time_DVersionCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_DVersionCNT is zero"
ENDIF

IF ($sc_$cpu_Time_D1HzISRCNT <> 0 ) THEN
   write "$sc_$cpu_Time_D1HzISRCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_D1HzISRCNT is zero"
ENDIF

IF ( $sc_$cpu_Time_D1HzTaskCNT <> 0) THEN
   write "$sc_$cpu_Time_D1HzTaskCNT is non-zero"
ELSE
   write "***** WARNING ****** $sc_$cpu_Time_D1HzTaskCNT is zero"
ENDIF

write "All counters tested"
wait 5

write ";**********************************************************************"
write ";  Step 12.2: Command the counters to reset"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_RESET_EID, DEBUG, 1

/$sc_$cpu_TIME_ResetCtrs
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
   ut_setrequirements cTIME2002, "P"
ELSE
   ut_setrequirements cTIME2002, "F"
ENDIF

write "Counter value for $sc_$cpu_Time_CMDPC = ", $sc_$cpu_Time_CMDPC
write "Counter value for $sc_$cpu_Time_CMDEC = ", $sc_$cpu_Time_CMDEC
;;write "Counter value for $sc_$cpu_TimeDVerifyCNT = ", $sc_$cpu_TimeDVerifyCNT
;;write "Counter value for $sc_$cpu_TimeDVerifyER = ", $sc_$cpu_TimeDVerifyER
;;write "Counter value for $sc_$cpu_TimeDTSDetectCNT = ", $sc_$cpu_TimeDTSDetectCNT
;;write "Counter value for $sc_$cpu_TimeDTatTSBCNT = ", $sc_$cpu_TimeDTatTSBCNT
;;write "Counter value for $sc_$cpu_TimeDTsTaskCNT = ", $sc_$cpu_TimeDTsTaskCNT
;;write "Counter value for $sc_$cpu_TimeDVersionCNT = ", $sc_$cpu_TimeDVersionCNT
;;write "Counter value for $sc_$cpu_TimeD1HzISRCNT = ", $sc_$cpu_TimeD1HzISRCNT
;;write "Counter value for $sc_$cpu_TimeD1HzTaskCNT = ", $sc_$cpu_TimeD1HzTaskCNT

write ";**********************************************************************"
write ";  Step 12.3: Verify all counters are zero"
write ";**********************************************************************"

IF ($sc_$cpu_Time_CMDPC < 6) THEN
  write "<*> Passed (2002) - $sc_$cpu_Time_CMDPC was <= 5"
  ut_setrequirements cTIME2002, "P"
ELSE
  write "<!> Failed (2002) - $sc_$cpu_Time_CMDPC did not reset."
  ut_setrequirements cTIME2002, "F"
ENDIF

IF ($sc_$cpu_Time_CMDEC = 0 ) THEN
  write "<*> Passed (2002) - $sc_$cpu_Time_CMDEC reset."
  ut_setrequirements cTIME2002, "P"
ELSE
  write "<!> Failed (2002) - $sc_$cpu_Time_CMDEC did not reset."
  ut_setrequirements cTIME2002, "F"
ENDIF

;;IF ($sc_$cpu_TimeDVerifyCNT = 0 ) THEN
;;  write "<*> Passed - $sc_$cpu_TimeDVerifyCNT reset"
;;ELSE
;;  write "<!> Failed - $sc_$cpu_TimeDVerifyCNT did not reset."
;;ENDIF
;;
;;IF ($sc_$cpu_TimeDVerifyER = 0 ) THEN
;;  write "<*> Passed - $sc_$cpu_TimeDVerifyER reset."
;;ELSE
;;  write "<!> Failed - $sc_$cpu_TimeDVerifyER did not reset."
;;ENDIF
;;
;;IF ($sc_$cpu_TimeDTSDetectCNT  = 0 ) THEN
;;   write "<*> Passed - $sc_$cpu_TimeDTSDetectCNT reset."
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeDTSDetectCNT did not reset."
;;ENDIF
;;
;;IF ($sc_$cpu_TimeDTatTSBCNT = 0 ) THEN
;;   write "<*> Passed - $sc_$cpu_TimeDTatTSBCNT reset."
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeDTatTSBCNT did not reset."
;;ENDIF
;;
;;IF ($sc_$cpu_TimeDTsISRCNT = 0 ) THEN
;;   write "<*> Passed - $sc_$cpu_TimeDTsISRCNT reset."
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeDTsISRCNT did not reset."
;;ENDIF
;;
;;IF ( $sc_$cpu_TimeDTsTaskCNT = 0) THEN
;;   write "<*> Passed - $sc_$cpu_TimeDTsTaskCNT reset."
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeDTsTaskCNT did not reset."
;;ENDIF
;;
;;IF ( $sc_$cpu_TimeDVersionCNT = 0) THEN
;;   write "<*> Passed - $sc_$cpu_TimeDVersionCNT reset."
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeDVersionCNT did not reset."
;;ENDIF
;;
;;IF ($sc_$cpu_TimeD1HzISRCNT = 0 ) THEN
;;   write "<*> Passed - $sc_$cpu_TimeD1HzISRCNT reset."
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeD1HzISRCNT did not reset."
;;ENDIF
;;
;;IF ( $sc_$cpu_TimeD1HzTaskCNT = 0) THEN
;;   write "<*> Passed - Counter $sc_$cpu_TimeD1HzTaskCNT is zero"
;;ELSE
;;   write "<!> Failed - $sc_$cpu_TimeD1HzTaskCNT did not reset."
;;ENDIF

write "All counters tested"
wait 5

write " Disable all DEBUG message types from EVT Services "
/$SC_$CPU_EVS_DISEVENTTYPE DEBUG 

write ";**********************************************************************"
write ";  Step 13.0: Test the request to add two time values"
write ";"
write "; Requirement cTIME2309"
write ";**********************************************************************"
LOCAL TIME_ADD_SEC_RESULT
LOCAL TIME_ADD_SUB_RESULT
LOCAL TELEM_TIME_ADD_SEC_RESULT
LOCAL TELEM_TIME_ADD_SUB_RESULT

ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_ADDTIME_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_TIMEADD ADD_TIME1_SECONDS=x'EE6B28' ADD_TIME1_SUBSECS=x'80000000' ADD_TIME2_SECONDS=x'11E1A3' ADD_TIME2_SUBSECS=x'A0000000' 

write "Request to add two numbers sent at ", CMD_TIME_1
write " >>>> TIME ADD EXPECTED RESULTS <<<<< "
write "Time 1 seconds is 15625000, time 2 is 1171875, sum is 16796875 "
write "Time 1 Sub Seconds is 2147483648, time 2 Sub Seconds is 2684354560, sum is 536870912 and should add 1 to seconds (16796875 + 1)"
write "Result of add SECONDS = 16796876 "
write "Result of add SUBSECONDS = 536870912 "
wait 5 

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2009) - Expected Event Message received."
  ut_setrequirements cTIME2009, "P"
ELSE
  write "<!> Failed (2009) - Expected event message ", TST_TIME_ADDTIME_INF_EID, " not received."
  ut_setrequirements cTIME2309, "F"
ENDIF

write "The result of the time add is $sc_$cpu_TT_ADD_SECS = ", $sc_$cpu_TT_ADD_SECS
write "The result of the time add is $sc_$cpu_TT_ADD_SUBS = ", $sc_$cpu_TT_ADD_SUBS

IF ($sc_$cpu_TT_ADD_SECS = ADD_SECONDS_RESULT) THEN
  write "<*> Passed (2309) - Added Seconds are correct."
  ut_setrequirements cTIME2309, "P"
ELSE
  write "<!> Failed (2309) - Added Seconds not equal to expected result."
  ut_setrequirements cTIME2309, "F"
ENDIF

IF ($sc_$cpu_TT_ADD_SUBS = ADD_SUBSECONDS_RESULT) THEN
  write "<*> Passed (2309) - Added Sub Seconds are correct."
  ut_setrequirements cTIME2309, "P"
ELSE
  write "<!> Failed (2309) - Added Sub Seconds not equal to expected result."
  ut_setrequirements cTIME2309, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 14.0: Test the request to subtract two time values"
write ";"
write "; Requirement cTIME 2310"
write ";**********************************************************************"
LOCAL TIME_SUBTRACT_SEC_RESULT
LOCAL TIME_SUBTRACT_SUB_RESULT
LOCAL TELEM_TIME_SUBTRACT_SEC_RESULT
LOCAL TELEM_TIME_SUBTRACT_SUB_RESULT

ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SUBTRACTTIME_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_TIMESUB SUB_TIME1_SECONDS=x'B2D05E' SUB_TIME1_SUBSECS=x'80000000' SUB_TIME2_SECONDS=x'6B28' SUB_TIME2_SUBSECS=x'A0000000'

write "Request to subtract two numbers sent at ", CMD_TIME_1
write "Time 1 seconds is 11718750, time 2 is 27432, expected result is 11691318 (x'b26536') "
write "Time 1 sub seconds is 2147483648 (x'80000000'), time 2 sub seconds is 2684354560 (x'A0000000'), expected result is 3758096384 (x'E0000000')"
write " "
wait 5 

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2010) - Expected Event Message received."
  ut_setrequirements cTIME2310, "P"
ELSE
  write "<!> Failed (2010) - Expected event message ", TST_TIME_SUBTRACTTIME_INF_EID, " not received."
  ut_setrequirements cTIME2310, "F"
ENDIF

TIME_SUBTRACT_SEC_RESULT = x'B2D05E' - x'6B28'
TIME_SUBTRACT_SUB_RESULT = x'80000000' - x'A0000000'
TIME_SUBTRACT_SEC_RESULT = TIME_SUBTRACT_SEC_RESULT - 1 

TELEM_TIME_SUBTRACT_SEC_RESULT = $sc_$cpu_TT_SUBTRACT_SECS
TELEM_TIME_SUBTRACT_SUB_RESULT = $sc_$cpu_TT_SUBTRACT_SECS

write "The result of the time subtract is $sc_$cpu_TT_SUBTRACT_SECS = ", $sc_$cpu_TT_SUBTRACT_SECS
write "The result of the time subtract is $sc_$cpu_TT_SUBTRACT_SUBS = ", $sc_$cpu_TT_SUBTRACT_SUBS
write "The computed value of Seconds is = ",TIME_SUBTRACT_SEC_RESULT
write "The computed value of Sub Seconds is = ",TIME_SUBTRACT_SUB_RESULT

IF (TELEM_TIME_SUBTRACT_SEC_RESULT = TELEM_TIME_SUBTRACT_SEC_RESULT) THEN
  write "<*> Passed (2309) - Subtracted Seconds are correct."
  ut_setrequirements cTIME2310, "P"
ELSE
  write "<!> Failed (2309) - Subtracted Seconds not equal to expected result."
  ut_setrequirements cTIME2310, "F"
ENDIF

IF (TELEM_TIME_SUBTRACT_SUB_RESULT = TELEM_TIME_SUBTRACT_SUB_RESULT) THEN
  write "<*> Passed (2309) - Subtracted Sub Seconds are correct."
  ut_setrequirements cTIME2310, "P"
ELSE
  write "<!> Failed (2309) - Subtracted Sub Seconds not equal to expected result."
  ut_setrequirements cTIME2310, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 15.0: Test the request to compare two time values"
write ";"
write "; Requirement cTIME 2311"
write ";**********************************************************************"
write ";  Step 15.1: Compare times - result = -1"
write ";             TIME1 - x1000 SUBSECONDS - x'80000000' " 
write ";             TIME2 - x2000 SUBSECONDS - x'80000000' "
write ";**********************************************************************"
LOCAL TIME_COMP_RESULT
LOCAL TELEM_TIME_COMP_RESULT

ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_COMPTIME_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_COMPARE COMP_TIME1_SECONDS=x'1000' COMP_TIME1_SUBSECS=x'80000000' COMP_TIME2_SECONDS=x'2000' COMP_TIME2_SUBSECS=x'80000000'

write "Request to compare two numbers sent at ", CMD_TIME_1
wait 5 

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2011) - Expected Event Message received."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2011) - Expected event message ", TST_TIME_COMPTIME_INF_EID, " not received."
  ut_setrequirements cTIME2311, "F"
ENDIF

write "First time to compare is Seconds = x'1000' Sub Seconds = x'80000000', second number Seconds = x'2000' Sub Seconds = x'80000000' "
write "Result of request to compare two numbers is $sc_$cpu_TT_COMPRESULTS = ", $sc_$cpu_TT_COMPRESULTS

TIME_COMP_RESULT = -1
TELEM_TIME_COMP_RESULT = $sc_$cpu_TT_COMPRESULTS

IF (TIME_COMP_RESULT = TELEM_TIME_COMP_RESULT) THEN
  write "<*> Passed (2311) - Time Compare was as expected."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2311) - Time Compare not equal to expected result."
  ut_setrequirements cTIME2311, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 15.2: Compare times - result = -1"
write ";             TIME1 - x1000 SUBSECONDS - x'80000000' " 
write ";             TIME2 - x1000 SUBSECONDS - x'80001000' "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_COMPTIME_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_COMPARE COMP_TIME1_SECONDS=x'1000' COMP_TIME1_SUBSECS=x'80000000' COMP_TIME2_SECONDS=x'1000' COMP_TIME2_SUBSECS=x'80001000' 

write "Request to compare two numbers sent at ", CMD_TIME_1
wait 5 

write "First time of compare is Seconds = x'1000' Sub Seconds = x'80000000', second time is Seconds = x'1000' Sub Seconds = x'8001000' "
write "Result of request to compare two numbers is $sc_$cpu_TT_COMPRESULTS = ", $sc_$cpu_TT_COMPRESULTS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2011) - Expected Event Message received."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2011) - Expected event message ", TST_TIME_COMPTIME_INF_EID, " not received."
  ut_setrequirements cTIME2311, "F"
ENDIF

TIME_COMP_RESULT = -1
TELEM_TIME_COMP_RESULT = $sc_$cpu_TT_COMPRESULTS

IF (TIME_COMP_RESULT = TELEM_TIME_COMP_RESULT) THEN
  write "<*> Passed (2311) - Time Compare was as expected."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2311) - Time Compare not equal to expected result."
  ut_setrequirements cTIME2311, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 15.3: Compare times - result = 0"
write ";             TIME1 - x1000 SUBSECONDS - x'600000' " 
write ";             TIME2 - x1000 SUBSECONDS - x'600000' "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_COMPTIME_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_COMPARE COMP_TIME1_SECONDS=x'1000' COMP_TIME1_SUBSECS=x'600000' COMP_TIME2_SECONDS=x'1000' COMP_TIME2_SUBSECS=x'600000' 

write "Request to compare two numbers sent at ", CMD_TIME_1
wait 5 

write "First time of compare is Seconds = x'1000' Sub Seconds = x'600000', second time is Seconds = x'1000' Sub Seconds = x'600000' "
write "Result of request to compare two numbers is $sc_$cpu_TT_COMPRESULTS = ", $sc_$cpu_TT_COMPRESULTS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2011) - Expected Event Message received."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2011) - Expected event message ", TST_TIME_COMPTIME_INF_EID, " not received."
  ut_setrequirements cTIME2311, "F"
ENDIF

TIME_COMP_RESULT = 0
TELEM_TIME_COMP_RESULT = $sc_$cpu_TT_COMPRESULTS

IF (TIME_COMP_RESULT = TELEM_TIME_COMP_RESULT) THEN
  write "<*> Passed (2311) - Time Compare was as expected."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2311) - Time Compare not equal to expected result."
  ut_setrequirements cTIME2311, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 15.4: Compare times - result = 1"
write ";             TIME1 - x1000 SUBSECONDS - x'80001000' " 
write ";             TIME2 - x1000 SUBSECONDS - x'80000000' "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_COMPTIME_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_COMPARE COMP_TIME1_SECONDS=x'1000' COMP_TIME1_SUBSECS=x'80001000' COMP_TIME2_SECONDS=x'1000'  COMP_TIME2_SUBSECS=x'80000000'

write "Request to compare two numbers sent at ", CMD_TIME_1
wait 5 

write "First time of compare is Seconds = x'1000' Sub Seconds = x'80001000', second time is Seconds = x'1000' Sub Seconds = x'80000000' "
write "Result of request to compare two numbers is $sc_$cpu_TT_COMPRESULTS = ", $sc_$cpu_TT_COMPRESULTS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2011) - Expected Event Message received."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2011) - Expected event message ", TST_TIME_COMPTIME_INF_EID, " not received."
  ut_setrequirements cTIME2311, "F"
ENDIF

TIME_COMP_RESULT = 1
TELEM_TIME_COMP_RESULT = $sc_$cpu_TT_COMPRESULTS

IF (TIME_COMP_RESULT = TELEM_TIME_COMP_RESULT) THEN
  write "<*> Passed (2311) - Time Compare was as expected."
  ut_setrequirements cTIME2311, "P"
ELSE
  write "<!> Failed (2311) - Time Compare not equal to expected result."
  ut_setrequirements cTIME2311, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 16.0: Test the request to convert subseconds to microseconds"
write ";"
write "; Requirement cTIME2312"
write ";**********************************************************************"
LOCAL SUBSEC_1, SUBSEC_2, SUBSEC_3, SUBSEC_4
LOCAL MICROSEC_1, MICROSEC_2, MICROSEC_3, MICROSEC_4

SUBSEC_1 = x'40000000'
SUBSEC_2 = x'80000000'
SUBSEC_3 = x'C0000000'
SUBSEC_4 = x'100000000'

; Expected Results
MICROSEC_1 = x'3D090'
MICROSEC_2 = x'7A120'
MICROSEC_3 = x'B71AF'
MICROSEC_4 = x'F4240'

write ";**********************************************************************"
write "; Step 16.1: Compute the first conversion. "
write "; 	    Converting '",SUBSEC_1,"'"
write ";            Expected result = '",MICROSEC_1,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SUBTOMICROSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_SUBTOMICROSEC SUBSECONDS_TO_CONVERT=SUBSEC_1

write "Request to convert subseconds to microseconds sent at ", CMD_TIME_1
wait 5

write "The returned microseconds value is = ", $sc_$cpu_TT_SSTOMS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2312) - Expected Event Message received."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2312) - Expected event message ", TST_TIME_SUBTOMICROSEC_INF_EID, " not received."
  ut_setrequirements cTIME2312, "F"
ENDIF
 
IF (MICROSEC_1 = $sc_$cpu_TT_SSTOMS) THEN
  write "<*> Passed (2312) - Conversion successful."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2311) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2312, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 16.2: Compute the second conversion."
write "; 	    Converting '",SUBSEC_2,"'"
write ";            Expected result = '",MICROSEC_2,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SUBTOMICROSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_SUBTOMICROSEC SUBSECONDS_TO_CONVERT=SUBSEC_2

write "Request to convert subseconds to microseconds sent at ", CMD_TIME_1
wait 5

write "The returned microseconds values is = ", $sc_$cpu_TT_SSTOMS
 
IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2312) - Expected Event Message received."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2312) - Expected event message ", TST_TIME_SUBTOMICROSEC_INF_EID, " not received."
  ut_setrequirements cTIME2312, "F"
ENDIF

IF (MICROSEC_2 = $sc_$cpu_TT_SSTOMS) THEN
  write "<*> Passed (2312) - Conversion successful."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2311) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2312, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 16.3: Compute the third conversion."
write "; 	    Converting '",SUBSEC_3,"'"
write ";            Expected result = '",MICROSEC_3,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SUBTOMICROSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_SUBTOMICROSEC SUBSECONDS_TO_CONVERT=SUBSEC_3

write "Request to convert subseconds to microseconds sent at ", CMD_TIME_1
wait 5
 
write "The returned microseconds values is = ", $sc_$cpu_TT_SSTOMS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2312) - Expected Event Message received."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2312) - Expected event message ", TST_TIME_SUBTOMICROSEC_INF_EID, " not received."
  ut_setrequirements cTIME2312, "F"
ENDIF

write ";; Comparing ", MICROSEC_3, " with ", $SC_$CPU_TT_SSTOMS

IF (MICROSEC_3 = $sc_$cpu_TT_SSTOMS) THEN
  write "<*> Passed (2312) - Conversion successful."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2311) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2312, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 16.4: Compute the fourth conversion."
write "; 	    Converting '",SUBSEC_4,"'"
write ";            Expected result = '",MICROSEC_4,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_SUBTOMICROSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_SUBTOMICROSEC SUBSECONDS_TO_CONVERT=SUBSEC_4

write "Request to convert subseconds to microseconds sent at ", CMD_TIME_1
wait 5
 
write "The returned microseconds values is = ", $sc_$cpu_TT_SSTOMS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2312) - Expected Event Message received."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2312) - Expected event message ", TST_TIME_SUBTOMICROSEC_INF_EID, " not received."
  ut_setrequirements cTIME2312, "F"
ENDIF

;;; What happened to MICROSEC_4 ?????
IF ($sc_$cpu_TT_SSTOMS = MAX_MICROSEC) THEN
  write "<*> Passed (2312) - Conversion successful."
  ut_setrequirements cTIME2312, "P"
ELSE
  write "<!> Failed (2311) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2312, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 17.0: Test the request to convert microseconds to subseconds"
write ";"
write "; Requirement cTIME 2313"
write ";**********************************************************************"
write "; Step 17.1: Compute the first conversion."
write "; 	    Converting '",MICROSEC_1,"'"
write ";            Expected result = '",SUBSEC_1,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_MICROTOSUBSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_MICROTOSUBSEC MICROSECONDS_TO_CONVERT=MICROSEC_1

write "Request to convert microseconds to subseconds sent at ", CMD_TIME_1
wait 5
 
write "The returned Subseconds value is = ", $sc_$cpu_TT_MSTOSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2313) - Expected Event Message received."
   ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Expected event message ", TST_TIME_MICROTOSUBSEC_INF_EID, " not received."
   ut_setrequirements cTIME2313, "F"
ENDIF

IF (SUBSEC_1 = $sc_$cpu_TT_MSTOSS) THEN
  write "<*> Passed (2313) - Conversion successful."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2313, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 17.2: Compute the second conversion."
write "; 	    Converting '",MICROSEC_2,"'"
write ";            Expected result = '",SUBSEC_2,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_MICROTOSUBSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_MICROTOSUBSEC MICROSECONDS_TO_CONVERT=MICROSEC_2

write "Request to convert microseconds to subseconds sent at ", CMD_TIME_1
wait 5

write "The returned Subseconds value is = ", $sc_$cpu_TT_MSTOSS

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2313) - Expected Event Message received."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Expected event message ", TST_TIME_MICROTOSUBSEC_INF_EID, " not received."
  ut_setrequirements cTIME2313, "F"
ENDIF

IF (SUBSEC_2 = $sc_$cpu_TT_MSTOSS) THEN
  write "<*> Passed (2313) - Conversion successful."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2313, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 17.3: Compute the third conversion."
write "; 	    Converting '",MICROSEC_3,"'"
write ";            Expected result = '",SUBSEC_3,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_MICROTOSUBSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_MICROTOSUBSEC MICROSECONDS_TO_CONVERT=MICROSEC_3

write "Request to convert microseconds to subseconds sent at ", CMD_TIME_1
wait 5

write "The returned Subseconds value is = ", $sc_$cpu_TT_MSTOSS
 
IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2313) - Expected Event Message received."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Expected event message ", TST_TIME_MICROTOSUBSEC_INF_EID, " not received."
  ut_setrequirements cTIME2313, "F"
ENDIF

write ";; Comparing x'BFFFF000' with ", $SC_$CPU_TT_MSTOSS

;;; Why not SUBSEC_3 ????
IF (x'BFFFF000' = $sc_$cpu_TT_MSTOSS) THEN
  write "<*> Passed (2313) - Conversion successful."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2313, "F"
ENDIF

wait 5

write ";**********************************************************************"
write ";  Step 17.4: Compute the fourth conversion."
write "; 	    Converting '",MICROSEC_4,"'"
write ";            Expected result = '",SUBSEC_4,"'"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_TIME, TST_TIME_MICROTOSUBSEC_INF_EID, INFO, 1

CMD_TIME_1 =%GMT

/$sc_$cpu_TST_TIME_MICROTOSUBSEC MICROSECONDS_TO_CONVERT=MICROSEC_4

write "Request to convert microseconds to subseconds sent at ", CMD_TIME_1
wait 5

write "The returned Subseconds value is = ", $sc_$cpu_TT_MSTOSS

IF ($sc_$cpu_find_event[1].num_found_messages > 0) THEN
  write "<*> Passed (2313) - Expected Event Message received."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Expected event message ", TST_TIME_MICROTOSUBSEC_INF_EID, " not received."
  ut_setrequirements cTIME2313, "F"
ENDIF

IF (SUBSEC_4 = $sc_$cpu_TT_MSTOSS) THEN
  write "<*> Passed (2313) - Conversion successful."
  ut_setrequirements cTIME2313, "P"
ELSE
  write "<!> Failed (2313) - Conversion from subseconds to microseconds."
  ut_setrequirements cTIME2313, "F"
ENDIF

wait 5

write ";**********************************************************************"
write "; Step 18.0: Send the command to enter Flywheel State."
write "; "
write "; Requirement cTIME 2012; cTIME 2012.1 "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_STATE_EID, INFO, 1

/$SC_$CPU_TIME_SetState FLYWHEEL
wait 5

IF ($sc_$cpu_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2012) - Expected Event Message received."
  ut_setrequirements cTIME2012, "P"
ELSE
  write "<!> Failed (2012) - Expected event message ", CFE_TIME_STATE_EID, " not received."
  ut_setrequirements cTIME2012, "F"
ENDIF

;; Verify the TLM points
if ($SC_$CPU_TIME_FLAGFLY = 1 AND $SC_$CPU_TIME_FLAGCFLY = 1) then
  write "<*> Passed (2012) - TLM points indicate FLYWHEEL State."
  ut_setrequirements cTIME2012, "P"
else
  write "<!> Failed (2012) - TLM points do not indicate Flywheel State entered."
  ut_setrequirements cTIME2012, "F"
endif

;; Need to test Rqmt 2012.1
;; HOW???

write ";***********************************************************************"
write "; End of sequential print "
write ";***********************************************************************"

seq print off all

write ";**********************************************************************"
write "; Step 19.0: Test Clean-up"
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";***********************************************************************"
write "; Requirements Report "
write ";***********************************************************************"

REQUIREMENTS:


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
