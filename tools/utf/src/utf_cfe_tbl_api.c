/*
** File: utf_cfe_tbl_api.c
** $Id: utf_cfe_tbl_api.c 1.9 2012/01/13 12:52:00GMT-05:00 acudmore Exp  $
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
** Purpose:  cFE Table Services (TBL) library API source file
**
** Author:   D. Kobe/the Hammers Company, Inc.
**
** Notes:    
**
** $Log: utf_cfe_tbl_api.c  $
** Revision 1.9 2012/01/13 12:52:00GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.8 2010/10/25 15:06:36EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.7 2010/10/04 14:57:37EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.6 2009/06/24 09:18:13EDT rmcgraw 
** DCR8290:4 OS_BSP to CFE_PSP_ and OS_Mem to CFE_PSP_Mem changes
** Revision 1.5 2008/09/08 10:28:50EDT wfmoleski 
** Originally, the CFE_TBL_Manage was removed from the CFE_TBL_Load function because it appeared
** to not make any sense why this would occur. However, CFS Checksum unit testing verified that this
** call was needed to simulate the fsw effects of a table load. So, this call was added back in with a
** test to return CFE_SUCCESS if CFE_TBL_Manage returns CFE_TBL_INFO_UPDATED.
** Revision 1.4 2008/09/04 07:31:12EDT wfmoleski 
** Code that called CFE_TBL_Manage at the bottom of CFE_TBL_Load was commented out since this was an 
** incorrect implementation and caused unit test problems.
** Revision 1.3 2008/07/30 13:05:05EDT wfmoleski 
** Several Table Services modifications affected the UTF including a new API. The test16 example was 
** modified to execute this new API.
** Revision 1.2 2008/07/18 10:22:26EDT wfmoleski 
** The first argument of the CFE_TBL_Register function hook implemented in the UTF does not match
** the fsw. This was changed so that they match.
** Revision 1.1 2008/04/17 08:09:38EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.11 2008/04/07 09:39:35EDT wfmoleski 
** Added the capability for the specified hook functions. Added return code setting code for several
** OS functions that seemed appropriate.
** Revision 1.10 2007/09/24 07:51:51EDT wfmoleski 
** Made changes to UTF source to reflect the API change to GetPoolBuf in the cFE fsw.
** Revision 1.9 2007/06/07 11:54:15EDT wfmoleski 
** Changes due to a problem found & fixed in the fsw with registering critical tables.
** Revision 1.8 2007/05/25 11:07:49EDT wfmoleski 
** Modification to add Critical Table support as well as other fsw changes for cFE 4.1.0.
** Revision 1.7 2006/11/09 08:36:01EST wfmoleski 
** UTF modifications for cFE 4.0.
** Revision 1.6 2006/10/03 10:54:33EDT wfmoleski 
** Changed the event in question from Info to Error.
** Revision 1.5 2006/06/22 11:04:58EDT wfmoleski 
** Added the CFE_TBL_GetInfo implementation and modifications to reflect the fsw code changes. This included ES_GetBufPool argument changes.
** Revision 1.4 2006/06/12 16:41:06EDT nsschweiss 
** Removed debug messages.
** Revision 1.3 2006/05/26 15:04:30EDT nsschweiss 
** Changes for UTF 3.2.
** Revision 1.2 2006/05/09 14:42:30EDT nsschweiss 
** Modified to use up-to-date macro names.
** Modified as needed to use new Load Buffer type.
** Replaced comment syntax with ansi C comment syntax.
** Revision 1.1 2006/03/21 16:44:28EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.5 2005/12/20 16:59:55EST NSSCHWEISS 
** Extended capability to revert api calls to default return codes from a procedure call in the test driver.  Also expanded capability to cover 3 newest TBL api calls.
** Revision 1.4 2005/12/19 13:02:34EST NSSCHWEISS 
** Fix handling of validation callback functions.
** Revision 1.3 2005/12/01 20:19:17EST NSSCHWEISS 
** Commented out invalid call to validation callback function.
** Revision 1.2 2005/12/01 18:49:08EST NSSCHWEISS 
** utf 2.3.0.
** Revision 1.1 2005/11/16 16:26:35EST NSSCHWEISS 
** Initial revision
** Member added to project d:/mksdata/MKS-LRO-REPOSITORY/utf/src_utf/project.pj
** Starting point is August 31 preliminary code from D. Kobe.
** 09/20/05 nschweiss Updating for UTF purposes.
** 09/21/05 nschweiss Changed OS_MemCpy call in Load to memcpy.
** 09/22/05 nschweiss Allocated table buffers for double bufferred tables
**          in Register().
** 09/23/05 nschweiss Modify Register so that same actions are performed
**      for single and double buffered tables.
** 09/27/05 nschweiss In CFE_TBL_LoadWorkingBuffer(), after buffer is 
**             written to, set notification bits.
** 09/28/05 nschweiss In CFE_TBL_Update(), set & clear notification bits.
** 09/28/05 nschweiss   Collapsed TBL_UpdateInternal into TBL_Update.
**       Changed UpdateInternal call in Load() to Update() so that
**       status bits would be set.
** 09/28/05 nschweiss Revised Load() to call LoadWorkingBuffer so that
**   notify bits are set.
** 09/29/05 nschweiss Collapsed GetWorkingBuffer into LoadWorkingBuffer.
**   Augmented LoadWorkingBuffer to set TableLoadedOnce flag;
**   augmented Update to warn programmer if table has never been loaded.
** 10/19/05 nschweiss Changed name of 1st argument in UTF_TBL_LoadTableFromGround
**          to Full_Table_Name for clarity.
** 11/28/05 nschweiss Added CFE_TBL_Manage(), GetStatus(), CFE_TBL_Validate().
*/

/*
** Required header files...
*/
#include "cfe_platform_cfg.h"
#include "cfe.h"
#include "cfe_tbl.h"
#include "cfe_error.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_task.h"
#include "cfe_es_cds.h"
#include "string.h"
#include "utf_types.h"
#include "utf_cfe.h"

/*
** External Globals
*/
extern CFE_TBL_TaskData_t CFE_TBL_TaskData;

/* Function Hook Stuff */
typedef struct
{
    int32 (*CFE_TBL_Register)(CFE_TBL_Handle_t*, const char *,uint32, uint16, CFE_TBL_CallbackFuncPtr_t);
    int32 (*CFE_TBL_Load)(CFE_TBL_Handle_t, CFE_TBL_SrcEnum_t, const void *);
    int32 (*CFE_TBL_Manage)(CFE_TBL_Handle_t);
    int32 (*CFE_TBL_GetAddress)(void **, CFE_TBL_Handle_t);
    int32 (*CFE_TBL_GetAddresses)(void **[], uint16, const CFE_TBL_Handle_t []);
} UTF_TBL_HookTable_t;

UTF_TBL_HookTable_t UTF_TBL_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
/*
** Local Data
*/
int32 cfe_tbl_api_return_value[NUM_OF_CFE_TBL_API_PROCS]=
 {
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE
 };

/******************************************************************************
**  Function: UTF_TBL_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a SB function call.
*/
void UTF_TBL_set_function_hook(int Index, void *FunPtr)
{
    if      (Index == CFE_TBL_REGISTER_HOOK)     { UTF_TBL_HookTable.CFE_TBL_Register = FunPtr; }
    else if (Index == CFE_TBL_LOAD_HOOK)         { UTF_TBL_HookTable.CFE_TBL_Load = FunPtr; }
    else if (Index == CFE_TBL_MANAGE_HOOK)       { UTF_TBL_HookTable.CFE_TBL_Manage = FunPtr; }
    else if (Index == CFE_TBL_GETADDRESS_HOOK)   { UTF_TBL_HookTable.CFE_TBL_GetAddress = FunPtr; }
    else if (Index == CFE_TBL_GETADDRESSES_HOOK) { UTF_TBL_HookTable.CFE_TBL_GetAddresses = FunPtr; }
    else                                         { UTF_error("Invalid TBL Hook Index In Set Hook Call %d", Index); }
}

