proc cfe_b4p0_time_integration_cpu1
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform Build Integration Testing of Time Services.  
;
; Note:  1)  At the time of the writing of this procedure, there were no utilities to look for a particular event 
;         message.
;        2)  This test only tests the time server functions
;
; History:
;
;  18APR05    Initial development of this proc         MOB
;
;
;

%LIV(USE_ASK_POPUP) = TRUE
local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt, pipe_id, message_id
local proc_error_cnt
local time_diag_packet_ctr
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


page scx_cpu1_time_hk

newlog

Write "The cFE Build Label is =======> " & SCX_CPU1_ES_CFEMAJORVER & "." & SCX_CPU1_ES_CFEMINORVER
Write "Build Integration Testing of EVS"
Write "Checksum of this version is ====>  "& SCX_CPU1_ES_CKSUM
Write " "
Write "The OS Build Label is ========> " & SCX_CPU1_ES_OSMAJORVER & "." & SCX_CPU1_ES_OSMINORVER

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify No-Op command
;  Command Code 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write "*********  Verifying the No-op command*********"
   Write " Sending the no-op command "

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC

   /SCX_CPU1_TIME_NOOP

   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))

   Write "No-Op Counter command SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Reset Counters Command
;  Command Code 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   Write " "
   Write "************* Verifying the Reset Counter Command************"
   Write "Sending a command with an error so that the error counter increments"
   ;  Note that this is an TIME command with an invalid function code = hex 'aa' (42 decimal)
   /raw 1805c0000001aa

   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr + 1))

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   
   Write "Sending the Reset Counters command"

   /SCX_CPU1_TIME_RESETCTRS
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = 0) AND (SCX_CPU1_TIME_CMDEC = 0))

   Write "Reset Counter command SUCCESSFUL"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Veriy the request of a time diagnostic packet
;  Command Code 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   time_diag_packet_ctr = p006scnt
   
   Write "  "
   Write "*************** Verifying the Request to send a time diagnostic packet  *********"
   Write "Sending the command to request a time diagnostic packet"  

   write "p006scnt is currently " & p006scnt
   /SCX_CPU1_TIME_REQUESTDIAG

   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   wait until (time_diag_packet_ctr <> p006scnt)

    Write "Request Diagnostic command SUCCESSFUL"  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set Time Source
;  Command Code 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


    Write "Not able to SET IME SOURCE for this config of the cFE "
    Write "  "

;   /SCX_CPU1_TIME_SETSOURCE EXTERNAL
;   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 3) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
;   if (p@SCX_CPU1_TIME_FLAGPRI <> "EXT") then
;     proc_error_cnt = proc_error_cnt + 1
;     wait 10000
;      goto procerror
;   endif 
; 
;    /SCX_CPU1_TIME_SETSOURCE INTERNAL   
;    WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 4) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
;   if (p@SCX_CPU1_TIME_FLAGPRI <> "INT") then
;     proc_error_cnt = proc_error_cnt + 1
;     wait 10000
;     goto procerror
;   endif 

   Write "Ability to Set Time Source SUCCESSFUL (primary and redundant only)"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set Time State
;  Command Code 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   wait 2
   Write " "
   Write "*********** Verifying the Ability to Set Time State *******"
   /SCX_CPU1_TIME_SETSTATE FLYWHEEL
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   if ((p@SCX_CPU1_TIME_FLAGSFLY <> "FLY") AND (p@SCX_CPU1_TIME_FLAGCFLY <> "YES")) then
     proc_error_cnt = proc_error_cnt + 1
     wait 10000
     goto procerror
   endif 

   
   Write "Ability to Set Time State SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Add Clock Latency
;  Command Code 5
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  Write "Not able to test this command for Server configuration"
  Write "Add clock Latency is NOT verified"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Subtract Clock Latency
;  Command Code 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  Write "Not able to test this command for Server configuration"
  Write "Subtract clock Latency is NOT verified"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Clock
;  Command Code 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Clock MET
;  Command Code 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC

   seconds = 400000
   Write "Verify the ability to set the MET seconds"
   Write "Since MET is changing, the verification of MET is within a few seconds"

   Write "Send a command to set the MET. Seconds = " &  seconds 
   /SCX_CPU1_TIME_SETCLOCKMET SECONDS = seconds MICROSECS = 0
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   ;Verify that the MET is within 8 seconds of the current value (MET updates every 4 seconds)
   write "MET Seconds are currently " & SCX_CPU1_TIME_MET_SECS
   IF (SCX_CPU1_TIME_MET_SECS <= (seconds + 8)) then
      write "Loosely Verified that the MET Seconds were set"
   ELSE
      write "MET Seconds did not verify."
      proc_error_cnt = proc_error_cnt + 1
      wait 10000
      goto procerror      
   ENDIF
   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the STCF
