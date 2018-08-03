PROC $sc_$cpu_cfe_stress
;*******************************************************************************
;  Test Name:  $sc_$cpu_cfe_stress
; 
;  Test Description
;	The purpose of this test is to stress the cFE system.
;
;  Prerequisite Conditions
;       The ASIST ground station is up and running.
;
;  Assumptions and Constraints
;
;  Change History
;	Date		Name		Description
;	04/28/08	W. Moleski	Initial development.
;	02/08/12	W. Moleski	Added variable for ram disk
;
;  Arguments
;	None 
;
;  Procedures Called
;	Name					Description
;
;  Expected Test Results and Analysis
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"

%liv (log_procedure) = logging

;; Define local variables
local ramDir = "RAM:0"
local ramDirLogical = "/ram/"

write ";*********************************************************************"
write "; Step 1: Upload the necessary files for the cFE Stress test."
write ";*********************************************************************"
;; Upload the table load file used by TST_TBL3
s ftp_file (ramDir,"tsttbl3_tbl2load.dat","tsttbl3_tbl2load.dat","$CPU","P")
wait 10

;; Reset the Processor Reset Counter so that a Power-On reset does not occur
;; when the processor Resets = Max Resets
/$SC_$CPU_ES_RESETPRCNT
wait 5

write ";*********************************************************************"
write "; Step 2: Command a Processor Reset."
write ";*********************************************************************"
/$SC_$CPU_ES_PROCESSORRESET
wait 10

close_data_center
wait 75

cfe_startup $CPU
wait 5

;; Display the Telemetry pages used by this test
page $SC_$CPU_ES_HK
page $SC_$CPU_EVS_HK
page $SC_$CPU_SB_HK
page $SC_$CPU_SB_PIPE
page $SC_$CPU_TBL_HK
page $SC_$CPU_TO_HK

write ";*********************************************************************"
write "; Step 3: Start the Stress Applications.   "
write ";*********************************************************************"
local fileCtr = 1
local fullFileName
local fileName
local logSize = CFE_ES_SYSTEM_LOG_SIZE - 200

;; Load and start the TST_ES application
s load_start_app ("TST_ES", "$CPU")
wait 5

;; Load and start the TST_ESS2 application
s load_start_app ("TST_ESS2", "$CPU")
wait 5

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Load and start the TST_ESS3 application
s load_start_app ("TST_ESS3", "$CPU")
wait 5

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Setup the Event Filter for Table Services for Event ID=35
/$SC_$CPU_EVS_ADDEVTFLTR Application="CFE_TBL" Event_ID=35 EVENT_Mask=X'FFA0'
wait 1

;; Load and start the TST_TBL3 application
s load_start_app ("TST_TBL3", "$CPU")
wait 5

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Load and start the TST_TBL4 application
s load_start_app ("TST_TBL4", "$CPU")
wait 5

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Load and start the TST_SB3 application
s load_start_app ("TST_SB3", "$CPU")
wait 5

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Load and start the TST_SB2 application 
s load_start_app ("TST_SB2", "$CPU")
wait 5

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif


write ";*********************************************************************"
write "; Step 4: Loop forever dumping the syslog when it's full!!!!   "
write ";*********************************************************************"
;;wait
local i

while 1 do
  wait until $SC_$CPU_ES_SYSLOGBYTEUSED > logSize

  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
enddo

STOPTEST:
write ";*********************************************************************"
write "; Step 5: Delete the Stress Test Applications."
write ";*********************************************************************"
;; Delete the TST_ESS2 application
/$SC_$CPU_ES_DELETEAPP Application="TST_ESS2"
wait 2

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Delete the TST_ESS3 application
/$SC_$CPU_ES_DELETEAPP Application="TST_ESS3"
wait 2

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Delete the TST_ES application
/$SC_$CPU_ES_DELETEAPP Application="TST_ES"
wait 2

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Delete the TST_TBL3 application
/$SC_$CPU_ES_DELETEAPP Application="TST_TBL3"
wait 2

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Delete the TST_TBL4 application
/$SC_$CPU_ES_DELETEAPP Application="TST_TBL4"
wait 2

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Delete the TST_SB3 application
/$SC_$CPU_ES_DELETEAPP Application="TST_SB3"
wait 2

if $SC_$CPU_ES_SYSLOGBYTEUSED > logSize then
  ;; Dump the system log
  fullFileName = ramDirLogical & "syslog" & fileCtr
  /$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=fullFileName
  wait 1

  ;; Clear the sys log
  /$SC_$CPU_ES_CLEARSYSLOG
  wait 1

  ;; Get the file to the ground
  fileName = "syslog"&fileCtr
  s ftp_file (ramDir,fileName,fileName,"$CPU","G")
  wait 3

  ;; Delete the file onboard to prevent filling up RAM
  s ftp_file (ramDir,"na",fileName,"$CPU","R")
  wait 2

  ;; Increment the file counter
  fileCtr = fileCtr + 1
endif

;; Delete the TST_SB2 application 
/$SC_$CPU_ES_DELETEAPP Application="TST_SB2"
wait 2

/$SC_$CPU_ES_WRITESYSLOG2FILE SYSLOGFILENAME=ramDirLogical & "syslogfinal"
wait 5

;; Get the System Log files dumped above to the ground
s ftp_file (ramDir,"syslogfinal","syslogfinal","$CPU","G")
wait 3

;; Delete the file onboard to prevent filling up RAM
s ftp_file (ramDir,"na","syslogfinal","$CPU","R")
wait 3

;**********************************************************************
;  End procedure 
;**********************************************************************
ENDPROC 
