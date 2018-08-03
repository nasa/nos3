PROC $sc_$cpu_lcx_gencmds
;*******************************************************************************
;  Test Name:  lcx_gencmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker eXtended (LCX)
;	application general commands function properly. All LCX commands will be
;	tested as well as invalid commands and an application reset to see if
;	the LCX application behaves appropriately
;
;  Requirements Tested
;     LCX1000	Upon receipt of a No-Op command, LCX shall increment the LCX
;		Valid Command Counter and generate an event message.
;     LCX1001	Upon receipt of a Reset command, LCX shall reset the following
;		housekeeping variables to a value of zero:
;		  a) Valid Command Counter
;		  b) Command Rejected Counter
;		  c) Passive RTS Execution Counter
;		  d) Actionpoint Sample Count
;		  e) TLM Count
;		  f) RTS Execution Counter
;     LCX1002	For all LCX commands, if the length contained in the message
;		header is not equal to the expected length, LC shall reject the
;		command and issue an event message.
;     LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message.
;     LCX1004	If LCX rejects any command, LCX shall abort the command
;		execution, increment the LCX Command Rejected Counter and issue
;		an event message
;     LCX2004	For each Watchpoint, the flight software shall maintain the
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
;		  j) Most recent comparison age
;     LCX3006	For each Actionpoint, the flight software shall maintain the
;               following statistics in the dump-only Actionpoint Results Table:
;                 a) The result of the last Sample(Pass,Fail,Error,or Stale)
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
;     LCX4000	Upon receipt of a Set LCX Application State To Active Command,
;		LCX shall set the state of the LCX Application to Active
;     LCX4001	Upon receipt of a Set LCX Application State to Passive Command,
;               LCX shall set the LCX Application State to Passive
;     LCX4002	Upon receipt of a Set LCX Application State to Disable Command,
;               LCX shall set the LCX Application State to Disabled
;     LCX4003	Upon receipt of a Set Actionpoint to Active Command, LCX shall
;               set the state for the command-specified Actionpoint to ACTIVE
;               such that the actionpoint is evaluated and the table-defined
;               actions are taken based on the evaluation
;     LCX4004	Upon receipt of a Set All Actionpoints to Active Command, LCX
;               shall set the state for all Actionpoints to ACTIVE such that the
;               actionpoints are evaluated and the table-defined actions are
;               taken based on the evaluation
;     LCX4005   Upon receipt of a Set Actionpoint to Passive Command, LCX shall
;               set the state for the command-specified Actionpoint to PASSIVE
;               such that the actionpoint is evaluated, however, no actions
;               are taken
;     LCX4006   Upon receipt of a Set All Actionpoints to Passive Command, LCX
;               shall set the state for the all Actionpoints to PASSIVE such
;               that all actionpoints are evaluated, however, no actions are
;               taken
;     LCX4007   Upon receipt of a Set Actionpoint to Disabled Command, LCX shall
;               set the state for the command-specified Actionpoint to DISABLED
;               such that the actionpoints are not evaluated and no actions
;               are taken
;     LCX4008   Upon receipt of a Set All Actionpoints to Disabled Command, LCX
;               shall set the state for all Actionpoint to DISABLED such that:
;                 a) the actionpoints are not evaluated
;                 b) no actions are taken
;                 c) no event messages generated.
;     LCX4009   Upon receipt of a Set Actionpoint to Permanent Disable, LCX
;		shall mark the command-specified Actionpoint such that the
;		Actionpoint cannot be Activated
;     LCX4009.1	If a command is received to Activate an Actionpoint which has
;		been permanently disabled, the command shall be rejected.
;     LCX4010	Upon receipt of a Reset Actionpoint Statistics command, LCX
;		shall set to zero, all the following Actionpoint Statistics for
;		the command-specified Actionpoints:
;		   a) Total number of FAIL to PASS transitions
;		   b) Total number of PASS to FAIL transitions
;		   c) Number of consecutive FAIL results
;		   d) Total number of FAIL results
;		   e) Total number of RTS executions
;		   f) Total number of event messages sent relating to that
;		      Actionpoint
;     LCX4011	Upon receipt of a Reset All Actionpoint Statistics command, LCX
;		shall set to zero, all the following Actionpoint Statistics for
;		all Actionpoints:
;		   a) Total number of FAIL to PASS transitions
;		   b) Total number of PASS to FAIL transitions
;		   c) Number of consecutive FAIL results
;		   d) Total number of FAIL results
;		   e) Total number of RTS executions
;		   f) Total number of event messages sent relating to that
;		      Actionpoint
;     LCX4012	Upon receipt of a Reset Watchpoint Statistics command, LCX shall
;		set to zero all of the following Watchpoint Statistics for the
;		command-specified Watchpoints:
;		   a) Total sample count for this watchpoint
;		   b) Number of times result transitioned from FALSE to TRUE
;		   c) Number of consecutive TRUE results
;		   d) Total number of TRUE results
;		   e) Most recent FALSE to TRUE transistion value
;		   f) Most recent FALSE to TRUE transistion timestamp
;		   g) Most recent TRUE to FALSE transistion value
;		   h) Most recent TRUE to FALSE transistion timestamp
;     LCX4013	Upon receipt of a Reset All Watchpoint Statistics command, LCX
;		shall set to zero all of the following Watchpoint Statistics for
;		all Watchpoints:
;		   a) Total sample count for this watchpoint
;		   b) Number of times result transitioned from FALSE to TRUE
;		   c) Number of consecutive TRUE results
;		   d) Total number of TRUE results
;		   e) Most recent FALSE to TRUE transistion value
;		   f) Most recent FALSE to TRUE transistion timestamp
;		   g) Most recent TRUE to FALSE transistion value
;		   h) Most recent TRUE to FALSE transistion timestamp
;     LCX8000	LC shall generate a housekeeping message containing the
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
;     LCX9000	Upon cFE Power-On LCX shall initialize the following
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
;     LCX9001	Upon cFE Power-On LCX shall initialize the following Watchpoint
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
;     LCX9002	Upon cFE Power-On LCX shall initialize the following Actionpoint
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
;       lcx_wdt1           Sets up the Watchpoint Definition table files for
;			  testing
;       lcx_adt1           Sets up the Actionpoint Definition table files for
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
#include "to_lab_events.h"
#include "lc_msgdefs.h"
#include "lc_platform_cfg.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"

