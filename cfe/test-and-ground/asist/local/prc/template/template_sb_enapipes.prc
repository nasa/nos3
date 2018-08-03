PROC $sc_$cpu_sb_enapipes
;**********************************************************************
;  Test Name:  $sc_$cpu_sb_enapipes
;  Test Level: Build Verification
;  Test Type:  Functional
; 
;  Test Description
;	Pipe Overflow and MsgId-to-Pipe Limit errors are generated. A 
;	special test application written in C is used to enable/disable 
;	the processing of messages in the pipe so that the pipe can 
;	become full. Note that a copy of the test application is placed 
;	in each of the three CPUs, and that these test applications 
;	subscribe to the same commands. After processing a command sent 
;	by the ground control computer (ASIST or ITOS), each of the three 
;	test applications will send a telemetry packet. Thus, many 
;	telemetry packets are generated. A sequential print is used to 
;	capture these telemetry packets to a file. 
;	A huge SB message is sent to 63 destinations (pipes). That test 
;	is repeated using the zero-copy SB capability. A disable/enable 
;	SB routing is performed. The SB routing info file is saved and 
;	downlinked. SB statistics and SB network info packets are dumped 
;	to the ground computer. A special test application written in C 
;	is loaded into the flight software to perform these tests. The 
;	special test application also requests to send a message with a 
;	message length equal to one byte, which is an error condition.
;
;  Requirements Tested
;     cSB4000	Upon receipt of a NOOP command, the cFE shall increment the
;		command counter.
;     cSB4003	Upon receipt of a Command, the cFE shall save the SB routing
;		information to a file.
;     cSB4004	Upon receipt of a Command, the cFE shall enable or disable
;		routing the command specified message to the command specified
;		pipe. 
;     cSB4005	The cFE shall send an SB Message containing the following
;		housekeeping telemetry items:
;		(a) command counter
;		(c) no subscriptions counter
;		(d) message send error counter 
;     cSB4007	Upon receipt of a Command, the cFE shall save Pipe information
;		to the command specified file.
;
;     cSB4007.1 If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename.
;     cSB4300	The cFE shall provide a zero-copy message transfer mode for
;		intra-processor communication.
;     cSB4301	Upon receipt of a Request to create a Pipe, the cFE shall create
;		a Pipe with the Request-specified Pipe Depth and the 
;		Request-specified name.
;     cSB4302	Upon receipt of a Request to delete a Pipe, the cFE shall 
;		Unsubscribe all messages to the Request-specified Pipe, then
;		remove the Pipe from the Routing Information.
;     cSB4303	Upon receipt of a Request to Subscribe to an SB message, the cFE
;		shall establish a route using the Request-specified Message ID,
;		the Request-specified Pipe ID, the Request-specified 
;		MsgId-to-Pipe limit and the Request-specified QoS.
;     cSB4304	Upon receipt of a Request to Unsubscribe to an SB Message, the
;		cFE shall remove the route corresponding to the
;		Request-specified Message Id and the Request-specified Pipe Id
;		from the Routing Information.
;     cSB4305	Upon receipt of a Request to send an SB Message, the cFE shall
;		route the SB Message to the Pipes of all Applications that have
;		Subscribed to the SB Message.
;     cSB4305.1 The cFE shall limit the number of messages of a particular
;		Message ID that can be sent to an Application's Pipe. 
;     cSB4305.3 If routing a Message to an Application's Pipe results in a Pipe
;		Overflow, the cFE shall abort the send to that pipe, issue an
;		event, and continue sending to the remaining pipes.
;     cSB4305.4 If routing a Message to an Application's Pipe would exceed the
;		MsgId-to-Pipe Limit, the cFE shall abort the send to that pipe,
;		issue an event, and continue sending to the remaining pipes.
;     cSB4305.5 Upon receipt of a Request to send a Message, if no Applications
;		have Subscribed to the Message, the cFE shall increment the No
;		Subscriber's Counter and send an event that displays the Message
;		ID.
;     cSB4305.6 If the SB Message is greater than the <MISSION_DEFINED,32767>
;		bytes then the cFE shall not send the message, issue an event
;		message and increment the message send error counter.
;     cSB4306	Upon receipt of a Request to receive a SB Message from a Pipe
;		without a timeout, the cFE shall remove the next SB Message 
;		from the Pipe and provide the message to the Application. 
;     cSB4307	Upon receipt of a Request to receive a SB Message from a Pipe
;		with a pending timeout, the cFE shall suspend execution of the
;		Application until a SB Message is present on the Pipe or the
;		timeout has expired.
;     cSB4308	Upon receipt of a Request to receive a SB Message from a Pipe
;		with an infinite timeout, the cFE shall suspend execution of the
;		Application until a SB Message is present on the Pipe.
;		Note: The SB command input pipe FSW uses an infinite timeout.
;     cSB4309	Upon receipt of a Request, the cFE shall provide sender 
;		information for the last message received on an Application's
;		Pipe.
;     cSB4701	The cFE shall support a < PLATFORM_DEFINED > maximum system
;		packet size.
;     cSB4704	The cFE shall support a maximum of 16 Destinations for a Message
;		ID.
;     cSB4705	The cFE shall support a maximum of 64 pipes per processor.
;
;  Prerequisite Conditions
;	Open event log and telemetry archive files
;
;  Assumptions and Constraints
write ";  A copy of the special test application written in C resides on one or"
write ";  more CPUs. The test application creates a group of pipes used for "
write ";  this test. The ones relevent to this test are constructed as follows:"
write ";				Limit				Depth"
write ";		ID 1000		ID 1001		ID 1002		"
write ";  TstSBPipe1	1		N/A		30		1"
write ";  TstSBPipe2	4		2		N/A		5"
write ";  TstSBPipe3	10		N/A		N/A		32"
write ";  TstSBPipe4	N/A		N/A		257		256"
write ";  TstSBPipe5-18	N/A		N/A		10		256"
;
;  Change History
;
;	Date		   Name		Description
;	 6/ 3/2005  Ingrid Albright	Original Procedure
;       12/ 4/2006  Ingrid Albright	Updated from new scenario. Now a 
;					combination of SB Pipes and SB Loading.
;	 2/20/2007  Walt Moleski	Updated for cFE 4.0. Removed all 
;					requirements not tested by this proc.
;	 7/ 5/2007  Walt Moleski	Removed deleted rqmt 4305.2
;	02/08/2012  Walt Moleski	Added variable for ram disk and replaced
;					ut_setupevt with ut_setupevents
;
;  Arguments
;
;	Name			Description
;	None
;
;  Procedures Called
;
;	Name			Description
;  	print_sb_pipes		Prints the status of all the test app pipes.
; 
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_platform_cfg.h"
#include "cfe_sb_events.h"
#include "cfe_evs_events.h"
#include "tst_sb_events.h"
#include "to_lab_events.h"

