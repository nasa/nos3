/*
** $Id: cfe_tbl_internal.c 1.15 2014/08/22 16:30:24GMT-05:00 lwalling Exp  $
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
** Purpose:  cFE Table Services (TBL) utility function source file
**
** Author:   D. Kobe/the Hammers Company, Inc.
**
** Notes:
**
** $Log: cfe_tbl_internal.c  $
** Revision 1.15 2014/08/22 16:30:24GMT-05:00 lwalling 
** Change signed loop counters to unsigned
** Revision 1.14 2012/02/22 15:13:51EST lwalling 
** Remove obsolete TODO comments
** Revision 1.13 2012/01/13 12:17:39EST acudmore 
** Changed license text to reflect open source
** Revision 1.12 2012/01/04 17:27:07EST sstrege 
** Added LockFlag check when checking for inactive buffer use
** Revision 1.11 2011/12/28 14:02:55EST lwalling 
** Add validation tests for spacecraft ID and processor ID to CFE_TBL_ReadHeaders()
** Revision 1.10 2011/09/02 14:58:26EDT jmdagost 
** Added new-line characters where missing in syslog calls.
** Revision 1.9 2010/10/27 17:53:29EDT dkobe 
** Added TableLoadedOnce flag to Critical Table Registry
** Revision 1.8 2010/10/27 16:36:20EDT dkobe 
** Added Table CRC computation and maintenance to internal table functions
** Revision 1.7 2010/10/27 13:56:32EDT dkobe 
** Added TBL notification registry data and message sending function
** Revision 1.6 2010/10/25 15:00:32EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.5 2010/10/04 15:18:52EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.4 2009/06/10 09:20:06EDT acudmore 
** Changed OS_Mem* and OS_BSP* calls to CFE_PSP_* calls
** Revision 1.3 2008/07/29 15:55:15EDT dkobe 
** Moved CFE_TBL_DumpToBuffer from cfe_tbl_internal.c to cfe_tbl_api.c
** Revision 1.2 2008/07/29 14:05:35EDT dkobe 
** Removed redundant FileCreateTimeSecs and FileCreateTimeSubSecs fields
** Revision 1.1 2008/04/17 08:05:34EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/tbl/project.pj
** Revision 1.51 2007/08/07 12:52:42EDT David Kobe (dlkobe) 
** Modified CFE_ES_GetPoolBuf API's first parameter to be of type uint32**
** Revision 1.50 2007/07/13 11:27:15EDT dlkobe 
** Moved prototype for CFE_TBL_UpdateCriticalTblCDS to cfe_tbl_internal.h
** Revision 1.49 2007/07/03 10:19:17EDT dlkobe 
** Removed unused CFE_TBL_AddAccessLink function
** Revision 1.48 2007/06/07 09:34:52EDT dlkobe 
** Corrected Critical Table CDS Name and CDS Registry Init
** Revision 1.47 2007/04/30 11:01:16EDT rjmcgraw 
** Changed EarlyInit to return error when detected
** Revision 1.46 2007/04/28 16:14:44EDT dlkobe 
** Changed RegisterCDS call to use internal RegisterCDSEx call
**
**
*/


