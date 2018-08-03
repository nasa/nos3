PROC $sc_$cpu_hk_copytable1
;*******************************************************************************
;  Test Name:  hk_copytables
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the files for the copy
;       tables used during BVT.  Note that the message ids used are
;       borrowed from the other CFS applications (MM, FM, MD, and SCH). 
;
;  Table 1:
;	20 input messages, 3 output messages, pieces of each input packet go to 
;	each output packet. Table will have 60 entries.  Used for: GenCmds,
;	BasicHousekeeping, MissingData, Stress Table Load
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
;	05/01/08 	Barbie Medina	Original Procedure.
;       05/28/08	Barbie Medina   Corrected error in offsets
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
LOCAL OutputPacket3
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
LOCAL InputPacket12
LOCAL InputPacket13
LOCAL InputPacket14
LOCAL InputPacket15
LOCAL InputPacket16
LOCAL InputPacket17
LOCAL InputPacket18
LOCAL InputPacket19
LOCAL InputPacket20

;; CPU1 is the default
appid = 0xfa6
OutputPacket1 = 0x89c  
OutputPacket2 = 0x89d
OutputPacket3 = 0x89e
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
InputPacket12 = 0x992
InputPacket13 = 0x993
InputPacket14 = 0x994
InputPacket15 = 0x995
InputPacket16 = 0x996
InputPacket17 = 0x997
InputPacket18 = 0x998
InputPacket19 = 0x999
InputPacket20 = 0x99a

if ("$CPU" = "CPU2") then
   appid = 0xfc4
   OutputPacket1 = 0x99c  
   OutputPacket2 = 0x99d
   OutputPacket3 = 0x99e
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
   InputPacket12 = 0xa92
   InputPacket13 = 0xa93
   InputPacket14 = 0xa94
   InputPacket15 = 0xa95
   InputPacket16 = 0xa96
   InputPacket17 = 0xa97
   InputPacket18 = 0xa98
   InputPacket19 = 0xa99
   InputPacket20 = 0xa9a
elseif ("$CPU" = "CPU3") then
   appid = 0xfe4
   OutputPacket1 = 0xa9c  
   OutputPacket2 = 0xa9d
   OutputPacket3 = 0xa9e
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
   InputPacket12 = 0x892
   InputPacket13 = 0x893
   InputPacket14 = 0x894
   InputPacket15 = 0x895
   InputPacket16 = 0x896
   InputPacket17 = 0x897
   InputPacket18 = 0x898
   InputPacket19 = 0x899
   InputPacket20 = 0x89a
endif 

write ";*********************************************************************"
write ";  Step 1.0:  Define Copy Table 1. "
write ";*********************************************************************"
; Entry 1
entry = 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 2
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 50
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 3
entry = entry +1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket1
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 4
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 5
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 48
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 6
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket2
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 7
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket3
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 8
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 46
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket3
$SC_$CPU_HK_CopyTable[entry].InputOffset = 21
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 9
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket3
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 10
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket4
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 11
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 44
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket4
$SC_$CPU_HK_CopyTable[entry].InputOffset = 37
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 12
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket4
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 13
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket5
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 14
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 42
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket5
$SC_$CPU_HK_CopyTable[entry].InputOffset = 38
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 15
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket5
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 16
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket6
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 17
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 40
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket6
$SC_$CPU_HK_CopyTable[entry].InputOffset = 22
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 18
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket6
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 19
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 36
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket7
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 20
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2 
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 38
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket7
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 21
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket7
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 22
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 40
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket8
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 23
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 36
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket8
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 24
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket8
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 25
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 44
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket9
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 26
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 34
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket9
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 27
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket9
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 28
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 48
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket10
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 29
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 32
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket10
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 30
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket10
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 31
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 52
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket11
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 32
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 30
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket11
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 33
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 13
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket11
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19 
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 34
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 56
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket12
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 35
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 28
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket12
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 36
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 15
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket12
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 37
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 60
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket13
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 38
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 26
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket13
$SC_$CPU_HK_CopyTable[entry].InputOffset = 13
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 39
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 17
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket13
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 40
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 64
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket14
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 41
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 24
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket14
$SC_$CPU_HK_CopyTable[entry].InputOffset = 17
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 42
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 19
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket14
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 43
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 68
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket15
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 44
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 22
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket15
$SC_$CPU_HK_CopyTable[entry].InputOffset = 25
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 45
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 21
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket15
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 46
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 72
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket16
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 47
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 20
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket16
$SC_$CPU_HK_CopyTable[entry].InputOffset = 41
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 48
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 23
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket16
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 49
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 76
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket17
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 50
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 18
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket17
$SC_$CPU_HK_CopyTable[entry].InputOffset = 42
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 51
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 25
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket17
$SC_$CPU_HK_CopyTable[entry].InputOffset = 43
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 52
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 80
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket18
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 53
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 16
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket18
$SC_$CPU_HK_CopyTable[entry].InputOffset = 26
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 54
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 27
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket18
$SC_$CPU_HK_CopyTable[entry].InputOffset = 27
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 55
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 84
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket19
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 56
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 14
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket19
$SC_$CPU_HK_CopyTable[entry].InputOffset = 18
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 57
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 29
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket19
$SC_$CPU_HK_CopyTable[entry].InputOffset = 19
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

; Entry 58
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket1
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 88
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket20
$SC_$CPU_HK_CopyTable[entry].InputOffset = 12
$SC_$CPU_HK_CopyTable[entry].NumBytes = 4

; Entry 59
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket2
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 12
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket20
$SC_$CPU_HK_CopyTable[entry].InputOffset = 14
$SC_$CPU_HK_CopyTable[entry].NumBytes = 2

; Entry 60
entry = entry + 1
$SC_$CPU_HK_CopyTable[entry].OutputMid = OutputPacket3
$SC_$CPU_HK_CopyTable[entry].OutputOffset = 31
$SC_$CPU_HK_CopyTable[entry].InputMid = InputPacket20
$SC_$CPU_HK_CopyTable[entry].InputOffset = 15
$SC_$CPU_HK_CopyTable[entry].NumBytes = 1

;zero out the rest of the table
for entry=61 to HK_COPY_TABLE_ENTRIES do
  $SC_$CPU_HK_CopyTable[entry].OutputMid = 0
  $SC_$CPU_HK_CopyTable[entry].OutputOffset = 0
  $SC_$CPU_HK_CopyTable[entry].InputMid = 0
  $SC_$CPU_HK_CopyTable[entry].InputOffset = 0
  $SC_$CPU_HK_CopyTable[entry].NumBytes = 0
enddo 

local HKCopyTblName = "HK." & HK_COPY_TABLE_NAME
local endmnemonic = "$SC_$CPU_HK_CopyTable[" & HK_COPY_TABLE_ENTRIES & "].NumBytes"

s create_tbl_file_from_cvt("$CPU", appid, "Copy Table 1", "hk_cpy_tbl.tbl", HKCopyTblName,"$SC_$CPU_HK_CopyTable[1].InputMid",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_hk_copytable1                               "
write ";*********************************************************************"
ENDPROC
