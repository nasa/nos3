PROC $sc_$cpu_lcx_resetcds
;*******************************************************************************
;  Test Name:  lcx_resetcds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker eXtended (LCX)
;	functions properly properly when an application, cFE processor, or cFE
;	power-on reset is performed. This test verifies that the appropriate
;	data items are initialized to their proper values after each reset.
;	Also, this test verifies that the data is restored from the cFE Critical
;	Data Store (CDS) after an Application and Processesor reset is
;	performed. Finally, this test verifies that the LCX application restores
;	the default values if the CDS is corrupted.
;
;	NOTE: This test SHOULD NOT be executed if the configuration parameter
;	      indicating Save Critical Data is set to NO by the Mission.
;
;  Requirements Tested
;   LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message
;   LCX2004	For each Watchpoint, the flight software shall maintain the
;               following statistics in the dump-only Watchpoint Results Table:
;                 a) The result of the last relational comparison (False, True,
;                    Error or Stale)
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;                    False to True result
;                 d) The number of consecutive times the comparison has yielded
;                    a True result
;                 e) The cumulative number of times the comparison has yielded a
;                    True result
;                 f) Most recent FALSE to TRUE transition value
;                 g) Most recent FALSE to TRUE transition timestamp
;                 h) Most recent TRUE to FALSE transition value
;                 i) Most recent TRUE to FALSE transition timestamp
;   LCX3006	For each Actionpoint, the flight software shall maintain the
;               following statistics in the dump-only Actionpoint Results Table:
;                 a) The result of the last Sample(Pass,Fail,Error,Stale)
;                 b) The current state (PermOff,Disabled,Active,Passive,Unused)
;                 c) The number of times this Actionpoint has crossed from the
;                    Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;                    Pass to Fail state
;                 e) The number of consecutive times the equation result =
;                    Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;   LCX4000	Upon receipt of a Set LCX Application State To Active Command,
;		LCX shall set the state of the LCX Application to Active
;   LCX4004	Upon receipt of a Set All Actionpoints to Active Command, LCX
;               shall set the state for all Actionpoints to ACTIVE such that the
;               actionpoints are evaluated and the table-defined actions are
;               taken based on the evaluation
;   LCX8000	LCX shall generate a housekeeping message containing the
;               following:
;                 a) Valid  Command Counter
;                 b) Command Rejected Counter
;                 c) Number of Start RTS commands NOT sent to SC task because
;                    LCX Application is PASSIVE
;                 d) Current LC Application State (LCX_ACTIVE, LCX_PASSIVE,
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
;                 d) Current LC State to <PLATFORM_DEFINED> Default Power-on
;                    State
;                 e) Actionpoint Sample Count
;                 f) TLM Count
;                 g) RTS Execution Counter
;                 h) Watch Results (bitmapped)
;                 i) Action Results (bitmapped)
;   LCX9001	Upon cFE Power-On LCX shall initialize the following Watchpoint
;               data to Zero (or value specified) for all Watchpoints:
;                 a) The result of the last watchpoint relational comparison to
;                    STALE
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
;   LCX9002	Upon cFE Power-On LCX shall initialize the following Actionpoint
;               data to Zero (or value specified for all Actionpoints:
;                 a) The result of the last Actionpoint Sample to STALE
;                 b) The current state as defined in the ADT
;                 c) The number of times this Actionpoint has crossed from the
;                    Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;                    Pass to Fail state
;                 e) The number of consecutive times the equation result =Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;   LCX9004	Upon a cFE Processor Reset or LCX Application Reset, if the 
;		<PLATFORM_DEFINED> Save Critical Data parameter is set to YES,
;		LCX shall restore the following data:
;		  a) LCX housekeeping data
;		  b) WDT
;		  c) Watchpoint statistics
;		  d) ADT
;		  e) Actionpoint statistics
;   LCX9004.1	LCX shall initialize the LCX Application State to
;		<PLATFORM_DEFINED> Default Reset State
;   LCX9004.1.1	If the platform defined Default Reset State indicates to use the
;		state of the LCX Application prior to the reset, LCX shall set
;		the state of the LCX Application to the state restored from the
;		CDS
;   LCX9004.3	If LCX determines the Critical Data is invalid, LCX shall
;		perform the same initialization as a cFE Power-On (see LCX9000,
;		LCX9001 and LCX9002)
;   LCX9005	Upon any initialization, LCX shall validate the Watchpoint
;               Definition Table for the following:
;                 a) valid operator
;                 b) data size
;                 c) Message ID
;   LCX9006	Upon any initialization, LCX shall validate the Actionpoint
;               Definition Table for the following:
;                 a) valid default state
;                 b) RTS number (in range)
;                 c) Event Type (DEBUG, INFO, ERROR, CRITICAL)
;                 d) Failure Count (in range)
;                 e) Action Equation syntax
;   LCX9007	Upon any initialization, LCX shall subscribe to the messages
;               defined in the WDT.
;   LCX9007.1	For a cFE Processor Reset, if the Save Critical Data parameter
;		is YES, LCX shall subscribe to the messages defined in the WDT
;		restored from the CDS
;   LCX9007.2	For an LCX Application Reset, if the Save Critical Data
;		parameter is YES, LCX shall subscribe to the messages defined
;		in the WDT restored from the CDS
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands. 
;	The LCX commands and TLM items exist in the GSE database. 
;	A display page exists for the LC Housekeeping telemetry packet. 
;	LC Test application exists and can be loaded and executed
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date	        Name		Description
;	09/27/12	Walt Moleski	Original Procedure for LCX
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
;       ut_sendrawcmd     Send raw commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_pfindicate     Print the pass fail status of a particular requirement
;                         number.
;       ut_setupevents    Performs setup to verify that a particular event
;                         message was received by ASIST.
;	ut_setrequirements    A directive to set the status of the cFE
;			      requirements array.
;       ftp_file          Procedure to load file containing a table
;       lcx_wdt1          Sets up the Watchpoint Definition table files for
;			  testing
;       lcx_adt1          Sets up the Actionpoint Definition table files for
;			  testing
;       lcx_wdt2          Sets up the Watchpoint Definition table files for
;			  testing
;       lcx_adt2          Sets up the Actionpoint Definition table files for
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
#define LCX_2004       1
#define LCX_3006       2
#define LCX_4000       3
#define LCX_4004       4
#define LCX_8000       5
#define LCX_9000       6
#define LCX_9001       7
#define LCX_9002       8
#define LCX_9004       9
#define LCX_90041     10
#define LCX_900411    11
#define LCX_90043     12
#define LCX_9005      13
#define LCX_9006      14
#define LCX_9007      15
#define LCX_90071     16
#define LCX_90072     17

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

