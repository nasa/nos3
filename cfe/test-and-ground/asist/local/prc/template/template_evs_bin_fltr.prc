PROC $sc_$cpu_evs_bin_fltr
;  
;  Test Name:   bin_fltr
;  Test Level:  Build Verification
;  Test Type:   Functional 
;        
;  Original:    Eva I. Stattel
;
;******************************************************************************
;
;  Test Description:
;
;  The purpose of bin_fltr test is to verify the correct functionality of the
;  Binary Filter Process in the cFE FSW.
;
;  1st. Upon Power-On bin_fltr gets default values for the following:
;         Event Msg Format enabled
;         Output Port enabled
;         Logging Mode Enabled
;         Evt Msg Generation Status
;         Event Msg Types Statuses 
;         Applications Registered for EVS
;         Enable the DEBUG evt Msg Type
;         
;  2nd.   Test Binary Filter Behavior for all Registered Applications:
;           Verify (only after their correct registration)
;           that no-op evt messages are registered for filtering as follows:
;           
;                    App Name    Msg ID    Mask  
;                    --------------------------
;                    CFE_EVS       0         0
;                    CFE_SB        23        0
;                    CFE_ES        3         0 
;                    CFE_TIME      4         0
;                    CFE_TBL       A         0
;
;          Send no-op commands and verify that evt messages
;          are generated accordingly
;          Change Binary masks to no-op evt msgs to x'ffff' (1 evt only)
;          Clear for all apps no-op evt msg ctrs
;          Send 5 no-op commands/app verify that only one evt is gened/per app
;          Delete filters for no-op evt msgs and verify deletion of their 
;	   related information.
;          Send 5 no-op commands/app verify that the evts are generated and 
;	   respective counters increment accordingly
; Step 2.5   Test Add Event Filter Command -- Nominal and error cases.
; Step 2.6   Register for max number of events for SB task, and then request one
;            more.  Check for error message.
; Step 2.7   Attempt to add event filter to CI_APP.
;            Should fail since CI registers for 8 events in flight software.
; Step 2.8   Check Application Data File to confirm SB & CI tasks are registered.
; Step 2.9   Test Delete Event Filter Command -- Nominal and error cases.
;
;  3rd.  Register EVS test Application to further test binary filter processing:
;          Load/Start Test App
;          Enable DEBUG evt message type for Test App
;          Request generation of every event message registered for filtering
;	   with Test app and verify their correct generation according to their
;	   masks as follows:
;
;                    ID     Mask      # msgs   generation
;                     1        0      20       All 20 sequentially
;                     2  x'ffff'       1       First one only
;                     3        1      10       Every other one   
;                     4  x'fff8'       8       First eight only
;                     5        2      10       Every other 2
;                     17       0      20       All 20 sequentially
;
;          Change masks for test apps filters as follows:
;                     
;                     EVENT_ID=1 EVENT_MASK = 2
;                     EVENT_ID=2 EVENT_MASK = x'ffff'
;                     EVENT_ID=3 EVENT_MASK = 1
;                     EVENT_ID=4 EVENT_MASK = x'fff8'
;                     EVENT_ID=5 EVENT_MASK = 2
;                     EVENT_ID=8 EVENT_MASK = 0
;
;          Verify correct change of masks
;          Request generation of 20 iterations of every event message
;          Verify their correct generation according to their masks
;          Clear app bin fltr ctr for evt msg ID 4
;          Request generation of 20 of msg ID 4 (mask x'fff8)
;          Verify generation of 8 iterations only of event msg ID 4/mask
;          Verify correct increment of respective counters
;          
;   4th.   Add filter for evt msg ID 12 Mask 0 (not registered for filtering)
;          Verify addition of evt msg ID 12
;          Add filter for evt msg ID 12 Mask 0 (already registered for filtering)
;          Verify that error evt msg is generated indicating that the evt msg
;          cannot be registered since it is already registered
;          Add filter for evt msg ID 13 Mask 0 (not registered for filtering)
;          Verify addition fo evt msg ID 13
;          Add filter for evt msg ID 14 Mask 0 (not registered for filtering)
;          Verify that error evt msg is generated indicating that the evt msg 
;          cannot be registered because the max number of evts registered has 
;          already been reached
;
;   5th.   Test Max Evt msg sent ctrs and bin fltr ctrs:
;          Clear all test app binary filter counters
;          Clear app evt msg sent ctr
;          Clear total evt msg sent ctr
;          Set mask for evt msg ID 1 to x'0000' (always send)
;          Send 10 noop commands to CFE_SB (to cause respective ctr increments)
;          Using test app:
;            Request generation of 10 iterations of evt msg id 15 (not 
;	     registered for filtering) 
;            Verify respective ctr increments
;            Request gen of 65524 iterations of evt msg ID 1
;            Verify correct increment of respective ctrs
;            Request gen of 1 iterations of evt msg ID 1
;            Verify correct increment of respective ctrs
;            Request gen of 9 iterations of evt msg ID 1
;            Verify correct increment of respective ctrs
;            Request gen of 1 iterations of evt msg ID 1
;            Verify correct increment of respective ctrs
;            Request gen of 10 iterations of evt msg ID 1
;            Verify correct increment of respective ctrs
;            Request gen of 10 iterations of evt msg ID 8 (registered for 
;	     filtring)
;            Verify that respective ctrs reach their maximum values and do not
;	     rollover to zero.
;
;    6th.  Set binary filter mask for specified applications and events.
;          Verify that mask was set correctly, and works correctly.
;          
;************************************************************************
;
;  Requirements:
;
;   cEVS3003	Upon receipt of Command, the cFE shall set the following
;		counters to zero in the Event Services telemetry.
;   cEVS3004	Upon receipt of Command, the cFE shall write the following 
;		information to the Command specified cFE EVS Application Data
;		file for each registered Application: 
;			d)   Application Event IDs (for events to be  
; 			     filtered)
;			e)   Application Binary Filter Masks (one per 
;			     registered Event ID)
;			f)   Application Binary Filter Counters (one per   
;    			     registered Event
;   cEVS3009	Upon receipt of Command, the cFE shall set the Command-specified
;		Application's Event Message Sent Counter to zero. 
;   cEVS3010	Upon receipt of Command, the cFE shall set an Application's
;		Binary Filter Counter to zero for the Command-specified EventID.
;   cEVS3011	Upon receipt of Command, the cFE shall set all of an 
;		Application's Binary Filter Counters to zero.
;   cEVS3012	Upon receipt of Command the cFE shall set an Application's
;		Binary Filter Mask to the Command-specified Event Filter for the
;		given Application Event ID.
;   cEVS3019    Upon receipt of Command, the cFE shall add the specified Event
;		Filter for the specified cFE Application and Event ID.
;   cEVS3019.1  If the Event ID is already registered for filtering, the cFE
;		shall reject the command and generate an event message.
;   cEVS3019.2  If the maximum number of Event IDs have been registered for the
;		specified Application then the cFE shall reject the command and
;		generate an event message.
;   cEVS3020	Upon receipt of a command, the cFE shall delete the specified
;		Event Filter for the specified cFE Application and Event ID.
;   cEVS3020.1  If the Event ID is not registered for filtering then the cFE
;		reject the command and generate an event message.
;   cEVS3100	Upon receipt of Request, the cFE shall register an Application
;		for event service, enabling the Application Event Service Enable
;		Status and storing the following request specified Application
;		data:
;			a) Application Event IDs (for events to be filtered)
;			b) Application Binary Filter Masks (one per registered
;			   Event ID)
;   cEVS3100.1	Upon receipt of Request to register an Application for event
;		service, the cFE shall create one Application Binary Filter
;		Counter per registered Event ID.
;   cEVS3103	Upon receipt of a Request to send an Event Message, the cFE 
;		shall create a Short or Long Event Message, as specified by the
;		SB Event Format Mode, ONLY if the following cFE conditions are
;		met:
;			a) The requesting Application's Event Service Enable
;			   Status is Enabled.
;			b) The requesting Application's evt registered message
;			   filtering algorithm indicates the message should be
;			   sent.
;			c) The requesting Application's Event Type Enable Status;			   is Enabled for the Event Type of the request-
;			   specified Event Message.
;  cEVS3103.3	If the request specified Event ID has been registered for binary
; 		event filtering, the cFE shall increment the request specified
;		Application's Binary Filter Counter for the request-specified
;		Event ID.
;  cEVS3103.3.1 If the Binary Filter Counter has reached its maximum value of 
;		65535, the cFE shall retain the maximum value (i.e. do not 
;		rollover to zero).  
;  cEVS3104	For each created Event Message, the cFE shall increment the 
;		Application Event Message Sent Counter for the request-specified
;		Application. 
;  cEVS3104.1	If the Application Event Message Sent Counter has reached its
;		maximum value of 65535, the cFE shall retain the maximum value
;		(i.e. do not rollover to zero).
;  cEVS3105	For each created Event Message, the cFE shall increment the
;		Event Message Sent Counter. 
;  cEVS3105.1	If the Event Message Sent Counter has reached its maximum value
;		of 65535, the cFE shall retain the maximum value (i.e. do not 
;		rollover to zero).
;  cEVS3106	Upon receipt of a request, the cFE shall set an Application's 
;		Binary Filter Counter to zero for the Application 
;		request-specified Event ID.
;  cEVS3107	Upon receipt of a request, the cFE shall set all of an
;		Application's Binary Filter counters to zero for the 
;		request-specified Application.
;  cEVS3302	The cFE shall support <PLATFORM_DEFINED,8> Event Message Filters
;		per cFE Application.
;
;******************************************************************************
;      Prerequisite conditions and constraints:
;      A test application exists
;      CFE_SB task has registered with EVS.  None of its events are registered.
;******************************************************************************
;      Expected test results Upon power-on:
;
;      Output port
;      Event msg format
;      Event msg generation status 
;      Event msg types statuses 
;      Applications:
;        Filtered Evt IDS
;        Binary filter masks (one per registered ID)
;        Binary filter counters (one per registered ID)
;        App evt msg sent counter
;      Total evt message sent counter
;      
;******************************************************************************
;      Expected of bld 3.1 only
;******************************************************************************
;      When no other filters exist for a given app, each newlly added filter is
;      found at the end of the buffer.
;      whereas, when a filter is deleted while no other empty slots exist, and a new
;      filter is added, this will take the place of the one deleted. 
;
;******************************************************************************
;
;     Change History:
;     
;     Date 		Name	 		Description
;     ---------  -----------------  --------------------------------------------
;     02/15/06   Eva I. Stattel	    Original
;     06/15/06   EIS                Removed all references to CI_APP and TO_APP 
;                                   because they are not core CFE applications
;                                   removed references to ut_sendcmd where 
;				    applicable
;     06/19/06   EIS                run proc although cFE Bld 3.1 test app has
;				    not yet been compiled with LRO
;     07/13/06   EIS                command $sc_$cpu_evs_rstallfltrs added 
;     07/25/06   EIS                replaced .dat with .dat at get_file_to_cvt
;				    calls
;     07/25/06   EIS                registration of no-op evt for CFE_ES added
;                                   modified check for registered no-op evt 
;				    messages in Bld 3.3 they are added at the
;				    top of the buffer rather than at the bottom
;				    (Bld 3.1)
;     07/25/06   EIS                replaced = with <> in step 4.2.1 when
;				    checking for ID 13
;     02/13/07   Nancy Schweiss     Updated for cFE Build 4.0.0. Added tests for
;				    req'ts 3019 and 3020.
;     06/20/07   Walt Moleski	    Updated for cFE Build 4.1.0.
;     08/12/09   Walt Moleski	    Updated to use the event definitions for
;				    each application rather than hard-coded IDs
;     10/13/11   Walt Moleski	    Updated to use variables for RAM disk and
;				    some general clean-up
;     09/17/14   Walt Moleski	    Updated to work with cFE6.4.0.0 and the fact
;				    that the SCH application is running.
;     05/04/16  W. Moleski      Updated for 6.5.0 testing using CPU1 for
;                               commanding and added a hostCPU variable for the
;                               utility procs that connect to the host IP.
;
;******************************************************************************
;     Procedures/Utilities called
;
;      Name               		Description
;     ---------------  		------------------------------------------------
;      ut_runproc          	Directive to formally run the procedure and
;				capture the log file.
;      ut_pfindicate       	Directive to print the pass fail status of a
;				particular requirement number.
;      ut_setupevents         	Directive to look for a particular event and
;				increment a value in the CVT to indicate receipt
;      ut_tlmupdate        	Procedure to wait for a specified telemetry
;				point to update.
;      ut_tlmwait          	Procedure to wait for a specified telemetry
;				point to update to a specified value.
;      get_file_to_cvt     	Procedure to write some specified FSW data to a
;				file and then ftp the file from the FSW hardware
;				to ASIST hardware and load file to the CVT.  
;           NOTE: the preceding action generated 2 debug evt msgs.
;      load_start_app     	Procedure to load and start a user application
;				from the /s/opr/accounts/cfebx/apps/cpux 
;				directory.
;      ut_setrequirements  	Directive to set cfe requirements.
;
;******************************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "ut_statusdefs.h"       
#include "cfe_evs_events.h"
#include "tst_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_sb_events.h"
#include "cfe_time_events.h"
#include "cfe_tbl_events.h"
 
%liv (log_procedure) = logging

;;#include "cfe_evs_bin_fltr_reqts.h" 
;; all the defines below were moved from the cfe_evs_bin_fltr_reqts.h
#define cEVS3003	0
#define cEVS3004 	1
#define cEVS3009 	2
#define cEVS3010 	3
#define cEVS3011 	4
#define cEVS3012 	5
#define cEVS3019 	6
#define cEVS3019_1 	7
#define cEVS3019_2 	8
#define cEVS3020 	9
#define cEVS3020_1 	10
#define cEVS3100 	11
#define cEVS3100_1 	12
#define cEVS3103 	13
#define cEVS3103_3 	14
#define cEVS3103_3_1 	15
#define cEVS3104 	16
#define cEVS3104_1 	17
#define cEVS3105 	18
#define cEVS3105_1 	19
#define cEVS3106 	20
#define cEVS3107 	21
#define cEVS3302 	22

global ut_req_array_size = 22
global ut_requirement[0 .. ut_req_array_size]
 
;
;   Variables Definition:
;    
FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO 
 
local cfe_requirements[0..ut_req_array_size] = ["cEVS3003","cEVS3004", ;;
"cEVS3009","cEVS3010","cEVS3011","cEVS3012","cEVS3019","cEVS3019_1", ;;
"cEVS3019_2","cEVS3020","cEVS3020_1","cEVS3100","cEVS3100_1","cEVS3103", ;;
"cEVS3103_3","cEVS3103_3_1","cEVS3104","cEVS3104_1","cEVS3105","cEVS3105_1", ;;
"cEVS3106","cEVS3107","cEVS3302"]
 
local i = 0 
local j = 0
local cfe_apps = 0
local cmdexctr = $SC_$CPU_EVS_CMDPC
;cfe_evs_task_ndx stores the index into $SC_$CPU_EVS_AppData array for EVS task data
local cfe_evs_task_ndx = 0  
local cfe_ci_task_ndx = 0  
local cfe_sb_task_ndx = 0  
local cfe_es_task_ndx = 0
local cfe_time_task_ndx = 0
local cfe_tbl_task_ndx = 0

; apps_registered stores the index into $sc_$CPU_EVS_APP array for TST_EVS task
local apps_registered = 0   
local lclevtmsgsentctr = 0
local EVSevtmsgsentctr = 0
local ESevtmsgsentctr = 0
local SBevtmsgsentctr = 0
local TIMEevtmsgsentctr = 0
local TBLevtmsgsentctr = 0
local EVSbinfltrctr = 0
local SBbinfltrctr = 0
local ESbinfltrctr = 0
local TBLbinfltrctr = 0
local TIMEbinfltrctr = 0
local TSTEVSevtmsgsentctr = 0
local evs_cmdcnt 
local evs_errcnt
local dest_pathname 

local ramDir = "RAM:0"
local hostCPU = "$CPU"
 
write ";***********************************************************************"
write "; Step 1.0: Setup/Initialization"
write ";***********************************************************************"
write "; Step 1.1: Power On"
;                  Expect the following defaults
;                  Evt Msg Format = LONG
;                  Output Port Enable = 1
;                  Logging Mode = OVERWRITE
;                  Evt Msg Generation Status = ENABLE
;                  Evt Msg Types
;                  DEBUG DISABLE 
;                  INFO ENABLE
;                  ERROR  ENABLE
;                  CRITICAL ENABLE and 
;                  all requirements set to their initial value of "U" 
;                  for undefined.
write ";***********************************************************************"
 
