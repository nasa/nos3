PROC $sc_$cpu_hk_stressmissingdata
;*******************************************************************************
;  Test Name:  hk_stressmissingdata
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to stress the Housekeeping (HK) application
;       by sending it data with a large number of input messages missing.  
;
;  Requirements Tested
;       HK2000	 HK shall collect flight software housekeeping data from table
;                -specified input messages
;       HK2001	 HK shall output up to a maximum <Mission-Defined> table-defined
;                messages, at the scheduled rate, by combining input message 
;                data starting at the table-defined offset and table-defined 
;                number of bytes to the table-defined offset in the output
;		 message.
;       HK2001.2 If HK does not receive a message from an application, HK shall
;                use all values associated with last received message for that
;		 application in the combined message for that telemetry
;		 collection period
;       HK2001.3 If HK does not receive a message from an application, HK app
;                shall set a Stale flag and send an event specifying the message
;		 ID for the missing message
;       HK2001.6 If the <PLATFORM_DEFINED> parameter Discard Combo Packets is
;                set to YES and HK does not receive a message from an
;                application, HK shall discard the combined message containing
;                the values associated with the missing application message for
;                that telemetry collection period.
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
;       03/09/11        Walt Moleski    Added variables for app and table names
;       08/29/12        Walt Moleski    Updated to add check of Discard Packet
;					configuration parameter
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait      Wait for a specified telemetry point to update to
;                         a specified value. 
;       ut_sendcmd      Send commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_sendrawcmd   Send raw commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_pfindicate   Print the pass fail status of a particular requirement
;                         number.
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

ut_requirement[HK_20011] = "N"
ut_requirement[HK_20015] = "N"
ut_requirement[HK_20017] = "N"

;; Set 2001.2 to "N" if the DISCARD configuration parameter is set
if (HK_DISCARD_INCOMPLETE_COMBO = 1) then
  ut_requirement[HK_20012] = "N"
else
  ut_requirement[HK_20016] = "N"
endif

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
write ";  Step 1.3:  Start the Housekeeping (HK) and Test Applications."
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
write ";  Step 3.0: Send only 1/2 the data."
write ";*********************************************************************"
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=32 DataPattern=0x12345678
ut_tlmupdate $SC_$CPU_TST_HK_CMDPC

