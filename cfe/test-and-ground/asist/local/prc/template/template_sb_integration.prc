proc $sc_$cpu_sb_integration

;******************************************************************************
;  Test Name:  $sc_$CPU_sb_integration
;  Test Level: Build Integration
;
;  Test Description
;       The purpose of this test is to perform Build Integration Testing of the
;       cFE Software Bus (SB).
;
;  Prerequisite Conditions
;       None
;
;  Assumptions and Constraints
;
;  Change History
;       Date            Name                    Description
;       04/18/05        M. Bartholomew  Initial development
;       01/24/08        S. Jonke        Updated for build 5.0
;
;  Arguments
;       None
;
;  Procedures Called
;       Name                                    Description
;                               processed and command error counters.
;
;  Expected Test Results and Analysis
;******************************************************************************
#include "ut_statusdefs.h"
#include "cfe_sb_events.h"
                                       

%LIV(USE_ASK_POPUP) = TRUE
local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt, pipe_id, message_id
local proc_error_cnt
local file_time
local flindx
local rtindx, endloop, route_entry


;LIMIT ON CDH_ROUTE.NSBSTREAM
;LIMIT ON ACE_ROUTE.NSBSTREAM
;LIMIT ON PSE_ROUTE.NSBSTREAM

;LIMIT ON P019
;LIMIT ON P039
;LIMIT ON P059


page $SC_$CPU_sb_hk

Write "The cFE Build Label is =======> " & $SC_$CPU_ES_CFEMAJORVER & "." & $SC_$CPU_ES_CFEMINORVER
Write "Build Integration Testing of EVS"
Write "Checksum of this version is ====>  "& $SC_$CPU_ES_CKSUM
Write " "
Write "The OS Build Label is ========> " & $SC_$CPU_ES_OSMAJORVER & "." & $SC_$CPU_ES_OSMINORVER

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Enable Debug Events
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   Write "*********  Enabling Debug Event Messages *********"

   ut_sendcmd "$SC_$CPU_EVS_ENAEVENTTYPE DEBUG"
   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Debug events have been enabled."
   else
     write "<!> Error: Could not enable Debug events."
   endif
   
   wait 5


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify No-Op command
;  Command Code 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   Write "*********  Verifying the No-op command*********"

   ut_setupevt $SC, $CPU, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO
   
   ut_sendcmd "$SC_$CPU_SB_NOOP"

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
; Verify Reset Counters Command
;  Command Code 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ut_setupevt "$SC", "$CPU", "CFE_SB", CFE_SB_BAD_CMD_CODE_EID, "ERROR"
   
   error_cmd_ctr = $SC_$CPU_SB_CMDEC + 1
   
   Write " "
   Write "Sending a command with an error so that the error counter increments"
   ;  Note that this is an SB command with an invalid function code = hex 'aa' (42 decimal)
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
     ut_sendrawcmd "$SC_$CPU_SB", "1803c0000001aa"
   elseif ("$CPU" = "CPU2") then
     ut_sendrawcmd "$SC_$CPU_SB", "1823c0000001aa"
   elseif ("$CPU" = "CPU3") then
     ut_sendrawcmd "$SC_$CPU_SB", "1843c0000001aa"
   endif
   
   ut_tlmwait $SC_$CPU_SB_CMDEC, {error_cmd_ctr} 
   
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: Error counter incremented!"
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Invalid command."
     endif
   else
     write "<!> Error: Error counter did not increment!"
   endif
   
   wait 5

   Write "Sending the Reset Counters command"
   ut_setupevt "$SC", "$CPU", "CFE_SB", CFE_SB_CMD1_RCVD_EID, "DEBUG"

   ut_sendcmd "$SC_$CPU_SB_RESETCTRS"
   
   ; Check if the CMDPC counter reset
   ut_tlmwait $SC_$CPU_SB_CMDPC, 0
   
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: Command counter was reset!"
   else
     write "<!> Error: Command counter did not reset!"
   endif
   
   ut_tlmwait $SC_$CPU_SB_CMDEC, 0
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: Error counter was reset!"
   else
     write "<!> Error: Error counter did not reset!"
   endif
   
   if ($SC_$CPU_num_found_messages = 1) then
     Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
   else
     Write "<!> Error: Event Message not received for Reset Counters command."
   endif
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Dump Statistics Command
;  Command Code 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ut_setupevt "$SC", "$CPU", "CFE_SB", CFE_SB_SND_STATS_EID, "DEBUG"

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      packet_sequence_cnt = p00ascnt
   elseif ("$CPU" = "CPU2") then
      packet_sequence_cnt = p02ascnt
   elseif ("$CPU" = "CPU3") then
      packet_sequence_cnt = p04ascnt
   endif

   Write "Verifying the Dump Statistic Command"
   Write "Sending the command the Dump the sB Statistics Command"
   ut_sendcmd "$SC_$CPU_SB_DUMPSTATS"
   
   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Dump Statistics command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Dump Statistics command."
     endif
   else
     write "<!> Error: Did not process Dump Statistics command."
   endif

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      WAIT UNTIL (p00ascnt <> packet_sequence_cnt) 
   elseif ("$CPU" = "CPU2") then
      WAIT UNTIL (p02ascnt <> packet_sequence_cnt)
   elseif ("$CPU" = "CPU3") then
      WAIT UNTIL (p04ascnt <> packet_sequence_cnt)
   endif
   
   Write "SB Statistics Packet was received"
   Write "Need to add MORE verification (other than pkt scnt)"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Save Routing Info Command
