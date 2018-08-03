PROC $sc_$cpu_print_sb_pipes(stepnum, totalpipes)
;**********************************************************************
;  Proc Name:  $sc_$cpu_print_sb_pipes
;
;  Proc Description
;       This procedure will print out the routing inforation for the 
;       SB Test Pipes.
;
;  Change History
;
;	Date		Name			Description
;	8/11/2005  	Ingrid Albright		Original Procedure
;
;  Arguments
;	
;	Name		Description
;       stepnum         String that gets appended to the get_file_to_cvt
;                       filename to make it unique.
;       totalpipes      The number of test SB pipes that should be printed in
;			the table.
;
;  Procedures Called
;
;	Name			Description
;       get_file_to_cvt         Gets the routing table from the FSW.
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "cfe_platform_cfg.h"

%liv (log_procedure) = logging

local outputfilename
local numpipes[0..totalpipes]

outputfilename = "$sc_$cpu_sb_pipe_" & stepnum & ".dat"

start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", outputfilename, "$CPU")

%liv (log_procedure) = FALSE

for i=0 to totalpipes do
   numpipes[i] = 0
enddo
for i=1 to CFE_SB_MAX_MSG_IDS do
   for j=0 to totalpipes do
      if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe"&j) then
         numpipes[j] = numpipes[j]+1
      endif
   enddo
enddo  

local pipelimit
for i=0 to totalpipes do
  write ">>> numpipes[",i,"] = ", numpipes[i]
  if (numpipes[i] != 0) then
    pipelimit = i
  endif
enddo
write ">>> pipelimit = ", pipelimit

local arrayat[0..pipelimit]
for i=0 to pipelimit do
  arrayat[i] = 1
  if (numpipes[i] != 0) then
    local {"pipearray_"&i}[1..numpipes[i]]
  else
    local {"pipearray_"&i}[1..1]
  endif
enddo

;;for i=0 to pipelimit do
;;  arrayat[i] = 1
;;enddo
for i=1 to CFE_SB_MAX_MSG_IDS do
   for j=0 to pipelimit do
      if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe"&j) then
         {"pipearray_"&j}[arrayat[j]] = i
         arrayat[j] = arrayat[j]+1
      endif
   enddo
enddo

write "# ",stepnum," ###########################################"
write "Pipe         Pipe Id   Msg Id   Msg Count   State"
for j=0 to pipelimit do
   for i=1 to numpipes[j] do
      write $SC_$CPU_SB_RouteEntry[{"pipearray_"&j}[i]].SB_PIPENAME, "   ", ;;
      %hex($SC_$CPU_SB_RouteEntry[{"pipearray_"&j}[i]].SB_PIPEID,2), "        ", ;;
      %hex($SC_$CPU_SB_RouteEntry[{"pipearray_"&j}[i]].SB_MSGID,4), "     ", ;;
      $SC_$CPU_SB_RouteEntry[{"pipearray_"&j}[i]].SB_MSGCNT, "           ", ;;
      p@$SC_$CPU_SB_RouteEntry[{"pipearray_"&j}[i]].SB_STATE   
   enddo
enddo
write "#################################################"

%liv (log_procedure) = logging
ENDPROC
