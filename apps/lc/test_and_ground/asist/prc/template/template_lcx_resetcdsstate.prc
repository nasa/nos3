PROC $sc_$cpu_lcx_resetcdsstate
;*******************************************************************************
;  Test Name:  lcx_resetcds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker (LC) restores
;	the proper LC Application State when an application and cFE processor
;	reset is performed.
;
;	NOTE: This test SHOULD NOT be executed if the configuration parameter
;	      indicating Save Critical Data is set to NO by the Mission.
;
;  Requirements Tested
;   LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message
;   LCX4000	Upon receipt of a Set LCX Application State To Active Command,
;               LCX shall set the state of the LC Application to Active
;   LCX8000	LCX shall generate a housekeeping message containing the
;               following:
;                 a) Valid  Command Counter
;                 b) Command Rejected Counter
;                 c) Number of Start RTS commands NOT sent to SC task because
;                    LCX Application is PASSIVE
;                 d) Current LCX Application State (LCX_ACTIVE, LCX_PASSIVE,
;                    LCX_DISABLED)...
;                 e) Total count of actionpoints sampled while LCX_ACTIVE or
;                    LCX_PASSIVE...
;                 f) Total count of packets monitored for watchpoints (cmd and
;                    telemetry)
;                 g) Total count of commands sent to SC task to start an RTS
;                 h) Selected data from watchpoint results table
;                 i) Selected data from actionpoint results table
;   LCX9000	Upon cFE Power-On LCX shall initialize the following
;		Housekeeping data to Zero (or value specified):
;                 a) Valid Command Counter
;                 b) Command Rejected Counter
;                 c) Passive RTS Execution Counter
;                 d) Current LCX State to <PLATFORM_DEFINED> Default Power-on
;                    State
;                 e) Actionpoint Sample Count
;                 f) TLM Count
;                 g) RTS Execution Counter
;                 h) Watch Results (bitmapped)
;                 i) Action Results (bitmapped)
;   LCX9001	Upon cFE Power-On LCX shall initialize the following Watchpoint
;               data to Zero (or value specified) for all Watchpoints:
;                 a) The result of the last watchpoint relational comparison to
;                    Stale
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;                    False to True result
;                 d) The number of consecutive times the comparison has yielded
;                    a True result
;                 e) The cumulative number of times the comparison has yielded
;                    a True result
;                 f) The value that caused the last False-to-True crossing, and
;                    the crossing time stamp
;                 g) The value that caused the last True-to-False crossing, and
;                    the crossing time stamp
;   LC9002	Upon cFE Power-On LC shall initialize the following Actionpoint
;               data to Zero (or value specified for all Actionpoints:
;                 a) The result of the last Actionpoint Sample to NOT MEASURED
;                 b) The current state as defined in the ADT
;                 c) The number of times this Actionpoint has crossed from the
;                    Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;                    Pass to Fail state
;                 e) The number of consecutive times the equation result =
;                    Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;   LC9004.1	LC shall initialize the LC Application State to
;		<PLATFORM_DEFINED> Default Reset State
;   LC9005	Upon any initialization, LC shall validate the Watchpoint
;               Definition Table for the following:
;                 a) valid operator
;                 b) data size
;                 c) Message ID
;   LC9006	Upon any initialization, LC shall validate the Actionpoint
;               Definition Table for the following:
;                 a) valid default state
;                 b) RTS number (in range)
;                 c) Event Type (DEBUG, INFO, ERROR, CRITICAL)
;                 d) Failure Count (in range)
;                 e) Action Equation syntax
;   LC9007	Upon any initialization, LC shall subscribe to the messages
;               defined in the WDT.
;   LC9007.2	For an LC Application Reset, if the Save Critical Data parameter
;		is YES, LC shall subscribe to the messages defined in the WDT
;		restored from the CDS
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands. 
;	The LC commands and TLM items exist in the GSE database. 
;	A display page exists for the LC Housekeeping telemetry packet. 
;	LC Test application loaded and running
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date	        Name		Description
;	05/21/09	Walt Moleski	Original Procedure.
;	02/08/11	Walt Moleski	Added variables for app and table names
;					and replaced hard-coded instances
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait        Wait for a specified telemetry point to update to
;                         a specified value. 
;       ut_sendcmd        Send commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_pfindicate     Print the pass fail status of a particular requirement
;                         number.
;       ut_setupevents    Performs setup to verify that a particular event
;                         message was received by ASIST.
;	ut_setrequirements    A directive to set the status of the cFE
;			      requirements array.
;       ftp_file          Procedure to load file containing a table
;       lc_wdt1           Sets up the Watchpoint Definition table files for
;			  testing
;       lc_adt1           Sets up the Actionpoint Definition table files for
;			  testing
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_sb_events.h"
#include "to_lab_events.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"

