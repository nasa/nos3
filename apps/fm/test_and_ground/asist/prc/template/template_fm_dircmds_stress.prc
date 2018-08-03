PROC $sc_$cpu_fm_dircmds_stress
;*******************************************************************************
;  Test Name:  FM_DirCmds_Stress
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to stress the File Manager (FM) Directory
;   Command functions. The FM_DirCreate, FM_DirDelete, FM_DirListFile, and
;   FM_DirListTlm commands will be tested to see if the FM application handles
;   error cases for these, both expected and unexpected.
;
;  Requirements Tested
;    FM1002	If the computed length of any FM command is not equal to the
;		length contained in the message header, FM shall reject
;		the command
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command execution,
;		increment the FM Command Rejected Counter and issue an error
;		event message
;    FM1005	If the filename specified in any FM command is not valid, FM
;		shall reject the command
;    FM1006	If the directory specified in any FM command is not valid, FM
;		shall reject the command
;    FM1008	The CFS FM FSW shall utilize full path specifications having a
;		maximum length of <PLATFORM_DEFINED> characters for all command
;		input arguments requiring a file or pathname.
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified file system
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
;		is not specified.
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
;    -The CFE & FM are up and running and ready to accept commands. 
;    -The FM commands and TLM items exist in the GSE database. 
;    -The display page for the FM telemetry exists. 
;
;  Assumptions and Constraints
;    -None
;
;  Change History
;
;	Date	   Name		Description
;	07/07/08   D. Stewart	Original Procedure
;	12/08/08   W. Moleski   Added requirement 3002.2 and did general cleanup
;	01/14/10   W. Moleski   Updated the proc for FM 2.1.0.0 and did some
;				more cleanup
;	02/25/11   W. Moleski   Added variables for App name and ram directory
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

%liv (log_procedure) = logging

#define FM_1002     0
#define FM_1003     1
#define FM_1004     2
#define FM_1005     3
#define FM_1006     4
#define FM_1008     5
#define FM_3000     6
#define FM_3001     7
#define FM_3001_1   8
#define FM_3002     9
#define FM_3002_1   10
#define FM_3002_2   11
#define FM_3003     12
#define FM_4000     13
#define FM_5000     14

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 14
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables
local cfe_requirements[0 .. ut_req_array_size] = ["FM_1002", "FM_1003", "FM_1004", "FM_1005", "FM_1006", "FM_1008", "FM_3000", "FM_3001", "FM_3001.1", "FM_3002", "FM_3002.1", "FM_3002.2", "FM_3003", "FM_4000", "FM_5000"]

local rawcmd

; more for reference than use, but used for raw commands, hex values:
local endOfStringChar = "00"

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testSourceDir = ramDir & "/FMSOURCE"
local testDestDir   = ramDir & "/FMDEST"

local testDestDir2  = "/cf"

;; /ram + 47 characters -> will make 63 chars + null with testFile2 name
local longDirName1 = ramDir & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
;; /ram + 46 characters -> will make 62 chars + null with testFile2 name
local longDirName2 = ramDir & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
;; /ram + 45 characters -> will make 61 chars + null with testFile2 name
local longDirName3 = ramDir & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local longDir1NameOnly  = "THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local longDir2NameOnly  = "THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local longDir3NameOnly  = "THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local testFile  = "FMDIRCMDSSTRESS.TST"
local testFile2 = "FMLIMIT.TST"
local testFile3 = "FMZEROLEN.TST"
local testFile4 = "FMLARGE.TST"

local verySmallName = "G"

local uploadDir  = ramDirPhys & "/FMSOURCE"
local uploadDir2 = ramDirPhys & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir3 = ramDirPhys & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir4 = ramDirPhys & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local uploadDestDir  = ramDirPhys & "/FMDEST"
local downloadDir = "/FMDEST"

local forwardSlash = "/"

local fullOriginalTestFileName = testSourceDir & forwardSlash & testFile
local fullTestFileName1 = testSourceDir & forwardSlash & testFile3
local fullTestFileName2 = testSourceDir & forwardSlash & testFile4
local fullTestLongName1 = longDirName1 & forwardSlash & testFile2
local fullTestLongName2 = longDirName2 & forwardSlash & testFile2
local fullTestLongName3 = longDirName3 & forwardSlash & testFile2
local fullTestShortName = ramDir & forwardSlash & verySmallName

local badPath = ramDir & "/imaginary/file.tst"

local fileSystem    = "/ram"
local beginFileName = "testfile"
local extension     = ".tst"

local copyNumber  
local numFileCopies = FM_DIR_LIST_FILE_ENTRIES + 10

local listFileNum = 1

local tokenType = ""
local tokenRtn

local numFilesWritEM
local hexFromCvt

local cmpString

local CharsInCreate_63 = "ERROR"
local CharsInCreate_62 = "ERROR"
local CharsInCreate_61 = "ERROR"

local CharsInDirLstTlm_63 = "ERROR"
local CharsInDirLstTlm_62 = "ERROR"
local CharsInDirLstTlm_61 = "ERROR"

