PROC $sc_$cpu_fm_filedelete_stress
;*******************************************************************************
;  Test Name:  FM_FileDelete_Stress
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to stress the File Manager (FM) File Delete
;   Command functions. The FM_Delete and FM_DeleteAll commands will be tested
;   to see if the FM application handles error cases for these, both expected
;   and unexpected. The FM_DirCreate command is also used to facilitate the
;   testing, but is not stressed in this scenario.
;
;  Requirements Tested
;    FM1002	If the computed length of any FM command is not equal to the
;		length contained in the message header, FM shall reject the 
;		command
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue an 
;		event message
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
;    FM2007	Upon receipt of a Delete All Files command, FM shall delete all 
;		the files in the command-specified directory
;    FM2007.2	If the command-specified directory contains a subdirectory, FM
;		FM shall not delete the subdirectory
;    FM2007.2.1	For any subdirectories are not deleted, FM shall issue one
;		event message.
;    FM2008	Upon receipt of a Delete command, FM shall delete the
;		command-specified file.
;    FM2008.2	If the command-specified file is a directory, FM shall reject
;		the command
;    FM2011     Upon receipt of a File Info command, FM shall generate a message
;               containing the following for the command-specified file:
;                       a) the file size,
;                       b) last modification time,
;                       c) file status (Open, Closed, or Non-existent)
;                       d) <MISSION_DEFINED> CRC
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified file system
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
;	07/02/08   D. Stewart	Original Procedure
;	12/08/08   W. Moleski	Added Requirements 2007.2 and 2008.2
;	01/28/10   W. Moleski	Updated proc for FM 2.1.0.0
;       02/28/11   W. Moleski   Added variables for App name and ram directory
;       10/04/11   W. Moleski   Added Step 3.4 to test the Delete File request
;			 	fix that does not send an FM event.
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG.
;				Also added test of 2007.2.1.
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

#define FM_1002     0
#define FM_1003     1
#define FM_1004     2
#define FM_1005     3
#define FM_1006     4
#define FM_1008     5
#define FM_2007     6
#define FM_20072    7
#define FM_200721   8
#define FM_2008     9
#define FM_20082    10
#define FM_2011     11
#define FM_3000     12
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

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1002", "FM_1003", "FM_1004", "FM_1005", "FM_1006", "FM_1008", "FM_2007", "FM_2007.2", "FM_2007.2.1", "FM_2008", "FM_2008.2", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local rawcmd

; more for reference than use, but used for raw commands, hex values:
local slashInHex   = "2F"
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

local testFile  = "FMDELETESTRESS.TST"
local testFile2 = "FMLIMIT.TST"
local testFile3 = "FMZEROLEN.TST"
local testFile4 = "FMLARGE.TST"

local verySmallName = "G"

local uploadDir  = ramDirPhys & "/FMSOURCE"
local uploadDir2 = ramDirPhys & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir3 = ramDirPhys & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir4 = ramDirPhys & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local forwardSlash = "/"

local fullOriginalTestFileName = testSourceDir & forwardSlash & testFile

local fullTestFileName1 = testSourceDir & forwardSlash & testFile3
local fullTestFileName2 = testSourceDir & forwardSlash & testFile4

local fullTestLongName1 = longDirName1 & forwardSlash & testFile2
local fullTestLongName2 = longDirName2 & forwardSlash & testFile2
local fullTestLongName3 = longDirName3 & forwardSlash & testFile2

local fullTestShortName = ramDir & forwardSlash & verySmallName

local badPath = ramDir & "/imaginary/file.tst"

local CharsInDelFile_63
local CharsInDelFile_62
local CharsInDelFile_61

local CharsInDelAll_63
local CharsInDelAll_62
local CharsInDelAll_61

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
Write ";  Step 2.0: File System Preparation."
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
write ";  Step 2.3: Upload Test File to test Directory."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0: Exercise File Delete Command."
write ";*********************************************************************"
write ";  Step 3.1: Send Delete File Command with bad command lengths."
write ";*********************************************************************"
write ";  Step 3.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "188CC000004205CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000004205CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000004205CE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_PKT_ERR_EID, "."
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
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "188CC000004005CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000004005CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000004005CE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_PKT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send Delete File Command with invalid file names."
write ";*********************************************************************"
write ";  Step 3.2.1: Invalid Path (non-existant path & file):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=badPath

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005;2008) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2008) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.2: No File Name Present only a Directory name"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=testSourceDir & forwardSlash & ""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005;2008.2) - Command Rejected as expected, File Name ",  testSourceDir, forwardSlash, "", " is NOT allowed."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_20082, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else  
  write "<!> Failed (1004;1005;2008.2) - Command Accepted, File Name ",  testSourceDir, forwardSlash, "", " should not be allowed."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_20082, "F"
endif

