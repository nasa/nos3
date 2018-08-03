PROC $sc_$cpu_lcx_stress
;*******************************************************************************
;  Test Name:  lcx_stress
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker (LC) functions
;	properly when monitoring the maximum number of WatchPoints (WP). All
;	evaluate to false. The maximum number of ActionPoints (AP) also evaluate
;	to fail.
;
;  Requirements Tested
;   LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message
;   LCX2000	The flight software shall monitor a maximum of
;		<PLATFORM_DEFINED> Watchpoints.
;   LCX2003	Upon receipt of a message, LCX shall compare the data in the
;		message to the table-defined value using the table-defined
;		comparison value and comparison operator for each data point
;               defined in the Watchpoint Definition Table (WDT) if the LCX
;		Application State is one of the following:
;                 a) Active
;                 b) Passive
;   LCX2003.2	LCX shall support the following comparison values:
;                 a) =
;                 b) !=
;                 c) >
;                 d) >=
;                 e) <
;                 f) <=
;   LC2003.5	If the Watchpoint cannot be evaluated, LCX shall set the 
;		Watchpoint Results Table to Error for the erroneous Watchpoint.
;   LCX2004	For each Watchpoint, the flight software shall maintain the
;		following statistics in the dump-only Watchpoint Results Table:
;                 a) The result of the last relational comparison (False, True,
;		     Error or Stale)
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;		     False to True result
;                 d) The number of consecutive times the comparison has yielded
;		     a True result
;                 e) The cumulative number of times the comparison has yielded a
;		     True result
;                 f) Most recent FALSE to TRUE transition value
;                 g) Most recent FALSE to TRUE transition timestamp
;                 h) Most recent TRUE to FALSE transition value
;                 i) Most recent TRUE to FALSE transition timestamp
;   LCX3000	Limit Checker shall support up to a maximum of
;		<PLATFORM_DEFINED> Actionpoints.
;   LCX3001	Upon receipt of a Sample Request, LCX shall process the request
;		specified actionpoints defined in the Actionpoint Definition
;		Table (ADT) if the LCX Application State is one of the
;		following:
;                 a) Active
;                 b) Passive
;   LCX3001.1	LCX shall support the following Reverse Polish Operators:
;                 a) and
;                 b) or
;                 c) xor
;                 d) not
;                 e) equals
;   LCX3001.3	If the Actionpoint cannot be evaluated, LCX shall set the
;		Actionpoint Results Table to ERROR for the erroneous Actionpoint
;   LCX3002	Each table-defined Actionpoint shall be evaluated and the 
;		results stored in the dump-only Actionpoint Results Table if the
;		Actionpoint state is either:
;                 a) Active
;                 b) Passive
;   LCX3002.3	If the equation has yielded a Fail result for the table-defined
;		consecutive number of times limit and the Actionpoint is
;		currently Active, LCX shall:
;                 a) generate an event message
;                 b) send a command to start the table-defined  RTS
;                 c) Increment the counter indicating Total count of commands
;		     sent to SC task to start an RTS
;   LCX3002.3.1	Once an RTS is initiated, LCX shall change the current state of
;		the associated Actionpoint to Passive.
;   LCX3002.4	If the equation has yielded a Fail result for the defined
;		consecutive number of times and the Actionpoint is currently
;		Passive, LCX shall
;                 a) generate an event message indicating that the Actionpoint
;		     Failed but the action was not taken
;                 b) Increment the counter indicating Number of Start RTS
;		     commands NOT sent to SC task because LCX Application is
;		     PASSIVE
;   LCX3006	For each Actionpoint, the flight software shall maintain the
;		following statistics in the dump-only Actionpoint Results Table:
;                 a) The result of the last Sample(Pass,Fail,Error,Stale)
;                 b) The current state (PermOff,Disabled,Active,Passive,Unused)
;                 c) The number of times this Actionpoint has crossed from the
;		     Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;		     Pass to Fail state
;                 e) The number of consecutive times the equation result =Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;   LCX4000	Upon receipt of a Set LCX Application State To Active Command,
;		LCX shall set the state of the LCX Application to Active
;   LCX4003	Upon receipt of a Set Actionpoint to Active Command, LCX shall
;		set the state of the command-specified Actionpoint to ACTIVE
;		such that the actionpoint is evaluated and the table-defined 
;		actions are taken based on the evaluation
;   LCX4004	Upon receipt of a Set All Actionpoints to Active Command, LCX
;		shall set the state for all Actionpoints to ACTIVE such that
;		the actionpoints are evaluated and the table-defined actions are
;		taken based on the evaluation
;   LCX4006	Upon receipt of a Set All Actionpoints to Passive Command, LCX
;		shall set the state for the all Actionpoints to PASSIVE such
;		that all actionpoints are evaluated, however, no actions are
;		taken
;   LCX8000	LCX shall generate a housekeeping message containing the
;		following:
;                 a) Valid  Command Counter
;                 b) Command Rejected Counter
;                 c) Number of Start RTS commands NOT sent to SC task because
;		     LCX Application is PASSIVE
;                 d) Current LCX Application State (LCX_ACTIVE, LCX_PASSIVE,
;		     LCX_DISABLED)...
;                 e) Total count of actionpoints sampled while LCX_ACTIVE or
;		     LCX_PASSIVE...
;                 f) Total count of packets monitored for watchpoints (cmd and
;		     tlm)
;                 g) Total count of commands sent to SC task to start an RTS
;                 h) Selected data from watchpoint results table
;                 i) Selected data from actionpoint results table
;   LCX9000	Upon cFE Power-On LCX shall initialize the following
;		Housekeeping data to Zero (or value specified):
;                 a) Valid Command Counter
;                 b) Command Rejected Counter
;                 c) Passive RTS Execution Counter
;                 d) Current LC State to <PLATFORM_DEFINED> Default Power-on
;		     State
;                 e) Actionpoint Sample Count
;                 f) TLM Count
;                 g) RTS Execution Counter
;                 h) Watch Results (bitmapped)
;                 i) Action Results (bitmapped)
;   LCX9001	Upon cFE Power-On LCX shall initialize the following Watchpoint
;		data to Zero (or value specified) for all Watchpoints:
;                 a) The result of the last watchpoint relational comparison to
;		     STALE
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;		     False to True result
;                 d) The number of consecutive times the comparison has yielded
;		     a True result
;                 e) The cumulative number of times the comparison has yielded a
;		     True result
;                 f) The value that caused the last False-to-True crossing, and
;		     the crossing time stamp
;                 g) The value that caused the last True-to-False crossing, and
;		     the crossing time stamp
;   LCX9002	Upon cFE Power-On LCX shall initialize the following Actionpoint
;		data to Zero (or value specified for all Actionpoints:
;                 a) The result of the last Actionpoint Sample to STALE
;                 b) The current state as defined in the ADT
;                 c) The number of times this Actionpoint has crossed from the
;		     Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;		     Pass to Fail state
;                 e) The number of consecutive times the equation result =Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
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
;	08/10/12	Walt Moleski	Original Procedure for LCX
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
;       ftp_file      Procedure to load file containing a table
;       lcx_wdt1      Sets up the Watchpoint Definition table files for testing
;       lcx_adt1      Sets up the Actionpoint Definition table files for testing
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
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"
#include "sc_events.h"

