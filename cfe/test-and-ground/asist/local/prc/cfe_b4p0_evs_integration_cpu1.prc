proc cfe_b4p0_evs_integration_cpu1
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform Build Integration Testing of Event Services.  
;
; Note:  At the time of the writing of this procedure, there were no utilities to look for a particular event 
;         message
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
local event_packet_ctr
local output_port
local file_time
local output_port_mask
local output_port_temp
local time_cmd_ctr

;LIMIT ON CDH_ROUTE.NSBSTREAM
;LIMIT ON ACE_ROUTE.NSBSTREAM
;LIMIT ON PSE_ROUTE.NSBSTREAM

;LIMIT ON P019
;LIMIT ON P039
;LIMIT ON P059


page scx_cpu1_evs_hk

newlog

Write "The cFE Build Label is =======> " & SCX_CPU1_ES_CFEMAJORVER & "." & SCX_CPU1_ES_CFEMINORVER
Write "Build Integration Testing of EVS"
Write "Checksum of this version is ====>  "& SCX_CPU1_ES_CKSUM
Write " "
Write "The OS Build Label is ========> " & SCX_CPU1_ES_OSMAJORVER & "." & SCX_CPU1_ES_OSMINORVER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Setup for test
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;  In build 3, Event Messages of type DEBUG are disabled by default.  Need to enable 
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   wait 3
   Write "Sening a command to Enable Event Messages of type DEBUG"
   /SCX_CPU1_EVS_ENAEVENTTYPE DEBUG

   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))
   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify No-Op command (Command Code 1)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   Write "*********  Verifying the No-op command*********"
   Write " Sending the no-op command "

success_cmd_ctr = SCX_CPU1_EVS_CMDPC
error_cmd_ctr = SCX_CPU1_EVS_CMDEC

   /SCX_CPU1_EVS_NOOP

   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))

   Write "No-Op Counter command SUCCESSFUL"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Reset Counters Command (Command Code 2)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   Write " "
   Write "************* Verifying the Reset Counter Command************"
   Write "Sending a command with an error so that the error counter increments"
   ;  Note that this is an EVS command with an invalid function code = hex 'aa' (42 decimal)
   /raw 1801c0000001aa

   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr + 1))

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   
   Write "Sending the Reset Counters command"
   /SCX_CPU1_EVS_RESETCTRS
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = 0) AND (SCX_CPU1_EVS_CMDEC = 0))

   Write "Reset Counter command SUCCESSFUL"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Disabling of an Event Type (Command Code 4)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   event_packet_ctr = p008scnt
   
   Write "  "
   Write "*************** Verifying the Disabling of Event Type command *********"
   Write "Sending the command to disable an Event Type Command"  
   Write "For this test, we are disabling the INFO messages"

   Write "First verify getting TIME and ES events"
   write "Event_packet_ctr is currently " &  event_packet_ctr
   write "p008scnt is currently " & p008scnt
   /SCX_CPU1_ES_NOOP
   /SCX_CPU1_TIME_NOOP
   /raw 1805c0000001aa  ; This is an invalid function code

   Write "Verify received the events"
   wait until (p008scnt = (event_packet_ctr + 3))
   
   Write "Now disable all INFO messages"
   event_packet_ctr = p008scnt

   /SCX_CPU1_EVS_DISEVENTTYPE INFO
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))
   
   event_packet_ctr = p008scnt
   Write "Now sending those No-Ops again.  Should only get the ERROR event message"
   wait 3
   /SCX_CPU1_ES_NOOP
   /SCX_CPU1_TIME_NOOP
   /raw 1805c0000001aa  ; This is an invalid function code

   wait 2
   Write "Verify did not received the INFO events"

   wait until (p008scnt = event_packet_ctr +1)
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Enabling of an Event Type (Command Code 3)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC

   Write " "
   Write "*********** Verifying the Enabling of an event type *******"
   Write "Now enabling all INFO messages"
   event_packet_ctr = p008scnt

   /SCX_CPU1_EVS_ENAEVENTTYPE INFO
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))

   Write "Now sending those No-Ops again.  Should get them"
   event_packet_ctr = p008scnt
   wait 3
   /SCX_CPU1_ES_NOOP
   /SCX_CPU1_TIME_NOOP
   /raw 1805c0000001aa  ; This is an invalid function code

   wait 2
   Write "Verify did not received the events"
   wait until (p008scnt = (event_packet_ctr + 3))

   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify Setting of the Event Format Mode - Long/Short