/$SC_$CPU_ES_POWERONRESET
wait 10
                                                                                
close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write" >>> Default evt msg format Event Msg Format is ", p@$SC_$CPU_EVS_MSGFMTMODE
;
write" >>> Default Logging Mode is ", p@$SC_$CPU_EVS_LOGMODE
;
;     assign initial value of total evt msg sent ctr to local variable
;
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC

write" >>> TEMSC Total Event Msg Sent Ctr = ", $SC_$CPU_EVS_MSGSENTC

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
;
write" >>> Initial Requirements Report"
;
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo
;
write ";***********************************************************************"
write "; Step 1.2: Retrieve application data"
;         Assign the, "A" value to requirement cEVS3004 which indicates that
;         further analysis of the output from the app data file is required
;         Expect default apps, but no test application registered.
;         Expect default evt msg type statuses as follows:
;             DEBUG DISABLE 
;             INFO ENABLE
;             ERROR  ENABLE
;             CRITICAL ENABLE
write ";***********************************************************************"
cmdexctr =$SC_$CPU_EVS_CMDPC + 1

start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_1_2.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

if (ut_tw_status = UT_TW_Success) then
  for i = 1 to CFE_ES_MAX_APPLICATIONS do
    if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
      cfe_evs_task_ndx = i
    elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
      cfe_sb_task_ndx = i
    elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
      cfe_es_task_ndx = i
    elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
      cfe_time_task_ndx = i
    elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
      cfe_tbl_task_ndx = i
    endif
  enddo
else
  write " <!> Failed - Did not write the evs app file"
endif

write " TEMSC Total Evt Msg Sent Ctr = ", $SC_$CPU_EVS_MSGSENTC

EVSevtmsgsentctr =  $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
SBevtmsgsentctr  =  $SC_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC
ESevtmsgsentctr  =  $SC_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC
TIMEevtmsgsentctr =  $SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC
TBLevtmsgsentctr  =  $SC_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC

write " >>> For all registered apps:"
write " >>> App Name  |  App Evt Msg  |   Evt ID   |   Mask    |  Bin Fltr"
write "               |      Ctr      |            |           |    Ctr  "
;
for i = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_EVS_AppData[i].AppName <> "") then
    write " >>> ", $SC_$CPU_EVS_AppData[i].AppName, "        ", $SC_$CPU_EVS_APP[i].APPMSGSENTC
    apps_registered = apps_registered + 1
    if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
      for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
	;;; Replacing 3 with NOOP define
        if ($SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId = CFE_ES_NOOP_INF_EID) then
	  ESbinfltrctr = $SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr
          write " ------------------------------- ", $SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId,"     |  ",%hex($SC_$CPU_EVS_AppData[i].BinFltr[j].Msk, 5),"  |  ",$SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr
        endif
      enddo
    endif
  endif
enddo

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" 
write " >>> Initial Total Number of Registered Applications: ", apps_registered 
write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> cFE Core Applications:"

for i = 1 to CFE_ES_MAX_APPLICATIONS do
  if  %substring($SC_$CPU_EVS_AppData[i].AppName,1,4) = "CFE_" then 
    cfe_apps = cfe_apps + 1
    write ">>> ", $SC_$CPU_EVS_AppData[i].AppName
  endif
enddo

write " >>> Default Event Msg Types Statuses"

;; This logic assumes that the CFE_ applications are reported first.
;; If this is not true, this test will fail
for i = 1 to cfe_apps do 
  write " >>> ", $SC_$CPU_EVS_AppData[i].AppName
  write  " DEBUG  ",  p@$SC_$CPU_EVS_AppData[i].EvtTypeAF.Debug  
  write  " INFO   ",  p@$SC_$CPU_EVS_AppData[i].EvtTypeAF.Info   
  write  " ERROR  ",  p@$SC_$CPU_EVS_AppData[i].EvtTypeAF.Err   
  write  " CRIT   ",  p@$SC_$CPU_EVS_AppData[i].EvtTypeAF.Crit   
enddo

write ";***********************************************************************"
write "; Step 1.3: Set up initial condition"
write ";***********************************************************************"
write "; Step 1.3.1: Enable DEBUG event msg type for registered applications"
write ";             with only one command"
write ";***********************************************************************"
 
;; cFE6.4.0.0: With SCH sending ID=16 events, the total cannot be verified here
;; The checks are made in other steps and commented out here
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC + 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"

ut_tlmwait $SC_$CPU_EVS_MSGSENTC {lclevtmsgsentctr}

;;EVSevtmsgsentctr = EVSevtmsgsentctr + 1

; the counters increment because execution of ENAAPPEVTTYPE generates an evt msg
;;if ($SC_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;;  write "<*> Passed (3105) - Message sent counters are correct."
;;  ut_setrequirements cEVS3105, "P"
;;else 
;;  ut_setrequirements cEVS3105, "F"
;;  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
;;  lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
;;endif
;;
;;if ($SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
;;  write "<*> Passed (3104) - Message counters are correct."
;;  ut_setrequirements cEVS3104, "P"
;;else 
;;  ut_setrequirements cEVS3104, "F"
;;  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
;;endif

;; Disable SCH Debug Events if SCH is running
local SCHisRunning = FALSE
for i = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_EVS_AppData[i].AppName = "SCH") then 
    SCHisRunning = TRUE
  endif
enddo

if (SCHisRunning = TRUE) then
  cmdexctr =$SC_$CPU_EVS_CMDPC + 1

  /$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION="SCH" DEBUG

  ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 
endif

write " >>> EVS evt msg sent ctr = ", $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write " >>> TEMSC Total Evt Msg Sent Ctr = ", $SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 1.3.2: Retrieve application data"
;         Expect Event Msg Type DEBUG status ENABLE for registered apps
;         Until otherwise stated, expect total evt msg sent and CFE_EVS app evt 
;         msg sent ctr to increment by two when retrieving application data.
;
;         Note: the evt msg type must be enabled for an evt msg of such type to
;               be generated
write ";***********************************************************************"

;;lclevtmsgsentctr = lclevtmsgsentctr + 1
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC + 1
EVSevtmsgsentctr =  $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_132.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
else
  write "<!> Failed at tlm wait at get file to CVT"
endif   

if ($SC_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Message sent counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - Message counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write " >>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
                     
write " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" 
for i = 1 to cfe_apps do
  write " >>> For App name ",$SC_$CPU_EVS_AppData[i].AppName
  write " >>> DEBUG evt msg type status is ", p@$SC_$CPU_EVS_AppData[i].EvtTypeAF.Debug
  if ($sc_$CPU_EVS_AppData[i].EvtTypeAF.Debug <> 1) then
    write "<!> Failed - Incorrect Evt Msg type DEBUG status"
  endif
enddo
 
write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write" >>> Initial Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo
 
;-------------------------------------------------------------------------------
;                   APP    No-op Evt ID 
;                ------------------------
;                   EVS       N/REG
;                   SB        N/REG
;                   ES        3
;                   TIME      N/REG
;                   TBL       N/REG
 
write ";***********************************************************************"
write "; Step 1.3.3: For the CFE_SB app, check if there are any filters "
write ";	     registered for EID=0. If so, delete them."
write ";***********************************************************************"
;; Loop for each event filter
for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[j].EvtId = 0) then
    write "CFE_SB invalid filter found. Removing it."
    ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1

    /$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=0
    wait 5

    ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
    if (ut_tw_status = UT_TW_Success) then
      write "<*> Passed - Removed CFE_SB Event Filter for EID =0"
      EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    else
      write "<!> Failed to remove CFE_SB Event Filter"
    endif
  endif
enddo

write ";***********************************************************************"
write "; Step 2.0: Test Binary Filter Behavior for Registered Applications"
write ";***********************************************************************"
write "; Step 2.1: Add filters for no-op evt msgs for apps that do not have "
write ";           them registered for filtering"
write ";***********************************************************************"
 
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

local evtID = CFE_EVS_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_EVS"" EVENT_ID=evtID EVENT_MASK=0"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (ut_tw_status = UT_Success) THEN
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Message sent counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counters are correct."
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif 
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - EVS Add Filter command."
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - EVS add Filter command" 
endif 

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;; Set the values to the actuals in order to avoid accumulating errors
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

evtID = CFE_SB_CMD0_RCVD_EID
Ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=evtID EVENT_MASK=0"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (ut_tw_status = UT_Success) THEN
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Message sent counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counters are correct."
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif  
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - SB Add Filter command."
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - SB add Filter command" 
endif 

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;; Set the values to the actuals in order to avoid accumulating errors
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

evtID = CFE_ES_NOOP_INF_EID
Ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_ES"" EVENT_ID=evtID EVENT_MASK=0"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (ut_tw_status = UT_Success) THEN
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  if ($SC_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Message sent counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counters are correct."
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif 
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - ES Add Filter command."
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - ES add Filter command" 
endif 

write " TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write " >>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;; Set the values to the actuals in order to avoid accumulating errors
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

evtID = CFE_TIME_NOOP_EID
Ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_TIME"" EVENT_ID=evtID EVENT_MASK=0"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (ut_tw_status = UT_Success) THEN
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Message sent counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counters are correct."
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif 
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - TIME Add Filter command."
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - TIME add Filter command" 
endif 

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;; Set the values to the actuals in order to avoid accumulating errors
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

evtID = CFE_TBL_NOOP_INF_EID
Ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_TBL"" EVENT_ID=evtID EVENT_MASK=0"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (ut_tw_status = UT_Success) THEN
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Message sent counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counters are correct."
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif 
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - TBL Add Filter command."
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - TIME add Filter command" 
endif 

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;; Set the values to the actuals in order to avoid accumulating errors
lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

write ";***********************************************************************"
write "; Step 2.1.1: Retrieve applications data"
;
;                  Expect: for registered apps: their No-op event messages 
;                  are registered for filtering as follows
;
;                  APP NAME       NO-OP EVT ID   MASK   BIN FLTR CTR
;                  --------------------------------------------------
;                  CFE_EVS        0              0      INIT VALUE
;                  CFE_SB         35             0      "
;                  CFE_ES         3              0      "
;                  CFE_TIME       4              0      "
;                  CFE_TBL        10             0      "
;
;        Upon verification of correct addition of no-op evt msgs set correct 
;	 value for:
;            cEVS3019
;            cEVS3104d, cEVS3104e and 
;	     cEVS3104f (this is the first part of their test, 
;                     they are fully verified later)
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_211.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
else
  write "<!> Failed -  Could not retrieve evs app data file"
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Message sent counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  write "<*> Passed (3104) - Message counts correct"
  ut_setrequirements cEVS3104, "P"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write " >>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

local j
local foundEVSEvt = FALSE
local foundSBEvt = FALSE
local foundESEvt = FALSE
local foundTIMEEvt = FALSE
local foundTBLEvt = FALSE
local EVSNoopIdx  = 0
local SBNoopIdx   = 0
local ESNoopIdx   = 0
local TIMENoopIdx = 0
local TBLNoopIdx  = 0

for i = 1 to cfe_apps do
  if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    ;; Find Event ID 0
    for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      if ($SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId = CFE_EVS_NOOP_EID) then
        if ($SC_$CPU_EVS_AppData[i].BinFltr[j].Msk = 0) AND ;;
           ($SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr = 0) then
	  foundEVSEvt = TRUE
	  EVSNoopIdx = j
          write "EVS NO-OP command filter found."
        endif
      endif
    enddo
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    ;; Find Event ID 35
    for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      if ($SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId = CFE_SB_CMD0_RCVD_EID) then
        if ($SC_$CPU_EVS_AppData[i].BinFltr[j].Msk = 0) AND ;;
           ($SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr = 0) then
          foundSBEvt = TRUE
	  SBNoopIdx = j
          write "SB NO-OP command filter found."
        endif
      endif
    enddo
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    ;; Find Event ID 3
    for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      if ($SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId = CFE_ES_NOOP_INF_EID) then
        if ($SC_$CPU_EVS_AppData[i].BinFltr[j].Msk = 0) AND ;;
           ($SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr = 0) then
	  foundESEvt = TRUE
	  ESNoopIdx = j
          write "ES NO-OP command filter found."
        endif
      endif
    enddo
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    ;; Find Event ID 4
    for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      if ($SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId = CFE_TIME_NOOP_EID) then
        if ($SC_$CPU_EVS_AppData[i].BinFltr[j].Msk = 0) AND ;;
           ($SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr = 0) then
	  foundTIMEEvt = TRUE
	  TIMENoopIdx = j
          write "TIME NO-OP command filter found."
        endif
      endif
    enddo
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    ;; Find Event ID 10
    for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      if ($SC_$CPU_EVS_AppData[i].BinFltr[j].EvtId = CFE_TBL_NOOP_INF_EID) then
        if ($SC_$CPU_EVS_AppData[i].BinFltr[j].Msk = 0) AND ;;
           ($SC_$CPU_EVS_AppData[i].BinFltr[j].Ctr = 0) then
	  foundTBLEvt = TRUE
	  TBLNoopIdx = j
          write "TBL NO-OP command filter found."
        endif
      endif
    enddo
  endif
enddo

; NOTE: cEVS3004 is set to "A" indicating further analysis required 
; Setting this to "P" because if all the filters are found above, that verifies
; the file was written since the CVT holds the contents of the file      
           
if (foundEVSEvt = TRUE AND foundSBEvt = TRUE AND foundESEvt = TRUE AND ;;
    foundTIMEEvt = TRUE AND foundTBLEvt = TRUE) then
  ut_setrequirements cEVS3004, "P"
  ut_setrequirements cEVS3019, "P" 
  write "<*> Passed (3004;3019) - All NOOP events found"
else
  ut_setrequirements cEVS3004, "F"
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3004;3019) - invalid values found while verifying added NO-OP command filter"
endif

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
;
write" >>> Requirements Report"
;
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo
   
write ";***********************************************************************"
write "; Step 2.2: Send 10 No-op commands to cause generation of their event"
write ";           messages for registered applications"
;
;             Expect 10 Noop Evt Msgs generated per app
;             Expect total evt msg sent counter to increment by 10/app
;             Expect for every registered app, evt msg sent ctr incrments by 10
;                     
;             cEVS3103 (bin fltr algorithm indicates SEND message)
write ";***********************************************************************"


; increment app counters
lclevtmsgsentctr  = $SC_$CPU_EVS_MSGSENTC
EVSevtmsgsentctr   = $SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC + 10
ESevtmsgsentctr   = $SC_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC + 10
SBevtmsgsentctr   = $SC_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC + 10
TIMEevtmsgsentctr = $SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC + 10
TBLevtmsgsentctr  = $SC_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC + 10
 
;;;;   send no-op commands
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_NOOP_EID, INFO, 1
 
write "; BEFORE EVS_NOOP cmds: "
write ";      lclevtmsgsentctr = ",lclevtmsgsentctr 
write ";      EVSevtmsgsentctr = ",EVSevtmsgsentctr 

for i = 1 to 10 do
  /$SC_$CPU_EVS_NOOP
  wait 3 
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3103;3105) - All NOOP events found"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counts correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - EVS NOOP events generated = ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 10"
endif  

;------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO, 1

write "; BEFORE SB_NOOP cmds: "
write ";      lclevtmsgsentctr = ",lclevtmsgsentctr 
write ";      SBevtmsgsentctr = ",SBevtmsgsentctr 

for i = 1 to 10 do
  /$SC_$CPU_SB_NOOP
  wait 3
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10
write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3103;3105) - All NOOP events found"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC = SBevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counts correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - SB app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC,"; Expected ",SBevtmsgsentctr
  endif
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - SB NOOP events generated = ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 10"
endif                     

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 1

write "; BEFORE ES_NOOP cmds: "
write ";      lclevtmsgsentctr = ",lclevtmsgsentctr 
write ";      ESevtmsgsentctr = ",ESevtmsgsentctr 

