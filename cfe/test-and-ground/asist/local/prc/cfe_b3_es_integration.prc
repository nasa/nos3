proc cfe_b3_es_integration
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform Build Integration Testing of Executive Services.  
;
; Note:  At the time of the writing of this procedure, there were no utilities to look for a particular event 
;         message
;        Also note that this procedure is written for CPU3.  It will be made generic at a later time
;
; History:
;
;  18APR05    Initial development of this proc         MOB
;
;
;
%LIV(USE_ASK_POPUP) = TRUE
local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt
local proc_error_cnt
local event_packet_ctr
local file_time
local app_info_file_index
local found_app


;LIMIT ON CDH_ROUTE.NSBSTREAM
;LIMIT ON ACE_ROUTE.NSBSTREAM
;LIMIT ON PSE_ROUTE.NSBSTREAM

;LIMIT ON P019
;LIMIT ON P039
;LIMIT ON P059


page scx_cpu3_es_hk
page scx_cpu3_es_app_info

newlog

Write "The cFE Build Label is =======> " & SCX_CPU3_ES_CFEMAJORVER & "." & SCX_CPU3_ES_CFEMINORVER
Write "Build Integration Testing of EVS"
Write "Checksum of this version is ====>  "& SCX_CPU3_ES_CKSUM
Write " "
Write "The OS Build Label is ========> " & SCX_CPU3_ES_OSMAJORVER & "." & SCX_CPU3_ES_OSMINORVER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Setup for test
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;  In build 3, Event Messages of type DEBUG are disabled by default.  Need to enable 
   success_cmd_ctr = SCX_CPU3_EVS_CMDPC
   error_cmd_ctr = SCX_CPU3_EVS_CMDEC
   wait 5
   Write "Sening a command to Enable Event Messages of type DEBUG"
   /SCX_CPU3_EVS_ENAEVENTTYPE DEBUG

   WAIT UNTIL ((SCX_CPU3_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU3_EVS_CMDEC = error_cmd_ctr))
   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify No-Op command (Command Code 0)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   Write "*********  Verifying the No-op command*********"
   Write " Sending the no-op command "

   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC

   /SCX_CPU3_ES_NOOP

   WAIT UNTIL ((SCX_CPU3_ES_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU3_ES_CMDEC = error_cmd_ctr))

   Write "No-Op Counter command SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Reset Counters Command (Command Code 1)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC
   Write " "
   Write "************* Verifying the Reset Counter Command************"
   Write "Sending a command with an error so that the error counter increments"
   ;  Note that this is an ES command with an invalid function code = hex 'aa' (42 decimal)
   /raw 1846c0000001aa

   WAIT UNTIL ((SCX_CPU3_ES_CMDPC = success_cmd_ctr) AND (SCX_CPU3_ES_CMDEC = error_cmd_ctr + 1))

   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC
   
   Write "Sending the Reset Counters command"
   /SCX_CPU3_ES_RESETCTRS
   WAIT UNTIL ((SCX_CPU3_ES_CMDPC = 0) AND (SCX_CPU3_ES_CMDEC = 0))

   Write "Reset Counter command SUCCESSFUL"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify ES Restart Command (Command Code 2)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC
   event_packet_ctr = p048scnt
   
   Write "  "
   Write "*************** Verifying the ES Restart command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify ES Shell Command (Command Code 3)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC

   Write " "
   Write " *********** Verifying the ES Shell Command  *******"

   Write " NOT VALID FOR BUILD 3"
   Write " "

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify ES Start Application Command (Command Code 4)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC
   found_app = FALSE
  
   Write " "
   Write " ************ Verifying the Starting of an Application *********"
   Write " Using the utility procedure to load and start an app - load_start_app.prc"
   Write " Load and start the App"

   start load_start_app ("TST_EVS", "CPU3", "TST_EVS_TaskMain") 

   Write " Verify that ES registered the app by dumping the ES App Info File"

   s get_file_to_cvt ("RAM:", "es_app_info.log", "scx_cpu3_es_app_info.log", "3")
   ;Loop thru the table looking for the TST_EVS app
   for app_info_file_index = 1 to 32 do
      if (SCX_CPU3_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_EVS") then
         found_app = TRUE
      endif
   enddo

   if (found_app <> TRUE) then
      write " ERROR.  Did not find the application that was just loaded"
      wait 1000
      goto procerror
   else
      write "The Application was found in the ES Application Info File"
      write "Send a command to the test Application and verify that it was received"
      event_packet_ctr = p048scnt
      wait 5
      /SCX_CPU3_TST_EVS_NOOP
      wait until (p048scnt = event_packet_ctr + 1)
      write "ES Start Application Command was successful"
    endif


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   
;   (Command Code 5 UNDEFINED)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Send a Reset Warm Command
;   (Command Code 6)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
   Write "  "
   Write "*************** Verifying the ES Reset WARM command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Send a Reset Cold Command
;   (Command Code 7)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   Write "  "
   Write "*************** Verifying the ES Reset COLD command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Request Info about a cFE Application
;   (Command Code 8)
;  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   Write "  "
   Write "*************** Verifying the ES Query One Command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Request Info about all of the cFE Applications
;   (Command Code 9)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
   success_cmd_ctr = SCX_CPU3_ES_CMDPC
   error_cmd_ctr = SCX_CPU3_ES_CMDEC
   wait 3

   Write "  "
   Write "*************** Verifying the ES Query All Apps Command *********"
   Write "Using the get_file_to_cvt.prc to request and dump the info"
  
   s get_file_to_cvt ("RAM:", "es_app_info.log", "scx_cpu3_es_app_info.log", "3")
   ;  Loop through the tasks and write the info to the log and check if expected tasks are there

   for app_info_file_index = 1 to 32 do
      if (SCX_CPU3_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_EVS") then
         found_app = TRUE
      endif
   enddo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Send Clear Activity Log Command
;   (Command Code 10)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   Write "  "
   Write "*************** Verifying the ES Clear Activity Log Command  *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Write the Activity Log Command

;   (Command Code 11)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   Write "  "
   Write "*************** Verifying the ES Write Activity Log Command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Send a Clear Mode Log Command
;   (Command Code 12)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
 
   Write "  "
   Write "*************** Verifying the ES Clear Mode Log Command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Send a Write Mode Log Command
;   (Command Code 13)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   Write "  "
   Write "*************** Verifying the Write Mode Log Command *********"
 
   Write " NOT VALID FOR BUILD 3"
   Write " "

goto procterm

procerror:

procterm:
     Write "Test complete"
     ; Now clean up
     newlog 
endproc
