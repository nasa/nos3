PROC $sc_$cpu_fm_filecopy_basic
;*******************************************************************************
;  Test Name:  FM_FileCopy_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM) File Copy
;   Commands function properly. The FM_FileCopy and FM_FileInfo commands will
;   be tested to see if the FM application handles these as desired by the
;   requirements. The FM_DirCreate command is also used to facilitate the
;   testing.
;
;  Requirements Tested
;   FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;   FM1004	If FM rejects any command, FM shall abort the command
;		execution, increment the FM Command Rejected Counter and
;		issue an error event message
;   FM1005	If the filename specified in any FM command is not valid,
;		FM shall reject the command
;   FM2002	Upon receipt of a File Copy command, FM shall copy the
;		command-specified file to the command-specified destination file
;   FM2002.1	If the command-specified destination file exists, FM shall
;		reject the command
;   FM2011	Upon receipt of a File Info command, FM shall generate a message
;		containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open or Closed),
;			d) <MISSION_DEFINED> CRC
;   FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified filesystem.
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
;				2002.1 & 2002.2.
;	01/26/10   W. Moleski	Updated for FM 2.1.0.0 which removed 2002.2
;       02/28/11   W. Moleski   Added variables for App name and ram directory
;       08/18/11   W. Moleski   Added the Overwrite argument to the Copy cmds
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
#define FM_2002     3
#define FM_20021    4
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

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_1005", "FM_2002", "FM_2002.1", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDirSource = ramDir & "/FMSOURCE"
local testDirDest = ramDir & "/FMDEST"

local testFile = "FMCOPY.TST"
local testFileCopy2 = "FMCOPYDEST.TST"
local uploadDir = ramDirPhys & "/FMSOURCE"

local fullOriginalTestFileName = testDirSource & "/" & testFile
local fullTestFileCopy1Name = testDirDest & "/" & testFile
local fullTestFileCopy2Name = testDirDest & "/" & testFileCopy2

local fileSize1
local fileSize2

local crc1
local crc2

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
write ";  Step 2.1: Send Create Directory Command to Create Source Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$sc_$cpu_FM_DirCreate DirName=testDirSource

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Send Create Directory Command to Create Dest Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$sc_$cpu_FM_DirCreate DirName=testDirDest

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Upload Test File to Source Directory."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  File Copy Test - Copy File To Directory."
write ";*********************************************************************"
write ";  Step 3.1:  Perform File Info Command to Verify Source File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;2011) - File Info Command Completed."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received"
    ut_setrequirements FM_1003, "F"
  endif

  ;; Need to check the File Status in order to verify the file exists
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullOriginalTestFileName,"' does not exist"
  endif

  if ($sc_$cpu_FM_InfoFileName = fullOriginalTestFileName) THEN
    write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2:  File Info Command to Verify Dest File Does Not Exist in"
write ";  the Destination Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileCopy1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;2011) - File Info command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1003) - Event message ",FM_GET_FILE_INFO_CMD_EID," NOT received"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info command incremented the CMDEC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Perform Copy File Command to Copy Source File to"
write ";  Destination Directory. This should be rejected since the destination"
write ";  is a directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_TGT_ERR_EID, "ERROR", 2

errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName= testDirDest & "/"

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1005;2002) - File Copy command failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_TGT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif

else
  write "<!> Failed (1004;1005;2002) - File Copy command succeeded when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: File Info Command to Verify Source File Still Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;2011) - File Info Command Completed."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  ;; Need to check the File Status in order to verify the file exists
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullOriginalTestFileName,"' does not exist"
  endif

  if ($sc_$cpu_FM_InfoFileName = fullOriginalTestFileName) THEN
    write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "F"
  endif

  fileSize1 = $sc_$cpu_FM_InfoFileSize
  crc1 = $sc_$cpu_FM_CRC
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"

  fileSize1 = "cmd failed"
  crc1 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  File Copy Test - Copy File To File."
write ";*********************************************************************"
write ";  Step 4.1:  File Info Command to Verify Dest File Does Not Exist"
write ";  in the Destination Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileCopy2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;2011) - File Info command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1003) - Event message ",FM_GET_FILE_INFO_CMD_EID," NOT received"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info command incremented the CMDEC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Perform Copy File Command to Copy Source File to"
write ";  Destination Directory specifying a Destination FileName different  "
write ";  than the Source File Name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=fullTestFileCopy2Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002) - File Copy command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - File Copy command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: File Info Command to Verify Source File Still Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;2011) - File Info Command Completed."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  ;; Need to check the File Status in order to verify the file exists
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullOriginalTestFileName,"' does not exist"
  endif

  if ($sc_$cpu_FM_InfoFileName = fullOriginalTestFileName) THEN
    write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "F"
  endif

  fileSize1 = $sc_$cpu_FM_InfoFileSize
  crc1 = $sc_$cpu_FM_CRC
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"

  fileSize2 = "cmd failed"
  crc2 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4:  Perform File Info Command to Verify Destination File"
