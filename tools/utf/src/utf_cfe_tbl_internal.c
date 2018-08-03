/*
** File: utf_cfe_tbl_internal.c
** $Id: utf_cfe_tbl_internal.c 1.6 2012/01/13 12:52:00GMT-05:00 acudmore Exp  $
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
**  $Date: 2012/01/13 12:52:00GMT-05:00 $
**  $Revision: 1.6 $
** $Log: utf_cfe_tbl_internal.c  $
** Revision 1.6 2012/01/13 12:52:00GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.5 2010/10/25 15:06:41EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 14:57:41EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/06/24 09:18:09EDT rmcgraw 
** DCR8290:4 OS_BSP to CFE_PSP_ and OS_Mem to CFE_PSP_Mem changes
** Revision 1.2 2008/07/30 13:05:05EDT wfmoleski 
** Several Table Services modifications affected the UTF including a new API. The test16 example was 
** modified to execute this new API.
** Revision 1.1 2008/04/17 08:09:38EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.8 2007/09/24 07:51:50EDT wfmoleski 
** Made changes to UTF source to reflect the API change to GetPoolBuf in the cFE fsw.
** Revision 1.7 2007/05/25 11:07:49EDT wfmoleski 
** Modification to add Critical Table support as well as other fsw changes for cFE 4.1.0.
** Revision 1.6 2006/11/09 08:41:40EST wfmoleski 
** UTF mods for cFE 4.0 and LoadFromFile function update
** Revision 1.5 2006/06/22 11:04:58EDT wfmoleski 
** Added the CFE_TBL_GetInfo implementation and modifications to reflect the fsw code changes. This included ES_GetBufPool argument changes.
** Revision 1.4 2006/06/09 16:02:37EDT nsschweiss 
** Removed debug statements.
** Revision 1.3 2006/05/26 15:05:34EDT nsschweiss 
** Changes for UTF 3.2.
** Revision 1.2 2006/05/09 14:50:24EDT nsschweiss 
** Commented in calls to Executive Services which are now available in UTF.
** Incorporated new Load Buffer type.
** Modified as needed for new macro names.
** Changed comment syntax to meet cFE style and C ansi standard.
** Revision 1.1 2006/03/21 16:44:28EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.1 2005/11/16 16:29:12EST NSSCHWEISS 
** Initial revision
** Member added to project d:/mksdata/MKS-LRO-REPOSITORY/utf/src_utf/project.pj
** 2005/11/30 nschweiss Edited EarlyInit().
** 05/10/12 nschweiss Commented CFE_ES_GetAppID call back in in 
**          CFE_TBL_ValidateAppID definition.
** 05/10/11 nschweiss Correct CFE_TBL_InitData to initialize Table Registry
**          starting at 0 rather than 1.
** 05/09/23 nschweiss Define CFE_TBL_UpdateInternal.
**           Updated CFE_TBL_RemoveAccessLink to release memory.
**   CFE_TBL_GetWorkingBuffer had been leaving LoadInProgress field to true.
**   Corrected & greatly simplified.
** 05/09/22 nschweiss Defined LoadFromFile
** 05/09/21 nschweiss Added definition for CFE_TBL_InitData.
** Modified CFE_TBL_FindFreeRegistryEntry.
** 05/09/20 nschweiss Updated CFE_TBL_ValidateAppID for UTF use.
**
**
*/

/*
** Required header files...
*/
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "utf_types.h"
#include "cfe.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_events.h"
#include "cfe_es_cds.h"
#include "utf_cfe_tbl.h"


/*
**  Globals
*/
CFE_TBL_TaskData_t CFE_TBL_TaskData;

/*****************************************************************************/
/**
** \brief Performs a byte swap on a uint32 integer
**
** \par Description
**        Converts a big-endian uint32 integer to a little-endian uint32 integer
**        and vice-versa.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  Uint32ToSwapPtr Pointer to uint32 value to be swapped.
** 
** \param[out] *Uint32ToSwapPtr The swapped uint32 value
**
**                     
******************************************************************************/
void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr);

