PROC $sc_$cpu_sb_reset
;**********************************************************************
;  Test Name:  sb_reset
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	Performs a Power-on Reset and a Processor Reset. To change the
;       SB values for the Reset tests, a subscription and unsubscription
;       to a CCSDS message is performed.
;
;       This test has a tricky part. A raw command is sent from ASIST or
;       ITOS to the TO application, which should send that packet in
;       telemetry because the raw command is a telemetry packet. In order
;       to make this routing path viable, a new message ID is used and
;       a new subscription is added to the SB routing information.
;
;  Requirements Tested
;     cSB4303	Upon Receipt of a Request to Subscribe to an SB message, the cFE
;		shall establish a route using the Request-specified Message ID,
;		the Request-specified ;Msg-to-Pipe limit and the
;		Request-specified QoS.
;     cSB4303.1	If the Subscription is a duplicate of a previous subscription
;		then the cFE shall issue an Event Message.
;     cSB4310	Upon reciept of Request, the cFE shall free resources allocation
;		for the specified Application.
;     cSB4500	Upon a Power-on Reset, the cFE shall initialize the Routing
;		Information and clear all error counters.
;     cSB4501  	Upon a Processor Reset, the cFE shall initialize the Routing
;		Information and clear all error counters.
;
;  Prerequisite Conditions
;	Open event log and telemetry archive files.
;
;  Assumptions and Constraints
;  (1) The TO application must support cFE Software Bus Subscribe and
;      Unsubscribe commands.
;  (2) TO_TLM_Pipe has a PipeId = 8. If that assumption is not true,
;      the proc may fail to provide correct results.
;  (3) The checksum for the command messages (packets) is not used for
;      cFE because Command Ingest (CI) is not a part of the cFE. We
;      assume that the cFE's CI does not reject commands with bad checksums.
;
;  Change History
;
;	Date		   Name		Description
;	06/13/05	Mike Tong	Original Procedure.
;	01/03/06	Walt Moleski	Modified for cFE B3.1
;	02/12/07	Walt Moleski	Modified for cFE B4.0
;	02/08/12	Walt Moleski	Added variable for ram disk, replaced
;					ut_setupevt with ut_setupevents
;	09/09/14	Walt Moleski	Removed requirements 4311 & 4311.1
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
;                         a specified value. 
;       ut_sendcmd      Send commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_sendrawcmd   Send raw commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_pfindicate   Print the pass fail status of a particular requirement
;                         number.
;       ut_setupevents     Performs setup to verify that a particular event
;                         message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			 requirements array.
;
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_sb_events.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "tst_sb_events.h"
#include "to_lab_events.h"

%liv (log_procedure) = logging

#define SB_4303		0
#define SB_43031	1
#define SB_4310		2
#define SB_4500		3
#define SB_4501		4

global ut_req_array_size = 4
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["SB_4303", "SB_4303.1", "SB_4310", "SB_4500", "SB_4501"]

LOCAL cmd
LOCAL rawcmd
LOCAL wait_time = 10
LOCAL ctr
LOCAL stepnum 
LOCAL outputfilename
LOCAL maxroute = CFE_SB_MAX_MSG_IDS
LOCAL routefound
LOCAL msgid
LOCAL i, j, k
LOCAL no_sub_ctr
LOCAL cmd_ec_ctr
LOCAL cmd_ctr
LOCAL msgrcv_ec_ctr
LOCAL subscr_ec_ctr
LOCAL stream_1, stream_2

LOCAL SB_MsgId[maxroute]
LOCAL SB_PipeId[maxroute]
LOCAL SB_State[maxroute]
LOCAL SB_AppName[maxroute,16]
LOCAL SB_PipeName[maxroute,16]

local ramDir = "RAM:0"
local hostCPU = "$CPU"

write ";*********************************************************************"
write ";  Run initialization procedure [ if necessary ]                      "
write ";*********************************************************************"

PAGE $SC_$CPU_TST_SB_PIPE0

write ";*********************************************************************"
write ";  Step 1:  Power-on Reset test.                                      "
write ";*********************************************************************"
write ";  Step 1.1:  Perform a Power-on Reset.                               "
write ";********************************************************************"
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

write ";*********************************************************************"
write ";  Step 1.2:  Start the Test Application (TST_SB) and add the         "
write ";  required subscriptions to not receive No Subscription events.      "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_SB", TST_SB_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_SB",hostCPU)

ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Test Application Started"
  else
    Write "<!> Failed - Test Application start Event Message not received."
  endif
else
  Write "<!> Failed - Test Application start Event Messages not received."
endif

local pkt_id

;; Need to set the stream based upon the cpu being used
;; CPU1 is the default
stream_1 = x'900'
stream_2 = x'901'
pkt_id = x'101'

;;if ("$CPU" = "CPU2") then
;;  stream_1 = x'A00'
;;  stream_2 = x'A01'
;;  pkt_id = x'201'
;;elseif ("$CPU" = "CPU3") then
;;  stream_1 = x'B00'
;;  stream_2 = x'B01'
;;  pkt_id = x'301'
;;endif

write "Sending command to add subscription for TST_SB HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream_1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write "Sending command to add subscription for TST_SB TLM packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream_2 Pkt_Size=x'0' Priority=x'1' Reliability=x'0' Buflimit=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 1.3:  Save original SB routing file data in order to compare  "
write ";  it later after performing a reset.                                 "
write ";*********************************************************************"
stepnum = "1.3"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

for i=1 to maxroute - 1 do
  SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[i].SB_MsgId
  SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[i].SB_PipeId
  SB_State[i] = $SC_$CPU_SB_RouteEntry[i].SB_State

  for j=1 to 16 do
    SB_AppName[i,j] = $SC_$CPU_SB_RouteEntry[i].SB_AppName[j]
    SB_PipeName[i,j] = $SC_$CPU_SB_RouteEntry[i].SB_PipeName[j]
  enddo
enddo

%liv (log_procedure) = TRUE

