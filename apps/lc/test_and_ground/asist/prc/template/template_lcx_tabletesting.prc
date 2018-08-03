PROC $sc_$cpu_lcx_tabletesting
;*****************************************************************************
;  Test Name:  lcx_tabletesting
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker eXtended (LCX)
;	functions properly when loading new Watchpoint Definition Tables (WDT)
;	and Actionpoint Definition Tables (ADT). The tables will be updated
;	while the LC application is in the Active, Passive, and Disabled state.
;	Also, the tables will be updated individually as well as simultaneously
;	to ensure that the LC application behaves appropriately. Other tests
;	include attempts to load invalid tables which should fail validation.
;
;  Requirements Tested
;   LCX1003     If LCX accepts any command as valid, LCX shall execute the
;               command, increment the LCX Valid Command Counter and issue an
;               event message
;   LCX2004      For each Watchpoint, the flight software shall maintain the
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
;   LCX2005	Upon receipt of a table update indication, LCX shall validate
;		the Watchpoint Definition Table for the following:
;		  a) Valid operator
;		  b) Data size
;		  c) Message ID
;   LCX3006      For each Actionpoint, the flight software shall maintain the
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
;   LCX3007	Upon receipt of a table update indication, LCX shall validate
;		the Actionpoint Definition Table for the following:
;		  a) valid default state
;		  b) RTS number (in range)
;		  c) Event Type (DEBUG,INFO,ERROR,CRITICAL)
;		  d) Failure Count (in range)
;		  e) Action Equation syntax
;   LCX4000     Upon receipt of a Set LCX Application State To Active Command,
;               LCX shall set the state of the LCX Application to Active
;   LCX4001	Upon receipt of a Set LCX Application State to Passive Command,
;               LCX shall set the LCX Application State to Passive
;   LCX4002	Upon receipt of a Set LCX Application State to Disable Command,
;               LCX shall set the LCX Application State to Disabled
;   LCX4004     Upon receipt of a Set All Actionpoints to Active Command, LCX
;               shall set the state for all Actionpoints to ACTIVE such that the
;               actionpoints are evaluated and the table-defined actions are
;               taken based on the evaluation
;   LCX8000     LCX shall generate a housekeeping message containing the
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
;   LCX9000     Upon cFE Power-On LCX shall initialize the following
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
;   LCX9001     Upon cFE Power-On LCX shall initialize the following Watchpoint
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
;   LCX9002     Upon cFE Power-On LCX shall initialize the following Actionpoint
;               data to Zero (or value specified for all Actionpoints:
;                 a) The result of the last Actionpoint Sample to STALE
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
;   LCX9005      Upon any initialization, LCX shall validate the Watchpoint
;               Definition Table for the following:
;                 a) valid operator
;                 b) data size
;                 c) Message ID
;   LCX9006     Upon any initialization, LCX shall validate the Actionpoint
;               Definition Table for the following:
;                 a) valid default state
;                 b) RTS number (in range)
;                 c) Event Type (DEBUG, INFO, ERROR, CRITICAL)
;                 d) Failure Count (in range)
;                 e) Action Equation syntax
;   LCX9007     Upon any initialization, LCX shall subscribe to the messages
;               defined in the WDT.
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
;	10/01/12	Walt Moleski	Initial procedure for LCX
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
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"

%liv (log_procedure) = logging

#define LCX_1003       0
#define LCX_2004       1
#define LCX_2005       2
#define LCX_3006       3
#define LCX_3007       4
#define LCX_4000       5
#define LCX_4001       6
#define LCX_4002       7
#define LCX_4004       8
#define LCX_8000       9
#define LCX_9000      10
#define LCX_9001      11
#define LCX_9002      12
#define LCX_9005      13
#define LCX_9006      14
#define LCX_9007      15

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

global ut_req_array_size = 15
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_2004", ;;
"LCX_2005","LCX_3006","LCX_3007","LCX_4000","LCX_4001","LCX_4002","LCX_4004", ;;
"LCX_8000","LCX_9000","LCX_9001","LCX_9002","LCX_9005","LCX_9006","LCX_9007"]


