proc $sc_$cpu_fm_fileinfodisplay (filename, crcVal, expResult)
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the file info
;          cmd and then write the contents of the packet to the log.
;
; Waring:  It is advised that before running a fresh procedure that calls this
;          to initialize the telemetry points P08Bscnt, P18Bscnt, & P28Bscnt 
;          to 0 so that a previous proc run does not interfere with whether
;          the packet is viewed as stale or not 
;
; History:
;
;       Date       Name         Description
;	07/31/08   D. Stewart	Initial development of this proc.
;       02/28/11   W. Moleski   Added variables for App name and ram directory
;       05/09/12   W. Moleski   Added check of ChildCMDPC and ChildCMDEC
;==============================================================================

if (%nargs < 2) then
  crcVal = 0
  expResult = "Pass"
elseif (%nargs = 2) then
  expResult = "Pass"
endif

local cmdctr
local errcnt
local seqCount
local writeToLog = 1
local actualResult

;****************************************
;  start of sub-proc
;****************************************
; grab the current sequence counter for the info packet
; CPU1 is the default
seqCount = P08Bscnt

if ("$CPU" = "CPU2") then
  seqCount = P18Bscnt
elseif ("$CPU" = "CPU3") then
  seqCount = P28Bscnt
endif

write "  Packet Sequence Counter before cmd = ", seqCount

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1
local childCmdCtr = $SC_$CPU_FM_CHILDCMDPC + 1

/$SC_$CPU_FM_FileInfo File=filename CRC_Method=crcVal

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))   
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  actualResult = "Pass"
  if (expResult = "Pass") then
    write "<*> Passed - File Info command accepted."
    ;; Check if the ChildCMDPC incremented
    if ($SC_$CPU_FM_CHILDCMDPC = childCmdCtr) then
      write "<*> Passed - Child CMDPC incremented as expected."
    else
      write "<!> Failed - Child CMDPC did not increment. Expected '",childCmdCtr,"'; Got '", $SC_$CPU_FM_CHILDCMDPC,"'"
    endif
  else
    write "<!> Failed - File Info command rejected when success was expected."
  endif
else
  actualResult = "Fail"
  if (expResult = "Fail") then
    write "<*> Passed - File Info command rejected as expected."
  else
    write "<!> Failed - File Info command accepted when failure was expected."
  endif
endif

if (actualResult = "Pass") then
  ; Write contents of the packet to the log
  write "Contents of the File Info Packet:"
  local newSeqCount = P08Bscnt 

  if ("$CPU" = "CPU2") then
    newSeqCount = P18Bscnt
  elseif ("$CPU" = "CPU3") then
    newSeqCount = P28Bscnt
  endif

  if (seqCount = newSeqCount) then
    write "<?> WARNING - Info Packet not received"
    writeToLog = 0
  endif
  write ""
  write "  Sequence Counter = ", newSeqCount

  if (writeToLog = 1) then
    write "  File Name        = ", $SC_$CPU_FM_InfoFileName
    write "  File Status      = ", P@$SC_$CPU_FM_FileStatus
    write "  File Size        = ", $SC_$CPU_FM_InfoFileSize
    write "  Compute CRC Flag = ", $SC_$CPU_FM_ComputeCRC
    write "  CRC              = ", $SC_$CPU_FM_CRC
    write "  last Mod Time    = ", $SC_$CPU_FM_ModTime
    write ""
  endif
endif

ENDPROC