%liv (log_procedure) = logging

#define LCX_1000       0
#define LCX_1001       1
#define LCX_1002       2
#define LCX_1003       3
#define LCX_1004       4
#define LCX_2004       5
#define LCX_3006       6
#define LCX_4000       7
#define LCX_4001       8
#define LCX_4002       9
#define LCX_4003      10
#define LCX_4004      11
#define LCX_4005      12
#define LCX_4006      13
#define LCX_4007      14
#define LCX_4008      15
#define LCX_4009      16
#define LCX_40091     17
#define LCX_4010      18
#define LCX_4011      19
#define LCX_4012      20
#define LCX_4013      21
#define LCX_8000      22
#define LCX_9000      23
#define LCX_9001      24
#define LCX_9002      25

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

#define CMDFAIL       1
#define CMDSUCCESS    2
global ut_req_array_size = 25
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1000","LCX_1001", ;;
    "LCX_1002","LCX_1003","LCX_1004","LCX_2004","LCX_3006","LCX_4000", ;;
    "LCX_4001","LCX_4002","LCX_4003","LCX_4004","LCX_4005","LCX_4006", ;;
    "LCX_4007","LCX_4008","LCX_4009","LCX_4009.1","LCX_4010","LCX_4011", ;;
    "LCX_4012","LCX_4013","LCX_8000","LCX_9000","LCX_9001","LCX_9002"]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL index
LOCAL wpindex, wrtAppId
LOCAL apindex, artAppId
LOCAL ADTentries = 12
LOCAL APResults = ADTentries/2
LOCAL WDTentries = 30
LOCAL CmdStatus 
LOCAL State
Local rdlindex
Local cmdctr
Local maxwp = LC_MAX_WATCHPOINTS - 1
Local maxap = LC_MAX_ACTIONPOINTS - 1
local expectedResults
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

;; Build the Actionpoint Definition Table
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

;; Set the LC HK packet ID, WRT and ART appIds based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0A7"
wrtAppId = "0FB9"
artAppId = "0FB8"

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
;;first check the 12 APs that are being used
;;255 is because they are disabled and not measured
  for apindex = 1 to APResults do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
      break                                   
    endif
  enddo
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = APResults+1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
      break                                   
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000) - WP or AP Housekeeping telemetry NOT initialized at startup."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", $SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed Results = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
    endif
    if (apindex < APACKED) then
      write "  AP Packed Results = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
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
  write "  CURLCSTATE        =", $SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT       =", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT         =", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT            =", $SC_$CPU_LC_RTSCNT 
  if (wpindex < WPACKED) then
    write "  WP Packed Results = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
  endif
  if (apindex < APACKED) then
    write "  AP Packed Results = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
  endif
  ut_setrequirements LCX_9000, "F"
endif

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

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (9001) - Watchpoint Results Table NOT initialized at startup. Index = ",index
  write "  Index of failure   =", index
  write " WatchResults        =", $SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    =", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count =", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    =", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      =", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        =", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        =", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LCX_9001, "F"
else
  write "<*> Passed (9001) - Watchpoint Results Table initialized properly."
  ut_setrequirements LCX_9001, "P"  
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
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
  write "  Index of failure       =", index
  write "  Action Results         =", $SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          =", $SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     =", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     =", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count =", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         =", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          =", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LCX_9002, "F"
else
  write "<*> Passed (9002) - Actionpoint Results Table initialized properly."
  ut_setrequirements LCX_9002, "P"  
endif

write ";*********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages "
write ";*********************************************************************"
cmdctr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the SC and CFE_TBL application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=LCAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0:  No-op Testing"
write ";*********************************************************************"
write ";  Step 2.1:  Send valid No-op command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_NOOP_INF_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_LC_NOOP"
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1000;1003) - NOOP command sent properly."
  ut_setrequirements LCX_1000, "P"
  ut_setrequirements LCX_1003, "P"
else
  write "<!> Failed (1000;1003) - NOOP command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1000, "F"
  ut_setrequirements LCX_1003, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1000;1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1000, "P"
  ut_setrequirements LCX_1003, "P"
else
  write "<!> Failed (1000;1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_NOOP_INF_EID, "."
  ut_setrequirements LCX_1000, "F"
  ut_setrequirements LCX_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2:  Send No-op command with invalid command length"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000200BD"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000200BD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000200BD"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Reset Testing"
write ";*********************************************************************"
write ";  Step 3.1:  Send valid Reset command"
write ";*********************************************************************"
;; use test app to set the counters to non-zero
/$SC_$CPU_TST_LC_SETCOUNTERS

wait 10

