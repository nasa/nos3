PROC $sc_$cpu_check_sb_msgcnt(stage, stepnum, msgid, change[])
;**********************************************************************
;  Proc Name:  $sc_$cpu_check_sb_msgcnt
;
;  Proc Description
;       This procedure will check whether the SB Test Pipes' (of specified
;       message id) message count change is as expected.
;
;  Change History
;
;	Date		Name			Description
;	8/11/2005  	Ingrid Albright		Original Procedure
;
;  Arguments
;	
;	Name		Description
;       stage           Determines whether to save the current message counts
;			or to compare the input count change to previously
;			saved values.
;       stepnum         String that gets appended to the get_file_to_cvt
;                       filename to make it unique.
;       msgid           The message id of which pipes the msg count should be
;			looked at.
;       change          Array that holds the expected difference in message
;			count between the saved and current values.
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

local outputfilename
local totalpipes = 4
global pipes_msgcnt[1..totalpipes]
global pipes_name[1..totalpipes]
global ing_result
local result_array[1..totalpipes]
local new_msgcnt[1..totalpipes]
local at

local ramDir = "RAM:0"
local hostCPU = "$CPU"

outputfilename = "$sc_$cpu_sb_pipe_" & stepnum & "_" & stage & ".dat"

start get_file_to_cvt(ramDir, "cfe_sb_route.dat", outputfilename, hostCPU)

%liv (log_procedure) = FALSE

if (stage = "set") then
   
   for j=1 to totalpipes do
      pipes_msgcnt[j] = 0
      pipes_name[j] = "N/A"
   enddo

   at = 1
   for i=1 to CFE_SB_MAX_MSG_IDS do
      for j=1 to totalpipes do
         if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe"&j) then
            if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = msgid) then
               pipes_msgcnt[j] = $SC_$CPU_SB_RouteEntry[i].SB_MSGCNT
               pipes_name[j] = $SC_$CPU_SB_RouteEntry[i].SB_PIPENAME
               if (j > at) then
                  at = j
               endif
            endif
         endif
      enddo
   enddo  
   
elseif (stage = "check") then

   for j=1 to totalpipes do
      new_msgcnt[j] = 0
   enddo

   at = 1
   for i=1 to CFE_SB_MAX_MSG_IDS do
      for j=1 to totalpipes do
         if ($SC_$CPU_SB_RouteEntry[i].SB_PIPENAME = "TstSBPipe"&j) then
            if ($SC_$CPU_SB_RouteEntry[i].SB_MSGID = msgid) then
               new_msgcnt[j] = $SC_$CPU_SB_RouteEntry[i].SB_MSGCNT
               if (j > at) then
                  at = j
               endif
            endif
         endif
      enddo
   enddo  
   
   write "*** Pipes subscribed to Message ID ", %hex(msgid,4)
   for i=1 to at do
      if (new_msgcnt[i] = (pipes_msgcnt[i]+change[i])) then
         write "*** ", pipes_name[i]," ",pipes_msgcnt[i],"->",new_msgcnt[i]," PASS"
         result_array[i] = true
      else
         write "*** ", pipes_name[i]," ",pipes_msgcnt[i],"->",new_msgcnt[i]," FAIL"
         result_array[i] = false
      endif
   enddo
   for i=(at+1) to totalpipes do
      result_array[i] = true
   enddo

   ing_result = (result_array[1] AND result_array[2] AND result_array[3] AND result_array[4])
endif

%liv (log_procedure) = logging
ENDPROC
