/*
**  File:  
**    cfe_es_cds.c
**
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Purpose:  
**    This file implements the cFE Executive Services Critical Data Store functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**
** $Log: cfe_es_cds.c  $
** Revision 1.7 2014/08/22 15:50:11GMT-05:00 lwalling 
** Changed signed loop counters to unsigned
** Revision 1.6 2014/05/05 13:21:40EDT acudmore 
** Changed strncpy call to use correct define ( CFE_ES_CDS_MAX_FULL_NAME_LEN )
** Revision 1.5 2014/04/23 15:25:52GMT-05:00 acudmore 
** Reversed "if" clause order to check if array index is out of bounds before using it.
** Revision 1.4 2012/02/08 16:24:11GMT-05:00 lwalling 
** Added test for function result
** Revision 1.3 2012/01/13 11:50:00EST acudmore 
** Changed license text to reflect open source
** Revision 1.2 2009/06/10 09:08:53EDT acudmore 
** Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
** Revision 1.1 2008/04/17 08:05:03EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.2.1.16 2007/07/11 11:42:30EDT David Kobe (dlkobe) 
** Only allocates CDS Registry Record after successful registration
** Revision 1.2.1.15 2007/06/07 09:35:33EDT dlkobe 
** Corrected Error in Delete CDS function
** Revision 1.2.1.14 2007/05/04 15:51:31EDT dlkobe 
** Added CFE_ES_DeleteCDS function to allow TBL and ES to use common code
** Revision 1.2.1.13 2007/04/28 16:08:07EDT dlkobe 
** Corrected yet another file corruption
** Revision 1.2.1.12 2007/04/28 15:57:01EDT dlkobe 
** Added "cfe_es_cds.h" includes to resolve sudden errors.
** Revision 1.2.1.11 2007/04/28 15:50:26EDT dlkobe 
** Fixed typos leftover from restructuring
** Revision 1.2.1.10 2007/04/28 15:48:45EDT dlkobe 
** Correcting Corrupted File
** Revision 1.2.1.9 2007/04/28 15:35:20EDT dlkobe 
** Corrected Typos from restructuring
** Revision 1.2.1.8 2007/04/28 15:32:38EDT dlkobe 
** Restructured CFE_ES_RegisterCDS call to avoid Application Unknown Error
** Revision 1.2.1.7 2007/04/28 15:04:43EDT dlkobe 
** Corrected Variable Name Typo
** Revision 1.2.1.6 2007/04/28 14:49:02EDT dlkobe 
** Baseline Implementation of Critical Tables
** Revision 1.2.1.5 2007/04/26 15:39:35EDT rjmcgraw 
** Added error returns in CFE_ES_CDS_EarlyInit
** Revision 1.2.1.4 2007/04/13 16:24:02EDT apcudmore 
** Changed EarlyInit function to return int32 status
** Revision 1.2.1.3 2007/04/04 16:06:41EDT dlkobe 
** Made numerous changes to correct issues identified during ES code walkthrough
** Revision 1.2.1.2 2006/10/30 15:28:29GMT-05:00 dlkobe 
** Removed extra "PoolSize" variable
** Revision 1.2.1.1 2006/10/30 11:05:04GMT-05:00 dlkobe 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**
*/

/*
** Required header files.
*/
#include "private/cfe_private.h"
#include "cfe_es_apps.h"
#include "cfe_es_cds.h"
#include "cfe_es_global.h"
#include "cfe_psp.h"
#include "cfe_es_cds_mempool.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Define offset addresses for CDS data segments */
/* Each segment is guaranteed to start at a 4-byte offset boundary */
#define CDS_REG_SIZE_OFFSET ((sizeof(CFE_ES_Global.CDSVars.ValidityField)+3) & 0xfffffffc)
#define CDS_REG_OFFSET      (((CDS_REG_SIZE_OFFSET + sizeof(CFE_ES_Global.CDSVars.MaxNumRegEntries)) + 3) & 0xfffffffc)
#define CDS_POOL_OFFSET     (((CDS_REG_OFFSET + (CFE_ES_CDS_MAX_NUM_ENTRIES * sizeof(CFE_ES_CDS_RegRec_t))) + 3) & 0xfffffffc)