write "<**> Check that the source directory still exists:"

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testSourceDir)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if (p@$SC_$CPU_FM_FileStatus = "Directory") THEN
    write "<*> Passed (2008) - File status indicates that the directory exists."
  else
    write "<!> Failed (2008) - Directory '",testSourceDir,"' does not exist and should! Dir was Deleted!!"
    ut_setrequirements FM_2008, "F"
    goto PROCEND
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3: Good Dir, non-existent File Name"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=testSourceDir & forwardSlash & "nonexist.tst"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005;2008) - Command Rejected as expected, File Name ",  testSourceDir, forwardSlash, "nonexist.tst", " is NOT allowed."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else  
  write "<!> Failed (1004;1005;2008) - Command Accepted, Source File Name ",  testSourceDir, forwardSlash, "nonexist.tst", " should not be allowed."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Send Delete File Command testing the bounds of the full"
write ";  path names for the File Name."
write ";*********************************************************************"
write ";  Step 3.3.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: File Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID,  "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005;2008) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2008) - Command Accepted zero character file name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3: Create Directory of near maximum allowable length:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=longDirName1

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
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

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
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

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=longDirName3

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
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
write ";  Step 3.3.4: Upload files to the new directories."
write ";*********************************************************************"
;; Upload the Test file to each of the directories created above
s ftp_file (uploadDir2, testFile2, testFile2, "$CPU", "P")
s ftp_file (uploadDir3, testFile2, testFile2, "$CPU", "P")
s ftp_file (uploadDir4, testFile2, testFile2, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.3.5: Test File Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.3.5.1: 63 chars + null."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=fullTestLongName1

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2008) - Delete Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_evs_eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelFile_63 = "Yes"
else
  write "<!> Failed (1008;2008) - Delete Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2008, "F"

  CharsInDelFile_63 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.5.2: 62 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=fullTestLongName2

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2008) - Delete Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelFile_62 = "Yes"
else
  write "<!> Failed (1003;2008) - Delete Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2008, "F"

  CharsInDelFile_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.5.3: 61 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=fullTestLongName3

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (2008) - Delete Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelFile_61 = "Yes"
else
  write "<!> Failed (1003;2008) - Delete Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2008, "F"

  CharsInDelFile_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.6: Upload a file to the root directory."
write ";*********************************************************************"
s ftp_file (ramDirPhys, verySmallName, verySmallName, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.3.7: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=fullTestShortName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2008) - Delete Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2008) - Delete Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2008, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Test the File Delete request via the TST_FM application."
write ";*********************************************************************"
;; Upload the file to the ram directory
s ftp_file (ramDirPhys, testFile, testFile, "$CPU", "P")

ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_DELETEFILE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 2

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

/$SC_$CPU_TST_FM_Delete File=ramDir & forwardSlash & testFile

ut_tlmwait $SC_$CPU_TST_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Delete Request sent properly to TST_FM."
else
  write "<!> Failed - Delete Request rejected."
endif

ut_tlmwait $SC_$CPU_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Event message ", $SC_$CPU_find_event[1].eventid, " received"
else
  write "<!> Failed - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_DELETEFILE_INF_EID, "."
endif

wait 5

;; Verify that the FM Delete File event message did not get generated
if ($SC_$CPU_find_event[2].num_found_messages = 0) then
  write "<*> Passed - FM Delete File Event message was not generated"
else
  write "<!> Failed - FM Delete File Event message received."
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Exercise File Delete All Command."
write ";*********************************************************************"
write ";  Step 4.1: Send Delete All Command with bad command lengths."
write ";*********************************************************************"
write ";  Step 4.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "188CC000004207CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000004207CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000004207CE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_PKT_ERR_EID, "."
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
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "188CC000004007CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000004007CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000004007CE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_PKT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2: Send Delete All Command with invalid Dir names."
write ";*********************************************************************"
write ";  Step 4.2.1: Invalid Path (non-existant Dir):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=ramDir & "/imaginary/"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1004;1006;2007) - DeleteAll Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006;2007) - DeleteAll Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: Send Delete All Command testing the bounds of the full"
write ";  path names for the Directory Name."
write ";*********************************************************************"
write ";  Step 4.3.1: Directory Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.2: Directory Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1004;1006;2007) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1006, "P"
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1006;2007) - Command Accepted zero character directory name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1006, "F"
  ut_setrequirements FM_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3.3: Create Directories of near maximum allowable length:"
write ";*********************************************************************"
write "directories already exist"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.4: Upload files to the new directories."
write ";*********************************************************************"
; 63 + null total path length 
s ftp_file (uploadDir2, testFile2, testFile2, "$CPU","P")

; 62 + null total path length 
s ftp_file (uploadDir3, testFile2, testFile2, "$CPU","P")

; 61 + null total path length 
s ftp_file (uploadDir4, testFile2, testFile2, "$CPU","P")

; 69 + null total path length 
s ftp_file (uploadDir2, testFile, testFile, "$CPU","P")

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5: Test Directory Names of near maximum allowable length, "
write ";  so that the actual files in the directories both exceed and meet   "
write ";  the boundary."
write ";*********************************************************************"
write ";  Step 4.3.5.1: 63 chars + null and larger in directory."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=longDirName1

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2007) - DeleteAll Command Accepted."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelAll_63 = "INSPECT LOG"
else
  write "<!> Failed (1003;1008;2007) - DeleteAll Command Rejected. Should have accepted command"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2007, "F"

  CharsInDelAll_63 = "INSPECT LOG"
