PROC $sc_$cpu_lcx_monitoring
;*******************************************************************************
;  Test Name:  lcx_monitoring
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;  	The purpose of this test is to verify that Limit Checker (LC) functions
;	properly when monitoring WatchPoints (WP). All WP evaluate to false so
;	no thresholds will be reached to cause ActionPoints (AP) to take action.
;	During this test the LC is set as follows:
;		LC Active with all AP disabled.  
;		LC Active with all AP active.
;		LC Active with all AP passive
;		LC Active with AP in mix of states (active, passive, disabled,
;		   permanently disabled, unused)
;		LC Passive with all AP active 
;		LC Disabled with all AP active
;
;  Requirements Tested
;    LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message
;    LCX2003	Upon receipt of a message, LCX shall compare the data in the
;		message to the table-defined value using the table-defined
;		comparison value and comparison operator for each data point
;		defined in the Watchpoint Definition Table (WDT) if the LCX
;		Application State is one of the following:
;                 a) Active
;                 b) Passive
;    LCX2003.2	LCX shall support the following comparison values:
;                 a)  =
;                 b)  !=
;                 c)  >
;                 d)  >=
;                 e)  <
;                 f)  <=
;    LCX2003.3	If the WDT comparison operator specifies that a Custom Function
;		shall be performed, LCX shall apply the custom function to the
;		data contained in the message
;    LCX2004	For each Watchpoint, the flight software shall maintain the
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
;    LCX3001	Upon receipt of a Sample Request, LCX shall process the request
;		specified actionpoints defined in the Actionpoint Definition
;		Table (ADT) if the LCX Application State is one of the
;		following:
;                 a) Active
;                 b) Passive
;    LCX3001.1	LCX shall support the following Reverse Polish Operators:
;                 a) and
;                 b) or
;                 c) xor
;                 d) not
;                 e) equals
;    LCX3002	Each table-defined Actionpoint shall be evaluated and the
;		results stored in the dump-only Actionpoint Results Table if the
;		Actionpoint state is either:
;                 a) Active
;                 b) Passive
;    LCX3003	If the Actionpoint is Disabled, LCX shall skip processing that
;		actionpoint
;    LCX3004	If the Actionpoint is Unused, LCX shall skip processing that 
;		actionpoint
;    LCX3005	If the Actionpoint is Permanently Disabled, LCX shall skip
;		processing that actionpoint
;    LCX3006	For each Actionpoint, the flight software shall maintain the
;		following statistics in the dump-only Actionpoint Results Table:
;                 a) The result of the last Sample(Pass,Fail,Error, or Stale)
;                 b) The current state (PermOff,Disabled,Active,Passive,Unused)
;                 c) The number of times this Actionpoint has crossed from the
;		     Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;		     Pass to Fail state
;                 e) The number of consecutive times the equation result =
;		     Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;    LCX4000	Upon receipt of a Set LCX Application State To Active Command,
;		LCX shall set the state of the LCX Application to Active
;    LCX4001	Upon receipt of a Set LCX Application State to Passive Command,
;		LC shall set the LCX Application State to Passive
;    LCX4002	Upon receipt of a Set LCX Application State to Disable Command,
;               LC shall set the LCX Application State to Disabled
;    LCX4003	Upon receipt of a Set Actionpoint to Active Command, LCX shall
;		set the state for the command-specified Actionpoint to ACTIVE
;		such that the actionpoint is evaluated and the table-defined
;		actions are taken based on the evaluation
;    LCX4004	Upon receipt of a Set All Actionpoints to Active Command, LCX
;		shall set the state for all Actionpoints to ACTIVE such that the
;		actionpoints are evaluated and the table-defined actions are
;		taken based on the evaluation
;    LCX4005	Upon receipt of a Set Actionpoint to Passive Command, LCX shall
;		set the state for the command-specified Actionpoint to PASSIVE
;		such that the actionpoint is evaluated, however, no actions
;               are taken
;    LCX4006	Upon receipt of a Set All Actionpoints to Passive Command, LCX
;		shall set the state for the all Actionpoints to PASSIVE such
;		that all actionpoints are evaluated, however, no actions are
;		taken
;    LCX4007	Upon receipt of a Set Actionpoint to Disabled Command, LCX shall
;		set the state for the command-specified Actionpoint to DISABLED
;		such that the actionpoints are not evaluated and no actions
;               are taken
;    LC4008	Upon receipt of a Set All Actionpoints to Disabled Command, LCX
;		shall set the state for all Actionpoint to DISABLED such that:
;                 a) the actionpoints are not evaluated 
;                 b) no actions are taken
;                 c) no event messages generated.
;    LCX4009	Upon receipt of a Set Actionpoint to Permanent Disable, LCX 
;		shall mark the command-specified Actionpoint such that the 
;		Actionpoint cannot be Activated
;    LCX8000	LCX shall generate a housekeeping message containing the
;		following:
;                 a) Valid  Command Counter
;                 b) Command Rejected Counter
;                 c) Number of Start RTS commands NOT sent to SC task because
;		     LCX Application is PASSIVE
;                 d) Current LC Application State (LCX_ACTIVE, LCX_PASSIVE,
;		     LCX_DISABLED)...
;                 e) Total count of actionpoints sampled while LCX_ACTIVE or
;		     LCX_PASSIVE...
;                 f) Total count of packets monitored for watchpoints (cmd and
;		     telemetry)
;                 g) Total count of commands sent to SC task to start an RTS
;                 h) Selected data from watchpoint results table
;                 i) Selected data from actionpoint results table
;    LCX9000	Upon cFE Power-On LCX shall initialize the following
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
;    LCX9001	Upon cFE Power-On LCX shall initialize the following Watchpoint
;		data to Zero (or value specified) for all Watchpoints:
;                 a) The result of the last watchpoint relational comparison to
;		     STALE
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;		     False to True result
;                 d) The number of consecutive times the comparison has yielded
;		     a True result
;                 e) The cumulative number of times the comparison has yielded
;		     a True result
;                 f) The value that caused the last False-to-True crossing, and
;		     the crossing time stamp
;                 g) The value that caused the last True-to-False crossing, and
;		     the crossing time stamp
;    LCX9002	Upon cFE Power-On LCX shall initialize the following Actionpoint
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
;    LCX9005    Upon any initialization, LCX shall validate the Watchpoint
;		Definition Table for the following:
;                 a) valid operator
;                 b) data size
;                 c) Message ID
;    LCX9006	Upon any initialization, LCX shall validate the Actionpoint
;		Definition Table for the following:
;                 a) valid default state
;                 b) RTS number (in range)
;                 c) Event Type (DEBUG, INFO, ERROR, CRITICAL)
;                 d) Failure Count (in range)
;                 e) Action Equation syntax
;    LCX9007	Upon any initialization, LCX shall subscribe to the messages
;		defined in the WDT.
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
;       ut_setupevents       Performs setup to verify that a particular event
;                         message was received by ASIST.
;	ut_setrequirements    A directive to set the status of the cFE
;			      requirements array.
;       ftp_file          Procedure to load file containing a table
;       lcx_wdt1          Sets up the Watchpoint Definition table files
;       lcx_adt1b     	  Sets up the Actionpoint Definition table files
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
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"