/*****************************************************************************/
/**
** \brief Determines whether a CDS currently exists
**
** \par Description
**        Reads a set of bytes from the beginning and end of the CDS memory
**        area and determines if a fixed pattern is present, thus determining
**        whether the CDS still likely contains valid data or not.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS         \copydoc CFE_SUCCESS
** \return #CFE_ES_CDS_INVALID  \copydoc CFE_ES_CDS_INVALID
** \return Any of the return values from #CFE_PSP_ReadFromCDS
**                     
******************************************************************************/
int32 CFE_ES_ValidateCDS(void);

/*****************************************************************************/
/**
** \brief Initializes the contents of the CDS
**
** \par Description
**        Stores a fixed pattern at the beginning and end of the CDS memory
**        to tag it for future verification following a reset.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CDSSize Total size of CDS memory area (in bytes)
**
** \return #OS_SUCCESS          \copydoc OS_SUCCESS
** \return Any of the return values from #CFE_PSP_WriteToCDS
** \return Any of the return values from #CFE_ES_CreateCDSPool
**                     
******************************************************************************/
int32 CFE_ES_InitializeCDS(uint32 CDSSize);

/*****************************************************************************/
/**
** \brief Initializes the CDS Registry
**
** \par Description
**        Initializes the data structure used to keep track of CDS blocks and
**        who they belong to.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS         \copydoc CFE_SUCCESS
**                     
******************************************************************************/
int32 CFE_ES_InitCDSRegistry(void);


/*****************************************************************************/
/**
** \brief Rebuilds memory pool for CDS and recovers existing registry
**
** \par Description
**        Scans memory for existing CDS and initializes memory pool and registry
**        settings accordingly
**
** \par Assumptions, External Events, and Notes:
**        -# Assumes the validity of the CDS has already been determined
**
** \return #CFE_SUCCESS         \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_PSP_ReadFromCDS
**                     
******************************************************************************/
int32 CFE_ES_RebuildCDS(void);

