PROC $sc_$cpu_hk_stresshousekeeping
;*******************************************************************************
;  Test Name:  hk_stresshousekeeping
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to stress the Housekeeping subsystems by 
;       setting up a copy table with a large number of input and output
;	messages. It also tests sending invalid message ids in the Output
;	Message x requests and receiving input packets whose lengths are smaller
;	than what was defined in the copy table 
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
;       HK2001.5 If the <PLATFORM_DEFINED> parameter Discard Combo Packets is
;		 set to NO and the input message offset + bytes for any input
;		 message specified in the HK table is greater than the received
;		 message length then HK shall use the last received data
;		 associated with that message and issue no more than one event
;		 per input message.
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
;	06/04/08	Barbie Medina	Original Procedure.
;       07/02/10        Walt Moleski    Updated to use the default table name
;                                       and to call $sc_$cpu_hk_start_apps
;       03/09/11        Walt Moleski    Added variables for app and table name
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
;                         message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			requirements array.
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
#include "cfe_tbl_events.h"
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
LOCAL OutputPacket5
LOCAL OutputPacket6
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
local HKCopyTblName = HKAppName & "." & HK_COPY_TABLE_NAME

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
page $SC_$CPU_HK_COMBINED_PKT5
page $SC_$CPU_HK_COMBINED_PKT6

write ";*********************************************************************"
write ";  Step 1.2: Creating the copy table used for testing and upload it"
write ";********************************************************************"
s $SC_$CPU_hk_copytable3

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
write ";  Step 1.3:  Start the Housekeeping (HK) and Test Applications.  "
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
write ";  Step 2.1: Send 2 input messages as per the copy table."
write ";*********************************************************************"
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

/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket2 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket3 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket4 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket5 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10
/$SC_$CPU_TO_ADDPACKET Stream=OutputPacket6 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=32 DataPattern=0x01234567
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=32 DataPattern=0x89abcdef
wait 2

