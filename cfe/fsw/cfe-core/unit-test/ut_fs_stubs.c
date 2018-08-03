/*
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** File:
** $Id: ut_fs_stubs.c 1.6 2014/05/28 09:21:48GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for File Service routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.6 $
** $Log: ut_fs_stubs.c  $
** Revision 1.6 2014/05/28 09:21:48GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.5 2012/01/13 13:59:30EST acudmore 
** Added license text
** Revision 1.4 2010/10/25 18:21:50EDT jmdagost 
** Updated CFE_FS_ExtractFilenameFromPath().
** Revision 1.3 2008/08/28 08:52:19EDT apcudmore 
** changed CFS_ call back to CFE_
** Revision 1.2 2008/08/15 11:26:53EDT njyanchik 
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:45BST ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.10 2007/07/05 15:04:28EDT David Kobe (dlkobe) 
** Baseline following complete TBL unit testing
** Revision 1.9 2007/05/10 15:14:25EDT njyanchik 
** Another update of Jonathans UT
** Revision 1.8 2007/05/01 13:28:15EDT njyanchik 
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.7 2007/03/16 15:12:12EST dlkobe 
** Swapped parameter list for CFE_FS_ReadHeader API.
** Revision 1.6 2007/03/02 15:18:05EST njyanchik 
** Jonathan provided me with a newer tbl unit test to check in
** Revision 1.4 2006/11/08 15:09:27GMT-05:00 rjmcgraw 
** Changed return in WriteHeader API to sizeof(CFE_FS_Header_t)
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_fs.h"
#include "ut_stubs.h"

/*
** External global variables
*/
extern char  UT_ReadHdr[];
extern int32 UT_ReadHdrOffset;
extern char  cMsg[];

extern UT_SetRtn_t FSWriteHdrRtn;
extern UT_SetRtn_t FSReadHdrRtn;
extern UT_SetRtn_t FSSetTimestampRtn;
extern UT_SetRtn_t FSIsGzFileRtn;
extern UT_SetRtn_t FSDecompressRtn;
extern UT_SetRtn_t FSExtractRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_FS_InitHeader stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_InitHeader.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        None
**
******************************************************************************/
void CFE_FS_InitHeader(CFE_FS_Header_t *hdr, const char *Description, uint32 SubType)
{
   memset(hdr,0,sizeof(CFE_FS_Header_t));
}