#define CMDFAIL       1
#define CMDSUCCESS    2

global ut_req_array_size = 17
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_2004", ;;
    "LCX_3006","LCX_4000","LCX_4004","LCX_8000","LCX_9000","LCX_9001", ;;
    "LCX_9002","LCX_9004","LCX_9004.1","LCX_9004.1.1","LCX_9004.3", ;;
    "LCX_9005","LCX_9006","LCX_9007","LCX_9007.1","LCX_9007.2"]


;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADTentries = 12
LOCAL APResults = 6
LOCAL ADT2entries = 9
LOCAL AP2Results = 5
LOCAL WDTentries = 30
LOCAL WDT2entries = 28
LOCAL CmdStatus 
LOCAL State
Local cmdctr
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
page $SC_$CPU_LC_ART
page $SC_$CPU_LC_WRT
page $SC_$CPU_LC_ADT
page $SC_$CPU_LC_WDT

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

;; CPU1 is the default
local wpAPID = "0FB9"

if ("$CPU" = "CPU2") then
  wpAPID = "0FD7"
elseif ("$CPU" = "CPU3") then
  wpAPID = "0FF7"
endif

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;check initialization of WRT
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

;; Enable DEBUG events for the LC and "CFE_SB" applications ONLY
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
write ";  Step 2.1: Set the LC Application State to Active. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
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
write ";  Step 2.2: Set the Action Point States to Active. "
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Send packets for all WP defined in WDT, data run #1  "
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.3",1)

write ";*********************************************************************"
write ";  Step 2.4: Send Sample Requests for the first 10 APs. "
write ";*********************************************************************"
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=9 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 2.5: Check the housekeeping counters "
write ";*********************************************************************"
if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 10) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 1) AND ($SC_$CPU_LC_WPSINUSE = 30) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 11) THEN
   for wpindex = 1 to WPACKED do
      if (wpindex = 1) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
            break
         endif
      elseif  (wpindex = 2) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
            break
         endif
      elseif (wpindex >=3) and (wpindex <=7) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
            break
         endif
      elseif  (wpindex = 8) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
            break
         endif
      else
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
            break
         endif
      endif
   enddo

;; for the 10 APs that are being used, should be 0xFF since they are disabled have not been measured
;; rest should be 0x33 since they are not used and not measured
   for apindex = 1 to APACKED do
     if (apindex <= 3) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
          break
        endif
     elseif  (apindex = 4) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
          break
        endif
     elseif  (apindex = 5) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
          break
        endif
     elseif  (apindex = 6) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
          break
        endif
     else
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
          break
        endif
     endif
   enddo

   if (wpindex < WPACKED) OR (apindex < APACKED) then
       write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
       write "  CMDPC              = ", $SC_$CPU_LC_CMDPC
       write "  CMDEC              = ", $SC_$CPU_LC_CMDEC
       write "  CURLCSTATE         = ", p@$SC_$CPU_LC_CURLCSTATE
       write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT
       write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
       write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT
       write "  Passive RTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT
       write "  WP in use          = ", $SC_$CPU_LC_WPSINUSE
       write "  Active APs         = ", $SC_$CPU_LC_ACTIVEAPS

       if (wpindex < WPACKED) then
         write "  WP Packed index    = ", wpindex
         write "  WP Packed Results  = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
       endif
       if (apindex < APACKED) then
         write "  AP Packed index    = ", apindex
         write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
       endif
       ut_setrequirements LCX_8000, "F"
  else
       write "<*> Passed (8000) - Housekeeping telemetry updated properly."
       ut_setrequirements LCX_8000, "P"
