/*
** File: utf_osfileapi.c
** $Id: utf_osfileapi.c 1.12 2014/04/17 08:14:46GMT-05:00 wmoleski Exp  $
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
** $Revision: 1.12 $
** $Log: utf_osfileapi.c  $
** Revision 1.12 2014/04/17 08:14:46GMT-05:00 wmoleski 
** Added the OS_rewidDir and OS_fsBytesFree functions to the UTF source
** Revision 1.11 2012/01/13 12:52:01EST acudmore 
** Changed license text to reflect open source
** Revision 1.10 2010/10/25 15:06:38EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.9 2010/10/04 14:57:52EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.8 2010/09/20 13:42:58EDT wmoleski 
** The UTF source prototypes for OS_API_Init, OS_FS_Init, OS_TaskCreate and OS_TimerAPIInit were
** modified to match the OSAL 3.1 changes.
** Revision 1.7 2009/10/13 14:55:25EDT wmoleski 
** Changes required to be compatible with cFE 6.0.0
** Revision 1.6 2008/09/04 07:24:42EDT wfmoleski 
** The UTF implementation did not have the return code setting code implemented in OS_lseek. This code  was added.
** Revision 1.5 2008/09/04 07:19:36EDT wfmoleski 
** Function hook support was added for the OS_close function. Test code was also added.
** Revision 1.4 2008/06/03 13:52:10EDT wfmoleski 
** Added the OS_opendir and OS_readdir function hooks that were needed by the CFS File Manager
** to complete unit testing.
** Revision 1.3 2008/06/03 13:43:13EDT wfmoleski 
** The return code argument was modified from a uint32 to an int32 to accomodate the OS return
** codes that have negative values.
** Revision 1.2 2008/06/03 13:39:04EDT wfmoleski 
** Updated to contain the OS_FD _Table structure and the code to populate and remove entries from it.
** Revision 1.1 2008/04/17 08:09:40EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.9 2008/04/07 09:39:36EDT wfmoleski 
** Added the capability for the specified hook functions. Added return code setting code for several
** OS functions that seemed appropriate.
** Revision 1.8 2007/06/05 08:32:48EDT wfmoleski 
** Updated to reflect the cFE 4.1.0 implementation.
** Revision 1.7 2007/05/25 11:12:55EDT wfmoleski 
** Modifications to add new API functions and minor differences from cFE 4.1.0
** Revision 1.6 2006/11/15 12:52:42EST wfmoleski 
** Migrated the fsw changes to the UTF
** Revision 1.5 2006/11/14 08:05:32EST wfmoleski 
** Added OS_ShellOutputToFile and OS_initfs functions to the UTF.
** Revision 1.4 2006/10/03 10:55:28EDT wfmoleski 
** Changed the UTF function signatures that changed due to the original change to the fsw.
** Revision 1.3 2006/06/22 14:01:13EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.2 2006/03/24 14:28:02EST sslegel 
** Updated file header
**
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "common_types.h"
#include "osapi.h"
#include "utf_types.h"
#include "utf_osfileapi.h"

/****************************************************************************************
                                     DEFINES
****************************************************************************************/

#define ERROR -1

/****************************************************************************************
                                   GLOBAL DATA
****************************************************************************************/
OS_FDTableEntry OS_FDTable[OS_MAX_NUM_OPEN_FILES];

/* Function Hook Stuff */
typedef struct
{
    int32 (*OS_creat)(const char *,int32);
    int32 (*OS_write)(int32, void *, uint32);
    int32 (*OS_read)(int32, void *, uint32);
    os_dirp_t (*OS_opendir)(const char *path);
    os_dirent_t* (*OS_readdir) (os_dirp_t directory);
    int32 (*OS_close) (int32  filedes);
} UTF_OSFILEAPI_HookTable_t;

UTF_OSFILEAPI_HookTable_t UTF_OSFILEAPI_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