/*****************************************************************************/
/**
** \brief CFE_FS_WriteHeader stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_WriteHeader.  The user can adjust the response by setting
**        the values in the FSWriteHdrRtn structure prior to this function
**        being called.  If the value FSWriteHdrRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSWriteHdrRtn.value.
**        The size of the CFE_FS_Header_t structure, in bytes, is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or the size of the
**        CFE_FS_Header_t structure in bytes.
**
******************************************************************************/
int32 CFE_FS_WriteHeader(int32 filedes, CFE_FS_Header_t *hdr)
{
    int32   status = sizeof(CFE_FS_Header_t);
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (FSWriteHdrRtn.count > 0)
    {
        FSWriteHdrRtn.count--;

        if(FSWriteHdrRtn.count == 0)
        {
            status = FSWriteHdrRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_FS_WriteHeader called: %ld; %s",
                 filedes, hdr->Description);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_ReadHeader stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_ReadHeader.  The user can adjust the response by setting
**        the values in the FSReadHdrRtn structure prior to this function
**        being called.  If the value FSReadHdrRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSReadHdrRtn.value.
**        The size of the CFE_FS_Header_t structure, in bytes, is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or the size of the
**        CFE_FS_Header_t structure in bytes.
**
******************************************************************************/
int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes)
{
    int32   status = sizeof(CFE_FS_Header_t);
    boolean flag = FALSE;

    if (FSReadHdrRtn.count > 0)
    {
        FSReadHdrRtn.count--;

        if (FSReadHdrRtn.count == 0)
        {
            status = FSReadHdrRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        memcpy(Hdr, &UT_ReadHdr[UT_ReadHdrOffset], sizeof(CFE_FS_Header_t));
        UT_ReadHdrOffset += sizeof(CFE_FS_Header_t);
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_FS_ReadHeader called: %ld; %s",
                 FileDes, Hdr->Description);
        UT_Text(cMsg);
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_SetTimestamp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_SetTimestamp.  The user can adjust the response by setting
**        the values in the FSSetTimestampRtn structure prior to this function
**        being called.  If the value FSSetTimestampRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        FSSetTimestampRtn.value.  OS_FS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_FS_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    int32   status = OS_FS_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (FSSetTimestampRtn.count > 0)
    {
        FSSetTimestampRtn.count--;

        if (FSSetTimestampRtn.count == 0)
        {
            status = FSSetTimestampRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        UT_Text("  CFE_FS_SetTimeStamp called");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_EarlyInit(void)
{
    return CFE_SUCCESS;
}


/*****************************************************************************/
/**
** \brief CFE_FS_ExtractFilenameFromPath stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_ExtractFilenameFromPath.  The user can adjust the response by
**        setting the values in the FSExtractRtn structure prior to this
**        function being called.  If the value FSExtractRtn.count is greater
**        than zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSExtractRtn.value.
**        Otherwise returns CFE_SUCCESS or an error flag dependent on the
**        function's input values.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, CFE_FS_BAD_ARGUMENT,
**        CFE_FS_INVALID_PATH, CFE_FS_FNAME_TOO_LONG, or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
    int   i,j;
    int   StringLength;
    int   DirMarkIdx;
    int32 status = CFE_SUCCESS;
    
    if (FSExtractRtn.count > 0)
    {
        FSExtractRtn.count--;

        if (FSExtractRtn.count == 0)
        {
            status = FSExtractRtn.value;
        }
    }
    else
    {
        if (OriginalPath == NULL || FileNameOnly == NULL)
        {
            status = CFE_FS_BAD_ARGUMENT;
        }
        else
        {
            /* Get the string length of the original file path */
            StringLength = strlen(OriginalPath);
   
            /* Extract the filename from the Path:
                 Find the last '/' Character */
            DirMarkIdx = -1;

            for (i = 0; i < StringLength; i++)
            {
                if (OriginalPath[i] == '/')
                {
                    DirMarkIdx = i;
                }
            }

            /* Verify the filename isn't too long */
            if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
            {
                /* Extract the filename portion */
                if (DirMarkIdx > 0)
                {
                    /* Extract the filename portion */
                    j = 0;

                    for (i = DirMarkIdx + 1; i < StringLength; i++)
                    {
                        FileNameOnly[j] = OriginalPath[i];
                        j++;
                    }

                    FileNameOnly[j] = '\0';
                }
                else
                {
                    status = CFE_FS_INVALID_PATH;
                }
            }
            else
            {
               status = CFE_FS_FNAME_TOO_LONG;
            }
        }
    }
   
    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_Decompress stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_Decompress.  The user can adjust the response by setting
**        the values in the FSDecompressRtn structure prior to this function
**        being called.  If the value FSDecompressRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSDecompressRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_Decompress(const char * SourceFile, const char * DestinationFile)
{    

    int32 status = CFE_SUCCESS;

    if (FSDecompressRtn.count > 0)
    {
        FSDecompressRtn.count--;

        if (FSDecompressRtn.count == 0)
        {
            status = FSDecompressRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_IsGzFile stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_IsGzFile.  The user can adjust the response by setting
**        the values in the FSIsGzFileRtn structure prior to this function
**        being called.  If the value FSIsGzFileRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSIsGzFileRtn.value.
**        FALSE is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or FALSE.
**
******************************************************************************/
boolean CFE_FS_IsGzFile(const char *FileName)
{
    int32 status = FALSE;

    if (FSIsGzFileRtn.count > 0)
    {
        FSIsGzFileRtn.count--;

        if (FSIsGzFileRtn.count == 0)
        {
            status = FSIsGzFileRtn.value;
        }
    }
    
    return status;
}
