PROC $SC_$CPU_sb_cmds_err
;**********************************************************************
;  Test Name:  sb_cmds_err
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This scenario verifies that the flight software will reject
;       SB commands with bad data in the command fields.
;
;  Requirements Tested
;     cSB4004    Upon receipt of a Command, the cFE shall enable or disable
;		 routing the command specified message to the command
;		 specified pipe.
;     cSB4005    The cFE shall send an SB Message containing the
;                following housekeeping telemetry items:
;                  (b) command error counter.
;     cSB4305.6  If the SB Message is greater than the <MISSION_DEFINED,32767>
;                bytes then the cFE shall not send the message, issue an event
;                message and increment the message send error counter.
;     cSB4701    The cFE shall support a <PLATFORM _DEFINED> maximum
;                system packet size.
;
;  Prerequisite Conditions
;	Open event log and telemetry archive files.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	04/27/05	Mike Tong	Original Procedure.
;	12/19/05	Walt Moleski	Mods for cFE 3.1
;	08/30/06	Walt Moleski	Updated for cFE 3.3 regression testing
;	02/08/12	Walt Moleski	Replaced ut_setupevt with ut_setupevents
;
;  Arguments
;
;	Name			Description
;	None	
;
;  Procedures Called
;
;	Name			Description
;       ut_tlmwait      Wait for a specified telemetry point to update to
;                       a specified value. 
;       ut_sendcmd      Send commands to the spacecraft. Verifies command
;                       processed and command error counters.
;       ut_sendrawcmd   Send raw commands to the spacecraft. Verifies command
;                       processed and command error counters.
;       ut_pfindicate   Print the pass fail status of a particular requirement
;                       number.
;       ut_setupevents  Performs setup to verify that a particular event
;                       message was received by ASIST.
;
;  Expected Test Results and Analysis
;	This proc should run to completion with no unexpected errors.
;       There is no stopping for manual inputs.
;       The proc is straight-forward. Manual analysis is not needed.
;
;**********************************************************************
;  Define variables
;**********************************************************************

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_mission_cfg.h"
#include "cfe_sb_events.h"

#define SB_4004	 0
#define SB_4005	 1
#define SB_43056 2
#define SB_4701	 3

global ut_req_array_size = 3
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SB_4004", "SB_4005", "SB_4305.6", "SB_4701"]

LOCAL wait_time = 10
LOCAL rawcmd
LOCAL i
LOCAL cmdSize

write ";**********************************************************************"
write ";  Step 1:  Send a command with an invalid command code. "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_BAD_CMD_CODE_EID, "ERROR", 1

rawcmd = "18"&%hex($CPU_CMDAPID_BASE + SB_CMDAPID_OFF, 2)&"C0000001"    ; 6 bytes
;;rawcmd = rawcmd & "0700"    ; 2 bytes
rawcmd = rawcmd & "aa"    ; 2 bytes

Ut_sendrawcmd "$SC_$CPU_SB", {rawcmd}

if (ut_rcs_status = UT_RCS_CmdFailure) then
;  Look for the event message
   if ($SC_$CPU_find_event[1].num_found_messages > 0) then
      Write "<*> Passed (4005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd!!!" 
      ut_setrequirements SB_4005 "P"
   else
      Write "<!> Failed (4005) - Event Message not received for Invalid command code command."
      ut_setrequirements SB_4005 "F"
   endif
else
   Write "<!> Failed (4005) - Invalid command code test failed."
   ut_setrequirements SB_4005 "F"
endif

wait wait_time

write ";**********************************************************************"
write ";  Step 2a:  Send a raw command with a too big packet size of           "
write ";           CFE_SB_MAX_SB_MSG_SIZE bytes + 1 (= 32769 bytes).          "
write ";  32769 bytes - 7 bytes =  32762 bytes = x'7FFA'                      "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_MSG_TOO_BIG_EID, "ERROR", 1

cmdSize = CFE_SB_MAX_SB_MSG_SIZE - 6
write "Setting cmdSize to ", cmdSize
rawcmd = "18"&%hex($CPU_CMDAPID_BASE + SB_CMDAPID_OFF, 2)&"C000"&%hex(cmdSize,4) 
rawcmd = rawcmd & "05000000"    ; 4 bytes

Ut_sendrawcmd "$SC_$CPU_SB", {rawcmd}

;  Look for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
   Write "<*> Passed (4305.6;4701) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd!!!" 
   ut_setrequirements SB_43056 "P"
   ut_setrequirements SB_4701 "P"
else
   Write "<!> Failed (4305.6;4701) - Event Message not received for Too large packet size command."
   Write "<!> Too large packet size test failed."
   ut_setrequirements SB_43056 "F"
   ut_setrequirements SB_4701 "F"
endif

wait wait_time

write ";**********************************************************************"
write ";  Step 2b:  Send a raw command with a packet size of                  "
write ";           CFE_SB_MAX_SB_MSG_SIZE bytes (= 32769 bytes).              "
write ";  32768 bytes - 7 bytes =  32761 bytes = x'7FF9'                      "
write "; This step should pass since the MAX size is a valid length           "
write ";**********************************************************************"

cmdSize = CFE_SB_MAX_SB_MSG_SIZE - 7
write "Setting cmdSize to ", cmdSize
rawcmd = "18"&%hex($CPU_CMDAPID_BASE + SB_CMDAPID_OFF, 2)&"C000"&%hex(cmdSize,4) 
rawcmd = rawcmd & "00010005"    ; 4 bytes