%liv (log_procedure) = logging

;**********************************************************************
;  Define variables
;**********************************************************************
global ut_req_array_size
ut_req_array_size = 23
global ut_requirement[0..ut_req_array_size]

#define cSB4000   0
#define cSB4003   1
#define cSB4004a  2
#define cSB4004b  3
#define cSB4005a  4
#define cSB4005c  5
#define cSB4005d  6
#define cSB4007   7
#define cSB40071  8
#define cSB4300   9
#define cSB4301   10
#define cSB4302   11
#define cSB4303   12
#define cSB4304   13
#define cSB4305   14
#define cSB43055  15
#define cSB43056  16
#define cSB4306   17
#define cSB4307   18
#define cSB4308   19
#define cSB4309   20
#define cSB4701   21
#define cSB4704   22
#define cSB4705   23

%liv (log_procedure) = FALSE

for i=0 to ut_req_array_size do
   ut_requirement[i] = "U"
enddo

%liv (log_procedure) = logging

local requirements[0..ut_req_array_size] = ["cSB4000","cSB4003","cSB4004(a)","cSB4004(b)","cSB4005(a)","cSB4005(c)","cSB4005(d)","cSB4007","cSB4007.1","cSB4300","cSB4301","cSB4302","cSB4303","cSB4304","cSB4305","cSB4305.5","cSB4305.6","cSB4306","cSB4307","cSB4308","cSB4309","cSB4701","cSB4704","cSB4705"]

global ing_result

LOCAL errorcounter, errorcounter2, cmdCtr, timeoutcounter, stream, expectedresult[1..4], numberOfInitialPipes

local ramDir = "RAM:0"
local hostCPU = "$CPU"

write ";**********************************************************************"
write "; Step 1.0:  Initialize the CPU for this test."
write ";**********************************************************************"
write "; Step 1.1: Command a Power-On Reset on $CPU"
write ";*********************************************************************"
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
    Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    Write "<!> Failed - Event Message not received for ENAEVENTTYPE command."
  endif
else
  write "<!> Failed - Could not enable Debug events."
endif

;Determine the number of registered pipes before the test app is started.
numberOfInitialPipes=0
s get_file_to_cvt (ramDir, "cfe_sb_pipe.dat", "$sc_$cpu_cfe_sb_pipe_step-1_0.dat", hostCPU) 

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_PIPES do
   if ($SC_$CPU_PE[i].$SC_$CPU_SBPF_PIPENAME <> "") then
      numberOfInitialPipes = numberOfInitialPipes+1 
   endif
enddo

%liv (log_procedure) = logging

write "*** Number of pipes found before starting test app: ", numberOfInitialPipes, "."

write ";**********************************************************************"
write ";  Step 1.2:  Start the test application."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_MAX_PIPES_MET_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_SB, TST_SB_INIT_INF_EID, INFO, 2
s load_start_app ("TST_SB", hostCPU)

ut_tlmwait $sc_$CPU_find_event[1].num_found_messages 1 60

if ($sc_$CPU_find_event[1].num_found_messages >= 1) then
   ut_setrequirements cSB4705, "P"
   write "<*> Passed (4705) - Found pipe Max Pipe creation error."
else
   ut_setrequirements cSB4705, "F"
   write "<!> Failed (4705) - did not find pipe Max Pipe creation error."
endif

ut_tlmwait $sc_$CPU_find_event[2].num_found_messages 1 60
;;if (UT_TW_Status = UT_Success) then
if ($sc_$CPU_find_event[2].num_found_messages = 1) then
   write "<*> Passed - TST_SB App loaded."
else
   write "<!> Failed - TST_SB App failed to load."
   goto END
endif

wait 5

; Filter out the annoying requsting housekeeping messages we will receive from 
; the test app.
/$SC_$CPU_EVS_AddEvtFltr APPLICATION="TST_SB" EVENT_ID=TST_SB_HK_INF_EID EVENT_MASK=X'FFFF'
wait 5

write ";**********************************************************************"
write "; Step 1.3: Send a ""Add telemetry packet subscription to TO downlink"""
write ";           command. "
write ";"
write ";	  Result: The CFE_SB_ROUT_ADDED_EID event message was received."
write ";**********************************************************************"

;; CPU1 is the default
stream = x'900'

;;if ("$CPU" = "CPU2") then
;;   stream = x'A00'
;;elseif ("$CPU" = "CPU3") then
;;   stream = x'B00'
;;endif

start $sc_$CPU_print_sb_pipes("rout_1-2-0", 5)

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($sc_$CPU_SB_RouteEntry[i].SB_MSGID = stream) then
      ut_setrequirements cSB4003, "F"
      write "<!> Failed (4003) - part 0 - unexpected entry in routing table."
      i=CFE_SB_MAX_MSG_IDS+1
   endif
   if (i=CFE_SB_MAX_MSG_IDS) then
      ut_setrequirements cSB4003, "P"
      write "<*> Passed (4003) - part 0 - routing table as expected."
   endif
enddo
%liv (log_procedure) = logging

write ";  Add a subscription for the SB Test App housekeeping telemetry packet."

ut_setupevents $sc, $CPU, TO_LAB_APP, TO_ADDPKT_INF_EID, INFO, 1

/$sc_$CPU_TO_ADDPACKET Stream=stream Pkt_Size=x'0' Priority=x'0' Reliability=x'0' Buflimit=x'4'

ut_tlmwait $sc_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - part 1."
else
   write "<!> Failed - part 1 (Found ",$sc_$CPU_find_event[1].num_found_messages," messages)."
   goto END
endif