int32 os_fileapi_return_value[NUM_OF_OSFILEAPI_PROCS]=
 {
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSFILEAPI_Set_Api_Return_Code() --                                   */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSFILEAPI_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode)
{
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_OSFILEAPI_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_OSFILEAPI_Set_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }
   /* If index is valid, assign return value and report action */
   os_fileapi_return_value[ApiIndex] = ReturnCode;

   if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
   	  UTF_put_text("UTF Info: Return code for OSFILEAPI Api #%d returned to default\n",
   	               ApiIndex);
   }
   else
   {
   	  UTF_put_text("UTF Info: Return value for OSFILEAPI Api #%d set to %lx\n", 
   	                ApiIndex, ReturnCode);
   }
   return OS_FS_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSFILEAPI_Use_Default_Api_Return_Code() --                          */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSFILEAPI_Use_Default_Api_Return_Code(int32 ApiIndex)
{
	int32 Success;
	
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_OSFILEAPI_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_OSFILEAPI_Use_Default_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }

   /* If index is valid, assign return value and report action */
   Success = UTF_OSFILEAPI_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);
   
   return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_OSFILEAPI_Set_Api_Return_Code() --                           */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_OSFILEAPI_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
	/* Check for correct number of arguments */
	if (argc != 3)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_OSFILEAPI_Set_Api_Return_Code. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
	UTF_OSFILEAPI_Set_Api_Return_Code(Index, Code);
	return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_OSFILEAPI_Use_Default_Api_Return_Code() --                   */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_OSFILEAPI_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

	/* Check for correct number of arguments */
	if (argc != 2)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_OSFILEAPI_Set_Api_Return_Code. Expected 1.\n",
	   argc -1 );
	   UTF_exit();
	}
 
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
 
    /* Call function to set API return code */
	UTF_OSFILEAPI_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
	return;
}

/****************************************************************************************
                                INITIALIZATION FUNCTION
****************************************************************************************/
int32 OS_check_name_length(const char *path);
int32 OS_TranslatePath(const char *VirtualPath, char *LocalPath);

/******************************************************************************
**  Function: UTF_OSAPI_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a OSAPI function call.
*/
void UTF_OSFILEAPI_set_function_hook(int Index, void *FunPtr)
{
    if      (Index == OS_CREAT_HOOK)  { UTF_OSFILEAPI_HookTable.OS_creat = FunPtr; }
    else if (Index == OS_WRITE_HOOK)  { UTF_OSFILEAPI_HookTable.OS_write = FunPtr; }
    else if (Index == OS_READ_HOOK)   { UTF_OSFILEAPI_HookTable.OS_read = FunPtr; }
    else if (Index == OS_OPENDIR_HOOK) { UTF_OSFILEAPI_HookTable.OS_opendir = FunPtr; }
    else if (Index == OS_READDIR_HOOK) { UTF_OSFILEAPI_HookTable.OS_readdir = FunPtr; }
    else if (Index == OS_CLOSE_HOOK)  { UTF_OSFILEAPI_HookTable.OS_close = FunPtr; }
    else                              { UTF_error("Invalid OSFILEAPI Hook Index In Set Hook Call %d", Index); }
}

/****************************************************************************************
                                INITIALIZATION FUNCTION
****************************************************************************************/

int32 OS_FS_Init(void)
{
    int i;
    /* Initialize the file system constructs */
    for (i =0; i < OS_MAX_NUM_OPEN_FILES; i++)
    {
        OS_FDTable[i].OSfd =       -1;
        strcpy(OS_FDTable[i].Path, "\0");
        OS_FDTable[i].User =       0;
        OS_FDTable[i].IsValid =    FALSE;
    }

    return OS_SUCCESS;
}
/****************************************************************************************
                                    Filesys API
****************************************************************************************/

/*
** Standard File system API
*/

/*--------------------------------------------------------------------------------------
    Name: OS_creat

    Purpose: creates a file specified by const char *path, with read/write
             permissions by access. The file is also automatically opened by the
             create call.

    Returns: OS_FS_INVALID_POINTER if path is NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_PATH_TOO_LONG if path exceeds the maximum number of chars
             OS_FS_ERR_NAME_TOO_LONG if the name of the file is too long
             OS_FS_ERROR if permissions are unknown or OS call fails
             OS_FS_SUCCESS if success

---------------------------------------------------------------------------------------*/

