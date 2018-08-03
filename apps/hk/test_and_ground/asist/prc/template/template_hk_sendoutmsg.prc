PROC $sc_$cpu_hk_sendoutmsg(OutputPktID, DataBytePattern[], PktNo, Checking, StaleData)
;*******************************************************************************
;  Test Name:  hk_sendoutmsg
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;       The procedure is used by all tests that need to receive and validate
;       HK Combined Output Packets.  It sends a request to TST_HK to send 
;       a specific output packet and the verifies that the data is as expected.
;
;  Change History
;       
;       Date               Name         Description
;       06/02/08        Barbie Medina   Original Procedure.
;       08/29/12        Walt Moleski	Updated to handle Discard of Packet if
;					data is missing and config param is set
;*******************************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "hk_platform_cfg.h"
#include "hk_events.h"
#include "cfs_hk_requirements.h"

%liv (log_procedure) = logging

LOCAL count
LOCAL currentCnt
LOCAL missing
LOCAL entry
LOCAL berror
LOCAL Data[0 .. 80]

local HKAppName = "HK"

wait 5
currentCnt = $SC_$CPU_HK_CMBPKTSSENT
count =  $SC_$CPU_HK_CMBPKTSSENT+1
missing = $SC_$CPU_HK_MISSDATACTR+1

;; The event is the same no matter what Checking is set to
;; Commenting out this if
;;if (Checking = MissingYes) then
;;  ut_setupevents "$SC","$CPU",{HKAppName},HK_OUTPKT_MISSING_DATA_EID,"DEBUG", 1
;;elseif (Checking = TstPktLen) or (Checking = TstLenTstTbl) then
;;  ut_setupevents "$SC","$CPU",{HKAppName},HK_OUTPKT_MISSING_DATA_EID,"DEBUG", 1
;;endif

ut_setupevents "$SC","$CPU",{HKAppName},HK_OUTPKT_MISSING_DATA_EID,"DEBUG", 1

;;send output packet 1 command to test app
/$SC_$CPU_TST_HK_SENDOUTMSG MsgId=OutputPktID Pad=0

;;wait for output packet to update
wait 10

if ((Checking = MissingYes) AND (HK_DISCARD_INCOMPLETE_COMBO = 1)) then
   ;; Need to verify that the Packet Sent count did not increment
   if (currentCnt = $SC_$CPU_HK_CMBPKTSSENT) then
      write "<*> Passed (2001.6) - Combined Packet Sent Counter did not increment"
      ut_setrequirements HK_20016, "P"
   else
      write "<!> Failed (2001.6) - Combined Packet Sent Counter incremented when not expected."
      ut_setrequirements HK_20016, "F"
   endif
   goto skipData
endif

for entry=1 to 80 do 
    if (PktNo = 1) then
      Data[entry-1] = $sc_$cpu_HK_Tab1Pkt1Data[entry] 
    elseif (PktNo = 2) then
      Data[entry-1] = $sc_$cpu_HK_Tab1Pkt2Data[entry] 
    elseif (PktNo = 3) then
      Data[entry-1] = $sc_$cpu_HK_Tab1Pkt3Data[entry] 
    elseif (PktNo = 4) then
      Data[entry-1] = $sc_$cpu_HK_Tab1Pkt4Data[entry] 
    elseif (PktNo = 5) then
      Data[entry-1] = $sc_$cpu_HK_Tab1Pkt5Data[entry] 
    elseif (PktNo = 6) then
      Data[entry-1] = $sc_$cpu_HK_Tab1Pkt6Data[entry] 
    else
      write "<!> Output Packet specified does not exist"
    endif
enddo

;;check data for correctness
entry = 1
berror=0
while (entry < 81) do
  if (Data[entry] = DataBytePattern[entry]) then
    entry = entry + 1
  else
    write "<!> Failed (2000;2001) - Output packet received but data not as expected"
    write "Byte............",entry
    write "Received........", %hex(Data[entry],2)
    write "Expected........", %hex(DataBytePattern[entry],2)
    ut_setrequirements HK_2000, "F"
    ut_setrequirements HK_2001, "F"
    if (Checking = TblUpdate) or (Checking = TstLenTstTbl) then
      ut_setrequirements HK_20011, "F"
    endif
    berror = entry
    entry = 81
  endif
enddo

if (berror = 0) then
  write "<*> Passed (2000;2001) - Output packet received, data as expected"
  ut_setrequirements HK_2000, "P"
  ut_setrequirements HK_2001, "P"
  if (Checking = TblUpdate) or (Checking = TstLenTstTbl) then
    ut_setrequirements HK_20011, "P"
  elseif (Checking = MissingYes) then
    ut_setrequirements HK_20012, "P"
  endif
elseif (Checking = MissingYes) then
  if (berror = StaleData) then
    write "<!> Failed (2001.2) Stale data not as expected"
    ut_setrequirements HK_20012, "F"
  else
    write "Byte error is not associated with stale data"
  endif
endif

wait 5

;; Check that the Combined Packet Sent counter incremented
if (count = $sc_$cpu_HK_CMBPKTSSENT) then
    write "<*> Passed (3000) - Combined Packet Sent Counter Updated as expected"
    ut_setrequirements HK_3000, "P"
else
    write "<*> Failed (3000) HK sent a combined packet but did not increment sent counter"
    ut_setrequirements HK_3000, "F"
endif

skipData:
if (Checking = MissingYes) then
  wait 5
  if (missing =  $sc_$cpu_HK_MISSDATACTR) then
    write "<*> Passed (2001.3) - Missing Data Counter Updated as expected"
    ut_setrequirements HK_20013, "P"
  else
    write "<*> Failed (2001.3) HK did not increment missing data counter"
    ut_setrequirements HK_20013, "F"
  endif

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (2001.3) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements HK_20013, "P"
  else
    write "<!> Failed (2001.3) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_OUTPKT_MISSING_DATA_EID, "."
    ut_setrequirements HK_20013, "F"
  endif
elseif (Checking = TstPktLen) or (Checking = TstLenTstTbl) then
  wait 5
  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (2001.3) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements HK_20013, "P"
  else
    write "<!> Failed (2001.3) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",HK_OUTPKT_MISSING_DATA_EID, "."
    ut_setrequirements HK_20013, "F"
  endif
endif

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_hk_sendoutmsg                        "
write ";*********************************************************************"
ENDPROC

