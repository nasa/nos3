PROC $sc_$cpu_load_ats_rts(filename,loadfilename,create_flag,full_tbl_flag)
#define FTF_TYPE ".ftf"

ON ERROR resume

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

   global rts_length
   GLOBAL rts_table_id
   global rts_enable_on_load=0

   EXTERNAL citlfile, citnwds, spacecraft_id
   GLOBAL bytes_per_uaddr, bytes_per_daddr, bytes_per_value
   local start_addr,end_addr,line_buffer,this_addr,this_data,this_length
   LOCAL loc_fname, fhandle, description, status, load_type, dump_size
   LOCAL fname_length, size_units, addr_units, file_spacecraft_id
   local where_in_rts,sc_length
   local processor="",table_id="",memory_type=""
   local array_length,data_array[200]
   local pkt_id

   if (%nargs = 2) then
     create_flag = 1
     full_tbl_flag = 0
   elseif (%nargs = 3) then
     full_tbl_flag = 0
   endif

   loc_fname = %ENV("STOL_IMAGE") & "/" & filename & ".ftf"
   fhandle = FILE_OPEN(loc_fname)
   IF (fhandle < 0) THEN
      ERROR "Can't open load file " & loc_fname
      goto procterm
   ENDIF
   where_in_rts=1
   WRITE "LOAD: file is " & loc_fname

   // First, read three header lines...description
   description = get_load_description_record(fhandle)
   IF (%NWORDS(description) = 9 or %NWORDS(description) = 11 or ;;
       %NWORDS(description) = 12) THEN
       IF (processor = "")   processor = %WORD(description, 1)
       IF (table_id = "")    table_id  = %WORD(description, 5)
       IF (memory_type = "") memory_type = %WORD(description, 9)
       IF (load_type = "")   load_type = %WORD(description, 4)

       IF (%WORD(description, 11) != "") THEN
          bytes_per_value = %CONVERT(%WORD(description, 11), 10)
       ELSE
          bytes_per_value = 1
       ENDIF

       bytes_per_daddr = 1
       IF (start_addr = "") THEN
          start_addr = %CONVERT(%WORD(description, 8), 16)
          IF (%WORD(description, 10) != "") THEN
             bytes_per_daddr = %CONVERT(%WORD(description, 10), 10)
             start_addr = start_addr * bytes_per_daddr
          ENDIF
       ELSE
          start_addr = start_addr * bytes_per_uaddr
       ENDIF
         
       IF (end_addr = "") THEN
          end_addr = start_addr + dump_size - bytes_per_value
       ELSE
          end_addr = end_addr * bytes_per_uaddr
       ENDIF
         
       IF (start_addr MOD 2 != 0) THEN
          ERROR "LOAD start address must be even"
          goto procterm
       ENDIF
   ELSE
       citlstte = 0
       sch_ldst = 0
       citlabrt = 1
       ERROR "Invalid DESCRIPTION record"
       goto procterm
   ENDIF
   printf "Processor=%s, Tableid=%s, MemType=%s",processor,table_id,memory_type
   status=0
   while (status >= 0) do
       status = FILE_READ(fhandle, line_buffer)
       if (status = -1) then
          break   // End of file
       ELSEIF (status = -2) then
          status=file_close(fhandle)
          error "Problem reading file...bye"   
 	  goto procterm
       ELSEIF (%SUBSTRING(line_buffer, 1, 1) = ";") THEN
          CONTINUE
       endif
       IF (%NWORDS(line_buffer) = 3) THEN
          // Its of form <addr>,<#words>,<data>
          this_addr = %CONVERT(%WORD(line_buffer, 1), 16)*bytes_per_daddr
          this_length = %CONVERT(%WORD(line_buffer,2),16)*bytes_per_value
          this_data = %SUBSTRING(%WORD(line_buffer,3),1,this_length*2)
          array_length = %length(this_data)/4
          IF (%DEFINED("data_array") AND      ;;
              %DIMENSION(data_array,1) != array_length) THEN
             DROP data_array
             LOCAL data_array[array_length]
          ENDIF
            
          printf "Addr=%8X, Length=%d, Data=%s",this_addr,this_length,this_data
          data_array = DECODE_HEX(this_data,2)

	  ;; load the CVT with the data
          FOR i=1 to this_length/2 do
            if (load_type = "SCPRTS") then
               $SC_$CPU_SC_RTSDATA[where_in_rts] = data_array[i]
            else
               $SC_$CPU_SC_ATSDATA[where_in_rts] = data_array[i]
            endif   
            where_in_rts=where_in_rts+1
          enddo
       ELSE
          status=file_close(fhandle)
          ERROR "Invalid DATA record -->" & line_buffer
          goto procterm
       ENDIF
   ENDDO
   status=file_close(fhandle)
   rts_length=where_in_rts-1

   local tbl_name,bin_file_name

   bin_file_name=filename&".bin"
   if (load_type = "SCPRTS") then
      tbl_name=sprintf("SC.RTS_TBL%03d",%int(table_id)) ;
      if (full_tbl_flag) then
         sc_length=telemetry_attr("$SC_$CPU_SC_RTSDATA","DIMENSION")
         for i= rts_length+1 to sc_length do
            $SC_$CPU_SC_RTSDATA[i]=0
         enddo
         rts_length=sc_length
      endif
      local lastname=sprintf("$SC_$CPU_SC_RTSDATA[%d]",rts_length)
      local firstname= "$SC_$CPU_SC_RTSDATA[1]"
      pkt_id = "0FBC"
      if ("$CPU" = "CPU2") then
	pkt_id = "0FDA"
      elseif ("$CPU" = "CPU3") then
	pkt_id = "0FFA"
      endif
   else
      if (full_tbl_flag) then
         sc_length=telemetry_attr("$SC_$CPU_SC_ATSDATA","DIMENSION")
         for i= rts_length+1 to sc_length do
            $SC_$CPU_SC_ATSDATA[i]=0
         enddo
         rts_length=sc_length
      endif
      local lastname=sprintf("$SC_$CPU_SC_ATSDATA[%d]",rts_length)
      local firstname= "$SC_$CPU_SC_ATSDATA[1]"
      if (table_id = "0001") then
         tbl_name="SC.ATS_TBL1"
      else
         tbl_name="SC.ATS_TBL2"
      endif
      pkt_id = "0FBB"
      if ("$CPU" = "CPU2") then
	pkt_id = "0FD9"
      elseif ("$CPU" = "CPU3") then
	pkt_id = "0FF9"
      endif
   endif

   ;; Create the load file if the Flag indicates '1'
   if (create_flag = 1) then
     start create_tbl_file_from_cvt ("$CPU",pkt_id,"Time Sequence Load file", ;;
                loadfilename,tbl_name,firstname,lastname)
   endif

procterm:
;; Restore procedure logging
%liv (log_procedure) = logging

ENDPROC
