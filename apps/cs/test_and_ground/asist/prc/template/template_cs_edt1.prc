PROC $sc_$cpu_cs_edt1
;*******************************************************************************
;  Test Name:  cs_edt1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the default EEPROM 
;	Definition Table for the Checksum Application.
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
;                                       proper host IP address.
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
local numEntries = 0
local quarterSize = 0
local halfSize = 0
local tblIndex = 0

;; Parse the memory table to find the EEPROM entries and add them to the table
for i=1 to MEM_TABLE_SIZE do
  if (p@$SC_$CPU_TST_CS_MemType[i] = "EEPROM") then
    numEntries = numEntries + 1

    ;; Setup 4 table entries for each EEPROM region
    if ((tblIndex + 4) < (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1)) then
      quarterSize = $SC_$CPU_TST_CS_Size[i] / 4
      halfSize = $SC_$CPU_TST_CS_Size[i] / 2

      ;; Entry 1 of 2048 or quartersize
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].StartAddr = $SC_$CPU_TST_CS_StartAddr[i]
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].State = CS_STATE_ENABLED
      if (quarterSize > 2048) then
        $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].NumBytes = 2048
      else
        $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].NumBytes = quarterSize
      endif
      tblIndex = tblIndex + 1

      ;; Entry 2 of halfSize
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].StartAddr = $SC_$CPU_TST_CS_StartAddr[i]+quarterSize
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].NumBytes = halfSize
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].State = CS_STATE_DISABLED
      tblIndex = tblIndex + 1

      ;; Entry 3 of 2048 or quarterSize
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].StartAddr = $SC_$CPU_TST_CS_StartAddr[i]+halfSize
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].State = CS_STATE_ENABLED
      if (quarterSize > 2048) then
        $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].NumBytes = 2048
      else
        $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].NumBytes = quarterSize
      endif
      tblIndex = tblIndex + 1

      ;; Entry 4 of full size
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].StartAddr = $SC_$CPU_TST_CS_StartAddr[i]
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].NumBytes = $SC_$CPU_TST_CS_Size[i]
      $SC_$CPU_CS_EEPROM_DEF_TABLE[tblIndex].State = CS_STATE_DISABLED
      tblIndex = tblIndex + 1
    endif
  endif
enddo

write "==> Number of EEPROM entries in OS Memory Table = ",numEntries
write "==> Number of EEPROM entries created in Def Table = ",tblIndex - 1

local maxEntry = CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1

;; Clear out the rest of the table
for i = tblIndex to maxEntry do
  $SC_$CPU_CS_EEPROM_DEF_TABLE[i].NumBytes = 0
  $SC_$CPU_CS_EEPROM_DEF_TABLE[i].State = CS_STATE_EMPTY
  $SC_$CPU_CS_EEPROM_DEF_TABLE[i].StartAddr = 0
enddo

local endmnemonic = "$SC_$CPU_CS_EEPROM_DEF_TABLE[" & maxEntry & "].NumBytes"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defTblId,"EEPROM Memory Definition Table Load 1","eeprom_def_ld_1",eeDefTblName,"$SC_$CPU_CS_EEPROM_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_cs_edt1                              "
write ";*********************************************************************"
ENDPROC