if ($SC_$CPU_LC_CMDPC <> 0) and ($SC_$CPU_LC_CMDEC <> 0) and ;;
   ($SC_$CPU_LC_PASSRTSCNT <> 0) and ($SC_$CPU_LC_APSAMPLECNT <> 0) and ;;
   ($SC_$CPU_LC_MONMSGCNT <> 0) and ($SC_$CPU_LC_RTSCNT <> 0) then
  ut_setupevents "$SC", "$CPU", {LCAppName}, LC_RESET_DBG_EID, "DEBUG", 1 
  /$SC_$CPU_LC_RESETCTRS
  wait 5

  ut_tlmwait $SC_$CPU_LC_CMDPC, 0
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001;1003) - Valid Command Counter was reset."
    ut_setrequirements LCX_1001, "P"
    ut_setrequirements LCX_1003, "P"
  else
    write "<!> Failed (1001;1003) - Valid Command Counter was NOT reset (",ut_tw_status,")."
    ut_setrequirements LCX_1001, "F"
    ut_setrequirements LCX_1003, "F"
  endif

  ut_tlmwait $SC_$CPU_LC_CMDEC, 0
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001) - Command Rejected Counter was reset."
    ut_setrequirements LCX_1001, "P"
  else
    write "<!> Failed (1001) - Command Rejected Counter was NOT reset (",ut_tw_status,")."
    ut_setrequirements LCX_1001, "F"
  endif

  ut_tlmwait $SC_$CPU_LC_PASSRTSCNT, 0
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001) - Count of RTS sequences not initiated because application state is passive was reset."
    ut_setrequirements LCX_1001, "P"
  else
    write "<!> Failed (1001) - Count of RTS sequences not initiated because application state is passive was NOT reset (",ut_tw_status,")."
    ut_setrequirements LCX_1001, "F"
  endif

  ut_tlmwait $SC_$CPU_LC_APSAMPLECNT, 0
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001) - Total Count of APs Sampled was reset."
    ut_setrequirements LCX_1001, "P"
  else
    write "<!> Failed (1001) - Total Count of APs Sampled was NOT reset (",ut_tw_status,")."
    ut_setrequirements LCX_1001, "F"
  endif

  ut_tlmwait $SC_$CPU_LC_MONMSGCNT, 0
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001) - Total Count of Messages Monitored was reset."
    ut_setrequirements LCX_1001, "P"
  else
    write "<!> Failed (1001) - Total Count of Messages Monitored was NOT reset (",ut_tw_status,")."
    ut_setrequirements LCX_1001, "F"
  endif

  ut_tlmwait $SC_$CPU_LC_RTSCNT, 0
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (1001) - Total Count of RTS Sequences Initiated was reset."
    ut_setrequirements LCX_1001, "P"
  else 
    write "<!> Failed (1001) - Total Count of RTS Sequences Initiated was NOT reset (",ut_tw_status,")."
    ut_setrequirements LCX_1001, "F"
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements LCX_1001, "P"
    ut_setrequirements LCX_1003, "P"
  else
    write "<!> Failed (1001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_RESET_DBG_EID, "."
    ut_setrequirements LCX_1001, "F"
    ut_setrequirements LCX_1003, "F"
  endif

  wait 5
else
  write ";*********************************************************************"
  write ";  ERROR IN TEST APPLICATION:  SetCounters did not work"
  write ";*********************************************************************"
endif

write ";*********************************************************************"
write ";  Step 3.2:  Send Reset command with invalid command length"
write ";*******************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000201BC"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000201BC"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000201BC"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  Application State Testing"
write ";  This set of steps does not verify that LC operates correctly in each"
write ";  state. They verify that the state changed when the Set State Command"
write ";  is issued"
write ";"
write ";  NOTE: All possible transitions from active to passive to disabled"
write ";        are tested."
write ";*********************************************************************"
write ";  Step 4.1:  Send Set LC Application State to Active"
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
write ";  Step 4.2:  Send Set LC Application State to Passive"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1

State = LC_STATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
  write "<*> Passed (1003;4001) - Set LC Application State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4001, "P"
else
  write "<!> Failed (1003;4001) - Set LC Application State to Passive command not sent properly (", ut_sc_status, ")."
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

wait 5

write ";*********************************************************************"
write ";  Step 4.3:  Send Set LC Application State to Disable"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1

State = LC_STATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_DISABLED) then
  write "<*> Passed (1003;4002) - Set LC Application State to Disabled command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4002, "P"
else
  write "<!> Failed (1003;4002) - Set LC Application State to Disabled command not sent properly (", ut_sc_status, ")."
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

wait 5

write ";*********************************************************************"
write ";  Step 4.4:  Send Set LC Application State to Active"
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
write ";  Step 4.5:  Send Set LC Application State to Disable"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1

State = LC_STATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"

if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_DISABLED) then
  write "<*> Passed (1003;4002) - Set LC Application State to Disable command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4002, "P"
else
  write "<!> Failed (1003;4002) - Set LC Application State to Disable command not sent properly (", ut_sc_status, ")."
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

wait 5

write ";*********************************************************************"
write ";  Step 4.6:  Send Set LC Application State to Passive"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1

State = LC_STATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
  write "<*> Passed (1003;4001) - Set LC Application State to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4001, "P"
else
  write "<!> Failed (1003;4001) - Set LC Application State to Passive command not sent properly (", ut_sc_status, ")."
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

wait 5

write ";*********************************************************************"
write ";  Step 4.7:  Send Set LC Application State to Active"
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
write ";  Step 4.8:  Send Set LC Application State with an invalid"
write  "; command length     "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the deafult
rawcmd = "18a4c000000602BC00010000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000602BC00010000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000602BC00010000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9:  Send Set LC Application State with an invalid state"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000502BC00060000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000502BC00060000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000502BC00060000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_LCSTATE_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0:  Set AP Commands Testing"
write ";  NOTE: This does not test that data is processed correctly. All "
write ";  possible transitions from active to passive to disabled are tested."
write ";*********************************************************************"
write ";  Step 5.1:  Send Set AP to Active Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 
 
;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.2:  Using Set AP to Active Command activate remainder of APs"
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Active command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Active."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.3:  Send Set AP to Passive Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_PASSIVE

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4005, "F"
endif
 
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_PASSIVE) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4005) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4005, "P"
else
  write "<!> Failed (1003;4005) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4:  Send the Set AP to Passive Command for the remaining APs"
write ";*********************************************************************"
State = LC_APSTATE_PASSIVE
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Passive command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_PASSIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Passive."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Passive as expected. State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.5:  Send the Set AP to Disabled Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4007) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4007, "P"
else
  write "<!> Failed (1003;4007) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4007, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 
 
;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_PASSIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4007) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4007, "P"
else
  write "<!> Failed (1003;4007) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6:  Send the Set AP to Disabled Command for the remaining APs"
write ";*********************************************************************"
state = LC_APSTATE_DISABLED
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Disabled command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Disabled."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Disabled as expected. State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.7:  Send the Set AP to Active Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_ACTIVE

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif
   
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5
 
