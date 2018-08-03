PROC $sc_$cpu_fm_filerename_basic
;*******************************************************************************
;  Test Name:  FM_FileRename_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM) File
;   Rename Command functions properly. The FM_FileRename, and FM_FileInfo
;   commands will be tested to see if the FM application handles these as
;   desired by the requirements. The FM_DirCreate command is also used to
;   facilitate the testing.
;
;  Requirements Tested
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue an
;		event message
;    FM1004	If FM rejects any command, FM shall abort the command execution,
;		increment the FM Command Rejected Counter and issue an error
;		event message
;    FM2005	Upon receipt of a Rename command, FM shall rename the
;		command-specified file to the command-specified destination file
;    FM2005.1	If the command-specified destination file exists, FM shall
;		reject the command
;    FM2011	Upon receipt of a File Info command, FM shall generate a message
;		containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open, Closed, or Non-existent)
;			d) <MISSION_DEFINED> CRC
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified filesystem.
;    FM4000	FM shall generate a housekeeping message containing the
;		following:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;			c) For each file system: Total number of open files 
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
;    The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	04/14/08   D. Stewart	Original Procedure
;	12/04/08   W. Moleski	Adding tests for new requirements 2005.1 and
;				2005.2 and doing some general cleanup
;	01/29/10   W. Moleski	Updated for FM 2.1.0.0
;       03/01/11   W. Moleski   Added variables for App name and ram directory
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
#include "fm_events.h"
#include "fm_platform_cfg.h"
#include "tst_fm_events.h"

%liv (log_procedure) = logging

#define FM_1003     0
#define FM_1004     1
#define FM_2005     2
#define FM_20051    3
#define FM_2011     4
#define FM_3000     5
#define FM_4000     6
#define FM_5000     7

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 7
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_2005", "FM_2005.1", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local rawcmd

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir1  = ramDir & "/FMTEST1"
local testDir2  = ramDir & "/FMTEST2"
local uploadDir = ramDirPhys & "/FMTEST1"

local testFile1 = "FMRENAME1.TST"
local testFile2 = "FMRENAME2.TST"
local testFile3 = "FMRENAME3.TST"
local fullTestFile1Name = testDir1 & "/" & testFile1
local fullTestFile2Name = testDir2 & "/" & testFile2
local fullTestFile3Name = testDir2 & "/" & testFile3

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
;; CPU1 is the default
hkPktId = "p08A"

if ("$CPU" = "CPU2") then
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
write ";  Step 2.1: Send Create Directory Command to Create Test Directory 1."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir1

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Create Directory command sent properly."
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
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
write ";  Step 2.2: Send Create Directory Command to Create Test Directory 2."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir2

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (3000) - Create Directory command sent properly."
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
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
write ";  Step 2.3: Upload Test File to test Directory 1."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile1, testFile1, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0:  File Rename Test."
write ";*********************************************************************"
write ";  Step 3.1:  Perform File Info Command to Verify Test File 1 exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile1Name,"' exists"
  else
    write "<!> Failed - File Info Telemetry indicates that '",fullTestFile1Name,"' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
    write "<*> Passed - File Info Telemetry Matches expected file: ",fullTestFile1Name
  else
    write "<!> Failed - File Info Telemetry does not match expected file: ",fullTestFile1Name
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2:  File Info Command to Verify Test File 2 Does Not Exist."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile2Name,"' does not exist"
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
write ";  Step 3.3: Perform Rename File Command to Rename File 1 to File 2"
write ";  specifying a Destination File Name & Directory different than"
write ";  File 1's  Name & Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileRename File=fullTestFile1Name DestName=fullTestFile2Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2005) - File Rename command sent properly."
  ut_setrequirements FM_2005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_RENAME_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2005) - File Rename command not sent properly (", UT_TW_status, ")."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: File Info Command to Verify Test File 1 No Longer Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile1Name,"' does not exist"
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
write ";  Step 3.5:  File Info Command to Verify Test File 2 Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile2Name)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile2Name,"' exists"
  else
    write "<!> Failed - File Info Telemetry indicates that '",fullTestFile2Name,"' does not exist"
  endif

  if ( $SC_$CPU_FM_InfoFileName = fullTestFile2Name ) THEN
    write "<*> Passed (2005) - File Info Telemetry Matches expected file: " & fullTestFile2Name
    ut_setrequirements FM_2005, "P"
  else
    write "<!> Failed (2005) - File Info Telemetry does not match expected file: " & fullTestFile2Name
    ut_setrequirements FM_2005, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: File Info Command to Verify Test File 3 Does Not Exist."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile3Name,"' does not exist"
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
write ";  Step 3.7: Perform Rename File Command to Rename File 2 to File 3"
write ";  specifying a Destination File Name different than File 2's"
write ";  Name but in the same Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileRename File=fullTestFile2Name DestName=fullTestFile3Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2005) - File Rename command sent properly."
  ut_setrequirements FM_2005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_RENAME_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2005) - File Rename command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.8: File Info Command to Verify Test File 2 No Longer Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile2Name,"' does not exist"
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
write ";  Step 3.9:  File Info Command to Verify Test File 3 Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File Info Telemetry indicates that '",fullTestFile3Name,"' exists"
  else
    write "<!> Failed - File Info Telemetry indicates that '",fullTestFile3Name,"' does not exist"
  endif

  if ( $SC_$CPU_FM_InfoFileName = fullTestFile3Name ) THEN
    write "<*> Passed (2005) - File Info Telemetry Matches expected file: " & fullTestFile3Name
    ut_setrequirements FM_2005, "P"
  else
    write "<!> Failed (2005) - File Info Telemetry does not match expected file: " & fullTestFile3Name
    ut_setrequirements FM_2005, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.10:  Test Renaming an open file"
write ";*********************************************************************"
write ";  Step 3.10.1:  Open Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

local expOpenFiles = $SC_$CPU_FM_NumOpen + 1
cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

/$SC_$CPU_TST_FM_Open File=fullTestFile3Name

ut_tlmwait $SC_$CPU_TST_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - File Open command sent properly"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Expected event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_FM_OPENFILE_INF_EID, "."
  endif
else
  write "<!> Failed - Did not Open file '", fullTestFile3Name, "'"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.10.2:  Verify File is open."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

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
    write "<*> Passed - HK Telemetry Reports number of open files correctly: " & $SC_$CPU_FM_NumOpen
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
write ";  Step 3.10.3: Perform Rename File Command attempting to Rename a"
write ";  Source File that is open"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileRename File=fullTestFile3Name DestName=testDir2 & "/openSrc.tst"

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2005) - Rename Command with an open source file accepted."
  ut_setrequirements FM_2005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2005) - Rename Command of an open file rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.10.4:  File Info Command to Verify Dest File Exists "
write ";  in Destination Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

local openFileDestName = testDir2 & "/openSrc.tst"
cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (openFileDestName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ;;Check the status of the file
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File Info Telemetry indicates that '",openFileDestName,"' exists"
  else
    write "<!> Failed - File Info Telemetry indicates that '",openFileDestName,"' does not exist"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.11: Perform Rename File Command attempting to Rename a"
write ";  Source File to an existing Destination file."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_TGT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileRename File=openFileDestName DestName=openFileDestName

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2005.1) - Rename Command Rejected Counter incremented as expected."
  ut_setrequirements FM_20051, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Expected event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_RENAME_TGT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2005.1) - Rename Command to an existing destination file passed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_20051, "F"
endif

wait 5

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

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_filerename_basic   "
write ";*********************************************************************"
ENDPROC