/*******************************************************************
**                                                                 
** CFE_TBL_Register() -- Register a table with cFE to obtain       
**                       Table Management Services                 
**
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_Register( CFE_TBL_Handle_t *TblHandlePtr,
                        const char *Name,
                        uint32  Size,
                        uint16  TblOptionFlags,
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr )
{
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    CFE_TBL_RegistryRec_t      *RegRecPtr = NULL;
    CFE_TBL_CritRegRec_t       *CritRegRecPtr = NULL;
    CFE_TBL_LoadBuff_t         *WorkingBufferPtr;
    int32  Status = CFE_SUCCESS;
    int32  NameLen = 0;
    int16  RegIndx = -1;
    uint32 ThisAppId;
    char   AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    char   TblName[CFE_TBL_MAX_FULL_NAME_LEN] = {""};
    char   LogMsg[128];

   /* Handle Function Hook */
   if (UTF_TBL_HookTable.CFE_TBL_Register)
      return(UTF_TBL_HookTable.CFE_TBL_Register(TblHandlePtr, Name, Size, TblOptionFlags,TblValidationFuncPtr));
    
   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_REGISTER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_REGISTER_PROC];
   }

    /* Check to make sure calling application is legit */
    Status = CFE_TBL_ValidateAppID(&ThisAppId);
     
    /* Check input parameters */
    if (Status == CFE_SUCCESS)
    {
        /* Assume we can't make a table and return a bad handle for now */
        *TblHandlePtr = CFE_TBL_BAD_TABLE_HANDLE; 
    
        /* Make sure specified table name is not too long or too short */
        NameLen = strlen(Name);
        if ((NameLen > CFE_TBL_MAX_NAME_LENGTH) || (NameLen == 0))
        {
            Status = CFE_TBL_ERR_INVALID_NAME;

            /* Perform a buffer overrun safe copy of name for debug log message */
            strncpy(LogMsg, "CFE_TBL:Register-Table Name (", 29);
            strncpy(&LogMsg[29], Name, 70);
            strcat(LogMsg, ") is too long\n");
            CFE_ES_WriteToSysLog(LogMsg);

            strncpy(TblName, Name, CFE_TBL_MAX_NAME_LENGTH);
            TblName[CFE_TBL_MAX_NAME_LENGTH-1] = '\0';
        }
        else
        {
            /* Modify specified name to be processor specific name */
            /* of the form "AppName.TableName"                     */
            CFE_TBL_FormTableName(TblName, Name, ThisAppId);

           /* Make sure the specified size is acceptable */
            /* Single buffered tables are allowed to be up to CFE_TBL_MAX_SNGL_TABLE_SIZE */
            /* Double buffered tables are allowed to be up to CFE_TBL_MAX_DBL_TABLE_SIZE  */
            if (Size == 0)
            {
                Status = CFE_TBL_ERR_INVALID_SIZE;

                CFE_ES_WriteToSysLog("CFE_TBL:Register-Table %s has size of zero\n", Name);
            }
            else if ((Size > CFE_TBL_MAX_SNGL_TABLE_SIZE) &&
                     ((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_SNGL_BUFFER))
            {
                Status = CFE_TBL_ERR_INVALID_SIZE;

                CFE_ES_WriteToSysLog("CFE_TBL:Register-Single Buffered Table '%s' has size %d > %d\n",
                                     Name, Size, CFE_TBL_MAX_SNGL_TABLE_SIZE);
        		}
            else if ((Size > CFE_TBL_MAX_DBL_TABLE_SIZE) &&
                     ((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER))
            {
                Status = CFE_TBL_ERR_INVALID_SIZE;

                CFE_ES_WriteToSysLog("CFE_TBL:Register-Dbl Buffered Table '%s' has size %d > %d\n",
                                     Name, Size, CFE_TBL_MAX_DBL_TABLE_SIZE);
            }
            
            /* Verify Table Option settings are legal */
            /* User defined table addresses are only legal for single buffered, dump-only, non-critical tables */
            if ((TblOptionFlags & CFE_TBL_OPT_USR_DEF_MSK) == CFE_TBL_OPT_USR_DEF_ADDR)
            {
                if (((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER) ||
                    ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_LOAD_DUMP) ||
                    ((TblOptionFlags & CFE_TBL_OPT_CRITICAL_MSK) == CFE_TBL_OPT_CRITICAL))
                {
                    Status = CFE_TBL_ERR_INVALID_OPTIONS;
                    
                    CFE_ES_WriteToSysLog("CFE_TBL:Register-User Def tbl '%s' cannot be dbl buff, load/dump or critical\n",
                                         Name);
                }
            }
            else if ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_DUMP_ONLY)
            {
                /* Dump Only tables cannot be double buffered */
                if ((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER)
                {
                    Status = CFE_TBL_ERR_INVALID_OPTIONS;
                    
                    CFE_ES_WriteToSysLog("CFE_TBL:Register-Dump Only tbl '%s' cannot be double buffered\n",
                                         Name);
                }
            }
        }
    }
    else  /* Application ID was invalid */
    {
        CFE_ES_WriteToSysLog("CFE_TBL:Register-Bad AppId(%d)\n", ThisAppId);
    }
    
    /* If input parameters appear acceptable, register the table */
    if (Status == CFE_SUCCESS)
    {
        /* Lock Registry for update.  This prevents two applications from        */
        /* trying to register/share tables at the same location at the same time */
        /*CFE_TBL_LockRegistry();*/
        
        /* Check for duplicate table name */
        RegIndx = CFE_TBL_FindTableInRegistry(TblName);
 /*       UTF_put_text("CFE_TBL_FindTableInRegistry returned %d\n",RegIndx);*/
        
        /* Check to see if table is already in the registry */
        if (RegIndx != CFE_TBL_NOT_FOUND)
        {
            /* Get pointer to Registry Record Entry to speed up processing */
            RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndx];
            
            /* If this app previously owned the table, then allow them to re-register */
            if (RegRecPtr->OwnerAppId == ThisAppId)
            {  /* same owner */
                /* If the new table is the same size as the old, then no need to reallocate memory */
                if (Size != RegRecPtr->Size)
                {  /* new size */
                    /* If the new size is different, the old table must deleted      */
                    /* but this function can't do that because it is probably shared */
                    /* and is probably still being accessed.  Someone else will need */
                    /* to clean up this mess.                                        */
                    Status = CFE_TBL_ERR_DUPLICATE_DIFF_SIZE;

                    CFE_ES_WriteToSysLog("CFE_TBL:Register-Attempt to register existing table ('%s') with different size(%d!=%d)\n",
                                         TblName, Size, RegRecPtr->Size);
                }
                else
                {
                    /* Warn calling application that this is a duplicate registration */
                    Status = CFE_TBL_WARN_DUPLICATE;
                }
            }
            else /* Duplicate named table owned by another Application */
            {
                Status = CFE_TBL_ERR_DUPLICATE_NOT_OWNED;

                CFE_ES_WriteToSysLog("CFE_TBL:Register-App(%d) Registering Duplicate Table '%s' owned by App(%d)\n",
                                     ThisAppId, TblName, RegRecPtr->OwnerAppId);
            }
        } 
        else  /* Table not already in registry */
        {
    	   UTF_put_text("UTF INFO: CFE_TBL_Register called for new table %s.\n", &TblName);
        	
            /* Locate empty slot in table registry */
            RegIndx = CFE_TBL_FindFreeRegistryEntry();
 /*           UTF_put_text("CFE_TBL_FindFreeRegistryEntry returned %d\n",RegIndx);*/
        }
        
        /* Check to make sure we found a free entry in registry */
        if (RegIndx == CFE_TBL_NOT_FOUND)
        {
            Status = CFE_TBL_ERR_REGISTRY_FULL;
            CFE_ES_WriteToSysLog("CFE_TBL:Register-Registry full\n");
        }

        /* Search Access Descriptor Array for free Descriptor */
        *TblHandlePtr = CFE_TBL_FindFreeHandle();
        
        /* Check to make sure there was a handle available */
        if (*TblHandlePtr == CFE_TBL_END_OF_LIST)
        {
            Status = CFE_TBL_ERR_HANDLES_FULL;
            CFE_ES_WriteToSysLog("CFE_TBL:Register-No more free handles\n");
        }

        /* If no errors, then initialize the table registry entry     */
        /* and return the registry index to the caller as the handle  */
        if ((Status & CFE_SEVERITY_BITMASK) != CFE_SEVERITY_ERROR)
        {
            /* Get pointer to Registry Record Entry to speed up processing */
            RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndx];
            
            /* Initialize Registry Record to default settings 
            CFE_TBL_InitRegistryRecord(RegRecPtr); */

            if ((TblOptionFlags & CFE_TBL_OPT_USR_DEF_MSK) != CFE_TBL_OPT_USR_DEF_ADDR)
            {
                RegRecPtr->UserDefAddr = FALSE;
                
                /* Allocate the memory buffer(s) for the table and inactive table, if necessary */
                Status = CFE_ES_GetPoolBuf((uint32 **)&RegRecPtr->Buffers[0].BufferPtr,
                                           CFE_TBL_TaskData.Buf.PoolHdl,                                           
                                           Size);
                if(Status < 0)
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:Register-1st Buf Alloc GetPool fail Stat=0x%08X MemPoolHndl=0x%08X\n",
                                         Status, CFE_TBL_TaskData.Buf.PoolHdl);
                }
                else
                {
                    /* Zero the memory buffer */
                    Status = CFE_SUCCESS;
                    CFE_PSP_MemSet(RegRecPtr->Buffers[0].BufferPtr, 0x0, Size);
                }
            }
            else
            {
                /* Zero the memory buffer */
                Status = CFE_SUCCESS;
                CFE_PSP_MemSet(RegRecPtr->Buffers[0].BufferPtr, 0x0, Size);
            }

            /* Save the size of the table */
            RegRecPtr->Size = Size;
                     
            /* if ((TblOptionFlags & CFE_TBL_OPT_DBL_BUFFER) == CFE_TBL_OPT_DBL_BUFFER) */
            if (1) /* Treat within UTF as double-buffered no matter how specified */
                   /* This makes code simpler and is transparent to tester */
            {
                /* Allocate memory for the dedicated secondary buffer */
                Status = CFE_ES_GetPoolBuf((uint32 **)&RegRecPtr->Buffers[1].BufferPtr,
                                           CFE_TBL_TaskData.Buf.PoolHdl,                                           
                                           Size);
                if(Status < 0)
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:Register-2nd Buf Alloc GetPool fail Stat=0x%08X MemPoolHndl=0x%08X\n",
                                         Status, CFE_TBL_TaskData.Buf.PoolHdl);
                }
                else
                {
                    /* Zero the dedicated secondary buffer */
                    Status = CFE_SUCCESS;
                    CFE_PSP_MemSet(RegRecPtr->Buffers[1].BufferPtr, 0x0, Size);
                }

                RegRecPtr->ActiveBufferIndex = 0;
                 RegRecPtr->DblBuffered = TRUE;
             }
            else  /* Single Buffered Table */
            {
                RegRecPtr->DblBuffered = FALSE;
                RegRecPtr->ActiveBufferIndex = 0;
            }
            
            /* Save the size of the table */
            RegRecPtr->Size = Size;
                       
            /* Save the Callback function pointer */
            RegRecPtr->ValidationFuncPtr = TblValidationFuncPtr;
            
            /* Initialize other fields in Registry */
            strncpy(RegRecPtr->Name, TblName, CFE_TBL_MAX_FULL_NAME_LEN);
            RegRecPtr->LoadPending = FALSE;
            RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
            RegRecPtr->TableLoadedOnce = FALSE;
            
            /* Set the "Dump Only" flag to value based upon selected option */
            if ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_DUMP_ONLY)
            {
                RegRecPtr->DumpOnly = TRUE;
            }
            else
            {
                RegRecPtr->DumpOnly = FALSE;
            }
            
            /* Initialize the Table Access Descriptor */
            AccessDescPtr = &CFE_TBL_TaskData.Handles[*TblHandlePtr];
            
            AccessDescPtr->AppId = ThisAppId;
            AccessDescPtr->LockFlag = FALSE;
            AccessDescPtr->Updated = FALSE;
            RegRecPtr->ValidateInactiveIndex = CFE_TBL_NO_VALIDATION_PENDING;
            RegRecPtr->ValidateActiveIndex = CFE_TBL_NO_VALIDATION_PENDING;

            if ((RegRecPtr->DumpOnly) && (!RegRecPtr->UserDefAddr))
            {
                /* Dump Only Tables are assumed to be loaded at all times    */
                /* unless the address is specified by the application. In    */
                /* that case, it isn't loaded until the address is specified */
                RegRecPtr->TableLoadedOnce = TRUE;
            }

            AccessDescPtr->RegIndex = RegIndx;
            
            AccessDescPtr->PrevLink = CFE_TBL_END_OF_LIST;         /* We are the head of the list */
            AccessDescPtr->NextLink = CFE_TBL_END_OF_LIST;         /* We are the end of the list */
            
            AccessDescPtr->UsedFlag = TRUE;
            
            /* Make sure the Table Registry entry points to First Access Descriptor */
            RegRecPtr->HeadOfAccessList = *TblHandlePtr;

            /* If the table is a critical table, allocate space for it in the Critical Data Store */
            /* OR locate its previous incarnation there and extract its previous contents */
            if ((TblOptionFlags & CFE_TBL_OPT_CRITICAL_MSK) == CFE_TBL_OPT_CRITICAL)
            {
                /* Register a CDS under the table name and determine if the table already exists there */
                Status = CFE_ES_RegisterCDSEx(&RegRecPtr->CDSHandle, Size, TblName, TRUE);
                
                if (Status == CFE_ES_CDS_ALREADY_EXISTS)
                {
/*                    CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, TRUE); */
                    Status = CFE_ES_RestoreFromCDS(WorkingBufferPtr->BufferPtr, RegRecPtr->CDSHandle);
                        
                    if (Status != CFE_SUCCESS)
                    {
                        /* If an error occurred while trying to get the previous contents, */
                        /* Log the error in the System Log and pretend like we created a new CDS */
                        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
                        CFE_ES_WriteToSysLog("CFE_TBL:Register-Failed to recover '%s.%s' from CDS (ErrCode=0x%08X)\n",
                                             AppName, Name, Status);
                        Status = CFE_SUCCESS;     
                    }
                    else
                    {
                        /* Try to locate the associated information in the Critical Table Registry */
                        CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, RegRecPtr->CDSHandle);
                            
                        if (CritRegRecPtr != NULL)
                        {
                            strncpy(WorkingBufferPtr->DataSource, CritRegRecPtr->LastFileLoaded, OS_MAX_PATH_LEN);
                            WorkingBufferPtr->FileCreateTimeSecs = CritRegRecPtr->FileCreateTimeSecs;
                            WorkingBufferPtr->FileCreateTimeSubSecs = CritRegRecPtr->FileCreateTimeSubSecs;
                            RegRecPtr->TimeOfLastUpdate.Seconds = CritRegRecPtr->TimeOfLastUpdate.Seconds;
                            RegRecPtr->TimeOfLastUpdate.Subseconds = CritRegRecPtr->TimeOfLastUpdate.Subseconds;
                        }
                        else
                        {
                            /* Save source information into the registry */
                            sprintf(WorkingBufferPtr->DataSource, "--UNKNOWN--");
                            WorkingBufferPtr->FileCreateTimeSecs = 0;
                            WorkingBufferPtr->FileCreateTimeSubSecs = 0;
                            RegRecPtr->TimeOfLastUpdate.Seconds = 0;
                            RegRecPtr->TimeOfLastUpdate.Subseconds = 0;
                        }
                            
                        /* Flag the table as having been successfully initialized */
                        RegRecPtr->TableLoadedOnce = TRUE;
                            
                        /* Make sure everyone who sees the table knows that it has been updated 
                        CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr); */
                        
                        /* Make sure the caller realizes the contents have been initialized */
                        Status = CFE_TBL_INFO_RECOVERED_TBL;
                    }
                        
                    /* Mark the table as critical for future reference */
                    RegRecPtr->CriticalTable = TRUE;
                }
                else if (Status == CFE_SUCCESS)
                {
                    /* Find and initialize a free entry in the Critical Table Registry */
                    CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, CFE_ES_CDS_BAD_HANDLE);
                        
                    if (CritRegRecPtr != NULL)
                    {
                        CritRegRecPtr->CDSHandle = RegRecPtr->CDSHandle;
                        strncpy(CritRegRecPtr->Name, TblName, CFE_TBL_MAX_FULL_NAME_LEN);
                        CritRegRecPtr->FileCreateTimeSecs = 0;
                        CritRegRecPtr->FileCreateTimeSubSecs = 0;
                        CritRegRecPtr->LastFileLoaded[0] = '\0';
                        CritRegRecPtr->TimeOfLastUpdate.Seconds = 0;
                        CritRegRecPtr->TimeOfLastUpdate.Subseconds = 0;
                            
                        CFE_ES_CopyToCDS(CFE_TBL_TaskData.CritRegHandle, CFE_TBL_TaskData.CritReg);
                    }
                    else
                    {
                        CFE_ES_WriteToSysLog("CFE_TBL:Register-Failed to find a free Crit Tbl Reg Rec for '%s'\n", 
                                             RegRecPtr->Name);
                    }     
                        
                    /* Mark the table as critical for future reference */
                    RegRecPtr->CriticalTable = TRUE;
                }
                else
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:Register-Failed to register '%s.%s' as a CDS (ErrCode=0x%08X)\n",
                                         AppName, Name, Status);
                                             
                    /* Notify caller that although they asked for it to be critical, it isn't */
                    Status = CFE_TBL_WARN_NOT_CRITICAL;
                }
            }
            
            /* The last step of the registration process is claiming ownership.    */
            /* By making it the last step, other APIs do not have to lock registry */
            /* to share the table or get its address because registry entries that */
            /* are unowned are not checked to see if they match names, etc.        */
            RegRecPtr->OwnerAppId = ThisAppId;
        }
        
        /* Unlock Registry for update */
        /*CFE_TBL_UnlockRegistry();*/
    }
    
    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

        CFE_EVS_SendEventWithAppID(CFE_TBL_REGISTER_ERR_EID,
                                   CFE_EVS_ERROR,
                                   CFE_TBL_TaskData.TableTaskAppId,
                                   "%s Failed to Register '%s', Status=0x%08X",
                                   AppName, TblName, Status);
    }

    return Status;
}   /* End of CFE_TBL_Register() */

