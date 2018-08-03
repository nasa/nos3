PROC $sc_$cpu_evs_ctr_check(test_app_expected_counter,EVS_total_msgs_sent,AppBinFltrCtrMsg1)
;
;  Purpose: 
;           This is a sub-procedure  
;           To verify the values of Application Event Message Sent Counter
;           (from CVT and Telemetry)
;           EVS Event Msg Sent Ctr
;           and Application Binary Filter Counter     
;
;           Upon verification of values stated above the following Requirements
;           cEVS3004     (info from App info data file)
;           cEVS3018k_1  (App evt msg sent ctr in SB telemetry)
;           cEVS3103.3   (increment app bin filter ctr)
;           cEVS3104     (increment application evt msg sent ctr)
;           cEVS3105     (increment EVS evt msg sent ctr)
;           are accordingly assigned a value of Pass or Fail.
;         
;*******************************************************************************
#include "cfe_evs_gen_reqts.h"
 
global test_app_expected_counter
global EVS_total_msgs_sent
global AppBinFltrCtrMsg1
global added_app_location

write "test app expected counter = ", test_app_expected_counter
write "EVS total messages sent ctr = ", EVS_total_msgs_sent
write "App Binary Filter Ctr for Evt msg 1 = ", AppBinFltrCtrMsg1

if (($sc_$cpu_EVS_AppData[added_app_location].EventCounter = test_app_expected_counter) AND ;;
    ($sc_$cpu_EVS_APP[added_app_location].APPMSGSENTC = test_app_expected_counter)) then 
  ut_setrequirements cEVS3004, "P"
  ut_setrequirements cEVS3018k_1, "P"
  ut_setrequirements cEVS3104, "P"
  write"<*> Passed (3004;3018k_1;3104) - counters are all correct."
elseif (($sc_$cpu_EVS_AppData[added_app_location].EventCounter <> test_app_expected_counter) AND ;;
        ($sc_$cpu_EVS_APP[added_app_location].APPMSGSENTC <> test_app_expected_counter)) then 
  ut_setrequirements cEVS3004, "F"
  ut_setrequirements cEVS3018k_1, "F"
  ut_setrequirements cEVS3104, "F"
  write"<!> Failed (3004;3018k_1;3104) - CVT and telemetry App evt msg sent ctr are not equal "
elseif (($sc_$cpu_EVS_AppData[added_app_location].EventCounter = test_app_expected_counter) AND ;;
        ($sc_$cpu_EVS_APP[added_app_location].APPMSGSENTC <> test_app_expected_counter)) then
  ut_setrequirements cEVS3004, "P"
  ut_setrequirements cEVS3104, "P"
  write"<*> Passed (3004;3104) - App event counter is correct."
  ut_setrequirements cEVS3018k_1, "F"
  write"<!> Failed (3018k_1) - App msg sent counter is not as expected."
elseif (($sc_$cpu_EVS_AppData[added_app_location].EventCounter <> test_app_expected_counter) AND ;;
        ($sc_$cpu_EVS_APP[added_app_location].APPMSGSENTC = test_app_expected_counter)) then
  ut_setrequirements cEVS3004, "F"
  write"<!> Failed (3004) - App event counter is not as expected."
  ut_setrequirements cEVS3018k_1, "P"
  ut_setrequirements cEVS3104, "P"
  write"<*> Passed (3018k_1;3104) - App msg sent counter is correct."
endif
 
;*******************************************************************
if $sc_$cpu_EVS_MSGSENTC = EVS_total_msgs_sent  then
  ut_setrequirements cEVS3105, "P"
  write"<*> Passed (3105) - Total EVS msg sent counter is not as expected."  
else
  ut_setrequirements cEVS3105, "F"
  write"<!> Failed (3105) - Total message count is not correct"  
endif
 
;*******************************************************************
if $sc_$cpu_EVS_AppData[added_app_location].BinFltr[1].Ctr = AppBinFltrCtrMsg1 then
  ut_setrequirements cEVS3103_3, "P"
  write "<*> Passed (3103.3) - Bindary filter counter is correct. "  
else
  ut_setrequirements cEVS3103_3, "F"
  write"<!> Failed (3103.3) - Binary Filter counter is not correct. Expected ",AppBinFltrCtrMsg1,"; Actual = ",$sc_$cpu_EVS_AppData[added_app_location].BinFltr[1].Ctr
endif

;*******************************************************************
write"  CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
write"  CCC    Test App Evt Msgs Snt Ctr            = ", $sc_$cpu_EVS_AppData[added_app_location].EventCounter
write"  CCC    Test App Telemetry Evt Msgs Sent Ctr = ", $sc_$cpu_EVS_APP[added_app_location].APPMSGSENTC
write"  CCC    EVS Telemetry msgs sent Ctr          = ", $sc_$cpu_EVS_MSGSENTC
write"  CCC    Test App Evt Msg 1 Bin filter Ctr    = ", $sc_$cpu_EVS_AppData[added_app_location].BinFltr[1].Ctr

write"  CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
;
ENDPROC ; $sc_$cpu_evs_ctr_check
