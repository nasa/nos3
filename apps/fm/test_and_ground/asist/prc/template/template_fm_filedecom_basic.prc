PROC $sc_$cpu_fm_filedecom_basic
;*******************************************************************************
;  Test Name:  FM_FileDecom_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM) File
;   Decompress Command functions properly. The FM_FileDecompress and FM_FileInfo
;   commands will be tested to see if the FM application handles these as
;   desired by the requirements.
;
;  Requirements Tested
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command
;		execution, increment the FM Command Rejected Counter and
;		issue an error event message
;    FM2009	Upon receipt of a Decompress command, FM shall decompress the
;		command-specified file to the command-specified file.
;    FM2009.1	If the command-specified destination file exists, FM shall
;		reject the command.
;    FM2011	Upon receipt of a File Info command, FM shall generate a message
;		containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open, Closed, or Nonexistent)
;			d) <MISSION_DEFINED> CRC
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
;
;  Prerequisite Conditions
;    The cFE & FM are up and running and ready to accept commands. 
;    The FM commands and TLM items exist in the GSE database. 
;    The display pages for the FM Telemetry exist. 
;
;  Assumptions and Constraints
;    All compressed files have been compressed using the gzip utility on the
;       ground system.
;    The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	05/08/08   D. Stewart	Original Procedure
;	12/08/08   W. Moleski	Added requirement 2009.1 and general cleanup
;	01/27/10   W. Moleski	Updating for FM 2.1.0.0
;       02/28/11   W. Moleski   Added variables for App name and ram directory
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
#define FM_2009     2
#define FM_20091    3
#define FM_2011     4
#define FM_4000     5
#define FM_5000     6

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 6
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_2009", "FM_2009.1", "FM_2011", "FM_4000", "FM_5000"]

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir1 = ramDir & "/FMTEST1"
local testDir2 = ramDir & "/FMTEST2"

local uploadDir = ramDirPhys & "/FMTEST1"

local testFileCompressed   = "FMDECOMORIG.TST.gz"
local testFileUncompressed = "FMDECOM.TST"
local fullOriginalTestFileName  = testDir1 & "/" & testFileCompressed
local fullTestFileUncompressed1 = testDir1 & "/" & testFileUncompressed
local fullTestFileUncompressed2 = testDir2 & "/" & testFileUncompressed

local fileSize1
local fileSize2

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
write ";  Step 2.1: Create Test Directory 1."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir1

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
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
write ";  Step 2.2: Create Test Directory 2."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir2

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
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
s ftp_file (uploadDir, testFileCompressed, testFileCompressed, "$CPU","P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0:  File Decompress Test - Decompress File to Same Directory"
write ";*********************************************************************"
write ";  Step 3.1:  File Info Command to Verify Source File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullOriginalTestFileName,"' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullOriginalTestFileName) THEN
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
write ";  Step 3.2:  File Info Command to Verify Destination File does not"
write ";  exist in Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileUncompressed1)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Perform FM_FileDecompress Command to Decompress Source"
write ";  File to Destination File in the same directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_Decompress File=fullOriginalTestFileName DestName=fullTestFileUncompressed1

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2009) - File Decompress command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2009, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DECOM_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2009) - File Decompress command not sent properly (", UT_TW_status, ")."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: File Info Command to Verify Source File Still Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullOriginalTestFileName,"' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullOriginalTestFileName) THEN
    write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2011, "F"
  endif

  fileSize1 = $SC_$CPU_FM_InfoFileSize
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"

  fileSize1 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5:  File Info Command to Verify Destination File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileUncompressed1)

if ( cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullTestFileUncompressed1,"' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullTestFileUncompressed1) THEN
    write "<*> Passed (2009;2011) - File Info Telemetry Matches expected file: ", fullTestFileUncompressed1
    ut_setrequirements FM_2009, "P"
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2009;2011) - File Info Telemetry does not match expected file: ", fullTestFileUncompressed1
    ut_setrequirements FM_2009, "F"
    ut_setrequirements FM_2011, "F"
  endif

  fileSize2 = $SC_$CPU_FM_InfoFileSize
else
  write "<!> Failed (1003;2009;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2009, "F"
  ut_setrequirements FM_2011, "F"

  fileSize2 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6:  Verify that the decompressed file is larger than the"
write ";  compressed file."
write ";*********************************************************************"
if (fileSize1 < fileSize2) THEN
  write "<*> Passed (2009) - Decompressed file larger than Compressed File."
  ut_setrequirements FM_2009, "P"
else
  write "<!> Failed (2009) - Decompressed file NOT larger than Compressed File."
  ut_setrequirements FM_2009, "F"
endif

