PROC $sc_$cpu_md_deftables
;*******************************************************************************
;  Test Name:  MD_InitReset
;  Test Level: Build Verification 
;  Test Type:  Functional
;            
;  Test Description
;   The purpose of this test is to setup and load the default set of dwell
;   tables used by some of the tests
;   
;
;  Requirements Tested
;	None
;
;  Prerequisite Conditions
;    The CFS is up and running and ready to accept commands. The MD
;    commands and TLM items exist in the GSE database. The display page
;    for the MD Housekeeping exists. An MD test application exists.
;    The MD and TST_MD applications are running.
;
;  Assumptions and Constraints
;   None
;
;  Change History
;
;   Date        Name	    Description
;   08/13/08	S. Jonke    Original Procedure
;   09/30/08	S. Jonke    Removed some unneccessary code and added comments
;   12/07/09	W. Moleski  Turned logging off around code that did not provide
;			    any significant benefit of logging
;   04/28/11	W. Moleski  Added variables for App and table names
;
;  Arguments
;   None
;
;  Procedures Called
;   None 
; 
;  Required Post-Test Analysis
;   None
;
;  Notes
;
;
;   Table 1
;     Entry#  Length  Delay  Offset
;     1       1       1      testdata_addr+0
;     2       1       0      testdata_addr+1
;     3       0       0      0
;
;     Entries = 2;  Size = 2;  Total Delay = 1
;
; 
;   Table 2
;     Entry#  Length  Delay  Offset
;     1       2       1      testdata_addr+2
;     2       2       0      testdata_addr+4
;     3       0       0      0
; 
;     Entries = 2;  Size = 4;  Total Delay = 1
; 
; 
;   Table 3
;     Entry#  Length  Delay  Offset
;     1       2       1      testdata_addr+6
;     2       4       0      testdata_addr+8
;     3       0       0      0
;
;     Entries = 2;  Size = 6;  Total Delay = 1
; 
; 
;   Table 4
;     Entry#  Length  Delay  Offset
;     1       1       1      testdata_addr+12
;     2       1       0      testdata_addr+13
;     3       0       0      0
;
;     Entries = 2;  Size = 2;  Total Delay = 1
;
;**********************************************************************

;; Turn off logging for the includes
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "tst_md_events.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "md_platform_cfg.h"
#include "md_events.h"
#include "cfe_tbl_events.h"

%liv (log_procedure) = logging

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

; LOCAL Variables
local cmdcnt, errcnt
local rawcmd
local stream1, dwell1, dwell2, dwell3, dwell4
local passed
LOCAL dwell_tbl1_load_pkt, dwell_tbl2_load_pkt, dwell_tbl3_load_pkt, dwell_tbl4_load_pkt
LOCAL dwell_tbl1_load_appid, dwell_tbl2_load_appid, dwell_tbl3_load_appid, dwell_tbl4_load_appid
local dwl_tbl_1_index, dwl_tbl_2_index, dwl_tbl_3_index, dwl_tbl_4_index
local testdata_addr, addr

local MDAppName = "MD"
local ramDir = "RAM:0"
local MDTblName1 = MDAppName & ".DWELL_TABLE1"
local MDTblName2 = MDAppName & ".DWELL_TABLE2"
local MDTblName3 = MDAppName & ".DWELL_TABLE3"
local MDTblName4 = MDAppName & ".DWELL_TABLE4"

;; CPU1 is the default
dwell_tbl1_load_pkt = "0FA8"
dwell_tbl2_load_pkt = "0FA9"
dwell_tbl3_load_pkt = "0FAA"
dwell_tbl4_load_pkt = "0FAB"
dwell_tbl1_load_appid = 4008
dwell_tbl2_load_appid = 4009
dwell_tbl3_load_appid = 4010
dwell_tbl4_load_appid = 4011

if ("$CPU" = "CPU2") then
  dwell_tbl1_load_pkt = "0FC6"
  dwell_tbl2_load_pkt = "0FC7"
  dwell_tbl3_load_pkt = "0FC8"
  dwell_tbl4_load_pkt = "0FC9"
  dwell_tbl1_load_appid = 4038
  dwell_tbl2_load_appid = 4039
  dwell_tbl3_load_appid = 4040
  dwell_tbl4_load_appid = 4041
elseif ("$CPU" = "CPU3") then
  dwell_tbl1_load_pkt = "0FE6"
  dwell_tbl2_load_pkt = "0FE7"
  dwell_tbl3_load_pkt = "0FE8"
  dwell_tbl4_load_pkt = "0FE9"
  dwell_tbl1_load_appid = 4070
  dwell_tbl2_load_appid = 4071
  dwell_tbl3_load_appid = 4072
  dwell_tbl4_load_appid = 4073
endif

write ";*********************************************************************"
write ";  Start procedure $SC_$CPU_md_DefTables                            "
write ";*********************************************************************"
; get the address of the TST_MD test data area
testdata_addr = $SC_$CPU_TST_MD_TSTDATAADR

write ";*********************************************************************"
write ";  Setup initial dwell table 1"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       1       1      testdata_addr+0
;   2       1       0      testdata_addr+1
;   3       0       0      0
;
;  Entries = 2;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Signature="Table #1 default signature"

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+1
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #1 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl1_load_appid,"Dwell Table #1 Load","md_dwl_ld_sg_tbl1",MDTblName1,"$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Enabled","$SC_$CPU_MD_Dwell_Load_Tbl1.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl1", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #1."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName1"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #1 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #1 Validate command."
  endif
else
  write "<!> Failed - InActive Table #1 validation."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1

  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #1 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #1 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #1 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #1. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName1"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #1 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #1 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Setup initial dwell table 2"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+2