;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.8: Send the Set AP to Active Command for the remaining APs"
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Active command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Active."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Active as expected. State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.9:  Send the Set AP to Disabled Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_DISABLED

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4007) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4007, "P"
else
  write "<!> Failed (1003;4007) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4007, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 
 
;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4007) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4007, "P"
else
  write "<!> Failed (1003;4007) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.10: Send the Set AP to Disabled Command for the remaining APs"
write ";*********************************************************************"
State = LC_APSTATE_DISABLED
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Disabled command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Disabled."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Disabled as expected. State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.11:  Send the Set AP to Passive Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_PASSIVE

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4005, "P"
else
  write "<!> Failed (1003;4005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4005, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 
 
;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_PASSIVE) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_DISABLED) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4005) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4005, "P"
else
  write "<!> Failed (1003;4005) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.12: Sent the Set AP to Passive Command for the remaining APs"
write ";*********************************************************************"
State = LC_APSTATE_PASSIVE
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Passive command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_PASSIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Passive."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Passive as expected. State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.13:  Send the Set AP to Active Command for the first AP."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1
index = 0
State = LC_APSTATE_ACTIVE

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5
 
;; Check the State of each entry
expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if (rdlindex = 0) then
    if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
      expectedResults = FALSE
      break
    endif
  elseif ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_PASSIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - AP states are set as expected."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Active as expected. Actual State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.14: Send the Set AP to Active Command for the remaining APs"
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

for index = 1 to ADTentries-1 do

  ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

  if (ut_sc_status <> UT_SC_Success) then
    write "<!> Failed (1003;4003) - Set AP to Active command not sent properly (", ut_sc_status, "). Index of failure = ", index
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4003, "F"
    break
  endif
enddo

;; Check if the correct number of event messages were received
if ($SC_$CPU_find_event[1].num_found_messages = ADTentries - 1) then
  write "<*> Passed (1003;4003) - The correct number of Event messages were received."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - Did not rcv correct number of Event messages. Expected ",ADTentries - 1, " rcv'd ", $SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif 

;; Dump the Results table
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

expectedResults = TRUE
for rdlindex = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState <> LC_APSTATE_ACTIVE) then
    expectedResults = FALSE
    break
  endif
enddo

if (expectedResults = TRUE) then
  write "<*> Passed (1003;4003) - All AP States were set to Active."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
else
  write "<!> Failed (1003;4003) - AP #", rdlindex, " was not set to Active as expected. State = ",p@$SC_$CPU_LC_ART[rdlindex].CurrentState
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
endif

write ";*********************************************************************"
write ";  Step 5.15:  Send the Set AP State with an invalid command length"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000603BC00010000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000603BC00010000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000603BC00010000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.16:  Send the Set AP State command with an invalid state"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_NEW_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
;;rawcmd = "18a4c000000503BC00010006"
;;
;;if ("$CPU" = "CPU2") then
;;  rawcmd = "19a4c000000503BC00010006"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1aa4c000000503BC00010006"
;;endif
;;
;;ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

;; This does not have to be a raw command since there are no restrictions on
;; the NewAPState argument
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=4"

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATE_NEW_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.17:  Send Set AP using an AP with state NOT USED"
write ";  The ADT defines only 12 APs, so anything above that is unused"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_CURR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
index = 13
State = LC_APSTATE_ACTIVE

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=index NewAPState=State"

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATE_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0:  Set All AP Commands Testing"
write ";*********************************************************************"
write ";  Step 6.1:  Using the Set AP commands set to a mix of active, passive,"
write  "; and disabled. The ADT defines only 12 APs."
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=5 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=8 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=10 NewAPState=State"
State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=1 NewAPState=State" 
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=4 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=6 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=7 NewAPState=State"
State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=3 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=9 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=11 NewAPState=State"

;; Dump the ART to verify the state settings above
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

write ";*********************************************************************"
write ";  Step 6.2:  Set ALL APs to Active"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"
CmdStatus = CMDSUCCESS

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

for index = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) then
     CmdStatus = CMDFAIL
     break
  endif
enddo
for index = ADTentries to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) then
     CmdStatus = CMDFAIL
     break
  endif
enddo

if (ut_sc_status = UT_SC_Success) and (CmdStatus = CMDSUCCESS) then
  write "<*> Passed (1003;4004) - Set All APs to Active command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4004, "P"
else
  write "<!> Failed (1003;4004) - Set All APs to Active command not sent properly (", ut_sc_status, "). index = ", index
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3:  Using the Set AP commands set to a mix of active, passive,"
write ";  and disabled"
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=5 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=8 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=10 NewAPState=State"
State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=1 NewAPState=State" 
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=4 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=6 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=7 NewAPState=State"
State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=3 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=9 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=11 NewAPState=State"

;; Dump the ART to verify the state settings above
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

write ";*********************************************************************"
write ";  Step 6.4:  Set ALL APs to Passive"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"
CmdStatus = CMDSUCCESS

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4006) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4006, "P"
else
  write "<!> Failed (1003;4006) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4006, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

for index = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) then
     CmdStatus = CMDFAIL
     break
  endif
enddo
for index = ADTentries to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) then
     CmdStatus = CMDFAIL
     break
  endif
enddo

if (ut_sc_status = UT_SC_Success) and (CmdStatus = CMDSUCCESS) then
  write "<*> Passed (1003;4006) - Set All APs to Passive command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4006, "P"
else
  write "<!> Failed (1003;4006) - Set All APs to Passive command not sent properly (", ut_sc_status, "). index = ",index
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.5: Using the Set AP commands set to a mix of active, passive,"
write ";  and disabled"
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=5 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=8 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=10 NewAPState=State"
State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=1 NewAPState=State" 
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=4 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=6 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=7 NewAPState=State"
State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=3 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=9 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=11 NewAPState=State"

;; Dump the ART to verify the state settings above
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

write ";*********************************************************************"
write ";  Step 6.6:  Set ALL APs to Disabled"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_INF_EID, "INFO", 1

State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"
CmdStatus = CMDSUCCESS

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4008) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4008, "P"
else
  write "<!> Failed (1003;4008) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4008, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

