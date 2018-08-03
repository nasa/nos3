PROC $sc_$cpu_evs_test_app_info
;****************************************************************
;
; Tester           EIS
; Date             OCT-26-2005
; Purpose          To provide Application Data 
;                  to calling procedure, including:
;                  
;                  Application Evt Message Generation Status
;                  Event message types statuses"
; 
;****************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"

%liv (log_procedure) = logging

global added_app_location
GLOBAL SAVE_EVS_AppData_EventCounter[CFE_ES_MAX_APPLICATIONS]
GLOBAL SAVE_EVS_AppData_AppName[CFE_ES_MAX_APPLICATIONS]
GLOBAL SAVE_EVS_AppData_ActiveFlag[CFE_ES_MAX_APPLICATIONS]
GLOBAL SAVE_EVS_AppData_EvtTypeAF_Debug[CFE_ES_MAX_APPLICATIONS]
GLOBAL SAVE_EVS_AppData_EvtTypeAF_Info[CFE_ES_MAX_APPLICATIONS]
GLOBAL SAVE_EVS_AppData_EvtTypeAF_Err[CFE_ES_MAX_APPLICATIONS]
GLOBAL SAVE_EVS_AppData_EvtTypeAF_Crit[CFE_ES_MAX_APPLICATIONS] 
 
GLOBAL test_app_expected_counter = 0
 
global AppNAF[2] = ["AppName", "ActiveFlag"]
global appevsena = "ActiveFlag"
global appname = "AppName"
global appevtctr = "EventCounter"
global ETSub[1..4] = ["Debug", "Info", "Err", "Crit"]
global DebugEvtTy = "Debug"
global InfoEvtTy = "Info"
global ErrEvtTy = "Err"
global CritEvtTy = "Crit"
 
#include "cfe_evs_gen_reqts.h"
 
local app_added = TRUE
local app_name  = FALSE
 
FOR i = 0 to ut_req_array_size DO
  write "Requirement at [",i,"] = ", ut_requirement[i]
ENDDO
 
FOR i = 1 to CFE_ES_MAX_APPLICATIONS DO 
  ;; check that there is an added application name
  IF ({"SAVE_EVS_AppData_" & appname}[i] <> $sc_$cpu_EVS_AppData[i].{appname}) then
    ;;  verify name matches the one expected
    write " Added Application is ", $sc_$cpu_EVS_AppData[i].{appname}
    app_added = FALSE
    if ($sc_$cpu_EVS_AppData[i].{appname} = "TST_EVS") then        
      app_name = TRUE
      added_app_location = i
      goto check_ctr
    else
      if app_name = FALSE then
        write" No test app found"
      endif
    endif
  ELSE
    if app_added = TRUE then
      write "No added app at iteration ", i
    endif
  endif
enddo 

check_ctr:
;; check the test application evt msg sent counter

if ({"SAVE_EVS_AppData_" & appevtctr}[i] = 0) AND ;;
   ($sc_$cpu_EVS_AppData[i].{appevtctr} = 1) then 
  ut_setrequirements cEVS3004, "P"
  ut_setrequirements cEVS3100_2, "P"
  ut_setrequirements cEVS3104, "P"
  write "<*> Passed (3004;3100.2;3104) - Test app evt ctr incremented"
  write " Original app evt Counter value = ", SAVE_EVS_AppData_EventCounter[i] 
  write " Current app evt Counter value = ", $sc_$cpu_EVS_AppData[i].EventCounter
  test_app_expected_counter = $sc_$cpu_EVS_AppData[i].EventCounter   
else ; indicate problem to user with application evt msg sent counter 
  ut_setrequirements cEVS3004, "A"
  ut_setrequirements cEVS3100_2, "A"
  ut_setrequirements cEVS3104, "A"
  write "<!> Failed (3004;3100.2;3104) - App Event Counter did not increment."
endif  ; end of check for app evt msg sent counter

; ******************************************************************************
; check of EVS enable status
; *****************************************************************************
;
if ({"SAVE_EVS_AppData_" & appevsena}[i] = 0) AND ;;
   ($sc_$cpu_EVS_AppData[i].{appevsena} = 1) AND ;;
   (p@$sc_$cpu_EVS_APP[i].APPENASTAT = "ENA") then
  ut_setrequirements cEVS3004, "P"
  ut_setrequirements cEVS3008, "P"
  ut_setrequirements cEVS3018k_2a, "P"
  write "<*> Passed (3004;3008;3018k.2a) - EVS app data matches telemetry."
else ; inform user that the EVS status was either not found or is in incorrect
  ut_setrequirements cEVS3018k_2a, "A"
  ut_setrequirements cEVS3004, "A"
  ut_setrequirements cEVS3008, "A"
  write "<!> Failed (3004;3008;3018k.2a) - Require further analysis "
endif

write "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
write "*** For Task ", $sc_$cpu_EVS_AppData[i].{appname}, "*****"
write "*** Value of 0 means disabled, 1 means enabled *******"
WRITE "*** Application evt msg gen Original Value = ", {"SAVE_EVS_AppData_" & appevsena}[i] 
WRITE "***** Application evt msg gen New Value      = ", $sc_$cpu_EVS_AppData[i].{appevsena}
WRITE "*** Telemetry app evt msg gen Value        = ", $sc_$cpu_EVS_APP[i].APPENASTAT
wait 5
write "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
 
;*******************************************************************************
; check event message type statuses
;*******************************************************************************
write "----Checking Event Message Status----"
write ETSub[1], " = ", $sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[1]}
write ETSub[2], " = ", $sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[2]}
write ETSub[3], " = ", $sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[3]}
write ETSub[4], " = ", $sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[4]}
if (($sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[1]} = 0) AND ;;
    ($sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[2]} = 1) AND  ;;
    ($sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[3]} = 1) AND  ;;
    ($sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[4]} = 1)) then  
  for m = 1 to 4 do
    WRITE "*** Evt msg type ", ETSub[m]
    WRITE "*** Evt Msg Type status current value     = ", $sc_$cpu_EVS_AppData[i].EvtTypeAF.{ETSub[m]}
    wait 5
    ut_setrequirements cEVS3004, "P"
    write "<*> Passed (3004) - Test App info rcv'd"
  enddo
else  ; fail requirements if evt msg type statuses are not as expected
  ut_setrequirements cEVS3004, "A"
  ut_setrequirements cEVS3100, "A"
  write "<!> Failed (3004;3100) - Requries further analysis"
endif  ; end of check for evt msg type statuses
 
ENDPROC ; $sc_$cpu_evs_test_app_info
