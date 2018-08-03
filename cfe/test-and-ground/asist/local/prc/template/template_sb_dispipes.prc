PROC $sc_$cpu_sb_dispipes
;**********************************************************************
;  Test Name:  $sc_$cpu_sb_dispipes
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
;     cSB4001	Upon receipt of Command, the cFE shall reset all counters in
;		housekeeping telemetry.
;     cSB4002	Upon receipt of a Command, the cFE shall send an SB Message
;		containing the SB statistics. 
;     cSB4003	Upon receipt of a Command, the cFE shall save the SB routing
;		information to a file.
;     cSB4003.1	If a file is not specified, the cFE shall use the 
;		<PLATFORM_DEFINED> filename. 
;     cSB4005	The cFE shall send an SB Message containing the following
;		housekeeping telemetry items:
;		(e) message receive error counter
;		(f) pipe overflow error counter
;		(g) msgId-to-pipe limit error counter.
;     cSB4008	Upon receipt of a Command, the cFE shall save the message map
;		information to the Command specified file.
;     cSB4008.1 If a file is not specified, the cFE shall use the
;		<PLATFORM_DEFINED> filename. 
;     cSB4301	Upon receipt of a Request to create a Pipe, the cFE shall create
;		a Pipe with the Request-specified Pipe Depth and the 
;		Request-specified name.
;     cSB4303	Upon receipt of a Request to Subscribe to an SB message, the cFE
;		shall establish a route using the Request-specified Message ID,
;		the Request-specified Pipe ID, the Request-specified 
;		MsgId-to-Pipe limit and the Request-specified QoS.
;     cSB4305.1 The cFE shall limit the number of messages of a particular 
;		Message ID that can be sent to an Application's Pipe. 
;     cSB4305.3 If routing a Message to an Application s Pipe results in a Pipe
;		Overflow, the cFE shall abort the send to that pipe, issue an
;		event, and continue sending to the remaining pipes.
;     cSB4305.4 If routing a Message to an Application s Pipe would exceed the
;		MsgId-to-Pipe Limit, the cFE shall abort the send to that pipe,
;		issue an event, and continue sending to the remaining pipes.
;     cSB4500	Upon a Power-on Reset the cFE shall initialize the Routing 
;		Information and clear all error counters. 
;     cSB4700	The cFE shall support a maximum of 256 Message Id's.
;     cSB4705	The cFE shall support a maximum of 64 pipes per processor.
;     cSB4706	The cFE shall support a maximum Pipe depth of 256 SB Messages.
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
write ";  TstSBPipe1	 1		N/A		30		1"
write ";  TstSBPipe2	 4		2		N/A		5"
write ";  TstSBPipe3	 10		N/A		N/A		32"
write ";  TstSBPipe4	 N/A		N/A		257		256"
write ";  TstSBPipe5-18	 N/A		N/A		10		256"
;
;  Change History
;
;	Date		Name		Description
;	 6/ 3/2005   Ingrid Albright	Original Procedure
;       12/ 4/2006   Ingrid Albright	Updated from new scenario. Now a 
;					combination of SB Pipes and SB Loading.
;	 2/20/2007   Walt Moleski	Removed requirements not tested in this
;					procedure.
;	 7/ 5/2007   Walt Moleski	Removed deleted rqmts (4006;4005.2)
;	 8/ 1/2007   Walt Moleski	Added tests for Rqmts 4003.1,4008, &
;					4008.1. Steps 1.4 thru 1.6 were added.
;
;  Arguments
;
;	Name			Description
;	None
;
;  Procedures Called
;
;	Name			Description
;       $sc_$cpu_print_sb_pipes	 Prints the status of all the test app pipes.
;       $sc_$cpu_check_sb_msgcnt Checks if the change in the message count
;				 per msg id is as expected.
; 
;  Expected Test Results and Analysis
;
;**********************************************************************
;  Define variables
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

global ut_req_array_size
ut_req_array_size = 17
global ut_requirement[0..ut_req_array_size]

#define cSB4001   0
#define cSB4002   1
#define cSB4003   2
#define cSB40031  3
#define cSB4005e  4
#define cSB4005f  5
#define cSB4005g  6
#define cSB4008   7
#define cSB40081  8
#define cSB4301   9
#define cSB4303   10
#define cSB43051  11
#define cSB43053  12
#define cSB43054  13
#define cSB4500   14
#define cSB4700   15
#define cSB4705   16
#define cSB4706   17

%liv (log_procedure) = FALSE

for i=0 to ut_req_array_size do
   ut_requirement[i] = "U"
enddo

%liv (log_procedure) = logging

local requirements[0..ut_req_array_size] = ["cSB4001","cSB4002","cSB4003","cSB4003.1", "cSB4005(e)","cSB4005(f)","cSB4005(g)","cSB4008", "cSB4008.1","cSB4301","cSB4303","cSB4305.1","cSB4305.3","cSB4305.4","cSB4500","cSB4700","cSB4705","cSB4706"]

global ing_result

LOCAL errorcounter, errorcounter2, cmdCtr, timeoutcounter, errorcounters[0..5], errorcounternames[0..5], stream, expectedresult[1..4]

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

write ";**********************************************************************"
write ";  Step 1.1.1:  Send a ""Dump Software Bus statistics packet"" command."
write ";  Requirements: cSB4002"
write ";**********************************************************************"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", "step1_1_1", hostCPU)

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_STATS_EID, DEBUG, 1

/$SC_$CPU_SB_DumpStats

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4002) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4002, "F"
else 
   ut_setrequirements cSB4002, "P"
   write "<*> Passed (4002) - number of event messages as expected."
endif

write "*** Dump Stats Results ***"
write "*** Message Ids"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMMIDIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPMIDIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMMIDALW
write "*** Pipes"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMPIU   
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPPIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPALW  
write "*** Buffer Memory"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMBMIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPBMIU 
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMBMALW 
write "*** Subscriptions"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPSIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMSALW
write "*** Buffers"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSBBIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPSBBIU 
write "*** Pipe Depth"
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPDALW
write "**************************"

local numroutes = 0

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME <> "") then
      numroutes = numroutes + 1
   endif
enddo

%liv (log_procedure) = logging

