PROC $sc_$cpu_fm_dircmds_basic
;*******************************************************************************
;  Test Name:  FM_DirCmds_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;    The purpose of this test is to verify that the File Manager (FM) Directory
;    Commands function properly. The FM_DirCreate, FM_DirDelete, FM_DirListFile,
;    and FM_DirListTlm commands will be tested to see if the FM application
;    handles these as desired by the requirements. The FM_delete command is
;    also used to facilitate the testing.
;
;  Requirements Tested
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command  Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command
;		execution, increment the FM Command Rejected Counter and
;		issue an error event message
;    FM1006	If the directory specified in any FM command is not valid,
;		FM shall reject the command
;    FM2008	Upon receipt of a Delete command, FM shall delete the
;		command-specified file.
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified filesystem.
;    FM3001	Upon receipt of a Delete command, FM shall remove the
;		command-specified directory from the command-specified
;		filesystem.
;    FM3001.1	If the specified directory contains at least one file or
;		subdirectory, the command shall be rejected
;    FM3002	Upon receipt of a Directory Listing To File command, FM shall
;		write the contents of the command-specified directory on any of
;		the on-board file systems to the command-specified file. The
;		following shall be written:
;			a) Directory name 
;			b) file size in bytes of each file
;			c) last modification time of each file
;			d) Filename of each file
;    FM3002.1	FM shall issue an event message that reports:
;			a) The number of filenames written to the specified file
;			b) The total number of files in the directory 
;			c) The command-specified file's filename
;    FM3002.2	FM shall use the <PLATFORM_DEFINED> default filename if a file
;		is not specified
;    FM3003	Upon receipt of a Directory Listing command, FM shall generate
;		a message containing the following for up to <PLATFORM_DEFINED>
;		consecutive files starting at the command specified offset:
;			a) Directory name 
;			b) file size in bytes of each file
;			c) last modification time of each file
;			d) Filename of each file
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
;    The display page for the FM Housekeeping exists. 
;
;  Assumptions and Constraints
;    None.
;
;  Change History
;
;	Date	   Name		Description
;	04/16/08   D. Stewart	Original Procedure
;	12/08/08   W. Moleski   Added Requirement and did some general cleanup
;	01/13/10   W. Moleski   Updated for FM 2.1.0.0 and did some more cleanup
;	02/25/11   W. Moleski   Added variables for the App Name and ram dir
;	01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG
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

%liv (log_procedure) = logging

#define FM_1003     0
#define FM_1004     1
#define FM_1006     2
#define FM_2008     3
#define FM_3000     4
#define FM_3001     5
#define FM_3001_1   6
#define FM_3002     7
#define FM_3002_1   8
#define FM_3002_2   9
#define FM_3003     10
#define FM_4000     11
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

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_1006", "FM_2008", "FM_3000", "FM_3001", "FM_3001.1", "FM_3002", "FM_3002.1", "FM_3002.2", "FM_3003", "FM_4000", "FM_5000"]

local rawcmd

local forwardSlash = "/"

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local testDir = ramDir & "/FMTEST"
local testDirName = "FMTEST"
local listDir = ramDir
local parentDir = ramDir
local listFilePre  = "fmtestlist"
local listFileExt = ".lst"

local testFile = "FMDIRFUNC.TST"
local uploadDir = "RAM:0/FMTEST"
local downloadDir = "RAM:0"

local fullTestFileName = testDir & forwardSlash & testFile

local cmdctr
local errcnt

local filenum = 1

local currentDis

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
write ";  Step 2.0: File System Directory Functions Test"
write ";*********************************************************************"
write ";  Step 2.1: Verify the directory does not already exist."
write ";*********************************************************************"
write ";  Step 2.1.1: Perform directory listing to file commands."
write ";*********************************************************************"
write ";  Step 2.1.1.1: Listing to file command: List parent directory"
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (parentDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt,"Pass")
     
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3002, "P"

  ;; This event is setup in the dirfiledisplay proc invoked above
  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif
else
  write "<!> Failed (1003;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3002, "F"
  ut_setrequirements FM_3002_1, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.1.1.2: Listing to file command: List target (missing) dir"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_FILE_SRC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (testDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt,"Fail")