;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADT1entries = 12
LOCAL AP1Results = 6
LOCAL ADT2entries = 9
LOCAL AP2Results = 5
LOCAL ADT2aentries = 12
LOCAL AP2aResults = 6
LOCAL ADT3entries = 10
LOCAL AP3Results = 5
LOCAL WDT1entries = 30
LOCAL WDT2entries = 28
LOCAL WDT2aentries = 31
LOCAL WDT3entries = 20
LOCAL State
Local cmdctr, errctr
local LCAppName = LC_APP_NAME
local ramDir = "RAM:0"
local defaultTblDir = "CF:0/apps"
local ARTTblName = LCAppName & ".LC_ART"
local ADTTblName = LCAppName & ".LC_ADT"
local WRTTblName = LCAppName & ".LC_WRT"
local WDTTblName = LCAppName & ".LC_WDT"

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
;; first check the 12 APs that are being used
;; 255 is because they are disabled and not measured
  for apindex = 1 to AP1Results do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
      break                                   
    endif
  enddo
;; then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = AP1Results+1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
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
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_9000, "F"
  else
    write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
  endif  
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC             =", $SC_$CPU_LC_CMDPC 
  write "  CMDEC             =", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT        =", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE        =", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT       =", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT         =", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT            =", $SC_$CPU_LC_RTSCNT 
  if (wpindex < WPACKED) then
    write "  WP Packed Results =", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
  endif
  if (apindex < APACKED) then
    write "  AP Packed Results =", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
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
  write "<!> Failed (9001;9005;9007) - Watchpoint Results Table NOT initialized at startup."
  write "  Index of failure   =", index
  write " WatchResults        =", $SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    =", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count =", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    =", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      =", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        =", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        =", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
  ut_setrequirements LCX_9005, "F"
  ut_setrequirements LCX_9007, "F"
else
  write "<*> Passed (9001;9005;9007) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"
  ut_setrequirements LCX_9005, "P"
  ut_setrequirements LCX_9007, "P"
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
  if (index < ADT1entries) then
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
  write "  Index of failure       =", index
  write "  Action Results         =", $SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          =", $SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     =", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     =", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count =", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         =", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          =", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
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
write ";  Step 2.0:  Table update while LC is Active Test"
write ";*********************************************************************"
write ";  Step 2.1:  Send command to set the LC application state to Active as"
write ";  well as the command to set all the defined APs to Active."
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

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send the commands to update the WDT with a new table."
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_wdt2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the table
s load_table("lc_def_wdt2.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for WDT sent successfully."
else
  write "<!> Failed - Load command for WDT did not execute successfully."
endif

;; Send the commands to validate each table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for WDT sent successfully."
else
  write "<!> Failed - Validate command for WDT did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation successful message rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv WDT Validation successful message"
    ut_setrequirements LCX_2005, "F"
  endif
else
  write "<!> Failed (2005) - WDT Failed Validation"
  ut_setrequirements LCX_2005, "F"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_RCVD_EID,"DEBUG", 2
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_REMOVED_EID,"DEBUG", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for WDT sent successfully."
else
  write "<!> Failed - Activate command for WDT did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the Table Update Success message"
else
  write "<*> Failed - Did not rcv the Table Update Success message for the WDT table"
endif

;; Check for the correct number of SB Subscription messages
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 20
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the correct number of Subscription Removed messages from SB."
else
  write "<!> Failed - Expected 20 Subscription Removed messages from SB. Only rcv'd ", $sc_$cpu_find_event[3].num_found_messages
endif

if ($sc_$cpu_find_event[2].num_found_messages > 1) then
  write "<*> Passed - Rcv'd more than 1 Subscription messages from SB."
else
  write "<!> Failed - Did not get any Subscription Rcvd messages from SB."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 28) AND ($SC_$CPU_LC_ACTIVEAPS <> 12) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after WDT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

write ";*********************************************************************"
write ";  Step 2.4: Dump the WRT and check counters"
write ";*********************************************************************"
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
  write "<!> Failed (2004) - Watchpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (2004) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_2004, "P"  