;   2       2       0      testdata_addr+4
;   3       0       0      0
;
;  Entries = 2;  Size = 4;  Total Delay = 1

;;; Set up dwell load table 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Signature="Table #2 default signature"

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+4
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #2 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl2_load_appid,"Dwell Table #2 Load", "md_dwl_ld_sg_tbl2",MDTblName2,"$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Enabled","$SC_$CPU_MD_Dwell_Load_Tbl2.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl2", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #2."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG",1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName2"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #2 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #2 Validate command."
  endif
else
  write "<!> Failed - InActive Table #2 validation."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO",1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #2 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #2 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #2. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName2"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #2 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #2 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Setup initial dwell table 3"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       2       1      testdata_addr+6
;   2       4       0      testdata_addr+8
;   3       0       0      0
;
;  Entries = 2;  Size = 6;  Total Delay = 1

;;; Set up dwell load table 3
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Signature="Table #3 default signature"

$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Length = 2
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+6
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Length = 4
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+8
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #3 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO,1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl3_load_appid,"Dwell Table #3 Load", "md_dwl_ld_sg_tbl3",MDTblName3,"$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Enabled","$SC_$CPU_MD_Dwell_Load_Tbl3.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl3", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #3."
write ";**********************************************************************"
ut_setupevents "$SC","$CPU","CFE_TBL",CFE_TBL_VAL_REQ_MADE_INF_EID,"DEBUG", 1

ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName3"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #3 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #3 Validate command."
  endif
else
  write "<!> Failed - InActive Table #3 validation."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1
                                                                                
  wait 5
                                                                                
  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #3 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #3 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #3 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #3. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName3"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #3 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #3 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Setup initial dwell table 4"
write ";*********************************************************************"
; Set up the table data
; Table summary
;   Entry#  Length  Delay  Offset
;   1       1       1      testdata_addr+12
;   2       1       0      testdata_addr+13
;   3       0       0      0
;
;  Entries = 2;  Size = 2;  Total Delay = 1

;;; Set up dwell load table 4
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled=0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Signature="Table #4 default signature"

$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Delay = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_Offset = testdata_addr+12
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[1].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Length = 1
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_Offset = testdata_addr+13
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[2].MD_TLE_SymName = ""

$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Length = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Delay = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_Offset = 0
$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3].MD_TLE_SymName = ""

; Create a load file for dwell table #4 with this data (including signature)
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_FILE_LOADED_INF_EID, INFO, 1

s create_tbl_file_from_cvt ("$CPU",dwell_tbl4_load_appid,"Dwell Table #4 Load", "md_dwl_ld_sg_tbl4",MDTblName4,"$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Enabled","$SC_$CPU_MD_Dwell_Load_Tbl4.MD_DTL_Entry[3]")

cmdcnt = $SC_$CPU_TBL_CMDPC+1

start load_table ("md_dwl_ld_sg_tbl4", "$CPU")

ut_tlmwait $SC_$CPU_TBL_CMDPC, {cmdcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Table load command sent."
else
  write "<!> Failed - Table load command."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", CFE_TBL_FILE_LOADED_INF_EID, "."
endif

wait 5

write ";**********************************************************************"
write "; Validate the inactive buffer for Dwell Table #4."
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VAL_REQ_MADE_INF_EID, "DEBUG", 1
                                                                                
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=MDTblName4"

if (UT_SC_Status = UT_SC_Success) then
  write "<*> Passed - Inactive Table #4 validate command sent."
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event Msg ",$SC_$CPU_find_event[1].eventid," Found!"
  else
    write "<!> Failed - Event Message not received for Table #4 Validate command."
  endif
else
  write "<!> Failed - InActive Table #4 validation."
endif

; Look for the Validation success event
if ($SC_$CPU_EVS_EVENTID <> CFE_TBL_VALIDATION_INF_EID) then
  ut_setupevents "$SC", "$CPU", "CFE_TBL", CFE_TBL_VALIDATION_INF_EID, "INFO", 1

  wait 5

  if ($SC_$CPU_find_event[1].num_found_messages != 1) then
    write "<!> Failed - Inactive Dwell Table #4 validation. Event Message not received."
  else
    write "<*> Passed - Inactive Dwell Table #4 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
  endif
else
  write "<*> Passed - Inactive Dwell Table #4 validated. Event Msg ",$SC_$CPU_evs_eventid," Found!"
endif
                                                                                
wait 5

write ";**********************************************************************"
write "; Activate the load for Dwell Table #4. "
write ";**********************************************************************"
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_LOAD_PEND_REQ_INF_EID, DEBUG, 1
ut_setupevents $SC, $CPU, CFE_TBL, CFE_TBL_UPDATE_SUCCESS_INF_EID, INFO, 2

ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATableName=MDTblName4"

if (ut_sc_status <> UT_SC_Success) then
  write "<!> Failed - Activate command not sent properly."
endif

; Look for the Activation success event
wait 5

if ($SC_$CPU_find_event[2].num_found_messages = 1) then
  write "<*> Passed - Dwell Table #4 Activated. Event Msg ",$SC_$CPU_find_event[2].eventid," Found!"
else
  write "<!> Failed - Dwell Table #4 activation. Event Message not received."
endif

if ($SC_$CPU_find_event[1].num_found_messages != 1) then
  write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",CFE_TBL_LOAD_PEND_REQ_INF_EID, "."
endif

wait 10
;;;; Dump the Table Registry
s get_file_to_cvt (ramDir,"cfe_tbl_reg.log","$sc_$cpu_tbl_reg.log","$CPU")
wait 10

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_MD_DefTables                            "
write ";*********************************************************************"
ENDPROC
