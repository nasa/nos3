PROC $sc_$cpu_hk_gencmds
;*******************************************************************************
;  Test Name:  hk_gencmds
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Housekeeping (HK) general 
;       commands function properly. The HK_NoOp and HK_Reset commands will be 
;       tested as well as invalid commands and an application reset to see if 
;       the HK application behaves appropriately. 
;
;  Requirements Tested
;    HK1000	Upon receipt of a No-Op command, HK shall increment the HK 
;               Valid Command Counter and generate an event message.
;    HK1001	Upon receipt of a Reset command, HK shall reset the following 
;               housekeeping variables to a value of zero:
;                    a)	HK Valid Command Counter
;                    b)	HK Command Rejected Counter
;                    c)	Number of Output Messages Sent
;                    d)	Missing Data Counter
;    HK1002	For all HK commands, if the length contained in the message 
;               header is not equal to the expected length, HK shall reject 
;               the command.
;    HK1003	If HK accepts any command as valid, HK shall execute the 
;               command, increment the HK Valid Command Counter and issue an
;		event message
;    HK1004	If HK rejects any command, HK shall abort the command execution,
;               increment the HK Command Rejected Counter and issue an error 
;               event message
;    HK3000	HK shall generate a housekeeping message containing the 
;		following:
;                    a)	Valid Command Counter
;                    b)	Command Rejected Counter
;                    c)	Number of Output Messages Sent
;                    d)	Missing Data Counter
;    HK4000	Upon initialization of the HK Application, HK shall initialize
;               the following data to Zero
;                    a)	Valid Command Counter
;                    b)	Command Rejected Counter
;                    c)	Number of Output Messages Sent
;                    d)	Missing Data Counter
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands. 
;	The HK commands and TLM items exist in the GSE database. 
;	A display page exists for the HK Housekeeping telemetry packet. 
;	HK Test application loaded and running
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date	        Name		Description
;	05/02/08	Barbie Medina	Original Procedure.
;	07/02/10	Walt Moleski    Updated to use the default table name
;                                       and to call $sc_$cpu_hk_start_apps
;	03/08/11	Walt Moleski	Added variable for app name
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
;       hk_createcopytable1  Sets up the copy table files for the testing
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
#include "hk_platform_cfg.h"
#include "hk_events.h"
#include "tst_hk_events.h"

%liv (log_procedure) = logging

#define HK_1000		0
#define HK_1001		1
#define HK_1002		2
#define HK_1003		3
#define HK_1004		4
#define HK_3000		5
#define HK_4000		6

global ut_req_array_size = 6
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["HK_1000", "HK_1001", "HK_1002", "HK_1003", "HK_1004", "HK_3000", "HK_4000"]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL outmsgid

local HKAppName = "HK"

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

;; Display the pages
page $SC_$CPU_HK_HK
page $SC_$CPU_TST_HK_HK

write ";*********************************************************************"
write ";  Step 1.2: Creating the copy table used for testing and upload it"
write ";********************************************************************"
s $SC_$CPU_hk_copytable1

;; Parse the filename configuration parameters for the default table filenames
local tableFileName = HK_COPY_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo

write "==> Default Copy Table filename = '",tableFileName,"'"

s ftp_file("CF:0/apps", "hk_cpy_tbl.tbl", tableFileName, "$CPU", "P")

write ";*********************************************************************"
write ";  Step 1.3:  Start the Housekeeping (HK) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
s $sc_$cpu_hk_start_apps("1.3")
wait 5

write ";*********************************************************************"
write ";  Step 1.4: Verify that the HK Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Check the HK tlm items to see if they are 0 or NULL
;; the TST_HK application sends its HK packet
if ($SC_$CPU_HK_CMDPC = 0) AND ($SC_$CPU_HK_CMDEC = 0) AND ;;
   ($SC_$CPU_HK_CMBPKTSSENT = 0) AND ($SC_$CPU_HK_MISSDATACTR = 0) THEN
  write "<*> Passed (4000) - Housekeeping telemetry initialized properly."
  ut_setrequirements HK_4000, "P"
