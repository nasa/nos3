/*
** File: utf_osfilesys.c
** $Id: utf_osfilesys.c 1.7 2014/04/17 08:14:46GMT-05:00 wmoleski Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Purpose: This module defines the UTF versions of functions for the OS APIs abstraction layer
**          filesystem implementation for POSIX.  The real functions are contained in the file osfileapi.c.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2014/04/17 08:14:46GMT-05:00 $
** $Revision: 1.7 $
** $Log: utf_osfilesys.c  $
** Revision 1.7 2014/04/17 08:14:46GMT-05:00 wmoleski 
** Added the OS_rewidDir and OS_fsBytesFree functions to the UTF source
** Revision 1.6 2012/01/13 12:52:02EST acudmore 
** Changed license text to reflect open source
** Revision 1.5 2010/10/25 15:06:37EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 14:57:54EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/10/13 14:55:26EDT wmoleski 
** Changes required to be compatible with cFE 6.0.0
** Revision 1.2 2008/06/03 13:39:04EDT wfmoleski 
** Updated to contain the OS_FD _Table structure and the code to populate and remove entries from it.
** Revision 1.1 2008/04/17 08:09:40EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.9 2007/05/25 11:12:55EDT wfmoleski 
** Modifications to add new API functions and minor differences from cFE 4.1.0
** Revision 1.8 2006/12/21 08:36:54EST wfmoleski 
** Added this new function to the UTF and modified the example to test this new function.
** Revision 1.7 2006/11/15 12:52:43EST wfmoleski 
** Migrated the fsw changes to the UTF
** Revision 1.6 2006/11/14 08:05:32EST wfmoleski 
** Added OS_ShellOutputToFile and OS_initfs functions to the UTF.
** Revision 1.5 2006/06/22 14:01:14EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.4 2006/04/10 15:40:03EDT sslegel 
** Added documentation and removed code in OS_mkfs to create the file system directory.  For unit testing this directory should already exist.
** Revision 1.3 2006/03/29 13:48:55EST nsschweiss
** Modification to include statements to enable system to build under MacOS:  <sys/param.h> and
** <sys/mount.h> need to be added; <sys/vfs.h> must _not_ be included.
** Revision 1.2 2006/03/24 14:28:23EST sslegel
** Updated file header
**
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#if defined (__ppc__)
#include <sys/param.h>
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#endif

#include "common_types.h"
#include "osapi.h"
#include "utf_types.h"

/****************************************************************************************
                                     DEFINES
****************************************************************************************/

#define ERROR (-1)

int32 OS_TranslatePath(const char *VirtualPath, char *LocalPath);

/****************************************************************************************
                                   GLOBAL DATA
****************************************************************************************/
/*
** This is the volume table reference. It is defined in the BSP/startup code for the board
*/
OS_VolumeInfo_t OS_VolumeTable [NUM_TABLE_ENTRIES] =
{
/* Dev Name  Phys Dev  Vol Type    Volatile?  Free?     IsMounted? Volname  MountPt BlockSz */
{"unused",   "unused",  FS_BASED,   TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"unused",   "unused",  FS_BASED,   TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"unused",   "unused",  FS_BASED,   TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"unused",   "unused",  FS_BASED,   TRUE,      TRUE,     FALSE,     " ",      " ",     0        }
};

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
                    uint8 FreeFlag, uint8 IsMounted, char *VolumeName, char *MountPoint, uint32 BlockSize)
{
    int i;

    if (strlen(DeviceName) > 32)
        UTF_error("Add Volume DeviceName Too Long MAX:32 ACT:%d", strlen(DeviceName));
    else if (strlen(PhysDevName) > 32)
        UTF_error("Add Volume PhysDevName Too Long MAX:32 ACT:%d", strlen(PhysDevName));
    else if (strlen(VolumeName) > 32)
        UTF_error("Add Volume VolumeName Too Long MAX:32 ACT:%d", strlen(VolumeName));
    else if (strlen(MountPoint) > 64)
        UTF_error("Add Volume MountPoint Too Long MAX:32 ACT:%d", strlen(MountPoint));

    /* find an open entry in the Volume Table */
    for (i=0; i < NUM_TABLE_ENTRIES; i++)
    {
        if (OS_VolumeTable[i].FreeFlag == TRUE) {
            strcpy(OS_VolumeTable[i].DeviceName, DeviceName);
            strcpy(OS_VolumeTable[i].PhysDevName, PhysDevName);
            OS_VolumeTable[i].VolumeType = VolumeType;
            OS_VolumeTable[i].VolatileFlag = VolatileFlag;
            OS_VolumeTable[i].IsMounted = IsMounted;
            strcpy(OS_VolumeTable[i].VolumeName, VolumeName);
            strcpy(OS_VolumeTable[i].MountPoint, MountPoint);
            OS_VolumeTable[i].BlockSize = BlockSize;
            OS_VolumeTable[i].FreeFlag = FreeFlag;
            return;
        }
    }

    UTF_error("Volume Table Full");
}