/*****************************************************************************/
/**
** \brief Initializes CDS data constructs
**
** \par Description
**        Locates and validates any pre-existing CDS memory or initializes the
**        memory as a fresh CDS.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \par SysLog Messages
**
**
** \return None
**
******************************************************************************/
int32 CFE_ES_CDS_EarlyInit(void)
{
    uint32  MinRequiredSize = 0;
    int32   Status = CFE_SUCCESS;
    
    /* Compute the minimum size required for the CDS with the current configuration of the cFE */
    MinRequiredSize = (sizeof(CFE_ES_Global.CDSVars.ValidityField) * 2) +            /* Minimum size for validity fields */
                      (CFE_ES_CDS_MAX_NUM_ENTRIES * sizeof(CFE_ES_CDS_RegRec_t)) +   /* Minimum size for CDS Registry contents */
                      CFE_ES_CDSReqdMinSize(CFE_ES_CDS_MAX_NUM_ENTRIES);             /* Max # of Min Sized Blocks */
    
    /* Get CDS size from OS BSP */
    Status = CFE_PSP_GetCDSSize(&CFE_ES_Global.CDSVars.CDSSize);
    
    /* If the size was obtained successfully and meets the minimum requirements, then check its contents */
    if (Status == CFE_PSP_SUCCESS)
    {
        /* Always truncate the size to the nearest 4 byte boundary */
        CFE_ES_Global.CDSVars.CDSSize &= 0xfffffffc;
        
        if (CFE_ES_Global.CDSVars.CDSSize >= MinRequiredSize)
        {
            Status = CFE_ES_ValidateCDS();
            
            /* If the CDS is accessible but invalid, then create a new one */
            if (Status == CFE_ES_CDS_INVALID)
            {
                Status = CFE_ES_InitializeCDS(CFE_ES_Global.CDSVars.CDSSize);
            }
            else if (Status == CFE_SUCCESS)
            {
                /* If a valid CDS was found, rebuild the memory pool */
                Status = CFE_ES_RebuildCDS();
                
                /* If rebuilding is not possible, then create a new one from scratch */
                if (Status == CFE_ES_CDS_INVALID)
                {
                    Status = CFE_ES_InitializeCDS(CFE_ES_Global.CDSVars.CDSSize);
                }
            }
            else /* Unrecoverable error while reading the CDS */
            {
                CFE_ES_WriteToSysLog("CFE_CDS:EarlyInit-Read error validating CDS (Err=0x%08X)\n", (unsigned int)Status);
                CFE_ES_Global.CDSVars.MemPoolSize = 0;
                return Status;
            }

            if (Status == CFE_SUCCESS)
            {
                /* Create CDS registry access mutex */
                OS_MutSemCreate(&CFE_ES_Global.CDSVars.RegistryMutex,
                                 CFE_ES_CDS_MUT_REG_NAME,
                                 CFE_ES_CDS_MUT_REG_VALUE);
            }     
        }
        else /* Size < MinRequiredSize */
        {
            CFE_ES_WriteToSysLog("CFE_CDS:EarlyInit-CDS Size (%d) less than required (%d)\n", 
	                         (int)CFE_ES_Global.CDSVars.CDSSize, (int)MinRequiredSize);
            CFE_ES_Global.CDSVars.MemPoolSize = 0;
            return Status;
        }
    }
    else /* Error getting the size of the CDS from the BSP */
    {
        CFE_ES_WriteToSysLog("CFE_CDS:EarlyInit-Unable to obtain CDS Size from BSP (Err=0x%08X)\n", (unsigned int)Status);
        CFE_ES_Global.CDSVars.MemPoolSize = 0;
        return Status;
    }
    
    
    return(CFE_SUCCESS);
    
}   /* End of CFE_ES_CDS_EarlyInit() */

/*******************************************************************
**
** CFE_ES_RegisterCDSEx
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name, boolean CriticalTbl)
{
    int32   Status = CFE_SUCCESS;
    uint32  RegIndx;
    CFE_ES_CDS_RegRec_t *RegRecPtr = NULL;
    
    /* Lock Registry for update.  This prevents two applications from */
    /* trying to register CDSs at the same location at the same time  */
    CFE_ES_LockCDSRegistry();

    /* Check for duplicate CDS name */
    RegIndx = CFE_ES_FindCDSInRegistry(Name);

    /* Check to see if CDS is already in the registry */
    if (RegIndx != CFE_ES_CDS_NOT_FOUND)
    {
        /* Get pointer to Registry Record Entry to speed up processing */
        RegRecPtr = &CFE_ES_Global.CDSVars.Registry[RegIndx];

        /* If the new CDS is the same size as the old, then no need to reallocate memory */
        if (BlockSize != RegRecPtr->Size)
        {
            /* If the new size is different, the old CDS must be deleted first  */
            /* Free the old memory */
            Status = CFE_ES_PutCDSBlock(RegRecPtr->MemHandle);
        }
        else
        {
            /* Warn calling application that this is a duplicate registration */
            Status = CFE_ES_CDS_ALREADY_EXISTS;
               
            /* Return the index into the registry as the handle to the CDS */
            *HandlePtr = RegIndx;
        }
    }
    else  /* CDS not already in registry */
    {
        /* Locate empty slot in table registry */
        RegIndx = CFE_ES_FindFreeCDSRegistryEntry();
        
        /* Check to make sure we found a free entry in registry */
        if (RegIndx == CFE_ES_CDS_NOT_FOUND)
        {
            Status = CFE_ES_CDS_REGISTRY_FULL;
            CFE_ES_WriteToSysLog("CFE_CDS:Register-Registry full\n");
        }
        else
        {
            RegRecPtr = &CFE_ES_Global.CDSVars.Registry[RegIndx];
        }
    }

    /* If no errors, then initialize the CDS registry entry     */
    if (Status == CFE_SUCCESS)
    {
        /* Allocate the memory buffer for the CDS */
        Status = CFE_ES_GetCDSBlock((CFE_ES_CDSBlockHandle_t *)&RegRecPtr->MemHandle, BlockSize);
            
        if (Status == CFE_SUCCESS)
        {
           RegRecPtr->Taken = TRUE;
        
           /* Save the size of the CDS */
           RegRecPtr->Size = BlockSize;
        
           /* Save flag indicating whether it is a Critical Table or not */
           RegRecPtr->Table = CriticalTbl;

           /* Save CDS Name in Registry */
           strncpy(RegRecPtr->Name, Name, CFE_ES_CDS_MAX_FULL_NAME_LEN);
               
           /* Return the index into the registry as the handle to the CDS */
           *HandlePtr = RegIndx;
        
           /* If we succeeded at creating a CDS, save updated registry in the CDS */
           Status = CFE_ES_UpdateCDSRegistry();
                
           if (Status != CFE_SUCCESS)
           {
              CFE_ES_WriteToSysLog("CFE_CDS:RegCDS-Failed to update CDS Registry (Stat=0x%08X)\n", (unsigned int)Status);
           }
        }
    }

    /* Unlock Registry for update */
    CFE_ES_UnlockCDSRegistry();
    
    return (Status);

}  /* End of CFE_ES_RegisterCDSEx() */

