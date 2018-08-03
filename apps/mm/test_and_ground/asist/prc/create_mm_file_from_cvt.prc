proc create_mm_file_from_cvt (processor_id, var_id, appid, appidnum, fdescription, filename)

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

;==============================================================================
;
; Purpose: The purpose of this procedure is to write a Memory Manager Load
;          File from the CVT. This procedure assumes that the MM File Header
;	   has already been initialized for the supplied appid
;
; History:
;
; 19MAR08  WFM     Initial development of this proc.
;
;==============================================================================
ON ERROR RESUME

if (%nargs < 6) then
  error "USAGE : CREATE_MM_FILE_FROM_CVT processor_id, var_id, appid, appidnum, fdescription, filename"
  return
endif

local supress

local account = %env("ACCOUNT")
local content_type="1665549617"
local sub_type=8
local length=12
local spacecraft_id="66"
local processor_id
local fdescription

supress=0

; ===========================================================
; Translate from cpu'x' to 'x'
; ===========================================================
if (%length(processor_id) = "4") then
   processor_id = %int(%substring(processor_id,4,4))
endif

if (processor_id > "3") then
  error "processor_id"&processor_id&" is not VALID !!!"
  return
endif

;appid = "P" & appid

;============================================================
local contentVar
local subtypeVar
local lengthVar
local scidVar
local pidVar
local appidVar
local timesecsVar
local timesubsecsVar
local descVar

contentVar = var_id & "CFE_CONTENTTYPE"
{contentVar} = content_type
subtypeVar = var_id & "CFE_SUBTYPE"
{subtypeVar} = sub_type
lengthVar = var_id & "CFE_LENGTH"
{lengthVar} = length
scidVar = var_id & "CFE_SPACECRAFTID"
{scidVar} = spacecraft_id
pidVar = var_id & "CFE_PROCESSORID"
{pidVar} = processor_id
appidVar = var_id & "CFE_APPLICATIONID"
{appidVar} = appidnum
timesecsVar = var_id & "CFE_CREATETIMESECONDS"
{timesecsVar} = %gmt
timesubsecsVar = var_id & "CFE_CREATETIMESUBSECS"
{timesubsecsVar} = 0
descVar = var_id & "CFE_DESCRIPTION"
{descVar} = fdescription

;PEFECFE_CONTENTTYPE = content_type
;PEFECFE_SUBTYPE = sub_type
;PEFECFE_LENGTH = length
;PEFECFE_SPACECRAFTID = spacecraft_id
;PEFECFE_PROCESSORID = processor_id
;PEFECFE_APPLICATIONID = application_id
;PEFECFE_CREATETIMESECONDS = %gmt
;PEFECFE_CREATETIMESUBSECS = 0
;PEFECFE_DESCRIPTION = fdescription

if (supress = "0") then
  write "**********  ", filename, "  **********"
  write
  write "       Content Type: ", p@{contentVar}
  write "           Sub Type: ", {subtypeVar}
  write "             Length: ", {lengthVar}
  write "      Spacecraft Id: ", p@{scidVar}
  write "       Processor Id: ", p@{pidVar}
  write "     Application Id: ", {appidVar}
  write "   Create Time Secs: ", {timesecsVar}
  write "Create Time Subsecs: ", {timesubsecsVar}
  write "   File Description: ", {descVar}
  write
  write "********** MM Load Header **********"
  local hdrVar
  hdrVar = var_id & "ADDROFFSET"
  write "             Offset: ", {hdrVar}
  hdrVar = var_id & "SYMNAME"
  write "            SymName: '", {hdrVar}, "'"
  hdrVar = var_id & "NUMBYTES"
  write "          Num Bytes: ", {hdrVar}
  hdrVar = var_id & "CRC"
  write "                CRC: ", {hdrVar}
  hdrVar = var_id & "MEMTYPE"
  write "        Memory Type: ", p@{hdrVar}
  write
endif

CVT_TO_FILE %name(filename) %name(appid)

;s PARTIAL_CVT_TO_FILE_BETA ({"tblname"}, tbl_appid, start_offset, end_offset) 
;
;write cat_command
;native cat_command
;
;write del_command
;native del_command

ERROR_RETURN:
%liv (log_procedure) = logging

ENDPROC
