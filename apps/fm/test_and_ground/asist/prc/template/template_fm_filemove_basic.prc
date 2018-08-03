PROC $sc_$cpu_fm_filemove_basic
;*******************************************************************************
;  Test Name:  FM_FileMove_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM) File Move
;   Commands function properly. The FM_FileMove, and FM_FileInfo commands will
;   be tested to see if the FM application handles these as desired by the
;   requirements. The FM_DirCreate command is used to facilitate the testing.
;
;  Requirements Tested
;   FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue an
;		event message
;   FM1004	If FM rejects any command, FM shall abort the command execution,
;		increment the FM Command Rejected Counter and issue an error
;		event message
;   FM1005	If the filename specified in any FM command is not valid, FM
;		shall reject the command
;   FM2004	Upon receipt of a Move command, FM shall move the
;		command-specified file to the command-specified destination file
;   FM2004.1	If the command-specified destination file exists, FM shall
;		reject the command
;   FM2011	Upon receipt of a File Info command, FM shall generate a
;		message containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open, Closed, or Non-existent)
;			d) <MISSION_DEFINED> CRC
;   FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified filesystem
;   FM4000	FM shall generate a housekeeping message containing the
;		following:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;			c) For each file system: Total number of open files 
;   FM5000	Upon initialization of the FM Application, FM shall initialize
;		the following data to Zero
;			a) Valid Command Counter
;			b) Command Rejected Counter
;
;
;  Prerequisite Conditions
;    The cFE & FM are up and running and ready to accept commands. 
;    The FM commands and TLM items exist in the GSE database. 
;    The display pages for the FM Telemetry exist. 
;
;  Assumptions and Constraints
;    The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	05/08/08   D. Stewart	Original Procedure
;	12/03/08   W. Moleski	Updated the procedure to contain Requirements
;				2004.1 and 2004.2 and did some general cleanup
;	01/29/10   W. Moleski	Updated for FM 2.1.0.0
;       02/28/11   W. Moleski   Added variables for App name and ram directory
;       08/18/11   W. Moleski   Added Overwrite argument to Move commands
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG
;
;  Arguments
;	None
;
;  Procedures Called
;
;	Name			Description
; 
;  Required Post-Test Analysis

;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "fm_platform_cfg.h"
#include "fm_events.h"
#include "tst_fm_events.h"

%liv (log_procedure) = logging

#define FM_1003     0
#define FM_1004     1
#define FM_1005     2
#define FM_2004     3
#define FM_20041    4
#define FM_2011     5
#define FM_3000     6
#define FM_4000     7
#define FM_5000     8

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 8
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_1005", "FM_2004", "FM_2004.1", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir1  = ramDir & "/FMDIR1"
local testDir2  = ramDir & "/FMDIR2"
local uploadDir = ramDirPhys & "/FMDIR1"

local testFile      = "FMMOVE.TST"
local testFileMove2 = "FMMOVE2.TST"
local testFileMove3 = "FMMOVE3.TST"

local fullOriginalTestFileName = testDir1 & "/" & testFile
local fullTestFileMove1Name    = testDir2 & "/" & testFile
local fullTestFileMove2Name    = testDir2 & "/" & testFileMove2
local fullTestFileMove3Name    = testDir1 & "/" & testFileMove3

local lastSrcFile = fullOriginalTestFileName
local currentSrcFile = fullOriginalTestFileName

local cmdctr
local errcnt

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
local hkPktId

;; Set the FM HK packet ID based upon the cpu being used
if ("$CPU" = "CPU1" OR "$CPU" = "") then
  hkPktId = "p08A"
elseif ("$CPU" = "CPU2") then
  hkPktId = "p18A"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p28A"
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
write ";  Step 2.1: Perform Create Directory Command to Create Directory 1."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir1

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Perform Create Directory Command to Create Directory 2."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir2

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Upload Test File to Directory 1."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Invalid File Move Tests"
write ";*********************************************************************"
write ";  Step 3.1:  Verify source File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (currentSrcFile)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File Info Telemetry indicates that '",currentSrcFile,"' exists"
  else
    write "<!> Failed - File Info Telemetry indicates that '",currentSrcFile, "' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = currentSrcFile) THEN
    write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", currentSrcFile
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", currentSrcFile
    ut_setrequirements FM_2011, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Verify move file 1 does not exist"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileMove1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFileMove1Name,"' does not exist"
  else
    write "<!> Failed - File Info Telemetry indicates a status of '",p@$SC_$CPU_FM_FIleStatus, "'. Expected 'Not Used'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Perform Move File Command to Move File 1 to Directory 2."