/*******************************************************************
**
** CFE_ES_ValidateCDS
**
** NOTE: For complete prolog information, see prototype above
********************************************************************/

int32 CFE_ES_ValidateCDS(void)
{
    /* Assume the CDS is invalid */
    int32 Status;
    
    /* Perform 2 checks to validate the CDS Memory Pool */
    /* First, determine if the first validity check field is correct */
    Status = CFE_PSP_ReadFromCDS(&CFE_ES_Global.CDSVars.ValidityField, 0, sizeof(CFE_ES_Global.CDSVars.ValidityField));

    if (Status == CFE_PSP_SUCCESS)
    {
        if (strncmp(CFE_ES_Global.CDSVars.ValidityField, "_CDSBeg_", sizeof(CFE_ES_Global.CDSVars.ValidityField)) == 0)
        {
            Status = CFE_PSP_ReadFromCDS(&CFE_ES_Global.CDSVars.ValidityField, 
                                       (CFE_ES_Global.CDSVars.CDSSize-sizeof(CFE_ES_Global.CDSVars.ValidityField)), 
                                       sizeof(CFE_ES_Global.CDSVars.ValidityField));
        
            if (Status == CFE_PSP_SUCCESS)
            {
                if (strncmp(CFE_ES_Global.CDSVars.ValidityField, "_CDSEnd_", sizeof(CFE_ES_Global.CDSVars.ValidityField)) == 0)
                {
                    Status = CFE_SUCCESS;
                }
                else /* Validity Field failed */
                {
                    Status = CFE_ES_CDS_INVALID;
                }
            }
            else /* BSP reported an error reading from CDS */
            {
                CFE_ES_WriteToSysLog("CFE_CDS:Validate-2nd ReadFromCDS Failed. Status=0x%X\n", (unsigned int)Status);
            }
        }
        else /* Validity Field failed */
        {
            Status = CFE_ES_CDS_INVALID;
        }
    }
    else /* BSP reported an error reading from CDS */
    {
        CFE_ES_WriteToSysLog("CFE_CDS:Validate-1st ReadFromCDS Failed. Status=0x%X\n", (unsigned int)Status);
    }
    
    return Status;
}   /* End of CFE_ES_ValidateCDS() */

