proc cfe_b4p0_sb_integration_cpu1
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform Build Integration Testing of the cFE Software Bus.  
;
;
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
local file_time

;LIMIT ON CDH_ROUTE.NSBSTREAM
;LIMIT ON ACE_ROUTE.NSBSTREAM
;LIMIT ON PSE_ROUTE.NSBSTREAM

;LIMIT ON P019
;LIMIT ON P039
;LIMIT ON P059


page scx_cpu1_sb_hk

newlog

Write "The cFE Build Label is =======> " & SCX_CPU1_ES_CFEMAJORVER & "." & SCX_CPU1_ES_CFEMINORVER
Write "Build Integration Testing of EVS"
Write "Checksum of this version is ====>  "& SCX_CPU1_ES_CKSUM
Write " "
Write "The OS Build Label is ========> " & SCX_CPU1_ES_OSMAJORVER & "." & SCX_CPU1_ES_OSMINORVER

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify No-Op command
;  Command Code 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   Write "Sending the no-op command"
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC

   /SCX_CPU1_SB_NOOP

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))

   Write "No-Op Counter command SUCCESSFUL"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Reset Counters Command
;  Command Code 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   Write " "
   Write "Sending a command with an error so that the error counter increments"
   ;  Note that this is an SB command with an invalid function code = hex 'aa' (42 decimal)
   /raw 1803c0000001aa

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr + 1))

   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC

   Write "Sending the Reset Counters command"

   /SCX_CPU1_SB_RESETCTRS
   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = 0) AND (SCX_CPU1_SB_CMDEC = 0))

   Write "Reset Counter command SUCCESSFUL"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Dump Statistics Command
;  Command Code 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   packet_sequence_cnt = p04ascnt

   Write "Verifying the Dump Statistic Command"
   Write "Sending the command the Dump the sB Statistics Command"
   /SCX_CPU1_SB_DUMPSTATS
   WAIT UNTIL (p00ascnt <> packet_sequence_cnt) 
   
   Write "SB Statistics Packet was received"
   Write "Need to add MORE verification (other than pkt scnt)"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Save Routing Info Command
;  Command Code 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   page scx_cpu1_sb_hk
   page scx_cpu1_sb_routing
   wait 5
   file_time = FILE_LIST[3842].file_write_time
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   wait 3
   Write "Sending the Save Route Command Using default filename"  
   Write "Calling procedure to save and dump the route"
   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "cfe_sb_route.it_dat1","CPU1")

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[3842].file_write_time )

   wait 5
   file_time = FILE_LIST[3842].file_write_time
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   wait 3
   Write "Now try sending writing the file and specify the filename that you want to use"
   start get_file_to_cvt("RAM:0", "cfe_sb_myroute.dat", "cfe_sb_route.it_dat2","CPU1","SB_ROUTE")
   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[3842].file_write_time )

   Write "Need to verify SB Route table manually"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Verify the Enable and Disable Route Command