write ";  This command should be rejected."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_TGT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileMove Overwrite=0 File=currentSrcFile DestName=testDir2

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;2004) - File Move command to a directory failed as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_MOVE_TGT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2004) - File Move command to a directory was accepted when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Perform Move File Command to Move Directory 1 to File 1."
write ";  This command should be rejected."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileMove Overwrite=0 File=testDir1 DestName=fullTestFileMove1Name

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1005;2004) - File Move command failed with a directory as a source file."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_MOVE_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2004) - File Move command for a directory was accepted when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  File Move Tests"
write ";*********************************************************************"
write ";  Step 4.1:  Verify Move File 2 does not exist."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileMove2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFileMove2Name,"' does not exist"
  else
    write "<!> Failed - File Info Telemetry indicates a status of '",p@$SC_$CPU_FM_FIleStatus, "'. Expected 'Not Used'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Perform Move File Command to move the current source file "
write ";  to a Destination File Name specified by Move File 2."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileMove Overwrite=0 File=currentSrcFile DestName=fullTestFileMove2Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - File Move command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2004, "P"

  lastSrcFile = currentSrcFile
  currentSrcFile = fullTestFileMove2Name

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_MOVE_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2004) - File Move command not sent properly (", UT_TW_status, ")."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: File Info Command to Verify source File no longer exists"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (lastSrcFile)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2004) - File Info Telemetry indicates that '",lastSrcFile,"' does not exist"
    ut_setrequirements FM_2004, "P"
  else
    write "<!> Failed (2004) - File Info Telemetry indicates a status of '",p@$SC_$CPU_FM_FIleStatus, "'. Expected 'Not Used'"
    ut_setrequirements FM_2004, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4:  File Info Command to Verify move file 2 exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileMove2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed (2004) - File Info Telemetry indicates that '",fullTestFileMove2Name,"' exists"
    ut_setrequirements FM_2004, "P"
  else
    write "<!> Failed (2004) - File Info Telemetry indicates that '",fullTestFileMove2Name, "' does not exist"
    ut_setrequirements FM_2004, "F"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullTestFileMove2Name) THEN
    write "<*> Passed (2004;2011) - File Info Telemetry Matches expected file: ", fullTestFileMove2Name
    ut_setrequirements FM_2004, "P"
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2004;2011) - File Info Telemetry does not match expected file: ", fullTestFileMove2Name
    ut_setrequirements FM_2004, "F"
    ut_setrequirements FM_2011, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5:  File Info Command to Verify move file 3 does not exist. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileMove3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFileMove3Name,"' does not exist"
  else
    write "<!> Failed - File Info Telemetry indicates a status of '",p@$SC_$CPU_FM_FIleStatus, "'. Expected 'Not Used'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6:  Perform Move File Command to Move the current source file "
write ";  to the Destination File specifed by move file 3. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileMove Overwrite=0 File=currentSrcFile DestName=fullTestFileMove3Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004) - File Move command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2004, "P"

  lastSrcFile = currentSrcFile
  currentSrcFile = fullTestFileMove3Name

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_MOVE_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2004) - File Move command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7:  File Info Command to Verify source file no longer exists"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (lastSrcFile)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2004) - File Info Telemetry indicates that '",lastSrcFile,"' does not exist"
    ut_setrequirements FM_2004, "P"
  else
    write "<!> Failed (2004) - File Info Telemetry indicates a status of '",p@$SC_$CPU_FM_FIleStatus, "'. Expected 'Not Used'"
    ut_setrequirements FM_2004, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.8:  File Info Command to Verify move file 3 exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileMove3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed (2004) - File Info Telemetry indicates that '",fullTestFileMove3Name,"' exists"
    ut_setrequirements FM_2004, "P"
  else
    write "<!> Failed (2004) - File Info Telemetry indicates that '",fullTestFileMove3Name, "' does not exist"
    ut_setrequirements FM_2004, "F"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullTestFileMove3Name) THEN
    write "<*> Passed (2004;2011) - File Info Telemetry Matches expected file: ", fullTestFileMove2Name
    ut_setrequirements FM_2004, "P"
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2004;2011) - File Info Telemetry does not match expected file: ", fullTestFileMove2Name
    ut_setrequirements FM_2004, "F"
    ut_setrequirements FM_2011, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9: Open Source File Test "
