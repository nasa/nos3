PROC login_exit_dirs

     DIRECTIVE CFE_Startup (cpu) IS
       alias LRO_Startup
     BEGIN

     #include "cfe_utils.h"

     ON ERROR RESUME

     local cpu_value

     ; ===================================
     ; Translate from cpu'x' to 'x'
     ; ===================================

     if  (%length(cpu) = "0") then
       if (MISSION = "CFE") then
          error "Please choose CPU 1,2,or 3"
          return
       endif
       cpu = 1
     endif
     if (%length(cpu) = "4") then
        cpu = %substring(cpu,4,4)
     endif

     if (cpu > "3") or (cpu < "1") then
        error "CPU"&cpu&" is not VALID !!!"
        return
     endif

     cpu_value = "cpu" & cpu

     
//     start_data_center
     dc_open_server(5002)
     wait until (GSE_ICHAN = 1) and (MDTFWDLINK = 1)
     open_tlm
     open_cmd {cpu_value}

     END

     DIRECTIVE CFE_Shutdown IS
       alias LRO_Shutdown
     BEGIN

     ON ERROR RESUME

     close_data_center
     wait until (GSE_ICHAN = 2) and (MDTFWDLINK = 2)
     native "rstol exit; rstol y"

     END

ENDPROC