endif

write ";*********************************************************************"
write ";  Step 2.5: Send the commands to update the ADT with a new table."
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_adt2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the ADT table
s load_table("lc_def_adt2.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ADT sent successfully."
else
  write "<!> Failed - Load command for ADT did not execute successfully."
endif

;; Send the command to validate the ADT table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 2

;; Send the command to validate the ADT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation successful message rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv ADT Validation successful message"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (3007) - ADT Failed Validation"
  ut_setrequirements LCX_3007, "F"
endif

;; Send the commands to activate the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_UPDATE_SUCCESS_INF_EID, "INFO", 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName

;; Wait until you receive the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd Table Update Success messages"
else
  write "<*> Failed - Did not rcv Table Update Success message for the ADT table"
endif

wait 5

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 28) AND ($SC_$CPU_LC_ACTIVEAPS <> 9) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after ADT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

;; Check the Actionpoint Packed Results - Not Measured & Active
local resultStatus = TRUE
for apindex = 1 to AP2Results do
  if (apindex <> 5) then
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
      resultStatus = FALSE
      break
    endif
  else
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x37) then
      resultStatus = FALSE
      break
    endif
  endif
enddo

if (resultStatus = TRUE) then
  write "<*> Passed (8000) - AP Packed Results are as expected after ADT load."
else
  write "<!> Failed (8000) - AP Packed Results are NOT correct."
  write "  AP Packed index   = ", apindex
  write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
endif

write ";*********************************************************************"
write ";  Step 2.7: Dump the ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (3006) - Actionpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (3006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_3006, "P"  
endif

write ";*********************************************************************"
write ";  Step 2.8: Update both tables at the same time"
write ";*********************************************************************"
;; Create the load files
s $SC_$CPU_lcx_wdt2a
s $SC_$CPU_lcx_adt2a

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to load the table
s load_table("lc_def_wdt2a.tbl","$CPU")
s load_table("lc_def_adt2a.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Load commands for WDT and ADT did not execute successfully."
endif

;; Send the commands to validate each table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Validate commands for WDT and ADT did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation message rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv WDT and ADT Validation success messages"
    ut_setrequirements LCX_2005, "F"
  endif
  if ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation message rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv ADT Validation message"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (2005;3007) - WDT and/or ADT Failed Validation"
  ut_setrequirements LCX_2005, "F"
  ut_setrequirements LCX_3007, "F"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_RCVD_EID,"DEBUG", 2
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_REMOVED_EID,"DEBUG", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName
/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Activate commands for WDT and ADT did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the Table Update Success messages for both tables"
else
  write "<*> Failed - Did not rcv the Table Update Success message for both tables"
endif

;; Check for the correct number of SB Subscription messages
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 19
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the correct number of Subscription Removed messages from SB."
else
  write "<!> Failed - Expected 19 Subscription Removed messages from SB. Only rcv'd ", $sc_$cpu_find_event[3].num_found_messages
endif

if ($sc_$cpu_find_event[2].num_found_messages > 1) then
  write "<*> Passed - Rcv'd more than 1 Subscription messages from SB."
else
  write "<!> Failed - Did not get any Subscription Rcvd messages from SB."
endif

wait 5

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 31) AND ($SC_$CPU_LC_ACTIVEAPS <> 12) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after WDT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

;; Check the Actionpoint Packed Results - Not Measured & Active
resultStatus = TRUE
for apindex = 1 to AP2aResults do
  if (apindex < 7) then
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
      resultStatus = FALSE
      break
    endif
  else
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
      resultStatus = FALSE
      break
    endif
  endif
enddo

if (resultStatus = TRUE) then
  write "<*> Passed (8000) - AP Packed Results are as expected after ADT load."
else
  write "<!> Failed (8000) - AP Packed Results are NOT correct."
  write "  AP Packed index   = ", apindex
  write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
endif

write ";*********************************************************************"
write ";  Step 2.10: Dump the WRT and ADT and check counters"
write ";*********************************************************************"
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
  write "<!> Failed (2004) - Watchpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (2004) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_2004, "P"  
endif