%liv (log_procedure) = logging

#define LCX_1003       0
#define LCX_2003       1
#define LCX_20032      2
#define LCX_20033      3
#define LCX_2004       4
#define LCX_3001       5
#define LCX_30011      6
#define LCX_3002       7
#define LCX_3003       8
#define LCX_3004       9
#define LCX_3005      10
#define LCX_3006      11
#define LCX_4000      12
#define LCX_4001      13
#define LCX_4002      14
#define LCX_4003      15
#define LCX_4004      16
#define LCX_4005      17
#define LCX_4006      18
#define LCX_4007      19
#define LCX_4008      20
#define LCX_4009      21
#define LCX_8000      22
#define LCX_9000      23
#define LCX_9001      24
#define LCX_9002      25
#define LCX_9005      26
#define LCX_9006      27

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

#define CMDFAIL       1
#define CMDSUCCESS    2
global ut_req_array_size = 27
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_2003", ;;
    "LCX_2003.2","LCX_2003.3","LCX_2004","LCX_3001","LCX_3001.1","LCX_3002", ;;
    "LCX_3003","LCX_3004","LCX_3005","LCX_3006","LCX_4000","LCX_4001", ;;
    "LCX_4002","LCX_4003","LCX_4004","LCX_4005","LCX_4006","LCX_4007", ;;
    "LCX_4008","LCX_4009","LCX_8000","LCX_9000","LCX_9001","LCX_9002", ;;
    "LCX_9005","LCX_9006"] 

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADTentries = 12
LOCAL APResultEntries = ADTentries/2
LOCAL WDTentries = 30
LOCAL CmdStatus 
LOCAL State
Local rdlindex
Local maxwp = LC_MAX_WATCHPOINTS - 1
Local maxap = LC_MAX_ACTIONPOINTS - 1
Local MsgId[20]
Local Size
Local Pattern[32]
local LCAppName = LC_APP_NAME
local ramDir = "RAM:0"
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
write ";  Step 1.2: Creating the WDT and ADT used for testing and upload it"
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