for i = 1 to 10 do
  /$SC_$CPU_ES_NOOP
  wait 3
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10 
write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3103;3105) - All NOOP events found"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif
 
  if ($sc_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC = ESevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counts correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - ES app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC,"; Expected ",ESevtmsgsentctr
  endif
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - ES NOOP events generated = ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 10"
endif                     

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1

write "; BEFORE TIME_NOOP cmds: "
write ";      lclevtmsgsentctr = ",lclevtmsgsentctr 
write ";      TIMEevtmsgsentctr = ",TIMEevtmsgsentctr 

for i = 1 to 10 do
  /$SC_$CPU_TIME_NOOP
  wait 3
enddo                      
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10
write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  write " TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC         
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3103;3105) - All NOOP events found"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC = TIMEevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counts correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - TIME app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC,"; Expected ",TIMEevtmsgsentctr
  endif   
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - TIME NOOP events generated = ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 10"
endif

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_TBL, CFE_TBL_NOOP_INF_EID, INFO, 1

write "; BEFORE TBL_NOOP cmds: "
write ";      lclevtmsgsentctr = ",lclevtmsgsentctr 
write ";      TBLevtmsgsentctr = ",TBLevtmsgsentctr 

for i = 1 to 10 do
  /$SC_$CPU_TBL_NOOP
  wait 3
enddo                      
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10
write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
                     
if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  write " TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC         
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3103;3105) - All NOOP events found"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC = TBLevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - Message counts correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - TIME app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC,"; Expected ",TBLevtmsgsentctr
  endif       
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - TBL NOOP events generated = ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 10"
endif                   
 
;-------------------------------------------------------------------------------
 
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

write " ========================================="
for i = 1 to cfe_apps do
  write ">>> ",$SC_$CPU_EVS_AppData[i].AppName, " App Evt msg sent ctr = ",$SC_$CPU_EVS_APP[i].APPMSGSENTC 
enddo
write " ========================================="

write ";***********************************************************************"
write "; Step 2.2.1:  Retrieve applications data "
;           Expect app bin fltr ctr for no-op evt msg increment by 10 per app 
;              cEVS3103_3
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_221.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
endif   

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $SC_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;                     
;      increment local app bin fltr ctrs
;
EVSbinfltrctr  = EVSbinfltrctr  + 10
SBbinfltrctr   = SBbinfltrctr   + 10
ESbinfltrctr   = ESbinfltrctr   + 10
TBLbinfltrctr  = TBLbinfltrctr  + 10
TIMEbinfltrctr = TIMEbinfltrctr + 10

for i = 1 to cfe_apps do
  if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr = 10) then 
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - EVS NOOP Binary Filter count = 10"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - EVS NOOP Binary Filter count = ", $SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr, "; Expected 10"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr = 10) then 
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - SB NOOP Binary Filter count = 10"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - SB NOOP Binary Filter count = ", $SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr, "; Expected 10"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr = 10) then 
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - ES NOOP Binary Filter count = 10"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - ES NOOP Binary Filter count = ", $SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr, "; Expected 10"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr = 10) then 
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - TIME NOOP Binary Filter count = 10"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - TIME NOOP Binary Filter count = ", $SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr, "; Expected 10"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr = 10) then 
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - TBL NOOP Binary Filter count = 10"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - TBL NOOP Binary Filter count = ", $SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr, "; Expected 10"
    endif
  endif
enddo

write ";***********************************************************************"
write "; Step 2.3: Change Binary Filter Masks"
write ";***********************************************************************"
write "; Step 2.3.1: Change binary filter masks to X'ffff' for the no-op evt "
write ";           message for registered apps. Mask x'ffff' indicates "
write ";           generation of only one instance of the message and then stop"
;                  cEVS3012
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG, 1

evtID = CFE_EVS_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_EVS"" EVENT_ID=evtID FILTERMASK=X'ffff'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3012, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3012;3105) - Set EVS Binary Filter mask successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
    if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - EVS Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
    endif
  else  
    ut_setrequirements cEVS3012, "F"
    write "<!> Failed (3012) - Set EVS Binary Filter mask failed to generate expected event message"
  endif
endif                     

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
 
;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG, 1

evtID = CFE_SB_CMD0_RCVD_EID
ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_SB"" EVENT_ID=evtID FILTERMASK=X'ffff'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3012, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3012;3105) - Set SB Binary Filter mask successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
    if ($sc_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC = SBevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - SB Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - SB app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC,"; Expected ",SBevtmsgsentctr
    endif
  else
    ut_setrequirements cEVS3012, "F"
    write "<!> Failed (3012) - Set SB Binary Filter mask failed to generate expected event message"
  endif                     
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG, 1

evtID = CFE_ES_NOOP_INF_EID
ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_ES"" EVENT_ID=evtID FILTERMASK=X'ffff'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3012, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3012;3105) - Set ES Binary Filter mask successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
    if ($sc_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC = ESevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - ES Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - ES app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC,"; Expected ",ESevtmsgsentctr
    endif
  else
    ut_setrequirements cEVS3012, "F"
    write "<!> Failed (3012) - Set ES Binary Filter mask failed to generate expected event message"
  endif
endif                     

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
;
;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG, 1

evtID = CFE_TIME_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TIME"" EVENT_ID=evtID FILTERMASK=X'ffff'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3012, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3012;3105) - Set TIME Binary Filter mask successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
    if ($sc_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC = TIMEevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - TIME Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - TIME app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC,"; Expected ",TIMEevtmsgsentctr
    endif
  else
    ut_setrequirements cEVS3012, "F"
    write "<!> Failed (3012) - Set TIME Binary Filter mask failed to generate expected event message"
  endif                     
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG, 1

evtID = CFE_TBL_NOOP_INF_EID
ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TBL"" EVENT_ID=evtID FILTERMASK=X'ffff'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3012, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3012;3105) - Set TBL Binary Filter mask successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
    if ($sc_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC = TBLevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - TBL Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - TBL app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC,"; Expected ",TBLevtmsgsentctr
    endif
  else
    ut_setrequirements cEVS3012, "F"
    write "<!> Failed (3012) - Set TBL Binary Filter mask failed to generate expected event message"
  endif  
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

write ";***********************************************************************"
write "; Step 2.3.1.1: Retrieve applications data "
;           Expect for registered apps that their no-op evt msgs have masks = "
;	    x'ffff'
;              cEVS3012 (Initial ver. Fully verified after correct evt msg gen)
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_2311.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
endif   

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;
;           for all registered apps
;

for i = 1 to cfe_apps do
  if ($sc_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Msk = X'ffff') then
      ut_setrequirements cEVS3012, "P"
      write "<*> Passed (3012) - EVS Binary Filter mask set correctly"
    else 
      ut_setrequirements cEVS3012, "F"
      write "<!> Failed (3012) - Mask for CFE_EVS no-op = ", %hex($SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Msk, 4), "; Expected 'FFFF'"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Msk = X'ffff') then
      ut_setrequirements cEVS3012, "P"
      write "<*> Passed (3012) - SB Binary Filter mask set correctly"
    else 
      ut_setrequirements cEVS3012, "F"
      write "<!> Failed (3012) - Mask for CFE_SB no-op = ", %hex($SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Msk, 4), "; Expected 'FFFF'"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Msk = X'ffff') then
      ut_setrequirements cEVS3012, "P"
      write "<*> Passed (3012) - ES Binary Filter mask set correctly"
    else 
      ut_setrequirements cEVS3012, "F"
      write "<!> Failed (3012) - Mask for CFE_ES no-op = ", %hex($SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Msk, 4), "; Expected 'FFFF'"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Msk = X'ffff') then
      ut_setrequirements cEVS3012, "P"
      write "<*> Passed (3012) - TIME Binary Filter mask set correctly"
    else 
      ut_setrequirements cEVS3012, "F"
      write "<!> Failed (3012) - Mask for CFE_TIME no-op = ", %hex($SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Msk, 4), "; Expected 'FFFF'"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Msk = X'ffff') then
      ut_setrequirements cEVS3012, "P"
      write "<*> Passed (3012) - TBL Binary Filter mask set correctly"
    else 
      ut_setrequirements cEVS3012, "F"
      write "<!> Failed (3012) - Mask for CFE_TBL no-op = ", %hex($SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Msk, 4), "; Expected 'FFFF'"
    endif
  endif
enddo
 
write ";***********************************************************************"
write "; Step 2.3.1.2: Clear for registered apps bin fltr ctrs for no-op evt "
write ";               messages."
;                     cEVS3010 (partial, command to clear bin fltr ctr)
write ";***********************************************************************"

ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG, 1

evtID = CFE_EVS_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION =""CFE_EVS"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3010, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3010;3105) - EVS Reset Binary Filter counter sent successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
  else
    ut_setrequirements cEVS3010, "F"
    write "<!> Failed (3010) - EVS Reset Binary Filter counter did generate expected event message"
  endif     
else
  ut_setrequirements cEVS3010, "F"
  write "<!> Failed (3010) - EVS Reset Binary Filter counter command"
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG, 1

evtID = CFE_SB_CMD0_RCVD_EID
ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION = ""CFE_SB"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3010, "P"
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3010;3105) - SB Reset Binary Filter counter sent successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
  else
    ut_setrequirements cEVS3010, "F"
    write "<!> Failed (3010) - SB Reset Binary Filter counter did generate expected event message"
  endif                     
else
  ut_setrequirements cEVS3010, "F"
  write "<!> Failed (3010) - SB Reset Binary Filter counter command"
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG, 1

evtID = CFE_ES_NOOP_INF_EID
ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION = ""CFE_ES"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3010, "P"
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3010;3105) - ES Reset Binary Filter counter sent successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
  else
    ut_setrequirements cEVS3010, "F"
    write "<!> Failed (3010) - ES Reset Binary Filter counter did generate expected event message"
  endif                     
else
  ut_setrequirements cEVS3010, "F"
  write "<!> Failed (3010) - ES Reset Binary Filter counter command"
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG, 1

evtID = CFE_TIME_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION =""CFE_TIME"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3010, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3010;3105) - TIME Reset Binary Filter counter sent successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
  else
    ut_setrequirements cEVS3010, "F"
    write "<!> Failed (3010) - TIME Reset Binary Filter counter did generate expected event message"
  endif   
else
  ut_setrequirements cEVS3010, "F"
  write "<!> Failed (3010) - TIME Reset Binary Filter counter command"
endif                  

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG, 1

evtID = CFE_TBL_NOOP_INF_EID
ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION = ""CFE_TBL"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3010, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3010;3105) - TBL Reset Binary Filter counter sent successfully"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
  else
    ut_setrequirements cEVS3010, "F"
    write "<!> Failed (3010) - TBL Reset Binary Filter counter did generate expected event message"
  endif 
else
  ut_setrequirements cEVS3010, "F"
  write "<!> Failed (3010) - TBL Reset Binary Filter counter command"
endif                    

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

write ";***********************************************************************"
write "; Step 2.3.1.3: Retrieve applications data "
;		Expect for registered apps that their bin fltr ctrs for
;		no-op evt msgs are all set to zero
;		Upon verification of values for bin fltr ctrs set req
;		cEVS3010 (full ver)                    
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_2313.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr} 60

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
endif   

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

for i = 1 to cfe_apps do
  if ($sc_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3010, "P"
      write "<*> Passed (3010) - EVS NOOP Binary Filter Count is set to 0"
    else 
      ut_setrequirements cEVS3010, "F"
      write "<!> Failed (3010) - EVS NOOP Binary Filter Count was ",$SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3010, "P"
      write "<*> Passed (3010) - SB NOOP Binary Filter Count is set to 0"
    else 
      ut_setrequirements cEVS3010, "F"
      write "<!> Failed (3010) - SB NOOP Binary Filter Count was ",$SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3010, "P"
      write "<*> Passed (3010) - ES NOOP Binary Filter Count is set to 0"
    else 
      ut_setrequirements cEVS3010, "F"
      write "<!> Failed (3010) - ES NOOP Binary Filter Count was ",$SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3010, "P"
      write "<*> Passed (3010) - TIME NOOP Binary Filter Count is set to 0"
    else 
      ut_setrequirements cEVS3010, "F"
      write "<!> Failed (3010) - TIME NOOP Binary Filter Count was ",$SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($sc_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3010, "P"
      write "<*> Passed (3010) - TBL NOOP Binary Filter Count is set to 0"
    else 
      ut_setrequirements cEVS3010, "F"
      write "<!> Failed (3010) - TBL NOOP Binary Filter Count was ",$SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr,"; Expected 0"
    endif
  endif
enddo

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Initial Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo
 
write ";***********************************************************************"
write "; Step 2.3.2: Send 5 No-op commands for each registered application."
;           Expect 1 no-op evt msg per registered app
;           Expect each app evt msg sent ctr to increment by 1 per event 
;           Expect total evt msg sent ctr to increment by 1 per event generated
;           Upon verification of ctr increments set reqs cEVS3104 and cEVS3105 
;	    to their correct values Also set cEVS3103 indicating verification of
;	    bin fltr algorithm function
write ";***********************************************************************"
                       
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_NOOP_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_EVS_NOOP 
  wait 3                    
enddo
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3103;3104) - EVS Event counters are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Sent 5 EVS NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 1"
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_SB_NOOP 
  wait 3                    
enddo
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  SBevtmsgsentctr = SBevtmsgsentctr + 1
  if ($sc_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC = SBevtmsgsentctr)  then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3103;3104) - SB Event counters are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - SB app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC,"; Expected ",SBevtmsgsentctr
  endif 
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Sent 5 SB NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 1"
endif                     

write " >>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write " TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;
;------------------------------------------------------------------------------
;
ut_setupevents $sc, $cpu, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_ES_NOOP 
  wait 3                    
enddo
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ESevtmsgsentctr = ESevtmsgsentctr + 1 
  if ($sc_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC = ESevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3103;3104) - ES Event counters are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - ES app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC,"; Expected ",ESevtmsgsentctr
  endif
      
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif             
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Sent 5 ES NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 1"
endif   

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
 
;------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_TIME_NOOP 
  wait 3                    
enddo
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  TIMEevtmsgsentctr = TIMEevtmsgsentctr + 1
  if ($sc_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC = TIMEevtmsgsentctr) then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3103;3104) - TIME Event counters are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - TIME app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC,"; Expected ",TIMEevtmsgsentctr
  endif                
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif  
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Sent 5 TIME NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 1"
endif     

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
 
;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_TBL, CFE_TBL_NOOP_INF_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_TBL_NOOP 
  wait 3                    
enddo
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  TBLevtmsgsentctr = TBLevtmsgsentctr + 1
  if ($sc_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC = TBLevtmsgsentctr)  then
    ut_setrequirements cEVS3103, "P"
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3103;3104) - TBL Event counters are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - TBL app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC,"; Expected ",TBLevtmsgsentctr
  endif
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif               
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Sent 5 TBL NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 1"
endif                     

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
 
;-------------------------------------------------------------------------------
for i = 1 to cfe_apps do
  write ">>> ",$SC_$CPU_EVS_AppData[i].AppName, " - evt msg sent ctr ",$SC_$CPU_EVS_APP[i].APPMSGSENTC
enddo

write ";***********************************************************************"
write "; Step 2.3.2.1: Retrieve applications data" 
;      Expect an increment by 5 for each app bin flt ctr for each no-op evt msg
;      upon verification of ctrs set cEVS3103_3 accordingly.
;      Upon correct gneration of evt msgs and counter increments, set cEVS3012
;      accordingly, because this fully proves this req.
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                     
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_2321.dat", hostCPU)
  
ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}
 
if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
endif   

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
;      
;      for apps
for i = 1 to cfe_apps do
  if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr = 5) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - EVS NOOP Binary Filter counter set to 5"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - EVS NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr,"; Expected 5"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr = 5) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - SB NOOP Binary Filter counter set to 5"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - SB NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr,"; Expected 5"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr = 5) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - ES NOOP Binary Filter counter set to 5"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - ES NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr,"; Expected 5"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr = 5) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - TIME NOOP Binary Filter counter set to 5"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - TIME NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr,"; Expected 5"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr = 5) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - TBL NOOP Binary Filter counter set to 5"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - TBL NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr,"; Expected 5"
    endif
  endif
enddo
 
write ";***********************************************************************"
write "; Step 2.4: Delete Filters for No-ops"
write ";***********************************************************************"
write "; Step 2.4.1: For all registered apps "
write ";***********************************************************************"
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1

evtID = CFE_EVS_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_EVS"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 10

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - EVS NOOP Delete Filter successful."

    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3105) - Counters are correct"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif

    if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - EVS Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
    endif 
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - EVS NOOP Delete Filter did not generate the expected event message."
  endif
else 
  ut_setrequirements cEVS3020, "F"
  write "<!> Failed (3020) - EVS NOOP Filter Delete Filter command."
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
 
;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1