Ut_sendrawcmd "$SC_$CPU_SB", {rawcmd}

;  Look for the event message
if (ut_rcs_status = UT_RCS_Success) then
   Write "<*> Passed (4701) - Command with ",cmdSize," sent successfully" 
   ut_setrequirements SB_4701 "P"
else
   Write "<!> Failed (4701) - Max packet size command test failed."
   ut_setrequirements SB_4701 "F"
endif

write ";**********************************************************************"
write ";  Step 3:  Send a Disable Routing command with a bad MsgId=x'0FFF'.   "
write ";  Important assumption:  TO_TLM_PIPE has a PipeId = 8.                "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_DSBL_RTE1_EID, "ERROR", 1

;;Ut_sendcmd  "$SC_$CPU_SB_DisRoute MID=x'0FFF', PID=8"
/$SC_$CPU_SB_DisRoute MID=x'0FFF', PID=8
wait 5

;  Look for the event message
;;if (ut_sc_status = UT_SC_CmdFailure) then
   if ($SC_$CPU_find_event[1].num_found_messages > 0) then
      Write "<*> Passed (4004;4005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd!!!" 
      ut_setrequirements SB_4004 "P"
      ut_setrequirements SB_4005 "P"
   else
      Write "<!> Bad Msg Id for Disable Routing command test did not get the correct event."
      ut_setrequirements SB_4004 "F"
      ut_setrequirements SB_4005 "F"
   endif
;;else
;;   Write "<!> Bad Msg Id for Disable Routing command test did not get the correct event."
;;   ut_setrequirements SB_4004 "F"
;;   ut_setrequirements SB_4005 "F"
;;endif

wait wait_time

write ";**********************************************************************"
write ";  Step 4:  Send a Disable Routing command with a bad pipe ID=x'FE'.   "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_DSBL_RTE3_EID, "ERROR", 1

Ut_sendcmd  "$SC_$CPU_SB_DisRoute MID=x'0890', PID=x'FE'"

if (ut_sc_status = UT_SC_CmdFailure) then
;  Look for the event message
   if ($SC_$CPU_find_event[1].num_found_messages > 0) then
      Write "<*> Passed (4004;4005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd!!!" 
      ut_setrequirements SB_4004 "P"
      ut_setrequirements SB_4005 "P"
   else
      Write "<!> Failed (4004;4005) - Bad Pipe Id for Disable Routing command test did not get the correct event."
      ut_setrequirements SB_4004 "F"
      ut_setrequirements SB_4005 "F"
   endif
else
   Write "<!> Failed (4004;4005) - Bad Pipe Id for Disable Routing command test failed."
   ut_setrequirements SB_4004 "F"
   ut_setrequirements SB_4005 "F"
endif

wait wait_time

write ";**********************************************************************"
write ";  Step 5:  Send a Enable Routing command with a bad MsgId=x'0FFF'.    "
write ";  Important assumption:  TO_TLM_Pipe has a PipeId = 8.                "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_ENBL_RTE1_EID, "ERROR", 1 

Ut_sendcmd  "$SC_$CPU_SB_EnaRoute MID=x'0FFF', PID=8"

if (ut_sc_status = UT_SC_CmdFailure) then
;  Look for the event message
   if ($SC_$CPU_find_event[1].num_found_messages > 0) then
      Write "<*> Passed (4004;4005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd!!!" 
      ut_setrequirements SB_4004 "P"
      ut_setrequirements SB_4005 "P"
   else
      Write "<!> Failed (4004;4005) - Bad Msg Id for Enable Routing command test did not get the correct event."
      ut_setrequirements SB_4004 "F"
      ut_setrequirements SB_4005 "F"
   endif
else
   Write "<!> Failed (4004;4005) - Bad Msg Id for Enable Routing command test failed."
   ut_setrequirements SB_4004 "F"
   ut_setrequirements SB_4005 "F"
endif

wait wait_time

write ";**********************************************************************"
write ";  Step 6:  Send a Enable Routing command with a bad pipe ID=x'FE'.    "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_ENBL_RTE3_EID, "ERROR", 1

Ut_sendcmd  "$SC_$CPU_SB_EnaRoute MID=x'0890', PID=x'FE'"

if (ut_sc_status = UT_SC_CmdFailure) then
;  Look for the event message
   if ($SC_$CPU_find_event[1].num_found_messages > 0) then
      Write "<*> Passed (4004;4005) - Event Msg ",$SC_$CPU_find_event[1].eventid," rcv'd!!!" 
      ut_setrequirements SB_4004 "P"
      ut_setrequirements SB_4005 "P"
   else
      Write "<!> Failed (4004;4005) - Bad Pipe Id for Enable Routing command test did not get the correct event."
      ut_setrequirements SB_4004 "F"
      ut_setrequirements SB_4005 "F"
   endif
else
   Write "<!> Failed (4004;4005) - Bad Pipe Id for Enable Routing command test failed."
   ut_setrequirements SB_4004 "F"
   ut_setrequirements SB_4005 "F"
endif

wait wait_time

write "**** Requirements Status Reporting"
                                                                                
write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"
                                                                                
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables
                                                                                
write ";**********************************************************************"
write ";  End procedure $SC_$CPU_sb_cmds_err                                  "
write ";**********************************************************************"
ENDPROC
