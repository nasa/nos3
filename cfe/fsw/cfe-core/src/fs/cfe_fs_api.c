/*
** $Id: cfe_fs_api.c 1.8 2014/08/22 17:06:20GMT-05:00 lwalling Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:  cFE File Services (FS) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
** $Log: cfe_fs_api.c  $
** Revision 1.8 2014/08/22 17:06:20GMT-05:00 lwalling 
** Change signed loop counters to unsigned
** Revision 1.7 2012/01/13 12:11:28EST acudmore 
** Changed license text to reflect open source
** Revision 1.6 2010/11/03 15:09:41EDT jmdagost 
** Added cfe.h include file.
** Revision 1.5 2010/10/25 17:51:05EDT jmdagost 
** Added filename length test to CFE_FS_ExtractFilenameFromPath()
** Revision 1.4 2009/06/10 09:13:43EDT acudmore 
** Converted OS_Mem* and OS_BSP* calls to CFE_PSP_*
** Revision 1.3 2008/08/28 08:39:58EDT apcudmore 
** fixed CFS names. ( CFS-->CFE )
** Revision 1.2 2008/06/20 15:40:30EDT apcudmore 
** Added support for OSAL based module loader
**  - Removed calls and references to the BSP based module loader
** Revision 1.1 2008/04/17 08:05:16EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/fs/project.pj
** Revision 1.14 2007/09/05 09:45:01EDT David Kobe (dlkobe) 
** Corrected OS_lseek return status data type
** Revision 1.13 2007/09/05 09:43:12EDT David Kobe (dlkobe) 
** Corrected usage of return codes from OS_lseek and OS_write
** Revision 1.12 2007/05/17 13:05:09EDT wfmoleski 
** 
** Revision 1.11 2007/03/16 15:12:12EST dlkobe 
** Swapped parameter list for CFE_FS_ReadHeader API.
** Revision 1.10 2006/09/01 11:39:24GMT-05:00 dlkobe 
** Added processor endianess check and swapped header data if necessary
** Revision 1.9 2006/07/25 14:37:52GMT-05:00 njyanchik 
** It turns out the both the FS app and the OSAL were incorrect where file descriptors are
** concerned. the file descriptors should be int32 across the board.
**
*/


/*
** Required header files...
*/
#include "private/cfe_private.h"
#include "cfe_fs_priv.h"
#include "cfe_fs.h"
#include "cfe_time.h"
#include "osapi.h"
#include "cfe_psp.h"
#include "cfe_es.h"
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_ReadHeader() -- read cFE file header structure                   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes)
{
    int32   Result;
    int32   EndianCheck = 0x01020304;
    
    /*
    ** Ensure that we are at the start of the file...
    */
    Result = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (Result == OS_SUCCESS)
    {
        /*
        ** Read header structure into callers buffer...
        */
        Result = OS_read(FileDes, Hdr, sizeof(CFE_FS_Header_t));

	    /* Determine if this processor is a little endian processor */
    	if ((*(char *)(&EndianCheck)) == 0x04)
    	{
    	    /* If this is a little endian processor, then convert the header data structure from */
    	    /* its standard big-endian format into a little endian format to ease user access    */
    	    CFE_FS_ByteSwapCFEHeader(Hdr);
    	}
    }
    
    return(Result);

} /* End of CFE_FS_ReadHeader() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_InitHeader() -- intialize cFE file header structure              */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
   CFE_PSP_MemSet(Hdr, 0, sizeof(CFE_FS_Header_t));
   strncpy((char *)Hdr->Description, Description, sizeof(Hdr->Description) - 1);
   Hdr->SubType = SubType;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_WriteHeader() -- write cFE file header structure                 */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr)
{
    CFE_TIME_SysTime_t Time;
    int32   Result;
    int32   EndianCheck = 0x01020304;

    /*
    ** Ensure that we are at the start of the file...
    */
    Result = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (Result == OS_SUCCESS)
    {
        /*
        ** Fill in the ID fields...
        */
          Hdr->SpacecraftID  = CFE_PSP_GetSpacecraftId();
          Hdr->ProcessorID   = CFE_PSP_GetProcessorId();
          CFE_ES_GetAppID((uint32 *)&(Hdr->ApplicationID));

          /* Fill in length field */

          Hdr->Length = sizeof(CFE_FS_Header_t);

          /* put the header, 'cfe1' in hex, in to the content type */
          Hdr->ContentType = 0x63464531;

          
        /*
        ** Fill in the timestamp fields...
        */
          Time = CFE_TIME_GetTime();
          Hdr->TimeSeconds = Time.Seconds;
          Hdr->TimeSubSeconds = Time.Subseconds;

        /*
        ** Determine if this is a little endian processor
        */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure from */
            /* the native little endian format to the required CFE standard big endian format    */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }

        /*
        ** Write header structure from callers buffer...
        */
        Result = OS_write(FileDes, Hdr, sizeof(CFE_FS_Header_t));

        /*
        ** Determine if this is a little endian processor
        */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure back */
            /* from the required CFE standard big endian format to the little endian format      */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }

    }

    return(Result);

} /* End of CFE_FS_WriteHeader() */