evtID = CFE_SB_CMD0_RCVD_EID
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 10

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - SB NOOP Delete Filter successful."

    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3105) - Counters are correct"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif

    if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - EVS Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
    endif 
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - SB NOOP Delete Filter did not generate the expected event message."
  endif
else 
  ut_setrequirements cEVS3020, "F"
  write "<!> Failed (3020) - SB NOOP Filter Delete Filter command."
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
 
;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1

evtID = CFE_ES_NOOP_INF_EID
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_ES"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 10

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - ES NOOP Delete Filter successful."

    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3105) - Counters are correct"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif

    if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - EVS Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
    endif 
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - ES NOOP Delete Filter did not generate the expected event message."
  endif
else 
  ut_setrequirements cEVS3020, "F"
  write "<!> Failed (3020) - ES NOOP Filter Delete Filter command."
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1

evtID = CFE_TIME_NOOP_EID
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_TIME"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 10

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - TIME NOOP Delete Filter successful."

    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3105) - Counters are correct"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif

    if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - EVS Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
    endif 
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - TIME NOOP Delete Filter did not generate the expected event message."
  endif
else 
  ut_setrequirements cEVS3020, "F"
  write "<!> Failed (3020) - TIME NOOP Filter Delete Filter command."
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1

evtID = CFE_TBL_NOOP_INF_EID
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_TBL"" EVENT_ID=evtID"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 10

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - TBL NOOP Delete Filter successful."

    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3105) - Counters are correct"
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif

    if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
      ut_setrequirements cEVS3104, "P"
      write "<*> Passed (3104) - EVS Message counts are correct"
    else 
      ut_setrequirements cEVS3104, "F"
      write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
    endif 
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - TBL NOOP Delete Filter did not generate the expected event message."
  endif
else 
  ut_setrequirements cEVS3020, "F"
  write "<!> Failed (3020) - TBL NOOP Filter Delete Filter command."
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 

write ";***********************************************************************"
write "; Step 2.4.2: Retrieve data for registered applications "
;      Expect for no-op evt msgs IDs the value 65535, bin fltr ctrs & masks = 0
;            cEVS3020
write ";***********************************************************************"
  
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_242.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
endif   

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
;     for apps
;
;; Check the No-op Command 
for i = 1 to cfe_apps do
  if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    if ($SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].EvtId = 65535) AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Msk = 0)       AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3020, "P"
      write "<*> Passed (3020) - EVS NOOP Filter parameters are correct"
    else
      ut_setrequirements cEVS3020, "F"
      write "<!> Failed (3020) - EVS NOOP Filter parameters are not correct"
      write ">>> Event ID = ",$SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].EvtId,"; Expected 65535"
      write ">>> Mask     = ",$SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Msk,"; Expected 0"
      write ">>> Counter  = ",$SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    if ($SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].EvtId = 65535) AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Msk = 0)       AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3020, "P"
      write "<*> Passed (3020) - SB NOOP Filter parameters are correct"
    else
      ut_setrequirements cEVS3020, "F"
      write "<!> Failed (3020) - SB NOOP Filter parameters are not correct"
      write ">>> Event ID = ",$SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].EvtId,"; Expected 65535"
      write ">>> Mask     = ",$SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Msk,"; Expected 0"
      write ">>> Counter  = ",$SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    if ($SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].EvtId = 65535) AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Msk = 0)       AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3020, "P"
      write "<*> Passed (3020) - ES NOOP Filter parameters are correct"
    else
      ut_setrequirements cEVS3020, "F"
      write "<!> Failed (3020) - ES NOOP Filter parameters are not correct"
      write ">>> Event ID = ",$SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].EvtId,"; Expected 65535"
      write ">>> Mask     = ",$SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Msk,"; Expected 0"
      write ">>> Counter  = ",$SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    if ($SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].EvtId = 65535) AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Msk = 0)       AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3020, "P"
      write "<*> Passed (3020) - TIME NOOP Filter parameters are correct"
    else
      ut_setrequirements cEVS3020, "F"
      write "<!> Failed (3020) - TIME NOOP Filter parameters are not correct"
      write ">>> Event ID = ",$SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].EvtId,"; Expected 65535"
      write ">>> Mask     = ",$SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Msk,"; Expected 0"
      write ">>> Counter  = ",$SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    if ($SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].EvtId = 65535) AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Msk = 0)       AND ;;
       ($SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3020, "P"
      write "<*> Passed (3020) - TBL NOOP Filter parameters are correct"
    else
      ut_setrequirements cEVS3020, "F"
      write "<!> Failed (3020) - TBL NOOP Filter parameters are not correct"
      write ">>> Event ID = ",$SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].EvtId,"; Expected 65535"
      write ">>> Mask     = ",$SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Msk,"; Expected 0"
      write ">>> Counter  = ",$SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr,"; Expected 0"
    endif
  endif
enddo
 
;-------------------------------------------------------------------------------
write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo
 
write ";***********************************************************************"
write "; Step 2.4.3: Send 5 No-op commands for each registered application "
;           Expect 5 evt msgs are generated per registered app
;           Expect Total Evt msg sent ctr to increment 5 x number of apps
;           cEVS3103
;           Upon verification of ctr increments set reqs cEVS3104 and
;           cEVS3105 to their correct values and cDELFltr (full ver)
write ";***********************************************************************"
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_NOOP_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_EVS_NOOP 
  wait 3                    
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 5

if ($SC_$CPU_find_event[1].num_found_messages = 5) then              
  ut_setrequirements cEVS3020, "P"
  lclevtmsgsentctr = lclevtmsgsentctr + 5
  EVSevtmsgsentctr = EVSevtmsgsentctr + 5

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3020;3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - EVS Message counts are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif  
else
  ut_setrequirements cEVS3020, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3020;3103) - Sent 5 EVS NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 5"
endif                        
 
write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;--------------------------------------------------------------------- 
ut_setupevents $sc, $cpu, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_SB_NOOP 
  wait 3                    
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 5

if ($SC_$CPU_find_event[1].num_found_messages = 5) then              
  ut_setrequirements cEVS3020, "P"
  lclevtmsgsentctr = lclevtmsgsentctr + 5
  SBevtmsgsentctr  = SBevtmsgsentctr  + 5

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3020;3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC = SBevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - SB Message counts are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - SB app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_sb_task_ndx].APPMSGSENTC,"; Expected ",SBevtmsgsentctr
  endif
else
  ut_setrequirements cEVS3020, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3020;3103) - Sent 5 SB NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 5"
endif  

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;------------------------------------------------------------------------------ 
ut_setupevents $sc, $cpu, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_ES_NOOP 
  wait 3                    
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 5
            
if ($SC_$CPU_find_event[1].num_found_messages = 5) then              
  ut_setrequirements cEVS3020, "P"
  lclevtmsgsentctr = lclevtmsgsentctr + 5
  ESevtmsgsentctr  = ESevtmsgsentctr  + 5

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3020;3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC = ESevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - ES Message counts are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - ES app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_es_task_ndx].APPMSGSENTC,"; Expected ",ESevtmsgsentctr
  endif
else
  ut_setrequirements cEVS3020, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3020;3103) - Sent 5 ES NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 5"
endif    

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_TIME_NOOP 
  wait 3                    
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 5
                     
if ($SC_$CPU_find_event[1].num_found_messages = 5) then              
  ut_setrequirements cEVS3020, "P"
  lclevtmsgsentctr = lclevtmsgsentctr + 5
  TIMEevtmsgsentctr = TIMEevtmsgsentctr + 5

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3020;3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC = TIMEevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - TIME Message counts are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - TIME app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC,"; Expected ",TIMEevtmsgsentctr
  endif
else
  ut_setrequirements cEVS3020, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3020;3103) - Sent 5 TIME NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 5"
endif                        

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
;-------------------------------------------------------------------------------
ut_setupevents $sc, $cpu, CFE_TBL, CFE_TBL_NOOP_INF_EID, INFO, 1

for i = 1 to 5 do
  /$SC_$CPU_TBL_NOOP 
  wait 3                    
enddo
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 5

if ($SC_$CPU_find_event[1].num_found_messages = 5) then              
  ut_setrequirements cEVS3020, "P"
  lclevtmsgsentctr = lclevtmsgsentctr + 5
  TBLevtmsgsentctr  = TBLevtmsgsentctr  + 5

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3020;3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC = TBLevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - TBL Message counts are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - TBL app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_tbl_task_ndx].APPMSGSENTC,"; Expected ",TBLevtmsgsentctr
  endif
else
  ut_setrequirements cEVS3020, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3020;3103) - Sent 5 TBL NOOP commands that generated ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 5"
endif   

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
 
for i = 1 to cfe_apps do
  write ">>> ",$SC_$CPU_EVS_AppData[i].AppName, " - evt msg sent ctr ",$SC_$CPU_EVS_APP[i].APPMSGSENTC
enddo
 
write ";***********************************************************************"
write "; Step 2.4.4: Retrieve applications data"
;                   Upon verification of apps binary filter ctr for no-op evt
;		    msg ctr = 0 set req cEVS3020 to correct value
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_244.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if (ut_tw_status = UT_TW_Success) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
endif   

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write " >>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;      for all registered apps 
;
for i = 1 to cfe_apps do
  if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_EVS") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - EVS NOOP Binary Filter counter set to 0"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - EVS NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[EVSNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - SB NOOP Binary Filter counter set to 0"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - SB NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[SBNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_ES") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - ES NOOP Binary Filter counter set to 0"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - ES NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[ESNoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - TIME NOOP Binary Filter counter set to 0"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - TIME NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[TIMENoopIdx].Ctr,"; Expected 0"
    endif
  elseif ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TBL") then
    if ($sc_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr = 0) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - TBL NOOP Binary Filter counter set to 0"
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - TBL NOOP Binary Filter counter set to ",$SC_$CPU_EVS_AppData[i].BinFltr[TBLNoopIdx].Ctr,"; Expected 0"
    endif
  endif
enddo
 
write ";***********************************************************************"
write "; Step 2.5: Attempt to add filter for events which are already "
write ";           registered via IPC. This tests cEVS3019_1."
write ";	   Expected outcome: Should receive an error message and error "
write ";           counter should increment."
write ";***********************************************************************"
; Case: Attempt to add a filter for CI App Noop Event (#5)
; Expected outcome: Error since event was already registered via IPC.

; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_EVT_FILTERED_EID, ERROR, 1

; Send command
ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CI_LAB_APP"" EVENT_ID=5 EVENT_MASK=x'bead'"

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3019_1, "P"
  write "<*> Passed (3019.1) - Add Event Filter command failed as expected."
else
  ut_setrequirements cEVS3019_1, "F"
  write "<!> Failed (3019.1) - Add Event Filter command passed when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
  ut_setrequirements cEVS3019_1, "P"
  write "<*> Passed (3019.1) - Expected Error Event Message received."
else
  ut_setrequirements cEVS3019_1, "F"
  write "<!> Failed (3019.1) - Expected Error Event Message ",CFE_EVS_EVT_FILTERED_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

; Case: Attempt to add a filter for SB Noop Event.
; Expected outcome: Expect event and its filter to be added since the event isn't registered in flight code.

; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 

; Send command
evtID = CFE_SB_CMD0_RCVD_EID
ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=evtID EVENT_MASK=x'abba'"

; Wait for event
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

; Check for acceptance of command
if (ut_sc_status = UT_SC_Success) then
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019.1) - Add Event Filter command."
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Passed (3019.1) - Add Event Filter command."
endif

; Update local 'Sent Message Count' values
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  ;; Test 'Sent Message Count' values 
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  if ($sc_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC = EVSevtmsgsentctr) then
    ut_setrequirements cEVS3104, "P"
    write "<*> Passed (3104) - EVS Message counts are correct"
  else 
    ut_setrequirements cEVS3104, "F"
    write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
  endif  
endif

; Case: Repeat command to add a filter for SB Noop Event.
; Expected outcome: Error since event has now been added to EVS.

; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_EVT_FILTERED_EID, ERROR, 1

; Send command
evtID = CFE_SB_CMD0_RCVD_EID
ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=evtID EVENT_MASK=x'daba'"

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3019_1, "P"
  write "<*> Passed (3019.1) - Add Event Filter command failed as expected."
else
  ut_setrequirements cEVS3019_1, "F"
  write "<!> Failed (3019.1) - Add Event Filter command passed when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3019_1, "P"
  write "<*> Passed (3019.1) - Expected Error Event Message received."
else
  ut_setrequirements cEVS3019_1, "F"
  write "<!> Failed (3019.1) - Expected Error Event Message ",CFE_EVS_EVT_FILTERED_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

write ";***********************************************************************"
write "; Step 2.6:  Attempt to register more events for SB task with EVS."
write ";	    Register maximum number and then one more. "
write ";	    This tests cEVS3019_2."
write ";	    Expected outcome: Should receive an error message and error"
write ";	    counter should increment when attempt is made to exceed max"
write ";	    number of events for task."
write ";***********************************************************************"
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_MAXREGSFILTER_EID, ERROR, 2

;; Start with EID=2 and increment until the error is generated
local eventToFltr = 2, eventMask = x'abba'

while ($SC_$CPU_find_event[2].num_found_messages = 0) do
  ; Set up to capture event
  ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
  ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_MAXREGSFILTER_EID, ERROR, 2

  ; Send Add Event Filter command
  ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=eventToFltr EVENT_MASK=eventMask"

  ; Wait for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

  ; Check if the success message was rcv'd
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
  endif

  ;; Setup for the next Event ID
  eventToFltr = eventToFltr + 1
enddo

;; If we get here, event message 2 was rcv'd
lclevtmsgsentctr = lclevtmsgsentctr + 1

; Test 'Sent Message Count' values 
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

;****************************
; Attempt to add one more
;****************************
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_MAXREGSFILTER_EID, ERROR, 1

; Send command
ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=55 EVENT_MASK=x'c1d2'"

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3019_2, "P"
  write "<*> Passed (3019.2) - Add Event Filter command failed as expected."
else
  ut_setrequirements cEVS3019_2, "F"
  write "<!> Failed (3019.2) - Add Event Filter command passed when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3019_2, "P"
  write "<*> Passed (3019.2) - Expected Error Event Message received."
else
  ut_setrequirements cEVS3019_2, "F"
  write "<!> Failed (3019.2) - Expected Error Event Message ",CFE_EVS_ERR_MAXREGSFILTER_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

; Test 'Sent Message Count' values 
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ";***********************************************************************"
write "; Step 2.7: Attempt to add event filter to CI_APP. This should fail "
write ";           since CI registers for 8 events in flight software."
write ";***********************************************************************"
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_MAXREGSFILTER_EID, ERROR, 1

; Send command
ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CI_LAB_APP"" EVENT_ID=23 EVENT_MASK=x'3d3d'"

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3019_2, "P"
  write "<*> Passed (3019.2) - Add Event Filter command failed as expected."
else
  ut_setrequirements cEVS3019_2, "F"
  write "<!> Failed (3019.2) - Add Event Filter command passed when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3019_2, "P"
  write "<*> Passed (3019.2) - Expected Error Event Message received."
else
  ut_setrequirements cEVS3019_2, "F"
  write "<!> Failed (3019.2) - Expected Error Event Message ",CFE_EVS_ERR_MAXREGSFILTER_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

; Test 'Sent Message Count' values 
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ";***********************************************************************"
write "; Step 2.7.1: Check Application Data File to confirm SB and CI tasks are"
write ";             registered for 8 events each."
; "Expect these values for CI task:"
;       Event Ids are 1..8
;       Event Masks are all 0x0000.
; "Expect these values for SB task:"
;      EVENT_ID=CFE_SB_CMD0_RCVD_EID EVENT_MASK=x'abba'
;      EVENT_ID=2 EVENT_MASK=x'abba'
;      EVENT_ID=3 EVENT_MASK=x'cafe'
;      EVENT_ID=4 EVENT_MASK=x'feed'
;      EVENT_ID=5 EVENT_MASK=x'789e'
;      EVENT_ID=6 EVENT_MASK=x'3d3d'
write ";***********************************************************************"

local goodfile = 0;

; Capture command execution counter
cmdexctr = $SC_$CPU_EVS_CMDPC + 1

; Write Application Data to File
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_271.dat", hostCPU)

; Check for correct completion
ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  goodfile = 1;
endif

; Test 'Sent Message Count' values 
ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct"
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