%liv (log_procedure) = logging

#define LCX_1003       0
#define LCX_2000       1
#define LCX_2003       2
#define LCX_20032      3
#define LCX_20035      4
#define LCX_2004       5
#define LCX_3000       6
#define LCX_3001       7
#define LCX_30011      8
#define LCX_30013      9
#define LCX_3002      10
#define LCX_30023     11
#define LCX_300231    12
#define LCX_30024     13
#define LCX_3006      14
#define LCX_4000      15
#define LCX_4003      16
#define LCX_4004      17
#define LCX_4006      18
#define LCX_8000      19
#define LCX_9000      20      
#define LCX_9001      21
#define LCX_9002      22

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

global ut_req_array_size = 22
global ut_requirement[0 .. ut_req_array_size]
global rts001_started

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_2000", ;;
    "LCX_2003","LCX_2003.2","LCX_2003.5","LCX_2004","LCX_3000","LCX_3001", ;;
    "LCX_3001.1","LCX_3001.3","LCX_3002","LCX_3002.3","LCX_3002.3.1", ;;
    "LCX_3002.4","LCX_3006","LCX_4000","LCX_4003","LCX_4004","LCX_4006", ;;
    "LCX_8000","LCX_9000","LCX_9001","LCX_9002"] 

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL wrtAppId, artAppId
LOCAL CmdStatus 
LOCAL State
Local rdlindex
Local maxwp = LC_MAX_WATCHPOINTS - 1
Local maxap = LC_MAX_ACTIONPOINTS - 1
local msgID
local Pattern[32]
local LCAppName = LC_APP_NAME
local ramDir = "RAM:0"
local defaultTblDir = "CF:0/apps"
local ARTTblName = LCAppName & ".LC_ART"
local ADTTblName = LCAppName & ".LC_ADT"
local WRTTblName = LCAppName & ".LC_WRT"
local WDTTblName = LCAppName & ".LC_WDT"

