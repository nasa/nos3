PROC $sc_$cpu_fm_runspecialchartests (directorySpec)
;*******************************************************************************
;  Proc Name:  FM_RunStressTests
;  Test Level: none 
;		     
;  Proc Description
;	The purpose of this procedure is execute the FM Special Character test
;	procedures. The procedures are executed one after the other until
;	completed.
;
;  Change History
;	Date	   Name		Description
;	08/22/11   W. Moleski	Original Procedure
;	01/06/15   W. Moleski	Added the Argument description below
;
;  Arguments
;       directorySpec   The name of an existing directory under the test_logs
;                       directory to store the generated log files.
;                       NOTE: If specified and the directory does not exist, the
;                             log files will be lost.
;
;**********************************************************************

; Determine if proc was called with minimum # of parameters
if (%nargs < 1) then
  directorySpec = ""
endif

write ";*********************************************************************"
write ";  Executing SpecialChars1 procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_specialchars1",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_specialchars1")
endif
wait 2

write ";*********************************************************************"
write ";  Executing SpecialChars2 procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_specialchars2",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_specialchars2")
endif
wait 2

write ";*********************************************************************"
write ";  Executing SpecialChars3 procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_specialchars3",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_specialchars3")
endif
wait 2

write ";*********************************************************************"
write ";  Executing SpecialChars4 procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_specialchars4",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_specialchars4")
endif
wait 2

write ";*********************************************************************"
write ";  Executing SpecialChars5 procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_specialchars5",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_specialchars5")
endif
wait 2

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_runspecialchartests "
write ";*********************************************************************"
ENDPROC
