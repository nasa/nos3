PROC $sc_$cpu_cs_edt2
;*******************************************************************************
;  Test Name:  cs_edt2
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate an EEPROM Definition Table
;	for the Checksum Application that contains several valid entries, an
;	entry with an invalid address, an entry that contains an invalid range
;	and an entry with an invalid state
;
;  Requirements Tested:
;	None
;
;  Prerequisite Conditions
;	The TST_CS_MemTbl application must be executing for this procedure to
;	generate the appropriate EEPROM Definition Table
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	07/18/11	Walt Moleski	Initial release.
;       09/19/12        Walt Moleski    Added write of new HK items and added a
;                                       define of the OS_MEM_TABLE_SIZE that
;                                       was removed from osconfig.h in 3.5.0.0
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address. Changed define
;					of OS_MEM_TABLE_SIZE to MEM_TABLE_SIZE.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;      create_tbl_file_from_cvt Procedure that creates a load file from
;                               the specified arguments and cvt
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "osconfig.h"
#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_tbldefs.h"

%liv (log_procedure) = logging

#define MEM_TABLE_SIZE       10

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL defTblId, defPktId
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "$CPU"
local eeDefTblName = CSAppName & "." & CS_DEF_EEPROM_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAC"
defPktId = 4012

write ";*********************************************************************"
write ";  Define the Application Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED
local eepromEntry = 0
local quarterSize = 0
local halfSize = 0
local tblIndex = 0

;; Parse the memory table to find the EEPROM entries and add them to the table
for i=1 to MEM_TABLE_SIZE do
  if (p@$SC_$CPU_TST_CS_MemType[i] = "EEPROM") then
    eepromEntry = i
    break
  endif
enddo

quarterSize = $SC_$CPU_TST_CS_Size[eepromEntry] / 4
halfSize = $SC_$CPU_TST_CS_Size[eepromEntry] / 2
local ovrlapAddr = $SC_$CPU_TST_CS_StartAddr[eepromEntry] + $SC_$CPU_TST_CS_Size[eepromEntry]
ovrlapAddr = ovrlapAddr - 1000

;; States are 0=Empty; 1=Enabled; 2=Disabled; 3=Undefined
$SC_$CPU_CS_EEPROM_DEF_TABLE[0].State = 2
$SC_$CPU_CS_EEPROM_DEF_TABLE[0].StartAddr = $SC_$CPU_TST_CS_StartAddr[eepromEntry]
$SC_$CPU_CS_EEPROM_DEF_TABLE[0].NumBytes = $SC_$CPU_TST_CS_Size[eepromEntry]
$SC_$CPU_CS_EEPROM_DEF_TABLE[1].State = 1
$SC_$CPU_CS_EEPROM_DEF_TABLE[1].StartAddr = $SC_$CPU_TST_CS_StartAddr[eepromEntry]
$SC_$CPU_CS_EEPROM_DEF_TABLE[1].NumBytes = quarterSize
$SC_$CPU_CS_EEPROM_DEF_TABLE[2].State = 1
$SC_$CPU_CS_EEPROM_DEF_TABLE[2].StartAddr = ovrlapAddr
$SC_$CPU_CS_EEPROM_DEF_TABLE[2].NumBytes = 2048
$SC_$CPU_CS_EEPROM_DEF_TABLE[3].State = 1
$SC_$CPU_CS_EEPROM_DEF_TABLE[3].StartAddr = $SC_$CPU_TST_CS_StartAddr[eepromEntry]+16
$SC_$CPU_CS_EEPROM_DEF_TABLE[3].NumBytes = halfSize
$SC_$CPU_CS_EEPROM_DEF_TABLE[4].State = 7
$SC_$CPU_CS_EEPROM_DEF_TABLE[4].StartAddr = $SC_$CPU_TST_CS_StartAddr[eepromEntry]+16

local maxEntry = CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1

;; Clear out the rest of the table
for i = 5 to maxEntry do
  $SC_$CPU_CS_EEPROM_DEF_TABLE[i].NumBytes = 0
  $SC_$CPU_CS_EEPROM_DEF_TABLE[i].State = CS_STATE_EMPTY
  $SC_$CPU_CS_EEPROM_DEF_TABLE[i].StartAddr = 0
enddo

local endmnemonic = "$SC_$CPU_CS_EEPROM_DEF_TABLE[" & maxEntry & "].NumBytes"

;; Create the Table Load file that should fail validation for Range Error
s create_tbl_file_from_cvt (hostCPU,defTblId,"EEPROM Definition Table Invalid Load","eeprom_def_invalid",eeDefTblName,"$SC_$CPU_CS_EEPROM_DEF_TABLE[0].State",endmnemonic)

;; Fix the Range Error
$SC_$CPU_CS_EEPROM_DEF_TABLE[2].StartAddr = $SC_$CPU_TST_CS_StartAddr[eepromEntry]+32
;; Create the Table Load file that should fail validation State Error
s create_tbl_file_from_cvt (hostCPU,defTblId,"EEPROM Definition Table Invalid Load 2","eeprom_def_invalid2",eeDefTblName,"$SC_$CPU_CS_EEPROM_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_edt2                              "
write ";*********************************************************************"
ENDPROC
