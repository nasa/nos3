proc create_tbl_file_from_cvt (processor_id, application_id, fdescription, filename, tblname, start_mnemonic, end_mnemonic, spacecraft_id)
;
local logging = %liv(log_procedure)
%liv (log_procedure) = FALSE
;
;==============================================================================
;
; Purpose: The purpose of this procedure is to write a table 
;          file header from the CVT.
;
; History:
;
; 18JAN06          Initial development of this proc.          SMA
; 08FEB06          Added partial dump capability.             SMA
; 09FEB06          Correct byte offset calculations.          SMA   
;                  Calculate cfe_file_and_tbl_hdr_offset.     
;                  Change filename parameters to lowercase 
;                  as needed. 
; 23MAR06          Change proc name from write_file to        SMA
;                  create_tbl_file_from_cvt. Move arguments
;                  check.                                  
; 08JAN08          Removed the _BETA from Get_File_to_CVT     WFM
;		   since the new ASIST version is not a BETA.
; 08JAN08          Added spacecraft_id argument.              WFM
;
ON ERROR RESUME

if (%nargs < 7) then
  error "USAGE : CREATE_TBL_FILE_FROM_CVT processor_id, application_id, fdescription, filename, tblname, start_mnemonic, end_mnemonic, spacecraft_id"
  goto ERROR_RETURN
elseif (%nargs = 7) then
  spacecraft_id="66"
endif

local supress

local account = %env("ACCOUNT")
local appid = "P0EFE"
local tbl_appid = "P" & %hex(telemetry_attr(start_mnemonic, "APID"),4)
local content_type="1665549617"
local sub_type=8
local length=12
;;local spacecraft_id="66"
;;local processor_id
;;local application_id
;;local fdescription
local tdescription
local temp_filename = %lower(filename) & ".tmp"
local start_offset =  (telemetry_attr(start_mnemonic, "packet_offset"))
local end_offset =  (telemetry_attr(end_mnemonic, "packet_offset"))  + (telemetry_attr(end_mnemonic,"length") - 1)
local cfe_file_and_table_hdr_offset = telemetry_attr(appid,"length")

local cat_command 
cat_command = "cd /s/opr/accounts/" & account & "/image;"
cat_command = cat_command & "cat " & temp_filename & " " & %lower(tblname)  & " > " & %lower(filename) 

local del_command
del_command = "cd /s/opr/accounts/" & account & "/image;"
del_command = del_command & "rm " & %lower(tblname) & " " & temp_filename

supress=0

; ===========================================================
; Translate from cpu'x' to 'x'
; ===========================================================
if (%length(processor_id) = "4") then
   processor_id = %int(%substring(processor_id,4,4))
endif

if (processor_id > "3") then
  error "processor_id"&processor_id&" is not VALID !!!"
  goto ERROR_RETURN
endif
;============================================================

PEFECFE_CONTENTTYPE = content_type
PEFECFE_SUBTYPE = sub_type
PEFECFE_LENGTH = length
PEFECFE_SPACECRAFTID = spacecraft_id
PEFECFE_PROCESSORID = processor_id
PEFECFE_APPLICATIONID = application_id
PEFECFE_CREATETIMESECONDS = %gmt
PEFECFE_CREATETIMESUBSECS = 0
PEFECFE_DESCRIPTION = fdescription
PEFECFE_BYTEOFFSET = start_offset - cfe_file_and_table_hdr_offset
PEFECFE_NUMBYTES= (end_offset - start_offset + 1)
PEFECFE_TBLNAME= tblname

if (supress = "0") then
  write "**********  ", filename, "  **********"
  write
  write "       Content Type: ", p@PEFECFE_CONTENTTYPE
  write "           Sub Type: ", PEFECFE_SUBTYPE 
  write "             Length: ", PEFECFE_LENGTH
  write "      Spacecraft Id: ", p@PEFECFE_SPACECRAFTID
  write "       Processor Id: ", p@PEFECFE_PROCESSORID 
  write "     Application Id: ", p@PEFECFE_APPLICATIONID 
  write "   Create Time Secs: ", PEFECFE_CREATETIMESECONDS
  write "Create Time Subsecs: ", PEFECFE_CREATETIMESUBSECS 
  write "   File Description: ", PEFECFE_DESCRIPTION
  write 
  write "**********  ", tblname, "  **********"
  write 
  write "     Start Mnemonic: ", start_mnemonic
  write "        Byte Offset: ", PEFECFE_BYTEOFFSET
  write "       End Mnemonic: ", end_mnemonic
  ; write "             Offset: ", end_offset
  write "    Number of Bytes: ", PEFECFE_NUMBYTES
  write
endif

CVT_TO_FILE %name(temp_filename) %name(appid)

; CVT_TO_FILE_BETA %name(tblname) %name(tbl_appid)

s PARTIAL_CVT_TO_FILE_BETA ({"tblname"}, tbl_appid, start_offset, end_offset) 

write cat_command
native cat_command

write del_command
native del_command

ERROR_RETURN:

%liv (log_procedure) = logging

ENDPROC