for index = 0 to ADTentries-1 do
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) then
     CmdStatus = CMDFAIL
     break
  endif
enddo
for index = ADTentries to LC_MAX_ACTIONPOINTS-1 do
   if ($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) then
     CmdStatus = CMDFAIL
     break
   endif
enddo

if (ut_sc_status = UT_SC_Success) and (CmdStatus = CMDSUCCESS) then
  write "<*> Passed (1003;4008) - Set All APs to Disabled command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4008, "P"
else
  write "<!> Failed (1003;4008) - Set All APs to Disabled command not sent properly (", ut_sc_status, "). index = ",index
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.7:  Send Set all AP with an invalid state"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_NEW_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=4"

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATE_NEW_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.0:  Set AP to Permanent Disable Testing"
write ";*********************************************************************"
write ";  Step 7.1:  Using the Set AP commands set to a mix of active, passive,"
write  "; and disabled.  The ADT defines only 10 APs."
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=5 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=8 NewAPState=State"
State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=1 NewAPState=State" 
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=4 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=6 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=7 NewAPState=State"
State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=3 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=9 NewAPState=State"

;; Dump the ART to verify the state settings above
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

write ";*********************************************************************"
write ";  Step 7.2:  Set 4 AP to Permanent Disabled (1 Active, 1 Passive,"
write ";  1 Disabled, 1 Not Used)"
write ";*********************************************************************"
write ";  Step 7.2.1: Try to Permanently Disable an AP that is Active. This "
write ";  should fail."
write ";*********************************************************************"
errcnt = $SC_$CPU_LC_CMDEC + 1
rdlindex = 0

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APOFF_CURR_ERR_EID, "ERROR", 1

/$SC_$CPU_LC_SETAPPERMOFF APNumber=0

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.2.2: Try to Permanently Disable an AP that is Passive. This "
write ";  should fail."
write ";*********************************************************************"
errcnt = $SC_$CPU_LC_CMDEC + 1
rdlindex = 4

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APOFF_CURR_ERR_EID, "ERROR", 1

/$SC_$CPU_LC_SETAPPERMOFF APNumber=4

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1

if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.2.3: Try to Permanently Disable an AP that is Disabled. This "
write ";  should pass."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APOFF_INF_EID, "INFO", 1
rdlindex = 2

ut_sendcmd "$SC_$CPU_LC_SETAPPERMOFF APNumber=2"

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4009) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4009, "P"
else
  write "<!> Failed (1003;4009) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_INF_EID, "."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4009, "F"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5 

if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_ART[rdlindex].CurrentState = LC_APSTATE_PERMOFF) then
  write "<*>  NOTE:  AP 2 is currently Permanently Disabled. "
  write "<*> Passed (1003;4009) -Set AP to Permanently Disabled command properly sent."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4009, "P"
else
  write "<!> Failed (1003;4009) - Set AP to Permanently Disabled  command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.2.2: Try to Permanently Disable an AP that is Not Used. This "
write ";  should fail."
write ";*********************************************************************"
errcnt = $SC_$CPU_LC_CMDEC + 1

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APOFF_CURR_ERR_EID, "ERROR", 1

/$SC_$CPU_LC_SETAPPERMOFF APNumber=12

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}

if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.3:  Send Set AP to Active for an AP that is Permanently "
write ";  Disabled. Using AP 2 for this test.  This should fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_CURR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
State = LC_APSTATE_ACTIVE

/$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4009.1) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
  ut_setrequirements LCX_40091, "P"
else
  write "<!> Failed (1002;1004;4009.1) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
  ut_setrequirements LCX_40091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.4:  Send Set AP to Passive for an AP that is Permanently "
write ";  Disabled. Using AP 2 for this test.  This should fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_CURR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
State = LC_APSTATE_PASSIVE

/$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4009.1) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
  ut_setrequirements LCX_40091, "P"
else
  write "<!> Failed (1002;1004;4009.1) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
  ut_setrequirements LCX_40091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.5:  Send Set AP to Disabled for an AP that is Permanently "
write ";  Disabled. Using AP 2 for this test. This should fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_CURR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
State = LC_APSTATE_DISABLED

/$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSTATE_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;4009.1) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
  ut_setrequirements LCX_40091, "P"
else
  write "<!> Failed (1002;1004;4009.1) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
  ut_setrequirements LCX_40091, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.6:  Send Set AP to Permanently Disabled for an AP that is "
write ";  Permanently Disabled. Using AP 2 for this test. Since the state is "
write ";  not Disabled this test should fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APOFF_CURR_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

/$SC_$CPU_LC_SETAPPERMOFF APNumber=2

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) AND ;;
   ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (1004) - Command Rejected Counter incremented and Event message ",$SC_$CPU_find_event[1].eventid, " received."
  ut_setrequirements LCX_1004, "P"
elseif ($SC_$CPU_LC_CMDEC <> errcnt) then
  write "<!> Failed (1004) - Command Rejected Counter did not increment as expected."
elseif ($SC_$CPU_find_event[1].num_found_messages <> 1) then
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 8.0:  Reset WP and AP Statistics Testing"
write ";  This series of tests verify that the statistics are reset correctly."
write ";  The steps to get the WRT and ART to be non-zero do no verify the "
write ";  correctness of the data"
write ";*********************************************************************"
write ";  Step 8.1 Make the WRT counters non-zero and then issue the WRT reset"
write ";  for each WP individually"
write ";*********************************************************************"

CmdStatus = CMDSUCCESS

;; use test app to set all the WRT counters to non-zero
/$SC_$CPU_TST_LC_SETWRT
wait 5

;; Dump the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
wait 5

for rdlindex = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($sc_$cpu_LC_WRT[rdlindex].EvaluationCount = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].FtoTValue  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].TtoFValue  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp  = 0) then
    write ";*********************************************************************"
    write ";  ERROR IN TEST APPLICATION:  SetWRT did not work"
    write ";*********************************************************************"
    CmdStatus = CMDFAIL
  endif
enddo

