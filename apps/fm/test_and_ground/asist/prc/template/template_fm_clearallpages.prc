PROC $sc_$cpu_fm_clearallpages
;*******************************************************************************
;  Proc Name:  FM_ClearAllPages
;  Test Level: none 
;		     
;  Proc Description
;   The purpose of this procedure is clear all of the pages associated with FM
;
;  Pages Closed:  FM HK Page
;                 FM File Info Page
;                 FM Dir List Tlm Page
;                 FM Dir List File Page
;                 FM Open File List Page
;                 FM Free Space Table Page
;                 FM Free Space Tlm Page
;                 TST FM HK Page
;
;
;  Change History
;
;	Date		Name		Description
;	08/08/08	D. Stewart	Original Procedure
;	06/18/09	W. Moleski	Created a template for this proc
;	03/04/10	W. Moleski	Added clear of the Free Space Tlm page
;
;  Arguments
;	None
;
;**********************************************************************

write ";*********************************************************************"
write ";  Clear all FM and TST_FM pages"
write ";*********************************************************************"
wait 5

write "Clearing FM HK Page."
clear $SC_$CPU_FM_HK

write "Clearing FM File Info Page."
clear $SC_$CPU_FM_FILE_INFO

write "Clearing FM Dir List File Page."
clear $SC_$CPU_FM_DIR_LIST_FILE

write "Clearing FM Dir List Tlm Page."
clear $SC_$CPU_FM_DIR_LIST

write "Clearing FM Open File List Page."
clear $SC_$CPU_FM_OPEN_FILE_LIST

write "Clearing FM Free Space Table Page."
clear $SC_$CPU_FM_FREESPACE_TABLE

write "Clearing FM Free Space TLM Page."
clear $SC_$CPU_FM_FREESPACE_TLM

write "Clearing TST FM HK Page."
clear $SC_$CPU_TST_FM_HK

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_clearallpages  "
write ";*********************************************************************"
ENDPROC