/****************************************************************************************
                                Filesys API
****************************************************************************************/

/*
** System Level API
*/

/*void OS_FS_Init(void)
{ */
    /* nothing to do */
/*} */

/*---------------------------------------------------------------------------------------
    Name: OS_mkfs

    Purpose: Makes a RAM disk on the target with a dos file system

    Returns: OS_FS_ERR_INVALID_POINTER if devname is NULL
             OS_FS_DRIVE_NOT_CREATED if the OS calls to create the the drive failed
             OS_FS_SUCCESS on creating the disk
---------------------------------------------------------------------------------------*/
int32 OS_mkfs (char *address, char *devname,char * volname, uint32 blocksize,
               uint32 numblocks)
{
    int i;

    /* find an open entry in the Volume Table */
    for (i = 0; i < NUM_TABLE_ENTRIES; i++)
    {
        if (OS_VolumeTable[i].FreeFlag == TRUE && OS_VolumeTable[i].IsMounted == FALSE
            && strcmp(OS_VolumeTable[i].DeviceName, devname) == 0)
            break;
    }

    if (i >= NUM_TABLE_ENTRIES)
        return OS_FS_ERR_DEVICE_NOT_FREE;

    /* make a disk if it is FS based */
    /*------------------------------- */
    if (OS_VolumeTable[i].VolumeType == FS_BASED)
    {

       /* now enter the info in the table */

       OS_VolumeTable[i].FreeFlag = FALSE;
       strcpy(OS_VolumeTable[i].VolumeName, volname);
       OS_VolumeTable[i].BlockSize = blocksize;

       /* note we don't know the mount point yet */

    }  /* VolumeType is something else that is not supported right now */
    else
    {
        return OS_FS_ERROR;
    }

    return OS_FS_SUCCESS;

} /* end OS_mkfs */

/*---------------------------------------------------------------------------------------
    Name: OS_initfs

    Purpose: Inititalizes a file system on the target
    
    Returns: OS_FS_ERR_INVALID_POINTER if devname is NULL
             OS_FS_DRIVE_NOT_CREATED if the OS calls to create the the drive failed
             OS_FS_SUCCESS on creating the disk
---------------------------------------------------------------------------------------*/
int32 OS_initfs (char *address,char *devname, char *volname, 
                uint32 blocksize, uint32 numblocks)
{
    int i;
/*    char Command[ OS_MAX_PATH_LEN +10 ]; */
    char FolderName[OS_MAX_PATH_LEN];
    
    /* find an open entry in the Volume Table */
    for (i = 0; i < NUM_TABLE_ENTRIES; i++)
    {
        if (OS_VolumeTable[i].FreeFlag == TRUE && OS_VolumeTable[i].IsMounted == FALSE
            && strcmp(OS_VolumeTable[i].DeviceName, devname) == 0)
            break;
    }

    if (i >= NUM_TABLE_ENTRIES)
        return OS_FS_ERR_DEVICE_NOT_FREE;


    /* make a disk if it is FS based */
    /*------------------------------- */
    if (OS_VolumeTable[i].VolumeType == FS_BASED)
    {

       /* now enter the info in the table */

       OS_VolumeTable[i].FreeFlag = FALSE;
       strcpy(OS_VolumeTable[i].VolumeName, volname);
       OS_VolumeTable[i].BlockSize = blocksize;
    
       /* note we don't know the mount point yet */
    
       /* for linux/osx we need to make the folder where this drive is located */
       strcpy(FolderName, OS_VolumeTable[i].PhysDevName);
       strcat(FolderName, devname);

    
       /* make the directory where the file system lives */
/*       sprintf(Command,"mkdir -p %s", FolderName);
       system(Command); */
       OS_mkdir(FolderName,0);
       
    }   /* VolumeType is something else that is not supported right now */
    else
    {
        return OS_FS_ERROR;
    }

   return OS_FS_SUCCESS; 
}/* end OS_initfs */

