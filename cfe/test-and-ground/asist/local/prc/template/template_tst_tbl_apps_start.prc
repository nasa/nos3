proc $sc_$cpu_tst_tbl_apps_start (step_num)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to perform startup for tbl
;           services integration test procedure
;
; History:
;  18APR05    Initial development of this proc         MOB
;  14FEB06    Converted to use on CPU3		WFM
;  20MAR07    Added include of cfe_platform_cfg.h and modified the loops for
;  	      the maximum number of apps to use the define instead of a 
;             hard-coded 32. WFM
;  01OCT07    Changed the name of this proc to $sc_$cpu_tst_tbl_start in order
;	      to be consistant with other templates.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"

%liv (log_procedure) = logging

local success_cmd_ctr, error_cmd_ctr
local packet_sequence_cnt
local proc_error_cnt
local event_packet_ctr
local file_time
local app_info_file_index
local found_app1, found_app2
local stream1, stream2

local ramDir = "RAM:0"

page $sc_$cpu_es_app_info

wait 10

write ";*********************************************************************"
write "; Step ",step_num, ".1: Determine if the Test applications are running."
write ";*********************************************************************"
start get_file_to_cvt (ramDir, "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")

;Loop thru the table looking for the TST_TBL apps (TST_TBL and TST_TBL2)
for app_info_file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_TBL") then
    found_app1 = TRUE
  elseif ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_TBL2") then
    found_app2 = TRUE
  endif
enddo

if ((found_app1 <> TRUE) AND (found_app2 <> TRUE)) then
  write "The Test Table applications are not running. They will be started."
  wait 10
else
  write "The Test Table Applications are running. Setup will be skipped!!!"
  goto procterm
endif

;  Load the test table applications
write ";*********************************************************************"
write ";  Step ",step_num, ".2: Loading the TST_TBL application"
write ";*********************************************************************"

start load_start_app ("TST_TBL", "$CPU", "TST_TBL_TaskMain")

wait 10

write ";*********************************************************************"
write "; Step ",step_num, ".3: Loading the TST_TBL2 application"
write ";*********************************************************************"

start load_start_app ("TST_TBL2", "$CPU", "TST_TBL2_TaskMain")

wait 10

write ";*********************************************************************"
write "; Step ",step_num, ".4: Verify the applications are running.          "
write ";*********************************************************************"
;  Dump ES apps
start get_file_to_cvt (ramDir, "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")

;Loop thru the table looking for the TST_TBL apps (TST_TBL and TST_TBL2)
for app_info_file_index = 1 to CFE_ES_MAX_APPLICATIONS do
    if ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_TBL") then
	found_app1 = TRUE
    elseif ($SC_$CPU_ES_ALE[app_info_file_index].ES_AL_AppName = "TST_TBL2") then
        found_app2 = TRUE
    endif
enddo

if ((found_app1 = TRUE) AND (found_app2 = TRUE)) then
    write "<*> Passed - The Table Test Applications were found in the ES Application Info File"
else
    write "<!> Failed - Did not find the applications that were just loaded"
    wait 10
    goto procerror
endif

write ";*********************************************************************"
write "; Step ", step_num, ".5: Send NOOP commands to the test Applications  "
write "; and verify that they were received. "
write ";*********************************************************************"

;; CPU1 is the default
event_packet_ctr = p004scnt

if ("$CPU" = "CPU2") then
  event_packet_ctr = p024scnt
elseif ("$CPU" = "CPU3") then
  event_packet_ctr = p044scnt
endif

wait 5

/$SC_$CPU_TST_TBL_NOOP
/$SC_$CPU_TST_TBL2_NOOP
   
if ("$CPU" = "CPU1" OR "$CPU" = "") then
  wait until (p004scnt >= event_packet_ctr + 2)
elseif ("$CPU" = "CPU2") then
  wait until (p024scnt >= event_packet_ctr + 2)
elseif ("$CPU" = "CPU3") then
  wait until (p044scnt >= event_packet_ctr + 2)
endif

write "<*> Passed - Test Table Applications are running."
Write "AFTER loading the Table Test Apps, there are " & $SC_$CPU_TBL_NUMTABLES & " tables registered"
  
Write "Setup complete"

write ";*********************************************************************"
write "; Step ", step_num, ".6: Send commands to subscribe to the HK packets "
write "; for each Test application. "
write ";*********************************************************************"

;; CPU1 is the default
stream1 = x'904'
stream2 = x'909'

if ("$CPU" = "CPU2") then
   stream1 = x'A04'
   stream2 = x'A09'
elseif ("$CPU" = "CPU3") then
   stream1 = x'B04'
   stream2 = x'B09'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream1 PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

/$SC_$CPU_TO_ADDPACKET STREAM=stream2 PKT_SIZE=X'ffff' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

wait 10

write ";*********************************************************************"
write "; Step ", step_num, ".7: Send commands to dump the table registry "
write "; before initializing the tables."
write ";*********************************************************************"

page $SC_$CPU_TBL_REGISTRY
wait 5
start get_file_to_cvt (ramDir, "cfe_tbl_reg.log", "$sc_$cpu_tbl_reg.log", "$CPU")
wait 20

goto procterm

procerror:
     Write "There was a problem with this procedure"
     wait 100

procterm:
    Write "Procedure completed!!!"

endproc