write "*** Number of entries in the routing table: ", numroutes, "."

write ";**********************************************************************"
write ";  Step 1.2:  Start the test application."
write ";**********************************************************************"

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_MAX_PIPES_MET_EID, ERROR, 1
ut_setupevents $SC, $CPU, TST_SB, TST_SB_INIT_INF_EID, INFO, 2
s load_start_app ("TST_SB", hostCPU)

ut_tlmwait $sc_$CPU_find_event[1].num_found_messages 1 60

if ($sc_$CPU_find_event[1].num_found_messages >= 1) then
   write "<*> Passed (4705) - Found Max pipe creation error."
   ut_setrequirements cSB4705, "P"
else
   write "<!> Failed (4705) - Did not find max pipe creation error."
   ut_setrequirements cSB4705, "F"
endif

ut_tlmwait $sc_$CPU_find_event[2].num_found_messages 1 60

if ($sc_$CPU_find_event[2].num_found_messages = 1) then
   write "<*> Passed - TST_SB App loaded."
else
   write "<!> Failed - TST_SB App failed to load."
   goto END
endif

wait 5

write ";**********************************************************************"
write ";  Step 1.3: Send a ""Add telemetry packet subscription to TO downlink"""
write ";	    command. "
write ";"
write ";	  Result: The CFE_SB_ROUT_ADDED_EID event message was received."
write ";**********************************************************************"

;; CPU1 is the default
stream = x'900'

;; if ("$CPU" = "CPU2") then
;;    stream = x'A00'
;; elseif ("$CPU" = "CPU3") then
;;    stream = x'B00'
;; endif

start $sc_$CPU_print_sb_pipes("rout_1-3-0", 5)

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

ut_setupevents $SC, $CPU, TO_LAB_APP, TO_ADDPKT_INF_EID, INFO, 1

/$sc_$CPU_TO_ADDPACKET Stream=stream Pkt_Size=x'0' Priority=x'0' Reliability=x'0' Buflimit=x'4'

ut_tlmwait $sc_$CPU_find_event[1].num_found_messages 1 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed - part 1."
else
   write "<!> Failed - part 1 (Found ",$sc_$CPU_find_event[1].num_found_messages," messages)."
   goto END
endif

wait 10

start $sc_$CPU_print_sb_pipes("rout_1-3-1", 5)

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($sc_$CPU_SB_RouteEntry[i].SB_MSGID = stream) then
      write "<*> Passed (4003) - part 1.5 - routing table as expected."
      ut_setrequirements cSB4003, "P"
      i=CFE_SB_MAX_MSG_IDS+1
   endif
   if (i=CFE_SB_MAX_MSG_IDS) then
      write "<!> Failed (4003) - part 1.5 - missing entry in routing table."
      ut_setrequirements cSB4003, "F"
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

start $sc_$cpu_print_sb_pipes("rout_1-3", 5)

write ";**********************************************************************"
write ";  Step 1.4: Send the command to get the Routing Information using the"
write ";            default file."
write ";**********************************************************************"
local work_dir = %env("WORK")
local filename

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_EID, DEBUG, 1
cmdCtr = $SC_$CPU_SB_CMDPC+1

/$SC_$CPU_SB_WRITEROUTING2FILE ROUTINFOFILENAME=""
;;wait 10

ut_tlmwait $SC_$CPU_SB_CMDPC {cmdCtr}
if (UT_TW_Status = UT_Success) then
   write "<*> Passed - Write Routing File command."
else
   write "<!> Failed - Write Routing File command did not increment CMDPC"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
   write "<!> Failed - Event message not received for Write SB Routing File command."
endif

;; Get the file to the ground whether or not the event msg was rcv'd
s ftp_file (ramDir,"cfe_sb_route.dat","$sc_$cpu_sb_route14.dat",hostCPU,"G")
wait 10

;; Check if the file above exists and pass the requirement if it does
filename = work_dir & "/image/$sc_$cpu_sb_route14.dat"
if file_exists(filename) then
  write "<*> Passed (4003.1) - SB Routing file downloaded successfully!"
  ut_setrequirements cSB40031, "P"
else
  write "<!> Failed (4003.1) - Did not download SB Routing file."
  ut_setrequirements cSB40031, "F"
endif

write ";**********************************************************************"
write ";  Step 1.5: Send the command to get the message map information using "
write ";            a specified file."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_EID, DEBUG, 1
 
start get_file_to_cvt(ramDir, "cfe_sb_msgmap.dat", "$sc_$cpu_sb_msgmap15.dat", hostCPU)
wait 10

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (4008) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements cSB4008, "P"
else
  write "<!> Failed (4008) - Event message not received for Write SB Msg Map File command."
  ut_setrequirements cSB4008, "F"
endif

write ";**********************************************************************"
write ";  Step 1.6: Send the command to get the message map information using "
write ";            the default file."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_EID, DEBUG, 1
cmdCtr = $SC_$CPU_SB_CMDPC+1

/$SC_$CPU_SB_WRITEMAP2FILE MAPINFOFILENAME=""
wait 5