/*******************************************************************
**                                                                
** CFE_TBL_Share() -- Obtain handle of table registered by another
**                    application                                 
**                                                                
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_Share( CFE_TBL_Handle_t *TblHandlePtr,
                     const char *TblName )
{
    int32   Status = CFE_SUCCESS;
    uint32  ThisAppId;
    int16   RegIndx = CFE_TBL_NOT_FOUND;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    CFE_TBL_RegistryRec_t      *RegRecPtr = NULL;
    char    AppName[OS_MAX_API_NAME] = {"UNKNOWN"};
    
   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_SHARE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_SHARE_PROC];
   }
    
    /* Get a valid Application ID for calling App */
    Status = CFE_TBL_ValidateAppID(&ThisAppId);
    
    if (Status == CFE_SUCCESS)
    {
        /* Lock Registry for update.  This prevents two applications from        */
        /* trying to register/share tables at the same location at the same time */
        CFE_TBL_LockRegistry();
    
        RegIndx = CFE_TBL_FindTableInRegistry(TblName);                             
                                                                                    
        /* If we found the table, then set get a new Access Descriptor and initialize it */   
        if (RegIndx != CFE_TBL_NOT_FOUND)
        {                                                                           
            /* Get pointer to Registry Record Entry to speed up processing */
            RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndx];
            
            /* Search Access Descriptor Array for free Descriptor */
            *TblHandlePtr = CFE_TBL_FindFreeHandle();
    
            /* Check to make sure there was a handle available */
            if (*TblHandlePtr == CFE_TBL_END_OF_LIST)
            {
                Status = CFE_TBL_ERR_HANDLES_FULL;
                CFE_ES_WriteToSysLog("CFE_TBL:Share-No more free handles\n");
            }
            else  /* Successful Share Operation */
            {
                /* Initialize the Table Access Descriptor */
                UTF_put_text("UTF INFO: AppId <%d> issued shared access to Table <%s>.\n", 
                		ThisAppId, TblName);
                
                AccessDescPtr = &CFE_TBL_TaskData.Handles[*TblHandlePtr];
        
                AccessDescPtr->AppId = ThisAppId;
                AccessDescPtr->LockFlag = FALSE;
                AccessDescPtr->Updated = FALSE;
                
                /* Check current state of table in order to set Notification flags properly */
                if (RegRecPtr->TableLoadedOnce)
                {
                    AccessDescPtr->Updated = TRUE;
                }
                
                AccessDescPtr->RegIndex = RegIndx;
                AccessDescPtr->UsedFlag = TRUE;
                
                AccessDescPtr->PrevLink = CFE_TBL_END_OF_LIST;             /* We are the new head of the list */
                AccessDescPtr->NextLink = RegRecPtr->HeadOfAccessList;
                
                /* Make sure the old head of the list now sees this as the head */
                CFE_TBL_TaskData.Handles[RegRecPtr->HeadOfAccessList].PrevLink = *TblHandlePtr;
        
                /* Make sure the Registry Record see this as the head of the list */
                RegRecPtr->HeadOfAccessList = *TblHandlePtr;
            }
        }
        else  /* Table could not be found in registry */
        {
            Status = CFE_TBL_ERR_INVALID_NAME;

            CFE_ES_WriteToSysLog("CFE_TBL:Share-Table '%s' not found in Registry\n", TblName);
        }        
    }
    else  /* Application ID was invalid */
    {
        CFE_ES_WriteToSysLog("CFE_TBL:Share-Bad AppId(%d)\n", ThisAppId);
    }
    
    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

        CFE_EVS_SendEventWithAppID(CFE_TBL_SHARE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   CFE_TBL_TaskData.TableTaskAppId,
                                   "%s Failed to Share '%s', Status=0x%08X",
                                   AppName, TblName, Status);
    }

    return Status;
}   /* End of CFE_TBL_Share() */

