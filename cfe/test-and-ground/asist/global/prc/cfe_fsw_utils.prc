proc cfe_fsw_utils
;
; History:
;
;   02/02/05        C. Firman        Added ut_sendappcmd directive
;   04/18/05        C. Firman        Added headers between directives
;   04/28/05        J. Polk          Changed "ut_sendresetcmd" tlm update
;                                    wait time from 8 to 24 seconds.
;   05/06/05        J. Rosenberg     Added ut_tlmfloatwait
;   05/06/05        R. Coon          Merged updates that had been made on PSE
;                                     ASIST workstation with changes made on
;                                     C&DH workstation
;   05/06/05        R. Coon          Corrected a bug in ut_tlmwait
;   05/13/05        C. Firman        Modified for CFE environment
;   06/24/05        T. Clement       Updated ut_setupevt to work for cFE
;   08/18/05        S. Applebee      Added ut_setrequirements
;   09/16/05        S. Applebee      Updated ut_setupevt to be consistent with
;                                     cFE. Added support for "CRIT".
;   10/11/05        T. Clement       Updated ut_setupevt by converting App name 
;                                     to upper case
;   03/31/06        W. Moleski       Updated ut_setupevt to use the new rdl.
;                                    Added ut_setupevents to handle multiples.
;   04/06/06        S. Applebee      Fixed initialization of 
;                                    $sc_$cpu_NUM_FOUND_MESSAGES in 
;                                    ut_setupevt.
;   09/12/06        S. Applebee      Modify ut_setrequirements to set to "A"
;                                    for analysis, from "P", and prohibit
;                                    changes from "A" except to "F". Also
;                                    modified ut_setevts as above.
;   11/01/06        S. Applebee      Added option for ut_setrequirements and 
;                                    ut_setevts to printout id and status 
;                                    after setting.
;   03/20/07        S. Applebee      Fixed ut_sendcmd, ut_sendrawcmd and
;                                    ut_sendresetcmd to allow
;                                    for variable ut_tlmupdate parameter.   
;   01/09/08        S. Applebee      Fixed ut_sendcmd to work with no passed
;                                    parameters.
;   07/14/08        S. Applebee      Added special handling for CF_APP in ut_sencdmd
;                                    and ut_sendrawcmd
;   03/26/06        S. Applebee      Updated ut_setupevt and ut_setupevents 
;                                    for GPM.
;   01/16/14        W. Moleski       ut_sendcmd, ut_sendappcmd, ut_sendrawcmd
;				     to detect rollover based upon the size of
;				     of the counters (DCR 22118)
;
; ********************* TELEMETRY FLOAT WAIT ***************************
;
DIRECTIVE ut_tlmfloatwait(tlm_mnemonic, value, tolerance, period) IS
BEGIN
goto preamb
;******************************************************************************
;  Test Name:  N/A
;  Test Level: Build Verification
;  Test Type:  N/A
;  Test Description
;      Digital computers cannot check for the equality of floating point values.
;      Instead, they can only check that their difference is less than some
;      pre-selected tolerance.  See the description of the arguments, below.
;      This utility waits for the specified telemetry mnemonic to update to the
;      specified value, with an allowable tolerance, or for the specified time
;      period.
;
;  Requirements Tested
;      None
;
;  Assumptions and Constraints
;      It is understood that the arguments 'value' and 'tolerance' are floating
;      point values.
;
;  Change History
;     Date           Name             Description
;   12/15/04        J. Rosenberg     Based on ut_tlmwait written by Richard Coon.
;                                    The WAIT UNTIL logic was suggested by
;                                    Mr. Coon.
;   01/26/05        J. Rosenberg     Improved documentation.
;
;  Arguments
;     Name              Description
;     tlm_mnemonic      Telemetry mnemonic of interest
;     value             Value to which the telemetry point should update
;     tolerance         A tolerance to be permitted about the expected value,
;                       'value'
;     period            Maximum time period in seconds to wait for the update
;
;  Global Parameters
;     Name              Description
;     UT_TW_Time        Elapsed time in seconds required for telemetry
;                        parameter to update
;     UT_TW_Status      Return status
;                         = UT_Success; telemetry updated as expected
;                         = UT_Failure; telemetry update failed
;
;  Procedures Called
;     Name              Description
;     None
;
;  Expected Test Results and Analysis
;     N/A
;
;******************************************************************************

preamb:

#include "ut_statusdefs.h"

local end_wait, start_time, current_value, accept_string