ut_tlmwait $SC_$CPU_SB_CMDPC {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Write Msg Map File command."
  ut_setrequirements cSB4008, "P"
else
  write "<!> Failed (4008) - Write Msg Map File command."
  ut_setrequirements cSB4008, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed (4008) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements cSB4008, "P"
else
   write "<!> Failed (4008) - Event message not received for Write SB Msg Map File command."
  ut_setrequirements cSB4008, "F"
endif

;; Get the file to the ground whether or not the event msg was rcv'd
s ftp_file (ramDir,"cfe_sb_msgmap.dat","$sc_$cpu_sb_msgmap16.dat",hostCPU,"G")
wait 10

;; Check if the file above exists and pass the requirement if it does
filename = work_dir & "/image/$sc_$cpu_sb_msgmap16.dat"
if file_exists(filename) then
  write "<*> Passed (4008.1) - SB Msg Map file downloaded successfully!"
  ut_setrequirements cSB40081, "P"
else
  write "<!> Failed (4008.1) - Did not download SB Msg Map file."
  ut_setrequirements cSB40081, "F"
endif

write ";**********************************************************************"
write "; Step 1.7: Send the WRITEMAP2FILE command without a path specification"
write ";           for the filename."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_ERR1_EID, ERROR, 1
local errCtr = $SC_$CPU_SB_CMDEC+1

/$SC_$CPU_SB_WRITEMAP2FILE MAPINFOFILENAME="nopathname"
wait 5

ut_tlmwait $SC_$CPU_SB_CMDEC {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Write Map File command failed as expected."
else
  write "<!> Failed - Write Map File command did not increment the CMDEC."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
   write "<!> Failed - Expected Event message ",CFE_SB_SND_RTG_ERR1_EID, " not received."
endif

write ";**********************************************************************"
write "; Step 1.8: Send the WRITEROUTING2FILE command without a path "
write ";           specification for the filename."
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_RTG_ERR1_EID, ERROR, 1
errCtr = $SC_$CPU_SB_CMDEC+1

/$SC_$CPU_SB_WRITEROUTING2FILE ROUTINFOFILENAME="nopathname"
wait 5

ut_tlmwait $SC_$CPU_SB_CMDEC {errCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Write Routing File command failed as expected."
else
  write "<!> Failed - Write Routing File command did not increment the CMDEC."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
   write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
   write "<!> Failed - Expected Event message ",CFE_SB_SND_RTG_ERR1_EID, " not received."
endif

write ";**********************************************************************"
write ";  Step 2.0: Disable Pipe Processing."
write ";**********************************************************************"

ut_setupevents $SC, $CPU, TST_SB, TST_SB_DISABLEPIPE_INF_EID, INFO, 1

/$SC_$CPU_TST_SB_DisablePipe

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
else
   write "<*> Passed - number of event messages as expected."
endif

write ";**********************************************************************"
write ";  Step 3.0: Cause message id to pipe limit error."
write ";   Req : 4305.1, 4005(g), 4305.4, 4303"
write ";**********************************************************************"
write ";  Step 3.1: Send a message with id 0."
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "3_1", x'1000')
expectedresult = [1, 1, 1, 0]

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1
/$SC_$CPU_TST_SB_SendMsg0

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   write "<*> Passed (4303) - TST_SB command counter incremented as expected."
   ut_setrequirements cSB4303, "P"
else
   ut_setrequirements cSB4303, "F"
   write "<!> Failed (4303) - TST_SB command counter didn't increment as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "3_1", x'1000', expectedresult)
if (ing_result) then
   ut_setrequirements cSB4303, "P"
   write "<*> Passed (4303) - msg counters as expected."
else
   ut_setrequirements cSB4303, "F"
   write "<!> Failed (4303) - msg counters incorrect."
endif

write ";**********************************************************************"
write ";  Step 3.2: Send two messages with id 1."
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "3_2", x'1001')
expectedresult = [0, 2, 0, 0]

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1

/$SC_$CPU_TST_SB_SendMsg1

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status <> UT_Success) then
   write "<!> Failed (4303) - SendMsg1 command didn't increment command counter as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
   ut_setrequirements cSB4303, "F"
else
   ut_setrequirements cSB4303, "P"
   write "<*> Passed (4303) - SendMsg1 command incremented command counter as expected."
endif

/$SC_$CPU_TST_SB_SendMsg1
ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr+1} 60
   
if (UT_TW_Status <> UT_Success) then
   write "<!> Failed (4303) - SendMsg1 command #2 didn't increment command counter as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
   ut_setrequirements cSB4303, "F"
else
   ut_setrequirements cSB4303, "P"
   write "<*>Passed (4303) - SendMsg1 command #2 incremented command counter as expected."
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "3_2", x'1001', expectedresult)

if (ing_result) then
   ut_setrequirements cSB4303, "P"
   write "<*> Passed (4303) - msg counters as expected."
else
   write "<!> Failed (4303) - msg counters incorrect."
   ut_setrequirements cSB4303, "F"
endif

write ";**********************************************************************"
write ";  Step 3.3: Send a message with id 0."
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "3_3", x'1000')
expectedresult = [0, 1, 1, 0]

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1

/$SC_$CPU_TST_SB_SendMsg0

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   ut_setrequirements cSB4303, "P"
   write "<*> Passed (4303) - SendMsg0 command incremented command counter as expected."
else
   ut_setrequirements cSB4303, "F"
   write "<!> Failed (4303) - SendMsg0 command didn't increment command counter as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "3_3", x'1000', expectedresult)
if (ing_result) then
   ut_setrequirements cSB4303, "P"
   write "<*> Passed (4303) - msg counters as expected."
else
   ut_setrequirements cSB4303, "F"
   write "<!> Failed (4303) - msg counters incorrect."
endif

write ";**********************************************************************"
write ";  Step 3.4: Send a message with id 1."
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "3_4", x'1001')
expectedresult = [0, 0, 0, 0]
errorcounter = $SC_$CPU_SB_MsgLimEC+1
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_MSGID_LIM_ERR_EID, ERROR, 1
/$SC_$CPU_TST_SB_SendMsg1
wait 10

if ($SC_$CPU_SB_MsgLimEC <> errorcounter) then
   write "<!> Failed (4303;4005g) - incorrect error counter value."
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_MsgLimEC, "."
   ut_setrequirements cSB4303, "F"
   ut_setrequirements cSB4005g, "F"
else
   write "<*> Passed (4303;4005g) - error counter as expected."
   ut_setrequirements cSB4303, "P"
   ut_setrequirements cSB4005g, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4303;4305.4) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4303, "F"
   ut_setrequirements cSB43054, "F"
else 
   write "<*> Passed (4303;4305.4) - number of event messages as expected."
   ut_setrequirements cSB4303, "P"
   ut_setrequirements cSB43054, "P"
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "3_4", x'1001', expectedresult)
if (ing_result) then
   write "<*> Passed (4303;4305.1) - msg counters as expected."
   ut_setrequirements cSB4303, "P"
   ut_setrequirements cSB43051, "P"
else   
   write "<!> Failed (4303;4305.1) - msg counters incorrect."
   ut_setrequirements cSB4303, "F"
   ut_setrequirements cSB43051, "F"
endif