; Examine file data
if (goodfile = 1) then
  ; Identify indexes for CI and SB data
  for i = 1 to CFE_ES_MAX_APPLICATIONS do
    if ($SC_$CPU_EVS_AppData[i].AppName = "CI_LAB_APP") then
      cfe_ci_task_ndx = i
    endif
    if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_SB") then
      cfe_sb_task_ndx = i
    endif
  enddo

  ;*************************************
  ; Record and check filter data for CI
  ;*************************************
  local badMasks = ""
  if (cfe_ci_task_ndx <> 0) then
    write "CI_LAB_APP"
    for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      write "Evt Msg ID ",$SC_$CPU_EVS_AppData[cfe_ci_task_ndx].BinFltr[i].EvtId, ;;
      " ------ mask ", %hex($SC_$CPU_EVS_AppData[cfe_ci_task_ndx].BinFltr[i].Msk, 5),;;
      " ------------ ctr " ,$SC_$CPU_EVS_AppData[cfe_ci_task_ndx].BinFltr[i].Ctr
      if ($SC_$CPU_EVS_AppData[cfe_ci_task_ndx].BinFltr[i].Msk <> x'0000') then
        if (badMasks = "") then
          badMasks = i
	else
	  badMasks = badMasks & "," & i
        endif
      endif
    enddo
  endif

  ;***************************************
  ; Check filter mask values for CI task.
  ;***************************************
  if (cfe_ci_task_ndx <> 0) then
;;    for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
;;      if ($SC_$CPU_EVS_AppData[cfe_ci_task_ndx].BinFltr[i].Msk = x'0000') then
    if (badMasks = "") then
      ut_setrequirements cEVS3302, "P"
      write "<*> Passed (3302) - All CI Masks are correct"
    else
      ut_setrequirements cEVS3302, "F"
      write "<!> Failed (3302) - CI Masks that were not set to 0 are '",badMasks,"'"
    endif
;;    enddo
  endif

  ;**************************
  ; Record filter data for SB
  ;**************************
  local valid_evt_ID_noop_ndx = 0
  local valid_evt_ID_2_ndx = 0
  local valid_evt_ID_3_ndx = 0
  local valid_evt_ID_4_ndx = 0
  local valid_evt_ID_5_ndx = 0
  local valid_evt_ID_6_ndx = 0

  if (cfe_sb_task_ndx <> 0) then
    write "CFE_SB"
    for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
      write "Evt Msg ID ",$SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId, ;;
      " ------ mask ", %hex($SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].Msk, 5),;;
      " ------------ ctr " ,$SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].Ctr

      if ($sc_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId = CFE_SB_CMD0_RCVD_EID) then
        valid_evt_ID_noop_ndx = i
      elseif ($sc_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId = 2) then
        valid_evt_ID_2_ndx = i
      elseif ($sc_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId = 3) then
        valid_evt_ID_3_ndx = i
      elseif ($sc_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId = 4) then
        valid_evt_ID_4_ndx = i
      elseif ($sc_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId = 5) then
        valid_evt_ID_5_ndx = i
      elseif ($sc_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[i].EvtId = 6) then
        valid_evt_ID_6_ndx = i
      endif
    enddo
  endif

  ;***************************************
  ; Check filter mask values for SB task.
  ;***************************************
  if (cfe_sb_task_ndx <> 0) then
    ; Check mask value for SB NOOP event
    if (valid_evt_ID_noop_ndx <> 0) then
      if ($SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_noop_ndx].Msk = x'abba') then
        ut_setrequirements cEVS3302, "P"
        write "<*> Passed (3302) - SB NOOP Mask is correct"
      else
        ut_setrequirements cEVS3302, "F"
        write "<!> Failed (3302) - SB NOOP Mask = ",$SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_noop_ndx].Msk,"; Expected x'abba'"
      endif
    else
      ut_setrequirements cEVS3302, "F"
      write "<!> Failed (3302) - Binary Filter data not found for SB NOOP"
    endif

    ; Check mask value for SB event #2
    if (valid_evt_ID_2_ndx <> 0) then
      if ($SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_2_ndx].Msk = x'abba') then
        ut_setrequirements cEVS3302, "P"
        write "<*> Passed (3302) - SB Mask for EID 2 is correct"
      else
        ut_setrequirements cEVS3302, "F"
        write "<!> Failed (3302) - SB EID=2 Mask = ",$SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_2_ndx].Msk,"; Expected x'abba'"
      endif
    else
      ut_setrequirements cEVS3302, "F"
      write "<!> Failed (3302) - Binary Filter data not found for SB EID=2"
    endif

    ; Check mask value for SB event #3
    if (valid_evt_ID_3_ndx <> 0) then
      if ($SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_3_ndx].Msk = x'abba') then
        ut_setrequirements cEVS3302, "P"
        write "<*> Passed (3302) - SB Mask for EID 3 is correct"
      else
        ut_setrequirements cEVS3302, "F"
        write "<!> Failed (3302) - SB EID=3 Mask = ",$SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_3_ndx].Msk,"; Expected x'abba'"
      endif
    else
      ut_setrequirements cEVS3302, "F"
      write "<!> Failed (3302) - Binary Filter data not found for SB EID=3"
    endif

    ; Check mask value for SB event #4
    if (valid_evt_ID_4_ndx <> 0) then
      if ($SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_4_ndx].Msk = x'abba') then
        ut_setrequirements cEVS3302, "P"
        write "<*> Passed (3302) - SB Mask for EID 4 is correct"
      else
        ut_setrequirements cEVS3302, "F"
        write "<!> Failed (3302) - SB EID=4 Mask = ",$SC_$CPU_EVS_AppData[cfe_sb_task_ndx].BinFltr[valid_evt_ID_4_ndx].Msk,"; Expected x'abba'"
      endif
    else
      ut_setrequirements cEVS3302, "F"
      write "<!> Failed (3302) - Binary Filter data not found for SB EID=4"
    endif
  endif ; Found SB index
endif ; Check for file write

write ";***********************************************************************"
write "; Step 2.8: Test Delete Event Filter Command"
write ";***********************************************************************"
;****************************************************
; Delete one of the Event Filters which was added.
; This command should execute successfully.
;****************************************************
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1 

; Send command
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=2"

; Wait for event
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

; Check for acceptance of command
if (ut_sc_status = UT_SC_Success) then
  ut_setrequirements cEVS3020, "P"
  write "<*> Passed (3020) - Delete Event Filter for SB EID=2 successful."
else
  ut_setrequirements cEVS3020, "F"
  write "<!> Failed (3020) - Delete Event Filter for SB EID=2 rejected."
endif

; Update local 'Sent Message Count' values
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct"
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif
endif

;*******************************************************************
; Repeat previous command.
; This time expect error since event filter should already be gone.
; Expect command to be rejected and event message to be issued.
;*******************************************************************
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_EVTIDNOREGS_EID, ERROR, 1

; Send command
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=2"

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3020_1, "P"
  write "<*> Passed (3020.1) - Delete Event Filter for SB EID=2 failed as expected."
else
  ut_setrequirements cEVS3020_1, "F"
  write "<!> Failed (3020.1) - Delete Event Filter for SB EID=2 successful when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3020_1, "P"
  write "<*> Passed (3020.1) - Expected Event message received."
else
  ut_setrequirements cEVS3020_1, "F"
  write "<!> Failed (3020.1) - Expected Error Event Message ",CFE_EVS_ERR_EVTIDNOREGS_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

;********************************************************************
; Attempt to delete event filter #42 which was never added to CFE_SB.
; Expect command to be rejected and error event message.
;********************************************************************
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_EVTIDNOREGS_EID, ERROR, 1

; Send command
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CFE_SB"" EVENT_ID=42" 

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3020_1, "P"
  write "<*> Passed (3020.1) - Delete Event Filter for SB EID=42 failed as expected."
else
  ut_setrequirements cEVS3020_1, "F"
  write "<!> Failed (3020.1) - Delete Event Filter for SB EID=42 successful when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3020_1, "P"
  write "<*> Passed (3020.1) - Expected Event message received."
else
  ut_setrequirements cEVS3020_1, "F"
  write "<!> Failed (3020.1) - Expected Error Event Message ",CFE_EVS_ERR_EVTIDNOREGS_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

;*******************************************************************************
; Attempt to delete event filter #31 from CI_APP.
; CI_APP's event filters were registered by API, and did not include #31.
; Expect command to be rejected and error event message.
;*******************************************************************************
; Set up to capture event
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_EVTIDNOREGS_EID, ERROR, 1

; Send command
ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""CI_LAB_APP"" EVENT_ID=31"

; Check for rejection of command
if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3020_1, "P"
  write "<*> Passed (3020.1) - Delete Event Filter for SB EID=31 failed as expected."
else
  ut_setrequirements cEVS3020_1, "F"
  write "<!> Failed (3020.1) - Delete Event Filter for SB EID=31 successful when failure was expected."
endif

; Check that event message was generated.
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

if (ut_tw_status = UT_TW_Success) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3020_1, "P"
  write "<*> Passed (3020.1) - Expected Event message received."
else
  ut_setrequirements cEVS3020_1, "F"
  write "<!> Failed (3020.1) - Expected Error Event Message ",CFE_EVS_ERR_EVTIDNOREGS_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
endif

write ";***********************************************************************"
write "; Step 2.9: Restore filter mask for SB NOOP event to 0."
; This is necessary because Step 5.2 assumes the filter mask is 0.
write ";***********************************************************************"
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG, 1

evtID = CFE_SB_CMD0_RCVD_EID
ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_SB"" EVENT_ID=evtID FILTERMASK=X'0000'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    ut_setrequirements cEVS3012, "P"
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1

    if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
      ut_setrequirements cEVS3105, "P"
      write "<*> Passed (3012;3105) - Counters are correct."
    else 
      ut_setrequirements cEVS3105, "F"
      write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
      lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
    endif
  else
    ut_setrequirements cEVS3012, "F"
    write "<!> Failed (3012) - Expected Error Event Message ",CFE_EVS_SETFILTERMSK_EID," was not received. Expected 1 message but Received ", $SC_$CPU_find_event[1].num_found_messages
  endif
else
  ut_setrequirements cEVS3012, "F"
  write "<!> Failed (3012) - Set Binary Filter Mask command"
endif

write ">>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC
write "TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC 
   
write ";***********************************************************************"
write "; Step 3.0: Register EVS test application to further test binary filter "
write ";	   processing. "
write ";***********************************************************************"
write "; Step 3.1: Load EVS Test App"
;                 Expect test app ID added to telemetry and 
;                 Test Application initialization event messages from CFE_ES and
;		  TST_EVS.
;                 Loading the test app will result in 2 INFO messages and 2 
;		  DEBUG messages.
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 4 

ut_setupevents $sc, $cpu, TST_EVS, TST_EVS_INIT_INF_EID, INFO, 1
;
start load_start_app ("TST_EVS", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}
;
;      verify registration of test app
;      assign values to local ctrs
;      NOTE: req cEVS3100 is assigned a value "A" here,
;      it shall be assigned the value "P" upon verification of 
;      changes to masks and binary filter counters

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  ut_setrequirements cEVS3100, "A"
  apps_registered = apps_registered + 1
  TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 1
else
  ut_setrequirements cEVS3100, "F"
  write "<!> Failed (3100) - TST_EVS application failed to start" 
  write">>> ALERT >>> ALERT >>> ALERT >>> ALERT >>> ALERT >>> ALERT >>> ALERT"
  write"Terminating this test because the test application failed to start"
  goto end_it
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC,"; Expected ",EVSevtmsgsentctr
endif  

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write ">>> Total Number of apps registered after test app = ", apps_registered 
write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write ">>> Test app event msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

write ";***********************************************************************"
write "; Step 3.1.1: Retrieve application data"
;                    Expect Test application information
;                    Upon verification of test app info set req cEVS3100 
;                    Upon verification of ctr increments set reqs cEVS3104 and
;                    cEVS3105 to their correct values
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                     
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_311.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if ($sc_$CPU_EVS_APP[apps_registered].APPENASTAT) then
  ut_setrequirements cEVS3100, "P"
  write "<*> Passed (3100) - TST_EVS Event Message Generation is enabled."
else 
  ut_setrequirements cEVS3100, "F"
  write "<!> Failed (3100) - TST_EVS Event Message Generation is not enabled."
endif

if ($SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Debug = 0) AND ;;
   ($SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Info = 1) AND ;;
   ($SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Err = 1) AND ;;
   ($SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Crit = 1) then 
  ut_setrequirements cEVS3100, "P"
  write "<*> Passed (3100) - TST_EVS app Event Types enabled properly."
else
  ut_setrequirements cEVS3100, "F"
  write "<!> Failed (3100) - TST_EVS app Event Types are not enabled properly."
  write "==> Debug = ",$SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Debug,"; Expected 0"
  write "==> Info  = ",$SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Info,"; Expected 1"
  write "==> Error = ",$SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Err,"; Expected 1"
  write "==> Crit  = ",$SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Crit,"; Expected 1"
endif   

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write ">>> Name of app added is ",$SC_$CPU_EVS_AppData[apps_registered].AppName
write ">>> Event Message Generation Status = ",$SC_$CPU_EVS_APP[apps_registered].APPENASTAT
write " >>> DEBUG evt msg type status is ",p@$SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Debug

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtId >= 0) AND ;;
     ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Msk >= 0) then
    ut_setrequirements cEVS3100, "P"
    write "<*> Passed (3100) - TST_EVS Binary Filter ",j," set properly."
  else 
    ut_setrequirements cEVS3100, "F"
    write "<!> Failed (3100) - TST_EVS Binary Filter ",j," not set properly. Expectd EvtID and Mask >= 0"
  endif

  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr >= 0) then
    ut_setrequirements cEVS3100_1, "P"
    write "<*> Passed (3100.1) - TST_EVS Binary Filter Counter ",j," set properly."
  else 
    ut_setrequirements cEVS3100_1, "F"
    write "<!> Failed (3100) - TST_EVS Binary Filter Counter ",j," not set properly. Expectd >= 0"
  endif
enddo

for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  write "Evt Msg ID ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtId, ;;
        " ------ mask ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].Msk, 5),;;
        " ------------ ctr " ,$SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr
enddo

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
;
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 3.2: Set DEBUG Event Msg Type Status ENABLE"
write ";***********************************************************************"
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ENAAPPEVTTYPE_EID, DEBUG, 1
 
ut_sendcmd "$SC_$CPU_EVS_ENAAPPEVTTYPE APPLICATION=""TST_EVS"" DEBUG"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if (ut_sc_status = UT_SC_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
  endif

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
 
write ";***********************************************************************"
write "; Step 3.2.1: Retrieve applications data "
;                    Expect DEBUG evt msg type status ENABLE for test App
;                    cEVS3203c
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                      
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_321.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write ">>> For test application ",$SC_$CPU_EVS_AppData[apps_registered].AppName 
write ">>> DEBUG evt msg type status is ",  p@$SC_$CPU_EVS_AppData[apps_registered].EvtTypeAF.Debug  
write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write ";***********************************************************************"
write "; Step 3.3: Request generation of 20 iters of every event msg "
write ";           registered for filtering within Test Application "
;
;                  Expect generation of evt msgs as follows:
;
;                  ID     Mask      # msgs   generation
;                  1        0      20       All 20 sequentially
;                  2  x'ffff'       1       First one only
;                  3        1      10       Every other one   
;                  4  x'fff8'       8       First eight only
;                  5        2      10       Every other 2
;                  17       0      20       All 20 sequentially
;
;                  Expect increment to app evt msg sent ctr by 69
;                  Expect increment to total evt msg sent ctr by 69  
;                  Upon respective verification set 
;                  cEVS3104
;                  cEVS3105
write ";***********************************************************************"
 
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 20

ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="1" Iters="20" MILLISECONDS="250"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 20) then
  lclevtmsgsentctr = lclevtmsgsentctr + 20
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - 20 Event messages rcvd."
else                     
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Did not rcv all expected EID=1 messages. Expected 20; Rcvd ",$SC_$CPU_find_event[1].num_found_messages
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

write ">>> test app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write "TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
 
;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 1

ut_setupevents $sc, $cpu, TST_EVS, 2, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="2" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - 1 Event messages rcvd."
else                     
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Did not rcv the expected EID=2 messages. Expected 1; Rcvd ",$SC_$CPU_find_event[1].num_found_messages
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

write " >>> app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC          

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
endif

write "TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 10

ut_setupevents $sc, $cpu, TST_EVS, 3, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="3" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - 10 Event messages rcvd."
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Did not rcv the expected EID=3 messages. Expected 10; Rcvd ",$SC_$CPU_find_event[1].num_found_messages
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

write ">>> test app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC     

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write "TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
 
;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 8