write ";*********************************************************************"
write ";  Step 2.2: Send Output Message 1 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x01
DataBytePattern[1] = 0xef 
DataBytePattern[2] = 0x45
DataBytePattern[3] = 0xab 
DataBytePattern[4] = 0x01
DataBytePattern[5] = 0xef 
DataBytePattern[6] = 0x45
DataBytePattern[7] = 0xab 
DataBytePattern[8] = 0x01
DataBytePattern[9] = 0xef 
DataBytePattern[10] = 0x45
DataBytePattern[11] = 0xab 
DataBytePattern[12] = 0x01
DataBytePattern[13] = 0xef 
DataBytePattern[14] = 0x45
DataBytePattern[15] = 0xab 
DataBytePattern[16] = 0x01
DataBytePattern[17] = 0xef 
DataBytePattern[18] = 0x45
DataBytePattern[19] = 0xab 
DataBytePattern[20] = 0x01
DataBytePattern[21] = 0xef
				
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo			

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.3: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x23
DataBytePattern[1] = 0xcd 
DataBytePattern[2] = 0x67
DataBytePattern[3] = 0x89 
DataBytePattern[4] = 0x23
DataBytePattern[5] = 0xcd 
DataBytePattern[6] = 0x67
DataBytePattern[7] = 0x89 
DataBytePattern[8] = 0x23
DataBytePattern[9] = 0xcd 
DataBytePattern[10] = 0x67
DataBytePattern[11] = 0x89 
DataBytePattern[12] = 0x23
DataBytePattern[13] = 0xcd 
DataBytePattern[14] = 0x67
DataBytePattern[15] = 0x89 
DataBytePattern[16] = 0x23
DataBytePattern[17] = 0xcd 
DataBytePattern[18] = 0x67
DataBytePattern[19] = 0x89 
DataBytePattern[20] = 0x23
DataBytePattern[21] = 0xcd
					
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.4: Send Output Message 3 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x45
DataBytePattern[1] = 0xab 
DataBytePattern[2] = 0x01
DataBytePattern[3] = 0xef 
DataBytePattern[4] = 0x45
DataBytePattern[5] = 0xab 
DataBytePattern[6] = 0x01
DataBytePattern[7] = 0xef 
DataBytePattern[8] = 0x45
DataBytePattern[9] = 0xab 
DataBytePattern[10] = 0x01
DataBytePattern[11] = 0xef 
DataBytePattern[12] = 0x45
DataBytePattern[13] = 0xab 
DataBytePattern[14] = 0x01
DataBytePattern[15] = 0xef 
DataBytePattern[16] = 0x45
DataBytePattern[17] = 0xab 
DataBytePattern[18] = 0x01
DataBytePattern[19] = 0xef 
DataBytePattern[20] = 0x45
DataBytePattern[21] = 0xab
							

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket3, DataBytePattern, Pkt3, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.5: Send Output Message 4 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x67
DataBytePattern[1] = 0x89 
DataBytePattern[2] = 0x23
DataBytePattern[3] = 0xcd 
DataBytePattern[4] = 0x67
DataBytePattern[5] = 0x89 
DataBytePattern[6] = 0x23
DataBytePattern[7] = 0xcd 
DataBytePattern[8] = 0x67
DataBytePattern[9] = 0x89 
DataBytePattern[10] = 0x23
DataBytePattern[11] = 0xcd 
DataBytePattern[12] = 0x67
DataBytePattern[13] = 0x89 
DataBytePattern[14] = 0x23
DataBytePattern[15] = 0xcd 
DataBytePattern[16] = 0x67
DataBytePattern[17] = 0x89 
DataBytePattern[18] = 0x23
DataBytePattern[19] = 0xcd 
DataBytePattern[20] = 0x67
DataBytePattern[21] = 0x89 
							

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.6: Send Output Message 5 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x01
DataBytePattern[1] = 0xef 
DataBytePattern[2] = 0x45
DataBytePattern[3] = 0xab 
DataBytePattern[4] = 0x01
DataBytePattern[5] = 0xef 
DataBytePattern[6] = 0x45
DataBytePattern[7] = 0xab 
DataBytePattern[8] = 0x01
DataBytePattern[9] = 0xef 
DataBytePattern[10] = 0x45
DataBytePattern[11] = 0xab 
DataBytePattern[12] = 0x01
DataBytePattern[13] = 0xef 
DataBytePattern[14] = 0x45
DataBytePattern[15] = 0xab 
DataBytePattern[16] = 0x01
DataBytePattern[17] = 0xef 
DataBytePattern[18] = 0x45
DataBytePattern[19] = 0xab 
							
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket5, DataBytePattern, Pkt5, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 2.7: Send Output Message 6 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x23
DataBytePattern[1] = 0xcd 
DataBytePattern[2] = 0x67
DataBytePattern[3] = 0x89 
DataBytePattern[4] = 0x23
DataBytePattern[5] = 0xcd 
DataBytePattern[6] = 0x67
DataBytePattern[7] = 0x89 
DataBytePattern[8] = 0x23
DataBytePattern[9] = 0xcd 
DataBytePattern[10] = 0x67
DataBytePattern[11] = 0x89 
DataBytePattern[12] = 0x23
DataBytePattern[13] = 0xcd 
DataBytePattern[14] = 0x67
DataBytePattern[15] = 0x89 
DataBytePattern[16] = 0x23
DataBytePattern[17] = 0xcd 
DataBytePattern[18] = 0x67
DataBytePattern[19] = 0x89 
							
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket6, DataBytePattern, Pkt6, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 3.0: Test unknown output id."
write ";*********************************************************************"
write ";  Step 3.1: Send Output Message command using invalid id"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{HKAppName},HK_UNKNOWN_COMBINED_PACKET_EID,"INFO",1

