proc $sc_$cpu_sc_start_apps (step_num)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to startup the CFS Stored Command
;	    Processor (SC) and test (TST_SC) applications if they are not 
;	    already running.
;
; History:
;  12JAN09 WFM	Initial development of this proc
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_es_events.h"
#include "sc_events.h"
#include "tst_sc_events.h"

%liv (log_procedure) = logging

global rts001_started

local file_index
local found_app1, found_app2
local stream1, stream2
local subStepNum = 1
local SCAppName = "SC"

write ";*********************************************************************"
write "; Step ",step_num, ".1: Determine if the applications are running."
write ";*********************************************************************"
start get_file_to_cvt ("RAM:0","cfe_es_app_info.log","$sc_$cpu_es_app_info.log","$CPU")

found_app1 = FALSE
found_app2 = FALSE

;Loop thru the table looking for the CS and TST_CS
for file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = SCAppName) then
    found_app1 = TRUE
  elseif ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = "TST_SC") then
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
  write ";  Step ",step_num, ".2: Load and start the Stored Command application"
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", {SCAppName}, SC_INIT_INF_EID, "INFO", 2

  s load_start_app (SCAppName,"$CPU","SC_AppMain")

  ; Wait for app startup events
  ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 70
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - SC Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for SC not received."
    endif
  else
    write "<!> Failed - SC Application start Event Message not received."
    goto procerror
  endif

  ;; CPU1 is the default
  stream1 = x'08AA'

  if ("$CPU" = "CPU2") then
     stream1 = x'09AA'
  elseif ("$CPU" = "CPU3") then
     stream1 = x'0AAA'
  endif

  /$SC_$CPU_TO_ADDPACKET STREAM=stream1 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

  subStepNum = 3
endif

if (found_app2 = FALSE) then
  write ";*********************************************************************"
  write "; Step ",step_num, ".",subStepNum,": Load and start the TST_SC application"
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", "TST_SC", TST_SC_INIT_INF_EID, "INFO", 2
  ut_setupevents "$SC", "$CPU", {SCAppName}, SC_RTS_START_INF_EID, "INFO", 3

  s load_start_app ("TST_SC","$CPU","TST_SC_AppMain")

  ; Wait for app startup events
  ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - TST_SC Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for TST_SC not received."
      write "Event Message count = ",$SC_$CPU_num_found_messages
    endif
  else
    write "<!> Failed - TST_SC Application start Event Message not received."
    goto procerror
  endif

  ;; CPU1 is the default
  stream2 = x'0939'

  if ("$CPU" = "CPU2") then
     stream2 = x'0A39'
  elseif ("$CPU" = "CPU3") then
     stream2 = x'0B39'
  endif

  /$SC_$CPU_TO_ADDPACKET STREAM=stream2 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

  wait 10

  ;; Check if RTS #1 started
  if ($SC_$CPU_find_event[3].num_found_messages = 1) then
    rts001_started = TRUE
  endif
endif

goto procterm

procerror:
     Write "There was a problem with this procedure"

procterm:
    Write "Procedure completed!!!"

endproc