start $sc_$CPU_print_sb_pipes("rout_1-2-1", 5)

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($sc_$CPU_SB_RouteEntry[i].SB_MSGID = stream) then
      ut_setrequirements cSB4003, "P"
      write "<*> Passed (4003) - part 1.5 - routing table as expected."
      i=CFE_SB_MAX_MSG_IDS+1
   endif
   if (i=CFE_SB_MAX_MSG_IDS) then
      ut_setrequirements cSB4003, "F"
      write "<!> Failed (4003) - part 1.5 - missing entry in routing table."
   endif
enddo

%liv (log_procedure) = logging

write ";  Add a subscription for the SB Test App telemetry packet."

/$sc_$CPU_TO_ADDPACKET Stream=(stream+1) Pkt_Size=x'0' Priority=x'0' Reliability=x'0' Buflimit=x'20'
ut_tlmwait $sc_$CPU_find_event[1].num_found_messages 2 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - part 2."
else
   write "<!> Failed - part 2 (Found ",$sc_$CPU_find_event[1].num_found_messages," messages)."
   goto END
endif

start $sc_$CPU_print_sb_pipes("rout_1-3", 5)

write ";**********************************************************************"
write ";  Step 2.0: Enable Pipe Processing."
write ";**********************************************************************"

ut_setupevents $sc, $CPU, TST_SB, TST_SB_ENABLEPIPES_INF_EID, INFO, 1

/$SC_$CPU_TST_SB_EnablePipe

ut_tlmwait $sc_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - TST_SB Enable Pipe command."
else
   write "<!> Failed - TST_SB Enable Pipe command (Found ",$sc_$CPU_find_event[1].num_found_messages," messages)."
endif

write ";**********************************************************************"
write ";  Step 3.0: Send a SB NOOP Command."
write ";   Req : 4000, 4005a"
write ";**********************************************************************"
;;cmdCtr = $SC_$CPU_SB_CMDPC+1
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO, 1

/$SC_$CPU_SB_NOOP
wait 10

;;ut_tlmwait $SC_$CPU_SB_CMDPC {cmdCtr}
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4000;4005a) - NO-OP command sent properly."
  ut_setrequirements cSB4000, "P"
  ut_setrequirements cSB4005a, "P"
else
  write "<!> Failed (4000;4005a) - NO-OP command not sent properly (", ut_tw_status, ")."
  write "CmdCtr = ", $SC_$CPU_SB_CMDPC, " expected CMDPC = ", cmdCtr
  ut_setrequirements cSB4000, "F"
  ut_setrequirements cSB4005a, "F"
endif

write ";**********************************************************************"
write ";  Step 4.0: Disable and Enable Routing for Pipe 5."
write ";   Req : 4003, 4004"
write ";**********************************************************************"
write ";  Step 4.1: Disable routing for pipe 5."
write ";**********************************************************************"
local pipe5counter
local pipe5index

start $sc_$cpu_print_sb_pipes("rout-4_1", 5)

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe5") then
      pipe5index = i
   endif
enddo

%liv (log_procedure) = logging

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_DSBL_RTE2_EID, DEBUG, 1

/$SC_$CPU_SB_DisRoute MID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGID PID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_PIPEID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4004b) - Disable Pipe 5 Routing."
   ut_setrequirements cSB4004b, "P"
else
   write "<!> Failed (4004b) - Disable Pipe 5 Routing - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4004b, "F"
endif

write ";**********************************************************************"
write ";  Step 4.1.1: Send disable route commands with invalid msgid & pipeids."
write ";   Req : "
write ";**********************************************************************"

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_DSBL_RTE1_EID, ERROR, 1

/$SC_$CPU_SB_DisRoute MID=0 PID=0

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Error message as expected."
else
   write "<!> Failed - Invalid arguments did not generate expected error."
endif

/$SC_$CPU_SB_DisRoute MID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGID PID=1

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 2

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Error message as expected."
else
   write "<!> Failed - Invalid arguments did not generate expected error."
endif

/$SC_$CPU_SB_DisRoute MID=($SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGID+5) PID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_PIPEID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 3

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Error message as expected."
else
   write "<!> Failed - Invalid arguments did not generate expected errors."
endif

write ";**********************************************************************"
write "; Step 4.2: Check that pipe 5 is shown as disabled in the routing table."
write ";**********************************************************************"
pipe5counter = $SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGCNT

start $sc_$cpu_print_sb_pipes("rout-4_2", 5)

if (p@$SC_$CPU_SB_RouteEntry[pipe5index].SB_STATE = "DIS") then
   write "<*> Passed (4003;4004b) - Pipe 5 routing is disabled."
   ut_setrequirements cSB4004b, "P"
   ut_setrequirements cSB4003, "P"
else
   write "*** Step 4.2 failed."
   write "<!> Failed (4003;4004b) - Pipe 5 routing is not disabled as expected."
   ut_setrequirements cSB4004b, "F"
   ut_setrequirements cSB4003, "F"
endif

write ";**********************************************************************"
write ";  Step 4.3: Send a message that would normally be received by pipe 5"
write ";            and then check the pipe 5 message received counter."
write ";**********************************************************************"

cmdCtr = $SC_$CPU_TST_SB_MSGCTR+(CFE_SB_MAX_DEST_PER_PKT-1)
write "*** Expecting message counter to be $SC_$CPU_TST_SB_MSGCTR+(CFE_SB_MAX_DEST_PER_PKT-1) = ", $SC_$CPU_TST_SB_MSGCTR,"+(", CFE_SB_MAX_DEST_PER_PKT,"-1)."

/$SC_$CPU_TST_SB_SendMsg2
wait 5

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {cmdCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4004b) - TST_SB message counter as expected."
   ut_setrequirements cSB4004b, "P"
else
   write "<!> Failed (4004b) - TST_SB message counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_MSGCTR, "."
   ut_setrequirements cSB4004b, "F"
endif

start $sc_$cpu_print_sb_pipes("rout-4_3", 5)
if (pipe5counter = $SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGCNT) then
   write "<*> Passed (4004b) - Pipe 5 message counter stayed the same."
   ut_setrequirements cSB4004b, "P"
else
   write "<!> Failed (4004b) - Pipe 5 message counter incremented while it was disabled."
   ut_setrequirements cSB4004b, "F"
endif

write ";**********************************************************************"
write ";  Step 4.4: Enable routing for pipe 5."
write ";**********************************************************************"
start $sc_$cpu_print_sb_pipes("rout-4_4", 5)

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_ENBL_RTE2_EID, DEBUG, 1