;;  Dump the ART and check counters"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2aentries) then
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
  write "<!> Failed (3006) - Actionpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (3006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_3006, "P"  
endif

write ";*********************************************************************"
write ";  Step 3.0:  Test Table updates with LC appplication state Passive   "
write ";*********************************************************************"
write ";  Step 3.1:  Send command to set the LC application state to Passive "
write ";  as well as the command to set all the defined APs to Active."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"

if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
  write "<*> Passed (1003;4001) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4001, "P"
else
write "<!> Failed (1003;4001) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4001, "P"
else
  write "<!> Failed (1003;4001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4001, "F"
endif

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send the commands to update the WDT with a new table."
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_wdt2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the table
s load_table("lc_def_wdt2.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for WDT sent successfully."
else
  write "<!> Failed - Load command for WDT did not execute successfully."
endif

;; Send the commands to validate each table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for WDT sent successfully."
else
  write "<!> Failed - Validate command for WDT did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1, 40
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation message rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv WDT Validation message"
    ut_setrequirements LCX_2005, "F"
  endif
else
  write "<!> Failed (2005) - WDT Failed Validation"
  ut_setrequirements LCX_2005, "F"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_RCVD_EID,"DEBUG", 2
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_REMOVED_EID,"DEBUG", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for WDT sent successfully."
else
  write "<!> Failed - Activate command for WDT did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the Table Update Success message"
else
  write "<*> Failed - Did not rcv the Table Update Success message for the WDT table"
endif

;; Check for the correct number of SB Subscription messages
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 19
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the correct number of Subscription Removed messages from SB."
else
  write "<!> Failed - Expected 19 Subscription Removed messages from SB. Only rcv'd ", $sc_$cpu_find_event[3].num_found_messages
endif

if ($sc_$cpu_find_event[2].num_found_messages > 1) then
  write "<*> Passed - Rcv'd more than 1 Subscription messages from SB."
else
  write "<!> Failed - Did not get any Subscription Rcvd messages from SB."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 28) AND ($SC_$CPU_LC_ACTIVEAPS <> 12) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after WDT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

write ";*********************************************************************"
write ";  Step 3.4: Dump the WRT and check counters"
write ";*********************************************************************"
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
  write "<!> Failed (2004) - Watchpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (2004) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_2004, "P"  
endif

write ";*********************************************************************"
write ";  Step 3.5: Send the commands to update the ADT with a new table."
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_adt2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the ADT table
s load_table("lc_def_adt2.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ADT sent successfully."
else
  write "<!> Failed - Load command for ADT did not execute successfully."
endif

;; Send the command to validate the ADT table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 2

;; Send the command to validate the ADT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation successful message rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv ADT Validation successful message"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (3007) - ADT Failed Validation"
  ut_setrequirements LCX_3007, "F"
endif

;; Send the commands to activate the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_UPDATE_SUCCESS_INF_EID, "INFO", 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName

;; Wait until you receive the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd Table Update Success messages"
else
  write "<*> Failed - Did not rcv Table Update Success message for the ADT table"
endif

wait 5

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 28) AND ($SC_$CPU_LC_ACTIVEAPS <> 9) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after ADT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

;; Check the Actionpoint Packed Results - Not Measured & Active
resultStatus = TRUE
for apindex = 1 to AP2Results do
  if (apindex <> 5) then
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
      resultStatus = FALSE
      break
    endif
  else
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x37) then
      resultStatus = FALSE
      break
    endif
  endif
enddo

if (resultStatus = TRUE) then
  write "<*> Passed (8000) - AP Packed Results are as expected after ADT load."
else
  write "<!> Failed (8000) - AP Packed Results are NOT correct."
  write "  AP Packed index   = ", apindex
  write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
endif

write ";*********************************************************************"
write ";  Step 3.7: Dump the ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (3006) - Actionpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (3006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_3006, "P"  
endif

