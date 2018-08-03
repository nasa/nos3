PROC $sc_$cpu_fm_specialchars3
;*******************************************************************************
;  Test Name:  FM_SpecialChars
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to stress the File Manager (FM) Command
;   functions that have a directory or file name as an argument. The arguments
;   are setup to contain special characters and this procedure documents which
;   characters are valid and which ones are not.
;
;  Requirements Tested
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command  Counter and issue
;		an event message
;    FM3002.1	FM shall issue an event message that reports:
;			a) The number of filenames written to the specified file
;			b) The total number of files in the directory 
;			c) The command-specified file's filename
;			d) Size of the Directory Listing file written
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
;	01/20/10   W. Moleski   Created this procedure and moved these tests
;				from other procedures into this one.
;       02/12/10   W. Moleski   Copied this procedure from the original and
;                               removed all but the Mid test in order to
;                               make the test run shorter.
;       03/01/11   W. Moleski   Added variables for App name and ram directory
;       01/06/15   W. Moleski   Modified CMD_EID events from INFO to DEBUG
;
;  Arguments
;	None
;
;  Procedures Called
;
;	Name				Description
;       ftp_file                        Used to upload files needed by this test
;       $sc_$cpu_fm_clearallpages       Removes all the FM pages from the screen
;       $sc_$cpu_fm_dirfiledisplay      Issues the DirListFile command and
;                                       performs the necessary actions to 
;                                       display the contents in the page
;       $sc_$cpu_fm_dirtlmdisplay       Issues the DirListTlm command
;       $sc_$cpu_fm_startfmapps         Starts the FM and TST_FM applications
;       ut_pfindicate                   Prints the status of a requirement
;       ut_setrequirements              Sets the status of a requirement
;       ut_setupevents                  Sets up an event to capture
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
#define FM_3002_1   1
#define FM_4000     2
#define FM_5000     3

;**********************************************************************
;  Define variables
;**********************************************************************
; GLOBAL Variables

global ut_req_array_size = 3
global ut_requirement[0 .. ut_req_array_size]

FOR i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
ENDDO

; LOCAL Variables
local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_3002.1", "FM_4000", "FM_5000"]

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDestDir   = ramDir & "/FMDEST"
local downloadDir   = ramDirPhys & "/FMDEST"
local fileSystem    = ramDir
local fsDownloadDir = ramDirPhys

local testFileCompressed = "FMDECOMORIG.TST.gz"
local forwardSlash = "/"
 
local beginFileName = "testfile"
local endFileName   = "name"
local extension     = ".tst"
local beginDirName  = "TEST"
local endDirName    = "DIRNAME"

local testFile = testDestDir & forwardSlash & "actualfile.dat"
local fullCompressedFileSpec = testDestDir & forwardSlash & testFileCompressed