; Initialize start time, wait time, and status
start_time = %gmt
UT_TW_Time = 0
UT_TW_Status = UT_Failure

; Check number of arguments
IF ((%nargs < 3) OR (%nargs > 4)) THEN
   WRITE "<!> Invalid call to ut_tlmfloatwait"
   GOTO TERM
ENDIF

; Check that telemetry menmonic is valid
IF (NOT telemetry_defined(tlm_mnemonic)) THEN
   WRITE "<!> Undefined telemetry mnemonic:", tlm_mnemonic
   GOTO TERM
ENDIF

; If wait period not specified, use default
IF (%nargs < 4) THEN
   period = 20
ENDIF

; Wait for telemetry value to update to the expected value or
; for the maximum wait period to end;
; Default time is 24 seconds
end_wait = start_time + period + 4

WAIT UNTIL ((%float_cmp({tlm_mnemonic},  value, tolerance) = 0) OR end_wait)
UT_TW_Time = %gmt - start_time

; Check if the telemetry value updated correctly
current_value = {tlm_mnemonic}
IF (%float_cmp(current_value,  value, tolerance) = 0) THEN
   UT_TW_Status = UT_Success
ELSE
   WRITE "<!> Telemetry value did not update to within acceptable range of the expected value."
   WRITE "    Current value: ", current_value, " Expected value: ", value
   accept_string = sprintf("    Acceptable range: (%-g, %-g)", ;;
      (value - tolerance), (value + tolerance))
   WRITE accept_string
   WRITE "    Total wait time: ", UT_TW_Time
ENDIF

TERM:
END

; ********************* TELEMETRY WAIT ***************************
;
directive ut_tlmwait(tlm_mnemonic, value, period) is
begin
goto preamb
;******************************************************************************
;  Test Name:  N/A
;  Test Level: Build Verification
;  Test Type:  N/A
;  Test Description
;      This utility waits for the specified telemetry mnemonic to update to the
;      specified value or for the specified time period.
;
;  Requirements Tested
;      None
;
;  Assumptions and Constraints
;      None
;
;  Change History
;     Date           Name             Description
;   10/15/04        R. Coon          Original version
;   11/18/04        R. Coon          Added parameter checking, return of
;                                     elapsed time and status
;   05/06/04        R. Coon          Replaced do until loop with wait until
;
;  Arguments
;     Name              Description
;     tlm_mnemonic      Telemetry mnemonic of interest
;     value             Value to which the telemetry point should update
;     period            Maximum time period in seconds to wait for the update
;
;  Global Parameters
;     Name              Description
;     UT_TW_Time        Elapsed time in seconds required for telemetry
;                        parameter to update
;     UT_TW_Status      Return status
;                         = UT_Success; telemetry updated as expected
;                         = UT_Failure; telemetry update failed
;
;  Procedures Called
;     Name              Description
;     None
;
;  Expected Test Results and Analysis
;     N/A
;
;******************************************************************************

preamb:

#include "ut_statusdefs.h"

local end_wait, start_time, current_value

; Initialize start time, wait time, and status
start_time = %gmt
UT_TW_Time = 0
UT_TW_Status = UT_Failure

; Check number of arguments
IF ((%nargs < 2) OR (%nargs > 3)) THEN
   WRITE "<!> Invalid call to ut_tlmwait"
   GOTO TERM
ENDIF

; Check that telemetry menmonic is valid
IF (NOT telemetry_defined(tlm_mnemonic)) THEN
   WRITE "<!> Undefined telemetry mnemonic:", tlm_mnemonic
   GOTO TERM
ENDIF

; If wait period not specified, use default
IF (%nargs < 3) THEN
   period = 20
ENDIF

; Wait for telemetry value to update to the expected value or
; for the maximum wait period to end
end_wait = start_time + period + 4
wait until (({tlm_mnemonic} = value) OR end_wait)
current_value = {tlm_mnemonic}
UT_TW_Time = %gmt - start_time
; Check if the telemetry value updated correctly
IF (current_value = value) THEN
   UT_TW_Status = UT_Success
ENDIF
TERM:
END

; ********************* TELEMETRY UPDATE ***************************
;
;
directive ut_tlmupdate(mnemonic, wait_time) is
begin
#include "ut_statusdefs.h"

local   time_flag, pktid, wtime
local   time_betwn_pkts
local   seqctr, ctr0, pktid, ctime
local   i, catb_1, catb_2, cal_ctime
time_betwn_pkts=5