;  Command Code 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      flindx = 3842 
   elseif ("$CPU" = "CPU2") then
      flindx = 3874
   elseif ("$CPU" = "CPU3") then
      flindx = 3906
   endif
   
   page $SC_$CPU_sb_hk
   page $SC_$CPU_sb_routing
   wait 5
   file_time = FILE_LIST[flindx].file_write_time
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
   wait 3
   Write "Sending the Save Route Command Using default filename"  
   Write "Calling procedure to save and dump the route"
   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "cfe_sb_route.it_dat1","$CPU")

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[flindx].file_write_time )

   wait 5
   file_time = FILE_LIST[flindx].file_write_time
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
   wait 3
   Write "Now try sending writing the file and specify the filename that you want to use"
   start get_file_to_cvt("RAM:0", "cfe_sb_myroute.dat", "cfe_sb_route.it_dat2","$CPU","SB_ROUTE")
   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[flindx].file_write_time )

   Write "Need to verify SB Route table manually"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enable and Disable Route Command
;  Command Code 4
;  Command Code 5
;  ASSUMPTION:  SB Routing table entry for Time diagnostic is 10!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC

   Write "Sending the Enable and Disable Route Command" 
   ;  Send a command to disable the route for the Time Diagnostic Packet to TO
   Write "Disabling an arbitrary message to an arbitrary pipe but hoping that it is the Time Diagnostic packet to TO"
   Write "First creating the sb route file and dumping to cvt"
   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "sb_route.it_dat2", "$CPU")
   
   pipe_id = 7  
   if ("$CPU" = "CPU1" or "$CPU" = "") then
      message_id = x'806'
   elseif ("$CPU" = "CPU2") then
      message_id = x'826'
   elseif ("$CPU" = "CPU3") then
      message_id = x'846'
   endif
   
   ; Determine the routing table entry for the given message id
   rtindx = 1
   endloop = 0
   route_entry = -1
   while ( endloop = 0 ) do
      if ($SC_$CPU_SB_RouteEntry[rtindx].SB_MSGID = message_id) then
         route_entry = rtindx
         endloop = 1
      elseif ($SC_$CPU_SB_RouteEntry[rtindx].SB_APPNAME = "") then
         endloop = 1
      elseif (rtindx >= 16320) then
         endloop = 1
      endif
      rtindx = rtindx + 1
   enddo
   
   if (route_entry = -1) then
      write "ERROR! There is no routing table entry for the Time Diagnostic Packet!"
      wait
   else
      write "Routing table index is " & route_entry
   endif

   Write "SETUP:"
   write "Message " & message_id & " is being disable from pipe " &pipe_id
   write "Current state of this message to this pipe BEFORE disabling is " & p@$SC_$CPU_SB_RouteEntry[route_entry].SB_STATE 
   if (p@$SC_$CPU_SB_RouteEntry[route_entry].SB_STATE <> "ENA") then
       write "<!> Error: Messsage was marked disabled when it should have been enabled."
   else
       write "<*> Success: Message marked enabled as expected!"
   endif  
   
   wait 5
   
   ; Send command ahead of time to make sure the ground system has valid packet
   ; sequence count value (not a stale value), otherwise first run after startup
   ; would incorrectly be read as a value of 0
   ut_sendcmd "$SC_$CPU_TIME_REQUESTDIAG"
   
   wait 5

   Write "Sending the command to TIME to request the diagnostic packet to make sure that we can get it."
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      packet_sequence_cnt = p006scnt
   elseif ("$CPU" = "CPU2") then
      packet_sequence_cnt = p026scnt
   elseif ("$CPU" = "CPU3") then
      packet_sequence_cnt = p046scnt
   endif
   
   write "Initial packet_sequence_cnt = " & packet_sequence_cnt
   
   wait 5

   ut_sendcmd "$SC_$CPU_TIME_REQUESTDIAG"

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p006scnt, {packet_sequence_cnt + 1}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p026scnt, {packet_sequence_cnt + 1}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p046scnt, {packet_sequence_cnt + 1}
   endif


   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the Time diagnostic packet as expected!"
   else
      write "<!> Error: Did not receive the Time diagnostic packet when we should have."
   endif

   Write "END SETUP:  Now testing Disable SB Route Command"
   write "Now disabling the message from the pipe"
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
 
   wait 2

   ut_setupevt $SC, $CPU, CFE_SB, CFE_SB_DSBL_RTE2_EID, DEBUG
   ut_sendcmd "$SC_$CPU_SB_DISROUTE MID=message_id  PID = pipe_id"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Disable Route command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Disable Route command."
     endif
   else
     write "<!> Error: Did not process Disable Route command."
   endif

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))

   wait 5

   Write "Dumping the sbroute.dat file again"
   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "sb_route.it_dat3","$CPU")
   ; Verify that the routing table shows that it is disabled
   wait 10
   
   write "Current state of this message to this pipe AFTER disabling is " & p@$SC_$CPU_SB_RouteEntry[route_entry].SB_STATE 
   if (p@$SC_$CPU_SB_RouteEntry[route_entry].SB_STATE <> "DIS") then
       write "<!> Error: Messsage was marked enabled when it should have been disabled."
   else
       write "<*> Success: Message marked disabled as expected!"
   endif  
   
   wait 5

   Write "Now send the command to TIME to request the diagnostic packet."
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      packet_sequence_cnt = p006scnt
   elseif ("$CPU" = "CPU2") then
      packet_sequence_cnt = p026scnt
   elseif ("$CPU" = "CPU3") then
      packet_sequence_cnt = p046scnt
   endif
    
   write "Initial packet_sequence_cnt = " & packet_sequence_cnt
   
   wait 5
  
   ut_sendcmd "$SC_$CPU_TIME_REQUESTDIAG"

   if ((("$CPU" = "CPU1" OR "$CPU" = "") AND (packet_sequence_cnt <> p006scnt)) ;;
     OR (("$CPU" = "CPU2") AND (packet_sequence_cnt <> p026scnt)) ;;
     OR (("$CPU" = "CPU3") AND (packet_sequence_cnt <> p046scnt))) then
      write "<!> Error: Received the packet when we should not have!"
   else
      write "<*> Success: Did not receive the packet"
   endif

   write " "
   Write "Now Testing the Enable SB Route Command"
   
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC

   ut_setupevt $SC, $CPU, CFE_SB, CFE_SB_ENBL_RTE2_EID, DEBUG
   ut_sendcmd "$SC_$CPU_SB_ENAROUTE MID=message_id  PID = pipe_id"

   if (UT_SC_Status = UT_SC_Success) then
     write "<*> Success: Enable Route command processed."
     if ($SC_$CPU_num_found_messages = 1) then
       Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
     else
       Write "<!> Error: Event Message not received for Enable Route command."
     endif
   else
     write "<!> Error: Did not process Enable Route command."
   endif

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))

   wait 5

   Write "Dumping the sbroute.dat file again"

   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "sb_route.it_dat4","$CPU")
   ; Verify that the routing table shows that it is enabled
   wait 10
   write "Current state of this message to this pipe AFTER enabling is " & p@$SC_$CPU_SB_RouteEntry[route_entry].SB_STATE 
   if (p@$SC_$CPU_SB_RouteEntry[route_entry].SB_STATE <> "ENA") then
       write "<!> Error: Messsage was marked disabled when it should have been enabled."
   else
       write "<*> Success: Message marked enabled as expected!"
   endif  

   wait 5
   
   Write "Now send the command to TIME to request the diagnostic packet."
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      packet_sequence_cnt = p006scnt
   elseif ("$CPU" = "CPU2") then
      packet_sequence_cnt = p026scnt
   elseif ("$CPU" = "CPU3") then
      packet_sequence_cnt = p046scnt
   endif
   
   write "Initial packet_sequence_cnt = " & packet_sequence_cnt
   
   wait 5

   ut_sendcmd "$SC_$CPU_TIME_REQUESTDIAG"

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      ut_tlmwait p006scnt, {packet_sequence_cnt + 2}
   elseif ("$CPU" = "CPU2") then
      ut_tlmwait p026scnt, {packet_sequence_cnt + 2}
   elseif ("$CPU" = "CPU3") then
      ut_tlmwait p046scnt, {packet_sequence_cnt + 2}
   endif

   if (UT_TW_Status = UT_Success) then
      write "<*> Success: Received the Time diagnostic packet as expected!"
   else
      write "<!> Error: Did not receive the Time diagnostic packet when we should have."
   endif

   wait 5
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Save Pipe Info Command
;  Command Code 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      flindx = 3843 
   elseif ("$CPU" = "CPU2") then
      flindx = 3875
   elseif ("$CPU" = "CPU3") then
      flindx = 3907
   endif
   
   page $SC_$CPU_sb_hk
   page $SC_$CPU_sb_pipe
   wait 3
   file_time = FILE_LIST[flindx].file_write_time
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
   wait 3
   Write "Sending the Save Pipe Command Using Default filename"  
   Write "Calling procedure to save and dump the route"
   start get_file_to_cvt("RAM:0", "cfe_sb_pipe.dat", "cfe_sb_pipe.it_dat1","$CPU")

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[flindx].file_write_time )

   file_time = FILE_LIST[flindx].file_write_time
   wait 3
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
   Write "Now try sending writing the file and specify the filename that you want to use"
   start get_file_to_cvt("RAM:0", "cfe_sb_mypipe.dat", "cfe_sb_pipe.it_dat2","$CPU","SB_PIPE")

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[flindx].file_write_time )
  
   Write "Need to verify SB Pipe table manually"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Save Map Info Command