/*
** Required header files...
*/
#include "cfe_msgids.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_events.h"
#include "cfe_error.h"
#include "cfe_es.h"
#include "cfe_evs.h"
#include "cfe_fs.h"
#include "cfe_psp.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************
**
** CFE_TBL_EarlyInit
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/
/******************************************************************************
**  Function:  CFE_TBL_EarlyInit()
**
**  Purpose:
**    Initialize the Table Services
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any TBL API's are called.
**
**  Return:
**    none
*/
int32 CFE_TBL_EarlyInit (void)
{
    uint16 i;
    uint32 j;
    int32 Status;


    /* Initialize the Table Registry */
    for(i=0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_InitRegistryRecord(&CFE_TBL_TaskData.Registry[i]);
    }

    /* Initialize the Table Access Descriptors */
    for (i=0; i<CFE_TBL_MAX_NUM_HANDLES; i++)
    {
        CFE_TBL_TaskData.Handles[i].AppId = CFE_ES_ERR_APPID;
        CFE_TBL_TaskData.Handles[i].RegIndex = 0;
        CFE_TBL_TaskData.Handles[i].PrevLink = CFE_TBL_END_OF_LIST;
        CFE_TBL_TaskData.Handles[i].NextLink = CFE_TBL_END_OF_LIST;
        CFE_TBL_TaskData.Handles[i].UsedFlag = FALSE;
        CFE_TBL_TaskData.Handles[i].LockFlag = FALSE;
        CFE_TBL_TaskData.Handles[i].Updated = FALSE;
        CFE_TBL_TaskData.Handles[i].BufferIndex = 0;
    }

    /* Initialize the Table Validation Results Records */
    for (i=0; i<CFE_TBL_MAX_NUM_VALIDATIONS; i++)
    {
        CFE_TBL_TaskData.ValidationResults[i].State = CFE_TBL_VALIDATION_FREE;
        CFE_TBL_TaskData.ValidationResults[i].CrcOfTable = 0;
        CFE_TBL_TaskData.ValidationResults[i].Result = 0;
        CFE_TBL_TaskData.ValidationResults[i].ActiveBuffer = FALSE;
        CFE_TBL_TaskData.ValidationResults[i].TableName[0] = '\0';
    }

    /* Initialize the Dump-Only Table Dump Control Blocks */
    for (i=0; i<CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_TaskData.DumpControlBlocks[i].State = CFE_TBL_DUMP_FREE;
        CFE_TBL_TaskData.DumpControlBlocks[i].DumpBufferPtr = NULL;
        CFE_TBL_TaskData.DumpControlBlocks[i].Size = 0;
        CFE_TBL_TaskData.DumpControlBlocks[i].TableName[0] = '\0';
        
        /* Prevent Shared Buffers from being used until successfully allocated */
        CFE_TBL_TaskData.LoadBuffs[i].Taken = TRUE;
    }

    CFE_TBL_TaskData.ValidationCtr = 0;

    CFE_TBL_TaskData.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND;
    CFE_TBL_TaskData.LastTblUpdated = CFE_TBL_NOT_FOUND;
    
    /*
    ** Create table registry access mutex
    */
    Status = OS_MutSemCreate(&CFE_TBL_TaskData.RegistryMutex,
                              CFE_TBL_MUT_REG_NAME,
                              CFE_TBL_MUT_REG_VALUE);
    if(Status != OS_SUCCESS)
    {
      CFE_ES_WriteToSysLog("CFE_TBL:Registry mutex creation failed! RC=0x%08x\n",(unsigned int)Status);
      return Status;
    }/* end if */                              

    /*
    ** Create working buffer access mutex
    */
    Status = OS_MutSemCreate(&CFE_TBL_TaskData.WorkBufMutex,
                              CFE_TBL_MUT_WORK_NAME,
                              CFE_TBL_MUT_WORK_VALUE);
    if(Status != OS_SUCCESS)
    {
      CFE_ES_WriteToSysLog("CFE_TBL:Working buffer mutex creation failed! RC=0x%08x\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_SB_InitMsg(&CFE_TBL_TaskData.HkPacket,
                    CFE_TBL_HK_TLM_MID,
                    sizeof(CFE_TBL_HkPacket_t), TRUE);

    /*
    ** Initialize table registry report packet (clear user data area)...
    */
    CFE_SB_InitMsg(&CFE_TBL_TaskData.TblRegPacket,
                    CFE_TBL_REG_TLM_MID,
                    sizeof(CFE_TBL_TblRegPacket_t), TRUE);

    /* Initialize memory partition and allocate shared table buffers. */
    Status = CFE_ES_PoolCreate(&CFE_TBL_TaskData.Buf.PoolHdl,
                                CFE_TBL_TaskData.Buf.Partition,                               
                                CFE_TBL_BUF_MEMORY_BYTES);        

    if(Status < 0)
    {
        CFE_ES_WriteToSysLog("CFE_TBL:InitBuffers PoolCreate fail Status=0x%X\n", (unsigned int)Status);
        return Status;
    }
    else
    {
        /* Initialize each of the shared load buffers */
        j=0;
        do
        {
            /* Allocate memory for shared load buffers */
            Status = CFE_ES_GetPoolBuf((uint32 **)&CFE_TBL_TaskData.LoadBuffs[j].BufferPtr,
                                       CFE_TBL_TaskData.Buf.PoolHdl,
                                       CFE_TBL_MAX_SNGL_TABLE_SIZE);

            if (Status < CFE_TBL_MAX_SNGL_TABLE_SIZE)
            {
                CFE_ES_WriteToSysLog("CFE_TBL:InitBuffers GetPoolBuf Fail Index=%d, Status=0x%X\n", (int)j, (unsigned int)Status);
                return Status;
            }
            else
            {
                /* The buffer is successfully created, so allow it to be used */
                CFE_TBL_TaskData.LoadBuffs[j].Taken = FALSE;
                
                CFE_PSP_MemSet(CFE_TBL_TaskData.LoadBuffs[j].DataSource, 0, OS_MAX_PATH_LEN);
                CFE_TBL_TaskData.LoadBuffs[j].FileCreateTimeSecs = 0;
                CFE_TBL_TaskData.LoadBuffs[j].FileCreateTimeSubSecs = 0;
            }

            j++;
        } while ((j < CFE_TBL_MAX_SIMULTANEOUS_LOADS) && 
                 (Status >= CFE_TBL_MAX_SNGL_TABLE_SIZE));
    }
    
    /* Try to obtain a previous image of the Critical Table Registry from the Critical Data Store */
    Status = CFE_ES_RegisterCDSEx(&CFE_TBL_TaskData.CritRegHandle, 
                                  (sizeof(CFE_TBL_CritRegRec_t)*CFE_TBL_MAX_CRITICAL_TABLES),
                                  "CFE_TBL.CritReg", TRUE);
                                
    /* Assume for the moment that nothing is already in the CDS and zero out the Critical Table Registry */
    for (i=0; i<CFE_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_TaskData.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
        CFE_TBL_TaskData.CritReg[i].FileCreateTimeSecs = 0;
        CFE_TBL_TaskData.CritReg[i].FileCreateTimeSubSecs = 0;
        CFE_TBL_TaskData.CritReg[i].LastFileLoaded[0] = '\0';
        CFE_TBL_TaskData.CritReg[i].TimeOfLastUpdate.Seconds = 0;
        CFE_TBL_TaskData.CritReg[i].TimeOfLastUpdate.Subseconds = 0;
        CFE_TBL_TaskData.CritReg[i].TableLoadedOnce = FALSE;

    }
    
    if (Status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* Try to recover the Critical Table Registry from the CDS */
        Status = CFE_ES_RestoreFromCDS(CFE_TBL_TaskData.CritReg, CFE_TBL_TaskData.CritRegHandle);
        
        if (Status != CFE_SUCCESS)
        {
            /* Note if we were unable to recover error free Critical Table Registry from the CDS */
            CFE_ES_WriteToSysLog("CFE_TBL:EarlyInit-Failed to recover Critical Table Registry (Err=0x%08X)\n", (unsigned int)Status);
        }
        
        /* Whether we recovered the Critical Table Registry or not, we are successful with initialization */
        Status = CFE_SUCCESS;
    }
    else if (Status != CFE_SUCCESS)
    {
        /* Not being able to support Critical Tables is not the end of the world */
        /* Note the problem and move on */
        CFE_ES_WriteToSysLog("CFE_TBL:EarlyInit-Failed to create Critical Table Registry (Err=0x%08X)\n", (unsigned int)Status);
        
        /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
        Status = CFE_SUCCESS;
    }
    else
    {
        /* Save the initial version of the Critical Table Registry in the CDS */
        Status = CFE_ES_CopyToCDS(CFE_TBL_TaskData.CritRegHandle, CFE_TBL_TaskData.CritReg);
        
        if (Status != CFE_SUCCESS)
        {
            /* Not being able to support Critical Tables is not the end of the world */
            /* Note the problem and move on */
            CFE_ES_WriteToSysLog("CFE_TBL:EarlyInit-Failed to save Critical Table Registry (Err=0x%08X)\n", (unsigned int)Status);
        
            /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
            Status = CFE_SUCCESS;
        }
    }
    
    return Status;
            
}/* end CFE_TBL_EarlyInit */


/*******************************************************************
**
** CFE_TBL_InitRegistryRecord
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/


void CFE_TBL_InitRegistryRecord (CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->OwnerAppId = CFE_TBL_NOT_OWNED;
    RegRecPtr->Size = 0;
    RegRecPtr->NotificationMsgId = 0;
    RegRecPtr->NotificationCC = 0;
    RegRecPtr->NotificationParam = 0;
    RegRecPtr->Buffers[0].BufferPtr = NULL;
    RegRecPtr->Buffers[0].FileCreateTimeSecs = 0;
    RegRecPtr->Buffers[0].FileCreateTimeSubSecs = 0;
    RegRecPtr->Buffers[0].Crc = 0;
    RegRecPtr->Buffers[0].Taken = FALSE;
    RegRecPtr->Buffers[0].DataSource[0] = '\0';
    RegRecPtr->Buffers[1].BufferPtr = NULL;
    RegRecPtr->Buffers[1].FileCreateTimeSecs = 0;
    RegRecPtr->Buffers[1].FileCreateTimeSubSecs = 0;
    RegRecPtr->Buffers[1].Crc = 0;
    RegRecPtr->Buffers[1].Taken = FALSE;
    RegRecPtr->Buffers[1].DataSource[0] = '\0';
    RegRecPtr->ValidationFuncPtr = NULL;
    RegRecPtr->TimeOfLastUpdate.Seconds = 0;
    RegRecPtr->TimeOfLastUpdate.Subseconds = 0;
    RegRecPtr->HeadOfAccessList = CFE_TBL_END_OF_LIST;
    RegRecPtr->LoadInProgress =  CFE_TBL_NO_LOAD_IN_PROGRESS;
    RegRecPtr->ValidateActiveIndex = CFE_TBL_NO_VALIDATION_PENDING;
    RegRecPtr->ValidateInactiveIndex = CFE_TBL_NO_VALIDATION_PENDING;
    RegRecPtr->CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    RegRecPtr->CriticalTable = FALSE;
    RegRecPtr->TableLoadedOnce = FALSE;
    RegRecPtr->LoadPending = FALSE;
    RegRecPtr->DumpOnly = FALSE;
    RegRecPtr->DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;
    RegRecPtr->UserDefAddr = FALSE;
    RegRecPtr->DblBuffered = FALSE;
    RegRecPtr->NotifyByMsg = FALSE;
    RegRecPtr->ActiveBufferIndex = 0;
    RegRecPtr->Name[0] = '\0';
    RegRecPtr->LastFileLoaded[0] = '\0';
}


/*******************************************************************
**
** CFE_TBL_ValidateHandle
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/


int32 CFE_TBL_ValidateHandle(CFE_TBL_Handle_t TblHandle)
{
    int32 Status = CFE_SUCCESS;

    /* Is the handle out of range? */
    if (TblHandle >= CFE_TBL_MAX_NUM_HANDLES)
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;

        CFE_ES_WriteToSysLog("CFE_TBL:ValidateHandle-Table Handle=%d is > %d\n", TblHandle, CFE_TBL_MAX_NUM_HANDLES);
    }
    else
    {
        /* Check to see if the Handle is no longer valid for this Table */
        if (CFE_TBL_TaskData.Handles[TblHandle].UsedFlag == FALSE)
        {
            Status = CFE_TBL_ERR_INVALID_HANDLE;

            CFE_ES_WriteToSysLog("CFE_TBL:ValidateHandle-Table Handle=%d is for unused Table Handle\n", TblHandle);
        }
    }
    return Status;
}   /* End of CFE_TBL_ValidateHandle() */