local testchar1  = "!"
local testchar2  = "@"
local testchar3  = "~"
local testchar4  = "$"
local testchar5  = "#"
local testchar6  = "^"
local testchar7  = "&"
local testchar8  = "_"
local testchar9  = "-"
local testchar10 = "*"
local testchar11 = "("
local testchar12 = ")"
local testchar13 = "+"
local testchar14 = "="
local testchar15 = "{"
local testchar16 = "}"
local testchar17 = "["
local testchar18 = "]"
local testchar19 = "|"
local testchar20 = "\"
local testchar21 = ":"
local testchar22 = ";"
local testchar23 = """"
local testchar24 = "'"
local testchar25 = "<"
local testchar26 = ","
local testchar27 = ">"
local testchar28 = "?"
local testchar29 = " "
local testchar30 = "%"

local testNameNum
;; Leaving out the '%' special character since it causes problems with ASIST
;;;;;;;;;local totaltests = 30
local totaltests = 29
local listFileNum = 1
local catFileNum = 1

local Invalid_Create = ""
local OSError_Create = ""
local Allowed_Create = ""
local Logged_Create = 0

local Invalid_DirLstTlm = ""
local OSError_DirLstTlm = ""
local Allowed_DirLstTlm = ""
local Logged_DirLstTlm = 0

local Invalid_DirLstFile1 = ""
local OSError_DirLstFile1 = ""
local Allowed_DirLstFile1 = ""
local Logged_DirLstFile1 = 0

local Invalid_DirLstFile2 = ""
local OSError_DirLstFile2 = ""
local Allowed_DirLstFile2 = ""
local Logged_DirLstFile2 = 0

local Invalid_Delete = ""
local OSError_Delete = ""
local Allowed_Delete = ""
local Logged_Delete = 0

local Invalid_Copysrc = ""
local OSError_Copysrc = ""
local Allowed_Copysrc = ""
local Logged_Copysrc = 0

local Invalid_Copydest = ""
local OSError_Copydest = ""
local Allowed_Copydest = ""
local Logged_Copydest = 0

local Invalid_Catsrc1 = ""
local OSError_Catsrc1 = ""
local Allowed_Catsrc1 = ""
local Logged_Catsrc1 = 0

local Invalid_Catsrc2 = ""
local OSError_Catsrc2 = ""
local Allowed_Catsrc2 = ""
local Logged_Catsrc2 = 0

local Invalid_Catdest = ""
local OSError_Catdest = ""
local Allowed_Catdest = ""
local Logged_Catdest = 0

local Invalid_Decomsrc = ""
local OSError_Decomsrc = ""
local Allowed_Decomsrc = ""
local Logged_Decomsrc = 0

local Invalid_Decomdest = ""
local OSError_Decomdest = ""
local Allowed_Decomdest = ""
local Logged_Decomdest = 0

local Invalid_DelFile = ""
local OSError_DelFile = ""
local Allowed_DelFile = ""
local Logged_DelFile = 0

local Invalid_DelAll = ""
local OSError_DelAll = ""
local Allowed_DelAll = ""
local Logged_DelAll = 0

local Invalid_FileInfo = ""
local OSError_FileInfo = ""
local Allowed_FileInfo = ""
local Logged_FileInfo = 0

local Invalid_Movesrc = ""
local OSError_Movesrc = ""
local Allowed_Movesrc = ""
local Logged_Movesrc = 0

local Invalid_Movedest = ""
local OSError_Movedest = ""
local Allowed_Movedest = ""
local Logged_Movedest = 0

local Invalid_Renamesrc = ""
local OSError_Renamesrc = ""
local Allowed_Renamesrc = ""
local Logged_Renamesrc = 0

local Invalid_Renamedest = ""
local OSError_Renamedest = ""
local Allowed_Renamedest = ""
local Logged_Renamedest = 0

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
write ";  Step 2.0: Special Character Tests"
write ";*********************************************************************"
write ";  Step 2.1: Populate $CPU with the required directories and files"
write ";*********************************************************************"
write ";  Step 2.1.1: Create the Destination Directory"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

errcnt = $SC_$CPU_FM_CMDEC + 1
cmdctr = $SC_$CPU_FM_CMDPC + 1

;; Send the command
/$SC_$CPU_FM_DirCreate DirName=testDestDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "DirCreate Command Completed for directory '",testDestDir, "'"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ", FM_CREATE_DIR_CMD_EID, " NOT received."
    ut_setrequirements FM_1003, "F"
  endif
else
  write "<!> Failed (1003) - DirCreate Command incremented the CMDEC. Terminating procedure!!!"
  ut_setrequirements FM_1003, "F"
  goto PROCEND
endif

write ";*********************************************************************"
write ";  Step 2.1.2: Upload the required test files"
write ";*********************************************************************"
s ftp_file (downloadDir, "FMCAT1.TST", "actualfile.dat", "$CPU", "P")
wait 5

s ftp_file (downloadDir, testFileCompressed, testFileCompressed, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 2.2: Special Characters Middle test "
write ";*********************************************************************"
Invalid_Create = """"
OSError_Create = """"
Allowed_Create = """"

Invalid_DirLstTlm = """"
OSError_DirLstTlm = """"
Allowed_DirLstTlm = """"

Invalid_DirLstFile1 = """"
OSError_DirLstFile1 = """"
Allowed_DirLstFile1 = """"
Invalid_DirLstFile2 = """"
OSError_DirLstFile2 = """"
Allowed_DirLstFile2 = """"

Invalid_Delete = """"
OSError_Delete = """"
Allowed_Delete = """"

Invalid_Copysrc = """"
OSError_Copysrc = """"
Allowed_Copysrc = """"
Invalid_Copydest = """"
OSError_Copydest = """"
Allowed_Copydest = """"

