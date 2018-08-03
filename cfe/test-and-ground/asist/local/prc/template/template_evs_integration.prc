proc $SC_$CPU_evs_integration
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform Build Integration Testing of Event Services.  
;
; Note:  
;
; History:
;
;  18APR05    Initial development of this proc         MOB
;  30JAN08    Updated for build 5.0                    SJJ
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   Variables Definition:
;    
#include "cfe_platform_cfg.h"
#include "ut_statusdefs.h"       
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_time_events.h"

%LIV(USE_ASK_POPUP) = TRUE
local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt, pipe_id, message_id
local proc_error_cnt
local event_packet_ctr
local output_port
local file_time
local output_port_mask
local output_port_temp
local time_cmd_ctr
local cmdexctr
local cfe_time_task_ndx
local time_noop_eid
   

;LIMIT ON CDH_ROUTE.NSBSTREAM
;LIMIT ON ACE_ROUTE.NSBSTREAM
;LIMIT ON PSE_ROUTE.NSBSTREAM

;LIMIT ON P019
;LIMIT ON P039
;LIMIT ON P059


page $SC_$CPU_evs_hk

Write "The cFE Build Label is =======> " & $SC_$CPU_ES_CFEMAJORVER & "." & $SC_$CPU_ES_CFEMINORVER
Write "Build Integration Testing of EVS"
Write "Checksum of this version is ====>  "& $SC_$CPU_ES_CKSUM
Write " "
Write "The OS Build Label is ========> " & $SC_$CPU_ES_OSMAJORVER & "." & $SC_$CPU_ES_OSMINORVER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Setup for test
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   time_noop_eid = CFE_TIME_NOOP_EID

   Write "*********  Enabling Debug Event Messages *********"

   ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Debug events have been enabled."
   else
     write "<!> Error: Could not enable Debug events."
   endif
   
   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Retrieve cfe_time application index
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   cmdexctr =$SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "bin_fltr_1_2.dat", "$CPU")
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}
   
   if (ut_tw_status = UT_TW_Success) then
     for i = 1 to CFE_ES_MAX_APPLICATIONS do
       if ($SC_$CPU_EVS_AppData[i].AppName = "CFE_TIME") then
         cfe_time_task_ndx = i
       endif
     enddo
     
     write "<*> Success: cfe_time_task_ndx = " & cfe_time_task_ndx
   else
     write "<!> Error: could not write evs app file!"
   endif
   
   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify No-Op command (Command Code 1)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   Write "*********  Verifying the No-op command*********"
   Write " Sending the no-op command "

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_NOOP_EID, INFO
   
   ut_sendcmd "$SC_$CPU_EVS_NOOP"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: No-Op command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for NO-op command."
     endif
   else
     write "<!> Error: Did not process No-Op command."
   endif
   
   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Reset Counters Command (Command Code 2)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   write "Sending a command with an error so that the error counter increments"
   ;;;; Note that this is an EVS command with an invalid function code = hex 'aa' (42 decimal)
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC + 1
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
     ut_sendrawcmd "$SC_$CPU_EVS", "1801c0000001aa"
   elseif ("$CPU" = "CPU2") then
     ut_sendrawcmd "$SC_$CPU_EVS", "1821c0000001aa"
   elseif ("$CPU" = "CPU3") then
     ut_sendrawcmd "$SC_$CPU_EVS", "1841c0000001aa"
   endif
   
   ut_tlmwait $SC_$CPU_EVS_CMDEC, {error_cmd_ctr}
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: Error count incremented as expected."
   else
     write "<!> Error: Error count did not change."
   endif
   
   ut_setupevt "$SC", "$CPU", "CFE_EVS",  CFE_EVS_RSTCNT_EID, "DEBUG"
   
   ut_sendcmd "$SC_$CPU_EVS_RESETCTRS"
   
   ;; Look for the expected event
   if ($SC_$CPU_num_found_messages = 1) then
     write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
     write "<!> Error: Reset Counters Event Message not rcv'd."
   endif
   
   ;;; Check that the CMDPC and CMDEC are 0
   ut_tlmwait $SC_$CPU_EVS_CMDPC, 0
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: PC count reset as expected."
   else
     write "<!> Error: PC count did not reset to 0."
   endif
   
   ut_tlmwait $SC_$CPU_EVS_CMDEC, 0
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: Error count reset as expected."
   else
     write "<!> Error: Error count did not reset to 0."
   endif

   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Disabling of an Event Type (Command Code 4)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   Write "  "
   Write "*************** Verifying the Disabling of Event Type command *********"
   Write "Sending the command to disable an Event Type Command"  
   Write "For this test, we are disabling the INFO messages"

   Write "First verify getting TIME and ES events"
   write "Event_packet_ctr is currently " &  event_packet_ctr

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      write "p008scnt is currently " & p008scnt
   elseif ("$CPU" = "CPU2") then
      write "p028scnt is currently " & p028scnt
   elseif ("$CPU" = "CPU3") then
      write "p048scnt is currently " & p048scnt
   endif

   ut_sendcmd "$SC_$CPU_ES_NOOP"
   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1845c0000001aa"
   endif
   
   wait 5

   Write "Verify received the events"   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 3}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the INFO events as expected!"
   else
      write "<!> Error: Did not receive the INFO events when we should have."
   endif

   Write "Now disable all INFO messages"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DISEVTTYPE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_DISEVENTTYPE INFO"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Disable Event Type command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Disable Event Type command."
     endif
   else
     write "<!> Error: Did not process Disable Event Type command."
   endif
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   Write "Now sending those No-Ops again.  Should only get the ERROR event message"
   wait 3
   
   ut_sendcmd "$SC_$CPU_ES_NOOP"
   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1845c0000001aa"
   endif

   wait 5
   
   Write "Verify we only received only the DEBUG event"   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 1}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 1}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 1}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Did not receive the INFO events."
   else
      write "<!> Error: Received the INFO events when we should not have!"
   endif
   
   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Enabling of an Event Type (Command Code 3)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC

   Write " "
   Write "*********** Verifying the Enabling of an event type *******"
   Write "Now enabling all INFO messages"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ENAEVTTYPE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE INFO"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Enable Event Type command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Enable Event Type command."
     endif
   else
     write "<!> Error: Did not process Enable Event Type command."
   endif

   Write "Now sending those No-Ops again.  Should get them"
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   wait 3
   
   ut_sendcmd "$SC_$CPU_ES_NOOP"
   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_EVS", "1845c0000001aa"
   endif

   wait 5
   
   Write "Verify received the events"   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 3}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the INFO events as expected!"
   else
      write "<!> Error: Did not receive the INFO events when we should have."
   endif
   
   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Setting of the Event Format Mode - Long/Short