write ";**********************************************************************"
write ";  Step 4.0: Cause pipe overflow error."
write ";   Req : 4305.3, 4005(f), 4301"
write ";**********************************************************************"
write ";  Step 4.1: Send a message with id 0."
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "4_1", x'1000')
expectedresult = [0, 1, 1, 0]

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1

/$SC_$CPU_TST_SB_SendMsg0

ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

if (UT_TW_Status = UT_Success) then
   ut_setrequirements cSB4301, "P"
   write "<*> Passed (4301) - SendMsg0 command incremented command counter as expected."
else
   ut_setrequirements cSB4301, "F"
   write "<!> Failed (4301) - SendMsg0 command didn't increment command counter as expected."
   write "*** Expected ", cmdCtr, " got ", $SC_$CPU_TST_SB_P0CMDPC, "."
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "4_1", x'1000', expectedresult)   
if (ing_result) then
   write "<*> Passed (4301) - msg counters as expected."
   ut_setrequirements cSB4301, "P"
else
   ut_setrequirements cSB4301, "F"
   write "<!> Failed (4301) - msg counters incorrect."
endif

write ";**********************************************************************"
write ";  Step 4.2: Send a message with id 0."
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "4_2", x'1000')
expectedresult = [0, 0, 1, 0]
errorcounter = $SC_$CPU_SB_PipeOvrEC+1
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_Q_FULL_ERR_EID, ERROR, 1

/$SC_$CPU_TST_SB_SendMsg0
wait 5
ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_SB_PipeOvrEC <> errorcounter) then
   write "<!> Failed (4005f;4301) - incorrect error counter value."
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_PipeOvrEC, "."
   ut_setrequirements cSB4301, "F"
   ut_setrequirements cSB4005f, "F"
else
   write "<*> Passed (4005f;4301) - error counter value as expected."
   ut_setrequirements cSB4005f, "P"
   ut_setrequirements cSB4301, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4301;4305.3) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4301, "F"
   ut_setrequirements cSB43053, "F"
else
   ut_setrequirements cSB4301, "P"
   write "<*> Passed (4301;4305.3) - number of event messages as expected."
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "4_2", x'1000', expectedresult)
if (ing_result) then
   ut_setrequirements cSB4301, "P"
   ut_setrequirements cSB43053, "P"
   write "<*> Passed (4301;4305.3) - msg counters as expected."
else
   ut_setrequirements cSB4301, "F"
   ut_setrequirements cSB43053, "F"
   write "<!> Failed (4301;4305.3) - msg counters incorrect."
endif

write ";**********************************************************************"
write ";  Step 5.0: Send a Reset SB counters command."
write ";   Req : 4001"
write ";**********************************************************************"

; Increment $SC_$CPU_SB_MsgSndEC
errorcounter = $SC_$CPU_SB_MsgSndEC+1
/$SC_$CPU_TST_SB_SendMaxMsgP1
ut_tlmwait $SC_$CPU_SB_MsgSndEC {errorcounter} 30

; Increment $SC_$CPU_SB_MsgRecEC
errorcounter = $SC_$CPU_SB_MsgRecEC+1
/$SC_$CPU_TST_SB_IncrementMsgRcvEC
ut_tlmwait $SC_$CPU_SB_MsgRecEC {errorcounter} 30
if ($SC_$CPU_SB_MsgRecEC = errorcounter) then
   write "<*> Passed (4005e) - MsgRecEC counter correct."
   ut_setrequirements cSB4005e, "P"
else 
   write "<!> Failed (4005e) - MsgRecEC counter incorrect."
   ut_setrequirements cSB4005e, "F"
endif

; Increment $SC_$CPU_SB_NewPipeEC
errorcounter = $SC_$CPU_SB_NewPipeEC+1
/$SC_$CPU_TST_SB_IncrementNewPipeEC
ut_tlmwait $SC_$CPU_SB_NewPipeEC {errorcounter} 30
;;                     $SC_$CPU_SB_NoSubEC,                           ;;
;;                     "$SC_$CPU_SB_NoSubEC",                         ;;
local noSubECOld = $SC_$CPU_SB_NoSubEC

errorcounters =     [$SC_$CPU_SB_CMDPC,                             ;;
                     $SC_$CPU_SB_MsgSndEC,   $SC_$CPU_SB_MsgRecEC,  ;;  
                     $SC_$CPU_SB_NewPipeEC,                         ;;
                     $SC_$CPU_SB_PipeOvrEC,  $SC_$CPU_SB_MsgLimEC]
errorcounternames = ["$SC_$CPU_SB_CMDPC",                           ;;
                     "$SC_$CPU_SB_MsgSndEC", "$SC_$CPU_SB_MsgRecEC",;;
                     "$SC_$CPU_SB_NewPipeEC",                       ;;
                     "$SC_$CPU_SB_PipeOvrEC","$SC_$CPU_SB_MsgLimEC"]

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_CMD1_RCVD_EID, DEBUG, 1

/$SC_$CPU_SB_ResetCtrs

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4001) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4001, "F"
else 
   ut_setrequirements cSB4001, "P"
   write "<*> Passed (4001) - number of event messages as expected."
endif

wait 5

for i=0 to 5 do
   if ({errorcounternames[i]} <> 0) then
      write "<!> Failed (4001) - incorrect ", errorcounternames[i], " value."
      write "*** Expected 0 got ", {errorcounternames[i]}, ". Was ", errorcounters[i], " before reset command was sent."
      ut_setrequirements cSB4001, "F"
   elseif (errorcounters[i] = 0) then
      write "<!> Failed (4001) - ", errorcounternames[i], " was 0 before the reset command was sent."
      ut_setrequirements cSB4001, "F"
   else
      write "<*> Passed (4001) - error counter ", errorcounternames[i], " as expected. Was ", errorcounters[i], " before reset command was sent, is now 0."
      ut_setrequirements cSB4001, "P"
   endif
enddo

if (noSubECOld > $SC_$CPU_SB_NoSubEC) then
      write "<*> Passed (4001) - No subscribers error counter was reset. Was ", noSubECOld, " before reset command was sent, is now ", $SC_$CPU_SB_NoSubEC
      ut_setrequirements cSB4001, "P"
   else
      write "<!> Failed (4001) - No subscribers EC was not reset."
      ut_setrequirements cSB4001, "F"