/*******************************************************************
**
** CFE_ES_InitializeCDS
**
** NOTE: For complete prolog information, see prototype above
********************************************************************/

int32 CFE_ES_InitializeCDS(uint32 CDSSize)
{
    int32  Status = CFE_SUCCESS;
    uint32 MemBlock[32];
    uint32 NumWritten=0;
    uint32 Uint32Zero=0;
    
    /* Clear the CDS to ensure everything is gone */
    /* Create a block of zeros to write to the CDS */
    CFE_PSP_MemSet(MemBlock, 0, sizeof(MemBlock));
    
    /* While there is space to write another block of zeros, then do so */
    while (((NumWritten + sizeof(MemBlock)) < CDSSize) && (Status == OS_SUCCESS))
    {
        Status = CFE_PSP_WriteToCDS(MemBlock, NumWritten, sizeof(MemBlock));
        
        if (Status == CFE_PSP_SUCCESS)
        {
            NumWritten += sizeof(MemBlock);
        }
    }
    
    /* While there is space to write a uint32 of zeros, then do so */
    if ((Status == CFE_PSP_SUCCESS) && (NumWritten < CDSSize))
    {
        while (((NumWritten + sizeof(uint32)) < CDSSize) && (Status == CFE_PSP_SUCCESS))
        {
            Status = CFE_PSP_WriteToCDS(&Uint32Zero, NumWritten, sizeof(uint32));
            
            if (Status == CFE_PSP_SUCCESS)
            {
                NumWritten += sizeof(uint32);
            }
        }
    }

    if (Status != CFE_PSP_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CFE_CDS:Init-Clear CDS failed @ Offset=%d Status=0x%08X\n", (int)NumWritten, (unsigned int)Status);
    }
    else
    {
        /* Initialize the Validity Check strings */
        Status = CFE_PSP_WriteToCDS("_CDSBeg_", 0, sizeof(CFE_ES_Global.CDSVars.ValidityField));
        if (Status == CFE_PSP_SUCCESS)
        {
            Status = CFE_PSP_WriteToCDS("_CDSEnd_", 
                                      (CDSSize-sizeof(CFE_ES_Global.CDSVars.ValidityField)), 
                                      sizeof(CFE_ES_Global.CDSVars.ValidityField));
            if (Status != CFE_PSP_SUCCESS)
            {
                CFE_ES_WriteToSysLog("CFE_CDS:Init-'_CDSEnd_' write failed. Status=0x%08X\n", (unsigned int)Status);
            }
            else
            {
                /* Determine the amount of CDS memory is available for the CDS Memory Pool */
                /* Total CDS Size minus the validity fields and the space for the registry and the registry size */
                if (CDSSize < (CDS_POOL_OFFSET + sizeof(CFE_ES_Global.CDSVars.ValidityField)))
                {
                    /* Invalid - passing zero will cause the CFE_ES_CreateCDSPool() to fail */
                    CFE_ES_Global.CDSVars.MemPoolSize = 0;
                }
                else
                {
                    CFE_ES_Global.CDSVars.MemPoolSize = CDSSize - CDS_POOL_OFFSET - sizeof(CFE_ES_Global.CDSVars.ValidityField);
                }

                /* Initialize the variables for managing the CDS Memory Pool */
                Status = CFE_ES_CreateCDSPool(CFE_ES_Global.CDSVars.MemPoolSize, CDS_POOL_OFFSET);
                
                if (Status == CFE_SUCCESS)
                {
                    Status = CFE_ES_InitCDSRegistry();
                }
            }
        }
        else /* BSP reported an error writing to CDS */
        {
            CFE_ES_WriteToSysLog("CFE_CDS:Init-'_CDSBeg_' write failed. Status=0x%08X\n", (unsigned int)Status);
        }
    }
    
    return Status;
}   /* End of CFE_ES_InitializeCDS() */

/*******************************************************************
**
** CFE_ES_InitCDSRegistry
**
** NOTE: For complete prolog information, see prototype above
********************************************************************/