IF (%nargs < 1 OR %nargs > 2) GOTO ERROR1
; Check that telemetry menmonic is valid
IF (NOT telemetry_defined(mnemonic)) THEN
   WRITE "<!> Undefined telemetry mnemonic:", mnemonic
   GOTO TERM
ENDIF
UT_TU_Status=UT_TU_TlmNotUpdating
;
; should probably check value for a range here
;

; get the apid of pkt containing mnemonic
;
pktid = telemetry_attr(mnemonic, "APID_NUM")
if (%nargs = 1)THEN
   ctime = 24
else
   ctime = wait_time
endif
write " ; Telemetry update will wait a maximum of ", ctime, " seconds"
;
; get the current sequence count
;
; wait 1 pkt if wait_time is passed in
;
if (%nargs<>2) then
   seqctr = "P"&%hex(pktid,3)&"SCNT"
   ctr0 = %name(seqctr)          ; get initial seq ctr for specified pkt
   wtime = %gmt + ctime          ; set maximum wait time for change in seq ctr
   wait until (ctr0 != %name(seqctr) or wtime)
else
   seqctr = "P"&%hex(pktid,3)&"SCNT"
   ctr0 = %name(seqctr)          ; get initial seq ctr for specified pkt
   wtime = %gmt + ctime          ; set maximum wait time for change in seq ctr
   wait until (ctr0 != %name(seqctr) or wtime)
endif
if(ctr0!=%name(seqctr)) then
   UT_TU_Status=UT_TU_TlmUpdated
else
   UT_TU_Status=UT_TU_TlmNotUpdating
endif

GOTO TERM
ERROR1:
   UT_TU_Status = UT_TU_ParameterError
TERM:
end
; ************************** SEND COMMAND ******************************
;
;
directive ut_sendcmd( cmd_mnemonic, n_times, w_time) is
begin
#include "ut_statusdefs.h"
#include "cfe_utils.h"
local   cmd_prc_ver_pt, cmd_err_ver_pt
local   exp_cmd_prc_cnt, exp_cmd_err_cnt
local   j, wtime, cmd_prc_ver_cnt, cmd_err_ver_cnt
local   cmd_expr, cmd_tat
local temp
local   mission_prefix, loc_underscore, prcsr_subsys, mnem_len, rest_mnem
local   prcsr_prefix, actual_cmd_len, actual_cmd
local underscore = "_"
local ctr_size, ctr_rollover

if (%nargs = 1) then
   n_times = 1
endif

UT_SC_Status = UT_SC_CmdFailure
if (%nargs < 1) then
   write "<!> Argument error"
elseif (%nargs > 3) then
   write "<!> Argument error"
else
;
; find first underscore for the mission prefix
;
   loc_underscore = %locate (cmd_mnemonic, "_")
   mnem_len = %length (cmd_mnemonic)
   mission_prefix = %substring(cmd_mnemonic,1,loc_underscore)
;
; chop off the mission get the processor name next
;
   rest_mnem = %substring(cmd_mnemonic,loc_underscore+1, mnem_len)
   mnem_len = mnem_len - loc_underscore
   loc_underscore = %locate (rest_mnem, "_")
   prcsr_prefix = %substring(rest_mnem,1,loc_underscore)
;
; chop that part off and look for next underscore for subsys
;
   actual_cmd = %substring(rest_mnem,loc_underscore+1, mnem_len)
   mnem_len = mnem_len - loc_underscore
   loc_underscore = %locate (actual_cmd, "_")
   prcsr_subsys = %substring(actual_cmd,1,loc_underscore-1)

;
; construct processed count and error count
;

; temp fix for gpm
if (MISSION = "GPM") then
  prcsr_subsys= "" 
  underscore= ""
endif

   cmd_prc_ver_pt = mission_prefix&prcsr_prefix&prcsr_subsys&underscore&"CMDPC"
   cmd_err_ver_pt = mission_prefix&prcsr_prefix&prcsr_subsys&underscore&"CMDEC"

IF (mission_prefix = "CF_") THEN
   cmd_prc_ver_pt = mission_prefix&prcsr_prefix&"APPTLM."&prcsr_subsys&underscore&mission_prefix&"CMDPC"
   cmd_err_ver_pt = mission_prefix&prcsr_prefix&"APPTLM."&prcsr_subsys&underscore&mission_prefix&"CMDEC" 
ENDIF

;;   cmd_prc_ver_pt = mission_prefix&prcsr_prefix&prcsr_subsys&"_CMDPC"
;;   cmd_err_ver_pt = mission_prefix&prcsr_prefix&prcsr_subsys&"_CMDEC"