;   (Command Code 5)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC
  
   Write " "
   Write " ************ Verifying the Setting of the Event Format Mode *********"
   Write " First set the mode to short "

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETEVTFMTMOD_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SetEvtFmt Short"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Set Event Format Mode command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Set Event Format Mode command."
     endif
   else
     write "<!> Error: Did not process Set Event Format Mode command."
   endif

   if (p@$SC_$CPU_EVS_MSGFMTMODE = "SHORT") then
     Write "<*> Success: Event Format Mode set to SHORT."
   else
     Write "<!> Error: Event Format Mode wasn't set to SHORT!"
   endif 

   Write " Now set the mode to Long "

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETEVTFMTMOD_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SetEvtFmt Long"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Set Event Format Mode command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Set Event Format Mode command."
     endif
   else
     write "<!> Error: Did not process Set Event Format Mode command."
   endif

   if (p@$SC_$CPU_EVS_MSGFMTMODE = "LONG") then
     Write "<*> Success: Event Format Mode set to LONG."
   else
     Write "<!> Error: Event Format Mode wasn't set to LONG!"
   endif 

   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Disabling of an Event Type for an Application
;  (Command Code 7)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC

 
   Write " "
   Write " ************ Verifying the Disabling of an Event Type for an Application  *********"
   Write " Disable the Info messages to the CFE_TIME "

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DISAPPENTTYPE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_DisAppEvtType Application = ""CFE_TIME"" INFO"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Disable App Event Type command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Disable App Event Type command."
     endif
   else
     write "<!> Error: Did not process Disable App Event Type command."
   endif

   Write "Sending a TIME No-op, an invalid TIME command  and an ES No-op.  Should only see the invalid TIME and ES no-op"

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif

   wait 2

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_CC_ERR_EID, ERROR, 1
   ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 2

   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1845c0000001aa"
   endif
   ut_sendcmd "$SC_$CPU_ES_NOOP"

   wait 2

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 2}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 2}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 2}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the correct number of events."
   else
      write "<!> Error: Did not receive the correct number of events!"
   endif
   
   if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      Write "<*> Success: TIME Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for Invalid TIME command!"
   endif
   
   if ($SC_$CPU_find_event[2].num_found_messages = 1) then
      Write "<*> Success: ES Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for ES NOOP command."
   endif

   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enabling of an Event Type for an Application
;  (Command Code 6)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC

 
   Write " "
   Write " ************ Verifying the Enabling of an Event Type for an Application  *********"
   Write " Enable the Info messages to the CFE_TIME "

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ENAAPPEVTTYPE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_EnaAppEvtType Application = ""CFE_TIME"" INFO"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Enable App Event Type command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Disable App Event Type command."
     endif
   else
     write "<!> Error: Did not process Disable App Event Type command."
   endif


   Write "Sending a Time No-op and an ES No-op.  Should see the Time and ES no-op"
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   wait 2
   
   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1
   ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 2

   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   ut_sendcmd "$SC_$CPU_ES_NOOP"

   wait 2

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 2}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 2}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 2}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the correct number of events."
   else
      write "<!> Error: Did not receive the correct number of events!"
   endif
   
   if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      Write "<*> Success: TIME Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for TIME no-op command!"
   endif
   
   if ($SC_$CPU_find_event[2].num_found_messages = 1) then
      Write "<*> Success: ES Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for ES no-op command."
   endif

   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Disabling of All Events for an Application