/$SC_$CPU_SB_EnaRoute MID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGID PID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_PIPEID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Route command for Pipe 5."
  ;; Wait here until the routing table can get updated
  while (p@$SC_$CPU_SB_RouteEntry[pipe5index].SB_STATE = "DIS") do
    wait 10
    start $sc_$cpu_print_sb_pipes("while-4_4", 5)
  enddo
else 
   write "<!> Failed - Enable Route command - Expected event message was not received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
endif

start $sc_$cpu_print_sb_pipes("rout-4_4a", 5)

write ";**********************************************************************"
write ";  Step 4.4.1: Send enable route commands with invalid msgid & pipeids."
write ";   Req : "
write ";**********************************************************************"

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_ENBL_RTE1_EID, ERROR, 1

/$SC_$CPU_SB_EnaRoute MID=0 PID=0

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Expected Error message received."
else
   write "<!> Failed - Invalid arguments did not generate expected error."
endif

/$SC_$CPU_SB_EnaRoute MID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGID PID=1

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 2

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Expected Error message received."
else
   write "<!> Failed - Invalid arguments did not generate expected error."
endif

/$SC_$CPU_SB_EnaRoute MID=($SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGID+5) PID=$SC_$CPU_SB_RouteEntry[pipe5index].SB_PIPEID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 3

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Expected Error message received."
else
   write "<!> Failed - Invalid arguments did not generate expected error."
endif

write ";**********************************************************************"
write ";  Step 4.5: Check that pipe 5 is shown as enabled in the routing table."
write ";**********************************************************************"
start $sc_$cpu_print_sb_pipes("rout-4_5", 5)

if (p@$SC_$CPU_SB_RouteEntry[pipe5index].SB_STATE = "DIS") then
   write "<!> Failed (4003;4004a) - Pipe 5 is not enabled."
   ut_setrequirements cSB4004a, "F"
   ut_setrequirements cSB4003, "F"
elseif (p@$SC_$CPU_SB_RouteEntry[pipe5index].SB_STATE = "ENA") then
      write "<*> Passed (4003;4004a) - Pipe 5 enabled."
      ut_setrequirements cSB4004a, "P"
      ut_setrequirements cSB4003, "P"
endif

write ";**********************************************************************"
write ";  Step 4.6: Send a message that would normally be received by pipe 5."
write ";            and then check the pipe 5 message received counter."
write ";**********************************************************************"

cmdCtr = $SC_$CPU_TST_SB_MSGCTR+CFE_SB_MAX_DEST_PER_PKT

/$SC_$CPU_TST_SB_SendMsg2
wait 5

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {cmdCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4004a) - TST_SB message counter as expected."
   ut_setrequirements cSB4004a, "P"
else
   write "<!> Failed (4004a) - test app message counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_MSGCTR, "."
   ut_setrequirements cSB4004a, "F"
endif

wait 15

start $sc_$cpu_print_sb_pipes("rout-4_6", 5)
if (pipe5counter < $SC_$CPU_SB_RouteEntry[pipe5index].SB_MSGCNT) then
   write "<*> Passed (4003) - Pipe 5 message counter changed."
   ut_setrequirements cSB4003, "P"
else
   write "<!> Failed (4003) - Pipe 5 counter did not increment."
   ut_setrequirements cSB4003, "F"
endif

write ";**********************************************************************"
write ";  Step 5.0: Verify Pipe names."
write ";   Req : 4007,4301"
write ";   This also tests DCR 24."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_EID, DEBUG, 1

local numFound=0
s get_file_to_cvt (ramDir, "cfe_sb_pipe.dat", "$sc_$cpu_cfe_sb_pipe_step-5_0.dat", hostCPU) 
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (4007) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements cSB4007, "P"
else
  write "<!> Failed (4007) - Event message not received for Write SB Msg Map File command."
  ut_setrequirements cSB4007, "F"
endif

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_PIPES do
   for pipenum=0 to CFE_SB_MAX_PIPES do
      if ($SC_$CPU_PE[i].$SC_$CPU_SBPF_PIPENAME = "TstSBPipe"&pipenum) then
         numFound = numFound+1 
         continue
      endif
   enddo
enddo

%liv (log_procedure) = logging

if (numFound = (CFE_SB_MAX_PIPES-numberOfInitialPipes)) then
   write "<*> Passed (4301) - found ", numFound, " pipes."
   ut_setrequirements cSB4301, "P"
else
   write "<!> Failed (4301) - found ", numFound, " pipes. Expected ", (CFE_SB_MAX_PIPES-numberOfInitialPipes), "."
   ut_setrequirements cSB4301, "F"
endif

write ";**********************************************************************"
write ";  Step 5.1: Get the Pipe Information file using the default filename."
write ";   Req : 4007.1"
write ";**********************************************************************"
local work_dir = %env("WORK")
local filename
                                                                                
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_EID, DEBUG, 1
cmdCtr = $SC_$CPU_SB_CMDPC+1

/$SC_$CPU_SB_WRITEPIPE2FILE PIPEINFOFILENAME=""
wait 10 
                                                                                
ut_tlmwait $SC_$CPU_SB_CMDPC {cmdCtr}
if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Write Pipe Info File command."
else
   write "<!> Failed - Write Pipe Info File command."
endif
                                                                                
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
   write "<!> Failed - Event message not received for Write SB Routing File command."
endif

;; Get the file to the ground whether or not the event msg was rcv'd
s ftp_file (ramDir,"cfe_sb_pipe.dat","$sc_$cpu_sb_pipe51.dat",hostCPU,"G")
wait 10
                                                                                
;; Check if the file above exists and pass the requirement if it does
filename = work_dir & "/image/$sc_$cpu_sb_pipe51.dat"
if file_exists(filename) then
  write "<*> Passed (4007.1) - SB Pipe Info file downloaded successfully!"
  ut_setrequirements cSB40071, "P"
else
  write "<!> Failed (4007.1) - Did not download SB Pipe Info file."
  ut_setrequirements cSB40071, "F"
endif

write ";**********************************************************************"
write ";  Step 5.2: Send the WRITEPIPE2FILE command without a path"
write ";  	    specification for the filename."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_ERR1_EID, ERROR, 1
local errCtr = $SC_$CPU_SB_CMDEC+1

/$SC_$CPU_SB_WRITEPIPE2FILE PIPEINFOFILENAME="nopathname"
wait 10 
                                                                                
