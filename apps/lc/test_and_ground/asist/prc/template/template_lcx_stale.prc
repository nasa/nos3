PROC $sc_$cpu_lcx_stale
;*******************************************************************************
;  Test Name:  lcx_stale
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker eXtended (LCX) 
;	Application properly handles "stale" data. Stale data is data that has
;	not been received for x Sample commands as specified in the WatchPoint
;	Definition. 
;
;  Requirements Tested
;    LCX1003	If LCX accepts any command as valid, LCX shall execute the
;		command, increment the LCX Valid Command Counter and issue an
;		event message
;    LCX2001	For each Watchpoint specified in the Watchpoint Definition Table
;		(WDT), LCX shall specify an age value when the data becomes
;		"stale".
;    LCX2002	For each Watchpoint specified in the Watchpoint Definition Table
;		(WDT), LCX shall maintain the age of the data.
;    LCX2004	For each Watchpoint, the flight software shall maintain the
;		following statistics in the dump-only Watchpoint Results Table:
;		a) The result of the last relational comparison (False, True,
;		   Error, or Stale)
;		b) The number of times this Watchpoint has been compared
;		c) The number of times this Watchpoint has crossed from the
;		   False to True result
;		d) The number of consecutive times the comparison has yielded a
;		   True result
;		e) The cumulative number of times the comparison has yeilded a
;		   True result
;		f) Most recent FALSE to TRUE transition value
;		g) Most recent FALSE to TRUE transition timestamp
;		h) Most recent TRUE to FALSE transition value
;		i) Most recent TRUE to FALSE transition timestamp
;		j) Most recent comparison age
;    LCX3001	Upon receipt of a Sample Request, LCX shall process the request
;		specified actionpoints defined in the Actionpoint Definition
;		Table (ADT) if the LC Application State is one of the following:
;                 a) Active
;                 b) Passive
;    LCX3002	Each table-defined Actionpoint shall be evaluated and the 
;		results stored in the dump-only Actionpoint Results Table if the
;		Actionpoint state is either:
;                 a) Active
;                 b) Passive
;    LCX3006	For each Actionpoint, the flight software shall maintain the
;		following statistics in the dump-only Actionpoint Results Table:
;                 a) The result of the last Sample (Pass,Fail,Error,Stale)
;                 b) The current state (PermOff,Disabled,Active,Passive,Unused)
;                 c) The number of times this Actionpoint has crossed from the
;		     Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;		     Pass to Fail state
;                 e) The number of consecutive times the equation result =Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;    LCX4000	Upon receipt of a Set LCX Application State To Active Command,
;		LCX shall set the state of the LCX Application to Active
;    LCX8000	LCX shall generate a housekeeping message containing the
;		following:
;                 a) Valid Command Counter
;                 b) Command Rejected Counter
;                 c) Number of Start RTS commands NOT sent to SC task because
;		     LCX Application is PASSIVE
;                 d) Current LC Application State (LCX_ACTIVE, LCX_PASSIVE,
;		     LCX_DISABLED)
;                 e) Total count of actionpoints sampled while LCX_ACTIVE or
;		     LCX_PASSIVE
;                 f) Total count of packets monitored for watchpoints (cmd and
;		     tlm)
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
;                 e) The cumulative number of times the comparison has yielded a
;		     True result
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
;	08/10/12	Walt Moleski	Initial implementation.
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
;       lcx_wdt5      Sets up the Watchpoint Definition table file for testing
;       lcx_adt6      Sets up the Actionpoint Definition table file for testing
;
;  Expected Test Results and Analysis
;  The tables below depict the expected Actionpoint and Watchpoint evaluations.
;  After the first Sample request:
;  AP	Result		WP	Result	Age
;   0   Pass		 0	True	 2
;   1   Pass		 4	False	 2
;   2	Pass		 8	True	 2
;   3	Fail		12	False	 2
;   4	Pass		22	False	 2
;   5   Fail		23	False	 2
;   6	Pass		30	Error	 0
;   7	Error
;
;  After the WPs go "stale":
;  AP	Result		WP	Result	Age
;   0   Stale 		 0	Stale	 0
;   1   Pass		 4	Stale	 0
;   2	Stale		 8	Stale	 0
;   3	Fail		12	Stale	 0
;   4	Stale		22	Stale	 0
;   5   Stale		23	Stale	 0
;   6	Error		30	Error	 0
;   7	Error
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