endif

write ";**********************************************************************"
write ";  Step 5.1:  Send a ""Dump Software Bus statistics packet"" command."
write ";  Requirements: cSB4002"
write ";**********************************************************************"

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_STATS_EID, DEBUG, 1

/$SC_$CPU_SB_DumpStats

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4002) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4002, "F"
else 
   ut_setrequirements cSB4002, "P"
   write "<*> Passed (4002) - number of event messages as expected."
endif

write "*** Dump Stats Results ***"
write "*** Message Ids"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMMIDIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPMIDIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMMIDALW
write "*** Pipes"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMPIU   
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPPIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPALW  
write "*** Buffer Memory"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMBMIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPBMIU 
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMBMALW 
write "*** Subscriptions"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPSIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMSALW
write "*** Buffers"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSBBIU
write "***  Desc In Use: ", $SC_$CPU_SB_STAT.SB_SMPSBBIU 
write "*** Pipe Depth"
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPDALW
for i=1 to CFE_SB_MAX_PIPES do
  write "***       PipeID: ", $SC_$CPU_SB_STAT.SB_SMPDS[i].SB_PDPIPEID
  write "***        Depth: ", $SC_$CPU_SB_STAT.SB_SMPDS[i].SB_PDDEPTH
  write "***       In Use: ", $SC_$CPU_SB_STAT.SB_SMPDS[i].SB_PDINUSE
  write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPDS[i].SB_PDPKINUSE
enddo
write "**************************"

wait 5

write ";**********************************************************************"
write ";  Step 6.0: Send a command to the test application to process one "
write ";            message on each pipe. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, TST_SB, TST_SB_PROCESSONEPIPEMSG_EID, INFO, 1

/$SC_$CPU_TST_SB_ProcessOnePipeMsg
wait 20

start $sc_$cpu_print_sb_pipes("6", 3)

write ";**********************************************************************"
write ";  Step 7.0:  Send a message to the pipes. "
write ";"
write ";	     Result: No error messages are received. "
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "7_0", x'1000')
expectedresult = [1, 1, 1, 0]

errorcounter = $SC_$CPU_SB_PipeOvrEC
errorcounter2 = $SC_$CPU_SB_MsgLimEC

ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_MSGID_LIM_ERR_EID, ERROR, 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_Q_FULL_ERR_EID, ERROR, 2

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1
/$SC_$CPU_TST_SB_SendMsg0
ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

wait 30

if ($SC_$CPU_SB_PipeOvrEC <> errorcounter) then
   write "<!> Failed (4005f;4305.3) - incorrect PipeOvr error counter value."
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_PipeOvrEC, "."
   ut_setrequirements cSB43053, "F"
   ut_setrequirements cSB4005f, "F"
else
   write "<*> Passed (4005f;4305.3) - PipeOvr error counter as expected."
   ut_setrequirements cSB43053, "P"
   ut_setrequirements cSB4005f, "P"
endif

if ($SC_$CPU_SB_MsgLimEC <> errorcounter2) then
   write "<!> Failed (4005g;4305.4) - incorrect MsgLim error counter value."
   write "*** Expected ", errorcounter2, " got ", $SC_$CPU_SB_MsgLimEC, "."
   ut_setrequirements cSB43054, "F"
   ut_setrequirements cSB4005g, "F"
else
   write "<*> Passed (4005g;4305.4) - MsgLim error counter as expected."
   ut_setrequirements cSB43054, "P"
   ut_setrequirements cSB4005g, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages <> 0) then
   write "<!> Failed (4305.4) - MsgLim event messages received."
   write "*** Expected 0 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB43054, "F"
else
   write"<*> Passed (4305.4) - Did not rcv any MsgLim event messages as expected."
   ut_setrequirements cSB43054, "P"
endif

if ($SC_$CPU_find_event[2].num_found_messages <> 0) then
   write "<!> Failed (4305.3) - Pipe Overflow event messages received."
   write "*** Expected 0 got ", $SC_$CPU_find_event[2].num_found_messages, "."
   ut_setrequirements cSB43053, "F"
else
   write"<*> Passed (4305.3) - Did not rcv any Pipe Overflow event messages as expected."
   ut_setrequirements cSB43053, "P"
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "7_0", x'1000', expectedresult)
if (ing_result) then
   ut_setrequirements cSB43053, "P"
   ut_setrequirements cSB43054, "P"
   write "<*> Passed (4305.3;4305.4) -  msg counters as expected."
else
   ut_setrequirements cSB43053, "F"
   ut_setrequirements cSB43054, "F"
   write "<!> Failed (4305.3;4305.4) - msg counters incorrect."
endif

start $sc_$cpu_print_sb_pipes("7", 3)

write ";**********************************************************************"
write ";  Step 8.0: Send a message with id 0. "
write ";"
write ";	 Results: ""Pipe Overflow"" error event messages were received "
write ";		for Pipe 1. "
write ";		The ""Pipe Overflow"" error counter, "
write ";		$SC_$CPU_SB_PipeOvrEC, increases."
write ";		The ""MsgId-to-Pipe Limit"" error event message was "
write ";		received for Pipe 2. "
write ";		The ""MsgId-to-Pipe Limit"" error counter, "
write ";		$SC_$CPU_SB_MsgLimEC, incremented."
write ";"
write ";**********************************************************************"

start $sc_$cpu_check_sb_msgcnt("set", "8_0", x'1000')
expectedresult = [0, 0, 1, 0]

errorcounter = $SC_$CPU_SB_PipeOvrEC+1
errorcounter2 = $SC_$CPU_SB_MsgLimEC+1

;; Delete the event filter for Pipe Overflow Error and Msg Limit
;; This is needed since the events are filtered and would not be rcv'd
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_Q_FULL_ERR_EID
wait 5
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_MSGID_LIM_ERR_EID
wait 5

ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_MSGID_LIM_ERR_EID, ERROR, 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_Q_FULL_ERR_EID, ERROR, 2

write "*** PipeOverEC = ", $SC_$CPU_SB_PipeOvrEC
write "*** MsgLimEC   = ", $SC_$CPU_SB_MsgLimEC

