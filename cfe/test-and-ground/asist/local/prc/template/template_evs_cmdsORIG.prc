proc $sc_$cpu_evs_cmds
;*****************************************************************************
; Test Name:  EVS_Cmd
; Test Level: Build Verification
; Test Type:  Functional
;
; Test Description
;    The purpose of this test is to verify the CFE_EVS Command functionality for
;    the Event Service (CFE_EVS) function of the Core Flight Executive (cFE)
;    software. The operation of all CFE_EVS commands will be verified for valid
;    and invalid commands. Valid commands for event logging and binary filter
;    processing will be tested in their respective test applications. Invalid
;    commands will be sent via STOL raw commands.  Appropriate telemetry will be
;    examined to verify proper action of the CFE_EVS command issued. 
;    The TST_EVS test application will be used as needed to send commands 
;    not available thru cFE core applications.
; 
; Requirements Tested
;   cEVS3000	Upon receipt of Command the cFE shall enable/disable, as
;		specified in the Command, the future generation of Event
;		Messages for the Command-specified Event Type.
;   cEVS3002	Upon receipt of Command the cFE shall generate a NO-OP event
;		message.
;   cEVS3003	Upon receipt of Command the cFE shall set the following 
;		counters to zero in Event Services telemetry.
;                 * Valid Command Counter
;	          * Invalid Command Counter
;                 * Event Message Sent Counter
;                 * Event Message Truncation Counter
;	          * Unregistered Application Send Counter
;   cEVS3004	Upon receipt of Command, the cFE shall write the following
;		information to the Command specified cFE EVS Application Data
;               file for each registered Application:
;                 * Application Event Message Sent Counter
;                 * Application Event Service Enable Status
;                 * Application Event Type Enable Statuses (one per Event Type)
;                 * Application Event IDs (for events to be filtered)
;                 * Application Binary Filter Masks (one per registered EID)
;                 * Application Binary Filter Cntrs (one per registered EID)
;   cEVS3004.1	If a file is not specified, the cFE shall use the 
;               <PLATFORM_DEFINED> filename. 
;   cEVS3005	Upon receipt of valid command, the cFE shall increment the
;		valid command counter.
;   cEVS3006	Upon receipt of an invalid command, the cFE shall increment
;		the invalid command counter.
;   cEVS3007	Upon receipt of Command the cFE shall enable/disable, as
;		specified in the Command, the future generation of Event
;		Messages for the Command-specified Application and Event Type.
;   cEVS3008	Upon receipt of Command the cFE shall enable/disable, as
;		specified in the Command, the future generation of Event
;		Messages for the Command-specified Application.
;   cEVS3009	Upon receipt of Command, the cFE shall set the Command-
;		specified Application's Event Message Sent Counter to zero.
;   cEVS3010	Upon receipt of Command, the cFE shall set an Application's
;		Binary Filter Counter to zero for the Command-specified Event ID
;   cEVS3011	Upon receipt of Command, the cFE shall set all of an
;		Application's Binary Filter Counters to zero.
;   cEVS3012	Upon receipt of Command the cFE shall set an Application's 
;		Binary Filter Mask to the Command-specified Event Filter for the
;		given Application Event ID.
;   cEVS3014	<OPTIONAL> Upon receipt of Command, the cFE shall set the Event
;		Logging Mode to the Command-specified mode, either overwrite or
;		discard.
;   cEVS3017	Upon receipt of Command the cFE shall enable/disable, as
;		specified in the Command, the routing of all future Event
;		Messages to the Command-specified Event Message Port.
;   cEVS3018	The cFE shall provide the following Event Service data items
;		in telemetry (SB Messages):
;		  * Valid Command Counter
;		  * Invalid Command Counter
;		  * Event Message Sent Counter
;		  * Event Message Truncation Counter
;		  * Unregistered Application Send Counter
;		  * Event Message Output Port Enable Statuses
;		  * For each registered Application:
;		    * Application Event Message Sent Counter
;		    * Application Event Service Enable Status
;   cEVS3300	The cFE shall support <PLATFORM_DEFINED, 4> Event Message ports.
;
; Prerequisite Conditions:
;   Availability of the TST_EVS test application To send event messages 
;   according to the following specifications:
; 
;   1a. Command, /TST_EVS_SendEvMsg, to the Test Application with 
;     the following parameters:
;     Event Id
;     Event Type (Debug | Information | Error | Critical)
;     Number of Iterations 
;
;   1b. In response to the command, the application requests the generation 
;     of the command-specified Event Message with the command-specified
;     Event Type. The request will be made the number of times specified
;     in the iterations parameter. In most cases the test string is:
;
;     'Iteration No. = ' %d, 'Msg ID = '  %d , Where n is the current number 
;     within the iteration.
;
; Assumptions and Constraints:
;   All cFE applications can generate DEBUG, INFO, ERROR, and CRIT event types.
;
; Change History
;
;     Date	 Name         Description
;     ----       -----------  -----------
;     06/27/05   S. Applebee  Original Procedure
;     07/25/05   S. Applebee  Post Walkthru Enhancements
;     08/08/05   S. Applebee  Additional Post Walkthru changes 
;     04/12/06   E. Stattel   Run with Bld 3.1 of cFE FSW
;                             DEBUG evt msg type status is DISABLE by default
;                             while INFO, ERR and CRIT are ENABLE
;                             CFE_TBL task has been added since Bld 1 of cFE
;			      FSW and all no-op evt msgs are of type INFO 
;     06/02/06   E.Stattel    Post walk through check for ERROR evt 
;                             msg ID 9 added where applicable
;                             Also changes to Evt msg Type statuses
;     06/07/06   EIS          Updated to run with LRO
;                             deleted all references to CI and TO because these
;                             applications are in flux.
;                             local variable added_app_lctn added
;                             made cfe_app_cnt itself minus 1 where applicable 
;     12/26/06   N. Schweiss  Update for cFE 4.0.
;     07/31/07	 W. Moleski   Added test for Rqmt 3300. Added a Power-On reset
;			      as the first command of the test. Re-ordered the
;			      Step numbers to be sequential.
;     09/30/11	 W. Moleski   Cleaned up the proc and added a variable for the
;			      ram directory specification.
;     10/27/11	 W. Moleski   Modified to be like other tests
;     09/15/14	 W. Moleski   Modified to add the SCH app since this is started
;			      rather than the SCH_LAB application
;     05/04/16  W. Moleski    Updated for 6.5.0 testing using CPU1 for 
;			      commanding and added a hostCPU variable for the
;                             utility procs that connect to the host IP.
;			      Replaced SCH with SCH_LAB for cFE 6.5.0 testing.
;
;  Arguments
;
;  Procedures/Utilities Called
;         
;     Name              Description
;     ----              -----------
;     ut_runproc	Directive to formally run the procedure and capture
;                       the log file.
;     ut_sendcmd        Directive to send commands to the spacecraft  
;                       and wait for the command to be accepted/rejected 
;                       by the spacecraft. 
;     ut_sendrawcmd     Directive to send raw commands to the spacecraft 
;                       and wait for the command to be accepted/rejected 
;                       by the spacecraft. 
;     ut_setupevents    Directive to look for a particular event and increment
;                       a value in the CVT to indicate receipt.  
;     ut_tlmwait        Procedure to wait for a specified telemetry point to
;                       update to a specified value.
;     ut_pfindicate     Directive to print the pass fail status of a
;			particular requirement number.
;     get_file_to_cvt   Procedure to write some specified FSW data to a file
;			and then ftp the file from the FSW hardware to ASIST
;			hardware and load file to the CVT.
;     load_start_app    Procedure to load and start a user application from
;                       the /s/opr/accounts/cfebx/apps/cpux  directory.
;     evs_send_debug, info, error, and crit
;                       sub-procedures to send evt msgs of every type
;
;  Expected Test Results and Analysis
;    CFE software will recognize and reject all invalid commands.
;    CFE software will correctly increment counts and process all valid 
;    commands. 
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h" 
#include "tst_evs_events.h" 
#include "cfe_evs_events.h" 
#include "$sc_$cpu_is_app_loaded.prc"

%liv (log_procedure) = logging