int32 OS_creat  (const char *path, int32  access)
{
    int status;
    char local_path[OS_MAX_LOCAL_PATH_LEN];
    int perm;
    mode_t mode;
    uint32 PossibleFD;
    
   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_CREAT_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_CREAT_PROC];
   	}

	/* Handle the Function Hook */
    if (UTF_OSFILEAPI_HookTable.OS_creat)
        return(UTF_OSFILEAPI_HookTable.OS_creat(path,access));

    if (path == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(path) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** check if the name of the file is too long
    */
    if (OS_check_name_length(path) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    /* 
    ** Translate path
    */
    if (OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS)
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    for ( PossibleFD = 0; PossibleFD < OS_MAX_NUM_OPEN_FILES; PossibleFD++)
    {
        if( OS_FDTable[PossibleFD].IsValid == FALSE)
        {
            break;
        }
    }

    if (PossibleFD >= OS_MAX_NUM_OPEN_FILES)
    {
        return OS_FS_ERR_NO_FREE_FDS;
    }

    /* Mark the table entry as valid so no other 
     * task can take that ID */
    OS_FDTable[PossibleFD].IsValid =    TRUE; 

    switch(access)
    {
        case OS_READ_ONLY:
            perm = O_RDONLY;
            break;
        case OS_WRITE_ONLY:
            perm = O_WRONLY;
            break;
        case OS_READ_WRITE:
            perm = O_RDWR;
            break;
        default:
            return OS_FS_ERROR;
    }

    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    status =  open(local_path, perm | O_CREAT, mode);

    if (status != ERROR)
    {
        OS_FDTable[PossibleFD].OSfd = status;
        strncpy(OS_FDTable[PossibleFD].Path, path, OS_MAX_PATH_LEN);
        OS_FDTable[PossibleFD].User = 0;
        
        return PossibleFD;
    }
    else
    {
    	OS_FDTable[PossibleFD].IsValid = FALSE; 
        return OS_FS_ERROR;
    }

} /* end OS_creat */

/*--------------------------------------------------------------------------------------
    Name: OS_open

    Purpose: Opens a file. access parameters are OS_READ_ONLY,OS_WRITE_ONLY, or
             OS_READ_WRITE

    Returns: OS_FS_INVALID_POINTER if path is NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_PATH_TOO_LONG if path exceeds the maximum number of chars
             OS_FS_ERR_NAME_TOO_LONG if the name of the file is too long
             OS_FS_ERROR if permissions are unknown or OS call fails
             a file descriptor if success
---------------------------------------------------------------------------------------*/

int32 OS_open   (const char *path,  int32 access,  uint32  mode)
{
    int status;
    char local_path[OS_MAX_LOCAL_PATH_LEN];
    int perm;
    uint32 PossibleFD;
    
   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_OPEN_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_OPEN_PROC];
   	}

    if(path == NULL)
       return OS_FS_ERR_INVALID_POINTER;

    if (strlen(path) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** check if the name of the file is too long
    */
    if (OS_check_name_length(path) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    for ( PossibleFD = 0; PossibleFD < OS_MAX_NUM_OPEN_FILES; PossibleFD++)
    {
        if( OS_FDTable[PossibleFD].IsValid == FALSE)
        {
            break;
        }
    }

    if (PossibleFD >= OS_MAX_NUM_OPEN_FILES)
    {
        return OS_FS_ERR_NO_FREE_FDS;
    }

    /* Mark the table entry as valid so no other 
     * task can take that ID */
    OS_FDTable[PossibleFD].IsValid =    TRUE;
    
    switch(access)
    {
        case OS_READ_ONLY:
            perm = O_RDONLY;
            break;
        case OS_WRITE_ONLY:
            perm = O_WRONLY;
            break;
        case OS_READ_WRITE:
            perm = O_RDWR;
            break;
        default:
            return OS_FS_ERROR;
    }

    /* open the file with the R/W permissions */

    status =  open(local_path,perm);

    if (status != ERROR)
    {
        OS_FDTable[PossibleFD].OSfd = status;
        strncpy(OS_FDTable[PossibleFD].Path, path, OS_MAX_PATH_LEN);
        OS_FDTable[PossibleFD].User = 0;

        return PossibleFD;
    }
    else
    {
    	OS_FDTable[PossibleFD].IsValid = FALSE;
        return OS_FS_ERROR;
    }

} /* end OS_open */


/*--------------------------------------------------------------------------------------
    Name: OS_close

    Purpose: Closes a file.

    Returns: OS_FS_ERROR if file  descriptor could not be closed
             OS_FS_SUCCESS if success
---------------------------------------------------------------------------------------*/

int32 OS_close (int32  filedes)
{
    int status=0;
    
   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_CLOSE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_CLOSE_PROC];
   	}

	/* Handle the Function Hook */
    if (UTF_OSFILEAPI_HookTable.OS_close)
        return(UTF_OSFILEAPI_HookTable.OS_close(filedes));
        
    /* Make sure the file descriptor is legit before using it */
    if (filedes < 0 || filedes >= OS_MAX_NUM_OPEN_FILES || OS_FDTable[filedes].IsValid == FALSE)
    {
        return OS_FS_ERR_INVALID_FD;
    }
    else
    {    
       status = close ((int) OS_FDTable[filedes].OSfd);
       if (status == ERROR)
       {
           return OS_FS_ERROR;
       }
       else
       {
           /* fill in the table before returning */
          OS_FDTable[filedes].OSfd =       -1;
          strcpy(OS_FDTable[filedes].Path, "\0");
          OS_FDTable[filedes].User =       0;
          OS_FDTable[filedes].IsValid =    FALSE;
          return OS_FS_SUCCESS;
       }
    }

}/* end OS_close */