write ";*********************************************************************"
write ";  Step 4.9.1:  Open the Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

local expOpenFiles = $SC_$CPU_FM_NumOpen + 1
cmdctr = $SC_$CPU_TST_FM_CMDPC + 1
errcnt = $SC_$CPU_TST_FM_CMDEC + 1

/$SC_$CPU_TST_FM_Open File=currentSrcFile

wait until (($SC_$CPU_TST_FM_CMDPC = cmdctr) OR ($SC_$CPU_TST_FM_CMDEC = errcnt ))
if ($SC_$CPU_TST_FM_CMDPC = cmdctr) then
  write "<*> Passed - File reported Open by test app."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_FM_OPENFILE_INF_EID
  endif
else
  write "<!> Failed - File Open command not sent properly to test app."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9.2:  Verify File is open."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (currentSrcFile)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Open") THEN
    write "<*> Passed - File Info Telemetry indicates File is open"
  else
    write "<!> Failed - File Info Telemetry does not indicate File is open"
  endif

  if ($SC_$CPU_FM_NumOpen = expOpenFiles) THEN
    write "<*> Passed - HK Telemetry Reports number of open files correctly: ",$SC_$CPU_FM_NumOpen
  else
    write "<!> Failed - HK Telemetry Reports number of open files incorrectly. Expected ",expOpenFiles," rcv'd ",$SC_$CPU_FM_NumOpen
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9.3: Perform Move File Command attempting to Move a Source"
write ";  File that is open. This should fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_OS_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$SC_$CPU_FM_FileMove Overwrite=0 File=currentSrcFile DestName=testDir2 & "/openSrc.tst"

ut_tlmwait $SC_$CPU_FM_CHILDCMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_2004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_MOVE_OS_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2004) - Move Command accepted with an open file."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_2004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9.4: Verify the destination file does not exist."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (testDir2 & "/openSrc.tst")

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",testDir2,"/openSrc.tst' exists"
  else
    write "<!> Failed - File Info Telemetry indicates a status of '",p@$SC_$CPU_FM_FIleStatus, "'. Expected 'Not Used'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command incremented the CMDEC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.9.5: Close the destination file created above."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

/$SC_$CPU_TST_FM_Close File=currentSrcFile

ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Close File command for sent properly."
else
  write "<!> Failed - Close File command. CMDPC = ",$SC_$CPU_TST_FM_CMDPC,"; Expected ",cmdctr
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.10: File Move Command with an existing destination file."
write ";*********************************************************************"
write ";  Step 4.10.1: Upload Test File to Directory 1."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 4.10.2: File Move Command with an existing destination file."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_TGT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1
 
/$SC_$CPU_FM_FileMove Overwrite=0 File=fullOriginalTestFileName DestName=currentSrcFile

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2004.1) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_20041, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_MOVE_TGT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2004.1) - Move Command did not increment CMDEC as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_20041, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.10.3: File Move Command with an existing destination file and"
write ";  the overwrite flag set. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1
 
/$SC_$CPU_FM_FileMove Overwrite=1 File=fullOriginalTestFileName DestName=currentSrcFile

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2004.1) - Move Command with overwrite flag set was successful."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_20041, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_MOVE_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2004.1) - Move Command with overwrite flag set did not increment CMDPC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_20041, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0:  Perform a Power-on Reset to clean-up from this test."
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

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_filemove_basic  "
write ";*********************************************************************"
ENDPROC