;   (Command Code 9)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif

   Write " "
   Write " ************ Verifying the Disabling of All Events for an Application  *********"
   write "Verifying the receipt of TIME INFO and ERROR event messages as well as ES event message"

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1
   ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 2
   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_CC_ERR_EID, ERROR, 3

   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   ut_sendcmd "$SC_$CPU_ES_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1845c0000001aa"
   endif
   
   wait 2
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 3}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the correct number of events."
   else
      write "<!> Error: Did not receive the correct number of events!"
   endif

   if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      Write "<*> Success: TIME Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for TIME no-op command!"
   endif
   
   if ($SC_$CPU_find_event[2].num_found_messages = 1) then
      Write "<*> Success: ES Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for ES no-op command."
   endif

   if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for Invalid command."
   endif

   Write "Now disable all TIME event messages"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DISAPPEVT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_DISAPPEVGEN Application = ""CFE_TIME"""

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Disable Application Events command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Disable Application Events command."
     endif
   else
     write "<!> Error: Did not process Disable Application Events command."
   endif

   Write "Now send the same 3 event messages and verify that we only get the ES event message"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   wait 2
   
   ut_setupevt $SC, $CPU, CFE_ES, CFE_ES_NOOP_INF_EID, INFO

   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   ut_sendcmd "$SC_$CPU_ES_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1845c0000001aa"
   endif
   
   wait 2
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 1}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 1}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 1}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the correct number of events."
   else
      write "<!> Error: Did not receive the correct number of events!"
   endif

   if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Success: ES Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      write "<!> Error: Event Message not received for ES no-op command."
   endif

   if (p@$SC_$CPU_EVS_APP[cfe_time_task_ndx].APPENASTAT = "DIS") then
      write "<*> Success: Events disabled for TIME application."
   else
      write "<!> Error: Events were not disabled for TIME application!"
   endif   

   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enabling of All Events for an Application
;   (Command Code 8)
;   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC


   Write " "
   Write " ************ Verifying the Enabling of All Events for an Application  *********"

   Write "Enable the TIME event messages - all"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ENAAPPEVT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_ENAAPPEVGEN Application = ""CFE_TIME"""

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Enable Application Events command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Enable Application Events command."
     endif
   else
     write "<!> Error: Did not process Enable Application Events command."
   endif


   Write "Now send the same 3 event messages and verify that we get ALL 3"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   wait 2 

   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1
   ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 2
   ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_CC_ERR_EID, ERROR, 3

   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   ut_sendcmd "$SC_$CPU_ES_NOOP"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1805c0000001aa"     ; Invalid Time command generates an event message of type ERROR
   elseif ("$CPU" = "CPU2") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
      ut_sendrawcmd "$SC_$CPU_TIME", "1845c0000001aa"
   endif
   
   wait 2
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p008scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p028scnt, {event_packet_ctr + 3}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p048scnt, {event_packet_ctr + 3}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the correct number of events."
   else
      write "<!> Error: Did not receive the correct number of events!"
   endif

   if ($SC_$CPU_find_event[1].num_found_messages = 1) then
      Write "<*> Success: ES Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for TIME no-op command!"
   endif
   
   if ($SC_$CPU_find_event[2].num_found_messages = 1) then
      Write "<*> Success: TIME Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for ES no-op command."
   endif

   if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[3].eventid," Found!"
   else
      Write "<!> Error: Event Message not received for Invalid command."
   endif

   if (p@$SC_$CPU_EVS_APP[cfe_time_task_ndx].APPENASTAT = "ENA") then
      write "<*> Success: Events enabled for TIME application."
   else
      write "<!> Error: Events were not enabled for TIME application!"
   endif   

   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Resetting of an Application's Event Counter