if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1006) - Dir List to File command rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[2].eventid, " received "
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", FM_GET_DIR_FILE_OS_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006) - Dir List to File command completed, suggests Directory " & testDir & " exists."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.1.1.3: Listing to file command: missing filename specification"
write ";  This command should pass and use the default file name. "
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (parentDir, "", downloadDir, listFilePre & filenum & listFileExt)
     
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3002;3002.1;3002.2) - Dir List to File command using default file."
  ut_setrequirements FM_3002, "P"
  ut_setrequirements FM_3002_1, "P"
  ut_setrequirements FM_3002_2, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Expected event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3002;3002.1;3002.2) - Dir List to File command rejected with null file specification."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3002, "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3002_2, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.1.2: Perform directory listing to TLM commands."
write ";*********************************************************************"
write ";  Step 2.1.2.1: Listing to tlm command: List parent directory"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (parentDir, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3003) - Dir Listing to Tlm command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.1.2.2: Listing to tlm command: List target for a directory"
write ";  that does not exist yet."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_SRC_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir, 0, "Fail")

if (errcnt = $SC_$CPU_FM_CMDEC) then
  write "<*> Passed (1006) - Dir List to Tlm command rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[3].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", FM_GET_DIR_PKT_SRC_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006;3003) - Dir List to Tlm command accepted with missing directory"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_3003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Create Test Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir

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
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3: Verify The directory is created and empty"
write ";*********************************************************************"
write ";  Step 2.3.1: Perform directory listing to file commands."
write ";*********************************************************************"
write ";  Step 2.3.1.1: Listing to file command: List parent directory"
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (parentDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt)
     
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_3002, "P"
  ut_setrequirements FM_1003, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"

    FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
      if ( $SC_$CPU_FM_FileListEntry[currentDis].Name = testDirName ) then
        write "<*> Passed (3000) - Directory found in file listing at file name number " & currentDis
        ut_setrequirements FM_3000, "P"
        goto EXITFOR3
      elseif ( currentDis = $SC_$CPU_FM_NumFilesWritten ) then
        write "<!> Failed (3000) - Directory not found in file listing"
        ut_setrequirements FM_3000, "F"
      endif
    ENDDO
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif
else
  write "<!> Failed (1003;3000;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
  ut_setrequirements FM_3002, "F"
  ut_setrequirements FM_3002_1, "F"
endif

EXITFOR3:
filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.3.1.2: Listing to file command: List target (new) directory"
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt)
     
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3000;3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_3000, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif

  if ($SC_$CPU_FM_TotalFilesInDir = 0) THEN
    write "<*> Passed (3000;3002) - Directory Created and emtpy."
    ut_setrequirements FM_3000,   "P"
    ut_setrequirements FM_3002,   "P"
  else
    write "<!> Failed (3000;3002) - Directory Created, but file indicates wrong number of files: " & $SC_$CPU_FM_TotalFilesInDir
    ut_setrequirements FM_3000,   "F"
    ut_setrequirements FM_3002,   "F"
  endif
else
  write "<!> Failed (1003;3000;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
  ut_setrequirements FM_3002, "F"
  ut_setrequirements FM_3002_1, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.3.2: Perform directory listing to telemetry commands."