endif

else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC                       =", $SC_$CPU_LC_CMDPC
  write "  CMDEC                       =", $SC_$CPU_LC_CMDEC
  write "  CURLCSTATE                  =", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT                 =", $SC_$CPU_LC_APSAMPLECNT
  write "  MONMSGCNT                   =", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT                      =", $SC_$CPU_LC_RTSCNT
  write "  Passive RTSCNT              =", $SC_$CPU_LC_PASSRTSCNT
  write "  WP in use                   =", $SC_$CPU_LC_WPSINUSE
  write "  Active APs                  =", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6: Dump the WRT and ART to verify the statistics. "
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x19) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x54) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x230) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index > 5) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2004) - WRT contains an entry that is not set properly."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", %hex($SC_$CPU_LC_WRT[index].FtoTValue,2)
  write " T to F Value        = ", %hex($SC_$CPU_LC_WRT[index].TtoFValue,2)
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2004) - WRT contains the proper values."
  ut_setrequirements LCX_2004, "P"
endif

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 6) and (index < 9)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
      break
    endif
  elseif ((index = 10) or (index = 11)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
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
  write "<!> Failed (3006) - ART contains an entry that is not set properly."
  write "  Index of failure       =", index
  write "  Action Results         =", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          =", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     =", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     =", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count =", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         =", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          =", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3006) - ART contains the proper values."
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 2.7: Perform an Application Reset"
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
write ";  Step 2.8: Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3

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

;check initialization of housekeeping 
;; These should be the values set prior to the Application Restart
if ($SC_$CPU_LC_CMDPC = 2) AND ($SC_$CPU_LC_CMDEC = 0) AND ;;
   ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 10) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 1) THEN
   for wpindex = 1 to WPACKED do
      if (wpindex = 1) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
            break
         endif
      elseif  (wpindex = 2) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
            break
         endif
      elseif (wpindex >=3) and (wpindex <=7) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
            break
         endif
      elseif  (wpindex = 8) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
            break
         endif
      else
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
            break
         endif
      endif
   enddo

;; for the 10 APs that are being used, should be 0xFF since they are disabled have not been measured
;; rest should be 0x33 since they are not used and not measured
   for apindex = 1 to APACKED do
     if (apindex <= 3) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
          break
        endif
     elseif  (apindex = 4) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
          break
        endif
     elseif  (apindex = 5) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
          break
        endif
     elseif  (apindex = 6) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
          break
        endif
     else
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
          break
        endif
     endif
   enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9004) - Housekeeping telemetry NOT initialized after application reset."
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
    ut_setrequirements LCX_9004, "F"
  else
    write "<*> Passed (9004) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9004, "P"
  endif  
else
  write "<!> Failed (9004) - Housekeeping telemetry NOT initialized after application reset."
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
  ut_setrequirements LCX_9004, "F"
endif

;; Check the LC Application State
if (LC_STATE_WHEN_CDS_RESTORED = LC_STATE_FROM_CDS) then
  if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) then
    write "<*> Passed (9004.1;9004.1.1) - LC Application State initialized properly."
    ut_setrequirements LCX_90041, "P"
    ut_setrequirements LCX_900411, "P"
  else
    write "<!> Failed (9004.1;9004.1.1) - LC Application State NOT set properly. Expected Active. State = ",p@$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
    ut_setrequirements LCX_900411, "F"
  endif
else
  if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_WHEN_CDS_RESTORED) then
    write "<*> Passed (9004.1) - LC Application State initialized properly."
    ut_setrequirements LCX_90041, "P"
  else  
    write "<!> Failed (9004.1) - LC Application State NOT set properly. Expected ",LC_STATE_WHEN_CDS_RESTORED,". State = ",$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
  endif
endif

;;check initialization of WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x19) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x54) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x230) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index > 5) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (9004) - WRT NOT initialized properly after application reset."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9004, "F"
else
  write "<*> Passed (9004) - WRT initialized properly."
  ut_setrequirements LCX_9004, "P"  
endif

