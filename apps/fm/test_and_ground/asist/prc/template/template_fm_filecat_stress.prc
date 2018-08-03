PROC $sc_$cpu_fm_filecat_stress
;*******************************************************************************
;  Test Name:  FM_FileCat_Stress
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to stress the File Manager (FM) File
;   Concatenate Command function. The FM_FileConcatenate command will be
;   tested to see if the FM application handles error cases for these, both
;   expected and unexpected. The FM_FileInfo and FM_DirCreate commands are
;   also used to facilitate the testing, but are not stressed in this scenario.
;
;  Requirements Tested
;    FM1002	If the computed length of any FM command is not equal to
;		the length contained in the message header, FM shall reject
;		the command
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command  Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command
;		execution, increment the FM Command Rejected Counter and
;		issue an error event message
;    FM1005	If the filename specified in any FM command is not valid, FM
;		shall reject the command
;    FM1008	The CFS FM FSW shall utilize full path specifications having a
;		maximum length of <MISSION_DEFINED> characters for all command
;		input arguments requiring a file or pathname.
;    FM2010	Upon receipt of a Concatenate command, FM shall concatenate the
;		command-specified file with the second command-specified file,
;		copying the result to the command-specified destination file.
;    FM2011	Upon receipt of a File Info command, FM shall generate a message
;		containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open, Closed, or Nonexistent)
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
;    -The CFE & FM are up and running and ready to accept commands. 
;    -The FM commands and TLM items exist in the GSE database. 
;    -The display page for the FM telemetry exists. 
;
;  Assumptions and Constraints
;    -The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	09/03/08   D. Stewart	Original Procedure
;	01/25/10   W. Moleski	Updated for FM 2.1.0.0, moved special character
;				tests to separate proc and general cleanup
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
;	None
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
#define FM_2010     5
#define FM_2011     6
#define FM_3000     7
#define FM_4000     8
#define FM_5000     9

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 9
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1002", "FM_1003", "FM_1004", "FM_1005", "FM_1008", "FM_2010", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local rawcmd

; more for reference than use, but used for raw commands, hex values:
local slashInHex   = "2F"
local endOfStringChar = "00"

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir = ramDir & "/FMTEST"
local spDir1  = ramDir & "/SP1"
local spDir2  = ramDir & "/SP2"
local outDir  = ramDir & "/OUT"

;; /ram + 47 characters -> will make 63 chars + null with testFile2 name
local longDirName1 = ramDir & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
;; /ram + 46 characters -> will make 62 chars + null with testFile2 name
local longDirName2 = ramDir & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
;; /ram + 45 characters -> will make 61 chars + null with testFile2 name
local longDirName3 = ramDir & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local testFile  = "FMCAT1.TST"
local testFile1 = "FMCAT2.TST"
local testFile2 = "FMLIMIT.TST"

local verySmallName = "A"

local uploadDir  = ramDirPhys & "/FMTEST"
local uploadDir2 = ramDirPhys & "/THISISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir3 = ramDirPhys & "/THISIREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"
local uploadDir4 = ramDirPhys & "/THISREALLYLONGDIRECTORYNAMEFORTHETESTINGOFFM"

local forwardSlash = "/"

local fullTestFile1Name = testDir & forwardSlash & testFile
local fullTestFile2Name = testDir & forwardSlash & testFile1

local fullTestLongName1  = longDirName1 & forwardSlash & testFile2
local fullTestLongName2  = longDirName2 & forwardSlash & testFile2
local fullTestLongName3  = longDirName3 & forwardSlash & testFile2

local fullTestShortName = ramDir & forwardSlash & verySmallName

local badPath = ramDir & "/imaginary/file.tst"

local outputfile = "outfile"
local extension = ".tst"

local outfilenum = 1

local filesize1
local filesize2

local CharsInFile1_63
local CharsInFile1_62
local CharsInFile1_61

local CharsInFile2_63
local CharsInFile2_62
local CharsInFile2_61

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

write ""
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
write ";  Step 2.1: Send Create Directory Command to Create Test Directories."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName= testDir

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

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=outdir

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

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=spDir1

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

ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DirCreate DirName=spDir2

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
write ";  Step 2.2: Upload Test Files to test Directory."
write ";*********************************************************************"
;; Upload the Test file
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")