else
  write "<!> Failed (4000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC                    = ",$SC_$CPU_HK_CMDPC
  write "  CMDEC                    = ",$SC_$CPU_HK_CMDEC
  write "  Combined Packets Sent    = ",$SC_$CPU_HK_CMBPKTSSENT
  write "  Missing Data Counter     = ",$SC_$CPU_HK_MISSDATACTR
  write "  Memory Pool Handle       = ",$SC_$CPU_HK_MEMPOOLHNDL
  ut_setrequirements HK_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.5: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the HK application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=HKAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0: HK General Commanding tests."
write ";*********************************************************************"
write ";  Step 2.1: Send a valid No-Op command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_NOOP_CMD_EID, "INFO", 1

ut_sendcmd "$SC_$CPU_HK_NOOP"
if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1000;1003;3000) - NOOP command sent properly."
  ut_setrequirements HK_1000, "P"
  ut_setrequirements HK_1003, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1000;1003;3000) - NOOP command not sent properly (", ut_sc_status, ")."
  ut_setrequirements HK_1000, "F"
  ut_setrequirements HK_1003, "F"
  ut_setrequirements HK_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1000;1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1000, "P"
  ut_setrequirements HK_1003, "P"
else
  write "<!> Failed (1000;1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_NOOP_CMD_EID, "."
  ut_setrequirements HK_1000, "F"
  ut_setrequirements HK_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send a No-Op command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_CMD_LEN_ERR_EID, "ERROR", 1

local errcnt = $SC_$CPU_HK_CMDEC + 1

;; CPU1 is the default
rawcmd = "189ac000000200BD"

if ("$CPU" = "CPU2") then
  rawcmd = "199ac000000200BD"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1a9ac000000200BD"
endif

ut_sendrawcmd "$SC_$CPU_HK", (rawcmd)

ut_tlmwait $SC_$CPU_HK_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;3000) - Command Rejected Counter incremented."
  ut_setrequirements HK_1002, "P"
  ut_setrequirements HK_1004, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1002;1004;3000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements HK_1002, "F"
  ut_setrequirements HK_1004, "F"
  ut_setrequirements HK_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_CMD_LEN_ERR_EID, "."
  ut_setrequirements HK_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Send a valid ResetCtrs command."
write ";*********************************************************************"
;; Check that the counters are not 0
if ($SC_$CPU_HK_CMDPC = 0) then
  ;; Send a NOOP command
  /$SC_$CPU_HK_NOOP
  wait 5
endif

if ($SC_$CPU_HK_CMDEC = 0) then
  /raw {rawcmd}
  wait 5
endif

if ($SC_$CPU_HK_CMBPKTSSENT = 0) or ($SC_$CPU_HK_MISSDATACTR = 0) then
   ;;  using test application send output message 1 command to HK
   ;; CPU1 is the default
   outmsgid = 0x89c

   if ("$CPU" = "CPU2") then
      outmsgid = 0x99c
   elseif ("$CPU" = "CPU3") then
      outmsgid = 0xa9c
   endif

  /$SC_$CPU_TST_HK_SENDOUTMSG MsgId=outmsgid Pad= 0
  wait 5
endif


;; Setup for the expected event
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_RESET_CNTRS_CMD_EID, "DEBUG", 1

/$SC_$CPU_HK_RESETCTRS
wait 5

ut_tlmwait $SC_$CPU_HK_CMDPC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;1003;3000) - Valid Command Counter was reset."
  ut_setrequirements HK_1001, "P"
  ut_setrequirements HK_1003, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1001;1003;3000) - Valid Command Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements HK_1001, "F"
  ut_setrequirements HK_1003, "F"
  ut_setrequirements HK_3000, "F"
endif

