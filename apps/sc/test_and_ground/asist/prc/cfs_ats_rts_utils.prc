PROC cfs_ats_rts_utils
#define FTF_TYPE ".ftf"

   global rts_length
   GLOBAL rts_table_id
   global rts_enable_on_load=0
   global sc_load_full_tables=1

   DIRECTIVE compile_rts(filename,table_number) IS
   BEGIN
      local ftf_name,scs_name

      ftf_name=filename & ".ftf"
      scs_name=filename & ".scs"
      SCP -processor=cfs -swap=n -rts={table_number} {scs_name} {ftf_name}
    END

    DIRECTIVE compile_ats(filename,which,startTime) IS
    BEGIN
         local ftf_name,scs_name

         ftf_name=filename & ".ftf"
         scs_name=filename & ".scs"
	 if (%nargs = 2) then
           SCP -processor=cfs -swap=n -ats={which} {scs_name} {ftf_name}
	 else
           SCP -processor=cfs -swap=n -ats={which} -DSTART={startTime} {scs_name} {ftf_name}
	 endif
   END

ENDPROC