/*--------------------------------------------------------------------------------------
    Name: OS_mount

    Purpose: mounts a drive.

---------------------------------------------------------------------------------------*/
int32 OS_mount (const char *devname, char* mountpoint)
{
   int i;
   /* find the device in the table */
    for (i = 0; i < NUM_TABLE_ENTRIES; i++)
    {
        if (OS_VolumeTable[i].FreeFlag == FALSE && OS_VolumeTable[i].IsMounted == FALSE
            && strcmp(OS_VolumeTable[i].DeviceName, devname) == 0)
            break;
    }

    /* make sure we found the device */
    if (i >= NUM_TABLE_ENTRIES)
        return OS_FS_ERR_DRIVE_NOT_CREATED;

    /* attach the mountpoint */
    strcpy(OS_VolumeTable[i].MountPoint, mountpoint);
    OS_VolumeTable[i].IsMounted = TRUE;


    return OS_FS_SUCCESS;
}/* end OS_mount */

/*--------------------------------------------------------------------------------------
    Name: OS_unmount

    Purpose: unmounts a drive. and therefore makes all file descriptors pointing into
             the drive obsolete.

    Returns: OS_FS_ERR_INVALID_POINTER if name is NULL
             OS_FS_ERR_PATH_TOO_LONG if the absolute path given is too long
             OS_FS_ERROR if the OS calls failed
             OS_FS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_unmount (const char *mountpoint)
{
    int i;

    if (mountpoint == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(mountpoint) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    for (i = 0; i < NUM_TABLE_ENTRIES; i++)
    {
        if (OS_VolumeTable[i].FreeFlag == FALSE && OS_VolumeTable[i].IsMounted == TRUE
             && strcmp(OS_VolumeTable[i].MountPoint, mountpoint) == 0)
            break;
    }

    /* make sure we found the device */
    if (i >= NUM_TABLE_ENTRIES)
        return OS_FS_ERROR;

    /* release the informationm from the table */
   OS_VolumeTable[i].IsMounted = FALSE;
   strcpy(OS_VolumeTable[i].MountPoint, "");

    return OS_FS_SUCCESS;

}/* end OS_umount */

/*--------------------------------------------------------------------------------------
    Name: OS_fsBlocksFree

    Purpose: Returns the number of free blocks in a volume

    Returns: OS_FS_INVALID_POINTER if name is NULL
             OS_FS_ERROR if the OS call failed
             The number of bytes free in a volume if success
---------------------------------------------------------------------------------------*/
int32 OS_fsBlocksFree (const char *name)
{

   int           status;
   int32         NameStatus;
   struct statfs stat_buf;
   char          tmpFileName[128];

   if ( name == NULL )
   {
      return(OS_FS_ERR_INVALID_POINTER);
   }
   strncpy(tmpFileName,name,128);
   NameStatus = OS_TranslatePath(name, tmpFileName);
   status = statfs(tmpFileName, &stat_buf);

   if ( status == 0 )
   {
      return(stat_buf.f_bfree);
   }
   return OS_FS_ERROR;

}/* end OS_fsBlocksFree */