if (CmdStatus = CMDSUCCESS) then
  local midWP = maxwp / 2

  ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WPSTATS_INF_EID, "INFO", 1

  /$SC_$CPU_LC_RESETWPSTATS WPNumber=0
  /$SC_$CPU_LC_RESETWPSTATS WPNumber=maxwp
  /$SC_$CPU_LC_RESETWPSTATS WPNumber=midWP

  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 3
  if (UT_TW_Status = UT_Success) THEN
    write "<*> Passed (1003;4012) - Event messages ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4012, "P"
  else
    write "<!> Failed (1003;4012) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_WPSTATS_INF_EID, " 3 times."
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4012, "F"
  endif

  s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
  wait 5

  expectedResults = TRUE
  for rdlindex = 0 to LC_MAX_WATCHPOINTS-1 do
    if  (rdlindex = 0) or (rdlindex = midWP) or (rdlindex = maxwp) then
      if ($sc_$cpu_LC_WRT[rdlindex].EvaluationCount <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].FtoTValue  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].TtoFValue  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp  <> 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp  <> 0) then 
        write "<!> Failed (1003;2004;4012) - Reset WP Statistics did not clear index (",rdlindex, ") as expected."
        ut_setrequirements LCX_1003, "F"
        ut_setrequirements LCX_2004, "F"
        ut_setrequirements LCX_4012, "F"
	expectedResults = FALSE
        break
      endif
    else
      if ($sc_$cpu_LC_WRT[rdlindex].EvaluationCount = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].FtoTValue  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].TtoFValue  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp  = 0) then 
        write "<!> Failed (1003;2004;4012) - WP Statistics cleared for an index (",rdlindex, ") that was not commanded to clear"
        write "Evaluation Count        = ", $sc_$cpu_LC_WRT[rdlindex].EvaluationCount
        write "False to True Count     = ", $sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount
        write "Consecutive True Count  = ", $sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount 
        write "Cumulative True Count   = ", $sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount
        write "False to True Value     = ", $sc_$cpu_LC_WRT[rdlindex].FtoTValue
        write "Time Stamp (seconds)    = ", $sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp
        write "Time Stamp (subseconds) = ", $sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp
        write "True to False Value     = ", $sc_$cpu_LC_WRT[rdlindex].TtoFValue
        write "Time Stamp (seconds)    = ", $sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp 
        write "Time Stamp (subseconds) = ", $sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp
        ut_setrequirements LCX_1003, "F"
        ut_setrequirements LCX_2004, "F"
        ut_setrequirements LCX_4012, "F"
	expectedResults = FALSE
        break
      endif
    endif
  enddo

  if (expectedResults = TRUE) then
    write "<*> Passed (1003;2004;4012) Reset WP Statistics commands executed as expected."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_2004, "P"
    ut_setrequirements LCX_4012, "P"
  endif
endif

write ";*********************************************************************"
write ";  Step 8.2 Make the ART counters non-zero and then issue the ART reset"
write ";  for each AP individually"
write ";*********************************************************************"

CmdStatus = CMDSUCCESS

;; use test app to set the ART counters to non-zero
/$SC_$CPU_TST_LC_SETART
wait 5

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

for rdlindex = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($sc_$cpu_LC_ART[rdlindex].FailToPassCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].PassToFailCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].CumulativeFailCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount = 0) then 
    write ";*********************************************************************"
    write ";  ERROR IN TEST APPLICATION:  SetART did not work"
    write ";*********************************************************************"
    CmdStatus = CMDFAIL
  endif
enddo

if (CmdStatus = CMDSUCCESS) then
  local midAP = maxap / 2

  ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATS_INF_EID, "INFO", 1

  /$SC_$CPU_LC_RESETAPSTATS APNumber=0
  /$SC_$CPU_LC_RESETAPSTATS APNumber=maxap
  /$SC_$CPU_LC_RESETAPSTATS APNumber=midAP

  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 3
  if (UT_TW_Status = UT_Success) THEN
    write "<*> Passed (1003;4010) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4010, "P"
  else
    write "<!> Failed (1003;4010) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_INF_EID, "."
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4010, "F"
  endif

  s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
  wait 5

  expectedResults = TRUE
  for rdlindex = 0 to LC_MAX_ACTIONPOINTS-1 do
    if  (rdlindex = 0) or (rdlindex = midAP) or (rdlindex = maxap) then
      if ($sc_$cpu_LC_ART[rdlindex].FailToPassCount <> 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].PassToFailCount <> 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount <> 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].CumulativeFailCount <> 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount <> 0) then 
        write "<!> Failed (1003;3006;4010) - Reset AP Statistics did not clear index (", rdlindex, ") as expected."
        write "Fail to Pass Count     = ",$sc_$cpu_LC_ART[rdlindex].FailToPassCount
        write "Pass to Fail Count     = ",$sc_$cpu_LC_ART[rdlindex].PassToFailCount
        write "Consecutive Fail Count = ",$sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount
        write "Cumulative Fail Count  = ",$sc_$cpu_LC_ART[rdlindex].CumulativeFailCount
        write "Cumulative RTS Count   = ",$sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount 
        ut_setrequirements LCX_1003, "F"
        ut_setrequirements LCX_3006, "F"
        ut_setrequirements LCX_4010, "F"
	expectedResults = FALSE
        break
      endif
    else
      if ($sc_$cpu_LC_ART[rdlindex].FailToPassCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].PassToFailCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].CumulativeFailCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount = 0) then 
        write "<!> Failed (1003;3006;4010) - Reset AP Statistics cleared an index (", rdlindex, ") that was not commanded to clear."
        write "Fail to Pass Count     = ",$sc_$cpu_LC_ART[rdlindex].FailToPassCount
        write "Pass to Fail Count     = ",$sc_$cpu_LC_ART[rdlindex].PassToFailCount
        write "Consecutive Fail Count = ",$sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount
        write "Cumulative Fail Count  = ",$sc_$cpu_LC_ART[rdlindex].CumulativeFailCount
        write "Cumulative RTS Count   = ",$sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount 
        ut_setrequirements LCX_1003, "F"
        ut_setrequirements LCX_3006, "F"
        ut_setrequirements LCX_4010, "F"
	expectedResults = FALSE
        break
      endif
    endif
  enddo

  if (expectedResults = TRUE) then
    write "<*> Passed (1003;3006;4010) Reset AP Statistics command executed as expected."
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_3006, "P"
    ut_setrequirements LCX_4010, "P"
  endif
