PROC $sc_$cpu_sc_appoffend
;*******************************************************************************
;  Test Name:  sc_appoffend
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates an ATS Append table load image file containing a
;	full table of commands with the last command going over the end of the 
;	table buffer.
;
;  Change History
;
;	Date		   Name		Description
;	01/28/11	Walt Moleski	Original Procedure.
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
local i,j,atsCmdCtr,nextCmd,nextCRC

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

;; JumpATS Command example = '0005 000F 4B6D 1AA9 C000 0005 093A 000F 4DF8'
;; ATS Command entry size = 9 words
;; Loop for each word in the ATS Buffer
j = 1
atsCmdCtr = 1
for i = 1 to SC_APPEND_BUFF_SIZE - 18  do
  if (j = 1) then
    $SC_$CPU_SC_ATSAPPENDDATA[i] = atsCmdCtr 
    atsCmdCtr = atsCmdCtr + 1
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

;; Create the last command to be longer than the remaining words in the buffer
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = atsCmdCtr
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'192B'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'C000'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'0027'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'0365'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'5343'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'2E41'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'5453'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'5F54'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'424C'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = x'3100'
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0
i = i + 1
$SC_$CPU_SC_ATSAPPENDDATA[i] = 0

%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_SC_ATSAPPENDDATA[" & SC_APPEND_BUFF_SIZE & "]"

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"Append Table Cmd off end Load","$cpu_appoffend_ld",ATSAppendATblName,"$SC_$CPU_SC_ATSAPPENDDATA[1]",endmnemonic)
wait 5

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_appoffend"
write ";*********************************************************************"
ENDPROC