/*--------------------------------------------------------------------------------------
    Name: OS_read

    Purpose: reads up to nbytes from a file, and puts them into buffer.

    Returns: OS_FS_ERR_INVALID_POINTER if buffer is a null pointer
             OS_FS_ERROR if OS call failed
             number of bytes read if success
---------------------------------------------------------------------------------------*/
int32 OS_read  (int32  filedes, void *buffer, uint32 nbytes)
{
    int status;

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_READ_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_READ_PROC];
   	}
	/* Handle the Function Hook */
    if (UTF_OSFILEAPI_HookTable.OS_read)
        return(UTF_OSFILEAPI_HookTable.OS_read(filedes,buffer,nbytes));

    if (buffer == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    /* Make sure the file descriptor is legit before using it */
    if (filedes < 0 || filedes >= OS_MAX_NUM_OPEN_FILES || OS_FDTable[filedes].IsValid == FALSE)
    {
        return OS_FS_ERR_INVALID_FD;
    }
    else
    { 
        status = read (OS_FDTable[filedes].OSfd, (char*) buffer, (size_t) nbytes);
 
        if (status == ERROR)
            return OS_FS_ERROR;
    }

    return status;
}/* end OS_read */

/*--------------------------------------------------------------------------------------
    Name: OS_write

    Purpose: writes to a file. copies up to a maximum of nbtyes of buffer to the file
             described in filedes

    Returns: OS_FS_INVALID_POINTER if buffer is NULL
             OS_FS_ERROR if OS call failed
             number of bytes written if success
---------------------------------------------------------------------------------------*/

int32 OS_write (int32  filedes, void *buffer, uint32 nbytes)
{
    int status;

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_WRITE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_WRITE_PROC];
   	}
	/* Handle the Function Hook */
    if (UTF_OSFILEAPI_HookTable.OS_write)
        return(UTF_OSFILEAPI_HookTable.OS_write(filedes,buffer,nbytes));

    if (buffer == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    /* Make sure the file descriptor is legit before using it */
    if (filedes < 0 || filedes >= OS_MAX_NUM_OPEN_FILES || OS_FDTable[filedes].IsValid == FALSE)
    {
        return OS_FS_ERR_INVALID_FD;
    }
    else
    {
        status = write(OS_FDTable[filedes].OSfd, buffer, nbytes );
    
        if (status != ERROR)
            return  status;
        else
            return OS_FS_ERROR;
    }    

}/* end OS_write */


/*--------------------------------------------------------------------------------------
    Name: OS_chmod

    Notes: This is not going to be implemented because there is no use for this function.
---------------------------------------------------------------------------------------*/

int32 OS_chmod  (const char *path, uint32 access)
{
   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_CHMOD_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_CHMOD_PROC];
   	}

     return OS_FS_UNIMPLEMENTED;
} /* end OS_chmod */

/*--------------------------------------------------------------------------------------
    Name: OS_stat

    Purpose: returns information about a file or directory in a os_fs_stat structure

    Returns: OS_FS_ERR_INVALID_POINTER if path or filestats is NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ERR_PATH_TOO_LONG if the path is too long to be stored locally
*****        OS_FS_ERR_NAME_TOO_LONG if the name of the file is too long to be stored
             OS_FS_ERROR id the OS call failed
             OS_FS_SUCCESS if success

    Note: The information returned is in the structure pointed to by filestats
---------------------------------------------------------------------------------------*/