;check initialization of ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 6) and (index < 9)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
      break
    endif
  elseif ((index = 10) or (index = 11)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
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
  write "<!> Failed (9004) - ART NOT initialized properly after application reset."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9004, "F"
else
  write "<*> Passed (9004) - ART initialized properly."
  ut_setrequirements LCX_9004, "P"  
endif

write ";*********************************************************************"
write ";  Step 2.11: Send packets for all WP defined in WDT, data run #2  "
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.11",2)

write ";*********************************************************************"
write ";  Step 2.12: Send Sample Request for all 12 APs  "
write ";*********************************************************************"
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 2.13: Check the housekeeping counters "
write ";*********************************************************************"
if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 22) AND ($SC_$CPU_LC_MONMSGCNT = 40) AND ;;
   ($SC_$CPU_LC_RTSCNT = 1) AND ($SC_$CPU_LC_WPSINUSE = 30) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 11) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x15) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x50) then
        break
      endif
    elseif  (wpindex = 3) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x01) then
        break
      endif
    elseif (wpindex >=4) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif
  enddo

  for apindex = 1 to APACKED do
    if (apindex =1) or (apindex = 4) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break
      endif
    elseif (apindex = 2) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x54) then
        break
      endif
    elseif (apindex =3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x94) then
        break
      endif
    elseif (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC              = ", $SC_$CPU_LC_CMDPC
    write "  CMDEC              = ", $SC_$CPU_LC_CMDEC
    write "  CURLCSTATE         = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT
    write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT
    write "  Passive RTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT
    write "  WP in use          = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs         = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index    = ", wpindex
      write "  WP Packed Results  = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index    = ", apindex
      write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC           = ", $SC_$CPU_LC_CMDPC
  write "  CMDEC           = ", $SC_$CPU_LC_CMDEC
  write "  CURLCSTATE      = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT     = ", $SC_$CPU_LC_APSAMPLECNT
  write "  MONMSGCNT       = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT          = ", $SC_$CPU_LC_RTSCNT
  write "  Passive RTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  WP in use       = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs      = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.14: Dump the WRT and ART to verify the statistics. "
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <>0x19) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0054) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x45) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0xff54) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0230) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x130) then
      break
    endif
  elseif (index = 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0012456f) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0012546f) then
      break
    endif
  elseif (index = 7) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x23451200) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x23451300) then
      break
    endif
  elseif (index = 8) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x542) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x546) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0xf0ab1543) then
      break
    endif
  elseif (index = 10) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x3f9ddcc6) then
      break
    endif
  elseif (index > 10) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2004) - WRT contains an entry that is not set properly."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", %hex($SC_$CPU_LC_WRT[index].FtoTValue,2)
  write " T to F Value        = ", %hex($SC_$CPU_LC_WRT[index].TtoFValue,2)
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2004) - WRT contains the proper values."
  ut_setrequirements LCX_2004, "P"
endif

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 3) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 3) and (index < 6)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 6) and (index < 9)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
      break
    endif
  elseif ((index = 10) OR (index = 11)) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
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
  write "<!> Failed (3006) - ART contains an entry that is not set properly."
  write "  Index of failure       =", index
  write "  Action Results         =", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          =", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     =", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     =", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count =", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         =", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          =", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3006) - ART contains non-zero values."
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 2.15: Perform a Processor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 2.16: Start the Limit Checker (LC) Application and "
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
write ";  Step 2.17: Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 2.18: Verify that the LC Housekeeping telemetry packet is being"
write ";  generated and the appropriate items are initialized. "
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

;;check initialization of housekeeping 
;; These should have the values from before the Reset
if ($SC_$CPU_LC_APSAMPLECNT = 22) AND ($SC_$CPU_LC_MONMSGCNT = 40) AND ;;
   ($SC_$CPU_LC_RTSCNT = 1) AND ($SC_$CPU_LC_WPSINUSE = 30) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 11) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x15) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x50) then
        break
      endif
    elseif  (wpindex = 3) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x01) then
        break
      endif
    elseif (wpindex >=4) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif
  enddo

  for apindex = 1 to APACKED do
    if (apindex =1) or (apindex = 4) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break
      endif
    elseif (apindex = 2) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x54) then
        break
      endif
    elseif (apindex =3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x94) then
        break
      endif
    elseif (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9004) - Housekeeping telemetry NOT correct after Processor Reset."
    write "  CMDPC              = ", $SC_$CPU_LC_CMDPC
    write "  CMDEC              = ", $SC_$CPU_LC_CMDEC
    write "  CURLCSTATE         = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT
    write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT
    write "  Passive RTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT
    write "  WP in use          = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs         = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index    = ", wpindex
      write "  WP Packed Results  = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index    = ", apindex
      write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_9004, "F"
  else
    write "<*> Passed (9004) - Housekeeping telemetry was restored properly."
    ut_setrequirements LCX_9004, "P"
  endif
else
  write "<!> Failed (9004) - Housekeeping telemetry NOT correct after Processor Reset."
  write "  CMDPC           = ", $SC_$CPU_LC_CMDPC
  write "  CMDEC           = ", $SC_$CPU_LC_CMDEC
  write "  CURLCSTATE      = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT     = ", $SC_$CPU_LC_APSAMPLECNT
  write "  MONMSGCNT       = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT          = ", $SC_$CPU_LC_RTSCNT
  write "  Passive RTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  WP in use       = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs      = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_9004, "F"
endif

;; Check the LC Application State
;; If the CDS was restored properly, the State should be 'Active'
;; Otherwise, Disabled
if (LC_STATE_WHEN_CDS_RESTORED = LC_STATE_FROM_CDS) then
  if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) then
    write "<*> Passed (9004.1;9004.1.1) - LC Application State initialized properly."
    ut_setrequirements LCX_90041, "P"
    ut_setrequirements LCX_900411, "P"
  else  
    write "<!> Failed (9004.1;9004.1.1) - LC Application State NOT set properly. Expected Active. State = ",p@$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
    ut_setrequirements LCX_900411, "F"
  endif