s ftp_file("CF:0/apps", "lc_def_wdt1.tbl", wdtFileName, "$CPU", "P")

;; Generate the Actionpoint Definition Table
s $SC_$CPU_lcx_adt1b

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file("CF:0/apps", "lc_def_adt1b.tbl", adtFileName, "$CPU", "P")

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
write ";  Step 1.5: Verify that the LC Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Add the HK message receipt test
local hkPktId, artPktId, wrtPktId

;; Set the SC HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0A7"
artPktId = "0FB8"
wrtPktId = "0FB9"

if ("$CPU" = "CPU2") then
  hkPktId = "p1A7"
  artPktId = "0FD6"
  wrtPktId = "0FD7"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2A7"
  artPktId = "0FF6"
  wrtPktId = "0FF7"
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
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being received."
  ut_setrequirements LCX_8000, "P"
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
  for apindex = 1 to APResultEntries do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
      break                                   
    endif
  enddo
;;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = APResultEntries+1 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
      break                                   
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000;9005;9006) - Housekeeping telemetry NOT initialized at startup."
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
    ut_setrequirements LCX_9005, "F"
    ut_setrequirements LCX_9006, "F"
  else
    write "<*> Passed (9000;9005;9006) - Housekeeping telemetry initialized properly."
    ut_setrequirements LCX_9000, "P"
    ut_setrequirements LCX_9005, "P"
    ut_setrequirements LCX_9006, "P"
  endif  
else
  write "<!> Failed (9000;9005;9006) - Housekeeping telemetry NOT initialized at startup."
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
  ut_setrequirements LCX_9005, "F"
  ut_setrequirements LCX_9006, "F"
endif

;; Dump the WRT
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