ut_setupevents $sc, $cpu, TST_EVS, 4, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="4" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 8) then
  lclevtmsgsentctr = lclevtmsgsentctr + 8
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - 8 Event messages rcvd."
else                     
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Did not rcv the expected EID=4 messages. Expected 8; Rcvd ",$SC_$CPU_find_event[1].num_found_messages
endif
     
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

write ">>> test app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC  

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
                          
write "TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 10

ut_setupevents $sc, $cpu, TST_EVS, 5, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="5" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - 10 Event messages rcvd."
else                     
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Did not rcv the expected EID=5 messages. Expected 8; Rcvd ",$SC_$CPU_find_event[1].num_found_messages
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

write ">>> test app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
                          
write "TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 20

ut_setupevents $sc, $cpu, TST_EVS, 17, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="17" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 20) then
  lclevtmsgsentctr = lclevtmsgsentctr + 20
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - 20 Event messages rcvd."
else                     
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Did not rcv the expected EID=5 messages. Expected 8; Rcvd ",$SC_$CPU_find_event[1].num_found_messages
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

write ">>> test app evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC  

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
                          
write "TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.3.1: Retrieve applications data "
;                    Expect increment by 20 of each test app bin fltr ctr for
;		     those evts registered for filtering
;                    Upon verification of bin fltr ctr/evt msg, set cEVS3103_3
;		     to its correct value
;
;      NOTE: Correct evt msg gen pattern to be verified through analysis of 
;	     ASIST log.
write ";***********************************************************************"

lclevtmsgsentctr = lclevtmsgsentctr + 1
                     
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_331.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

local eventID = 65535
for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  eventID = $sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtID
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtID <> 65535) then
    if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].Ctr = 20) then
      ut_setrequirements cEVS3103, "P"
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103;3103.3) - Filter ID ",eventID," counter is correct."
    else 
      ut_setrequirements cEVS3103, "F"
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103;3103.3) - Filter ID ",eventID," counter expected = 20. Actual count = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr
    endif
  endif
enddo

;;for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do
;;  write " >>> Bin Fltr ID = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtID , " Bin Fltr Ctr  = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr
;;enddo

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 3.3.2: Reset binary filter counters for test application"
;                    Upon execution and verification of command, set
;                    cEVS3011 accordingly
write ";***********************************************************************"
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTALLFILTER_EID, DEBUG, 1

cmdexctr = $SC_$CPU_EVS_CMDPC + 1
/$SC_$CPU_EVS_RSTALLFLTRS APPLICATION="TST_EVS"
wait 5

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 
if (ut_tw_status = UT_TW_Success) then
  ut_setrequirements cEVS3011, "P"
  write "<*> Passed (3011) - TST_EVS Reset All Filters command send successfully."
else
  ut_setrequirements cEVS3011, "F"
  write "<!> Failed (3011) - TST_EVS Reset All Filters command did not increment CMDPC."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3011, "P"
  write "<*> Passed (3011) - Event message received."
else
  ut_setrequirements cEVS3011, "F"
  write "<!> Failed (3011) - Did not rcv expected event message. Exepected 1; Rcv'd ",$SC_$CPU_find_event[1].num_found_messages
endif 
   
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.3.2.1: Retrieve applications data" 
;                     Verify that all test app bin fltr ctrs are set to zero
;                     per req cEVS3011
write ";***********************************************************************"
;                                          
lclevtmsgsentctr = lclevtmsgsentctr + 1

start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_3321.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$cpu_EVS_AppData[apps_registered].BinFltr[i].Ctr = 0) then
    ut_setrequirements cEVS3011, "P"
    write "<*> Passed (3011) - Filter ",i," was Reset."
  else
    ut_setrequirements cEVS3011, "F"
    write "<!> Failed (3011) - Filter ",i," was not Reset. Expected msg count = 0. It is set to ", $sc_$cpu_EVS_AppData[apps_registered].BinFltr[i].Ctr
  endif 
enddo

;; Display all the filter params
for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  write ">>> ",  $sc_$cpu_EVS_AppData[apps_registered].AppName, " -  Evt ID ", $sc_$cpu_EVS_AppData[apps_registered].BinFltr[i].EvtId, " - Mask - ", %hex($sc_$cpu_EVS_AppData[apps_registered].BinFltr[i].Msk, 5), " App Bin Fltr Ctr ", $sc_$cpu_EVS_AppData[apps_registered].BinFltr[i].Ctr 
enddo 

write ";***********************************************************************"
write "; Step 3.4: Change 6 event messages registered for filtering within test"
write ";           app by deleting all registered evts and then adding 6 evt "
write ";           IDs with with specific masks"
write ";***********************************************************************"
write "; Step 3.4.1: Delete binary filters for evts 1 to 5 and 17" 
;                    cEVS3020 (Initial ver)
write ";***********************************************************************"
local eventIDs[6] = [1,2,3,4,5,17]

for i = 1 to 6 do
  cmdexctr =$SC_$CPU_EVS_CMDPC + 1
  ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_DELFILTER_EID, "DEBUG", 1

  /$SC_$CPU_EVS_DELEVTFLTR APPLICATION="TST_EVS" EVENT_ID=eventIDs[i]

  ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 
  if (ut_tw_status = UT_TW_Success) then
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - Delete Event Filter command successful."
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    lclevtmsgsentctr = lclevtmsgsentctr + 1
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - Delete Event Filter command did not increment CMDPC."
  endif

  ;; Check for the event message
  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
  if (ut_tw_status = UT_TW_Success) then
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - Expected event message rcv'd."
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - Expected event message was not rcv'd"
  endif

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  write " >>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
enddo

;-------------------------------------------------------------------------------
;;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID = 2"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    ut_setrequirements cEVS3020, "P"
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    write "<*> Passed (3020) - Delete Event Filter command successful."
;  else 
;    ut_setrequirements cEVS3020, "F"
;    write "<!> Failed (3020) - Delete Event Filter command did not generate the expected event message."
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;    write "<*> Passed (3105) - Counters are correct."
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;else
;  ut_setrequirements cEVS3020, "F"
;  write "<!> Failed (3020) - Delete Event Filter command did not increment CMDPC."
;endif
;
;write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;
;-------------------------------------------------------------------------------
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1 
;   
;ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID = 3"
; 
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
; 
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    ut_setrequirements cEVS3020, "P"
;    write "<*> Passed (3020) - Delete Event Filter command successful."
;  else 
;    ut_setrequirements cEVS3020, "F"
;    write "<!> Failed (3020) - Delete Event Filter command did not generate the expected event message."
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;    write "<*> Passed (3105) - Counters are correct."
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;else
;  ut_setrequirements cEVS3020, "F"
;  write "<!> Failed (3020) - Delete Event Filter command did not increment CMDPC."
;endif
; 
;write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;
;-------------------------------------------------------------------------------
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID = 4"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3020, "P"
;    write "<*> Passed (3020) - Delete Event Filter command successful."
;  else 
;    ut_setrequirements cEVS3020, "F"
;    write "<!> Failed (3020) - Delete Event Filter command did not generate the expected event message."
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;    write "<*> Passed (3105) - Counters are correct."
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;else
;  ut_setrequirements cEVS3020, "F"
;  write "<!> Failed (3020) - Delete Event Filter command did not increment CMDPC."
;endif
;
;write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
write " >>> EVS evt msg sent ctr = ",$SC_$CPU_EVS_APP[cfe_evs_task_ndx].APPMSGSENTC

;-------------------------------------------------------------------------------
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID = 5" 
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    ut_setrequirements cEVS3020, "P"
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    write "<*> Passed (3020) - Delete Event Filter command successful."
;  else 
;    ut_setrequirements cEVS3020, "F"
;    write "<!> Failed (3020) - Delete Event Filter command did not generate the expected event message."
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;    write "<*> Passed (3105) - Counters are correct."
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;else
;  ut_setrequirements cEVS3020, "F"
;  write "<!> Failed (3020) - Delete Event Filter command did not increment CMDPC."
;endif
; 
;write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID = 17" 
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3020, "P"
;    write "<*> Passed (3020) - Delete Event Filter command successful."
;  else 
;    ut_setrequirements cEVS3020, "F"
;    write "<!> Failed (3020) - Delete Event Filter command did not generate the expected event message."
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;    write "<*> Passed (3105) - Counters are correct."
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;else
;  ut_setrequirements cEVS3020, "F"
;  write "<!> Failed (3020) - Delete Event Filter command did not increment CMDPC."
;endif
;
;write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.4.1.1: Retrieve applications data" 
;                Expect for test app No event messages registered for filtering
;                cEVS3020 (full ver)
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                     
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_3411.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId = 65535) then
    ut_setrequirements cEVS3020, "P"
    write "<*> Passed (3020) - Filter ", i," has been deleted."
  else 
    ut_setrequirements cEVS3020, "F"
    write "<!> Failed (3020) - Filter ", i," was NOT deleted "
  endif
enddo
 
for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  write ">>> ", $SC_$CPU_EVS_AppData[apps_registered].AppName, " -  Evt ID ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId
enddo

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 3.4.2: Add binary filters for 6 evt msgs" 
;                    cEVS3019 (init ver)
;
;                    Evt Id      Mask
;                    -------------------
;                    1           2
;                    2           x'ffff'
;                    3           1
;                    4           x'fff8'
;                    5           2
;                    8           0
write ";***********************************************************************"
eventIDs[6] = 8
local eventMasks[6] = [2,x'ffff',1,x'fff8',2,0]
 
for i = 1 to 6 do
  cmdexctr =$SC_$CPU_EVS_CMDPC + 1
  ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ADDFILTER_EID, "DEBUG", 1

  /$SC_$CPU_EVS_ADDEVTFLTR APPLICATION="TST_EVS" EVENT_ID=eventIDs[i] EVENT_MASK=eventMasks[i]

  ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 
  if (ut_tw_status = UT_TW_Success) then
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
    ut_setrequirements cEVS3019, "P"
    write "<*> Passed (3019) - Add Event Filter command sent successfully for ID= 1"
  else 
    ut_setrequirements cEVS3019, "F"
    write "<!> Failed (3019) - Add Event Filter command for ID=",eventIDs[i]," did not increment CMDPC"
  endif

  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
  if (ut_tw_status = UT_TW_Success) then
    ut_setrequirements cEVS3019, "P"
    write "<*> Passed (3019) - Expected event message rcv'd."
  else 
    ut_setrequirements cEVS3019, "F"
    write "<!> Failed (3019) - Expected event message was not rcv'd for ID=",eventIDs[i]
  endif

  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct."
  else 
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif

  write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
enddo

;-------------------------------------------------------------------------------
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=2 EVENT_MASK = x'ffff'"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3019, "P"
;  else 
;    ut_setrequirements cEVS3019, "F"
;    write " >>> ALERT: cEVSADDFLtr failed at send cmd to add filter 2" 
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write " >>> ALERT: cEVS3105 failed at total evt msg sent ctr check"
;    write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;endif
;
;write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
;
;-------------------------------------------------------------------------------
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=3 EVENT_MASK = 1"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3019, "P"
;  else 
;    ut_setrequirements cEVS3019, "F"
;    write " >>> ALERT: cEVSADDFLtr failed at send cmd to add filter 3" 
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write " >>> ALERT: cEVS3105 failed at total evt msg sent ctr check"
;    write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;endif
;
;write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
;
; ------------------------------------------------------------------------------
; 
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=4 EVENT_MASK = x'fff8'"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
; 
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3019, "P"
;  else 
;    ut_setrequirements cEVS3019, "F"
;    write " >>> ALERT: cEVSADDFLtr failed at send cmd to add filter 4" 
;  endif
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write " >>> ALERT: cEVS3105 failed at total evt msg sent ctr check"
;    write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;endif
;
;write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
;
;-------------------------------------------------------------------------------
; 
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=5 EVENT_MASK = 2"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3019, "P"
;  else 
;    ut_setrequirements cEVS3019, "F"
;    write " >>> ALERT: cEVSADDFLtr failed at send cmd to add filter 5" 
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write " >>> ALERT: cEVS3105 failed at total evt msg sent ctr check"
;    write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;endif
;
;write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
;
;-------------------------------------------------------------------------------
; 
;ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1 
;
;ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=8 EVENT_MASK = 0"
;
;ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
;
;if (ut_sc_status = UT_SC_Success) then
;  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
;    lclevtmsgsentctr = lclevtmsgsentctr + 1
;    write " TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    EVSevtmsgsentctr = EVSevtmsgsentctr + 1
;    ut_setrequirements cEVS3019, "P"
;  else 
;    ut_setrequirements cEVS3019, "F"
;    write " >>> ALERT: cEVSADDFLtr failed at send cmd to add filter 8" 
;  endif
;
;  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
;    ut_setrequirements cEVS3105, "P"
;  else 
;    ut_setrequirements cEVS3105, "F"
;    write " >>> ALERT: cEVS3105 failed at total evt msg sent ctr check"
;    write " Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
;  endif
;endif
;
;write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
;
write ";***********************************************************************"
write "; Step 3.4.3: Retrieve test application data"
;           Upon verification of added filter info based on the following:
;
;                    Evt Id      Mask      B F Ctr
;                    -----------------------------  
;                    1           2           0
;                    2           x'ffff'     0
;                    3           1           0
;                    4           x'fff8'     0
;                    5           2           0
;                    8           0           0
;
;                    set cEVS3019 to its correct value (full ver)
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_343.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
local evtID4ndx = 0       ; for later processing of evt msg ID 4

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

; for added filter 1, check its bin fltr info
for j = 1 to 6 do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtId = eventIDs[j]) then
    if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Msk = eventMasks[j]) then
      if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr = 0) then
        ut_setrequirements cEVS3019, "P"
        write "<*> Passed (3019) - Event Filter[",j,"] is correct."
      else
        ut_setrequirements cEVS3019, "F"
        write "<!> Failed (3019) - Event Filter[",j,"] Ctr = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr,"; Expected 0"
      endif
    else
      write "<!> Failed (3019) - Event Filter[",j,"] Mask = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Msk,"; Expected ",eventMasks[j]
    endif
  else
    write "<!> Failed - Event Filter[",j,"] ID = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtId,"; Expected ",eventIDs[j]
  endif                 
enddo

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 3.5: Request generation of every event msg registered for "
write ";           filtering within Test Application, 20 iterations of each."
;                  Expect Every event message generated according to its 
;		   event mask
;                  Evt Id      Mask      B F Ctr    # generated
;                  --------------------------------------------  
;                  1           2           0            10
;                  2           x'ffff'     0             1
;                  3           1           0            10
;                  4           x'fff8'     0             8
;                  5           2           0            10
;                  8           0           0            20
;
;                  and increments the total evt msg sent ctr and app evt msg
;		   sent ctr
;                  Set cEVS3104 and cEVS3105 to their corresponding values
write ";***********************************************************************"
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 10

ut_setupevents $sc, $cpu, TST_EVS, 1, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="1" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
endif