;;; Need to set the message ID and data pattern based upon the cpu being used
;;; CPU1 is the default
msgID = 0x989

if ("$CPU" = "CPU2") then
  msgID = 0xa89
elseif ("$CPU" = "CPU3") then
  msgID = 0x889
endif

Pattern[1] = 0xff
Pattern[2] = 0xff
Pattern[3] = 0xff
Pattern[4] = 0xff
Pattern[5] = 0xff
Pattern[6] = 0xff
Pattern[7] = 0xff
Pattern[8] = 0x13
Pattern[9] = 0x46
Pattern[10] = 0xff
Pattern[11] = 0xf4
Pattern[12] = 0xff
Pattern[13] = 0xff
Pattern[14] = 0xff
Pattern[15] = 0xff
Pattern[16] = 0xff
for index = 17 to 32 do
  Pattern[index] = 0
enddo

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
write ";  Step 1.2: Create the WDT and ADT table loads used for testing and "
write ";  upload them to the proper location. Also, display the LC pages."
write ";********************************************************************"
s $SC_$CPU_lcx_wdt4

;; Parse the filename configuration parameters for the default table filenames
local wdtFileName = LC_WDT_FILENAME
local slashLoc = %locate(wdtFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  wdtFileName = %substring(wdtFileName,slashLoc+1,%length(wdtFileName))
  slashLoc = %locate(wdtFileName,"/")
enddo

write "==> Default LC Watchpoint Table filename = '",wdtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_wdt4.tbl", wdtFileName, "$CPU", "P")

s $SC_$CPU_lcx_adt4

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_adt4.tbl", adtFileName, "$CPU", "P")

;; Display the pages
page $SC_$CPU_LC_HK
page $SC_$CPU_TST_LC_HK
page $SC_$CPU_LC_ADT
page $SC_$CPU_LC_WDT
page $SC_$CPU_LC_ART
page $SC_$CPU_LC_WRT

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
write ";  Step 1.5: Verify that the LC Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Add the HK message receipt test
local hkPktId

;; Set the SC HK packet ID, WRT and ART app ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0A7"
artAppId = "0FB8"
wrtAppId = "0FB9"

if ("$CPU" = "CPU2") then
  hkPktId = "p1A7"
  artAppId = "0FD6"
  wrtAppId = "0FD7"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2A7"
  artAppId = "0FF6"
  wrtAppId = "0FF7"
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
;;255 is because they are disabled and not measured
  for apindex = 1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus <> 255) then
      break                                   
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
    write "  CMDPC              = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC              = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE         = ", $SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT 
    write "  PASSRTSCNT         = ", $SC_$CPU_LC_PASSRTSCNT

    if (wpindex < WPACKED) then
      write "  Index of failure   = ", wpindex
      write "  WP Packed Results  = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
    endif
    if (apindex < APACKED) then
      write "  Index of Failure   = ", apindex
      write "  AP Packed Results  = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus
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
  write "  CURLCSTATE        = ", $SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
  write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT

  ut_setrequirements LCX_9000, "F"
endif

;; Dump the Watchpoint Results Table
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
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

if (index < LC_MAX_WATCHPOINTS) then
  write "<!> Failed (9001) - Watchpoint Results Table NOT initialized at startup."
  write " Index of failure    = ", index
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
else
  write "<*> Passed (9001) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
endif

;; Dump the Actionpoint Results Table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    break
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (9002) - Actionpoint Results Table NOT initialized at startup."
  write "  Index of failure       = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
else
  write "<*> Passed (9002) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
endif

write ";*********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdctr = $SC_$CPU_EVS_CMDPC + 2

