PROC $sc_$cpu_fm_openfiles
;*******************************************************************************
;  Test Name:  FM_OpenFiles
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the CFS File Manager (FM) 
;   application allows the maximum number of files to be opened simultaneously
;   as required. The FM_ListOpenFiles, FM_Delete, FM_DeleteAll and FM_Close
;   commands are also tested for required functionality. The free space for
;   the volumes will be monitored for changes.
;
;  Requirements Tested
;    FM1002	If the computed length of any FM command is not equal to the
;		length contained in the message header, FM shall reject the
;		command.
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command execution,
;		increment the FM Command Rejected Counter and issue an error
;		event.
;    FM2007	Upon receipt of a Delete All Files command, FM shall delete all
;		the files in the command-specified directory
;    FM2007.1	If the command-specified directory contains an open file, FM 
;		shall not delete the open file.
;    FM2007.1.1	For any open files that are not deleted, FM shall issue one 
;		event message.
;    FM2008	Upon receipt of a Delete command, FM shall delete the
;		command-specified file.
;    FM2008.1	If the command-specified file is open, FM shall reject the
;		command to delete this file.
;    FM2012	Upon receipt of a List Open Files command, FM shall generate a
;		message containing:
;			a) the number of open files 
;			b) up to <PLATFORM_DEFINED> names/paths (of each open
;			   file) and the application that has opened the file
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified file system
;    FM4000	FM shall generate a housekeeping message containing the
;		following:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;			c) For each file system: Total number of open files 
;    FM4001     Upon receipt of a Report Device Free Space command, FM shall
;               generate a message containing for each enabled device in the FM
;               device table the amount of available free space.
;    FM5000	Upon initialization of the FM Application, FM shall initialize
;		the following data to Zero
;			a) Valid Command Counter
;			b) Command Rejected Counter
;
;  Prerequisite Conditions
;    The cFE & FM are up and running and ready to accept commands. 
;    The FM commands and TLM items exist in the GSE database. 
;    The display page for the FM Housekeeping exists. 
;
;  Assumptions and Constraints
;
;  Change History
;
;	Date	   Name		Description
;	11/21/08   W. Moleski	Original Procedure
;	02/01/10   W. Moleski	Updated for FM 2.1.0.0
;       03/01/11   W. Moleski   Added variables for App name and ram directory
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG
;
;  Arguments
;	None
;
;  Procedures Called
;	Name			Description
;
; 
;  Required Post-Test Analysis
;	None
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "osconfig.h"
#include "fm_platform_cfg.h"
#include "fm_events.h"
#include "tst_fm_events.h"
#include "fm_defs.h"

%liv (log_procedure) = logging

#define FM_1002     0
#define FM_1003     1
#define FM_1004     2
#define FM_2007     3
#define FM_20071    4
#define FM_200711   5
#define FM_2008     6
#define FM_20081    7
#define FM_2012     8
#define FM_3000     9
#define FM_4000     10
#define FM_4001     11
#define FM_5000     12

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 12
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1002", "FM_1003", "FM_1004", "FM_2007", "FM_2007.1", "FM_2007.1.1", "FM_2008", "FM_2008.1", "FM_2012", "FM_3000", "FM_4000", "FM_4001", "FM_5000"]

local rawcmd, cmdctr, errcnt

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local freeSpaceTblName = FMAppName & "." & FM_TABLE_CFE_NAME

local testDir1   = ramDir & "/FMTEST1"
local testDir2   = ramDir & "/FMTEST2"
local uploadDir1 = ramDirPhys & "/FMTEST1"
local uploadDir2 = ramDirPhys & "/FMTEST2"

local testFile = "FMINFO.TST"

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

write ";*********************************************************************"
write ";  Step 1.2: Create the FreeSpace table load image, upload this image, "
write ";  and start the File Manager (FM) and Test (TST_FM) Applications. "
write ";********************************************************************"
s $sc_$cpu_fm_tableloadfile
wait 5

s ftp_file ("CF:0/apps", "$cpu_fmdevtbl_ld_1", FM_TABLE_FILENAME, "$CPU", "P")
wait 5

