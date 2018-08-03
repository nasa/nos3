PROC $sc_$cpu_fm_fileinfo_stress
;*******************************************************************************
;  Test Name:  FM_FileInfo_Stress
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to stress the File Manager (FM) File Info
;   Command function. The FM_FileInfo command will be tested to see if the FM
;   application handles error cases for these, both expected and unexpected.
;   The FM_DirCreate command is also used to facilitate the testing, but is not
;   stressed in this scenario.
;
;  Requirements Tested
;    FM1002	If the computed length of any FM command is not equal to
;		the length contained in the message header, FM shall reject
;		the command
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command
;		execution, increment the FM Command Rejected Counter and
;		issue an error event message
;    FM1005	If the filename specified in any FM command is not valid, FM
;		shall reject the command
;    FM1008	The CFS FM FSW shall utilize full path specifications having a
;		maximum length of <PLATFORM_DEFINED> characters for all command
;		input arguments requiring a file or pathname.
;    FM2011	Upon receipt of a File Info command, FM shall generate a message
;		containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open, Closed, or Non-existent)
;			d) <MISSION_DEFINED> CRC
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified file system
;    FM4000	FM shall generate a housekeeping message containing the
;		following:
;			a) Valid Command Counter
;			b) Command Rejected Counter
;			c) For each file system: Total number of open files 
;    FM5000	Upon initialization of the FM Application, FM shall
;		initialize the following data to Zero
;			a) Valid Command Counter
;			b) Command Rejected Counter
;
;
;  Prerequisite Conditions
;    The CFE & FM are up and running and ready to accept commands. 
;    The FM commands and TLM items exist in the GSE database. 
;    The display page for the FM telemetry exists. 
;
;  Assumptions and Constraints
;	 The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	06/06/08   D. Stewart	Original Procedure
;	12/10/08   W. Moleski	General cleanup
;	01/28/10   W. Moleski	Updated for FM 2.1.0.0 and more cleanup
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

%liv (log_procedure) = logging

#define FM_1002     0
#define FM_1003     1
#define FM_1004     2
#define FM_1005     3
#define FM_1008     4
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

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1002", "FM_1003", "FM_1004", "FM_1005", "FM_1008", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local rawcmd

local endOfStringChar = "00"

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir = ramDir & "/FMTEST"

;; /ram + 47 characters -> will make 63 chars + null with testFile2 name
local longDirName1 = ramDir & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
;; /ram + 46 characters -> will make 62 chars + null with testFile2 name
local longDirName2 = ramDir & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
;; /ram + 45 characters -> will make 61 chars + null with testFile2 name
local longDirName3 = ramDir & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local testFile  = "FMINFOSTRESS.TST"
local testFile2 = "FMLIMIT.TST"
local verySmallName = "A"

local uploadDir  = ramDir & "/FMTEST"
local uploadDir2 = ramDir & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir3 = ramDir & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir4 = ramDir & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local forwardSlash = "/"

local fullTestFileName = testDir & forwardSlash & testFile

local fullTestLongName1  = longDirName1 & forwardSlash & testFile2
local fullTestLongName2  = longDirName2 & forwardSlash & testFile2
local fullTestLongName3  = longDirName3 & forwardSlash & testFile2

local fullTestShortName = ramDir & forwardSlash & verySmallName

local badPath = ramDir & "/imaginary/file.tst"

local CharsInFile_63
local CharsInFile_62
local CharsInFile_61

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
write ";  Step 2.1: Send Create Directory Command to Create a Test Directory."
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG"

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$sc_$cpu_FM_DirCreate DirName=testDir
wait 5

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
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
write ";  Step 2.2: Upload Test File to test Directory."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU","P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0: Exercise File Info Command."
write ";*********************************************************************"
write ";  Step 3.1: Send File Info Command with bad command lengths."
write ";*********************************************************************"
write ";  Step 3.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000460ACE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000460ACE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000460ACE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if (errcnt = $SC_$CPU_FM_CMDEC) then
  write "<*> Passed (1002;1004) - File Info command rejected as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_PKT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - File Info command completed with an invalid length"
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";    Step 3.1.2: Length too short:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000440ACE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000440ACE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000440ACE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if (errcnt = $SC_$CPU_FM_CMDEC) then
  write "<*> Passed (1002;1004) - File Info command rejected as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_PKT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - File Info command completed with an invalid length"
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send File Info Command with invalid File Names."
write ";*********************************************************************"
write ";  Step 3.2.1: Invalid Path (non-existant dir & file name"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (badPath)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info command rejected as expected."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1004, "F"
  endif

  ;;Check the status of the file
  if (p@$sc_$cpu_FM_FileStatus = "Not Used") THEN
    write "<*> Passed - File Info Telemetry indicates that '",badPath,"' does not exist"
  else
    write "<!> Failed - File Info Telemetry indicates that '",badPath, "' exists"
  endif
