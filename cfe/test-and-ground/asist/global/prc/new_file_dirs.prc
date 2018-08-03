PROC new_file_dirs

     DIRECTIVE File_to_CVT_Beta(filename,variable) IS
     BEGIN
        file_to_cvt filename variable
     END
     DIRECTIVE old_File_to_CVT_Beta(filename,variable) IS
     BEGIN
        external spacecraft_id
        global gse_file_to_cvt_options
        local appid_number
        local the_command,where,the_date_command
        local supress
        local cfecore
        
        supress = 1
        cfecore = 1

        if(%nargs < 2) then
           error "Usage: FILE_TO_CVT filename,variable"
        endif

        where=%env("WORK") & "/image"

        if (cfecore = "1") then
          appid_number = telemetry_attr(variable,"APID")
          the_date_command = "cvt -ws file_list[" & appid_number
          the_date_command = the_date_command  & "].file_write_time "
          the_date_command = the_date_command & """`date +%y-%j-%T -r " 
          the_date_command = the_date_command & where  & "/" 
          the_date_command = the_date_command & %lower(filename) & "`"""
          if (supress = "0") then
             write "The unix command is ", the_date_command
          endif
          native the_date_command
          file_list[appid_number].file_write_name = %lower(filename)
        endif
        
        the_command="cd " & where & ";file2cvt " & gse_file_to_cvt_options
        the_command = the_command & " " & variable
        the_command = the_command & " " & %lower(filename)
        if (supress = "0") then
          write "The unix command is ",the_command
        endif
        native the_command
        
     END

     DIRECTIVE CVT_to_file_beta(filename,variable) IS
     BEGIN
        cvt_to_file filename variable
     END
     DIRECTIVE old_CVT_to_file_beta(filename,variable) IS
     BEGIN
        external spacecraft_id
        global gse_dump_to_cvt_options
        local the_command,where

       if(%nargs < 2) then
           error "Usage: CVT_TO_FILE filename,variable"
        endif
        where=%env("WORK") & "/image"
        the_command="cd " & where & ";cvt2file_beta "
        the_command = the_command & " " & variable
        the_command = the_command & " " & %lower(filename)
        write "The unix command is ",the_command
        native the_command
     END

global gse_file_to_cvt_options
 
;  gse_file_to_cvt_options = ""

ENDPROC