if ($sc_$CPU_EVS_App[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
    
;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 1

ut_setupevents $sc, $cpu, TST_EVS, 2, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="2" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  lclevtmsgsentctr = lclevtmsgsentctr + 1
endif

if ($sc_$CPU_EVS_App[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 10

ut_setupevents $sc, $cpu, TST_EVS, 3, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="3" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
endif

if ($sc_$CPU_EVS_App[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 8

ut_setupevents $sc, $cpu, TST_EVS, 4, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="4" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 8) then
  lclevtmsgsentctr = lclevtmsgsentctr + 8
endif

if ($sc_$CPU_EVS_App[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 10

ut_setupevents $sc, $cpu, TST_EVS, 5, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="5" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 10) then
  lclevtmsgsentctr = lclevtmsgsentctr + 10
endif

if ($sc_$CPU_EVS_App[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

;-------------------------------------------------------------------------------
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 20

ut_setupevents $sc, $cpu, TST_EVS, 8, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="8" Iters="20" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, {TSTEVSevtmsgsentctr}

if ($SC_$CPU_find_event[1].num_found_messages = 20) then
  lclevtmsgsentctr = lclevtmsgsentctr + 20
endif

if ($sc_$CPU_EVS_App[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif
 
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.5.1: Retrieve applications data"
;                    Expect for every app evt msg registered for filtering 
;		     increment by 20/Evt ID
;                    Upon verification of evt bin flt ctr increment/reg evt set
;		     correct value for req cEVS3103_3
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_351.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

for j = 1 to CFE_EVS_MAX_EVENT_FILTERS do 
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtId <> 65535) then
    if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr = 20) then
      ut_setrequirements cEVS3103_3, "P"
      write "<*> Passed (3103.3) - Filter[",j,"] counter is correct."
    else 
      ut_setrequirements cEVS3103_3, "F"
      write "<!> Failed (3103.3) - Filter[",j,"] counter = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[j].Ctr,"; Expected 20."
    endif

    if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[j].EvtId = 4) then
      evtID4ndx = j
    endif
  endif
enddo

write " >>> ",$SC_$CPU_EVS_AppData[apps_registered].AppName
for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  write " >>>           Evt ID = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId
  write " >>>           Mask   = ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].Msk, 5)
  write " >>>     Bin Fltr Ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].Ctr 
enddo 

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"

for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 3.6: Request via Test app the reset of app bin fltr ctr for "
write "            evt msg ID 4 with mask = x'FFF8'"
;
;                  cEVS3106 (initial ver) 
write ";***********************************************************************"
cmdexctr =$SC_$CPU_EVS_CMDPC + 1

ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG, 1

/$SC_$CPU_TST_EVS_RESETFILTER RAPPNAME="TST_EVS" REVENTID=X'4' 
                  
ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

if ($sc_$CPU_EVS_CMDPC = cmdexctr) then
  ut_setrequirements cEVS3106, "P"
  write "<*> Passed (3106) - Reset Filter command sent successfully."
else
  ut_setrequirements cEVS3106, "F"
  write "<!> Failed (3106) - Reset Filter command did not increment CMDPC."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then 
  lclevtmsgsentctr = lclevtmsgsentctr + 2
  TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 1
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif  
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.6.1: Retrieve applications data "
;                    Expect test app bin fltr ctr for evt ID 4 = 0
;                    Upon verification of reset for bin fltr ctr set
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                    
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_361.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}
    
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;===============================================================================

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr = 0) then
  ut_setrequirements cEVS3106, "P"
  write "<*> Passed (3106) - Event ID 4 Filter Counter has been reset."
else
  ut_setrequirements cEVS3106, "F"
  write "<!> Failed (3106) - Event ID 4 Filter Counter = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr,"; Expected 0"
endif

write " > > > > > > > > > > > > > > > > > > > > > > > > > >"
write ">>> ", $SC_$CPU_EVS_AppData[apps_registered].AppName
write ">>>  Evt ID ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].EvtId
write ">>>  Mask   ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Msk, 4)
write ">>>  App Bin Fltr Ctr ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr 
write "< < < < < < < < < < < < < < < < < < < < < < < < < < "

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 3.7: Request generation of 20 iterations of event message ID 4"
;                  Expect Evt Msg generated 8 times only according to its mask
;                  cEVS3103 set to "A" because evaluation of evt msg gen pattern
;                  must be done post-process
;                  cEVS3104 and cEVS3105 
write ";***********************************************************************"
TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 8

ut_setupevents $sc, $cpu, TST_EVS, 4, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="4" Iters="20" MILLISECONDS="150"

; Need to allow a minimum of 3 seconds for all 20 event requests to be issued.
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 8) then
  lclevtmsgsentctr = lclevtmsgsentctr + 8
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - Expected number of events rcvd."
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Rcvd ",$SC_$CPU_find_event[1].num_found_messages,"; Expected 8."
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif  

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.7.1: Retrieve applications data "
;                    Expect app bin filter ctr for evt ID 4 equal to 20, upon 
;                    bin fltr ctr verification set
;                    cEVS3103_3 to pass
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_371.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr = 20) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Bin Filter counter for ID=4 correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - Bin Filter counter for ID=4 = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr,"; Expected 20"
endif                       
                    
write ";***********************************************************************"
write "; Step 3.8: Request generation of event message 4 for 20 iterations."
;           Expect No Evt Msg generated because the fltr algorithm depends on 
;           the value of the app bin fltr ctr for the specified evt ID in
;           in combination with the mask to cause the gen of evt msgs and at
; 	    this point the bin fltr ctr = 20
;                   cEVS3103 
;                   cEVS3104 and cEVS3105 
write ";***********************************************************************"
ut_setupevents $sc, $cpu, TST_EVS, 4, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="4" Iters="20" MILLISECONDS="150"
wait 24
                    
if ($SC_$CPU_find_event[1].num_found_messages <> 0) then
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - ",$SC_$CPU_find_event[1].num_found_messages," event messages were generated when none were expected."
else                    
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - No event messages were generated as expected."
endif
   
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Counters are correct."
else
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
  TSTEVSevtmsgsentctr = $SC_$CPU_EVS_App[apps_registered].APPMSGSENTC
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
                     
write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 3.8.1: Retrieve applications data "
;                    Expect app bin filter ctr for evt ID 4 equal to 40, upon 
;                    bin fltr ctr verification set                     
;                    cEVS3103_3 to pass
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_381.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr = 40) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Filter counter is correct."
else 
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - Filter counter = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr,"; Expected 40."
endif
           
write ">>> Evt ID is  ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].EvtId
write ">>> bin fltr ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID4ndx].Ctr 

write ";***********************************************************************"
write "; Step 4.0: Add filter for event message ID 12 which is not registered "
write ";           for filtering"
write ";***********************************************************************"
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=12 EVENT_MASK=X'0'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 
                      
if (ut_tw_status = UT_TW_Success) then
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
    lclevtmsgsentctr = lclevtmsgsentctr + 1
    ut_setrequirements cEVS3019, "P"
    write "<*> Passed (3019) - Add Event Filter command sent successfully."
  else
    ut_setrequirements cEVS3019, "F"
    write "<!> Failed (3019) - Event message for Add Event Filter command not rcv'd."
  endif
endif

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 4.0.1: Retrieve applications data "
;                     Expect evt msg ID 12 added for filtering
write ";***********************************************************************"
lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_401.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

local valid_evt_ID_12_ndx = 0

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId = 12) then
    ut_setrequirements cEVS3019, "P"
    write "<*> Passed (3019) - Event ID=12 found in binary filter table."
    valid_evt_ID_12_ndx = i
  endif
enddo

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[valid_evt_ID_12_ndx].Msk = 0) then
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - Event ID=12 mask is set correctly."

  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[valid_evt_ID_12_ndx].Ctr = 0) then
    ut_setrequirements cEVS3019, "P"
    write "<*> Passed (3019) - Event ID=12 counter is set correctly."
  else
    ut_setrequirements cEVS3019, "F"
    write "<!> Failed (3019) - Event ID=12 counter = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[valid_evt_ID_12_ndx].Ctr,"; Expected 0."
  endif
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - Event ID=12 counter = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[valid_evt_ID_12_ndx].Msk,"; Expected 0."
endif

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 4.1: Add filter for event message ID 12 which has been previously"
write ";           registered for filtering"
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_EVT_FILTERED_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=12 EVENT_MASK=X'0'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  lclevtmsgsentctr = lclevtmsgsentctr + 1
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - Add Event Filter command failed as expected."
else 
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - Add Event Filter Error event message was not generated for an event already being filtered."
endif 

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
 
write ";***********************************************************************"
write "; Step 4.1.1: Retrieve applications data "
;                    Expect NO evt ID 12 added for filtering
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                     
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_411.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
 
write " >>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
;
write"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId = 12) then
    if (i <> valid_evt_ID_12_ndx) then
      ut_setrequirements cEVS3019, "F"
      write "<!> Failed (3019) - Found EID=12 at a different location than expected."
    else
      ut_setrequirements cEVS3019, "P"
    endif
  endif
enddo

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 4.2: Add filter for event message ID 13 which is not registered "
write ";           for filtering"
;                  cEVS3019 is set to pass if command succeeds,
;                  otherwise it is set to fail"
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG, 1

ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=13 EVENT_MASK=X'0'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  ut_setrequirements cEVS3019, "P"
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - Add Event Filter command for EID=13 did not generated the expected event message."
endif

lclevtmsgsentctr = lclevtmsgsentctr + 1

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 4.2.1: Retrieve applications data "
;                    Expect evt ID 13 added for filtering
write ";***********************************************************************"
 
lclevtmsgsentctr = lclevtmsgsentctr + 1
                     
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_421.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif
 
write " >>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
 
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
local evt_ID_13_ndx

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId = 13) then
    ut_setrequirements cEVS3019, "P"
    evt_ID_13_ndx = i
  endif
enddo

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evt_ID_13_ndx].EvtId <> 13) then
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - Did not find EID=13 registered for filtering."
endif

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 4.3: Add filter for event message ID 14 which is not registered "
write ";           for filtering"
;               Expect Error message indicating that the event message ID 14 
;		filter cannot be added because the maximum number of events that
;		can registered for filtering has already been reached.
;               cEVS3019 is set to pass if above statement is true, otherwise
;               the requirement is set to Fail
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, CFE_EVS, CFE_EVS_ERR_MAXREGSFILTER_EID, ERROR, 1

ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""TST_EVS"" EVENT_ID=14 EVENT_MASK=X'0'"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if ($SC_$CPU_find_event[1].num_found_messages = 1) then              
  ut_setrequirements cEVS3019, "P"
  write "<*> Passed (3019) - Max filters event message rcv'd."
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1
  lclevtmsgsentctr = lclevtmsgsentctr + 1 
else
  ut_setrequirements cEVS3019, "F"
  write "<!> Failed (3019) - Did not rcv the expected event message. Maybe the event was added??."
endif
 
if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC
 
write ";***********************************************************************"
write "; Step 4.4: Retrieve applications data"
;        Expected evt msg configuration for test app after step 4.3 is executed
;                    ID     Mask      
;                    ------------
;                     1     0      
;                     2     x'ffff'     
;                     3     1        
;                     4     x'fff8'     
;                     5     2     
;                     8     0    
;                     12    0
;                     13    0
write ";***********************************************************************"

lclevtmsgsentctr = lclevtmsgsentctr + 1
                   
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_4_4.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
  lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

;===============================================================================

write " > > > > > > > > > > > > > > >> > > > > >> > > >> > >"
write " Event Msg IDs currently registered for filtering with Test App"

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  write " >>> Evt Msg ID ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId
enddo
write " > > > > > > > > > > > > > > >> > > > > >> > > >> > >"

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write" >>> Requirements Report"
 
local proceed = TRUE

for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
  if (ut_requirement[i] = "F") then
    proceed = FALSE
  endif
enddo

if (proceed = FALSE) then
  goto end_it
endif

write "************************************************************************"
write"Table 1 - Summary Max Counter Test Sequence of Events"
write"-------------------------------------------------------------------------"
write"----         ------           Total Evt  App Evt      Test App ID1  Number of"
write"Step         Request          Msg Sent   Msg Sent     Bin Fltr Ctr  Evt Msgs"
write""
write"5.2      10     SB No-ops        10        0            0              10"
write"5.3      10     TST_EVS Id=12    20        10           0              10"
write"5.4      65514  TST_EVS Id=1     65534     65524	     65514           65514"
write"5.5      1      TST_EVS Id=1     65535     65525       65515           1"      
write"5.6      9      TST_EVS Id=1     65535     65534       65524           9"
write"5.7      1      TST_EVS Id=1     65535     65535	     65525           1"
write"5.8      9      TST_EVS Id=1     65535     65535	     65534           9"
write"5.9      1      TST_EVS Id=1     65535     65535       65535           1"
write"5.10     1      TST_EVS Id=1     65535     65535       65535           0" 
write"5.11     10     TST_EVS Id=8     65535     65535       65535           10"
write"-------------------------------------------------------------------------"
 
write ";***********************************************************************"
write "; Step 5.0: Test Maximum Event Msg Sent Counters and"
write ";           Binary Filter Counters (See table 1) " 
write ";***********************************************************************"
write "; Step 5.1: Reset Counters"
write ";***********************************************************************"
write "; Step 5.1.1: Request via TST_EVS the reset of all TST_EVS binary filter"
write ";	     counters"
;                    cEVS3107
write ";***********************************************************************"

TSTEVSevtmsgsentctr = TSTEVSevtmsgsentctr + 1
lclevtmsgsentctr = lclevtmsgsentctr + 2

cmdexctr = $SC_$CPU_EVS_CMDPC + 1

ut_setupevents $sc, $cpu, TST_EVS, TST_EVS_RESETALLFILTERIDS_EID, INFO, 1

/$sc_$CPU_TST_EVS_RESETALLFILTERS AAPPNAME="TST_EVS"  

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 

if ($SC_$CPU_EVS_CMDPC = cmdexctr) then
  ut_setrequirements cEVS3107, "P"
  write "<*> Passed (3107) - Reset All Filters command sent successfully."
else
  ut_setrequirements cEVS3107, "F"
  write "<!> Failed (3107) - Reset All Filters command did not increment CMDPC."
endif
 
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = TSTEVSevtmsgsentctr) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else 
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected ",TSTEVSevtmsgsentctr
endif  

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  EVSevtmsgsentctr = EVSevtmsgsentctr + 1                            
  if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
    ut_setrequirements cEVS3105, "P"
    write "<*> Passed (3105) - Counters are correct."
  else
    ut_setrequirements cEVS3105, "F"
    write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
    lclevtmsgsentctr = $sc_$CPU_EVS_MSGSENTC
  endif
endif

write ">>> TEMSC Total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 5.1.1.1: Retrieve app data"
;                      Expect all tst app binary filter counters to be zero
;                      cEVS3107
write ";***********************************************************************"
;
lclevtmsgsentctr = lclevtmsgsentctr + 1

start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_5111.dat", hostCPU)

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, {lclevtmsgsentctr}

if ($sc_$CPU_EVS_MSGSENTC = lclevtmsgsentctr) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected ",lclevtmsgsentctr 
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC
                    
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].Ctr = 0) then
    ut_setrequirements cEVS3107, "P"
    write "<*> Passed (3107) - Binary Filter counter # ",i," reset."
  else
    ut_setrequirements cEVS3107, "F"
    write "<!> Failed (3107) - Binary Filter counter # ",i," set to ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].Ctr,"; Expected 0"
  endif 
enddo
for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  write ">>> ", $SC_$CPU_EVS_AppData[apps_registered].AppName, " -  Evt ID ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId, " - Mask - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].Msk, 5), " App Bin Fltr Ctr ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[i].Ctr 
enddo 

write ";***********************************************************************"
write "; Step 5.1.2: Disable evt msg type DEBUG for CFE_EVS to prevent of evt"
write ";	     generation of evt msgs when reseting ctrs and getting file"
write ";             to CVT"
write ";***********************************************************************"
;     NOTE: NO more increment by 2 of total evt msg sent counter nor of CFE_EVS
;           msg sent counter due to generation of evt msgs from app data 
;	    retrieval because the status of DEBUG evt msg type for CFE_EVS will
;	    be hereafter, DISABLE
  
cmdexctr =$SC_$CPU_EVS_CMDPC + 1

ut_sendcmd "$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION=""CFE_EVS"" DEBUG"

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 

if (ut_sc_status = UT_SC_CmdFailure) then              
  write " <!> Failed - DISABLE CFE_EVS debug event msg type"
endif
 
write ";***********************************************************************"
write "; Step 5.1.3: Clear TST_EVS evt msgs sent ctr"
;                    Expect app evt msg sent ctr = 0
;                    cEVS3009 (clear)
write ";***********************************************************************"
;
cmdexctr =$SC_$CPU_EVS_CMDPC + 1

ut_sendcmd "$SC_$CPU_EVS_RSTAPPCTRS APPLICATION=""TST_EVS"""

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 

if (ut_sc_status = UT_SC_CmdFailure) then
  write " <!> Failed - TST_EVS app event msg counter reset"
else
  if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 0) then
    ut_setrequirements cEVS3009, "P"
    write "<*> Passed (3009) - TST_EVS msg sent counter reset."
    TSTEVSevtmsgsentctr = $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC
  else
    ut_setrequirements cEVS3009, "F"
    write "<!> Failed (3009) - TST_EVS msg sent counter = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 0"
  endif 
endif

write ";***********************************************************************"
write "; Step 5.1.4: Clear total event messages sent counter. "
;                    Expect total event msg sent ctr = 0
;                    cEVS3003 (clear/reset) This requirement is only partially
;		     tested here.  The purpose of the command is to reset the
;		     Total Evt Msg Sent Ctr
;                    The requirement is fully tested in the EVS Cmds test.
;                    The standard utility, ut_sendcmd, is not used here to send
;		     command because the command counter should not increment 
;		     like most commands.
write ";***********************************************************************"
 