;; Enable DEBUG events for the LC and CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=LCAppName DEBUG
wait 2
/$SC_$CPU_EVS_EnaAppEVTType Application="CFE_TBL" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0: Test LC Active, all AP passive, all WP trigger, and all "
write ";  AP fail."
write ";*********************************************************************"
write ";  Step 2.1:  Send a Set LC Application State to Active Command"
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

;; Set all APs to Passive
State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4006) - Set ALL AP State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4006, "P"
else
  write "<!> Failed (1003;4006) - Set ALL AP State to Passive command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDPACKET MsgId=msgID, DataSize=16, DataPattern=Pattern
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC


write ";*********************************************************************"
write ";  Step 2.3: Send Sample Request for all APs. All APs should fail but "
write ";  no RTSs should fire since they are all PASSIVE. "
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=maxap UpdateAge=0
wait 5

write ";*********************************************************************"
write ";  Step 2.4: Check housekeeping counters"
write ";*********************************************************************"
;;if ($SC_$CPU_LC_PASSRTSCNT = LC_MAX_ACTIONPOINTS) AND ;;
if ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = LC_MAX_ACTIONPOINTS) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 1) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) AND ;;
   ($SC_$CPU_LC_WPSINUSE = LC_MAX_WATCHPOINTS) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 0) THEN
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
      break
    endif
  enddo

  ;; rest should be 0x99 since they are passive and fail
  for apindex = 1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus <> 0x99) then
      break
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
      write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus,2)
    endif
      ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (2000;8000) - Housekeeping telemetry updated properly."

    ut_setrequirements LCX_2000, "P"
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (2000;8000) - Housekeeping telemetry NOT correct after processing WPs."
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
  ut_setrequirements LCX_2000, "F"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5: Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
     ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
     ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
    break
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_2003, "F"
  ut_setrequirements LCX_20032, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 2.6: Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    break
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;30024;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_30011, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30024, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;30024;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_30012, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30024, "P"
  ut_setrequirements LCX_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 2.7: Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 3.0: Test LC Passive, all AP active, all WP trigger, and all "
write ";  AP fail."
write ";*********************************************************************"
write ";  Step 3.1:  Send a Set LC Application State to Passive Command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_PASSIVE
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

;; Set all APs to Active
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4006) - Set ALL AP State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4006, "P"
else
  write "<!> Failed (1003;4006) - Set ALL AP State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDPACKET MsgId=msgID, DataSize=16, DataPattern=Pattern
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC


write ";*********************************************************************"
write ";  Step 3.3: Send Sample Request for all APs. All APs should fail but "
write ";  no RTSs should fire since the LC Application state is PASSIVE. "
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=maxap UpdateAge=0
wait 5

write ";*********************************************************************"
write ";  Step 3.4: Check housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_PASSRTSCNT = LC_MAX_ACTIONPOINTS) AND ;;
   ($SC_$CPU_LC_CURLCSTATE = LC_STATE_PASSIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = LC_MAX_ACTIONPOINTS) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 1) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) AND ;;
   ($SC_$CPU_LC_WPSINUSE = LC_MAX_WATCHPOINTS) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 0) THEN
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
      break
    endif
  enddo

  ;; rest should be 0x99 since they are passive and fail
  for apindex = 1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus <> 0x99) then
      break
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
      write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus,2)
    endif
      ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (2000;8000) - Housekeeping telemetry updated properly."

    ut_setrequirements LCX_2000, "P"
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (2000;8000) - Housekeeping telemetry NOT correct after processing WPs."
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
  ut_setrequirements LCX_2000, "F"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.5: Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
     ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
    break
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_2003, "F"
  ut_setrequirements LCX_20032, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 3.6: Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    break
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;30024;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_30011, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30024, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;30024;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_30012, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30024, "P"
  ut_setrequirements LCX_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 3.7: Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 4.0: Test LC Active, all AP active, all WP trigger, and all "
write ";  AP fail."
write ";*********************************************************************"
write ";  Step 4.1:  Send a Set LC Application State to Active Command"
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

;; Set all APs to Active
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Passive command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Start the Stored Command (SC) and TST_SC Applications. "
write ";*********************************************************************"
rts001_started = FALSE
s $sc_$cpu_sc_start_apps("3.2")

wait 5

write ";*********************************************************************"
write ";  Step 4.3: Enable DEBUG Event Messages for the SC application."
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the SC and CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application="SC" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";***********************************************************************"
write ";  Step 4.4: Create and load a valid table to RTS #10.                 "
write ";***********************************************************************"
local work = %env("WORK")
;; Need to check if the .scs file used below exists. If not, end the proc
local filename = work & "/image/$sc_$cpu_lcx_rts10_load.scs"
if NOT file_exists(filename) then
  write "<!> Failed - Expected load file '",filename, "' not found. Skipping this test."
  goto step3_8