/*******************************************************************
**
** CFE_TBL_ValidateAppID
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_ValidateAppID(uint32 *AppIdPtr)
{
    int32 Status = CFE_ES_GetAppID(AppIdPtr);

    if (Status == CFE_SUCCESS)
    {
        if (*AppIdPtr >= CFE_ES_MAX_APPLICATIONS)
        {
            Status = CFE_TBL_ERR_BAD_APP_ID;

            CFE_ES_WriteToSysLog("CFE_TBL:ValidateAppID-AppId=%d > Max Apps (%d)\n",
                                 (int)(*AppIdPtr), CFE_ES_MAX_APPLICATIONS);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:ValidateAppID-GetAppID failed (Stat=0x%08X)\n", (unsigned int)Status);
    }

    return Status;
}   /* End of CFE_TBL_ValidateAppID() */

/*******************************************************************
**
** CFE_TBL_ValidateAccess
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_ValidateAccess(CFE_TBL_Handle_t TblHandle, uint32 *AppIdPtr)
{
    int32 Status = CFE_SUCCESS;

    /* Check to make sure App ID is legit */
    Status = CFE_TBL_ValidateAppID(AppIdPtr);

    if (Status == CFE_SUCCESS)
    {
        /* Check table handle validity */
        Status = CFE_TBL_ValidateHandle(TblHandle);

        if (Status == CFE_SUCCESS)
        {
            Status = CFE_TBL_CheckAccessRights(TblHandle, *AppIdPtr);

            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("CFE_TBL:ValidateAccess-App(%d) no access to Tbl Handle=%d (Stat=0x%08X)\n",
                                     (int)(*AppIdPtr), (int)TblHandle, (unsigned int)Status);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_TBL:ValidateAccess-Invalid Tbl Handle=%d\n", (int)TblHandle);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:ValidateAccess-Bad AppId=%d\n", (int)(*AppIdPtr));
    }

    return Status;
}   /* End of CFE_TBL_ValidateAccess() */


/*******************************************************************
**
** CFE_TBL_CheckAccessRights
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_CheckAccessRights(CFE_TBL_Handle_t TblHandle, uint32 ThisAppId)
{
    int32 Status = CFE_SUCCESS;

    if (ThisAppId != CFE_TBL_TaskData.Handles[TblHandle].AppId)
    {
        /* The Table Service Task always has access rights so that tables */
        /* can be manipulated via ground command                          */
        if (ThisAppId != CFE_TBL_TaskData.TableTaskAppId)
        {
            Status = CFE_TBL_ERR_NO_ACCESS;
        }
    }

    return Status;
}

/*******************************************************************
**
** CFE_TBL_RemoveAccessLink
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_RemoveAccessLink(CFE_TBL_Handle_t TblHandle)
{
    int32 Status = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
    CFE_TBL_RegistryRec_t      *RegRecPtr     = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

    /* Lock Access to the table while we modify the linked list */
    CFE_TBL_LockRegistry();

    /* If we are removing the head of the linked list, then point */
    /* the head pointer to the link after this one                */
    if (AccessDescPtr->PrevLink == CFE_TBL_END_OF_LIST)
    {
        RegRecPtr->HeadOfAccessList = AccessDescPtr->NextLink;

        /* Update the next link, if there is one, to be the new head of the list */
        if (AccessDescPtr->NextLink != CFE_TBL_END_OF_LIST)
        {
            CFE_TBL_TaskData.Handles[AccessDescPtr->NextLink].PrevLink = CFE_TBL_END_OF_LIST;
        }
    }
    else /* Access Descriptor is not the head of the list */
    {
        /* Set the next link on the previous link to the next link of the link being removed */
        CFE_TBL_TaskData.Handles[AccessDescPtr->PrevLink].NextLink = AccessDescPtr->NextLink;

        /* If this link is not the end of the list, then complete two way linkage */
        /* by setting the next link's previous link to the previous link of the link being removed */
        if (AccessDescPtr->NextLink != CFE_TBL_END_OF_LIST)
        {
            CFE_TBL_TaskData.Handles[AccessDescPtr->NextLink].PrevLink = AccessDescPtr->PrevLink;
        }
    }

    /* Return the Access Descriptor to the pool */
    AccessDescPtr->UsedFlag = FALSE;

    /* If this was the last Access Descriptor for this table, we can free the memory buffers as well */
    if (RegRecPtr->HeadOfAccessList == CFE_TBL_END_OF_LIST)
    {
        /* Only free memory that we have allocated.  If the image is User Defined, then don't bother */
        if (RegRecPtr->UserDefAddr == FALSE)
        {
            /* Free memory allocated to buffers */
            Status = CFE_ES_PutPoolBuf(CFE_TBL_TaskData.Buf.PoolHdl, (uint32 *)RegRecPtr->Buffers[0].BufferPtr);
            RegRecPtr->Buffers[0].BufferPtr = NULL;

            if (Status < 0)
            {
                CFE_ES_WriteToSysLog("CFE_TBL:RemoveAccessLink-PutPoolBuf[0] Fail Stat=0x%08X, Hndl=0x%08lX, Buf=0x%08lX\n",
                        (unsigned int)Status, (unsigned long)CFE_TBL_TaskData.Buf.PoolHdl, (unsigned long)RegRecPtr->Buffers[0].BufferPtr);
            }

            /* If a double buffered table, then free the second buffer as well */
            if (RegRecPtr->DblBuffered)
            {
                Status = CFE_ES_PutPoolBuf(CFE_TBL_TaskData.Buf.PoolHdl, (uint32 *)RegRecPtr->Buffers[1].BufferPtr);
                RegRecPtr->Buffers[1].BufferPtr = NULL;

                if (Status < 0)
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:RemoveAccessLink-PutPoolBuf[1] Fail Stat=0x%08X, Hndl=0x%08lX, Buf=0x%08lX\n",
                            (unsigned int)Status, (unsigned long)CFE_TBL_TaskData.Buf.PoolHdl, (unsigned long)RegRecPtr->Buffers[1].BufferPtr);
                }
            }
            else
            {
                /* If a shared buffer has been allocated to the table, then release it as well */
                if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
                {
                    /* Free the working buffer */
                    CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].Taken = FALSE;
                    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
                }
            }
        }
    }

    /* Unlock the registry to allow others to modify it */
    CFE_TBL_UnlockRegistry();

    return Status;
}   /* End of CFE_TBL_RemoveAccessLink() */


