proc $sc_$cpu_evs_send_debug
;JAN 11 2006 CFE_TBL added
;APR 14 2006 updated message for CFE_SB
on ERROR resume

#include "ut_statusdefs.h"

write
write "*** Sending DEBUG message for CFE_EVS."
write "*** NO messages are being generated at this time ***"
write ""

write "*** Sending DEBUG message for CFE_SB."
write "*** NO messages are being generated at this time ***"
write ""

write "*** Sending DEBUG message for CFE_ES."
write "*** NO messages are being generated at this time ***"
write ""

write "*** Sending DEBUG message for CFE_TIME."
write "*** NO messages are being generated at this time ***"
write ""

write "*** Sending DEBUG message for CFE_TBL."
write "*** NO messages are being generated at this time ***"
write ""

write "*** Sending DEBUG message for CI_APP."
write "*** NO messages are being generated at this time ***"
write

write "*** Sending DEBUG message for TO_APP."
write "*** NO messages are being generated at this time ***"
write

write "*** Sending DEBUG message for TST_EVS."
/$SC_$CPU_TST_EVS_SendEvtMsg DEBUG EventId = "1" Iters = "1" Milliseconds = "0"
wait 5

endproc