ut_tlmwait $SC_$CPU_SB_CMDEC {errCtr}
if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Write Pipe Info File command failed as expected."
else
   write "<!> Failed - Write Pipe Info File command did not increment CMDEC."
endif
                                                                                
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
   write "<!> Failed - Expected event message ", CFE_SB_SND_RTG_ERR1_EID, " not received."
endif

write ";**********************************************************************"
write ";  Step 6.0: Verify Pipe routes."
write ";   Req : 4003, 4303"
write ";**********************************************************************"

local numFound
numFound=0

start $sc_$cpu_print_sb_pipes("start", 5)

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe1") then
      if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1000') then
         numFound=numFound+1
         write "*** ",numFound,". Found route TstSBPipe1 0x1000."
      elseif ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1002') then
         numFound=numFound+1
         write "*** ",numFound,". Found route TstSBPipe1 0x1002."
      endif
   endif
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe2") then
      if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1000') then
         numFound=numFound+1
         write "*** ",numFound,". Found route TstSBPipe2 0x1000."
      elseif ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1001') then
         numFound=numFound+1
         write "*** ",numFound,". Found route TstSBPipe2 0x1001."
      endif
   endif
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe3") then
      if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1000') then
         numFound=numFound+1
         write "*** ",numFound,". Found route TstSBPipe3 0x1000."
      endif
   endif
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe4") then
      if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1002') then
         numFound=numFound+1
         write "*** ",numFound,". Found route TstSBPipe4 0x1002."
      endif
   endif
   for pipenum=5 to 55 do
      if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe"&pipenum) then
         if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1002') then
            numFound=numFound+1
            write "*** ",numFound,". Found route TstSBPipe",pipenum," 0x1002."
         endif
      endif
   enddo
enddo

%liv (log_procedure) = logging

if (numFound = (4+CFE_SB_MAX_DEST_PER_PKT)) then
   write "<*> Passed (4003;4303) - found ", numFound, " routes."
   ut_setrequirements cSB4003, "P"
   ut_setrequirements cSB4303, "P"
else
   write "<!> Failed (4003;4303) - found ", numFound, " routes."
   ut_setrequirements cSB4003, "F"
   ut_setrequirements cSB4303, "F"
endif

write ";**********************************************************************"
write ";  Step 7.0: Send message with id 2."
write ";   Req : 4305, 4704"
write ";**********************************************************************"

cmdCtr = $SC_$CPU_TST_SB_MSGCTR+CFE_SB_MAX_DEST_PER_PKT

/$SC_$CPU_TST_SB_SendMsg2
wait 5

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {cmdCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4305;4704) - TST_SB message counter as expected."
   ut_setrequirements cSB4305, "P"
   ut_setrequirements cSB4704, "P"
else
   write "<!> Failed (4305;4704) - TST_SB message counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_MSGCTR, "."
   ut_setrequirements cSB4305, "F"
   ut_setrequirements cSB4704, "F"
endif

write ";**********************************************************************"
write ";  Step 8.0: Check a non-timeout pipe."
write ";   Req : 4306"
write ";**********************************************************************"

cmdCtr = $SC_$CPU_TST_SB_MSGCTR+3

/$SC_$CPU_TST_SB_SendMsg0

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4306) - TST_SB message counter as expected."
   ut_setrequirements cSB4306, "P"
else
   write "<!> Failed (4306) - TST_SB message counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_MSGCTR, "."
   ut_setrequirements cSB4306, "F"
endif

write ";**********************************************************************"
write ";  Step 9.0: Test a pipe with timeout."
write ";   Req : 4307"
write ";**********************************************************************"
write ";  Step 9.1: Wait for 10 minutes and verify that the timeout counter"
write ";            increments by 10."
write ";**********************************************************************"
timeoutcounter = $SC_$CPU_TST_SB_TIMEOUTCOUNTER+10

;;wait (10.0*60)+30
wait (10*60)+30

if ($SC_$CPU_TST_SB_TIMEOUTCOUNTER <> timeoutcounter) then
   write "<!> Failed (4307) - timeout counter didn't increment as expected."
   write "*** Expected ", timeoutcounter, " got ", $SC_$CPU_TST_SB_TIMEOUTCOUNTER, "."
   ut_setrequirements cSB4307, "F"
else
   write "<*> Passed (4307) - timeout counter as expected."
   ut_setrequirements cSB4307, "P"
endif

write ";**********************************************************************"
write ";  Step 9.2: Send two messages in a row and check that the timeout"
write ";            counter does not increment."
write ";**********************************************************************"

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1
/$SC_$CPU_TST_SB_SendMsg0
ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 10
timeoutcounter = $SC_$CPU_TST_SB_TIMEOUTCOUNTER
cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+2

wait 45
/$SC_$CPU_TST_SB_SendMsg0
wait 45
/$SC_$CPU_TST_SB_SendMsg0

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4307) - TST_SB command counter as expected."
   ut_setrequirements cSB4307, "P"
else
   write "<!> Failed (4307) - test app command counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
   ut_setrequirements cSB4307, "F"
endif

if ($SC_$CPU_TST_SB_TIMEOUTCOUNTER = timeoutcounter) then
   write "<*> Passed (4307) - TST_SB timeout counter as expected."
   ut_setrequirements cSB4307, "P"
else
   write "<!> Failed (4307) - TST_SB timeout counter incremented unexpectedly."
   write "*** Expected ", timeoutcounter, " got ", $SC_$CPU_TST_SB_TIMEOUTCOUNTER, "."
   ut_setrequirements cSB4307, "F"
endif

write ";**********************************************************************"
write ";  Step 10.0: Send a message and check last sender information."
write ";   Req : 4309"
write ";**********************************************************************"
;; Wait until the Last Sender is "TST_SB"
ut_tlmwait $SC_$CPU_TST_SB_LastSender,"TST_SB"

cmdCtr = $SC_$CPU_TST_SB_MSGCTR+1
/$SC_$CPU_TST_SB_SendMsg1

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {cmdCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4309) - TST_SB Msg Counter incremented."
   ut_setrequirements cSB4309, "P"
else
   write "<!> Failed (4309) - TST_SB Message Counter did not increment."
   ut_setrequirements cSB4309, "F"
endif

;; Check the Last Sender for CI_Lab_App for commands
if ($SC_$CPU_TST_SB_LastSender = "CI_LAB_APP") then
   write "*** Step 10.0 passed."
   ut_setrequirements cSB4309, "P"