;  Command Code 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      flindx = 3844 
   elseif ("$CPU" = "CPU2") then
      flindx = 3876
   elseif ("$CPU" = "CPU3") then
      flindx = 3908
   endif
   
   page $SC_$CPU_sb_hk
   page $SC_$CPU_sb_msgmap
   wait 3
   file_time = FILE_LIST[flindx].file_write_time
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
   wait 2
   Write "Sending the Save Map Command using default filename"  
   Write "Calling procedure to save and dump the route"
   wait 5
   start get_file_to_cvt("RAM:0", "cfe_sb_msgmap.dat", "cfe_sb_map.it_dat1","$CPU")

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[flindx].file_write_time )

   wait 4
   file_time = FILE_LIST[flindx].file_write_time
   success_cmd_ctr = $SC_$CPU_SB_CMDPC
   error_cmd_ctr = $SC_$CPU_SB_CMDEC
   wait 4

   Write "Now try sending writing the file and specify the filename that you want to use"
   wait 5
   start get_file_to_cvt("RAM:0", "cfe_sb_mymap.dat", "cfe_sb_map.it_dat2","$CPU","SB_MSGMAP")
   wait 2

   WAIT UNTIL (($SC_$CPU_SB_CMDPC = success_cmd_ctr + 1) AND ($SC_$CPU_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[flindx].file_write_time )
 
   Write "Need to verify SB Map table manually"

procend:

   Write "Test complete"
endproc