int32 OS_stat   (const char *path, os_fstat_t *filestats)
{
   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_STAT_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_STAT_PROC];
   	}

    int ret_val;
    char local_path[OS_MAX_LOCAL_PATH_LEN];

    if (path == NULL || filestats == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(path) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** check if the name of the file is too long
    */
    if (OS_check_name_length(path) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    ret_val = stat( (char*) local_path, filestats);
    if (ret_val == ERROR)
        return OS_FS_ERROR;
    else
        return OS_FS_SUCCESS;

} /* end OS_stat */

/*--------------------------------------------------------------------------------------
    Name: OS_lseek

    Purpose: sets the read/write pointer to a specific offset in a specific file.
             Whence is either OS_SEEK_SET,OS_SEEK_CUR, or OS_SEEK_END

    Returns: the new offset from the beginning of the file
             OS_FS_ERROR if OS call failed
---------------------------------------------------------------------------------------*/

int32 OS_lseek  (int32  filedes, int32 offset, uint32 whence)
{
     off_t status;
     int where;

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_LSEEK_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_LSEEK_PROC];
   	}

    /* Make sure the file descriptor is legit before using it */
    if (filedes < 0 || filedes >= OS_MAX_NUM_OPEN_FILES || OS_FDTable[filedes].IsValid == FALSE)
    {
        return OS_FS_ERR_INVALID_FD;
    }
    else
    {
        switch(whence)
        {
            case OS_SEEK_SET:
                where = SEEK_SET;
                break;
            case OS_SEEK_CUR:
                where = SEEK_CUR;
                break;
            case OS_SEEK_END:
                where = SEEK_END;
                break;
            default:
                return OS_FS_ERROR;
        }

    
        status = lseek( OS_FDTable[filedes].OSfd, (int) offset, (int) where );

        if ( (int) status != ERROR)
            return (int32) status;
        else
            return OS_FS_ERROR;
    } 
}/* end OS_lseek */

/*--------------------------------------------------------------------------------------
    Name: OS_remove

    Purpose: removes a given filename from the drive

    Returns: OS_FS_SUCCESS if the driver returns OK
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ERROR if there is no device or the driver returns error
             OS_FS_ERR_INVALID_POINTER if path is NULL
             OS_FS_ERR_PATH_TOO_LONG if path is too long to be stored locally
             OS_FS_ERR_NAME_TOO_LONG if the name of the file to remove is too long to be
             stored locally
---------------------------------------------------------------------------------------*/