;
; store current values
;
   ;; Get the size of the counter and determine the roll-over value
   ctr_size = telemetry_attr(cmd_prc_ver_pt,"length")
   ctr_rollover = 256
   if (ctr_size = 2) then
      ctr_rollover = 65536
   endif

   exp_cmd_prc_cnt = {cmd_prc_ver_pt} + n_times
   if (exp_cmd_prc_cnt >= ctr_rollover) then
	exp_cmd_prc_cnt = exp_cmd_prc_cnt - ctr_rollover
   endif
;;   if (exp_cmd_prc_cnt >= 256) then
;;	exp_cmd_prc_cnt = exp_cmd_prc_cnt - 256
;;   endif
   exp_cmd_err_cnt =  {cmd_err_ver_pt}
;
; send the actual command number of specified times
;
   cmd_expr = "/"&(cmd_mnemonic)
   FOR j = 1 to n_times DO
      Write "Sending command:  ",cmd_mnemonic
      %cmd(cmd_expr)
      if (n_times > 1)THEN
	 wait 1
      endif
   ENDDO
; wait for telemetry to update
   if (%nargs < 3) then
     w_time = 24
   endif
;;      ut_tlmupdate (cmd_prc_ver_pt)
;   elseif (%nargs = 3) then
;        ut_tlmupdate (cmd_prc_ver_pt, {w_time})

    if (%nargs > 0) then
      ut_tlmupdate {cmd_prc_ver_pt}, {w_time}

      if  (UT_TU_Status = UT_TU_TlmUpdated ) THEN
         cmd_tat = 4*(n_times/8) + 16
         DO UNTIL cmd_tat = 0
            cmd_prc_ver_cnt = {cmd_prc_ver_pt}
            cmd_err_ver_cnt = {cmd_err_ver_pt}
            if (cmd_prc_ver_cnt = exp_cmd_prc_cnt;;
            AND cmd_err_ver_cnt = exp_cmd_err_cnt)THEN
               cmd_tat = 0
               UT_SC_Status = UT_SC_Success
            else
               cmd_tat = cmd_tat - 1
               wait 1
            endif
          enddo
          if (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
          AND cmd_err_ver_cnt = exp_cmd_err_cnt) THEN
	     UT_SC_Status = UT_SC_CmdDropped
          elseif (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
          AND cmd_err_ver_cnt <> exp_cmd_err_cnt) THEN
	     UT_SC_Status = UT_SC_CmdFailure
          endif
       elseif (UT_TU_Status = UT_TU_TlmNotUpdating)THEN
         UT_SC_Status = UT_SC_TlmNotUpdating
         write "Telemetry not updating"
       endif
   else
       write "<!> Invalid parameter call"
   endif
endif
end
; ************************** SEND APP COMMAND ******************************
; used to send an application command with different subsystem than
; the cmd name.
;
directive ut_sendappcmd( subsys, cmd_mnemonic, n_times, w_time) is
begin
#include "ut_statusdefs.h"
local   cmd_prc_ver_pt, cmd_err_ver_pt
local   exp_cmd_prc_cnt, exp_cmd_err_cnt
local   j, wtime, cmd_prc_ver_cnt, cmd_err_ver_cnt
local   cmd_expr, cmd_tat
local temp
local   mission_prefix, loc_underscore, prcsr_subsys, mnem_len, rest_mnem
local   prcsr_prefix, actual_cmd_len, actual_cmd
local ctr_size, ctr_rollover

UT_SC_Status = UT_SC_CmdFailure
if (%nargs = 2) then
   n_times = 1
endif
if (%nargs < 1) then
   write "<!> Argument error"
elseif (%nargs > 4) then
   write "<!> Argument error"
else
;
; find first underscore for the mission prefix
;
   loc_underscore = %locate (cmd_mnemonic, "_")
   mnem_len = %length (cmd_mnemonic)
   mission_prefix = %substring(cmd_mnemonic,1,loc_underscore)
;
; chop off the mission get the processor name next
;
   rest_mnem = %substring(cmd_mnemonic,loc_underscore+1, mnem_len)
   mnem_len = mnem_len - loc_underscore
   loc_underscore = %locate (rest_mnem, "_")
   prcsr_prefix = %substring(rest_mnem,1,loc_underscore)
;
; construct processed count and error count
;
   cmd_prc_ver_pt = mission_prefix&prcsr_prefix&subsys&"_CMDPC"
   cmd_err_ver_pt = mission_prefix&prcsr_prefix&subsys&"_CMDEC"
