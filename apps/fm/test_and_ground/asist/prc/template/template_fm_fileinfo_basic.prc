PROC $sc_$cpu_fm_fileinfo_basic
;*******************************************************************************
;  Test Name:  FM_FileInfo_Basic
;  Test Level: Build Verification 
;  Test Type:  Functional
;		     
;  Test Description
;   The purpose of this test is to verify that the File Manager (FM) 
;   File Info and File Close Commands functions properly. The FM_FileInfo
;   and FM_Close commands will be tested to see if the FM application
;   handles these as desired by the requirements. The FM_DirCreate command
;   is also used to facilitate the testing.
;
;  Requirements Tested
;    FM1003	If FM accepts any command as valid, FM shall execute the
;		command, increment the FM Valid Command Counter and issue
;		an event message
;    FM1004	If FM rejects any command, FM shall abort the command execution,
;		increment the FM Command Rejected Counter and issue an error
;		event.
;    FM2007	Upon receipt of a Delete All Files command, FM shall delete all
;		the files in the command-specified directory
;    FM2007.1	If the command-specified directory contains an open file, FM
;		shall not delete the open file.
;    FM2007.1.1	For any open files that are not deleted, FM shall issue one
;		event message
;    FM2008	Upon receipt of a Delete command, FM shall delete the
;		command-specified file.
;    FM2008.1	If the command-specified file is open, FM shall reject the
;		command to delete the file.
;    FM2011	Upon receipt of a File Info command, FM shall generate a message
;		containing the following for the command-specified file:
;			a) the file size, 
;			b) last modification time,
;			c) file status (Open, Closed, or Non-existent)
;			d) <MISSION_DEFINED> CRC
;    FM3000	Upon receipt of a Create Directory command, FM shall create the
;		command-specified directory on the command-specified filesystem.
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
;    The "last mod time" of the files should not change. The file
;    is not "touched" if the only action is an open command.
;    The CRC is just generated, it is not verified.
;
;  Change History
;
;	Date	   Name		Description
;	04/10/08   D. Stewart	Original Procedure
;	12/08/08   W. Moleski   General Cleanup
;	01/28/10   W. Moleski   Updated for FM 2.1.0.0 and more cleanup
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
#define FM_2007     2
#define FM_20071    3
#define FM_200711   4
#define FM_2008     5
#define FM_20081    6
#define FM_2011     7
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

local cfe_requirements[0 .. ut_req_array_size] = ["FM_1003", "FM_1004", "FM_2007", "FM_2007.1", "FM_2007.1.1", "FM_2008", "FM_2008.1", "FM_2011", "FM_3000", "FM_4000", "FM_5000"]

local rawcmd

local FMAppName = FM_APP_NAME
local ramDir = "/ram"
local ramDirPhys = "RAM:0"
local testDir = ramDir & "/FMTEST"
local uploadDir = ramDirPhys & "/FMTEST"

local testFile = "FMINFO.TST"
local fullTestFileName = testDir & "/" & testFile

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
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_CREATE_DIR_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

/$SC_$CPU_FM_DirCreate DirName=testDir

ut_tlmwait  $SC_$CPU_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
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
  write "<!> Failed (1003;3000) - Create Directory command not sent properly (", UT_TW_Status, ")."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_3000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2: Upload Test File to test Directory."
write ";*********************************************************************"
;; Upload the Test file
; proc ftp_file (remote_directory, filename, dest_filename, cpu, getorput)
s ftp_file (uploadDir, testFile, testFile, "$CPU", "P")
wait 5

write ";*********************************************************************"
write ";  Step 3.0:  File Info Test."
write ";*********************************************************************"
write ";  Step 3.1:  Perform File Info Command to Verify Test File exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_FM_InfoFileName = fullTestFileName) THEN
    write "<*> Passed - File Info Telemetry Matches expected file: ", fullTestFileName
  else
    write "<!> Failed - File Info Telemetry does not match expected file: ", fullTestFileName
  endif

  ;; Need to check the File Status in order to verify the file exists
  if ($SC_$CPU_FM_FileStatus > 1) THEN
    write "<*> Passed - File status indicates that the file exists."
  else
    write "<!> Failed - expected file '",fullTestFileName,"' does not exist"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2: Open Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_OPENFILE_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1
errcnt = $SC_$CPU_TST_FM_CMDEC + 1

/$SC_$CPU_TST_FM_Open File=fullTestFileName

wait until (($SC_$CPU_TST_FM_CMDPC = cmdctr) OR ($SC_$CPU_TST_FM_CMDEC = errcnt))
if ($SC_$CPU_TST_FM_CMDPC = cmdctr) then
  write "<*> Passed - File reported Open by test app."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed -  Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", TST_FM_OPENFILE_INF_EID, "."
  endif
else
  write "<!> Failed - File Open command not sent properly to test app."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3: Send File Info Command to Verify Test File is open. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Open") THEN
    write "<*> Passed - File Info Telemetry indicates File is open"
  else
    write "<!> Failed - File Info Telemetry does not indicate File is open"
  endif

  if ($SC_$CPU_FM_NumOpen = 1) THEN
    write "<*> Passed (4000) - HK Telemetry Reports number of open files correctly: ", $SC_$CPU_FM_NumOpen
    ut_setrequirements FM_4000, "P"
  else
    write "<!> Failed (4000) - HK Telemetry Reports number of open files incorrectly: ", $SC_$CPU_FM_NumOpen
    ut_setrequirements FM_4000, "F"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.1: Perform Delete File Command attempting to Delete a"