write ";*********************************************************************"
write ";  Step 3.1: Send Output Message 1 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x12
DataBytePattern[1] = 0xef 
DataBytePattern[2] = 0x56
DataBytePattern[3] = 0xab 
DataBytePattern[4] = 0x12
DataBytePattern[5] = 0xef 
DataBytePattern[6] = 0x56
DataBytePattern[7] = 0xab 
DataBytePattern[8] = 0x12
DataBytePattern[9] = 0xef 
DataBytePattern[10] = 0x56
DataBytePattern[11] = 0xab 
DataBytePattern[12] = 0x12
DataBytePattern[13] = 0xef 
DataBytePattern[14] = 0x56
DataBytePattern[15] = 0xab 
DataBytePattern[16] = 0x12
DataBytePattern[17] = 0xef 
DataBytePattern[18] = 0x56
DataBytePattern[19] = 0xab 
DataBytePattern[20] = 0x12
DataBytePattern[21] = 0xef
				
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo			

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 3.2: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x34
DataBytePattern[1] = 0xcd 
DataBytePattern[2] = 0x78
DataBytePattern[3] = 0x89 
DataBytePattern[4] = 0x34
DataBytePattern[5] = 0xcd 
DataBytePattern[6] = 0x78
DataBytePattern[7] = 0x89 
DataBytePattern[8] = 0x34
DataBytePattern[9] = 0xcd 
DataBytePattern[10] = 0x78
DataBytePattern[11] = 0x89 
DataBytePattern[12] = 0x34
DataBytePattern[13] = 0xcd 
DataBytePattern[14] = 0x78
DataBytePattern[15] = 0x89 
DataBytePattern[16] = 0x34
DataBytePattern[17] = 0xcd 
DataBytePattern[18] = 0x78
DataBytePattern[19] = 0x89 
DataBytePattern[20] = 0x34
DataBytePattern[21] = 0xcd
					
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 3.3: Send Output Message 3 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x56
DataBytePattern[1] = 0xab 
DataBytePattern[2] = 0x12
DataBytePattern[3] = 0xef 
DataBytePattern[4] = 0x56
DataBytePattern[5] = 0xab 
DataBytePattern[6] = 0x12
DataBytePattern[7] = 0xef 
DataBytePattern[8] = 0x56
DataBytePattern[9] = 0xab 
DataBytePattern[10] = 0x12
DataBytePattern[11] = 0xef 
DataBytePattern[12] = 0x56
DataBytePattern[13] = 0xab 
DataBytePattern[14] = 0x12
DataBytePattern[15] = 0xef 
DataBytePattern[16] = 0x56
DataBytePattern[17] = 0xab 
DataBytePattern[18] = 0x12
DataBytePattern[19] = 0xef 
DataBytePattern[20] = 0x56
DataBytePattern[21] = 0xab
							
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket3, DataBytePattern, Pkt3, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 3.4: Send Output Message 4 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x78
DataBytePattern[1] = 0x89 
DataBytePattern[2] = 0x34
DataBytePattern[3] = 0xcd 
DataBytePattern[4] = 0x78
DataBytePattern[5] = 0x89 
DataBytePattern[6] = 0x34
DataBytePattern[7] = 0xcd 
DataBytePattern[8] = 0x78
DataBytePattern[9] = 0x89 
DataBytePattern[10] = 0x34
DataBytePattern[11] = 0xcd 
DataBytePattern[12] = 0x78
DataBytePattern[13] = 0x89 
DataBytePattern[14] = 0x34
DataBytePattern[15] = 0xcd 
DataBytePattern[16] = 0x78
DataBytePattern[17] = 0x89 
DataBytePattern[18] = 0x34
DataBytePattern[19] = 0xcd 
DataBytePattern[20] = 0x78
DataBytePattern[21] = 0x89					

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 3.5: Send Output Message 5 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x12
DataBytePattern[1] = 0xef 
DataBytePattern[2] = 0x56
DataBytePattern[3] = 0xab 
DataBytePattern[4] = 0x12
DataBytePattern[5] = 0xef 
DataBytePattern[6] = 0x56
DataBytePattern[7] = 0xab 
DataBytePattern[8] = 0x12
DataBytePattern[9] = 0xef 
DataBytePattern[10] = 0x56
DataBytePattern[11] = 0xab 
DataBytePattern[12] = 0x12
DataBytePattern[13] = 0xef 
DataBytePattern[14] = 0x56
DataBytePattern[15] = 0xab 
DataBytePattern[16] = 0x12
DataBytePattern[17] = 0xef 
DataBytePattern[18] = 0x56
DataBytePattern[19] = 0xab
			
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket5, DataBytePattern, Pkt5, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 3.6: Send Output Message 6 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x34
DataBytePattern[1] = 0xcd 
DataBytePattern[2] = 0x78
DataBytePattern[3] = 0x89 
DataBytePattern[4] = 0x34
DataBytePattern[5] = 0xcd 
DataBytePattern[6] = 0x78
DataBytePattern[7] = 0x89 
DataBytePattern[8] = 0x34
DataBytePattern[9] = 0xcd 
DataBytePattern[10] = 0x78
DataBytePattern[11] = 0x89 
DataBytePattern[12] = 0x34
DataBytePattern[13] = 0xcd 
DataBytePattern[14] = 0x78
DataBytePattern[15] = 0x89 
DataBytePattern[16] = 0x34
DataBytePattern[17] = 0xcd 
DataBytePattern[18] = 0x78
DataBytePattern[19] = 0x89 
							
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket6, DataBytePattern, Pkt6, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 4.0: Send all the data."
write ";*********************************************************************"
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket1 DataSize=32 DataPattern=0x11112222
wait 2
/$SC_$CPU_TST_HK_SENDINMSG MsgId=InputPacket2 DataSize=32 DataPattern=0xaaaabbbb
wait 2

write ";*********************************************************************"
write ";  Step 4.1: Send Output Message 1 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa 
DataBytePattern[20] = 0x11
DataBytePattern[21] = 0xbb
				
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo			

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 4.2: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa 
DataBytePattern[20] = 0x11
DataBytePattern[21] = 0xbb
					
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 4.3: Send Output Message 3 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x22
DataBytePattern[1] = 0xaa 
DataBytePattern[2] = 0x11
DataBytePattern[3] = 0xbb 
DataBytePattern[4] = 0x22
DataBytePattern[5] = 0xaa 
DataBytePattern[6] = 0x11
DataBytePattern[7] = 0xbb 
DataBytePattern[8] = 0x22
DataBytePattern[9] = 0xaa 
DataBytePattern[10] = 0x11
DataBytePattern[11] = 0xbb 
DataBytePattern[12] = 0x22
DataBytePattern[13] = 0xaa 
DataBytePattern[14] = 0x11
DataBytePattern[15] = 0xbb 
DataBytePattern[16] = 0x22
DataBytePattern[17] = 0xaa 
DataBytePattern[18] = 0x11
DataBytePattern[19] = 0xbb 
DataBytePattern[20] = 0x22
DataBytePattern[21] = 0xaa

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket3, DataBytePattern, Pkt3, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 4.4: Send Output Message 4 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x22
DataBytePattern[1] = 0xaa 
DataBytePattern[2] = 0x11
DataBytePattern[3] = 0xbb 
DataBytePattern[4] = 0x22
DataBytePattern[5] = 0xaa 
DataBytePattern[6] = 0x11
DataBytePattern[7] = 0xbb 
DataBytePattern[8] = 0x22
DataBytePattern[9] = 0xaa 
DataBytePattern[10] = 0x11
DataBytePattern[11] = 0xbb 
DataBytePattern[12] = 0x22
DataBytePattern[13] = 0xaa 
DataBytePattern[14] = 0x11
DataBytePattern[15] = 0xbb 
DataBytePattern[16] = 0x22
DataBytePattern[17] = 0xaa 
DataBytePattern[18] = 0x11
DataBytePattern[19] = 0xbb 
DataBytePattern[20] = 0x22
DataBytePattern[21] = 0xaa

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 4.5: Send Output Message 5 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa 
			
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket5, DataBytePattern, Pkt5, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 4.6: Send Output Message 6 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa
						
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket6, DataBytePattern, Pkt6, NoChecks, 0)