endif

write ";*********************************************************************"
write ";  Step 8.3 Test Reset WP stats with invalid command length"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000606BC000A0000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000606BC000A0000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000606BC000A0000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5


write ";*********************************************************************"
write ";  Step 8.4 Test Reset AP stats with invalid command length"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000605BC000A0000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000605BC000A0000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000605BC000A0000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 9.0:  Reset All WP and AP Statistics Testing"
write ";  This series of tests verify that the statistics are reset correctly."
write ";  These steps to get the WRT and ART to be non-zero do not verify the"
write ";  correctness of the data"
write ";*********************************************************************"
write ";  Step 9.1 Make the WRT counters non-zero and then issue the WRT reset"
write ";  all command."
write ";*********************************************************************"

CmdStatus = CMDSUCCESS

;; use test app to set the WRT counters to non-zero
/$SC_$CPU_TST_LC_SETWRT
wait 5

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
wait 5

for rdlindex = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($sc_$cpu_LC_WRT[rdlindex].EvaluationCount = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].FtoTValue  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].TtoFValue  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp  = 0) or ;;
     ($sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp  = 0) then
    write ";*********************************************************************"
    write ";  ERROR IN TEST APPLICATION:  SetWRT did not work"
    write ";*********************************************************************"
    CmdStatus = CMDFAIL
  endif
enddo

if (CmdStatus = CMDSUCCESS) then
  ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WPSTATS_INF_EID, "INFO", 1
  cmdctr = $SC_$CPU_LC_CMDPC + 1
  /$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
  ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1

  if (UT_TW_Status = UT_Success) THEN
    write "<*> Passed (1003;4013) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4013, "P"
  else       
    write "<!> Failed (1003;4013) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_INF_EID, "."
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4013, "F"
  endif

  s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtAppId)
  wait 5

  ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}

  if (UT_TW_status = UT_Success) then
    for rdlindex = 0 to LC_MAX_WATCHPOINTS-1 do
      if ($sc_$cpu_LC_WRT[rdlindex].EvaluationCount = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].FtoTValue  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].TtoFValue  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp  = 0) and ;;
	 ($sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp  = 0) then 
	write "<*> Passed (1003;2004;4013) Reset All WP Statistics command properly sent."
	ut_setrequirements LCX_1003, "P"
	ut_setrequirements LCX_2004, "P"
	ut_setrequirements LCX_4013, "P"
      else
	write "<!> Failed (1003;2004;4013) - Reset All WP Statistics command not sent properly (", ut_sc_status, ")."
	write "WP                      = ", rdlindex
	write "Evaluation Count        = ", $sc_$cpu_LC_WRT[rdlindex].EvaluationCount
	write "False to True Count     = ", $sc_$cpu_LC_WRT[rdlindex].FalsetoTrueCount
	write "Consecutive True Count  = ", $sc_$cpu_LC_WRT[rdlindex].ConsectiveTrueCount 
	write "Cumulative True Count   = ", $sc_$cpu_LC_WRT[rdlindex].CumulativeTrueCount
	write "False to True Value     = ", $sc_$cpu_LC_WRT[rdlindex].FtoTValue
	write "Time Stamp (seconds)    = ", $sc_$cpu_LC_WRT[rdlindex].SecFtoTTimeStamp
	write "Time Stamp (subseconds) = ", $sc_$cpu_LC_WRT[rdlindex].SubSecFtoTTimeStamp
	write "True to False Value     = ", $sc_$cpu_LC_WRT[rdlindex].TtoFValue
	write "Time Stamp (seconds)    = ", $sc_$cpu_LC_WRT[rdlindex].SecTtoFTimeStamp 
	write "Time Stamp (subseconds) = ", $sc_$cpu_LC_WRT[rdlindex].SubSecTtoFTimeStamp
	ut_setrequirements LCX_1003, "F"
	ut_setrequirements LCX_2004, "F"
	ut_setrequirements LCX_4013, "F"
	break
      endif
    enddo
  else
    write "<!> Failed (1003;2004;4013) - Reset All WP Statistics command not sent properly (", ut_sc_status, ")."
    write "WP                      = ", rdlindex
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_2004, "F"
    ut_setrequirements LCX_4013, "F"
  endif
endif

write ";*********************************************************************"
write ";  Step 9.2 Make the ART counters non-zero and then issue the ART reset"
write ";  all command. "
write ";*********************************************************************"

CmdStatus = CMDSUCCESS

;; use test app to set the ART counters to non-zero
/$SC_$CPU_TST_LC_SETART
wait 5

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
wait 5

for rdlindex = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($sc_$cpu_LC_ART[rdlindex].FailToPassCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].PassToFailCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].CumulativeFailCount = 0) or ;;
     ($sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount = 0) then 
    write ";*********************************************************************"
    write ";  ERROR IN TEST APPLICATION:  SetART did not work"
    write ";*********************************************************************"
    CmdStatus = CMDFAIL
  endif
enddo

