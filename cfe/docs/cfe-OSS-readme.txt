core Flight Executive (cFE) Open Source Release Readme

cFE Release 6.5.0 

Date: 
June 7, 2016

Introduction:
 The Core Flight Executive is a portable, platform independent embedded system 
 framework originally developed by NASA Goddard Space Flight Center and now 
 maintained by the NASA wide community Configuration Control Board (CCB). This 
 framework is used as the basis for the flight software for satellite data 
 systems and instruments, but can be used on other embedded systems. 
 The Core Flight Executive is written in C and depends on another software
 library called the Operating System Abstraction Layer (OSAL). The OSAL
 is available at http://sourceforge.net/projects/osal/ 
 and github.com/nasa/osal

 This software is licensed under the NASA Open Source Agreement. 
 http://ti.arc.nasa.gov/opensource/nosa

 The Core Flight Executive consists of the following subsystems:
  - Executive Services - initializes and controls applications
  - Software Bus - A publish and subscribe messaging system based
                   on CCSDS command and telemetry packets
  - Time Services - Manages system time
  - Event Services - Event reporting and logging services for applications
  - Table Services - Data/parameter load and update services for applications  

 The Core Flight Executive is intended to host a suite of applications
 and libraries. A small subset of the applications and libraries are included in 
 this distribution. A set of lab applications provide a means of scheduling the 
 cFS system, as well as, commanding and receiving telemetry. A sample library 
 and sample application are included to help verify that the build and runtime 
 are configured correctly.

Release Notes:

cFE Release 6.5.0 does NOT support the following system requirements:

cES1515.1 "If the creation of the operating system object fails, the cFE shall 
           perform a power on reset."

The cause of this requirement failure is due to a failure to call the 
CFE_PSP_Restart function from the CFE_ES_CreateObjects function when the return 
from OS_TaskCreate != OS_SUCCESS (cfe_es_start.c, line 865).  Instead the 
CFE_PSP_Panic function is being called (cfe_es_start.c, line 882).  The 
CFE_PSP_Panic function makes a call to exit(-1), resulting in termination of the 
cFS system vs. a power on reset.  This failure will be addressed in the next
release of the cFE.  It should be noted that the current behavior is consistent
with previous releases of the cFE.  There are two workarounds to this failure:

   1. Replace the call to CFE_PSP_Panic (cfe_es_start.c, line 882) with a call
      to CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON). 

   2. Update the CFE_PSP_Panic function to perform the desired behavior.

cES1702.3 "If the CPU exception was caused by the Operating System or cFE Core 
           then the cFE shall initiate a <PLATFORM_DEFINED> response."

In the previous release of the cFE (version 6.4.2), this requirement was being
satisfied via the PSP making a call to the cFE platform defined exception 
handling function.  Changes to the PSP (PSP version 1.3.0) resulted in the 
failure of this requirement. cFE requirements must be satisfied by the cFE.  
This failure will be addressed in the next release of the cFE.  To workaround 
this failure:

Replace the call to CFE_ES_ProcessCoreException (cfe_psp_exception.c, line 155) 
with:

CFE_ES_EXCEPTION_FUNCTION((uint32 )task_id, 
                          (uint8 *)CFE_PSP_ExceptionReasonString, 
	 	          (uint32 *)&CFE_PSP_ExceptionContext, 
                          sizeof(CFE_PSP_ExceptionContext_t));

Note: The CFE_ES_EXCEPTION_FUNCTION prototype will need to be imported into
the cfe_psp_exception.c source file.  Also, the cfe platform configuration
variable CFE_ES_EXCEPTION_FUNCTION defined in the cFE_platform_cfg.h file will
need to be updated to call the desired function.
  

cES1703.3 "If the Floating Point exception was caused by the OS or cFE Core then 
           the cFE shall initiate a <PLATFORM_DEFINED> response."

