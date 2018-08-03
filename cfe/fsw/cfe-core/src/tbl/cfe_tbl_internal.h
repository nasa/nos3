/*
** $Id: cfe_tbl_internal.h 1.8 2012/01/13 12:17:40GMT-05:00 acudmore Exp  $
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
** Purpose:  cFE Table Services (TBL) utility function interface file
**
** Author:   D. Kobe/the Hammers Company, Inc.
**
** Notes:    
**
** $Log: cfe_tbl_internal.h  $
** Revision 1.8 2012/01/13 12:17:40GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.7 2011/12/28 14:02:59EST lwalling 
** Add validation tests for spacecraft ID and processor ID to CFE_TBL_ReadHeaders()
** Revision 1.6 2010/10/27 13:56:54EDT dkobe 
** Added internal function prototype for sending notification messages
** Revision 1.5 2010/10/25 15:00:34EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 15:18:53EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2008/12/08 16:10:53EST dkobe 
** Correcting errors generated during detailed design document generation
** Revision 1.2 2008/07/29 15:55:18EDT dkobe 
** Moved CFE_TBL_DumpToBuffer from cfe_tbl_internal.c to cfe_tbl_api.c
** Revision 1.1 2008/04/17 08:05:35EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/tbl/project.pj
** Revision 1.20 2007/07/13 11:27:15EDT David Kobe (dlkobe) 
** Moved prototype for CFE_TBL_UpdateCriticalTblCDS to cfe_tbl_internal.h
** Revision 1.19 2007/07/03 10:19:19EDT dlkobe 
** Removed unused CFE_TBL_AddAccessLink function
** Revision 1.18 2007/04/28 14:48:58EDT dlkobe 
** Baseline Implementation of Critical Tables
** Revision 1.16 2007/03/16 13:57:16EST dlkobe 
** Updated return code documentation for CFE_TBL_LoadFromFile function.
** Revision 1.15 2007/03/16 09:10:10EST dlkobe 
** Added CFE_TBL_CleanUpApp API
** Revision 1.14 2006/09/01 12:07:39GMT-05:00 dlkobe 
** Added processor endianess check to ensure the cFE TBL header is always big-endian.
** Revision 1.13 2006/07/25 14:37:53GMT-05:00 njyanchik 
** It turns out the both the FS app and the OSAL were incorrect where file descriptors are
** concerned. the file descriptors should be int32 across the board.
** Revision 1.12 2006/06/08 14:25:58EDT dlkobe 
** Added legal statement to TBL service source and header files
** Revision 1.11 2006/05/24 14:53:47GMT-05:00 dlkobe 
** Created new function for initializing a record in Table Registry
**
**
*/

#ifndef _cfe_tbl_internal_
#define _cfe_tbl_internal_

/*
** Required header files...
*/
#include "cfe_tbl.h"
#include "cfe_tbl_task.h"
#include "cfe_tbl_filedef.h"

/*********************  Macro and Constant Type Definitions   ***************************/

#define CFE_TBL_NOT_OWNED               0xFFFFFFFF
#define CFE_TBL_NOT_FOUND               (-1)
#define CFE_TBL_END_OF_LIST             (CFE_TBL_Handle_t)0xFFFF

/*****************************  Function Prototypes   **********************************/

/*****************************************************************************/
/**
** \brief Validates the Application ID associated with calling Application
**
** \par Description
**        Validates Application ID of calling App.  Validation
**        consists of ensuring the AppID is between zero and
**        #CFE_ES_MAX_APPLICATIONS.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppIdPtr Pointer to value that will hold AppID on return.
** 
** \param[out] *AppIdPtr The AppID as obtained from #CFE_ES_GetAppID
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_BAD_APP_ID          \copydoc CFE_TBL_ERR_BAD_APP_ID
**                     
******************************************************************************/
int32   CFE_TBL_ValidateAppID(uint32 *AppIdPtr);

