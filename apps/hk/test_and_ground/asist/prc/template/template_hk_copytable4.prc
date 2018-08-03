PROC $sc_$cpu_hk_copytable4
;*******************************************************************************
;  Test Name:  hk_copytable4
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the files for the copy
;       tables used during BVT.  Note that the message ids used are
;       borrowed from the other CFS applications (MM, FM, MD, and SCH). 
;
;  Table 1:
;      11 input messages, 2 output messages, odd sized input and output
;      packets.  Used for StressHousekeeping, StressTableLoad
;
;  Requirements Tested
;       None
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.
;	The HK commands and TLM items exist in the GSE database. 
;	A display page exists for the HK Housekeeping telemetry packet.
;	HK Test application loaded and running
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	05/19/08 	Barbie Medina	Original Procedure.
;       03/08/11        Walt Moleski    Modified to use platform definitions for
;                                       Table name and number of entries.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			 Description
;       create_tbl_file_from_cvt Procedure that creates a load file from
;                                the specified arguments and cvt
;                                
;
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "hk_platform_cfg.h"

%liv (log_procedure) = logging

write ";*********************************************************************"
write ";  define local variables "
write ";*********************************************************************"

LOCAL entry
LOCAL appid
LOCAL OutputPacket1
LOCAL OutputPacket2
LOCAL InputPacket1
LOCAL InputPacket2
LOCAL InputPacket3
LOCAL InputPacket4
LOCAL InputPacket5
LOCAL InputPacket6
LOCAL InputPacket7
LOCAL InputPacket8
LOCAL InputPacket9
LOCAL InputPacket10
LOCAL InputPacket11

;; CPU1 is the default
appid = 0xfa6
OutputPacket1 = 0x89c  
OutputPacket2 = 0x89d
;; Use CPU2 IDs
InputPacket1 = 0x987
InputPacket2 = 0x988
InputPacket3 = 0x989
InputPacket4 = 0x98a
InputPacket5 = 0x98b
InputPacket6 = 0x98c
InputPacket7 = 0x98d
InputPacket8 = 0x98e
InputPacket9 = 0x98f
InputPacket10 = 0x990
InputPacket11 = 0x991

if ("$CPU" = "CPU2") then
   appid = 0xfc4
   OutputPacket1 = 0x99c  
   OutputPacket2 = 0x99d
   ;; Use CPU3 IDs
   InputPacket1 = 0xa87
   InputPacket2 = 0xa88
   InputPacket3 = 0xa89
   InputPacket4 = 0xa8a
   InputPacket5 = 0xa8b
   InputPacket6 = 0xa8c
   InputPacket7 = 0xa8d
   InputPacket8 = 0xa8e
   InputPacket9 = 0xa8f
   InputPacket10 = 0xa90
   InputPacket11 = 0xa91
elseif ("$CPU" = "CPU3") then
   appid = 0xfe4
   OutputPacket1 = 0xa9c  
   OutputPacket2 = 0xa9d
   ;; Use CPU1 IDs
   InputPacket1 = 0x887
   InputPacket2 = 0x888
   InputPacket3 = 0x889
   InputPacket4 = 0x88a
   InputPacket5 = 0x88b
   InputPacket6 = 0x88c
   InputPacket7 = 0x88d
   InputPacket8 = 0x88e
   InputPacket9 = 0x88f
   InputPacket10 = 0x890
   InputPacket11 = 0x891
endif 

write ";*********************************************************************"
write ";  Step 1.0:  Define Copy Table 4. "
write ";*********************************************************************"
; Entry 1
entry = 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 2
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 3
entry = entry +1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 4
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 5
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket3
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 6
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket3
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 7
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket4
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 8
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket4
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 9
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket5
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 10
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket5
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 11
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket6
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 12
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket6
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 13
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket7
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 14
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket7
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 15
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket8
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 16
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket8
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 17
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket9
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 18
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket9
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 19
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket10
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 20
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2 
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket10
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 21
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket11
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 22
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket11
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

;zero out the rest of the table
for entry=23 to HK_COPY_TABLE_ENTRIES do
  $SC_$CPU_HK_CopyTable[entry].OutputMid = 0
  $SC_$CPU_HK_CopyTable[entry].OutputOffset = 0
  $SC_$CPU_HK_CopyTable[entry].InputMid = 0
  $SC_$CPU_HK_CopyTable[entry].InputOffset = 0
  $SC_$CPU_HK_CopyTable[entry].NumBytes = 0
enddo 

local HKCopyTblName = "HK." & HK_COPY_TABLE_NAME
local endmnemonic = "$SC_$CPU_HK_CopyTable[" & HK_COPY_TABLE_ENTRIES & "].NumBytes"
 
s create_tbl_file_from_cvt("$CPU",appid,"Copy Table 4","hk_cpy_tbl.tbl",HKCopyTblName,"$SC_$CPU_HK_CopyTable[1].InputMid",endmnemonic)


write ";*********************************************************************"
write ";  End procedure $sc_$cpu_hk_copytable4                               "
write ";*********************************************************************"
ENDPROC