/******************************************************************************
**  Function:  CFE_TBL_EarlyInit()
**
**  Purpose:
**    Initialize the Table Servies
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
    int16 i;
    int32 j;
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
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_SB_InitMsg(&CFE_TBL_TaskData.HkPacket,
                    CFE_TBL_HK_TLM_MID,
                    sizeof(CFE_TBL_HkPacket_t), TRUE);

    /*
    ** Initialize table registry report packet (clear user data area)...
    */
 /*   CFE_SB_InitMsg(&CFE_TBL_TaskData.TblRegPacket,
                    CFE_TBL_REG_TLM_MID,
                   sizeof(CFE_TBL_TblRegPacket_t), TRUE);*/

    /* Initialize memory partition and allocate shared table buffers. */
    
    Status = CFE_ES_PoolCreate(&CFE_TBL_TaskData.Buf.PoolHdl,
    						   CFE_TBL_TaskData.Buf.Partition,
                               CFE_TBL_BUF_MEMORY_BYTES);
 
    if(Status < 0)
    {
        CFE_ES_WriteToSysLog("CFE_TBL:InitBuffers PoolCreate fail Status=0x%X\n", Status);
    }
    else
    {
	    /* Initialize each of the shared load buffers */
    	j=0;
    	/* original : while ((j < CFE_TBL_MAX_SIMULTANEOUS_LOADS) && (Status == CFE_SUCCESS)) */
    	while ((j < CFE_TBL_MAX_SIMULTANEOUS_LOADS) && (Status >= 0))
	    {
    	    /* Allocate memory for shared load buffers */
        	Status = CFE_ES_GetPoolBuf((uint32 **)&CFE_TBL_TaskData.LoadBuffs[j].BufferPtr,
        							   CFE_TBL_TaskData.Buf.PoolHdl,
            	                       CFE_TBL_MAX_SNGL_TABLE_SIZE);
	        /* UTF_put_text("UTF debug: CFE_ES_GetPoolBuf returned %x.\n", Status); */

	        if (Status < 0)
	        {
	            CFE_ES_WriteToSysLog("CFE_TBL:InitBuffers GetPoolBuf Fail Index=%d, Status=0x%X\n", j, Status);
	        }
	        else
	        {
	            CFE_TBL_TaskData.LoadBuffs[j].Taken = FALSE;
	            /* UTF_put_text("UTF Info: Success return from CFE_ES_GetPoolBuf for j=%d.\n",j); */
	        }

	        j++;
    	}
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
    }
    
    if (Status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* Try to recover the Critical Table Registry from the CDS */
        Status = CFE_ES_RestoreFromCDS(CFE_TBL_TaskData.CritReg, CFE_TBL_TaskData.CritRegHandle);
        
        if (Status != CFE_SUCCESS)
        {
            /* Note if we were unable to recover error free Critical Table Registry from the CDS */
            CFE_ES_WriteToSysLog("CFE_TBL:EarlyInit-Failed to recover Critical Table Registry (Err=0x%08X)\n", Status); 
        }
        
        /* Whether we recovered the Critical Table Registry or not, we are successful with initialization */
        Status = CFE_SUCCESS;
    }
    else if (Status != CFE_SUCCESS)
    {
        /* Not being able to support Critical Tables is not the end of the world */
        /* Note the problem and move on */
        CFE_ES_WriteToSysLog("CFE_TBL:EarlyInit-Failed to create Critical Table Registry (Err=0x%08X)\n", Status);
        
        /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
        Status = CFE_SUCCESS;
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
    RegRecPtr->Buffers[0].BufferPtr = NULL;
    RegRecPtr->Buffers[0].FileCreateTimeSecs = 0;
    RegRecPtr->Buffers[0].FileCreateTimeSubSecs = 0;
    RegRecPtr->Buffers[0].Taken = FALSE;
    RegRecPtr->Buffers[0].DataSource[0] = '\0';
    RegRecPtr->Buffers[1].BufferPtr = NULL;
    RegRecPtr->Buffers[1].FileCreateTimeSecs = 0;
    RegRecPtr->Buffers[1].FileCreateTimeSubSecs = 0;
    RegRecPtr->Buffers[1].Taken = FALSE;
    RegRecPtr->Buffers[1].DataSource[0] = '\0';
    RegRecPtr->ValidationFuncPtr = NULL;
    RegRecPtr->TimeOfLastUpdate.Seconds = 0;
    RegRecPtr->TimeOfLastUpdate.Subseconds = 0;
    RegRecPtr->HeadOfAccessList = CFE_TBL_END_OF_LIST;
    RegRecPtr->LoadInProgress =  CFE_TBL_NO_LOAD_IN_PROGRESS;
    RegRecPtr->ValidateActiveIndex = CFE_TBL_NO_VALIDATION_PENDING;
    RegRecPtr->ValidateInactiveIndex = CFE_TBL_NO_VALIDATION_PENDING;
    RegRecPtr->TableLoadedOnce = FALSE;
    RegRecPtr->LoadPending = FALSE;
    RegRecPtr->DumpOnly = FALSE;
    RegRecPtr->DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;
    RegRecPtr->UserDefAddr = FALSE;
    RegRecPtr->DblBuffered = FALSE;
    RegRecPtr->ActiveBufferIndex = 0;
    RegRecPtr->Name[0] = '\0';
    RegRecPtr->LastFileLoaded[0] = '\0';
}

