/*
** File: utf_osfilesys.h
**  $Id: utf_osfilesys.h 1.3 2010/10/25 14:09:24GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This file contains the Unit Test Framework function prototypes for utf_osfilesys.c.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/10/25 14:09:24GMT-05:00 $
** $Revision: 1.3 $
** $Log: utf_osfilesys.h  $
** Revision 1.3 2010/10/25 14:09:24GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:05:01EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:09:32EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.3 2006/06/22 14:01:14EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.2 2006/04/10 15:45:16EDT sslegel 
** Added documentation
** Revision 1.1 2006/03/21 16:44:26EST sslegel
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
*/

#ifndef _utf_osfilesys_
#define _utf_osfilesys_

/*
**   Include section
*/
#include "common_types.h"

/*
**   Exported Functions
*/

/******************************************************************************
**  Function:  UTF_add_volume()
**
**  Purpose:
**    Configures the Volume Table for filesystem calls.  Each call to this function
**    will add another entry to the Volume Table.
**
**    There are two ways to configure the filesystem,
**        1. by making a single call to UTF_add_volume to set everything
**        2. by making a call to UTF_add_volume to set only the device name and physical device and then call OS_mkfs and OS_mount
**           The only time you would want to use option 2 is where the task you are testing calls OS_mkfs and OS_mount.  Most of
**           the time the system will do this for you on startup.
**
**    Option 1
**    - adds the device "/nonvol0"
**    - maps the filesystem to a directory "tmp" in the current working directory
**    - is filesystem based
**    - is not volatile
**    - is not free
**    - is mounted
**    - the volume name is "CF"
**    - the virtual filesystem is "/cf", i.e. references to the virtual directory "/cf" get translated
**      to the physical directory "tmp/nonvol0/".  Note that this directory should already exist.
**    - block size is 0
**
**      UTF_add_volume("/nonvol0", "tmp", FS_BASED, FALSE, FALSE, TRUE, "CF", "/cf", 0);
**
**    Option 2
**    - adds the device "/nonvol0"
**    - maps the filesystem to a directory "tmp" in the current working directory
**    - is filesystem based
**    - is not volatile
**    - is free - needed for OS_mkfs
**    - is not mounted - needed for OS_mount
**    - the volume name is " " - set by OS_mkfs
**    - the virtual filesystem is " ", - set by OS_mount
**    - block size is 0
**
**      UTF_add_volume("/nonvol0", "tmp", FS_BASED, FALSE, TRUE, FALSE, " ", " ", 0);
**      status = OS_mkfs(0, "/nonvol0", "CF", 0, 0);
**      status = OS_mount("/nonvol0", "/cf");
*/
void UTF_add_volume(char *DeviceName, char *PhysDevName, uint32 VolumeType, uint8 VolatileFlag,
                    uint8 FreeFlag, uint8 IsMounted, char *VolumeName, char *MountPoint, uint32 BlockSize);

#endif