ut_tlmwait $SC_$CPU_HK_CMDEC, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;3000) - Command Rejected Counter was reset."
  ut_setrequirements HK_1001, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1001;3000) - Command Rejected Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements HK_1001, "F"
  ut_setrequirements HK_3000, "F"
endif

write ";*********************************************************************"
write ";    CmbPktsSent may not be 0, not sure what it will be "
write ";    If step fails add check to see if it's less than before "
write ";*********************************************************************"
ut_tlmwait $SC_$CPU_HK_CMBPKTSSENT, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;3000) - Combined packets sent counter was reset."
  ut_setrequirements HK_1001, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1001;3000) - Combined packets sent counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements HK_1001, "F"
  ut_setrequirements HK_3000, "F"
endif

ut_tlmwait $SC_$CPU_HK_MISSDATACTR, 0
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1001;3000) - Missing Data Counter was reset."
  ut_setrequirements HK_1001, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1001;3000) - Missing Data Counter was NOT reset (",ut_tw_status,")."
  ut_setrequirements HK_1001, "F"
  ut_setrequirements HK_3000, "F"
endif


if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1001, "P"
else
  write "<!> Failed (1001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_RESET_CNTRS_CMD_EID, "."
  ut_setrequirements HK_1001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Send a ResetCtrs command with an invalid command length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_CMD_LEN_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HK_CMDEC + 1

;; CPU1 is the default
rawcmd = "189ac000000201BC"

if ("$CPU" = "CPU2") then
  rawcmd = "199ac000000201BC"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1a9ac000000201BC"
endif

ut_sendrawcmd "$SC_$CPU_HK", (rawcmd)

ut_tlmwait $SC_$CPU_HK_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004;3000) - Command Rejected Counter incremented."
  ut_setrequirements HK_1002, "P"
  ut_setrequirements HK_1004, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1002;1004;3000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements HK_1002, "F"
  ut_setrequirements HK_1004, "F"
  ut_setrequirements HK_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_CMD_LEN_ERR_EID, "."
  ut_setrequirements HK_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.5: Send a Send Housekeeping command with an invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_MSG_LEN_ERR_EID, "ERROR", 1

;; CPU1 is the default
rawcmd = "189bc00000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "199bc00000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1a9bc00000000000"
endif

ut_sendrawcmd "$SC_$CPU_HK", (rawcmd)

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002;1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1002, "P"
  ut_setrequirements HK_1004, "P"
else
  write "<!> Failed (1002;1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_MSG_LEN_ERR_EID, "."
  ut_setrequirements HK_1002, "F"
  ut_setrequirements HK_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6: Send a Send Combined Packet command with an invalid length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_MSG_LEN_ERR_EID, "ERROR", 1

;; CPU1 is the default
rawcmd = "189cc00000000000"

if ("$CPU" = "CPU2") then
  rawcmd = "199cc00000000000"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1a9cc00000000000"
endif

ut_sendrawcmd "$SC_$CPU_HK", (rawcmd)

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_MSG_LEN_ERR_EID, "."
  ut_setrequirements HK_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Send an invalid HK command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {HKAppName}, HK_CC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_HK_CMDEC + 1

;; CPU1 is the default
rawcmd = "189ac0000001aa"

if ("$CPU" = "CPU2") then
  rawcmd = "199ac0000001aa"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1a9ac0000001aa"
endif

ut_sendrawcmd "$SC_$CPU_HK", (rawcmd)

ut_tlmwait $SC_$CPU_HK_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3000) - Command Rejected Counter incremented."
  ut_setrequirements HK_1004, "P"
  ut_setrequirements HK_3000, "P"
else
  write "<!> Failed (1004;3000) - Command Rejected Counter did not increment as expected."
  ut_setrequirements HK_1004, "F"
  ut_setrequirements HK_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements HK_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_CC_ERR_EID, "."
  ut_setrequirements HK_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $sc_$cpu_hk_gencmds                                  "
write ";*********************************************************************"
ENDPROC
