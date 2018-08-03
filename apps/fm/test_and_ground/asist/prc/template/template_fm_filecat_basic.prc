PROC $sc_$cpu_fm_filecat_basic
;*******************************************************************************
;  Test Name:  FM_FileCat_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM) File
;   Concatenate Command functions properly. The FM_FileCat and FM_FileInfo
;   commands will be tested to see if the FM application handles these as
;   desired by the requirements.
;
;  Requirements Tested
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command execution,
;		increment the FM Command Rejected Counter and issue an error
;		event message
;    FM2010	Upon receipt of a Concatenate command, FM shall concatenate the
;		command-specified file with the second command-specified file,
;		copying the result to the command-specified destination file.
;    FM2010.1	If the command-specified destination file exists, FM shall
;		reject the command
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
;    The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	05/08/08   D. Stewart	Original Procedure
;	12/09/08   W. Moleski	Added Requirement 2010.1 and general cleanup
;	01/20/10   W. Moleski	Updated for FM 2.1.0.0 and more cleanup
;       02/28/11   W. Moleski   Added variables for App name and ram directory
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG
;
;  Arguments
;	None
;
;  Procedures Called
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
#define FM_2010     2
#define FM_20101    3
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

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_2010", "FM_2010.1", "FM_2011", "FM_4000", "FM_5000"]

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir1 = ramDir & "/FMTEST1"
local testDir2 = ramDir & "/FMTEST2"
local testDir3 = ramDir & "/FMTEST3"

local testFile1 = "FMCAT1.TST"
local testFile2 = "FMCAT2.TST"
local testFile3 = "FMCAT3.TST"

local outputFile1 = "FMCATOUT1.TST"
local outputFile2 = "FMCATOUT2.TST"
local outputFile3 = "FMCATOUT3.TST"
local outputFile4 = "FMCATOUT4.TST"
local outputFile5 = "FMCATOUT5.TST"

local uploadDir1 = ramDirPhys & "/FMTEST1"
local uploadDir2 = ramDirPhys & "/FMTEST2"

local fullTestFile1Name = testDir1 & "/" & testFile1
local fullTestFile2Name = testDir1 & "/" & testFile2
local fullTestFile3Name = testDir2 & "/" & testFile3

local fullOutputFile1Name = testDir1 & "/" & outputFile1
local fullOutputFile2Name = testDir3 & "/" & outputFile2
local fullOutputFile3Name = testDir1 & "/" & outputFile3
local fullOutputFile4Name = testDir2 & "/" & outputFile4
local fullOutputFile5Name = testDir3 & "/" & outputFile5

local cmdctr
local errcnt

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
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
write ";  Step 1.2: Create the FreeSpace table load image and start the "
write ";  File Manager (FM) and Test (TST_FM) Applications. "
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
write ";  Step 2.2: Send Create Directory Command to Create Test Directory 2."
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
write ";  Step 2.3: Send Create Directory Command to Create Test Directory 3."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir3

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
write ";  Step 2.4: Upload Test Files."
write ";*********************************************************************"
;; Upload the Test files
s ftp_file (uploadDir1, testFile1, testFile1, "$CPU", "P")
s ftp_file (uploadDir1, testFile2, testFile2, "$CPU", "P")
s ftp_file (uploadDir2, testFile3, testFile3, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  File Concatenate Test 1 - Concatenate Files in same"
write ";  directory to Same Directory."
write ";*********************************************************************"
write ";  Step 3.1:  File Info Command to Verify Source File 1 exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2:  File Info Command to Verify Source File 2 exists."
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
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile2Name) THEN
  write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullTestFile2Name
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullTestFile2Name
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3:  File Info Command to Verify Destination File does not"
write ";  exist in Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Perform FM_FileCat Command to Concatenate Source File 2"
write ";  to Source File 1 and place the result in the Destination"
write ";  File in the same directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID , "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName=fullOutputFile1Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2010) - File Cat command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - File Cat command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID , "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: File Info Command to Verify Source File 1 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: File Info Command to Verify Source File 2 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile2Name)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ( $SC_$CPU_FM_InfoFileName = fullTestFile2Name ) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile2Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile2Name
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7:  File Info Command to Verify Destination File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ( $SC_$CPU_FM_InfoFileName = fullOutputFile1Name ) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullOutputFile1Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullOutputFile1Name
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0:  File Concatenate Test 2 - Concatenate Files in same"
write ";  directory to Different Directory."
write ";*********************************************************************"
write ";  Step 4.1:  File Info Command to Verify Destination File does not"
write ";  exist in Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Perform FM_FileCat Command to Concatenate Source File 2"
write ";  to Source File 1 and place the result in the Destination"
write ";  File in a different directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID , "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName=fullOutputFile2Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2010) - File Cat command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - File Cat command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID , "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: File Info Command to Verify Source File 1 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4: File Info Command to Verify Source File 2 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile2Name)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile2Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile2Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile2Name
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.5: File Info Command to Verify Destination File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile2Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullOutputFile2Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullOutputFile2Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullOutputFile2Name
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0:  File Concatenate Test 3 - Concatenate Files in different"
write ";  directories to the same Directory as File 1."
write ";*********************************************************************"
write ";  Step 5.1:  File Info Command to Verify Source File 1 exists in the"
write ";  first directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

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
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.2:  File Info Command to Verify Source File 2 exists in the"
write ";  second directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

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
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile3Name) THEN
  write "<*> Passed (2011) - File Info Telemetry Matches expected file: ", fullTestFile3Name
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2011) - File Info Telemetry does not match expected file: ", fullTestFile3Name
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3:  File Info Command to Verify Destination File does not"
write ";  exist in the first directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4: Perform FM_FileCat Command to Concatenate Source File 3"
write ";  to Source File 1 and place the result in the Destination"
write ";  File in the same directory as Source File 1."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID , "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile3Name DestName=fullOutputFile3Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2010) - File Cat command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - File Cat command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID , "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5: File Info Command to Verify Source File 1 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6: File Info Command to Verify Source File 3 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile3Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile3Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile3Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.7:  File Info Command to Verify Destination File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullOutputFile3Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullOutputFile3Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullOutputFile3Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.0:  File Concatenate Test 4 - Concatenate Files in different"
write ";  directories to the same Directory as File 3."
write ";*********************************************************************"
write ";  Step 6.1:  File Info Command to Verify Destination File does not"
write ";  exist in the second directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile4Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2: Perform FM_FileCat Command to Concatenate Source File 3"
write ";  to Source File 1 and place the result in the Destination"
write ";  File in the same directory as Source File 3."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID , "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile3Name DestName=fullOutputFile4Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2010) - File Cat command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - File Cat command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID , "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3: File Info Command to Verify Source File 1 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.4: File Info Command to Verify Source File 3 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile3Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile3Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile3Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.5:  File Info Command to Verify Destination File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile4Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullOutputFile4Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullOutputFile4Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullOutputFile4Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.0:  File Concatenate Test 5 - Concatenate Files in different"
write ";  directories to a 3rd directory."
write ";*********************************************************************"
write ";  Step 7.1:  File Info Command to Verify Destination File does not"
write ";  exist in the third directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile5Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File status indicates that the file does not exist."
  else
    write "<!> Failed - Expected file status 'Not Used'. Rcv'd '", p@$SC_$CPU_FM_FileStatus, "'"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.2: Perform FM_FileCat Command to Concatenate Source File 3"