int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    int32              Result = OS_FS_SUCCESS;
    CFE_FS_Header_t    TempHdr;
    int32              EndianCheck = 0x01020304;
    CFE_TIME_SysTime_t OutTimestamp = NewTimestamp;
    int32              FileOffset = 0;
    
    FileOffset = ((char *)&TempHdr.TimeSeconds - (char *)&TempHdr.ContentType);
    Result = OS_lseek(FileDes, FileOffset, OS_SEEK_SET);
    
    if (Result == FileOffset)
    {
        /*
        **  Determine if this is a little endian processor
        */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this processor is a little endian processor, then convert the timestamp to a big */
            /* endian format so that it is compatible with the standard cFE File Header format     */
            CFE_FS_ByteSwapUint32(&OutTimestamp.Seconds);
            CFE_FS_ByteSwapUint32(&OutTimestamp.Subseconds);
        }
        
        Result = OS_write(FileDes, &OutTimestamp.Seconds, sizeof(OutTimestamp.Seconds));
        
        /* On a good write, the value returned will equal the number of bytes written */
        if (Result == sizeof(OutTimestamp.Seconds))
        {
            Result = OS_write(FileDes, &OutTimestamp.Subseconds, sizeof(OutTimestamp.Subseconds));
            
            if (Result == sizeof(OutTimestamp.Subseconds))
            {
                Result = OS_FS_SUCCESS;
            }
            else
            {
                CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to write Seconds (Status=0x%08X)\n", (unsigned int)Result);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to write Seconds (Status=0x%08X)\n", (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to lseek time fields (Status=0x%08X)\n", (unsigned int)Result);
    }
    
    return(Result);
} /* End of CFE_FS_SetTimestamp() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_ByteSwapCFEHeader() -- byte swap cFE file header structure       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_FS_ByteSwapCFEHeader(CFE_FS_Header_t *Hdr)
{
    CFE_FS_ByteSwapUint32(&Hdr->ContentType);
    CFE_FS_ByteSwapUint32(&Hdr->SubType);
    CFE_FS_ByteSwapUint32(&Hdr->Length);
    CFE_FS_ByteSwapUint32(&Hdr->SpacecraftID);
    CFE_FS_ByteSwapUint32(&Hdr->ProcessorID);
    CFE_FS_ByteSwapUint32(&Hdr->ApplicationID);
    CFE_FS_ByteSwapUint32(&Hdr->TimeSeconds);
    CFE_FS_ByteSwapUint32(&Hdr->TimeSubSeconds);

} /* End of CFE_FS_ByteSwapCFEHeader() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_ByteSwapUint32() -- byte swap an uint32                          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_FS_ByteSwapUint32(uint32 *Uint32ToSwapPtr)
{
    int32 Temp = *Uint32ToSwapPtr;
    char *InPtr = (char *)&Temp;
    char *OutPtr = (char *)Uint32ToSwapPtr;
    
    OutPtr[0] = InPtr[3];
    OutPtr[1] = InPtr[2];
    OutPtr[2] = InPtr[1];
    OutPtr[3] = InPtr[0];    
} /* End of CFE_FS_ByteSwapUint32() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* Function: CFE_FS_ExtractFilenameFromPath                                */
/*                                                                         */
/*  This function extracts the filename from a given path/filename         */
/*   combination.                                                          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
   uint32 i,j;
   int    StringLength;
   int    DirMarkIdx;
   int32   ReturnCode;
   
   if ( OriginalPath == NULL || FileNameOnly == NULL )
   {
      ReturnCode = CFE_FS_BAD_ARGUMENT;
   }
   else
   {
      
       /*
       ** Get the string length of the original file path
       */
       StringLength = strlen(OriginalPath);
   
       /*
       ** Extract the filename from the Path
       */
    
       /* 
       ** Find the last '/' Character 
       */
       DirMarkIdx = -1;
       for ( i = 0; i < StringLength; i++ )
       {
          if ( OriginalPath[i] == '/' )
          {
             DirMarkIdx = i;
          }
       }
    
       /*
       ** Verify the filename isn't too long
       */
       if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
       {       
          /* 
          ** Extract the filename portion 
          */
          if ( DirMarkIdx > 0 )
          {    
             /* 
             ** Extract the filename portion 
             */
             j = 0;
             for ( i = DirMarkIdx + 1; i < StringLength; i++ )
             {
                FileNameOnly[j] = OriginalPath[i];
                j++;
             }
             FileNameOnly[j] = '\0';
    
             ReturnCode = CFE_SUCCESS;       
          }
          else
          { 
             ReturnCode = CFE_FS_INVALID_PATH;
          }
       }
       else 
       {
           ReturnCode = CFE_FS_FNAME_TOO_LONG;
       }
    }
   
    return(ReturnCode);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* Function: CFE_FS_BSPIsGzFile                                            */
/*                                                                         */
/*  This function will check to see if the file to load                    */
/*  is compressed with gzip. Currently it is based on the                  */
/*  file name. The file name must end in ".gz".                            */ 
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CFE_FS_IsGzFile(const char *FileName)
{
   size_t    StringLength;
   
   if ( FileName == NULL )
   {
      return(FALSE);
   }
   /*
   ** Get the string length of the SourceFile name
   */
   StringLength = strlen(FileName);
   
   /*
   ** The compressed filename must be named "something.gz"
   ** Because of that, the shortest compressed filename is "a.gz"
   ** Check for the length before determining if the file is zipped
   */
   if ( StringLength < 4 )
   {
      return(FALSE);
   }

   /*
   ** if the last 3 characters are ".gz"
   ** Then assume the file is zipped.
   */
   if ( (FileName[StringLength - 3] == '.') &&
        (FileName[StringLength - 2] == 'g') &&
        (FileName[StringLength - 1] == 'z'))
   {
       return(TRUE);       
   }
   else
   {
       return(FALSE);
   }
  
}


/************************/
/*  End of File Comment */
/************************/