;   (Command Code 5)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
  
   Write " "
   Write " ************ Verifying the Setting of the Event Format Mode *********"
   Write " First set the mode to short "

   /SCx_CPU1_EVS_SetEvtFmt Short
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = (success_cmd_ctr + 1)) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))
   if (p@SCX_CPU1_EVS_MSGFMTMODE <> "SHORT") then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
   endif 

   Write " Now set the mode to Long "

   /SCx_CPU1_EVS_SetEvtFmt Long
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = (success_cmd_ctr + 2)) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))
   if (p@SCX_CPU1_EVS_MSGFMTMODE <> "LONG") then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
   endif
   
   write "Verified the Setting of the Event Format Mode"
   write " " 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Disabling of an Event Type for an Application
;  (Command Code 7)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC

 
   Write " "
   Write " ************ Verifying the Disabling of an Event Type for an Application  *********"
   Write " Disable the Info messages to the CFE_TIME "


   /SCx_CPU1_EVS_DisAppEvtType Application = "CFE_TIME" INFO
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))  
   Write "Sending a TIME No-op, an invalid TIME command  and an ES No-op.  Should only see the invalid TIME and ES no-op"
   event_packet_ctr = p008scnt
   wait 2
   /SCX_CPU1_TIME_NOOP
   /raw 1805c0000001aa  ; This is an invalid function code
   /SCX_CPU1_ES_NOOP

   wait 2

   wait until (p008scnt = (event_packet_ctr + 2))
   
   write "Need to verify that received the ES event "
   write "????"
   Write "Verified The Disable an Event Type for an Applcation Command"      


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enabling of an Event Type for an Application
;  (Command Code 6)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC

 
   Write " "
   Write " ************ Verifying the Enabling of an Event Type for an Application  *********"
   Write " Enable the Info messages to the CFE_TIME "

   /SCx_CPU1_EVS_EnaAppEvtType Application = "CFE_TIME" INFO
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))  
   Write "Sending a Time No-op and an ES No-op.  Should see the Time and ES no-op"
   event_packet_ctr = p008scnt
   wait 2
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_ES_NOOP

   wait 2
   wait until (p008scnt = (event_packet_ctr + 2))

   write "Need to verify that received the ES event "
   write "????"
   
   Write "Verify The Enable Event Type for an Applcation Command"   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Disabling of All Events for an Application
;   (Command Code 9)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   event_packet_ctr = p008scnt

   Write " "
   Write " ************ Verifying the Disabling of All Events for an Application  *********"
   write "Verifying the receipt of TIME INFO and ERROR event messages as well as ES event message"
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_ES_NOOP
   /raw 1805c0000001aa     ; Invalid Time command generates an event message of type ERROR
   wait 2
   wait until (p008scnt = event_packet_ctr + 3)
   write "Need to verify that received the events "
   write "Visually for now ?????"

   Write "Now disable the TIME event messages - all"


   /SCX_CPU1_EVS_DISAPPEVGEN Application = "CFE_TIME"
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))  

   Write "Now send the same 3 event messages and verify that we only get the ES event message"
   event_packet_ctr = p008scnt
   wait 2
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_ES_NOOP
   /raw 1805c0000001aa     ; Invalid Time command generates an event message of type ERROR
   wait until (p008scnt = event_packet_ctr + 1)       
    if (p@SCX_CPU1_EVS_APP[4].APPENASTAT <> "DIS") then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
   endif   

   Write "Verified the Ability to Disable All Events for an Application"   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enabling of All Events for an Application
;   (Command Code 8)
;   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC


   Write " "
   Write " ************ Verifying the Enabling of All Events for an Application  *********"

   Write "Enable the TIME event messages - all"
   event_packet_ctr = p008scnt
   /SCX_CPU1_EVS_ENAAPPEVGEN Application = "CFE_TIME"
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))  
   Write "Now send the same 3 event messages and verify that we get ALL 3"
   event_packet_ctr = p008scnt
   wait 2 
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_ES_NOOP
   /raw 1805c0000001aa     ; Invalid Time command generates an event message of type ERROR

   wait until (p008scnt = (event_packet_ctr + 3))
   
   if (p@SCX_CPU1_EVS_APP[4].APPENASTAT <> "ENA") then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
   endif 
  
   Write "Verified the Ability to Enable All Events for an application"   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Resetting of an Application's Event Counter
