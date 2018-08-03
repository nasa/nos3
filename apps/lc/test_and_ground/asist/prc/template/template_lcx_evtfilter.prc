PROC $sc_$cpu_lcx_evtfilter
;*******************************************************************************
;  Test Name:  lcx_evtfilter
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker eXtended (LCX)
;	application properly filters the Pass to Fail and Fail to Pass 
;	transition events when the table-defined maximum number of times has 
;	been reached for an ActionPoint. Several data runs are performed using
;	AP 5, 6, and 9 to reach the filtered limit. The Expected Results section
;	contains a grid detailing the expected results from each data run.
;
;  Requirements Tested
;   LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message
;   LCX3001	Upon receipt of a Sample Request, LCX shall process the request
;		specified actionpoints defined in the Actionpoint Definition
;		Table (ADT) if the LCX Application State is one of the
;		following:
;                 a) Active
;                 b) Passive
;   LCX3001.2	If the equation result for an Actionpoint results in a Pass, LCX
;		shall set the Number of Consecutive Fail values to zero
;   LCX3002	For each table-defined Actionpoint, LCX shall store the results
;		in the dump-only Actionpoint Results Table if the Actionpoint 
;		state is either:
;                 a) Active
;                 b) Passive
;   LCX3002.1	If the Actionpoint equation results in a transition from PASS to
;		FAIL, LCX shall issue an event message indicating the failure
;   LCX3002.1.1	If the PASS to FAIL transition event message has been sent for
;		the table-defined number of times, LCX shall apply the 
;		table-defined event message filter.
;   LCX3002.2	If the Actionpoint equation results in a transition from FAIL to
;		PASS, LCX shall issue an event message indicating that the
;		actionpoint is now within limits
;   LCX3002.2.1	If the FAIL to PASS transition event message has been sent for
;		the table-defined number of times, LCX shall apply the 
;		table-defined event message filter.
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
;                 a) The result of the last Sample(Pass,Fail,Error,or Stale)
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
;                 d) Current LCX State to <PLATFORM_DEFINED> Default Power-on
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
;                 f) The cumulative number of times the equation result =Failed
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
;	The LCX application utilizes the command and telemetry mnemonics of LC
;	not LCX. Therefore, changes may be required if one is switching from LC
;	to LCX.
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
;       ftp_file      Procedure to load file containing a table
;       lcx_wdt1       Sets up the Watchpoint Definition table files for testing
;       lcx_adt5       Sets up the Actionpoint Definition table files for testing
;
;  Expected Test Results and Analysis
;  The grid below shows the expected values in the ART table after each data
;  run. There are only 4 defined data runs which have to be executed multiple
;  times in order to test the event filtering per action point. Thus, the grid
;  below shows the actual data run executed in sequence.