/*******************************************************************
**                                                             
** CFE_TBL_Unregister() -- Unregister a previously registered  
**                         table and free associated resources 
**                                                             
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_Unregister ( CFE_TBL_Handle_t TblHandle )
{
    int32   Status;
    uint32  ThisAppId;
    CFE_TBL_RegistryRec_t *RegRecPtr = NULL;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    char    AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

	/* Handle Preset Return Code */
	if (cfe_tbl_api_return_value[CFE_TBL_UNREGISTER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
	{
		return cfe_tbl_api_return_value[CFE_TBL_UNREGISTER_PROC];
	}

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);
    
    if (Status == CFE_SUCCESS)
    {
        /* Get a pointer to the relevant Access Descriptor */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        
        /* Get a pointer to the relevant entry in the registry */
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
        
        /* Verify that the application unregistering the table owns the table */
        if (RegRecPtr->OwnerAppId == ThisAppId)
        {
        		UTF_put_text("UTF Info: Valid CFE_TBL_Unregister call made from owner app.\n");
        		
            /* Mark table as free, although, technically, it isn't free until the */
            /* linked list of Access Descriptors has no links in it.              */
            /* NOTE: Allocated memory is freed when all Access Links have been    */
            /*       removed.  This allows Applications to continue to use the    */
            /*       data until they acknowledge that the table has been removed. */
            RegRecPtr->OwnerAppId = (uint32)CFE_TBL_NOT_OWNED;

            /* Remove Table Name */
            RegRecPtr->Name[0] = '\0';
        }
        else
        {	
        	UTF_put_text("UTF Info: Valid CFE_TBL_Unregister call made from non-owner app.\n");
        }
        
        /* Remove the Access Descriptor Link from linked list */
        /* NOTE: If this removes the last access link, then   */
        /*       memory buffers are set free as well.         */
        CFE_TBL_RemoveAccessLink(TblHandle);
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:Unregister-App(%d) does not have access to Tbl Handle=%d\n",
                             ThisAppId, TblHandle);
    }
    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

        CFE_EVS_SendEventWithAppID(CFE_TBL_UNREGISTER_ERR_EID,
                                   CFE_EVS_ERROR,
                                   CFE_TBL_TaskData.TableTaskAppId,
                                   "%s Failed to Unregister '?', Status=0x%08X",
                                   AppName, Status);
    }

    return Status;
}   /* End of CFE_TBL_Unregister() */