endif
;; Using the SCP utilities, compile and build the RTS load file
compile_rts "$sc_$cpu_lcx_rts10_load" 10
s $sc_$cpu_load_ats_rts("$sc_$cpu_lcx_rts10_load","$cpu_rts010_lc")

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1

local cmdctr = $SC_$CPU_TBL_CMDPC + 1

start load_table ("$cpu_rts010_lc", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for RTS #10 sent successfully."
else
  write "<!> Failed - Load command for RTS #10 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.5: Send the Table Services Validate command for RTS #10. "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME="SC.RTS_TBL010"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - RTS #10 Table validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Validate command."
  endif
else
  write "<!> Failed - RTS #10 Table validation failed."
endif

;; Wait for the validation message
wait 20

;; If the message is rcv'd, then validation passed
if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - RTS #10 Table Validation was successful. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #10 Table validation failed. Expected Event Msg ",CFE_TBL_VALIDATION_INF_EID, " not rcv'd."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.6: Send the Table Services Activate command for RTS #10.  "
write ";***********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_LOAD_PEND_REQ_INF_EID,"DEBUG",1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",2

cmdctr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATableName="SC.RTS_TBL010"

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate RTS #10 Table command sent properly."
else
  write "<!> Failed - Activate RTS #10 Table command."
endif

wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - RTS #10 Table Updated successfully. Event Msg ",$SC_$CPU_find_event[2].eventid," rcv'd."
else
  write "<!> Failed - RTS #10 Table update failed. Event Msg not received for activate command."
endif

wait 5

write ";***********************************************************************"
write ";  Step 4.7: Send the Enable command for RTS #10. "
write ";***********************************************************************"
ut_setupevents "$SC", "$CPU", "SC", SC_ENABLE_RTS_DEB_EID, "DEBUG", 1

cmdctr = $SC_$CPU_SC_CMDPC + 1
;; Send the Command
/$SC_$CPU_SC_EnableRTS RTSID=10

ut_tlmwait  $SC_$CPU_SC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - SC Enable RTS #10 command sent properly."
else
  write "<!> Failed - SC Enable RTS #10 command did not increment CMDPC."
endif

;; Check for the event message
ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",SC_ENABLE_RTS_DEB_EID," rcv'd."
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," rcv'd. Expected Event Msg ",SC_ENABLE_RTS_DEB_EID,"."
endif

wait 5

step3_8:
write ";*********************************************************************"
write ";  Step 4.8: Send a single packet for all WPs defined in WDT. The WDT "
write ";  loaded contains the same entry for the maximum WPs that can be "
write ";  defined. Therefore, only 1 packet needs to be sent. "
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDPACKET MsgId=msgID, DataSize=16, DataPattern=Pattern
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC


write ";*********************************************************************"
write ";  Step 4.9: Send Sample Request for all APs. All APs should fail and "
write ";  RTS #10 should fire. RTS #10 contains an LC and SC NO-OP commands."
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=maxap UpdateAge=0
wait 5

write ";*********************************************************************"
write ";  Step 4.10: Check housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = LC_MAX_ACTIONPOINTS) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 1) AND ;;
   ($SC_$CPU_LC_RTSCNT = LC_MAX_ACTIONPOINTS) AND ;;
   ($SC_$CPU_LC_WPSINUSE = LC_MAX_WATCHPOINTS) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 0) THEN
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
      break
    endif
  enddo

  ;; rest should be 0x99 since they are active and fail
  for apindex = 1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus <> 0x99) then
      break
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
      write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.ALlStatus,2)
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
write ";  Step 4.11: Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
     ($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
     ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
    break
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_2003, "F"
  ut_setrequirements LCX_20032, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 4.12: Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
    break
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3002.3;3002.3.1;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_30011, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30023, "F"
  ut_setrequirements LCX_300231, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3002.3;3002.3.1;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30023, "P"
  ut_setrequirements LCX_300231, "P"
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 5.0: Anomaly Tests "
write ";*********************************************************************"
write ";  Step 5.1: Create & load a WDT containing a Watchpoint that has an "
write ";  invalid offset. "
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_wdt4a

cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the table
s load_table("lc_def_wdt4a.tbl","$CPU")

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
    write "<*> Passed - WDT Validation successful message rcv'd"
  else
    write "<!> Failed - Did not rcv WDT Validation successful message"
  endif
else
  write "<!> Failed - WDT Failed Validation"
endif

;; Send the command to activate the table
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO", 1

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

wait 5

write ";*********************************************************************"
write ";  Step 5.2: Create and load an ADT that has a AP that relies on the "
write ";  Watchpoint above. This should set the AP Result to ERROR. "
write ";*********************************************************************"
;; Create the load file
s $SC_$CPU_lcx_adt4a
                                                                                
cmdctr = $SC_$CPU_TBL_CMDPC + 1

;; Send the command to load the ADT table
s load_table("lc_def_adt4a.tbl","$CPU")
                                                                                
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
wait 10
if ($sc_$cpu_find_event[1].num_found_messages = 1) AND ;;
   ($sc_$cpu_find_event[2].num_found_messages = 1) then
  write "<*> Passed - ADT Validation successful message rcv'd"
else
  write "<!> Failed - Did not rcv ADT Validation successful message"
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

;; Set the first AP to Active
State = LC_STATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State"

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4003) - Set AP #0 State to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Set AP #0 State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3: Send a packet for the WP defined above. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WP_OFFSET_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_PACKET_INF_EID, "INFO", 2