if (CmdStatus = CMDSUCCESS) then
  ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATS_INF_EID, "INFO", 1
  cmdctr = $SC_$CPU_LC_CMDPC + 1
  /$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
 
  if (UT_TW_Status = UT_Success) THEN
    write "<*> Passed (1003;4011) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements LCX_1003, "P"
    ut_setrequirements LCX_4011, "P"
  else       
    write "<!> Failed (1003;4011) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APOFF_INF_EID, "."
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_4011, "F"
  endif

  s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artAppId)
  wait 5

  ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
 
  if (UT_TW_status = UT_Success) then
    for rdlindex = 0 to LC_MAX_ACTIONPOINTS-1 do
      if ($sc_$cpu_LC_ART[rdlindex].FailToPassCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].PassToFailCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].CumulativeFailCount = 0) and ;;
	 ($sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount = 0) then 
	write "<*> Passed (1003;3006;4011) Reset All AP Statistics command properly sent."
	ut_setrequirements LCX_1003, "P"
	ut_setrequirements LCX_3006, "P"
	ut_setrequirements LCX_4011, "P"
      else
	write "<!> Failed (1003;3006;4011) - Reset All AP Statistics command not sent properly (", ut_sc_status, ")."
	write "AP                     = ", rdlindex
	write "Fail to Pass Count     = ",$sc_$cpu_LC_ART[rdlindex].FailToPassCount
	write "Pass to Fail Count     = ",$sc_$cpu_LC_ART[rdlindex].PassToFailCount
	write "Consecutive Fail Count = ",$sc_$cpu_LC_ART[rdlindex].ConsecutiveFailCount
	write "Cumulative Fail Count  = ",$sc_$cpu_LC_ART[rdlindex].CumulativeFailCount
	write "Cumulative RTS Count   = ",$sc_$cpu_LC_ART[rdlindex].CumulativeRTSExecCount 
	ut_setrequirements LCX_1003, "F"
	ut_setrequirements LCX_3006, "F"
	ut_setrequirements LCX_4011, "F"
	break
      endif
    enddo
  else
    write "<!> Failed (1003;3006;4011) - Reset All AP Statistics command not sent properly (", ut_sc_status, ")."
    write "AP                     = ", rdlindex
    ut_setrequirements LCX_1003, "F"
    ut_setrequirements LCX_3006, "F"
    ut_setrequirements LCX_4011, "F"
  endif
endif

write ";*********************************************************************"
write ";  Step 10.0: Invalid WP and AP Number Testing"
write ";*********************************************************************"
write ";  Step 10.1 Test setting APSTATE to active with an invalid AP #"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_APNUM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
State = LC_APSTATE_ACTIVE 

/$SC_$CPU_LC_SETAPSTATE APNumber=180 NewAPState=State
ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATE_APNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.2: Test setting APSTATE to passive with an invalid AP #"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_APNUM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
State = LC_APSTATE_PASSIVE 

/$SC_$CPU_LC_SETAPSTATE APNumber=222 NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATE_APNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.3: Test setting APSTATE to disable with an invalid AP #"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATE_APNUM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1
State = LC_APSTATE_DISABLED 

/$SC_$CPU_LC_SETAPSTATE APNumber=254 NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATE_APNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.4 Test setting PERMOFF with an invalid AP#"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APOFF_APNUM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

/$SC_$CPU_LC_SETAPPERMOFF APNumber=260

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APOFF_APNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.5 Test for Sample command with invalid AP #"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSAMPLE_APNUM_ERR_EID, "ERROR", 1

;; CPU1 is the default
rawcmd = "18a6c00000070000fffc00000000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a6c00000070000fffc00000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa6c00000070000fffc00000000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSAMPLE_APNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.6 Sample on AP not used and perm off"
write ";*********************************************************************"
write ";  Step 10.6.1: Test Sample on AP not used"
write ";*********************************************************************"
local apNotUsed=0

;; Find an AP that is Perm Off
for rdlindex = 0 to LC_MAX_ACTIONPOINTS-1 do
  if ($SC_$CPU_LC_ART[rdlindex].CurrentState = LC_ACTION_NOT_USED) then
    apNotUsed = rdlIndex
    break
  endif
enddo

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSAMPLE_CURR_ERR_EID, "ERROR", 1

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=apNotUsed EndAP=apNotUsed UpdateAge=0

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSAMPLE_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.6.2: Test Sample on AP perm off "
write ";*********************************************************************"
local apPermOff=0

;; Find an AP that is Perm Off
for rdlindex = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (p@$SC_$CPU_LC_ART[rdlindex].CurrentState = "Permanently Disabled") then
    apPermOff = rdlIndex
    break
  endif
enddo

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSAMPLE_CURR_ERR_EID, "ERROR", 1

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=apPermOff EndAP=apPermOff UpdateAge=0

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSAMPLE_CURR_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.7 Test Reset WP stats with invalid WP number"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WPSTATS_WPNUM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

/$SC_$CPU_LC_RESETWPSTATS WPNumber=250

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_WPSTATS_WPNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 10.8 Test Reset AP stats with invalid AP number"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSTATS_APNUM_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

/$SC_$CPU_LC_RESETAPSTATS APNumber=250

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", LC_APSTATS_APNUM_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 11.0: Invalid command Testing"
write ";*********************************************************************"
write ";  Step 11.1: Invalid command number"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_CC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_LC_CMDEC + 1

;; CPU1 is the default
rawcmd = "18a4c000000108BC"

if ("$CPU" = "CPU2") then
  rawcmd = "19a4c000000108BC"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa4c000000108BC"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait $SC_$CPU_LC_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented."
  ut_setrequirements LCX_1002, "P"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements LCX_1002, "F"
  ut_setrequirements LCX_1004, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_CC_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 11.2: Invalid Sample Command length"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_APSAMPLE_LEN_ERR_EID, "ERROR", 1

;; CPU1 is the default
rawcmd = "18a6c00000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a6c00000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa6c00000000000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1

if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_APSAMPLE_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5


write ";*********************************************************************"
write ";  Step 11.3: Invalid HK Command length"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_HKREQ_LEN_ERR_EID, "ERROR", 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "18a5c00000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "19a5c00000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1aa5c00000000000"
endif

ut_sendrawcmd "$SC_$CPU_LC", (rawcmd)

ut_tlmwait  $SC_$CPU_find_event[1].num_found_messages, 1

if (UT_TW_Status = UT_Success) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LCX_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_HKREQ_LEN_ERR_EID, "."
  ut_setrequirements LCX_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 12.0:  Clean-up"
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
write ";  End procedure $SC_$CPU_lcx_gencmds                                  "
write ";*********************************************************************"
ENDPROC

