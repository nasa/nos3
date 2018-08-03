PROC partial_cvt_to_file_beta(filename,variable,start_offset,end_offset)

        local supress = 1
        external spacecraft_id
        global gse_dump_to_cvt_options
        local the_command,where

       if(%nargs < 4) then
           error "Usage: PARTIAL_CVT_TO_FILE filename,variable,start_offset,end_offset"
        endif
        where=%env("WORK") & "/image"
        the_command="cd " & where & ";cvt2file "
        the_command = the_command & " -s " & start_offset
        the_command = the_command & " -e " & end_offset
        the_command = the_command & " " & variable
        the_command = the_command & " " & %lower(filename)
        write "The unix command is ",the_command
        native the_command

global gse_file_to_cvt_options

ENDPROC