write ";*********************************************************************"
write ";  Step 2.3.2.1: Listing to tlm command: List parent directory"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (parentDir, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  FOR currentDis = 1 to FM_DIR_LIST_PKT_ENTRIES DO
    if ($SC_$CPU_FM_DirList[currentDis].name = testDirName) then
      write "<*> Passed (3000) - Directory found in tlm listing at file name number " & currentDis
      ut_setrequirements FM_3000, "P"
      goto EXITFOR4
    elseif ( currentDis = FM_DIR_LIST_PKT_ENTRIES) then
      write "<*> Failed (3000) - Directory not found in tlm listing"
      ut_setrequirements FM_3000, "F"
    endif
  ENDDO
else
  write "<!> Failed (1003;3000;3003) - Dir Listing to Tlm command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
  ut_setrequirements FM_3003, "F"
endif

EXITFOR4:
wait 5

write ";*********************************************************************"
write ";  Step 2.3.2.2: Listing to tlm command: List target (new) directory"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (3000;3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3000, "P"
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_TotalFiles = 0) THEN
    write "<*> Passed (3000;3003) - Directory Created and empty."
    ut_setrequirements FM_3000,   "P"
    ut_setrequirements FM_3003,   "P"
  else
    write "<!> Failed (3000;3003) - Directory Created, but Tlm indicates wrong number of files: ", $SC_$CPU_FM_TotalFiles
    ut_setrequirements FM_3000,   "F"
    ut_setrequirements FM_3003,   "F"
  endif
else
  write "<!> Failed (1003;3000;3003) - Dir Listing to Tlm command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
  ut_setrequirements FM_3003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.3.3: Compare results of file with telemetry response."
write ";*********************************************************************"
write " Compare the Telemetry version to the downloaded file:"

if ($SC_$CPU_FM_DirName = $SC_$CPU_FM_DirNameInFile) THEN
  write "<*> Passed (3000;3002;3002.1;3003) - File and Telemetry Directories match"
  ut_setrequirements FM_3000,   "P"
  ut_setrequirements FM_3002,   "P"
  ut_setrequirements FM_3002_1, "P"
  ut_setrequirements FM_3003,   "P"
else
  write "<!> Failed (3000;3002;3002.1;3003) - File and Telemetry Directories do NOT match"
  ut_setrequirements FM_3000,   "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

if ($SC_$CPU_FM_TotalFiles = $SC_$CPU_FM_TotalFilesInDir) THEN
  if ($SC_$CPU_FM_TotalFiles = 0) THEN
    write "<*> Passed (3000;3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory match"
    ut_setrequirements FM_3000,   "P"
    ut_setrequirements FM_3002,   "P"
    ut_setrequirements FM_3002_1, "P"
    ut_setrequirements FM_3003,   "P"
  else
    write "<!> Failed (3000;3002;3002.1;3003) - Total Number of Files in Directory match, but indicate wrong number of files: ", $SC_$CPU_FM_TotalFiles
    ut_setrequirements FM_3000,   "F"
    ut_setrequirements FM_3002,   "F"
    ut_setrequirements FM_3002_1, "F"
    ut_setrequirements FM_3003,   "F"
  endif
else
  write "<!> Failed (3000;3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory do NOT match"
  ut_setrequirements FM_3000,   "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.4: Upload a File to Test Directory."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 2.5: Verify that the file now exists in the directory"
write ";*********************************************************************"
write ";  Step 2.5.1: Perform directory listing to file command."
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt)

if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif

  if ($SC_$CPU_FM_FileListEntry[1].Name = testFile) THEN
    write "<*> Passed - File " & testFile & " Exists in directory as indicated by the File."
  else
    write "<!> Failed - File " & testFile & " Does NOT Exist in directory as indicated by the File."
  endif
else
  write "<!> Failed (1003;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3002, "F"
  ut_setrequirements FM_3002_1, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.5.2: Perform directory listing to telemetry command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_DirList[1].Name = testFile) THEN
    write "<*> Passed - File " & testFile & " Exists in directory as indicated by tlm."
  else
    write "<!> Failed - File " & testFile & " Does NOT Exist in directory as indicated by tlm."
  endif
else
  write "<!> Failed (1003;3003) - Dir Listing to Tlm command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.5.3: Compare results of file with telemetry response."
write ";*********************************************************************"
write " Compare the Telemetry version to the downloaded file:"

if ($SC_$CPU_FM_DirName = $SC_$CPU_FM_DirNameInFile) THEN
  write "<*> Passed (3002;3002.1;3003) - File and Telemetry Directories match"
  ut_setrequirements FM_3002,   "P"
  ut_setrequirements FM_3002_1, "P"
  ut_setrequirements FM_3003,   "P"
else
  write "<!> Failed (3002;3002.1;3003) - File and Telemetry Directories do NOT match"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

