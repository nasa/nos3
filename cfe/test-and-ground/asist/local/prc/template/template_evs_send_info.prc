proc $sc_$cpu_evs_send_info
;JAN 11 2006 added CFE_TBL and no-op cmd to TO_APP
;APR 14 2006 updated processing for CFE_SB task
;APR 17 2006 added TST_EVS no-op 
on ERROR resume

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_sb_events.h"
#include "cfe_tbl_events.h"
#include "cfe_time_events.h"

%liv (log_procedure) = logging

write "*** Sending INFO message for CFE_EVS."
ut_setupevents $SC, $CPU, CFE_EVS, CFE_EVS_NOOP_EID, INFO, 1

/$SC_$CPU_EVS_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

write "*** Sending INFO message for CFE_SB."
ut_setupevents $SC, $CPU, CFE_SB, CFE_SB_CMD0_RCVD_EID, INFO, 1

/$SC_$CPU_SB_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

write "*** Sending INFO message for CFE_ES."
ut_setupevents $SC, $CPU, CFE_ES, CFE_ES_NOOP_INF_EID, INFO, 1

/$SC_$CPU_ES_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF
 
write "*** Sending INFO message for CFE_TIME."
ut_setupevents $SC, $CPU, CFE_TIME, CFE_TIME_NOOP_EID, INFO, 1

/$SC_$CPU_TIME_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

write "*** Sending INFO message for CFE_TBL."
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_NOOP_INF_EID, INFO, 1

/$SC_$CPU_TBL_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

write "*** Sending INFO message for CI_APP."
ut_setupevents $SC, $CPU, CI_LAB_APP, 5, INFO, 1

/$SC_$CPU_CI_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

write "*** Sending INFO message for TO_APP."
ut_setupevents $SC, $CPU, TO_LAB_APP, 18, INFO, 1

/$SC_$CPU_TO_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

write "*** Sending INFO message for TST_EVS."
ut_setupevents $SC, $CPU, TST_EVS, 17, INFO, 1

/$SC_$CPU_TST_EVS_NOOP
wait 5

IF ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "Event Message generated."
ELSE
  write "Event Message NOT generated."
ENDIF

endproc