/*--------------------------------------------------------------------------------------
    Name: OS_fsBytesFree
        
    Purpose: Returns the number of free bytes in a volume

    Returns: OS_FS_ERR_INVALID_POINTER if name is NULL
             OS_FS_ERROR if the OS call failed
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
       
int32 OS_fsBytesFree (const char *name, uint64 *bytes_free)
{
   int             status;
   int32           NameStatus;
   struct statfs  stat_buf;
   uint64          bytes_free_local;
   char            tmpFileName[OS_MAX_LOCAL_PATH_LEN +1];

   if ( name == NULL || bytes_free == NULL )
   {
      return(OS_FS_ERR_INVALID_POINTER);
   }

   /*
   ** Check the length of the volume name
   */
   if ( strlen(name) >= OS_MAX_PATH_LEN )
   {
      return(OS_FS_ERR_PATH_TOO_LONG);
   }

   /*
   ** Translate the path
   */
   NameStatus = OS_TranslatePath(name, tmpFileName);

   status = statvfs(tmpFileName, &stat_buf);
   if ( status == 0 )
   {
      bytes_free_local = stat_buf.f_bfree * stat_buf.f_bsize;
      *bytes_free = bytes_free_local;
      return(OS_FS_SUCCESS);
   }
   return(OS_FS_ERROR);

}/* end OS_fsBytesFree */

/*--------------------------------------------------------------------------------------
    Name: OS_chkfs

    Purpose: Checks the drives for inconsisenties and either repairs it or not

    Returns: OS_FS_ERR_INVALID_POINTER if name is NULL
             OS_FS_SUCCESS if success
             OS_FS_ERROR if the OS calls fail

---------------------------------------------------------------------------------------*/
os_fshealth_t OS_chkfs (const char *name, boolean repair)
{

/* LOOK INTO USING e2fsck TO CHECK THE FILE SYSTEM !!! */
    return OS_FS_UNIMPLEMENTED;
}/* end OS_chkfs */