s $sc_$cpu_fm_startfmapps
wait 5

write ";*********************************************************************"
write ";  Step 1.3: Verify that the FM Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
local hkPktId, fsindex

;; Set the FM HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p08A"
fsindex = "0FBA"

if ("$CPU" = "CPU2") then
  hkPktId = "p18A"
  fsindex = "0FD8"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p28A"
  fsindex = "0FF8"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4000) - Housekeeping packet is being generated."
  ut_setrequirements FM_4000, "P"
else
  write "<!> Failed (4000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements FM_4000, "F"
endif

;; Check the HK tlm items to see if they are 0 or NULL
if ($SC_$CPU_FM_CMDPC = 0) AND ($SC_$CPU_FM_CMDEC = 0) THEN
  write "<*> Passed (5000) - Housekeeping telemetry initialized properly."
  ut_setrequirements FM_5000, "P"
else
  write "<!> Failed (5000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC           = ",$SC_$CPU_FM_CMDPC
  write "  CMDEC           = ",$SC_$CPU_FM_CMDEC
  ut_setrequirements FM_5000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.0: File System Preparation."
write ";*********************************************************************"
write ";  Step 2.1: Send the Create Directory Command for the first Test "
write ";  Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir1

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Create Directory command sent properly."
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly (", UT_TW_Status, ")."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send the Create Directory Command for the second Test "
write ";  Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir2

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Create Directory command sent properly."
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly (", UT_TW_Status, ")."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Save the open file count and free space for '", ramDir,"'"
write ";*********************************************************************"
local openFileCnt = $SC_$CPU_FM_NUMOPEN
write "** Number of currently open files = ",openFileCnt

s get_tbl_to_cvt (ramDirPhys, freeSpaceTblName, "A", "$cpu_fmfreespacetbl","$CPU",fsindex)
wait 5

local ramDirIndex=0

;; Find the ramDir entry index
for i = 0 to FM_TABLE_ENTRY_COUNT-1 do
  if ($SC_$CPU_FM_FreeSpaceTBL[i].Name = ramDir) then
    ramDirIndex = i
    break
  endif
enddo

;; Now get the Free Space
local upper32Size=0, lower32Size=0

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_GetFreeSpace

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Get Free Space command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_4001, "P"

  ;; Set the sizes specified in the packet
  upper32Size = $SC_$CPU_FM_FreeSpacePkt[ramDirIndex].Upper32
  lower32Size = $SC_$CPU_FM_FreeSpacePkt[ramDirIndex].Lower32
  write "Free space on '",$SC_$CPU_FM_FreeSpacePkt[ramDirIndex].Name, "' is ", %hex(upper32Size)," and ",%hex(lower32Size)
else
  write "<!> Failed (1003;4001) - Get Free Space command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_4001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Upload the max number of open files + 2 files to $CPU."
write ";*********************************************************************"
local destFileName
local switchCnt = OS_MAX_NUM_OPEN_FILES/ 4
switchCnt = switchCnt * 3
local totalExpected1, totalExpected2

;; Upload 75% of the files to testDir1 and the remaining 25% to testDir2
write "will upload ", switchCnt, " files to '", uploadDir1, "'"

for i = 1 to OS_MAX_NUM_OPEN_FILES DO
  destFileName = "FMINFO" & i & ".TST"

  if (i < switchCnt) then
    ;; Upload the same Test file but with a different destination name
    write "Uploading file as '", destFileName, "' to '", uploadDir1, "'"
    s ftp_file(uploadDir1, testFile, destFileName, "$CPU", "P")
    totalExpected1 = totalExpected1 + 1
  else
    ;; Upload the same Test file but with a different destination name
    write "Uploading file as '", destFileName, "' to '", uploadDir2, "'"
    s ftp_file (uploadDir2, testFile, destFileName, "$CPU", "P")
    totalExpected2 = totalExpected2 + 1
  endif
enddo

wait 5