;
;
; store current values
;
   ;; Get the size of the counter and determine the roll-over value
   ctr_size = telemetry_attr(cmd_prc_ver_pt,"length")
   ctr_rollover = 256
   if (ctr_size = 2) then
      ctr_rollover = 65536
   endif

   exp_cmd_prc_cnt = {cmd_prc_ver_pt} + n_times
   if (exp_cmd_prc_cnt >= ctr_rollover) then
	exp_cmd_prc_cnt = exp_cmd_prc_cnt - ctr_rollover
   endif
;;   if (exp_cmd_prc_cnt >= 256) then
;;	exp_cmd_prc_cnt = exp_cmd_prc_cnt - 256
;;   endif
   exp_cmd_err_cnt =  {cmd_err_ver_pt}
;
; send the actual command number of specified times
;
   cmd_expr = "/"&(cmd_mnemonic)
   FOR j = 1 to n_times DO
      Write "Sending command:  ",cmd_mnemonic
      %cmd(cmd_expr)
      if (n_times > 1)THEN
	 wait 1
      endif
   ENDDO
; wait for telemetry to update
   if (%nargs <= 3) then
      ut_tlmupdate (cmd_prc_ver_pt)
;   elseif (%nargs = 3) then
;        ut_tlmupdate (cmd_prc_ver_pt, {w_time})

      if  (UT_TU_Status = UT_TU_TlmUpdated ) THEN
         cmd_tat = 4*(n_times/8) + 16
         DO UNTIL cmd_tat = 0
            cmd_prc_ver_cnt = {cmd_prc_ver_pt}
            cmd_err_ver_cnt = {cmd_err_ver_pt}
            if (cmd_prc_ver_cnt = exp_cmd_prc_cnt;;
            AND cmd_err_ver_cnt = exp_cmd_err_cnt)THEN
               cmd_tat = 0
               UT_SC_Status = UT_SC_Success
            else
               cmd_tat = cmd_tat - 1
               wait 1
            endif
          enddo
          if (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
          AND cmd_err_ver_cnt = exp_cmd_err_cnt) THEN
	     UT_SC_Status = UT_SC_CmdDropped
          elseif (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
          AND cmd_err_ver_cnt <> exp_cmd_err_cnt) THEN
	     UT_SC_Status = UT_SC_CmdFailure
          endif
       elseif (UT_TU_Status = UT_TU_TlmNotUpdating)THEN
         UT_SC_Status = UT_SC_TlmNotUpdating
         write "Telemetry not updating"
       endif
   else
       write "<!> Invalid parameter call"
   endif
endif
end
;
; ***************************** SEND RESET COMMAND ***********************
;
directive ut_sendresetcmd(prefix, w_time) is
begin
#include "ut_statusdefs.h"
local   cmd_prc_ver_pt, cmd_err_ver_pt
local   exp_cmd_prc_cnt, exp_cmd_err_cnt
local   j, wtime, cmd_prc_ver_cnt, cmd_err_ver_cnt
local   cmd_expr
local temp

IF (%nargs < 1 OR %nargs > 3) GOTO ERROR1
IF (%nargs = 1) then
  w_time = 24
ENDIF

cmd_prc_ver_pt = prefix&"_CMDPC"
cmd_err_ver_pt = prefix&"_CMDEC"

;
; store current values
;

exp_cmd_prc_cnt = 0
exp_cmd_err_cnt = 0


; construct and send the reset command
;
cmd_expr = "/"&prefix&"_RESETCTRS"
%cmd(cmd_expr)
; wait for telemetry to update
ut_tlmupdate (cmd_prc_ver_pt) {w_time}
IF (UT_TU_Status = UT_TU_TlmUpdated) THEN
  wtime = w_time
  DO UNTIL wtime = 0
      cmd_prc_ver_cnt = {cmd_prc_ver_pt}
      cmd_err_ver_cnt = {cmd_err_ver_pt}
      IF(cmd_prc_ver_cnt = exp_cmd_prc_cnt;;
        AND cmd_err_ver_cnt = exp_cmd_err_cnt) THEN
         wtime = 0
         UT_SRC_Status = UT_SRC_Success
      ELSE
	wait 1
        wtime = wtime - 1
      ENDIF
   ENDDO
   IF (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
     AND  cmd_err_ver_cnt = exp_cmd_err_cnt) THEN
	UT_SRC_Status = UT_SRC_CmdDropped
   ELSEIF (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
     AND  cmd_err_ver_cnt <> exp_cmd_err_cnt) ;;
     THEN
	UT_SRC_Status = UT_SRC_CmdFailure
   ENDIF