int32 CFE_ES_InitCDSRegistry(void)
{
    int32 Status = CFE_SUCCESS;
    uint32 i = 0;
    
    /* Initialize the local CDS Registry */
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES;
    for (i=0; i<CFE_ES_Global.CDSVars.MaxNumRegEntries; i++)
    {
        CFE_ES_Global.CDSVars.Registry[i].Name[0] = '\0';
        CFE_ES_Global.CDSVars.Registry[i].Size = 0;
        CFE_ES_Global.CDSVars.Registry[i].MemHandle = 0;
        CFE_ES_Global.CDSVars.Registry[i].Taken = FALSE;
        CFE_ES_Global.CDSVars.Registry[i].Table = FALSE;
    }
    
    /* Copy the number of registry entries to the CDS */
    Status = CFE_PSP_WriteToCDS(&CFE_ES_Global.CDSVars.MaxNumRegEntries, 
                               CDS_REG_SIZE_OFFSET, 
                               sizeof(CFE_ES_Global.CDSVars.MaxNumRegEntries));
    
    if (Status == CFE_PSP_SUCCESS)
    {
        Status = CFE_ES_UpdateCDSRegistry();
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_CDS:InitReg-Failed to write Reg Size. Status=0x%08X\n", (unsigned int)Status);
    }
    
    return Status;
}   /* End of CFE_ES_InitCDSRegistry() */

/*******************************************************************
**
** CFE_ES_UpdateCDSRegistry
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_UpdateCDSRegistry(void)
{
    int32 Status = CFE_SUCCESS;
    
    /* Copy the contents of the local registry to the CDS */
    Status = CFE_PSP_WriteToCDS(CFE_ES_Global.CDSVars.Registry, 
                              CDS_REG_OFFSET,
                              sizeof(CFE_ES_Global.CDSVars.Registry));
    
    if (Status != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CFE_CDS:UpdateReg-Failed to write CDS Registry. Status=0x%08X\n", (unsigned int)Status);
    }
    
    return Status;
}

/*******************************************************************
**
** CFE_ES_CDS_ValidateAppID
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_CDS_ValidateAppID(uint32 *AppIdPtr)
{
    int32 Status = CFE_ES_GetAppID(AppIdPtr);

    if (Status == CFE_SUCCESS)
    {
        if (*AppIdPtr >= CFE_ES_MAX_APPLICATIONS)
        {
            Status = CFE_ES_ERR_APPID;

            CFE_ES_WriteToSysLog("CFE_CDS:ValidateAppID-AppId=%d > Max Apps (%d)\n",
                                 (int)(*AppIdPtr), CFE_ES_MAX_APPLICATIONS);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_CDS:ValidateAppID-GetAppID failed (Stat=0x%08X)\n", (unsigned int)Status);
    }

    return Status;
}   /* End of CFE_ES_CDS_ValidateAppID() */


/*******************************************************************
**
** CFE_ES_FormCDSName
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

void CFE_ES_FormCDSName(char *FullCDSName, const char *CDSName, uint32 ThisAppId)
{
    char AppName[OS_MAX_API_NAME];

    CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

    /* Ensure that AppName is null terminated */
    AppName[OS_MAX_API_NAME-1] = '\0';

    /* Complete formation of processor specific table name */
    sprintf(FullCDSName, "%s.%s", AppName, CDSName);

    return;
}   /* End of CFE_ES_FormCDSName() */


/*******************************************************************
**
** CFE_ES_LockCDSRegistry
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_LockCDSRegistry(void)
{
    int32   Status;

    Status = OS_MutSemTake(CFE_ES_Global.CDSVars.RegistryMutex);

    if (Status == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }

    return Status;

}   /* End of CFE_ES_LockCDSRegistry() */