else
   write "*** Step 10.0 failed - last message sender information incorrect: expected CI_LAB_APP; got '", $SC_$CPU_TST_SB_LastSender, "'."
   ut_setrequirements cSB4309, "F"
endif

write ";**********************************************************************"
write ";  Step 11.0: Test zero-copy capability."
write ";   Req : 4300"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_SB, TST_SB_SENDMAXMSGSIZEZEROCPY_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_SB, TST_SB_ZEROCOPYMSGSENT_INF_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_SB, TST_SB_ZEROCOPYMSGRCVD_INF_EID, INFO, 3

/$SC_$CPU_TST_SB_SendMaxMsgZC
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
  if ($sc_$CPU_find_event[2].num_found_messages = 1 AND ;;
    $sc_$CPU_find_event[3].num_found_messages = 1) then
    write "<*> Passed (4300) - Expected event message received."
    ut_setrequirements cSB4300, "A"
  else
    write "<!> Failed (4300) - Expected event messages were not received."
    ut_setrequirements cSB4300, "F"
  endif
else
   write "<!> Failed (4300) - Expected event message was not received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4300, "F"
endif

wait 10

write ";**********************************************************************"
write ";  Step 12.0: Test max packet size."
write ";   Req : 4701, 4005(d), 4305.6"
write ";**********************************************************************"
write ";  Step 12.1: Send a message of size max + 1."
write ";**********************************************************************"

errCtr = $SC_$CPU_TST_SB_P0CMDEC+1
errorcounter = $SC_$CPU_SB_MsgSndEC+1
ut_setupevents  $SC, $CPU, CFE_SB, CFE_SB_MSG_TOO_BIG_EID, ERROR, 1

/$SC_$CPU_TST_SB_SendMaxMsgP1
wait 5

ut_tlmwait $SC_$CPU_TST_SB_P0CMDEC {errCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4701) - TST_SB command counter incremented as expected."
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4701) - TST_SB CmdEC didn't increment as expected."
   write "*** Expected ", errCtr, " got ", $SC_$CPU_TST_SB_P0CMDEC, "."
   ut_setrequirements cSB4701, "F"
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4305.6;4701) - number of event messages as expected."
   ut_setrequirements cSB43056, "P"
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4305.6;4701) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB43056, "F"
   ut_setrequirements cSB4701, "F"
endif

if ($SC_$CPU_SB_MsgSndEC = errorcounter) then
   write "<*> Passed (4005d;4701) - error counter as expected."
   ut_setrequirements cSB4005d, "P"
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4005d;4701) - incorrect error counter. "
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_MsgSndEC, "."
   ut_setrequirements cSB4005d, "F"
   ut_setrequirements cSB4701, "F"
endif

write ";**********************************************************************"
write ";  Step 12.2: Send a message of size max."
write ";**********************************************************************"
local msgCtr = $SC_$CPU_TST_SB_MSGCTR
cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1
errorcounter = $SC_$CPU_SB_MsgSndEC
ut_setupevents  $SC, $CPU, CFE_SB, CFE_SB_MSG_TOO_BIG_EID, ERROR, 1

/$SC_$CPU_TST_SB_SendMaxMsg
wait 5

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4701) - TST_SB command counter incremented as expected."
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4701) - TST_SB command counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
   ut_setrequirements cSB4701, "F"
endif

wait 60

if ($SC_$CPU_find_event[1].num_found_messages = 0) then
   write "<*> Passed (4701) - number of event messages as expected."
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4701) - incorrect number of event messages received."
   write "*** Expected 0 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4701, "F"
endif

if ($SC_$CPU_SB_MsgSndEC = errorcounter) then
   write "<*> Passed (4005d;4701) - error counter as expected."
   ut_setrequirements cSB4005d, "P"
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4005d;4701) - incorrect error counter. "
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_MsgSndEC, "."
   ut_setrequirements cSB4005d, "F"
   ut_setrequirements cSB4701, "F"
endif

if ($SC_$CPU_TST_SB_MSGCTR = msgCtr) then
   write "<*> Passed (4701) - message counter as expected."
   ut_setrequirements cSB4701, "P"
else
   write "<!> Failed (4701) - incorrect message counter."
   write "*** Expected ", msgCtr, " got ", $SC_$CPU_TST_SB_MSGCTR, "."
   ut_setrequirements cSB4701, "F"
endif

write ";**********************************************************************"
write ";  Step 12.3: Send a message of size max + 1 via zero copy."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_SB, TST_SB_SENDMAXMSGSIZEPLUSONEZEROCPY_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_MSG_TOO_BIG_EID, ERROR, 2

/$SC_$CPU_TST_SB_SendMaxMsgP1ZC
wait 10

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60
if (UT_TW_Status = UT_Success) then
  if ($sc_$cpu_find_event[2].num_found_messages = 1) then
    write "<*> Passed (4701) - Zero Copy Max + 1 command generated an error as expected."
    ut_setrequirements cSB4701, "P"
  else
    write "<!> Failed (4701) - Command succeeded when failure was expected."
    ut_setrequirements cSB4701, "F"
  endif
else
   write "<!> Failed (4701) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4701, "F"
endif

write ";**********************************************************************"
write ";  Step 13.0: Test message length of zero."
write ";   Req : "
write ";**********************************************************************"

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1

ut_setupevents $SC, $CPU, TST_SB, TST_SB_BADLENGTHZEROMSG_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_MSG_TOO_BIG_EID, ERROR, 2

/$SC_$CPU_TST_SB_SendBadLngthMsg
wait 10

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - TST_SB command counter incremented as expected."
else
   write "<!> Failed - TST_SB command counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
endif

ut_tlmwait $sc_$cpu_find_event[2].num_found_messages 1 60
if (UT_TW_Status = UT_Success) then
   write "<!> Failed - incorrect number of event messages received."
   write "*** Expected 0 got ", $SC_$CPU_find_event[1].num_found_messages, "."
else
   write "<*> Passed - number of event messages as expected."
endif

start $sc_$cpu_print_sb_pipes("rout_13-0", 10)