/*******************************************************************
**
** Name:    CFE_TBL_ValidateHandle
**
** Purpose: Validates handle given by calling App to Table API. Validation
**          includes ensuring the value is within an acceptable range and
**          the Access Descriptor that it identifies is being "used".
**
** Assumptions, External Events, and Notes:
**
**          Note:
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblHandle  - Handle to be validated
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_TBL_ERR_INVALID_HANDLE
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
** Name:    CFE_TBL_ValidateAppID
**
** Purpose: Validates Application ID of calling App.  Validation
**          consists of ensuring the AppID is between zero and
**          CFE_ES_MAX_APPLICATIONS.
**
** Assumptions, External Events, and Notes:
**
**          Note:
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          AppIdPtr - Pointer to value that will hold AppID on return.
**
** Outputs:
**          AppId - The AppID as obtained from CFE_ES_GetAppID
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_TBL_ERR_BAD_APP_ID
**          CFE_ES_???
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
                                 *AppIdPtr, CFE_ES_MAX_APPLICATIONS);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:ValidateAppID-GetAppID failed (Stat=0x%08X)\n", Status);
    }

    return Status;
}   /* End of CFE_TBL_ValidateAppID() */

/*******************************************************************
**
** Name: CFE_TBL_ValidateAccess
**
** Purpose:  Validates whether the calling application has the right to
**           access the table identified with the given TblHandle.  Validation
**           consists of verifying the calling Application's AppID, verifying
**           the legitimacy of the given TblHandle, and checking to make sure
**           the Access Descriptor identified by the TblHandle is associated
**           with the calling Application.
**
** Assumptions, External Events, and Notes:
**
**          Note:
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblHandle - Handle of table whose access is desired.
**          AppIdPtr  - Pointer to value that will hold AppID on return.
**
** Outputs:
**          AppId - The AppID as obtained from CFE_ES_GetAppID
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_TBL_ERR_BAD_APP_ID
**          CFE_ES_???
**          CFE_TBL_ERR_INVALID_HANDLE
**          CFE_TBL_ERR_NO_ACCESS
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
                                     *AppIdPtr, TblHandle, Status);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_TBL:ValidateAccess-Invalid Tbl Handle=%d\n", TblHandle);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:ValidateAccess-Bad AppId=%d\n", *AppIdPtr);
    }

    return Status;
}   /* End of CFE_TBL_ValidateAccess() */