if ($SC_$CPU_FM_TotalFiles = $SC_$CPU_FM_TotalFilesInDir) THEN
  if ($SC_$CPU_FM_TotalFiles = 1) THEN
    write "<*> Passed (3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory match"
    ut_setrequirements FM_3002,   "P"
    ut_setrequirements FM_3002_1, "P"
    ut_setrequirements FM_3003,   "P"
  else
    write "<!> Failed (3002;3002.1;3003) - Total Number of Files in Directory match, but indicate wrong number of files: " & $SC_$CPU_FM_TotalFiles
    ut_setrequirements FM_3002,   "F"
    ut_setrequirements FM_3002_1, "F"
    ut_setrequirements FM_3003,   "F"
  endif
else
  write "<!> Failed (3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory do NOT match"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

if ($SC_$CPU_FM_NumFilesWritten > 0) THEN
  if ($SC_$CPU_FM_DirList[1].Name = $SC_$CPU_FM_FileListEntry[$SC_$CPU_FM_DirOffset+1].Name) THEN
    write "<*> Passed (3002;3002.1;3003) - File and Telemetry First Filename listed match"
    ut_setrequirements FM_3002,   "P"
    ut_setrequirements FM_3002_1, "P"
    ut_setrequirements FM_3003,   "P"
  else
    write "<!> Failed (3002;3002.1;3003) - File and Telemetry First Filename listed do NOT match"
    ut_setrequirements FM_3002,   "F"
    ut_setrequirements FM_3002_1, "F"
    ut_setrequirements FM_3003,   "F"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.6: Attempt to Delete the created directory with file in it."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_DIR_EMPTY_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$SC_$CPU_FM_DirDelete DirName=testDir

ut_tlmwait  $SC_$CPU_FM_CHILDCMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;3001.1) - Delete Directory command rejected as expected."
  ut_setrequirements FM_1004,   "P"
  ut_setrequirements FM_3001_1, "P"
else
  write "<!> Failed (1004;1006) - Delete Directory command accepted when failure was expected."
  ut_setrequirements FM_1004,   "F"
  ut_setrequirements FM_3001_1, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1004, "P"
else
  write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_DIR_EMPTY_ERR_EID, "."
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7: Verify that the directory/file was not deleted"
write ";*********************************************************************"
write ";  Step 2.7.1: Perform directory listing to file command."
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt)

if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"
  else
    write "<!> Failed (3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif

  if ($SC_$CPU_FM_FileListEntry[1].Name = testFile) THEN
    write "<*> Passed (3001.1;3002) - File " & testFile & " Still Exists in directory as indicated by the File."
    ut_setrequirements FM_3001_1, "P"
    ut_setrequirements FM_3002,   "P"
  else
    write "<!> Failed (3001.1;3002) - File " & testFile & " No Longer Exists in directory!"
    ut_setrequirements FM_3001_1, "F"
    ut_setrequirements FM_3002,   "F"
  endif
else
  write "<!> Failed (1003;3001.1;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003,   "F"
  ut_setrequirements FM_3001_1, "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.7.2: Perform directory listing to telemetry command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (3001.1;3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Paased (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_DirList[1].Name = testFile) THEN
    write "<*> Passed (3001.1) - File " & testFile & " Still Exists in directory."
    ut_setrequirements FM_3001_1, "P"
  else
    write "<!> Failed (3001.1) - File " & testFile & " No Longer Exists in directory!"
    ut_setrequirements FM_3001_1, "F"
  endif
else
  write "<!> Failed (1003;3001.1;3003) - Dir Listing to Tlm command failed."
  ut_setrequirements FM_1003,   "F"
  ut_setrequirements FM_3001_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.7.3: Compare results of file with telemetry response."
write ";*********************************************************************"
write " Compare the Telemetry version to the downloaded file:"

if ($SC_$CPU_FM_DirName = $SC_$CPU_FM_DirNameInFile) THEN
  write "<*> Passed (3001.1;3002;3002.1;3003) - File and Telemetry Directories match"
  ut_setrequirements FM_3001_1, "P"
  ut_setrequirements FM_3002,   "P"
  ut_setrequirements FM_3002_1, "P"
  ut_setrequirements FM_3003,   "P"