;   (Command Code 10)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC
   
   Write " "
   Write "Sending a few Time NoOps to make sure that the event message counter is not zero"
   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   ut_sendcmd "$SC_$CPU_TIME_NOOP"

   wait until ($SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC <> 0)
   Write "Now send the command to reset the App Event Counter"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTEVTCNT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RstAppCtrs Application = ""CFE_TIME"""

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Reset Application Event Counter command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Reset Application Event Counter command."
     endif
   else
     write "<!> Error: Did not process Reset Application Event Counter command."
   endif
   
   ut_tlmwait $SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC, {0} 

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Application Event Counter was reset."
   else
      write "<!> Error: Application Event Counter wasn't reset!"
   endif

   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Add an Event for Filtering 
;   (Command Code 16)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   local isfiltered
   
;   PAGE $SC_$CPU_EVS_APP4_DATA
   PAGE $SC_$CPU_EVS_APP_DATA_MAIN

   ;  Check to make sure that TIME NOOP event is not registered for filtering
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC
   local ead_filenum
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ead_filenum = 3841
   elseif ("$CPU" = "CPU2") then
      ead_filenum = 3873
   elseif ("$CPU" = "CPU3") then
      ead_filenum = 3905
   endif
   
   isfiltered = FALSE
   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "$sc_$cpu_cfe_evs_app.dat", "$CPU")   
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}
   
   if (ut_tw_status = UT_TW_Success) then
      for i = 1 to 8 do
         write $SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtId
         if ($SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtID = CFE_TIME_NOOP_EID ) then
            isfiltered = TRUE
         endif
      enddo
   else
     write "<!> Error: could not write evs app file!"
   endif
   
   if (isfiltered = TRUE) then
      write "TIME NOOP event is already registered for filtering so need to delete it"
      
      ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG
      ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION = ""CFE_TIME"" EVENT_ID = CFE_TIME_NOOP_EID"
      
      if (UT_SC_Status = UT_SC_Success) then
         write "<*> Success: Delete Filter command processed."
         if ($SC_$CPU_num_found_messages = 1) then
            Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
         else
            Write "<!> Error: Event Message not received for Delete Filter command."
         endif
      else
         write "<!> Error: Did not process Delete Filter command."
      endif
      
      wait 5
   
      ;  Check AGAIN to make sure that TIME NOOP event is not registered for filtering
      isfiltered = FALSE
      cmdexctr = $SC_$CPU_EVS_CMDPC + 1
      start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "$sc_$cpu_cfe_evs_app.dat", "$CPU")   
      ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}
      
      if (ut_tw_status = UT_TW_Success) then
         for i = 1 to 8 do
            write $SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtId
            if ($SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtID = CFE_TIME_NOOP_EID ) then
               isfiltered = TRUE
            endif
         enddo
      else
        write "<!> Error: could not write evs app file!"
      endif

      if (isfiltered = TRUE) then
         write "<!> Error: TIME NOOP event filter was not removed!"
      endif
   endif

   write "Make sure that TIME NOOP command generates an event message"
   
   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO
   ut_sendcmd "$SC_$CPU_TIME_NOOP"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: TIME no-op command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for TIME no-op command."
      endif
   else
      write "<!> Error: Did not process TIME no-op command."
   endif

   write "Now sending the command to add TIME NOOP event for filtering"

   wait 3
   
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_TIME"" EVENT_ID=time_noop_eid EVENT_MASK= x'FFFE'"
   
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Add Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Add Filter command."
      endif
   else
      write "<!> Error: Did not process Add Filter command."
   endif
   
   wait 5
   
   write "Verify that the TIME NOOP event is being filtered"
   write "Should only get 2 event messages based on the filter"
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      event_packet_ctr = p008scnt
   elseif ("$CPU" = "CPU2") then
      event_packet_ctr = p028scnt
   elseif ("$CPU" = "CPU3") then
      event_packet_ctr = p048scnt
   endif
   
   wait 2

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO

   wait 2
   for i = 1 to 4 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 2) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found only twice!"
   else
      Write "<!> Error: Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif

   Write "Verify that the EVS Application Data File indicates the addition of the filter"
   
   isfiltered = FALSE
   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "$sc_$cpu_cfe_evs_app.dat", "$CPU")   
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      for i = 1 to 8 do
         write $SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtId
         if ($SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtID = CFE_TIME_NOOP_EID ) then
            isfiltered = TRUE
         endif
      enddo
   else
     write "<!> Error: could not write evs app file!"
   endif

   if (isfiltered = TRUE) then
      write "<*> Success: TIME NOOP event filter was added."
   else
      write "<!> Error: TIME NOOP event filter was not removed!"
   endif

   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Delete an Event for Filtering 
;   (Command Code 17)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   
;   PAGE $SC_$CPU_EVS_APP4_DATA
   PAGE $SC_$CPU_EVS_APP_DATA_MAIN
   
   write "Sending the command to Delete an Event Filter (CFE_TIME NOOP event)"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DELFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_DELEVTFLTR APPLICATION = ""CFE_TIME"" EVENT_ID = time_noop_eid"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Delete Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Delete Filter command."
      endif
   else
      write "<!> Error: Did not process Delete Filter command."
   endif
   
   ;  Check to make sure that TIME NOOP event is not registered for filtering
   isfiltered = FALSE
   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "$sc_$cpu_cfe_evs_app.dat", "$CPU")   
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}
   
   if (ut_tw_status = UT_TW_Success) then
      for i = 1 to 8 do
         write $SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtId
         if ($SC_$CPU_EVS_AppData[cfe_time_task_ndx].BinFltr[i].EvtID = CFE_TIME_NOOP_EID ) then
            isfiltered = TRUE
         endif
      enddo
   else
     write "<!> Error: could not write evs app file!"
   endif

   if (isfiltered <> TRUE) then
      write "<*> Success: the event filter was removed."
   else
      write "<!> Error: the event filter was not removed!"
   endif

   write "Verify that the TIME NOOP event is NOT registered for filtering"  

   wait 2
   
   write "Now sending 4 TIME NOOP commands.  Should get all 4 event messages"
   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO

   for i = 1 to 4 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 4) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found 4 times!"
   else
      Write "<!> Error: Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif

   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Setting of a Filter Mask
;   (Command Code 11)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   local new_event_packet_ctr

   Write " "
   Write " ************ Verifying the Setting of a Filter Mask  *********"
   wait 3
   Write "First must register the filter"
   
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_TIME"" EVENT_ID=time_noop_eid EVENT_MASK= x'FFFE'"
   
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Add Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Add Filter command."
      endif
   else
      write "<!> Error: Did not process Add Filter command."
   endif

   write "Sending the command to filter all TIME NOOP Messages"
   wait 2

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TIME"" EVENT_ID= time_noop_eid FILTERMASK=X'ffff'"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Filter Mask command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Filter Mask command."
      endif
   else
      write "<!> Error: Did not process Set Filter Mask command."
   endif

   wait 2
   
   write "Now sending 4 TIME NOOP commands.  Should get only 1 event message"
   Write "Note that the FFFF mask allows 1 event message"
   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO

   for i = 1 to 4 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 1) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found 1 time!"
   else
      Write "<!> Error: Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif

   wait 2
   
   write "Now setting the filter so that we get them again"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TIME"" EVENT_ID= time_noop_eid FILTERMASK=X'0000'"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Filter Mask command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Filter Mask command."
      endif
   else
      write "<!> Error: Did not process Set Filter Mask command."
   endif
   
   wait 3

   write "Now sending 4 TIME NOOP commands.  Should get all 4 event messages"
   Write "Note that the 0000 mask allows all event messages"

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO

   for i = 1 to 4 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 4) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found 4 times!"
   else
      Write "<!> Error: TIME NOOP Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif
   
   wait 5
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enabling of Output Ports
;   (Command Code 12)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   Write " "
   Write " ************ Verifying the Enabling of an Output Port (Port 4)*********"
   write "The current output port bit mask is " & $SC_$CPU_EVS_OUTPUTPORT

   output_port_mask = b'1000'
   output_port_temp = $SC_$CPU_EVS_OUTPUTPORT
   output_port = %AND(output_port_temp, output_port_mask)
   write "The output port mask is " & output_port
   wait 3
   if (output_port = 8 ) then
      ; Port is enabled - disable the port first
      
      ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DISPORT_EID, DEBUG
      ut_sendcmd "$SC_$CPU_EVS_DISPORT PORT_FOUR"
   
      if (UT_SC_Status = UT_SC_Success) then
         write "<*> Success: Disable Ports command processed."
         if ($SC_$CPU_num_found_messages = 1) then
            Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
         else
            Write "<!> Error: Event Message not received for Disable Ports command."
         endif
      else
         write "<!> Error: Did not process Disable Ports command."
      endif

       output_port = %AND($SC_$CPU_EVS_OUTPUTPORT, b'1000')
        if (output_port <> 0) then
          write "<!> Error: The EVS_DISPORT command failed. Port still enabled."
       endif
   endif

   write "Sending the command to Enable Output Port 4"
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ENAPORT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_ENAPORT PORT_FOUR"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Enable Ports command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Enable Ports command."
      endif
   else
      write "<!> Error: Did not process Enable Ports command."
   endif
   
   ;  Verify that telemety indicates the fourth bit was set
   wait 3
   output_port_mask = b'1000'
   output_port_temp = $SC_$CPU_EVS_OUTPUTPORT
   output_port = %AND(output_port_temp, output_port_mask)
   write "The output port mask is " & output_port
   wait 3
   if (output_port = 8) then
      write "<*> Success: Port 4 was enabled."
   else
       write "<!> Error: The EVS_ENAPORT command failed. Port not enabled."
   endif

   wait 5
     
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Disabling of Output Ports
;   (Command Code 13)
;  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
   output_port = $SC_$CPU_EVS_OUTPUTPORT

   Write " "
   Write " ************ Verifying the Disabling of an Output Port *********"
   write "The current output port bit mask is " & $SC_$CPU_EVS_OUTPUTPORT
   write "Sending the command to Disable Output Port 4"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DISPORT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_DISPORT PORT_FOUR"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Disable Ports command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Disable Ports command."
      endif
   else
      write "<!> Error: Did not process Disable Ports command."
   endif

   output_port = %AND($SC_$CPU_EVS_OUTPUTPORT, b'1000')
   if (output_port = 0) then
      write "<*> Success: Port 4 was disabled."
   else
      write "<!> Error: The EVS_DISPORT command failed. Port still enabled."
   endif

   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Resetting An Application's filter
;   (Command Code 14)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC

   time_cmd_ctr = $SC_$CPU_TIME_CMDPC

   Write " "
   Write " ************ Verifying the resetting of a filter for an APP  *********"
   Write "First reset the event message counter for time"

   Write "Reset the event counter to start fresh"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTEVTCNT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RSTAPPCTRS APPLICATION=""CFE_TIME"""

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Reset App Event Counters command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Reset App Event Counters command."
      endif
   else
      write "<!> Error: Did not process Reset App Event Counters command."
   endif

   ut_tlmwait $SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC, {0}

   if (ut_tw_status <> UT_TW_Success) then
      write "<!> Error: App Event Counters were not reset!"
   endif

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION=""CFE_TIME"" EVENT_ID=time_noop_eid"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Reset Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Reset Filter command."
      endif
   else
      write "<!> Error: Did not process Reset Filter command."
   endif

   Write "Set a filter such that Time NoOp events will get filtered"
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TIME"" EVENT_ID=time_noop_eid FILTERMASK=X'8'"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Filter Mask command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Filter Mask command."
      endif
   else
      write "<!> Error: Did not process Set Filter Mask command."
   endif

   Write "Now send Time Noops and verify only got 8 and then no more"

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO
   wait 2
   for i = 1 to 9 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 8) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found only 8 times!"
   else
      Write "<!> Error: TIME NOOP Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif
 
   Write "Now Reseting the filter"
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION=""CFE_TIME"" EVENT_ID=time_noop_eid"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Reset Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Reset Filter command."
      endif
   else
      write "<!> Error: Did not process Reset Filter command."
   endif

   Write "Verify that we are now getting the event messages again"

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO
   ut_sendcmd "$SC_$CPU_TIME_NOOP"
   
   if (UT_SC_Status = UT_SC_Success) then
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: TIME NOOP Event Message not received!"
      endif
   else
      write "<!> Error: Did not process TIME NOOP command."
   endif
   
   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Resetting of all filters for the specified app
