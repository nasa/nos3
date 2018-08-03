PROC $sc_$cpu_print_evs_log_file

; History
;
; 06/11/28 nsschweiss Changed CFE_ES_MODE_LOG_ENTRIES to CFE_ES_ER_LOG_ENTRIES.
;

%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"
#define $cpu

write

#ifdef cpu1
write  p@PF00CFE_SPACECRAFTID & " " & p@PF00CFE_PROCESSORID & " " & p@PF00CFE_DESCRIPTION
write
write "File Write Name: ", FILE_LIST[3840].FILE_WRITE_NAME, "                                              TELEMETRY"
write
write "File Write Time (GMT): ", FILE_LIST[3840].file_write_time, "  Create Time (Sec):   ", %rpad(%unsigned(%dec(PF00CFE_CREATETIMESECONDS,12)),7," "), "       Local Evt Log Full Flag: ", p@$sc_$cpu_evs_logfull
write "File Application ID: ", PF00CFE_APPLICATIONID, "                  Create Time (SubSec):  ", %rpad(%unsigned(%dec(PF00CFE_CREATETIMESUBSECS,12)),10," ") , "  Logging Mode: ", p@$sc_$cpu_evs_logmode
write "Content Type: ", p@PF00CFE_CONTENTTYPE, "                                                         Local Evt Log Overflow Ctr: ", $sc_$cpu_evs_logoverflowc
write
#endif

#ifdef cpu2
write  p@PF20CFE_SPACECRAFTID & " " & p@PF20CFE_PROCESSORID & " " & p@PF20CFE_DESCRIPTION
write
write "File Write Name: ", FILE_LIST[3872].FILE_WRITE_NAME, "                                              TELEMETRY"
write
write "File Write Time (GMT): ", FILE_LIST[3872].file_write_time, "  Create Time (Sec):   ", %rpad(%unsigned(%dec(PF20CFE_CREATETIMESECONDS,12)),7," "), "       Local Evt Log Full Flag: ", p@$sc_$cpu_evs_logfull
write "File Application ID: ", PF20CFE_APPLICATIONID, "                  Create Time (SubSec):  ", %rpad(%unsigned(%dec(PF20CFE_CREATETIMESUBSECS,12)),10," ") , "  Logging Mode: ", p@$sc_$cpu_evs_logmode
write "Content Type: ", p@PF20CFE_CONTENTTYPE, "                                                         Local Evt Log Overflow Ctr: ", $sc_$cpu_evs_logoverflowc
write 
#endif

#ifdef cpu3
write  p@PF40CFE_SPACECRAFTID & " " & p@PF40CFE_PROCESSORID & " " & p@PF40CFE_DESCRIPTION
write
write "File Write Name: ", FILE_LIST[3904].FILE_WRITE_NAME, "                                              TELEMETRY"
write
write "File Write Time (GMT): ", FILE_LIST[3904].file_write_time, "  Create Time (Sec):   ", %rpad(%unsigned(%dec(PF40CFE_CREATETIMESECONDS,12)),7," "), "       Local Evt Log Full Flag: ", p@$sc_$cpu_evs_logfull
write "File Application ID: ", PF40CFE_APPLICATIONID, "                  Create Time (SubSec):  ", %rpad(%unsigned(%dec(PF40CFE_CREATETIMESUBSECS,12)),10," ") , "  Logging Mode: ", p@$sc_$cpu_evs_logmode
write "Content Type: ", p@PF40CFE_CONTENTTYPE, "                                                         Local Evt Log Overflow Ctr: ", $sc_$cpu_evs_logoverflowc
write
#endif

write "List of Event Log"
write
write "     Event Time (UTC)      ScId    PrcId    AppID     EvtId   EvtTp    Event Message Text"

for i=1 to CFE_ES_ER_LOG_ENTRIES do 
write " ", %dec(i,2) , "  ", $SC_$CPU_EVS_LOG[i].ccsds_header.u_time.ttime, "   ", %rpad(p@$SC_$CPU_EVS_LOG[i].EVTLOGENTRY.SCID,3," "), "     ", %rpad(p@$SC_$CPU_EVS_LOG[i].EVTLOGENTRY.PRCID,4," "), "     ", %rpad($SC_$CPU_EVS_LOG[i].EVTLOGENTRY.AppName,8," "), "    ", %rpad(p@$SC_$CPU_EVS_LOG[i].EVTLOGENTRY.EVTID,2," "), "    ", %rpad(p@$SC_$CPU_EVS_LOG[i].EVTLOGENTRY.EVTTYPE,4," "), "     ", p@$SC_$CPU_EVS_LOG[i].EVTMSG
ENDDO
write

%liv (log_procedure) = TRUE
ENDPROC

