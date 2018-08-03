proc $sc_$cpu_fm_dirfiledisplay (directory, filename, downloadDir, DLfilename, expResult)
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the Dir Listing to File
;          cmd, download the file if one exists, and then write the contents
;          to the log.
;
; Parameters:  directory has to be fully qualified 
;              filename has to be fully qualified 
;              downloadDir is of the form RAM:0/dir for use in the ftp_file proc
;              DLfilename is the name of the file found at downloadDir
;
; History:
;
;       Date	   Name         Description 
; 	08/05/08   D. Stewart	Initial development of this proc.
; 	01/14/10   W. Moleski   Added an optional 5th argument to indicate the
; 	05/10/12   W. Moleski   Fixed the ftp_file spec to use the DLFilename
;				as the second argument since filename is the 
;				fully qualified name to use in the command.
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG
;==============================================================================

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "fm_events.h"
#include "fm_platform_cfg.h"

%liv (log_procedure) = logging

;; If the optional argument is not specified, expect Pass
if (%nargs = 4) then
  expResult = "Pass"
endif

local cmdctr, errcnt, childCmdCtr
local actualResult

;; Determine the Packet IDs for the FM Dir List file CVT
local fileAppPktID
local idBasedOnSC

;; CPU1 is the default
fileAppPktID = "P0F0C"
idBasedOnSC = "PF0C"

if ("$CPU" = "CPU2") then
  fileAppPktID = "P0F2C"
  idBasedOnSC = "PF2C"
elseif ("$CPU" = "CPU3") then
  fileAppPktID = "P0F4C"
  idBasedOnSC = "PF4C"
endif

local tokenType = ""
local numFilesWritEM
local currentDis
local maxToDis

;****************************************
;  start of sub-proc
;****************************************
; set up the expected event messages and check them in calling proc if needed
ut_setupevents "$SC", "$CPU", "FM", FM_GET_DIR_FILE_CMD_EID, "DEBUG", 5

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1
childCmdCtr = $SC_$CPU_FM_CHILDCMDPC + 1

/$SC_$CPU_FM_DirListFile DirName=directory File=filename

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))   
if (($SC_$CPU_FM_CMDPC = cmdctr) AND ($SC_$CPU_FM_CHILDCMDPC = childCmdCtr)) then
  actualResult = "Pass"
  if (expResult = "Pass") then
    write "<*> Passed - Dir Listing to File command Accepted."

    if ($SC_$CPU_find_event[5].num_found_messages = 1) then
      write "<*> Passed - Event message ", $SC_$CPU_find_event[5].eventid," received"
    else
      write "<!> Failed - Event message ",FM_GET_DIR_FILE_CMD_EID," was not received"
    endif
  else
    write "<!> Failed - Dir Listing to File command Accepted when failure was expected."
  endif
else
  actualResult = "Fail"
  if (expResult = "Fail") then
    write "<*> Passed - Dir Listing to File command Rejected."
  else
    write "<!> Failed - Dir Listing to File command Rejected when success was expected."
  endif
endif

wait 5

;; if the Dir List command passed, parse the file
if (actualResult = "Pass") then
  ; Dump the file that was created
  ;; Check if the default file was substituted
  if (filename = "") then
    filename = "fm_dirlist.out"
  endif
  s ftp_file (downloadDir, DLfilename, DLfilename, "$CPU", "G")
  wait 15

  ; populate the tlm page
  FILE_TO_CVT %name(DLfilename) %name(fileAppPktID)

  local appid_number
  local the_command,where,the_date_command

  where=%env("WORK") & "/image"

  appid_number = telemetry_attr(fileAppPktID,"APID")
  file_list[appid_number].file_write_name = %lower(DLfilename)
  the_date_command = "cvt -ws file_list[" & appid_number
  the_date_command = the_date_command  & "].file_write_time "
  the_date_command = the_date_command & """`date +%y-%j-%T -r "
  the_date_command = the_date_command & where  & "/"
  the_date_command = the_date_command & %lower(DLfilename) & "`"""
  native the_date_command

  wait 5

  write ""
  write "Dir Listing to File produced a file"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) then
    ; using the %token(identifier[, string]) directive
    ; parse Event message for the number of files written after it is received 
    tokenType = %token(numFilesWritEM,$SC_$CPU_find_event[5].event_txt)

    DO UNTIL tokenType = "INTEGER"
      tokenType = %token(numFilesWritEM)
    ENDDO

    maxToDis = $SC_$CPU_FM_NumFilesWritten
    if (numFilesWritEM <> maxToDis) then
      write " Mismatched number of written files, from EM: ", numFilesWritEM, " From file: ", maxToDis
      write " Using number from EM"
      maxToDis = numFilesWritEM
    endif
  else
    maxToDis = 5
  endif

  write ""
  write "Contents of the Directory Listing File:"
  write ""
  write " File header:"
  write "  CFE_ContentType         = ",p@%name(idBasedOnSC & "CFE_ContentType")
  write "  CFE_SubType             = ", %name(idBasedOnSC & "CFE_SubType")
  write "  CFE_Length              = ", %name(idBasedOnSC & "CFE_Length")
  write "  CFE_SpacecraftID        = ",p@%name(idBasedOnSC & "CFE_SpacecraftID")
  write "  CFE_ProcessorID         = ",p@%name(idBasedOnSC & "CFE_ProcessorID")
  write "  CFE_ApplicationID       = ", %name(idBasedOnSC & "CFE_ApplicationID")
  write "  CFE_CreateTimeSeconds   = ", %name(idBasedOnSC & "CFE_CreateTimeSeconds")
  write "  CFE_CreateTimeSubSecs   = ", %name(idBasedOnSC & "CFE_CreateTimeSubSecs")
  write "  CFE_Description         = ", %name(idBasedOnSC & "CFE_Description")
  write ""
  write " Directory & File Stats:"
  write "  Directory Name          = ", $SC_$CPU_FM_DirNameInFile
  write "  Total Files in Dir      = ", $SC_$CPU_FM_TotalFilesInDir
  write "  Number of Files Written = ", $SC_$CPU_FM_NumFilesWritten
  FOR currentDis = 1 to maxToDis DO
    if (currentDis = 1) then
      write ""
      write " File Listing: "
    endif
    write "  File " & currentDis & " Name           = ",$SC_$CPU_FM_FileListEntry[currentDis].Name
    write "  File " & currentDis & " Size           = ",$SC_$CPU_FM_FileListEntry[currentDis].FileSize
    write "  File " & currentDis & " Last Mod Time  = ",$SC_$CPU_FM_FileListEntry[currentDis].LastModTime
  ENDDO
endif

ENDPROC