local CharsInDirLstFile_Dir_63 = "ERROR"
local CharsInDirLstFile_Dir_62 = "ERROR"
local CharsInDirLstFile_Dir_61 = "ERROR"

local CharsInDirLstFile_File_63 = "ERROR"
local CharsInDirLstFile_File_62 = "ERROR"
local CharsInDirLstFile_File_61 = "ERROR"

local CharsInDelete_63 = "ERROR"
local CharsInDelete_62 = "ERROR"
local CharsInDelete_61 = "ERROR"

local cmdctr
local errcnt

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
write ";  Step 2.1: Send Create Directory Command to Create a Source Dir."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=testSourceDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
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
write ";  Step 2.2: Send Create Directory Command to Create Destination Dir."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=testDestDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
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
write ";  Step 2.3: Upload Test File to test Directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0: Exercise Create Directory command."
write ";*********************************************************************"
write ";  Step 3.1: Send Create Directory commands with bad command lengths."
write ";*********************************************************************"
write ";  Step 3.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000420CCE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000420CCE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000420CCE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", FM_CREATE_DIR_PKT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.1.2: Length too short:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000400CCE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000400CCE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000400CCE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", FM_CREATE_DIR_PKT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send Create Directory cmds with invalid directory names."
write ";*********************************************************************"
write ";  Step 3.2.1: Dir already exists:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=testSourceDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))  
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1006) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", FM_CREATE_DIR_SRC_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.2: Directory spec with an existing file name:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= testSourceDir & forwardSlash & testFile

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1006) - DirCreate Command Rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", FM_CREATE_DIR_SRC_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else  
  write "<!> Failed (1004;1005) - DirCreate Command Accepted."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3: 2 + depth directory of non-existent directories"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_OS_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= ramDir & "/nonexist1/nonexist2/nonexist3/nonexist4"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CHILDCMDEC = errcnt))
if ($SC_$CPU_FM_CHILDCMDEC = errcnt) then
  write "<*> Passed (1004;1006) - DirCreate Command Rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", FM_CREATE_DIR_OS_ERR_EID
  endif
else  
  write "<!> Failed (1004;1006) - DirCreate Command Accepted when rejection was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.4: Perform directory listing to file command."
write ";*********************************************************************"
write "<**> Check the contents of ",fileSystem, " to verify what directories exist"
s $sc_$cpu_fm_dirfiledisplay (fileSystem, ramDir & "/fm_creatstress_1.lst", ramDirPhys, "fm_creatstress_1.lst")

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Send Create Directory commands testing the bounds of the"
write ";  full path name."
write ";*********************************************************************"
write ";  Step 3.3.1: Directory Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"
wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: Directory Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1006) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ",$SC_$CPU_find_event[1].eventid
  endif
else
  write "<!> Failed (1004;1006) - Command Accepted zero character file name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3: Test Directory Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.3.3.1: 63 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

local testDirName = longDirName1 & "FMLIMITSTEST"
local testDirNameOnly = longDir1NameOnly & "FMLIMITSTEST"
/$SC_$CPU_FM_DirCreate DirName=testDirName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;3000) - DirCreate Command Accepted."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CREATE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;3000) - DirCreate Command Rejected. Should have accepted command"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3000, "F"
endif

write "INSPECT LOG HERE:"

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3.2: Check that the longer dir still exists."
write ";*********************************************************************"
write ""
write "<**> Check the contents of ",ramDir," to verify the directory exists"
write ""
s $sc_$cpu_fm_dirfiledisplay (ramDir,ramDir & "/fm_creatstress_2.lst",ramDirPhys,"fm_creatstress_2.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO  
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testDirNameOnly) then
    write "<*> Passed - Directory found in file listing at file name number ",currentDis
    CharsInCreate_63 = "YES"
    goto EXITFOR1
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<!> Failed - Directory not found in file listing"
    CharsInCreate_63 = "NO"
  endif
ENDDO
EXITFOR1:

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3.3: 62 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