ELSEIF(UT_TU_Status = UT_TU_TlmNotUpdating)THEN
  UT_SRC_Status = UT_SRC_TlmNotUpdating
ELSE
  write "invalid parameter call"
ENDIF
GOTO TERM
ERROR1:
   write "argument error"
TERM:
end
;
; ******************** SEND RAW COMMAND ****************************
;
;
directive ut_sendrawcmd (prefix, cmd_value, w_time)is
begin

local cmd_value, apid
#include "ut_statusdefs.h"
local   cmd_prc_ver_pt, cmd_err_ver_pt
local   exp_cmd_prc_cnt, exp_cmd_err_cnt
local   j, wtime, cmd_prc_ver_cnt, cmd_err_ver_cnt
local   cmd_tat
local temp
local sdo_prefix, loc_underscore, sdo_subsys, mnem_len, rest_mnem
local ctr_size, ctr_rollover

if (%nargs < 1 OR %nargs > 3) then
   write "<!> Argument error"
else
;
;
; construct processed count and error count
;
    cmd_prc_ver_pt = prefix&"_CMDPC"
    cmd_err_ver_pt = prefix&"_CMDEC"

IF (prefix = "CF") THEN
   cmd_prc_ver_pt = prefix&"_APPTLM."&prefix&"_CMDPC"
   cmd_err_ver_pt = prefix&"_APPTLM."&prefix&"_CMDEC" 
ENDIF

;
; store current values
;
   ;; Get the size of the counter and determine the roll-over value
   ctr_size = telemetry_attr(cmd_prc_ver_pt,"length")
   ctr_rollover = 256
   if (ctr_size = 2) then
      ctr_rollover = 65536
   endif

   exp_cmd_prc_cnt = {cmd_prc_ver_pt}+1
   if (exp_cmd_prc_cnt >= ctr_rollover) then
      exp_cmd_prc_cnt = exp_cmd_prc_cnt - ctr_rollover
   endif
;;   if (exp_cmd_prc_cnt >= 256) then
;;      exp_cmd_prc_cnt = exp_cmd_prc_cnt - 256
;;   endif
   exp_cmd_err_cnt =  {cmd_err_ver_pt}
;
; send the actual command number of specified times
;
   Write "Sending:  ",cmd_value
   /raw {cmd_value}
;wait for telemetry to update
   if (%nargs < 3) then
     ut_tlmupdate (cmd_prc_ver_pt)
   elseif (%nargs=3) then
      UT_TLMUPDATE {cmd_prc_ver_pt} {w_time}
   endif

   IF (UT_TU_Status = UT_TU_TlmUpdated ) THEN
      cmd_tat = 8
;  wait
      DO UNTIL cmd_tat = 0
         cmd_prc_ver_cnt = {cmd_prc_ver_pt}
         cmd_err_ver_cnt = {cmd_err_ver_pt}
         IF (cmd_prc_ver_cnt = exp_cmd_prc_cnt;;
         AND cmd_err_ver_cnt = exp_cmd_err_cnt)THEN
            cmd_tat = 0
            UT_RCS_Status = UT_RCS_Success
         ELSEIF (cmd_prc_ver_cnt = exp_cmd_prc_cnt - 1 ;;
              AND cmd_err_ver_cnt = exp_cmd_err_cnt +1)THEN
            UT_RCS_Status = UT_RCS_CmdFailure
           cmd_tat = 0
         ELSE
           cmd_tat = cmd_tat - 1
           wait 1
         ENDIF
      ENDDO
      IF (cmd_prc_ver_cnt <> exp_cmd_prc_cnt ;;
      AND  cmd_err_ver_cnt = exp_cmd_err_cnt) THEN
	  UT_RCS_Status = UT_RCS_CmdDropped
      ENDIF
   ELSEIF(UT_TU_Status = UT_TU_TlmNotUpdating)THEN
      UT_RCS_Status = UT_RCS_TlmNotUpdating
   ELSE
      write "invalid parameter call"
   endif
ENDIF
end
;
; ******************** P F Indicate ***************************
;
;
directive ut_pfindicate(req_num, status) is
begin
write "FSW Requirement: ", %rpad(req_num,32," "), " P/F: ", status
;;write "CFE FSW Requirement:    ", req_num, " P/F :   ", status
end
;
; ******************** SETUP FOR EVENT  ****************************
;
;