/*******************************************************************
**                                                            
** CFE_TBL_Load() -- Load a specified table with data from the
**                   specified source                         
**                                                            
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_Load( CFE_TBL_Handle_t TblHandle,
                    CFE_TBL_SrcEnum_t SrcType,
                    const void *SrcDataPtr )
{
	int32                       Status;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

   /* Handle Function Hook */
   if (UTF_TBL_HookTable.CFE_TBL_Load)
      return(UTF_TBL_HookTable.CFE_TBL_Load(TblHandle, SrcType, SrcDataPtr));

   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_LOAD_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_LOAD_PROC];
   }
    
    /* Get pointers to pertinent records in registry and handles */
    AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
    RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
    
    /* Load Working Buffer, and set proper notify bits */
    Status = CFE_TBL_LoadWorkingBuffer(TblHandle, SrcType, SrcDataPtr);
    
    if (Status == CFE_SUCCESS)
    {
       Status = CFE_TBL_Manage(TblHandle);
       if (Status == CFE_TBL_INFO_UPDATED)
         Status = CFE_SUCCESS;
    }
        
    return Status;
}   /* End of CFE_TBL_Load() */

/*******************************************************************
**                                                            
** CFE_TBL_LoadWorkingBuffer() -- Load a specified table with data from the
**                   specified source                         
**                                                            
********************************************************************/
int32 CFE_TBL_LoadWorkingBuffer( CFE_TBL_Handle_t TblHandle,
                    CFE_TBL_SrcEnum_t SrcType,
                    const void *SrcDataPtr )
{
    int32                      Status;
    uint32                     ThisAppId;
    uint8                      *WorkingBufferPtr;
    uint8   				   InactiveBufferIndex;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_LoadBuff_t          LoadBuff;
    
    /* Verify access rights and get a valid Application ID for calling App */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);
    UTF_put_text("Call to CFE_TBL_ValidateAccess returned %d\n",Status); 
 /*   Status = CFE_SUCCESS; */
    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

        /* Check to see if this is a dump only table */
        if (RegRecPtr->DumpOnly)
        {
            Status = CFE_TBL_ERR_DUMP_ONLY;

            CFE_ES_WriteToSysLog("CFE_TBL:Load-App(%d) attempted to load Dump Only Tbl '%s'\n",
                                 ThisAppId, RegRecPtr->Name);
        }
        else
        { /* Not DumpOnly */
            /* Obtain a pointer to the working buffer (1 of the table's 2 dedicated buffers) */
		 	InactiveBufferIndex = 1 - RegRecPtr->ActiveBufferIndex;
		 	/*UTF_put_text("Will write to buffer #%hu\n",InactiveBufferIndex); */
			WorkingBufferPtr = RegRecPtr->Buffers[InactiveBufferIndex].BufferPtr;
 
             /* Perform appropriate update to working buffer */
             /* Determine whether the load is to occur from a file or from a block of memory */
             if (SrcType == CFE_TBL_SRC_FILE)
             {
                 /* In case the file contains a partial table load, get the active buffer contents first */
                 memcpy(WorkingBufferPtr, 
                              RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr, 
                              RegRecPtr->Size);
                              
                 LoadBuff.BufferPtr = WorkingBufferPtr;
    
                 /* Load the data from the file into the specified buffer */
                 Status = CFE_TBL_LoadFromFile(&LoadBuff, RegRecPtr, (const char *)SrcDataPtr);
   				 UTF_put_text("Call to CFE_TBL_LoadFromFile returned %x\n",Status);             }
             else if (SrcType == CFE_TBL_SRC_ADDRESS)
             {
                 /* When the source is a block of memory, it is assumed to be a complete load */
                 /*UTF_put_text("Next step: perform CFE_PSP_MemCpy for %d bytes.\n", RegRecPtr->Size);*/
                 /*UTF_put_text("Will copy value %d.\n", *(int32*)SrcDataPtr);*/
                 CFE_PSP_MemCpy((void*)WorkingBufferPtr, 
                              (void *)SrcDataPtr, 
                              RegRecPtr->Size);
             }
             else
             {
                 Status = CFE_TBL_ERR_ILLEGAL_SRC_TYPE;
             } /* End if SrcType */
                
             /* Update status to reflect that buffer was written */
             if (Status == CFE_SUCCESS)
             {
             	if (RegRecPtr->ValidationFuncPtr != NULL)
             	{ /* Ready for validation */
             		RegRecPtr->ValidateInactiveIndex = CFE_TBL_INFO_VALIDATION_PENDING;
             		RegRecPtr->LoadInProgress = InactiveBufferIndex;
             	}
             	else
             	{ /* Ready for Update */
	    			   RegRecPtr->LoadPending = TRUE;
             	}        			                       			                  	
              } /* Update status */                
          } /* Not dump only */
    } /* Success */
    
    return Status;
}   /* End of CFE_TBL_LoadWorkingBuffer() */