;; Upload the Test file
s ftp_file (uploadDir, testFile1, testFile1, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0: Exercise File Concatenate Command."
write ";*********************************************************************"
write ";  Step 3.1: Send Concatenate File Commands with bad command lengths."
write ";*********************************************************************"
write ";  Step 3.1.1: Length too long:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000C209CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000C209CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000C209CE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - File Cat command rejected as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1002) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1002, "P"
  else
    write "<!> Failed (1002) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_PKT_ERR_EID
    ut_setrequirements FM_1002, "F"
  endif
else
  write "<!> Failed (1002;1004) - File Cat command completed, cmd length invalid."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.1.2: Length too short:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_PKT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;; CPU1 is the default
rawcmd = "188CC00000C009CE"

if ("$CPU" = "CPU2") then
  rawcmd = "198CC00000C009CE"
elseif ("$CPU" = "CPU3") then
  rawcmd = "1A8CC00000C009CE"
endif

ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1002;1004) - File Cat command rejected as expected."
  ut_setrequirements FM_1002, "P"
  ut_setrequirements FM_1004, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1002) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1002, "P"
  else
    write "<!> Failed (1002) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_PKT_ERR_EID
    ut_setrequirements FM_1002, "F"
  endif
else
  write "<!> Failed (1002;1004) - File Cat command completed, cmd length invalid."
  ut_setrequirements FM_1002, "F"
  ut_setrequirements FM_1004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Send Concat File Command with invalid Src File Names."
write ";*********************************************************************"
write ";  Step 3.2.1: Invalid Src file 1 names:"
write ";*********************************************************************"
write ";  Step 3.2.1.1: Invalid Path (non-existant dir and file name):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC1_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=badPath File2=fullTestFile2Name DestName= outDir & forwardSlash & outputfile & outfilenum & extension

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - File Cat command rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005) - File Cat command completed when failure expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.1.2: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & forwardSlash & outputfile & outfilenum & extension)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command executed properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Expected Event message received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2010) - File Status indicates File not found."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates destination File exists."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC."
  ut_setrequirements FM_2011, "F"
endif

outfilenum = outfilenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 3.2.1.3: Valid Dir but no File specified:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC1_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1= testdir & forwardSlash & "" File2=fullTestFile2Name DestName= outDir & forwardSlash & outputfile & outfilenum & extension

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.1.4: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & forwardSlash & outputfile & outfilenum & extension)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2010) - File Status indicates File not found."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates destination File exists."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC."
  ut_setrequirements FM_2011, "F"
endif

outfilenum = outfilenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3: Invalid Src file 2 names:"
write ";*********************************************************************"
write ";  Step 3.2.3.1: Invalid Path (non-existant dir and file name):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC2_ERR_EID "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=badPath DestName= outDir & forwardSlash & outputfile & outfilenum & extension

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC2_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command accepted when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3.2: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & forwardSlash & outputfile & outfilenum & extension)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2010) - File Status indicates File not found."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates destination File exists."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC."
  ut_setrequirements FM_2011, "F"
endif

outfilenum = outfilenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3.3: Valid Dir but no File specified:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC2_ERR_EID "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2= testdir & forwardSlash & "" DestName= outDir & forwardSlash & outputfile & outfilenum & extension

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC2_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.3.4: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & forwardSlash & outputfile & outfilenum & extension)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2010) - File Status indicates File not found."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates destination File exists."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC."
  ut_setrequirements FM_2011, "F"
endif

outfilenum = outfilenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 3.2.5: Invalid Src file names continued:"
write ";*********************************************************************"
write ";  Step 3.2.5.1: Both src are existing Dirs but no Files specified:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC1_ERR_EID "ERROR", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC2_ERR_EID "ERROR", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1= spDir1 & forwardSlash & "" File2= spDir2 & forwardSlash & "" DestName= outDir & forwardSlash & outputfile & outfilenum & extension

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.5.2: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & forwardSlash & outputfile & outfilenum & extension)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command sent properly."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2010) - File Status indicates File not found."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates destination File exists."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (2011) - File Info command incremented the CMDEC."
  ut_setrequirements FM_2011, "F"
endif

outfilenum = outfilenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 3.2.5.3: Log the size of the next source file:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFile1Name)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
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

filesize1 = $SC_$CPU_FM_InfoFileSize

write "Info packet file size for ", fullTestFile1Name, ": ", filesize1

wait 5

write ";*********************************************************************"
write ";  Step 3.2.5.4: Src files are same file:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile1Name DestName= outDir & forwardSlash & outputfile & outfilenum & extension

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2010) - File Cat command sent properly."
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  endif
else
  write "<!> Failed (1003;2010) - File Cat command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2.5.5: Check that output file exists and is larger than"