/*******************************************************************
**
** CFE_ES_UnlockCDSRegistry
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_UnlockCDSRegistry(void)
{
    int32   Status;

    Status = OS_MutSemGive(CFE_ES_Global.CDSVars.RegistryMutex);

    if (Status == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }

    return Status;

}   /* End of CFE_ES_UnlockCDSRegistry() */


/*******************************************************************
**
** CFE_ES_FindCDSInRegistry
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_FindCDSInRegistry(const char *CDSName)
{
    int32 RegIndx = CFE_ES_CDS_NOT_FOUND;
    int32 i = -1;

    do
    {
        /* Point to next record in the CDS Registry */
        i++;

        /* Check to see if the record is currently being used */
        if (CFE_ES_Global.CDSVars.Registry[i].Taken == TRUE)
        {
            /* Perform a case sensitive name comparison */
            if (strcmp(CDSName, CFE_ES_Global.CDSVars.Registry[i].Name) == 0)
            {
                /* If the names match, then return the index */
                RegIndx = i;
            }
        }
    } while ( (RegIndx == CFE_ES_CDS_NOT_FOUND) && (i < (CFE_ES_Global.CDSVars.MaxNumRegEntries-1)) );

    return RegIndx;
}   /* End of CFE_ES_FindCDSInRegistry() */


/*******************************************************************
**
** CFE_ES_FindFreeCDSRegistryEntry
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_FindFreeCDSRegistryEntry(void)
{
    int32 RegIndx = CFE_ES_CDS_NOT_FOUND;
    int32 i = 0;

    while ( (RegIndx == CFE_ES_CDS_NOT_FOUND) && (i < CFE_ES_Global.CDSVars.MaxNumRegEntries) )
    {
        if (CFE_ES_Global.CDSVars.Registry[i].Taken == FALSE)
        {
            RegIndx = i;
        }
        else
        {
            i++;
        }
    }

    return RegIndx;
}   /* End of CFE_ES_FindFreeCDSRegistryEntry() */


/*******************************************************************
**
** CFE_ES_RebuildCDS
**
** NOTE: For complete prolog information, see above
********************************************************************/

int32 CFE_ES_RebuildCDS(void)
{
    int32 Status = CFE_SUCCESS;
    int32 PoolOffset;
    
    /* First, determine if the CDS registry stored in the CDS is smaller or equal */
    /* in size to the CDS registry we are currently configured for                */
    /* Copy the number of registry entries to the CDS */
    Status = CFE_PSP_ReadFromCDS(&CFE_ES_Global.CDSVars.MaxNumRegEntries, 
                               CDS_REG_SIZE_OFFSET, 
                               sizeof(CFE_ES_Global.CDSVars.MaxNumRegEntries));
                               
    if ((Status == CFE_PSP_SUCCESS)  &&
        (CFE_ES_Global.CDSVars.MaxNumRegEntries <= CFE_ES_CDS_MAX_NUM_ENTRIES))
    {
        Status = CFE_PSP_ReadFromCDS(&CFE_ES_Global.CDSVars.Registry,
                                   CDS_REG_OFFSET,
                                   (CFE_ES_Global.CDSVars.MaxNumRegEntries * sizeof(CFE_ES_CDS_RegRec_t)));
                            
        if (Status == CFE_PSP_SUCCESS)
        {
            /* Calculate the starting offset of the memory pool */
            PoolOffset = (CDS_REG_OFFSET + (CFE_ES_Global.CDSVars.MaxNumRegEntries * sizeof(CFE_ES_CDS_RegRec_t)) + 3) & 0xfffffffc;;

            /* Calculate the size of the memory pool */
            CFE_ES_Global.CDSVars.MemPoolSize = CFE_ES_Global.CDSVars.CDSSize - PoolOffset - sizeof(CFE_ES_Global.CDSVars.ValidityField);
            
            /* Scan the memory pool and identify the created but currently unused memory blocks */
            Status = CFE_ES_RebuildCDSPool(CFE_ES_Global.CDSVars.MemPoolSize, PoolOffset);
        }
        else
        {
            /* Registry in CDS is unreadable */
            Status = CFE_ES_CDS_INVALID;
            CFE_ES_WriteToSysLog("CFE_CDS:Rebuild-Registry in CDS is unreadable\n");
        }
    }
    else
    {
        /* Registry in CDS is too large to recover */
        Status = CFE_ES_CDS_INVALID;
        CFE_ES_WriteToSysLog("CFE_CDS:Rebuild-Registry in CDS too large to recover\n");
    }
           
    return Status;    
}