/*****************************************************************************/
/**
** \brief Validates specified handle to ensure legality
**
** \par Description
**        Validates handle given by calling App to Table API. Validation
**        includes ensuring the value is within an acceptable range and
**        the Access Descriptor that it identifies is being "used".
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblHandle  - Handle to be validated
** 
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copydoc CFE_TBL_ERR_INVALID_HANDLE
**                     
******************************************************************************/
int32   CFE_TBL_ValidateHandle(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Determines whether handle is associated with calling Application
**
** \par Description
**        Validates whether the calling application has the right to
**        access the table identified with the given TblHandle.  Validation
**        consists of verifying the calling Application's AppID, verifying
**        the legitimacy of the given TblHandle, and checking to make sure
**        the Access Descriptor identified by the TblHandle is associated
**        with the calling Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblHandle Handle of table whose access is desired.
**
** \param[in]  AppIdPtr  Pointer to value that will hold AppID on return.
** 
** \param[out] *AppIdPtr The AppID as obtained from #CFE_ES_GetAppID
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_BAD_APP_ID          \copydoc CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copydoc CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_NO_ACCESS           \copydoc CFE_TBL_ERR_NO_ACCESS
**                     
******************************************************************************/
int32   CFE_TBL_ValidateAccess(CFE_TBL_Handle_t TblHandle, uint32 *AppIdPtr);

/*****************************************************************************/
/**
** \brief Determines if calling application has the right to access specified table
**
** \par Description
**        Validates whether the calling application has the right to
**        access the table identified with the given TblHandle.  Validation
**        consists of checking to make sure the Access Descriptor identified
**        by the TblHandle is associated with the calling Application.
**
** \par Assumptions, External Events, and Notes:
**        Note: The TblHandle and ThisAppId parameters are assumed to be valid.
**
** \param[in]  TblHandle Handle of table whose access is desired.
** 
** \param[in]  ThisAppId Application ID of Application making the call
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_ACCESS           \copydoc CFE_TBL_ERR_NO_ACCESS
**                     
******************************************************************************/
int32   CFE_TBL_CheckAccessRights(CFE_TBL_Handle_t TblHandle, uint32 ThisAppId);


/*****************************************************************************/
/**
** \brief Removes Access Descriptor from Table's linked list of Access Descriptors
**
** \par Description
**        Removes the given Access Descriptor from the Linked List
**        of Access Descriptors associated with the table specified
**        in the Access Descriptor itself.
**
** \par Assumptions, External Events, and Notes:
**        -# This function CAN block and should not be called by ISRs.
**        -# This function assumes the Access Descriptor is completely
**           filled out and the TblHandle has been validated.
**
** \param[in]  TblHandle Handle of Access Descriptor to be removed.
** 
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS                     
**
******************************************************************************/
int32   CFE_TBL_RemoveAccessLink(CFE_TBL_Handle_t TblHandle);


/*****************************************************************************/
/**
** \brief Obtains the data address for the specified table
**
** \par Description
**        Validates the given TblHandle, finds the location of the
**        Table data and returns the address to the data to the caller.
**
** \par Assumptions, External Events, and Notes:
**        -# It is possible that an Application that was sharing a table
**           would discover, upon making this call, that the table has
**           been unregistered by another Application.  In this situation,
**           this function would return #CFE_TBL_ERR_UNREGISTERED.
**        -# ThisAppId parameter is assumed to be validated.
**
** \param[in]  TblPtr    Pointer to pointer that will hold address of data upon return.
** \param[in]  TblHandle Handle of Table whose address is needed.
** \param[in]  ThisAppId AppID of application making the address request.
** 
** \param[out] *TblPtr   Address of Table Data.
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copydoc CFE_TBL_ERR_INVALID_HANDLE 
** \retval #CFE_TBL_ERR_NO_ACCESS           \copydoc CFE_TBL_ERR_NO_ACCESS      
** \retval #CFE_TBL_ERR_UNREGISTERED        \copydoc CFE_TBL_ERR_UNREGISTERED
**                
******************************************************************************/
int32   CFE_TBL_GetAddressInternal(void **TblPtr, CFE_TBL_Handle_t TblHandle, uint32 ThisAppId);


/*****************************************************************************/
/**
** \brief Returns any pending non-error status code for the specified table.
**
** \par Description
**        Returns any pending non-error status code for the specified table.
**
** \par Assumptions, External Events, and Notes:
**        Note: This function assumes the TblHandle has been validated.
**
** \param[in]  TblHandle Handle of Table whose pending notifications are
**                       to be returned.
** 
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATE_PENDING     \copydoc CFE_TBL_INFO_UPDATE_PENDING
** \retval #CFE_TBL_INFO_UPDATED            \copydoc CFE_TBL_INFO_UPDATED
**
******************************************************************************/
int32   CFE_TBL_GetNextNotification(CFE_TBL_Handle_t TblHandle);


/*****************************************************************************/
/**
** \brief Returns the Registry Index for the specified Table Name
**
** \par Description
**        Locates given Table Name in the Table Registry and
**        returns the appropriate Registry Index.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblName - Pointer to character string containing complete
**                       Table Name (of the format "AppName.TblName").
** 
** \retval #CFE_TBL_NOT_FOUND or the Index into Registry for Table with specified name
**                     
******************************************************************************/
int16            CFE_TBL_FindTableInRegistry(const char *TblName);

/*****************************************************************************/
/**
** \brief Locates a free slot in the Table Registry.
**
** \par Description
**        Locates a free slot in the Table Registry.
**
** \par Assumptions, External Events, and Notes:
**        Note: This function assumes the registry has been locked.
**
** \retval #CFE_TBL_NOT_FOUND or Index into Table Registry of unused entry                     
******************************************************************************/
int16            CFE_TBL_FindFreeRegistryEntry(void);

/*****************************************************************************/
/**
** \brief Locates a free Access Descriptor in the Table Handles Array.
**
** \par Description
**        Locates a free Access Descriptor in the Table Handles Array.
**
** \par Assumptions, External Events, and Notes:
**        Note: This function assumes the registry has been locked.
**
** \retval #CFE_TBL_END_OF_LIST or Table Handle of unused Access Descriptor                     
******************************************************************************/
CFE_TBL_Handle_t CFE_TBL_FindFreeHandle(void);


/*****************************************************************************/
/**
** \brief Creates a Full Table name from application name and table name
**
** \par Description
**        Takes a given Table Name and combines it with the calling
**        Application's name to make a processor specific name of the
**        form: "AppName.TblName"
**
** \par Assumptions, External Events, and Notes:
**        Note: AppName portion will be truncated to OS_MAX_API_NAME.
**
** \param[in]  FullTblName pointer to character buffer of #CFE_TBL_MAX_FULL_NAME_LEN size
**                         that will be filled with the processor specific Table Name.
** 
** \param[in]  TblName pointer to character string containing the Application's local name for
**                     the Table.
**
** \param[in]  ThisAppId the Application ID of the Application making the call. 
**
** \param[out] *FullTblName processor specific Table Name of the form "AppName.TblName".
**
** \retval None                     
******************************************************************************/
void CFE_TBL_FormTableName(char *FullTblName, const char *TblName, uint32 ThisAppId);


/*****************************************************************************/
/**
** \brief Locks access to the Table Registry
**
** \par Description
**        Locks the Table Registry to prevent multiple tasks/threads
**        from modifying it at once.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS                     
******************************************************************************/
int32   CFE_TBL_LockRegistry(void);

/*****************************************************************************/
/**
** \brief Unlocks access to the Table Registry
**
** \par Description
**        Unlocks Table Registry to allow other tasks/threads to
**        modify the Table Registry contents.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**                     
******************************************************************************/
int32   CFE_TBL_UnlockRegistry(void);


/*****************************************************************************/
/**
** \brief Finds the address of a buffer compatible with the specified table
**
** \par Description
**        Checks to see if the specified table has a dedicated working
**        buffer (i.e. - is a double buffered table) or requires one
**        from the common table buffer pool.  If it requires one from
**        the pool, it locates, locks and returns its address.  If the
**        table is double buffered, the access list is scanned to ensure
**        that nobody is currently using the inactive buffer.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes the TblHandle and MinBufferSize values
**           are legitimate.
**
** \param[in]  WorkingBufferPtr  Pointer to variable that will contain the
**                               address of the first byte of the working buffer
**
** \param[in]  RegRecPtr         Pointer to Table Registry Entry for Table for whom
**                               a working buffer is to be obtained
**
** \param[in]  CalledByApp       Boolean that identifies whether this internal API
**                               function is being called by a user Application (TRUE)
**                               or by the Table Services Application (FALSE)
** 
** \param[out] *WorkingBufferPtr Address of first byte of working buffer
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_BUFFER_AVAIL     \copydoc CFE_TBL_ERR_NO_BUFFER_AVAIL                     
**
******************************************************************************/
int32   CFE_TBL_GetWorkingBuffer(CFE_TBL_LoadBuff_t **WorkingBufferPtr,
                                 CFE_TBL_RegistryRec_t *RegRecPtr,
                                 boolean CalledByApp);
                                 

/*****************************************************************************/
/**
** \brief Loads a table buffer with data from a specified file
**
** \par Description
**        Locates the specified filename in the onboard filesystem
**        and loads its contents into the specified working buffer.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes parameters have been verified.
**
** \param[in]  WorkingBufferPtr Pointer to a working buffer that is to be loaded
**                              with the contents of the specified file
**
** \param[in]  RegRecPtr        Pointer to Table Registry record for table whose
**                              buffer is to filled with data from the specified file
** 
** \param[in]  Filename         Pointer to ASCII string containing full path and filename
**                              of table image file to be loaded
**
** \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
** \retval #OS_FS_ERR_INVALID_POINTER        \copydoc OS_FS_ERR_INVALID_POINTER      
** \retval #OS_FS_ERR_PATH_TOO_LONG          \copydoc OS_FS_ERR_PATH_TOO_LONG      
** \retval #OS_FS_ERR_PATH_INVALID           \copydoc OS_FS_ERR_PATH_INVALID      
** \retval #OS_FS_ERR_NAME_TOO_LONG          \copydoc OS_FS_ERR_NAME_TOO_LONG      
** \retval #OS_FS_ERR_NO_FREE_FDS            \copydoc OS_FS_ERR_NO_FREE_FDS      
** \retval #OS_FS_ERROR                      \copydoc OS_FS_ERROR      
** \retval #CFE_TBL_ERR_FILE_TOO_LARGE       \copydoc CFE_TBL_ERR_FILE_TOO_LARGE      
** \retval #CFE_TBL_WARN_SHORT_FILE          \copydoc CFE_TBL_WARN_SHORT_FILE         
** \retval #CFE_TBL_WARN_PARTIAL_LOAD        \copydoc CFE_TBL_WARN_PARTIAL_LOAD       
** \retval #CFE_TBL_ERR_FILENAME_TOO_LONG    \copydoc CFE_TBL_ERR_FILENAME_TOO_LONG   
** \retval #CFE_TBL_ERR_FILE_FOR_WRONG_TABLE \copydoc CFE_TBL_ERR_FILE_FOR_WRONG_TABLE
** \retval #CFE_TBL_ERR_NO_STD_HEADER        \copydoc CFE_TBL_ERR_NO_STD_HEADER       
** \retval #CFE_TBL_ERR_NO_TBL_HEADER        \copydoc CFE_TBL_ERR_NO_TBL_HEADER       
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID       \copydoc CFE_TBL_ERR_BAD_CONTENT_ID      
** \retval #CFE_TBL_ERR_BAD_SUBTYPE_ID       \copydoc CFE_TBL_ERR_BAD_SUBTYPE_ID      
**                     
******************************************************************************/
int32   CFE_TBL_LoadFromFile(CFE_TBL_LoadBuff_t *WorkingBufferPtr, 
                             CFE_TBL_RegistryRec_t *RegRecPtr, const char *Filename);


/*****************************************************************************/
/**
** \brief Updates the active table buffer with contents of inactive buffer
**
** \par Description
**        Copies pertinent data from working buffer (inactive buffer)
**        to the active buffer (for single buffered tables) or just
**        changes index to identifying the active buffer (for double
**        buffered tables).
**
** \par Assumptions, External Events, and Notes:
**        -# All parameters are assumed to be verified before function
**           is called.
**
** \param[in]  TblHandle      Handle of Table to be updated.
** 
** \param[in]  RegRecPtr      Pointer to Table Registry Entry for table to be updated
**
** \param[in]  AccessDescPtr  Pointer to appropriate access descriptor for table-application interface
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS                     
******************************************************************************/
int32   CFE_TBL_UpdateInternal( CFE_TBL_Handle_t TblHandle, 
                                CFE_TBL_RegistryRec_t *RegRecPtr,
                                CFE_TBL_AccessDescriptor_t *AccessDescPtr );


/*****************************************************************************/
/**
** \brief Sets flags in access descriptors associated with specified table
**
** \par Description
**        Sets the flag in each access descriptor for a table to indicate the
**        contents of the table have been updated.
**
** \par Assumptions, External Events, and Notes:
**        -# All parameters are assumed to be verified before function
**           is called.
**
** \param[in]  RegRecPtr      Pointer to Table Registry Entry for table to be updated
******************************************************************************/
void   CFE_TBL_NotifyTblUsersOfUpdate( CFE_TBL_RegistryRec_t *RegRecPtr );


/*****************************************************************************/
/**
** \brief Reads Table File Headers
**
** \par Description
**        Reads Table File Headers and performs rudimentary checks
**        on header contents to ensure the acceptability of the
**        file format.
**
** \par Assumptions, External Events, and Notes:
**        -# FileDescriptor is assumed to be valid
**
** \param[in]  FileDescriptor    File Descriptor, as provided by OS_fopen
** 
** \param[in]  StdFileHeaderPtr  Pointer to buffer to be filled with the contents
**                               of the file's standard cFE Header
**
** \param[in]  TblFileHeaderPtr  Pointer to buffer to be filled with the contents
**                               of the file's standard cFE Table Header
**
** \param[in]  LoadFilename      Pointer to character string containing full path
**                               and filename of table image to be loaded
** 
** \param[out] *StdFileHeaderPtr Contents of standard cFE File Header
**
** \param[out] *TblFileHeaderPtr Contents of standard cFE Table File Header
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_STD_HEADER       \copydoc CFE_TBL_ERR_NO_STD_HEADER 
** \retval #CFE_TBL_ERR_NO_TBL_HEADER       \copydoc CFE_TBL_ERR_NO_TBL_HEADER 
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID      \copydoc CFE_TBL_ERR_BAD_CONTENT_ID
** \retval #CFE_TBL_ERR_BAD_SUBTYPE_ID      \copydoc CFE_TBL_ERR_BAD_SUBTYPE_ID
** \retval #CFE_TBL_ERR_BAD_SPACECRAFT_ID   \copydoc CFE_TBL_ERR_BAD_SPACECRAFT_ID
** \retval #CFE_TBL_ERR_BAD_PROCESSOR_ID    \copydoc CFE_TBL_ERR_BAD_PROCESSOR_ID
**                     
******************************************************************************/
int32 CFE_TBL_ReadHeaders( int32 FileDescriptor, 
                           CFE_FS_Header_t *StdFileHeaderPtr, 
                           CFE_TBL_File_Hdr_t *TblFileHeaderPtr,
                           const char *LoadFilename );



/*****************************************************************************/
/**
** \brief Initializes the entries of a single Table Registry Record
**
** \par Description
**        Initializes the contents of a single Table Registry Record to default values
**
** \par Assumptions, External Events, and Notes:
**        -# This function is intended to be called before populating a table registry record
**
******************************************************************************/
void CFE_TBL_InitRegistryRecord (CFE_TBL_RegistryRec_t *RegRecPtr);


/*****************************************************************************/
/**
** \brief Byte swaps a CFE_TBL_File_Hdr_t structure
**
** \par Description
**        Converts a big-endian version of a CFE_TBL_File_Hdr_t structure to
**        a little-endian version and vice-versa.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  HdrPtr   Pointer to table header that needs to be swapped.
** 
** \param[out] *HdrPtr  The swapped header
**
**                     
******************************************************************************/
void CFE_TBL_ByteSwapTblHeader(CFE_TBL_File_Hdr_t *HdrPtr);


/*****************************************************************************/
/**
** \brief Searches the Critical Table Registry for the given handle
**
** \par Description
**        This function scans the Critical Table Registry to find the specified
**        handle.  Once located, the function returns a pointer to the appropriate
**        Critical Table Registry Record that contains information on where the
**        contents of the Table came from and when.  If a matching record is not
**        found, the pointer returned is NULL.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] **CritRegRecPtr    Pointer to a pointer that should be initialized with
**                               the start address of the located Critical Table Registry
**                               Record. 
**
** \param[in]  CDSHandleToFind   CDS Handle to be located in Critical Table Registry.
** 
** \param[out] *CritRegRecPtr    Pointer to the start address of the located Critical
**                               Table Registry Record.  \c NULL if the record is not
**                               found.
**
******************************************************************************/
void CFE_TBL_FindCriticalTblInfo(CFE_TBL_CritRegRec_t **CritRegRecPtr, CFE_ES_CDSHandle_t CDSHandleToFind);


/*****************************************************************************/
/**
** \brief Updates a CDS associated with a Critical Table
**
** \par Description
**        Copies the contents of the active buffer into a previously allocated
**        CDS associated with the table.  The Critical Table Registry is also
**        updated and copied into the CDS to keep relevant information on the
**        source of the data contained in the table.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  RegRecPtr Pointer to Registry Record of Critical Table whose CDS
**                       needs to be updated.
** 
**                     
******************************************************************************/
void CFE_TBL_UpdateCriticalTblCDS(CFE_TBL_RegistryRec_t *RegRecPtr);


/*****************************************************************************/
/**
** \brief When enabled, will send a manage notification command message
**
** \par Description
**        Whenever an application uses the #CFE_TBL_NotifyByMessage API, Table services
**        will call this routine whenever a table requires management by the owning
**        Application.  This routine will then issue the appropriate message to the
**        software bus.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  RegRecPtr Pointer to Registry Record of Table whose owner needs notifying.
** 
**                     
******************************************************************************/
int32 CFE_TBL_SendNotificationMsg(CFE_TBL_RegistryRec_t *RegRecPtr);


/*****************************************************************************/
/**
** \brief Gathers data and puts it into the Housekeeping Message format
**
** \par Description
**        Gathers data from the Table Services Application, computes necessary data values and identifies
**        what Table Validation information needs to be reported in Housekeeping Telemetry.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
extern  void CFE_TBL_GetHkData(void);


/*****************************************************************************/
/**
** \brief Convert Table Registry Entry for a Table into a Message
**
** \par Description
**        Extracts the Table Registry information for the table specified by the
**        #CFE_TBL_TaskData_t::HkTlmTblRegIndex variable.  It then formats the
**        Registry contents into a format appropriate for downlink.
**
** \par Assumptions, External Events, and Notes:
**        #CFE_TBL_TaskData_t::HkTlmTblRegIndex is assumed to be a valid index into
**           the Table Registry.
**
** \retval None
******************************************************************************/
extern void CFE_TBL_GetTblRegData(void);


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
extern void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr);


/*
** Globals specific to the TBL module
*/
extern CFE_TBL_TaskData_t CFE_TBL_TaskData;



#endif /* _cfe_tbl_internal_ */