;   (Command Code 10)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   
   Write " "
   Write "Sending a few Time NoOps to make sure that the event message counter is not zero"
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP

   wait until (SCX_CPU1_EVS_APP[4].APPMSGSENTC <> 0)
   Write "Now send the command to reset the App Event Counter"

   /SCX_CPU1_EVS_RstAppCtrs Application = "CFE_TIME"
   wait until (SCX_CPU1_EVS_APP[4].APPMSGSENTC = 0)

   Write "Verified the Application Event Reset Command"   


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Add an Event for Filtering (B3 capability)
;   (Command Code 16)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   local isfiltered
   isfiltered = FALSE
   event_packet_ctr = p008scnt
   PAGE SCX_CPU1_EVS_APP4_DATA

   write "Make sure that TIME NOOP command generates an event message"
   /SCX_CPU1_TIME_NOOP
   wait until (p008scnt = event_packet_ctr + 1)
   s get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "scx_cpu1_cfe_evs_app.dat", "1")   
   ;  Check to make sure that Event ID 4 is not registered for filtering (Assumes AppData 4 which should be TIME)
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   for i = 1 to 8 do
      if (SCX_CPU1_EVS_AppData[4].BinFltr[i].EvtID = 4 ) then
         isfiltered = TRUE
      endif
   enddo
   if (isfiltered = TRUE) then
      write "Time Event ID 4 is already registered for filtering so need to delete it"
      /SCX_CPU1_EVS_DELEVTFLTR APPLICATION = "CFE_TIME" EVENT_ID = 4
      WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))  
   
      ;  Check AGAIN to make sure that Event ID 4 is not registered for filtering
      for i = 1 to 8 do
         if (SCX_CPU1_EVS_AppData[4].BinFltr[i].EvtID = 4 ) then
            isfiltered = TRUE
         endif
      enddo

      if (isfiltered = TRUE) then
         write "Time Event ID 4 is already registered for filtering so I give up"
         wait 1000
         goto procerror
      endif
   endif

   write "Now sending the command to add Event ID 4 for filtering"
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC    
   wait 3
   /SCX_CPU1_EVS_ADDEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=x'4' EVENT_MASK= x'FFFE'
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr)) 

   s get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "scx_cpu1_cfe_evs_app.dat", "1")
   write "Verify that the TIME event ID 4 is being filtered"
   write "Should only get 2 event messages based on the filter"
   event_packet_ctr = p008scnt
   wait 2

   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   wait 5

   if (p008scnt <> (event_packet_ctr + 2)) then
      write "Add Event Filter did not work"
      wait 1000
      goto procterm 
   endif

   Write "Verify that the EVS Application Data File indicates the addition of the filter"

   for i = 1 to 8 do
         if (SCX_CPU1_EVS_AppData[4].BinFltr[i].EvtID = 4 ) then
            isfiltered = TRUE
         endif
   enddo

   if (isfiltered <> TRUE) then
         write "Time Event ID 4 did not get added for filtering"
         wait 1000
         goto procerror
   endif
   Write "Verified the Add Event Filter Command"   

   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Ability to Delete an Event for Filtering (B3 capability)