;   (Command Code 15)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;     

   Write " "
   Write " ************ Verifying the resetting ALL filters for an APP  *********"
   Write "First set a filter such that Time NoOp events will get filtered"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTEVTCNT_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RSTAPPCTRS APPLICATION=""CFE_TIME"""

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Reset App Event Counters command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Reset App Event Counters command."
      endif
   else
      write "<!> Error: Did not process Reset App Event Counters command."
   endif

   ut_tlmwait $SC_$CPU_EVS_APP[cfe_time_task_ndx].APPMSGSENTC, {0}

   if (ut_tw_status <> UT_TW_Success) then
      write "<!> Error: App Event Counters were not reset!"
   endif

   Write "Resetting the Filter"
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RSTBINFLTRCTR APPLICATION=""CFE_TIME"" EVENT_ID=time_noop_eid"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Reset Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Reset Filter command."
      endif
   else
      write "<!> Error: Did not process Reset Filter command."
   endif

   Write "Setting the Mask for Time Event 4"
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TIME"" EVENT_ID= time_noop_eid FILTERMASK=X'8'"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Filter Mask command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Filter Mask command."
      endif
   else
      write "<!> Error: Did not process Set Filter Mask command."
   endif

   Write "Now send Time Noops and verify only got 8"


   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO
   wait 2
   for i = 1 to 9 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 8) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found only 8 times!"
   else
      Write "<!> Error: TIME NOOP Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif

   Write "Then setup a filter such that Another Time Event is filtered"
   Write "First need to register the Event for filtering"
   
   local set_stcf_eid
   set_stcf_eid = CFE_TIME_STCF_EID
      
   wait 3

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_ADDFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_ADDEVTFLTR APPLICATION=""CFE_TIME"" EVENT_ID=set_stcf_eid EVENT_MASK= x'FFFE'"
   
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Add Filter command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Add Filter command."
      endif
   else
      write "<!> Error: Did not process Add Filter command."
   endif

   wait 3 

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_SETFILTERMSK_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETBINFLTRMASK APPLICATION=""CFE_TIME"" EVENT_ID=set_stcf_eid FILTERMASK=X'4'"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Filter Mask command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Filter Mask command."
      endif
   else
      write "<!> Error: Did not process Set Filter Mask command."
   endif

   Write "Now send the TIME Set Clock STCF command which generates the event"


   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO
   wait 2
   for i = 1 to 4 do
      ut_sendcmd "$SC_$CPU_TIME_SETCLOCKSTCF SECONDS=X'ffff' MICROSECS=X'3000'"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the Set Clock STCF commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 4) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found 4 times!"
   else
      Write "<!> Error: TIME NOOP Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif
   
   Write "Now send it again and verify that we don't get it"

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO
   wait 2
   for i = 1 to 2 do
      ut_sendcmd "$SC_$CPU_TIME_SETCLOCKSTCF SECONDS=X'1000' MICROSECS=X'2000'"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the Set Clock STCF commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 0) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," was not found."
   else
      Write "<!> Error: Set STCF Event Message received when it shouldn't have been!"
      write $SC_$CPU_num_found_messages
   endif

   Write "Now send the command to reset all filters for the application"


   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_RSTALLFILTER_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_RSTALLFLTRS APPLICATION=""CFE_TIME"""
   
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Reset All Filters command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Reset All Filters command."
      endif
   else
      write "<!> Error: Did not process Reset All Filters command."
   endif
   
   wait 5
  
   Write "Send commands and verify that we are getting events messages again"

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO
   wait 2
   for i = 1 to 9 do
      ut_sendcmd "$SC_$CPU_TIME_NOOP"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the TIME NOOP commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 8) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found only 8 times!"
   else
      Write "<!> Error: TIME NOOP Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO
   wait 2
   for i = 1 to 5 do
      ut_sendcmd "$SC_$CPU_TIME_SETCLOCKSTCF SECONDS=X'1000' MICROSECS=X'2000'"
      if (UT_SC_Status <> UT_SC_Success) then
         write "<!> Error: one of the Set Clock STCF commands was not processed!"
      endif
   enddo
   wait 2
   
   if ($SC_$CPU_num_found_messages = 4) then
      Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," found only 4 times."
   else
      Write "<!> Error: Set STCF Event Message not received correct number of times!"
      write $SC_$CPU_num_found_messages
   endif
   
   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the writing of the EVS Log