else
  write "<!> Failed (1003;2011) - File Info command incremented the CMDEC."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.2: Vaild Dir but no File specified. This will work"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (testdir & forwardSlash & "")

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  ;; Check the File Status 
  if (p@$sc_$cpu_FM_FileStatus = "Directory") THEN
    write "<*> Passed - File Info Telemetry indicates the file is a directory."
  else
    write "<!> Failed - File Info Telemetry indicates a status of '",p@$sc_$cpu_FM_FIleStatus, "'. Expected 'Directory'"
  endif

  if ($sc_$cpu_FM_InfoFileName = testdir & forwardSlash & "") THEN
    write "<*> Passed - File Info Telemetry Matches expected file name: ",testdir & forwardSlash
  else
    write "<!> Failed - File Info Telemetry does not match expected file name: ",testdir & forwardSlash
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3: A File specification containing an invalid character. "
write ";  This should fail"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testdir & forwardSlash & ")",0,"Fail")

if (errcnt = $SC_$CPU_FM_CMDEC) then
  write "<*> Passed (1005;2011) - File Info Command failed as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_SRC_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2011) - File Info Command accepted when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Send File Info Command testing the bounds of the full"
write ";  path names for the File Name."
write ";*********************************************************************"
write ";  Step 3.3.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: cmd parameter is forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: File Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

; set up a command with correct length, but has 0 characters in the full path file name
;; CPU1 is the default
rawcmd = "188CC00000450ACE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000450ACE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000450ACE"
endif

; this will add the "0 character" file name to the command header set above
rawcmd = rawcmd & endOfStringChar

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if (errcnt = $SC_$CPU_FM_CMDEC) then
  write "<*> Passed (1002;1004) - File Info command rejected as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_SRC_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2011) - File Info command Accepted zero character file name."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3: Create Directory of near maximum allowable length:"
write ";*********************************************************************"
ut_setupevt "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG"

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_DirCreate DirName=longDirName1

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
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
ut_setupevt "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG"

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_DirCreate DirName=longDirName2

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
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

ut_setupevt "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG"

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_DirCreate DirName=longDirName3

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1003;3000) - Create Directory command sent properly."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_3000, "P"

  if ($SC_$CPU_num_found_messages = 1) THEN
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
write ";  Step 3.3.4: Upload file to the new directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (uploadDir2, testFile2, testFile2, "$CPU", "P")
wait 5

s ftp_file (uploadDir3, testFile2, testFile2, "$CPU", "P")
wait 5

s ftp_file (uploadDir4, testFile2, testFile2, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.3.5: Test File Name of maximum allowable length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestLongName1)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (1008;2011) - File Info Command Accepted 63 Char + Null full path name."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  CharsInFile_63 = "Yes"
else
  write "<!> Failed (1003;1008;2011) - File Info command Rejected 63 Char + Null full path name."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2011, "F"

  CharsInFile_63 = "No"
endif

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestLongName2)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Accepted 62 Char + Null full path name."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  CharsInFile_62 = "Yes"
else
  write "<!> Failed (1003;1008;2011) - File Info command Rejected 62 Char + Null full path name."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2011, "F"

  CharsInFile_62 = "No"
endif

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestLongName3)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Accepted 61 Char + Null full path name."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  CharsInFile_61 = "Yes"
else
  write "<!> Failed (1003;1008;2011) - File Info command Rejected 61 Char + Null full path name."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2011, "F"

  CharsInFile_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.6: Upload file to the root directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file ("RAM:0", verySmallName, verySmallName, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.3.7: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestShortName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Test the File Info command CRC argument."
write ";*********************************************************************"
write ";  Step 3.4.1: CRC argument of 1 which equates to CRC_8"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestShortName,1)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4.2: CRC argument of 2 which equates to CRC_16"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestShortName,2)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4.3: CRC argument of 3 which equates to CRC_32"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestShortName,3)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4.4: Invalid CRC argument of 4. This should generate the "
write ";  CRC warning event message."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_WARNING_EID, "INFO", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestShortName,4)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received (Cmd received)"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed - Event message ",$SC_$CPU_find_event[2].eventid, " received"
  else
    write "<!> Failed - Expected Event message ",FM_GET_FILE_INFO_WARNING_EID," was not received"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

END:

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

write ""
write "63 Characters + NULL allowed in File Name? " & CharsInFile_63
write "62 Characters + NULL allowed in File Name? " & CharsInFile_62
write "61 Characters + NULL allowed in File Name? " & CharsInFile_61
write ""

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $SC_$CPU_fm_fileinfo_stress                          "
write ";*********************************************************************"
ENDPROC