#define LCX_1003      0
#define LCX_2001      1
#define LCX_2002      2
#define LCX_2004      3
#define LCX_3001      4
#define LCX_3002      5
#define LCX_3006      6
#define LCX_4000      7
#define LCX_8000      8
#define LCX_9000      9      
#define LCX_9001      10
#define LCX_9002      11

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

#define CMDFAIL       1
#define CMDSUCCESS    2

global ut_req_array_size = 11
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LCX_1003","LCX_2001", ;;
    "LCX_2002","LCX_2004","LCX_3001","LCX_3002","LCX_3006","LCX_4000", ;;
    "LCX_8000","LCX_9000","LCX_9001","LCX_9002"] 

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL stream1
LOCAL WRTAppid
LOCAL ARTAppid
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADTentries = 8
LOCAL WDTentries = 31
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
s $SC_$CPU_lcx_wdt5

;; Parse the filename configuration parameters for the default table filenames
local wdtFileName = LC_WDT_FILENAME
local slashLoc = %locate(wdtFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  wdtFileName = %substring(wdtFileName,slashLoc+1,%length(wdtFileName))
  slashLoc = %locate(wdtFileName,"/")
enddo

write "==> Default LC Watchpoint Table filename = '",wdtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_wdt5.tbl", wdtFileName, "$CPU", "P")

s $SC_$CPU_lcx_adt6

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_adt6.tbl", adtFileName, "$CPU", "P")

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
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
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
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
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
;; 255 because the WPs are all "stale" to start
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
      break
    endif
  enddo
;first check the APs that are being used
;;255 is because they are disabled and not measured
  for apindex = 1 to ADTentries do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
      break                                   
    endif
  enddo
;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = ADTentries+1 to APACKED do
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
write ";  Step 1.6: Verify that the WDT specifies the age for some entries."
write ";*********************************************************************"
local ageSpecified = FALSE

for index = 0 to WDTentries do
  if ($SC_$CPU_LC_WDT[index].StaleAge > 0) then
    ageSpecified = TRUE
    break
  endif
enddo

;; Check if at least 1 entry specified an Age
if (ageSpecified = TRUE) then
  write "<*> Passed (2001) - Watchpoint Definition Table specified an Age."
  ut_setrequirements LCX_2001, "P"  
else
  write "<!> Failed (2001) - Watchpoint Definition Table does not contain an entry with a stale age."
  ut_setrequirements LCX_2001, "F"  
  write "<!> Terminating test...."
  goto step3
endif

write ";*********************************************************************"
write ";  Step 2.0:  Send packets to make APs transition. "
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
write ";  Step 2.3 Send Sample Request for all APs."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=7 UpdateAge=1
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (3001) - Received the expected Sample event"
  ut_setrequirements LCX_3001, "P"
else
  write "<!> Failed (3001) - Did not rcv expected Sample event; count = ",$SC_$CPU_find_event[1].num_found_messages
  ut_setrequirements LCX_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4 Dump ART and WRT"   
write ";*********************************************************************"

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", WRTAppid)
wait 5

;; Write out AP info
for index = 0 to ADTentries - 1 do
  write "  Action Point ",index,":"
  write "  Action Result          = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Passive AP Count       = ", $SC_$CPU_LC_ART[index].PassiveAPCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  write " "
enddo

;; Check the APs for the correct evaluation
if ($SC_$CPU_LC_ART[0].ActionResult = LC_ACTION_PASS) AND ;;
   ($SC_$CPU_LC_ART[1].ActionResult = LC_ACTION_FAIL) AND ;;
   ($SC_$CPU_LC_ART[2].ActionResult = LC_ACTION_FAIL) AND ;;
   ($SC_$CPU_LC_ART[3].ActionResult = LC_ACTION_FAIL) AND ;;
   ($SC_$CPU_LC_ART[4].ActionResult = LC_ACTION_FAIL) AND ;;
   ($SC_$CPU_LC_ART[5].ActionResult = LC_ACTION_PASS) AND ;;
   ($SC_$CPU_LC_ART[6].ActionResult = LC_ACTION_PASS) AND ;;
   ($SC_$CPU_LC_ART[7].ActionResult = LC_ACTION_ERROR) then
  write "<*> Passed (3002) - All APs evaluated correctly"
  ut_setrequirements LCX_3002, "P"
else
  write "<!> Failed (3002) - At least one AP did not evaluate to the expected result"
  ut_setrequirements LCX_3002, "F"
endif

write "  Watch Point 0:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[0].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[0].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[0].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[0].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[0].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[0].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[0].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[0].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[0].WatchResults = LC_WATCH_TRUE) AND ;;
   ($SC_$CPU_LC_WRT[0].CountdownToStale = 2) then
  write "<*> Passed (2002;2004) - WRT entry 0 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 0 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write " "
write "  Watch Point 4:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[4].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[4].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[4].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[4].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[4].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[4].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[4].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[4].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[4].WatchResults = LC_WATCH_TRUE) AND ;;
   ($SC_$CPU_LC_WRT[4].CountdownToStale = 2) then
  write "<*> Passed (2002;2004) - WRT entry 4 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 4 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write " "
write "  Watch Point 8:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[8].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[8].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[8].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[8].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[8].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[8].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[8].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[8].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[8].WatchResults = LC_WATCH_FALSE) AND ;;
   ($SC_$CPU_LC_WRT[8].CountdownToStale = 2) then
  write "<*> Passed (2002;2004) - WRT entry 8 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 8 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write " "
write "  Watch Point 12:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[12].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[12].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[12].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[12].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[12].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[12].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[12].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[12].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[12].WatchResults = LC_WATCH_FALSE) AND ;;
   ($SC_$CPU_LC_WRT[12].CountdownToStale = 2) then
  write "<*> Passed (2002;2004) - WRT entry 12 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 12 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write " "
write "  Watch Point 22:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[22].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[22].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[22].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[22].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[22].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[22].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[22].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[22].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[22].WatchResults = LC_WATCH_FALSE) AND ;;
   ($SC_$CPU_LC_WRT[22].CountdownToStale = 2) then
  write "<*> Passed (2002;2004) - WRT entry 22 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 22 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write " "
write "  Watch Point 23:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[23].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[23].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[23].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[23].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[23].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[23].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[23].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[23].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[23].WatchResults = LC_WATCH_FALSE) AND ;;
   ($SC_$CPU_LC_WRT[23].CountdownToStale = 2) then
  write "<*> Passed (2002;2004) - WRT entry 23 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 23 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write " "
write "  Watch Point 30:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[30].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[30].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[30].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[30].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[30].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[30].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[30].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[30].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[30].WatchResults = LC_WATCH_ERROR) AND ;;
   ($SC_$CPU_LC_WRT[30].CountdownToStale = 0) then
  write "<*> Passed (2002;2004) - WRT entry 30 specifies correct statistics."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - WRT entry 30 does not contain the expected statistics."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.5 Send Sample Requests until Watch Points go stale. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1

for i = 1 to 3 DO
  /$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=7 UpdateAge=1
  wait 2
enddo

write ";*********************************************************************"
write ";  Step 2.6 Dump ART and WRT"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", WRTAppid)
wait 5

;; Write out AP info
for index = 0 to ADTentries - 1 do
  write "  Action Point ",index,":"
  write "  Action Result          = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Passive AP Count       = ", $SC_$CPU_LC_ART[index].PassiveAPCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  write " "
enddo

;; Check the APs for the correct evaluation
if ($SC_$CPU_LC_ART[0].ActionResult = LC_ACTION_PASS) AND ;;
   ($SC_$CPU_LC_ART[1].ActionResult = LC_ACTION_STALE) AND ;;
   ($SC_$CPU_LC_ART[2].ActionResult = LC_ACTION_FAIL) AND ;;
   ($SC_$CPU_LC_ART[3].ActionResult = LC_ACTION_FAIL) AND ;;
   ($SC_$CPU_LC_ART[4].ActionResult = LC_ACTION_STALE) AND ;;
   ($SC_$CPU_LC_ART[5].ActionResult = LC_ACTION_STALE) AND ;;
   ($SC_$CPU_LC_ART[6].ActionResult = LC_ACTION_ERROR) AND ;;
   ($SC_$CPU_LC_ART[7].ActionResult = LC_ACTION_ERROR) then
  write "<*> Passed (3002;3006) - All APs evaluated correctly"
  ut_setrequirements LCX_3002, "P"
  ut_setrequirements LCX_3006, "P"
else
  write "<!> Failed (3002;3006) - At least one AP did not evaluate to the expected result"
  ut_setrequirements LCX_3002, "F"
  ut_setrequirements LCX_3006, "F"
endif

write "  Watch Point 0:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[0].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[0].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[0].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[0].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[0].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[0].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[0].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[0].CountdownToStale

write " "
write "  Watch Point 4:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[4].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[4].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[4].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[4].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[4].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[4].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[4].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[4].CountdownToStale
write " "
write "  Watch Point 8:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[8].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[8].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[8].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[8].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[8].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[8].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[8].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[8].CountdownToStale
write " "
write "  Watch Point 12:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[12].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[12].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[12].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[12].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[12].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[12].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[12].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[12].CountdownToStale
write " "
write "  Watch Point 22:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[22].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[22].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[22].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[22].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[22].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[22].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[22].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[22].CountdownToStale
write " "
write "  Watch Point 23:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[23].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[23].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[23].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[23].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[23].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[23].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[23].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[23].CountdownToStale

;; Check the Watchpoints to determine if the Statistics are correct
if ($SC_$CPU_LC_WRT[0].WatchResults = LC_WATCH_STALE) AND ;;
   ($SC_$CPU_LC_WRT[4].WatchResults = LC_WATCH_STALE) AND ;;
   ($SC_$CPU_LC_WRT[8].WatchResults = LC_WATCH_STALE) AND ;;
   ($SC_$CPU_LC_WRT[12].WatchResults = LC_WATCH_STALE) AND ;;
   ($SC_$CPU_LC_WRT[22].WatchResults = LC_WATCH_STALE) AND ;;
   ($SC_$CPU_LC_WRT[23].WatchResults = LC_WATCH_STALE) AND ;;
   ($SC_$CPU_LC_WRT[30].WatchResults = LC_WATCH_ERROR) then
  write "<*> Passed (2002;2004) - WRT specifies the correct results."
  ut_setrequirements LCX_2002, "P"
  ut_setrequirements LCX_2004, "P"
else
  write "<!> Failed (2002;2004) - At least one WRT entry does not contain the expected result."
  ut_setrequirements LCX_2002, "F"
  ut_setrequirements LCX_2004, "F"
endif

;; Send the Sample Request one last time
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=7 UpdateAge=7
wait 5

write ";*********************************************************************"
write ";  Step 2.7 Dump ART and WRT"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", "$CPU", ARTAppid)
wait 5 

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", "$CPU", WRTAppid)
wait 5

;; Write out AP info
for index = 0 to ADTentries - 1 do
  write "  Action Point ",index,":"
  write "  Action Result          = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Passive AP Count       = ", $SC_$CPU_LC_ART[index].PassiveAPCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  write " "
enddo

write "  Watch Point 0:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[0].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[0].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[0].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[0].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[0].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[0].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[0].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[0].CountdownToStale
write " "
write "  Watch Point 4:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[4].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[4].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[4].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[4].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[4].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[4].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[4].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[4].CountdownToStale
write " "
write "  Watch Point 8:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[8].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[8].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[8].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[8].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[8].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[8].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[8].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[8].CountdownToStale
write " "
write "  Watch Point 12:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[12].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[12].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[12].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[12].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[12].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[12].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[12].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[12].CountdownToStale
write " "
write "  Watch Point 22:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[22].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[22].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[22].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[22].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[22].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[22].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[22].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[22].CountdownToStale
write " "
write "  Watch Point 23:"
write "  WatchResults        = ", p@$SC_$CPU_LC_WRT[23].WatchResults
write "  Evaluation Count    = ", $SC_$CPU_LC_WRT[23].EvaluationCount
write "  False to True Count = ", $SC_$CPU_LC_WRT[23].FalsetoTrueCount
write "  Consecutive True    = ", $SC_$CPU_LC_WRT[23].ConsectiveTrueCount
write "  Cum True Count      = ", $SC_$CPU_LC_WRT[23].CumulativeTrueCount
write "  F to T Value        = ", $SC_$CPU_LC_WRT[23].FtoTValue
write "  T to F Value        = ", $SC_$CPU_LC_WRT[23].TtoFValue
write "  Stale Counter       = ", $SC_$CPU_LC_WRT[23].CountdownToStale

step3:
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
write ";  End procedure $SC_$CPU_lcx_stale"
write ";*********************************************************************"
ENDPROC