else
  write "<!> Failed (3001.1;3002;3002.1;3003) - File and Telemetry Directories do NOT match"
  ut_setrequirements FM_3001_1, "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

if ($SC_$CPU_FM_TotalFiles = $SC_$CPU_FM_TotalFilesInDir) THEN
  if ($SC_$CPU_FM_TotalFiles = 1) THEN
    write "<*> Passed (3001.1;3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory match"
    ut_setrequirements FM_3001_1, "P"
    ut_setrequirements FM_3002,   "P"
    ut_setrequirements FM_3002_1, "P"
    ut_setrequirements FM_3003,   "P"
  else
    write "<!> Failed (3001.1;3002;3002.1;3003) - Total Number of Files in Directory match, but indicate wrong number of files: ", $SC_$CPU_FM_TotalFiles
    ut_setrequirements FM_3001_1, "F"
    ut_setrequirements FM_3002,   "F"
    ut_setrequirements FM_3002_1, "F"
    ut_setrequirements FM_3003,   "F"
  endif
else
  write "<!> Failed (3001.1;3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory do NOT match"
  ut_setrequirements FM_3001_1, "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

if ($SC_$CPU_FM_DirList[1].Name = $SC_$CPU_FM_FileListEntry[$SC_$CPU_FM_DirOffset+1].Name) THEN
  write "<*> Passed (3001.1;3002;3002.1;3003) - File and Telemetry match"
  ut_setrequirements FM_3001_1, "P"
  ut_setrequirements FM_3002,   "P"
  ut_setrequirements FM_3002_1, "P"
  ut_setrequirements FM_3003,   "P"
else
  write "<!> Failed (3001.1;3002;3002.1;3003) - File and Telemetry do NOT match"
  ut_setrequirements FM_3001_1, "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.8: Delete the loaded file."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_Delete File = fullTestFileName

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;2008) - Delete File command completed."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2008, "P"
else
  write "<!> Failed (1003;2008) - Delete File command rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2008, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9: Verify that the file was deleted"
write ";*********************************************************************"
write ";  Step 2.9.1: Perform directory listing to file command."
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt)

if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif

  if ($SC_$CPU_FM_TotalFilesInDir = 0) THEN
    write "<*> Passed (2008) - File no longer exists in directory according to dir list file."
    ut_setrequirements FM_2008, "P"
  else
    write "<!> Failed (2008) - File still reported as present in file; $SC_$CPU_FM_TotalFilesInDir: ", $SC_$CPU_FM_TotalFilesInDir
    ut_setrequirements FM_2008, "F"
  endif
else
  write "<!> Failed (1003;2008;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003,   "F"
  ut_setrequirements FM_2008,   "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.9.2: Perform directory listing to telemetry command."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_TotalFiles = 0) THEN
    write "<*> Passed (2008) - File no longer exists in directory"
    ut_setrequirements FM_2008, "P"
  else
    write "<!> Failed (2008) - File still reported as present in telemetry; $SC_$CPU_FM_TotalFiles: ", $SC_$CPU_FM_TotalFiles
    ut_setrequirements FM_2008, "F"
  endif
else
  write "<!> Failed (1003;2008;3003) - Dir Listing to Tlm command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2008, "F"
  ut_setrequirements FM_3003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.9.3: Compare results of file with telemetry response."
write ";*********************************************************************"
write " Compare the Telemetry version to the downloaded file:"

if ($SC_$CPU_FM_DirName = $SC_$CPU_FM_DirNameInFile) THEN
  write "<*> Passed (3002;3002.1;3003) - File and Telemetry Directories match"
  ut_setrequirements FM_3002,   "P"
  ut_setrequirements FM_3002_1, "P"
  ut_setrequirements FM_3003,   "P"
else
  write "<!> Failed (3002;3002.1;3003) - File and Telemetry Directories do NOT match"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

