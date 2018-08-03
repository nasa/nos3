PROC $sc_$cpu_sc_rtsoffend
;*******************************************************************************
;  Test Name:  sc_rtsoffend
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates an RTS table load image file containing a full
;	table of commands with the last command going over the end of the table
;	buffer.
;
;  Change History
;
;	Date		   Name		Description
;	09/01/11	Walt Moleski	Original Procedure.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "sc_platform_cfg.h"

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd, stream
LOCAL cmdCtr, errcnt
LOCAL rtsPktId,rtsAppId
local SCAppName = "SC"
local RTSTbl15Name = SCAppName & "." & SC_RTS_TABLE_NAME & "015"

;; Set the pkt and app Ids for the appropriate CPU
;; CPU1 is the default
rtsPktId = "0FBC"
rtsAppId = 4028

if ("$CPU" = "CPU2") then
  rtsPktId = "0FDA"
  rtsAppId = 4058
elseif ("$CPU" = "CPU3") then
  rtsPktId = "0FFA"
  rtsAppId = 4090
endif

write ";***********************************************************************"
write ";  Create a table load containing a command that runs off the end of the"
write ";  RTS buffer. "
write ";***********************************************************************"
;; Using the RDL, populate the RTS data stream with the data you desire
;;
local j,nextCmd,nextCRC,nextCmdNum

;; Setup the command word and CRC word buffers
;; CPU1 is the default
nextCmd = x'18A9'
nextCRC = x'008F'

if ("$CPU" = "CPU2") then
  nextCmd = x'19A9'
  nextCRC = x'008E'
elseif ("$CPU" = "CPU3") then
  nextCmd = x'1AA9'
  nextCRC = x'008D'
endif

;; SC_NOOP Command = '0000 1xA9 C000 0001 00yy'
;; Loop for each word in the ATS Buffer
j = 1
nextCmdNum = 1
for i = 1 to SC_RTS_BUFF_SIZE - 10  do
  if (j = 1) then
    $SC_$CPU_SC_RTSDATA[i] = 5 
  elseif (j = 2) then
    $SC_$CPU_SC_RTSDATA[i] = nextCmd
  elseif (j = 3) then
    $SC_$CPU_SC_RTSDATA[i] = x'C000'
  elseif (j = 4) then
    $SC_$CPU_SC_RTSDATA[i] = x'0001'
  elseif (j = 5) then
    $SC_$CPU_SC_RTSDATA[i] = nextCRC
  endif
  j = j + 1
  if (j = 6) then
    j = 1
  endif
enddo

;; Create the last command to be longer than the remaining words in the buffer
$SC_$CPU_SC_RTSDATA[i] = 5
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'192B'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'C000'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'0027'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'0365'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'5343'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'2E41'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'5453'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'5F54'
i = i + 1
$SC_$CPU_SC_RTSDATA[i] = x'424C'
i = i + 1
$SC_$CPU_SC_ATSDATA[i] = x'3100'

%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_SC_RTSDATA[" & SC_RTS_BUFF_SIZE & "]"

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS 15 Table Command off end Load", "$cpu_rts015_load",RTSTbl15Name,"$SC_$CPU_SC_RTSDATA[1]",endmnemonic)
wait 5

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_rtsoffend"
write ";*********************************************************************"
ENDPROC