In the previous release of the cFE (version 6.4.2), this requirement was being
satisfied via the PSP making a call to the cFE platform defined exception 
handling function.  Changes to the PSP (PSP version 1.3.0) resulted in the 
failure of this requirement. cFE requirements must be satisfied by the cFE.  
This failure will be addressed in the next release of the cFE.  To workaround 
this failure:

Replace the call to CFE_ES_ProcessCoreException (cfe_psp_exception.c, line 155) 
with:

CFE_ES_EXCEPTION_FUNCTION((uint32 )task_id, 
                          (uint8 *)CFE_PSP_ExceptionReasonString, 
	 	          (uint32 *)&CFE_PSP_ExceptionContext, 
                          sizeof(CFE_PSP_ExceptionContext_t));

Note: The CFE_ES_EXCEPTION_FUNCTION prototype will need to be imported into
the cfe_psp_exception.c source file.  Also, the cfe platform configuration
variable CFE_ES_EXCEPTION_FUNCTION defined in the cFE_platform_cfg.h file will
need to be updated to call the desired function.

There were some minor API changes to this build that may result in compiler 
warnings with applications/tasks built via previous cFE releases.  It is known
these API changes affect the Health and Safety (HS) application version 2.2.0
and the Stored Command (SC) application version 2.4.0.  Minor updates are 
needed to support compatability.  These updates include:

   1. In sc_cmds.c, line 911 needs to be updated to:

int32 TableID = (int32) ((CFE_TBL_NotifyCmd_t *) CmdPacket)->Payload.Parameter;
 
   2. In hs_monitors.c, line 268 needs to be updated to:

(HS_AppData.EMTablePtr[TableIndex].EventID == EventPtr->Payload.PacketID.EventID)) 
 	
   3. In hs_monitors.c, line 270 needs to be updated to:

if ( strncmp(HS_AppData.EMTablePtr[TableIndex].AppName, 
     EventPtr->Payload.PacketID.AppName, OS_MAX_API_NAME) == 0 )

For detailed information on what is included/missing from this release, reference
the cFE 6.5.0 Version Description Document.
 
Software Included:
 Core Flight Executive ( cFE ) 6.5.0
 Platform Support Package (PSP) library
 Classic core Flight System (cFS) Mission Build system 
 Cmake Mission Build system

 Applications:
    Sample Library -- an example of a CFS library
    Sample App -- an example of a CFS application
    CI_LAB -- A test application to send CCSDS Command packets to 
             the system over UDP/IP
    TO_LAB -- A test application to subscribe to and send CCSDS 
              telemetry packets over UDP/IP
    SCH_LAB -- A test application to schedule housekeeping 
               packet collection.

 Simple Command and Telemetry utilities

    The command and telemetry utilities provide a basic ground system 
    for desktop testing via UDP/IP connections to ci_lab and to_lab. 
    The utilities use python 2.x and the QT 4.x GUI libraries. The forms 
    were designed in the QT4 designer program. The python bindings used are the 
    PyQT4 bindings. On Ubuntu 12.04 LTS for example: use the following command 
    to install the software needed to run the utilities:

    $ sudo apt-get install python-qt4 pyqt4-dev-tools
    
    cmdUtil -- A simple command line utility to format and send CCSDS command 
               over UDP/IP to the CI_LAB application. Located in cfe/tools/cmdUtil.
               Before using cmdGui, run "make" in this directory to build the 
               cmdUtil binary.

    cmdGUI -   A simple python/QT4 based utility that allows you to define and 
               send commands using the cmdUtil program. 
               cmdGui is started by running:
               $ python ./CommandSystem.py

    tlmUtil -- A simple python/QT4 based utility that allows you to define and
               display telemetry received from the TO_LAB application.
               tlmUtil is started by running: 
               $ python ./TelemetrySystem.py 
 