if ($SC_$CPU_FM_TotalFiles = $SC_$CPU_FM_TotalFilesInDir) THEN
  if ($SC_$CPU_FM_TotalFiles = 0) THEN
    write "<*> Passed (2008;3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory match"
    ut_setrequirements FM_2008,   "P"
    ut_setrequirements FM_3002,   "P"
    ut_setrequirements FM_3002_1, "P"
    ut_setrequirements FM_3003,   "P"
  else
    write "<!> Failed (2008;3002;3002.1;3003) - Total Number of Files in Directory match, but indicate a file exists in Directory"
    ut_setrequirements FM_2008,   "F"
    ut_setrequirements FM_3002,   "F"
    ut_setrequirements FM_3002_1, "F"
    ut_setrequirements FM_3003,   "F"
  endif
else
  write "<!> Failed (2008;3002;3002.1;3003) - File and Telemetry Total Number of Files in Directory do NOT match"
  ut_setrequirements FM_2008,   "F"
  ut_setrequirements FM_3002,   "F"
  ut_setrequirements FM_3002_1, "F"
  ut_setrequirements FM_3003,   "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.10: Delete the created directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_CMD_EID, "DEBUG", 1

cmdCtr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirDelete DirName=testDir

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;3001) - Delete Dir command completed."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3001, "P"
else
  write "<!> Failed (1003;3001) - Delete Dir command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.11: Verify the directory no longer exists."
write ";*********************************************************************"
write ";  Step 2.11.1: Perform directory listing to file commands."
write ";*********************************************************************"
write ";  Step 2.11.1.1: Listing to file command: List parent directory"
write ";*********************************************************************"

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (parentDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt)
     
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3002) - Dir Listing to File command accepted."
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[5].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[5].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"

    FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
      if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testDirName) then
        write "<!> Failed (3001) - Directory found in file listing at file name number ", currentDis
        ut_setrequirements FM_3001, "F"
        goto EXITFOR5
      elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
        write "<*> Passed (3001) - Directory not found in file listing"
        ut_setrequirements FM_3001, "P"
      endif
    ENDDO
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif
else
  write "<!> Failed (1003;3002;3002.1) - Dir Listing to File command rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3002, "F"
  ut_setrequirements FM_3002_1, "F"
endif

EXITFOR5:
filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.11.1.2: Listing to file command: List target (deleted) dir"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_SRC_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (testDir, listDir & forwardSlash & listFilePre & filenum & listFileExt, downloadDir, listFilePre & filenum & listFileExt, "Fail")

if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1006;3001) - Dir List to File command rejected as expected."
  ut_setrequirements FM_1006, "P"
  ut_setrequirements FM_3001, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", FM_GET_DIR_FILE_SRC_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006) - Dir List to File command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

filenum = filenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 2.11.2: Perform directory listing to TLM commands."
write ";*********************************************************************"
write ";  Step 2.11.2.1: Listing to tlm command: List parent directory"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (parentDir, 0)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  FOR currentDis = 1 to FM_DIR_LIST_PKT_ENTRIES DO
    if ( $SC_$CPU_FM_DirList[currentDis].name = testDirName ) then
      write "<!> Failed (3001) - Directory found in tlm listing at file name number ", currentDis
      ut_setrequirements FM_3001, "F"
      goto EXITFOR6
    elseif ( currentDis = FM_DIR_LIST_PKT_ENTRIES) then
      write "<*> Passed (3001) - Directory not found in tlm listing"
      ut_setrequirements FM_3001, "P"
    endif
  ENDDO
else
  write "<!> Failed (1003;3001;3003) - Dir Listing to Tlm command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3001, "F"
  ut_setrequirements FM_3003, "F"
endif

EXITFOR6:
wait 5

write ";*********************************************************************"
write ";  Step 2.11.2.2: Listing to tlm command: List target (deleted) dir"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_SRC_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirtlmdisplay (testDir, 0, "Fail")

if ( errcnt = $SC_$CPU_FM_CMDEC ) then
  write "<*> Passed (1006;3001) - Dir List to Tlm command rejected as expected."
  ut_setrequirements FM_1006, "P"
  ut_setrequirements FM_3001, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", FM_GET_DIR_PKT_SRC_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006;3003) - Dir List to Tlm command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_3003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Perform a Power-on Reset to clean-up from this test."
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
write ";  End procedure $sc_$cpu_fm_dircmds_basic                            "
write ";*********************************************************************"
ENDPROC