write ";*********************************************************************"
write ";  Step 3.8: Update both tables at the same time"
write ";*********************************************************************"
;; Create the load files
s $SC_$CPU_lcx_wdt2a
s $SC_$CPU_lcx_adt2a

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to load the table
s load_table("lc_def_wdt2a.tbl","$CPU")
s load_table("lc_def_adt2a.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Load commands for WDT and ADT did not execute successfully."
endif

;; Send the commands to validate each table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Validate commands for WDT and ADT did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation message rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv WDT and ADT Validation success messages"
    ut_setrequirements LCX_2005, "F"
  endif
  if ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation message rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv ADT Validation message"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (2005;3007) - WDT and/or ADT Failed Validation"
  ut_setrequirements LCX_2005, "F"
  ut_setrequirements LCX_3007, "F"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_RCVD_EID,"DEBUG", 2
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_REMOVED_EID,"DEBUG", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName
/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Activate commands for WDT and ADT did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the Table Update Success messages for both tables"
else
  write "<*> Failed - Did not rcv the Table Update Success message for both tables"
endif

;; Check for the correct number of SB Subscription messages
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 19
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the correct number of Subscription Removed messages from SB."
else
  write "<!> Failed - Expected 19 Subscription Removed messages from SB. Only rcv'd ", $sc_$cpu_find_event[3].num_found_messages
endif

if ($sc_$cpu_find_event[2].num_found_messages > 1) then
  write "<*> Passed - Rcv'd more than 1 Subscription messages from SB."
else
  write "<!> Failed - Did not get any Subscription Rcvd messages from SB."
endif

wait 5

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.9: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 31) AND ($SC_$CPU_LC_ACTIVEAPS <> 12) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after WDT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

;; Check the Actionpoint Packed Results - Not Measured & Active
resultStatus = TRUE
for apindex = 1 to AP2aResults do
  if (apindex < 7) then
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
      resultStatus = FALSE
      break
    endif
  else
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
      resultStatus = FALSE
      break
    endif
  endif
enddo

if (resultStatus = TRUE) then
  write "<*> Passed (8000) - AP Packed Results are as expected after ADT load."
else
  write "<!> Failed (8000) - AP Packed Results are NOT correct."
  write "  AP Packed index   = ", apindex
  write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
endif

write ";*********************************************************************"
write ";  Step 3.10: Dump the WRT and ADT and check counters"
write ";*********************************************************************"
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
  write "<!> Failed (2004) - Watchpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (2004) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_2004, "P"  
endif

;;  Dump the ART and check counters"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2aentries) then
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
  write "<!> Failed (3006) - Actionpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (3006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_3006, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.0:  Test Table updates with LC appplication state Disabled  "
write ";*********************************************************************"
write ";  Step 4.1:  Send command to set the LC application state to Disabled"
write ";  as well as the command to set all the defined APs to Active."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"

if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=State) then
  write "<*> Passed (1003;4002) - Set LC Application State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4002, "P"
else
  write "<!> Failed (1003;4002) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4002) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4002, "P"
else
  write "<!> Failed (1003;4002) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4002, "F"
endif

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Send the commands to update the WDT with a new table."
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_wdt2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the table
s load_table("lc_def_wdt2.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for WDT sent successfully."
else
  write "<!> Failed - Load command for WDT did not execute successfully."
endif

;; Send the commands to validate each table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for WDT sent successfully."
else
  write "<!> Failed - Validate command for WDT did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation successful message rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv WDT Validation successful message"
    ut_setrequirements LCX_2005, "F"
  endif
else
  write "<!> Failed (2005) - WDT Failed Validation"
  ut_setrequirements LCX_2005, "F"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_RCVD_EID,"DEBUG", 2
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_REMOVED_EID,"DEBUG", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for WDT sent successfully."
else
  write "<!> Failed - Activate command for WDT did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the Table Update Success message"
else
  write "<*> Failed - Did not rcv the Table Update Success message for the WDT table"
endif

;; Check for the correct number of SB Subscription messages
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 19
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the correct number of Subscription Removed messages from SB."
else
  write "<!> Failed - Expected 19 Subscription Removed messages from SB. Only rcv'd ", $sc_$cpu_find_event[3].num_found_messages
endif

if ($sc_$cpu_find_event[2].num_found_messages > 1) then
  write "<*> Passed - Rcv'd more than 1 Subscription messages from SB."
else
  write "<!> Failed - Did not get any Subscription Rcvd messages from SB."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 28) AND ($SC_$CPU_LC_ACTIVEAPS <> 12) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after WDT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

