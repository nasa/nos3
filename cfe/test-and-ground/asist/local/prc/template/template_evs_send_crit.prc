proc $sc_$cpu_evs_send_crit
;JAN 11 2006 CFE_TBL added
on ERROR resume

#include "ut_statusdefs.h"

write
write "*** Sending CRITICAL message for CFE_EVS."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for CFE_SB."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for CFE_ES."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for CFE_TIME."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for CFE_TBL."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for CI_APP."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for TO_APP."
write "*** NO CRITICAL messages are available at this time ***"
write ""

write "*** Sending CRITICAL message for TST_EVS."
/$SC_$CPU_TST_EVS_SendEvtMsg CRIT  EventId = "1" Iters = "1" Milliseconds = "0"
wait 5 

endproc
