Readme file for the gen_sch_tbl script.  

Given a small set of key application scheduling parameters, this script will 
generate the scheduler definition table (sch_def_schtbl.c) used by the CFS 
Scheduler (SCH) application.  This is not to be confused with the scheduler lab
application SCH_LAB.  This script will NOT generate the scheduler message 
definition table (sch_def_msgtbl.c).  

NOTE: Currently there is no input error checking. This needs to be added.Some basic instruction:This script will convert a simple input file of the format:
The first two lines define the number of minor frames per major frame andthe max number of activity messages that the scheduler can send per minorframe time slot.
SCH_MAX_MINOR_FRAMES max_num_minor_framesMSGS_PER_MINOR_FRAMES messages_per_minor_frame #
The rest of the file is one line per application of the following form:
APP_ID PRIO RATE_Hz INITIAL_MF MSG_TYPE(WU_HK)
where:
APP_ID: app ID or prefix (i.e. SCH, DS, GNC, etc)PRIO: priority in the range of 1 to 255 (high to low)RATE_Hz: cycle rate for the appINITIAL_MF: initial minor frame number to start in (basicallystart offset into major frame)MSG_TYPE(WU_HK): activity message type to send, currently supported areeither WU for a wakeup message or HK for a housekeepingmessage #
USAGE:gen_sch_tbl.py sch_input_config_file sch_def_file.cex:gen_sch_tbl.py sch.in sch_def_schtbl.c
There is a unit_tests directory with a few different types of sch.in input 
files to test the script with and provide as examples. You can cd to the 
unit_tests directory and run the "ut" script which runs all of the sample 
input files through the script and produces sample .c files for testing and 
reference.

This is the unit_tests/sch-100x10.in.  When the ut script is executed, it will
generate the file sch_def_schtbl-100x10.c

SCH_MAX_MINOR_FRAMES 100
MSGS_PER_MINOR_FRAMES  10
APP_ID PRIO   RATE_Hz    INITIAL_MF MSG_TYPE(WU_HK)
CFE_TIME  60     1        97          HK
CFE_EVS   61     1        97          HK
CFE_SB    64     1        97          HK
CFE_ES    68     1        97          HK
CFE_TBL   70     1        97          HK
SCH      115     1        98          HK
APP_1    120     100       0          WU 
APP_2    125     50        1          WU
APP_3    130     25        1          WU
CI_LAB   142     10        1          HK
TO_LAB   145     10        5          HK
APP_4    150      5        5          WU
HS       151     10        0          HK
DS       154     10        0          HK
APP_5    160      1       50          WU

This sch.in file will generate a sch table with 100 minor frame slots, with 
space for 10 activity messages per minor frame.  Core cFE services apps,
CFE_TIME, CFE_EVS, CFE_SB, CFE_ES, CFE_TBL are sent HK messages at 1 Hz in
minor frame 97, and have the highest priorities in the system. The sch app
with a priority of 115 is sent a send HK message at 1Hz in slot 98.  Next
follow mission specific apps APP_1,2,3, ci, to, hs, ds, APP_5 with their 
given priorities and cycle rates.  An initial minor frame specifies which 
minor frame (basically an offset into the major frame in which to run) to
start running in.