write ";*********************************************************************"
write ";  Step 4.4: Dump the WRT and check counters"
write ";*********************************************************************"
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
  write "<!> Failed (2004) - Watchpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (2004) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_2004, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.5: Send the commands to update the ADT with a new table."
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_adt2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the ADT table
s load_table("lc_def_adt2.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for ADT sent successfully."
else
  write "<!> Failed - Load command for ADT did not execute successfully."
endif

;; Send the command to validate the ADT table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 2

;; Send the command to validate the ADT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation successful message rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv ADT Validation successful message"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (3007) - ADT Failed Validation"
  ut_setrequirements LCX_3007, "F"
endif

;; Send the commands to activate the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_UPDATE_SUCCESS_INF_EID, "INFO", 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName

;; Wait until you receive the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd Table Update Success messages"
else
  write "<*> Failed - Did not rcv Table Update Success message for the ADT table"
endif

wait 5

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 28) AND ($SC_$CPU_LC_ACTIVEAPS <> 9) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after ADT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

;; Check the Actionpoint Packed Results - Not Measured & Active
local resultStatus = TRUE
for apindex = 1 to AP2Results do
  if (apindex <> 5) then
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
      resultStatus = FALSE
      break
    endif
  else
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x37) then
      resultStatus = FALSE
      break
    endif
  endif
enddo

if (resultStatus = TRUE) then
  write "<*> Passed (8000) - AP Packed Results are as expected after ADT load."
else
  write "<!> Failed (8000) - AP Packed Results are NOT correct."
  write "  AP Packed index   = ", apindex
  write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
endif

write ";*********************************************************************"
write ";  Step 4.7: Dump the ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2entries) then
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
  write "<!> Failed (3006) - Actionpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (3006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_3006, "P"  
endif

write ";*********************************************************************"
write ";  Step 4.8: Update both tables at the same time"
write ";*********************************************************************"
;; Create the load files
s $SC_$CPU_lcx_wdt2a
s $SC_$CPU_lcx_adt2a

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to load the table
s load_table("lc_def_wdt2a.tbl","$CPU")
s load_table("lc_def_adt2a.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Load commands for WDT and ADT did not execute successfully."
endif

;; Send the commands to validate each table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Validate commands for WDT and ADT did not execute successfully."
endif

;; Wait for the CFE Validation Success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation message rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv WDT and ADT Validation success messages"
    ut_setrequirements LCX_2005, "F"
  endif
  if ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation message rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv ADT Validation message"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (2005;3007) - WDT and/or ADT Failed Validation"
  ut_setrequirements LCX_2005, "F"
  ut_setrequirements LCX_3007, "F"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_RCVD_EID,"DEBUG", 2
ut_setupevents "$SC","$CPU","CFE_SB",CFE_SB_SUBSCRIPTION_REMOVED_EID,"DEBUG", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName
/$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Activate commands for WDT and ADT did not execute successfully."
endif

;; Wait for the success message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 2
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the Table Update Success messages for both tables"
else
  write "<*> Failed - Did not rcv the Table Update Success message for both tables"
endif

;; Check for the correct number of SB Subscription messages
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 19
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Rcv'd the correct number of Subscription Removed messages from SB."
else
  write "<!> Failed - Expected 19 Subscription Removed messages from SB. Only rcv'd ", $sc_$cpu_find_event[3].num_found_messages
endif

if ($sc_$cpu_find_event[2].num_found_messages > 1) then
  write "<*> Passed - Rcv'd more than 1 Subscription messages from SB."
else
  write "<!> Failed - Did not get any Subscription Rcvd messages from SB."
endif

wait 5

cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LC_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9: Check the housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_WPSINUSE <> 31) AND ($SC_$CPU_LC_ACTIVEAPS <> 12) then
  write "<!> Failed (8000) - Housekeeping telemetry NOT as expected after WDT Table Update."
  write " WPs In Use = ", $SC_$CPU_LC_WPSINUSE
  write " APs Active = ", $SC_$CPU_LC_ACTIVEAPS
  ut_setrequirements LCX_8000, "F"