;  Command Code 4
;  Command Code 5
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC

   Write "Sending the Enable and Disable Route Command" 
   ;  Send a command to disable the route for the Time Diagnostic Packet to TO
   Write "Disabling an arbitrary message to an arbitrary pipe but hoping that it is the Time Diagnostic packet to TO"
   Write "First creating the sb route file and dumping to cvt"
   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "sb_route.it_dat2", "CPU1")
   
   pipe_id = SCX_CPU1_SB_RouteEntry[1].SB_PIPEID  
   message_id = SCX_CPU1_SB_RouteEntry[7].SB_MSGID
   Write "SETUP:"
   write "Message " & message_id & " is being disable from pipe " &pipe_id
   write "Current state of this message to this pipe BEFORE disabling is " & p@SCX_CPU1_SB_RouteEntry[7].SB_STATE 
   if (p@SCX_CPU1_SB_RouteEntry[7].SB_STATE <> "ENA") then
       write "ERROR!  This message should be disabled!"
       goto procerror
   endif

   Write "Sending the command to TIME to request the diagnostic packet to make sure that we can get it."
   packet_sequence_cnt = p006scnt
   wait 2

   /SCX_CPU1_TIME_REQUESTDIAG
   WAIT UNTIL (p006scnt = packet_sequence_cnt + 1)
   Write "Verified that we received the Time diagnostic packet"
   Write " "

   Write "END SETUP:  Now testing Disable SB Route Command"
   write "Now disabling the message from the pipe"
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
 
   wait 2
   /SCX_CPU1_SB_DISROUTE MID=message_id  PID = pipe_id

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))

   Write "Dumping the sbroute.dat file again"
   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "sb_route.it_dat3","CPU1")
   ; Verify that the routing table shows that it is disabled
   wait 3
   write "Current state of this message to this pipe AFTER disabling is " & p@SCX_CPU1_SB_RouteEntry[7].SB_STATE 
   if (p@SCX_CPU1_SB_RouteEntry[7].SB_STATE <> "DIS") then
       write "ERROR!  This message should be disabled!"
       proc_error_cnt = proc_error_cnt + 1
       goto procerror
   endif  

   Write "Now send the command to TIME to request the diagnostic packet."
   packet_sequence_cnt = p006scnt
   
   /SCX_CPU1_TIME_REQUESTDIAG
  
   if (packet_sequence_cnt <> p006scnt) then
      write "ERROR!: Should not have received the packet"
      proc_error_cnt = proc_error_cnt + 1
      goto procerror
   endif

   write " "
   Write "Now Testing the Enable SB Route Command"
   
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   /SCX_CPU1_SB_ENAROUTE MID=message_id  PID = pipe_id

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))

   Write "Dumping the sbroute.dat file again"

   start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", "sb_route.it_dat4","CPU1")
   ; Verify that the routing table shows that it is enabled
   wait 3
   write "Current state of this message to this pipe AFTER enabling is " & p@SCX_CPU1_SB_RouteEntry[7].SB_STATE 
   if (p@SCX_CPU1_SB_RouteEntry[7].SB_STATE <> "ENA") then
       write "ERROR!  This message should be enabled!"
       proc_error_cnt = proc_error_cnt + 1
       goto procerror
   endif  

   Write "Now send the command to TIME to request the diagnostic packet."
   packet_sequence_cnt = p006scnt

   wait 2   
   /SCX_CPU1_TIME_REQUESTDIAG
  
   WAIT UNTIL (packet_sequence_cnt <> p006scnt)

   Write "Verified that received that packet on the ground"
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Dump Network command
;  Command Code 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ; build 2
   write "This capability is not available in Build 3.0"
   write " "



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Save Pipe Info Command
;  Command Code 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   page scx_cpu1_sb_hk
   page scx_cpu1_sb_pipe
   wait 3
   file_time = FILE_LIST[3843].file_write_time
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   wait 3
   Write "Sending the Save Pipe Command Using Default filename"  
   Write "Calling procedure to save and dump the route"
   start get_file_to_cvt("RAM:0", "cfe_sb_pipe.dat", "cfe_sb_pipe.it_dat1","CPU1")

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[3843].file_write_time )

   file_time = FILE_LIST[3843].file_write_time
   wait 3
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   Write "Now try sending writing the file and specify the filename that you want to use"
   start get_file_to_cvt("RAM:0", "cfe_sb_mypipe.dat", "cfe_sb_pipe.it_dat2","CPU1","SB_PIPE")

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[3843].file_write_time )
  
   Write "Need to verify SB Pipe table manually"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Verify Save Map Info Command
;  Command Code 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   page scx_cpu1_sb_hk
   page scx_cpu1_sb_msgmap
   wait 3
   file_time = FILE_LIST[3844].file_write_time
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   wait 2
   Write "Sending the Save Map Command using default filename"  
   Write "Calling procedure to save and dump the route"
   start get_file_to_cvt("RAM:0", "cfe_sb_msgmap.dat", "cfe_sb_map.it_dat1","CPU1")

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[3844].file_write_time )

   wait 3
   file_time = FILE_LIST[3844].file_write_time
   success_cmd_ctr = SCX_CPU1_SB_CMDPC
   error_cmd_ctr = SCX_CPU1_SB_CMDEC
   wait 2

   Write "Now try sending writing the file and specify the filename that you want to use"
   start get_file_to_cvt("RAM:0", "cfe_sb_mymap.dat", "cfe_sb_map.it_dat2","CPU1","SB_MSGMAP")

   WAIT UNTIL ((SCX_CPU1_SB_CMDPC = success_cmd_ctr + 1) AND (SCX_CPU1_SB_CMDEC = error_cmd_ctr))
   WAIT UNTIL (file_time <> FILE_LIST[3844].file_write_time )
 
   Write "Need to verify SB Map table manually"


 
goto procterm

procerror:
     Write "ERROR:  There were problems in this procedure"
     wait
procterm:
     Write "Test complete"
     newlog 
endproc