write ";  exists in Destination Directory with the new name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileCopy2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullTestFileCopy2Name,"' does not exist"
  endif

  if ($sc_$cpu_FM_InfoFileName = fullTestFileCopy2Name) THEN
    write "<*> Passed (2002;2011) - File Info Telemetry Matches expected file: ", fullTestFileCopy2Name
    ut_setrequirements FM_2002, "P"
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2002;2011) - File Info Telemetry does not match expected file: ", fullTestFileCopy2Name
    ut_setrequirements FM_2002, "F"
    ut_setrequirements FM_2011, "F"
  endif

  fileSize2 = $sc_$cpu_FM_InfoFileSize
  crc2 = $sc_$cpu_FM_CRC
else
  write "<!> Failed (1003;2002;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
  ut_setrequirements FM_2011, "F"

  fileSize2 = "cmd failed"
  crc2 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5:  Verify files size and checksum of the copied file"
write ";  matches original."
write ";*********************************************************************"
if (fileSize1 = fileSize2) THEN
  write "<*> Passed (2002) - File Sizes Match."
  ut_setrequirements FM_2002, "P"
else
  write "<!> Failed (2002) - File Sizes do NOT Match."
  ut_setrequirements FM_2002, "F"
endif

write "  File Size of Original = ", fileSize1
write "  File Size of Copy 2   = ", fileSize2

if (crc1 = crc2) THEN
  write "<*> Passed (2002) - CRCs Match."
  ut_setrequirements FM_2002, "P"
else
  write "<!> Failed (2002) - CRCs do NOT Match."
  ut_setrequirements FM_2002, "F"
endif

write "  CRC of Original = ", crc1
write "  CRC of Copy 2   = ", crc2

wait 5

write ";*********************************************************************"
write ";  Step 4.6:  Test that copying an open file is blocked."
write ";*********************************************************************"
write ";  Step 4.6.1:  Open Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

local filesOpen = $sc_$cpu_FM_NumOpen + 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1
errcnt = $SC_$CPU_TST_FM_CMDEC + 1

/$sc_$cpu_TST_FM_Open File=fullOriginalTestFileName

wait until (($SC_$CPU_TST_FM_CMDPC = cmdctr) OR ($SC_$CPU_TST_FM_CMDEC = errcnt))   
if ($SC_$CPU_TST_FM_CMDPC = cmdctr) then
  write "<*> Passed - File reported Open by test app."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_OPENFILE_INF_EID, "."
  endif
else
  write "<!> Failed - File Open command not sent properly to test app."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6.2:  Verify File is open."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - File Info Command Completed."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Open") THEN
    write "<*> Passed - File status indicates that the file is OPEN."
  else
    write "<!> Failed - Expected file status 'Open'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed - File Info Command Failed."
endif

if ($sc_$cpu_FM_NumOpen = filesOpen) THEN
  write "<*> Passed - HK Telemetry Reports number of open files correctly: ", $sc_$cpu_FM_NumOpen
else
  write "<!> Failed - The number of open files is incorrect. Expected ",filesOpen," TLM reports ",$sc_$cpu_FM_NumOpen
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6.3: Perform Copy File Command attempting to Copy a Source"
write ";  File that is open. This should succeed. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName= testDirDest & "/openSrc.tst"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))   
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2002) - CMDPC incremented as expected."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received"
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - Copying an open file incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6.4:  File Info Command to Verify Dest File Exists in the "
write ";  Destination Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (testDirDest & "/openSrc.tst")

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received"
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file does exist."
  else
    write "<!> Failed - Status indicates the file does not exist."
  endif
else
  write "<!> Failed (1003;2011) - File Info command incremented the CMDEC"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6.5: Close the source file"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

/$sc_$cpu_TST_FM_Close File=fullOriginalTestFileName

ut_tlmwait  $SC_$CPU_TST_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Close File command for '", fullOriginalTestFileName, "' sent properly."
else
  write "<!> Failed - Close File command for '", fullOriginalTestFileName, "'. CMDPC = ",$SC_$CPU_TST_FM_CMDPC,"; Expected ",cmdctr
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Send the File Copy command specifying an existing "
write ";  destination file. Verify that the command is rejected.     "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_TGT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=fullTestFileCopy2Name

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}  
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2002.1) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_20021, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Expected Event message ", $SC_$CPU_find_event[1].eventid, " received."
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_TGT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2002.1) - Copy File Command succeeded when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_20021, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.8: Send the File Copy command specifying an existing "
write ";  destination file with the Overwite flag set. Verify that the command"
write ";  is successful. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$sc_$cpu_FM_FileCopy Overwrite=1 File=fullOriginalTestFileName DestName=fullTestFileCopy2Name

ut_tlmwait $SC_$CPU_FM_CMDPC, {cmdCtr}  
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2002.1) - Copy Command with overwrite flag set was sent successfully."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_20021, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Expected Event message ", $SC_$CPU_find_event[1].eventid, " received."
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_TGT_ERR_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002.1) - Copy Command with overwrite flag set did not increment CMDPC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_20021, "F"
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
write ";  End procedure $SC_$CPU_fm_filecopy_basic                                    "
write ";*********************************************************************"
ENDPROC
