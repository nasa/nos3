PROC $sc_$cpu_sc_atsoddbyte
;*******************************************************************************
;  Test Name:  sc_atsoddbyte
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates an ATS table load image file containing two odd
;	byte commands along with another command contained in the buffer. The
;	command that follows each odd byte command must start on an even word
;	boundary. This means that the buffer must pad to the next word.
;
;  Change History
;
;	Date		   Name		Description
;	01/27/11	Walt Moleski	Original Procedure.
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

#include "sc_msgdefs.h"
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
write ";  Create a table load containing the odd byte commands for ATS A.  "
write ";***********************************************************************"
;; Using the RDL, populate the ATS data stream with the data you desire
;;
local atsCmdCtr,nextCmd[3],nextCRC[3],hkPktId

;; Setup the command word and CRC word buffers
;; Command 1 is the TST_SC_GETTIME command which is 9 bytes
;; Command 2 is the SB_ENAROUTE command which is 11 bytes
;; Command 3 is the SC_NOOP command which is 8 bytes
;; CPU1 is the default
nextCmd[1] = x'1937'
nextCRC[1] = x'0416'
nextCmd[2] = x'1803'
nextCRC[2] = x'0425'
nextCmd[3] = x'18A9'
nextCRC[3] = x'008F'
hkPktId = "p0AA"

if ("$CPU" = "CPU2") then
  nextCmd[1] = x'1A37'
  nextCRC[1] = x'0415'
  nextCmd[2] = x'1823'
  nextCRC[2] = x'0405'
  nextCmd[3] = x'19A9'
  nextCRC[3] = x'008E'
  hkPktId = "p1AA"
elseif ("$CPU" = "CPU3") then
  nextCmd[1] = x'1B37'
  nextCRC[1] = x'0414'
  nextCmd[2] = x'1843'
  nextCRC[2] = x'0465'
  nextCmd[3] = x'1AA9'
  nextCRC[3] = x'008D'
  hkPktId = "p2AA"
endif

;; Each command is prepended with the ATS Command number and 
;; a 4-byte Time to execute the command
;; Command 1 is the TST_SC_GETTIME = '1x37 C000 0002 04yy 00'
$SC_$CPU_SC_ATSDATA[1] = x'0001'
$SC_$CPU_SC_ATSDATA[4] = nextCmd[1]
$SC_$CPU_SC_ATSDATA[5] = x'C000'
$SC_$CPU_SC_ATSDATA[6] = x'0002'
$SC_$CPU_SC_ATSDATA[7] = nextCRC[1]
;; Command data
$SC_$CPU_SC_ATSDATA[8] = x'0100'

;; Command 2 is the SB_ENAROUTE    = '18x3 C000 0004 04yy 0002 03'
$SC_$CPU_SC_ATSDATA[9] = x'0002'
$SC_$CPU_SC_ATSDATA[12] = nextCmd[2]
$SC_$CPU_SC_ATSDATA[13] = x'C000'
$SC_$CPU_SC_ATSDATA[14] = x'0004'
$SC_$CPU_SC_ATSDATA[15] = nextCRC[2]
;; Command data
$SC_$CPU_SC_ATSDATA[16] = x'0002'
$SC_$CPU_SC_ATSDATA[17] = x'0300'

;; Command 3 is the SC_NOOP        = '1xA9 C000 0001 00yy'
$SC_$CPU_SC_ATSDATA[18] = x'0003'
$SC_$CPU_SC_ATSDATA[21] = nextCmd[3]
$SC_$CPU_SC_ATSDATA[22] = x'C000'
$SC_$CPU_SC_ATSDATA[23] = x'0001'
$SC_$CPU_SC_ATSDATA[24] = nextCRC[3]

;; Clear out the rest of the ATS buffer
for i = 25 to SC_ATS_BUFF_SIZE do
  $SC_$CPU_SC_ATSDATA[i] = 0
enddo

%liv (log_procedure) = logging

local currentMET = $SC_$CPU_TIME_STCFSecs + $SC_$CPU_TIME_METSecs
if (SC_TIME_TO_USE = SC_USE_UTC) then
  currentMET = currentMET - $SC_$CPU_TIME_LeapSecs
endif
write "-- CURRENT MET => ", currentMET, " (hex) = ", %hex(currentMET,8)

local highWord = %ashiftr(currentMET,16)
local lowWord = %and(currentMET,x'0000FFFF')
  
;; NOTE: The lowWord times may need to be adjusted to account for the amount of
;; time it takes to Load, Validate and Activate the load file created below
;; The absolute time-tag for command 1
$SC_$CPU_SC_ATSDATA[2] = highWord
$SC_$CPU_SC_ATSDATA[3] = lowWord + 200

;; The absolute time-tag for command 2
$SC_$CPU_SC_ATSDATA[10] = highWord
$SC_$CPU_SC_ATSDATA[11] = lowWord + 210

;; The absolute time-tag for command 3
$SC_$CPU_SC_ATSDATA[19] = highWord
$SC_$CPU_SC_ATSDATA[20] = lowWord + 220

local endmnemonic = "$SC_$CPU_SC_ATSDATA[" & SC_ATS_BUFF_SIZE & "]"

;; Create the ATS Table Load file
s create_tbl_file_from_cvt ("$CPU",atsPktId,"ATS A odd byte command Load","$cpu_ats_oddbyteld",ATSATblName,"$SC_$CPU_SC_ATSDATA[1]",endmnemonic)
wait 5

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_atsoddbyte"
write ";*********************************************************************"
ENDPROC