/$SC_$CPU_TST_HK_SENDOUTMSG MsgId=InputPacket1 Pad=0
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (2001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements HK_2001, "P"
else
   write "<!> Failed (2001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_UNKNOWN_COMBINED_PACKET_EID, "."
   ut_setrequirements HK_2001, "F"
endif

write ";*********************************************************************"
write ";  Step 4.0: Test message length error."
write ";*********************************************************************"
write ";  Step 4.1: Update Copytable and send input messages.  One has a "
write ";            length smaller than offset +bytes"
write ";*********************************************************************"
s $SC_$CPU_hk_copytable2

;; Load the table
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table("hk_cpy_tbl.tbl", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Copy Table #2 sent successfully."
else
  write "<!> Failed - Load command for Copy Table #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=HKCopyTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for Copy Table #2 sent successfully."
else
  write "<!> Failed - Validate command for Copy Table #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Validate command."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=HKCopyTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for Copy Table #2 sent successfully."
else
  write "<!> Failed - Activate command for Copy Table #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Activate command."
endif

write ";*********************************************************************"
write ";  Step 4.2: Send 20 input messages as defined in the copy table."
write ";  InputPacket19 will be too short"
write ";*********************************************************************"
;; CPU1 is the default
;; Use CPU2 IDs
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
InputPacket21 = 0x9a2

if ("$CPU" = "CPU2") then
;; Use CPU3 IDs
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
   InputPacket21 = 0xaa2
elseif ("$CPU" = "CPU3") then
   ;; Use CPU1 IDs
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
   InputPacket21 = 0x8a2
endif 

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=4 DataPattern=0x11111111
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=8 DataPattern=0x22222222
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket3 DataSize=16 DataPattern=0x33333333
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket4 DataSize=32 DataPattern=0x44444444
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket5 DataSize=32 DataPattern=0x55555555
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket6 DataSize=16 DataPattern=0x66666666
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket7 DataSize=8 DataPattern=0x77777777
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket8 DataSize=4 DataPattern=0x88888888
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket9 DataSize=4 DataPattern=0x99999999
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket10 DataSize=8 DataPattern=0xaaaaaaaa
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket11 DataSize=8 DataPattern=0xbbbbbbbb
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket12 DataSize=4 DataPattern=0xcccccccc
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket13 DataSize=4 DataPattern=0xdddddddd
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket14 DataSize=8 DataPattern=0xeeeeeeee
wait 2
;;/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket15 DataSize=16 DataPattern=0x12345678
;;wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket16 DataSize=32 DataPattern=0x16161616
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket17 DataSize=32 DataPattern=0x17171717
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket18 DataSize=16 DataPattern=0x18181818
wait 2

ut_setupevents "$SC", "$CPU", {HKAppName}, HK_ACCESSING_PAST_PACKET_END_EID, "ERROR", 1

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket19 DataSize=4 DataPattern=0x19191919
wait 2

wait 10
if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
   write"<*> Passed (2001.5) - Event message", $SC_$CPU_find_event[1].eventid, " received"
   ut_setrequirements HK_20015, "P"
else
   write "<!> Failed (2001.5) - Event message ", $SC_$CPU_evs_eventid," received.  Expected Event message ", HK_ACCESSING_PAST_PACKET_END_EID, "."
   ut_setrequirements HK_20015, "F"
endif

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket20 DataSize=4 DataPattern=0x20202020
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket21 DataSize=16 DataPattern=0xffffffff
wait 2

write ";*********************************************************************"
write ";  Step 4.3: Send Output Message 1 command and check data"
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

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, TblUpdate, 0)

write ";*********************************************************************"
write ";  Step 4.4: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x20
DataBytePattern[1] = 0x20 
DataBytePattern[2] = 0x00
DataBytePattern[3] = 0x00 
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

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, TstLenTstTbl, 0)

write ";*********************************************************************"
write ";  Step 4.5: Send Output Message 4 command and check data"
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
DataBytePattern[17] = 0x00 
DataBytePattern[18] = 0x00
DataBytePattern[19] = 0x11