%liv (log_procedure) = logging

#define LCX_1003       0
#define LCX_4000       1
#define LCX_8000       2
#define LCX_9000       3
#define LCX_9001       4
#define LCX_9002       5
#define LCX_90041      6
#define LCX_9005       7
#define LCX_9006       8
#define LCX_9007       9
#define LCX_90072     10

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

global ut_req_array_size = 10
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_4000", ;;
    "LCX_8000","LCX_9000","LCX_9001","LCX_9002","LCX_9004.1","LCX_9005", ;;
    "LCX_9006","LCX_9007","LCX_9007.2"]


;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL stream1
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADTentries = 12
LOCAL APResults = 6
LOCAL State
Local cmdctr
local CDSGood
local LCAppName = LC_APP_NAME
local ramDir = "RAM:0"
local defaultTblDir = "CF:0/apps"
local ARTTblName = LCAppName & ".LC_ART"
local WRTTblName = LCAppName & ".LC_WRT"

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
write ";  Step 1.1:  Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 1.2: Creating the WDT and ADT used for testing and upload them"
write ";  to the spacecraft default file location for the LC application. "
write ";********************************************************************"
s $SC_$CPU_lcx_wdt1

;; Parse the filename configuration parameters for the default table filenames
local wdtFileName = LC_WDT_FILENAME
local slashLoc = %locate(wdtFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  wdtFileName = %substring(wdtFileName,slashLoc+1,%length(wdtFileName))
  slashLoc = %locate(wdtFileName,"/")
enddo

write "==> Default LC Watchpoint Table filename = '",wdtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_wdt1.tbl", wdtFileName, "$CPU", "P")

s $SC_$CPU_lcx_adt1

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_adt1.tbl", adtFileName, "$CPU", "P")

;; Display the pages used by this test
page $SC_$CPU_LC_HK
page $SC_$CPU_TST_LC_HK

write ";*********************************************************************"
write ";  Step 1.3:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2

s load_start_app (LCAppName,"$CPU", "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for LC not received."
  endif
else
  write "<!> Failed - LC Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'8A7'

if ("$CPU" = "CPU2") then
  stream1 = x'9A7'
elseif ("$CPU" = "CPU3") then
  stream1 = x'AA7'
endif

write "Sending command to add subscription for LC housekeeping packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 1.4:  Start the Limit Checker Test Application (TST_LC) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_LC", "$CPU", "TST_LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_LC not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_LC Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'933'

if ("$CPU" = "CPU2") then
  stream1 = x'A33'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B33'
endif
                                                                                
write "Sending command to add subscription for TST_LC HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

wait 5

write ";*********************************************************************"
write ";  Step 1.5: Verify that the LC Housekeeping telemetry packet is being"
write ";  generated and the apropriate items are initialized. "
write ";*********************************************************************"
;; Add the HK message receipt test
local hkPktId

;; Set the SC HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0A7"

if ("$CPU" = "CPU2") then
  hkPktId = "p1A7"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2A7"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements LCX_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements LCX_8000, "F"
endif

;check initialization of housekeeping 
if ($SC_$CPU_LC_CMDPC = 0) AND ($SC_$CPU_LC_CMDEC = 0) AND ;;
   ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_POWER_ON_RESET) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 0) AND ($SC_$CPU_LC_MONMSGCNT = 0) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) THEN