;**********************************************************************
;  Define variables
;**********************************************************************
local i = 0
local cfe_app_cnt = 9        ; dependant on number of apps being processed
local added_app_lctn = 0
local cfe_applications[cfe_app_cnt] = ["CFE_EVS","CFE_SB","CFE_ES","CFE_TIME","CFE_TBL","CI_LAB_APP","TO_LAB_APP","SCH_LAB_APP","TST_EVS"]
local cfe_debug_msgs[cfe_app_cnt] = [1,0,0,0,0,0,0,0,1]
local cfe_info_msgs[cfe_app_cnt] =  [1,1,1,1,1,1,1,0,1]
local cfe_error_msgs[cfe_app_cnt] = [0,0,0,0,0,0,0,0,1]
local cfe_crit_msgs[cfe_app_cnt] =  [0,0,0,0,0,0,0,0,1]

local cfe_port_cnt = 4
local cfe_ports[cfe_port_cnt] = ["PORT_ONE","PORT_TWO","PORT_THREE","PORT_FOUR"]

local raw_command

local previous_event_type_status[cfe_app_cnt]
local current_event_type_status[cfe_app_cnt]
local event_type_status_flag[cfe_app_cnt]

local previous_app_msg_sent_ctr[cfe_app_cnt]
local current_app_msg_sent_ctr[cfe_app_cnt]
local msg_ctr_status_flag[cfe_app_cnt]

local previous_port_mask
local port_mask_status_flag

local previous_bin_fltr_ctr[cfe_app_cnt]
local current_bin_fltr_ctr[cfe_app_cnt]
local bin_fltr_msgs[cfe_app_cnt] =  [0,1,0,0,0,0,1,0,1]

#define EVS_3000	0
#define EVS_3002	1
#define EVS_3003	2
#define EVS_3004	3
#define EVS_30041	4
#define EVS_3005	5
#define EVS_3006	6
#define EVS_3007	7
#define EVS_3008	8
#define EVS_3009	9
#define EVS_3010	10
#define EVS_3011	11
#define EVS_3012	12
#define EVS_3014	13
#define EVS_3017	14
#define EVS_3018	15
#define EVS_3300        16

global ut_req_array_size = 16
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
   ut_requirement[i] = "U"
ENDDO

local cfe_requirements[0 .. ut_req_array_size] = ["cEVS3000","cEVS3002","cEVS3003","cEVS3004","cEVS3004_1","cEVS3005","cEVS3006","cEVS3007","cEVS3008","cEVS3009","cEVS3010","cEVS3011","cEVS3012","cEVS3014","cEVS3017","cEVS3018","cEVS3300"]
 
local ramDir = "RAM:0"
local logicalRamDir = "/ram/"
local hostCPU = "$CPU"

write ";***********************************************************************"
write "; Step 1.0: Test setup"
write ";***********************************************************************"
write "; Step 1.1: Command a Power-on Reset on $CPU"
write ";***********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

write ";*********************************************************************"
write "; Enable DEBUG Event Messages "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_EVS", CFE_EVS_ENAEVTTYPE_EID, "DEBUG", 1

ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Debug events have been enabled."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

write ";***********************************************************************"
write "; Step 1.2: Start the TST_EVS application."
write ";***********************************************************************"
;; Check if TST_EVS app is running

start load_start_app ("TST_EVS", hostCPU)
wait 10

start get_file_to_cvt (ramDir,"cfe_es_app_info.log","cfe_es_app_info_afterinit.log",hostCPU)

IF ($sc_$cpu_is_app_loaded("TST_EVS")) THEN
  write "<*> Passed - TST_EVS initialization OK."
ELSE
  write "<!> Failed - TST_EVS failed to initialize !!!!"
  goto TST_APP_FAILED
ENDIF

write ";***********************************************************************"
write "; Step 1.3: Retrieve the application data file."
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_13.dat", hostCPU)
wait 5

;;Report of evt message type statuses for cFE core and added test apps
;; Find the index of TST_EVS
for i = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($sc_$cpu_evs_AppData[i].AppName = "TST_EVS") then
    added_app_lctn = i
  endif
enddo
 
IF (added_app_lctn > 0) THEN
  write "  >>> Default status for every event message type "
  FOR i = 1 to cfe_app_cnt DO
    write " For app ", $sc_$cpu_EVS_AppData[i].AppName
    write " DEBUG = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
    write " INFO = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
    write " ERROR = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
    write " CRIT = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
    write " Evt msg types mask"
    write " CEID"
    write " ",$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Err, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Info, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
  ENDDO
ELSE
   write "*** Warning; added_app_lctn = ", added_app_lctn
   wait 10
ENDIF
 
write ";***********************************************************************"
write "; Step 1.4: Generate application data file with filename argument and "
write ";           retrieve file."
write ";***********************************************************************"
local work = %env("WORK") 
local src_filename = "user_appdatafilename"
local dest_filename =  "$sc_$cpu_" & src_filename
local dest_pathname = work & "/image/" & dest_filename
local cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_WriteAppData2File AppDataFileName=logicalRamDir & src_filename

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3004, "P"
  write "<*> Passed (3004) - Write App Data to File command sent successfully."
else
  ut_setrequirements EVS_3004, "F"
  write "<*> Passed (3004) - Write App Data to File command did not increment CMDPC."
endif

start ftp_file (ramDir, src_filename , dest_filename, hostCPU, "g")
wait 10
IF file_exists(dest_pathname) THEN
  ut_setrequirements EVS_3004, "P"
  write "<*> Passed (3004) - Destination File exists."
else
  ut_setrequirements EVS_3004, "F"
  write "<*> Passed (3004) - Destination file does not exist."
ENDIF

write ";***********************************************************************"
write "; Step 1.5: Generate application data file _without_ filename argument."
write ";***********************************************************************"
cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_WriteAppData2File AppDataFileName=""

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_30041, "P"
  write "<*> Passed (3004.1) - Write App Data to File command sent successfully."
else
  ut_setrequirements EVS_30041, "F"
  write "<*> Passed (3004.1) - Write App Data to File command did not increment CMDPC."
endif

src_filename = "cfe_evs_app.dat" ;default name
dest_filename =  "$sc_$cpu_" & src_filename
start ftp_file (ramDir, src_filename , dest_filename, hostCPU, "g")
wait 10

dest_pathname = work & "/image/" & dest_filename
IF file_exists(dest_pathname) THEN
  ut_setrequirements EVS_30041, "P"
  write "<*> Passed (3004.1) - Default Destination File exists."
else
  ut_setrequirements EVS_30041, "F"
  write "<!> Failed (3004.1) - Default Destination file does not exist."
ENDIF

write ";***********************************************************************"
write "; Step 2.0: Send noop command."
write ";           Observe NOOP info message in ASIST event window"
write ";           cEVS3002"
write ";           cEVS3005"
write ";           cEVS3018 (Valid Command Counter)"
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_NOOP_EID, INFO, 1

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_NOOP

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3002, "P"
  ut_setrequirements EVS_3005, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3002;3005;3018) - EVS NOOP command sent successfully."
else
  ut_setrequirements EVS_3002, "F"
  ut_setrequirements EVS_3005, "F"
  ut_setrequirements EVS_3018, "F"
  write "<*> Passed (3002;3005;3018) - EVS NOOP command did not increment CMDPC."
endif

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  ut_setrequirements EVS_3002, "P"
  write "<*> Passed (3002) - Expected Event Message rcvd"
ELSE
  ut_setrequirements EVS_3002, "F"
  write "<!> Failed (3002) - Expected NO-OP Event Message was not generated."
ENDIF 

write ";***********************************************************************"
write "; Step 3.0: Enable Application Event Types"
write ";***********************************************************************"
write "; Step 3.1: Application Event Type Command Test"
write ";***********************************************************************"
write "; Step 3.1.1: Send Enable Application Event Type command"
write ";             FSW indicates invalid command. Command Error count "
write ";             increments. Command Processed counter does not change."
write ";             Use value outside of valid range: 1 and F. "
write ";             Use cFE application that does not exist."
write ";             cEVS3006"
write ";             cEVS3007"
write ";             cEVS3018 (Invalid Command Counter)"
write ";***********************************************************************"
;; Setup event to capture
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1
 
local errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_ENAAPPEVTTYPEMASK APPLICATION="CFE_SB" BITMASK=X'0'  
wait 8

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3007, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3006;3007;3018) - Enable App Event Type Mask command failed as expected."
ELSE
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3007, "F"
  ut_setrequirements EVS_3018, "F"
  write "<*> Passed (3006;3007;3018) - Invalid Enable App Event Type Mask command did not increment CMDEC."
ENDIF

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message rcvd"
ELSE
  write "<!> Failed - Expected Error Event Message was not generated."
ENDIF 
 
write ";***********************************************************************"
write "; Step 3.1.2: Disable DEBUG Event Message Type"
write ";             to continue testing Enable/Disable evt type status"
write ";***********************************************************************"
/$SC_$CPU_EVS_DISEVENTTYPE DEBUG 

write ";***********************************************************************"
write "; Step 3.1.3: Command to Enable Evt Msg Type"
write ";             Sending command with invalid Application Name"        
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_NOAPPIDFOUND_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_ENAAPPEVTTYPE APPLICATION="CFE_" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3007, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3006;3007;3018) - Enable App Event Type command failed as expected."
ELSE
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3007, "F"
  ut_setrequirements EVS_3018, "F"
  write "<*> Passed (3006;3007;3018) - Invalid Enable App Event Type command did not increment CMDEC."
ENDIF
 
IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message rcvd"
ELSE
  write "<!> Failed - Expected Error Event Message was not generated."
ENDIF 

write ";***********************************************************************"
write "; Step 3.1.4: Command to Enable Evt Msg Type"
write ";             to continue testing Disable/Enable evt type status"        
write ";***********************************************************************"
/$SC_$CPU_EVS_ENAEVENTTYPE DEBUG
wait 10
 
; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_314.dat",hostCPU)
wait 5
 
;event message types 
write "  >>> Default status for every event message type "
FOR i = 1 to cfe_app_cnt DO
  write " For app ", $sc_$cpu_EVS_AppData[i].AppName
  write " DEBUG = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
  write " INFO = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
  write " ERROR = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
  write " CRIT = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
  write " Evt msg types mask"
  write " CEID"
  write " ",$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Err, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Info, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

write ";***********************************************************************"
write "; Step 3.2: Send command to Disable Application Event Type command"
write ";             cEVS3006"
write ";             cEVS3007"
write ";             cEVS3018 (Invalid Command Counter)"
write ";***********************************************************************"
;; Setup for event
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_DISAPPEVTTYPEMASK APPLICATION="CFE_SB" BITMASK=X'0'  
wait 8

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3007, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3006;3007;3018) - Disable App Event Type Mask command failed as expected."
ELSE
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3007, "F"
  ut_setrequirements EVS_3018, "F"
  write "<!> Failed (3006;3007;3018) - Invalid Disable App Event Type Mask command did not increment CMDEC."
ENDIF

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message rcvd"
ELSE
  write "<!> Failed - Expected Error Event Message was not generated."
ENDIF 

write ";***********************************************************************"
write "; Step 3.2.1: Send command to Disable Application Event Type"
write ";             Sending command with invalid Application Name"
write ";***********************************************************************"
;;; 9 = CFE_EVS_ERR_NOAPPIDFOUND_EID
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_NOAPPIDFOUND_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION="CFE_" DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3007, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3006;3007;3018) - Disable App Event Type command failed as expected."
ELSE
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3007, "F"
  ut_setrequirements EVS_3018, "F"
  write "<*> Passed (3006;3007;3018) - Invalid Disable App Event Type command did not increment CMDEC."
ENDIF
 
IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message rcvd"
ELSE
  write "<!> Failed - Expected Error Event Message was not generated."
ENDIF 

write ";***********************************************************************"
write "; Step 3.2.2: Send Disable Application Event Type for DEBUG messages "
write ";             for each cFE application: CFE_SB, CFE_ES, CFE_EVS, "
write ";             CFE_TIME, TST_EVS"
write ";             Telemetry indicates DEBUG messages ENABLE."
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_322.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION=cfe_applications[i] DEBUG
  wait 5
ENDDO

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_322a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug 
ENDDO

local evs3007Passed = TRUE
write "" 
write "     DEBUG message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.2.3: Send DEBUG message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS,CFE_TIME, TST_EVS	"
write ";             Messages not processed by CFE_EVS."
write ";             Verify by telemetry message counts not incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
;; This offset represents the number of DEBUG events generated for each app
;; in the cfe_applications array
local evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_323.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_323a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.2.4: Send Disable Application Event Type command for INFO msgs" 
write ";             for each cFE application: CFE_SB, CFE_ES, CFE_EVS, "
write ";             CFE_TIME, TST_EVS"
write ";             Telemetry indicates INFO messages disabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_324.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION=cfe_applications[i] INFO
  wait 3
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_324a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
ENDDO

write "" 
write "     INFO message status"
write "     --------------------"
evs3007Passed = TRUE
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.2.5: Send INFO message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS"
write ";             Messages not processed by CFE_EVS."
write ";             Verify by telemetry INFO message counts not incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_325.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_info

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_325a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.2.6: Send Disable Application Event Type command for ERROR "
write ";             messages for each cFE application: CFE_SB, CFE_ES, "
write ";             CFE_EVS, CFE_TIME, TST_EVS"
write ";             Telemetry indicates ERROR messages disabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_326.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION=cfe_applications[i] ERROR
  wait 3
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_326a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
ENDDO

evs3007Passed = TRUE
write "" 
write "     ERROR message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.2.7: Send ERROR message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS"
write ";             Messages not processed by CFE_EVS."
write ";             Verify by telemetry ERROR message counts not incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_327.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_error

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_327a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]
 
evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i]  + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
     msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.2.8: Send Disable Application Event Type command for CRITICAL "
write ";             messages for each cFE application: CFE_SB, CFE_ES, "
write ";             CFE_EVS, CFE_TIME, TST_EVS "
write ";             Telemetry indicates CRITICAL messages disabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_328.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_DISAPPEVTTYPE APPLICATION=cfe_applications[i] CRIT
  wait 3
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_328a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
ENDDO

evs3007Passed = TRUE
write "" 
write "     CRITICAL message status"
write "     ----------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN 
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.2.9: Send CRITICAL message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, TO_APP, TST_EVS."
write ";             Messages not processed by CFE_EVS."
write ";             Verify by telemetry ctrs not incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_329.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_crit

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_329a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i]  + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
     msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Disable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Disable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.3: Enable Application Event Type Command Test"
write ";***********************************************************************"
write "; Step 3.3.1: Send Enable Application Event Type command for DEBUG msgs" 
write ";             for each cFE application: CFE_SB, CFE_ES, CFE_EVS, "
write ";             CFE_TIME, TST_EVS."
write ";             Telemetry indicates DEBUG messages enabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_331.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_ENAAPPEVTTYPE APPLICATION=cfe_applications[i] DEBUG
  wait 3
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_331a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug 
ENDDO

evs3007Passed = TRUE
write "" 
write "     DEBUG message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.3.2: Send DEBUG message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS"
write ";             Expect evt msgs, DEBUG evt msg type status is ENABLE"
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry message counts incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_332.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO
previous_app_msg_sent_ctr[cfe_app_cnt] = p@$sc_$cpu_EVS_AppData[added_app_lctn].EventCounter

s $sc_$cpu_evs_send_debug
wait 5

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_332a.dat",hostCPU)

wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [1,0,0,0,0,0,0,0,1]

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.3.3: Send Enable Application Event Type command for INFO "
write ";             messages for each cFE application: CFE_SB, CFE_ES,"
write ";             CFE_EVS, CFE_TIME, TST_EVS.		"
write ";             Telemetry indicates INFO messages enabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_333.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
  previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_ENAAPPEVTTYPE APPLICATION=cfe_applications[i] INFO
  wait 5
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_333a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
ENDDO