/*******************************************************************
**                                                            
** UTF_TBL_LoadTableFromGround                       
**                                                            
** NOTE: For complete prolog information, see 'utf_cfe_tbl.h'                                                                 
********************************************************************/
int32	UTF_TBL_LoadTableFromGround(char *Full_Table_Name, char *File_Name)
{
	CFE_TBL_Handle_t TblHandle;
	int32 Status;
	/* Get table handle based on table name */
	TblHandle = CFE_TBL_FindTableInRegistry(Full_Table_Name);
	
	if (TblHandle == CFE_TBL_NOT_FOUND)
	{
		UTF_error("Error in UTF_TBL_LoadTableFromGround: Couldn't find RegIndx for %s\n",
		Full_Table_Name);
		return CFE_TBL_ERR_INVALID_HANDLE;
	}
			
	/* Load working buffer */
	Status = CFE_TBL_LoadWorkingBuffer(  TblHandle,
                    CFE_TBL_SRC_FILE,
                    (const void *)File_Name );
    UTF_put_text("Call to CFE_TBL_LoadWorkingBuffer returns %lx\n",Status);
	return Status;
}

/*******************************************************************
**                                                                 
** CFE_TBL_Update() -- Update contents of table if an update       
**                     is pending                                  
**                                                                 
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_Update( CFE_TBL_Handle_t TblHandle )
{
    int32                       Status = CFE_SUCCESS;
    uint32                      ThisAppId;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
     char                       AppName[OS_MAX_API_NAME]={"UNKNOWN"};
 
   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_UPDATE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_UPDATE_PROC];
   }

    /* Verify access rights and get a valid Application ID for calling App */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    /* Translate AppID of caller into App Name */
    CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
    
    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

        /* Switch active buffer index to what was previously the     */
	    /* working buffer.                                           */
	    /* (In operational code, this is performed in this manner    */
	    /* only for double buffered tables.                          */
	    /* Here, we'll do it whether or not table is double buffered */
	    /* for simplicity, since the implementation should be        */
	    /* transparent to application developer.   )                 */
	    RegRecPtr->ActiveBufferIndex = 1 - RegRecPtr->ActiveBufferIndex;
	    /* UTF_put_text("Active Buffer set to #%hu\n",RegRecPtr->ActiveBufferIndex);*/
        
        /* Set notifications */
     	/* Load is no longer pending */
    	RegRecPtr->LoadPending = FALSE;
    	RegRecPtr->TableLoadedOnce = TRUE;
    	RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    		
	    /* Table has been updated */
    	AccessDescPtr->Updated = TRUE;          
        CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_SUCCESS_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   CFE_TBL_TaskData.TableTaskAppId,
                                   "%s Successfully Updated '%s'",
                                   AppName, RegRecPtr->Name);      	        
    }
 	else
    {
        /* If there was a warning (ie - Table is currently locked), then do not issue a message */
        if (Status == CFE_SUCCESS)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_SUCCESS_INF_EID,
                                       CFE_EVS_INFORMATION,
                                       CFE_TBL_TaskData.TableTaskAppId,
                                       "%s Successfully Updated '%s'",
                                       AppName, RegRecPtr->Name);
                            
            /* Save the index of the table for housekeeping telemetry */
            CFE_TBL_TaskData.LastTblUpdated = AccessDescPtr->RegIndex;
        }       
    }

    return Status;
}   /* End of CFE_TBL_Update() */

/*******************************************************************
**                                                          
** CFE_TBL_GetAddress() -- Obtain the current address of the
**                         contents of the specified table  
**                                                          
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_GetAddress( void **TblPtr,
                          CFE_TBL_Handle_t TblHandle )
{
    int32   Status = CFE_SUCCESS;
    uint32  ThisAppId;

   /* Handle Function Hook */
   if (UTF_TBL_HookTable.CFE_TBL_GetAddress)
      return(UTF_TBL_HookTable.CFE_TBL_GetAddress(TblPtr, TblHandle));

   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_GETADDRESS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_GETADDRESS_PROC];
   }
    
    /* Assume failure at returning the table address */
    *TblPtr = NULL;
    
    /* Validate the calling application's AppID */
    Status = CFE_TBL_ValidateAppID(&ThisAppId);
    
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_TBL_GetAddressInternal(TblPtr, TblHandle, ThisAppId);

        /* NOTE: GetAddressInternal calls GetNextNotification which may not */
        /*       be equal to CFE_SUCCESS and still not be an error.         */
        /*       Therefore, a write to the SysLog is unnecessary.           */
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:GetAddress-Bad AppId=%d\n", ThisAppId);
    }
    
    return Status;
}   /* End of CFE_TBL_GetAddress() */

/*******************************************************************
**                                                                
** CFE_TBL_ReleaseAddress() -- Release previously obtained pointer
**                          to the contents of the specified table
**                                                                
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_ReleaseAddress( CFE_TBL_Handle_t TblHandle )
{
    int32   Status;
    uint32  ThisAppId;
 
    /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_RELEASEADDRESS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_RELEASEADDRESS_PROC];
   }
    
    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);
    
    if (Status == CFE_SUCCESS)
    {
        /* Clear the lock flag */
        CFE_TBL_TaskData.Handles[TblHandle].LockFlag = FALSE;
    
        /* Return any pending warning or info status indicators */
        Status = CFE_TBL_GetNextNotification(TblHandle);

        /* NOTE: GetNextNotification may not return CFE_SUCCESS  */
        /*       and still not be an error.                      */
        /*       Therefore, a write to the SysLog is unnecessary.*/
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:ReleaseAddress-App(%d) does not have access to Tbl Handle=%d\n",
                             ThisAppId, TblHandle);
    }
    
    return Status;
}   /* End of CFE_TBL_ReleaseAddress() */

/*******************************************************************
**                                                                
** CFE_TBL_GetAddresses() -- Obtain the current addresses of the contents of a collection of tables. 
**                           If an address is not obtainable, the function returns a NULL for that ptr
**                           and continues to get additional table addresses.  Failure return codes
**                           only apply to the first table that was unsuccessful.  
**                                                                
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_GetAddresses( void **TblPtrs[],
                            uint16 NumTables,
                            const CFE_TBL_Handle_t TblHandles[] )
{
    uint16  i;
    int32   Status = CFE_SUCCESS;
    uint32  ThisAppId;

   /* Handle Function Hook */
   if (UTF_TBL_HookTable.CFE_TBL_GetAddresses)
      return(UTF_TBL_HookTable.CFE_TBL_GetAddresses(TblPtrs, NumTables, TblHandles));

   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_GETADDRESSES_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_GETADDRESSES_PROC];
   }
    
    
    /* Assume failure at returning the table addresses */
    for (i=0; i<NumTables; i++)
    {
        *TblPtrs[i] = NULL;
    }
    
    /* Validate the calling application's AppID */
    Status = CFE_TBL_ValidateAppID(&ThisAppId);
    
    if (Status == CFE_SUCCESS)
    {
        for (i=0; i<NumTables; i++)
        {
            /* Continue to get the return status until one returns something other than CFE_SUCCESS */
            if (Status == CFE_SUCCESS)
            {
                Status = CFE_TBL_GetAddressInternal(TblPtrs[i], TblHandles[i], ThisAppId);
            }
            else
            {
                /* Don't bother getting the status of other tables once one has returned */
                /* a non CFE_SUCCESS value.                                              */
                CFE_TBL_GetAddressInternal(TblPtrs[i], TblHandles[i], ThisAppId);
            }
        
            i++;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:GetAddresses-Bad AppId=%d\n", ThisAppId);
    }
        
    return Status;
}   /* End of CFE_TBL_GetAddresses() */