;check initialization of WRT
for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
     ($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;     ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
    break
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (9001) - Watchpoint Results Table NOT initialized at startup."
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

;; Dump the ART
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

;check initialization of ART
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
  write "<!> Failed (9002) - Actionpoint Results Table NOT initialized at startup."
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
write ";  Step 1.6: Setup Data Packet message ids "
write ";*********************************************************************"
;; For CPU1 use CPU2 Message IDs
MsgId[1] = 0x987
MsgId[2] = 0x988
MsgId[3] = 0x989
MsgId[4] = 0x98a
MsgId[5] = 0x98b
MsgId[6] = 0x98c
MsgId[7] = 0x98d
MsgId[8] = 0x98e
MsgId[9] = 0x98f
MsgId[10] = 0x990
MsgId[11] = 0x991
MsgId[12] = 0x992
MsgId[13] = 0x993
MsgId[14] = 0x994
MsgId[15] = 0x995
MsgId[16] = 0x996
MsgId[17] = 0x997
MsgId[18] = 0x998
MsgId[19] = 0x999
MsgId[20] = 0x99a

if ("$CPU" = "CPU2") then 
  ;; Use CPU3 Message IDs
  MsgId[1] = 0xa87
  MsgId[2] = 0xa88
  MsgId[3] = 0xa89
  MsgId[4] = 0xa8a
  MsgId[5] = 0xa8b
  MsgId[6] = 0xa8c
  MsgId[7] = 0xa8d
  MsgId[8] = 0xa8e
  MsgId[9] = 0xa8f
  MsgId[10] = 0xa90
  MsgId[11] = 0xa91
  MsgId[12] = 0xa92
  MsgId[13] = 0xa93
  MsgId[14] = 0xa94
  MsgId[15] = 0xa95
  MsgId[16] = 0xa96
  MsgId[17] = 0xa97
  MsgId[18] = 0xa98
  MsgId[19] = 0xa99
  MsgId[20] = 0xa9a
elseif ("$CPU" = "CPU3") then 
  ;; Use CPU1 Message IDs
  MsgId[1] = 0x887
  MsgId[2] = 0x888
  MsgId[3] = 0x889
  MsgId[4] = 0x88a
  MsgId[5] = 0x88b
  MsgId[6] = 0x88c
  MsgId[7] = 0x88d
  MsgId[8] = 0x88e
  MsgId[9] = 0x88f
  MsgId[10] = 0x890
  MsgId[11] = 0x891
  MsgId[12] = 0x892
  MsgId[13] = 0x893
  MsgId[14] = 0x894
  MsgId[15] = 0x895
  MsgId[16] = 0x896
  MsgId[17] = 0x897
  MsgId[18] = 0x898
  MsgId[19] = 0x899
  MsgId[20] = 0x89a
endif

write ";*********************************************************************"
write ";  Step 2.0:  Test LC Passive, All Ap Active, no thresholds are reached"
write ";*********************************************************************"
write ";  Step 2.1:  Send a Set LC Application State to Passive Command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_PASSIVE

ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) then
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
write ";  Step 2.2:  Send a Set All APs to Active Command"
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
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
  write "<!> Failed (1003;4004) - Set All APs to Active command not sent properly (", ut_sc_status, ")."
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3 Send packets for all WP defined in WDT"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendmonpackets("2.3")

write ";*********************************************************************"
write ";  Step 2.4 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since LC is passive (and all WP evaluate to False anyway)"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 10

write ";*********************************************************************"
write ";  Step 2.5 Check housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 12) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 20) AND ($SC_$CPU_LC_RTSCNT = 0) THEN
  ;; for the WP measured results for wpindex 1-7 should be 0 (false), wpindex 8 which is 240 (0xF0), rest 255 (0xFF)
  for wpindex = 1 to WPACKED do
    if (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 240) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
        break
      endif
    endif 
  enddo

  ;; for the 12 APs that are being used, 
  ;; should be 68 (0x44) since they are active and pass
  ;; rest should be 51 (0x33) since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= APResultEntries) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 68) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC        = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC        = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT   = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE   = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT  = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT    = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT       = ", $SC_$CPU_LC_RTSCNT 
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6 Dump WRT and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) - WRT show WPs not evaluated properly."
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
     ut_setrequirements LCX_20033, "F"
     ut_setrequirements LCX_2004, "F"

else
     write "<*> Passed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) -  WRT shows WPs evaluated properly."
     ut_setrequirements LCX_2003, "P"
     ut_setrequirements LCX_20032, "P"
     ut_setrequirements LCX_20033, "P"
     ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 2.7 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
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
  write "<!> Failed (3001;3001.1;3003;3004;3006) - ART show APs not evaluated properly."
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
  ut_setrequirements LCX_3004, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3004, "P"
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 3.0:  Test LC Disabled, All Ap Active, no thresholds are reached"
write ";*********************************************************************"
write ";  Step 3.1:  Send a Set LC Application State to Disabled Command"
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
write ";  Step 3.2:  Send a Set All APs to Active Command"
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
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
  write "<!> Failed (1003;4004) - Set All APs to Active command not sent properly (", ut_sc_status, ")."
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3 Send packets for all WP defined in WDT"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendmonpackets("3.3")

