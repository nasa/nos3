PROC $sc_$cpu_sc_rtsoddbyte
;*******************************************************************************
;  Test Name:  sc_rtsoddbyte
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This procedure creates a table load image file for RTS #3 containing two
;	odd byte commands along with another command contained in the buffer.
;	Each command that follows the odd byte command must start on an even
;	word boundary. This means that the buffer must pad to the next word.
;
;  Change History
;
;	Date		   Name		Description
;	02/04/11	Walt Moleski	Original Procedure.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;   create_tbl_file_from_cvt	Creates a Table Load image from the CVT
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
LOCAL rtsPktId,rtsAppId
local SCAppName = "SC"
local RTS3TblName = SCAppName & "." & SC_RTS_TABLE_NAME & "003"

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
write ";  Create a table load containing the odd byte commands for RTS #3.  "
write ";***********************************************************************"
;; Using the RDL, populate the RTS data stream with the data you desire
;;
local nextCmd[3],nextCRC[3],hkPktId

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
$SC_$CPU_SC_RTSDATA[1] = 5
$SC_$CPU_SC_RTSDATA[2] = nextCmd[1]
$SC_$CPU_SC_RTSDATA[3] = x'C000'
$SC_$CPU_SC_RTSDATA[4] = x'0002'
$SC_$CPU_SC_RTSDATA[5] = nextCRC[1]
;; Command data
$SC_$CPU_SC_RTSDATA[6] = x'0100'

;; Command 2 is the SB_ENAROUTE    = '18x3 C000 0004 04yy 0002 03'
$SC_$CPU_SC_RTSDATA[7] = 5
$SC_$CPU_SC_RTSDATA[8] = nextCmd[2]
$SC_$CPU_SC_RTSDATA[9] = x'C000'
$SC_$CPU_SC_RTSDATA[10] = x'0004'
$SC_$CPU_SC_RTSDATA[11] = nextCRC[2]
;; Command data
$SC_$CPU_SC_RTSDATA[12] = x'0002'
$SC_$CPU_SC_RTSDATA[13] = x'0300'

;; Command 3 is the SC_NOOP        = '1xA9 C000 0001 00yy'
$SC_$CPU_SC_RTSDATA[14] = 5
$SC_$CPU_SC_RTSDATA[15] = nextCmd[3]
$SC_$CPU_SC_RTSDATA[16] = x'C000'
$SC_$CPU_SC_RTSDATA[17] = x'0001'
$SC_$CPU_SC_RTSDATA[18] = nextCRC[3]

;; Clear out the rest of the ATS buffer
for i = 19 to SC_RTS_BUFF_SIZE do
  $SC_$CPU_SC_RTSDATA[i] = 0
enddo

%liv (log_procedure) = logging

local endmnemonic = "$SC_$CPU_SC_RTSDATA[" & SC_RTS_BUFF_SIZE & "]"

;; Create the RTS Table Load file
s create_tbl_file_from_cvt ("$CPU",rtsPktId,"RTS 3 odd byte command Load","$cpu_rts3oddbyteld",RTS3TblName,"$SC_$CPU_SC_RTSDATA[1]",endmnemonic)
wait 5

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_sc_rtsoddbyte"
write ";*********************************************************************"
ENDPROC