evs3007Passed = TRUE
write "" 
write "     INFO message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.3.4: Send INFO message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS.	"
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry INFO message counts incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_334.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_info

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_334a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF ((previous_app_msg_sent_ctr[i] = current_app_msg_sent_ctr[i]) AND cfe_info_msgs[i]) THEN
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    msg_ctr_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.3.5: Send Enable Application Event Type command for ERROR "
write ";             messages for each cFE application: CFE_SB, CFE_ES, "
write ";             CFE_EVS, CFE_TIME, TST_EVS.		"
write ";             Telemetry indicates ERROR messages enabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_335.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_ENAAPPEVTTYPE APPLICATION=cfe_applications[i] ERROR
  wait 5
ENDDO

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_335a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
ENDDO

evs3007Passed = TRUE
write "" 
write "     ERROR message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.3.6: Send ERROR message for each cFE application: CFE_SB,"
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS."
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry message counts incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_336.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_error

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_336a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF ((previous_app_msg_sent_ctr[i] = current_app_msg_sent_ctr[i]) AND cfe_error_msgs[i]) THEN
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    msg_ctr_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type counts."
endif

write ";***********************************************************************"
write "; Step 3.3.7: Send Enable Application Event Type command for CRITICAL "
write ";             messages for each cFE application: CFE_SB, CFE_ES,"
write ";             CFE_EVS, CFE_TIME, TST_EVS.	"
write ";             Telemetry indicates CRITICAL messages enabled."
write ";             cEVS3004"
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_337.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_ENAAPPEVTTYPE APPLICATION=cfe_applications[i] CRIT
  wait 5
ENDDO

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_337a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
ENDDO

evs3007Passed = TRUE
write "" 
write "     CRITICAL message status"
write "     -----------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type command."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type command."
endif

write ";***********************************************************************"
write "; Step 3.3.8: Send CRITICAL message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS.	"
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry CRITICAL message counts incrementing."
write ";             cEVS3007"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_338.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_crit

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_338a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evs3007Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF ((previous_app_msg_sent_ctr[i] = current_app_msg_sent_ctr[i]) AND cfe_crit_msgs[i]) THEN
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3007Passed = FALSE
  ELSE
    msg_ctr_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3007Passed = TRUE) then
  ut_setrequirements EVS_3007, "P"
  write "<*> Passed (3007) - Enable App Event Type counts."
else
  ut_setrequirements EVS_3007, "F"
  write "<!> Failed (3007) - Enable App Event Type counts."
endif

; Disable status for DEBUG event message type before sending enabling command
/$SC_$CPU_EVS_DISEVENTTYPE DEBUG 
wait 5
 
; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_338b.dat",hostCPU)
wait 5
 
;event message types 
FOR i = 1 to cfe_app_cnt DO
  write " For app ", $sc_$cpu_EVS_AppData[i].AppName
  write " DEBUG = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
  write " INFO = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
  write " ERROR = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
  write " CRIT = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
  write " Evt msg types mask"
  write " CEID"
  write " ",$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Err, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Info, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

write ";***********************************************************************"
write "; Step 4.0: Enable/Disable Event Types Test"
write ";***********************************************************************"
write "; Step 4.1: Enable DEBUG Event Type"
write ";***********************************************************************"
cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_ENAEVENTTYPE DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Enable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_41.dat", hostCPU)
wait 5
 
;event message types 
FOR i = 1 to cfe_app_cnt DO
  write " For app ", $sc_$cpu_EVS_AppData[i].AppName
  write " DEBUG = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
  write " INFO = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
  write " ERROR = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
  write " CRIT = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
  write " Evt msg types mask"
  write " CEID"
  write " ",$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Err, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Info, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

write ";***********************************************************************"
write "; Step 4.2: Disable Event Type Command Test"
write ";***********************************************************************"
write "; Step 4.2.1: Send Disable Event Type Command for DEBUG messages"
write ";            Telemetry indicates DEBUG messages enabled."
write ";            cEVS3000"
write ";            cEVS3004"
write ";***********************************************************************"
FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_DISEVENTTYPE DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Disable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_421.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

local evs3000Passed = TRUE
write "" 
write "     DEBUG message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.2.2: Send DEBUG message for each cFE application: CFE_SB,  "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS "
write ";             Messages not processed by CFE_EVS. "
write ";             Expect NO generation of event messages. "
write ";             Verify by telemetry message counts incrementing. "
write ";             cEVS3000 "
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_422.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_422a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.2.3: Send Disable Event Type Command for INFO messages	"
write ";             Telemetry indicates INFO messages disabled."
write ";             cEVS3000"
write ";             cEVS3004"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_423.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_DISEVENTTYPE INFO

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Disable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_423a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
ENDDO

evs3000Passed = TRUE
write "" 
write "     INFO message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.2.4:  Send INFO message for each cFE application: CFE_SB, "
write ";              CFE_ES, CFE_EVS, CFE_TIME, TST_EVS	"
write ";              Messages not processed by CFE_EVS."
write ";              Verify by telemetry INFO message counts not incrementing."
write ";              cEVS3000"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_424.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_info

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_424a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.2.5: Send Disable Event Type Command for ERROR messages	"
write ";             Telemetry indicates ERROR messages disabled."
write ";             cEVS3000"
write ";             cEVS3004"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_425.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_DISEVENTTYPE ERROR

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Disable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_425a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
ENDDO

evs3000Passed = TRUE
write "" 
write "     ERROR message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.2.6: Send ERROR message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS	"
write ";             Messages not processed by CFE_EVS."
write ";             Verify by telemetry ERROR message counts not incrementing."
write ";             cEVS3000"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_426.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_error

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_426a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.2.7: Send Disable Event Type Command for CRITICAL messages"
write ";             Telemetry indicates CRITICAL messages disabled."
write ";             cEVS3000"
write ";             cEVS3004"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_427.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_DISEVENTTYPE CRIT

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Disable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_427a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
ENDDO

evs3000Passed = TRUE
write "" 
write "     CRITICAL message status"
write "     -----------------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.2.8: Send CRITICAL message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, CI_APP, TO_APP, TST_EVS	"
write ";             Messages not processed by CFE_EVS."
write ";             Verify by telemetry CRITICAL message counts not "
write ";	     incrementing."
write ";             cEVS3000"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_428.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_crit

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_428a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Disable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Disable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.3: Enable Event Type Command Test"
write ";***********************************************************************"
write "; Step 4.3.1: Send Enable Event Type Command for DEBUG messages	"
write ";             Telemetry indicates DEBUG messages enabled. "
write ";             cEVS3000 "
write ";             cEVS3004 "
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_431.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_ENAEVENTTYPE DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Enable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_431a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

evs3000Passed = TRUE
write "" 
write "     DEBUG message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.3.2: Send DEBUG message for each cFE application: CFE_SB,  "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS	 "
write ";             Messages processed by CFE_EVS. "
write ";             Expect event msgs. "
write ";             Verify by telemetry counts incrementing. "
write ";             cEVS3000 "
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_432.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_432a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [1,0,0,0,0,0,0,0,1]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.3.3: Send Enable Event Type Command for INFO messages	"
write ";             Telemetry indicates INFO messages enabled."
write ";             cEVS3000"
write ";             cEVS3004"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_433.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_ENAEVENTTYPE INFO

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Enable Event Type command did not increment CMDPC."
endif

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_433a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
ENDDO

evs3000Passed = TRUE
write "" 
write "     INFO message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.3.4: Send INFO message for each cFE application: CFE_SB, "
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS	"
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry message counts incrementing."
write ";             cEVS3000"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_434.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_info

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_434a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [2,1,1,1,1,1,1,0,1]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.3.5: Send Enable Event Type Command for ERROR messages"
write ";             Telemetry indicates ERROR messages enabled."
write ";             cEVS3000"
write ";             cEVS3004"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_435.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_ENAEVENTTYPE ERROR

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Enable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_435a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
ENDDO

evs3000Passed = TRUE
write "" 
write "     ERROR message status"
write "     --------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.3.6: Send ERROR message for each cFE application: CFE_SB, "
write ";             CFE_EVS, CFE_TIME, TST_EVS"
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry ERROR message counts incrementing."
write ";             cEVS3000"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_436.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_error

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_436a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [1,1,1,1,1,0,1,0,1]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 4.3.7: Send Enable Event Type Command for CRITICAL messages"
write ";             Telemetry indicates CRITICAL messages enabled."
write ";             cEVS3000"
write ";             cEVS3004"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_437.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit 
ENDDO

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_ENAEVENTTYPE CRIT

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command sent successfully."
else
  ut_setrequirements EVS_3000, "F"
  write "<*> Passed (3000) - Enable Event Type command did not increment CMDPC."