/*******************************************************************
**
** CFE_TBL_GetAddressInternal
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/


int32 CFE_TBL_GetAddressInternal(void **TblPtr, CFE_TBL_Handle_t TblHandle, uint32 ThisAppId)
{
    int32   Status = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    /* Check table handle validity */
    Status = CFE_TBL_ValidateHandle(TblHandle);

    if (Status == CFE_SUCCESS)
    {
        /* Get a pointer to the Access Descriptor */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];

        /* Verify that we are allowed access to the table */
        Status = CFE_TBL_CheckAccessRights(TblHandle, ThisAppId);

        if (Status == CFE_SUCCESS)
        {
            /* Get a pointer to the Table Registry entry */
            RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

            /* If table is unowned, then owner must have unregistered it when we weren't looking */
            if (RegRecPtr->OwnerAppId == CFE_TBL_NOT_OWNED)
            {
                Status = CFE_TBL_ERR_UNREGISTERED;

                CFE_ES_WriteToSysLog("CFE_TBL:GetAddressInternal-App(%d) attempt to access unowned Tbl Handle=%d\n",
                                     (int)ThisAppId, (int)TblHandle);
            }
            else /* Table Registry Entry is valid */
            {
                /* Lock the table and return the current pointer */
                AccessDescPtr->LockFlag = TRUE;

                /* Save the buffer we are using in the access descriptor */
                /* This is used to ensure that if the buffer becomes inactive while */
                /* we are using it, no one will modify it until we are done */
                AccessDescPtr->BufferIndex = RegRecPtr->ActiveBufferIndex;

                *TblPtr = RegRecPtr->Buffers[AccessDescPtr->BufferIndex].BufferPtr;

                /* Return any pending warning or info status indicators */
                Status = CFE_TBL_GetNextNotification(TblHandle);

                /* Clear Table Updated Notify Bit so that caller only gets it once */
                AccessDescPtr->Updated = FALSE;
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_TBL:GetAddressInternal-App(%d) does not have access to Tbl Handle=%d\n",
                                 (int)ThisAppId, (int)TblHandle);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:GetAddressInternal-App(%d) using invalid Tbl Handle=%d\n",
                             (int)ThisAppId, (int)TblHandle);
    }

    return Status;
}   /* End of CFE_TBL_GetAddressInternal() */


/*******************************************************************
**
** CFE_TBL_GetNextNotification
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_GetNextNotification(CFE_TBL_Handle_t TblHandle)
{
    int32 Status = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
    CFE_TBL_RegistryRec_t *RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

    if (!RegRecPtr->TableLoadedOnce)
    {
        /* If the table has never been loaded, return an error code for the address */
        Status = CFE_TBL_ERR_NEVER_LOADED;
    }
    else if (AccessDescPtr->Updated)
    {
        /* If the table has been updated recently, return the update status */
        Status = CFE_TBL_INFO_UPDATED;
    }

    return Status;
}   /* End of CFE_TBL_GetNextNotification() */


/*******************************************************************
**
** CFE_TBL_FindTableInRegistry
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int16 CFE_TBL_FindTableInRegistry(const char *TblName)
{
    int16 RegIndx = CFE_TBL_NOT_FOUND;
    int16 i = -1;

    do
    {
        /* Point to next record in the Table Registry */
        i++;

        /* Check to see if the record is currently being used */
        if (CFE_TBL_TaskData.Registry[i].OwnerAppId != CFE_TBL_NOT_OWNED)
        {
            /* Perform a case sensitive name comparison */
            if (strcmp(TblName, CFE_TBL_TaskData.Registry[i].Name) == 0)
            {
                /* If the names match, then return the index */
                RegIndx = i;
            }
        }
    } while ( (RegIndx == CFE_TBL_NOT_FOUND) && (i < (CFE_TBL_MAX_NUM_TABLES-1)) );

    return RegIndx;
}   /* End of CFE_TBL_FindTableInRegistry() */


/*******************************************************************
**
** CFE_TBL_FindFreeRegistryEntry
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int16 CFE_TBL_FindFreeRegistryEntry(void)
{
    int16 RegIndx = CFE_TBL_NOT_FOUND;
    int16 i = 0;

    while ( (RegIndx == CFE_TBL_NOT_FOUND) && (i < CFE_TBL_MAX_NUM_TABLES) )
    {
        /* A Table Registry is only "Free" when there isn't an owner AND */
        /* all other applications are not sharing or locking the table   */
        if ((CFE_TBL_TaskData.Registry[i].OwnerAppId == CFE_TBL_NOT_OWNED) &&
            (CFE_TBL_TaskData.Registry[i].HeadOfAccessList == CFE_TBL_END_OF_LIST))
        {
            RegIndx = i;
        }
        else
        {
            i++;
        }
    }

    return RegIndx;
}   /* End of CFE_TBL_FindFreeRegistryEntry() */


/*******************************************************************
**
** CFE_TBL_FindFreeHandle
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

CFE_TBL_Handle_t CFE_TBL_FindFreeHandle(void)
{
    CFE_TBL_Handle_t HandleIndx = CFE_TBL_END_OF_LIST;
    int16 i = 0;

    while ((HandleIndx == CFE_TBL_END_OF_LIST) && (i < CFE_TBL_MAX_NUM_HANDLES))
    {
        if (CFE_TBL_TaskData.Handles[i].UsedFlag == FALSE)
        {
            HandleIndx = i;
        }
        else
        {
            i++;
        }
    }

    return HandleIndx;
}   /* End of CFE_TBL_FindFreeHandle() */