;  Data                                      Psv Cons Cum  RTS
;  Run #  AP#  EID  Result  State  F2P  P2F  RTS  FC   FC  Ctr
;          5   None  Pass   Active  0    0    0    0    0   0
;   1      6   None  Fail   Active  0    0    0    1    1   0
;          9   1010  Fail   Passiv  0    0    0    1    1   1
;  -------------------------------------------------------------
;          5    58   Fail   Active  0    1    0    1    1   0
;   2      6    61   Pass   Active  1    0    0    0    1   0
;          9    60   Fail   Passiv  0    0    1    2    2   1
;  -------------------------------------------------------------
;          5    61   Pass   Active  1    1    0    0    1   0
;   3      6    58   Fail   Active  1    1    0    1    2   0
;          9    61   Pass   Passiv  1    0    1    0    2   1
;  -------------------------------------------------------------
;          5    58   Fail   Active  1    2    0    1    2   0
;   4      6    61   Pass   Active  2    1    0    0    2   0
;          9   None  Pass   Passiv  1    0    1    0    2   1
;  -------------------------------------------------------------
;          5    61   Pass   Active  2    2    0    0    2   0
;   1      6    58   Fail   Active  2    2    0    1    3   0
;          9    58   Fail   Passiv  1    1    2    1    3   1
;  -------------------------------------------------------------
;          5   None  Fail   Active  2    3    0    1    3   0
;   2      6    61   Pass   Active  3    2    0    0    3   0
;          9   None  Fail   Passiv  1    1    3    2    4   1
;  -------------------------------------------------------------
;          5   None  Pass   Active  3    3    0    0    3   0
;   3      6    58   Fail   Active  3    3    0    1    4   0
;          9   None  Pass   Passiv  2    1    3    0    4   1
;  -------------------------------------------------------------
;          5   None  Fail   Active  3    4    0    1    4   0
;   4      6   None  Pass   Active  4    3    0    0    4   0
;          9   None  Pass   Passiv  2    1    3    0    4   1
;  -------------------------------------------------------------
;          5   None  Pass   Active  4    4    0    0    4   0
;   1      6   None  Fail   Active  4    4    0    1    5   0
;          9   None  Fail   Passiv  2    2    4    1    5   1
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "to_lab_events.h"
#include "lc_msgdefs.h"
#include "lc_platform_cfg.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"

%liv (log_procedure) = logging

#define LCX_1003       0
#define LCX_3001       1
#define LCX_30012      2
#define LCX_3002       3
#define LCX_30021      4
#define LCX_300211     5
#define LCX_30022      6
#define LCX_300221     7
#define LCX_30023      8
#define LCX_300231     9
#define LCX_30024     10
#define LCX_3006      11
#define LCX_4000      12
#define LCX_8000      13
#define LCX_9000      14      
#define LCX_9001      15
#define LCX_9002      16

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

#define CMDFAIL       1
#define CMDSUCCESS    2

global ut_req_array_size = 16
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_3001", ;;
   "LCX_3001.2","LCX_3002","LCX_3002.1","LCX_3002.1.1","LCX_3002.2", ;;
   "LCX_3002.2.1","LCX_3002.3","LCX_3002.3.1","LCX_3002.4","LCX_3006", ;;
   "LCX_4000","LCX_8000","LCX_9000","LCX_9001","LCX_9002"] 

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL stream1
LOCAL WRTAppid
LOCAL ARTAppid
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADTentries = 12
LOCAL WDTentries = 30
LOCAL CmdStatus 
LOCAL State
Local maxwp = LC_MAX_WATCHPOINTS - 1
Local maxap = LC_MAX_ACTIONPOINTS - 1
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
write ";  Step 1.2: Create the WDT and ADT table loads used for testing and "
write ";  upload them to the proper location. Also, display the LC pages."
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

s $SC_$CPU_lcx_adt5

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_adt5.tbl", adtFileName, "$CPU", "P")

;; Display the pages
page $SC_$CPU_LC_HK
page $SC_$CPU_TST_LC_HK
page $SC_$CPU_LC_ADT
page $SC_$CPU_LC_WDT
page $SC_$CPU_LC_ART
page $SC_$CPU_LC_WRT

write ";*********************************************************************"
write ";  Step 1.3:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions."
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2

s load_start_app (LCAppName,"$CPU", "LC_AppMain")

; Wait for app startup events
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_num_found_messages = 1) then
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
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_num_found_messages = 1) then
    write "<*> Passed - TST_LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_LC not received."
    write "Event Message count = ",$SC_$CPU_num_found_messages
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

;; Set the SC HK packet ID and table AppIDs based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0A7"
ARTAppid = "0FB8"
WRTAppid = "0FB9"

if ("$CPU" = "CPU2") then
  hkPktId = "p1A7"
  ARTAppid = "0FD6"
  WRTAppid = "0FD7"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2A7"
  ARTAppid = "0FF6"
  WRTAppid = "0FF7"
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
;first check the 10 APs that are being used
;;255 is because they are disabled and not measured
  for apindex = 1 to 10 do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
      break                                   
    endif
  enddo