write ";**********************************************************************"
write ";  Step 14.0: Test no subscribers error counter."
write ";   Req : 4305.5, 4005(c)"
write ";**********************************************************************"
local pipe2index

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
      if (($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe2") AND ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1001')) then
         pipe2index = i
      endif
enddo

%liv (log_procedure) = logging

ut_setupevents $SC, $CPU, TST_SB, TST_SB_UNSUBSCRIBEPIPECMD_INF_EID, INFO, 1
/$SC_$CPU_TST_SB_UnsubPipe MSGID=$SC_$CPU_SB_RouteEntry[pipe2index].SB_MSGID PIPEID=$SC_$CPU_SB_RouteEntry[pipe2index].SB_PIPEID
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

start $sc_$cpu_print_sb_pipes("pipe2unsub", 5)

/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_SEND_NO_SUBS_EID
wait 5

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SEND_NO_SUBS_EID, INFO, 1
errorcounter = $SC_$CPU_SB_NoSubEC+1

/$SC_$CPU_TST_SB_SendMsg1
wait 5

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_SB_NoSubEC <> errorcounter) then
   write "<!> Failed (4005c;4305.5) - incorrect error counter value."
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_NoSubEC, "."
   ut_setrequirements cSB4005c, "F"
   ut_setrequirements cSB43055, "F"
else
   write "<*> Passed (4005c;4305.5) - error counter as expected."
   ut_setrequirements cSB4005c, "P"
   ut_setrequirements cSB43055, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4305.5) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB43055, "F"
else
   ut_setrequirements cSB43055, "P"
   write "<*> Passed (4305.5) - number of event messages as expected."
endif

write ";**********************************************************************"
write ";  Step 15.0: Delete Pipe 1."
write ";   	     NOTE: Once the pipe is deleted, infinate ERROR messages"
write ";   	     will be generated until the TST_SB app is terminated."
write ";   	     This is due to the fact that TST_SB requests to receive"
write ";   	     messages from that pipe. This step masks this event so"
write ";   	     that only one of these error messages is received."
write ";   Req : 4302, 4003"
write ";**********************************************************************"
s $sc_$cpu_print_sb_pipes("beforeremove", 5)
local pipe1index=0,deletedPipe=0

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
      if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe1") then
         pipe1index = i
      endif
enddo

%liv (log_procedure) = logging

;; Add an Event Filter for the CFE_SB Error Event for the deleted pipe
/$SC_$CPU_EVS_ADDEVTFLTR Application="CFE_SB" Event_ID=CFE_SB_BAD_PIPEID_EID Event_Mask=X'ffff'
wait 5

;; Save the deleted pipe ID
deletedPipe = $SC_$CPU_SB_ROUTEENTRY[pipe1index].SB_PIPEID

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SUBSCRIPTION_REMOVED_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_PIPE_DELETED_EID, DEBUG, 2
ut_setupevents $SC, $CPU, TST_SB, TST_SB_DELETEPIPE_INF_EID, INFO, 3
/$SC_$CPU_TST_SB_DeletePipe PIPEID=$SC_$CPU_SB_ROUTEENTRY[pipe1index].SB_PIPEID
wait 10

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60
if ($SC_$CPU_find_event[1].num_found_messages = 2 AND ;;
    $sc_$CPU_find_event[2].num_found_messages = 1 AND ;;
    $sc_$CPU_find_event[3].num_found_messages = 1) then
   ut_setrequirements cSB4302, "P"
   write "<*> Passed (4302) - number of event messages as expected."
else
   write "<!> Failed (4302) - incorrect number of event messages received."
   write "*** Expected 2 of EID ",CFE_SB_SUBSCRIPTION_REMOVED_EID," got ", $SC_$CPU_find_event[1].num_found_messages, "."
   write "*** Expected 1 of EID ",CFE_SB_PIPE_DELETED_EID," got ", $SC_$CPU_find_event[2].num_found_messages, "."
   write "*** Expected 1 of EID ",TST_SB_DELETEPIPE_INF_EID," got ", $SC_$CPU_find_event[3].num_found_messages, "."
   ut_setrequirements cSB4302, "F"
endif

;; Check if the proper pipe was deleted
write ";; Checking if tlm reported proper pipe ID of the deleted pipe."
if ($SC_$CPU_TST_SB_PipeDeleted = deletedPipe) then	
   write "<*> Passed (4302) - Pipe deleted is correct (", $SC_$CPU_TST_SB_PipeDeleted, ")."
   ut_setrequirements cSB4302, "P"
else
  write "<!> Failed (4302) - Pipe deleted is incorrect. Expected ", deletedPipe, " got ",$SC_$CPU_TST_SB_PipeDeleted
  ut_setrequirements cSB4302, "F"
endif

write "*** TST_SB_MSGCTR = ", $SC_$CPU_TST_SB_MSGCTR
cmdCtr = $SC_$CPU_TST_SB_MSGCTR+15
/$SC_$CPU_TST_SB_SendMsg2
wait 5

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {cmdCtr} 60
if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4302) - TST_SB Message counter as expected."
   ut_setrequirements cSB4302, "P"
else
   write "<!> Failed (4302) - TST_SB message counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_MSGCTR, "."
   ut_setrequirements cSB4302, "F"
endif

start $sc_$cpu_print_sb_pipes("rout_delpipe", 5)

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe1") then
      write "<!> Failed (4003;4302) - Pipe 1 still in the routing table."
      ut_setrequirements cSB4003, "F"
      ut_setrequirements cSB4302, "F"
   endif
enddo

%liv (log_procedure) = logging

write ";**********************************************************************"
write ";  Step 16.0: Fill Pipe ID 20 with the maximum sized msg up to it's "
write ";   	     depth. Verify that memory in use increases after the "
write ";   	     pipe is filled. Verify that the memory is released after"
write ";   	     the pipe is deleted."
write ";**********************************************************************"
local pipeIndex=0,deletedPipe=0

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
      if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe20") then
         pipeIndex = i
      endif
enddo

%liv (log_procedure) = logging

;; Send the SB_DumpStats command
/$SC_$CPU_SB_DumpStats
wait 5

;; Write out the memory in use value before filling the pipe
write "; Buffer memory in Use = ", $SC_$CPU_SB_Stat.SB_SMBMIU

;; Send the command to fill the pipe
/$SC_$CPU_TST_SB_FillPipe
wait 5

;; Send the SB_DumpStats command
/$SC_$CPU_SB_DumpStats
wait 5