else
  if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_WHEN_CDS_RESTORED) then
    write "<*> Passed (9004.1) - LC Application State initialized properly."
    ut_setrequirements LCX_90041, "P"
  else  
    write "<!> Failed (9004.1) - LC Application State NOT set properly. Expected ",LC_STATE_WHEN_CDS_RESTORED,". State = ",$SC_$CPU_LC_CURLCSTATE
    ut_setrequirements LCX_90041, "F"
  endif
endif

write ";*********************************************************************"
write ";  Step 2.19: Dump the WRT and ART to verify the statistics. "
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <>0x19) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0054) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x45) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0xff54) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0230) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x130) then
      break
    endif
  elseif (index = 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0012456f) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0012546f) then
      break
    endif
  elseif (index = 7) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x23451200) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x23451300) then
      break
    endif
  elseif (index = 8) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x542) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x546) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0xf0ab1543) then
      break
    endif
  elseif (index = 10) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0x3f9ddcc6) then
      break
    endif
  elseif (index > 10) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
        ($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
        ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;        ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (9004) - WRT was not properly restored after the Processor Reset."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", %hex($SC_$CPU_LC_WRT[index].FtoTValue,2)
  write " T to F Value        = ", %hex($SC_$CPU_LC_WRT[index].TtoFValue,2)
  ut_setrequirements LCX_9004, "F"
else
  write "<*> Passed (9004) - WRT was restored properly."
  ut_setrequirements LCX_9004, "P"
endif

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 3) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 3) and (index < 6)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 6) and (index < 9)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
      break
    endif
  elseif (index = 10) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 11) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
        ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
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
  write "<!> Failed (9004) - ART was not properly restored after the Processor Reset."
  write "  Index of failure       =", index
  write "  Action Results         =", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          =", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     =", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     =", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count =", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         =", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          =", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9004, "F"
else
  write "<*> Passed - ART was properly restored."
  ut_setrequirements LCX_9004, "P"
endif

write ";*********************************************************************"
write ";  Step 2.20: Dump the SB Routing Table and verify that the MsgIDs in "
write ";  the WDT are defined."
write ";*********************************************************************"
;; CPU1 is the default
local wdtAPID = "0FB7"

if ("$CPU" = "CPU2") then
  wdtAPID = "0FD5"
elseif ("$CPU" = "CPU3") then
  wdtAPID = "0FF5"
endif

;; Dump the WDT table to make sure we have the correct one
s get_tbl_to_cvt(ramDir, "LC.LC_WDT", "A", "$cpu_lc_wdt.dat", "$CPU", wdtAPID)
wait 5

;; Dump the SB Routing table 
s get_file_to_cvt (ramDir, "cfe_sb_route.dat", "$cpu_sb_route.dat", "$CPU")
wait 5

local wdtMsgID
local foundSubscription = 0

for index = 0 to WDTentries-1 do
  wdtMsgID = $SC_$CPU_LC_WDT[index].MessageID
  write "== Looking for wdt #",index, " msgID = ", %hex(wdtMsgId,4)
  for sbIndex = 1 to 16320 do
    if  ($SC_$CPU_SB_RouteEntry[sbIndex].SB_AppName = LCAppName) AND ;;
	($SC_$CPU_SB_RouteEntry[sbIndex].SB_MsgID = wdtMsgID) then
      foundSubscription = foundSubscription + 1
      break
    elseif ($SC_$CPU_SB_RouteEntry[sbIndex].SB_AppName = "") then
      break
    endif
  enddo
enddo

if (foundSubscription = WDTentries) then
  write "<*> Passed (9007;9007.1) - All message IDs in WDT have subscriptions."
  ut_setrequirements LCX_9007, "P"
  ut_setrequirements LCX_90071, "P"
else
  write "<!> Failed (9007;9007.1) - Expected 30 message ID subscriptions. Found ",foundSubscription
  ut_setrequirements LCX_9007, "F"
  ut_setrequirements LCX_90071, "F"
endif

write ";*********************************************************************"
write ";  Step 3.0: Invalid CDS Data Tests - Processor Resets"
write ";***********************************************************************"
;; Display the CDS Registry page
page $SC_$CPU_ES_CDS_REGISTRY

;; Dump the CDS Registry
s get_file_to_cvt(ramDir, "cfe_cds_reg.log", "$cpu_cds_reg.log", "$CPU")
wait 5

write ";***********************************************************************"
write ";  Step 3.1: Send the command to set the Processor Reset Counter to 0. "
write ";**********************************************************************"
cmdCtr = $SC_$CPU_ES_CMDPC + 1

/$SC_$CPU_ES_RESETPRCNT

ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Reset PR Counter Command."
else
  write "<!> Failed - Reset PR Counter Command did not increment the CMDPC."
endif

; Check the telemetry counter
if ($SC_$CPU_ES_PROCRESETCNT = 0) then
  write "<*> Passed - Processor Reset Counter set to 0."
else
  write "<!> Failed - Processor Reset Counter did not reset."
endif

write ";*********************************************************************"
write ";  Step 3.2: Corrupt the CDS for the WDT Table "
write ";*********************************************************************"
local addval

addval = 16 + CFE_ES_RESET_AREA_SIZE + (CFE_ES_RAM_DISK_SECTOR_SIZE * CFE_ES_RAM_DISK_NUM_SECTORS) + 20 + 4