write ";  File that is open"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_SRC_ERR_EID, "ERROR", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_Delete File=fullTestFileName

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt))
if ($SC_$CPU_FM_CMDEC = errcnt) then
  write "<*> Passed (2008) - Delete Command Rejected Counter incremented as expected."
  ut_setrequirements FM_2008, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1004;2008.1) - Event message ", $SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1004, "P"
    ut_setrequirements FM_20081, "P"
  else
    write "<!> Failed (1004;2008.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_DELETE_SRC_ERR_EID
    ut_setrequirements FM_1004, "F"
    ut_setrequirements FM_20081, "F"
  endif
else
  write "<!> Failed (1004;2008;2008.1) - Delete Command May have allowed Deletion of an open file."
  ut_setrequirements FM_1004, "F"
  ut_setrequirements FM_2008, "F"
  ut_setrequirements FM_20081, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.2: Perform File Info Command to Verify File still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileName)

if (cmdctr = $SC_$CPU_FM_CMDPC) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Open") THEN
    write "<*> Passed - File Info Telemetry indicates File is open"
  else
    write "<!> Failed - File Info Telemetry does not indicate File is open"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.3: Perform Delete All Files Command attempting to Delete a"
write ";  File that is open"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_CMD_EID, "DEBUG", 1
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_DELETE_ALL_WARNING_EID, "INFO", 2

cmdctr = $SC_$CPU_FM_CMDPC + 1
errcnt = $SC_$CPU_FM_CMDEC + 1

/$SC_$CPU_FM_DeleteAll DirName=testDir

wait until (($SC_$CPU_FM_CMDPC = cmdctr) OR ($SC_$CPU_FM_CMDEC = errcnt ))
if ($SC_$CPU_FM_CMDPC = cmdctr) then
  write "<*> Passed (2007) - Command Accepted Counter incremented as expected."
  ut_setrequirements FM_2007, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Expected event message ", $SC_$CPU_find_event[1].eventid," received."
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_DELETE_ALL_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if ($SC_$CPU_find_event[2].num_found_messages = 1) THEN
    write "<*> Passed (2007.1.1) - Event message ", $SC_$CPU_find_event[2].eventid, " received"
    ut_setrequirements FM_200711, "P"
  else
    write "<!> Failed (2007.1.1) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",FM_DELETE_ALL_WARNING_EID, "."
    ut_setrequirements FM_200711, "F"
  endif
else
  write "<!> Failed (1003;2007;2007.1.1) - Command Rejected Delete All command."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2007, "F"
  ut_setrequirements FM_200711, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.3.4: Perform File Info Command to Verify File still exists."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileName)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Open") THEN
    write "<*> Passed (2007.1) - File Info Telemetry indicates File is open"
    ut_setrequirements FM_20071, "P"
  else
    write "<!> Failed (2007.1) - File Info Telemetry does not indicate File is open"
    ut_setrequirements FM_20071, "F"
  endif
else
  write "<!> Failed (1003;2007.1;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_20071, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.4:  Close Test File."
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", "TST_FM", TST_FM_CLOSEFILE_INF_EID, "INFO", 1

cmdctr = $SC_$CPU_TST_FM_CMDPC + 1

/$SC_$CPU_TST_FM_Close File=fullTestFileName

ut_tlmwait $SC_$CPU_TST_FM_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - File Close command sent properly."

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  else
    write "<!> Failed - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",TST_FM_CLOSEFILE_INF_EID
  endif
else
  write "<!> Failed - File Close command not sent properly."
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.5: Send File Info Command to Verify Test File is closed. "
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {FMAppName}, FM_GET_FILE_INFO_CMD_EID, "DEBUG", 1

cmdctr = $SC_$CPU_FM_CMDPC + 1

s $sc_$cpu_fm_fileinfodisplay (fullTestFileName)

if ( cmdctr = $SC_$CPU_FM_CMDPC ) then
  write "<*> Passed (2011) - File Info Command Completed."
  ut_setrequirements FM_2011, "P"

  if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
    write "<*> Passed (1003) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
    ut_setrequirements FM_1003, "P"
  else
    write "<!> Failed (1003) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ", FM_GET_FILE_INFO_CMD_EID, "."
    ut_setrequirements FM_1003, "F"
  endif

  if (p@$SC_$CPU_FM_FileStatus = "Closed") THEN
    write "<*> Passed - File Info Telemetry indicates File is closed"
  else
    write "<!> Failed - File Info Telemetry does not indicate File is closed!"
  endif
else
  write "<!> Failed (1003;2011) - File Info Command Failed."
  ut_setrequirements FM_1003, "F"
  ut_setrequirements FM_2011, "F"
endif

wait 5

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
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

s $sc_$cpu_fm_clearallpages

write ""
write ";*********************************************************************"
write ";  End procedure $sc_$cpu_fm_fileinfo_basic                           "
write ";*********************************************************************"
ENDPROC