/*******************************************************************
**
** CFE_ES_DeleteCDS
**
** NOTE: For complete prolog information, see 'cfe_es_cds.h'
********************************************************************/

int32 CFE_ES_DeleteCDS(const char *CDSName, boolean CalledByTblServices)
{
    int32                Status;
    int32                RegIndx;
    CFE_ES_CDS_RegRec_t *RegRecPtr = NULL;
    char                 OwnerName[OS_MAX_API_NAME];
    uint32               AppId;
    uint32               i;
    
    /* Lock Registry for update.  This prevents two applications from */
    /* trying to change the CDS registry at the same time  */
    CFE_ES_LockCDSRegistry();

    /* Find CDS name in registry */
    RegIndx = CFE_ES_FindCDSInRegistry(CDSName);

    /* Check to see if CDS is already in the registry */
    if (RegIndx != CFE_ES_CDS_NOT_FOUND)
    {
        /* Get pointer to Registry Record Entry to speed up processing */
        RegRecPtr = &CFE_ES_Global.CDSVars.Registry[RegIndx];
        
        /* Critical tables are not allowed to be deleted via an ES Command. */
        /* They must be deleted by a Table Services Command */
        if (RegRecPtr->Table != CalledByTblServices)
        {
            Status = CFE_ES_CDS_WRONG_TYPE_ERR;
        }
        else
        {
            /* Check to see if the owning application is still active */
            /* First, extract the owning application name */
            i=0;
            while ((i < (OS_MAX_API_NAME-1) && (RegRecPtr->Name[i] != '.')))
            {
                OwnerName[i] = RegRecPtr->Name[i];
                i++;
            }
            
            /* Null terminate the application name */
            OwnerName[i] = '\0';
            
            /* Check to see if the Application Name is in the Registered Apps list */
            Status = CFE_ES_GetAppIDByName(&AppId, OwnerName);
            
            /* If we can't find the name, then things are good */
            if (Status == CFE_ES_ERR_APPNAME)
            {
                /* Free the registry entry and the CDS memory block associated with it */
                Status = CFE_ES_PutCDSBlock(RegRecPtr->MemHandle);
    
                /* Report any errors incurred while freeing the CDS Memory Block */
                if (Status < 0)
                {
                    CFE_ES_WriteToSysLog("CFE_ES:DeleteCDS-Failed to free CDS Mem Block (Handle=0x%08lX)(Stat=0x%08X)\n",
                            (unsigned long)RegRecPtr->MemHandle, (unsigned int)Status);
                }
                else
                {
                    /* Remove entry from the CDS Registry */
                    RegRecPtr->Taken = FALSE;
        
                    Status = CFE_ES_UpdateCDSRegistry();
            
                    if (Status != CFE_SUCCESS)
                    {
                        CFE_ES_WriteToSysLog("CFE_ES:DeleteCDS-Failed to update CDS Registry (Stat=0x%08X)\n", (unsigned int)Status);
                    }
                }
            }
            else
            {
                Status = CFE_ES_CDS_OWNER_ACTIVE_ERR;
            }
        }
    }
    else  /* Error - CDS not in registry */
    {
        Status = CFE_ES_CDS_NOT_FOUND_ERR;
    }

    /* Unlock Registry for future updates */
    CFE_ES_UnlockCDSRegistry();

    return Status;
}   /* End of CFE_ES_DeleteCDS() */

/* end of file */
