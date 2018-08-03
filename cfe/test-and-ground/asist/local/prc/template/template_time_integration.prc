proc $SC_$CPU_time_integration
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform Build Integration Testing of Time Services.  
;
; Note:  This test only tests the time server functions
;
; History:
;
;  18APR05    Initial development of this proc         MOB
;  10JAN08    Updated for cFE 5.0                      SJJ
;
;
;
%LIV(USE_ASK_POPUP) = TRUE

#include "ut_statusdefs.h"
#include "cfe_time_events.h"


local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt, pipe_id, message_id
local proc_error_cnt
local time_diag_packet_ctr, new_time_diag_packet_ctr
local time_hk_packet_ctr
local seconds, subseconds, microseconds
local current_stcf_sec 
local current_stcf_subsec 

;LIMIT ON CDH_ROUTE.NSBSTREAM
;LIMIT ON ACE_ROUTE.NSBSTREAM
;LIMIT ON PSE_ROUTE.NSBSTREAM

;LIMIT ON P019
;LIMIT ON P039
;LIMIT ON P059


page $SC_$CPU_time_hk

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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write "*********  Verifying the No-op command*********"
   Write " Sending the no-op command "

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO
   
   ut_sendcmd "$SC_$CPU_TIME_NOOP"

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
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Reset Counters Command
;  Command Code 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_CC_ERR_EID, "ERROR"
   
   error_cmd_ctr = $SC_$CPU_TIME_CMDEC + 1
   
   Write "Sending a command with an error so that the error counter increments"
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
     ut_sendrawcmd "$SC_$CPU_TIME", "1805c0000001aa"
   elseif ("$CPU" = "CPU2") then
     ut_sendrawcmd "$SC_$CPU_TIME", "1825c0000001aa"
   elseif ("$CPU" = "CPU3") then
     ut_sendrawcmd "$SC_$CPU_TIME", "1845c0000001aa"
   endif
   
   ut_tlmwait $SC_$CPU_TIME_CMDEC, {error_cmd_ctr} 
   
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

   Write "Sending the Reset Counters command"

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_RESET_EID, "DEBUG"

   ut_sendcmd "$SC_$CPU_TIME_RESETCTRS"
   
   ; Check if the CMDPC counter reset
   ut_tlmwait $SC_$CPU_TIME_CMDPC, 0
   
   if (UT_TW_Status = UT_Success) then
     write "<*> Success: Command counter was reset!"
   else
     write "<!> Error: Command counter did not reset!"
   endif
   
   ut_tlmwait $SC_$CPU_TIME_CMDEC, 0
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
;  Verify the request of a time diagnostic packet
;  Command Code 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_DIAG_EID, DEBUG

   success_cmd_ctr = $SC_$CPU_TIME_CMDPC + 1
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      time_diag_packet_ctr = p006scnt
   elseif ("$CPU" = "CPU2") then
      time_diag_packet_ctr = p026scnt
   elseif ("$CPU" = "CPU3") then
      time_diag_packet_ctr = p046scnt
   endif
   
   Write "  "
   Write "*************** Verifying the Request to send a time diagnostic packet  *********"
   Write "Sending the command to request a time diagnostic packet"  

   write "time_diag_packet_ctr is currently " & time_diag_packet_ctr
   ut_sendcmd "$SC_$CPU_TIME_REQUESTDIAG"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Request Diag command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Request Diag command."
      endif
   else
       Write "<!> Error: Request Diag command failed!"
   endif
   
   if ("$CPU" = "CPU1" OR "$CPU" = "") then
      new_time_diag_packet_ctr = p006scnt
   elseif ("$CPU" = "CPU2") then
      new_time_diag_packet_ctr = p026scnt
   elseif ("$CPU" = "CPU3") then
      new_time_diag_packet_ctr = p046scnt
   endif
   
   if (new_time_diag_packet_ctr <> time_diag_packet_ctr) then
      Write "<*> Success: Time Diagnostic Packet Counter changed!"
   else
      Write "<!> Error: Time Diagnostic Packet Counter didn't change."
   endif
   
   wait 10
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set Time Source
;  Command Code 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write " "
   Write "*********** Verifying the Ability to Set Time Source (Produces Error) *******"

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_SOURCE_CFG_EID, "ERROR"

   ut_sendcmd "$SC_$CPU_TIME_SETSOURCE EXTERNAL"
   if (UT_SC_Status = UT_SC_CmdFailure) then
      write "<*> Success: Set Time Source command failed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Time Source command."
      endif
   else
       Write "<!> Error: Set Time Source command succeeded when it should have failed."
   endif

   wait 5
 
   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_SOURCE_CFG_EID, "ERROR"

   ut_sendcmd "$SC_$CPU_TIME_SETSOURCE INTERNAL"
   if (UT_SC_Status = UT_SC_CmdFailure) then
      write "<*> Success: Set Time Source command failed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Time Source command."
      endif
   else
       Write "<!> Error: Set Time Source command succeeded when it should have failed."
   endif
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set Time State
;  Command Code 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   wait 2
   Write " "
   Write "*********** Verifying the Ability to Set Time State *******"
   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_STATE_EID, "INFO"
   
   ut_sendcmd "$SC_$CPU_TIME_SETSTATE FLYWHEEL"
   
   wait 5

   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Time State command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Time State command."
      endif
   else
       Write "<!> Error: Set Time State command failed!"
   endif

   ; verify the telemetry
   if ((p@$SC_$CPU_TIME_FLAGSFLY = "FLY") AND (p@$SC_$CPU_TIME_FLAGCFLY = "YES")) then
      write "<*> Success: TLM points indicate FLYWHEEL State!"
   else
      write "<!> Error: TLM points do not indicate Flywheel State entered."
   endif 

   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Add Clock Latency
