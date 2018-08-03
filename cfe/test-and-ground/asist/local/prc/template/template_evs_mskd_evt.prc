PROC $sc_$cpu_evs_mskd_evt
;
;  Purpose: 
;           This is a sub-procedure  
;           To request generation of event messages of every evt msg type:
;           One event msg is registered for filtering which shall not be
;           generated (per cEVS3103 specification) since its mask is set to FFFF (always filter)
;           The other event msg is not registered for filtering and shall be 
;           generated.
;
;           Requirement cEVS3103 
;           Upon receipt of a Request to send an Event Message, the cFE shall
;           create a Short or Long Event Message, as specified by the SB Event
;           Format Mode, ONLY if the following cFE conditions are met:
;           The requesting Application's Event Service Enable Status is Enabled.
;           The requesting Application's registered message filtering algorithm 
;           indicates the message should be sent.
;           The requesting Application's Event Type Enable Status is Enabled 
;           for the Event Type of the request-specified Event Message. 
;
; *****************************************************************************
 
#include "cfe_evs_gen_reqts.h"
 
FOR i = 0 to ut_req_array_size DO
  write "Requirement at [",i,"] = ", ut_requirement[i] 
ENDDO 
 
write "***********************************************************************"
write "  _________"   
write "           |  Request generation of DEBUG type event messages"
write "           | "
 
ut_setupevents $sc, $cpu, TST_EVS, 1, DEBUG, 1

/$sc_$cpu_TST_EVS_SendEvtMsg DEBUG EventId="1" Iters="5" Milliseconds="100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - DEBUG Evt Msg ID 1 was not generated as expected. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[1].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Events were generated when not expected."
endif
 
ut_setupevents $sc, $cpu, TST_EVS, 7, DEBUG, 1

/$sc_$cpu_TST_EVS_SendEvtMsg DEBUG EventId="7" Iters="5" Milliseconds="100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 5) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - DEBUG Evt Msg ID 7 events were generated. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[7].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - The expected events were not generated."
endif
 
write "************************************************************************"
write "  _________"
write "           |  Request generation of INFO type event messages"
write "           | "
 
ut_setupevents $sc, $cpu, TST_EVS, 1, INFO, 1

/$sc_$cpu_TST_EVS_SendEvtMsg INFO EventId="1" Iters="5" Milliseconds="100"
wait 2

if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - INFO Evt Msg ID 1 was not generated as expected. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[1].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Events were generated when not expected."
endif
 
ut_setupevents $sc, $cpu, TST_EVS, 7, INFO, 1
 
/$sc_$cpu_TST_EVS_SendEvtMsg INFO EventId="7" Iters="5" Milliseconds="100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 5) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - INFO Evt Msg ID 7 events were generated. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[7].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - The expected events were not generated."
endif
 
write "************************************************************************"
write "  _________"   
write "           |  Request generation of ERROR type event messages"
write "           | "
 
ut_setupevents $sc, $cpu, TST_EVS, 1, ERROR, 1

/$sc_$cpu_TST_EVS_SendEvtMsg ERROR EventId="1" Iters="5" Milliseconds="100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - ERROR Evt Msg ID 1 was not generated as expected. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[1].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Events were generated when not expected."
endif
 
ut_setupevents $sc, $cpu, TST_EVS, 7, ERROR, 1

/$sc_$cpu_TST_EVS_SendEvtMsg ERROR EventId="7" Iters="5" Milliseconds="100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 5) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - ERROR Evt Msg ID 7 events were generated. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[7].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - The expected events were not generated."
endif             
 
write "************************************************************************"
write "  _________"
write "           |  Request generation of CRITICAL type event messages"
write "           | "
 
ut_setupevents $sc, $cpu, TST_EVS, 1, CRITICAL, 1
 
/$sc_$cpu_TST_EVS_SendEvtMsg CRIT EventId = "1" Iters = "5" Milliseconds = "100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 0) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - CRIT Evt Msg ID 1 was not generated as expected. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[1].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - Events were generated when not expected."
endif             
 
ut_setupevents $sc, $cpu, TST_EVS, 7, CRITICAL, 1
 
/$sc_$cpu_TST_EVS_SendEvtMsg CRIT EventId = "7" Iters = "5" Milliseconds = "100"
wait 2
 
if ($sc_$cpu_find_event[1].num_found_messages = 5) then
  ut_setrequirements cEVS3103, "P"
  write "<*> Passed (3103) - CRIT Evt Msg ID 7 events were generated. Binary filter mask = ", %hex($sc_$cpu_EVS_AppData[7].BinFltr[7].Msk, 4) 
else
  ut_setrequirements cEVS3103, "F"
  write "<!> Failed (3103) - The expected events were not generated."
endif             
 
ENDPROC ; $sc_$cpu_evs_mskd_evt
