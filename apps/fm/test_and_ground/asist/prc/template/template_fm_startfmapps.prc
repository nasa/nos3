PROC $sc_$cpu_fm_startfmapps
;*******************************************************************************
;  Proc Name:  FM_StartFMApps
;  Test Level: none 
;		     
;  Proc Description
;   The purpose of this procedure is to start the FM and TST_FM apps
;    and add any telemetry subscriptions so that commands can be sent from
;    the command line and telemetry seen outside of testing procedures
;    The pages for all telemetry subscriptions are also opened
;
;  Tlm Subscriptions: FM HK
;                     FM File Info
;                     FM Dir List to Tlm
;                     FM Open File List
;		      FM Free Space Packet
;                     TST_FM HK
;
;  Pages Opened:  FM HK Page
;                 FM File Info Page
;                 FM Dir List Tlm Page
;                 FM Dir List File Page
;                 FM Open File List Page
;                 FM Free Space Table Page
;                 TST FM HK Page
;
;
;  Change History
;
;	Date		   Name		Description
;	05/01/08   D. Stewart	Original Procedure
;       08/08/08   D. Stewart   Modified to be used by all fm testing procedures
;       01/13/10   W. Moleski	Added code to turn logging off around the
;				includes and added the FM Free Space Telemetry
;				packet subscription
;       03/01/11   W. Moleski   Added variables for App name
;       01/07/15   W. Moleski   Updated this proc to determine if the apps are
;				executing and only start then if they are not.
;				Also, the Child Task Priority setting is checked
;				to make sure it is lower than the priority of
;				the FM app.
;
;  Arguments
;	None
;
;**********************************************************************

;; Turn off logging for the includes
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "cfe_platform_cfg.h"
#include "fm_platform_cfg.h"
#include "fm_events.h"
#include "tst_fm_events.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"

%liv (log_procedure) = logging

;**********************************************************************
;  Define variables
;**********************************************************************
; LOCAL Variables
local stream1, stream2, stream3, stream4, stream5

local found_app1, found_app2
local FMAppName = FM_APP_NAME
local ramDir = "RAM:0"
;; This is the default priority set in the load_start_app procedure
local fm_task_priority = 200
local appDir = %env("WORK") & "/apps/$CPU/"

write ";*********************************************************************"
write ";  Display all FM and TST_FM pages"
write ";*********************************************************************"
write "Opening FM HK Page."
page $SC_$CPU_FM_HK

write "Opening FM File Info Page."
page $SC_$CPU_FM_FILE_INFO

write "Opening FM Dir List File Page."
page $SC_$CPU_FM_DIR_LIST_FILE

write "Opening FM Dir List Tlm Page."
page $SC_$CPU_FM_DIR_LIST

write "Opening FM Open File List Page."
page $SC_$CPU_FM_OPEN_FILE_LIST

write "Opening FM Free Space Table Page."
page $SC_$CPU_FM_FREESPACE_TABLE

write "Opening TST FM HK Page."
page $SC_$CPU_TST_FM_HK

write ";*********************************************************************"
write ";  Determine if the applications are running."
write ";*********************************************************************"
start get_file_to_cvt (ramDir, "cfe_es_app_info.log", "$sc_$cpu_es_app_info.log", "$CPU")

found_app1 = FALSE
found_app2 = FALSE

;Loop thru the table looking for the CS and TST_CS
for file_index = 1 to CFE_ES_MAX_APPLICATIONS do
  if ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = FMAppName) then
    found_app1 = TRUE
  elseif ($SC_$CPU_ES_ALE[file_index].ES_AL_AppName = "TST_FM") then
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

