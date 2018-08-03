PROC $sc_$cpu_hk_basichousekeeping
;*******************************************************************************
;  Test Name:  hk_basichousekeeping
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Housekeeping (HK) can collect
;       housekeeping data from an average number of input message streams (20)
;       and combine the input message data into an average number of output
;       messages (3). It also tests HK sending its housekeeping data and 
;       updating the copy table.  
;
;  Requirements Tested
;       HK2000	 HK shall collect flight software housekeeping data from table
;                -specified input messages
;       HK2001	 HK shall output up to a maximum <Mission-Defined> table-defined
;                messages, at the scheduled rate, by combining input message 
;                data starting at the table-defined offset and table-defined 
;                number of bytes to the table-defined offset in the output
;		 message.
;       HK2001.1 Upon a table update, HK shall update the output message formats
;                specified in the table during normal execution. 
;       HK3000	 HK shall generate a housekeeping message containing the
;		 following:
;                    a)	Valid Command Counter
;                    b)	Command Rejected Counter
;                    c)	Number of Output Messages Sent
;                    d)	Missing Data Counter
;       HK4000	Upon initialization of the HK Application, HK shall initialize
;               the following data to Zero
;                    a)	Valid Command Counter
;                    b)	Command Rejected Counter
;                    c)	Number of Output Messages Sent
;                    d)	Missing Data Counter
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
;	05/16/08	Barbie Medina	Original Procedure.
;       05/18/08        Barbie Medina   updates to add waits to sending input
;                                       packet commands
;       06/03/08        Barbie Medina   removed use of ut_tlmupdate from wait
;                                       for output packets to update
;       07/02/10        Walt Moleski	Updated to use the default table name
;					and to call $sc_$cpu_hk_start_apps
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait      Wait for a specified telemetry point to update to
;                       a specified value. 
;       ut_sendcmd      Send commands to the spacecraft. Verifies command
;                       processed and command error counters.
;       ut_sendrawcmd   Send raw commands to the spacecraft. Verifies command
;                       processed and command error counters.
;       ut_pfindicate   Print the pass fail status of a particular requirement
;                       number.
;       ut_setupevents  Performs setup to verify that a particular event
;                       message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			 requirements array.
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "to_lab_events.h"
#include "hk_platform_cfg.h"
#include "hk_events.h"
#include "tst_hk_events.h"
#include "cfs_hk_requirements.h"

%liv (log_procedure) = logging

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

ut_requirement[HK_20012] = "N"
ut_requirement[HK_20013] = "N"
ut_requirement[HK_20015] = "N"
ut_requirement[HK_20016] = "N"
ut_requirement[HK_20017] = "N"

;**********************************************************************
; Set the local values
;**********************************************************************
LOCAL cfe_requirements[0 .. ut_req_array_size] = ["HK_2000","HK_2001", ;;
	"HK_2001.1","HK_2001.2","HK_2001.3","HK_2001.5","HK_2001.6", ;;
	"HK_2001.7","HK_3000","HK_4000"]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL entry
LOCAL appid
LOCAL OutputPacket1
LOCAL OutputPacket2
LOCAL OutputPacket3
LOCAL OutputPacket4
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
LOCAL InputPacket21
LOCAL DataBytePattern[0 .. 80]

local HKAppName = "HK"

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
write ";  Step 1.1:  Command a Power-On Reset on $CPU. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

;; Display the pages
page $SC_$CPU_HK_HK
page $SC_$CPU_TST_HK_HK
page $SC_$CPU_HK_COMBINED_PKT1
page $SC_$CPU_HK_COMBINED_PKT2
page $SC_$CPU_HK_COMBINED_PKT3
page $SC_$CPU_HK_COMBINED_PKT4

write ";*********************************************************************"
write ";  Step 1.2: Creating the copy table used for testing and upload it"
write ";********************************************************************"
s $SC_$CPU_hk_copytable1