/*******************************************************************
**                                                              
** CFE_TBL_ReleaseAddresses() -- Release previously obtained    
**             pointers to the contents of the specified tables 
**                                                              
** NOTE: For complete prolog information, see 'cfe_tbl.h'                                                                 
********************************************************************/
int32 CFE_TBL_ReleaseAddresses( uint16 NumTables,
                                const CFE_TBL_Handle_t TblHandles[] )
{
    int32   Status = CFE_SUCCESS;
    uint16  i;
 
     /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_RELEASEADDRESSES_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_RELEASEADDRESSES_PROC];
   }
       
   for (i=0; i<NumTables; i++)
   {
        /* Continue to get the return status until one returns something other than CFE_SUCCESS */
      if (Status == CFE_SUCCESS)
      {
          Status = CFE_TBL_ReleaseAddress(TblHandles[i]);
      }
      else
      {
            /* Don't bother getting the status of other tables once one has returned */
            /* a non CFE_SUCCESS value.                                              */
          CFE_TBL_ReleaseAddress(TblHandles[i]);
      }
   }
    
    return Status;
}   /* End of CFE_TBL_ReleaseAddresses() */

/*******************************************************************
**
** CFE_TBL_Validate() -- Validate a table image for the specified table
**
** NOTE: For complete prolog information, see 'cfe_tbl.h'
********************************************************************/
int32 CFE_TBL_Validate( CFE_TBL_Handle_t TblHandle )
{
    int32                       Status = CFE_SUCCESS;
    uint32                      ThisAppId;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    char                        AppName[OS_MAX_API_NAME]={"UNKNOWN"};
    
   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_VALIDATE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_VALIDATE_PROC];
   }

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

        /* Identify the image to be validated, starting with the Inactive Buffer */
        if (RegRecPtr->ValidateInactiveIndex != CFE_TBL_NO_VALIDATION_PENDING)
        {
            /* Identify whether the Inactive Buffer is a shared buffer or a dedicated one */
            if (RegRecPtr->DblBuffered)
            {
                /* Call the Application's Validation function for the Inactive Buffer */
                Status = (RegRecPtr->ValidationFuncPtr)(RegRecPtr->Buffers[(1U-RegRecPtr->ActiveBufferIndex)].BufferPtr);
            }
            else
            {
                /* Call the Application's Validation function for the appropriate shared buffer */
                Status = (RegRecPtr->ValidationFuncPtr)
                          (CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr);
            }

            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_INF_EID,
                                           CFE_EVS_INFORMATION,
                                           CFE_TBL_TaskData.TableTaskAppId,
                                           "%s validation successful for Inactive '%s'",
                                           AppName, RegRecPtr->Name);
                RegRecPtr->LoadPending = TRUE;
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_ERR_EID,
                                           CFE_EVS_ERROR,
                                           CFE_TBL_TaskData.TableTaskAppId,
                                           "%s validation failed for Inactive '%s', Status=0x%08X",
                                           AppName, RegRecPtr->Name, Status);
            }

            /* Save the result of the Validation function for the Table Services Task */
            /* CFE_TBL_TaskData.ValidationResults[RegRecPtr->ValidateInactiveIndex].Result = Status;*/

            /* Once validation is complete, set flags to indicate response is ready */
            /*CFE_TBL_TaskData.ValidationResults[RegRecPtr->ValidateInactiveIndex].State = CFE_TBL_VALIDATION_PERFORMED;*/
            RegRecPtr->ValidateInactiveIndex = CFE_TBL_NO_VALIDATION_PENDING;
        }
        else if (RegRecPtr->ValidateActiveIndex != CFE_TBL_NO_VALIDATION_PENDING)
        {
            /* Perform validation on the currently active table buffer */
            /* Identify whether the Active Buffer is a shared buffer or a dedicated one */
            if (RegRecPtr->DblBuffered)
            {
                /* Call the Application's Validation function for the Dedicated Active Buffer */
                Status = (RegRecPtr->ValidationFuncPtr)(RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr);
            }
            else
            {
                /* Call the Application's Validation function for the static buffer */
                Status = (RegRecPtr->ValidationFuncPtr)(RegRecPtr->Buffers[0].BufferPtr);
            }

            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_INF_EID,
                                           CFE_EVS_INFORMATION,
                                           CFE_TBL_TaskData.TableTaskAppId,
                                           "%s validation successful for Active '%s'",
                                           AppName, RegRecPtr->Name);
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_ERR_EID,
                                           CFE_EVS_ERROR,
                                           CFE_TBL_TaskData.TableTaskAppId,
                                           "%s validation failed for Active '%s', Status=0x%08X",
                                           AppName, RegRecPtr->Name, Status);
            }

            /* Save the result of the Validation function for the Table Services Task */
            CFE_TBL_TaskData.ValidationResults[RegRecPtr->ValidateActiveIndex].Result = Status;

            /* Once validation is complete, reset the flags */
            CFE_TBL_TaskData.ValidationResults[RegRecPtr->ValidateActiveIndex].State = CFE_TBL_VALIDATION_PERFORMED;
            RegRecPtr->ValidateActiveIndex = CFE_TBL_NO_VALIDATION_PENDING;

            /* Since the validation was successfully performed (although maybe not a successful result) */
            /* return a success status */
            Status = CFE_SUCCESS;
        }
        else
        {
            Status = CFE_TBL_INFO_NO_VALIDATION_PENDING;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:Validate-App(%d) does not have access to Tbl Handle=%d\n",
                             ThisAppId, TblHandle);
    }

    return Status;
}   /* End of CFE_TBL_Validate() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TBL_Set_Api_Return_Code() --                                    */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_TBL_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode)
{
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_TBL_API_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_TBL_Set_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }
   /* If index is valid, assign return value and report action */
   cfe_tbl_api_return_value[ApiIndex] = ReturnCode;

   if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
   	  UTF_put_text("UTF Info: Return code for CFE TBL Api #%d returned to default\n",
   	               ApiIndex);
   }
   else
   {
   	  UTF_put_text("UTF Info: Return value for CFE TBL Api #%d set to %lx\n", 
   	                ApiIndex, ReturnCode);
   }
   return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TBL_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_TBL_Use_Default_Api_Return_Code(int32 ApiIndex)
{
	int32 Success;
	
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_TBL_API_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_TB;_Use_Default_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }

   /* If index is valid, assign return value and report action */
   Success = UTF_CFE_TBL_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);
   
   return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TBL_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_TBL_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_TBL_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
	/* Check for correct number of arguments */
	if (argc != 3)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_TBL_Set_Api_Return_Code. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
	UTF_CFE_TBL_Set_Api_Return_Code(Index, Code);
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TBL_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_TBL_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_TBL_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

	/* Check for correct number of arguments */
	if (argc != 2)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_TBL_Set_Api_Return_Code. Expected 1.\n",
	   argc -1 );
	   UTF_exit();
	}
 
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
 
    /* Call function to set API return code */
	UTF_CFE_TBL_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
	return;
}
/*******************************************************************
**
** CFE_TBL_Manage() -- Perform standard table maintenance on the
**                     specified table
**
** NOTE: For complete prolog information, see 'cfe_tbl.h'
********************************************************************/
int32 CFE_TBL_Manage( CFE_TBL_Handle_t TblHandle )
{
    int32   Status = CFE_SUCCESS;
    boolean FinishedManaging = FALSE;

   /* Handle Function Hook */
   if (UTF_TBL_HookTable.CFE_TBL_Manage)
      return(UTF_TBL_HookTable.CFE_TBL_Manage(TblHandle));

   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_MANAGE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_MANAGE_PROC];
   }

    while (!FinishedManaging)
    {
        /* Determine if the table has a validation or update that needs to be performed */
        Status = CFE_TBL_GetStatus(TblHandle);

        if (Status == CFE_TBL_INFO_VALIDATION_PENDING)
        {
            /* Validate the specified Table */
            Status = CFE_TBL_Validate(TblHandle);
 
            if (Status != CFE_SUCCESS)
            {
                /* If an error occurred during Validate, then do not perform any more managing */
                FinishedManaging = TRUE;
            }
        }
        else if (Status == CFE_TBL_INFO_DUMP_PENDING)
        {
            /* Dump the specified Table */
            Status = CFE_TBL_DumpToBuffer(TblHandle);
            
            /* After a Dump, always assume we are done (Dumps are on DumpOnly tables and cannot be "Updated") */
            FinishedManaging = TRUE;
        }
        else if (Status == CFE_TBL_INFO_UPDATE_PENDING)
        {
            /* Update the specified Table */
            Status = CFE_TBL_Update(TblHandle);
            
             /* If the update performed nominally, let the caller know the table has changed */
            if (Status == CFE_SUCCESS)
            {
                Status = CFE_TBL_INFO_UPDATED;
            }           

            /* After an update, always assume we are done and return Update Status */
            FinishedManaging = TRUE;
        }
        else
        {
            FinishedManaging = TRUE;
        }
    }

    return Status;
}   /* End of CFE_TBL_Manage() */