/*-------------------------------------------------------------------------------------
 * Name: OS_TranslatePath
 *
 * Purpose: Because of the abstraction of the filesystem across OSes, we have to change
 *          the name of the {file, directory, drive} to be what the OS can actually
 *          accept
---------------------------------------------------------------------------------------*/
int32 OS_TranslatePath(const char* VirtualPath, char *LocalPath)
{
    char devname [OS_MAX_PATH_LEN];
    char filename[OS_MAX_PATH_LEN];
    int NumChars;
    int  DeviceLen;
    int  FilenameLen;
    int i=0;

    /*
    ** Check to see if the path pointers are NULL
    */
    if (VirtualPath == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (LocalPath == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    /*
    ** Check to see if the path is too long
    */
    if (strlen(VirtualPath) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;
    /*
    ** All valid Virtual paths must start with a '/' character
    */
    if ( VirtualPath[0] != '/' )
       return OS_FS_ERR_PATH_INVALID;

    /*
    ** Fill the file and device name to be sure they do not have garbage
    */
    memset((void *)devname,0,OS_MAX_PATH_LEN);
    memset((void *)filename,0,OS_MAX_PATH_LEN);

    /*
    ** We want to find the number of chars to where the second "/" is.
    ** Since we know the first one is in spot 0, we start looking at 1,
    ** and go until we find it.
    */
    NumChars = 1;
    while ((VirtualPath[NumChars] != '/') && (NumChars <= strlen(VirtualPath)))
    {
        NumChars++;
    }

    /*
    ** Don't let it overflow to cause a segfault when trying to get the highest 
    ** level directory
    */
    if (NumChars > strlen(VirtualPath))
        NumChars = strlen(VirtualPath);

    /*
    ** copy over only the part that is the device name
    */
    strncpy(devname, VirtualPath, NumChars);
    devname[NumChars] = '\0'; /* Truncate it with a NULL. */
    DeviceLen = strlen(devname);

    /*
    ** Copy everything after the devname as the path/filename
    */
    strncpy(filename, &(VirtualPath[NumChars]), OS_MAX_PATH_LEN);
    FilenameLen = strlen(filename);

#if 0
    printf("VirtualPath: %s, Length: %d\n",VirtualPath,(int)strlen(VirtualPath));
    printf("NumChars: %d\n",NumChars);
    printf("devname: %s\n",devname);
    printf("filename: %s\n",filename);
#endif

    /*
    ** look for the dev name we found in the VolumeTable
    */
    for (i = 0; i < NUM_TABLE_ENTRIES; i++)
    {
        if (OS_VolumeTable[i].FreeFlag == FALSE &&
            strncmp(OS_VolumeTable[i].MountPoint, devname,NumChars) == 0)
        {
            break;
        }
    }

    /*
    ** Make sure we found a valid drive
    */
    if (i >= NUM_TABLE_ENTRIES)
        return OS_FS_ERR_DRIVE_NOT_CREATED;

    /*
    ** copy over the physical first part of the drive
    */
    strncpy(LocalPath,OS_VolumeTable[i].PhysDevName,OS_MAX_LOCAL_PATH_LEN);
    NumChars = strlen(LocalPath);

    /*
    ** Add the device name ( Linux, Cygwin, OS X only )
    */
    strncat(LocalPath, OS_VolumeTable[i].DeviceName, (OS_MAX_LOCAL_PATH_LEN - NumChars));
    NumChars = strlen(LocalPath);

    /*
    ** Add the file name
    */
    strncat(LocalPath, filename, (OS_MAX_LOCAL_PATH_LEN - NumChars));

#if 0
    printf("Result of TranslatePath = %s\n",LocalPath);
#endif

    return OS_FS_SUCCESS;

} /* end OS_TranslatePath*/

/*---------------------------------------------------------------------------------------
    Name: OS_FS_GetErrorName()

    Purpose: a handy debugging tool that will copy the name of the error code to a buffer

    Returns: OS_FS_ERROR if given error number is unknown
             OS_FS_SUCCESS if given error is found and copied to the buffer
--------------------------------------------------------------------------------------- */
int32 OS_FS_GetErrorName(int32 error_num, os_fs_err_name_t * err_name)
{
    os_fs_err_name_t local_name;
    int32 return_code;

    return_code = OS_FS_SUCCESS;

    switch (error_num)
    {
        case OS_FS_SUCCESS:
            strcpy(local_name,"OS_FS_SUCCESS"); break;
        case OS_FS_ERROR:
            strcpy(local_name,"OS_FS_ERROR"); break;
        case OS_FS_ERR_INVALID_POINTER:
            strcpy(local_name,"OS_FS_ERR_INVALID_POINTER"); break;
        case OS_FS_ERR_PATH_TOO_LONG:
            strcpy(local_name,"OS_FS_ERR_PATH_TOO_LONG"); break;
        case OS_FS_ERR_NAME_TOO_LONG:
            strcpy(local_name,"OS_FS_ERR_NAME_TOO_LONG"); break;
        case OS_FS_UNIMPLEMENTED:
            strcpy(local_name,"OS_FS_UNIMPLEMENTED"); break;
        case OS_FS_ERR_DRIVE_NOT_CREATED:
            strcpy(local_name,"OS_FS_ERR_DRIVE_NOT_CREATED"); break;

        default: strcpy(local_name,"ERROR_UNKNOWN");
                 return_code = OS_FS_ERROR;
    }

    strcpy((char*) err_name, local_name);

    return return_code;
}

/*--------------------------------------------------------------------------------------
    Name: OS_FS_GetPhysDriveName
    
    Purpose: Gets the name of the physical volume underneath the drive,
             when given the mount point of the drive

    Returns: OS_FS_ERR_INVALID_POINTER if either  parameter is NULL
             OS__SUCCESS if success
             OS_FS_ERROR if the mountpoint could not be found
---------------------------------------------------------------------------------------*/
int32 OS_FS_GetPhysDriveName(char * PhysDriveName, char * MountPoint)
{
    char LocalDrvName [OS_MAX_LOCAL_PATH_LEN];
    int32 ReturnCode;
    int32 status;
    
    if (MountPoint == NULL || PhysDriveName == NULL)
    {
        return OS_FS_ERR_INVALID_POINTER;
    }

    status = OS_TranslatePath((const char *)MountPoint, (char *)LocalDrvName);
    if (status != OS_SUCCESS)
    {
        ReturnCode = OS_FS_ERROR;
    }
    else
    {
        ReturnCode = OS_SUCCESS;
        strcpy(PhysDriveName,LocalDrvName);
    }

    return ReturnCode;
}/* end OS_FS_GetPhysDriveName */

