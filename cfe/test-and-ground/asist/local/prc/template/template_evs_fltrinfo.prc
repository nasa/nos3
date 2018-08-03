PROC $sc_$cpu_evs_fltrinfo
;
; Purpose:
; To provide binary filter information upon request from a main procedure.
; Checking requirements cEVS3100 and cEVS3100.1
; These requirements combined indicate that upon registration of an
; application:
;   its event message types statuses, 
;   event ID for events registered for filtering
;       Nov 16 2006 Update 
;       Note: all possible to register (8) events are recorded
;       and their respective masks,
; and app event msg sent counter must be created
; 
;************************************************************
 
#include "cfe_evs_gen_reqts.h"
 
global added_app_location
global BinFMask = "Msk"
global AppBinFltrCtrMsg1 = $SC_$CPU_EVS_AppData[added_app_location].BinFltr[1].Ctr
 
FOR i = 0 to ut_req_array_size DO
  write "Requirement at [",i,"] = ", ut_requirement[i]
ENDDO

LOCAL i = added_app_location
LOCAL AppEvts = 8
;
; IDs of evt msgs registered for filtering.
;
IF ($sc_$cpu_EVS_AppData[i].BinFltr[1].EvtId = 1) AND ;;
   ($sc_$cpu_EVS_AppData[i].BinFltr[2].EvtId = 2) AND ;;
   ($sc_$cpu_EVS_AppData[i].BinFltr[3].EvtId = 3) AND ;;
   ($sc_$cpu_EVS_AppData[i].BinFltr[4].EvtId = 4) AND ;;
   ($sc_$cpu_EVS_AppData[i].BinFltr[5].EvtId = 5) THEN
  ut_setrequirements cEVS3100, "P"
  write"<*> Passed (3100) - Event 1 - 5 registered for filtering"
ELSE
  ut_setrequirements cEVS3100, "F"
  write"<!> Failed (3100) - Wrong events registered for filtering"
ENDIF
;
; Mask values of evts msgs registered for filtering 
;
  IF ($sc_$cpu_EVS_AppData[i].BinFltr[1].Msk = x'0000') AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[2].Msk = x'ffff') AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[3].Msk = x'0001') AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[4].Msk = x'fff8') AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[5].Msk = x'0002') THEN
    ut_setrequirements cEVS3100, "P"
    write"<*> Passed (3100) - Binary Filter masks are correct"
  ELSE
    ut_setrequirements cEVS3100, "F"
    write"<!> Failed (3100) - Binary Filter masks are not correct."
  ENDIF
;
; Values of counters for the evts msgs registered for filtering 
;
  IF ($sc_$cpu_EVS_AppData[i].BinFltr[1].Ctr = 0) AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[2].Ctr = 0) AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[3].Ctr = 0) AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[4].Ctr = 0) AND ;;
     ($sc_$cpu_EVS_AppData[i].BinFltr[5].Ctr = 0) THEN
    ut_setrequirements cEVS3100_1, "P"
    write"<*> Passed (3100.1) - Binary Filter counts are set to 0"
  ELSE
    ut_setrequirements cEVS3100_1, "F"
    write"<!> Failed (3100.1) -  At least 1 Binary Filter count is not 0."
  ENDIF
  
  write "Evt Msg  |  Bin Fltr  |  App Bin  "
  write "  ID     |    Mask    |  Fltr Ctr "
  write "_________________________________ "
  for j = 1 to AppEvts do 
    WRITE "   ", $sc_$cpu_EVS_AppData[i].BinFltr[j].EvtId, "     |   ",  ;;
           %hex($sc_$cpu_EVS_AppData[i].BinFltr[j].Msk, 4), "    |   ",  ;;
            $sc_$cpu_EVS_AppData[i].BinFltr[j].Ctr
  enddo 
 
ENDPROC          ;$sc_$cpu_evs_fltrinfo
