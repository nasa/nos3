PROC $sc_$cpu_fm_runbuildtests (directorySpec)
;*******************************************************************************
;  Proc Name:  FM_RunBuildTests
;  Test Level: none 
;		     
;  Proc Description
;   The purpose of this procedure is execute the FM build test procedures. The
;   procedures are executed one after the other until complete.
;
;  Change History
;	Date	   Name		Description
;	01/06/15   W. Moleski	Original Procedure
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
write ";  Executing DirCmds_basic procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_dircmds_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_dircmds_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing Dirrename procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_dirrename",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_dirrename")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filecat_basic procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filecat_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filecat_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filecopy_basic procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filecopy_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filecopy_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filedecom_basic procedure                               "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filedecom_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filedecom_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing fileinfo_basic procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_fileinfo_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_fileinfo_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filemove_basic procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filemove_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filemove_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing filerename_basic procedure                              "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_filerename_basic",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_filerename_basic")
endif
wait 2

write ";*********************************************************************"
write ";  Executing GenCmds procedure                              "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_gencmds",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_gencmds")
endif
wait 2

write ";*********************************************************************"
write ";  Executing OpenFiles procedure                              "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("$sc_$cpu_fm_openfiles",directorySpec)
else
  s ut_runproc("$sc_$cpu_fm_openfiles")
endif
wait 2

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_runbuildtests                           "
write ";*********************************************************************"
ENDPROC