write ";*********************************************************************"
write ";  Step 1.4:  Send a raw command. The raw command should be routed to "
write ";  the TO application, which should downlink it as a telemetry packet."
write ";  At this time, the TO application never receives the raw command    "
write ";  because we have not yet added this new subscription to the SB      "
write ";  routing data. Therefore, the raw command causes an error event     "
write ";  message.                                                           "
write ";*********************************************************************"
;; NOTE: For cFE 4.1.0, the DELEVTFLTR command for the CFE_SB_SEND_NO_SUBS_EID
;;       event must be sent in order to receive this event.
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_SEND_NO_SUBS_EID
wait 5

ctr = $SC_$CPU_SB_NoSubEC + 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SEND_NO_SUBS_EID, "INFO", 1

rawcmd = "0F" & %hex($CPU_CMDAPID_BASE + 16, 2) & "C0000035"     ; 6 bytes, primary header.
rawcmd = rawcmd & "000000000000"  ; 6 bytes, secondary header, time.
rawcmd = rawcmd & "00000001"    ; 4 bytes, CmdCounter.
rawcmd = rawcmd & "00000000"    ; 4 bytes, CmdErrCounter.
rawcmd = rawcmd & "00000001"    ; 4 bytes, MsgCounter.
rawcmd = rawcmd & "00000003"    ; 4 bytes, EnaDisFlag.
rawcmd = rawcmd & "0000270E"    ; 4 bytes, PipeDeleted.
rawcmd = rawcmd & "00000003"    ; 4 bytes, ProcessorId.
rawcmd = rawcmd & "00000062"    ; 4 bytes, TimeoutCounter.
rawcmd = rawcmd & "7861832063736420202020202020202020202020" ; 20 bytes, LastSenderId = 'raw cmd'

Ut_sendrawcmd "$SC_$CPU_TST_SB", {rawcmd}

ut_tlmwait  $SC_$CPU_SB_NoSubEC, {ctr}

IF (UT_TW_Status = UT_Success  ) THEN
  write "<*> Passed - The SB No Subscriptions counter incremented. It equals ", ctr, "."
ELSE
  write "<!> Failed - SB No Subscriptions counter did not increment. Expected", ctr, " rcv'd ",$SC_$CPU_SB_NoSubEC
ENDIF

;  Look for the event message
if ($SC_$CPU_find_event[1].num_found_messages >= 1) then
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  Write "<!> Failed - Event Message not received for No Subscription."
endif

write ";*********************************************************************"
write ";  Step 2:  First subscription test. A raw command is sent from       "
write ";  ASIST or ITOS to the TO application, which should send that packet "
write ";  in telemetry because the raw command is a telemetry packet.        "
write ";  In order to make this routing path viable, a new message ID is     "
write ";  used and a new subscription is added to the SB routing information."
write ";*********************************************************************"
write ";  Step 2.1:  Send a Add telemetry packet subscription to TO          "
write ";  downlink command.                                                  "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TO_LAB_APP", TO_ADDPKT_INF_EID, "INFO", 1

/$SC_$CPU_TO_AddPacket Stream=pkt_id Pkt_Size=0 Priority=0 Reliability=0 BufLimit=2
wait 5

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed (4303) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements SB_4303, "P"
else
  write "<!> Failed (4303) - Event Message not received for TO_AddPacket command."
  ut_setrequirements SB_4303, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2:  Remove the subscription for the TST_SB telemetry packet."
write ";  This command is needed in order to receive the packet expected in  "
write ";  the next two steps.                                                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TO_LAB_APP", TO_REMOVEPKT_INF_EID, "INFO", 1

/$SC_$CPU_TO_RemovePacket, Stream=stream_2

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for TO_RemovePacket command."
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3:  Send a raw command.                                     "
write ";  The raw command is received by ASIST or ITOS as a SB test          "
write ";  telemetry packet where TST_SB_PipeDeleted = 9998.                  "
write ";*********************************************************************"
rawcmd = %hex(pkt_id, 4) & "C0000035"     ; 6 bytes, primary header.
rawcmd = rawcmd & "000000000000"  ; 6 bytes, secondary header, time.
rawcmd = rawcmd & "00000001"    ; 4 bytes, CmdCounter.
rawcmd = rawcmd & "00000000"    ; 4 bytes, CmdErrCounter.
rawcmd = rawcmd & "00000001"    ; 4 bytes, MsgCounter.
rawcmd = rawcmd & "00000003"    ; 4 bytes, EnaDisFlag.
rawcmd = rawcmd & "0000270E"    ; 4 bytes, PipeDeleted.
rawcmd = rawcmd & "00000003"    ; 4 bytes, ProcessorId.
rawcmd = rawcmd & "00000062"    ; 4 bytes, TimeoutCounter.
rawcmd = rawcmd & "7861832063736420202020202020202020202020" ; 20 bytes, LastSenderId = 'raw cmd'

Ut_sendrawcmd "$SC_$CPU_TST_SB", {rawcmd}

ut_tlmwait  $SC_$CPU_TST_SB_PipeDeleted, 9998

IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Raw command was downlinked as an SB test telemetry packet."
ELSE
  write "<!> Failed - Raw command was not downlinked as an SB test telemetry packet."
ENDIF

write ";*********************************************************************"
write ";  Step 2.4:  Send a raw command.                                     "
write ";  The raw command is received by ASIST or ITOS as a SB test          "
write ";  telemetry packet where TST_SB_PipeDeleted = 9999.                  "
write ";*********************************************************************"
rawcmd = %hex(pkt_id, 4) & "C0000035"     ; 6 bytes, primary header.
rawcmd = rawcmd & "000000000000"  ; 6 bytes, secondary header, time.
rawcmd = rawcmd & "00000002"    ; 4 bytes, CmdCounter.
rawcmd = rawcmd & "00000000"    ; 4 bytes, CmdErrCounter.
rawcmd = rawcmd & "00000002"    ; 4 bytes, MsgCounter.
rawcmd = rawcmd & "00000003"    ; 4 bytes, EnaDisFlag.
rawcmd = rawcmd & "0000270F"    ; 4 bytes, PipeDeleted.
rawcmd = rawcmd & "00000003"    ; 4 bytes, ProcessorId.
rawcmd = rawcmd & "00000062"    ; 4 bytes, TimeoutCounter.
rawcmd = rawcmd & "7861832063736420202020202020202020202020" ; 20 bytes, LastSenderId = 'raw cmd'

