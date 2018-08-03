proc ut_runproc(proc_to_run, opt_parm)
goto preamb

;******************************************************************************
;  Test Name:  N/A
;  Test Level: Build Verification
;  Test Type:  N/A
;  Test Description
;      Control entry to procedures for ST-5 FSW testing by common entry
;      point and captures the procedure run in a log.
;
;  Requirements Tested
;      None
;
;  Assumptions and Constraints
;      None
;
;  Change History
;     Date           Name             Description
;   04/09/03        C. Firman         ST5 version
;   10/12/04        R. Coon           Modified for SDO
;   08/18/05        S. Applebee       Modified for cFE
;   09/06/05        S. Applebee       Added Workstation Info
;   06/02/06        S. Applebee       Add logoff when done option
;   03/27/07        N. Schweiss       Changed to handle x.x.x fmt for CFE version.
;   11/24/10        W. Moleski	      Changed to handle x.x.x.x CFE version fmt.
;
;  Arguments
;     Name              Description
;     proc_to_run       String giving the name of procedure to run
;     logoff_when_done  String giving logoff option
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

#include "cfe_utils.h"

local rstol_dir
local log_name, proc_name, v
local stime, etime
local hostname
local cmd_tlm_path
local gds_label
local gds_version
local logoff_when_done = 0

; remove "/" from tlm and cmd compile date

local cmddbdate = command_parm_attr("verid_cmd", "date", "description")
local h_cmddbdate

for i = 1 to %length(cmddbdate) do
  if %substring(cmddbdate,i,I) = "/" then
     h_cmddbdate = cmddbdate
     cmddbdate = %substring(h_cmddbdate,1,i-1) & "-" & %substring(h_cmddbdate,i+1, %length(h_cmddbdate))
  endif
enddo

local tlmdbdate = vidtlm.date
local h_tlmdbdate

for i = 1 to %length(tlmdbdate) do
  if %substring(tlmdbdate,i,I) = "/" then
     h_tlmdbdate = tlmdbdate
     tlmdbdate = %substring(h_tlmdbdate,1,i-1) & "-" & %substring(h_tlmdbdate,i+1, %length(h_tlmdbdate))
  endif
enddo

;
; get the procedure name from the input string - there may be
; input args to the procedure
;
v = %locate(proc_to_run, "(")
if (v <> 0) then
   proc_name = %substring(proc_to_run, 1 ,v-1)
else
   proc_name = proc_to_run
endif
;
; construct the log name
;
stime = %gmt
etime = "-" & %year(stime) & "-" & %day_of_year(stime) & "-" & ;;
        sprintf("%02d",%hour(stime)) & "h" & ;;
        sprintf("%02d",%minute(stime)) & "m" & ;;
        sprintf("%02d",%second(stime)) & "s"
log_name = proc_name & etime & ".log"
;
; close the previous log
;
newlog

; Build the database prefix using SC and CPU definitions from cfe_utils.h
local db_prefix, cpu_prefix

; If SC is set, use it
if (SC <> "") then
  db_prefix = SC
endif

;; Add the CPU definition
db_prefix = db_prefix & CPU_CFG

cpu_prefix = db_prefix

; Set the database prefix for CPU1
if (numCPUs > 1) then
  cpu_prefix = db_prefix & "1"
endif

; setup the database mnemonics for CPU1
local cksumItem = cpu_prefix & "_ES_CKSUM"
local cfeMajorVerItem = cpu_prefix & "_ES_CFEMAJORVER"
local cfeMinorVerItem = cpu_prefix & "_ES_CFEMINORVER"
local cfeRevisionItem = cpu_prefix & "_ES_CFEREVISION"
local cfeMissionRevItem = cpu_prefix & "_ES_CFEMSNREV"
local osMajorVerItem = cpu_prefix & "_ES_OSMAJORVER"
local osMinorVerItem = cpu_prefix & "_ES_OSMINORVER"

;
; Output FSW configuration information
;
write "****************** FSW Configuration ******************"
;; Below is the original CFE implementation
;;;;write "        CPU1 Checksum:    ", SCX_CPU1_ES_CKSUM
;;;;write "        CPU1 cFE Version: ", gsescstatval21
;;;;write "        CPU1 OS Version:  ", SCX_CPU1_ES_OSMAJORVER & "." & SCX_CPU1_ES_OSMINORVER
;;;;write " "
;;;;write "        CPU2 Checksum:    ", SCX_CPU2_ES_CKSUM
;;;;write "        CPU2 cFE Version: ", gsescstatval22
;;;;write "        CPU2 OS Version:  ", SCX_CPU2_ES_OSMAJORVER & "." & SCX_CPU2_ES_OSMINORVER
;;;;write " "
;;;;write "        CPU3 Checksum:    ", SCX_CPU3_ES_CKSUM
;;;;write "        CPU3 cFE Version: ", gsescstatval23
;;;;write "        CPU3 OS Version:  ", SCX_CPU3_ES_OSMAJORVER & "." & SCX_CPU3_ES_OSMINORVER
;;;;write ""
;;;;write "        Workstation: ", %upper(%liv(LOCALHOST)) 