int32 OS_remove (const char *path)
{
    int status;
    char local_path[OS_MAX_LOCAL_PATH_LEN];

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_REMOVE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_REMOVE_PROC];
   	}

    if (path == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(path) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /* check if the name of the file is too long */
    if (OS_check_name_length(path) == OS_FS_ERROR)
        return OS_FS_ERR_NAME_TOO_LONG;

    /*
    ** Translate the path
    */  
    if ( OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    status = remove (local_path);
    if (status != ERROR)
        return OS_FS_SUCCESS;
    else
        return OS_FS_ERROR;

} /* end OS_remove */

/*--------------------------------------------------------------------------------------
    Name: OS_rename

    Purpose: renames a file

    Returns: OS_FS_SUCCESS if the rename works
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ERROR if the file could not be opened or renamed.
             OS_FS_INVALID_POINTER if old or new are NULL
             OS_FS_ERR_PATH_TOO_LONG if the paths given are too long to be stored locally
             OS_FS_ERR_NAME_TOO_LONG if the new name is too long to be stored locally
---------------------------------------------------------------------------------------*/

int32 OS_rename (const char *old, const char *new)
{
    int status;
    char old_path[OS_MAX_LOCAL_PATH_LEN];
    char new_path[OS_MAX_LOCAL_PATH_LEN];

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_RENAME_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_RENAME_PROC];
   	}

    if (old == NULL || new == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(old) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    if (strlen(new) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** check if the names of the files are too long
    */
    if (OS_check_name_length(old) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    if (OS_check_name_length(new) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(old, (char *)old_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(new, (char *)new_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

     status = rename (old_path, new_path);
     if (status != ERROR)
         return OS_FS_SUCCESS;
     else
         return OS_FS_ERROR;

}/*end OS_rename */

/*--------------------------------------------------------------------------------------
    Name: OS_cp
    
    Purpose: Copies a single file from src to dest

    Returns: OS_FS_SUCCESS if the operation worked
             OS_FS_ERROR if the file could not be accessed
             OS_FS_INVALID_POINTER if src or dest are NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ERR_PATH_TOO_LONG if the paths given are too long to be stored locally
             OS_FS_ERR_NAME_TOO_LONG if the dest name is too long to be stored locally
---------------------------------------------------------------------------------------*/
int32 OS_cp (const char *src, const char *dest)
{
    int status = OS_FS_SUCCESS;
    char src_path[OS_MAX_LOCAL_PATH_LEN];
    char dest_path[OS_MAX_LOCAL_PATH_LEN];

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_CP_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_CP_PROC];
   	}

    /* leavenough space for the two paths and the command */
    char command [OS_MAX_PATH_LEN * 2 + 5];

    if (src == NULL || dest == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(src) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;
    
    if (strlen(dest) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** check if the names of the files are too long
    */
    if (OS_check_name_length(src) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    if (OS_check_name_length(dest) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(src, (char *)src_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(dest, (char *)dest_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

/*     status = cp(src_path, dest_path); */
    sprintf(command,"cp %s %s",src_path, dest_path);
     
     status = system(command);     
     if (status != ERROR)
         status = OS_FS_SUCCESS;
     else
         status = OS_FS_ERROR; 

  return status;
     
}/*end OS_cp */

/*--------------------------------------------------------------------------------------
    Name: OS_mv
    
    Purpose: moves a single file from src to dest

    Returns: OS_FS_SUCCESS if the operation worked
             OS_FS_ERROR if the file could not be accessed
             OS_FS_INVALID_POINTER if src or dest are NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ERR_PATH_TOO_LONG if the paths given are too long to be stored locally
             OS_FS_ERR_NAME_TOO_LONG if the dest name is too long to be stored locally
---------------------------------------------------------------------------------------*/
int32 OS_mv (const char *src, const char *dest)
{
    int status = OS_FS_SUCCESS;
    char src_path[OS_MAX_LOCAL_PATH_LEN];
    char dest_path[OS_MAX_LOCAL_PATH_LEN];

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_MV_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_MV_PROC];
   	}

    /* leavenough space for the two paths and the command */
    char command [OS_MAX_LOCAL_PATH_LEN * 2 + 5];

    if (src == NULL || dest == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(src) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;
    
    if (strlen(dest) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** check if the names of the files are too long
    */
    if (OS_check_name_length(src) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    if (OS_check_name_length(dest) != OS_FS_SUCCESS)
        return OS_FS_ERR_NAME_TOO_LONG;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(src, (char *)src_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(dest, (char *)dest_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

/*    status = mv(src_path, dest_path); */

    sprintf(command,"mv %s %s",src_path, dest_path);
     
     status = system(command);    
     if (status != ERROR)
         status = OS_FS_SUCCESS;
     else
         status = OS_FS_ERROR;

  return status;
  
}/*end OS_mv */

/*
** Directory API
*/
/*--------------------------------------------------------------------------------------
    Name: OS_mkdir

    Purpose: makes a directory specified by path.

    Returns: OS_FS_ERR_INVALID_POINTER if path is NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ERR_PATH_TOO_LONG if the path is too long to be stored locally
             OS_FS_ERROR if the OS call fails
             OS_FS_SUCCESS if success

    Note: The access parameter is currently unused.
---------------------------------------------------------------------------------------*/

int32 OS_mkdir (const char *path, uint32 access)
{
   int status;
   mode_t mode;
   char local_path[OS_MAX_LOCAL_PATH_LEN];

   /* Handle Preset Return Code */
   if (os_fileapi_return_value[OS_MKDIR_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
	return os_fileapi_return_value[OS_MKDIR_PROC];
   }

   if (path == NULL)
        return OS_FS_ERR_INVALID_POINTER;

   if (strlen(path) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** Translate the path
    */  
    if (OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

/*    mode = S_IFDIR |S_IRWXU | S_IRWXG | S_IRWXO; */
    mode = S_IRWXU | S_IRWXG | S_IRWXO;
    status = mkdir(local_path, mode);

    if (status != ERROR)
    {
        return OS_FS_SUCCESS;
    }
    else
    {
        return OS_FS_ERROR;
    }

}/* end OS_mkdir */

/*--------------------------------------------------------------------------------------
    Name: OS_opendir

    Purpose: opens a directory for searching

    Returns: NULL if path is NULL,path is too long, OS call fails
             a pointer to a directory if success
---------------------------------------------------------------------------------------*/


os_dirp_t OS_opendir (const char *path)
{
    os_dirp_t dirdescptr;
    char local_path[OS_MAX_LOCAL_PATH_LEN];

	/* Handle the Function Hook */
    if (UTF_OSFILEAPI_HookTable.OS_opendir)
        return(UTF_OSFILEAPI_HookTable.OS_opendir(path));

    if (path == NULL)
        return NULL;

    if (strlen(path) > OS_MAX_PATH_LEN)
        return NULL;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS )
    {
        return NULL;
    }

    dirdescptr = opendir( (char*) local_path);

    if (dirdescptr == NULL)
        return NULL;
    else
        return dirdescptr;

} /* end OS_opendir */

/*--------------------------------------------------------------------------------------
    Name: OS_closedir

    Purpose: closes a directory

    Returns: OS_FS_SUCCESS if success
             OS_FS_ERROR if close failed
---------------------------------------------------------------------------------------*/

int32 OS_closedir (os_dirp_t directory)
{
    int status;

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_CLOSEDIR_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_CLOSEDIR_PROC];
   	}

    if (directory == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    status = closedir(directory);
    if (status != ERROR)
        return OS_FS_SUCCESS;
    else
        return OS_FS_ERROR;

} /* end OS_closedir */

/*--------------------------------------------------------------------------------------
    Name: OS_readdir

    Purpose: obtains directory entry data for the next file from an open directory

    Returns: a pointer to the next entry for success
             NULL if error or end of directory is reached
---------------------------------------------------------------------------------------*/


os_dirent_t *  OS_readdir (os_dirp_t directory)
{
    os_dirent_t *tempptr;

	/* Handle the Function Hook */
    if (UTF_OSFILEAPI_HookTable.OS_readdir)
        return(UTF_OSFILEAPI_HookTable.OS_readdir(directory));

    if (directory == NULL)
        return NULL;

    tempptr = readdir( directory);

    if (tempptr != NULL)
        return tempptr;

    else
        return NULL;


    /* should never reach this point in the code */
    return NULL;

} /* end OS_readdir */

/*--------------------------------------------------------------------------------------
    Name: OS_rewinddir

    Purpose: Rewinds the directory pointer

    Returns: N/A
---------------------------------------------------------------------------------------*/
void  OS_rewinddir (os_dirp_t directory )
{
    if (directory != NULL)
    {
       rewinddir( directory);
    }
}

/*--------------------------------------------------------------------------------------
    Name: OS_rmdir

    Purpose: removes a directory from  the structure (must be an empty directory)

    Returns: OS_FS_ERR_INVALID_POINTER if path us NULL
             OS_FS_ERR_PATH_INVALID if path cannot be parsed
             OS_FS_ER_PATH_TOO_LONG
---------------------------------------------------------------------------------------*/


int32  OS_rmdir (const char *path)
{
    int status;
    char local_path [OS_MAX_LOCAL_PATH_LEN];

   	/* Handle Preset Return Code */
   	if (os_fileapi_return_value[OS_RMDIR_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_fileapi_return_value[OS_RMDIR_PROC];
   	}

    if (path == NULL)
        return OS_FS_ERR_INVALID_POINTER;

    if (strlen(path) >= OS_MAX_PATH_LEN)
        return OS_FS_ERR_PATH_TOO_LONG;

    /*
    ** Translate the path
    */
    if ( OS_TranslatePath(path, (char *)local_path) != OS_FS_SUCCESS )
    {
        return OS_FS_ERR_PATH_INVALID;
    }

    status = rmdir(local_path);

    if (status != ERROR)
        return OS_FS_SUCCESS;
    else
        return OS_FS_ERROR;

}/* end OS_rmdir */

/* --------------------------------------------------------------------------------------
    Name: OS_check_path_length

    Purpose: Checks the length of the file name at the end of the path.

    Returns: OS_FS_ERROR if path is NULL, path is too long, there is no '/' in the path
             name, the name is too long
             OS_SUCCESS if success

    NOTE: This is only an internal function and is not intended for use by the user
 ---------------------------------------------------------------------------------------*/

int32 OS_check_name_length(const char *path)
{
    char* name_ptr;
    char* end_of_path;
    int name_len;

    if (path == NULL)
        return OS_FS_ERR_INVALID_POINTER;


    if (strlen(path) > OS_MAX_PATH_LEN)
        return OS_FS_ERROR;
    /* checks to see if there is a '/' somewhere in the path */
    name_ptr = strrchr(path, '/');
    if (name_ptr == NULL)
        return OS_FS_ERROR;

    /* strrchr returns a pointer to the last '/' char, so we advance one char */
    name_ptr = name_ptr + sizeof(char);

    /* end_of_path points to the null terminator at the end of the path */
    end_of_path = strrchr(path,'\0');

    /* pointer subraction to see how many characters there are in the name */
    name_len = ((int) end_of_path - (int)name_ptr) / sizeof(char);

    if( name_len > OS_MAX_FILE_NAME)
        return OS_FS_ERROR;

    return OS_FS_SUCCESS;

}/* end OS_check_path_length */

/* --------------------------------------------------------------------------------------
Name: OS_ShellOutputToFile
    
Purpose: Takes a shell command in and writes the output of that command to the specified file
    
Returns: OS_FS_ERROR if the command was not executed properly 
         OS_SUCCESS if success
 ---------------------------------------------------------------------------------------*/
int32 OS_ShellOutputToFile(char* Cmd, int32 OS_fd)
{   
    /* this is a #define to avoid a 'variable length array' warning */
    /* 15 is for the size of the redirection string that is added 
     * to the command */
    #define OS_REDIRECTSTRSIZE 15
    char LocalCmd [OS_MAX_CMD_LEN + OS_REDIRECTSTRSIZE]; 
    char String [OS_REDIRECTSTRSIZE];
    int32 ReturnCode;
    int32 Result;

    /* Make sure the file descriptor is legit before using it */
    if (OS_fd < 0 || OS_fd >= OS_MAX_NUM_OPEN_FILES || OS_FDTable[OS_fd].IsValid == FALSE)
    {
        ReturnCode = OS_FS_ERR_INVALID_FD;
    }
    else
    {
      strncpy(LocalCmd,Cmd,OS_MAX_CMD_LEN +OS_REDIRECTSTRSIZE);

      /* Make sure that we are able to access this file */
/*      fchmod(OS_FDTable[OS_fd].OSfd,0777); */
  
      /* add in the extra chars necessary to perform the redirection
         1 for stdout and 2 for stderr. they are redirected to the 
         file descriptor passed in
      */
      sprintf(String, " 1>&%d 2>&%d",(int)OS_FDTable[OS_fd].OSfd, (int)OS_FDTable[OS_fd].OSfd);
      strcat(LocalCmd, String);

      Result = system(LocalCmd);
     
      if (Result == 0)
      {
          ReturnCode = OS_SUCCESS;
      }
      else
      {
          ReturnCode = OS_FS_ERROR;
      }
    }
    return ReturnCode;
}/* end OS_ShellOutputToFile */

/* --------------------------------------------------------------------------------------
Name: OS_FDGetInfo
    
Purpose: Copies the information of the given file descriptor into a structure passed in
    
Returns: OS_FS_ERR_INVALID_FD if the file descriptor passed in is invalid
         OS_FS_SUCCESS if the copying was successfull
 ---------------------------------------------------------------------------------------*/
int32 OS_FDGetInfo (int32 filedes, OS_FDTableEntry *fd_prop)
{
    /* Make sure the file descriptor is legit before using it */
    if (filedes < 0 || filedes >= OS_MAX_NUM_OPEN_FILES || OS_FDTable[filedes].IsValid == FALSE)
    {
        /* Make sure user knows this is not a valid descriptor if he forgets to check the
         * return code */
        (*(fd_prop)).IsValid = FALSE;
        return OS_FS_ERR_INVALID_FD;
    }
    else
    { 
        *fd_prop = OS_FDTable[filedes];
        return OS_FS_SUCCESS;
    }
}/* end OS_FDGetInfo */