else
  write "<*> Passed (8000) - Housekeeping telemetry initialized properly after table update."
  ut_setrequirements LCX_8000, "P"
endif

;; Check the Actionpoint Packed Results - Not Measured & Active
resultStatus = TRUE
for apindex = 1 to AP2aResults do
  if (apindex < 7) then
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x77) then
      resultStatus = FALSE
      break
    endif
  else
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
      resultStatus = FALSE
      break
    endif
  endif
enddo

if (resultStatus = TRUE) then
  write "<*> Passed (8000) - AP Packed Results are as expected after ADT load."
else
  write "<!> Failed (8000) - AP Packed Results are NOT correct."
  write "  AP Packed index   = ", apindex
  write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
endif

write ";*********************************************************************"
write ";  Step 4.10: Dump the WRT and ADT and check counters"
write ";*********************************************************************"
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
  write "<!> Failed (2004) - Watchpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (2004) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_2004, "P"  
endif

;;  Dump the ART and check counters"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", apAPID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADT2aentries) then
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
  write "<!> Failed (3006) - Actionpoint Results Table NOT initialized as expected."
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
  write "<*> Passed (3006) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_3006, "P"  
endif

write ";*********************************************************************"
write ";  Step 5.0: Table Validation Error Test"
write ";*********************************************************************"
write ";  Step 5.1: Create the table load files that contain validation errors"
write ";  for the WDT and ADT tables."
write ";*********************************************************************"
;; Create the load files
s $SC_$CPU_lcx_wdt3
s $SC_$CPU_lcx_adt3

cmdctr = $SC_$CPU_TBL_CMDPC + 2

;; Send the command to load the table
s load_table("lc_def_wdt3.tbl","$CPU")
s load_table("lc_def_adt3.tbl","$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load commands for WDT and ADT sent successfully."
else
  write "<!> Failed - Load command for WDT and ADT did not execute successfully."
endif

;; Send the command to validate the WDT table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_ERR_EID, "ERROR", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for WDT sent successfully."
else
  write "<!> Failed - Validate command for WDT did not execute successfully."
endif

;; Wait for the Table Validation message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - WDT Validation failed as expected."
  if ($sc_$cpu_find_event[2].num_found_messages = 1) AND ;;
     ($sc_$cpu_find_event[3].num_found_messages = 1) then
    write "<*> Passed (2005) - WDT Validation status and error messages rcv'd"
    ut_setrequirements LCX_2005, "P"
  else
    write "<!> Failed (2005) - Did not rcv the expected WDT Validation messages"
    ut_setrequirements LCX_2005, "F"
  endif
else
  write "<!> Failed (2005) - WDT passed Validation when failure was expected."
  ut_setrequirements LCX_2005, "F"
endif

;; Send the command to validate the ADT table
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_ERR_EID, "ERROR", 2
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_ERR_EID, "ERROR", 3

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to validate the WDT
/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for ADT sent successfully."
else
  write "<!> Failed - Validate command for ADT did not execute successfully."
endif

;; Wait for the Table Validation message
ut_tlmwait $sc_$cpu_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - ADT Validation failed as expected."
  if ($sc_$cpu_find_event[1].num_found_messages = 1) AND ;;
     ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (3007) - ADT Validation status and error messages rcv'd"
    ut_setrequirements LCX_3007, "P"
  else
    write "<!> Failed (3007) - Did not rcv the expected ADT Validation messages"
    ut_setrequirements LCX_3007, "F"
  endif
else
  write "<!> Failed (3007) - ADT passed Validation when failure was expected."
  ut_setrequirements LCX_3007, "F"
endif

write ";*********************************************************************"
write ";  Step 6.0:  Clean-up"
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
write ";  End procedure $SC_$CPU_lcx_tabletesting "
write ";*********************************************************************"
ENDPROC