Ut_sendrawcmd "$SC_$CPU_TST_SB", {rawcmd}

ut_tlmwait  $SC_$CPU_TST_SB_PipeDeleted, 9999

IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Raw command was downlinked as an SB test telemetry packet."
ELSE
  write "<!> Failed - Raw command was not downlinked as an SB test telemetry packet."
ENDIF

write ";*********************************************************************"
write ";  Step 2.5:  Add the subscription for the TLM packet from the Test   "
write ";  Application (TST_SB) in order to prevent the No Subscriptions      "
write ";  events from coming in.                                             "
write ";*********************************************************************"
write "Sending command to add subscription for TST_SB TLM packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream_2 Pkt_Size=x'0' Priority=x'0' Reliability=x'0' Buflimit=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 2.6:  Increment the command error counter in order to test if "
write ";  it resets after Processor and Power On resets. Do this by sending "
write ";  an Enable Routing command with a bad pipe ID=x'FE'."
write ";*********************************************************************"
Ut_sendcmd  "$SC_$CPU_SB_EnaRoute MID=x'0890', PID=x'FE'"

if (ut_sc_status = UT_SC_CmdFailure) then
  write "<*> Passed - Command error counter has a non-zero value for the Power-on Reset test."
else
  write "<!> Failed - Command error counter did not increment."
endif

write ";*********************************************************************"
write ";  Step 2.7:  Increment the Message Receive error counter in order to "
write ";  test if it resets after a Processor and Power On reset. Do this by "
write ";  sending a command to the test application.                         "
write ";*********************************************************************"
msgrcv_ec_ctr = $SC_$CPU_SB_MSGRECEC + 1

/$SC_$CPU_TST_SB_IncrementMsgRcvEC