endif

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_437a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
ENDDO

evs3000Passed = TRUE
write "" 
write "     CRITICAL message status"
write "     -----------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type command."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type command."
endif

write ";***********************************************************************"
write "; Step 4.3.8: Send CRITICAL message for each cFE application: CFE_SB,"
write ";             CFE_ES, CFE_EVS, CFE_TIME, TST_EVS	"
write ";             Messages processed by CFE_EVS."
write ";             Verify by telemetry CRITICAL message counts incrementing."
write ";             cEVS3000"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_438.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt-2 DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_crit

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_438a.dat",hostCPU)
wait 5
 
FOR i = 1 to cfe_app_cnt-2 DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [1,0,0,0,0,0,0,0,1]

evs3000Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3000Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3000Passed = TRUE) then
  ut_setrequirements EVS_3000, "P"
  write "<*> Passed (3000) - Enable Event Type counts."
else
  ut_setrequirements EVS_3000, "F"
  write "<!> Failed (3000) - Enable Event Type counts."
endif

write ";***********************************************************************"
write "; Step 5.0: Event Message Generation Test"
write ";***********************************************************************"
write "; Step 5.1: ENABLE event generation for all message types. "
write ";***********************************************************************"

/$SC_$CPU_EVS_ENAEVENTTYPE DEBUG
wait 5
/$SC_$CPU_EVS_ENAEVENTTYPE INFO
wait 5
/$SC_$CPU_EVS_ENAEVENTTYPE ERROR
wait 5
/$SC_$CPU_EVS_ENAEVENTTYPE CRIT
wait 5

; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_51.dat", hostCPU)
wait 5
 
;event message types 
FOR i = 1 to cfe_app_cnt DO
  write " For app ", $sc_$cpu_EVS_AppData[i].AppName
  write " DEBUG = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
  write " INFO = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
  write " ERROR = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
  write " CRIT = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
  write " Evt msg types mask"
  write " CEID"
  write " ",$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Err, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Info, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

write ";***********************************************************************"
write "; Step 5.2: Disable Application Event Generation Command Test"
write ";***********************************************************************"
write "; Step 5.2.1: Send Disable Application Event Generation Command for "
write ";             each application. Telemetry indicates that events are "
write ";	     disabled for each application."
write ";             cEVS3004"
write ";             cEVS3008"
write ";             cEVS3018"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_521.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].ActiveFlag 
ENDDO

FOR i = 1 to cfe_app_cnt DO
  /$SC_$CPU_EVS_DISAPPEVGEN APPLICATION=cfe_applications[i]
  wait 5
ENDDO

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_521a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].ActiveFlag
ENDDO

local evs3008Passed = TRUE
write "" 
write "     APP EVENT message status"
write "     ------------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3008Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3008Passed = TRUE) then
  ut_setrequirements EVS_3008, "P"
  write "<*> Passed (3008) - Disable App Event Generation command."
else
  ut_setrequirements EVS_3008, "F"
  write "<!> Failed (3008) - Disable App Event Generation command."
endif

write ";***********************************************************************"
write "; Step 5.2.2: Send a DEBUG, INFO, ERROR, and CRIT command for cFE app"
write ";             Verify that app's commands are not processed."
write ";             cEVS3008"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_522.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

s $sc_$cpu_evs_send_info

s $sc_$cpu_evs_send_error

s $sc_$cpu_evs_send_crit

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_522a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [0,0,0,0,0,0,0,0,0]

evs3008Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3008Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3008Passed = TRUE) then
  ut_setrequirements EVS_3008, "P"
  write "<*> Passed (3008) - Disable App Event Generation counts."
else
  ut_setrequirements EVS_3008, "F"
  write "<!> Failed (3008) - Disable App Event Generation counts."
endif

write ";***********************************************************************"
write "; Step 5.3: Enable Application Event Generation Command Test "
write ";***********************************************************************"
write "; Step 5.3.1: Send Enable Application Event Generation Command for each"
write ";             application. Telemetry indicates that events are enabled "
write ";             for each app. "
write ";             cEVS3004 "
write ";             cEVS3008 "
write ";             cEVS3018 "
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_531.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].ActiveFlag 

  /$SC_$CPU_EVS_ENAAPPEVGEN APPLICATION=cfe_applications[i]
  wait 5
ENDDO

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_531a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_event_type_status[i] = p@$sc_$cpu_EVS_AppData[i].ActiveFlag
ENDDO

evs3008Passed = TRUE
write "" 
write "     APP EVENT message status"
write "     ------------------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_event_type_status[i] = current_event_type_status[i]) THEN
    event_type_status_flag[i] = "*** FAILED ***"
    evs3008Passed = FALSE
  ELSE
    event_type_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_event_type_status[i] , " -> " , current_event_type_status[i], "   ", event_type_status_flag[i]
ENDDO

if (evs3008Passed = TRUE) then
  ut_setrequirements EVS_3008, "P"
  write "<*> Passed (3008) - Enable App Event Generation command."
else
  ut_setrequirements EVS_3008, "F"
  write "<!> Failed (3008) - Enable App Event Generation command."
endif

write ";***********************************************************************"
write "; Step 5.3.2: Send a DEBUG, INFO, ERROR, and CRIT command for cFE app"
write ";             Verify that app's commands are processed."
write ";             cEVS3008"
write ";***********************************************************************"
; Retrieve application data file
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_532.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

s $sc_$cpu_evs_send_info

s $sc_$cpu_evs_send_error

s $sc_$cpu_evs_send_crit

; Retrieve application data file
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_532a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [2,2,2,2,2,1,2,0,4]

evs3008Passed = TRUE
write "" 
write "     Msg Sent Counter"
write "     ----------------"
;; cfe_app_cnt-2 becasue of SCH and TST_EVS apps
FOR i = 1 to cfe_app_cnt-2 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3008Passed = FALSE
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3008Passed = TRUE) then
  ut_setrequirements EVS_3008, "P"
  write "<*> Passed (3008) - Enable App Event Generation counts."
else
  ut_setrequirements EVS_3008, "F"
  write "<!> Failed (3008) - Enable App Event Generation counts."
endif

;event message types 
FOR i = 1 to cfe_app_cnt DO
  write " For app ", $sc_$cpu_EVS_AppData[i].AppName
  write " DEBUG = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
  write " INFO = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Info
  write " ERROR = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Err
  write " CRIT = ", p@$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit
  write " Evt msg types mask"
  write " CEID"
  write " ",$sc_$cpu_EVS_AppData[i].EvtTypeAF.Crit, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Err, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Info, $sc_$cpu_EVS_AppData[i].EvtTypeAF.Debug
ENDDO

write ";***********************************************************************"
write "; Step 6.0: Enable/Disable Ports Test"
write ";***********************************************************************"
write "; Step 6.1: Send invalid Enable Port Mask command. "
write ";           In cFE 4.2.0, the valid range for the mask is x'1' - x'f' "
write ";             cEVS3006 "
write ";             cEVS3017 "
write ";             cEVS3018 (Invalid Command Counter) "
write ";***********************************************************************"
local output_port_mask = 0

output_port_mask = $SC_$CPU_EVS_OUTPUTPORT 

write " Prior to sending enable port command with mask of zero, Output Port Mask = ", %bin($SC_$CPU_EVS_OUTPUTPORT, 4)

; Command to enable port with mask value of zero
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_ENAPORTMASK BITMASK=X'0'  

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3017, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3006;3017;3018) - Enable Port Mask command failed as expected."
ELSE
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3007, "F"
  ut_setrequirements EVS_3018, "F"
  write "<*> Passed (3006;3007;3018) - Invalid Enable Port Mask command did not increment CMDEC."
ENDIF
 
IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Expected Event Message rcvd"
ELSE
  write "<!> Failed - Expected Error Event Message was not generated."
ENDIF 

write " After sending enable port command with mask of zero, Output Port Mask = ", %bin($SC_$CPU_EVS_OUTPUTPORT, 4) 

