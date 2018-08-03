proc $sc_$cpu_cf_start_apps (step_num)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Purpose:  The purpose of this procedure is to startup the CFS CCSDS File
;	    Delivery Protocol (CF) and test (TST_CF) applications if they are
;	    not already running.
;
; History:
;	03/22/10	Walt Moleski	Initial development of this proc
;	11/03/10	Walt Moleski	Modified to restart the CF app if it is
;					already running.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_es_events.h"
#include "cf_events.h"
#include "cf_msgids.h"
#include "tst_cf_events.h"

%liv (log_procedure) = logging

local file_index
local found_app1, found_app2
local stream1, stream2, stream3
local subStepNum = 1
local old_dnLink
global dn_link
local CFAppName = "CF"
local ramDir = "RAM:0"

write ";*********************************************************************"
write "; Step ",step_num, ".1: Determine if the applications are running."
write ";*********************************************************************"
;; In order to avoid the CFDP commands from being sent prior to the app being
;; started, the dn_link global variable needs to be set to 0
old_dnLink = dn_link
dn_link = 0

s get_file_to_cvt (ramDir,"cfe_es_app_info.log","$sc_$cpu_es_app_info.log","$CPU")

dn_link = old_dnLink

found_app1 = FALSE
found_app2 = FALSE

;Loop thru the table looking for the CF and TST_CF
for file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = CFAppName) then
    found_app1 = TRUE
  elseif ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = "TST_CF") then
    found_app2 = TRUE
  endif
enddo

;;if ((found_app1 = TRUE) AND (found_app2 = TRUE)) then
;;  write "The Applications are running. Setup will be skipped!!!"
;;  goto procterm
;;else
;;  write "At least one application is not running. They will be started."
;;  wait 10
;;endif

;; Increment the subStep
subStepNum = 2

;  Load the applications
;; Only perform this step if found_app1 = FALSE
if (found_app1 = FALSE) then
  write ";*********************************************************************"
  write ";  Step ",step_num, ".2: Load and start the CF application"
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", {CFAppName}, CF_INIT_EID, "INFO", 2

;; Commented out because the default Stacksize was not large enough for CF
;;  s load_start_app (CFAppName,"$CPU","CF_AppMain")
  s load_app (ramDir, CFAppName,"$CPU")
  /$SC_$CPU_ES_STARTAPP Application=CFAppName APP_ENTRY_PT="CF_AppMain" APP_FILE_NAME="/ram/cf.o" STACKSIZE=x'4000' PRIORITY=x'C8' RESTARTCPU

  ; Wait for app startup events
  ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1, 70
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - CF Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for CF not received."
    endif
  else
    write "<!> Failed - CF Application start Event Message not received."
    goto procerror
  endif
;; App is running. Just restart it
else
  write ";*********************************************************************"
  write ";  Step ",step_num, ".2: Restarting the CF application"
  write ";*********************************************************************"
  /$SC_$CPU_ES_RESTARTAPP Application=CFAppName
  wait 5
endif

;; CPU1 is the default
;; CF HK packet, Platform Config packet, Transaction Diag packet and 
;; the CF file downlink MID
stream1 = x'08B0'
stream2 = x'08B1'
stream3 = x'08B3'
local pduMID = CF_SPACE_TO_GND_PDU_MID

if ("$CPU" = "CPU2") then
   stream1 = x'09B0'
   stream2 = x'09B1'
   stream3 = x'09B3'
elseif ("$CPU" = "CPU3") then
   stream1 = x'0AB0'
   stream2 = x'0AB1'
   stream3 = x'0AB3'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream1 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
/$SC_$CPU_TO_ADDPACKET STREAM=stream2 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
/$SC_$CPU_TO_ADDPACKET STREAM=stream3 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
/$SC_$CPU_TO_ADDPACKET STREAM=pduMID PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'20'

subStepNum = 3

if (found_app2 = FALSE) then
  write ";*********************************************************************"
  write "; Step ",step_num, ".",subStepNum,": Load and start the TST_CF application"
  write ";*********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", "TST_CF", TST_CF_INIT_INF_EID, "INFO", 2

  s load_start_app ("TST_CF","$CPU","TST_CF_AppMain")

  ; Wait for app startup events
  ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - TST_CF Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for TST_CF not received."
      write "Event Message count = ",$SC_$CPU_num_found_messages
    endif
  else
    write "<!> Failed - TST_CF Application start Event Message not received."
    goto procerror
  endif
;; App is running. Just restart it
else
  write ";*********************************************************************"
  write ";  Step ",step_num, ".2: Restarting the TST_CF application"
  write ";*********************************************************************"
  /$SC_$CPU_ES_RESTARTAPP Application="TST_CF"
  wait 5
endif

;; CPU1 is the default
stream1 = x'093F'

if ("$CPU" = "CPU2") then
   stream1 = x'0A3F'
elseif ("$CPU" = "CPU3") then
   stream1 = x'0B3F'
endif

/$SC_$CPU_TO_ADDPACKET STREAM=stream1 PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'

wait 10

goto procterm

procerror:
     Write "There was a problem with this procedure"

procterm:
    Write "Procedure completed!!!"

endproc
