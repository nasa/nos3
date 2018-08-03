proc $sc_$cpu_fm_dirtlmdisplay (directoryName, offsetGiven, expResult)
;==============================================================================
;
; Purpose: The purpose of this procedure is to perform the Dir Listing to Tlm
;          cmd and then write the contents of the packet to the log.
;
; Waring:  It is advised that before running a fresh procedure that calls this
;          to initialize the telemetry points P08Cscnt, P18Cscnt, & P28Cscnt 
;          to 0 so that a previous proc run does not interfere with whether
;          the packet is viewed as stale or not 
;
; History:
;
; 08/05/2008	Initial development of this proc.		DDS
; 09/22/2009	Updated to make CPU1 the default.		WFM
; 01/13/2010	Updated to work with FM 2.1.0.0			WFM
;

#include "fm_platform_cfg.h"

;; If the optional argument is not specified, expect Pass
if (%nargs = 2) then
  expResult = "Pass"
endif

local cmdctr
local errcnt
local seqCount
local writeToLog = 1
local currentDis
local numToDisplay
local maxFilesToDisplay = FM_DIR_LIST_PKT_ENTRIES
local actualResult

;****************************************
;  start of sub-proc
;****************************************
; grab the current sequence counter for the packet
; CPU1 is the default
seqCount = P08Cscnt

if ("$CPU" = "CPU2") then
  seqCount = P18Cscnt
elseif ("$CPU" = "CPU3") then
  seqCount = P28Cscnt
endif

write "  Packet Sequence Counter before cmd = ", seqCount

; issue the command
cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_DirListTlm DirName = directoryName  Offset = offsetGiven

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))   
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  actualResult = "Pass"
  if (expResult = "Pass") then
    write "<*> Passed - Dir Listing to Tlm command accepted."
  else
    write "<!> Failed - Dir Listing to Tlm command accepted when failure was expected."
  endif
else
  actualResult = "Fail"
  if (expResult = "Fail") then
    write "<*> Passed - Dir Listing to Tlm command rejected."
  else
    write "<!> Failed - Dir Listing to Tlm command rejected when success was expected."
  endif
endif

wait 5

if (actualResult = "Pass") then
  ;; Write contents of the packet to the log
  write ""

  local newSeqCount = P08Cscnt

  if ("$CPU" = "CPU2") then
    newSeqCount = P18Cscnt
  elseif ("$CPU" = "CPU3") then
    newSeqCount = P28Cscnt
  endif

  if (seqCount = newSeqCount) then
    write "<!> WARNING - Info Packet not received"
    writeToLog = 0
  else
    write "<*> New Info Packet received"
  endif

  write ""
  write "Sequence Counter = ",newSeqCount 

  if (writeToLog = 1) then
    if ($sc_$cpu_FM_DirOffset > $sc_$cpu_FM_TotalFiles) then
      numToDisplay = 0
    elseif (($sc_$cpu_FM_TotalFiles - $sc_$cpu_FM_DirOffset) < maxFilesToDisplay) then
      numToDisplay = ($sc_$cpu_FM_TotalFiles - $sc_$cpu_FM_DirOffset)
    else
      numToDisplay = maxFilesToDisplay
    endif
    write ""
    write "Contents of the Directory Listing to Telemetry Packet:"
    write ""
    write " Directory Stats:"
    write "  Directory Name        = ", $sc_$cpu_FM_DirName
    write "  Directory Offset      = ", $sc_$cpu_FM_DirOffset
    write "  Total Files in Dir    = ", $sc_$cpu_FM_TotalFiles
    write "  Files in Packet       = ", $sc_$cpu_FM_PktFiles
    FOR currentDis = 1 to numToDisplay DO
      if ( currentDis = 1 ) then
        write ""
        write " File Listing: "
      endif
      write "  File " & currentDis & " Name          = ",$sc_$cpu_FM_DirList[currentDis].name
      write "  File " & currentDis & " Size          = ",$sc_$cpu_FM_DirList[currentDis].FileSize
      write "  File " & currentDis & " Last Mod Time = ",$sc_$cpu_FM_DirList[currentDis].LastModTime
    ENDDO
    write ""
  endif
endif

ENDPROC