Software Required:

 Operating System Abstraction Layer 4.2.0 or higher
    Can be obtained at http://sourceforge.net/projects/osal
     or github.com/nasa/osal

 Supported Build and Runtime Environment:

 Build Environment Supported:
  This software is configured to build "out of the box" on a pc-linux x86 
  ( 32 bit ) environment. 
 
  It should be possible to build on recent linux distributions
  ( CentOS, RedHat, Debian, Ubuntu, SUSE, etc ). It should also build and run
  as a 32 bit or 64 bit executable on a 64 bit system.  See the Version 
  Description Document, Section 1.5, for a listing of tested platforms. 

  It should be possible to build on Windows and Mac OS X, but is not currently 
  tested/supported. The platform support packages (psp) for mac-osx and pc-cygwin 
  have been decommissioned.  It is recommended to start with the pc-linux PSP 
  implementation for building on Windows and Mac OS X, updating the PSP 
  implementation as necessary for these PC operating systems.
  
Runtime Targets Supported:
   The "out of the box" targets in this distribution include:
     1. 32 bit x86 Linux 

Other targets: 
    Other targets are included via the OSAL and  PSP libraries, but may take 
    additional work to run. 

Note: The cFE assumes use of the "char" data type as an 8-bit type.

Quick start:
  The following assumes you are in a CentOS/RHEL terminal. The cFE distribution
  file is in a directory called "Projects". 

  Unpack the cFE distribution

  [cfe@localhost ~]$ cd Projects
  [cfe@localhost Projects]$ tar -zxf cFE-6.5.0-OSS-release.tar.gz
  [cfe@localhost Projects]$ cd cFE-6.5.0-OSS-release

  Unpack the OSAL distribution ( obtain from sources above )
  Assuming the file is in your Projects directory
  ( example: /home/acudmore/Projects/osal-4.2.0-release.tar.gz)

  [cfe@localhost cFE-6.4.1-OSS-release]$ tar -zxf ../Projects/osal-4.2.0-release.tar.gz
  [cfe@localhost cFE-6.4.1-OSS-release]$ mv osal-4.2.0-release osal

  Build the cFE source code

  NOTE: In the first command, make sure you enter a '.' then a space 
        then './setvars.sh' !!!
        ( see the bash command "source" )

  [cfe@localhost cFE-6.4.1-OSS-release]$ . ./setvars.sh
  [cfe@localhost cFE-6.4.1-OSS-release]$ cd build
  [cfe@localhost build]$ cd cpu1 
   ( Note: cpu1 is configured for the pc-linux target ) 
  [cfe@localhost pc-linux]$ make config
   ( Note: this copies all of the needed configuration make and header files ) 
  [cfe@localhost pc-linux]$ make     
   ( Note: this builds the OSAL, cFE, and apps ) 

  When the build completes, the cFE core executable that runs on on CentOS is 
  in the exe directory. It can be run by doing the following:
   
  [cfe@localhost cpu1]$ cd exe
  [cfe@localhost exe]$ ./core-linux.bin 

  At this point, the linux version of the cFE will start running. Note that the 
  OSAL uses POSIX message queues to implement the inter-task communication 
  queues.
  The cFE by default needs a larger "msg_max" parameter in linux to run. There 
  are two solutions to this problem:
    1. increase the /proc/sys/fs/mqueue/msg_max parameter 
    2. run the cFE core as root
       [cfe@localhost exe]$ su
       ( type in root password )
       [cfe@localhost exe]# ./core-linux.bin

  Root/sudo is also required to enable the linux real time scheduler for
  more accurate priorty control. 

  With the cFE starting, it should initialize, then read the cFE startup 
  script and load the library and applications.  
  New applications can be added by editing the cfe_es_startup.scr file.
  Stop the cFE and all applications by hitting control-c in the terminal

  When the cFE and CFS applications are running, you can start the 
  CommandSystem.py and TelemetrySystem.py and try some simple commanding. 
 
Where to find more info:
  There is much more information that is beyond the scope of a readme file.

  The current documents can be found in the following directories:
  cFE-6.5.0-OSS-release/docs
  cFE-6.5.0-OSS-release/osal/docs ( once the OSAL is installed )
  cFE-6.5.0-OSS-release/cfe/docs

  cFE-6.5.0-OSS-release/cfe/docs/doxygen/index.html -- a good place to start
         for the cFE 

  