write ";***********************************************************************"
write "; Step 6.2: Send invalid Disable Port Mask command."
write ";           In cFE 4.2.0, the valid range for the mask is x'1' - x'f' "
write ";           cEVS3006 "
write ";           cEVS3017 "
write ";           cEVS3018 (Invalid Command Counter) "
write ";***********************************************************************"
output_port_mask = $SC_$CPU_EVS_OUTPUTPORT 

write " Prior to sending disable port command with mask of zero, Output Port Mask = ", $SC_$CPU_EVS_OUTPUTPORT 

; Command to disable port with mask value of zero
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_DISPORTMASK BITMASK=X'0' 

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3017, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3006;3017;3018) - Disable Port Mask command failed as expected."
ELSE
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3007, "F"
  ut_setrequirements EVS_3018, "F"
  write "<*> Passed (3006;3007;3018) - Invalid Disable Port Mask command did not increment CMDEC."
ENDIF
 
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Expected Event Message rcvd"
ELSE
  write "<!> Failed - Expected Error Event Message was not generated."
ENDIF 

write " After sending disable port command with mask of zero, Output Port Mask = ", %bin($SC_$CPU_EVS_OUTPUTPORT,4)

write ";***********************************************************************"
write "; Step 6.3: Disable Ports Command Test"
write ";***********************************************************************"
write "; Step 6.3.1: Send Disable Port Command for each valid port."
write ";             Telemetry indicates that all ports have been disabled."
write ";             cEVS3017"
write ";             cEVS3018 (Event Message Output Port Enable Status)"
write ";***********************************************************************"
previous_port_mask = $SC_$CPU_EVS_OUTPUTPORT

FOR i = 1 to cfe_port_cnt DO
  write "     Sending DISABLE PORT command to ", cfe_ports[i]
  /$SC_$CPU_EVS_DISPORT {cfe_ports[i]}
  wait 5
ENDDO

IF ($SC_$CPU_EVS_OUTPUTPORT = b'0000') THEN
  port_mask_status_flag = "OK"
  ut_setrequirements EVS_3017, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3017;3018) - Disable Port command - All ports disabled"
ELSE
  port_mask_status_flag = "*** FAILED ***"
  ut_setrequirements EVS_3017, "F"
  ut_setrequirements EVS_3018, "F"
  write "<!> Failed (3017;3018) - Disable Port command did not disable all ports"
ENDIF

write
write "     Port Mask"
write "     ---------"
write "     ", %bin(previous_port_mask,4), " -> ", %bin($SC_$CPU_EVS_OUTPUTPORT,4), "     ", port_mask_status_flag

write ";***********************************************************************"
write "; Step 6.3.2: Send events for each cFE application to each disabled port"
write ";             Verify that app's event not processed."
write ";             cEVS3017"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_632.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

s $sc_$cpu_evs_send_info

s $sc_$cpu_evs_send_error

s $sc_$cpu_evs_send_crit

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_632a.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [2,2,2,2,2,1,2,0,4]

write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

write ";***********************************************************************"
write "; Step 6.4: Enable Ports Command Test"
write ";***********************************************************************"
write "; Step 6.4.1: Send Enable Port Command"
write ";             Telemetry indicates that port has been enabled."
write ";             cEVS3017"
write ";             cEVS3018 (Event Message Output Port Enable Status)"
write ";***********************************************************************"
previous_port_mask = $SC_$CPU_EVS_OUTPUTPORT

FOR i = 1 to cfe_port_cnt DO
  write "     Sending ENABLE PORT command to ", cfe_ports[i]
  /$SC_$CPU_EVS_ENAPORT {cfe_ports[i]}
  wait 5
ENDDO

IF ($SC_$CPU_EVS_OUTPUTPORT = b'1111') THEN 
  port_mask_status_flag = "OK"
  ut_setrequirements EVS_3017, "P"
  ut_setrequirements EVS_3018, "P"
  write "<*> Passed (3017;3018) - Enable Port command - All ports enabled"
ELSE
  port_mask_status_flag = "*** FAILED ***"
  ut_setrequirements EVS_3017, "F"
  ut_setrequirements EVS_3018, "F"
  write "<!> Failed (3017;3018) - Enable Port command did not enable all ports"
ENDIF

write
write "     Port Mask"
write "     ---------"
write "     ", %bin(previous_port_mask,4), " -> ", %bin($SC_$CPU_EVS_OUTPUTPORT,4), "     ", port_mask_status_flag

write ";***********************************************************************"
write "; Step 6.4.2: Send an event to enabled ports"
write ";             Verify that app's event is processed."
write ";             cEVS3017"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_642.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

s $sc_$cpu_evs_send_debug

s $sc_$cpu_evs_send_info

s $sc_$cpu_evs_send_error

s $sc_$cpu_evs_send_crit

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_642a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

evt_msg_sent_ctr_offset = [2,2,2,2,2,1,2,0,4]

write "" 
write "     Msg Sent Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-1 DO
  IF (previous_app_msg_sent_ctr[i] + evt_msg_sent_ctr_offset[i] = current_app_msg_sent_ctr[i]) THEN
    msg_ctr_status_flag[i] = "OK"
  ELSE
    msg_ctr_status_flag[i] = "*** FAILED ***"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

; Set ports back to initial configuration - only PORT_ONE is enabled
write "     Setting Ports back to initial configuration (Port 1 Enabled)"
write

FOR i = 2 to cfe_port_cnt DO
  /$SC_$CPU_EVS_DISPORT {cfe_ports[i]}
  wait 5
ENDDO

write "Port mask: ",%bin($SC_$CPU_EVS_OUTPUTPORT,4)

write ";***********************************************************************"
write "; Step 6.5: Test the number of ports supported. Try to Enable and "
write "; 	   and disable an invalid port."
write ";***********************************************************************"
;; Setup to capture event
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

;; Send an invalid ENAPORT command
;; CPU1 is the default
local rawcmd = "1801C00000030B281000"

;;if ("$CPU" = "CPU2") then
;;  rawcmd = "1821C00000030B081000"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1841C00000030B681000"
;;endif

ut_sendrawcmd "$SC_$CPU_EVS", (rawcmd)

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3300, "P"
  write "<*> Passed (3300) - Enable Port command failed as expected"
else
  ut_setrequirements EVS_3300, "F"
  write "<!> Failed (3300) - Enable Port command with an invalid port did not increment the error counter"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  ut_setrequirements EVS_3300, "P"
  write "<*> Passed (3300) - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  ut_setrequirements EVS_3300, "F"
  write "<!> Failed (3300) - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_INVALID_BITMASK_EID
endif

;; Send an invalid DISABLE PORT command
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

;; CPU1 is the default
rawcmd = "1801C00000030C2A1000"

;;if ("$CPU" = "CPU2") then
;;  rawcmd = "1821C00000030C091000"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1841C00000030C691000"
;;endif

ut_sendrawcmd "$SC_$CPU_EVS", (rawcmd)

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3300, "P"
  write "<*> Passed (3300) - Disable Port command failed as expected"
else
  ut_setrequirements EVS_3300, "F"
  write "<!> Failed (3300) - Disable Port command with an invalid port did not increment the error counter"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  ut_setrequirements EVS_3300, "P"
  write "<*> Passed (3300) - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  ut_setrequirements EVS_3300, "F"
  write "<!> Failed (3300) - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_INVALID_BITMASK_EID
endif

write ";***********************************************************************"
write "; Step 7.0: Reset Application Event Msg Sent Counters Tests"
write ";***********************************************************************"
write "; Step 7.1: Send an invalid Reset Application Event Msg Sent Counter "
write ";           Command. FSW indicates invalid command. Command Error count "
write ";	   increments. Command Processed counter does not change."
write ";           cEVS3018 (Invalid Command Counter)"
write ";           cEVS3018 (Application Event Message Sent Counter)"
write ";           cEVS3004"
write ";           cEVS3009"
write ";***********************************************************************"
;; Setup for the event to capture
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_NOAPPIDFOUND_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_RSTAPPCTRS APPLICATION="CFE_"

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3009, "P"
  write "<*> Passed (3006;3009) - Reset App Counters command failed as expected"
else
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3009, "F"
  write "<!> Failed (3006;3009) - Invalid Reset App Counters command did not increment the CMDEC"