write ";*********************************************************************"
write ";  Step 2.5: Verify the free space for '",ramDir,"' has been reduced. "
write ";*********************************************************************"
local newUpper32Size=0, newLower32Size=0

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_GetFreeSpace

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4001) - Get Free Space command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_4001, "P"

  ;; Get the new sizes reported for the ramDir
  newUpper32Size = $SC_$CPU_FM_FreeSpacePkt[ramDirIndex].Upper32
  newLower32Size = $SC_$CPU_FM_FreeSpacePkt[ramDirIndex].Lower32
  write "Free space on '",$SC_$CPU_FM_FreeSpacePkt[ramDirIndex].Name, "' is ", %hex(newUpper32Size)," and ",%hex(newLower32Size)
else
  write "<!> Failed (1003;4001) - Get Free Space command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_4001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Open Files Test"
write ";*********************************************************************"
write ";  Step 3.1:  Open the maximum number of files allowed."
write ";*********************************************************************"
;; Here we want to open all the files in each test directory created above

local fileOffset = 0
local allFilesRetrieved = FALSE
local filesToOpen = 0
local fullFileName

while (allFilesRetrieved = FALSE) do
  cmdctr = $SC_$CPU_FM_CMDPC + 1

  s $sc_$cpu_fm_dirtlmdisplay (testDir1, fileOffset)

  if (cmdctr = $SC_$CPU_FM_CMDPC) then
    write "<*> Passed - Directory Listing command."
  else
    write "<!> Failed - Directory Listing command."
  endif

  write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles

  if (fileOffset > $SC_$CPU_FM_TotalFiles) then
    filesToOpen = 0
    allFilesRetrieved = TRUE
  elseif (($SC_$CPU_FM_TotalFiles - fileOffset) < FM_DIR_LIST_PKT_ENTRIES) then
    filesToOpen = $SC_$CPU_FM_TotalFiles - fileOffset
  else
    filesToOpen = FM_DIR_LIST_PKT_ENTRIES
  endif

  for j = 1 to filesToOpen do
    fullFileName = testDir1 & "/" & $SC_$CPU_FM_DirList[j].Name
    ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

    cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

    /$SC_$CPU_TST_FM_Open File=fullFileName

    ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Open File command for '", fullFileName, "' sent properly."
    else
      write "<!> Failed - Open File command for '", fullFileName, "'"
    endif

    if ($SC_$CPU_num_found_messages = 1) THEN
      write "<*> Passed - Expected Event message ",$SC_$CPU_find_event[1].eventid, " received"
    else
      write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_OPENFILE_INF_EID, "."
    endif
  enddo

  fileOffset = fileOffset + FM_DIR_LIST_PKT_ENTRIES
  write "** FileOffset = ", fileOffset
  wait 5
enddo

;;*******************************
;; Open all the files in testDir2
;;*******************************
fileOffset = 0
allFilesRetrieved = FALSE
filesToOpen = 0

while (allFilesRetrieved = FALSE) do
  cmdctr = $SC_$CPU_FM_CMDPC + 1

  s $sc_$cpu_fm_dirtlmdisplay (testDir2, fileOffset)

  if (cmdctr = $SC_$CPU_FM_CMDPC) then
    write "<*> Passed - Directory Listing command."
  else
    write "<!> Failed - Directory Listing command."
  endif

  write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles

  if (fileOffset > $SC_$CPU_FM_TotalFiles) then
    filesToOpen = 0
    allFilesRetrieved = TRUE
  elseif (($SC_$CPU_FM_TotalFiles - fileOffset) < FM_DIR_LIST_PKT_ENTRIES) then
    filesToOpen = $SC_$CPU_FM_TotalFiles - fileOffset
  else
    filesToOpen = FM_DIR_LIST_PKT_ENTRIES
  endif

  for j = 1 to filesToOpen do
    fullFileName = testDir2 & "/" & $SC_$CPU_FM_DirList[j].Name
    ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

    cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

    /$SC_$CPU_TST_FM_Open File=fullFileName

    ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Open File command for '", fullFileName, "' sent properly."
    else
      write "<!> Failed - Open File command for '", fullFileName, "'"
    endif

    if ($SC_$CPU_num_found_messages = 1) THEN
      write "<*> Passed - Expected Event message ",$SC_$CPU_find_event[1].eventid, " received"
    else
      write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_OPENFILE_INF_EID, "."
    endif
  enddo

  fileOffset = fileOffset + FM_DIR_LIST_PKT_ENTRIES
  write "** FileOffset = ", fileOffset
  wait 5