;   (Command Code 19)
;  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   local elg_filenum
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      elg_filenum = 3840
   elseif ("$CPU" = "CPU2") then
      elg_filenum = 3872
   elseif ("$CPU" = "CPU3") then
      elg_filenum = 3904
   endif

   file_time = FILE_LIST[elg_filenum].file_write_time

   Write " "
   Write " ************ Verifying the ability to write and dump the EVS Log File Using default filename *********"
   page $SC_$CPU_evs_log
   wait 2

   cmdexctr =$SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log1","$CPU")
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}
   
   if (ut_tw_status = UT_TW_Success) then
      write "Verify that the time inside the file is different"
      WAIT UNTIL (file_time <> FILE_LIST[elg_filenum].file_write_time)
      write "<*> Success: EVS log file written with default name."
   else
      write "<!> Error: could not write EVS log file with default name!"
   endif

   wait 5

   file_time = FILE_LIST[elg_filenum].file_write_time

   Write " "
   Write " ************ Verifying the ability to write and dump the EVS Log File using specified filename *********"
   wait 2

   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_mylog.log", "cfe_evs.it_log2","$CPU", "EVS_LOG")
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      write "Verify that the time inside the file is different"
      WAIT UNTIL (file_time <> FILE_LIST[elg_filenum].file_write_time)
      write "<*> Success: EVS log file written with specified name."
   else
      write "<!> Error: could not write EVS log file with specified name!"
   endif

   write "Actual table data needs manual verification"
   
   wait 30

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the writing of the App file data for all registered 
;  applications
;   (Command Code 18)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   file_time = FILE_LIST[ead_filenum].file_write_time

   Write " "
   Write " ************ Verifying the ability to write and dump the EVS APP Data File  *********"
