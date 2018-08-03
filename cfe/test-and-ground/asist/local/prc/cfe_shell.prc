PROC cfe_shell(which)
;==============================================================================
;
; Purpose: The purpose of this procedure is to startup a command shell to the
;	   underlying operating system. This procedure calls the ASIST program
;	   'ashell' which displays a window allowing the operator to issue
;	   commands.
;
; History:
;
; 13SEP07	Added this prolog to this procedure.	WFM
;
;==============================================================================

   if which="" then
      which="CPU1"
   endif
   if which="CPU1" then
      native "gnome-terminal -t 'CFE Shell' -e 'ashell CPU1_SHELL_MESSAGE ""/SCX_CPU1_ES_SHELL OUTPUTFILENAME=\""\"" CMDSTRING=""'"
   elseif which="CPU2" then
      native "gnome-terminal -t 'CFE Shell' -e 'ashell CPU2_SHELL_MESSAGE ""/SCX_CPU2_ES_SHELL OUTPUTFILENAME=\""\"" CMDSTRING=""'"
   elseif which="CPU3" then
      native "gnome-terminal -t 'CFE Shell' -e 'ashell CPU3_SHELL_MESSAGE ""/SCX_CPU3_ES_SHELL OUTPUTFILENAME=\""\"" CMDSTRING=""'"
   endif

ENDPROC