enddo

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send the list open files command.         "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_OPEN_FILES_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_ListOpenFiles

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2012) - List Open Files command sent properly."
  ut_setrequirements FM_2012, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Expected Event message ", $SC_$CPU_find_event[1].eventid," not received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2012) - List Open Files command failed to increment CMDPC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2012, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Verify that the HK telemetry indicates the max number of "
write ";  open files."
write ";*********************************************************************"
openFileCnt = $SC_$CPU_FM_NUMOPEN

if (openFileCnt = OS_MAX_NUM_OPEN_FILES) then
  write "<*> Passed - Open File count = ", openFileCnt, " as expected."
else
  write "<!> Failed - Open File count = ", openFileCnt, ". Expected ",OS_MAX_NUM_OPEN_FILES
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Delete Files while Open Test"
write ";*********************************************************************"
write ";  Step 4.1: Perform Delete All Files Command attempting to delete the"
write ";  Files that are open in Test Directory 2. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_WARNING_EID, "INFO", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DeleteAll DirName=testDir2

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2007) - Delete All command sent properly."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Expected Event message ", $SC_$CPU_find_event[1].eventid," not received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2007) - Delete All command failed to increment CMDPC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"
endif

;; Check for the open file error event messages
if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
  write "<*> Passed (2007.1.1) - Event message ", $SC_$CPU_find_event[2].eventid, " received ( File Open ) as expected"
  ut_setrequirements FM_200711, "P"
else
  write "<!> Failed (2007.1.1) - Expected Event message ", $SC_$CPU_find_event[2].eventid," not received."
  ut_setrequirements FM_200711, "F"
endif

;;; Verify that the files were not deleted.
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir2, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - Directory Listing command."
else
  write "<!> Failed - Directory Listing command."
endif

write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles
if ($SC_$CPU_FM_TotalFiles = totalExpected2) then
  write "<*> Passed (2007.1) - Directory Listing verifies that no files were deleted."
  ut_setrequirements FM_20071, "P"
else
  write "<!> Failed (2007.1) - Directory '", testDir2, "' contained ",$SC_$CPU_FM_TotalFiles, " files. Expected ",totalExpected2, " files."
  ut_setrequirements FM_20071, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Send the Delete Command for several files that are open in"
write ";  Test Directory 1. "
write ";*********************************************************************"
;; retrieve the first FM_DIR_LIST_PKT_ENTRIES from testDir1
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir1, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - Directory Listing command."
else
  write "<!> Failed - Directory Listing command."
endif

;; Attempt to delete the first 10 files returned in the listing above
local filesToDelete = 10
if ($SC_$CPU_FM_TotalFiles < 10) then
  filesToDelete = $SC_$CPU_FM_TotalFiles
endif

for j = 1 to filesToDelete do
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID, "ERROR", 1

  fullFileName = testDir1 & "/" & $SC_$CPU_FM_DirList[j].Name
  write "*** CMDEC = ",$SC_$CPU_FM_CMDEC
  errcnt = $SC_$CPU_FM_CMDEC + 1

  /$SC_$CPU_FM_Delete File=fullFileName

  ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (2008.1)- Delete File command for '", fullFileName, "' failed as expected since the file was open."
    ut_setrequirements FM_20081, "P"

    ;; Look for Event Message
    if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
      write "<*> Passed (1004;2008) - Expected Event message ",$SC_$CPU_find_event[1].eventid, " received"
      ut_setrequirements FM_1004, "P"
      ut_setrequirements FM_2008, "P"
    else
      write "<!> Failed (1004;2008) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_SRC_ERR_EID
      ut_setrequirements FM_1004, "F"
      ut_setrequirements FM_2008, "F"
    endif
  else
    write "<!> Failed (2008.1) - Delete File command for '", fullFileName, "' worked when failure was expected."
    ut_setrequirements FM_20081, "F"
  endif