testDirName = longDirName2 & "FMLIMITSTEST"
testDirNameOnly = longDir2NameOnly & "FMLIMITSTEST"
/$SC_$CPU_FM_DirCreate DirName=testDirName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;3000) - DirCreate Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CREATE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;3000) - DirCreate Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3.4: Check that the dir exists."
write ";*********************************************************************"
write ""
write "<**> Check the contents of ",ramDir," to verify the directory exists"
write ""
s $sc_$cpu_fm_dirfiledisplay (ramDir,ramDir & "/fm_creatstress_3.lst",ramDirPhys,"fm_creatstress_3.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testDirNameOnly) then
    write "<*> Passed - Directory found in file listing at file name number ", currentDis
    CharsInCreate_62 = "YES"
    goto EXITFOR2
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<!> Failed - Directory not found in file listing"
    CharsInCreate_62 = "NO"
  endif
ENDDO
EXITFOR2:

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3.5: 61 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

testDirName = longDirName3 & "FMLIMITSTEST"
testDirNameOnly = longDir3NameOnly & "FMLIMITSTEST"
/$SC_$CPU_FM_DirCreate DirName=testDirName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;3000) - DirCreate Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CREATE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;3000) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3.6: Check that the dir exists."
write ";*********************************************************************"
write ""
write "<**> Check the contents of ",ramDir," to verify the directory exists"
write ""
s $sc_$cpu_fm_dirfiledisplay (ramDir,ramDir & "/fm_creatstress_4.lst",ramDirPhys,"fm_creatstress_4.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testDirNameOnly) then
    write "<*> Passed - Directory found in file listing at file name number ",currentDis
    CharsInCreate_61 = "YES"
    goto EXITFOR3
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<!> Failed - Directory not found in file listing"
    CharsInCreate_61 = "NO"
  endif
ENDDO
EXITFOR3:

wait 5

write ";*********************************************************************"
write ";  Step 3.3.4: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=fullTestShortName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3000) - DirCreate Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - DirCreate Command Rejected."
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
write ";  Step 4.0: Exercise Directory Listing to Telemetry command."
write ";*********************************************************************"
write ";  Step 4.1: Send Dir Listing to Tlm cmds with bad command lengths."
write ";*********************************************************************"
write ";  Step 4.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_PKT_ERR_EID,"ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000460FCE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000460FCE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000460FCE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid," received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ",$SC_$CPU_find_event[1].eventid
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.1.2: Length too short:"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_PKT_ERR_EID,"ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000440FCE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000440FCE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000440FCE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", $SC_$CPU_find_event[1].eventid
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Send Dir Listing to Tlm commands with invalid Dir names."
write ";*********************************************************************"
write ";  Step 4.2.1: Invalid Path (non-existant Dir):"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_SRC_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirtlmdisplay (ramDir & "/imaginary/", 0,"Fail")

if (errcnt = $SC_$CPU_FM_CMDEC) then
  write "<*> Passed (1006) - Dir List to Tlm command rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received."
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", $SC_$CPU_find_event[1].eventid
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
write ";  Step 4.2.2: Directory with an existing file name:"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_SRC_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirtlmdisplay (fullOriginalTestFileName, 0,"Fail")

if ( errcnt = $SC_$CPU_FM_CMDEC ) then
  write "<*> Passed (1006) - Dir List to Tlm command rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid," received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", $SC_$CPU_find_event[1].eventid
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
write ";  Step 4.3: Send Dir Listing to Tlm cmds testing the bounds of the"
write ";  full path names for the Directory Name."
write ";*********************************************************************"
write ";  Step 4.3.1: Directory Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"
wait 5

write ";*********************************************************************"
write ";  Step 4.3.2: Directory Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_SRC_ERR_EID,"ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirtlmdisplay ("", 0, "Fail")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1006) - Command Rejected as expected."
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ", $SC_$CPU_find_event[1].eventid
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006;1008) - Command Accepted zero character directory name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_1008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3.3: Upload files to the directories."
write ";*********************************************************************"
; 63 + null total path length 
;; Upload the Test file
s ftp_file (uploadDir2 & "FMLIMITSTEST", testFile2, testFile2, "$CPU", "P")

; 62 + null total path length 
;; Upload the Test file
s ftp_file (uploadDir3 & "FMLIMITSTEST", testFile2, testFile2, "$CPU", "P")

; 61 + null total path length 
;; Upload the Test file
s ftp_file (uploadDir4 & "FMLIMITSTEST", testFile2, testFile2, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 4.3.4: Test Directory Names of maximum allowable length."
write ";*********************************************************************"
write ";  Step 4.3.4.1: 63 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

testDirName = longDirName1 & "FMLIMITSTEST"

s $sc_$cpu_fm_dirtlmdisplay (testDirName, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3003) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;1008;3003) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3003, "F"
endif

write "INSPECT LOG HERE 1:"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.4.2: Check the dir for the files."
write ";*********************************************************************"
write ""
write "<**> Check the contents of ",testDirName," to verify the directory exists"
write ""

FOR currentDis = 1 to $SC_$CPU_FM_PktFiles DO
  if ($SC_$CPU_FM_DirList[currentDis].Name = testFile2) then
    write "<*> Passed - File found in file listing at file name number ", currentDis
    CharsInDirLstTlm_63 = "YES"
    goto EXITFOR4
  elseif (currentDis = $SC_$CPU_FM_PktFiles) then
    write "<!> Failed - File not found in packet listing"
    CharsInDirLstTlm_63 = "NO"
  endif
ENDDO
EXITFOR4:

wait 5

write ";*********************************************************************"
write ";  Step 4.3.4.3: 62 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_WARNING_EID, "INFO", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

testDirName = longDirName2 & "FMLIMITSTEST/"

s $sc_$cpu_fm_dirtlmdisplay (testDirName, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3003) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed - Name length warning message ",$SC_$CPU_find_event[2].eventid, " received."
;; Commenting out the else since this works in 2.4.1.0
;;  else
;;    write "<!> Failed - Expected Name length warning message ", FM_GET_DIR_PKT_WARNING_EID, " NOT received."
  endif

  CharsInDirLstTlm_62 = "YES"
else
  write "<!> Failed (1003;1008;3003) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3003, "F"

  CharsInDirLstTlm_62 = "NO"
endif

write "INSPECT LOG HERE 2:"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.4.4: Check the dir for the files."
write ";*********************************************************************"
write ""
write "<**> Check the contents of ",testDirName," to verify the directory exists"
write ""