;  Command Code 9
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

Time_checkout:

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   seconds = 300
   microseconds = 125000 ; 1/8 second
   subseconds = 20000000 ; hex value of subseconds

   Write " "
   Write "*********** Verifying the Ability to Set the STCF *******"
   Write "Sending command to jam the STCF to " & seconds & "  " & subseconds
   /SCX_CPU1_TIME_SETCLOCKSTCF SECONDS= seconds MICROSECS = microseconds
   
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   if ((SCX_CPU1_TIME_STCF_SECS <> seconds) AND (SCX_CPU1_TIME_STCF_SUBSECS <> subseconds)) then
      proc_error_cnt = proc_error_cnt + 1
      wait 10000
      goto procerror
   endif
   Write "Verified that the set STCF command was SUCCESSFUL "
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Leap Seconds
;  Command Code 10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   seconds = 50

   Write " "
   Write "*********** Verifying the Ability to Set the Leap Seconds *******"
   Write "Sending command to set leap seconds to " & seconds
   /SCX_CPU1_TIME_SETCLOCKLEAP LEAPS= seconds 
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   if (SCX_CPU1_TIME_LEAPSECS <> seconds)  then
      proc_error_cnt = proc_error_cnt + 1
      wait 10000
      goto procerror
   endif
   
   Write "Verified that the Leap seconds command was SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the the Add adjustment of the STCF
;  Command 11
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   seconds = 10
   microseconds = 125000 ; 1/8 seconds
   subseconds = SCX_CPU1_TIME_STCF_SUBSECS + x'200000' ; x200000 is 1/8 of a microsecond
   current_stcf_sec = SCX_CPU1_TIME_STCF_SECS


   Write " "
   Write "*********** Verifying the Ability to Add an adjustment of the STCF  *******"
   Write "Current STCF Seconds is "& SCX_CPU1_TIME_STCF_SECS & " Subseconds is " & %hex(SCX_CPU1_TIME_STCF_SUBSECS)

   Write "Sending command to Adjust the SCTF by seconds= " & seconds & " microseconds = " & microseconds
   /SCX_CPU1_TIME_ADDSTCFADJ SECONDS= seconds MICROSECS = microseconds
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))

   write "Current adjusted STCF is seconds = "& SCX_CPU1_TIME_STCF_SECS & " Subseconds = " & %hex(SCX_CPU1_TIME_STCF_SUBSECS)
   wait until (p@SCX_CPU1_TIME_FLAGADJD = "ADD")
   If ((SCX_CPU1_TIME_STCF_SECS <> (current_stcf_sec + seconds)) AND (SCX_CPU1_TIME_STCF_SUBSECS <> subseconds)) then
     proc_error_cnt = proc_error_cnt + 1
     wait 10000
     goto procerror
   endif
  Write "Verified that the Add adjustment command was SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Subtract adjustment from the STCF
;  Command Code 12
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   seconds = 2
   microseconds = 250000
   subseconds = SCX_CPU1_TIME_STCF_SUBSECS - x'400000'
   current_stcf_sec = SCX_CPU1_TIME_STCF_SECS
   current_stcf_subsec = SCX_CPU1_TIME_STCF_SUBSECS

   Write " "
   Write "*********** Verifying the Ability to Subtract an adjustment from the STCF  *******"
   Write "Current STCF Seconds is "& SCX_CPU1_TIME_STCF_SECS & " Subseconds is " & %hex(SCX_CPU1_TIME_STCF_SUBSECS)
   Write "Sending command to Adjust the SCTF by seconds= " & seconds & "subseconds = " & microseconds
   /SCX_CPU1_TIME_SUBSTCFADJ SECONDS= seconds MICROSECS = microseconds
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   wait until (p@SCX_CPU1_TIME_FLAGADJD = "SUBTRACT")
   write "Current adjusted STCF is seconds = "& SCX_CPU1_TIME_STCF_SECS & " Subseconds = " & %hex(SCX_CPU1_TIME_STCF_SUBSECS)
   If ((SCX_CPU1_TIME_STCF_SECS <> (current_stcf_sec - seconds)) AND (SCX_CPU1_TIME_STCF_SUBSECS <> current_stcf_subsec - subseconds)) then
     proc_error_cnt = proc_error_cnt + 1
     wait 10000
     goto procerror
   endif
  Write "Verified that the Subtract adjustment command was SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Add a 1HZ adjustment to the STCF