write ";*********************************************************************"
write ";  Step 5.0: was supposed to be turning off 1 out of 127 but can't"
write ";            test due to message id limitation. Step 3.0 turns off"
write ";            1 message (1/2 of the copytable definitions"
write ";*********************************************************************"

write ";*********************************************************************"
write ";  Step 6.0: Send no input messages, just request output messages"
write ";*********************************************************************"
write ";  Step 6.1: Send Output Message 1 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa 
DataBytePattern[20] = 0x11
DataBytePattern[21] = 0xbb
				
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo			

s $SC_$CPU_hk_sendoutmsg(OutputPacket1, DataBytePattern, Pkt1, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 6.2: Send Output Message 2 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa 
DataBytePattern[20] = 0x11
DataBytePattern[21] = 0xbb
					
for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket2, DataBytePattern, Pkt2, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 6.3: Send Output Message 3 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x22
DataBytePattern[1] = 0xaa 
DataBytePattern[2] = 0x11
DataBytePattern[3] = 0xbb 
DataBytePattern[4] = 0x22
DataBytePattern[5] = 0xaa 
DataBytePattern[6] = 0x11
DataBytePattern[7] = 0xbb 
DataBytePattern[8] = 0x22
DataBytePattern[9] = 0xaa 
DataBytePattern[10] = 0x11
DataBytePattern[11] = 0xbb 
DataBytePattern[12] = 0x22
DataBytePattern[13] = 0xaa 
DataBytePattern[14] = 0x11
DataBytePattern[15] = 0xbb 
DataBytePattern[16] = 0x22
DataBytePattern[17] = 0xaa 
DataBytePattern[18] = 0x11
DataBytePattern[19] = 0xbb 
DataBytePattern[20] = 0x22
DataBytePattern[21] = 0xaa

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket3, DataBytePattern, Pkt3, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 6.4: Send Output Message 4 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x22
DataBytePattern[1] = 0xaa 
DataBytePattern[2] = 0x11
DataBytePattern[3] = 0xbb 
DataBytePattern[4] = 0x22
DataBytePattern[5] = 0xaa 
DataBytePattern[6] = 0x11
DataBytePattern[7] = 0xbb 
DataBytePattern[8] = 0x22
DataBytePattern[9] = 0xaa 
DataBytePattern[10] = 0x11
DataBytePattern[11] = 0xbb 
DataBytePattern[12] = 0x22
DataBytePattern[13] = 0xaa 
DataBytePattern[14] = 0x11
DataBytePattern[15] = 0xbb 
DataBytePattern[16] = 0x22
DataBytePattern[17] = 0xaa 
DataBytePattern[18] = 0x11
DataBytePattern[19] = 0xbb 
DataBytePattern[20] = 0x22
DataBytePattern[21] = 0xaa

for entry = 22 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket4, DataBytePattern, Pkt4, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 6.5: Send Output Message 5 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa 
			
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket5, DataBytePattern, Pkt5, MissingYes, 0)

write ";*********************************************************************"
write ";  Step 6.6: Send Output Message 6 command and check data"
write ";*********************************************************************"
;;setup data expected packet byte data pattern
DataBytePattern[0] = 0x11
DataBytePattern[1] = 0xbb 
DataBytePattern[2] = 0x22
DataBytePattern[3] = 0xaa 
DataBytePattern[4] = 0x11
DataBytePattern[5] = 0xbb 
DataBytePattern[6] = 0x22
DataBytePattern[7] = 0xaa 
DataBytePattern[8] = 0x11
DataBytePattern[9] = 0xbb 
DataBytePattern[10] = 0x22
DataBytePattern[11] = 0xaa 
DataBytePattern[12] = 0x11
DataBytePattern[13] = 0xbb 
DataBytePattern[14] = 0x22
DataBytePattern[15] = 0xaa 
DataBytePattern[16] = 0x11
DataBytePattern[17] = 0xbb 
DataBytePattern[18] = 0x22
DataBytePattern[19] = 0xaa
						
for entry = 20 to 80 do
   DataBytePattern[entry] = 0	
enddo

s $SC_$CPU_hk_sendoutmsg(OutputPacket6, DataBytePattern, Pkt6, MissingYes, 0)

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
write ";  End procedure $SC_$CPU_hk_stressmissingdata                      "
write ";*********************************************************************"
ENDPROC
