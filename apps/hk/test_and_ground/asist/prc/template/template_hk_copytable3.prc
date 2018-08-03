PROC $sc_$cpu_hk_copytable3
;*******************************************************************************
;  Test Name:  hk_copytable3
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the files for the copy
;       tables used during BVT.  Note that the message ids used are
;       borrowed from the other CFS applications (MM, FM, MD, and SCH). 
;
;  Table 3:
;      2 input messages, 6 output messages (Table will have 128 entries)
;      Used for StressHousekeeping, StressMissingData, StressTableLoad
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
LOCAl OutputPacket3
LOCAl OutputPacket4
LOCAl OutputPacket5
LOCAl OutputPacket6
LOCAL InputPacket1
LOCAL InputPacket2

;; CPU1 is the default
appid = 0xfa6
OutputPacket1 = 0x89c  
OutputPacket2 = 0x89d
OutputPacket3 = 0x89e
OutputPacket4 = 0x89f
OutputPacket5 = 0x8a0
OutputPacket6 = 0x8a1
;; Use CPU2 IDs
InputPacket1 = 0x987
InputPacket2 = 0x99a

if ("$CPU" = "CPU2") then
   appid = 0xfc4
   OutputPacket1 = 0x99c  
   OutputPacket2 = 0x99d
   OutputPacket3 = 0x99e
   OutputPacket4 = 0x99f
   OutputPacket5 = 0x9a0
   OutputPacket6 = 0x9a1
   ;; Use CPU3 IDs
   InputPacket1 = 0xa87
   InputPacket2 = 0xa9a
elseif ("$CPU" = "CPU3") then
   appid = 0xfe4
   OutputPacket1 = 0xa9c  
   OutputPacket2 = 0xa9d
   OutputPacket3 = 0xa9e
   OutputPacket4 = 0xa9f
   OutputPacket5 = 0xaa0
   OutputPacket6 = 0xaa1
   ;; Use CPU1 IDs
   InputPacket1 = 0x887
   InputPacket2 = 0x89a
endif 


write ";*********************************************************************"
write ";  Step 1.0:  Define Copy Table 3. "
write ";*********************************************************************"
; Entry 1
entry = 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 2
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 3
entry = entry +1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 4
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 5
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 16
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 6
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 7
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 8
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 9
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 20
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 10
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 21
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 11
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 22
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 12
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 23
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 13
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 24
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 14
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 25
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 15
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 26
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 16
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 17
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 28
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 18
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 29
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 19
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 30
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 20
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2 
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 31
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 21
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 32
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 22
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 33
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 23
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 34
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 24
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 35
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 25
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 36
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 26
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 37
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 27
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 38
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 28
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 39
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 29
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 40
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 30
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 41
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 31
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 42
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 32
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 33
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 34
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 35
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 36
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 37
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 16
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 38
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 39
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 40
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 41
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 20
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 42
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 21
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 43
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 22
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 44
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 23
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 45
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 24
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 46
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 25
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 47
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 26
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 48
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 49
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 28
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 50
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 29
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 51
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 30
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 52
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 31
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 53
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 32
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 54
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 33
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 55
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 34
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 56
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 35
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 57
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 36
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 58
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 37
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 59
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 38
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 60
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 39
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 61
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 40
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 62
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 41
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 63
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 42
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 64
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1


; Entry 65
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 66
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 42
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 67
entry = entry +1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 41
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 68
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 40
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 69
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 39
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 70
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 38
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 71
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 37
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 72
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 36
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 73
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 35
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 74
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 34
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 75
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 33
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 76
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 32
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 77
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 31
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 78
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 30
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 79
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 29
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 80
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 28
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 81
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 82
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 26
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 83
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 25
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 84
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2 
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 24
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 85
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 23
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 86
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 22
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 87
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 21
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 88
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 20
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 89
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 90
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 91
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 92
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 16
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 93
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 94
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 95
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 96
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 97
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 98
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 42
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 99
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 41
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 100
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 40
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 101
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 39
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 102
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 38
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 103
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 37
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 104
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 36
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 105
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 35
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 106
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 34
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 107
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 33
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 108
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 32
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 109
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 31
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 110
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 30
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 111
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 29
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 112
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 28
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 113
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 114
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 26
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 115
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 25
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 116
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 24
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 117
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 23
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 118
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 22
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 119
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 21
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 120
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 20
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 121
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 122
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 123
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket5
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 124
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket6
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 16
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 125
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 33
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 126
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 33
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 127
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 33
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 128
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket4
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 33
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

;zero out the rest of the table
for entry=129 to HK_COPY_TABLE_ENTRIES do
  $SC_$CPU_HK_CopyTable[entry].OutputMid = 0
  $SC_$CPU_HK_CopyTable[entry].OutputOffset = 0
  $SC_$CPU_HK_CopyTable[entry].InputMid = 0
  $SC_$CPU_HK_CopyTable[entry].InputOffset = 0
  $SC_$CPU_HK_CopyTable[entry].NumBytes = 0
enddo

local HKCopyTblName = "HK." & HK_COPY_TABLE_NAME
local endmnemonic = "$SC_$CPU_HK_CopyTable[" & HK_COPY_TABLE_ENTRIES & "].NumBytes"
 
s create_tbl_file_from_cvt("$CPU",appid,"Copy Table 3","hk_cpy_tbl.tbl",HKCopyTblName,"$SC_$CPU_HK_CopyTable[1].InputMid",endmnemonic)


write ";*********************************************************************"
write ";  End procedure $sc_$cpu_hk_copytable3                               "
write ";*********************************************************************"
ENDPROC
