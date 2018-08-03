/******************************************************************************
** File: osconfig.h
** $Id: osconfig.h 1.1.1.3 2015/03/04 14:58:27EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose:
**   This header file contains the OS API  configuration parameters.
**
** Author:  A. Cudmore
**
** Notes:
**
** $Date: 2015/03/04 14:58:27EST $
** $Revision: 1.1.1.3 $
** $Log: osconfig.h  $
** Revision 1.1.1.3 2015/03/04 14:58:27EST sstrege 
** Added copyright information
** Revision 1.1.1.2 2008/10/17 15:48:31EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/unit_test/cfe_hdr_files/project.pj
** Revision 1.3 2008/08/29 09:55:35EDT apcudmore 
** Updated MAX Modules to 32
** Revision 1.2 2008/08/27 15:23:52EDT apcudmore 
** add OS_MAX_TIMERS define
** Revision 1.1 2008/07/22 16:51:58EDT apcudmore 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/build/cpu1/inc/project.pj
** Revision 1.2 2008/06/20 15:47:43EDT apcudmore 
** Added option for OS module loader
** Revision 1.1 2008/04/20 22:35:19EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-OSAL-REPOSITORY/build/inc/project.pj
** Revision 1.6 2008/02/12 13:27:59EST apcudmore 
** New API updates:
**   - fixed RTEMS osapi compile error
**   - related makefile fixes
**   - header file parameter update
** Revision 1.5 2008/02/07 11:27:29EST apcudmore 
** added support for dynamic load API
** Revision 1.4 2008/02/05 15:53:35EST apcudmore 
** Added memory range checking API.
** Revision 1.3 2008/01/31 15:03:53EST njyanchik 
** I used Greg's implementation to create a new #define to switch between the sockets implementation and the message queue implementation for Linux.
** Revision 1.2 2007/10/22 13:19:16ADT apcudmore 
** various fixes to prepare for OSAL release:
**  - Refine makefiles
**  - Coldfire m5235 BCC support
** Revision 1.1 2007/10/16 16:14:08EDT apcudmore 
** Initial revision
** Member added to project d:/mksdata/MKS-OSAL-REPOSITORY/build/inc/project.pj
** Revision 1.1 2007/08/24 11:23:05EDT apcudmore 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/build/cpu1/cfe/inc/project.pj
** Revision 1.18 2007/08/22 08:31:48EDT njyanchik 
** Add OS config Parameter for the shell input cmd file name for VxWorks6
** Revision 1.17 2007/03/20 09:29:39EST njyanchik 
** I added a counting semaphore implementation to all OS's. This also included removing the #define
** OS_MAX_SEMAPHORES and creating two new ones, OS_MAX_BIN_SEMAPHORES and
** OS_MAX_COUNT_SEMAPHORES in osconfig.h. Also, cfe_es_shell was changed in order to
** accommodate the chanes to the #defines.
** Revision 1.16 2007/02/27 15:22:03EST njyanchik 
** This CP has the initial import of the new file descripor table mechanism
** Revision 1.15 2006/12/11 14:52:42EST apcudmore 
** Added OS_INCLUDE_NETWORK define for including the OSAL network functions. 
** Revision 1.14 2006/10/06 10:16:08EDT njyanchik 
** This update not only  cleans up the file changes associated with the Shell
** Interface, but it also contains changes to config files. These changes include 
** updates for config parameters for both the ES and the OS
** Revision 1.13 2006/09/05 14:43:59GMT-05:00 njyanchik 
** The 2 #defines mentioned in the DCR were moved to the osconfig.h for each of
** of the 3 cpu's
** Revision 1.12 2006/02/27 11:53:06EST njyanchik 
** I removed references to the circular buffer, as well as changed the osx/ linux/ rtems versions 
** to not have a utility task at all.
** Revision 1.11 2006/02/10 09:37:44EST njyanchik 
** Uncommented the #define for turning the utilty task on in the 3 cpu directories
** Revision 1.10 2006/02/07 15:07:42EST apcudmore 
** Updated baseline configuration for vxWorks
** Revision 1.9 2006/02/06 21:17:38GMT rjmcgraw 
** Member moved from osconfig.h in project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-apps/cfe/config/inc/project.pj to osconfig.h in project d:/mksdata/MKS-CFE-PROJECT/fsw/build/cpu1/cfe/config/inc/project.pj.
** Revision 1.7 2006/02/02 08:32:57EST njyanchik 
** A low Priority task is created so it can read the data from the OS_printf circular buffer. That way, if
** it blocks, then the calling task doesn't block. This functionality can be turned off in osconfig.h
** by commenting out the #define for the utility task. If it is commented out, functionality is exactly 
** like it was before this change.
** Revision 1.6 2006/01/20 12:06:02EST njyanchik 
** Changed max buffer size to 172 and moved the  defines into osconfig.h
** Revision 1.5 2006/01/13 15:33:58EST rjmcgraw 
** Member moved from osconfig.h in project d:/mksdata/MKS-CFE-REPOSITORY/config/cpu1/inc/project.pj to osconfig.h in project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-apps/cfecore/config/inc/project.pj.
** Revision 1.3 2005/09/15 17:04:37EDT rmcgraw 
** Changed OS_MAX_PATH_LEN from 128 to 64
** Revision 1.2 2005/07/11 16:40:31EDT apcudmore
**
** Revision 1.1 2005/06/09 10:57:58EDT rperera
** Initial revision
**
******************************************************************************/

