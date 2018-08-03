PROC $sc_$cpu_fm_runstresstests (directorySpec)
;*******************************************************************************
;  Proc Name:  FM_RunStressTests
;  Test Level: none 
;		     
;  Proc Description
;   The purpose of this procedure is execute the FM stress test procedures. The
;   procedures are executed one after the other until complete.
;
;  Change History
;	Date	   Name		Description
;	12/16/08   W. Moleski	Original Procedure
;	01/06/15   W. Moleski	Added directorySpec argument to allow the caller
;				to specify where the log files are stored.
;
;  Arguments
;	directorySpec	The name of an existing directory under the test_logs
;			directory to store the generated log files.
;			NOTE: If specified and the directory does not exist, the
;			      log files will be lost.
;
;**********************************************************************

; Determine if proc was called with minimum # of parameters
if (%nargs < 1) then
  directorySpec = ""
endif

write ";*********************************************************************"
write ";  Executing DirCmds_stress procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_dircmds_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_dircmds_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filecat_stress procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filecat_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filecat_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filecopy_stress procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filecopy_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filecopy_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filedecom_stress procedure                               "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filedecom_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filedecom_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filedelete_stress procedure                              "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filedelete_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filedelete_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing fileinfo_stress procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_fileinfo_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_fileinfo_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filemove_stress procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filemove_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filemove_stress")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filerename_stress procedure                              "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filerename_stress",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filerename_stress")
endif
wait 2

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_runstresstests                           "
write ";*********************************************************************"
ENDPROC