Invalid_Catsrc1 = """"
OSError_Catsrc1 = """"
Allowed_Catsrc1 = """"
Invalid_Catsrc2 = """"
OSError_Catsrc2 = """"
Allowed_Catsrc2 = """"
Invalid_Catdest = """"
OSError_Catdest = """"
Allowed_Catdest = """"

Invalid_Decomdest = """"
OSError_Decomdest = """"
Allowed_Decomdest = """"
Invalid_Decomsrc = """"
OSError_Decomsrc = """"
Allowed_Decomsrc = """"

Allowed_FileInfo = """"
Invalid_FileInfo = """"

Invalid_DelFile = """"
OSError_DelFile = """"
Allowed_DelFile = """"
Invalid_DelAll = """"
OSError_DelAll = """"
Allowed_DelAll = """"

Invalid_Movesrc = """"
OSError_Movesrc = """"
Allowed_Movesrc = """"
Invalid_Movedest = """"
OSError_Movedest = """"
Allowed_Movedest = """"

Invalid_Renamesrc = """"
OSError_Renamesrc = """"
Allowed_Renamesrc = """"
Invalid_Renamedest = """"
OSError_Renamedest = """"
Allowed_Renamedest = """"

local testDirName
local testFileName, testFileName2
local testFileNameOnly
local validSpec
local dirListOutFile
local fullDirListOutFile
local concatInFile
local concatOutFile1
local concatOutFile2
local copyOutFile
local decomOutFile
local moveOutFile
local renameOutFile
local testDownloadDir

FOR testNameNum = 1 to totaltests DO
  ;; Setup the directory  and file names for this iteration
  testDirName = fileSystem & forwardSlash & beginDirName & %name("testchar" & testNameNum) & endDirName
  testFileName = testDestDir & forwardSlash & beginFileName & %name("testchar" & testNameNum) & endFileName & extension
  testFileNameOnly = beginFileName & %name("testchar" & testNameNum) & endFileName & extension
  testDownloadDir = fsDownloadDir & forwardSlash & beginDirName & %name("testchar" & testNameNum) & endDirName
  testFileName2 = testDirName & forwardSlash & testFileNameOnly

  ;; Dir Create Command
  ut_setupevents "$SC","$CPU",{FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName}, FM_CREATE_DIR_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC","$CPU",{FMAppName}, FM_CREATE_DIR_SRC_ERR_EID,"ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  /$SC_$CPU_FM_DirCreate DirName=testDirName

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> DirCreate Command rejected for directory '",testDirName, "'"
    validSpec = "No"

    ;; Checking if OS error occurred
    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "OS Error Event message ",$SC_$CPU_find_event[2].eventid," received"
      OSError_Create = OSError_Create & %name("testchar" & testNameNum)
      Logged_Create = Logged_Create + 1
    endif

    ;; Checking if SRC error occurred
    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Source Dir Error Event message ",$SC_$CPU_find_event[3].eventid, " received."
      Invalid_Create = Invalid_Create & %name("testchar" & testNameNum)
      Logged_Create = Logged_Create + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> DirCreate Command Completed for directory '",testDirName, "'"
      validSpec = "Yes"

      Allowed_Create = Allowed_Create & %name("testchar" & testNameNum)
      Logged_Create = Logged_Create + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_CREATE_DIR_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Dir List File Command - Filename
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_TGT_ERR_EID,"ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "No") then
    s $sc_$cpu_fm_dirfiledisplay (testDestDir,testFileName,downloadDir,testFileNameOnly,"Fail")
  else
    s $sc_$cpu_fm_dirfiledisplay (testDestDir,testFileName2,testDownloadDir,testFileNameOnly)
  endif

  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> DirListFile Command rejected for file '",testFileName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ",$SC_$CPU_find_event[2].eventid," received"
      OSError_DirLstFile2 = OSError_DirLstFile2 & %name("testchar" & testNameNum)
      Logged_DirLstFile2 = Logged_DirLstFile2 + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Target File Error Event message ",$SC_$CPU_find_event[3].eventid," received"
      Invalid_DirLstFile2 = Invalid_DirLstFile2 & %name("testchar" & testNameNum)
      Logged_DirLstFile2 = Logged_DirLstFile2 + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> DirListFile Command Completed for file '",testFileName, "'"

      Allowed_DirLstFile2 = Allowed_DirLstFile2 & %name("testchar" & testNameNum)
      Logged_DirLstFile2 = Logged_DirLstFile2 + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
        ut_setrequirements FM_3002_1, "P"
      else
        write "<!> Failed (1003;3002.1) - Event message ", FM_GET_DIR_FILE_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
        ut_setrequirements FM_3002_1, "F"
      endif
    endif
  endif

  ;; File Concatenate Command - Source File 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC1_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    concatInFile = testFileName2
    concatOutFile1 = testDirName & forwardSlash & "file1cat" & catFileNum & extension
  else
    concatInFile = testFileName
    concatOutFile1 = testDestDir & forwardSlash & "file1cat" & catFileNum & extension
  endif

  /$SC_$CPU_FM_FileCat File1=concatInFile File2=testFile DestName=concatOutFile1

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> FileCat Command Src1 rejected for file '",testFileName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Catsrc1 = OSError_Catsrc1 & %name("testchar" & testNameNum)
      Logged_Catsrc1 = Logged_Catsrc1 + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "Source File1 Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Catsrc1 = Invalid_Catsrc1 & %name("testchar" & testNameNum)
      Logged_Catsrc1 = Logged_Catsrc1 + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> FileCat Command Src1 completed for file '",testFileName, "'"
      Allowed_Catsrc1 = Allowed_Catsrc1 & %name("testchar" & testNameNum)
      Logged_Catsrc1 = Logged_Catsrc1 + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; File Concatenate Command - Source File 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_SRC2_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    concatOutFile2 = testDirName & forwardSlash & "file2cat" & catFileNum & extension
  else
    concatOutFile2 = testDestDir & forwardSlash & "file2cat" & catFileNum & extension
  endif

  /$SC_$CPU_FM_FileCat File1=testFile File2=concatInFile DestName=concatOutFile2

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> FileCat Command Src2 rejected for file '",testFileName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Catsrc2 = OSError_Catsrc2 & %name("testchar" & testNameNum)
      Logged_Catsrc2 = Logged_Catsrc2 + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "Source File2 Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Catsrc2 = Invalid_Catsrc2 & %name("testchar" & testNameNum)
      Logged_Catsrc2 = Logged_Catsrc2 + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> FileCat Command Src2 completed for file '",testFileName, "'"
      Allowed_Catsrc2 = Allowed_Catsrc2 & %name("testchar" & testNameNum)
      Logged_Catsrc2 = Logged_Catsrc2 + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  catFileNum = catFileNum + 1

  ;; File Concatenate Command - Destination File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CONCAT_TGT_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  /$SC_$CPU_FM_FileCat File1=testFile File2=testFile DestName=testFileName

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> FileCat Command Destintation rejected for file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Catdest = OSError_Catdest & %name("testchar" & testNameNum)
      Logged_Catdest = Logged_Catdest + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "Destination File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Catdest = Invalid_Catdest & %name("testchar" & testNameNum)
      Logged_Catdest = Logged_Catdest + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> FileCat Command Destination completed for file '",testFileName, "'"
      Allowed_Catdest = Allowed_Catdest & %name("testchar" & testNameNum)
      Logged_Catdest = Logged_Catdest + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_CONCAT_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Copy File Command - Source file
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_SRC_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    copyOutFile = testDirName & "/filecopy" & testNameNum & extension
  else
    copyOutFile = testDestDir & "/filecopy" & testNameNum & extension
  endif

  /$SC_$CPU_FM_FileCopy Overwrite=0 File=testFileName DestName=copyOutFile

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> FileCopy Command rejected for Source file '",testFileName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Copysrc = OSError_Copysrc & %name("testchar" & testNameNum)
      Logged_Copysrc = Logged_Copysrc + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "Source File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Copysrc = Invalid_Copysrc & %name("testchar" & testNameNum)
      Logged_Copysrc = Logged_Copysrc + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> FileCopy Command completed for Source file '",testFileName, "'"
      Allowed_Copysrc = Allowed_Copysrc & %name("testchar" & testNameNum)
      Logged_Copysrc = Logged_Copysrc + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_COPY_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Delete File Command
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  /$SC_$CPU_FM_Delete File=testFileName

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Delete File Command rejected for file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_DelFile = OSError_DelFile & %name("testchar" & testNameNum)
      Logged_DelFile = Logged_DelFile + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Destination File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_DelFile = Invalid_DelFile & %name("testchar" & testNameNum)
      Logged_DelFile = Logged_DelFile + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Decompress Command Destination completed for file '",testFileName, "'"
      Allowed_DelFile = Allowed_DelFile & %name("testchar" & testNameNum)
      Logged_DelFile = Logged_DelFile + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ",FM_DELETE_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Copy File Command - Destination file
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_COPY_TGT_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  /$SC_$CPU_FM_FileCopy Overwrite=0 File=fullCompressedFileSpec DestName=testFileName

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> FileCopy Command rejected for Destination file '",testFileName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Copydest = OSError_Copydest & %name("testchar" & testNameNum)
      Logged_Copydest = Logged_Copydest + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "Source File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Copydest = Invalid_Copydest & %name("testchar" & testNameNum)
      Logged_Copydest = Logged_Copydest + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> FileCopy Command completed for Source file '",testFileName, "'"
      Allowed_Copydest = Allowed_Copydest & %name("testchar" & testNameNum)
      Logged_Copydest = Logged_Copydest + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_COPY_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Decompress Command - Source File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_CFE_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_SRC_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    decomOutFile = testDirName & "/decomOut" & testNameNum & extension
  else
    decomOutFile = testDestDir & "/decomOut" & testNameNum & extension
  endif

  /$SC_$CPU_FM_Decompress File=testFileName DestName=decomOutFile

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Decompress Command Destination rejected for file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Decomsrc = OSError_Decomsrc & %name("testchar" & testNameNum)
      Logged_Decomsrc = Logged_Decomsrc + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Destination File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Decomsrc = Invalid_Decomsrc & %name("testchar" & testNameNum)
      Logged_Decomsrc = Logged_Decomsrc + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Decompress Command Destination completed for file '",testFileName, "'"
      Allowed_Decomsrc = Allowed_Decomsrc & %name("testchar" & testNameNum)
      Logged_Decomsrc = Logged_Decomsrc + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_DECOM_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Only need to delete the testFileName if the special character is valid
  ;; Since the following command checks if the destination exists
  if (validSpec = "Yes") then
    ;; Delete File Command
    ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_CMD_EID, "DEBUG", 1

    errcnt = $SC_$CPU_FM_CMDEC + 1
    if (errcnt = 256) then
      errcnt = 0
    endif
    cmdctr = $SC_$CPU_FM_CMDPC + 1
    if (cmdctr = 256) then
      cmdctr = 0
    endif

    /$SC_$CPU_FM_Delete File=testFileName

    wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Deleted File '",testFileName,"'"

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_DELETE_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    else
      write "==> Delete File Command rejected for file '",testFileName,"'"
    endif
  endif

  ;; Decompress Command - Destination File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_CFE_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DECOM_TGT_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  /$SC_$CPU_FM_Decompress DestName=testFileName File=fullCompressedFileSpec 

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Decompress Command Destination rejected for file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Decomdest = OSError_Decomdest & %name("testchar" & testNameNum)
      Logged_Decomdest = Logged_Decomdest + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "Destination File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Decomdest = Invalid_Decomdest & %name("testchar" & testNameNum)
      Logged_Decomdest = Logged_Decomdest + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Decompress Command Destination completed for file '",testFileName, "'"
      Allowed_Decomdest = Allowed_Decomdest & %name("testchar" & testNameNum)
      Logged_Decomdest = Logged_Decomdest + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_DECOM_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; File Info Command
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_FILE_INFO_SRC_ERR_EID,"ERROR",2

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    s $sc_$cpu_fm_fileinfodisplay (testFileName)
  else
    s $sc_$cpu_fm_fileinfodisplay (testFileName,0,"Fail")
  endif

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> File Info Command rejected for file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> File Error Event message ", $SC_$CPU_find_event[2].eventid, " received."
      Invalid_FileInfo = Invalid_FileInfo & %name("testchar" & testNameNum)
      Logged_FileInfo = Logged_FileInfo + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> File Info Command completed for file '",testFileName, "'"
      Allowed_FileInfo = Allowed_FileInfo & %name("testchar" & testNameNum)
      Logged_FileInfo = Logged_FileInfo + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_GET_FILE_INFO_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Move Command - Source File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_SRC_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    moveOutFile = testDirName & "/move" & testNameNum & extension
  else
    moveOutFile = testDestDir & "/move" & testNameNum & extension
  endif

  /$SC_$CPU_FM_FileMove Overwrite=0 File=testFileName DestName=moveOutFile

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Move Command rejected for source file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Movesrc = OSError_Movesrc & %name("testchar" & testNameNum)
      Logged_Movesrc = Logged_Movesrc + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) THEN
      write "==> File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Movesrc = Invalid_Movesrc & %name("testchar" & testNameNum)
      Logged_Movesrc = Logged_Movesrc + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Move Command completed for source file '",testFileName, "'"
      Allowed_Movesrc = Allowed_Movesrc & %name("testchar" & testNameNum)
      Logged_Movesrc = Logged_Movesrc + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_MOVE_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Move Command - Destination File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_MOVE_TGT_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    /$SC_$CPU_FM_FileMove Overwrite=0 DestName=testFileName File=moveOutFile
  else
    /$SC_$CPU_FM_FileMove Overwrite=0 DestName=testFileName File=testFile
  endif

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Move Command rejected for dest file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Movedest = OSError_Movedest & %name("testchar" & testNameNum)
      Logged_Movedest = Logged_Movedest + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) THEN
      write "==> File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Movedest = Invalid_Movedest & %name("testchar" & testNameNum)
      Logged_Movedest = Logged_Movedest + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Move Command completed for dest file '",testFileName, "'"
      Allowed_Movedest = Allowed_Movedest & %name("testchar" & testNameNum)
      Logged_Movedest = Logged_Movedest + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_MOVE_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Rename Command - Source File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_SRC_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    renameOutFile = testDirName & "/rename" & testNameNum & extension
  else
    renameOutFile = testDestDir & "/rename" & testNameNum & extension
  endif

  /$SC_$CPU_FM_FileRename File=testFileName DestName=renameOutFile

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Rename Command rejected for source file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Renamesrc = OSError_Renamesrc & %name("testchar" & testNameNum)
      Logged_Renamesrc = Logged_Renamesrc + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) THEN
      write "==> File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Renamesrc = Invalid_Renamesrc & %name("testchar" & testNameNum)
      Logged_Renamesrc = Logged_Renamesrc + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Rename Command completed for source file '",testFileName, "'"
      Allowed_Renamesrc = Allowed_Renamesrc & %name("testchar" & testNameNum)
      Logged_Renamesrc = Logged_Renamesrc + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_RENAME_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Rename Command - Destination File
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC", "$CPU", {FMAppName}, FM_RENAME_TGT_ERR_EID, "ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  if (validSpec = "Yes") then
    /$SC_$CPU_FM_FileRename DestName=testFileName File=renameOutFile
  else
    /$SC_$CPU_FM_FileRename DestName=testFileName File=testFile
  endif

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Rename Command rejected for dest file '",testFileName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Renamedest = OSError_Renamedest & %name("testchar" & testNameNum)
      Logged_Renamedest = Logged_Renamedest + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) THEN
      write "==> File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Renamedest = Invalid_Renamedest & %name("testchar" & testNameNum)
      Logged_Renamedest = Logged_Renamedest + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Rename Command completed for dest file '",testFileName, "'"
      Allowed_Renamedest = Allowed_Renamedest & %name("testchar" & testNameNum)
      Logged_Renamedest = Logged_Renamedest + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ",FM_RENAME_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; DirListPkt Command
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_PKT_SRC_ERR_EID,"ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  ;; Expect "Fail" for all the special characters
  if (validSpec = "No") then
    s $sc_$cpu_fm_dirtlmdisplay (testDirName, 0, "Fail")
  else
    s $sc_$cpu_fm_dirtlmdisplay (testDirName, 0)
  endif

  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> DirListTlm Command rejected for directory '",testDirName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ",$SC_$CPU_find_event[2].eventid," received"
      OSError_DirLstTlm = OSError_DirLstTlm & %name("testchar" & testNameNum)
      Logged_DirLstTlm = Logged_DirLstTlm + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Source Dir Error Event message ",$SC_$CPU_find_event[3].eventid," received."
      Invalid_DirLstTlm = Invalid_DirLstTlm & %name("testchar" & testNameNum)
      Logged_DirLstTlm = Logged_DirLstTlm + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> DirListTlm Command Completed for directory '",testDirName, "'"

      Allowed_DirLstTlm = Allowed_DirLstTlm & %name("testchar" & testNameNum)
      Logged_DirLstTlm = Logged_DirLstTlm + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_GET_DIR_PKT_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Dir List File Command - Directory Name
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC","$CPU",{FMAppName},FM_GET_DIR_FILE_SRC_ERR_EID,"ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  ;; Setup the output file name
  dirListOutFile = "dirlist" & listFileNum & extension
  if (validSpec = "Yes") then
    fullDirListOutFile = testDirName & forwardSlash & dirListOutFile
    s $sc_$cpu_fm_dirfiledisplay (testDirName, fullDirListOutFile, testDownloadDir, dirListOutFile)
  else
    fullDirListOutFile = testDestDir & forwardSlash & dirListOutFile
    s $sc_$cpu_fm_dirfiledisplay (testDirName, fullDirListOutFile, downloadDir, dirListOutFile, "Fail")
  endif

  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> DirListFile Command rejected for directory '",testDirName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ",$SC_$CPU_find_event[2].eventid," received"
      OSError_DirLstFile1 = OSError_DirLstFile1 & %name("testchar" & testNameNum)
      Logged_DirLstFile1 = Logged_DirLstFile1 + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Src Dir Error Event message ",$SC_$CPU_find_event[3].eventid," received"
      Invalid_DirLstFile1 = Invalid_DirLstFile1 & %name("testchar" & testNameNum)
      Logged_DirLstFile1 = Logged_DirLstFile1 + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> DirListFile Command Completed for directory '",testDirName, "'"

      Allowed_DirLstFile1 = Allowed_DirLstFile1 & %name("testchar" & testNameNum)
      Logged_DirLstFile1 = Logged_DirLstFile1 + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003;3002.1) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
        ut_setrequirements FM_3002_1, "P"
      else
        write "<!> Failed (1003;3002.1) - Event message ",FM_GET_DIR_FILE_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
        ut_setrequirements FM_3002_1, "F"
      endif
    endif
  endif

  listFileNum = listFileNum + 1

  ;; Delete All Files Command
  ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_ALL_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_ALL_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_ALL_SRC_ERR_EID,"ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif

  /$SC_$CPU_FM_DeleteAll DirName=testDirName

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> Delete All Command rejected for directory '",testDirName,"'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_DelAll = OSError_DelAll & %name("testchar" & testNameNum)
      Logged_DelAll = Logged_DelAll + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Destination File Error Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_DelAll = Invalid_DelAll & %name("testchar" & testNameNum)
      Logged_DelAll = Logged_DelAll + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> Delete All Command completed for directory '",testDirName, "'"
      Allowed_DelAll = Allowed_DelAll & %name("testchar" & testNameNum)
      Logged_DelAll = Logged_DelAll + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_DELETE_ALL_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif

  ;; Delete Dir Command
  ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_DIR_CMD_EID, "DEBUG", 1
  ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_DIR_OS_ERR_EID, "ERROR", 2
  ut_setupevents "$SC","$CPU",{FMAppName},FM_DELETE_DIR_SRC_ERR_EID,"ERROR", 3

  errcnt = $SC_$CPU_FM_CMDEC + 1
  if (errcnt = 256) then
    errcnt = 0
  endif
  cmdctr = $SC_$CPU_FM_CMDPC + 1
  if (cmdctr = 256) then
    cmdctr = 0
  endif
  
  /$SC_$CPU_FM_DirDelete DirName=testDirName

  wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
  if (errcnt = $SC_$CPU_FM_CMDEC) then
    write "==> DeleteDir Command rejected for directory '",testDirName, "'"

    if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
      write "==> OS Error Event message ", $SC_$CPU_find_event[2].eventid, " received"
      OSError_Delete = OSError_Delete & %name("testchar" & testNameNum)
      Logged_Delete = Logged_Delete + 1
    endif

    if ($SC_$CPU_find_event[3].num_found_messages = 1) then
      write "==> Source Dir Event message ", $SC_$CPU_find_event[3].eventid, " received."
      Invalid_Delete = Invalid_Delete & %name("testchar" & testNameNum)
      Logged_Delete = Logged_Delete + 1
    endif
  else
    if (cmdctr = $SC_$CPU_FM_CMDPC) then
      write "==> DeleteDir Command Completed for directory '",testDirName, "'"

      Allowed_Delete = Allowed_Delete & %name("testchar" & testNameNum)
      Logged_Delete = Logged_Delete + 1

      if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
        write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
        ut_setrequirements FM_1003, "P"
      else
        write "<!> Failed (1003) - Event message ", FM_DELETE_DIR_CMD_EID, " NOT received."
        ut_setrequirements FM_1003, "F"
      endif
    endif
  endif
ENDDO

Invalid_Create = Invalid_Create & """"
OSError_Create = OSError_Create & """"
Allowed_Create = Allowed_Create & """"
Invalid_DirLstTlm = Invalid_DirLstTlm & """"
OSError_DirLstTlm = OSError_DirLstTlm & """"
Allowed_DirLstTlm = Allowed_DirLstTlm & """"
Invalid_DirLstFile1 = Invalid_DirLstFile1 & """"
OSError_DirLstFile1 = OSError_DirLstFile1 & """"
Allowed_DirLstFile1 = Allowed_DirLstFile1 & """"
Invalid_Delete = Invalid_Delete & """"
OSError_Delete = OSError_Delete & """"
Allowed_Delete = Allowed_Delete & """"
Invalid_DelAll = Invalid_DelAll & """"
OSError_DelAll = OSError_DelAll & """"
Allowed_DelAll = Allowed_DelAll & """"
Invalid_DirLstFile2 = Invalid_DirLstFile2 & """"
OSError_DirLstFile2 = OSError_DirLstFile2 & """"
Allowed_DirLstFile2 = Allowed_DirLstFile2 & """"
Invalid_Copysrc = Invalid_Copysrc & """"
OSError_Copysrc = OSError_Copysrc & """"
Allowed_Copysrc = Allowed_Copysrc & """"
Invalid_Copydest = Invalid_Copydest & """"
OSError_Copydest = OSError_Copydest & """"
Allowed_Copydest = Allowed_Copydest & """"
Invalid_Catsrc1 = Invalid_Catsrc1 & """"
OSError_Catsrc1 = OSError_Catsrc1 & """"
Allowed_Catsrc1 = Allowed_Catsrc1 & """"
Invalid_Catsrc2 = Invalid_Catsrc2 & """"
OSError_Catsrc2 = OSError_Catsrc2 & """"
Allowed_Catsrc2 = Allowed_Catsrc2 & """"
Invalid_Catdest = Invalid_Catdest & """"
OSError_Catdest = OSError_Catdest & """"
Allowed_Catdest = Allowed_Catdest & """"
Invalid_Decomsrc = Invalid_Decomsrc & """"
OSError_Decomsrc = OSError_Decomsrc & """"
Allowed_Decomsrc = Allowed_Decomsrc & """"
Invalid_Decomdest = Invalid_Decomdest & """"
OSError_Decomdest = OSError_Decomdest & """"
Allowed_Decomdest = Allowed_Decomdest & """"
Allowed_FileInfo = Allowed_FileInfo & """"
Invalid_FileInfo = Invalid_FileInfo & """"
Invalid_DelFile = Invalid_DelFile & """"
OSError_DelFile = OSError_DelFile & """"
Allowed_DelFile = Allowed_DelFile & """"
Invalid_Movesrc = Invalid_Movesrc & """"
OSError_Movesrc = OSError_Movesrc & """"
Allowed_Movesrc = Allowed_Movesrc & """"
Invalid_Movedest = Invalid_Movedest & """"
OSError_Movedest = OSError_Movedest & """"
Allowed_Movedest = Allowed_Movedest & """"
Invalid_Renamesrc = Invalid_Renamesrc & """"
OSError_Renamesrc = OSError_Renamesrc & """"
Allowed_Renamesrc = Allowed_Renamesrc & """"
Invalid_Renamedest = Invalid_Renamedest & """"
OSError_Renamedest = OSError_Renamedest & """"
Allowed_Renamedest = Allowed_Renamedest & """"

wait 5

PROCEND:
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

write ""
write "Create Dir Cmd Results:"
write "    Logged: ",Logged_Create
write "    Allowed: ",Allowed_Create
write "    Invalid: ",Invalid_Create
write "    OS Error: ",OSError_Create
write ""
write "Dir List to Tlm Cmd Results:"
write "    Logged: ",Logged_DirLstTlm
write "    Allowed: ",Allowed_DirLstTlm
write "    Invalid: ",Invalid_DirLstTlm
write "    OS Error: ",OSError_DirLstTlm
write ""
write "Dir List to File Cmd - Directory argument Results:"
write "    Logged: ",Logged_DirLstFile1
write "    Allowed: ", Allowed_DirLstFile1
write "    Invalid: ", Invalid_DirLstFile1
write "    OS Error: ", OSError_DirLstFile1
write ""
write "Dir List to File Cmd - File argument Results:"
write "    Logged: ", Logged_DirLstFile2
write "    Allowed: ", Allowed_DirLstFile2
write "    Invalid: ", Invalid_DirLstFile2
write "    OS Error: ", OSError_DirLstFile2
write ""
write "Delete Dir Cmd Results: "
write "    Logged: ", Logged_Delete
write "    Allowed: ", Allowed_Delete
write "    Invalid: ", Invalid_Delete
write "    OS Error: ", OSError_Delete
write ""
write "Copy File Cmd - Source File argument Results:"
write "    Logged: ", Logged_Copysrc
write "    Allowed: ", Allowed_Copysrc
write "    Invalid: ", Invalid_Copysrc
write "    OS Error: ", OSError_Copysrc
write ""
write "Copy File Cmd - Destination File argument Results:"
write "    Logged: ", Logged_Copydest
write "    Allowed: ", Allowed_Copydest
write "    Invalid: ", Invalid_Copydest
write "    OS Error: ", OSError_Copydest
write ""
write "Concatenate Cmd - Source File1 argument Results:"
write "    Logged: ", Logged_Catsrc1
write "    Allowed: ", Allowed_Catsrc1
write "    Invalid: ", Invalid_Catsrc1
write "    OS Error: ", OSError_Catsrc1
write ""
write "Concatenate Cmd - Source File2 argument Results:"
write "    Logged: ", Logged_Catsrc2
write "    Allowed: ", Allowed_Catsrc2
write "    Invalid: ", Invalid_Catsrc2
write "    OS Error: ", OSError_Catsrc2
write ""
write "Concatenate Cmd - Detination File argument Results:"
write "    Logged: ", Logged_Catdest
write "    Allowed: ", Allowed_Catdest
write "    Invalid: ", Invalid_Catdest
write "    OS Error: ", OSError_Catdest
write ""
write "Decompress Cmd - Source File argument Results:"
write "    Logged: ", Logged_Decomsrc
write "    Allowed: ", Allowed_Decomsrc
write "    Invalid: ", Invalid_Decomsrc
write "    OS Error: ", OSError_Decomsrc
write ""
write "Decompress Cmd - Destination File argument Results:"
write "    Logged: ", Logged_Decomdest
write "    Allowed: ", Allowed_Decomdest
write "    Invalid: ", Invalid_Decomdest
write "    OS Error: ", OSError_Decomdest
write ""
write "Delete File Cmd Results:"
write "    Logged: ", Logged_DelFile
write "    Allowed: ", Allowed_DelFile
write "    Invalid: ", Invalid_DelFile
write "    OS Error: ", OSError_DelFile
write ""
write "Delete All Files Cmd Results:"
write "    Logged: ", Logged_DelAll
write "    Allowed: ", Allowed_DelAll
write "    Invalid: ", Invalid_DelAll
write "    OS Error: ", OSError_DelAll
write ""
write "File Info Cmd Results:"
write "    Logged: ", Logged_FileInfo
write "    Allowed: ", Allowed_FileInfo
write "    Invalid: ", Invalid_FileInfo
write ""
write "Move Cmd - Source File argument Results:"
write "    Logged: ", Logged_Movesrc
write "    Allowed: ", Allowed_Movesrc
write "    Invalid: ", Invalid_Movesrc
write "    OS Error: ", OSError_Movesrc
write ""
write "Move Cmd - Destination File argument Results:"
write "    Logged: ", Logged_Movedest
write "    Allowed: ", Allowed_Movedest
write "    Invalid: ", Invalid_Movedest
write "    OS Error: ", OSError_Movedest
write ""
write "Rename Cmd - Source File argument Results:"
write "    Logged: ", Logged_Renamesrc
write "    Allowed: ", Allowed_Renamesrc
write "    Invalid: ", Invalid_Renamesrc
write "    OS Error: ", OSError_Renamesrc
write ""
write "Rename Cmd - Destination File argument Results:"
write "    Logged: ", Logged_Renamedest
write "    Allowed: ", Allowed_Renamedest
write "    Invalid: ", Invalid_Renamedest
write "    OS Error: ", OSError_Renamedest
write ""

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_specialchars3                            "
write ";*********************************************************************"
ENDPROC