write ";  previously:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay ( outDir & forwardSlash & outputfile & outfilenum & extension )

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
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

filesize2 = $SC_$CPU_FM_InfoFileSize

write "Info packet file size for ", outDir, forwardSlash, outputfile, outfilenum, extension, ": ", filesize2

if ( filesize2 > filesize1 ) THEN
  write "<*> Passed - File Size is larger. Src file was: ", filesize1, " New file is: ", filesize2
else
  write "<!> Failed - File size is not larger. Src file was: ", filesize1, " New file is: ", filesize2
endif

outfilenum = outfilenum + 1

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Invalid Dest file:"
write ";*********************************************************************"
write ";  Step 3.3.1: Invalid Path (non-existant dir and file names):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_OS_ERR_EID, "ERROR", 1

;; NOTE: Since the FileCat command is sent to a child task for execution, the
;; FM_CMDPC increments indicating a good command was issued
;; Upon execution of the command, the Error Event is issued and the 
;; FM_ChildCMDEC counter increments
cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CHILDCMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName=badPath

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CHILDCMDEC = errcnt))
if ($SC_$CPU_FM_CHILDCMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_OS_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: Check that output file does not exist:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (badPath)

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info command rejected as expected."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_GET_FILE_INFO_CMD_EID
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Not Used") THEN
    write "<*> Passed (2010) - File Status indicates File not found."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates destination File exists."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3: existing Dir but no File specified:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_TGT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName= testdir & forwardSlash & ""

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_TGT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4: Dest File names the same as the src file names:"
write ";*********************************************************************"
write ";  Step 3.4.1: Same as Src file 1:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_TGT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName=fullTestFile1Name

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_TGT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4.2: Same as Src file 2:"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_TGT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName=fullTestFile2Name

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1005;2010) - File Cat command rejected as expected."
  ut_setrequirements FM_1005, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_TGT_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005;2010) - File Cat command completed when failure was expected."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Send Cat File Commands testing the bounds of the full"
write ";            path names for the Source File 1 Name."
write ";*********************************************************************"
write ";  Step 3.5.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.5.2: File Name 1 with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC1_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;;; set up a command with correct length, but has 0 characters in the full path src file 1 name
;;;; CPU1 is the default
;;rawcmd = "188CC00000C109CE"
;;
;;if ("$CPU" = "CPU2") then
;;  rawcmd = "198CC00000C109CE"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1A8CC00000C109CE"
;;endif
;;
;;; this will add the "0 character" src file name to the command header set above
;;rawcmd = rawcmd & endOfStringChar
;;
;;ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

;;*** No need to send a raw command when the you can issue the command below

/$SC_$CPU_FM_FileCat File1="" File2=fullTestFile2Name DestName=fullTestFile2Name

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
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

/$SC_$CPU_FM_DirCreate DirName=longDirName1

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
write ";  Step 3.5.4: Upload files to the new directories."
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
write ";  Step 3.5.5: Test File 1 Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.5.5.1: 63 chars + null for File 1 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestLongName1 File2=fullTestFile2Name DestName= outDir & "/LONGFILE1_1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.2: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/LONGFILE1_1.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 63 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  ;; Test the FileStatus. It should be 'Closed'
  if (p@$SC_$CPU_FM_FileStatus = "Closed") THEN
    write "<*> Passed (2010) - File Status indicates File is closed."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates '",p@$SC_$CPU_FM_FileStatus,"'. Expected 'Closed'."
    ut_setrequirements FM_2010, "F"
  endif

  CharsInFile1_63 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInFile1_63 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.3: 62 chars + null for File 1 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestLongName2 File2=fullTestFile2Name DestName= outDir & "/LONGFILE1_2.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.4: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/LONGFILE1_2.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 62 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  ;; Test the FileStatus. It should be 'Closed'
  if (p@$SC_$CPU_FM_FileStatus = "Closed") THEN
    write "<*> Passed (2010) - File Status indicates File is closed."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates '",p@$SC_$CPU_FM_FileStatus,"'. Expected 'Closed'."
    ut_setrequirements FM_2010, "F"
  endif

  CharsInFile1_62 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInFile1_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.5: 61 chars + null for File 1 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestLongName3 File2=fullTestFile2Name DestName= outDir & "/LONGFILE1_3.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.5.6: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/LONGFILE1_3.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 61 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  ;; Test the FileStatus. It should be 'Closed'
  if (p@$SC_$CPU_FM_FileStatus = "Closed") THEN
    write "<*> Passed (2010) - File Status indicates File is closed."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates '",p@$SC_$CPU_FM_FileStatus,"'. Expected 'Closed'."
    ut_setrequirements FM_2010, "F"
  endif

  CharsInFile1_61 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInFile1_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.6: Upload file to the root directory."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (ramDirPhys, verySmallName, verySmallName, "$CPU","P")

