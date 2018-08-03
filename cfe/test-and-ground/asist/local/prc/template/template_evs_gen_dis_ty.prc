PROC $sc_$cpu_evs_gen_dis_ty
;
;  Purpose: 
;           This is a sub-procedure  
;           To request generation of event messages of every evt msg type:
;
;           One of these event msgs is registered for filtering
;           and the other event msg is not. 
;           
;           These events shall NOT be generated because they are requested
;           upon verification of the conditions set in requirement cEVS3103
;           Which stipulates among other things that the Event Message Types
;             Debug, Inforamtional, Error and Critical must be enabled 
;           for the generation of event messages of the respective types;
;           and this event message generation request is made upon DISABLING
;           (cEVS3107) of all the event message types.
;
;***********************************************************
;
#include "cfe_evs_gen_reqts.h"
;
FOR i = 0 to ut_req_array_size DO
  write "Requirements  [",i,"] = ", ut_requirement[i] 
ENDDO
;
write"  _________"   
write   "        |  Request generation of DEBUG type event messages"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1

/$sc_$cpu_TST_EVS_SendEvtMsg DEBUG EventId = "1" Iters = "1" Milliseconds = "100"
wait 4
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
ut_setupevents $sc, $cpu, TST_EVS, 7, DEBUG, 1

/$sc_$cpu_TST_EVS_SendEvtMsg DEBUG EventId = "7" Iters = "1" Milliseconds = "100"
wait 4
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
;
write"*************************************************************"

write"  _________"
write   "        |  Request generation of INFO type event messages"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, INFO, 1

/$sc_$cpu_TST_EVS_SendEvtMsg INFO  EventId = "1" Iters = "1" Milliseconds = "100"
wait 4
                 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
ut_setupevents $sc, $cpu, TST_EVS, 7, INFO, 1
 
/$sc_$cpu_TST_EVS_SendEvtMsg INFO EventId = "7" Iters = "1" Milliseconds = "100"
wait 4
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
write"*************************************************************"
 
write"  _________"   
write   "        |  Request generation of ERROR type event messages"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, ERROR, 1

/$sc_$cpu_TST_EVS_SendEvtMsg ERROR EventId = "1" Iters = "1" Milliseconds = "100"
wait 4
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
ut_setupevents $sc, $cpu, TST_EVS, 7, ERROR, 1

/$sc_$cpu_TST_EVS_SendEvtMsg ERROR EventId = "7" Iters = "1" Milliseconds = "100"
wait 4
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
write"*************************************************************"
 
write"  _________"
write   "        |  Request generation of CRITICAL type event messages"
 
ut_setupevents $sc, $cpu, TST_EVS, 1, CRIT, 1

/$sc_$cpu_TST_EVS_SendEvtMsg CRIT EventId = "1" Iters = "1" Milliseconds = "100"
wait 4

if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
ut_setupevents $sc, $cpu, TST_EVS, 7, CRIT, 1

/$sc_$cpu_TST_EVS_SendEvtMsg CRIT EventId = "7" Iters = "1" Milliseconds = "100"
wait 4
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3007, "P"
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3007;3103) - Event message generated"
else
  ut_setrequirements cEVS3007, "F"
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3007;3103) - Did not generate expected Event message"
endif             
 
ENDPROC ; $sc_$cpu_evs_gen_dis_ty