;; Determine what to print based on the number of CPUs that exist
if (numCPUs = 1) then
  write "        Checksum:    ", {cksumItem}
  write "        cFE Version: ", {cfeMajorVerItem} & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  write "        OS Version:  ", {osMajorVerItem} & "." & {osMinorVerItem}
  write " "
endif

if (numCPUs >= 2) then
  ;; Output CPU1
  write "        CPU1 Checksum:    ", {cksumItem}
  write "        CPU1 cFE Version: ", {cfeMajorVerItem} & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  write "        CPU1 OS Version:  ", {osMajorVerItem} & "." & {osMinorVerItem}
  write " "

  ; Set the database prefix for CPU2
  cpu_prefix = db_prefix & "2"

  ; setup the database mnemonics for CPU2
  cksumItem = cpu_prefix & "_ES_CKSUM"
  cfeMajorVerItem = cpu_prefix & "_ES_CFEMAJORVER"
  cfeMinorVerItem = cpu_prefix & "_ES_CFEMINORVER"
  cfeRevisionItem = cpu_prefix & "_ES_CFEREVISION"
  cfeMissionRevItem = cpu_prefix & "_ES_CFEMSNREV"
  osMajorVerItem = cpu_prefix & "_ES_OSMAJORVER"
  osMinorVerItem = cpu_prefix & "_ES_OSMINORVER"

  write "        CPU2 Checksum:    ", {cksumItem}
  write "        CPU2 cFE Version: ", {cfeMajorVerItem} & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  write "        CPU2 OS Version:  ", {osMajorVerItem} & "." & {osMinorVerItem}
  write " "
endif

; This proc only supports up to 3 CPUs.
; If there are more than 3, the additional CPUs must be added below
if (numCPUs = 3) then
  ; Set the database prefix for CPU3
  cpu_prefix = db_prefix & "3"

  ; setup the database mnemonics for CPU3
  cksumItem = cpu_prefix & "_ES_CKSUM"
  cfeMajorVerItem = cpu_prefix & "_ES_CFEMAJORVER"
  cfeMinorVerItem = cpu_prefix & "_ES_CFEMINORVER"
  cfeRevisionItem = cpu_prefix & "_ES_CFEREVISION"
  cfeMissionRevItem = cpu_prefix & "_ES_CFEMSNREV"
  osMajorVerItem = cpu_prefix & "_ES_OSMAJORVER"
  osMinorVerItem = cpu_prefix & "_ES_OSMINORVER"

  write "        CPU3 Checksum:    ", {cksumItem}
  write "        CPU3 cFE Version: ", {cfeMajorVerItem} & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  write "        CPU3 OS Version:  ", {osMajorVerItem} & "." & {osMinorVerItem}
  write ""
endif

write "        Connection Status"
write "        -----------------"
write "        CFDP: ", %select(packet_valid("my_entity_id"),"UP","DOWN")
write "        UDP: ", p@GSE_ICHAN
write "        SWTS: ", p@GSE_SCHAN
; write "        ITOS: ", p@GSE_ZCHAN

;; Set the gds labels to not used by default in the case that it is defined
gds_label = ""
gds_version = "N/A"

#ifdef GDS_EXISTS
  write "        GDS:  ", %select(packet_valid("GDS_EXECUTOR_SIMULATION_TIME"),"UP", "DOWN")
  if (GSE_HCHAN = 1) then
    gds_label = "GDS"
    gds_version = GDS_EXECUTOR_GDS_VERSION_VALUE
  endif
#endif

write
write "        CMD / TLM Path"
write "        --------------"
if (GSE_ICHAN = 1) then
  cmd_tlm_path = "UDP"
elseif (GSE_TCHAN = 1) then
  cmd_tlm_path = "SWTS"
elseif (GSE_ZCHAN = 1) then
  cmd_tlm_path = "ITOS"
endif