cmdCtr = $SC_$CPU_TST_SB_P0CMDPC+1
/$SC_$CPU_TST_SB_SendMsg0
wait 5
ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {cmdCtr} 60

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_SB_PipeOvrEC <> errorcounter) then
   write "<!> Failed (4005f;4305.3)- incorrect PipeOvr error counter value."
   write "*** Expected ", errorcounter, " got ", $SC_$CPU_SB_PipeOvrEC, "."
   ut_setrequirements cSB43053, "F"
   ut_setrequirements cSB4005f, "F"
else
   write "<*> Passed (4005f;4305.3) - PipeOvr error counter as expected."
   ut_setrequirements cSB43053, "P"
   ut_setrequirements cSB4005f, "P"
endif

if ($SC_$CPU_SB_MsgLimEC <> errorcounter2) then
   write "<!> Failed (4005g;4305.4) - incorrect MsgLim error counter value."
   write "*** Expected ", errorcounter2, " got ", $SC_$CPU_SB_MsgLimEC, "."
   ut_setrequirements cSB43054, "F"
   ut_setrequirements cSB4005g, "F"
else
   write "<*> Passed (4005g;4305.4) - MsgLim error counter as expected."
   ut_setrequirements cSB43054, "P"
   ut_setrequirements cSB4005g, "P"
endif

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4305.4) - incorrect number of MsgLim event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB43054, "F"
else
   write"<*> Passed (4305.4) - number of MsgLim event messages as expected."
   ut_setrequirements cSB43054, "P"
endif

if ($SC_$CPU_find_event[2].num_found_messages <> 1) then
   write "<!> Failed (4305.3) - incorrect number of Pipe Overflow event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[2].num_found_messages, "."
   ut_setrequirements cSB43053, "F"
else
   write"<*> Passed (4305.3) - number of Pipe Overflow event messages as expected."
   ut_setrequirements cSB43053, "P"
endif

wait 30

start $sc_$cpu_check_sb_msgcnt("check", "8_0", x'1000', expectedresult)
if (ing_result) then
   ut_setrequirements cSB43053, "P"
   ut_setrequirements cSB43054, "P"
   write "<*> Passed (4305.3;4305.4) - msg counters as expected."
else
   ut_setrequirements cSB43053, "F"
   ut_setrequirements cSB43054, "F"
   write "<!> Failed (4305.3;4305.4) - msg counters incorrect."
endif

ut_setupevents "$SC", "$CPU", "", 0, "", 2

start $sc_$cpu_print_sb_pipes("8_0", 3)

write ";**********************************************************************"
write ";  Step 9.0: Send max pipe depth messages of id 2."
write ";            Req : 4706"
write ";**********************************************************************"
write ";  Step 9.1: Filter out the annoying Pipe 50 Message Id to Pipe Limit "
write ";            errors that we are receiving from the test app at this "
write ";            point. We aren't using Message Id to Pipe Limit error "
write ";            event messages anymore in the test from this point on."
write ";**********************************************************************"
/$SC_$CPU_EVS_AddEvtFltr APPLICATION="CFE_SB" EVENT_ID=X'12' EVENT_MASK=X'FFFF'

write ";**********************************************************************"
write ";  Step 9.2: Disable Routing for pipe 1 so that we won't get overflows."
write ";**********************************************************************"
local pipe1index=0

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe1") then
      pipe1index = i
   endif
enddo

%liv (log_procedure) = logging

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_DSBL_RTE2_EID, DEBUG, 1

/$SC_$CPU_SB_DisRoute MID=$SC_$CPU_SB_RouteEntry[pipe1index].SB_MSGID PID=$SC_$CPU_SB_RouteEntry[pipe1index].SB_PIPEID

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

write ";**********************************************************************"
write ";  Step 9.3: Disable Routing for pipes 5-18 so that we won't get msg id"
write ";            limits on them."
write ";**********************************************************************"
local pipeindex=0
for j=5 to 18 do
  %liv (log_procedure) = FALSE

  for i=1 to CFE_SB_MAX_MSG_IDS do
    if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = ("TstSBPipe"&j)) then
      pipeindex = i
    endif
  enddo
  %liv (log_procedure) = logging

  ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_DSBL_RTE2_EID, DEBUG, 1

  /$SC_$CPU_SB_DisRoute MID=$SC_$CPU_SB_RouteEntry[pipeindex].SB_MSGID PID=$SC_$CPU_SB_RouteEntry[pipeindex].SB_PIPEID

  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60
enddo

write ";**********************************************************************"
write ";  Step 9.4: Send CFE_SB_MAX_PIPE_DEPTH messages to pipe 4."
write ";**********************************************************************"
local msgCtrCurrent
msgCtrCurrent = $SC_$CPU_TST_SB_MSGCTR

start $sc_$cpu_print_sb_pipes("pipe4before", 4)

;; Delete the event filter for Pipe Overflow Error
;; This is needed since this event is filtered and would not be rcv'd
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_Q_FULL_ERR_EID
wait 5

; 1 for Pipe 4
errorcounter = $SC_$CPU_SB_PipeOvrEC+1

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_Q_FULL_ERR_EID , ERROR, 1

local logstoppoint = 3
local logresumepoint = CFE_SB_MAX_PIPE_DEPTH-2

local sbtstcmdpc = $SC_$CPU_TST_SB_P0CMDPC

for i=1 to logstoppoint do
   write "*** ",i,"."
   /$SC_$CPU_TST_SB_SendMsg2
   ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {sbtstcmdpc+i} 10
enddo

%liv (log_procedure) = FALSE

for i=(logstoppoint+1) to logresumepoint do
   /$SC_$CPU_TST_SB_SendMsg2
   ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {sbtstcmdpc+i} 10
   ; Account for the rollover that appears to happen.
   if ($SC_$CPU_TST_SB_P0CMDPC = 0) then
      sbtstcmdpc = 0-i
   endif
enddo

%liv (log_procedure) = logging

for i=(logresumepoint+1) to CFE_SB_MAX_PIPE_DEPTH+1 do
   write "*** ",i,"."
   /$SC_$CPU_TST_SB_SendMsg2
   ut_tlmwait $SC_$CPU_TST_SB_P0CMDPC {sbtstcmdpc+i} 10
   ; Account for the rollover that appears to happen.
   if ($SC_$CPU_TST_SB_P0CMDPC = 0) then
      sbtstcmdpc = 0-i
   endif