endif

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  ut_setrequirements EVS_3009, "P"
  write "<*> Passed (3009) - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  ut_setrequirements EVS_3009, "F"
  write "<!> Failed (3009) - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_NOAPPIDFOUND_EID
ENDIF

write ";***********************************************************************"
write "; Step 7.2: Send the Reset Application Command for each cFE app:"
write ";	   CFE_SB, CFE_ES, CFE_EVS, CFE_TIME, TST_EVS	"
write ";           cEVS3009"
write ";           cEVS3004"
write ";           cEVS3018 (Application Event Message Sent Counter)"
write ";***********************************************************************"
start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_72.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 

  /$SC_$CPU_EVS_RSTAPPCTRS APPLICATION=cfe_applications[i]
  wait 5
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_72a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_app_msg_sent_ctr[i] = p@$sc_$cpu_EVS_AppData[i].EventCounter 
ENDDO

local evs3009Passed = TRUE
write "" 
write "     App EMS Counter"
write "     ----------------"
FOR i = 1 to cfe_app_cnt-2 DO
  IF (current_app_msg_sent_ctr[i] <> 0) AND (cfe_applications[i] <> "CFE_EVS") THEN
    msg_ctr_status_flag[i] = "*** FAILED ***"
    evs3009Passed = FALSE
  ELSE
    msg_ctr_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_app_msg_sent_ctr [i] , " -> " , current_app_msg_sent_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3009Passed = TRUE) then
  ut_setrequirements EVS_3009, "P"
  write "<*> Passed (3009) - Reset App Counters command."
else
  ut_setrequirements EVS_3009, "F"
  write "<!> Failed (3009) - Reset App Counters command."
endif

write ";***********************************************************************"
write "; Step 8.0: Set / Reset Binary Filters Test "
write ";***********************************************************************"
write "; Step 8.1: Send an invalid Binary Filter Mask Commands"
write ";           Valid Mask range  x'0' to x'ffff'"
write ";           cEVS3006"
write ";           cEVS3018 (Invalid Command Counter)"
write ";***********************************************************************"
write "*** NOTE: Cannot test for Invalid Event ID / Invalid Mask."
write "***       Valid Range is '0' - 'ffff'. Both Event ID and"
write "***       and Event Mask are defined to be unsigned integers"
write "***       (16 bits) and selecting a value outside the valid "
write "***       range would exceed the bit allocation."
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_NOAPPIDFOUND_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION="CFE_" EVENT_ID=X'ffff' FILTERMASK=X'ffff'

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3012, "P"
  write "<*> Passed (3006;3012) - Set Binary Filter Mask command failed as expected"
else
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3012, "F"
  write "<!> Failed (3006;3012) - Invalid Set Binary Filter Mask command did not increment the CMDEC"
endif

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_NOAPPIDFOUND_EID
ENDIF

write ";***********************************************************************"
write "; Step 8.2: Send invalid Reset All Filters Commands for a non-existing"
write ";           Application. FSW indicates invalid command. Command Error "
write ";	   count increments.Command Processed counter does not change."
write ";           cEVS3006"
write ";           cEVS3011"
write ";           cEVS3018 (Invalid Command Counter)"
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_NOAPPIDFOUND_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_RSTALLFLTRS APPLICATION="CFE_"

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3011, "P"
  write "<*> Passed (3006;3011) - Reset All Binary Filter counters command failed as expected"
else
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3011, "F"
  write "<!> Failed (3006;3011) - Invalid Reset All Binary Filter Counters command did not increment the CMDEC"
endif

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_NOAPPIDFOUND_EID
ENDIF

write ";***********************************************************************"
write "; Step 9.0: Reset Binary Filter Counter Tests "
write ";***********************************************************************"
write "; Step 9.1: Send invalid Reset Binary Filter Counter command "
write ";             Select cFE application that does not exist. "
write ";             FSW indicates invalid command. Command Error count " 
write ";             increments. Command Processed counter does not change."
write ";             cEVS3006"
write ";             cEVS3010"
write ";             cEVS3018 (Invalid Command Counter)"
write ";***********************************************************************"
write "*** NOTE: Cannot test for Invalid Event ID. Valid Range is"
write "***       '0' - 'ffff'. Event ID is defined to be unsigned integer "
write "***       (16 bits) and selecting a value outside the valid range "
write "***       would exceed the bit allocation."
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_NOAPPIDFOUND_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION="CFE_" EVENT_ID=X'FFFF'

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3010, "P"
  write "<*> Passed (3006;3010) - Reset All Binary Filter counters command failed as expected"
else
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3010, "F"
  write "<!> Failed (3006;3010) - Invalid Reset All Binary Filter Counters command did not increment the CMDEC"
endif

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_NOAPPIDFOUND_EID
ENDIF