;   (Command Code 17)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   local isfiltered
   isfiltered = FALSE
   PAGE SCX_CPU1_EVS_APP4_DATA
   s get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "scx_cpu1_cfe_evs_app.dat", "1")   
   ;  Check to make sure that Event ID 4 is not registered for filtering (Assumes AppData 4 which should be TIME)
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   wait 3
   write "Sending the command to Delete an Event Filter (CFE_TIME Event ID 4)"
   /SCX_CPU1_EVS_DELEVTFLTR APPLICATION = "CFE_TIME" EVENT_ID = 4

   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))  

   s get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "scx_cpu1_cfe_evs_app.dat", "1")
   wait 5
   
   ;  Check to make sure that Event ID 4 is not registered for filtering
   for i = 1 to 8 do
      if (SCX_CPU1_EVS_AppData[4].BinFltr[i].EvtID = 4 ) then
         isfiltered = TRUE
      endif
   enddo

   if (isfiltered = TRUE) then
      write "EVS Delete Filter Command failed"
      wait 1000
      goto procerror
   endif


   write "Verify that the TIME event ID 4 is NOT registered for filtering"  

   event_packet_ctr = p008scnt
   wait 2
   write "Now sending 4 TIME NOOP commands.  Should get all 4 event messages"
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   wait 5
   if (p008scnt <> event_packet_ctr + 4) then
     write "EVS Delete Filter Command failed.  Event message being filtered"
     wait 1000
     goto procerror
   endif

   write "Verified the Delete Filter Command"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Setting of a Filter Mask
;   (Command Code 11)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC


   Write " "
   Write " ************ Verifying the Setting of a Filter Mask  *********"
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC    
   wait 3
   Write "First must register the filter"
   /SCX_CPU1_EVS_ADDEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=x'4' EVENT_MASK= x'FFFE'
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr)) 

   write "Sending the command to filter all TIME Messages of ID= 4"
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   wait 2
   /SCX_CPU1_EVS_SETBINFLTRMASK APPLICATION="CFE_TIME" EVENT_ID= 4 FILTERMASK=X'ffff'

   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))      
   Write "Now send 2 Time No-Op command and verify did not get an event message."
   Write "Note that the FFFF mask allows 1 event message"
   event_packet_ctr = p008scnt
   wait 2
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP

   wait 2
   if (p008scnt <> event_packet_ctr + 1) then
      proc_error_cnt = proc_error_cnt + 1
      goto procerror
   endif

   write "New setting the filter so that we get them again"

   /SCX_CPU1_EVS_SETBINFLTRMASK APPLICATION="CFE_TIME" EVENT_ID= 4 FILTERMASK=X'0000'
   wait 3
   event_packet_ctr = p008scnt
   wait 3
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   wait 3
   if (p008scnt <> event_packet_ctr + 2) then
      proc_error_cnt = proc_error_cnt + 1
      goto procerror
   endif
   
   Write "Verified the Set Filter Mask Command"  
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enabling of Output Ports
;   (Command Code 12)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC

   Write " "
   Write " ************ Verifying the Enabling of an Output Port (Port 4)*********"
   write "The current output port bit mask is " & SCX_CPU1_EVS_OUTPUTPORT

   output_port_mask = b'1000'
   output_port_temp = SCX_CPU1_EVS_OUTPUTPORT
   output_port = %AND(output_port_temp, output_port_mask)
   write "The output port mask is " & output_port
   wait 3
   if (output_port = 8 ) then
      /SCX_CPU1_EVS_DISPORT PORT_FOUR  ; Disable the port first
       output_port = %AND(SCX_CPU1_EVS_OUTPUTPORT, b'0000')
        if (output_port <> 0) then
          proc_error_cnt = proc_error_cnt + 1
          goto procerror
       endif
   endif

   write "Sending the command to Enable Output Port 4"
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   /SCX_CPU1_EVS_ENAPORT PORT_FOUR
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))      
   ;  Verify that telemety indicates the fourth bit was set
   wait 3
   output_port_mask = b'1000'
   output_port_temp = SCX_CPU1_EVS_OUTPUTPORT
   output_port = %AND(output_port_temp, output_port_mask)
   write "The output port mask is " & output_port
   wait 3
   if (output_port <> 8) then
          proc_error_cnt = proc_error_cnt + 1
          goto procerror
   endif
   Write "Verified that the Enabling of an output port works!" 
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Disabling of Output Ports
;   (Command Code 13)
;  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   output_port = SCX_CPU1_EVS_OUTPUTPORT

  
   Write " "
   Write " ************ Verifying the Disabling of an Output Port *********"
   write "The current output port bit mask is " & SCX_CPU1_EVS_OUTPUTPORT
   write "Sending the command to Disable Output Port 4"

   /SCX_CPU1_EVS_DISPORT PORT_FOUR  ; Disable the port first
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))      
       output_port = %AND(SCX_CPU1_EVS_OUTPUTPORT, b'0000')
       if (output_port <> 0) then
          proc_error_cnt = proc_error_cnt + 1
          goto procerror
       endif
   Write "Verified that the Disabling of an output port works!" 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Resetting An Application's filter