for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, TstLenTstTbl, 0)

write ";*********************************************************************"
write ";  Step 5.0: Test collection and output of odd sized messages"
write ";*********************************************************************"
write ";  Step 5.1: Load new table"
write ";*********************************************************************"
s $SC_$CPU_hk_copytable4

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table("hk_cpy_tbl.tbl", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Copy Table #4 sent successfully."
else
  write "<!> Failed - Load command for Copy Table #4 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=HKCopyTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for Copy Table #4 sent successfully."
else
  write "<!> Failed - Validate command for Copy Table #4 did not execute successfully."
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Validate command."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=HKCopyTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for Copy Table #4 sent successfully."
else
  write "<!> Failed - Activate command for Copy Table #4 did not execute successfully."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Activate command."
endif

write ";*********************************************************************"
write ";  Step 5.2: Send input messages"
write ";*********************************************************************"

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=4 DataPattern=0x01234567
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=8 DataPattern=0x12345678
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket3 DataSize=4 DataPattern=0x23456789
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket4 DataSize=8 DataPattern=0x3456789a
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket5 DataSize=4 DataPattern=0x456789ab
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket6 DataSize=3 DataPattern=0x56789a00
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket7 DataSize=8 DataPattern=0x6789abcd
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket8 DataSize=8 DataPattern=0x789abcde
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket9 DataSize=4 DataPattern=0x89abcdef
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket10 DataSize=8 DataPattern=0x9abcdef0
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket11 DataSize=4 DataPattern=0xabcdef01
wait 2

write ";*********************************************************************"
write ";  Step 5.3: Request output message 1"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x01
DataBytePattern[1] = 0x23 
DataBytePattern[2] = 0x34
DataBytePattern[3] = 0x56 
DataBytePattern[4] = 0x67
DataBytePattern[5] = 0x89 
DataBytePattern[6] = 0x9a
DataBytePattern[7] = 0x34 
DataBytePattern[8] = 0x45
DataBytePattern[9] = 0x67 
DataBytePattern[10] = 0x78
DataBytePattern[11] = 0x9a 
DataBytePattern[12] = 0xab
DataBytePattern[13] = 0xcd 
DataBytePattern[14] = 0xde
DataBytePattern[15] = 0x78 
DataBytePattern[16] = 0x89
DataBytePattern[17] = 0xab 
DataBytePattern[18] = 0xbc
DataBytePattern[19] = 0xde 
DataBytePattern[20] = 0xef
DataBytePattern[21] = 0x01

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, TblUpdate, 0)

write ";*********************************************************************"
write ";  Step 5.4: Request output message 2"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x01
DataBytePattern[1] = 0x12 
DataBytePattern[2] = 0x23
DataBytePattern[3] = 0x34 
DataBytePattern[4] = 0x45
DataBytePattern[5] = 0x56 
DataBytePattern[6] = 0x67
DataBytePattern[7] = 0x78 
DataBytePattern[8] = 0x89
DataBytePattern[9] = 0x9a 
DataBytePattern[10] = 0xab					

for entry = 11 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, TblUpdate, 0)

write ";*********************************************************************"
write ";  Step 6.0: Test gaps in the copy data"
write ";*********************************************************************"
write ";  Step 6.1: Load new table"
write ";*********************************************************************"
s $SC_$CPU_hk_copytable5

ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_FILE_LOADED_INF_EID, "INFO", 1
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VALIDATION_INF_EID, "INFO", 2
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_UPDATE_SUCCESS_INF_EID,"INFO",3

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