write "        ", cmd_tlm_path
write "        ", gds_label
write
write "        ASIST / GDS Configuration"
write "        -------------------------"
write "        Workstation: ", %upper(%liv(LOCALHOST))
write "        Account: ", %env("ACCOUNT")
write "        Version: ", ASIST_VERSION
write "        Tlm DB:  Version: ",vidtlm.version, "  Date: ", tlmdbdate, "  Time: ", vidtlm.timeof
write "        Cmd DB:  Version: ",%rpad(command_parm_attr("verid_cmd", "version", "description"),4," "),"  Date: ",cmddbdate,"  Time: ",command_parm_attr("verid_cmd", "timeof", "description")

#ifdef GDS_EXISTS
  write "        GDS: ", gds_version
#endif

write
write "        Telemetry Info"
write "        --------------"
;;; write "        Pkt Loss Count: ", TO_PCKTSDISCARD
write "        Pkt Loss Count: N/A"
write "****************  End FSW Configuration ***************"
;
; Start the procedure
;
write "Starting Procedure.... ", proc_to_run
rstol_dir = "rstol 'start " & proc_to_run & "'"
stime = %gmt
native(rstol_dir)

#ifdef GDS_EXISTS
write
write "*** GDS STATUS ***"
write
write "GDS: ", %select(packet_valid("GDS_EXECUTOR_SIMULATION_TIME"),"UP","DOWN")
if (GSE_HCHAN = 1) then
  gds_label = "GDS"
  gds_version = GDS_EXECUTOR_GDS_VERSION_VALUE
else
  gds_label = ""
  gds_version = "N/A"
endif
write "GDS Version: ", gds_version
write
#endif

write "*** Telemetry Info ***"
write
;;; write "Pkt Loss Count: ", TO_PCKTSDISCARD
write "Pkt Loss Count: N/A"
write
write "******************"
write

etime = %gmt
write "Elapsed time:  ", etime-stime, " seconds"
write "Elapsed time:  ", (etime-stime)/60, " minutes"
;
; wait for completion of procedure
;
wait(10)
;
; close the log to procedure log name for archive
;
local test_log_string = "/test_logs/"
if ((logoff_when_done = 0) and  (%length(opt_parm) <> 0)) then
  test_log_string = test_log_string & opt_parm & "/"
endif

write "Creating filtered log at " & %env("WORK") & test_log_string ;;
      & log_name & "f"
write "Creating filtered output log at " & %env("WORK") & test_log_string ;;
      & log_name & "p"
write "Creating filtered output log (without SFDUs) at " & %env("WORK") ;;
      & test_log_string & log_name & "s"
write "Creating filtered event log at " & %env("WORK") & test_log_string ;;
      & log_name & "e"
write "Creating filtered requirements log at " & %env("WORK") ;;
      & test_log_string & log_name & "r"

rstol_dir = "rstol 'newlog " & log_name & "'"
native(rstol_dir)
;
; Create filtered version of log
;
rstol_dir = "view.old.events -l " & %env("STOL_LOG") & "/" & log_name & ;;
            " <" & %env("STOL_LOG") & "/cfe_asist_filter >" & %env("WORK") & ;;
            test_log_string & log_name & "f"
; morewrite rstol_dir
native(rstol_dir)
;
; Create filtered version of log without procedure source lines
;
rstol_dir = "view.old.events -l " & %env("STOL_LOG") & "/" & log_name & ;;
            " <" & %env("STOL_LOG") & "/cfe_asist_mini_filter >" & ;;
	    %env("WORK") & test_log_string & log_name & "p"
native(rstol_dir)
;
; Create filtered version of log with only event lines
;
rstol_dir = "view.old.events -l " & %env("STOL_LOG") & "/" & log_name & ;;
            " <" & %env("STOL_LOG") & "/cfe_asist_tlm_filter >" & ;;
	    %env("WORK") & test_log_string & log_name & "e"
native(rstol_dir)
;
; Create filtered version of log with only requirement status lines
;
rstol_dir = "view.old.events -l " & %env("STOL_LOG") & "/" & log_name & ;;
            " <" & %env("STOL_LOG") & "/cfe_asist_req_filter | " & ;;
	    "grep ""FSW Requirement"" >" & %env("WORK") & test_log_string & log_name & "r"
native(rstol_dir)

;; Create filtered version of log without SFDU information in procedure source lines
;
rstol_dir = "view.old.events -l " & %env("STOL_LOG") & "/" & log_name & ;;
	    " <" & %env("STOL_LOG") & "/cfe_asist_mini_filter | " & ;;
	    "grep -vE ""SFDU is | Command is"" >" & %env("WORK") & ;;
	    test_log_string & log_name & "s"
native(rstol_dir)

;
; done
;

;;if (%length(logoff_when_done) <> 0) then
;;  logoff_when_done = %lower(logoff_when_done)
;;  if (logoff_when_done = "y") then
;;    cfe_shutdown
;;  endif
;;endif

endproc
