PROC $sc_$cpu_sc_appendfull
;*******************************************************************************
;  Test Name:  sc_appendfull
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates an ATS Append table load image file containing a
;	full table of valid commands. 
;
;  Change History
;
;	Date		   Name		Description
;	01/31/11	Walt Moleski	Original Procedure.
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
LOCAL atsPktId,atsAppId
local SCAppName = "SC"
local ATSAppendATblName = SCAppName & "." & SC_APPEND_TABLE_NAME

;; Set the pkt and app Ids for the appropriate CPU
;; CPU1 is the default
atsPktId = "0FBB"
atsAppId = 4027

if ("$CPU" = "CPU2") then
  atsPktId = "0FD9"
  atsAppId = 4057
elseif ("$CPU" = "CPU3") then
  atsPktId = "0FF9"
  atsAppId = 4089
endif

write ";***********************************************************************"
write ";  Create a table load containing as many valid commands that will fit. "
write ";***********************************************************************"
;; Using the RDL, populate the ATS data stream with the data you desire
;;
local i,j,atsCmdCtr,nextCmd,nextCRC,wordsLeft=0

;; Setup the command word and CRC word buffers
;; CPU1 is the default
nextCmd = x'18A9'
nextCRC = x'0982'

if ("$CPU" = "CPU2") then
  nextCmd = x'19A9'
  nextCRC = x'0983'
elseif ("$CPU" = "CPU3") then
  nextCmd = x'1AA9'
  nextCRC = x'0980'
endif

;; JumpATS Command example = '0005 0000 0000 1xA9 C000 0005 09yy 0000 0000'
;; ATS Command entry size = 9 words
;; Loop for each word in the Append Buffer
j = 1
atsCmdCtr = 1
for i = 1 to SC_APPEND_BUFF_SIZE do
  if (j = 1) then
    wordsLeft = SC_APPEND_BUFF_SIZE - i
    if (wordsLeft > 9) then
      $SC_$CPU_SC_ATSAPPENDDATA[i] = atsCmdCtr 
      atsCmdCtr = atsCmdCtr + 1
    else
      break
    endif
  elseif (j = 4) then
    $SC_$CPU_SC_ATSAPPENDDATA[i] = nextCmd
  elseif (j = 5) then
    $SC_$CPU_SC_ATSAPPENDDATA[i] = x'C000'
  elseif (j = 6) then
    $SC_$CPU_SC_ATSAPPENDDATA[i] = x'0005'
  elseif (j = 7) then
    $SC_$CPU_SC_ATSAPPENDDATA[i] = nextCRC
  else
    $SC_$CPU_SC_ATSAPPENDDATA[i] = 0
  endif
  j = j + 1
  if (j = 10) then
    j = 1
  endif
enddo

%liv (log_procedure) = logging

write "==> i after cmds loop = ", i

;; Loop for the remaining words in the buffer
for j = i to SC_APPEND_BUFF_SIZE do
  $SC_$CPU_SC_ATSAPPENDDATA[i] = 0
enddo

local endmnemonic = "$SC_$CPU_SC_ATSAPPENDDATA[" & SC_APPEND_BUFF_SIZE & "]"

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"Append Table Max Cmd Load","$cpu_appfull_ld",ATSAppendATblName,"$SC_$CPU_SC_ATSAPPENDDATA[1]",endmnemonic)
wait 5

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_appendfull"
write ";*********************************************************************"
ENDPROC