/*******************************************************************
**
** CFE_TBL_FormTableName
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

void CFE_TBL_FormTableName(char *FullTblName, const char *TblName, uint32 ThisAppId)
{
    char AppName[OS_MAX_API_NAME];

    CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

    /* Ensure that AppName is null terminated */
    AppName[OS_MAX_API_NAME-1] = '\0';

    /* Complete formation of processor specific table name */
    sprintf(FullTblName, "%s.%s", AppName, TblName);

    return;
}   /* End of CFE_TBL_FormTableName() */


/*******************************************************************
**
** CFE_TBL_LockRegistry
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_LockRegistry(void)
{
    int32   Status;

    Status = OS_MutSemTake(CFE_TBL_TaskData.RegistryMutex);

    if (Status == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;

    }

    return Status;

}   /* End of CFE_TBL_LockRegistry() */


/*******************************************************************
**
** CFE_TBL_UnlockRegistry
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_UnlockRegistry(void)
{
    int32   Status;

    Status = OS_MutSemGive(CFE_TBL_TaskData.RegistryMutex);

    if (Status == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;

    }

    return Status;

}   /* End of CFE_TBL_UnlockRegistry() */


/*******************************************************************
**
** CFE_TBL_GetWorkingBuffer
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_GetWorkingBuffer(CFE_TBL_LoadBuff_t **WorkingBufferPtr,
                               CFE_TBL_RegistryRec_t *RegRecPtr,
                               boolean CalledByApp)
{
    int32   Status = CFE_SUCCESS;
    int32   i;
    int32   InactiveBufferIndex;
    CFE_TBL_Handle_t AccessIterator;

    /* Initialize return pointer to NULL */
    *WorkingBufferPtr = NULL;

    /* If a load is already in progress, return the previously allocated working buffer */
    if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
    {
        if (RegRecPtr->DblBuffered)
        {
            *WorkingBufferPtr = &RegRecPtr->Buffers[(1U-RegRecPtr->ActiveBufferIndex)];
        }
        else
        {
            *WorkingBufferPtr = &CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress];
        }
    }
    else
    {
        /* If the table is uninitialized and the function is called by an application (rather than       */
        /* by the Table Services application), then use the current active buffer as the working buffer. */
        /* This allows many tasks with many tables to perform the initialization without conflict        */
        /* over the accessibility of the shared working buffers.                                         */
        if ((RegRecPtr->TableLoadedOnce == FALSE) && (CalledByApp == TRUE))
        {
            if (RegRecPtr->DblBuffered)
            {
                *WorkingBufferPtr = &RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex];
            }
            else
            {
                *WorkingBufferPtr = &RegRecPtr->Buffers[0];
            }
        }
        else
        {
            /* If the table is a double buffered table, then check to make sure the */
            /* inactive buffer has been freed by any Applications that may have been using it */
            if (RegRecPtr->DblBuffered)
            {
                /* Determine the index of the Inactive Buffer Pointer */
                InactiveBufferIndex = 1 - RegRecPtr->ActiveBufferIndex;

                /* Scan the access descriptor table to determine if anyone is still using the inactive buffer */
                AccessIterator = RegRecPtr->HeadOfAccessList;
                while ((AccessIterator != CFE_TBL_END_OF_LIST) && (Status == CFE_SUCCESS))
                {
                    if ((CFE_TBL_TaskData.Handles[AccessIterator].BufferIndex == InactiveBufferIndex) &&
		                (CFE_TBL_TaskData.Handles[AccessIterator].LockFlag))
                    {
                        Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;

                        CFE_ES_WriteToSysLog("CFE_TBL:GetWorkingBuffer-Inactive Dbl Buff Locked for '%s' by AppId=%d\n",
                                             RegRecPtr->Name, (int)CFE_TBL_TaskData.Handles[AccessIterator].AppId);
                    }

                    /* Move to next access descriptor in linked list */
                    AccessIterator = CFE_TBL_TaskData.Handles[AccessIterator].NextLink;
                }

                /* If buffer is free, then return the pointer to it */
                if (Status == CFE_SUCCESS)
                {
                    *WorkingBufferPtr = &RegRecPtr->Buffers[InactiveBufferIndex];
                    RegRecPtr->LoadInProgress = InactiveBufferIndex;
                }
            }
            else /* Single Buffered Table */
            {
                /* Take Mutex to make sure we are not trying to grab a working buffer that some */
                /* other application is also trying to grab. */
                Status = OS_MutSemTake(CFE_TBL_TaskData.WorkBufMutex);

                /* Make note of any errors but continue and hope for the best */
                if (Status != OS_SUCCESS)
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:GetWorkBuf-Internal error taking WorkBuf Mutex (Status=0x%08X)\n",
                            (unsigned int)Status);
                }

                /* Determine if there are any common buffers available */
                i = 0;
                while ((i < CFE_TBL_MAX_SIMULTANEOUS_LOADS) && (CFE_TBL_TaskData.LoadBuffs[i].Taken == TRUE))
                {
                    i++;
                }

                /* If a free buffer was found, then return the address to the associated shared buffer */
                if (i < CFE_TBL_MAX_SIMULTANEOUS_LOADS)
                {
                    CFE_TBL_TaskData.LoadBuffs[i].Taken = TRUE;
                    *WorkingBufferPtr = &CFE_TBL_TaskData.LoadBuffs[i];
                    RegRecPtr->LoadInProgress = i;

                    /* Translate OS_SUCCESS into CFE_SUCCESS */
                    Status = CFE_SUCCESS;
                }
                else
                {
                    Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;

                    CFE_ES_WriteToSysLog("CFE_TBL:GetWorkingBuffer-All shared buffers are locked\n");
                }

                /* Allow others to obtain a shared working buffer */
                OS_MutSemGive(CFE_TBL_TaskData.WorkBufMutex);
            }

            if ((*WorkingBufferPtr) != NULL)
            {
                /* In case the file contains a partial table load, get the active buffer contents first */
                CFE_PSP_MemCpy((*WorkingBufferPtr)->BufferPtr,
                          RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr,
                          RegRecPtr->Size);
            }
        }
    }


    return Status;

}   /* End of CFE_TBL_GetWorkingBuffer() */