;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 11 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
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
      write "  WP Packed Results  = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
    endif
    if (apindex < APACKED) then
      write "  AP Packed Results  = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
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

  if (wpindex < WPACKED) then
    write "  WP Packed Results = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
  endif
  if (apindex < APACKED) then
    write "  AP Packed Results = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
  endif
  ut_setrequirements LCX_9000, "F"
endif

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", WRTAppid)
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
  write "<!> Failed (9001) - Watchpoint Results Table NOT initialized at startup. Index = ",index
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
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
  write "<!> Failed (9002) - Actionpoint Results Table NOT initialized at startup. Index = ",index
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
write ";  Step 2.0:  Send packets to make APs 5 and 6 transition with each "
write ";  data pass."
write ";*********************************************************************"
write ";  Step 2.1:  Set LC Application State to Active Command"
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

if ($SC_$CPU_num_found_messages = 1) THEN
  write "<*> Passed (1003;4000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4000, "P"
else
  write "<!> Failed (1003;4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4000, "F"
endif

State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State
 
wait 5

write ";*********************************************************************"
write ";  Step 2.2 Send packets for data run 1"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.2",1)

write ";*********************************************************************"
write ";  Step 2.3 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
local expectedEID = LC_BASE_AP_EID + 10

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, {expectedEID}, "INFO", 2

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.3) - Received RTS event message"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30023, "P"
else
  write "<!> Failed (3001;3002;3002.3) - Did not receive RTS event message."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30023, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4 Dump ART and check counters"   
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

;; Set the ART table requirement to pass
ut_setrequirements LCX_3006, "P"

;; Check if AP 9's state is Passive
if (p@$SC_$CPU_LC_ART[9].CurrentState = "Passive") then
  write "<*> Passed (3002.3.1) - Action Point state set to passive after RTS execution"
  ut_setrequirements LCX_300231, "P"
else
  write "<!> Failed (3002.3.1) - Action Point 9 state is not correct. Should have been 'Passive'."
  ut_setrequirements LCX_300231, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5 Send packets for data run 2"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.5",2)

write ";*********************************************************************"
write ";  Step 2.6 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 3
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSIVE_FAIL_INF_EID, "INFO", 4

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30021, "P"
else
  write "<!> Failed (3001;3002;3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30021, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.2) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30022, "P"
else
  write "<!> Failed (3001;3002;3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30022, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[4].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.4) - Received AP Passive event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30024, "P"
else
  write "<!> Failed (3001;3002;3002.4) - Did not receive AP Passive event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30024, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

;; Test that the Consecutive Fail Counter for AP 6 has been set to zero (0)
if ($SC_$CPU_LC_ART[6].ConsecutiveFailCount = 0) then
  write "<*> Passed (3001.2) - Consecutive Fail Count reset when result passed"
  ut_setrequirements LCX_30012, "P"
else
  write "<!> Failed (3001.2) - Consecutive Fail Count was not reset to zero when the AP result passed."
  ut_setrequirements LCX_30012, "F"
endif

write ";*********************************************************************"
write ";  Step 2.8 Send packets for data run 3"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.8",3)

write ";*********************************************************************"
write ";  Step 2.9 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 3

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.2) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30022, "P"
else
  write "<!> Failed (3001;3002;3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30022, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30021, "P"
else
  write "<!> Failed (3001;3002;3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30021, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 2
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.2) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30022, "P"
else
  write "<!> Failed (3001;3002;3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30022, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.10 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6, AP 6 = index 7, and AP 9 = 10
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

write ";*********************************************************************"
write ";  Step 2.11 Send packets for data run 4"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.11",4)

write ";*********************************************************************"
write ";  Step 2.12 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 3

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30021, "P"
else
  write "<!> Failed (3001;3002;3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30021, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.2) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30022, "P"
else
  write "<!> Failed (3001;3002;3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30022, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.13 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