/*******************************************************************
**
** CFE_TBL_GetStatus() -- Obtain pending action status for specified table
**
** NOTE: For complete prolog information, see 'cfe_tbl.h'
********************************************************************/
int32 CFE_TBL_GetStatus( CFE_TBL_Handle_t TblHandle )
{
    int32                       Status = CFE_SUCCESS;
    uint32                      ThisAppId;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    
   /* Handle Preset Return Code */
   if (cfe_tbl_api_return_value[CFE_TBL_GETSTATUS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_tbl_api_return_value[CFE_TBL_GETSTATUS_PROC];
   }

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

        /* Perform validations prior to performing any updates */
        if (RegRecPtr->LoadPending)
        {
            Status = CFE_TBL_INFO_UPDATE_PENDING;
        }
        else if ((RegRecPtr->ValidateActiveIndex != CFE_TBL_NO_VALIDATION_PENDING) ||
                 (RegRecPtr->ValidateInactiveIndex != CFE_TBL_NO_VALIDATION_PENDING))
        {
            Status = CFE_TBL_INFO_VALIDATION_PENDING;
        }
        else if (RegRecPtr->DumpControlIndex != CFE_TBL_NO_DUMP_PENDING)
        {
            Status = CFE_TBL_INFO_DUMP_PENDING;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:GetStatus-App(%d) does not have access to Tbl Handle=%d\n",
                             ThisAppId, TblHandle);
    }

    return Status;
}   /* End of CFE_TBL_GetStatus() */


/*******************************************************************
**
** CFE_TBL_GetInfo() -- Obtain characteristics/information for specified table
**
** NOTE: For complete prolog information, see 'cfe_tbl.h'
********************************************************************/
int32 CFE_TBL_GetInfo( CFE_TBL_Info_t *TblInfoPtr, const char *TblName )
{
    int32                    Status = CFE_SUCCESS;
    int16                    RegIndx = CFE_TBL_NOT_FOUND;
    int32                    NumAccessDescriptors = 0;
    CFE_TBL_RegistryRec_t   *RegRecPtr;
    CFE_TBL_Handle_t         HandleIterator;

    /* Handle Preset Return Code */
    if (cfe_tbl_api_return_value[CFE_TBL_GETINFO_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
       return cfe_tbl_api_return_value[CFE_TBL_GETINFO_PROC];
    }

    RegIndx = CFE_TBL_FindTableInRegistry(TblName);

    /* If we found the table, then extract the information from the Registry */
    if (RegIndx != CFE_TBL_NOT_FOUND)
    {
        /* Get pointer to Registry Record Entry to speed up processing */
        RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndx];

        /* Return table characteristics */        
        TblInfoPtr->Size        = RegRecPtr->Size;
        TblInfoPtr->DblBuffered = RegRecPtr->DblBuffered;
        TblInfoPtr->DumpOnly    = RegRecPtr->DumpOnly;
        TblInfoPtr->UserDefAddr = RegRecPtr->UserDefAddr;
        TblInfoPtr->TableLoadedOnce = RegRecPtr->TableLoadedOnce;
        
        /* Return information on last load and update */
        TblInfoPtr->TimeOfLastUpdate = RegRecPtr->TimeOfLastUpdate;
        TblInfoPtr->FileCreateTimeSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSecs;
        TblInfoPtr->FileCreateTimeSubSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSubSecs;
        strncpy(TblInfoPtr->LastFileLoaded, RegRecPtr->LastFileLoaded, OS_MAX_PATH_LEN);
        
        /* Count the number of Access Descriptors to determine the number of users */
        HandleIterator = RegRecPtr->HeadOfAccessList;
        while (HandleIterator != CFE_TBL_END_OF_LIST)
        {
            NumAccessDescriptors++;
            HandleIterator = CFE_TBL_TaskData.Handles[HandleIterator].NextLink;
        }

        TblInfoPtr->NumUsers = NumAccessDescriptors;
        
        TblInfoPtr->Critical = RegRecPtr->CriticalTable;
    }
    else
    {
        Status = CFE_TBL_ERR_INVALID_NAME;
    }

    return Status;
}   /* End of CFE_TBL_GetInfo() */

/*******************************************************************
**
** CFE_TBL_Modified - Notify TBL Services that the contents of the table
**     has been modified by the application
**
** NOTE: For complete prolog information, see 'cfe_tbl.h'
********************************************************************/
int32 CFE_TBL_Modified( CFE_TBL_Handle_t TblHandle )
{
    int32                       Status = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    CFE_TBL_RegistryRec_t      *RegRecPtr = NULL;
    CFE_TBL_Handle_t            AccessIterator;
    uint32                      ThisAppId;
    uint32                      FilenameLen = 0;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

        AccessIterator = RegRecPtr->HeadOfAccessList;
        while (AccessIterator != CFE_TBL_END_OF_LIST)
        {
            CFE_TBL_TaskData.Handles[AccessIterator].Updated = TRUE;

            AccessIterator = CFE_TBL_TaskData.Handles[AccessIterator].NextLink;
        }
        
        /* If the table is a critical table, update the appropriate CDS with the new data */
        if (RegRecPtr->CriticalTable == TRUE)
        {
            CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
        }
        
        /* Keep a record of change for the ground operators reference */
        RegRecPtr->TimeOfLastUpdate = CFE_TIME_GetTime();
        RegRecPtr->LastFileLoaded[OS_MAX_PATH_LEN-1] = '\0';
        FilenameLen = strlen(RegRecPtr->LastFileLoaded);
        if (FilenameLen < (OS_MAX_PATH_LEN-4))
        {
            strncpy(&RegRecPtr->LastFileLoaded[FilenameLen], "(*)", 4);
        }
        else
        {
            strncpy(&RegRecPtr->LastFileLoaded[(OS_MAX_PATH_LEN-4)], "(*)", 4);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_TBL:Modified-App(%d) does not have access to Tbl Handle=%d\n",
                             ThisAppId, TblHandle);
    }

    
    return Status;
}

/************************/
/*  End of File Comment */
/************************/