/$SC_$CPU_TST_LC_SENDPACKET MsgId=msgID, DataSize=16, DataPattern=Pattern
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

;; Wait for the expected error message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2003.5) - Rcv'd the expected Offset Error message."
  ut_setrequirements LCX_20035, "P"
else
  write "<!> Failed (2003.5) - Event msg ",$SC_$CPU_evs_eventid," received. Expected ",LC_WP_OFFSET_ERR_EID,"."
  ut_setrequirements LCX_20035, "F"
endif

;; Verify the SendPacket Test App Command message 
if ($sc_$cpu_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Send Packet command sent properly."
else
  write "<!> Failed - Did not rcv the Send Packet command message."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4: Send a packet that WP is not subscribed to. "
write ";  Nothing should happen. "
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDPACKET MsgId=0x987, DataSize=16, DataPattern=Pattern
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 5.5: Send a sample request with an invalid AP specified. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSAMPLE_APNUM_ERR_EID, "ERROR", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 2

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=LC_MAX_ACTIONPOINTS EndAP=LC_MAX_ACTIONPOINTS UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

;; Wait for the expected error message
ut_tlmwait $sc_$cpu_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Rcv'd the expected AP Sample Error message."
  ut_setrequirements LCX_3000, "P"
else
  write "<!> Failed (3000) - Event msg ",$SC_$CPU_evs_eventid," received. Expected ",LC_APSAMPLE_APNUM_ERR_EID,"."
  ut_setrequirements LCX_3000, "F"
endif

;; Verify the SendPacket Test App Command message 
if ($sc_$cpu_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Send Sample command sent properly."
else
  write "<!> Failed - Did not rcv the Send Sample command message."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6: Send a Sample command for the first AP. "
write ";*********************************************************************"
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=0 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 5.7: Dump WRT and check WP #1 for Error Result "
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
wait 5

if ($SC_$CPU_LC_WRT[0].WatchResults = LC_WATCH_ERROR) then
  write "<*> Passed (2003.5) - WP #0 indicates ERROR in the Results."
  ut_setrequirements LCX_20035, "P"
else
  write "<!> Failed (2003.5) - WP #0 was not set to ERROR as expected."
  ut_setrequirements LCX_20035, "F"
endif

write ";*********************************************************************"
write ";  Step 5.8: Dump ART and AP #1 for Error Result "
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

if ($SC_$CPU_LC_ART[0].ActionResult = LC_ACTION_ERROR) then
  write "<*> Passed (3001.3) - AP #0 indicates ERROR in the Results."
  ut_setrequirements LCX_30013, "P"
else
  write "<!> Failed (3001.3) - AP #0 was not set to ERROR as expected."
  ut_setrequirements LCX_30013, "F"
endif

write ";*********************************************************************"
write ";  Step 6.0: Clean-up"
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
write ";  End procedure $SC_$CPU_lcx_stress "
write ";*********************************************************************"
ENDPROC