write ";*********************************************************************"
write ";  Step 2.14 Send packets for data run 1 a second time"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.14",1)

write ";*********************************************************************"
write ";  Step 2.15 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 3

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.2) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30022, "P"
else
  write "<!> Failed (3001;3002;3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30022, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30021, "P"
else
  write "<!> Failed (3001;3002;3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30021, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 2
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30021, "P"
else
  write "<!> Failed (3001;3002;3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30021, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.16 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

write ";*********************************************************************"
write ";  Step 2.17 Send packets for data run 2 a second time"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.17",2)

write ";*********************************************************************"
write ";  Step 2.18 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 3

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  write "<*> Passed (3001;3002;3002.1.1) - Did not receive Pass to Fail transition event because it was filtered."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_300211, "P"
else
  write "<!> Failed (3001;3002;3002.1.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300211, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.2) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30022, "P"
else
  write "<!> Failed (3001;3002;3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30022, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.19 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

wait 5

write ";*********************************************************************"
write ";  Step 2.20 Send packets for data run 3 a second time"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.20",3)

write ";*********************************************************************"
write ";  Step 2.21 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 3

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  if ($SC_$CPU_find_event[2].num_found_messages = 0) then
    write "<*> Passed (3001;3002;3002.2.1) - Did not receive Fail to Pass transition event because it was filtered."
    ut_setrequirements LCX_3001, "P"
    ut_setrequirements LCX_3002, "P"
    ut_setrequirements LCX_300221, "P"
  else
    write "<!> Failed (3001;3002;3002.2.1) - Number of events for Fail to Pass transition is not zero."
    ut_setrequirements LCX_3001, "F"
    ut_setrequirements LCX_3002, "F"
    ut_setrequirements LCX_300221, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3001;3002;3002.2.1) - Received Fail to Pass transition event when it should have been filtered and not received."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300221, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (3001;3002;3002.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_30021, "P"
else
  write "<!> Failed (3001;3002;3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_30021, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  if ($SC_$CPU_find_event[2].num_found_messages = 0) then
    write "<*> Passed (3001;3002;3002.2.1) - Did not receive Fail to Pass transition event because it was filtered."
    ut_setrequirements LCX_3001, "P"
    ut_setrequirements LCX_3002, "P"
    ut_setrequirements LCX_300221, "P"
  else
    write "<!> Failed (3001;3002;3002.2.1) - Number of events for Fail to Pass transition is not zero."
    ut_setrequirements LCX_3001, "F"
    ut_setrequirements LCX_3002, "F"
    ut_setrequirements LCX_300221, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3001;3002;3002.2.1) - Received Fail to Pass transition event when it should have been filtered and not received."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300221, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.22 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

wait 5

write ";*********************************************************************"
write ";  Step 2.23 Send packets for data run 4 a second time"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.23",4)

write ";*********************************************************************"
write ";  Step 2.24 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 3


/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  if ($SC_$CPU_find_event[2].num_found_messages = 0) then
    write "<*> Passed (3002.1.1) - Did not receive Pass to Fail transition event because it was filtered."
    ut_setrequirements LCX_3001, "P"
    ut_setrequirements LCX_3002, "P"
    ut_setrequirements LCX_300211, "P"
  else
    write "<!> Failed (3002.1.1) - Number of events for Pass to Fail transition is not zero."
    ut_setrequirements LCX_3001, "F"
    ut_setrequirements LCX_3002, "F"
    ut_setrequirements LCX_300211, "F"
    write "Event Message count = ",$SC_$CPU_find_event[2].num_found_messages
  endif