write " Corrupt the SC Table Data Critical Data Store by performing the "
write " following steps:"
write "     1. Enter 'sysMemTop ""OS_BSPReservedMemoryPtr""' in the UART window"
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_WDT on the CDS_Registry page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. Enter 'm <value calculated above>,2' in the UART window"
write "     6. Enter 4 and hit the enter or return key in the UART window"
write "     7. Enter 5 and hit the enter or return key in the UART window"
write "     8. Enter 6 and hit the enter or return key in the UART window"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 3.3: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to ACTIVE
State = LC_STATE_ACTIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
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
write ";  Step 3.4: Perform a Processor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 3.5: Create the WDT and ADT used for testing and upload them"
write ";  to the spacecraft default file location for the LC application. "
write ";********************************************************************"
s $SC_$CPU_lcx_wdt2

s ftp_file(defaultTblDir, "lc_def_wdt2.tbl", wdtFileName, "$CPU", "P")

s $SC_$CPU_lcx_adt2

s ftp_file(defaultTblDir, "lc_def_adt2.tbl", adtFileName, "$CPU", "P")

write ";*********************************************************************"
write ";  Step 3.6:  Start the Limit Checker (LC) Application and "
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
write ";  Step 3.7:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 3.8: Verify that the LC Housekeeping telemetry packet is being"
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

;check initialization of housekeeping 
if ($SC_$CPU_LC_CMDPC = 0) AND ($SC_$CPU_LC_CMDEC = 0) AND ;;
   ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 0) AND ($SC_$CPU_LC_MONMSGCNT = 0) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) THEN
;; 255 because the WPs are all not measured   
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
      break
    endif
  enddo
;;first check the 9 APs that are being used
;;255 is because they are disabled and not measured
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex < AP2Results) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 63) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000;9004.3) - WP or AP Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"
endif

;; Since the CDS was corrupted, the LC Application State should be set to
;; the POWER_ON_RESET state
if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_POWER_ON_RESET) then
  write "<*> Passed (9004.1;9004.1.1) - LC Application State initialized properly."
  ut_setrequirements LCX_90041, "P"
  ut_setrequirements LCX_900411, "P"
else  
  write "<!> Failed (9004.1;9004.1.1) - LC Application State NOT set properly. Expected Disabled. State = ",p@$SC_$CPU_LC_CURLCSTATE
  ut_setrequirements LCX_90041, "F"
  ut_setrequirements LCX_900411, "F"
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
  write "<!> Failed (9001;9004.3;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9004.3;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9005, "P"  
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 3.9: Corrupt the CDS for the ADT Table "
write ";*********************************************************************"

write " Corrupt the SC Table Data Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_ADT on that page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. m <value calculated in 5>,2"
write "     6. Enter 4 and hit the enter or return key"
write "     7. Enter 5 and hit the enter or return key"
write "     8. Enter 6 and hit the enter or return key"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 3.10: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to PASSIVE
State = LC_STATE_PASSIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Passive command not sent properly (", ut_sc_status, ")."
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
write ";  Step 3.11: Perform a Processor Reset"
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write ";*********************************************************************"
write ";  Step 3.12: Upload a different WDT and ADT to the spacecraft default"
write ";  file location for the LC application. "
write ";********************************************************************"
s ftp_file(defaultTblDir, "lc_def_wdt1.tbl", wdtFileName, "$CPU", "P")

s ftp_file(defaultTblDir, "lc_def_adt1.tbl", adtFileName, "$CPU", "P")

write ";*********************************************************************"
write ";  Step 3.13:  Start the Limit Checker (LC) Application and "
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
write ";  Step 3.14:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 3.15: Verify that the LC Housekeeping telemetry packet is being"
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

;check initialization of housekeeping 
if ($SC_$CPU_LC_CMDPC = 0) AND ($SC_$CPU_LC_CMDEC = 0) AND ;;
   ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
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
    write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"
endif

;; Check the LC Application State
if ($SC_$CPU_LC_CURLCSTATE=LC_STATE_POWER_ON_RESET) then
  write "<*> Passed (9004.1;9004.1.1) - LC Application State initialized properly."
  ut_setrequirements LCX_90041, "P"
  ut_setrequirements LCX_900411, "P"
else  
  write "<!> Failed (9004.1;9004.1.1) - LC Application State NOT set properly. Expected Passive. State = ",p@$SC_$CPU_LC_CURLCSTATE
  ut_setrequirements LCX_90041, "F"
  ut_setrequirements LCX_900411, "F"
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
  write "<!> Failed (9001;9004.3;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9004.3;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9005, "P"  
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 3.16: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC application ONLY
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
write ";  Step 4.0: Invalid CDS Data Tests - Application Resets"
write ";***********************************************************************"
write ";  Step 4.1: Corrupt the CDS for the WDT Table "
write ";**********************************************************************"