FOR currentDis = 1 to $SC_$CPU_FM_PktFiles DO
  if ($SC_$CPU_FM_DirList[currentDis].Name = testFile2) then
    write "<*> Passed - File found in file listing at file name number ", currentDis
    CharsInDirLstTlm_62 = "YES"
    goto EXITFOR5
  elseif (currentDis = $SC_$CPU_FM_PktFiles) then
    write "<!> Failed - File not found in packet listing"
    CharsInDirLstTlm_62 = "NO"
  endif
ENDDO
EXITFOR5:

wait 5

write ";*********************************************************************"
write ";  Step 4.3.4.5: 61 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_WARNING_EID, "INFO", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

testDirName = longDirName3 & "FMLIMITSTEST/"
s $sc_$cpu_fm_dirtlmdisplay (testDirName, 0)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3003) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed - Name length warning message ",$SC_$CPU_find_event[2].eventid, " received as expected."
;; Commenting out the else since this works in 2.4.1.0
;;  else
;;    write "<!> Failed - Expected Name length warning message ", FM_GET_DIR_PKT_WARNING_EID, " NOT received."
  endif

  CharsInDirLstTlm_61 = "YES"
else
  write "<!> Failed (1003;1008;3003) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3003, "F"

  CharsInDirLstTlm_61 = "NO"
endif

write "INSPECT LOG HERE 3:"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.4.6: Check the dir for the files."
write ";*********************************************************************"
write ""
write "<**> Check the contents of ",testDirName," to verify the directory exists"
write ""

FOR currentDis = 1 to $SC_$CPU_FM_PktFiles DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testFile2) then
    write "<*> Passed - File found in file listing at file name number ", currentDis
    CharsInDirLstTlm_61 = "YES"
    goto EXITFOR6
  elseif (currentDis = $SC_$CPU_FM_PktFiles) then
    write "<!> Failed - File not found in packet listing"
    CharsInDirLstTlm_61 = "NO"
  endif
ENDDO
EXITFOR6:

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5: Test Dir Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirtlmdisplay (fullTestShortName, 0)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (1003;3003) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3003, "P"
else
  write "<!> Failed (1003;3003) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3003, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_DIR_PKT_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4: Test large amount of files in Directory."
write ";*********************************************************************"
write ";  Step 4.4.1: Fill an empty Directory with files."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= testDestDir & "2"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

; Create FM_DIR_LIST_FILE_ENTRIES + 10 files in the new directory
; this is done as quickly as possible currently... we'll see how the system holds up
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_OS_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC
errcnt = $SC_$CPU_FM_CMDEC

;; Copy files until the Error Event occurs
FOR copyNumber = 1 to numFileCopies DO
  cmdctr = cmdctr + 1

  /$SC_$CPU_FM_FileCopy Overwrite=0 DestName= testDestDir & "2" & forwardSlash & "file" & copyNumber & ".tst" , File= fullOriginalTestFileName

  wait 1

  ;; If the Error Event message was generated, break out of the loop
  if ($SC_$CPU_find_event[1].num_found_messages > 0) then
    break;
  endif
ENDDO

local lastFileCopied = copyNumber

wait 5

write ";*********************************************************************"
write ";  Step 4.4.2: Send Dir Listing to Tlm commands to list a sample of the"
write ";  files created above."
write ";*********************************************************************"
; Setup the events to capture
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

;; List the first set of files
s $sc_$cpu_fm_dirtlmdisplay (testDestDir & "2", 0)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
endif

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

;; List the last set of files
local fileOffset = lastFileCopied - 20

s $sc_$cpu_fm_dirtlmdisplay (testDestDir & "2", fileOffset)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (3003) - Dir Listing to Tlm command sent properly."
  ut_setrequirements FM_3003, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.4.3: Delete all the files in the directory used above since "
write ";  the file system is full and subsequent steps that generate files "
write ";  will not work as expected. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName= testDestDir & "2"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> - Delete All Files Command Accepted. Files deleted."
else
  write "<!> - Delete All Files Command Rejected. Files may not have been deleted."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> - Event message ", $SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.0: Exercise Directory Listing to File command."
write ";*********************************************************************"
write ";  Step 5.1: Send Dir Listing to File cmds with bad command lengths."
write ";*********************************************************************"
write ";  Step 5.1.1: Length too long:"
write ";*********************************************************************"
listFileNum = 1

ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_PKT_ERR_EID,"ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000820ECE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000820ECE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000820ECE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid," received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ",$SC_$CPU_find_event[1].eventid
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.1.2: Length too short:"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_PKT_ERR_EID,"ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000800ECE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000800ECE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000800ECE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid," received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive EM ",$SC_$CPU_find_event[1].eventid
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.2: Send Dir Listing to File cmds with invalid dir names."
write ";*********************************************************************"
write ";  Step 5.2.1: Invalid Path (non-existant Dir):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_FILE_SRC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (ramDir & "/imaginary/", testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension, "Fail")
     
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1006) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event message ",$SC_$CPU_find_event[3].eventid," received."
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_find_event[3].eventid, " received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 5.2.2: Directory and an existing file name:"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_SRC_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC

s $sc_$cpu_fm_dirfiledisplay (fullOriginalTestFileName, testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension, "Fail")
     
if ($SC_$CPU_FM_CMDEC > errcnt) then
  write "<*> Passed (1004;1006) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", FM_GET_DIR_FILE_SRC_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 5.3: Send Dir Listing to File cmds with invalid File names."
write ";*********************************************************************"
write ";  Step 5.3.1: Invalid Path (non-existant File):"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_OS_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, badPath, "RAM:0/imaginary/", "file.tst","Fail")

if ($SC_$CPU_FM_CHILDCMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",FM_GET_DIR_FILE_OS_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3.2: No File Name Present"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_TGT_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, testDestDir & forwardSlash & "", downloadDir, "", "Fail" )
     
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",FM_GET_DIR_FILE_TGT_ERR_EID, " NOT received."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"

endif

wait 5

write ";*********************************************************************"
write ";  Step 5.3.3: Good Src Dir, existing File Name Present"
write ";*********************************************************************"
write "First create the default file: "
s $sc_$cpu_fm_dirfiledisplay (testDestDir, "", ramDirPhys, "fm_dirlist.out")

ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_TGT_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, "", ramDirPhys, "fm_dirlist.out")
     
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<!> Failed (1004;1005;3002.2) - Command Accepted Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_3002_2, "F"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid," received."
  else
    write "<!> Failed - Event message ",$SC_$CPU_find_event[1].eventid," NOT received."
  endif
else
  write "<*> Passed (1004;1005;3002.2) - Command Accepted Counter incremented as expected. File overwritten. Check log for changes."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_3002_2, "P"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4: Send Dir Listing to File cmds testing the bounds of the"
write ";  full path names for the Directory Name."
write ";*********************************************************************"
write ";  Step 5.4.1: Directory Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"
wait 5

write ";*********************************************************************"
write ";  Step 5.4.2: Directory Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_CMD_EID,"DEBUG", 1
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_SRC_ERR_EID,"ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay ("", testDestDir & "/dummyFile.dat", downloadDir, "dummyFile.dat","Fail")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1006) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"


  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[2].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[2].eventid, " Either the wrong EM is expected or the wrong one was received. "
  endif
else
  write "<!> Failed (1004;1006) - Command Accepted zero character directory name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.4.3: Create Directories of near maximum allowable length:"
write ";*********************************************************************"
write "directories already exist"
wait 5

write ";*********************************************************************"
write ";  Step 5.4.4: Upload files to the directories."
write ";*********************************************************************"
write "Files already exist"
wait 5

write ";*********************************************************************"
write ";  Step 5.4.5: Test Directory Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 5.4.5.1: 63 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_WARNING_EID,"INFO", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (longDirName1 & "FMLIMITSTEST", testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension)  ; 63 char + null

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3002) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
;; Commenting out the else since this works in 2.4.1.0
;;  else
;;    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
;;    ut_setrequirements FM_1003, "F"
  endif

  FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
    if ( $SC_$CPU_FM_FileListEntry[currentDis].Name = testFile2 ) then
      write "<*> Passed - File found in file listing at file name number ", currentDis
      CharsInDirLstFile_Dir_63 = "YES"
      goto EXITFOR7
    elseif ( currentDis = $SC_$CPU_FM_NumFilesWritten ) then
      write "<!> Failed - File not found in file listing"
    CharsInDirLstFile_Dir_63 = "NO"
    endif
  ENDDO
else
  write "<!> Failed (1003;1008;3002) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"

  CharsInDirLstFile_Dir_63 = "NO"
endif

EXITFOR7:
listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 5.4.5.2: Check the dir for the files."
write ";*********************************************************************"
write "<!> INSPECT LOG ABOVE HERE 1:"

wait 5

write ";*********************************************************************"
write ";  Step 5.4.5.3: 62 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_WARNING_EID,"INFO", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (longDirName2 & "FMLIMITSTEST/", testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension)

if (cmdctr <= $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3002) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
;; Commenting out the else since this works in 2.4.1.0
;;  else
;;    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
;;    ut_setrequirements FM_1003, "F"
  endif

  CharsInDirLstFile_Dir_62 = "YES"

  FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
    if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testFile2) then
      write "<*> Passed - File found in file listing at file name number ", currentDis
      CharsInDirLstFile_Dir_62 = "YES"
      goto EXITFOR8
    elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
      write "<!> Failed - File not found in file listing"
      CharsInDirLstFile_Dir_62 = "NO"
    endif
  ENDDO
else
  write "<!> Failed (1003;1008;3002) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"

  CharsInDirLstFile_Dir_62 = "NO"
endif

EXITFOR8:
listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 5.4.5.4: Check the dir for the files."
write ";*********************************************************************"
write "<!> INSPECT LOG ABOVE HERE 2:"