wait 5

write ";*********************************************************************"
write ";  Step 3.5.7: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestShortName File2=fullTestFile2Name DestName= outDir & "/SHORTEST1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2010) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5.7.1: Verify out file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/SHORTEST1.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (2010) - File Info Command Completed. File exists."
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  ;; Test the FileStatus. It should be 'Closed'
  if (p@$SC_$CPU_FM_FileStatus = "Closed") THEN
    write "<*> Passed (2010) - File Status indicates File is closed."
    ut_setrequirements FM_2010, "P"
  else
    write "<!> Failed (2010) - File Status indicates '",p@$SC_$CPU_FM_FileStatus,"'. Expected 'Closed'."
    ut_setrequirements FM_2010, "F"
  endif
else
  write "<!> Failed (1003;2010) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6: Send Cat File Commands testing the bounds of the full"
write ";            path names for the Source File 2 Name."
write ";*********************************************************************"
write ";  Step 3.6.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.2: File Name 2 with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC2_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;;; set up a command with correct length, but has 0 characters in the full path src file 2 name
;;;; CPU1 is the default
;;rawcmd = "188CC00000C109CE"
;;
;;if ("$CPU" = "CPU2") then
;;  rawcmd = "198CC00000C109CE"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1A8CC00000C109CE"
;;endif
;;
;;; this will add the "0 character" src file name 2 to the command header set above
;;rawcmd = rawcmd & longDirInHex & slashInHex & testFile2InHex & endOfStringChar & ;; This is the end of the src file 1
;;                  endOfStringChar  ; this may have to be expanded and maybe not
;;
;;ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2="" DestName= outDir & "/LONGFILE2_1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (1004;1005) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005) - Command Accepted zero character file name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.3: Create Directories of near maximum allowable lengths:"
write ";*********************************************************************"
write "Directories already exist"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.4: Upload files to the new directories."
write ";*********************************************************************"
write "Files already present"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5: Test File 2 Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.6.5.1: 63 chars + null for File 2 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1= fullTestFile1Name File2= fullTestLongName1 DestName= outDir & "/LONGFILE2_1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.2: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/LONGFILE2_1.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 63 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInFile2_63 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInFile2_63 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.3: 62 chars + null for File 2 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1= fullTestFile1Name File2= fullTestLongName2 DestName= outDir & "/LONGFILE2_2.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.4: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/LONGFILE2_2.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 62 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInFile2_62 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInFile2_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.5: 61 chars + null for File 2 name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestLongName3 DestName= outDir & "/LONGFILE2_3.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.5.6: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/LONGFILE2_3.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 61 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInFile2_61 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInFile2_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";    Step 3.6.6: Upload file to the root directory."
write ";*********************************************************************"
write "File already exists"

wait 5

write ";*********************************************************************"
write ";  Step 3.6.7: Test File Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestShortName DestName= outDir & "/SHORTEST2.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;1008;2010) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;1008;2010) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.6.7.1: Verify out file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (outDir & "/SHORTEST2.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;2010) - File Info Command Completed. File exists."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003;1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7: Send Cat File Commands testing the bounds of the full"
write ";            path names for the Dest File Name."
write ";*********************************************************************"
write ";  Step 3.7.1: File Name longer than maximum allowed length:"
write ";*********************************************************************"
write "code inspected: all cmd parameters are forced to be null terminated"

wait 5

write ";*********************************************************************"
write ";  Step 3.7.2: Dest Name with 0 characters (if possible):"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_TGT_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

;;; set up a command with correct length, but has 0 characters in the full path dest file name
;;;; CPU1 is the default
;;rawcmd = "188CC00000C109CE"
;;
;;if ("$CPU" = "CPU2") then
;;  rawcmd = "198CC00000C109CE"
;;elseif ("$CPU" = "CPU3") then
;;  rawcmd = "1A8CC00000C109CE"
;;endif
;;
;;; this will add the "0 character" dest file name to the command header set above
;;rawcmd = rawcmd & longDirInHex & slashInHex & testFile2InHex & endOfStringChar & ;; This is the end of the src file 1
;;                  longDirInHex & slashInHex & testFile2InHex & endOfStringChar & ;; This is the end of the src file 2
;;                  endOfStringChar  ; this may have to be expanded and maybe not
;;
;;ut_sendrawcmd "$SC_$CPU_FM", (rawcmd)

;;*** No need to send a raw command when you can issue the command below

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName=""
wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDEC = errcnt ) then
  write "<*> Passed (1004;1005) - Command Rejected as expected."
  ut_setrequirements FM_1004, "P"
  ut_setrequirements FM_1005, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
  else
    write "<!> Failed (1004) - Did not receive expected EM. Rcvd ", $SC_$CPU_evs_eventid, " expected ",FM_CONCAT_SRC1_ERR_EID
    ut_setrequirements FM_1004, "F"
  endif