write "  File Size of Compressed   = ", fileSize1
write "  File Size of Decompressed = ", fileSize2

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  File Decompress Test - Decompress File to Different"
write ";  Directory."
write ";*********************************************************************"
write ";  Step 4.1:  File Info Command to Verify Destination File does not"
write ";  exist in Destination Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileUncompressed2)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Perform FM_FileDecompress Command to Decompress Source"
write ";  File to Destination File in a different directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_Decompress File=fullOriginalTestFileName DestName=fullTestFileUncompressed2

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2009) - File Decompress command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2009, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DECOM_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2009) - File Decompress command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: File Info Command to Verify Source File Still Exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOriginalTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullOriginalTestFileName,"' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullOriginalTestFileName) THEN
    write "<*> Passed (2009;2011) - File Info Telemetry Matches expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2009, "P"
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2009;2011) - File Info Telemetry does not match expected file: ", fullOriginalTestFileName
    ut_setrequirements FM_2009, "F"
    ut_setrequirements FM_2011, "F"
  endif

  fileSize1 = $SC_$CPU_FM_InfoFileSize
else
  write "<!> Failed (1003;2009;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2009, "F"
  ut_setrequirements FM_2011, "F"

  fileSize1 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4:  File Info Command to Verify Destination File exists"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileUncompressed2)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2009;2011) - File Info Command Completed."
  ut_setrequirements FM_2009, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullTestFileUncompressed2,"' does not exist"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullTestFileUncompressed2) THEN
    write "<*> Passed (2009;2011) - File Info Telemetry Matches expected file: ", fullTestFileUncompressed2
    ut_setrequirements FM_2009, "P"
    ut_setrequirements FM_2011, "P"
  else
    write "<!> Failed (2009;2011) - File Info Telemetry does not match expected file: ", fullTestFileUncompressed2
    ut_setrequirements FM_2009, "F"
    ut_setrequirements FM_2011, "F"
  endif

  fileSize2 = $SC_$CPU_FM_InfoFileSize
else
  write "<!> Failed (1003;2009;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2009, "F"
  ut_setrequirements FM_2011, "F"

  fileSize2 = "cmd failed"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5:  Verify that the decompressed file is larger than the"
write ";  compressed file."
write ";*********************************************************************"
if (fileSize1 < fileSize2) THEN
  write "<*> Passed (2009) - Decompressed file larger than Compressed File."
  ut_setrequirements FM_2009, "P"
else
  write "<!> Failed (2009) - Decompressed file NOT larger than Compressed File."
  ut_setrequirements FM_2009, "F"
endif

write "  File Size of Compressed   = ", fileSize1
write "  File Size of Decompressed = ", fileSize2

wait 5

write ";*********************************************************************"
write ";  Step 4.6: Test that Decompress an open file is blocked."
write ";*********************************************************************"
write ";  Step 4.6.1: Open Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

local expOpenFiles = $SC_$CPU_FM_NumOpen + 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1
errcnt = $SC_$CPU_TST_FM_CMDEC + 1

/$SC_$CPU_TST_FM_Open File =fullOriginalTestFileName

wait until (($SC_$CPU_TST_FM_CMDPC = cmdctr) OR ($SC_$CPU_TST_FM_CMDEC = errcnt))
if ($SC_$CPU_TST_FM_CMDPC = cmdctr) then
  write "<*> Passed - File reported Open by test app."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "  Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "  Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_OPENFILE_INF_EID, "."
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
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Open") THEN
    write "<*> Passed - File status indicates that the file is open."
  else
    write "<!> Failed - Expected file status 'Open'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_2009, "F"
  ut_setrequirements FM_2011, "F"
endif

;; Check the Open File Counter
if ($SC_$CPU_FM_NumOpen = expOpenFiles) THEN
  write "<*> Passed - The number of open files is correct: ",$SC_$CPU_FM_NumOpen
else
  write "<!> Failed - Expected ",expOpenFiles, " open files. TLM reports ",$SC_$CPU_FM_NumOpen
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6.3: Perform Decompress File Command attempting to Decompress"
write ";  a Source File that is open. This should fail."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Decompress File=fullOriginalTestFileName DestName=testDir2 & "/openSrc.tst"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;2009) - Decompress Command accepted with an open file."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_2009, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Expected event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Received EM ", $SC_$CPU_find_event[1].eventid, "; Expected ",FM_DECOM_SRC_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2009) - Decompress Command accepted with an open source file."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_2009, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.6.4: Close the Source file"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1
errcnt = $SC_$CPU_TST_FM_CMDEC + 1

/$SC_$CPU_TST_FM_Close File=fullOriginalTestFileName

wait until (($SC_$CPU_TST_FM_CMDPC = cmdctr) OR ($SC_$CPU_TST_FM_CMDEC = errcnt))
if ($SC_$CPU_TST_FM_CMDPC = cmdctr) then
  write "<*> Passed - File Closed by test app."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "  Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "  Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_CLOSEFILE_INF_EID, "."
  endif
else
  write "<!> Failed - File Close command not sent properly to test app."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.7: Perform FM_FileDecompress Command to an existing "
write ";  Destination File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_TGT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Decompress File=fullOriginalTestFileName DestName=fullTestFileUncompressed1

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2009.1) - File Decompress command failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_20091, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DECOM_TGT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2009.1) - File Decompress command succeeded when failure was expected"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_20091, "F"
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

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_filedecom_basic                          "
write ";*********************************************************************"
ENDPROC