start load_table("hk_cpy_tbl.tbl", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Load command for Copy Table #2 sent successfully."
else
  write "<!> Failed - Load command for Copy Table #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Load command."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=HKCopyTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Validate command for Copy Table #2 sent successfully."
else
  write "<!> Failed - Validate command for Copy Table #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Validate command."
endif

cmdCtr = $SC_$CPU_TBL_CMDPC + 1

/$SC_$CPU_TBL_ACTIVATE ATABLENAME=HKCopyTblName

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Activate command for Copy Table #2 sent successfully."
else
  write "<!> Failed - Activate command for Copy Table #2 did not execute successfully."
endif

if ($SC_$CPU_find_event[3].num_found_messages = 1) then
  write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
else
  write "<!> Failed - Event Message not received for Activate command."
endif
write ";*********************************************************************"
write ";  Step 6.2: Send 19 input message"
write ";*********************************************************************"

/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=4 DataPattern=0x01234567
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=8 DataPattern=0x12345678
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket3 DataSize=16 DataPattern=0x23456789
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket4 DataSize=32 DataPattern=0x3456789a
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket5 DataSize=32 DataPattern=0x456789ab
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket6 DataSize=16 DataPattern=0x56789abc
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket7 DataSize=8 DataPattern=0x6789abcd
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket8 DataSize=4 DataPattern=0x789abcde
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket9 DataSize=4 DataPattern=0x89abcdef
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket10 DataSize=8 DataPattern=0x9abcdef0
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket11 DataSize=8 DataPattern=0xabcdef01
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket12 DataSize=4 DataPattern=0xbcdef012
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket13 DataSize=4 DataPattern=0xcdef0123
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket14 DataSize=8 DataPattern=0xdef01234
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket15 DataSize=16 DataPattern=0xef012345
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket16 DataSize=32 DataPattern=0xf0123456
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket17 DataSize=32 DataPattern=0x76543210
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket18 DataSize=16 DataPattern=0x87654321
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket19 DataSize=8 DataPattern=0x98765432
wait 2

write ";*********************************************************************"
write ";  Step 6.3: Request output message 1"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x01
DataBytePattern[1] = 0x12 
DataBytePattern[2] = 0x34
DataBytePattern[3] = 0x23 
DataBytePattern[4] = 0x45
DataBytePattern[5] = 0x67 
DataBytePattern[6] = 0x34
DataBytePattern[7] = 0x56 
DataBytePattern[8] = 0x78
DataBytePattern[9] = 0x9a 
DataBytePattern[10] = 0x45
DataBytePattern[11] = 0x67 
DataBytePattern[12] = 0x89
DataBytePattern[13] = 0xab 
DataBytePattern[14] = 0x56
DataBytePattern[15] = 0x78 
DataBytePattern[16] = 0x9a
DataBytePattern[17] = 0x67 
DataBytePattern[18] = 0x89
DataBytePattern[19] = 0x78 
DataBytePattern[20] = 0x89
DataBytePattern[21] = 0x9a 
DataBytePattern[22] = 0xbc
DataBytePattern[23] = 0xab 
DataBytePattern[24] = 0xcd
DataBytePattern[25] = 0xef 
DataBytePattern[26] = 0xbc
DataBytePattern[27] = 0xde 
DataBytePattern[28] = 0xf0
DataBytePattern[29] = 0x12 
DataBytePattern[30] = 0xcd
DataBytePattern[31] = 0xef 
DataBytePattern[32] = 0x01
DataBytePattern[33] = 0x23 
DataBytePattern[34] = 0xde
DataBytePattern[35] = 0xf0 
DataBytePattern[36] = 0x12
DataBytePattern[37] = 0xef 
DataBytePattern[38] = 0x01
DataBytePattern[39] = 0xf0 
DataBytePattern[40] = 0x76
DataBytePattern[41] = 0x87 
DataBytePattern[42] = 0x65
DataBytePattern[43] = 0x98 
DataBytePattern[44] = 0x76
DataBytePattern[45] = 0x54

for entry = 46 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 6.4: Request output message 2"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x98
DataBytePattern[1] = 0x76 
DataBytePattern[2] = 0x43
DataBytePattern[3] = 0x21 
DataBytePattern[4] = 0x10
DataBytePattern[5] = 0x76 
DataBytePattern[6] = 0x34
DataBytePattern[7] = 0x56 
DataBytePattern[8] = 0x01
DataBytePattern[9] = 0x23 
DataBytePattern[10] = 0xde
DataBytePattern[11] = 0xf0 
DataBytePattern[12] = 0xef
DataBytePattern[13] = 0x01 
DataBytePattern[14] = 0xf0
DataBytePattern[15] = 0x12 
DataBytePattern[16] = 0xab
DataBytePattern[17] = 0xcd 
DataBytePattern[18] = 0xf0
DataBytePattern[19] = 0x9a 
DataBytePattern[20] = 0xcd
DataBytePattern[21] = 0xef 
DataBytePattern[22] = 0x9a
DataBytePattern[23] = 0xbc 
DataBytePattern[24] = 0x89
DataBytePattern[25] = 0xab 
DataBytePattern[26] = 0x9a
DataBytePattern[27] = 0xbc 
DataBytePattern[28] = 0x67
DataBytePattern[29] = 0x89 
DataBytePattern[30] = 0x34
DataBytePattern[31] = 0x56 
DataBytePattern[32] = 0x89
DataBytePattern[33] = 0x23 
DataBytePattern[34] = 0x56
DataBytePattern[35] = 0x78 
DataBytePattern[36] = 0x01
DataBytePattern[37] = 0x23

for entry = 38 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 6.5: Request output message 3"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x76
DataBytePattern[1] = 0x65 
DataBytePattern[2] = 0x43
DataBytePattern[3] = 0x21 
DataBytePattern[4] = 0x54
DataBytePattern[5] = 0x12 
DataBytePattern[6] = 0x34
DataBytePattern[7] = 0x56 
DataBytePattern[8] = 0x01
DataBytePattern[9] = 0xf0 
DataBytePattern[10] = 0x12
DataBytePattern[11] = 0x34 
DataBytePattern[12] = 0xef
DataBytePattern[13] = 0xde 
DataBytePattern[14] = 0xf0
DataBytePattern[15] = 0x12 
DataBytePattern[16] = 0xcd
DataBytePattern[17] = 0xbc
DataBytePattern[18] = 0xde
DataBytePattern[19] = 0xf0 
DataBytePattern[20] = 0xab
DataBytePattern[21] = 0x9a 
DataBytePattern[22] = 0xbc
DataBytePattern[23] = 0xde 
DataBytePattern[24] = 0x89
DataBytePattern[25] = 0x78 
DataBytePattern[26] = 0x9a
DataBytePattern[27] = 0xbc 
DataBytePattern[28] = 0x67
DataBytePattern[29] = 0x56 
DataBytePattern[30] = 0x78
DataBytePattern[31] = 0x9a 
DataBytePattern[32] = 0x45
DataBytePattern[33] = 0x34 
DataBytePattern[34] = 0x56
DataBytePattern[35] = 0x78 
DataBytePattern[36] = 0x23

for entry = 37 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket3, DataBytePattern, Pkt3, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 6.6: Request output message 4"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x67
DataBytePattern[1] = 0x78 
DataBytePattern[2] = 0x89
DataBytePattern[3] = 0x9a 
DataBytePattern[4] = 0xab
DataBytePattern[5] = 0xbc 
DataBytePattern[6] = 0xcd
DataBytePattern[7] = 0xde 
DataBytePattern[8] = 0xef
DataBytePattern[9] = 0xf0 
DataBytePattern[10] = 0x01
DataBytePattern[11] = 0x12 
DataBytePattern[12] = 0x23
DataBytePattern[13] = 0x34 
DataBytePattern[14] = 0x45
DataBytePattern[15] = 0x56 
DataBytePattern[16] = 0x10
DataBytePattern[17] = 0x21 
DataBytePattern[18] = 0x32

for entry = 19 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 7.0:  Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $SC_$CPU_hk_stresshousekeeping                       "
write ";*********************************************************************"
ENDPROC