ut_tlmwait  $SC_$CPU_SB_MSGRECEC, {msgrcv_ec_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Receive error counter was incremented."
ELSE
  write "<!> Failed - Message Receive error counter did not increment."
ENDIF

wait 5

write ";*********************************************************************"
write ";  Step 2.8:  Increment the Subscribe error counter in order to "
write ";  test if it resets after a Processor and Power On reset. Do this by "
write ";  sending a command to the test application.                         "
write ";*********************************************************************"
subscr_ec_ctr = $SC_$CPU_SB_SUBSCREC + 1

/$SC_$CPU_TST_SB_IncrementSubsEC

ut_tlmwait  $SC_$CPU_SB_SUBSCREC, {subscr_ec_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Subscribe error counter was incremented."
ELSE
  write "<!> Failed - Subscribe error counter did not increment."
ENDIF

wait 5

write ";*********************************************************************"
write ";  Step 3: Processor Reset test. The Processor Reset should delete "
write ";  	  the new TO subscription.                                    "
write ";*********************************************************************"
cmd_ctr = $SC_$CPU_SB_CMDPC
cmd_ec_ctr = $SC_$CPU_SB_CMDEC
no_sub_ctr = $SC_$CPU_SB_NoSubEC 
msgrcv_ec_ctr = $SC_$CPU_SB_MSGRECEC
subscr_ec_ctr = $SC_$CPU_SB_SUBSCREC

write "CMDPC    counter BEFORE Processor Reset = ", $SC_$CPU_SB_CMDPC
write "CMDEC    counter BEFORE Processor Reset = ", $SC_$CPU_SB_CMDEC
write "NoSub    counter BEFORE Processor Reset = ", $SC_$CPU_SB_NoSubEC
write "MsgRcvEC counter BEFORE Processor Reset = ", $SC_$CPU_SB_MSGRECEC
write "SubscrEC counter BEFORE Processor Reset = ", $SC_$CPU_SB_SUBSCREC

; Check to see if all counters are non-zero
if (cmd_ctr > 0) AND (cmd_ec_ctr > 0) AND (msgrcv_ec_ctr > 0) AND (subscr_ec_ctr > 0) then
  write "<*> Passed (4501) - All counters are non-zero."
  ut_setrequirements SB_4501, "P"
ELSE
  write "<!> Failed (4501) - One of the counters is zero. Failing requirement."
  ut_setrequirements SB_4501, "F"
endif

write ";*********************************************************************"
write ";  Step 3.1:  Perform a Processor Reset.                              "
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
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

write ";*********************************************************************"
write ";  Step 3.2:  Verify the Error Counters did not reset.                "
write ";*********************************************************************"
write "CMDPC    counter AFTER Processor Reset = ", $SC_$CPU_SB_CMDPC
write "CMDEC    counter AFTER Processor Reset = ", $SC_$CPU_SB_CMDEC
write "NoSub    counter AFTER Processor Reset = ", $SC_$CPU_SB_NoSubEC
write "MsgRcvEC counter AFTER Processor Reset = ", $SC_$CPU_SB_MSGRECEC
write "SubscrEC counter AFTER Processor Reset = ", $SC_$CPU_SB_SUBSCREC

IF ($SC_$CPU_SB_CMDPC = 0) THEN
  write "<*> Passed (4501) - CMDPC counter Reset after a Processor Reset."
  ut_setrequirements SB_4501, "P"
ELSE
  write "<!> Failed (4501) - CMDPC counter was not Reset after a Processor Reset."
  ut_setrequirements SB_4501, "F"
ENDIF

IF ($SC_$CPU_SB_CMDEC = 0) THEN
  write "<*> Passed (4501) - CMDEC counter Reset after a Processor Reset."
  ut_setrequirements SB_4501, "P"
ELSE
  write "<!> Failed (4501) - CMDEC counter was not Reset after a Processor Reset."
  ut_setrequirements SB_4501, "F"
ENDIF

IF ($SC_$CPU_SB_NoSubEC >= 5) THEN
  write "<*> Passed (4501) - NoSubEC counter Reset after a Processor Reset."
  ut_setrequirements SB_4501, "P"
ELSE
    write "<!> Failed (4501) - NoSubEC counter was not Reset after a Processor Reset."
    ut_setrequirements SB_4501, "F"
ENDIF

IF ($SC_$CPU_SB_MSGRECEC = 0) THEN
  write "<*> Passed (4501) - MsgRcv Error counter Reset after a Processor Reset."
  ut_setrequirements SB_4501, "P"
ELSE
    write "<!> Failed (4501) - MsgRcv Error counter was not Reset after a Processor Reset."
    ut_setrequirements SB_4501, "F"
ENDIF

IF ($SC_$CPU_SB_SUBSCREC = 0) THEN
  write "<*> Passed (4501) - Subscribe Error counter Reset after a Processor Reset."
  ut_setrequirements SB_4501, "P"
ELSE
    write "<!> Failed (4501) - Subscribe Error counter was not Reset after a Processor Reset."
    ut_setrequirements SB_4501, "F"
ENDIF

write ";*********************************************************************"
write ";  Step 3.3:  Verify that the new subscription was deleted.           "
write ";  Compare shows that the files from the two Power on resets          "
write ";  are the same.                                                      "
write ";*********************************************************************"
stepnum = "3.3"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

;;msgid = x'0F00' + $CPU_CMDAPID_BASE + 16
msgid = pkt_id
routefound = 0
for i=1 to maxroute - 1 do

  if (SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[i].SB_MsgId AND  ;;
      SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[i].SB_PipeId AND  ;;
      SB_State[i] = $SC_$CPU_SB_RouteEntry[i].SB_State) then

    for j=1 to 16 do
      if (SB_AppName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_AppName[j] OR  ;;
          SB_PipeName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_PipeName[j]) then

        write "<!> Step ", stepnum, " Name failed at index =", i
      endif
    enddo

  else
    if ($SC_$CPU_SB_RouteEntry[i].SB_MsgId = msgid) then
      write "<!> Route found, which is bad. Pipe Id = ", $SC_$CPU_SB_RouteEntry[i].SB_PipeId
      routefound = routefound + 1
    else
      write "MsgID at Entry[",i,"] = ", $SC_$CPU_SB_RouteEntry[i].SB_MsgId
;      write "<!> Step ", stepnum, " failed at index =", i
    endif
  endif
enddo

%liv (log_procedure) = TRUE

NEXT32:
IF (routefound = 0) THEN
  write "<*> Passed (4501) - New subscription was not preserved after Processor Reset, which is good."
  write "Compare shows that the files from the two Power on resets are the same."
  ut_setrequirements SB_4501, "P"
else
  write "<!> Failed (4501) - Step ", stepnum, " failed. New subscription was preserved after Processor Reset."
  ut_setrequirements SB_4501, "F"
endif

WRITE ";**********************************************************************"
WRITE ";  Step 3.4 Sending command to Start the Test Application (TST_SB) and "
WRITE ";  add the required subscriptions to not receive No Subscription events."
WRITE ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_SB", TST_SB_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_SB",hostCPU)

; Wait for app startup events
;wait 10

ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Test Application Started"
  else
    Write "<!> Failed - Test Application start Event Message not received."
  endif
else
  Write "<!> Failed - Test Application start Event Messages not received."
endif

write "Sending command to add subscription for TST_SB HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream_1 Pkt_Size=x'0' Priority=x'0' Reliability=x'0' Buflimit=x'4'
wait 10

write "Sending command to add subscription for TST_SB TLM packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream_2 Pkt_Size=x'0' Priority=x'0' Reliability=x'0' Buflimit=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 3.5:  Send a raw command. The raw command should be routed to "
write ";  the TO application, which should downlink it as a telemetry packet."
write ";  At this time, the TO application never receives the raw command    "
write ";  because we have not yet added this new subscription to the SB      "
write ";  routing data. Therefore, the raw command causes an error event     "
write ";  message.                                                           "
write ";*********************************************************************"
;; NOTE: For cFE 4.1.0, the DELEVTFLTR command for the CFE_SB_SEND_NO_SUBS_EID
;;       event must be sent in order to receive this event.
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_SEND_NO_SUBS_EID
wait 5

ctr = $SC_$CPU_SB_NoSubEC + 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SEND_NO_SUBS_EID, "INFO", 1

rawcmd = %hex(pkt_id, 4) & "C0000035"     ; 6 bytes, primary header.
rawcmd = rawcmd & "000000000000"  ; 6 bytes, secondary header, time.
rawcmd = rawcmd & "00000005"    ; 4 bytes, CmdCounter.
rawcmd = rawcmd & "00000000"    ; 4 bytes, CmdErrCounter.
rawcmd = rawcmd & "00000005"    ; 4 bytes, MsgCounter.
rawcmd = rawcmd & "00000003"    ; 4 bytes, EnaDisFlag.
rawcmd = rawcmd & "0000270F"    ; 4 bytes, PipeDeleted.
rawcmd = rawcmd & "00000003"    ; 4 bytes, ProcessorId.
rawcmd = rawcmd & "00000062"    ; 4 bytes, TimeoutCounter.
rawcmd = rawcmd & "7861832063736420202020202020202020202020" ; 20 bytes, LastSenderId = 'raw cmd'

Ut_sendrawcmd "$SC_$CPU_TST_SB", {rawcmd}
wait 10

if ($SC_$CPU_SB_NoSubEC >= ctr) THEN
  write "<*> Passed (4501) - The SB No Subscriptions counter incremented."
  ut_setrequirements SB_4501, "P"
ELSE
  write "<!> Failed (4501) - SB No Subscriptions counter did not increment."
  ut_setrequirements SB_4501, "F"
ENDIF

;  Look for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  Write "<*> Passed (4501) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  ut_setrequirements SB_4501, "P"
else
  Write "<!> Failed (4501) - Event Message not received for No Subscription."
   ut_setrequirements SB_4501, "F"
endif

write ";*********************************************************************"
write ";  Step 4.0:  Second subscription test. A raw command is sent from      "
write ";  ASIST or ITOS to the TO application, which should send that packet "
write ";  in telemetry because the raw command is a telemetry packet.        "
write ";  In order to make this routing path viable, a new message ID is     "
write ";  used and a new subscription is added to the SB routing information."
write ";*********************************************************************"
write ";  Step 4.1:  Send a Add telemetry packet subscription to TO         "
write ";  downlink command.                                                "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TO_LAB_APP", TO_ADDPKT_INF_EID, "INFO", 1

/$SC_$CPU_TO_AddPacket Stream=pkt_id Pkt_Size=0 Priority=0 Reliability=0 BufLimit=2
wait wait_time

;  Look for the event message
if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for TO_AddPacket command."
endif

WAIT 10

write ";*********************************************************************"
write ";  Step 4.2:  Verify that the new subscription was added.             "
write ";*********************************************************************"
stepnum = "4.2"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

;msgid = x'0F00' + $CPU_CMDAPID_BASE + 16
msgid = pkt_id
routefound = 0
k = 0
for i=1 to maxroute - 1 do

  k = k + 1
  if (SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[k].SB_MsgId AND  ;;
      SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[k].SB_PipeId AND  ;;
      SB_State[i] = $SC_$CPU_SB_RouteEntry[k].SB_State) then

    for j=1 to 16 do
      if (SB_AppName[i,j] <> $SC_$CPU_SB_RouteEntry[k].SB_AppName[j] OR  ;;
          SB_PipeName[i,j] <> $SC_$CPU_SB_RouteEntry[k].SB_PipeName[j]) then

        write "<!> Step ", stepnum, " Name failed at index =", k
      endif
    enddo

  else
    if ($SC_$CPU_SB_RouteEntry[k].SB_MsgId = msgid) then
      write "Route found, which is good. Pipe Id = ", $SC_$CPU_SB_RouteEntry[k].SB_PipeId
      routefound = routefound + 1
    else
      write "Entry[",k,"] = ", $SC_$CPU_SB_RouteEntry[k].SB_MsgId
;;      write "<!> Step ", stepnum, " failed at index =", k
;;      goto NEXT42
    endif
  endif
enddo

NEXT42:
IF (routefound = 1) THEN
  write "<*> Passed - Files are the same except one new routing was added."
ELSE
  write "<!> Failed - Step ", stepnum, " failed. Number of routes found = ", routefound
ENDIF

%liv (log_procedure) = TRUE

write ";*********************************************************************"
write ";  Step 4.3:  Send again the ""Add telemetry packet subscription to    "
write ";  TO downlink"" command. SB will respond with a ""Duplicate           "
write ";  Subscription"" error event message. "
write ";*********************************************************************"
;; NOTE: For cFE 4.1.0, the DELEVTFLTR command for the CFE_SB_DUP_SUBSCRIP_EID
;;       event must be sent in order to receive this event.
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_DUP_SUBSCRIP_EID
wait 5

ut_setupevents "$SC", "$CPU", "TO_LAB_APP", TO_ADDPKT_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_DUP_SUBSCRIP_EID, "INFO", 2

local dupCnt = $SC_$CPU_SB_DUPSUBCNT+1

;;cmd = "$SC_$CPU_TO_AddPacket, Stream=" &  ;;
;;            "x'0F" & %hex($CPU_CMDAPID_BASE + 16, 2) & "', " &  ;;
;;            "Pkt_Size=0, Priority=0, Reliability=0, "        &  ;;
;;            "BufLimit=2"
;;
;;ut_sendcmd {cmd}

/$SC_$CPU_TO_AddPacket Stream=pkt_id Pkt_Size=0 Priority=0 Reliability=0 BufLimit=2
wait wait_time

;;if (UT_SC_Status = UT_SC_Success) then
  ;; Look for the event message
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (4303) - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
    ut_setrequirements SB_4303, "P"
    ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
    IF (UT_TW_Status = UT_Success) THEN
      write "<*> Passed (4303.1) - SB Duplicate Subscription event was received."
      ut_setrequirements SB_43031, "P"
    ELSE
      write "<!> Failed (4303.1) - SB Duplicate Subscription event not received."
      ut_setrequirements SB_43031, "F"
    ENDIF

    if ($SC_$CPU_SB_DUPSUBCNT = dupCnt) then
      write "<*> Passed (4303.1) - SB Duplicate Subscription count incremented."
      ut_setrequirements SB_43031, "P"
    else
      write "<!> Failed (4303.1) - SB Duplicate Subscription counter did not increment."
      ut_setrequirements SB_43031, "F"
    endif
  else
    write "<!> Failed (4303) - TO_AddPacket event not received."
    ut_setrequirements SB_4303, "F"
  endif
;;else
;;  write "<!> Failed (4303) - Error sending the TO_AddPacket command."
;;  ut_setrequirements SB_4303, "F"
;;endif

;WAIT 10

write ";*********************************************************************"
write ";  Step 4.4:  Verify that the Duplicate Subscription was not added.   "
write ";*********************************************************************"
stepnum = "4.4"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

;;msgid = x'0F00' + $CPU_CMDAPID_BASE + 16
msgid = pkt_id
routefound = 0
k = 0
for i=1 to maxroute - 1 do

  k = k + 1
  if (SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[k].SB_MsgId AND  ;;
      SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[k].SB_PipeId AND  ;;
      SB_State[i] = $SC_$CPU_SB_RouteEntry[k].SB_State) then

    for j=1 to 16 do
      if (SB_AppName[i,j] <> $SC_$CPU_SB_RouteEntry[k].SB_AppName[j] OR  ;;
          SB_PipeName[i,j] <> $SC_$CPU_SB_RouteEntry[k].SB_PipeName[j]) then

        write "<!> Step ", stepnum, " Name failed at index =", k
      endif
    enddo

  else

    if ($SC_$CPU_SB_RouteEntry[k].SB_MsgId = msgid) then
      write "Route found, which is good. Pipe Id = ", $SC_$CPU_SB_RouteEntry[k].SB_PipeId
      routefound = routefound + 1
    else
      write "Entry[",k,"] = ", $SC_$CPU_SB_RouteEntry[k].SB_MsgId
;      write "<!> Step ", stepnum, " failed at index =", k
;      goto NEXT44
    endif

  endif
enddo

NEXT44:
IF (routefound = 1) THEN
  write "<*> Passed - Files are the same except for one new routing, which is good."
ELSE
  write "<!> Failed - The Duplicate subscription caused a problem. Number of routes found = ", routefound
ENDIF

%liv (log_procedure) = TRUE

write ";*********************************************************************"
write ";  Step 5.0:  Unsubscription test. This test is to verify that the      "
write ";  expected results occur. The requirements for these steps are tested"
write ";  in other test procedures, however, these steps perform a bit of    "
write ";  cleanup for the SB_Reset test procedure.                           "
write ";*********************************************************************"
write ";  Step 5.1:  Send a ""Delete telemetry packet subscription from TO   "
write ";  downlink"" command. This step is performed to try to break the fsw."
write ";  There is no requirement tested by this step other "
write ";  than to execute the command and receive the expected messages.      "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TO_LAB_APP", TO_REMOVEPKT_INF_EID, "INFO", 1

/$SC_$CPU_TO_RemovePacket, Stream=pkt_id

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for TO_RemovePacket command."
endif

WAIT 10

write ";*********************************************************************"
write ";  Step 5.2:  Verify that the new subscription was deleted.           "
write ";*********************************************************************"
stepnum = "5.2"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

;;msgid = x'0F00' + $CPU_CMDAPID_BASE + 16
msgid = pkt_id
routefound = 0
for i=1 to maxroute - 1 do

  if (SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[i].SB_MsgId AND  ;;
      SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[i].SB_PipeId AND  ;;
      SB_State[i] = $SC_$CPU_SB_RouteEntry[i].SB_State) then

    for j=1 to 16 do
      if (SB_AppName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_AppName[j] OR  ;;
          SB_PipeName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_PipeName[j]) then

        write "<!> Step ", stepnum, " Name failed at index =", i
      endif
    enddo

  else

    if ($SC_$CPU_SB_RouteEntry[k].SB_MsgId = msgid) then
      write "<!> Failed - Route found, which is bad. Pipe Id = ", $SC_$CPU_SB_RouteEntry[k].SB_PipeId
      routefound = routefound + 1
    else
      write "Entry[",k,"] = ", $SC_$CPU_SB_RouteEntry[k].SB_MsgId
;      write "<!> Step ", stepnum, " failed at index =", i
;      goto NEXT52
    endif

  endif
enddo

NEXT52:
IF (routefound = 0) THEN
  write "<*> Passed - New subscription was deleted."
  write "Compare shows that the files from the Processor reset and the "
  write "unsubscription are the same."
ELSE
  write "<!> Failed - Step ", stepnum, " failed. Subscription was not deleted."
endif

%liv (log_procedure) = TRUE

write ";*********************************************************************"
write ";  Step 5.3:  Send again the ""Delete telemetry packet subscription   "
write ";  from TO downlink"" command.                                        "
write ";  This step is performed to try to break the fsw."
write ";  There is no requirement tested by this step other "
write ";  than to execute the command and receive the expected messages.      "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TO_LAB_APP", TO_REMOVEPKT_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_UNSUB_NO_SUBS_EID, "INFO", 2

/$SC_$CPU_TO_RemovePacket, Stream=pkt_id

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1

IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for TO_RemovePacket command."
endif

;;;;;;;;;;;;;;; EID 12 = CFE_SB_UNSUB_NO_SUBS_EID,;;;;;;;;;;
;;ut_tlmwait $SC_$CPU_find_event[2].eventid, 12
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1

IF (UT_TW_Status = UT_Success) THEN
  write "<*> Passed - Received SB Unsubscribe error event = 12."
ELSE
  write "<!> Failed - Did not receive SB Unsubscribe error event = 12."
ENDIF

write ";*********************************************************************"
write ";  Step 5.4:  Verify that the new subscription is still deleted.      "
write ";*********************************************************************"
stepnum = "5.4"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

;;msgid = x'0F00' + $CPU_CMDAPID_BASE + 16
msgid = pkt_id
routefound = 0
for i=1 to maxroute - 1 do

  if (SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[i].SB_MsgId AND  ;;
      SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[i].SB_PipeId AND  ;;
      SB_State[i] = $SC_$CPU_SB_RouteEntry[i].SB_State) then

    for j=1 to 16 do
      if (SB_AppName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_AppName[j] OR  ;;
          SB_PipeName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_PipeName[j]) then

        write "<!> Step ", stepnum, " Name failed at index = ", i
        goto NEXT54
      endif
    enddo

  else

    if ($SC_$CPU_SB_RouteEntry[k].SB_MsgId = msgid) then
      write "<!> Failed - Route found, which is bad. Pipe Id = ", $SC_$CPU_SB_RouteEntry[k].SB_PipeId
      routefound = routefound + 1
    else
      write "Entry[",k,"] = ", $SC_$CPU_SB_RouteEntry[k].SB_MsgId
;      goto NEXT54
    endif

  endif
enddo

NEXT54:
IF (routefound = 0) THEN
  write "<*> Passed - New subscription is still deleted."
  write "Compare shows that the files from the Processor reset and the "
  write "unsubscription are the same."
ELSE
  write "<!> Failed - Step ", stepnum, " failed. Subscription was not deleted."
endif
%liv (log_procedure) = TRUE

write ";*********************************************************************"
write ";  Step 5.5:  Send a raw command. As stated earlier, the raw command  "
write ";  should be routed to the TO application, which should downlink it   "
write ";  as a telemetry packet. However, because this routing subscription  "
write ";  has been deleted by the ""Delete telemetry packet subscription from"
write ";  TO downlink"" command, the TO application never receives the raw   "
write ";  command. Therefore, the raw command causes an error event message  "
write ";*********************************************************************"
;; NOTE: For cFE 4.1.0, the DELEVTFLTR command for the CFE_SB_SEND_NO_SUBS_EID
;;       event must be sent in order to receive this event.
/$SC_$CPU_EVS_DELEVTFLTR APPLICATION="CFE_SB" EVENT_ID=CFE_SB_SEND_NO_SUBS_EID
wait 5

ctr = $SC_$CPU_SB_NoSubEC + 1
ut_setupevents "$SC", "$CPU", "CFE_SB", CFE_SB_SEND_NO_SUBS_EID, "INFO", 1

rawcmd = %hex(pkt_id, 4) & "C0000035"     ; 6 bytes, primary header.
rawcmd = rawcmd & "000000000000"  ; 6 bytes, secondary header, time.
rawcmd = rawcmd & "0000000A"    ; 4 bytes, CmdCounter.
rawcmd = rawcmd & "00000000"    ; 4 bytes, CmdErrCounter.
rawcmd = rawcmd & "0000000A"    ; 4 bytes, MsgCounter.
rawcmd = rawcmd & "00000003"    ; 4 bytes, EnaDisFlag.
rawcmd = rawcmd & "0000270F"    ; 4 bytes, PipeDeleted.
rawcmd = rawcmd & "00000003"    ; 4 bytes, ProcessorId.
rawcmd = rawcmd & "00000062"    ; 4 bytes, TimeoutCounter.
rawcmd = rawcmd & "7861832063736420202020202020202020202020" ; 20 bytes, LastSenderId = 'raw cmd'

Ut_sendrawcmd "$SC_$CPU_TST_SB", {rawcmd}

ut_tlmwait  $SC_$CPU_SB_NoSubEC, {ctr}

IF (UT_TW_Status = UT_Success  ) THEN
  write "<*> Passed - The SB No Subscriptions counter incremented. It equals ", ctr, "."
ELSE
  write "<!> Failed - SB No Subscriptions counter did not increment."
ENDIF

;  Look for the event message
if ($SC_$CPU_find_event[1].num_found_messages > 0) then
  Write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  Write "<!> Failed - Event Message not received for No Subscription."
endif

write ";*********************************************************************"
write ";  Step 5.6:  Increment the command error counter in order to test if "
write ";  it resets after the Power On reset. Do this by sending "
write ";  an Enable Routing command with a bad pipe ID=x'FE'."
write ";*********************************************************************"
Ut_sendcmd  "$SC_$CPU_SB_EnaRoute MID=x'0890', PID=x'FE'"

if (ut_sc_status = UT_SC_CmdFailure) then
  write "<*> Passed - Command error counter has a non-zero value for the Power-on Reset test."
else
  write "<!> Failed - Command error counter did not increment."
endif

write ";*********************************************************************"
write ";  Step 5.7:  Increment the Message Receive error counter in order to "
write ";  test if it resets after a Power On reset. Do this by "
write ";  sending a command to the test application.                         "
write ";*********************************************************************"
msgrcv_ec_ctr = $SC_$CPU_SB_MSGRECEC + 1

/$SC_$CPU_TST_SB_IncrementMsgRcvEC

ut_tlmwait  $SC_$CPU_SB_MSGRECEC, {msgrcv_ec_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Message Receive error counter was incremented."
ELSE
  write "<!> Failed - Message Receive error counter did not increment."
ENDIF

wait 5

write ";*********************************************************************"
write ";  Step 5.8:  Increment the Subscribe error counter in order to "
write ";  test if it resets after a and Power On reset. Do this by "
write ";  sending a command to the test application.                         "
write ";*********************************************************************"
subscr_ec_ctr = $SC_$CPU_SB_SUBSCREC + 1

/$SC_$CPU_TST_SB_IncrementSubsEC

ut_tlmwait  $SC_$CPU_SB_SUBSCREC, {subscr_ec_ctr} 

if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Subscribe error counter was incremented."
ELSE
  write "<!> Failed - Subscribe error counter did not increment."
ENDIF

wait 5

write ";*********************************************************************"
write ";  Step 6.0:  Power-on Reset test. The Power-on Reset should delete     "
write ";  the previously added new TO subscription and reset the error ctrs. "
write ";*********************************************************************"
cmd_ctr = $SC_$CPU_SB_CMDPC
cmd_ec_ctr = $SC_$CPU_SB_CMDEC
;;no_sub_ctr = $SC_$CPU_SB_NoSubEC 
msgrcv_ec_ctr = $SC_$CPU_SB_MSGRECEC
subscr_ec_ctr = $SC_$CPU_SB_SUBSCREC

write "CMDPC    counter BEFORE Power-On Reset = ", $SC_$CPU_SB_CMDPC
write "CMDEC    counter BEFORE Power-On Reset = ", $SC_$CPU_SB_CMDEC
write "NoSub    counter BEFORE Power-On Reset = ", $SC_$CPU_SB_NoSubEC
write "MsgRcvEC counter BEFORE Power-On Reset = ", $SC_$CPU_SB_MSGRECEC
write "SubscrEC counter BEFORE Power-On Reset = ", $SC_$CPU_SB_SUBSCREC

; Check to see if all counters are non-zero
if (cmd_ctr > 0) AND (cmd_ec_ctr > 0) AND (msgrcv_ec_ctr > 0) AND (subscr_ec_ctr > 0) then
  write "<*> Passed (4500) - All counters are non-zero."
  ut_setrequirements SB_4500, "P"
ELSE
  write "<!> Failed (4500) - One of the counters is zero. Failing requirements."
  ut_setrequirements SB_4500, "F"
endif

write ";*********************************************************************"
write ";  Step 6.1:  Perform a Power-on Reset.                               "
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

write ";*********************************************************************"
write ";  Step 6.2:  Verify that the error counters are zero due to the      "
write ";  Power-on reset.                                                    "
write ";*********************************************************************"
write "CMDPC    counter AFTER Power-On Reset = ", $SC_$CPU_SB_CMDPC
write "CMDEC    counter AFTER Power-On Reset = ", $SC_$CPU_SB_CMDEC
write "NoSub    counter AFTER Power-On Reset = ", $SC_$CPU_SB_NoSubEC
write "MsgRcvEC counter AFTER Power-On Reset = ", $SC_$CPU_SB_MSGRECEC
write "SubscrEC counter AFTER Power-On Reset = ", $SC_$CPU_SB_SUBSCREC

if ($SC_$CPU_SB_CMDPC = 0) then
  write "<*> Passed (4500) - Command counter was reset to zero."
  ut_setrequirements SB_4500, "P"
ELSE
  write "<!> Failed (4500) - Command counter was not reset to zero."
  ut_setrequirements SB_4500, "F"
ENDIF

if ($SC_$CPU_SB_CMDEC = 0) then
  write "<*> Passed (4500) - Command error counter was reset to zero."
  ut_setrequirements SB_4500, "P"
ELSE
  write "<!> Failed (4500) - Command error counter was not reset to zero."
  ut_setrequirements SB_4500, "F"
ENDIF

if ($SC_$CPU_SB_MSGRECEC = 0) then
  write "<*> Passed (4500) - MsgRcv error counter was reset to zero."
  ut_setrequirements SB_4500, "P"
ELSE
  write "<!> Failed (4500) - MsgRcv error counter was not reset to zero."
  ut_setrequirements SB_4500, "F"
ENDIF

if ($SC_$CPU_SB_SUBSCREC = 0) then
  write "<*> Passed (4500) - Subscribe error counter was reset to zero."
  ut_setrequirements SB_4500, "P"
ELSE
  write "<!> Failed (4500) - Subscribe error counter was not reset to zero."
  ut_setrequirements SB_4500, "F"
ENDIF

IF ($SC_$CPU_SB_NoSubEC >= 5) THEN
  write "<*> Passed (4500) - NoSubEC counter was Reset after a Power-On Reset."
  ut_setrequirements SB_4500, "P"
ELSE
  write "<!> Failed (4500) - NoSubEC counter was not Reset after a Power-On Reset."
  ut_setrequirements SB_4500, "F"
ENDIF

write ";*********************************************************************"
write ";  Step 6.3:  Verify that the new subscription was deleted.           "
write ";  Compare shows that the files from the two Power on resets          "
write ";  are the same.                                                      "
write ";*********************************************************************"
stepnum = "6.3"
outputfilename = "$sc_$cpu_sb_reset_" & stepnum & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

;;msgid = x'0F00' + $CPU_CMDAPID_BASE + 16
msgid = pkt_id
routefound = 0
for i=1 to maxroute - 1 do

  if (SB_MsgId[i] = $SC_$CPU_SB_RouteEntry[i].SB_MsgId AND  ;;
      SB_PipeId[i] = $SC_$CPU_SB_RouteEntry[i].SB_PipeId AND  ;;
      SB_State[i] = $SC_$CPU_SB_RouteEntry[i].SB_State) then

    for j=1 to 16 do
      if (SB_AppName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_AppName[j] OR  ;;
          SB_PipeName[i,j] <> $SC_$CPU_SB_RouteEntry[i].SB_PipeName[j]) then

        write "<!> Step ", stepnum, " Name failed at index =", i
      endif
    enddo

  else
    if ($SC_$CPU_SB_RouteEntry[i].SB_MsgId = msgid) then
      write "<!> Route found, which is bad. Pipe Id = ", $SC_$CPU_SB_RouteEntry[i].SB_PipeId
      routefound = routefound + 1
    else
      write "MsgID at Entry[",i,"] = ", $SC_$CPU_SB_RouteEntry[i].SB_MsgId
;      write "<!> Step ", stepnum, " failed at index =", i
    endif
  endif
enddo

%liv (log_procedure) = TRUE

IF (routefound = 0) THEN
  write "<*> Passed (4500) - New subscription was not preserved after Power-on Reset, which is good."
  write "Compare shows that the files from the two Power on resets are the same."
  ut_setrequirements SB_4500, "P"
else
  write "<!> Failed (4500) - Step ", stepnum, " failed. New subscription was preserved after Power-on Reset."
  ut_setrequirements SB_4500, "F"
endif

write ";*********************************************************************"
write "; Step 7.0: Free Resources Test"
write ";*********************************************************************"
write "; Step 7.1: Startup the TST_ES application in order to use the command"
write "; 	   to delete an application."
write ";*********************************************************************"
;; Start the TST_ES app
write "; Starting the TST_ES application. "
s load_start_app ("TST_ES", hostCPU)
wait 10
                                                                                
;; Add an Event Filter for the TST_ES HK Request Event in order to
;; only receive this event 1 time rather than every 4 seconds
/$SC_$CPU_EVS_ADDEVTFLTR Application="TST_ES" Event_ID=39 Event_Mask=X'ffff'
                                                                                
write ";*********************************************************************"
write "; Step 7.2: Startup the TST_SB applications in order to "
write "; 	   delete TST_SB using the TST_ES application."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_SB", TST_SB_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_SB",hostCPU)

ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    Write "<*> Passed - Test Application Started"
  else
    Write "<!> Failed - Test Application start Event Message not received."
  endif
else
  Write "<!> Failed - Test Application start Event Messages not received."
endif

;  Dump the running apps
start get_file_to_cvt (ramDir, "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", hostCPU)
                                                                                
local tst_sbIndex = 0, app_info_file_index
;Loop thru the table looking for the TST_SB app
for app_info_file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_SB") then
    tst_sbIndex = app_info_file_index
  endif
enddo

if (tst_sbIndex > 0) then
  /$SC_$CPU_TST_ES_DELETEAPP APPID=$SC_$CPU_ES_ALE[tst_sbIndex].ES_AL_APPID
  wait 30
else
  write "-- The TST_SB application was not running and could not be deleted!!"
endif

;; Set the requirement to 'A' since the resource deletion messages are contained
;; in the UART window
ut_setrequirements SB_4310, "A"

write ";*********************************************************************"
write ";  Step 8.0:  Perform a Power-on Reset to clean-up from this test."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write "**** Requirements Status Reporting"
                                                                                
write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"
                                                                                
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

CLEAR $SC_$CPU_TST_SB_PIPE0

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sb_reset                                    "
write ";*********************************************************************"
ENDPROC