/*******************************************************************
**
** Name: CFE_TBL_CheckAccessRights
**
** Purpose:  Validates whether the calling application has the right to
**           access the table identified with the given TblHandle.  Validation
**           consists of checking to make sure the Access Descriptor identified
**           by the TblHandle is associated with the calling Application.
**
** Assumptions, External Events, and Notes:
**
**          Note: The TblHandle and ThisAppId parameters are assumed to be valid.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblHandle - Handle of table whose access is desired.
**          ThisAppId - AppID of calling Application.
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_TBL_ERR_NO_ACCESS
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
** Name:    CFE_TBL_RemoveAccessLink
**
** Purpose: Removes the given Access Descriptor from the Linked List
**          of Access Descriptors associated with the table specified
**          in the Access Descriptor itself.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function CAN block and should not be called by ISRs.
**          Note: This function assumes the Access Descriptor is completely
**                filled out and the TblHandle has been validated.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblHandle - Handle of Access Descriptor to be removed.
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_SUCCESS
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
                CFE_ES_WriteToSysLog("CFE_TBL:RemoveAccessLink-PutPoolBuf[0] Fail Stat=0x%08X, Hndl=0x%08X, Buf=0x%08X\n",
                                     Status, CFE_TBL_TaskData.Buf.PoolHdl, RegRecPtr->Buffers[0].BufferPtr);
            }

            /* If a double buffered table, then free the second buffer as well */
            if (RegRecPtr->DblBuffered)
            {
                Status = CFE_ES_PutPoolBuf(CFE_TBL_TaskData.Buf.PoolHdl, (uint32 *)RegRecPtr->Buffers[1].BufferPtr);
                RegRecPtr->Buffers[1].BufferPtr = NULL;

                if (Status < 0)
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:RemoveAccessLink-PutPoolBuf[1] Fail Stat=0x%08X, Hndl=0x%08X, Buf=0x%08X\n",
                                         Status, CFE_TBL_TaskData.Buf.PoolHdl, RegRecPtr->Buffers[1].BufferPtr);
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
** Name:    GetAddressInternal
**
** Purpose: Validates the given TblHandle, finds the location of the
**          Table data and returns the address to the data to the caller.
**
** Assumptions, External Events, and Notes:
**
**          Note: It is possible that an Application that was sharing a table
**                would discover, upon making this call, that the table has
**                been unregistered by another Application.  In this situation,
**                this function would return CFE_TBL_ERR_UNREGISTERED.
**          Note: ThisAppId parameter is assumed to be validated.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblPtr    - Pointer to pointer that will hold address of data upon return.
**          TblHandle - Handle of Table whose address is needed.
**          ThisAppId - AppID of application making the address request.
**
** Outputs:
**          TblPtr    - Address of Table Data.
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_TBL_ERR_INVALID_HANDLE
**          CFE_TBL_ERR_NO_ACCESS
**          CFE_TBL_ERR_UNREGISTERED
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
                                     ThisAppId, TblHandle);
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
                                 ThisAppId, TblHandle);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:GetAddressInternal-App(%d) using invalid Tbl Handle=%d\n",
                             ThisAppId, TblHandle);
    }

    return Status;
}   /* End of CFE_TBL_GetAddressInternal() */

/*******************************************************************
**
** Name:    CFE_TBL_GetNextNotification
**
** Purpose: Returns any pending non-error status code for the specified table.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function assumes the TblHandle has been validated.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblHandle - Handle of Table whose pending notifications are
**                      to be returned.
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_TBL_INFO_LOAD_PENDING
**          CFE_TBL_INFO_UPDATED
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
** Name:    CFE_TBL_FindTableInRegistry
**
** Purpose: Locates given Table Name in the Table Registry and
**          returns the appropriate Registry Index.
**
** Assumptions, External Events, and Notes:
**
**          Note:
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          TblName - Pointer to character string containing complete
**                    Table Name (of the format "AppName.TblName").
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_TBL_NOT_FOUND
**          Index into Registry for Table with specified name
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

int32 UTF_CFE_TBL_DisplayTableRegistryContents(void)
{
    int32 i = -1;
    CFE_TBL_Handle_t AccessIterator;
    CFE_TBL_RegistryRec_t *RegRecPtr;
	UTF_put_text("The Table Registry contains the following tables:\n");
	UTF_put_text("  Index   Table_Name            Size(B) Apps_With_Access Load FileName\n");
    do
    {
        /* Point to next record in the Table Registry */
        i++;
        RegRecPtr = &CFE_TBL_TaskData.Registry[i];
        AccessIterator = RegRecPtr->HeadOfAccessList;
        

        /* Check to see if the record is currently being used */
        /* either by owner or via shared access */
        if (AccessIterator != CFE_TBL_END_OF_LIST)
        /* if (CFE_TBL_TaskData.Registry[i].OwnerAppId != CFE_TBL_NOT_OWNED) */
        {
         	
            /* Print info about Table Entry */
            UTF_put_text ("%4d %24s %8d",
            			i, RegRecPtr->Name, RegRecPtr->Size);
            /* Identify applications which have access */
            AccessIterator = RegRecPtr->HeadOfAccessList;
            while (AccessIterator != CFE_TBL_END_OF_LIST)
            {
                UTF_put_text("%4d ",CFE_TBL_TaskData.Handles[AccessIterator].AppId);

                AccessIterator = CFE_TBL_TaskData.Handles[AccessIterator].NextLink;
                if (AccessIterator != CFE_TBL_END_OF_LIST) 
                		UTF_put_text(",");
            }
            UTF_put_text ("%s\n", RegRecPtr->LastFileLoaded);
        }
    } while (i < (CFE_TBL_MAX_NUM_TABLES-1 ));

    UTF_put_text ("(*) indicates the Table was updated via CFE_TBL_Modified API\n");
    
	return CFE_SUCCESS;
}	