;; If TST_FM is the only app running, then you must stop TST_FM
local cmdCtr
if (found_app1 = FALSE) AND (found_app2 = TRUE) then
  cmdCtr = $SC_$CPU_ES_CMDPC + 1

  /$SC_$CPU_ES_DELETEAPP Application="TST_FM"
  wait 5

  ut_tlmwait $SC_$CPU_ES_CMDPC, {cmdCtr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - TST_FM app stop command sent properly."
  else
    write "<!> Failed - TST_FM app stop command did not increment CMDPC."
  endif

  found_app2 = FALSE
endif

;; If FM app is not executing, start it
if (found_app1 = FALSE) then
  write ";*********************************************************************"
  write ";  Start the File Manager (FM) Application and add any required"
  write ";  subscriptions.  "
  write ";********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_STARTUP_EID, "INFO", 2

  if (FM_CHILD_TASK_PRIORITY > fm_task_priority) then
    s load_start_app (FMAppName,"$CPU","FM_AppMain")
  else
    ;; Upload the application file
    s load_app (ramDir,FMAppName,"$CPU")

    fm_task_priority = FM_CHILD_TASK_PRIORITY - 10

    ;; Start the FM application with the priority calculated above
    /$SC_$CPU_ES_StartApp Application=FMAppName APP_Entry_PT="FM_AppMain" APP_File_Name="/ram/fm.o" STACKSIZE=X'2000' PRIORITY=fm_task_priority RESTARTCPU
  endif

  ; Wait for app startup events
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - FM Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for FM not received."
    endif
  else
    write "<!> Failed - FM Application start Event Message not received."
    goto procerror
  endif

  ;;; Need to set the stream based upon the cpu being used
  ;; CPU1 is the default
  stream1 = x'88A'
  stream2 = x'88B'
  stream3 = x'88C'
  stream4 = x'88D'
  stream5 = x'88E'

  if ("$CPU" = "CPU2") then
    stream1 = x'98A'
    stream2 = x'98B'
    stream3 = x'98C'
    stream4 = x'98D'
    stream5 = x'98E'
  elseif ("$CPU" = "CPU3") then
    stream1 = x'A8A'
    stream2 = x'A8B'
    stream3 = x'A8C'
    stream4 = x'A8D'
    stream5 = x'A8E'
  endif

  write "Sending command to add subscription for FM HK packet."
  /$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
  wait 5

  write "Sending command to add subscription for FM File Info packet."
  /$SC_$CPU_TO_ADDPACKET Stream=stream2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
  wait 5

  ;Initialize the FM File Info packet sequence counters
  P08Bscnt = 0
  P18Bscnt = 0
  P28Bscnt = 0

  write "Sending command to add subscription for FM Dir List Tlm packet."
  /$SC_$CPU_TO_ADDPACKET Stream=stream3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
  wait 5

  ;Initialize the FM Dir List Tlm packet sequence counters
  P08Cscnt = 0
  P18Cscnt = 0
  P28Cscnt = 0

  write "Sending command to add subscription for FM Open File List packet."
  /$SC_$CPU_TO_ADDPACKET Stream=stream4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
  wait 5

  write "Sending command to add subscription for FM Free Space packet."
  /$SC_$CPU_TO_ADDPACKET Stream=stream5 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
  wait 5
endif

;; If TST_FM app is not executing, start it
if (found_app1 = FALSE) then
  write ";*********************************************************************"
  write ";  Start the File Manager Test (TST_FM) Application and add any"
  write ";  required subscriptions.  "
  write ";********************************************************************"
  ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
  ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_INIT_INF_EID, "INFO", 2

  s load_start_app ("TST_FM","$CPU","TST_FM_AppMain")

  ; Wait for app startup events
  ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
  IF (UT_TW_Status = UT_Success) THEN
    if ($SC_$CPU_num_found_messages = 1) then
      write "<*> Passed - TST_FM Application Started"
    else
      write "<!> Failed - CFE_ES start Event Message for FM not received."
    endif
  else
    write "<!> Failed - TST_FM Application start Event Message not received."
    goto procerror
  endif

  ;;; Need to set the stream based upon the cpu being used
  ;; CPU1 is the default
  stream1 = x'927'

  if ("$CPU" = "CPU2") then
    stream1 = x'A27'
  elseif ("$CPU" = "CPU3") then
    stream1 = x'B27'
  endif

  write "Sending command to add subscription for TST_FM HK packet."
  /$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
  wait 5
endif

write ";***********************************************************************"
write ";  Enable DEBUG Event Messages for the FM application "
write ";***********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the FM application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=FMAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

goto procterm

procerror:
write ";  There was a problem with this procedure"

procterm:
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_startfmapps "
write ";*********************************************************************"
ENDPROC
