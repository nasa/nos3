proc fill_in_spacecraft_status
;
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE
;
; ======================================================================
; Purpose: The purpose of this procedure is to set up and display 
;          checksum, cFE version, and OS version information in
;          the Spacecraft status block on the ASIST command window.
;
; History:
;
; 03JUN05                 Initial development of this proc.      SMA
; 06JUN05                 Add support to only display values     SMA
;                         for active cpu.
; 09JUN05                 Commented out support to only display  SMA
;                         values for commanmding cpu.
; 10OCT05                 Restored support to only display       SMA
;                         values for active cpu.
; 04NOV05                 Add support for cFE Minor Version      SMA
;                         Number.
; 21OCT09                 Added include statement and modified   WFM
;			  code to generically set values based
;			  upon definitions in the include file
; 14FEB12                 Added new version items and settings   WFM
;

#include "cfe_utils.h"

global to_cmd_sent

; Set up Labels, and initialize values for CPU1
if (numCPUs > 1) then
  gsescstatlbl11 = "CKSUM C1"
  gsescstatlbl21 = "cFEV  C1"
  gsescstatlbl31 = "OSVer C1"
else
  gsescstatlbl11 = "CKSUM"
  gsescstatlbl21 = "cFEV"
  gsescstatlbl31 = "OSVer"
endif

gsescstatval11 = 0
gsescstatval21 = "0.0.0.0"
gsescstatval31 = "0.0"

if (numCPUs >= 2) then
  gsescstatlbl12 = "CKSUM C2"
  gsescstatlbl22 = "cFEV  C2"
  gsescstatlbl32 = "OSVer C2"
  gsescstatval12 = 0
  gsescstatval22 = "0.0.0.0"
  gsescstatval32 = "0.0"
endif

if (numCPUs = 3) then
  gsescstatlbl13 = "CKSUM C3"
  gsescstatlbl23 = "cFEV  C3"
  gsescstatlbl33 = "OSVer C3"
  gsescstatval13 = 0
  gsescstatval23 = "0.0.0.0"
  gsescstatval33 = "0.0"
endif

gsescstatlbl14 = "Cmd Ctrlr"

; Build the database prefix using SC and CPU definitions from cfe_utils.h
local db_prefix, cpu_prefix

; If SC is set, use it
if (SC <> "") then
  db_prefix = SC
endif

;; Add the CPU definition
db_prefix = db_prefix & CPU_CFG

; setup the database mnemonics for CPU1
local cksumItem
local cfeMajorVerItem
local cfeMinorVerItem
local cfeRevisionItem
local cfeMissionRevItem
local osMajorVerItem
local osMinorVerItem
local osRevisionItem
local osMissionRevItem

; Display Values
if (to_cmd_sent = "1") then
  ; setup the database mnemonics for CPU1
  if (numCPUs > 1) then
    cpu_prefix = db_prefix & "1"
  else
    cpu_prefix = db_prefix
  endif

  cksumItem = cpu_prefix & "_ES_CKSUM"
  cfeMajorVerItem = cpu_prefix & "_ES_CFEMAJORVER"
  cfeMinorVerItem = cpu_prefix & "_ES_CFEMINORVER"
  cfeRevisionItem = cpu_prefix & "_ES_CFEREVISION"
  cfeMissionRevItem = cpu_prefix & "_ES_CFEMSNREV"
  osMajorVerItem = cpu_prefix & "_ES_OSMAJORVER"
  osMinorVerItem = cpu_prefix & "_ES_OSMINORVER"
  osRevisionItem = cpu_prefix & "_ES_OSREVISION"
  osMissionRevItem = cpu_prefix & "_ES_OSMISSIONREV"

  ; Set the values
  gsescstatval11 = {cksumItem}
  gsescstatval21 = {cfeMajorVerItem}  & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  gsescstatval31 = {osMajorVerItem} & "." & {osMinorVerItem} & "." & {osRevisionItem} & "." & {osMissionRevItem}

elseif (to_cmd_sent = "2") then
  ; setup the database mnemonics for CPU2
  cpu_prefix = db_prefix & "2"

  cksumItem = cpu_prefix & "_ES_CKSUM"
  cfeMajorVerItem = cpu_prefix & "_ES_CFEMAJORVER"
  cfeMinorVerItem = cpu_prefix & "_ES_CFEMINORVER"
  cfeRevisionItem = cpu_prefix & "_ES_CFEREVISION"
  cfeMissionRevItem = cpu_prefix & "_ES_CFEMSNREV"
  osMajorVerItem = cpu_prefix & "_ES_OSMAJORVER"
  osMinorVerItem = cpu_prefix & "_ES_OSMINORVER"
  osRevisionItem = cpu_prefix & "_ES_OSREVISION"
  osMissionRevItem = cpu_prefix & "_ES_OSMISSIONREV"

  ; Set the values
  gsescstatval12 = {cksumItem}
  gsescstatval22 = {cfeMajorVerItem}  & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  gsescstatval32 = {osMajorVerItem} & "." & {osMinorVerItem} & "." & {osRevisionItem} & "." & {osMissionRevItem}
elseif (to_cmd_sent = "3") then
  ; setup the database mnemonics for CPU2
  cpu_prefix = db_prefix & "3"

  cksumItem = cpu_prefix & "_ES_CKSUM"
  cfeMajorVerItem = cpu_prefix & "_ES_CFEMAJORVER"
  cfeMinorVerItem = cpu_prefix & "_ES_CFEMINORVER"
  cfeRevisionItem = cpu_prefix & "_ES_CFEREVISION"
  cfeMissionRevItem = cpu_prefix & "_ES_CFEMSNREV"
  osMajorVerItem = cpu_prefix & "_ES_OSMAJORVER"
  osMinorVerItem = cpu_prefix & "_ES_OSMINORVER"
  osRevisionItem = cpu_prefix & "_ES_OSREVISION"
  osMissionRevItem = cpu_prefix & "_ES_OSMISSIONREV"

  ; Set the values
  gsescstatval13 = {cksumItem}
  gsescstatval23 = {cfeMajorVerItem}  & "." & {cfeMinorVerItem} & "." & {cfeRevisionItem} & "." & {cfeMissionRevItem}
  gsescstatval33 = {osMajorVerItem} & "." & {osMinorVerItem} & "." & {osRevisionItem} & "." & {osMissionRevItem}
endif

%liv (log_procedure) = logging

endproc