;; 255 because the WPs are all not measured   
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
      break
    endif
  enddo
;;first check the 12 APs that are being used
;;255 is because they are disabled and not measured
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= APResults) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
        break                                   
      endif
    elseif ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
      break
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  Index of failure  = ", wpindex
      write "  WP Packed Results = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
    endif
    if (apindex < APACKED) then
      write "  Index of failure  = ", apindex
      write "  AP Packed Results = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
    endif
    ut_setrequirements LCX_9000, "F"
  else
    write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
  endif  
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
  if (wpindex < WPACKED) then
    write "  Index of failure  = ", wpindex
    write "  WP Packed Results =", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
  endif
  if (apindex < APACKED) then
    write "  Index of failure  = ", apindex
    write "  AP Packed Results =", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
  endif
  ut_setrequirements LCX_9000, "F"
endif

;check initialization of WRT
;; CPU1 is the default
local wpAPID = "0FB9"

if ("$CPU" = "CPU2") then
  wpAPID = "0FD7"
elseif ("$CPU" = "CPU3") then
  wpAPID = "0FF7"
endif

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
     ($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
    break
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (9001;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_9005, "P"  
endif

;check initialization of ART
;; CPU1 is the default
local apAPID = "0FB8"

if ("$CPU" = "CPU2") then
  apAPID = "0FD6"
elseif ("$CPU" = "CPU3") then
  apAPID = "0FF6"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (9002;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC and CFE_SB application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=LCAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_SB" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0: Application Reset Test"
write ";*********************************************************************"
write ";  Step 2.1: Set the LC Application State to a state different than the"
write ";  state to be restored." 
write ";*********************************************************************"
if (LC_STATE_WHEN_CDS_RESTORED <> LC_STATE_ACTIVE) then
  State = LC_STATE_ACTIVE
else
  State = LC_STATE_PASSIVE
endif

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4000) - Set LC Application State command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Perform an Application Reset"
write ";*********************************************************************"
cmdctr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_DELETEAPP Application=LCAppName
wait 5

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - LC app stop command sent properly."
else
  write "<!> Failed - LC app stop command did not increment CMDPC."
endif

wait 5

cmdctr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_DELETEAPP Application="TST_LC"

ut_tlmwait  $SC_$CPU_ES_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC app stop command sent properly."
else
  write "<!> Failed - TST_LC app stop command did not increment CMDPC."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_CDS_RESTORED_INF_EID, "INFO", 4

s load_start_app (LCAppName,"$CPU", "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for LC not received."
  endif
else
  write "<!> Failed - LC Application start Event Message not received."
endif

;; 20 subscriptions + 3 for LC App 
if ($SC_$CPU_find_event[3].num_found_messages = 23) then
  write "<*> Passed (9007;9007.2) - Rcv'd the correct number of Subscription events for LC."
  ut_setrequirements LCX_9007, "P"
  ut_setrequirements LCX_90072, "P"
else
  write "<!> Failed (9007;9007.2) - Expected 23 message subscription events. Rcv'd ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements LCX_9007, "F"
  ut_setrequirements LCX_90072, "F"
endif

CDSGood = "No"
;; See if the CDS was properly restored - Event Msg 
if ($SC_$CPU_find_event[4].num_found_messages = 1) then
  CDSGood = "Yes"
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'8A7'

if ("$CPU" = "CPU2") then
  stream1 = x'9A7'
elseif ("$CPU" = "CPU3") then
  stream1 = x'AA7'
endif

write "Sending command to add subscription for LC housekeeping packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 2.4: Start the Limit Checker Test Application (TST_LC) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_INIT_INF_EID, "INFO", 2
                                                                                
s load_start_app ("TST_LC", "$CPU", "TST_LC_AppMain")
                                                                                
; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_LC not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_LC Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'933'

if ("$CPU" = "CPU2") then
  stream1 = x'A33'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B33'
endif
                                                                                
write "Sending command to add subscription for TST_LC HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

wait 5

write ";*********************************************************************"
write ";  Step 2.5: Verify that the LC Housekeeping telemetry packet is being"
write ";  generated and the apropriate items are initialized. "
write ";*********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements LCX_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements LCX_8000, "F"
endif

;; Check the LC Application State
if (CDSGood = "Yes") then
  if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_WHEN_CDS_RESTORED) then
    write "<*> Passed (9004.1) - LC Application State initialized properly from CDS."
    ut_setrequirements LCX_90041, "P"
  else  
    write "<!> Failed (9004.1) - LC Application State NOT set properly in CDS. Expected ",LC_STATE_WHEN_CDS_RESTORED,". State = ",$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
  endif
else
  write "<!> Failed (9004.1) - LC Application State NOT set properly after Application Reset because CDS was not properly restored. Expected ",LC_STATE_WHEN_CDS_RESTORED,". State = ",$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6: Set the LC Application State to a state different than the"
write ";  state to be restored." 
write ";*********************************************************************"
if (LC_STATE_WHEN_CDS_RESTORED <> LC_STATE_ACTIVE) then
  State = LC_STATE_ACTIVE
else
  State = LC_STATE_PASSIVE
endif

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4000) - Set LC Application State command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4000, "F"
endif

wait 5


write ";*********************************************************************"
write ";  Step 2.7: Perform a Processor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 2.8: Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_CDS_RESTORED_INF_EID, "INFO", 3

s load_start_app (LCAppName,"$CPU", "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for LC not received."
  endif
else
  write "<!> Failed - LC Application start Event Message not received."
endif

CDSGood = "No"
;; See if the CDS was properly restored - Event Msg 
if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  CDSGood = "Yes"
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'8A7'

if ("$CPU" = "CPU2") then
  stream1 = x'9A7'
elseif ("$CPU" = "CPU3") then
  stream1 = x'AA7'
endif

write "Sending command to add subscription for LC housekeeping packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 2.9: Start the Limit Checker Test Application (TST_LC) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_INIT_INF_EID, "INFO", 2
                                                                                
s load_start_app ("TST_LC", "$CPU", "TST_LC_AppMain")
                                                                                
; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_LC not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_LC Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'933'

if ("$CPU" = "CPU2") then
  stream1 = x'A33'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B33'
endif

write "Sending command to add subscription for TST_LC HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Verify that the LC Housekeeping telemetry packet is being"
write ";  generated and the apropriate items are initialized. "
write ";*********************************************************************"
;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements LCX_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements LCX_8000, "F"
endif

;; Check the LC Application State
if (CDSGood = "Yes") then
  if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_WHEN_CDS_RESTORED) then
    write "<*> Passed (9004.1) - LC Application State initialized properly."
    ut_setrequirements LCX_90041, "P"
  else  
    write "<!> Failed (9004.1) - LC Application State NOT set properly. Expected ",LC_STATE_WHEN_CDS_RESTORED,". State = ",$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
  endif
else
  write "<!> Failed (9004.1) - LC Application State NOT set properly after Processor Reset because CDS was not properly restored. Expected ",LC_STATE_WHEN_CDS_RESTORED,". State = ",$SC_$CPU_LC_CURLCSTATE
  ut_setrequirements LCX_90041, "F"
endif

write ";*********************************************************************"
write ";  Step 3.0: Clean-up"
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write "**** Requirements Status Reporting"
                                                                                
write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"
                                                                                
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_lcx_resetcdsstate"
write ";*********************************************************************"
ENDPROC
