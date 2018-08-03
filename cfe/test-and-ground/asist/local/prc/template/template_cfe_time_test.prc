proc $sc_$cpu_cfe_time_test

#include "ut_statusdefs.h"

seq print on GMT using %s LAST_PKT_UTC using %s LAST_PKT_MET using %s to cfe_flywheel.data

ut_sendcmd "$SC_$CPU_TIME_SETSTATE FLYWHEEL" 1 55

ut_sendcmd "$SC_$CPU_TIME_SETCLOCKMET SECONDS=-31536000 MICROSECS=0" 1 55

wait 15

seq print off all

endproc