enddo

;;; Verify that the files were not deleted.
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir1, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - Directory Listing command."
else
  write "<!> Failed - Directory Listing command."
endif

write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles
if ($SC_$CPU_FM_TotalFiles = totalExpected1) then
  write "<*> Passed (2008.1) - Directory Listing verifies that no files were deleted."
  ut_setrequirements FM_20081, "P"
else
  write "<!> Failed (2008.1) - Directory '", testDir1, "' contained ",$SC_$CPU_FM_TotalFiles, " files. Expected ",totalExpected1, " files."
  ut_setrequirements FM_20081, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Close Files Test"
write ";*********************************************************************"
write ";  Step 5.1: Send the Close Command for 75% of the files contained in "
write ";  Test Directory 1. "
write ";*********************************************************************"
fileOffset = 0
allFilesRetrieved = FALSE
local filesToClose = 0
local closeCnt = 0

openFileCnt = $SC_$CPU_FM_NUMOPEN

while (allFilesRetrieved = FALSE) do
  cmdctr = $SC_$CPU_FM_CMDPC + 1

  s $sc_$cpu_fm_dirtlmdisplay (testDir1, fileOffset)

  if (cmdctr = $SC_$CPU_FM_CMDPC) then
    write "<*> Passed - Directory Listing command."
  else
    write "<!> Failed - Directory Listing command."
  endif

  write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles
  if (closeCnt = 0) then
    closeCnt = $SC_$CPU_FM_TotalFiles / 4
    closeCnt = closeCnt * 3
    write "*** Will close ", closeCnt, " files in '", testDir1, "'"
  endif

  if (fileOffset > closeCnt) then
    filesToClose = 0
    allFilesRetrieved = TRUE
  elseif ((closeCnt - fileOffset) < FM_DIR_LIST_PKT_ENTRIES) then
    filesToClose = closeCnt - fileOffset
  else
    filesToClose = FM_DIR_LIST_PKT_ENTRIES
  endif

  for j = 1 to filesToClose do
    fullFileName = testDir1 & "/" & $SC_$CPU_FM_DirList[j].Name
    ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

    cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

    /$SC_$CPU_TST_FM_Close File=fullFileName

    ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Close File command for '", fullFileName, "' sent properly."
    else
      write "<!> Failed - Close File command for '", fullFileName, "'.CMDPC = ",$SC_$CPU_TST_FM_CMDPC,"; Expected ",cmdctr
    endif

    if ($SC_$CPU_num_found_messages = 1) THEN
      write "<*> Passed - Expected Event message ",$SC_$CPU_find_event[1].eventid, " received"
    else
      write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_FM_CLOSEFILE_INF_EID
    endif
  enddo

  fileOffset = fileOffset + FM_DIR_LIST_PKT_ENTRIES
  write "** FileOffset = ", fileOffset
  wait 5
enddo

;;; Verify that the files were closed.
if ($SC_$CPU_FM_NUMOPEN = (openFileCnt - closeCnt)) then
  write "<*> Passed - Open File count as expected"
else
  write "<!> Failed - Open File count = ",$SC_$CPU_FM_NUMOPEN, ". Expected ",openFileCnt - closeCnt, "." 
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.2: Perform Delete All Files Command on Test Directory 1.    "
write ";  Verify that only the files that are open remain in the directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_WARNING_EID, "INFO", 2

;; Save the total files in testDir1
local totalDir1Files = $SC_$CPU_FM_TotalFiles
totalExpected1 = $SC_$CPU_FM_TotalFiles - closeCnt
write "*** Total Files in directory before delete = ", $SC_$CPU_FM_TotalFiles
write "*** Total Files expected after delete = ", totalExpected1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=testDir1

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2007) - Command Accepted Counter incremented as expected."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (2007.1.1) - Event message ", $SC_$CPU_find_event[2].eventid, " received"
    ut_setrequirements FM_200711, "P"
  else
    write "<!> Failed (2007.1.1) - Did not receive EM " & $SC_$CPU_find_event[2].eventid & " Either the wrong EM is expected or the wrong one was received. "
    ut_setrequirements FM_200711, "F"
  endif