enddo

write ";**********************************************************************"
write ";  Step 9.5: Check that the routing table shows that the message count"
write ";            is equal to the max depth."
write ";**********************************************************************"
local pipe4index

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if (($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe4") AND ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = x'1002')) then
      pipe4index = i
   endif
enddo

%liv (log_procedure) = logging

start $sc_$cpu_print_sb_pipes("pipe4after", 4)

if ($SC_$CPU_SB_RouteEntry[pipe4index].SB_MSGCNT < CFE_SB_MAX_PIPE_DEPTH) then
   write "<!> Failed (4003;4706) - incorrect pipe message counter. Expected ",CFE_SB_MAX_PIPE_DEPTH," got ", $SC_$CPU_SB_RouteEntry[pipe4index].SB_MSGCNT, "."
   ut_setrequirements cSB4706, "F"
   ut_setrequirements cSB4003, "F"
elseif ($SC_$CPU_SB_RouteEntry[pipe4index].SB_MSGCNT > CFE_SB_MAX_PIPE_DEPTH) then
   write "<!> Failed (4706) - Pipe 4 has accepted more than ",CFE_SB_MAX_PIPE_DEPTH," messages without overflowing."
   ut_setrequirements cSB4706, "F"
elseif ($SC_$CPU_SB_RouteEntry[pipe4index].SB_MSGCNT = CFE_SB_MAX_PIPE_DEPTH) then
   write "<*> Passed - Pipe 4 has accepted ",CFE_SB_MAX_PIPE_DEPTH," messages."
endif

if (($SC_$CPU_find_event[1].num_found_messages = 1) AND ($SC_$CPU_SB_PipeOvrEC = errorcounter)) then
   write "<*> Passed (4706) - message count on pipe 4 is ", $SC_$CPU_SB_RouteEntry[pipe4index].SB_MSGCNT, "."
   ut_setrequirements cSB4706, "P"
else
   write "<!> Failed (4706) - incorrect pipe overflow error counter. Expected ", errorcounter, " got ", $SC_$CPU_SB_PipeOvrEC, "."
   write "*** incorrect num found messages. Expected 1, got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4706, "F"
endif

write ";**********************************************************************"
write ";  Step 9.6: Enable processing for pipe 4 and check that"
write ";            CFE_SB_MAX_PIPE_DEPTH pipe messages are received."
write ";**********************************************************************"
/$SC_$CPU_TST_SB_EnablePipe

ut_tlmwait $SC_$CPU_TST_SB_MSGCTR {msgCtrCurrent+CFE_SB_MAX_PIPE_DEPTH} 60

write "*** Message counter: ", $SC_$CPU_TST_SB_MSGCTR, " expected ", (msgCtrCurrent+CFE_SB_MAX_PIPE_DEPTH), "."

write ";**********************************************************************"
write ";  Step 9.7: Enable routing for pipes 5-18."
write ";**********************************************************************"
local pipeindex=0
for j=5 to 18 do
  %liv (log_procedure) = FALSE

  for i=1 to CFE_SB_MAX_MSG_IDS do
    if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = ("TstSBPipe"&j)) then
      pipeindex = i
    endif
  enddo
  %liv (log_procedure) = logging

  ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_ENBL_RTE2_EID, DEBUG, 1

  /$SC_$CPU_SB_EnaRoute MID=$SC_$CPU_SB_RouteEntry[pipeindex].SB_MSGID PID=$SC_$CPU_SB_RouteEntry[pipeindex].SB_PIPEID

  ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60
enddo

wait 60

write ";**********************************************************************"
write ";  Step 10.0: Test the max number of message ids."
write ";  Requirements: cSB4700"
write ";**********************************************************************"
write ";  Step 10.1:  Send a ""Dump Software Bus statistics packet"" command."
write ";  Requirements: cSB4002"
write ";**********************************************************************"

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_STATS_EID, DEBUG, 1

/$SC_$CPU_SB_DumpStats

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4002) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4002, "F"
else 
   ut_setrequirements cSB4002, "P"
   write "<*> Passed (4002) - number of event messages as expected."
endif

write "*** Dump Stats Results ***"
write "*** Message Ids"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMMIDIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPMIDIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMMIDALW
write "*** Pipes"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMPIU   
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPPIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPALW  
write "*** Buffer Memory"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMBMIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPBMIU 
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMBMALW 
write "*** Subscriptions"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPSIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMSALW
write "*** Buffers"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSBBIU
write "***  Desc In Use: ", $SC_$CPU_SB_STAT.SB_SMPSBBIU 
write "*** Pipe Depth"
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPDALW
write "**************************"

write ";**********************************************************************"
write ";  Step 10.2: Create the max number of message ids."
write ";  Requirements: cSB4700"
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_MAX_MSGS_MET_EID, ERROR, 1

local msgidsleft = CFE_SB_MAX_MSG_IDS
local prevmsgid = 0
local lastMsgId = 0

%liv (log_procedure) = FALSE

for i=1 to CFE_SB_MAX_MSG_IDS do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME <> "") then
      if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID <> prevmsgid) then
         msgidsleft = msgidsleft - 1
      endif
      prevmsgid = $SC_$CPU_SB_RouteEntry[i].SB_MSGID

      if (SCX_$CPU_SB_RouteEntry[i].SB_Appname = "TST_SB") then
        if (SCX_$CPU_SB_RouteEntry[i].SB_MSGID > lastMsgId) AND ;;
           (SCX_$CPU_SB_RouteEntry[i].SB_MSGID < x'1100') then
          lastMsgId = SCX_$CPU_SB_RouteEntry[i].SB_MSGID
        endif
      endif
   endif
enddo

%liv (log_procedure) = logging

write "*** Number of message id's to use to hit the max: ", msgidsleft, "."
lastMsgId = lastMsgId + 1

for i=1 to msgidsleft+1 do
   write i, ". Sending ", %hex(lastMsgId,4)
   /$SC_$CPU_TST_SB_MaxMsgIDs MSGID=lastMsgId PIPEID=$SC_$CPU_SB_RouteEntry[pipe4index].SB_PIPEID
   wait 3
   lastMsgId = lastMsgId + 1