write ";*********************************************************************"
write ";  Step 5.4.5.5: 61 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_WARNING_EID,"INFO", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (longDirName3 & "FMLIMITSTEST/", testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3002) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
;; Commenting out the else since this works in 2.4.1.0
;;  else
;;    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
;;    ut_setrequirements FM_1003, "F"
  endif

  CharsInDirLstFile_Dir_61 = "YES"

  FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
    if ($SC_$CPU_FM_FileListEntry[currentDis].Name = testFile2) then
      write "<*> Passed - File found in file listing at file name number ", currentDis
      CharsInDirLstFile_Dir_61 = "YES"
      goto EXITFOR9
    elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
      write "<!> Failed - File not found in file listing"
      CharsInDirLstFile_Dir_61 = "NO"
    endif
  ENDDO
else
  write "<!> Failed (1003;1008;3002) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"

  CharsInDirLstFile_Dir_61 = "NO"
endif

EXITFOR9:
listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 5.4.5.6: Check the dir for the files."
write ";*********************************************************************"
write "<!> INSPECT LOG ABOVE HERE 3:"

wait 5

write ";*********************************************************************"
write ";  Step 5.4.6: Test Dir Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (fullTestShortName, testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1003;1008;3002) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"
else
  write "<!> Failed (1003;1008;3002) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_DIR_PKT_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 5.5: Send Dir Listing to File cmds testing the bounds of the"
write ";  full path names for the File Name."
write ";*********************************************************************"
write ";  Step 5.5.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"
wait 5

write ";*********************************************************************"
write ";  Step 5.5.2: File Name with 0 characters. FM now substitues the "
write ";  File Name with a default name. "
write ";*********************************************************************"
cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir,"",downloadDir,"defaultFile.dat")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1004;1005) - Command accepted with null file name!"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
else
  write "<!> Failed (1004;1005) - Command rejected with null Filename. Should have used default name."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5.3: Create Directory of near maximum allowable length:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= longDirName1

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

; The next two in this step are to find exactly where the limit is
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=longDirName2

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=longDirName3

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.5.4: Upload files to the new directories."
write ";*********************************************************************"
write "Files will be created (or not) in next steps"

wait 5

write ";*********************************************************************"
write ";  Step 5.5.5: Test File Names of maximum allowable length."
write ";*********************************************************************"
write ";  Step 5.5.5.1: 63 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_CMD_EID,"DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, longDirName1 & "/LONG111.TST", uploadDir2, "/LONG111.TST")

if (cmdctr <= $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3002) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;3002) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"

  CharsInDirLstFile_File_63 = "NO"
endif

listFileNum = listFileNum + 1

write "<!> INSPECT LOG ABOVE HERE 1:"

wait 5

write ";*********************************************************************"
write ";  Step 5.5.5.2: Check the dir for the files."
write ";*********************************************************************"
if ( CharsInDirLstFile_File_63 = "NO" ) then
  goto SKIP5552
endif

write ""
write "<**> Check the contents of ", longDirName1 , " to verify the File exists"
write ""
s $sc_$cpu_fm_dirfiledisplay (longDirName1, ramDir & "/fm_dlfstress_3.lst", ramDirPhys, "fm_dlfstress_3.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = "LONG111.TST") then
    write "<*> Passed - File found in file listing at file name number ", currentDis
    CharsInDirLstFile_File_63 = "YES"
    goto EXITFOR10
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<!> Failed - File not found in file listing"
  CharsInDirLstFile_File_63 = "NO"
  endif
ENDDO
EXITFOR10:
SKIP5552:

wait 5

write ";*********************************************************************"
write ";  Step 5.5.5.3: 62 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, longDirName2 & "/LONG222.TST", uploadDir3, "/LONG222.TST")

if (cmdctr <= $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3002) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
  CharsInDirLstFile_File_62 = "YES"
else
  write "<!> Failed (1008;3002) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"

  CharsInDirLstFile_File_62 = "NO"
endif

listFileNum = listFileNum + 1

write "<!> INSPECT LOG ABOVE HERE 2:"

wait 5

write ";*********************************************************************"
write ";  Step 5.5.5.4: Check the dir for the files."
write ";*********************************************************************"
if (CharsInDirLstFile_File_62 = "NO") then
  goto SKIP5554
endif

write ""
write "<**> Check the contents of ", longDirName2 , " to verify the File exists"
write ""
testDirName = longDirName2 & "/"
s $sc_$cpu_fm_dirfiledisplay (testDirName, ramDir & "/fm_dlfstress_4.lst", ramDirPhys, "fm_dlfstress_4.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = "LONG222.TST") then
    write "<*> Passed - File found in file listing at file name number ", currentDis
    CharsInDirLstFile_File_62 = "YES"
    goto EXITFOR11
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<!> Failed - File not found in file listing"
    CharsInDirLstFile_File_62 = "NO"
  endif
ENDDO
EXITFOR11:
SKIP5554:

wait 5

write ";*********************************************************************"
write ";    Step 5.5.5.5: 61 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName},FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName},FM_GET_DIR_FILE_OS_ERR_EID,"ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, longDirName3 & "/LONG333.TST", uploadDir4, "/LONG333.TST")

if (cmdctr <= $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;3002) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;3002) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"

  CharsInDirLstFile_File_61 = "NO"
endif

listFileNum = listFileNum + 1

write "<!> INSPECT LOG ABOVE HERE 3:"

wait 5

