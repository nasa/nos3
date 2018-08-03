PROC $sc_$cpu_sc_maxcmdats
;*******************************************************************************
;  Test Name:  sc_maxcmdats
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates an ATS table load image file containing one more
;	command than the maximum commands allowed in an ATS.
;
;  Change History
;
;	Date		   Name		Description
;	01/21/09	Walt Moleski	Original Procedure.
;	04/01/10	Walt Moleski	Added code to turn logging off for this
;					procedure and then restore it.
;       01/25/11        Walt Moleski    Modified to use variable names for the
;                                       SC app name and table name
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
local ATSATblName = SCAppName & "." & SC_ATS_TABLE_NAME & "1"

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
write ";  Create a table load containing the maximum + 1 commands to ATS A.  "
write ";***********************************************************************"
;; Using the RDL, populate the ATS data stream with the data you desire
;;
local j,atsCmdCtr,nextCmd[6],nextCRC[6],nextCmdNum

;; Setup the command word and CRC word buffers
;; CPU1 is the default
;; SC
nextCmd[1] = x'18A9'
nextCRC[1] = x'008F'
;; CFE_TBL
nextCmd[2] = x'1804'
nextCRC[2] = x'0022'
;; CFE_EVS
nextCmd[3] = x'1801'
nextCRC[3] = x'0126'
;; CFE_ES
nextCmd[4] = x'1806'
nextCRC[4] = x'0020'
;; CFE_SB
nextCmd[5] = x'1803'
nextCRC[5] = x'0025'
;; CFE_TIME
nextCmd[6] = x'1805'
nextCRC[6] = x'0023'

if ("$CPU" = "CPU2") then
  nextCmd[1] = x'19A9'
  nextCRC[1] = x'008E'
  nextCmd[2] = x'1824'
  nextCRC[2] = x'0002'
  nextCmd[3] = x'1821'
  nextCRC[3] = x'0106'
  nextCmd[4] = x'1826'
  nextCRC[4] = x'0000'
  nextCmd[5] = x'1823'
  nextCRC[5] = x'0005'
  nextCmd[6] = x'1825'
  nextCRC[6] = x'0003'
elseif ("$CPU" = "CPU3") then
  nextCmd[1] = x'1AA9'
  nextCRC[1] = x'008D'
  nextCmd[2] = x'1844'
  nextCRC[2] = x'0062'
  nextCmd[3] = x'1841'
  nextCRC[3] = x'0166'
  nextCmd[4] = x'1846'
  nextCRC[4] = x'0060'
  nextCmd[5] = x'1843'
  nextCRC[5] = x'0065'
  nextCmd[6] = x'1845'
  nextCRC[6] = x'0063'
endif

;; Loop for each word in the ATS Buffer
j = 1
atsCmdCtr = 1
nextCmdNum = 1
for i = 1 to SC_ATS_BUFF_SIZE do
  if (j = 1) then
    $SC_$CPU_SC_ATSDATA[i] = atsCmdCtr 
    atsCmdCtr = atsCmdCtr + 1
  elseif (j = 4) then
    $SC_$CPU_SC_ATSDATA[i] = nextCmd[nextCmdNum]
  elseif (j = 5) then
    $SC_$CPU_SC_ATSDATA[i] = x'C000'
  elseif (j = 6) then
    $SC_$CPU_SC_ATSDATA[i] = x'0001'
  elseif (j = 7) then
    $SC_$CPU_SC_ATSDATA[i] = nextCRC[nextCmdNum]
    nextCmdNum = nextCmdNum + 1
    if (nextCmdNum = 7) then
      nextCmdNum = 1
    endif
  else
    $SC_$CPU_SC_ATSDATA[i] = 0
  endif
  j = j + 1
  if (j = 8) then
    j = 1
  endif
enddo

%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_SC_ATSDATA[" & SC_ATS_BUFF_SIZE & "]"

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A Table Max+1 Commands Load","$cpu_ats_maxcmd_ld",ATSATblName,"$SC_$CPU_SC_ATSDATA[1]",endmnemonic)
wait 5

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_maxcmdsats"
write ";*********************************************************************"
ENDPROC