;   (Command Code 14)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC

   time_cmd_ctr = SCX_CPU1_TIME_CMDPC

   Write " "
   Write " ************ Verifying the resetting of a filter for an APP  *********"
   Write "First reset the event message counter for time"

   Write "Reset the event counter to start fresh"
   /SCX_CPU1_EVS_RSTAPPCTRS APPLICATION="CFE_TIME"
   wait until (SCX_CPU1_EVS_APP[4].APPMSGSENTC = 0)
   /SCX_CPU1_EVS_RSTBINFLTRCTR APPLICATION="CFE_TIME" EVENT_ID=X'4'
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 2) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   Write "Set a filter such that Time NoOp events will get filtered"
   /SCX_CPU1_EVS_SETBINFLTRMASK APPLICATION="CFE_TIME" EVENT_ID= 4 FILTERMASK=X'8' 
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr)) 

   Write "Now send Time Noops and verify only got 8 and then no more"
   event_packet_ctr = p008scnt
   wait 2
   for i = 1 to 9 do
      /SCX_CPU1_TIME_NOOP
     wait until (SCX_CPU1_TIME_CMDPC = time_cmd_ctr + i)
   enddo

   if (p008scnt <> event_packet_ctr + 8) then
     write p008scnt
     write event_packet_ctr
     proc_error_cnt = proc_error_cnt + 1
     wait
   endif
 
   Write "Now Reseting the filter"
   /SCX_CPU1_EVS_RSTBINFLTRCTR APPLICATION="CFE_TIME" EVENT_ID=X'4'
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 2) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
   event_packet_ctr = p008scnt
   Write "Verify that we are now getting the event messages again"
   time_cmd_ctr = SCX_CPU1_TIME_CMDPC
   /SCX_CPU1_TIME_NOOP
     wait until (SCX_CPU1_TIME_CMDPC = time_cmd_ctr + 1)
   if (p008scnt <> event_packet_ctr + 1) then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
   endif   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Resetting of all filters for the specified app