endif

write "INSPECT LOG HERE: 1"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5.2: Check that the longer file still exists."
write ";*********************************************************************"
write "<!> This needs to be inspected to see if the file name that exceeds the limits still exists in the directory"

write "<**> Check the contents of ", longDirName1, " to verify the directories still exist"
s $sc_$cpu_fm_dirfiledisplay (longDirName1, ramDir & "/fm_delstress_5.lst", ramDirPhys, "fm_delstress_5.lst")

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5.3: 62 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

local testDirName = longDirName2 & forwardSlash

/$SC_$CPU_FM_DeleteAll DirName=testDirName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2007) - DeleteAll Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelAll_62 = "INSPECT LOG"
else
  write "<!> Failed (1003;2007) - DeleteAll Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"

  CharsInDelAll_62 = "INSPECT LOG"
endif

write "INSPECT LOG HERE: 2"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5.4: Check that the files no longer exist."
write ";*********************************************************************"
write "<!> This needs to be inspected to see if the files have been deleted"

write "<**> Check the contents of ", longDirName2, " to verify the directories still exist"
s $sc_$cpu_fm_dirfiledisplay (testDirName, "/ram/fm_delstress_6.lst", "RAM:0", "fm_delstress_6.lst")

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5.5: 61 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

local testDirName = longDirName3 & forwardSlash

/$SC_$CPU_FM_DeleteAll DirName=testDirName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2007) - DeleteAll Command Accepted."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDelAll_61 = "INSPECT LOG"
else
  write "<!> Failed (1003;2007) - DeleteAll Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"

  CharsInDelAll_61 = "INSPECT LOG"
endif

write "INSPECT LOG HERE: 3"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.5.6: Check that the files no longer exist."
write ";*********************************************************************"
write "<!> This needs to be inspected to see if the files have been deleted"

write "<**> Check the contents of ", longDirName3, " to verify the directories still exist"
s $sc_$cpu_fm_dirfiledisplay (testDirName, "/ram/fm_delstress_7.lst", "RAM:0", "fm_delstress_7.lst")

wait 5

write ";*********************************************************************"
write ";  Step 4.3.6: Upload file to the root directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (ramDirPhys, verySmallName, verySmallName, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 4.3.7: Test Dir Name of smallest possible length."
write ";*********************************************************************"
s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_8.lst", ramDirPhys, "fm_delstress_8.lst")
write "<**> Check the contents of ", ramDir, "/G should exist"

; potentially dangerous

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=ramDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2007) - DeleteAll Command Accepted."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2007) - DeleteAll Command Rejected. If any Files were deleted this should be accepted."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"
endif

s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_9.lst", ramDirPhys, "fm_delstress_9.lst")
write "<**> Check the contents of ", ramDir, " after the step for what files were left, G should not exist"

wait 5

write ";*********************************************************************"
write ";  Step 4.3.8: Test if delete all removes empty directories."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=ramDir & "/emptydir"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (3000) - Create Directory command sent properly."
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_CREATE_DIR_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;3000) - Create Directory command not sent properly."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_10.lst", ramDir, "fm_delstress_10.lst")
write "<**> Check the contents of ", ramDir, " for the directory /emptydir"

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_WARNING_EID, "INFO", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=ramDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2007) - DeleteAll Command Accepted."
  ut_setrequirements FM_2007, "P"

  ; Check on event message 1  
  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  ; Check on event message 2  
  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (2007.2.1) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_200721, "P"
  else
    write "<!> Failed (2007.2.1) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_200721, "F"
  endif
else
  write "<!> Failed (1003;2007) - DeleteAll Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3.9: Check that the directory still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (ramDir & "/emptydir")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if (p@$SC_$CPU_FM_FileStatus = "Directory") THEN
    write "<*> Passed (2007.2) - File status indicates the Directory still exists."
    ut_setrequirements FM_20072, "P"
  else
    write "<!> Failed (2007.2) - File status indicates the Directory was deleted."
    ut_setrequirements FM_20072, "F"
  endif
else
  write "<!> Failed (1003;2007.2;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_20072, "F"
  ut_setrequirements FM_2011, "F"
endif

s $sc_$cpu_fm_dirfiledisplay (ramDir, ramDir & "/fm_delstress_11.lst", ramDir, "fm_delstress_11.lst")
write "<**> Check the contents of ", ramDir, " for the directory /emptydir (it should still exist!!)"

wait 5

PROCEND:

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

write ""
write "63 Characters + NULL allowed in Delete File Name? ", CharsInDelFile_63
write "62 Characters + NULL allowed in Delete File Name? ", CharsInDelFile_62
write "61 Characters + NULL allowed in Delete File Name? ", CharsInDelFile_61
write ""                                              
write "63 Characters + NULL or Larger total path names allowed? ", CharsInDelAll_63
write "62 Characters + NULL total path names allowed? ", CharsInDelAll_62
write "61 Characters + NULL total path names allowed? ", CharsInDelAll_61
write ""


drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_filedelete_stress                        "
write ";*********************************************************************"
ENDPROC