/*******************************************************************
**
** CFE_TBL_LoadFromFile
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_LoadFromFile(CFE_TBL_LoadBuff_t *WorkingBufferPtr,
                           CFE_TBL_RegistryRec_t *RegRecPtr,
                           const char *Filename)
{
    int32                Status = CFE_SUCCESS;
    CFE_FS_Header_t      StdFileHeader;
    CFE_TBL_File_Hdr_t   TblFileHeader;
    int32                FileDescriptor;
    size_t               FilenameLen = strlen(Filename);
    uint32               NumBytes;
    uint8                ExtraByte;

    if (FilenameLen > (OS_MAX_PATH_LEN-1))
    {
        Status = CFE_TBL_ERR_FILENAME_TOO_LONG;
    }
    else
    {
        /* Try to open the specified table file */
        FileDescriptor = OS_open(Filename, OS_READ_ONLY, 0);

        if (FileDescriptor >= 0)
        {
            Status = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);

            if (Status == CFE_SUCCESS)
            {
                /* Verify that the specified file has compatible data for specified table */
                if (strcmp(RegRecPtr->Name, TblFileHeader.TableName) == 0)
                {
                    if ((TblFileHeader.Offset + TblFileHeader.NumBytes) > RegRecPtr->Size)
                    {
                        Status = CFE_TBL_ERR_FILE_TOO_LARGE;
                    }
                    else
                    {
                        /* Any Table load that starts beyond the first byte is a "partial load" */
                        /* But a file that starts with the first byte and ends before filling   */
                        /* the whole table is just considered "short".                          */
                        if (TblFileHeader.Offset > 0)
                        {
                            Status = CFE_TBL_WARN_PARTIAL_LOAD;
                        }
                        else if (TblFileHeader.NumBytes < RegRecPtr->Size)
                        {
                            Status = CFE_TBL_WARN_SHORT_FILE;
                        }

                        NumBytes = OS_read(FileDescriptor,
                                           &WorkingBufferPtr->BufferPtr[TblFileHeader.Offset],
                                           TblFileHeader.NumBytes);

                        if (NumBytes != TblFileHeader.NumBytes)
                        {
                            Status = CFE_TBL_ERR_LOAD_INCOMPLETE;
                        }
                        
                        /* Check to see if the file is too large (ie - more data than header claims) */
                        NumBytes = OS_read(FileDescriptor, &ExtraByte, 1);
                        
                        /* If successfully read another byte, then file must have too much data */
                        if (NumBytes == 1)
                        {
                            Status = CFE_TBL_ERR_FILE_TOO_LARGE;
                        }

                        CFE_PSP_MemSet(WorkingBufferPtr->DataSource, 0, OS_MAX_PATH_LEN);
                        strncpy(WorkingBufferPtr->DataSource, Filename, OS_MAX_PATH_LEN);

                        /* Save file creation time for later storage into Registry */
                        WorkingBufferPtr->FileCreateTimeSecs = StdFileHeader.TimeSeconds;
                        WorkingBufferPtr->FileCreateTimeSubSecs = StdFileHeader.TimeSubSeconds;
                        
                        /* Compute the CRC on the specified table buffer */
                        WorkingBufferPtr->Crc = CFE_ES_CalculateCRC(WorkingBufferPtr->BufferPtr,
                                                                    RegRecPtr->Size,
                                                                    0,
                                                                    CFE_ES_DEFAULT_CRC);
                    }
                }
                else
                {
                    Status = CFE_TBL_ERR_FILE_FOR_WRONG_TABLE;
                }
            }

            OS_close(FileDescriptor);
        }
        else
        {
            /* Return error code obtained from OS_open */
            Status = FileDescriptor;
        }
    }

    return Status;
}


/*******************************************************************
**
** CFE_TBL_UpdateInternal
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_UpdateInternal( CFE_TBL_Handle_t TblHandle,
                              CFE_TBL_RegistryRec_t *RegRecPtr,
                              CFE_TBL_AccessDescriptor_t *AccessDescPtr )
{
    int32 Status = CFE_SUCCESS;
    CFE_TBL_Handle_t AccessIterator;
    boolean LockStatus = FALSE;
    
    if ((!RegRecPtr->LoadPending) || (RegRecPtr->LoadInProgress == CFE_TBL_NO_LOAD_IN_PROGRESS))
    {
        /* Question: Should calling CFE_TBL_Update on a table with no load pending */
        /* be considered an error?  Currently assuming it is not an error.         */
        Status = CFE_TBL_INFO_NO_UPDATE_PENDING;
    }
    else
    {
        if (RegRecPtr->DblBuffered)
        {
            /* To update a double buffered table only requires a pointer swap */
            RegRecPtr->ActiveBufferIndex = (uint8)RegRecPtr->LoadInProgress;

            /* Source description in buffer should already have been updated by either */
            /* the LoadFromFile function or the Load function (when a memory load).    */
            /* However, we need to copy it into active registry area */
            strncpy(RegRecPtr->LastFileLoaded,
                    RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].DataSource,
                    OS_MAX_PATH_LEN);

            CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);
            
            /* If the table is a critical table, update the appropriate CDS with the new data */
            if (RegRecPtr->CriticalTable == TRUE)
            {
                CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
            }
        }
        else
        {
            /* Check to see if the Table is locked by anyone */
            AccessIterator = RegRecPtr->HeadOfAccessList;
            while (AccessIterator != CFE_TBL_END_OF_LIST)
            {
                LockStatus = (LockStatus || CFE_TBL_TaskData.Handles[AccessIterator].LockFlag);

                AccessIterator = CFE_TBL_TaskData.Handles[AccessIterator].NextLink;
            }

            if (LockStatus)
            {
                Status = CFE_TBL_INFO_TABLE_LOCKED;

                CFE_ES_WriteToSysLog("CFE_TBL:UpdateInternal-Unable to update locked table Handle=%d\n", TblHandle);
            }
            else
            {
                /* To update a single buffered table requires a memcpy from working buffer */
                CFE_PSP_MemCpy(RegRecPtr->Buffers[0].BufferPtr,
                          CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr,
                          RegRecPtr->Size);

                /* Save source description with active buffer */
                strncpy(RegRecPtr->Buffers[0].DataSource,
                        CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].DataSource,
                        OS_MAX_PATH_LEN);
                strncpy(RegRecPtr->LastFileLoaded,
                        CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].DataSource,
                        OS_MAX_PATH_LEN);
                
                /* Save the file creation time from the loaded file into the Table Registry */
                RegRecPtr->Buffers[0].FileCreateTimeSecs = 
                    CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].FileCreateTimeSecs;
                RegRecPtr->Buffers[0].FileCreateTimeSubSecs = 
                    CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].FileCreateTimeSubSecs;
                
                /* Save the previously computed CRC into the new buffer */
                RegRecPtr->Buffers[0].Crc = CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].Crc;

                /* Free the working buffer */
                CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].Taken = FALSE;

                CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);
            
                /* If the table is a critical table, update the appropriate CDS with the new data */
                if (RegRecPtr->CriticalTable == TRUE)
                {
                    CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
                }
            }
        }
    }

    return Status;
}   /* End of CFE_TBL_UpdateInternal() */


/*******************************************************************
**
** CFE_TBL_NotifyTblUsersOfUpdate
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

void CFE_TBL_NotifyTblUsersOfUpdate(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_Handle_t AccessIterator;
    
    /* Reset Load in Progress Values */
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    RegRecPtr->TimeOfLastUpdate = CFE_TIME_GetTime();

    /* Clear notification of pending load (as well as NO LOAD) and notify everyone of update */
    RegRecPtr->LoadPending = FALSE;
    RegRecPtr->TableLoadedOnce = TRUE;
    AccessIterator = RegRecPtr->HeadOfAccessList;
    while (AccessIterator != CFE_TBL_END_OF_LIST)
    {
        CFE_TBL_TaskData.Handles[AccessIterator].Updated = TRUE;

        AccessIterator = CFE_TBL_TaskData.Handles[AccessIterator].NextLink;
    }
}