write ";***********************************************************************"
write "; Step 9.2: Send Reset Binary Filter Counter command for each cFE app."
write ";           cEVS3004"
write ";           cEVS3010"
write ";***********************************************************************"
start get_file_to_cvt (ramDir, "cfe_evs_app.dat", "cfe_evs_app_92.dat", hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   previous_bin_fltr_ctr[i] = p@$sc_$cpu_EVS_AppData[i].BinFltr[1].Ctr

;;  /$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION=$sc_$cpu_EVS_AppData[i].AppName EVENT_ID=X'1'
  /$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION=$sc_$cpu_EVS_AppData[i].AppName EVENT_ID=$sc_$cpu_EVS_AppData[i].BinFltr[1].EvtId
  wait 5
ENDDO

start get_file_to_cvt (ramDir,"cfe_evs_app.dat","cfe_evs_app_92a.dat",hostCPU)
wait 5

FOR i = 1 to cfe_app_cnt DO
   current_bin_fltr_ctr[i] = p@$sc_$cpu_EVS_AppData[i].BinFltr[1].Ctr
ENDDO

local evs3010Passed = TRUE
write ""
write "    BinFltr Counter values"
write "    ----------------------" 
FOR i = 1 to cfe_app_cnt DO
  IF ((current_bin_fltr_ctr[i] <> 0) AND bin_fltr_msgs[i]) THEN
    IF (current_bin_fltr_ctr[i] < previous_bin_fltr_ctr[i]) THEN
      msg_ctr_status_flag[i] = "OK"
    ELSE
      msg_ctr_status_flag[i] = "*** FAILED ***"
      evs3010Passed = FALSE
    ENDIF
  ELSE
    msg_ctr_status_flag[i] = "OK"
  ENDIF
  write "     ", cfe_applications[i], " - ", previous_bin_fltr_ctr [i] , " -> " , current_bin_fltr_ctr [i], "     ", msg_ctr_status_flag[i]
ENDDO

if (evs3010Passed = TRUE) then
  ut_setrequirements EVS_3010, "P"
  write "<*> Passed (3010) - Reset Binary Filter Counter command."
else
  ut_setrequirements EVS_3010, "F"
  write "<!> Failed (3010) - Reset Binary Filter Counter command."
endif

write ";***********************************************************************"
write "; Step 10.0: Reset Counters Test "
write ";***********************************************************************"
write "; Step 10.1: Check the message counters to ensure that they are not 0 "
write ";	    before sending the Reset Counters Command."
write ";            Verify that the CommandCounter, CommandErrCounter, "
write ";            MessagesendCounter, MessageTrunkCounter, and "
write ";	    UnregisteredAppCounter have non-zero values. "
write ";***********************************************************************"
if ($SC_$CPU_EVS_CMDPC = 0) then
  /$SC_$CPU_EVS_NOOP
endif

if ($SC_$CPU_EVS_CMDEC = 0) then
  /$SC_$CPU_EVS_RSTALLFLTRS APPLICATION="CFE_"
endif

if ($SC_$CPU_EVS_MSGSENTC = 0) then
  /$SC_$CPU_EVS_NOOP
endif

if ($SC_$CPU_EVS_MSGTRUNC = 0) then
  /$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId = "6" Iters = "1" Milliseconds = "10"
  ut_tlmwait $SC_$CPU_EVS_MSGTRUNC 1
endif

if ($SC_$CPU_EVS_UNREGAPPC = 0) then
  write "Unregister the TST_EVS test application"
  /$SC_$CPU_TST_EVS_UNREGISTER
  wait 5

  /$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId = "1" Iters = "1" Milliseconds = "15"
  ut_tlmwait $SC_$CPU_EVS_UNREGAPPC 1
endif

write ";***********************************************************************"
write "; Step 10.2: Send the Reset Counters Command.  Telemetry indicates that "
write "; 	    CommandCounter, CommandErrCounter, MessagesendCounter, "
write ";	    MessageTrunkCounter, and UnregisteredAppCounter have zero "
write ";	    values."
write ";            cEVS3003"
write ";            cEVS3018"
write ";***********************************************************************"
local previousCommandCounter
local previousCommandErrCounter
local previousMessageSendCounter
local previousMessageTrunkCounter
local previousUnregisteredAppCounter
local CounterStatusFlag

local cfeCountersCnt = 5
local cfeCounters[cfeCountersCnt] = ["CommandCounter", "CommandErrCounter", "MessageSendCounter", "MessageTrunkCounter", "UnregisteredAppCounter"]
local cfeCountersM[cfeCountersCnt] = ["CMDPC", "CMDEC", "MSGSENTC", "MSGTRUNC", "UNREGAPPC"]

previousCommandCounter = $SC_$CPU_EVS_CMDPC
previousCommandErrCounter = $SC_$CPU_EVS_CMDEC 
previousMessageSendCounter = $SC_$CPU_EVS_MSGSENTC 
previousMessageTrunkCounter = $SC_$CPU_EVS_MSGTRUNC 
previousUnregisteredAppCounter = $SC_$CPU_EVS_UNREGAPPC

; Set evt msg type DEBUG status to disable to prevent 
; generation of evt msg when reset ctr command is received

/$SC_$CPU_EVS_DISEVENTTYPE DEBUG 
wait 5

/$SC_$CPU_EVS_RESETCTRS
wait 5

FOR i = 1 to cfeCountersCnt DO
  IF {"$SC_$CPU_EVS_" & cfeCountersM[i]} = 0 THEN
    CounterStatusFlag = "OK"
  ELSE
    CounterStatusFlag = "*** FAILED ***"
  ENDIF 
  write "     ", cfeCounters[i]," = ",{"previous"& cfeCounters[i]}, " -> ", {"$SC_$CPU_EVS_" & cfeCountersM[i]}, "     ", CounterStatusFlag
ENDDO
write
  
IF ($SC_$CPU_EVS_CMDPC = 0) AND ($SC_$CPU_EVS_CMDEC = 0) AND ;;
   ($SC_$CPU_EVS_MSGSENTC = 0) AND ($SC_$CPU_EVS_MSGTRUNC = 0) AND ;;
   ($SC_$CPU_EVS_UNREGAPPC = 0) THEN
  ut_setrequirements EVS_3003, "P"
  write "<*> Passed (3003) - Reset Counters command."
ELSE
  ut_setrequirements EVS_3003, "F"
  write "<*> Passed (3003) - At least one counter was not reset."
ENDIF

/$SC_$CPU_EVS_ENAEVENTTYPE DEBUG 
wait 5

write ";***********************************************************************"
write "; Step 11.0: Log Mode Tests"
write ";***********************************************************************"
write "; Step 11.1: Send invalid Log Mode command. The FSW indicates invalid "
write ";	    command. The Command Error count increments and the "
write ";            Command Processed counter does not change."
write ";            cEVS3006 "
write ";            cEVS3018 (Invalid Command Counter) "
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_LOGMODE_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

;; CPU1 is the default
rawcmd = "1801C000000313000200"

;;if ("$CPU" = "CPU2") then
;;  rawcmd = "1821C000000313000200"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1841C000000313000200"
;;endif

ut_sendrawcmd "$SC_$CPU_EVS", (rawcmd)

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  ut_setrequirements EVS_3006, "P"
  ut_setrequirements EVS_3014, "P"
  write "<*> Passed (3006;3014) - Set Log Mode command failed as expected"
else
  ut_setrequirements EVS_3006, "F"
  ut_setrequirements EVS_3014, "F"
  write "<!> Failed (3006;3014) - Set Log Mode command with an invalid mode did not increment the CMDEC"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  ut_setrequirements EVS_3006, "P"
  write "<*> Passed (3006) - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  ut_setrequirements EVS_3006, "F"
  write "<!> Failed (3006) - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_LOGMODE_EID
endif

write ";***********************************************************************"
write "; Step 12.0: Enable and Disable Event Type Mask Tests"
write ";***********************************************************************"
write "; Step 12.1: Send invalid Enable command. The FSW should generate an "
write "; 	    Error event message."
write ";***********************************************************************"
;; Setup to capture event
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC+1

/$SC_$CPU_EVS_ENAEVENTTYPEMASK BITMASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  write "<*> Passed - Enable Event Type Mask command failed as expected"
else
  write "<!> Failed - Enable Event Type Mask command with an invalid mask did not increment the error counter"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_INVALID_BITMASK_EID
endif

write ";***********************************************************************"
write "; Step 12.2: Send invalid Disable command. The FSW should generate an "
write "; 	    Error event message."
write ";***********************************************************************"
;; Setup to capture event
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_INVALID_BITMASK_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC+1

/$SC_$CPU_EVS_DISEVENTTYPEMASK BITMASK=X'0'
wait 5

ut_tlmwait $SC_$CPU_EVS_CMDEC {errcnt}
IF (UT_TW_status = UT_Success) THEN
  write "<*> Passed - Disable Event Type Mask command failed as expected"
else
  write "<!> Failed - Disable Event Type Mask command with an invalid mask did not increment the error counter"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ERR_INVALID_BITMASK_EID
endif

write ";***********************************************************************"
write "; Step 12.3: Send a valid Disable command. "
write ";***********************************************************************"
;; Setup to capture event
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_DISEVTTYPE_EID, DEBUG, 1

local cmdcnt = $SC_$CPU_EVS_CMDPC+1

/$SC_$CPU_EVS_DISEVENTTYPEMASK BITMASK=X'2'
wait 5

ut_tlmwait $SC_$CPU_EVS_CMDPC {cmdcnt}
IF (UT_TW_status = UT_Success) THEN
  write "<*> Passed - Disable Event Type Mask command"
else
  write "<!> Failed - Enable Event Type Mask command did not increment the command counter"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_DISEVTTYPE_EID
endif

write ";***********************************************************************"
write "; Step 12.4: Send a valid Enable command. "
write ";***********************************************************************"
;; Setup to capture event
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ENAEVTTYPE_EID, DEBUG, 1

cmdcnt = $SC_$CPU_EVS_CMDPC + 1

/$SC_$CPU_EVS_ENAEVENTTYPEMASK BITMASK=X'2'
wait 8

ut_tlmwait $SC_$CPU_EVS_CMDPC {cmdcnt}
IF (UT_TW_status = UT_Success) THEN
  write "<*> Passed - Enable Event Type Mask command"
else
  write "<!> Failed - Enable Event Type Mask command did not increment the command counter"
endif

;; Check for the failure event
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Error event msg ",$SC_$CPU_evs_eventid," received"
else
  write "<!> Failed - Event msg ",$SC_$CPU_evs_eventid," received. Expected event msg ",CFE_EVS_ENAEVTTYPE_EID
endif

write ";***********************************************************************"
write "; Step 13.0: Send the WRITEAPPDATA2FILE command without a path "
write ";	    specification for the filename."
write ";***********************************************************************"
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_ERR_CRDATFILE_EID, ERROR, 1

errcnt = $SC_$CPU_EVS_CMDEC + 1

/$SC_$CPU_EVS_WRITEAPPDATA2FILE APPDATAFILENAME="nopathname"
wait 10

ut_tlmwait $SC_$CPU_EVS_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - WriteAppData command failed as expected."
else
  write "<!> Failed - WriteAppData command did not increment the CMDEC."
endif

;; Look for expected event #1
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Expected Event Message ", CFE_EVS_ERR_CRDATFILE_EID, " not received."
endif

;*******************************************************************************
; Requirements Report
;*******************************************************************************
ReqReport:
write
write "Requirement(s) Report"
write "---------------------"
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO

TST_APP_FAILED:
END_TEST:

validation on
end_it:

drop ut_req_array_size
drop ut_requirement

endproc