else
  write "<!> Failed (2007) - Command Rejected Delete All command."
  ut_setrequirements FM_2007, "F"
endif

;;; Verify that the open files were not deleted.
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir1, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - Directory Listing command."
else
  write "<!> Failed - Directory Listing command."
endif

write "*** Total Files in directory after delete = ", $SC_$CPU_FM_TotalFiles

if (totalExpected1 = $SC_$CPU_FM_TotalFiles) then
  write "<*> Passed (2007.1)- All Closed files were deleted"
  ut_setrequirements FM_20071, "P"
else
  write "<!> Failed (2007.1) - Expected ",totalExpected1, " files in the directory after delete. Found ",$SC_$CPU_FM_TotalFiles
  ut_setrequirements FM_20071, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3: Send the List Open Files Command to list the remaining   "
write ";  open files. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_OPEN_FILES_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_ListOpenFiles

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2012) - List Open Files command sent properly."
  ut_setrequirements FM_2012, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Expected Event message ", $SC_$CPU_find_event[1].eventid," not received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2012) - List Open Files command failed to increment CMDPC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2012, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4: Send the Close Command on the remaining open files       "
write ";*********************************************************************"
;;; NOTE: The total number of open files will never exceed 
write "*** Total Open Files = ", $SC_$CPU_FM_TotalOpenFiles

filesToClose = $SC_$CPU_FM_TotalOpenFiles
if ($SC_$CPU_FM_TotalOpenFiles > OS_MAX_NUM_OPEN_FILES) then
  filesToClose = OS_MAX_NUM_OPEN_FILES
endif

for j = 1 to filesToClose do
  ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

  cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

  fullFileName = $SC_$CPU_FM_OpenFileList[j].FileName
  /$SC_$CPU_TST_FM_Close File=fullFileName 

  ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed - Close File command for '", fullFileName, "' sent properly."
  else
    write "<!> Failed - Close File command for '", fullFileName, "'.CMDPC = ",$SC_$CPU_TST_FM_CMDPC,"; Expected ",cmdctr
  endif

  if ($SC_$CPU_num_found_messages = 1) THEN
    write "<*> Passed - Expected Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_CLOSEFILE_INF_EID, "."
  endif
enddo

;;; Verify that the files were closed.
if ($SC_$CPU_FM_NUMOPEN != 0) then
  write "<!> Failed - All files were not closed."
else
  write "<*> Passed - All files were closed as expected."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0: Delete Files Test"
write ";*********************************************************************"
write ";  Step 6.1: Send the Delete All Files Command on Test Directory 1. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DeleteAll DirName=testDir1

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2007) - Delete All command sent properly."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Expected event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Expected event message",FM_DELETE_ALL_CMD_EID, " was not received."
  ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2007) - Delete All command did not increment CMDPC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"
endif

;;; Verify that the files were deleted.
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir1, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - Directory Listing command."
else
  write "<!> Failed - Directory Listing command."
endif

write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles
if ($SC_$CPU_FM_TotalFiles = 0) then
  write "<*> Passed (2007) - Directory Listing verifies that ALL files were deleted."
  ut_setrequirements FM_2007, "P"
else
  write "<!> Failed (2007) - Directory '", testDir1, "' contained ",$SC_$CPU_FM_TotalFiles, " files. Expected 0 files."
  ut_setrequirements FM_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2: Send the Delete Command for files in Test Directory 2. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DeleteAll DirName=testDir2

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2007) - CMDPC incremented as expected."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Expected event message",FM_DELETE_ALL_CMD_EID, " was not received."
    ut_setrequirements FM_1003, "P"
  endif
else
  write "<!> Failed (1003;2007) - Delete All command did not increment CMDPC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"
endif