/*******************************************************************
**
** CFE_TBL_ReadHeaders
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_ReadHeaders( int32 FileDescriptor,
                           CFE_FS_Header_t *StdFileHeaderPtr,
                           CFE_TBL_File_Hdr_t *TblFileHeaderPtr,
                           const char *LoadFilename )
{
    int32 Status;
    int32 EndianCheck = 0x01020304;

    #if (CFE_TBL_VALID_SCID_COUNT > 0)
    static uint32 ListSC[2] = { CFE_TBL_VALID_SCID_1, CFE_TBL_VALID_SCID_2};
    uint32 IndexSC;
    #endif
    
    #if (CFE_TBL_VALID_PRID_COUNT > 0)
    static uint32 ListPR[4] = { CFE_TBL_VALID_PRID_1, CFE_TBL_VALID_PRID_2,
                                CFE_TBL_VALID_PRID_3, CFE_TBL_VALID_PRID_4};
    uint32 IndexPR;
    #endif
    
    /* Once the file is open, read the headers to determine the target Table */
    Status = CFE_FS_ReadHeader(StdFileHeaderPtr, FileDescriptor);

    /* Verify successful read of standard cFE File Header */
    if (Status != sizeof(CFE_FS_Header_t))
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_STD_HDR_ERR_EID,
                                   CFE_EVS_ERROR,
                                   CFE_TBL_TaskData.TableTaskAppId,
                                   "Unable to read std header for '%s', Status = 0x%08X",
                                   LoadFilename, (unsigned int)Status);

        Status = CFE_TBL_ERR_NO_STD_HEADER;
    }
    else
    {
        /* Verify the file type is a cFE compatible file */
        if (StdFileHeaderPtr->ContentType != CFE_FS_FILE_CONTENT_ID)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_TYPE_ERR_EID,
                                       CFE_EVS_ERROR,
                                       CFE_TBL_TaskData.TableTaskAppId,
                                       "File '%s' is not a cFE file type, ContentType = 0x%08X",
                                       LoadFilename, (unsigned int)StdFileHeaderPtr->ContentType);

            Status = CFE_TBL_ERR_BAD_CONTENT_ID;
        }
        else
        {
            /* Verify the SubType to ensure that it is a Table Image File */
            if (StdFileHeaderPtr->SubType != CFE_FS_TBL_IMG_SUBTYPE)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_SUBTYPE_ERR_EID,
                                           CFE_EVS_ERROR,
                                           CFE_TBL_TaskData.TableTaskAppId,
                                           "File subtype for '%s' is wrong. Subtype = 0x%08X",
                                           LoadFilename, (unsigned int)StdFileHeaderPtr->SubType);

                Status = CFE_TBL_ERR_BAD_SUBTYPE_ID;
            }
            else
            {
                Status = OS_read(FileDescriptor, TblFileHeaderPtr, sizeof(CFE_TBL_File_Hdr_t));

                /* Verify successful read of cFE Table File Header */
                if (Status != sizeof(CFE_TBL_File_Hdr_t))
                {
                    CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_TBL_HDR_ERR_EID,
                                               CFE_EVS_ERROR,
                                               CFE_TBL_TaskData.TableTaskAppId,
                                               "Unable to read tbl header for '%s', Status = 0x%08X",
                                               LoadFilename, (unsigned int)Status);

                    Status = CFE_TBL_ERR_NO_TBL_HEADER;
                }
                else
                {
                    /* All "required" checks have passed and we are pointing at the data */
                    Status = CFE_SUCCESS;

                    if ((*(char *)&EndianCheck) == 0x04)
                    {
                        /* If this is a little endian processor, then the standard cFE Table Header,   */
                        /* which is in big endian format, must be swapped so that the data is readable */
                        CFE_TBL_ByteSwapTblHeader(TblFileHeaderPtr);
                    }

                    /*
                     * Ensure termination of all local strings. These were read from a file, so they
                     * must be treated with appropriate care.  This could happen in case the file got
                     * damaged in transit or simply was not written properly to begin with.
                     *
                     * Since the "TblFileHeaderPtr" is a local buffer, this can be done directly.
                     */
                    TblFileHeaderPtr->TableName[sizeof(TblFileHeaderPtr->TableName) - 1] = '\0';

                    /* Verify Spacecraft ID contained in table file header [optional] */
                    #if (CFE_TBL_VALID_SCID_COUNT > 0)
                    if (Status == CFE_SUCCESS)
                    {
                        Status = CFE_TBL_ERR_BAD_SPACECRAFT_ID;
                        for (IndexSC = 0; IndexSC < CFE_TBL_VALID_SCID_COUNT; IndexSC++)
                        {
                            if (StdFileHeaderPtr->SpacecraftID == ListSC[IndexSC])
                            {
                                Status = CFE_SUCCESS;
                            }
                        }

                        if (Status == CFE_TBL_ERR_BAD_SPACECRAFT_ID)
                        {
                            CFE_EVS_SendEventWithAppID(CFE_TBL_SPACECRAFT_ID_ERR_EID,
                                                       CFE_EVS_ERROR,
                                                       CFE_TBL_TaskData.TableTaskAppId,
                                                       "Unable to verify Spacecraft ID for '%s', ID = 0x%08X",
                                                       LoadFilename, StdFileHeaderPtr->SpacecraftID);
                        }
                    }
                    #endif

                    /* Verify Processor ID contained in table file header [optional] */
                    #if (CFE_TBL_VALID_PRID_COUNT > 0)
                    if (Status == CFE_SUCCESS)
                    {
                        Status = CFE_TBL_ERR_BAD_PROCESSOR_ID;
                        for (IndexPR = 0; IndexPR < CFE_TBL_VALID_PRID_COUNT; IndexPR++)
                        {
                            if (StdFileHeaderPtr->ProcessorID == ListPR[IndexPR])
                            {
                                Status = CFE_SUCCESS;
                            }
                        }

                        if (Status == CFE_TBL_ERR_BAD_PROCESSOR_ID)
                        {
                            CFE_EVS_SendEventWithAppID(CFE_TBL_PROCESSOR_ID_ERR_EID,
                                                       CFE_EVS_ERROR,
                                                       CFE_TBL_TaskData.TableTaskAppId,
                                                       "Unable to verify Processor ID for '%s', ID = 0x%08X",
                                                       LoadFilename, StdFileHeaderPtr->ProcessorID);
                        }
                    }
                    #endif
                }
            }
        }
    }

    return Status;
}   /* End of CFE_TBL_ReadHeaders() */


/*******************************************************************
**
** CFE_TBL_ByteSwapTblHeader
**
** NOTE: For complete prolog information, see above
********************************************************************/