else
  write "<!> Failed (3002.1.1) - Received Pass to Fail transition event"
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300211, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  if ($SC_$CPU_find_event[3].num_found_messages = 0) then
    write "<*> Passed (3002.2.1) - Did not receive Fail to Pass transition event because it was filtered."
    ut_setrequirements LCX_3001, "P"
    ut_setrequirements LCX_3002, "P"
    ut_setrequirements LCX_300221, "P"
  else
    write "<!> Failed (3002.2.1) - Number of events for Fail to Pass transition is not zero."
    ut_setrequirements LCX_3001, "F"
    ut_setrequirements LCX_3002, "F"
    ut_setrequirements LCX_300221, "F"
    write "Event Message count = ",$SC_$CPU_find_event[3].num_found_messages
  endif
else
  write "<!> Failed (3002.2.1) - Received Fail to Pass transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300221, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.25 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

wait 5

write ";*********************************************************************"
write ";  Step 2.26 Send packets for data run 1 a third time"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendpackets("2.26",1)

write ";*********************************************************************"
write ";  Step 2.27 Send Sample Request for APs 5,6 and 9."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 3

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=6 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  if ($SC_$CPU_find_event[2].num_found_messages = 0) then
    write "<*> Passed (3001;3002;3002.2.1) - Did not receive Fail to Pass transition event because it was filtered."
    ut_setrequirements LCX_3001, "P"
    ut_setrequirements LCX_3002, "P"
    ut_setrequirements LCX_300221, "P"
  else
    write "<!> Failed (3001;3002;3002.2.1) - Number of events for Fail to Pass transition is not zero."
    ut_setrequirements LCX_3001, "F"
    ut_setrequirements LCX_3002, "F"
    ut_setrequirements LCX_300221, "F"
    write "Event Message count = ",$SC_$CPU_find_event[2].num_found_messages
  endif
else
  write "<!> Failed (3001;3002;3002.2.1) - Received Fail to Pass transition event"
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300221, "F"
endif

ut_tlmwait $SC_$CPU_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Failure) THEN
  if ($SC_$CPU_find_event[3].num_found_messages = 0) then
    write "<*> Passed (3001;3002;3002.1.1) - Did not receive Pass to Fail transition event because it was filtered."
    ut_setrequirements LCX_3001, "P"
    ut_setrequirements LCX_3002, "P"
    ut_setrequirements LCX_300211, "P"
  else
    write "<!> Failed (3001;3002;3002.1.1) - Number of events for Pass to Fail transition is not zero."
    ut_setrequirements LCX_3001, "F"
    ut_setrequirements LCX_3002, "F"
    ut_setrequirements LCX_300211, "F"
    write "Event Message count = ",$SC_$CPU_find_event[3].num_found_messages
  endif
else
  write "<!> Failed (3001;3002;3002.1.1) - Received Pass to Fail transition event."
  ut_setrequirements LCX_3001, "F"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_300211, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

if ($SC_$CPU_find_event[1].num_found_messages = 2) then
  write "<*> Passed - Received the correct number of Sample events"
else
  write "<!> Failed - Expected 2 Sample events; rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.28 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

;; Write out AP info
;; Note that LC internally stores AP starting at index 0. However, ASIST stores
;; arrays starting at 1. So, AP 5 = index 6 and AP 6 = index 7
write "  Action Point 5:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[5].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[5].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[5].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[5].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[5].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[5].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[5].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[5].CumulativeRTSExecCount
write " "
write "  Action Point 6:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[6].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[6].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[6].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[6].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[6].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[6].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[6].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[6].CumulativeRTSExecCount
write " "
write "  Action Point 9:"
write "  Action Result          = ", p@$SC_$CPU_LC_ART[9].ActionResult
write "  Current State          = ", p@$SC_$CPU_LC_ART[9].CurrentState
write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[9].FailToPassCount
write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[9].PassToFailCount
write "  Passive AP Count       = ", $SC_$CPU_LC_ART[9].PassiveAPCount
write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[9].ConsecutiveFailCount
write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[9].CumulativeFailCount
write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[9].CumulativeRTSExecCount
write " "

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Clean-up"
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
write ";  End procedure $SC_$CPU_lcx_evtfilter"
write ";*********************************************************************"
ENDPROC