#ifndef _osconfig_
#define _osconfig_

/*
** Platform Configuration Parameters for the OS API
*/

#define OS_MAX_TASKS                64
#define OS_MAX_QUEUES               64 
#define OS_MAX_COUNT_SEMAPHORES     20
#define OS_MAX_BIN_SEMAPHORES       20
#define OS_MAX_MUTEXES              20

/*
** Maximum length for an absolute path name
*/
#define OS_MAX_PATH_LEN     64

/* 
** The maxium length allowed for a object (task,queue....) name 
*/
#define OS_MAX_API_NAME     20

/* 
** The maximum length for a file name 
*/
#define OS_MAX_FILE_NAME    20

/* 
** These defines are for OS_printf
*/
#define OS_BUFFER_SIZE 172
#define OS_BUFFER_MSG_DEPTH 100

/* This #define turns on a utility task that
 * will read the statements to print from
 * the OS_printf function. If you want OS_printf
 * to print the text out itself, comment this out 
 * 
 * NOTE: The Utility Task #defines only have meaning 
 * on the VxWorks operating systems
 */
 
#define OS_UTILITY_TASK_ON


#ifdef OS_UTILITY_TASK_ON 
    #define OS_UTILITYTASK_STACK_SIZE 2048
    /* some room is left for other lower priority tasks */
    #define OS_UTILITYTASK_PRIORITY   245
#endif


/* 
** the size of a command that can be passed to the underlying OS 
*/
#define OS_MAX_CMD_LEN 1000

/*
** This define will include the OS network API.
** It should be turned off for targtets that do not have a network stack or 
** device ( like the basic RAD750 vxWorks BSP )
*/
#define OS_INCLUDE_NETWORK

/* 
** This is the maximum number of open file descriptors allowed at a time 
*/
#define OS_MAX_NUM_OPEN_FILES 128

/* 
** This defines the filethe input command of OS_ShellOutputToFile
** is written to in the VxWorks6 port 
*/
#define OS_SHELL_CMD_INPUT_FILE_NAME "/ram/OS_ShellCmd.in"

/* 
** This define sets the queue implentation of the Linux port to use sockets 
** commenting this out makes the Linux port use the POSIX message queues.
*/
/* #define OSAL_SOCKET_QUEUE */

/*
** This define sets the number of memory ranges that are defined in the memory range defintion
** table.
*/
#define OS_MEM_TABLE_SIZE 10


/*
** Module loader/symbol table is optional
*/
#define OS_INCLUDE_MODULE_LOADER

#ifdef OS_INCLUDE_MODULE_LOADER
   /*
   ** This define sets the size of the OS Module Table, which keeps track of the loaded modules in 
   ** the running system. This define must be set high enough to support the maximum number of
   ** loadable modules in the system. If the the table is filled up at runtime, a new module load
   ** would fail.
   */
   #define OS_MAX_MODULES 32 
#endif


/*
** This define sets the maximum symbol name string length. It is used in implementations that 
** support the symbols and symbol lookup.
*/
#define OS_MAX_SYM_LEN 64

/*
** This define sets the maximum number of timers available
*/
#define OS_MAX_TIMERS         5

#endif