;  Command Code 5
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write " "
   Write "*********** Verifying the Ability to Add Clock Latency (Produces Error) *******"

   seconds = 300
   microseconds = 125000 ; 1/8 second

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_DELAY_CFG_EID, "ERROR"

   ut_sendcmd "$SC_$CPU_TIME_AddClockLat SECONDS = seconds MICROSECS = microseconds"
   if (UT_SC_Status = UT_SC_CmdFailure) then
      write "<*> Success: Add Clock Latency command failed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Add Clock Latency command."
      endif
   else
       Write "<!> Error: Add Clock Latency command succeeded when it should have failed."
   endif
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Subtract Clock Latency
;  Command Code 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write " "
   Write "*********** Verifying the Ability to Subtract Clock Latency (Produces Error) *******"

   seconds = 300
   microseconds = 125000 ; 1/8 second

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_DELAY_CFG_EID, "ERROR"

   ut_sendcmd "$SC_$CPU_TIME_SubClockLat SECONDS = seconds MICROSECS = microseconds"
   if (UT_SC_Status = UT_SC_CmdFailure) then
      write "<*> Success: Subtract Clock Latency command failed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Subtract Clock Latency command."
      endif
   else
       Write "<!> Error: Subtract Clock Latency command succeeded when it should have failed."
   endif
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Clock
;  Command Code 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
  
   Write " "
   Write "*********** Verifying the Ability to Set Clock *******"

   seconds = 300
   microseconds = 125000 ; 1/8 second

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_TIME_EID, "INFO"

   ut_sendcmd "$SC_$CPU_TIME_SetClock SECONDS = seconds MICROSECS = microseconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Clock command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Clock command."
      endif
   else
       Write "<!> Error: Set Clock command failed!"
   endif
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Clock Mission Elapsed Time (MET)
;  Command Code 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

   seconds = 400000
   Write "Verify the ability to set the MET seconds"
   Write "Since MET is changing, the verification of MET is within a few seconds"

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_MET_EID, INFO

   Write "Send a command to set the MET. Seconds = " &  seconds 
   
   ut_sendcmd "$SC_$CPU_TIME_SETCLOCKMET SECONDS = seconds MICROSECS = 0"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Clock MET command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Clock MET command."
      endif
   else
       Write "<!> Error: Set Clock MET command failed!"
   endif
   
   wait 5

   ;Verify that the MET is within 8 seconds of the current value (MET updates every 4 seconds)
   write "MET Seconds are currently " & $SC_$CPU_TIME_METSECS
   if ($SC_$CPU_TIME_METSECS <= (seconds + 8)) then
      write "<*> Success: Loosely Verified that the MET Seconds were set!"
   else
      write "<!> Error: MET Seconds did not verify."
   endif
   
   wait 10
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Spacecraft Time Correlation Factor (STCF)
;  Command Code 9
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