;   (Command Code 15)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;     
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC

   time_cmd_ctr = SCX_CPU1_TIME_CMDPC


   Write " "
   Write " ************ Verifying the resetting ALL filters for an APP  *********"
   Write "First set a filter such that Time NoOp events will get filtered"

   Write "Reset the event counter to start fresh"
   /SCX_CPU1_EVS_RSTAPPCTRS APPLICATION="CFE_TIME"
   wait until (SCX_CPU1_EVS_APP[4].APPMSGSENTC = 0)
 
   Write "Resetting the Filter"
   /SCX_CPU1_EVS_RSTBINFLTRCTR APPLICATION="CFE_TIME" EVENT_ID= 4

   Write "Setting the Mask for Time Event 4"
   /SCX_CPU1_EVS_SETBINFLTRMASK APPLICATION="CFE_TIME" EVENT_ID= 4 FILTERMASK=X'8' 
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 3) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    

   Write "Now send Time Noops and verify only got 8"
   event_packet_ctr = p008scnt
   wait 2
   for i = 1 to 9 do
      /SCX_CPU1_TIME_NOOP
     wait until (SCX_CPU1_TIME_CMDPC = time_cmd_ctr + i)
   enddo

   if (p008scnt <> event_packet_ctr + 8) then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
   endif 

   Write "Then setup a filter such that Another Time Event is filtered"
   Write "First need to register the Event for filtering"

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC    
   wait 3
   /SCX_CPU1_EVS_ADDEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=10 EVENT_MASK= x'FFFE'
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr)) 
   wait 3
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC    
   wait 3 
   /SCX_CPU1_EVS_SETBINFLTRMASK APPLICATION="CFE_TIME" EVENT_ID=10 FILTERMASK=X'4' 
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr)) 
   event_packet_ctr = p008scnt
   time_cmd_ctr = SCX_CPU1_TIME_CMDPC
   Write "Now send the Time command which generates Event_ID 10"

   /SCX_CPU1_TIME_SETCLOCKSTCF SECONDS=X'ffff' MICROSECS=X'3000'
   wait 2 
   /SCX_CPU1_TIME_SETCLOCKSTCF SECONDS=X'ffff' MICROSECS=X'3000' 
   WAIT UNTIL (SCX_CPU1_TIME_CMDPC = time_cmd_ctr + 2)
   WAIT UNTIL (p008scnt = event_packet_ctr + 2) 
   Write "Received the Time Events on the ground "
   
   Write "Now send it again and verify that we don't get it"
   event_packet_ctr = p008scnt
   /SCX_CPU1_TIME_SETCLOCKSTCF SECONDS=X'1000' MICROSECS=X'2000'  
   WAIT UNTIL (SCX_CPU1_TIME_CMDPC = time_cmd_ctr + 2)
   if (p008scnt <> event_packet_ctr) then
      proc_error_cnt = proc_error_cnt + 1
      goto procerror
   endif

   Write "Now send the command to reset all filters for the application"

   /SCX_CPU1_EVS_RSTALLFLTRS APPLICATION="CFE_TIME" 
   wait 5
  
   Write "Send commands and verify that we are getting events messages again"
   event_packet_ctr = p008scnt
   time_cmd_ctr = SCX_CPU1_TIME_CMDPC
   wait 3
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_SETCLOCKSTCF SECONDS=X'ffff' MICROSECS=X'3000'  
   WAIT UNTIL (SCX_CPU1_TIME_CMDPC = time_cmd_ctr + 2)
    if (p008scnt <> event_packet_ctr + 2) then
      proc_error_cnt = proc_error_cnt + 1
      goto procerror
   endif
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the writing of the EVS Log
;   (Command Code 19)
;  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   file_time = FILE_LIST[3840].file_write_time

   Write " "
   Write " ************ Verifying the ability to write and dump the EVS Log File Using default filename *********"
   page scx_cpu1_evs_log
   wait 2

   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log1","1")
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
   write "Verify that the time inside the file is different"

   WAIT UNTIL (file_time <>FILE_LIST[3840].file_write_time  )

   wait 5
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   file_time = FILE_LIST[3840].file_write_time

   Write " "
   Write " ************ Verifying the ability to write and dump the EVS Log File using specified filename *********"
   wait 2

   start get_file_to_cvt ("RAM:0", "cfe_evs_mylog.log", "cfe_evs.it_log2","CPU1", "EVS_LOG")
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
   write "Verify that the time inside the file is different"

   WAIT UNTIL (file_time <>FILE_LIST[3840].file_write_time  )



   write "Actual table data needs manual verification"
   
   wait 30

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the writing of the App file data for all registered 
;  applications
;   (Command Code 18)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   file_time = FILE_LIST[3841].file_write_time


   Write " "
   Write " ************ Verifying the ability to write and dump the EVS APP Data File  *********"
   page scx_cpu1_evs_app1_data
   wait 2

   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "cfe_evs_app.it_data1","1") 
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
   write "Verify that the time inside the file is different"

   WAIT UNTIL (file_time <> FILE_LIST[3841].file_write_time)
   write "Actual table data needs manual verification"
  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the setting of the Log Mode
;   (Command Code 20)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  success_cmd_ctr = SCX_CPU1_EVS_CMDPC
  error_cmd_ctr = SCX_CPU1_EVS_CMDEC

   Write " "
   Write " ************ Verifying the ability to setting of the Log Mode (Overwrite/discard)  *********"
 
  /SCX_CPU1_EVS_SETLOGMODE DISCARD
  WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
  wait until (p@SCX_CPU1_EVS_LOGMODE = "DISCARD")

  /SCX_CPU1_EVS_SETLOGMODE OVERWRITE
  WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
  wait until (p@SCX_CPU1_EVS_LOGMODE = "OVERWRITE")

  Write "Verified the Setting of the Log Mode"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the clearing of the Log Mode