else
  write "<!> Failed (1004;1005) - Command Accepted zero character file name!"
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_1005, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.3: Create Directories of near maximum allowable lengths:"
write ";*********************************************************************"
write "Directories already exist"

wait 5

write ";*********************************************************************"
write ";  Step 3.7.4: Upload files to the new directories."
write ";*********************************************************************"
write "Files already present"

wait 5

write ";*********************************************************************"
write ";  Step 3.7.5: Dest File Name of maximum allowable length."
write ";*********************************************************************"
write ";  Step 3.7.5.1: 63 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName= longDirName1 & "/OUTLIM1.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 63 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 63 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.5.2: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (longDirName1 & "/OUTLIM1.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 63 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDest_63 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInDest_63 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.5.3: 62 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName= longDirName2 & "/OUTLIM2.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 62 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 62 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.5.4: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (longDirName2 & "/OUTLIM2.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ( $SC_$CPU_FM_CMDPC = cmdctr ) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 62 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDest_62 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInDest_62 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.5.5: 61 chars + null for Dest name."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1=fullTestFile1Name File2=fullTestFile2Name DestName= longDirName3 & "/OUTLIM3.TST"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - Command Accepted. Accepted 61 char full path spec"
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1008;2010) - Command Rejected. Should have accepted 61 char full path spec"
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.5.6: Check that the output file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (longDirName3 & "/OUTLIM3.TST")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1008;2010) - File Info Command Completed. Path name of 61 chars was allowed."
  ut_setrequirements FM_1008, "P"
  ut_setrequirements FM_2010, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif

  CharsInDest_61 = "Yes"
else
  write "<!> Failed (1008;2010) - File Info Command Failed."
  ut_setrequirements FM_1008, "F"
  ut_setrequirements FM_2010, "F"

  CharsInDest_61 = "No"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.6: Test Dest Name of smallest possible length."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_FileCat File1= fullTestFile1Name File2= fullTestFile2Name DestName= ramDir & "/B"

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (1003;2010) - Command Accepted."
  ut_setrequirements FM_1003, "P"
  ut_setrequirements FM_2010, "P"
else
  write "<!> Failed (1003;2010) - Command Rejected."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2010, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements FM_1003, "P"
else
  write "<!> Failed (1003) - Event message ", $SC_$CPU_evs_eventid," received. Expected Event message ", FM_CONCAT_CMD_EID, "."
  ut_setrequirements FM_1003, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.7.6.1: Verify out file exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

s $sc_$cpu_fm_fileinfodisplay (ramDir & "/B")

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2011) - File Info Command Completed. File exists."
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

wait 5

PROCEND:

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
write "63 Characters + NULL allowed in File Name 1? ", CharsInFile1_63
write "62 Characters + NULL allowed in File Name 1? ", CharsInFile1_62
write "61 Characters + NULL allowed in File Name 1? ", CharsInFile1_61
write ""
write "63 Characters + NULL allowed in File Name 2? ", CharsInFile2_63
write "62 Characters + NULL allowed in File Name 2? ", CharsInFile2_62
write "61 Characters + NULL allowed in File Name 2? ", CharsInFile2_61
write ""
write "63 Characters + NULL allowed in Dest File Name? ", CharsInDest_63
write "62 Characters + NULL allowed in Dest File Name? ", CharsInDest_62
write "61 Characters + NULL allowed in Dest File Name? ", CharsInDest_61
write ""

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_filecat_stress                           "
write ";*********************************************************************"
ENDPROC