write ";  to Source File 1 and place the result in the Destination"
write ";  File in a third directory different from the source files"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID , "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile3Name DestName=fullOutputFile5Name

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2010) - File Cat command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - File Cat command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID , "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.3: File Info Command to Verify Source File 1 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile1Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile1Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile1Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.4: File Info Command to Verify Source File 3 still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile3Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullTestFile3Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullTestFile3Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullTestFile3Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.5:  File Info Command to Verify Destination File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullOutputFile5Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2010;2011) - File Info Command Completed."
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
  ut_setrequirements FM_2011, "F"
endif

if ($SC_$CPU_FM_InfoFileName = fullOutputFile5Name) THEN
  write "<*> Passed (2010;2011) - File Info Telemetry Matches expected file: ", fullOutputFile5Name
  ut_setrequirements FM_2010, "P"
  ut_setrequirements FM_2011, "P"
else
  write "<!> Failed (2010;2011) - File Info Telemetry does not match expected file: ", fullOutputFile5Name
  ut_setrequirements FM_2011, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.6:  Test that Concatenating open files is blocked."
write ";*********************************************************************"
write ";  Step 7.6.1:  Open Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1
errcnt = $SC_$CPU_TST_FM_CMDEC + 1

/$SC_$cpu_TST_FM_Open File=fullTestFile1Name

wait until (($SC_$CPU_TST_FM_CMDPC = cmdctr) OR ($SC_$CPU_TST_FM_CMDEC = errcnt))
if ($SC_$CPU_TST_FM_CMDPC = cmdctr) then
  write "  File reported Open by test app."
else
  write "  File Open command not sent properly to test app."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "  Event message ",$SC_$CPU_find_event[1].eventid, " received"
else
  write "  Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_FM_OPENFILE_INF_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.6.2:  Verify File is open."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_FILE_INFO_SRC_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed - File Info Command Completed."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid, " Either the wrong EM is expected or the wrong one was received."
  endif
else
  write "<!> - File Info Command Failed."
endif

if (P@$SC_$CPU_FM_FileStatus = "Open" ) THEN
  write "<*> - File Info Telemetry indicates File is open"
else
  write "<!> - File Info Telemetry does not indicate File is open"
endif

if ($SC_$CPU_FM_NumOpen = 1) THEN
  write "<*> Passed - HK Telemetry Reports number of open files correctly: ", $SC_$CPU_FM_NumOpen
else
  write "<!> Failed - HK Telemetry Reports number of open files incorrectly: ", $SC_$CPU_FM_NumOpen
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.6.3: Perform Concatenate File Command attempting to"
write ";  Concatenate Source Files that are open. There is no requirement "
write ";  stating that you cannot concatenate open files. This succeeds. "
write ";*********************************************************************"
write ";  Step 7.6.3.1: Src File 1"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC1_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name, File2=fullTestFile2Name, DestName= testDir3 & "/openSrc1.tst"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1003;2010) - File Cat Command rejected with an open file as source file 1"
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive the expected EM. Rcvd ", $SC_$CPU_find_event[1].eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010) - File Cat Command accepted with an open file. Failure was expected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.6.3.2: Src File 2"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC2_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile2Name, File2=fullTestFile1Name, DestName= testDir3 & "/openSrc2.tst"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1003;2010) - File Cat Command accepted with an open file."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive the expected EM. Rcvd ", $SC_$CPU_find_event[1].eventid, " expected ",FM_CONCAT_SRC2_ERR_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2010) - File Cat Command failed with an open file."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.7:  Test Concatenating files to an existing destination file"
write ";  is blocked."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_TGT_ERR_EID , "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile2Name File2=fullTestFile3Name DestName=fullOutputFile5Name

ut_tlmwait  $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;2010.1) - File Cat command to an existing destination failed as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_20101, "P"
else
  write "<!> Failed (1004;2010.1) - File Cat command successful to an existing destination file."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_20101, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_TGT_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
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

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_filecat_basic                            "
write ";*********************************************************************"
ENDPROC