;   (Command Code 21)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  success_cmd_ctr = SCX_CPU1_EVS_CMDPC
  error_cmd_ctr = SCX_CPU1_EVS_CMDEC

  Write " "
  Write " ************ Verifying the ability to ability to clear the local event log  *********"

  Write "First we need to fill up the local event log"
  Write "Send SB Noops"
     for i = 1 to 25 do
       /SCX_CPU1_SB_NOOP
       wait 2
     enddo
  if (p@SCX_CPU1_EVS_LOGFULL <> "TRUE") then
     proc_error_cnt = proc_error_cnt + 1
     goto procerror
  endif
  Write "Verified that the local log is full"
  Write "Now sending the command to clear it"
  /SCX_CPU1_EVS_CLRLOG
  
  WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
  WAIT UNTIL (p@SCX_CPU1_EVS_LOGFULL = "FALSE")
  WAIT UNTIL (SCX_CPU1_EVS_LOGOVERFLOWC = 0)
  Write "Now dump the local event log"
  start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log2","1")
  wait 2
  for i = 1 to 20 do
     if (SCX_CPU1_EVS_LOG[1].EVTLOGENTRY.AppName <> "") then
        proc_error_cnt = proc_error_cnt + 1
     endif
  enddo
  if (proc_error_cnt <> 0) then
     write "ERROR:  Local Event log did not get cleared"
     goto procerror
  endif
  write "Verified the Clear Log Command"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify that you can dump the Event Log after dumping the
;   Application Data File - problem in build 1.0
; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   Write "Disable the DEBUG messages so that you don't get the event messages"
   Write "  when you write the log"

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   wait 3
   Write "Sening a command to DISABLE Event Messages of type DEBUG and clear the log"
   /SCX_CPU1_EVS_DISEVENTTYPE DEBUG
   /SCX_CPU1_EVS_CLRLOG

   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 2) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))

   wait 3
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   file_time = FILE_LIST[3840].file_write_time

   Write " "
   Write " ************ Verifying the dump the EVS APP Data File then EVS log *********"
   page scx_cpu1_evs_app1_data
   wait 2

   start get_file_to_cvt ("RAM:0", "cfe_evs_app.dat", "cfe_evs_app.it_data2","1") 
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    

   WAIT UNTIL (file_time <> FILE_LIST[3841].file_write_time)

   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   file_time = FILE_LIST[3840].file_write_time


   page scx_cpu1_evs_log
   wait 2

   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log3","1")
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    
   write "Verify that the time inside the file is different"

   WAIT UNTIL (file_time <> FILE_LIST[3840].file_write_time)

   ; Generate a few noops
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   /SCX_CPU1_TIME_NOOP
   wait 5
   success_cmd_ctr = SCX_CPU1_EVS_CMDPC
   error_cmd_ctr = SCX_CPU1_EVS_CMDEC
   file_time = FILE_LIST[3840].file_write_time

   start get_file_to_cvt ("RAM:0", "cfe_evs.log", "cfe_evs.it_log4","1")
   WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))    


   WAIT UNTIL (SCX_CPU1_EVS_LOG[1].EVTLOGENTRY.AppName = "CFE_TIME")
   WAIT UNTIL (SCX_CPU1_EVS_LOG[1].EVTLOGENTRY.EVTID = 4)
   WAIT UNTIL (SCX_CPU1_EVS_LOG[2].EVTLOGENTRY.AppName = "CFE_TIME")
   WAIT UNTIL (SCX_CPU1_EVS_LOG[2].EVTLOGENTRY.EVTID = 4)


goto procterm

procerror:
     Write "ERROR:  There were problems in this procedure"
     wait

procterm:
     Write "Test complete"
     ; Now clean up
    success_cmd_ctr = SCX_CPU1_EVS_CMDPC
    error_cmd_ctr = SCX_CPU1_EVS_CMDEC
    wait 5
    /SCX_CPU1_EVS_DELEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=4
    /SCX_CPU1_EVS_DELEVTFLTR APPLICATION="CFE_TIME" EVENT_ID=10

    /SCX_CPU1_EVS_RstAppCtrs Application = "CFE_TIME"
    /SCX_CPU1_EVS_ENAAPPEVGEN Application = "CFE_TIME"
    /SCX_CPU1_EVS_RSTALLFLTRS APPLICATION="CFE_TIME" 

    WAIT UNTIL ((SCX_CPU1_EVS_CMDPC = success_cmd_ctr + 5) AND (SCX_CPU1_EVS_CMDEC = error_cmd_ctr))
     newlog 
endproc