write ";*********************************************************************"
write ";  Step 5.5.5.6: Check the dir for the files."
write ";*********************************************************************"

if (CharsInDirLstFile_File_61 = "NO") then
  goto SKIP5556
endif

testDirName = longDirName3 & "/"
s $sc_$cpu_fm_dirfiledisplay (testDirName, ramDir & "/fm_dlfstress_5.lst", ramDirPhys, "fm_dlfstress_5.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = "LONG333.TST") then
    write "<*> Passed - File found in file listing at file name number ", currentDis
    CharsInDirLstFile_File_61 = "YES"
    goto EXITFOR12
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<!> Failed - File not found in file listing"
    CharsInDirLstFile_File_61 = "NO"
  endif
ENDDO
EXITFOR12:
SKIP5556:

wait 5

write ";*********************************************************************"
write ";  Step 5.5.6: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir, fileSystem & "/S", ramDirPhys, "/S")

if ( cmdctr <= $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (1003;1008;3002) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3002, "P"
else
  write "<!> Failed (1003;1008;3002) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_DIR_PKT_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.6: Test large amount of files in Directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_dirfiledisplay (testDestDir & "2", testDestDir & forwardSlash & beginFileName & listFileNum & extension, downloadDir, beginFileName & listFileNum & extension)

if (cmdctr <= $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (3002) - Command Accpeted."
  ut_setrequirements FM_3002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003;3002.1) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
    ut_setrequirements FM_3002_1, "P"
  else
    write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
    ut_setrequirements FM_3002_1, "F"
  endif
else
  write "<!> Failed (1003;3002) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3002, "F"
endif

listFileNum = listFileNum + 1

wait 5

write ";*********************************************************************"
write ";  Step 6.0: Exercise Delete Directory command."
write ";*********************************************************************"
write ";  Step 6.1: Send Delete Directory commands with bad command lengths."
write ";*********************************************************************"
write ";  Step 6.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000420DCE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000420DCE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000420DCE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.1.2: Length too short:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000400DCE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000400DCE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000400DCE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2: Send Delete Directory cmds with invalid directory names."
write ";*********************************************************************"
write ";  Step 6.2.1: Invalid Path (non-existant Dir):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName= ramDir & "/imaginary/"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1006) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid
  endif
else
  write "<!> Failed (1004;1006;3001) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.2.2: Directory and an existant file name:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName=fullOriginalTestFileName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid
  endif
else  
  write "<!> Failed (1004;1005;3001) - Command Accepted."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_3001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3: Send Delete Directory commands testing the bounds of the"
write ";  full path name."
write ";*********************************************************************"
write ";  Step 6.3.1: Directory Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"
wait 5

write ";*********************************************************************"
write ";  Step 6.3.2: Directory Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName=""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1004;1006) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid
  endif
else
  write "<!> Failed (1004;1006) - Command Accepted zero character Dir name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3: Test Directory Names of maximum allowable length."
write ";*********************************************************************"
write ";  Step 6.3.3.1: Create new Dirs (empty) for this next test."
write ";*********************************************************************"
write ";  Step 6.3.3.1.1: 63 chars + null."
write ";*********************************************************************"
cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= longDirName1 & "FMLIMITTEST1"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed - Directory Created."
else
  write "<!> Failed - Directory not Created."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.1.2: 62 chars + null."
write ";*********************************************************************"
cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= longDirName2 & "FMLIMITTEST2"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed - Directory Created."
else
  write "<!> Failed - Directory not Created."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.1.3: 61 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_OS_ERR_EID, "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= longDirName3 & "FMLIMITTEST3"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed - Directory Created."
else
  write "<!> Failed - Directory not Created."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.2: 63 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName= longDirName1 & "FMLIMITTEST1"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;3001) - Command Accepted."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3001, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_DELETE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelete_63 = "INSPECT LOG"
else
  write "<!> Failed (1003;1008;3001) - Command Rejected. Should have accepted command"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3001, "F"

  CharsInDelete_63 = "NO"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.3: Check that the dir no longer exists."
write ";*********************************************************************"
if (CharsInDelete_63 = "NO") then
  goto SKIP6333
endif

write ""
write "<!> INSPECT LOG HERE:"

s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_2.lst", ramDirPhys, "fm_delstress_2.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ( $SC_$CPU_FM_FileListEntry[currentDis].Name = ("THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM" & "FMLIMITTEST1")) then
    write "<!> Failed - File found in file listing at file name number ", currentDis
    CharsInDelete_63 = "NO"
    goto EXITFOR13
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<*> Passed - File not found in file listing"
  CharsInDelete_63 = "YES"
  endif
ENDDO
EXITFOR13:
SKIP6333:

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.4: 62 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName= longDirName2 & "FMLIMITTEST2/"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;3001) - DirDelete Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3001, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_DELETE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;1008;3001) - DirDelete Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3001, "F"

  CharsInDelete_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.5: Check that the dir no longer exists."
write ";*********************************************************************"
if (CharsInDelete_62 = "NO") then
  goto SKIP6335
endif