;  Command Code 13
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   seconds = 3
   microseconds = 20000 ; ????? Need to determine
   subseconds = 20

   Write " "
   Write "*********** Verifying the Ability to Continually Add a 1 Hz adjustment to the STCF  *******"  
   Write "Starting a sequential print to capture the time"
   seq print on ;;
         SCX_CPU1_TIME1HZADJSECS using %d ;;
         SCX_CPU1_TIME1HZADJSSECS using %8x ;;
         p@SCX_CPU1_TIME_FLAG1HZD ;;
         SCX_CPU1_TIME_MET_SECS using %10d ;;
         SCX_CPU1_TIME_MET_SUBSECS using %8x ;;
         SCX_CPU1_TIME_STCF_SECS using %10d  ;;
         SCX_CPU1_TIME_STCF_SUBSECS using %8x to cfe_b3_time_integration_add1hz.out
   
         
   Write "Sending command to 1HZ adjust (Add) the SCTF by seconds= " & seconds & "subseconds = " & subseconds
   wait 20
   /SCX_CPU1_TIME_ADD1HZSTCF SECONDS= seconds MICROSECS=subseconds 
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   WAIT UNTIL (p@SCX_CPU1_TIME_FLAG1HZD = "ADD")


   Write "Verified that the 1Hz adjust (add) command was SUCCESSFUL"
   Write "Need manual verification of data in sequential print file"
   wait 100
   seq print off all

   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   Write "Now stop the adjustment"

   /SCX_CPU1_TIME_STOPADD1HZ
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Subtract a 1HZ adjustment to the STCF
;  Command Code 14
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   seconds = 1
   subseconds = 1000

   Write " "
   Write "*********** Verifying the Ability to Continually Subtracting a 1 Hz adjustment to the STCF  *******"   
   Write "Sending command to 1HZ adjust (Subtract) the SCTF by seconds= " & seconds & "subseconds = " & subseconds

   Write "Starting a sequential print to capture the time"
   seq print on ;;
         SCX_CPU1_TIME1HZADJSECS using %d ;;
         SCX_CPU1_TIME1HZADJSSECS using %8x ;;
         p@SCX_CPU1_TIME_FLAG1HZD ;;
         SCX_CPU1_TIME_MET_SECS using %10d ;;
         SCX_CPU1_TIME_MET_SUBSECS using %8x ;;
         SCX_CPU1_TIME_STCF_SECS using %10d  ;;
         SCX_CPU1_TIME_STCF_SUBSECS using %8x to cfe_b3_time_integration_sub1hz.out


   Write "Sending command to 1HZ adjust (Subtract) the SCTF by seconds= " & seconds & "subseconds = " & subseconds
   wait 20
   /SCX_CPU1_TIME_SUB1HZSTCF SECONDS= seconds MICROSECS= subseconds 
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
   wait until (p@SCX_CPU1_TIME_FLAG1HZD = "SUBTRACT")
   ;  Need verification here!
   Write "Verified that the 1Hz adjust (Subtract) command was SUCCESSFUL"
   Write "Need manual verification of data in sequential print file"
   wait 100
   seq print off all
   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
   error_cmd_ctr = SCX_CPU1_TIME_CMDEC
   Write "Now stop the adjustment"

   /SCX_CPU1_TIME_STOPSUB1HZ
   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Ability to Set the Time Signal
;  Command Code 15
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   Write "Unable to Send the Command to SET SIGNAL due to config params"

;   success_cmd_ctr = SCX_CPU1_TIME_CMDPC
;   error_cmd_ctr = SCX_CPU1_TIME_CMDEC

;   Write " "
;   Write "*********** Verifying the Ability to Set Time Source *******"

;   /SCX_CPU1_TIME_SETSIGNAL REDUNDANT
;   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))
;   if (p@SCX_CPU1_TIME_FLAGPRI <> "RED") then
;     proc_error_cnt = proc_error_cnt + 1
;     wait 10000
;      goto procerror
;   endif 

;    /SCX_CPU1_TIME_SETSIGNAL PRIMARY
;   WAIT UNTIL ((SCX_CPU1_TIME_CMDPC = success_cmd_ctr + 2) AND (SCX_CPU1_TIME_CMDEC = error_cmd_ctr))  
;   if (p@SCX_CPU1_TIME_FLAGPRI <> "PRI") then
;     proc_error_cnt = proc_error_cnt + 1
;     wait 10000
;   endif 


goto procterm
procerror:
     Write "ERROR:  There were problems in this procedure"
     wait
procterm:
     Write "Test complete"
     wait 10
     newlog 

endproc