write ";*********************************************************************"
write ";  Step 3.4 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since LC is Disabled (and all WP evaluate to False anyway)"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 3.5 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_DISABLED) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 12) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 20) AND ($SC_$CPU_LC_RTSCNT = 0) THEN
;; for the WP measured results for wpindex 1-7 should be 0 (false), wpindex 8 which is 240 (0xF0), rest 255 (0xFF)
  for wpindex = 1 to WPACKED do
    if (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 240) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
        break
      endif
    endif 
  enddo

;; for the 10 APs that are being used, should be 68 (x44) since they are active and last results were pass
;; rest should be 51 (0x33) since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= APResultEntries) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 68) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break                                   
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC       = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC       = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE  = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT   = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT      = ", $SC_$CPU_LC_RTSCNT 
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.6 Dump WRT and check counters, nothing should change"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) - WRT show WPs not evaluated properly."
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
  ut_setrequirements LCX_20033, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_20033, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 3.7 Dump ART and check counters, nothing should change"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
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
  write "<!> Failed (3001;3001.1;3002;3004;3006) - ART show APs not evaluated properly."
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
  ut_setrequirements LCX_3004, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3002;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3004, "P"
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 4.0:  Test LC Active, All AP Disabled, no thresholds are reached"
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

wait 5

write ";*********************************************************************"
write ";  Step 4.2:  Send a Set All APs to Disabled Command"
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
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
  write "<!> Failed (1003;4008) - Set All APs to Disabled command not sent properly (", ut_sc_status, ")."
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3 Send packets for all WP defined in WDT"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendmonpackets("4.3")

write ";*********************************************************************"
write ";  Step 4.4 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since LC is passive (and all WP evaluate to False anyway)"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 4.5 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 12) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 40) AND ($SC_$CPU_LC_RTSCNT = 0) THEN
;; for the WP measured results for wpindex 1-7 should be 0 (false), wpindex 8 which is 240 (0xF0), rest 255 (0xFF)
  for wpindex = 1 to WPACKED do
    if (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 240) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
        break
      endif
    endif 
  enddo
;; for the 10 APs that are being used, should be 204 (xCC) since they are disabled and last result was pass
;; rest should be 51 (0x33) since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= APResultEntries) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 204) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC       = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC       = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE  = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT   = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT      = ", $SC_$CPU_LC_RTSCNT 
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 4.6 Dump WRT and check counters, message monitored should be 2"
write ";  for each entry. "
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
        ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) - WRT show WPs not evaluated properly."
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
  ut_setrequirements LCX_20033, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_20033, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 4.7 Dump ART and check counters, state should be disabled"
write ";   other counters should remain the same as before "
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
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
  write "<!> Failed (3001;3001.1;3002;3003;3004;3006) - ART show APs not evaluated properly."
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
  ut_setrequirements LCX_3003, "F"
  ut_setrequirements LCX_3004, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3003, "P"
  ut_setrequirements LCX_3004, "P"
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 5.0:  Test LC Active, All AP Active, no thresholds are reached"
write ";*********************************************************************"
write ";  Step 5.1:  Send a Set All APs to Active Command"
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
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
  write "<!> Failed (1003;4004) - Set All APs to Active command not sent properly (", ut_sc_status, ")."
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3 Send packets for all WP defined in WDT"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendmonpackets("5.3")

write ";*********************************************************************"
write ";  Step 5.4 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since LC is passive (and all WP evaluate to False anyway)"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 5.5 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 24) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 60) AND ($SC_$CPU_LC_RTSCNT = 0) THEN
;; for the WP measured results for wpindex 1-7 should be 0 (false), wpindex 8 which is 240 (0xF0), rest 255 (0xFF)
  for wpindex = 1 to WPACKED do
    if (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 240) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
        break
      endif
    endif 
  enddo