/*******************************************************************
**
** Name:    CFE_TBL_FindFreeRegistryEntry
**
** Purpose: Locates a free slot in the Table Registry.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function assumes the registry has been locked.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          None
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_TBL_NOT_FOUND
**          Index into Table Registry of unused entry
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
** Name:    CFE_TBL_FindFreeHandle
**
** Purpose: Locates a free Access Descriptor in the Table Handles Array.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function assumes the registry has been locked.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          None
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_TBL_END_OF_LIST
**          Table Handle of unused Access Descriptor
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
** Name:    CFE_TBL_FormTableName
**
** Purpose: Takes a given Table Name and combines it with the calling
**          Application's name to make a processor specific name of the
**          form: "AppName.TblName"
**
** Assumptions, External Events, and Notes:
**
**          Note: AppName portion will be truncated to OS_MAX_API_NAME.
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          FullTblName - pointer to character buffer of CFE_TBL_MAX_FULL_NAME_LEN size
**                        that will be filled with the processor specific Table Name.
**
**          TblName - pointer to character string containing the Application's local name for
**                    the Table.
**
**          ThisAppId - the Application ID of the Application making the call.
**
** Outputs:
**          FullTblName - processor specific Table Name of the form "AppName.TblName".
**
** Return Values:
**
**          None
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
** Name:    CFE_TBL_LockRegistry
**
** Purpose: Locks the Table Registry to prevent multiple tasks/threads
**          from modifying it at once.
**
** Assumptions, External Events, and Notes:
**
**          Note:
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          None
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_OS_???
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
** Name:    CFE_TBL_UnlockRegistry
**
** Purpose: Unlocks Table Registry to allow other tasks/threads to
**          modify the Table Registry contents.
**
** Assumptions, External Events, and Notes:
**
**          Note:
**
** Date Written:
**          06/15/2005
**
** Inputs:
**          None
**
** Outputs:
**          None
**
** Return Values:
**
**          CFE_SUCCESS
**          CFE_OS_???
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
** Name:    CFE_TBL_LoadFromFile

**

** Purpose: Locates the specified filename in the onboard filesystem
**          and loads its contents into the specified working buffer.
**

** Assumptions, External Events, and Notes:
**

**          Note: This function assumes parameters have been verified.

**

** Date Written:
**          06/15/2005
**

** Inputs:
**          None
**
** Outputs:
**          None

**

** Return Values:
**
         CFE_SUCCESS
**          CFE_TBL_ERR_FILE_NOT_FOUND
**          CFE_TBL_ERR_FILE_TOO_LARGE
**          CFE_TBL_WARN_FILE_TOO_SMALL
**          
********************************************************************/
int32 CFE_TBL_LoadFromFile(CFE_TBL_LoadBuff_t *WorkingBufferPtr,
                           CFE_TBL_RegistryRec_t *RegRecPtr,
                           const char *Filename)
{
    int32                Status = CFE_SUCCESS;
    CFE_FS_Header_t      StdFileHeader;
    CFE_TBL_File_Hdr_t   TblFileHeader;
    int32                FileDescriptor;
    int32                FilenameLen = strlen(Filename);
    uint32               NumBytes;
    uint8                ExtraByte;

    if (FilenameLen > (OS_MAX_PATH_LEN-1))
    {
        Status = CFE_TBL_ERR_FILENAME_TOO_LONG;
    }
    else
    {
        /* Try to open the specified table file */
        /* TODO: Put in PATH search capability to locate file */
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
            Status = FileDescriptor;
        }
    }

    return Status;
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
    
    /* Once the file is open, read the headers to determine the target Table */
    Status = CFE_FS_ReadHeader(StdFileHeaderPtr, FileDescriptor);

    if (Status != sizeof(CFE_FS_Header_t))
    {
        /* Error reading the standard cFE File Header */
        CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_STD_HDR_ERR_EID,
                                   CFE_EVS_ERROR,
                                   CFE_TBL_TaskData.TableTaskAppId,
                                   "Unable to read std header for '%s', Status = 0x%08X",
                                   LoadFilename, Status);

        /* Make sure the Status is not equal to CFE_SUCCESS */
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
                                       LoadFilename, StdFileHeaderPtr->ContentType);

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
                                           LoadFilename, StdFileHeaderPtr->SubType);

                Status = CFE_TBL_ERR_BAD_SUBTYPE_ID;
            }
            else
            {
                Status = OS_read(FileDescriptor, TblFileHeaderPtr, sizeof(CFE_TBL_File_Hdr_t));

                if (Status != sizeof(CFE_TBL_File_Hdr_t))
                {
                    CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_TBL_HDR_ERR_EID,
                                               CFE_EVS_ERROR,
                                               CFE_TBL_TaskData.TableTaskAppId,
                                               "Unable to read tbl header for '%s', Status = 0x%08X",
                                               LoadFilename, Status);

                    /* Make sure the status is not equal to CFE_SUCCESS */
                    Status = CFE_TBL_ERR_NO_TBL_HEADER;
                }
                else
                {
                    /* All checks have passed and we are pointing at the data */
                    Status = CFE_SUCCESS;
                    
                    /* Determine whether this processor is a little endian processor */
                    if ((*(char *)&EndianCheck) == 0x04)
                    {
                        /* If this is a little endian processor, then the standard cFE Table Header,   */
                        /* which is in big endian format, must be swapped so that the data is readable */
                        CFE_TBL_ByteSwapTblHeader(TblFileHeaderPtr);
                    }
                }
            }
        }
    }

    return Status;
}   /* End of CFE_TBL_ReadHeaders() */

