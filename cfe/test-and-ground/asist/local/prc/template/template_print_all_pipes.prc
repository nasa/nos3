PROC $sc_$cpu_print_all_pipes(stepnum)
;**********************************************************************
;  Proc Name:  $sc_$cpu_print_all_pipes
;
;  Proc Description
;       This procedure will print out the routing inforation for the 
;       all of the pipes in the routing table.
;
;  Change History
;
;	Date		Name			Description
;	8/30/2005  	Ingrid Albright		Original Procedure
;
;  Arguments
;	
;	Name			Description
;       stepnum                 String that gets appended to the getfiletocvt
;                               filename to make it unique.
;
;  Procedures Called
;
;	Name			Description
;       get_file_to_cvt         Gets the routing table from the FSW.
;
;**********************************************************************
%liv (log_procedure) = FALSE

local outputfilename
local totalpipes = 256
local pipes[0..totalpipes]

outputfilename = "$sc_$cpu_all_pipe_" & stepnum & ".dat"

start get_file_to_cvt("RAM:0", "cfe_sb_route.dat", outputfilename, "$CPU")

%liv (log_procedure) = FALSE

write "# ",stepnum," ###########################################"
write "Pipe            Pipe Id   Msg Id   Msg Count   State"
for i=1 to totalpipes do
   if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME <> "") then
      write %rpad($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME, 16, " "), ;;
       %hex($SC_$CPU_SB_RouteEntry[i].SB_PIPEID,2), "        ", ;;
       %hex($SC_$CPU_SB_RouteEntry[i].SB_MSGID,4), "     ", ;;
       %rpad($SC_$CPU_SB_RouteEntry[i].SB_MSGCNT, 12, " "), ;;
       p@$SC_$CPU_SB_RouteEntry[i].SB_STATE   
   endif
enddo
write "#################################################"

%liv (log_procedure) = TRUE
ENDPROC