;; for the 10 APs that are being used, should be 68 (x44) since they are active and pass
;; rest should be 51 (0x33) since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= APResultEntries) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 68) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC       = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC       = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE  = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT   = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT      = ", $SC_$CPU_LC_RTSCNT 
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 5.6 Dump WRT and check counters, message monitored should be 3 "
write ";  for all entries. "
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
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
  ut_setrequirements LCX_20033, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_20033, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 5.7 Dump ART and check counters"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
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
  write "<!> Failed (3001;3001.1;3002;3004;3006) - ART show APs not evaluated properly."
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
  ut_setrequirements LCX_3004, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3002;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3004, "P"
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 6.0:  Test LC Active, All AP Passive, no thresholds are reached"
write ";*********************************************************************"
write ";  Step 6.1:  Send a Set All APs to Passive Command"
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

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
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
  write "<!> Failed (1003;4006) - Set All APs to Passive command not sent properly (", ut_sc_status, ")."
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2 Send packets for all WP defined in WDT"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendmonpackets("6.2")

write ";*********************************************************************"
write ";  Step 6.3 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since LC is passive (and all WP evaluate to False anyway)"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 6.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 36) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 80) AND ($SC_$CPU_LC_RTSCNT = 0) THEN
;; for the WP measured results for wpindex 1-7 should be 0 (false), wpindex 8 which is 240 (0xF0), rest 255 (0xFF)
  for wpindex = 1 to WPACKED do
    if (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 240) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
        break
      endif
    endif 
  enddo
;; for the 10 APs that are being used, should be 136 (x88) since they are passive and pass
;; rest should be 51 (0x33) since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= APResultEntries) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 136) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC       = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC       = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE  = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT   = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT      = ", $SC_$CPU_LC_RTSCNT 
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 6.5 Dump WRT and check counters, message monitored should be 4 "
write ";  for all"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
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
  ut_setrequirements LCX_20033, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_20033, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 6.6 Dump ART and check counters, state is passive"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
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
  write "<!> Failed (3001;3001.1;3002;3004;3006) - ART show APs not evaluated properly."
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
  ut_setrequirements LCX_3004, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3002;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3004, "P"
  ut_setrequirements LCX_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 7.0: Test LC Active, APs a mix of states, no thresholds reached"
write ";*********************************************************************"
write ";  Step 7.1:  Send commands to set APs to mix of states"
write ";*********************************************************************"
State = LC_APSTATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=4 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=5 NewAPState=State"

State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=1 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=8 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=9 NewAPState=State"

State = LC_APSTATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=6 NewAPState=State"
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=7 NewAPState=State"

ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=3 NewAPState=State"
/$SC_$CPU_LC_SETAPPERMOFF APNumber=3

CmdStatus = CMDSUCCESS

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5

index = 0
if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) then
  CmdStatus = CMDFAIL
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PERMOFF) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) then
    CmdStatus = CMDFAIL
  endif
endif
if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  for index = ADTentries to LC_MAX_ACTIONPOINTS-1 do
    if ($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) then
      CmdStatus = CMDFAIL
      break
    endif
  enddo
endif

if (CmdStatus = CMDSUCCESS) then
  write "<*> Passed (1003;4003;4005;4007;4009) - APs set to mix of states correctly."
  ut_setrequirements LCX_1003, "P"
  ut_setrequirements LCX_4003, "P"
  ut_setrequirements LCX_4005, "P"
  ut_setrequirements LCX_4007, "P"
  ut_setrequirements LCX_4009, "P"
else
  write "<!> Failed (1003;4003;4005;4007;4009) - APs not set to mix of states correctly."
  write "<*> AP State = ", $SC_$CPU_LC_ART[index].CurrentState 
  ut_setrequirements LCX_1003, "F"
  ut_setrequirements LCX_4003, "F"
  ut_setrequirements LCX_4005, "F"
  ut_setrequirements LCX_4007, "F"
  ut_setrequirements LCX_4009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.2 Send packets for all WP defined in WDT"
