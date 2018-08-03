PROC $sc_$cpu_fm_filecopy_stress
;*******************************************************************************
;  Test Name:  FM_FileCopy_Stress
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to stress the File Manager (FM) File Copy
;   Command function. The FM_FileCopy command will be tested to see if the
;   FM application handles error cases for these, both expected and unexpected.
;   The FM_FileInfo and FM_DirCreate commands are also used to facilitate the
;   testing, but are not stressed in this scenario.
;
;  Requirements Tested
;    FM1002	If the computed length of any FM command is not equal to the
;		length contained in the message header, FM shall reject the
;		command
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
;    FM2002	Upon receipt of a File Copy command, FM shall copy the
;		command-specified file to the command-specified destination file
;    FM2002.1	If the command-specified destination file exists, FM shall
;		reject the command
;    FM2008	Upon receipt of a Delete command, FM shall delete the
;		command-specified file
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
;    -Copying files across different memory types is only tested to the
;     current extent of the available test bed. It is up to the
;     individual projects to modify the procedure or create a new one
;     to test copying between all of their available memory types or
;     drives.
;    -The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	06/11/08   D. Stewart	Original Procedure
;	12/03/08   W. Moleski	Updated to handle Requirement 2002.1 and
;				general cleanup
;	01/26/10   W. Moleski	Updated for FM 2.1.0.0 and moved special
;				character tests to a separate procedure
;       02/28/11   W. Moleski   Added variables for App name and ram directory
;       05/10/12   W. Moleski   Added checks for non-ram files after deletion
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
;
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
#define FM_1008     4
#define FM_2002     5
#define FM_20021    6
#define FM_2008     7
#define FM_3000     8
#define FM_4000     9
#define FM_5000     10

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 10
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1002", "FM_1003", "FM_1004", "FM_1005", "FM_1008", "FM_2002", "FM_2002.1", "FM_2008", "FM_3000", "FM_4000", "FM_5000"]

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

local testFile  = "FMCOPYSTRESS.TST"
local testFile2 = "FMLIMIT.TST"
local testFile3 = "FMZEROLEN.TST"
local testFile4 = "FMLARGE.TST"

local verySmallName = "A"

local uploadDir  = ramDirPhys & "/FMSOURCE"
local uploadDir2 = ramDirPhys & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir3 = ramDirPhys & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir4 = ramDirPhys & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local forwardSlash = "/"

local outFile1 =  "destsplist.tst"
local outFile2 =  "srcsplist.tst"

local fullOriginalTestFileName = testSourceDir & forwardSlash & testFile

local fullTestFileName1  = testSourceDir & forwardSlash & testFile3
local fullTestFileName2  = testSourceDir & forwardSlash & testFile4

local fullTestLongName1  = longDirName1 & forwardSlash & testFile2
local fullTestLongName2  = longDirName2 & forwardSlash & testFile2
local fullTestLongName3  = longDirName3 & forwardSlash & testFile2

local fullTestShortName = ramDir & forwardSlash & verySmallName

local badPath = ramDir & "/imaginary/file.tst"

local CharsInSrcFile_63
local CharsInSrcFile_62
local CharsInSrcFile_61

local CharsInDest_63
local CharsInDest_62
local CharsInDest_61

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
write ";  Step 1.3: Verify that the FM Housekeeping telemetry packet is being"
write ";  generated and that the items specified in the requirements are "
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

/$sc_$cpu_FM_DirCreate DirName= testSourceDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
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

/$sc_$cpu_FM_DirCreate DirName=testDestDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
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
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 3.0: Exercise File Copy to File Command."
write ";*********************************************************************"
write ";  Step 3.1: Send Copy File Command with bad command lengths."
write ";*********************************************************************"
write ";  Step 3.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "188CC000008402CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000008402CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000008402CE"
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
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_PKT_ERR_EID, "."
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
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

rawcmd = ""

;; CPU1 is the default
rawcmd = "188CC000008202CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC000008202CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC000008202CE"
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
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_PKT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1002;1004) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send Copy File Command with invalid Dest file names."
write ";*********************************************************************"
write ";  Step 3.2.1: Invalid Destination Path:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_OS_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 DestName=badPath File=fullOriginalTestFileName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CHILDCMDEC = errcnt))
if ($SC_$CPU_FM_CHILDCMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_OS_ERR_EID, "."
  endif
else
  write "<!> Failed (1004;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Send Copy File Command with invalid Source file names."
write ";*********************************************************************"
write ";  Step 3.3.1: Invalid Source Path:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=badPath DestName=testDestDir & "/invalidSrcPath.tst"

wait until (( $SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))   
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected Counter incremented as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_SRC_ERR_EID, "."
  endif
else
  write "<!> Failed (1004;1005) - Command Rejected Counter did not increment as expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testDestDir & "/invalidSrcPath.tst")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly."
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
write ";  Step 3.3.3: No Source File Name Present"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_SRC_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=testSourceDir & forwardSlash & "" DestName=testDestDir & "/SrcDirOnly.tst"

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1004;1005) - Command Rejected as expected, Source File Name ",  testSourceDir, forwardSlash, "", " is NOT allowed."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else  
  write "<!> Failed (1004;1005) - Command Accepted, Source File Name ",  testSourceDir, forwardSlash, "", " should not be allowed."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.4: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testDestDir & "/SrcDirOnly.tst")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt)) 
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command rejected as expected."
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
write ";  Step 3.4: Send Copy File Command with Destination File Name the"
write ";  same as the Source File Name"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_TGT_ERR_EID, "ERROR", 1

errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=fullOriginalTestFileName

ut_tlmwait $SC_$CPU_FM_CMDEC, {errcnt}
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1004;2002.1) - Command Rejected as expected, Dest File Name cannot be the same as original"
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_20021, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_TGT_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;2002.1) - Command Accepted a Dest File Name the same as the original."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_20021, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Send Copy File Command testing the bounds of the full"
write ";  path names for the Source File Name."
write ";*********************************************************************"
write ";  Step 3.5.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.5.2: File Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File="" DestName=testDestDir & "/LONG111.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - File Copy Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_SRC_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005) - Command Accepted zero character file name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.3: Create Directory of near maximum allowable length:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_DirCreate DirName=longDirName1

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

/$sc_$cpu_FM_DirCreate DirName=longDirName2

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

/$sc_$cpu_FM_DirCreate DirName=longDirName3

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
write ";  Step 3.5.4: Upload file to the new directories."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (uploadDir2, testFile2, testFile2, "$CPU", "P")
wait 5

;; Upload the Test file
s ftp_file (uploadDir3, testFile2, testFile2, "$CPU", "P")
wait 5

;; Upload the Test file
s ftp_file (uploadDir4, testFile2, testFile2, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5: Test File Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.5.5.1: 63 chars + null for File 1 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestLongName1 DestName=testDestDir & "/LONG111.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2002) - Copy Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2002) - Copy Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.2: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testDestDir & "/LONG111.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. Path name of 63 chars was allowed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",testDestDir,"/LONG111.TST' does not exist"
  endif

  CharsInSrcFile_63 = "Yes"
else
  write "<!> Failed (2011) - File Info Command Failed."
  ut_setrequirements FM_2011, "F"

  CharsInSrcFile_63 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.3: 62 chars + null for File 1 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestLongName2 DestName=testDestDir & "/LONG222.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2002) - Copy Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2002) - Copy Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.4: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testDestDir & "/LONG222.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. Path name of 62 chars was allowed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",testDestDir,"/LONG222.TST' does not exist"
  endif

  CharsInSrcFile_62 = "Yes"
else
  write "<!> Failed (2011) - File Info Command Failed."
  ut_setrequirements FM_2011, "F"

  CharsInSrcFile_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.5: 61 chars + null for File 1 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestLongName3 DestName=testDestDir & "/LONG333.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2002) - Copy Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2002) - Copy Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.6: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (testDestDir & "/LONG333.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. Path name of 61 chars was allowed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",testDestDir,"/LONG333.TST' does not exist"
  endif

  CharsInSrcFile_61 = "Yes"
else
  write "<!> Failed (2011) - File Info Command Failed."
  ut_setrequirements FM_2011, "F"

  CharsInSrcFile_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.6: Upload file to the root directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file ("RAM:0", verySmallName, verySmallName, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 3.5.7: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestShortName DestName=testDestDir & "/SHORTEST.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1003;2002) - Copy Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - Copy Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: Send Copy File Command testing the bounds of the full"
write ";  path names for the Destination File Name."
write ";*********************************************************************"
write ";  Step 3.6.1: Create Directory of near maximum allowable length:"
write ";*********************************************************************"
write "Directories already exist"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.2: Upload file to the new directory."
write ";*********************************************************************"
write "Files already present"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.3: Dest File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.4: Dest File Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_TGT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_TGT_ERR_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1004;1005) - Command Accepted zero character file name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5: Test Dest File Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.6.5.1: 63 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=longDirName1 & "/FMLONG1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2002) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2002) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.2: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (longDirName1 & "/FMLONG1.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. Path name of 63 chars was allowed."
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
    write "<!> Failed - expected file '",testDestDir,"/FMLONG1.TST' does not exist"
  endif

  CharsInDest_63 = "Yes"
else
  write "<!> Failed (2011) - File Info Command Failed."
  ut_setrequirements FM_2011, "F"

  CharsInDest_63 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.3: 62 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=longDirName2 & "/FMLONG2.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2002) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2002) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.4: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (longDirName2 & "/FMLONG2.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. Path name of 62 chars was allowed."
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
    write "<!> Failed - expected file '",testDestDir,"/FMLONG2.TST' does not exist"
  endif

  CharsInDest_62 = "Yes"
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"

  CharsInDest_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.5: 61 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=longDirName3 & "/FMLONG3.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2002) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ",FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2002) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.6: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (longDirName3 & "/FMLONG3.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. Path name of 61 chars was allowed."
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
    write "<!> Failed - expected file '",testDestDir,"/FMLONG3.TST' does not exist"
  endif

  CharsInDest_61 = "Yes"
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"

  CharsInDest_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.6: Test Dest File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=ramDir & "/B"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2002) - Copy Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - Copy Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.0: Copy files of different sizes and across memory types."