;   page $SC_$CPU_evs_app1_data
   page $SC_$CPU_evs_app_data_main
   wait 2

   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "cfe_evs_app.it_data1","$CPU") 
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      write "Verify that the time inside the file is different"
      WAIT UNTIL (file_time <> FILE_LIST[elg_filenum].file_write_time)
      write "<*> Success: EVS log file written with specified name."
   else
      write "<!> Error: could not write EVS log file with specified name!"
   endif
   
   wait 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the setting of the Log Mode
;   (Command Code 20)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write " "
   Write " ************ Verifying the ability to set the Log Mode (Overwrite/discard)  *********"
 
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_LOGMODE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETLOGMODE DISCARD"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Log Mode command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
         
         ut_tlmwait $SC_$CPU_EVS_LOGMODE, {1}  ; 1 = DISCARD
         if (ut_tw_status = UT_TW_Success) then
            write "<*> Success: Log Mode set to DISCARD."
         else
            write "<!> Error: Log Mode was not set to DISCARD!"
         endif
      else
         Write "<!> Error: Event Message not received for Set Log Mode command."
      endif
   else
      write "<!> Error: Did not process Set Log Mode command."
   endif
 
   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_LOGMODE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_SETLOGMODE OVERWRITE"

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Log Mode command processed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
         
         ut_tlmwait $SC_$CPU_EVS_LOGMODE, {0}  ; 0 = OVERWRITE
         if (ut_tw_status = UT_TW_Success) then
            write "<*> Success: Log Mode set to OVERWRITE."
         else
            write "<!> Error: Log Mode was not set to OVERWRITE!"
         endif
      else
         Write "<!> Error: Event Message not received for Set Log Mode command."
      endif
   else
      write "<!> Error: Did not process Set Log Mode command."
   endif

   wait 5
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the clearing of the Log Mode
;   (Command Code 21)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
   Write " "
   Write " ************ Verifying the ability to ability to clear the local event log  *********"
 
   Write "First we need to fill up the local event log"
   Write "Send SB Noops"

   for i = 1 to 25 do
      /$SC_$CPU_SB_NOOP
      wait 2
   enddo
   if (p@$SC_$CPU_EVS_LOGFULL = "TRUE") then
      write "<*> Success: Log is full."
   else
      write "<!> Error: Log is not full!"
   endif

   Write "Now sending the command to clear it"
   ut_sendcmd "$SC_$CPU_EVS_CLRLOG"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Clear Log command processed."
   else
      write "<!> Error: Did not process Clear Log command!"
   endif
   
   ut_tlmwait $SC_$CPU_EVS_LOGFULL, {0}
   if (ut_tw_status = UT_TW_Success) then
      write "<*> Success: Log no longer full."
   else
      write "<!> Error: Log is still full!"
   endif

   ut_tlmwait $SC_$CPU_EVS_LOGOVERFLOWC, {0}
   if (ut_tw_status = UT_TW_Success) then
      write "<*> Success: LOGOVERFLOWC is 0."
   else
      write "<!> Error: LOGOVERFLOWC isn't 0!"
   endif

   Write "Now dump the local event log"
   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log2","$CPU")
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      write "Verify the event log is empty"

      wait 2
      proc_error_cnt = 0
      for i = 1 to 20 do
         if ($SC_$CPU_EVS_LOG[1].EVTLOGENTRY.AppName <> "") then
            proc_error_cnt = proc_error_cnt + 1
         endif
      enddo
      if (proc_error_cnt = 0) then
         write "<*> Success: The event log is empty."
      else
         write "<!> Error:  Local Event log isn't empty!"
      endif
   else
      write "<!> Error: could not write EVS log!"
   endif

   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify that you can dump the Event Log after dumping the