write ";*********************************************************************"

s $sc_$cpu_lcx_sendmonpackets("7.2")

write ";*********************************************************************"
write ";  Step 7.3 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since LC is passive (and all WP evaluate to False anyway)"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 7.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 44) AND ;;
   ($SC_$CPU_LC_MONMSGCNT = 100) AND ($SC_$CPU_LC_RTSCNT = 0) THEN
;; for the WP measured results for wpindex 1-7 should be 0 (false), wpindex 8 which is 240 (0xF0), rest 255 (0xFF)
  for wpindex = 1 to WPACKED do
    if (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 240) then
        break
      endif
    elseif (wpindex > 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
        break
      endif
    endif 
  enddo
;; for the 10 APs that are being used, 132 (x84), 12 (x0C), 68 (x44),
;; 204 (xCC), and 136 (x88)  
;; rest should be 51 (0x33) since they are not used and not measured
  CmdStatus = CMDSUCCESS
  apindex = 1
  if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 132) then
    CmdStatus = CMDFAIL                                  
  endif

  if (CMDStatus = CMDSUCCESS) then
    apindex = apindex + 1
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 12) then
      CmdStatus = CMDFAIL                                  
    endif
  endif

  if (CMDStatus = CMDSUCCESS) then
    apindex = apindex + 1
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 68) then
      CmdStatus = CMDFAIL                                  
    endif
  endif

  if (CMDStatus = CMDSUCCESS) then
    apindex = apindex + 1
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 204) then
      CmdStatus = CMDFAIL                                  
    endif
  endif

  if (CMDStatus = CMDSUCCESS) then
    apindex = apindex + 1
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 136) then
      CmdStatus = CMDFAIL                                  
    endif
  endif

  if (CmdStatus = CMDSUCCESS) then
    for apindex = 7 to APACKED do
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
        break                                   
      endif
    enddo
  endif
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LCX_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LCX_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC       = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC       = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE  = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT   = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT      = ", $SC_$CPU_LC_RTSCNT 
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LCX_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.5 Dump WRT and check counters, message monitored should be 5 for all"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", wrtPktId)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 5) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) - WRT show WPs not evaluated properly."
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
  ut_setrequirements LCX_20033, "F"
  ut_setrequirements LCX_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2004;3001;3001.1;3004;3006) -  WRT shows WPs evaluated properly."
  ut_setrequirements LCX_2003, "P"
  ut_setrequirements LCX_20032, "P"
  ut_setrequirements LCX_20033, "P"
  ut_setrequirements LCX_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 7.6 Dump ART and check counters, states are mixed"
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", artPktId)
wait 5 

;checking that States did not change
CmdStatus = CMDSUCCESS
index = 0
if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
   ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
   ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
   ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
   ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
   ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
   ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
  CmdStatus = CMDFAIL
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PERMOFF) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;; 
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  index = index + 1
  if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
     ($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
     ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
     ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
    CmdStatus = CMDFAIL
  endif
endif

if (CmdStatus = CMDSUCCESS) then
  for index = ADTentries to LC_MAX_ACTIONPOINTS-1 do
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or  ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  enddo
endif

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3002;3003;3004;3005;3006) - ART show APs not evaluated properly."
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
  ut_setrequirements LCX_3003, "F"
  ut_setrequirements LCX_3004, "F"
  ut_setrequirements LCX_3005, "F"
  ut_setrequirements LCX_3006, "F"
else
  write "<*> Passed (3001;3001.1;3002;3003;3004;3005;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LCX_3001, "P"
  ut_setrequirements LCX_30011, "P"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3003, "P"
  ut_setrequirements LCX_3004, "P"
  ut_setrequirements LCX_3005, "P"
  ut_setrequirements LCX_3006, "P"
endif

step8_0:
write ";*********************************************************************"
write ";  Step 8.0:  Clean-up"
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
write ";  End procedure $SC_$CPU_lcx_monitoring                              "
write ";*********************************************************************"
ENDPROC