directive ut_setupevt (SCID, PROCESSORID, APPNAME, EVENTID, EVENTTYPE) is
begin

#include "cfe_utils.h"
LOCAL SCNUM, PROCESSORNUM, TYPENUM
LOCAL s_under_bar="_"
LOCAL p_under_bar = "_"

SCID        = %upper(SCID)
PROCESSORID = %upper(PROCESSORID)
APPNAME     = %upper(APPNAME)
EVENTTYPE   = %upper(EVENTTYPE)

;; Set the Spacecraft ID
SCNUM = CFE_SC

;; This is not needed since the CFE_SC is defined in cfe_utils.h
; set up the event codes
;;IF (SCID = "SCX") THEN
;;   SCNUM = 66
;;ELSEIF (SCID = "") THEN
;;   SCNUM = ""
;;   s_under_bar=""
;;ENDIF

;; Default to Processor 1
PROCESSORNUM = 1
IF (PROCESSORID = "CPU2" OR PROCESSORID = "PSE") THEN
   PROCESSORNUM = 2
ELSEIF (PROCESSORID = "CPU3" OR PROCESSORID = "ACE") THEN
   PROCESSORNUM = 3
;;ELSEIF (PROCESSORID = "") THEN
;;   p_under_bar=""
ENDIF

IF     (EVENTTYPE = "DEBUG")     THEN 
   TYPENUM = 1
ELSEIF (EVENTTYPE = "INFO")      THEN 
   TYPENUM = 2
ELSEIF (EVENTTYPE = "ERROR")     THEN 
   TYPENUM = 3
ELSEIF (EVENTTYPE = "CRITICAL")  OR (EVENTTYPE = "CRIT") THEN 
   TYPENUM = 4
ENDIF

;; Initialize the telemetry database items based upon the arguments passed
local db_prefix
if (SCID <> "") then
  db_prefix = SCID & "_"
endif

if (PROCESSORID <> "") then
  db_prefix = db_prefix & PROCESSORID & "_"
endif

{db_prefix & "FIND_EVENT[1].SCID"}        = SCNUM
{db_prefix & "FIND_EVENT[1].PROCESSORID"} = PROCESSORNUM 
{db_prefix & "FIND_EVENT[1].APPNAME"}     = APPNAME
{db_prefix & "FIND_EVENT[1].EVENTID"}     = EVENTID 
{db_prefix & "FIND_EVENT[1].EVENTTYPE"}   = TYPENUM
{db_prefix & "FIND_EVENT[1].NUM_FOUND_MESSAGES"} = 0
{db_prefix & "NUM_FOUND_MESSAGES"} = 0

;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].SCID"}        = SCNUM
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].PROCESSORID"} = PROCESSORNUM 
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].APPNAME"}     = APPNAME
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].EVENTID"}     = EVENTID 
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].EVENTTYPE"}   = TYPENUM
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].NUM_FOUND_MESSAGES"}   = 0
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "NUM_FOUND_MESSAGES"}   = 0


end

;
; ******************** SETUPEVENTS  **********************
;
;

directive ut_setupevents (SCID, PROCESSORID, APPNAME, EVENTID, EVENTTYPE, EVENTLOC) is
begin

#include "cfe_utils.h"

LOCAL SCNUM, PROCESSORNUM, TYPENUM
LOCAL s_under_bar="_"
LOCAL p_under_bar = "_"
LOCAL i, j

SCID        = %upper(SCID)
PROCESSORID = %upper(PROCESSORID)
EVENTTYPE = %upper(EVENTTYPE)

;; Set the Spacecraft ID
SCNUM = CFE_SC

;; This is not needed since the CFE_SC is defined in cfe_utils.h
; set up the event codes
;;IF (SCID = "SCX") THEN
;;   SCNUM = 66
;;ELSEIF (SCID="") THEN
;;   SCNUM = ""
;;   s_under_bar=""
;;ENDIF

;;IF     (PROCESSORID = "CPU1" OR PROCESSORID = "CDH") THEN
;; Default to Processor 1
PROCESSORNUM = 1
IF (PROCESSORID = "CPU2" OR PROCESSORID = "PSE") THEN
   PROCESSORNUM = 2
ELSEIF (PROCESSORID = "CPU3" OR PROCESSORID = "ACE") THEN
   PROCESSORNUM = 3
;;ELSEIF (PROCESSORID = "") THEN
;;   p_under_bar=""
ENDIF