/*******************************************************************
**
** CFE_TBL_DumpToBuffer
**
** NOTE: For complete prolog information, see 'cfe_tbl_internal.h'
********************************************************************/

int32 CFE_TBL_DumpToBuffer( CFE_TBL_Handle_t TblHandle )
{
    int32 Status = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
    CFE_TBL_RegistryRec_t      *RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_DumpControl_t      *DumpCtrlPtr = &CFE_TBL_TaskData.DumpControlBlocks[RegRecPtr->DumpControlIndex];
    CFE_TIME_SysTime_t          DumpTime;
    
    /* Copy the contents of the active buffer to the assigned dump buffer */
    CFE_PSP_MemCpy(DumpCtrlPtr->DumpBufferPtr->BufferPtr, RegRecPtr->Buffers[0].BufferPtr, DumpCtrlPtr->Size);
    
    /* Save the current time so that the header in the dump file can have the correct time */
    DumpTime = CFE_TIME_GetTime();
    DumpCtrlPtr->DumpBufferPtr->FileCreateTimeSecs = DumpTime.Seconds;
    DumpCtrlPtr->DumpBufferPtr->FileCreateTimeSubSecs = DumpTime.Subseconds;
    
    /* Disassociate the dump request from the table */
    RegRecPtr->DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;
    
    /* Notify the Table Services Application that the dump buffer is ready to be written to a file */
    DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;
    
    return Status;
}   /* End of CFE_TBL_DumpToBuffer() */

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

void CFE_TBL_CleanUpApp(uint32 AppId)
{
    uint32 i;
    CFE_TBL_RegistryRec_t *RegRecPtr = NULL;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    
    /* Scan Dump Requests to determine if any of the tables that */
    /* were to be dumped will be deleted */
    for (i=0; i<CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        /* Check to see if the table to be dumped is owned by the App to be deleted */
        if (CFE_TBL_TaskData.DumpControlBlocks[i].RegRecPtr->OwnerAppId == AppId)
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
            i=CFE_TBL_MAX_CRITICAL_TABLES;
        }
    }
}

/*******************************************************************
**
** CFE_TBL_UpdateCriticalTblCDS
**
** NOTE: For complete prolog information, see above
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
                             RegRecPtr->Name, Status);
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
            
            /* Update copy of Critical Table Registry in the CDS */
            Status = CFE_ES_CopyToCDS(CFE_TBL_TaskData.CritRegHandle, CFE_TBL_TaskData.CritReg);
            
            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("CFE_TBL:UpdateCritTbl-Unable to update Critical Table Registry in CDS (Err=0x%08X)\n",
                                     Status);
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

/************************/
/*  End of File Comment */
/************************/