Time_checkout:

   seconds = 300
   microseconds = 125000 ; 1/8 second
   subseconds = x'20000000' ; hex value of subseconds

   Write " "
   Write "*********** Verifying the Ability to Set the STCF *******"
   Write "Sending command to set the STCF to " & seconds & "  " & subseconds
   
   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_STCF_EID, INFO

   ut_sendcmd "$SC_$CPU_TIME_SETCLOCKSTCF SECONDS= seconds MICROSECS = microseconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Clock STCF command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Clock STCF command."
      endif
   else
       Write "<!> Error: Set Clock STCF command failed!"
   endif

   wait 5

   if (($SC_$CPU_TIME_STCFSECS = seconds) AND ($SC_$CPU_TIME_STCFSUBSECS = subseconds)) then
      write "<*> Success: The STCF was set!"
   else
      write "<!> Error: The STCF was not set correctly."
   endif
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Leap Seconds
;  Command Code 10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   seconds = 50

   Write " "
   Write "*********** Verifying the Ability to Set the Leap Seconds *******"
   Write "Sending command to set leap seconds to " & seconds

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_LEAPS_EID, INFO

   ut_sendcmd "$SC_$CPU_TIME_SETCLOCKLEAP LEAPS = seconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Leap Seconds command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Leap Seconds command."
      endif
   else
       Write "<!> Error: Set Leap Seconds command failed!"
   endif

   if ($SC_$CPU_TIME_LEAPSECS = seconds) then
      write "<*> Success: Leap Seconds were set!"
   else
      write "<!> Error: Leap seconds were not set correctly."
   endif
   
   wait 10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Add adjustment of the STCF
;  Command 11
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
   seconds = 10
   microseconds = 125000 ; 1/8 seconds
   subseconds = $SC_$CPU_TIME_STCFSUBSECS + x'20000000' ; x20000000 is 1/8 of a microsecond
   current_stcf_sec = $SC_$CPU_TIME_STCFSECS

   Write " "
   Write "*********** Verifying the Ability to Add an adjustment of the STCF  *******"
   Write "Current STCF Seconds is "& $SC_$CPU_TIME_STCFSECS & " Subseconds is " & %hex($SC_$CPU_TIME_STCFSUBSECS)
   Write "Sending command to Adjust the SCTF by seconds= " & seconds & " microseconds = " & microseconds

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO

   ut_sendcmd "$SC_$CPU_TIME_ADDSTCFADJ SECONDS = seconds MICROSECS = microseconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Add Adjustment command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Add Adjustment command."
      endif
   else
       Write "<!> Error: Set Add Adjustment command failed!"
   endif

   wait until (p@$SC_$CPU_TIME_FLAGADJD = "ADD")
   write "Current adjusted STCF is seconds = "& $SC_$CPU_TIME_STCFSECS & " Subseconds = " & %hex($SC_$CPU_TIME_STCFSUBSECS)
   If (($SC_$CPU_TIME_STCFSECS = (current_stcf_sec + seconds)) AND ($SC_$CPU_TIME_STCFSUBSECS = subseconds)) then
      write "<*> Success: Add Adjustment was set!"
   else
      write "<!> Error: Add Adjustment not set correctly."
   endif

   wait 10
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Subtract adjustment from the STCF
;  Command Code 12
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   seconds = 2
   microseconds = 250000
   subseconds = $SC_$CPU_TIME_STCFSUBSECS - x'40000000'
   current_stcf_sec = $SC_$CPU_TIME_STCFSECS
   current_stcf_subsec = $SC_$CPU_TIME_STCFSUBSECS

   Write " "
   Write "*********** Verifying the Ability to Subtract an adjustment from the STCF  *******"
   Write "Current STCF Seconds is "& $SC_$CPU_TIME_STCFSECS & " Subseconds is " & %hex($SC_$CPU_TIME_STCFSUBSECS)
   Write "Sending command to Adjust the SCTF by seconds= " & seconds & "subseconds = " & microseconds

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_DELTA_EID, INFO

   ut_sendcmd "$SC_$CPU_TIME_SUBSTCFADJ SECONDS = seconds MICROSECS = microseconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Set Subtract Adjustment command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Subtract Adjustment command."
      endif
   else
       Write "<!> Error: Set Subtract Adjustment command failed!"
   endif

   wait until (p@$SC_$CPU_TIME_FLAGADJD = "SUBTRACT")
   write "Current adjusted STCF is seconds = "& $SC_$CPU_TIME_STCFSECS & " Subseconds = " & %hex($SC_$CPU_TIME_STCFSUBSECS)
   If (($SC_$CPU_TIME_STCFSECS = (current_stcf_sec - seconds)) AND ($SC_$CPU_TIME_STCFSUBSECS = subseconds)) then
      write "<*> Success: Subtract Adjustment was set!"
   else
      write "<!> Error: Subtract Adjustment not set correctly."
   endif

   wait 10
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Add a 1HZ adjustment to the STCF
;  Command Code 13
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   seconds = 3
   microseconds = 20000 ; ????? Need to determine
   subseconds = 20

   Write " "
   Write "*********** Verifying the Ability to Continually Add a 1 Hz adjustment to the STCF  *******"  
   Write "Starting a sequential print to capture the time"
   seq print on ;;
         $SC_$CPU_TIME_1HZADJSECS using %d ;;
         $SC_$CPU_TIME_1HZADJSSECS using %8x ;;
         p@$SC_$CPU_TIME_FLAG1HZD ;;
         $SC_$CPU_TIME_METSECS using %10d ;;
         $SC_$CPU_TIME_METSUBSECS using %8x ;;
         $SC_$CPU_TIME_STCFSECS using %10d  ;;
         $SC_$CPU_TIME_STCFSUBSECS using %8x to cfe_b3_time_integration_add1hz.out
   
         
   Write "Sending command to 1HZ adjust (Add) the SCTF by seconds= " & seconds & " subseconds = " & subseconds

   wait 20
   ut_sendcmd "$SC_$CPU_TIME_ADD1HZSTCF SECONDS= seconds MICROSECS=subseconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Add 1HZ Adjustment to STCF command succeeded."
   else
       Write "<!> Error: Add 1HZ Adjustment to STCF command failed!"
   endif

   WAIT UNTIL (p@$SC_$CPU_TIME_FLAG1HZD = "ADD")

   Write "Need manual verification of data in sequential print file"
   wait 100
   seq print off all

   Write "Now stop the adjustment"

   ut_sendcmd "$SC_$CPU_TIME_STOPADD1HZ"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Stop Add 1HZ Adjustment to STCF command succeeded."
   else
       Write "<!> Error: Stop Add 1HZ Adjustment to STCF command failed!"
   endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Subtract a 1HZ adjustment to the STCF