write " Corrupt the SC Table Data Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_WDT on that page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. m <value calculated in 5>,2"
write "     6. Enter 4 and hit the enter or return key"
write "     7. Enter 5 and hit the enter or return key"
write "     8. Enter 6 and hit the enter or return key"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 4.2: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to ACTIVE
State = LC_STATE_ACTIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
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
write ";  Step 4.3: Stop the LC and TST_LC applications (Application Reset) "
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
write ";  Step 4.4: Create the WDT and ADT used for testing and upload them"
write ";  to the spacecraft default file location for the LC application. "
write ";********************************************************************"
s ftp_file(defaultTblDir, "lc_def_wdt1.tbl", wdtFileName, "$CPU", "P")

s ftp_file(defaultTblDir, "lc_def_adt1.tbl", adtFileName, "$CPU", "P")

write ";*********************************************************************"
write ";  Step 4.5:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3

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
write ";  Step 4.6:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 4.7: Verify that the LC Housekeeping telemetry packet is being"
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
    write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"
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
  write "<!> Failed (9001;9004.3;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9004.3;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9005, "P"  
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.8: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC application ONLY
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
write ";  Step 4.9: Corrupt the CDS for the ADT Table "
write ";*********************************************************************"

write " Corrupt the SC Table Data Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_ADT on that page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. m <value calculated in 5>,2"
write "     6. Enter 4 and hit the enter or return key"
write "     7. Enter 5 and hit the enter or return key"
write "     8. Enter 6 and hit the enter or return key"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 4.10: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to ACTIVE
State = LC_STATE_PASSIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Passive command not sent properly (", ut_sc_status, ")."
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
write ";  Step 4.11: Stop the LC and TST_LC applications (Application Reset) "
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
write ";  Step 4.12: Create the WDT and ADT used for testing and upload them"
write ";  to the spacecraft default file location for the LC application. "
write ";********************************************************************"
s ftp_file(defaultTblDir, "lc_def_wdt2.tbl", wdtFileName, "$CPU", "P")

s ftp_file(defaultTblDir, "lc_def_adt2.tbl", adtFileName, "$CPU", "P")

write ";*********************************************************************"
write ";  Step 4.13:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3

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

;; 19 subscriptions + 3 for LC App 
if ($SC_$CPU_find_event[3].num_found_messages = 22) then
  write "<*> Passed (9007;9007.2) - Rcv'd the correct number of Subscription events for LC."
  ut_setrequirements LCX_9007, "P"
  ut_setrequirements LCX_90072, "P"
else
  write "<!> Failed (9007;9007.2) - Expected 22 message subscription events. Rcv'd ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements LCX_9007, "F"
  ut_setrequirements LCX_90072, "F"
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
write ";  Step 4.14:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 4.15: Verify that the LC Housekeeping telemetry packet is being"
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
;;first check the 9 APs that are being used
;;255 is because they are disabled and not measured
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex < AP2Results) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 63) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"
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
  write "<!> Failed (9001;9004.3;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9004.3;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9005, "P"  
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.16: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC application ONLY
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
write ";  Step 4.17: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to ACTIVE
State = LC_STATE_ACTIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
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
write ";  Step 4.18: Stop the LC and TST_LC applications (Application Reset) "
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
write ";  Step 4.19: Corrupt the CDS for the WRT Statistics "
write ";*********************************************************************"

write " Corrupt the SC Table Data Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_CDS_WRT on that page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. m <value calculated in 5>,2"
write "     6. Enter 4 and hit the enter or return key"
write "     7. Enter 5 and hit the enter or return key"
write "     8. Enter 6 and hit the enter or return key"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 4.20:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3

s load_start_app (LCAppName,"$CPU", "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 45
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for LC not received."
  endif
else
  write "<!> Failed - LC Application start Event Message not received."
endif

;; 19 subscriptions + 3 for LC App 
if ($SC_$CPU_find_event[3].num_found_messages = 22) then
  write "<*> Passed (9007;9007.2) - Rcv'd the correct number of Subscription events for LC."
  ut_setrequirements LCX_9007, "P"
  ut_setrequirements LCX_90072, "P"
else
  write "<!> Failed (9007;9007.2) - Expected 22 message subscription events. Rcv'd ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements LCX_9007, "F"
  ut_setrequirements LCX_90072, "F"
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
write ";  Step 4.21:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 4.22: Verify that the LC Housekeeping telemetry packet is being"
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
;;first check the 9 APs that are being used
;;255 is because they are disabled and not measured
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex < AP2Results) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 63) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"  
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.23: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC application ONLY
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
write ";  Step 4.24: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to ACTIVE
State = LC_STATE_PASSIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Passive command not sent properly (", ut_sc_status, ")."
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
write ";  Step 4.25: Stop the LC and TST_LC applications (Application Reset) "
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
write ";  Step 4.26: Corrupt the CDS for the ART Statistics "
write ";*********************************************************************"

write " Corrupt the SC Table Data Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_CDS_ART on that page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. m <value calculated in 5>,2"
write "     6. Enter 4 and hit the enter or return key"
write "     7. Enter 5 and hit the enter or return key"
write "     8. Enter 6 and hit the enter or return key"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 4.27:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3

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

