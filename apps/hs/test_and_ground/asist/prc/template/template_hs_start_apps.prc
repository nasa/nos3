proc $sc_$cpu_hs_start_apps (step_num)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to startup the CFS Health and
;	    Safety (HS) and test (TST_HS) applications if they are not 
;	    already running.
;
; History:
;  18JUN09 WFM	Initial development of this proc
;  09/16/16        Walt Moleski    Updated for HS 2.3.0.0 using CPU1 for
;                                  commanding and added a hostCPU variable
;                                  for the utility procs that connect to
;                                  the host IP.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_es_events.h"
#include "hs_events.h"
#include "tst_hs_events.h"

;; Restore procedure logging
%liv (log_procedure) = logging

local file_index
local found_app1, found_app2
local stream1, stream2
local subStepNum = 1
local HSAppName = "HS"

local ramDir = "RAM:0"
local hostCPU = "$CPU"

write ";*********************************************************************"
write "; Step ",step_num, ".1: Determine if the applications are running."
write ";*********************************************************************"
start get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log",hostCPU)

found_app1 = FALSE
found_app2 = FALSE

;Loop thru the table looking for the CS and TST_CS
for file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = HSAppName) then
    found_app1 = TRUE
  elseif ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = "TST_HS") then
    found_app2 = TRUE
  endif
enddo

if ((found_app1 = TRUE) AND (found_app2 = TRUE)) then
  write "The Applications are running. Setup will be skipped!!!"
  goto procterm
else
  write "At least one application is not running. They will be started."
  wait 10
endif

;; Increment the subStep
subStepNum = 2

;  Load the applications
;; Only perform this step if found_app1 = FALSE
if (found_app1 = FALSE) then
  write ";*********************************************************************"
  write ";  Step ",step_num, ".2: Load and start the Health and Safety app"
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", {HSAppName}, HS_INIT_EID, "INFO", 2

  s load_start_app (HSAppName,hostCPU,"HS_AppMain")

  ; Wait for app startup events
  ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 70
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - HS Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for HS not received."
    endif
  else
    write "<!> Failed - HS Application start Event Message not received."
    goto procerror
  endif

  ;; Set CPU1 as the default
  stream1 = x'08AD'

  if ("$CPU" = "CPU2") then
     stream1 = x'09AD'
  elseif ("$CPU" = "CPU3") then
     stream1 = x'0AAD'
  endif

  /$SC_$CPU_TO_ADDPACKET STREAM=stream1 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

  subStepNum = 3
endif

if (found_app2 = FALSE) then
  write ";*********************************************************************"
  write "; Step ",step_num, ".",subStepNum,": Load and start the TST_HS application"
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", "TST_HS", TST_HS_INIT_INF_EID, "INFO", 2

  s load_start_app ("TST_HS",hostCPU,"TST_HS_AppMain")

  ; Wait for app startup events
  ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - TST_HS Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for TST_HS not received."
      write "Event Message count = ",$SC_$CPU_num_found_messages
    endif
  else
    write "<!> Failed - TST_HS Application start Event Message not received."
    goto procerror
  endif

  ;; Set CPU1 as the default
  stream2 = x'093C'

  if ("$CPU" = "CPU2") then
     stream2 = x'0A3C'
  elseif ("$CPU" = "CPU3") then
     stream2 = x'0B3C'
  endif

  /$SC_$CPU_TO_ADDPACKET STREAM=stream2 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

  wait 10
endif

goto procterm

procerror:
     Write "There was a problem with this procedure"

procterm:
    Write "Procedure completed!!!"

endproc