;  Command Code 14
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
   seconds = 1
   subseconds = 1000

   Write " "
   Write "*********** Verifying the Ability to Continually Subtract a 1 Hz adjustment to the STCF  *******"   
   Write "Sending command to 1HZ adjust (Subtract) the SCTF by seconds= " & seconds & " subseconds = " & subseconds

   Write "Starting a sequential print to capture the time"
   seq print on ;;
         $SC_$CPU_TIME_1HZADJSECS using %d ;;
         $SC_$CPU_TIME_1HZADJSSECS using %8x ;;
         p@$SC_$CPU_TIME_FLAG1HZD ;;
         $SC_$CPU_TIME_METSECS using %10d ;;
         $SC_$CPU_TIME_METSUBSECS using %8x ;;
         $SC_$CPU_TIME_STCFSECS using %10d  ;;
         $SC_$CPU_TIME_STCFSUBSECS using %8x to cfe_b3_time_integration_sub1hz.out


   Write "Sending command to 1HZ adjust (Subtract) the SCTF by seconds= " & seconds & "subseconds = " & subseconds

   ut_setupevt $SC, $CPU, CFE_TIME, CFE_TIME_1HZ_EID, INFO
   
   wait 20
   ut_sendcmd "$SC_$CPU_TIME_SUB1HZSTCF SECONDS = seconds MICROSECS = subseconds"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Subtract 1HZ Adjustment to STCF command succeeded."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Subtract 1HZ Adjustment to STCF command."
      endif
   else
       Write "<!> Error: Subtract 1HZ Adjustment to STCF command failed!"
   endif

   wait until (p@$SC_$CPU_TIME_FLAG1HZD = "SUBTRACT")

   Write "Need manual verification of data in sequential print file"
   wait 100
   seq print off all

   Write "Now stop the adjustment"

   ut_sendcmd "$SC_$CPU_TIME_STOPADD1HZ"
   if (UT_SC_Status = UT_SC_Success) then
      write "<*> Success: Stop Add 1HZ Adjustment to STCF command succeeded."
   else
       Write "<!> Error: Stop Add 1HZ Adjustment to STCF command failed!"
   endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Time Signal
;  Command Code 15
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   Write " "
   Write "*********** Verifying the Ability to Set Time Signal (Produces Error) *******"

   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_SIGNAL_CFG_EID, "ERROR"

   ut_sendcmd "$SC_$CPU_TIME_SETSIGNAL REDUNDANT"
   if (UT_SC_Status = UT_SC_CmdFailure) then
      write "<*> Success: Set Time Signal command failed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Time Signal command."
      endif
   else
       Write "<!> Error: Set Time Signal command succeeded when it should have failed."
   endif

   wait 5
   
   ut_setupevt "$SC", "$CPU", "CFE_TIME", CFE_TIME_SIGNAL_CFG_EID, "ERROR"

   ut_sendcmd "$SC_$CPU_TIME_SETSIGNAL PRIMARY"
   if (UT_SC_Status = UT_SC_CmdFailure) then
      write "<*> Success: Set Time Signal command failed."
      if ($SC_$CPU_num_found_messages = 1) then
         Write "<*> Success: Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
      else
         Write "<!> Error: Event Message not received for Set Time Signal command."
      endif
   else
       Write "<!> Error: Set Time Signal command succeeded when it should have failed."
   endif
   
   
   Write "Test complete"
endproc