;; 19 subscriptions + 3 for LC App 
if ($SC_$CPU_find_event[3].num_found_messages = 22) then
  write "<*> Passed (9007;9007.2) - Rcv'd the correct number of Subscription events for LC."
  ut_setrequirements LCX_9007, "P"
  ut_setrequirements LCX_90072, "P"
else
  write "<!> Failed (9007;9007.2) - Expected 22 message subscription events. Rcv'd ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements LCX_9007, "F"
  ut_setrequirements LCX_90072, "F"
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
write ";  Step 4.28:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 4.29: Verify that the LC Housekeeping telemetry packet is being"
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
;;first check the 9 APs that are being used
;;255 is because they are disabled and not measured
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex < AP2Results) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 63) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"  
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"  
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"  
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
  write "<!> Failed (9001;9004.3;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_90043, "F"  
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9004.3;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_90043, "P"  
  ut_setrequirements LCX_9005, "P"  
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"  
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"  
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.30: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC application ONLY
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
write ";  Step 4.31: Send the commands to set the counters, Watch Point and "
write ";  Action Point statistics to non-zero. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_COUNTERS_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_WRT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SET_ART_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the LC Housekeeping counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set Counters command sent properly."
else
  write "<!> Failed - TST_LC Set Counters command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set Counters Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set Counters Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Watchpoint Results Table (WRT) Statistics to non-zero
/$SC_$CPU_TST_LC_SETWRT

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set WRT command sent properly."
else
  write "<!> Failed - TST_LC Set WRT command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set WRT Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set WRT Event Message not received."
endif

cmdctr = $SC_$CPU_TST_LC_CMDPC + 1

;; Set the Actionpoint Results Table (ART) Statistics to non-zero
/$SC_$CPU_TST_LC_SETART

ut_tlmwait $SC_$CPU_TST_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Set ART command sent properly."
else
  write "<!> Failed - TST_LC Set ART command."
endif

; Wait for event message
ut_tlmwait  $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - TST_LC Set ART Event Message rcv'd"
else
  write "<!> Failed - TST_LC Set ART Event Message not received."
endif

;; Dumpt the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wpAPID)
wait 5

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

;; Set the LC Application State to ACTIVE
State = LC_STATE_ACTIVE

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
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
write ";  Step 4.32: Stop the LC and TST_LC applications (Application Reset) "
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
write ";  Step 4.33: Corrupt the CDS for the LC housekeeping data"
write ";*********************************************************************"

write " Corrupt the SC Table Data Critical Data Store by entering the following"
write " commands in the UART/minicom window:"
write "     1. sysMemTop ""OS_BSPReservedMemoryPtr"""
write "     2. Add ", addval, " to the displayed value"
write "     3. Note the CDS ""Handle"" for LC.LC_CDS_AppData on that page"
write "     4. Add the CDS Handle to the sum calculated in Step 2."
write "     5. m <value calculated in 5>,2"
write "     6. Enter 4 and hit the enter or return key"
write "     7. Enter 5 and hit the enter or return key"
write "     8. Enter 6 and hit the enter or return key"
write "     9. Type <CTRL-C> to end the modification command."
write " Type 'g' or 'go' in the ASIST command input field to continue."
wait

write ";*********************************************************************"
write ";  Step 4.34:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SUBSCRIPTION_RCVD_EID, "DEBUG", 3

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

;; 19 subscriptions + 3 for LC App 
if ($SC_$CPU_find_event[3].num_found_messages = 22) then
  write "<*> Passed (9007;9007.2) - Rcv'd the correct number of Subscription events for LC."
  ut_setrequirements LCX_9007, "P"
  ut_setrequirements LCX_90072, "P"
else
  write "<!> Failed (9007;9007.2) - Expected 22 message subscription events. Rcv'd ", $SC_$CPU_find_event[3].num_found_messages
  ut_setrequirements LCX_9007, "F"
  ut_setrequirements LCX_90072, "F"
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
write ";  Step 4.35:  Start the Limit Checker Test Application (TST_LC) and "
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
write ";  Step 4.36: Verify that the LC Housekeeping telemetry packet is being"
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
;;first check the 9 APs that are being used
;;255 is because they are disabled and not measured
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex < AP2Results) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 63) then
        break
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_90043, "F"  
  else
    write "<*> Passed (9000;9004.3) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_90043, "P"  
  endif  
else
  write "<!> Failed (9000;9004.3) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_90043, "F"  
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
  write "<!> Failed (9001;9004.3;9005) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_90043, "F"  
  ut_setrequirements LCX_9005, "F"
else
  write "<*> Passed (9001;9004.3;9005) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
  ut_setrequirements LCX_90043, "P"  
  ut_setrequirements LCX_9005, "P"  
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (9002;9004.3;9006) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
  ut_setrequirements LCX_90043, "F"  
  ut_setrequirements LCX_9006, "F"
else
  write "<*> Passed (9002;9004.3;9006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
  ut_setrequirements LCX_90043, "P"  
  ut_setrequirements LCX_9006, "P"  
endif

write ";*********************************************************************"
write ";  Step 5.0: Clean-up"
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
write ";  End procedure $SC_$CPU_lcx_resetcds"
write ";*********************************************************************"
ENDPROC