write "EVS command counters valid/invalid: ", $sc_$cpu_evs_cmdpc, "/", $sc_$cpu_evs_cmdec

/$SC_$CPU_EVS_ResetCtrs

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, 0 
write "EVS command counters valid/invalid: ", $sc_$cpu_evs_cmdpc, "/", $sc_$cpu_evs_cmdec

if ($sc_$CPU_EVS_MSGSENTC = 0) then
  ut_setrequirements cEVS3003, "P"
  write "<*> Passed (3003) - Counter reset."
else
  ut_setrequirements cEVS3003, "F"
  write "<!> Failed (3003) - EVS Msg Sent Counter = ",$sc_$CPU_EVS_MSGSENTC,"; Expected 0"
endif 

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 5.1.5: Set mask for evt msg ID 1 in Test app to 0 always send"
write ";***********************************************************************"
;                    NOTE: If the evt msg doesn't occur, 
;                    we know that it is not because of the bin filter mask.
;                    cEVS3012
;
cmdexctr =$SC_$CPU_EVS_CMDPC + 1

ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""TST_EVS"" EVENT_ID=X'1' FILTERMASK=X'0'"

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr} 

if (ut_sc_status = UT_SC_CmdFailure) then
  ut_setrequirements cEVS3012, "F"
  write "<!> Failed (3012) - Set Binary Filter mask command did not increment CMDPC."
else
  ut_setrequirements cEVS3012, "P"
  write "<*> Passed (3012) - Set Binary Filter mask command send successfully."
endif                     
 
write ";***********************************************************************"
write "; Step 5.1.5.1: Retrieve app data"
;                     Expect test app evt msg ID 1 with mask = 0,
;                     test app evt msg ID1 bin fltr ctr = 0
;                     cEVS3012
write ";***********************************************************************"
;
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_5151.dat", hostCPU)
wait 10

local evtID1 = 0

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId = 1) then
    evtID1 = i
    write "event id 1 index ", evtid1
  endif
enddo

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk = 0) then   
  ut_setrequirements cEVS3012, "P"
else
  ut_setrequirements cEVS3012, "F"
  write "<!> Failed (3012) - Binary Filter mask for EID=1 = ",$sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk,"; Expected 0"
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write " >>>   ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId,;;
      "   - -   ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      "   - -   ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, "   - -   ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, "   - -   ",$SC_$CPU_EVS_MSGSENTC

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 5.2: Send 10 no-ops to CFE_SB "
;                  Expect 10 SB NO-OP Evt Msgs and 
;                  total evt msg sent ctr = 10
;                  test app evt msg sent ctr = 0
;                  cEVS3105
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO, 1

ut_sendcmd $SC_$CPU_SB_NOOP 10
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10 
                 
if ($sc_$CPU_EVS_MSGSENTC = 10) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else 
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 10." 
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 5.2.1: Retrieve app data"
;                    test app evt msg ID1 bin fltr ctr = 0
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_521.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 0 ) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 0"
endif

write" >>> Requirements Report"
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 5.3: Request generation of 10 TST_EVS event id 12" 
;                  which is registered for filtering
;                  cEVS3105, cEVS3104
;                  Expect
;                  total evt msg sent ctr = 20
;                  tst app evt msg sent ctr = 10
;                  TST_EVS ID=12 Evt Msgs = 10
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, TST_EVS, 12, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="12" Iters="10" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_MSGSENTC, 20 

if ($SC_$CPU_find_event[1].num_found_messages <> 10) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 10"
endif

if ($sc_$CPU_EVS_MSGSENTC = 20) then
  ut_setrequirements cEVS3105, "P"
  write "<*> Passed (3105) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  write "<!> Failed (3105) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 10." 
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 10) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - TST_EVS Message counts are correct"
else
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - TST_EVS msg sent ctr = ",$sc_$CPU_EVS_App[apps_registered].APPMSGSENTC,"; Expected 10."
endif
                         
write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 5.3.1: Retrieve app data"
;                    test app evt msg ID1 bin fltr ctr = 0
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_531.dat", hostCPU)
wait 10                     

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 0 ) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 0"
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write" >>> Requirements Report"
 
for i = 0 to ut_req_array_size do
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
enddo

write ";***********************************************************************"
write "; Step 5.4: Request generation of 65514 iterations of TST_EVS event id 1"
;                  Expect 65514 TST_EVS ID=1 Evt Msgs
;                  total evt msg sent ctr = 65534
;                  tst app evt msg sent ctr = 65524
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="1" Iters="65514" MILLISECONDS="150"

wait until $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 65524

if ($SC_$CPU_find_event[1].num_found_messages <> 65514) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 65514"
endif

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC <> 65524) then
  write "<!> Failed - Unexpected App evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65524"
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 5.4.1: Retrieve app data"
;                    test app evt msg ID1 bin fltr ctr = 65514
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_541.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 65514 ) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65514."
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write ";***********************************************************************"
write "; Step 5.5: Request generation of 1 iteration of TST_EVS event id 1"
;                  total evt msg sent ctr = 65535
;                  tst app evt msg sent ctr = 65525
;                  cEVS3103_3
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="1" Iters="1" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 1"
endif

wait 10

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC <> 65525) then
  write "<!> Failed - Unexpected App evt msg sent ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65525"
endif

write ">>> TEMSC Total Evt Msg Sent Ctr = ",$SC_$CPU_EVS_MSGSENTC

write ";***********************************************************************"
write "; Step 5.5.1: Retrieve app data"
;                    test app evt msg ID1 bin fltr ctr = 65515
;                    cEVS3103_3
write ";***********************************************************************"

start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_551.dat", hostCPU)
wait 30

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[1].Ctr = 65515) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65515."
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write ";***********************************************************************"
write "; Step 5.6: Request generation of 9 iteration of TST_EVS event id 1"
;                  Expect 9 Evt Msg
;                  total evt msg sent ctr = 65535
;                  test app evt msg sent ctr = 65534
;
;                  cEVS3104, cEVS3105 and cEVS3105.1 (no rollover)
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, INFO, 1

/$SC_$CPU_TST_EVS_SendEvtMsg INFO EventId="1" Iters="9" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 9, 60 

if ($SC_$CPU_find_event[1].num_found_messages <> 9) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 9"
endif

if ($sc_$CPU_EVS_MSGSENTC = 65535) then
  ut_setrequirements cEVS3105_1, "P"
  write "<*> Passed (3105.1) - Counters are correct."
else
  ut_setrequirements cEVS3105_1, "F"
  write "<!> Failed (3105.1) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 65535." 
endif

wait 10

if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC <> 65534) then
  write "<!> Failed - Unexpected value found test app evt msg ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65534"
endif

write ";***********************************************************************"
write "; Step 5.6.1: Retrieve app data"
;                    Expect 9 evt msgs
;                    test app evt msg ID1 bin fltr ctr = 65524
;                    cEVS3103_3
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_561.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 65524) then
  ut_setrequirements cEVS3103_3, "P" 
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65524."
endif

write " Bin flt ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[1].Ctr
write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
;
write ";***********************************************************************"
write "; Step 5.7: Request generation of 1 iteration of TST_EVS event id 1"
;                  Expect 1 TST_EVS ID=1 Evt Msg
;                  total evt msg sent ctr = 65535
;                  test app evt msg sent ctr = 65535
;                  cEVS3104, cEVS3105, cEVS3105_1 
write ";***********************************************************************"
;
ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="1" Iters="1" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC, 65535

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 1"
endif

if ($sc_$CPU_EVS_MSGSENTC = 65535) then
  ut_setrequirements cEVS3105_1, "P"
  write "<*> Passed (3105.1) - Counters are correct."
else
  ut_setrequirements cEVS3105_1, "F"
  write "<!> Failed (3105.1) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 65535." 
endif
              
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 65535) then
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3104) - Counters are correct."
else
  ut_setrequirements cEVS3104, "F"
  write "<!> Failed (3104) - Unexpected value found for app evt msg ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65535"
endif

write ";***********************************************************************"
write "; Step 5.7.1: Retrieve app data"
;                    Expect 1 evt msg
;                    test app evt msg ID1 bin fltr ctr = 65525
;                    
;                    cEVS3103_3
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_571.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 65525) then
  ut_setrequirements cEVS3103_3, "P" 
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65525."
endif

write " Bin flt ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr
write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

write ";***********************************************************************"
write "; Step 5.8: Request generation of 9 TST_EVS event id 1"
;                  Expect 9 Evt Msgs
;                  total evt msg sent ctr = 65535
;                  test app evt msg sent ctr = 65535
;                  cEVS3104, cEVS3104, cEVS3105 and cEVS3105.1
write ";***********************************************************************"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="1" Iters="9" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 9 

if ( $SC_$CPU_find_event[1].num_found_messages <> 9) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 9"
endif                       

if ($sc_$CPU_EVS_MSGSENTC = 65535) then
  ut_setrequirements cEVS3105, "P"
  ut_setrequirements cEVS3105_1, "P"
  write "<*> Passed (3105;3105.1) - Counters are correct."
else
  ut_setrequirements cEVS3105, "F"
  ut_setrequirements cEVS3105_1, "F"
  write "<!> Failed (3105;3105.1) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 65535." 
endif
              
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 65535) then
  ut_setrequirements cEVS3104, "P"
  ut_setrequirements cEVS3104_1, "P"
  write "<*> Passed (3104;3104.1) - Counters are correct."
else
  ut_setrequirements cEVS3104, "F"
  ut_setrequirements cEVS3104_1, "F"
  write "<!> Failed (3104;3104.1) - Unexpected value found for app evt msg ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65535"
endif

write ";***********************************************************************"
write "; Step 5.8.1: Retrieve app data"
;                    Expect 1 evt msg
;                    test app evt msg ID1 bin fltr ctr = 65534
;                    cEVS3103_3
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_581.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 65534) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65534."
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write ";***********************************************************************"
write "; Step 5.9: Request generation of 1 TST_EVS event id 1"
;                  Expect No TST_EVS ID=1 Evt Msg
;                  total evt msg sent ctr = 65535
;                  test app evt msg sent ctr = 65535
write ";***********************************************************************"
                
ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1
      
/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="1" Iters="1" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 1"
endif                       

if ($sc_$CPU_EVS_MSGSENTC = 65535) then
  ut_setrequirements cEVS3105_1, "P"
  write "<*> Passed (3105.1) - Counters are correct."
else
  ut_setrequirements cEVS3105_1, "F"
  write "<!> Failed (3105.1) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 65535." 
endif
              
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 65535) then
  ut_setrequirements cEVS3104, "P"
  ut_setrequirements cEVS3104_1, "P"
  write "<*> Passed (3104;3104.1) - Counters are correct."
else
  ut_setrequirements cEVS3104, "F"
  ut_setrequirements cEVS3104_1, "F"
  write "<!> Failed (3104;3104.1) - Unexpected value found for app evt msg ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65535"
endif

;
write ";***********************************************************************"
write "; Step 5.9.1: Retrieve app data"
;                    Expect 1 evt msg
;                    test app evt msg ID1 bin fltr ctr = 65535
;                    cEVS3103_3
write ";***********************************************************************"
;
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_591.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 65535) then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3, "F"
  write "<!> Failed (3103.3) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65534."
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
 
write ";***********************************************************************"
write "; Step 5.10: Request generation of 1 TST_EVS event id 1"
;                   Expect No TST_EVS ID=1 Evt Msg
;                   total evt msg sent ctr = 65535
;                   test app evt msg sent ctr = 65535
write ";***********************************************************************"

ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1
      
/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId="1" Iters="1" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1 

if ($sc_$CPU_EVS_MSGSENTC = 65535) then
  ut_setrequirements cEVS3105_1, "P"
  write "<*> Passed (3105.1) - Counters are correct."
else
  ut_setrequirements cEVS3105_1, "F"
  write "<!> Failed (3105.1) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 65535." 
endif
              
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 65535) then
  ut_setrequirements cEVS3104, "P"
  ut_setrequirements cEVS3104_1, "P"
  write "<*> Passed (3104;3104.1) - Counters are correct."
else
  ut_setrequirements cEVS3104, "F"
  ut_setrequirements cEVS3104_1, "F"
  write "<!> Failed (3104;3104.1) - Unexpected value found for app evt msg ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65535"
endif

write ";***********************************************************************"
write "; Step 5.10.1: Retrieve app data"
;                     Expect NO evt msg
;                     test app evt msg ID1 bin fltr ctr = 65535
;                     cEVS3103_3_1 no rollover
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_5101.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr = 65535) then
  ut_setrequirements cEVS3103_3_1, "P"
  write "<*> Passed (3103.3.1) - Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3_1, "F"
  write "<!> Failed (3103.3.1) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65535."
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

write ";***********************************************************************"
write "; Step 5.11: Request generation of 10 TST_EVS event id 8 "
;                   Which is registered for filtering with a mask of 0
;                   This evt msg is requested to verify that total evt 
;                   msg sent ctr, app evt msg sent ctr and test app evt 
;                   msg ID 1 bin fltr ctr all retain their maximum values
;                   Expect 10 TST_EVS ID=8 Evt Msgs because the fact
;                   that the app et msg ID 1 bin fltr ctr has reached its
;                   maximum shall not affect generation of an evt msg
;                   of a different ID which is rigestered for filtering
;                   total evt msg sent ctr = 65535
;                   test app evt msg sent ctr = 65535
;                   cEVS3104.1
;                   cEVS3105.1
write ";***********************************************************************"
 
local evtID8ndx = 0

for i = 1 to CFE_EVS_MAX_EVENT_FILTERS do
  if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[i].EvtId = 8) then
    evtID8ndx = i
  endif
enddo

ut_setupevents $sc, $cpu, TST_EVS, 8, DEBUG, 1

/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId=8 Iters="10" MILLISECONDS="150"

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 10 

if ($SC_$CPU_find_event[1].num_found_messages <> 10) then
  write "<!> Failed - ",$SC_$CPU_find_event[1].num_found_messages," event messages generated. Expected 10"
endif

if ($sc_$CPU_EVS_MSGSENTC = 65535) then
  ut_setrequirements cEVS3105_1, "P"
  write "<*> Passed (3105.1) - Counters are correct."
else
  ut_setrequirements cEVS3105_1, "F"
  write "<!> Failed (3105.1) - total evt msg sent ctr = ",$SC_$CPU_EVS_MSGSENTC,"; Expected 65535." 
endif
              
if ($sc_$CPU_EVS_APP[apps_registered].APPMSGSENTC = 65535) then
  ut_setrequirements cEVS3104_1, "P"
  write "<*> Passed (3104.1) - Counters are correct."
else
  ut_setrequirements cEVS3104_1, "F"
  write "<!> Failed (3104;3104.1) - Unexpected value found for app evt msg ctr = ",$SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,"; Expected 65535"
endif

write ";***********************************************************************"
write "; Step 5.11.1: Retrieve app data"
;                     Expect 10 evt msgs of ID 8
;                     test app evt msg ID 8 bin fltr ctr = 10
;                     cEVS3103.3.1
write ";***********************************************************************"
 
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "bin_fltr_5111.dat", hostCPU)
wait 10

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[evtID8ndx].Ctr = 10) then
  ut_setrequirements cEVS3103_3_1, "P"
  write "<*> Passed (3103.3.1) - ID8 Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3_1, "F"
  write "<!> Failed (3103.3.1) - TST_EVS app event ID8 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID8ndx].Ctr,"; Expected 10."
endif

if ($sc_$CPU_EVS_AppData[apps_registered].BinFltr[1].Ctr = 65535) then
  ut_setrequirements cEVS3103_3_1, "P"
  write "<*> Passed (3103.3.1) - ID1 Binary filter count is correct."
else
  ut_setrequirements cEVS3103_3_1, "F"
  write "<!> Failed (3103.3.1) - TST_EVS app event ID1 ctr = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr,"; Expected 65535."
endif

write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> evt id      mask      fltr ctr    App ctr     Total ctr"
write ">>> ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].EvtId, ;;
      " - - ", %hex($SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Msk, 5), ;;
      " - - ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID1].Ctr, " - - ", ;;
      $SC_$CPU_EVS_APP[apps_registered].APPMSGSENTC,  " - - ",$SC_$CPU_EVS_MSGSENTC
write ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
write " >>> Test app bin fltr ctr for evt msg ID 8 = ",$SC_$CPU_EVS_AppData[apps_registered].BinFltr[evtID8ndx].Ctr 
write " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
;
write "************************************************************************"
;
printReq:
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO
;
end_it:
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables
;
ENDPROC