IF (EVENTTYPE = "DEBUG") THEN
   TYPENUM = 1
ELSEIF (EVENTTYPE = "INFO") THEN
   TYPENUM = 2
ELSEIF (EVENTTYPE = "ERROR") THEN
   TYPENUM = 3
ELSEIF (EVENTTYPE = "CRITICAL") OR (EVENTTYPE = "CRIT") THEN
   TYPENUM = 4
ENDIF

;; Initialize the telemetry database items based upon the arguments passed
local db_prefix
if (SCID <> "") then
  db_prefix = SCID & "_"
endif

if (PROCESSORID <> "") then
  db_prefix = db_prefix & PROCESSORID & "_"
endif

j = EVENTLOC

;; Set the pseudo telemetry values
{db_prefix & "FIND_EVENT["&j&"].SCID"}               = SCNUM
{db_prefix & "FIND_EVENT["&j&"].PROCESSORID"}        = PROCESSORNUM
{db_prefix & "FIND_EVENT["&j&"].NUM_FOUND_MESSAGES"} = 0
{db_prefix & "FIND_EVENT["&j&"].APPNAME"}            = %upper(APPNAME)
{db_prefix & "FIND_EVENT["&j&"].EVENTID"}            = EVENTID
{db_prefix & "FIND_EVENT["&j&"].EVENTTYPE"}          = TYPENUM

;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].SCID"} = SCNUM
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].PROCESSORID"} = PROCESSORNUM
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].NUM_FOUND_MESSAGES"}   = 0
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].APPNAME"} = %upper(APPNAME)
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].EVENTID"} = EVENTID
;;{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].EVENTTYPE"} = TYPENUM

write ";***********************************************************************"
write "; Setup event ", j, " with ", APPNAME, " ", EVENTTYPE, " ", EVENTID
write ";***********************************************************************"

end

;
; ******************** SET REQUIREMENTS  **********************
;
;
directive ut_setrequirements (REQNUM, STATE) is
begin

IF (%nargs <> 2) THEN
   write "<!> Invalid number of arguments"
ELSE

global ut_req_array_size
global ut_requirement[0 .. ut_req_array_size]
global ut_setreq_debug 
global ut_setreq_display

; define req id text if display option on
if (ut_setreq_display = "1") then
  global ut_requirement_text[0 .. ut_req_array_size]
endif

if ((ut_requirement[reqnum] = "U") ;;
   OR (state = "F")  ;;
   OR ((state = "A") and (ut_requirement[reqnum] <> "F"))) then
   ut_requirement[reqnum] = state
endif

;;if ((ut_requirement[reqnum] = "U") OR (state = "F")) then
;;   ut_requirement[reqnum] = state
;;endif

if (ut_setreq_display = "1") then
  write
  write "*** Attempting to set Requirement ", ut_requirement_text[reqnum], " to ", STATE, " ***."
  write "*** Current Requirement ", ut_requirement_text[reqnum]," status is ",ut_requirement[reqnum], " ***."  
  write
endif

if (ut_setreq_debug = "1") then
  write "Requirement Index: ", reqnum
  write "Requirement State: ", ut_requirement[reqnum]
endif

ENDIF
end

;
; ******************** SET EVENTS  **********************
;
;
directive ut_setevents (EVTNUM, EVTSTATE) is
begin

IF (%nargs <> 2) THEN
   write "<!> Invalid number of arguments"
ELSE

global ut_evt_array_size
global ut_evt[0 .. ut_evt_array_size]
global ut_setevt_debug 
global ut_setevt_display

; define evt id text if display option on
if (ut_setevt_display = "1") then
  global ut_evt_text[0 .. ut_evt_array_size]
endif

if ((ut_evt[evtnum] = "U") ;;
   OR (evtstate = "F")  ;;
   OR ((evtstate = "A") and (ut_evt[evtnum] <> "F"))) then
   ut_evt[evtnum] = evtstate
endif

;;if ((ut_evt[evtnum] = "U") OR (evtstate = "F")) then
;;   ut_evt[evtnum] = evtstate
;;endif

if (ut_setevt_display = "1") then
  write
  write "*** Attempting to set Event ", ut_evt_text[evtnum], " to ", EVTSTATE, " ***."
  write "*** Current Event ", ut_evt_text[evtnum]," status is ",ut_evt[evtnum], " ***."  
  write
endif

if (ut_setevt_debug = "1") then
  write "Event Index: ", evtnum
  write "Event State: ", ut_evt[evtnum]
endif

ENDIF

end

endproc