write ";*********************************************************************"
write ";  Step 4.1: Copy a zero length file."
write ";*********************************************************************"
write ";  Step 4.1.1: Upload zero length file to test Directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (uploadDir, testFile3, testFile3, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 4.1.2: Copy a zero length file."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestFileName1 DestName=testDestDir & "/FMZEROLEN1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2002) - Copy Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - Copy Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.2:  Copy an excessively large file."
write ";*********************************************************************"
write ";  Step 4.2.1: Upload excessively large file to test Directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (uploadDir, testFile4, testFile4, "$CPU", "P")

wait 5

write ";*********************************************************************"
write ";  Step 4.2.2: Copy an excessively large file. Depending upon the size"
write ";  of the disk, this command may or may not work. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_OS_ERR_EID, "ERROR", 2

;; Setup to look for the Child Task Accepted or Rejected command counters
cmdctr = $SC_$CPU_FM_CHILDCMDPC + 1
errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestFileName2 DestName=testDestDir & "/FMLARGE1.TST"

wait until (($SC_$CPU_FM_CHILDCMDPC = cmdctr) OR ($SC_$CPU_FM_CHILDCMDEC = errcnt))
if ($SC_$CPU_FM_CHILDCMDPC = cmdctr) then
  write "<*> Passed (1003;2002) - Copy Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
elseif ($SC_$CPU_FM_CHILDCMDEC = errcnt) then
  write "<*> Passed (1004;2002) - Copy Command Rejected. There may not be enough room on the disk for this operation."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[2].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_OS_ERR_EID, "."
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (2002) - Copy Command did not execute properly."
  ut_setrequirements FM_2002, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 4.3: Copy files across available memory types."
write ";*********************************************************************"
write ";  Step 4.3.1: Copy Regular file across available memory types."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullOriginalTestFileName DestName=testDestDir2 & forwardSlash & testFile

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2002) - Copy Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - Copy Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
endif

s $sc_$cpu_fm_fileinfodisplay (testDestDir2 & forwardSlash & testFile)

wait 5

write ";*********************************************************************"
write ";  Step 4.3.2:  Copy really large file across available memory types."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1

;; NOTE: Since this is a large file, the copy command may take some time to
;; complete. Also, since this command is passed to the Child Task, the CMDPC
;; increments if the command is good. The CHILDCMDPC increments when the
;; command completes 
local childCmdCtr = $SC_$CPU_FM_CHILDCMDPC + 1
cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_FileCopy Overwrite=0 File=fullTestFileName2 DestName=testDestDir2 & forwardSlash & testFile4

wait until ((($SC_$CPU_FM_CMDPC = cmdctr) AND ;;
	     ($SC_$CPU_FM_CHILDCMDPC = childCmdCtr)) OR ;;
	     ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr AND $SC_$CPU_FM_CHILDCMDPC = childCmdCtr) then
  write "<*> Passed (1003;2002) - Copy Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2002, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_COPY_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;2002) - Copy Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2002, "F"
endif

s $sc_$cpu_fm_fileinfodisplay (testDestDir2 & forwardSlash & testFile4)

wait 5

write ";*********************************************************************"
write ";  Step 4.4: Remove files from non-RAM file systems."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_Delete File=testDestDir2 & forwardSlash & testFile

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

s $sc_$cpu_fm_fileinfodisplay (testDestDir2 & forwardSlash & testFile)

wait 5

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$sc_$cpu_FM_Delete File=testDestDir2 & forwardSlash & testFile4

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

;; Verify the file has been deleted
s $sc_$cpu_fm_fileinfodisplay (testDestDir2 & forwardSlash & testFile4)

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
write "63 Characters + NULL allowed in Src File Name? ", CharsInSrcFile_63
write "62 Characters + NULL allowed in Src File Name? ", CharsInSrcFile_62
write "61 Characters + NULL allowed in Src File Name? ", CharsInSrcFile_61
write ""
write "63 Characters + NULL allowed in Dest File Name? ", CharsInDest_63
write "62 Characters + NULL allowed in Dest File Name? ", CharsInDest_62
write "61 Characters + NULL allowed in Dest File Name? ", CharsInDest_61
write ""

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_fm_filecopy_stress                          "
write ";*********************************************************************"
ENDPROC