;   Application Data File - problem in build 1.0
; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write "Disable the DEBUG messages so that you don't get the event messages"
   Write "  when you write the log"

   wait 3
   Write "Sending a command to DISABLE Event Messages of type DEBUG and clear the log"

   ut_setupevt $SC, $CPU, CFE_EVS, CFE_EVS_DISEVTTYPE_EID, DEBUG
   ut_sendcmd "$SC_$CPU_EVS_DISEVENTTYPE DEBUG"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Disable Event Type command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Disable Event Type command."
     endif
   else
     write "<!> Error: Did not process Disable Event Type command."
   endif

   ut_sendcmd "$SC_$CPU_EVS_CLRLOG"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Clear Log command processed."
   else
      write "<!> Error: Did not process Clear Log command!"
   endif

   wait 3
   file_time = FILE_LIST[ead_filenum].file_write_time

   Write " "
   Write " ************ Verifying the dump the EVS APP Data File then EVS log *********"
;   page $SC_$CPU_evs_app1_data
   page $SC_$CPU_evs_app_data_main
   wait 2

   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "cfe_evs_app.it_data2","$CPU") 
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      write "Verify that the time inside the file is different"
      WAIT UNTIL (file_time <> FILE_LIST[ead_filenum].file_write_time)
      write "<*> Success: EVS app data file written."
   else
      write "<!> Error: could not write EVS app data file!"
   endif
   
   wait 3

   file_time = FILE_LIST[elg_filenum].file_write_time

   page $SC_$CPU_evs_log
   wait 2

   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log3","$CPU")
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      write "Verify that the time inside the file is different"
      WAIT UNTIL (file_time <> FILE_LIST[elg_filenum].file_write_time)
      write "<*> Success: EVS log file written."
   else
      write "<!> Error: could not write EVS log file!"
   endif

   ; Generate a few noops
   /$SC_$CPU_TIME_NOOP
   /$SC_$CPU_TIME_NOOP
   /$SC_$CPU_TIME_NOOP
   wait 5
   success_cmd_ctr = $SC_$CPU_EVS_CMDPC
   error_cmd_ctr = $SC_$CPU_EVS_CMDEC
   file_time = FILE_LIST[elg_filenum].file_write_time

   cmdexctr = $SC_$CPU_EVS_CMDPC + 1
   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log4","$CPU")
   ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdexctr}

   if (ut_tw_status = UT_TW_Success) then
      ut_tlmwait $SC_$CPU_EVS_LOG[1].EVTLOGENTRY.AppName, {"CFE_TIME"}
      if (ut_tw_status = UT_TW_Success) then
         write "<*> Success: Entry 1 app is ", $SC_$CPU_EVS_LOG[1].EVTLOGENTRY.AppName
      else
         write "<!> Error: Entry 1 app was not CFE_TIME!"
      endif
      ut_tlmwait $SC_$CPU_EVS_LOG[1].EVTLOGENTRY.EVTID, {time_noop_eid}
      if (ut_tw_status = UT_TW_Success) then
         write "<*> Success: Entry 1 event ID is ", $SC_$CPU_EVS_LOG[1].EVTLOGENTRY.EVTID
      else
         write "<!> Error: Entry 1 event ID was not TIME NOOP EID!"
      endif
      ut_tlmwait $SC_$CPU_EVS_LOG[3].EVTLOGENTRY.AppName, {"CFE_TIME"}
      if (ut_tw_status = UT_TW_Success) then
         write "<*> Success: Entry 2 app is ", $SC_$CPU_EVS_LOG[3].EVTLOGENTRY.AppName
      else
         write "<!> Error: Entry 2 app was not CFE_TIME!"
      endif
      ut_tlmwait $SC_$CPU_EVS_LOG[2].EVTLOGENTRY.EVTID, {time_noop_eid}
      if (ut_tw_status = UT_TW_Success) then
         write "<*> Success: Entry 2 event ID is ", $SC_$CPU_EVS_LOG[1].EVTLOGENTRY.EVTID
      else
         write "<!> Error: Entry 2 event ID was not TIME NOOP EID!"
      endif
   else
      write "<!> Error: could not write EVS log file!"
   endif


procterm:
     Write "Test complete"
     ; Now clean up
    success_cmd_ctr = $SC_$CPU_EVS_CMDPC
    error_cmd_ctr = $SC_$CPU_EVS_CMDEC
    wait 5
    /$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=CFE_TIME_NOOP_EID
    /$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=CFE_TIME_STCF_EID

    /$SC_$CPU_EVS_RstAppCtrs Application = "CFE_TIME"
    /$SC_$CPU_EVS_ENAAPPEVGEN Application = "CFE_TIME"
    /$SC_$CPU_EVS_RSTALLFLTRS APPLICATION="CFE_TIME" 

    WAIT UNTIL (($SC_$CPU_EVS_CMDPC = success_cmd_ctr + 5) AND ($SC_$CPU_EVS_CMDEC = error_cmd_ctr))
endproc