;;; Verify that the files were deleted.
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir2, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - Directory Listing command."
else
  write "<!> Failed - Directory Listing command."
endif

write "*** Total Files in directory = ", $SC_$CPU_FM_TotalFiles
if ($SC_$CPU_FM_TotalFiles = 0) then
  write "<*> Passed (2007) - Directory Listing verifies that ALL files were deleted."
  ut_setrequirements FM_2007, "P"
else
  write "<!> Failed (2007) - Directory '", testDir2, "' contained ",$SC_$CPU_FM_TotalFiles, " files. Expected 0 files."
  ut_setrequirements FM_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.0: List Open Files Stress Test"
write ";*********************************************************************"
write ";  Step 7.1: Send the List Open Files Command with an invalid length. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_OPEN_FILES_PKT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000020BA8"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000020BA8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000020BA8"
endif

;; Too Long
ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_OPEN_FILES_PKT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

;; Send the command for a packet that is too short
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_OPEN_FILES_PKT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000000BA8"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000000BA8"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000000BA8"
endif

;; Too Short
ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1002) - Command Rejected Counter incremented."
  ut_setrequirements FM_1002, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_OPEN_FILES_PKT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.2: Send the Close Command for all open files.               "
write ";*********************************************************************"
openFileCnt = $SC_$CPU_FM_NUMOPEN

if (openFileCnt != 0) then
  ;;; Send the List Open Files command 
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_OPEN_FILES_CMD_EID, "DEBUG", 1

  cmdctr = $SC_$CPU_FM_CMDPC + 1

  /$SC_$CPU_FM_ListOpenFiles

  ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
  if (UT_TW_Status = UT_Success) then
    write "<*> Passed (2012) - List Open Files command sent properly."
    ut_setrequirements FM_2012, "P"

    if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
      write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid," received."
      ut_setrequirements FM_1003, "P"
    else
      write "<!> Failed (1003) - Expected Event message ", $SC_$CPU_find_event[1].eventid," not received."
      ut_setrequirements FM_1003, "F"
    endif
  else
    write "<!> Failed (1003;2012) - List Open Files command failed to increment CMDPC"
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_2012, "F"
  endif

  ;;; Send the close command for all open files contained in the Open File list
  ;;; generated by the above step
  write "*** Total Open Files = ", $SC_$CPU_FM_TotalOpenFiles
  filesToClose = $SC_$CPU_FM_TotalOpenFiles
  if ($SC_$CPU_FM_TotalOpenFiles > OS_MAX_NUM_OPEN_FILES) then
    filesToClose = OS_MAX_NUM_OPEN_FILES
  endif

  for j = 1 to filesToClose do
    ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

    cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

    fullFileName = $SC_$CPU_FM_OpenFileList[j].FileName
    /$SC_$CPU_TST_FM_Close File=fullFileName 

    ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Close File command for '", fullFileName, "' sent properly."
    else
      write "<!> Failed - Close File command for '", fullFileName, "'.CMDPC = ",$SC_$CPU_TST_FM_CMDPC,"; Expected ",cmdctr
    endif

    if ($SC_$CPU_num_found_messages = 1) THEN
      write "<*> Passed - Expected Event message ",$SC_$CPU_find_event[1].eventid, " received"
    else
      write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_CLOSEFILE_INF_EID, "."
    endif
  enddo

  ;;; Verify that the files were closed.
  if ($SC_$CPU_FM_NUMOPEN != 0) then
    write "<!> Failed - All files were not closed."
  else
    write "<*> Passed - All files were closed as expected."
  endif
else
  write "*** All files are closed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.3: Send the List Open Files Command when there are no open  "
write ";  files. Not sure what the results of this command will be. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_OPEN_FILES_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_ListOpenFiles

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2012) - List Open Files command sent properly."
  ut_setrequirements FM_2012, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Expected Event message ", $SC_$CPU_find_event[1].eventid," not received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2012) - List Open Files command failed to increment CMDPC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2012, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 8.0:  Perform a Power-on Reset to clean-up from this test."
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

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_openfiles                                "
write ";*********************************************************************"
ENDPROC