enddo
wait 5

;; Check that at least 1 error was generated
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
   ut_setrequirements cSB4700, "P"
   write "<*> Passed (4700) - Max msgid error was generated."
else
   ut_setrequirements cSB4700, "F"
   write "<!> Failed (4700) - did not find max msgid error."
   write "*** Found ", $SC_$CPU_find_event[1].num_found_messages, " messages."
endif

write ";**********************************************************************"
write ";  Step 10.3:  Send a ""Dump Software Bus statistics packet"" command."
write ";  Requirements: cSB4002"
write ";**********************************************************************"

ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_SND_STATS_EID, DEBUG, 1

/$SC_$CPU_SB_DumpStats

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages 1 60

if ($SC_$CPU_find_event[1].num_found_messages <> 1) then
   write "<!> Failed (4002) - incorrect number of event messages received."
   write "*** Expected 1 got ", $SC_$CPU_find_event[1].num_found_messages, "."
   ut_setrequirements cSB4002, "F"
else 
   ut_setrequirements cSB4002, "P"
   write "<*> Passed (4002) - number of event messages as expected."
endif

write "*** Dump Stats Results ***"
write "*** Message Ids"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMMIDIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPMIDIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMMIDALW
write "*** Pipes"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMPIU   
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPPIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPALW  
write "*** Buffer Memory"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMBMIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPBMIU 
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMBMALW 
write "*** Subscriptions"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSIU
write "***  Peak In Use: ", $SC_$CPU_SB_STAT.SB_SMPSIU
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMSALW
write "*** Buffers"
write "***  In Use:      ", $SC_$CPU_SB_STAT.SB_SMSBBIU
write "***  Desc In Use: ", $SC_$CPU_SB_STAT.SB_SMPSBBIU 
write "*** Pipe Depth"
write "***  Max Allowed: ", $SC_$CPU_SB_STAT.SB_SMMPDALW
write "**************************"

wait 5

write ";**********************************************************************"
write ";  Step 11.0:  Perform a Power-on reset and verify that the error "
write ";              counters are cleared."
write ";  Requirements: cSB4500"
write ";**********************************************************************"

; Increment $SC_$CPU_SB_MsgSndEC
errorcounter = $SC_$CPU_SB_MsgSndEC+1
/$SC_$CPU_TST_SB_SendMaxMsgP1
ut_tlmwait $SC_$CPU_SB_MsgSndEC {errorcounter} 60

; Increment $SC_$CPU_SB_MsgRecEC
errorcounter = $SC_$CPU_SB_MsgRecEC+1
/$SC_$CPU_TST_SB_IncrementMsgRcvEC
ut_tlmwait $SC_$CPU_SB_MsgRecEC {errorcounter} 60
if ($SC_$CPU_SB_MsgRecEC = errorcounter) then
   ut_setrequirements cSB4005e, "P"
   write "<*> Passed (4005e) - MsgRcv Error Counter incremented as expected."
else 
   ut_setrequirements cSB4005e, "F"
   write "<!> Failed (4005e) - MsgRcv Error Counter did not increment."
endif

; Increment $SC_$CPU_SB_NewPipeEC
errorcounter = $SC_$CPU_SB_NewPipeEC+1
/$SC_$CPU_TST_SB_IncrementNewPipeEC
ut_tlmwait $SC_$CPU_SB_NewPipeEC {errorcounter} 30

wait 30

LOCAL errorcounters2[0..5], errorcounternames2[0..5]
;;;; The NoSubEC is never 0 upon reset because there are 5 of these errors
;;;; upon startup. The RESETCTRS command does reset this counter.
;;;;;;;                     $SC_$CPU_SB_NoSubEC,                           ;;
errorcounters2 =     [$SC_$CPU_SB_CMDPC,                             ;;
                     $SC_$CPU_SB_MsgSndEC,   $SC_$CPU_SB_MsgRecEC,  ;;  
                     $SC_$CPU_SB_NewPipeEC,                         ;;
                     $SC_$CPU_SB_PipeOvrEC,  $SC_$CPU_SB_MsgLimEC]
errorcounternames2 = ["$SC_$CPU_SB_CMDPC",                           ;;
                     "$SC_$CPU_SB_MsgSndEC", "$SC_$CPU_SB_MsgRecEC",;;
                     "$SC_$CPU_SB_NewPipeEC",                       ;;
                     "$SC_$CPU_SB_PipeOvrEC","$SC_$CPU_SB_MsgLimEC"]

;; Perform a POWER-On Reset of $CPU
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
  
cfe_startup $CPU
wait 5

;;;for i=0 to 6 do
for i=0 to 5 do
   if ({errorcounternames2[i]} <> 0) then
      write "<!> Failed (4500) - incorrect ", errorcounternames[i], " value."
      write "*** Expected 0 got ", {errorcounternames2[i]}, ". Was ", errorcounters2[i], " before reset command was sent."
      ut_setrequirements cSB4500, "F"
   elseif (errorcounters2[i] = 0) then
      write "<!> Failed (4500) - ", errorcounternames2[i], " was 0 before the reset command was sent."
      ut_setrequirements cSB4500, "F"
   else
      write "<*> Passed (4500) - error counter ", errorcounternames2[i], " as expected. Was ", errorcounters2[i], " before reset command was sent, is now 0."
      ut_setrequirements cSB4500, "P"
   endif
enddo

; Test that the routing information is cleared.
local numFound
numFound=0

start $sc_$cpu_print_sb_pipes("reset", 5)

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
   ut_setrequirements cSB4500, "F"
   write "<!> Failed (4500) - Routing info not cleared. Found ", numFound, " routes."
else
   ut_setrequirements cSB4500, "P"
   write "<*> Passed (4500) - Routing info cleared."
endif

END:

write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

for i=0 to ut_req_array_size do
   ut_pfindicate {requirements[i]} {ut_requirement[i]}
enddo

write ";**********************************************************************"
write ";  End procedure $sc_$cpu_sb_dispipes"
write ";**********************************************************************"

drop ut_req_array_size
drop ut_requirement
ENDPROC