void CFE_TBL_ByteSwapTblHeader(CFE_TBL_File_Hdr_t *HdrPtr)
{
    CFE_TBL_ByteSwapUint32(&HdrPtr->Reserved);
    CFE_TBL_ByteSwapUint32(&HdrPtr->Offset);
    CFE_TBL_ByteSwapUint32(&HdrPtr->NumBytes);
} /* End of CFE_TBL_ByteSwapTblHeader() */


/*******************************************************************
**
** CFE_TBL_ByteSwapUint32
**
** NOTE: For complete prolog information, see above
********************************************************************/

void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr)
{
    int32 Temp = *Uint32ToSwapPtr;
    char *InPtr = (char *)&Temp;
    char *OutPtr = (char *)Uint32ToSwapPtr;
    
    OutPtr[0] = InPtr[3];
    OutPtr[1] = InPtr[2];
    OutPtr[2] = InPtr[1];
    OutPtr[3] = InPtr[0];    
} /* End of CFE_TBL_ByteSwapUint32() */

/*******************************************************************
**
** CFE_TBL_CleanUpApp
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_CleanUpApp(uint32 AppId)
{
    uint32 i;
    CFE_TBL_RegistryRec_t *RegRecPtr = NULL;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;

    /* Scan Dump Requests to determine if any of the tables that */
    /* were to be dumped will be deleted */
    for (i=0; i<CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        /* Check to see if the table to be dumped is owned by the App to be deleted */
        if ((CFE_TBL_TaskData.DumpControlBlocks[i].State != CFE_TBL_DUMP_FREE) && 
           (CFE_TBL_TaskData.DumpControlBlocks[i].RegRecPtr->OwnerAppId == AppId))
        {
            /* If so, then remove the dump request */
            CFE_TBL_TaskData.DumpControlBlocks[i].State = CFE_TBL_DUMP_FREE;
        }
    }
    
    /* Scan Access Descriptors to determine if the Application had access to any tables */
    for (i=0; i<CFE_TBL_MAX_NUM_HANDLES; i++)
    {
        /* Check to see if the Handle belongs to the Application being deleted */
        if (CFE_TBL_TaskData.Handles[i].AppId == AppId)
        {
            /* Delete the handle (and the table, if the App owned it) */
            /* Get a pointer to the relevant Access Descriptor */
            AccessDescPtr = &CFE_TBL_TaskData.Handles[i];

            /* Get a pointer to the relevant entry in the registry */
            RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

            /* Determine if the Application owned this particular table */
            if (RegRecPtr->OwnerAppId == AppId)
            {
                /* Mark table as free, although, technically, it isn't free until the */
                /* linked list of Access Descriptors has no links in it.              */
                /* NOTE: Allocated memory is freed when all Access Links have been    */
                /*       removed.  This allows Applications to continue to use the    */
                /*       data until they acknowledge that the table has been removed. */
                RegRecPtr->OwnerAppId = (uint32)CFE_TBL_NOT_OWNED;

                /* Remove Table Name */
                RegRecPtr->Name[0] = '\0';
            }
            
            /* Remove the Access Descriptor Link from linked list */
            /* NOTE: If this removes the last access link, then   */
            /*       memory buffers are set free as well.         */
            CFE_TBL_RemoveAccessLink(i);
        }
    }

    return CFE_SUCCESS;
}

/*******************************************************************
**
** CFE_TBL_FindCriticalTblInfo
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

void CFE_TBL_FindCriticalTblInfo(CFE_TBL_CritRegRec_t **CritRegRecPtr, CFE_ES_CDSHandle_t CDSHandleToFind)
{
    uint32 i;
    
    /* Assume the record is never found */
    *CritRegRecPtr = NULL;
    
    for (i=0; i<CFE_TBL_MAX_CRITICAL_TABLES; i++)
    {
        if (CFE_TBL_TaskData.CritReg[i].CDSHandle == CDSHandleToFind)
        {
            *CritRegRecPtr = &CFE_TBL_TaskData.CritReg[i];
            break;
        }
    }
}


/*******************************************************************
**
** CFE_TBL_UpdateCriticalTblCDS
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

void CFE_TBL_UpdateCriticalTblCDS(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_CritRegRec_t *CritRegRecPtr = NULL;
    
    int32 Status = CFE_SUCCESS;
    
    /* Copy an image of the updated table to the CDS for safekeeping */
    Status = CFE_ES_CopyToCDS(RegRecPtr->CDSHandle, RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr);
    
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CFE_TBL:UpdateCritTbl-Unable to update Critical Table '%s' in CDS (Err=0x%08X)\n", 
                             RegRecPtr->Name, (unsigned int)Status);
    }
    else
    {
        /* Locate entry in Critical Table Registry */   
        CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, RegRecPtr->CDSHandle);
        if (CritRegRecPtr != NULL)
        {
            /* Save information related to the source of the data stored in the table in Critical Table Registry */
            CritRegRecPtr->FileCreateTimeSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSecs;
            CritRegRecPtr->FileCreateTimeSubSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSubSecs;
            strncpy(CritRegRecPtr->LastFileLoaded, RegRecPtr->LastFileLoaded, OS_MAX_PATH_LEN);
            CritRegRecPtr->TimeOfLastUpdate = RegRecPtr->TimeOfLastUpdate;
            CritRegRecPtr->TableLoadedOnce = RegRecPtr->TableLoadedOnce;
            
            /* Update copy of Critical Table Registry in the CDS */
            Status = CFE_ES_CopyToCDS(CFE_TBL_TaskData.CritRegHandle, CFE_TBL_TaskData.CritReg);
            
            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("CFE_TBL:UpdateCritTbl-Unable to update Critical Table Registry in CDS (Err=0x%08X)\n",
                        (unsigned int)Status);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_TBL:UpdateCritTbl-Error finding '%s' in Critical Table Registry\n", 
                                 RegRecPtr->Name);
        }
    }
    
    /* Don't bother notifying the caller of the problem since the active table is still legitimate */
}


/*******************************************************************
**
** CFE_TBL_SendNotificationMsg
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_SendNotificationMsg(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    int32 Status = CFE_SUCCESS;
    
    /* First, determine if a message should be sent */
    if (RegRecPtr->NotifyByMsg)
    {
        /*
        ** Initialize notification message packet (clear user data area)...
        */
        CFE_SB_InitMsg(&CFE_TBL_TaskData.NotifyMsg,
                        RegRecPtr->NotificationMsgId,
                        sizeof(CFE_TBL_NotifyCmd_t), TRUE);
        
        /* Set the command code */
        CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &CFE_TBL_TaskData.NotifyMsg, RegRecPtr->NotificationCC);
        
        /* Set the command parameter */
        CFE_TBL_TaskData.NotifyMsg.Payload.Parameter = RegRecPtr->NotificationParam;
    
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_TBL_TaskData.NotifyMsg);
        Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TBL_TaskData.NotifyMsg);
    
        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_TBL_FAIL_NOTIFY_SEND_ERR_EID,
                              CFE_EVS_ERROR,
                              "Manage Notification Pkt Error(Status=0x%08X)",
                              (unsigned int)Status);
        }
    }
    
    return Status;
}

/************************/
/*  End of File Comment */
/************************/