;; Write out the memory in use value before filling the pipe
write "; Buffer memory in Use = ", $SC_$CPU_SB_Stat.SB_SMBMIU

/$SC_$CPU_TST_SB_DeletePipe PIPEID=$SC_$CPU_SB_ROUTEENTRY[pipeIndex].SB_PIPEID
wait 10

;; Report the pipe that was deleted
write "<*> Pipe deleted was ", $SC_$CPU_TST_SB_PipeDeleted, "."

;; Send the SB_DumpStats command
/$SC_$CPU_SB_DumpStats
wait 5

;; Write out the memory in use value after filling the pipe
write "; Buffer memory in Use = ", $SC_$CPU_SB_Stat.SB_SMBMIU

write ";**********************************************************************"
write ";  Step 17.0: Unsubscribe Message."
write ";   Req : 4304"
write ";**********************************************************************"

s $sc_$cpu_print_sb_pipes("beforeremove", 5)

local pipe2index

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
      if (($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe2") AND ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1000')) then
         pipe2index = i
         write "*** Found pipe 2 - index is ", i,",and PID = ",$SC_$CPU_SB_RouteEntry[i].SB_PIPEID, "."
      endif
enddo

%liv (log_procedure) = logging

ut_setupevents $SC, $CPU, TST_SB, TST_SB_UNSUBSCRIBEPIPECMD_INF_EID, INFO, 1

/$SC_$CPU_TST_SB_UnsubPipe MSGID=x'1000' PIPEID=$SC_$CPU_SB_RouteEntry[pipe2index].SB_PIPEID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

s $sc_$cpu_print_sb_pipes("afterremove", 5)

local passedrouting = true

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1000') then
      if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe2") then
         write "<!> Failed - Pipe 2 0x1000 stream still in routing table."
         passedrouting = false
      elseif ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe1") then
         write "*** - Pipe 1 0x1000 Subscription still in routing table."
      elseif ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe3") then
         write "*** - Pipe 3 0x1000 Subscription still in routing table."
      endif
   endif
enddo

%liv (log_procedure) = logging

if (passedrouting) then
   write "<*> Passed (4003;4304) - Routing table as expected."
   ut_setrequirements cSB4304, "P"
   ut_setrequirements cSB4003, "P"
else
   write "<!> Failed (4003;4304) - stream still in routing table."
   ut_setrequirements cSB4304, "F"
   ut_setrequirements cSB4003, "F"
endif

write ";**********************************************************************"
write ";  Step 18.0: Poll the infinite timeout pipe."
write ";   Req : 4308"
write ";**********************************************************************"
s $sc_$cpu_print_sb_pipes("beforeinfinite", 10)

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1
local msgCtrCurrent = $SC_$CPU_TST_SB_MSGCTR

/$SC_$CPU_TST_SB_InfiniteTimeout
wait 60

; Wait for 9 more minutes (10 total)
wait 60*9

write "*** Send a message, should not work because the test app is only looking at pipe 3"
/$SC_$CPU_TST_SB_SendMsg0

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if ((UT_TW_Status <> UT_Success) AND ($SC_$CPU_TST_SB_MSGCTR = msgCtrCurrent)) then
   write "<*> Passed (4308) - TST_SB command counter did not change."
   ut_setrequirements cSB4308, "P"
else
   write "<!> Failed (4308) - TST_SB command counter incremented unexpectedly."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
   ut_setrequirements cSB4308, "F"
endif

write "*** Send a message with ID 0 to take the app out of infinite timeout"
/RAW 1000C000000100A4

write "*** Wait for the test app command counter to increment, we should now get the $SC_$CPU_TST_SB_SendMsg0 command"
ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4308) - TST_SB command counter incremented as expected."
   ut_setrequirements cSB4308, "P"
else
   write "<!> Failed (4308) - TST_SB command counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
   ut_setrequirements cSB4308, "F"
endif

s $sc_$cpu_print_sb_pipes("afterinfinite", 10)

write ";**********************************************************************"
write "; Step 19.0: Test the Pass Message capability"
write ";*********************************************************************"
write "; Step 19.1: Send the TST_SB Pass Message command."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PASSMSGSENT_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PASSMSGRCVD_INF_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PASSMSG_INF_EID, INFO, 3

/$SC_$CPU_TST_SB_PassMsg

ut_tlmwait $sc_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$CPU_find_event[1].num_found_messages = 1) AND ;;
     ($sc_$CPU_find_event[3].num_found_messages = 1) then
    write "<*> Passed - TST_SB PassMsg command."
  else
    write "<!> Failed - TST_SB PassMsg command. Did not rcv the SENT or CMD events", TST_SB_PASSMSGSENT_INF_EID, " and ", TST_SB_PASSMSG_INF_EID
  endif
else
   write "<!> Failed - TST_SB PassMsg command. Did not rcv the RCVD event", TST_SB_PASSMSGRCVD_INF_EID
endif

write ";*********************************************************************"
write "; Step 19.2: Send the TST_SB Pass Message Zero Copy command."
write ";*********************************************************************"
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PASSMSGSENT_INF_EID, INFO, 1
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PASSMSGRCVD_INF_EID, INFO, 2
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PASSMSGZEROCOPY_INF_EID, INFO, 3

/$SC_$CPU_TST_SB_PassMsgZC

ut_tlmwait $sc_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  if ($sc_$CPU_find_event[1].num_found_messages = 1) AND ;;
     ($sc_$CPU_find_event[3].num_found_messages = 1) then
    write "<*> Passed - TST_SB PassMsg Zero Copy command."
  else
    write "<!> Failed - TST_SB PassMsg Zero Copy command. Did not rcv the SENT or CMD events", TST_SB_PASSMSGSENT_INF_EID, " and ", TST_SB_PASSMSGZEROCOPY_INF_EID
  endif
else
   write "<!> Failed - TST_SB PassMsg Zero Copy command. Did not rcv the RCVD event", TST_SB_PASSMSGRCVD_INF_EID
endif

wait 30

write ";**********************************************************************"
write "; Step 20.0: Command a Power-On Reset on $CPU"
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

END:

write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

for i=0 to ut_req_array_size do
   ut_pfindicate {requirements[i]} {ut_requirement[i]}
enddo

write ";**********************************************************************"
write ";  End procedure $sc_$cpu_sb_enapipes"
write ";**********************************************************************"

drop ut_req_array_size
drop ut_requirement
ENDPROC