s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_3.lst", ramDirPhys, "fm_delstress_3.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ( $SC_$CPU_FM_FileListEntry[currentDis].Name = ("THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM" & "FMLIMITTEST1") ) then
    write "<!> Failed - File found in file listing at file name number ", currentDis
    CharsInDelete_62 = "NO"
    goto EXITFOR14
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<*> Passed - File not found in file listing"
  CharsInDelete_62 = "YES"
  endif
ENDDO
EXITFOR14:
SKIP6335:

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.6: 61 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName= longDirName3 & "FMLIMITTEST3/"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;3001) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3001, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_DELETE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;1008;3001) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3001, "F"

  CharsInDelete_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.3.7: Check that the dir no longer exists."
write ";*********************************************************************"
if (CharsInDelete_61 = "NO") then
  goto SKIP6337
endif

s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_4.lst", ramDirPhys, "fm_delstress_4.lst")

FOR currentDis = 1 to $SC_$CPU_FM_NumFilesWritten DO
  if ($SC_$CPU_FM_FileListEntry[currentDis].Name = ("THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM" & "FMLIMITTEST1")) then
    write "<!> Failed - File found in file listing at file name number ", currentDis
    CharsInDelete_61 = "NO"
    goto EXITFOR15
  elseif (currentDis = $SC_$CPU_FM_NumFilesWritten) then
    write "<*> Passed - File not found in file listing"
  CharsInDelete_61 = "YES"
  endif
ENDDO
EXITFOR15:
SKIP6337:

wait 5

write ";*********************************************************************"
write ";  Step 6.3.4: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName=fullTestShortName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3001) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3001, "P"
else
  write "<!> Failed (1003;3001) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.5: Try to delete a directory with files in it."
write ";*********************************************************************"
write ";  Step 6.3.5.1: Make sure the directory has at least 1 file in it."
write ";*********************************************************************"
s $sc_$cpu_fm_dirfiledisplay (testDestDir & "2", testDestDir & "2/fm_dest2.lst", ramDirPhys, "fm_dest2.lst")

;; Check the number of files contained in the directory
if ($SC_$CPU_FM_TotalFilesInDir = 0) then
  ;; Upload the Test file
  s ftp_file (uploadDestDir & "2", testFile, testFile, "$CPU","P")
  wait 5
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.5.2: Send the Delete Directory command."
write ";*********************************************************************"
ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_DIR_EMPTY_ERR_EID,"ERROR", 1

errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$SC_$CPU_FM_DirDelete DirName= testDestDir & "2"

ut_tlmwait $SC_$CPU_FM_CHILDCMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1006;3001.1) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"
  ut_setrequirements FM_3001_1, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Did not receive EM ", $SC_$CPU_find_event[1].eventid
  endif
else
  write "<!> Failed (1004;1006;3001.1) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_3001_1, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.6: Remove the files from the directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName= testDestDir & "2"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> - Delete All Files Command Accepted. Files deleted."
else
  write "<!> - Delete All Files Command Rejected. Files may not have been deleted."
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
endif

wait 5

write ";*********************************************************************"
write ";  Step 6.3.7: Try to delete the same directory again (should work)."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirDelete DirName= testDestDir & "2"    

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;1008;3001) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_3001, "P"
else
  write "<!> Failed (1003;1008;3001) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_3001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_DIR_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

PROCEND:

write ";*********************************************************************"
write ";  Step 7.0:  Perform a Power-on Reset to clean-up from this test."
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

write ""
write "63 Characters + NULL allowed in Create Dir Cmd? ", CharsInCreate_63
write "62 Characters + NULL allowed in Create Dir Cmd? ", CharsInCreate_62
write "61 Characters + NULL allowed in Create Dir Cmd? ", CharsInCreate_61
write ""
write "63 Characters + NULL allowed in Dir List to Tlm Cmd? ", CharsInDirLstTlm_63
write "62 Characters + NULL allowed in Dir List to Tlm Cmd? ", CharsInDirLstTlm_62
write "61 Characters + NULL allowed in Dir List to Tlm Cmd? ", CharsInDirLstTlm_61
write ""
write "63 Characters + NULL allowed in Dir List to File Cmds Dir Field? ", CharsInDirLstFile_Dir_63
write "62 Characters + NULL allowed in Dir List to File Cmds Dir Field? ", CharsInDirLstFile_Dir_62
write "61 Characters + NULL allowed in Dir List to File Cmds Dir Field? ", CharsInDirLstFile_Dir_61
write ""
write "63 Characters + NULL allowed in Dir List to File Cmds File Field? ", CharsInDirLstFile_File_63
write "62 Characters + NULL allowed in Dir List to File Cmds File Field? ", CharsInDirLstFile_File_62
write "61 Characters + NULL allowed in Dir List to File Cmds File Field? ", CharsInDirLstFile_File_61
write ""
write "63 Characters + NULL allowed in Delete Dir Cmd? ", CharsInDelete_63
write "62 Characters + NULL allowed in Delete Dir Cmd? ", CharsInDelete_62
write "61 Characters + NULL allowed in Delete Dir Cmd? ", CharsInDelete_61
write ""
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_dircmds_stress                           "
write ";*********************************************************************"
ENDPROC