;; Parse the filename configuration parameters for the default table filenames
local tableFileName = HK_COPY_TABLE_FILENAME
local slashLoc = %locate(tableFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  tableFileName = %substring(tableFileName,slashLoc+1,%length(tableFileName))
  slashLoc = %locate(tableFileName,"/")
enddo

write "==> Default Copy Table filename = '",tableFileName,"'"

s ftp_file("CF:0/apps", "hk_cpy_tbl.tbl", tableFileName, "$CPU", "P")

write ";*********************************************************************"
write ";  Step 1.3:  Start the Housekeeping (HK) and Test Applications"
write ";********************************************************************"
s $sc_$cpu_hk_start_apps("1.3")
wait 5

write ";*********************************************************************"
write ";  Step 1.4: Verify that the HK Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Check the HK tlm items to see if they are 0 or NULL
;; the TST_HK application sends its HK packet
if ($SC_$CPU_HK_CMDPC = 0) AND ($SC_$CPU_HK_CMDEC = 0) AND ;;
   ($SC_$CPU_HK_CMBPKTSSENT = 0) AND ($SC_$CPU_HK_MISSDATACTR = 0) THEN
  write "<*> Passed (4000) - Housekeeping telemetry initialized properly."
  ut_setrequirements HK_4000, "P"
else
  write "<!> Failed (4000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC                    = ",$SC_$CPU_HK_CMDPC
  write "  CMDEC                    = ",$SC_$CPU_HK_CMDEC
  write "  Combined Packets Sent    = ",$SC_$CPU_HK_CMBPKTSSENT
  write "  Missing Data Counter     = ",$SC_$CPU_HK_MISSDATACTR
  write "  Memory Pool Handle       = ",$SC_$CPU_HK_MEMPOOLHNDL
  ut_setrequirements HK_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.5: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdCtr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the HK application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=HKAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0: Basic housekeeping collection and output message sending."
write ";*********************************************************************"
write ";  Step 2.1: Send 20 input messages as per the copy table."
write ";*********************************************************************"
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

/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=4 DataPattern =0x01234567
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=8 DataPattern =0x12345678
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket3 DataSize=16 DataPattern =0x23456789
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket4 DataSize=32 DataPattern =0x3456789a
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket5 DataSize=32 DataPattern =0x456789ab
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket6 DataSize=16 DataPattern =0x56789abc
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket7 DataSize=8 DataPattern =0x6789abcd
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket8 DataSize=4 DataPattern =0x789abcde
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket9 DataSize=4 DataPattern =0x89abcdef
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket10 DataSize=8 DataPattern =0x9abcdef0
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket11 DataSize=8 DataPattern =0xabcdef01
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket12 DataSize=4 DataPattern =0xbcdef012
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket13 DataSize=4 DataPattern =0xcdef0123
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket14 DataSize=8 DataPattern =0xdef01234
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket15 DataSize=16 DataPattern =0xef012345
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket16 DataSize=32 DataPattern =0xf0123456
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket17 DataSize=32 DataPattern =0x76543210
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket18 DataSize=16 DataPattern =0x87654321
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket19 DataSize=8 DataPattern =0x98765432
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket20 DataSize=4 DataPattern =0xa9876543
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

write ";*********************************************************************"
write ";  Step 2.2: Send Output Message 1 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x01
DataBytePattern[1] = 0x23 
DataBytePattern[2] = 0x45
DataBytePattern[3] = 0x67 
DataBytePattern[4] = 0x12
DataBytePattern[5] = 0x34 
DataBytePattern[6] = 0x56
DataBytePattern[7] = 0x78 
DataBytePattern[8] = 0x23
DataBytePattern[9] = 0x45 
DataBytePattern[10] = 0x67
DataBytePattern[11] = 0x89 
DataBytePattern[12] = 0x34
DataBytePattern[13] = 0x56 
DataBytePattern[14] = 0x78
DataBytePattern[15] = 0x9a 
DataBytePattern[16] = 0x45
DataBytePattern[17] = 0x67 
DataBytePattern[18] = 0x89
DataBytePattern[19] = 0xab 
DataBytePattern[20] = 0x56
DataBytePattern[21] = 0x78 
DataBytePattern[22] = 0x9a
DataBytePattern[23] = 0xbc 
DataBytePattern[24] = 0x67
DataBytePattern[25] = 0x89 
DataBytePattern[26] = 0xab
DataBytePattern[27] = 0xcd 
DataBytePattern[28] = 0x78
DataBytePattern[29] = 0x9a 
DataBytePattern[30] = 0xbc
DataBytePattern[31] = 0xde 
DataBytePattern[32] = 0x89
DataBytePattern[33] = 0xab 
DataBytePattern[34] = 0xcd
DataBytePattern[35] = 0xef 
DataBytePattern[36] = 0x9a
DataBytePattern[37] = 0xbc 
DataBytePattern[38] = 0xde
DataBytePattern[39] = 0xf0 
DataBytePattern[40] = 0xab
DataBytePattern[41] = 0xcd 
DataBytePattern[42] = 0xef
DataBytePattern[43] = 0x01 
DataBytePattern[44] = 0xbc
DataBytePattern[45] = 0xde 
DataBytePattern[46] = 0xf0
DataBytePattern[47] = 0x12 
DataBytePattern[48] = 0xcd
DataBytePattern[49] = 0xef 
DataBytePattern[50] = 0x01
DataBytePattern[51] = 0x23 
DataBytePattern[52] = 0xde
DataBytePattern[53] = 0xf0 
DataBytePattern[54] = 0x12
DataBytePattern[55] = 0x34 
DataBytePattern[56] = 0xef
DataBytePattern[57] = 0x01 
DataBytePattern[58] = 0x23
DataBytePattern[59] = 0x45 
DataBytePattern[60] = 0xf0
DataBytePattern[61] = 0x12 
DataBytePattern[62] = 0x34
DataBytePattern[63] = 0x56 
DataBytePattern[64] = 0x76
DataBytePattern[65] = 0x54 
DataBytePattern[66] = 0x32
DataBytePattern[67] = 0x10 
DataBytePattern[68] = 0x87
DataBytePattern[69] = 0x65 
DataBytePattern[70] = 0x43
DataBytePattern[71] = 0x21 
DataBytePattern[72] = 0x98
DataBytePattern[73] = 0x76 
DataBytePattern[74] = 0x54
DataBytePattern[75] = 0x32 
DataBytePattern[76] = 0xa9
DataBytePattern[77] = 0x87 
DataBytePattern[78] = 0x65
DataBytePattern[79] = 0x43								
s $sc_$cpu_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.3: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x65
DataBytePattern[1] = 0x43 
DataBytePattern[2] = 0x54
DataBytePattern[3] = 0x32 
DataBytePattern[4] = 0x43
DataBytePattern[5] = 0x21 
DataBytePattern[6] = 0x32
DataBytePattern[7] = 0x10
DataBytePattern[8] = 0x12
DataBytePattern[9] = 0x34 
DataBytePattern[10] = 0x01
DataBytePattern[11] = 0x23 
DataBytePattern[12] = 0xf0
DataBytePattern[13] = 0x12 
DataBytePattern[14] = 0xef
DataBytePattern[15] = 0x01 
DataBytePattern[16] = 0xf0
DataBytePattern[17] = 0x12 
DataBytePattern[18] = 0xef
DataBytePattern[19] = 0x01 
DataBytePattern[20] = 0xbc
DataBytePattern[21] = 0xde 
DataBytePattern[22] = 0xab
DataBytePattern[23] = 0xcd 
DataBytePattern[24] = 0xbc
DataBytePattern[25] = 0xde 
DataBytePattern[26] = 0xab
DataBytePattern[27] = 0xcd 
DataBytePattern[28] = 0x9a
DataBytePattern[29] = 0xbc 
DataBytePattern[30] = 0x89
DataBytePattern[31] = 0xab 
DataBytePattern[32] = 0x56
DataBytePattern[33] = 0x78 
DataBytePattern[34] = 0x45
DataBytePattern[35] = 0x67 
DataBytePattern[36] = 0x34
DataBytePattern[37] = 0x56 
DataBytePattern[38] = 0x23
DataBytePattern[39] = 0x45							

for entry = 40 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $sc_$cpu_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.4: Send Output Message 3 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0xf0
DataBytePattern[1] = 0x01 
DataBytePattern[2] = 0xef
DataBytePattern[3] = 0x12 
DataBytePattern[4] = 0xde
DataBytePattern[5] = 0x23 
DataBytePattern[6] = 0xcd
DataBytePattern[7] = 0x34 
DataBytePattern[8] = 0xbc
DataBytePattern[9] = 0x45 
DataBytePattern[10] = 0xab
DataBytePattern[11] = 0x56 
DataBytePattern[12] = 0x9a
DataBytePattern[13] = 0x10 
DataBytePattern[14] = 0x89
DataBytePattern[15] = 0x21 
DataBytePattern[16] = 0x78
DataBytePattern[17] = 0x32 
DataBytePattern[18] = 0x67
DataBytePattern[19] = 0x43								
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $sc_$cpu_hk_sendoutmsg(OutputPacket3, DataBytePattern, Pkt3, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 3.0:  Update Copy Table."
write ";*********************************************************************"
write ";  Step 3.1: Generate and upload updated copy table"
write ";*********************************************************************"
s $SC_$CPU_hk_copytable2

start load_table("hk_cpy_tbl.tbl", "$CPU")
wait 5

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=""HK.CopyTable"""
wait 5

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=""HK.CopyTable"""
wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send 21 input messages 20 are defined the copy table."
write ";  Input packet id x895, x995, xa95 no longer in copy table"
write ";  Output packet id x89e, x99e, xa9e no longer in copy table"
write ";*********************************************************************"
;; CPU1 is the default
OutputPacket4 = 0x89f
InputPacket21 = 0x9a2

if ("$CPU" = "CPU2") then
   OutputPacket4 = 0x99f
   InputPacket21 = 0xaa2
elseif ("$CPU" = "CPU3") then
   OutputPacket4 = 0xa9f
   InputPacket21 = 0x8a2
endif 

;; Already did Pkts 1 & 2 in Step 2.1
;;/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
;;/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=4 DataPattern =0x11111111
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=8 DataPattern =0x22222222
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket3 DataSize=16 DataPattern =0x33333333
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket4 DataSize=32 DataPattern =0x44444444
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket5 DataSize=32 DataPattern =0x55555555
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket6 DataSize=16 DataPattern =0x66666666
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket7 DataSize=8 DataPattern =0x77777777
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket8 DataSize=4 DataPattern =0x88888888
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket9 DataSize=4 DataPattern =0x99999999
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket10 DataSize=8 DataPattern =0xaaaaaaaa
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket11 DataSize=8 DataPattern =0xbbbbbbbb
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket12 DataSize=4 DataPattern =0xcccccccc
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket13 DataSize=4 DataPattern =0xdddddddd
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket14 DataSize=8 DataPattern =0xeeeeeeee
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket15 DataSize=16 DataPattern =0x12345678
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket16 DataSize=32 DataPattern =0x16161616
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket17 DataSize=32 DataPattern =0x17171717
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket18 DataSize=16 DataPattern =0x18181818
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket19 DataSize=8 DataPattern =0x19191919
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket20 DataSize=4 DataPattern =0x20202020
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket21 DataSize=16 DataPattern =0xffffffff
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

write ";*********************************************************************"
write ";  Step 3.3: Send Output Message 1 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0x11
DataBytePattern[2] = 0x11
DataBytePattern[3] = 0x11 
DataBytePattern[4] = 0x22
DataBytePattern[5] = 0x22 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0x22 
DataBytePattern[8] = 0x33
DataBytePattern[9] = 0x33 
DataBytePattern[10] = 0x33
DataBytePattern[11] = 0x33
DataBytePattern[12] = 0x44
DataBytePattern[13] = 0x44 
DataBytePattern[14] = 0x44
DataBytePattern[15] = 0x44 
DataBytePattern[16] = 0x55
DataBytePattern[17] = 0x55 
DataBytePattern[18] = 0x55
DataBytePattern[19] = 0x55 
DataBytePattern[20] = 0x66
DataBytePattern[21] = 0x66 
DataBytePattern[22] = 0x66
DataBytePattern[23] = 0x66 
DataBytePattern[24] = 0x77
DataBytePattern[25] = 0x77 
DataBytePattern[26] = 0x77 
DataBytePattern[27] = 0x77  
DataBytePattern[28] = 0x88 
DataBytePattern[29] = 0x88  
DataBytePattern[30] = 0x88 
DataBytePattern[31] = 0x88  
DataBytePattern[32] = 0x99 
DataBytePattern[33] = 0x99 
DataBytePattern[34] = 0x99
DataBytePattern[35] = 0x99 
DataBytePattern[36] = 0xaa
DataBytePattern[37] = 0xaa 
DataBytePattern[38] = 0xaa
DataBytePattern[39] = 0xaa
DataBytePattern[40] = 0xbb
DataBytePattern[41] = 0xbb 
DataBytePattern[42] = 0xbb
DataBytePattern[43] = 0xbb 
DataBytePattern[44] = 0xcc
DataBytePattern[45] = 0xcc
DataBytePattern[46] = 0xcc
DataBytePattern[47] = 0xcc 
DataBytePattern[48] = 0xdd
DataBytePattern[49] = 0xdd 
DataBytePattern[50] = 0xdd
DataBytePattern[51] = 0xdd 
DataBytePattern[52] = 0xee
DataBytePattern[53] = 0xee 
DataBytePattern[54] = 0xee
DataBytePattern[55] = 0xee 
DataBytePattern[56] = 0xff
DataBytePattern[57] = 0xff
DataBytePattern[58] = 0xff
DataBytePattern[59] = 0xff 
DataBytePattern[60] = 0x16
DataBytePattern[61] = 0x16 
DataBytePattern[62] = 0x16
DataBytePattern[63] = 0x16
DataBytePattern[64] = 0x17
DataBytePattern[65] = 0x17 
DataBytePattern[66] = 0x17
DataBytePattern[67] = 0x17 
DataBytePattern[68] = 0x18
DataBytePattern[69] = 0x18 
DataBytePattern[70] = 0x18
DataBytePattern[71] = 0x18 
DataBytePattern[72] = 0x19
DataBytePattern[73] = 0x19 
DataBytePattern[74] = 0x19
DataBytePattern[75] = 0x19 								
for entry = 76 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $sc_$cpu_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, TblUpdate, 0)

write ";*********************************************************************"
write ";  Step 3.4: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x20
DataBytePattern[1] = 0x20 
DataBytePattern[2] = 0x19
DataBytePattern[3] = 0x19 
DataBytePattern[4] = 0x18
DataBytePattern[5] = 0x18 
DataBytePattern[6] = 0x17
DataBytePattern[7] = 0x17 
DataBytePattern[8] = 0x16
DataBytePattern[9] = 0x16 
DataBytePattern[10] = 0xff
DataBytePattern[11] = 0xff 
DataBytePattern[12] = 0xee
DataBytePattern[13] = 0xee 
DataBytePattern[14] = 0xdd
DataBytePattern[15] = 0xdd 
DataBytePattern[16] = 0xcc
DataBytePattern[17] = 0xcc 
DataBytePattern[18] = 0xbb
DataBytePattern[19] = 0xbb 
DataBytePattern[20] = 0xaa
DataBytePattern[21] = 0xaa 
DataBytePattern[22] = 0x99
DataBytePattern[23] = 0x99 
DataBytePattern[24] = 0x88
DataBytePattern[25] = 0x88 
DataBytePattern[26] = 0x77
DataBytePattern[27] = 0x77 
DataBytePattern[28] = 0x66
DataBytePattern[29] = 0x66 
DataBytePattern[30] = 0x55
DataBytePattern[31] = 0x55 
DataBytePattern[32] = 0x44
DataBytePattern[33] = 0x44 
DataBytePattern[34] = 0x33
DataBytePattern[35] = 0x33 
DataBytePattern[36] = 0x22
DataBytePattern[37] = 0x22 
DataBytePattern[38] = 0x11
DataBytePattern[39] = 0x11								
for entry = 40 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $sc_$cpu_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, TblUpdate, 0)

write ";*********************************************************************"
write ";  Step 3.5: Send Output Message 4 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0xaa
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x99
DataBytePattern[3] = 0xcc 
DataBytePattern[4] = 0x88
DataBytePattern[5] = 0xdd 
DataBytePattern[6] = 0x77
DataBytePattern[7] = 0xee 
DataBytePattern[8] = 0x66
DataBytePattern[9] = 0xff 
DataBytePattern[10] = 0x55
DataBytePattern[11] = 0x16 
DataBytePattern[12] = 0x44
DataBytePattern[13] = 0x17 
DataBytePattern[14] = 0x33
DataBytePattern[15] = 0x18 
DataBytePattern[16] = 0x22
DataBytePattern[17] = 0x19 
DataBytePattern[18] = 0x19
DataBytePattern[19] = 0x11								
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $sc_$cpu_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, TblUpdate, 0)

write ";*********************************************************************"
write ";  Step 4.0:  Perform a Power-on Reset to clean-up from this test."
write ";*********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 75
                                                                                
cfe_startup $CPU
wait 5

write "**** Requirements Status Reporting"
                                                                                
write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"
                                                                                
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_hk_basichousekeeping                        "
write ";*********************************************************************"
ENDPROC
