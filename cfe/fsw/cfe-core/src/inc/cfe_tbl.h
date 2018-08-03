/*
**
**  $Id: cfe_tbl.h 1.9 2014/08/24 16:59:51GMT-05:00 sstrege Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**  Title:   Table Services API Application Library Header File
**
**  Purpose:
**     Unit specification for Table services library functions and macros.
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**  Notes:
**
**  $Log: cfe_tbl.h  $
**  Revision 1.9 2014/08/24 16:59:51GMT-05:00 sstrege 
**  Updated CFE_TBL_MAX_FULL_NAME_LEN definition to always fall on 4-byte boundary
**  Revision 1.8 2010/10/27 16:34:31EDT dkobe 
**  Added CRC field to Table Buffer data structure
**  Revision 1.7 2010/10/27 13:55:07EDT dkobe 
**  Added TBL Notification API
**  Revision 1.6 2010/10/04 15:24:59EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.5 2009/05/01 16:38:54EDT dkobe 
**  Updated documentation for CFE_TBL_Register dealing with Validation functions
**  Revision 1.4 2009/05/01 14:26:33EDT dkobe 
**  Modified return code contents to emphasize success vs failure of API calls
**  Revision 1.3 2008/07/29 18:32:18EDT dkobe 
**  Added CFE_TBL_Modified API
**  Revision 1.2 2008/07/29 15:55:19EDT dkobe 
**  Moved CFE_TBL_DumpToBuffer from cfe_tbl_internal.c to cfe_tbl_api.c
**  Revision 1.1 2008/04/17 08:05:24EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
**  Revision 1.13 2007/05/23 11:22:00EDT David Kobe (dlkobe) 
**  Added doxygen formatting
**  Revision 1.12 2007/04/28 14:48:57EDT dlkobe 
**  Baseline Implementation of Critical Tables
**  Revision 1.11 2007/03/28 15:18:17EST rjmcgraw 
**  Moved file related defines from cfe_tbl.h to cfe_fs.h
**  Revision 1.10 2006/11/03 15:41:43EST dlkobe 
**  Modified cfe_tbl.h so that the CFE_TBL_OPT_USR_DEF_ADDR 
**  macro includes the CFE_TBL_OPT_DUMP_ONLY option
**
**/

#ifndef _cfe_tbl_
#define _cfe_tbl_

/********************* Include Files  ************************/
#include "common_types.h"  /* Basic Data Types */
#include "cfe_time.h"
#include "osconfig.h"

/******************* Macro Definitions ***********************/
#define CFE_TBL_OPT_BUFFER_MSK   (0x0001)
#define CFE_TBL_OPT_SNGL_BUFFER  (0x0000)
#define CFE_TBL_OPT_DBL_BUFFER   (0x0001)

#define CFE_TBL_OPT_LD_DMP_MSK   (0x0002)
#define CFE_TBL_OPT_LOAD_DUMP    (0x0000)
#define CFE_TBL_OPT_DUMP_ONLY    (0x0002)

#define CFE_TBL_OPT_USR_DEF_MSK  (0x0004)
#define CFE_TBL_OPT_NOT_USR_DEF  (0x0000)
#define CFE_TBL_OPT_USR_DEF_ADDR (0x0006) /**< \brief NOTE: Automatically includes #CFE_TBL_OPT_DUMP_ONLY option */

#define CFE_TBL_OPT_CRITICAL_MSK (0x0008)
#define CFE_TBL_OPT_NOT_CRITICAL (0x0000)
#define CFE_TBL_OPT_CRITICAL     (0x0008)

#define CFE_TBL_OPT_DEFAULT      (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP)

/** Computation for maximum length allowed for a table name. <BR>
** NOTE: "+2" is for NULL Character and "." (i.e. - "AppName.TblName") */
#define CFE_TBL_MAX_FULL_NAME_LEN_COMP (CFE_TBL_MAX_NAME_LENGTH + OS_MAX_API_NAME + 2)
/* Ensure the table name falls on a 4-byte boundary */
#define CFE_TBL_MAX_FULL_NAME_LEN (((CFE_TBL_MAX_FULL_NAME_LEN_COMP + 3)/4)*4)

#define CFE_TBL_BAD_TABLE_HANDLE  (CFE_TBL_Handle_t) 0xFFFF

/******************  Data Type Definitions *********************/

typedef int32 (*CFE_TBL_CallbackFuncPtr_t)(void *TblPtr);

typedef int16 CFE_TBL_Handle_t;

typedef enum 
{
    CFE_TBL_SRC_FILE = 0, /**< When this option is selected, the \c SrcDataPtr 
                               will be interpreted as a pointer to a null 
                               terminated character string.  The string should 
                               specify the full path and filename of the file 
                               containing the initial data contents of the table. */
    CFE_TBL_SRC_ADDRESS   /**< When this option is selected, the \c SrcDataPtr will
                               be interpreted as a pointer to a memory location 
                               that is the beginning of the initialization data 
                               for loading the table OR, in the case of a "user defined"
                               dump only table, the address of the active table itself.
                               The block of memory is assumed to be of the same size 
                               specified in the #CFE_TBL_Register function Size parameter. */
} CFE_TBL_SrcEnum_t;

typedef struct
{
    uint32                Size;                             /**< \brief Size, in bytes, of Table */
    uint32                NumUsers;                         /**< \brief Number of Apps with access to the table */
    uint32                FileCreateTimeSecs;               /**< \brief File creation time from last file loaded into table */
    uint32                FileCreateTimeSubSecs;            /**< \brief File creation time from last file loaded into table */
    uint32                Crc;                              /**< \brief Most recently calculated CRC by TBL services on table contents */
    CFE_TIME_SysTime_t    TimeOfLastUpdate;                 /**< \brief Time when Table was last updated */
    boolean               TableLoadedOnce;                  /**< \brief Flag indicating whether table has been loaded once or not */
    boolean               DumpOnly;                         /**< \brief Flag indicating Table is NOT to be loaded */
    boolean               DblBuffered;                      /**< \brief Flag indicating Table has a dedicated inactive buffer */
    boolean               UserDefAddr;                      /**< \brief Flag indicating Table address was defined by Owner Application */
    boolean               Critical;                         /**< \brief Flag indicating Table contents are maintained in a CDS */
    char                  LastFileLoaded[OS_MAX_PATH_LEN];  /**< \brief Filename of last file loaded into table */
} CFE_TBL_Info_t;

/*************************** Function Prototypes ******************************/
/**
** \brief Register a table with cFE to obtain Table Management Services
**
** \par Description
**          When an application is created and initialized, it is responsible for creating its table images via 
**          the TBL API.  The application must inform the Table Service of the table name, table size and selection 
**          of optional table features.
**
** \par Assumptions, External Events, and Notes:
**          Note: This function call can block.  Therefore, interrupt service routines should NOT create 
**                their own tables.  An application should create any table(s) and provide the handle(s) 
**                to the interrupt service routine.
**
** \param[in] TblHandlePtr         a pointer to a #CFE_TBL_Handle_t type variable that will be assigned the 
**                                 table's handle.  The table handle is required for other API calls when 
**                                 accessing the data contained in the table.
**
** \param[in] Name                 The application-specific name.  This name will be combined with the name of the 
**                                 application to produce a processor specific name of the form 
**                                 "ApplicationName.TableName".  The processor specific name will be used in commands 
**                                 for modifying or viewing the contents of the table.
**
** \param[in] Size                 The size, in bytes, of the table to be created.  This is the size that will be 
**                                 allocated as a shared memory resource between the Table Management Service and 
**                                 the calling application.
**
** \param[in] TblOptionFlags       Flag bits indicating selected options for table.  A bitwise OR of the following 
**                                 option flags:
**                                 \arg #CFE_TBL_OPT_DEFAULT     - The default setting for table options is a 
**                                                                 combination of #CFE_TBL_OPT_SNGL_BUFFER and 
**                                                                 #CFE_TBL_OPT_LOAD_DUMP.  See below for a 
**                                                                 description of these two options.  This option 
**                                                                 is mutually exclusive with the 
**                                                                 #CFE_TBL_OPT_DBL_BUFFER, #CFE_TBL_OPT_DUMP_ONLY 
**                                                                 and #CFE_TBL_OPT_USR_DEF_ADDR options.
**                                 \arg #CFE_TBL_OPT_SNGL_BUFFER - When this option is selected, the table will use 
**                                                                 a shared session table for performing table 
**                                                                 modifications and a memory copy from the session 
**                                                                 table to the "active" table buffer will occur 
**                                                                 when the table is updated.  This is the preferred 
**                                                                 option since it will minimize memory usage.  
**                                                                 This option is mutually exclusive with the 
**                                                                 #CFE_TBL_OPT_DBL_BUFFER option
**                                 \arg #CFE_TBL_OPT_DBL_BUFFER  - When this option is selected, two instances of 
**                                                                 the table are created.   One is considered the 
**                                                                 "active" table and the other the "inactive" table.
**                                                                 Whenever table modifications occur, they do not 
**                                                                 require the use of a common session table.  
**                                                                 Modifications occur in the "inactive" buffer.  
**                                                                 Then, when it is time to update the table, 
**                                                                 the pointer to the "active" table is changed to 
**                                                                 point to the "inactive" buffer thus making it 
**                                                                 the new "active" buffer.  This feature is most 
**                                                                 useful for time critical applications (ie - 
**                                                                 interrupt service routines, etc).  This option is 
**                                                                 mutually exclusive with the 
**                                                                 #CFE_TBL_OPT_SNGL_BUFFER and #CFE_TBL_OPT_DEFAULT 
**                                                                 option.
**                                 \arg #CFE_TBL_OPT_LOAD_DUMP   - When this option is selected, the Table Service 
**                                                                 is allowed to perform all operations on the 
**                                                                 specified table.  This option is mutually 
**                                                                 exclusive with the #CFE_TBL_OPT_DUMP_ONLY option.
**                                 \arg #CFE_TBL_OPT_DUMP_ONLY   - When this option is selected, the Table Service 
**                                                                 will not perform table loads to this table.  This 
**                                                                 does not prevent, however, a task from writing 
**                                                                 to the table via an address obtained with the 
**                                                                 #CFE_TBL_GetAddress API function.  This option is 
**                                                                 mutually exclusive with the #CFE_TBL_OPT_LOAD_DUMP
**                                                                 and #CFE_TBL_OPT_DEFAULT options. If the Application wishes
**                                                                 to specify their own block of memory as the Dump Only table,
**                                                                 they need to also include the #CFE_TBL_OPT_USR_DEF_ADDR option
**                                                                 explained below.
**                                 \arg #CFE_TBL_OPT_NOT_USR_DEF - When this option is selected, Table Services allocates
**                                                                 memory for the table and, in the case of a double buffered
**                                                                 table, it allocates the same amount of memory again for the
**                                                                 second buffer.  This option is mutually exclusive with
**                                                                 the #CFE_TBL_OPT_USR_DEF_ADDR option.
**                                 \arg #CFE_TBL_OPT_USR_DEF_ADDR- When this option is selected, the Table Service 
**                                                                 will not allocate memory for the table.  Table Services
**                                                                 will require the Application to identify the location of the
**                                                                 active table buffer via the #CFE_TBL_Load function. This 
**                                                                 option implies the #CFE_TBL_OPT_DUMP_ONLY and the 
**                                                                 #CFE_TBL_OPT_SNGL_BUFFER options and is mutually 
**                                                                 exclusive of the #CFE_TBL_OPT_DBL_BUFFER option.
**                                 \arg #CFE_TBL_OPT_CRITICAL-     When this option is selected, the Table Service 
**                                                                 will automatically allocate space in the Critical Data Store (CDS)
**                                                                 for the table and insure that the contents in the CDS are the same
**                                                                 as the contents of the currently active buffer for the table.
**                                                                 This option is mutually exclusive of the #CFE_TBL_OPT_USR_DEF_ADDR
**                                                                 and #CFE_TBL_OPT_DUMP_ONLY options.  It should also be noted that
**                                                                 the use of this option with double buffered tables will prevent
**                                                                 the update of the double buffered table from being quick and
**                                                                 it could be blocked.  Therefore, critical tables should not be
**                                                                 updated by Interrupt Service Routines.
**
** \param[in] TblValidationFuncPtr is a pointer to a function that will be executed in the context of the Table 
**                                 Management Service when the contents of a table need to be validated.  If set 
**                                 to NULL, then the Table Management Service will assume any data is valid.  If 
**                                 the value is not NULL, it must be a pointer to a function with the following 
**                                 prototype: <BR>
**                                 <B> int32 CallbackFunc(void *TblPtr);</B> <BR>
**                                 where  <BR>
**                                 <B>TblPtr </B> will be a pointer to the table data that is to be verified.  When the 
**                                 function returns #CFE_SUCCESS, the data is considered valid and ready for a commit.  
**                                 When the function returns a negative value, the data is considered invalid and an 
**                                 Event Message will be issued containing the returned value.  If the function should
**                                 return a positive number, the table is considered invalid and the return code is
**                                 considered invalid.  Validation functions \b must return either #CFE_SUCCESS or a negative
**                                 number (whose value is at the developer's discretion).  The validation function 
**                                 will be executed in the Application's context so that Event Messages describing the 
**                                 validation failure are possible from within the function.
**
** \param[out]  *TblHandlePtr      Handle used to identify table to cFE when performing Table operations.
**                                 This value is returned at the address specified by TblHandlePtr.
**
** \returns
** \retcode #CFE_SUCCESS                      \retdesc \copydoc CFE_SUCCESS                        \endcode
** \retcode #CFE_TBL_INFO_RECOVERED_TBL       \retdesc \copydoc CFE_TBL_INFO_RECOVERED_TBL         \endcode
** \retcode                                   \retdesc <BR><BR>                                    \endcode
** \retcode #CFE_TBL_ERR_DUPLICATE_DIFF_SIZE  \retdesc \copydoc CFE_TBL_ERR_DUPLICATE_DIFF_SIZE    \endcode
** \retcode #CFE_TBL_ERR_DUPLICATE_NOT_OWNED  \retdesc \copydoc CFE_TBL_ERR_DUPLICATE_NOT_OWNED    \endcode
** \retcode #CFE_TBL_ERR_REGISTRY_FULL        \retdesc \copydoc CFE_TBL_ERR_REGISTRY_FULL          \endcode
** \retcode #CFE_TBL_ERR_HANDLES_FULL         \retdesc \copydoc CFE_TBL_ERR_HANDLES_FULL           \endcode
** \retcode #CFE_TBL_ERR_INVALID_SIZE         \retdesc \copydoc CFE_TBL_ERR_INVALID_SIZE           \endcode
** \retcode #CFE_TBL_ERR_INVALID_NAME         \retdesc \copydoc CFE_TBL_ERR_INVALID_NAME           \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID           \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID             \endcode
** \retcode #CFE_ES_ERR_APPNAME               \retdesc \copydoc CFE_ES_ERR_APPNAME                 \endcode
** \retcode #CFE_ES_ERR_BUFFER                \retdesc \copydoc CFE_ES_ERR_BUFFER                  \endcode
** \endreturns
**
** \sa #CFE_TBL_Unregister, #CFE_TBL_Share
**/
int32 CFE_TBL_Register( CFE_TBL_Handle_t *TblHandlePtr,                   /* Returned Handle */
                        const char   *Name,                               /* Application specific name  */
                        uint32  Size,                                     /* Size, in bytes, of table   */
                        uint16  TblOptionFlags,                           /* Tbl Options Settings     */
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr ); /* Ptr to func that validates tbl */

/*****************************************************************************/
/**
** \brief Obtain handle of table registered by another application
**
** \par Description
**        After a table has been created, other applications can gain access 
**        to that table via the table handle.  In order for two or more 
**        applications to share a table, the applications that do not create 
**        the table must obtain the handle using this function.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblHandlePtr  A pointer to a #CFE_TBL_Handle_t type variable 
**                           that will be assigned the table's handle.  The 
**                           table handle is required for other API calls 
**                           when accessing the data contained in the table.
**
** \param[in]  TblName       The processor specific name of the table.  It is important to note 
**                           that the processor specific table name is different from the table 
**                           name specified in the #CFE_TBL_Register API call.  The processor 
**                           specific table name includes the name of the application that created 
**                           the table.  The name would be of the form "ApplicationName.TableName".  
**                           An example of this would be "ACS.TamParams" for a table called "TamParams" 
**                           that was registered by the application called "ACS".
**
** \param[out] *TblHandlePtr Handle used to identify table to cFE when performing Table operations.
**                           This value is returned at the address specified by TblHandlePtr.
**
** \returns
** \retcode #CFE_SUCCESS              \retdesc \copydoc CFE_SUCCESS               \endcode
** \retcode                           \retdesc <BR><BR>                           \endcode
** \retcode #CFE_TBL_ERR_HANDLES_FULL \retdesc \copydoc CFE_TBL_ERR_HANDLES_FULL  \endcode
** \retcode #CFE_TBL_ERR_INVALID_NAME \retdesc \copydoc CFE_TBL_ERR_INVALID_NAME  \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID   \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID    \endcode
** \retcode #CFE_ES_ERR_APPNAME       \retdesc \copydoc CFE_ES_ERR_APPNAME        \endcode
** \retcode #CFE_ES_ERR_BUFFER        \retdesc \copydoc CFE_ES_ERR_BUFFER         \endcode
** \endreturns
**
** \sa #CFE_TBL_Unregister, #CFE_TBL_Register
**
******************************************************************************/
int32 CFE_TBL_Share( CFE_TBL_Handle_t *TblHandlePtr,      /* Returned Handle */
                     const char *TblName );

/*****************************************************************************/
/**
** \brief Unregister a previously registered table and free associated resources
**
** \par Description
**        When an application is being removed from the system, it should 
**        unregister those tables that it created.  The application should 
**        call this function as a part of its cleanup process.  The table 
**        will be removed from memory once all table addresses referencing 
**        it have been released.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                      that identifies the Table to be unregistered.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \endreturns
**
** \sa #CFE_TBL_Share, #CFE_TBL_Register
** 
******************************************************************************/
int32 CFE_TBL_Unregister ( CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Load a specified table with data from specified source
**
** \par Description
**        Once an application has created a table (#CFE_TBL_Register), it must 
**        provide the values that initialize the contents of that table.  The 
**        application accomplishes this with one of two different TBL API calls.  
**        This function call initializes the table with values that are held 
**        in a data structure.  
**
** \par Assumptions, External Events, and Notes:
**        This function call can block.  Therefore, interrupt service routines 
**        should NOT initialize their own tables.  An application should initialize 
**        any table(s) prior to providing the handle(s) to the interrupt service routine.
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be loaded.
**
** \param[in] SrcType    Flag indicating the nature of the given \c SrcDataPtr below.
**                       This value can be any one of the following:
**                          \arg #CFE_TBL_SRC_FILE    - \copydoc CFE_TBL_SRC_FILE
**                          \arg #CFE_TBL_SRC_ADDRESS - \copydoc CFE_TBL_SRC_ADDRESS 
**                                                      
** \param[in] SrcDataPtr Pointer to either a character string specifying a filename or
**                       a memory address of a block of binary data to be loaded into a table or,
**                       if the table was registered with the #CFE_TBL_OPT_USR_DEF_ADDR option,
**                       the address of the active table buffer.
**
** \returns
** \retcode #CFE_SUCCESS                   \retdesc \copydoc CFE_SUCCESS                   \endcode
** \retcode #CFE_TBL_WARN_SHORT_FILE       \retdesc \copydoc CFE_TBL_WARN_SHORT_FILE       \endcode
** \retcode #CFE_TBL_WARN_PARTIAL_LOAD     \retdesc \copydoc CFE_TBL_WARN_PARTIAL_LOAD     \endcode
** \retcode                                \retdesc <BR><BR>                               \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID        \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID        \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS         \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS         \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE    \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE    \endcode
** \retcode #CFE_ES_ERR_APPNAME            \retdesc \copydoc CFE_ES_ERR_APPNAME            \endcode
** \retcode #CFE_ES_ERR_BUFFER             \retdesc \copydoc CFE_ES_ERR_BUFFER             \endcode
** \retcode #CFE_TBL_ERR_DUMP_ONLY         \retdesc \copydoc CFE_TBL_ERR_DUMP_ONLY         \endcode
** \retcode #CFE_TBL_ERR_ILLEGAL_SRC_TYPE  \retdesc \copydoc CFE_TBL_ERR_ILLEGAL_SRC_TYPE  \endcode
** \retcode #CFE_TBL_ERR_LOAD_IN_PROGRESS  \retdesc \copydoc CFE_TBL_ERR_LOAD_IN_PROGRESS  \endcode
** \retcode #CFE_TBL_ERR_NO_BUFFER_AVAIL   \retdesc \copydoc CFE_TBL_ERR_NO_BUFFER_AVAIL   \endcode
** \retcode #CFE_TBL_ERR_FILE_NOT_FOUND    \retdesc \copydoc CFE_TBL_ERR_FILE_NOT_FOUND    \endcode
** \retcode #CFE_TBL_ERR_FILE_TOO_LARGE    \retdesc \copydoc CFE_TBL_ERR_FILE_TOO_LARGE    \endcode
** \retcode #CFE_TBL_ERR_BAD_CONTENT_ID    \retdesc \copydoc CFE_TBL_ERR_BAD_CONTENT_ID    \endcode
** \retcode #CFE_TBL_ERR_PARTIAL_LOAD      \retdesc \copydoc CFE_TBL_ERR_PARTIAL_LOAD      \endcode
** \endreturns
**
** \sa #CFE_TBL_Update, #CFE_TBL_Validate, #CFE_TBL_Manage 
**
******************************************************************************/
int32 CFE_TBL_Load( CFE_TBL_Handle_t TblHandle,
                    CFE_TBL_SrcEnum_t SrcType,
                    const void *SrcDataPtr );

/*****************************************************************************/
/**
** \brief Update contents of a specified table, if an update is pending
**
** \par Description
**        An application is \b required to perform a periodic check for an update 
**        for all the tables that it creates.  Typically, the application that 
**        created the table would call this function at the start or conclusion 
**        of any routine processing cycle or at regular intervals.  To determine 
**        whether an update is pending prior to making this call, the Application 
**        can use the #CFE_TBL_GetStatus API first.  If a table update is pending, 
**        it will take place during this function call.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be updated.
**                                      
** \returns
** \retcode #CFE_SUCCESS                    \retdesc \copydoc CFE_SUCCESS                     \endcode
** \retcode #CFE_TBL_INFO_NO_UPDATE_PENDING \retdesc \copydoc CFE_TBL_INFO_NO_UPDATE_PENDING  \endcode
** \retcode                                 \retdesc <BR><BR>                                 \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID         \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID          \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS          \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS           \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE     \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE      \endcode
** \retcode #CFE_ES_ERR_APPNAME             \retdesc \copydoc CFE_ES_ERR_APPNAME              \endcode
** \retcode #CFE_ES_ERR_BUFFER              \retdesc \copydoc CFE_ES_ERR_BUFFER               \endcode
** \endreturns
**
** \sa #CFE_TBL_Load, #CFE_TBL_Validate, #CFE_TBL_Manage
**
******************************************************************************/
int32 CFE_TBL_Update( CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Obtain the current address of the contents of the specified table
**
** \par Description
**        When a table has been created and initialized, it is available to 
**        any application that can identify it with its unique handle.  In 
**        order to view the data contained in the table, an application must 
**        call this function or #CFE_TBL_GetAddresses.
**
** \par Assumptions, External Events, and Notes:
**        -# This call can be a blocking call when the table is not double buffered 
**           and is shared with another application of lower priority that just happens 
**           to be in the middle of a table update of the specific table.  If this occurs, 
**           the application performing the table update will automatically have its 
**           priority elevated in order to release the resource as soon as possible.
**        -# An application must always release the returned table address using the 
**           #CFE_TBL_ReleaseAddress or #CFE_TBL_ReleaseAddresses function prior to 
**           either a #CFE_TBL_Update call or any blocking call (e.g. - pending on software 
**           bus message, etc).  Table updates cannot occur while table addresses have not 
**           been released.
**
** \param[in]  TblPtr     The address of a pointer that will be loaded with the address of 
**                        the first byte of the table.  This pointer can then be typecast 
**                        by the calling application to the appropriate table data structure.
**
** \param[in]  TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                        identifies the Table whose address is to be returned.
**
** \param[out] *TblPtr    Address of the first byte of data associated with the specified table.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_TBL_INFO_UPDATED       \retdesc \copydoc CFE_TBL_INFO_UPDATED        \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_UNREGISTERED   \retdesc \copydoc CFE_TBL_ERR_UNREGISTERED    \endcode
** \retcode #CFE_TBL_ERR_NEVER_LOADED   \retdesc \copydoc CFE_TBL_ERR_NEVER_LOADED    \endcode
** \endreturns
**
** \sa #CFE_TBL_ReleaseAddress, #CFE_TBL_GetAddresses, #CFE_TBL_ReleaseAddresses
**
******************************************************************************/
int32 CFE_TBL_GetAddress( void **TblPtr,
                          CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Release previously obtained pointer to the contents of the specified table
**
** \par Description
**        Each application is \b required to release a table address obtained through 
**        the #CFE_TBL_GetAddress function.
**
** \par Assumptions, External Events, and Notes:
**        An application must always release the returned table address using the 
**        #CFE_TBL_ReleaseAddress function prior to either a #CFE_TBL_Update call 
**        or any blocking call (e.g. - pending on software bus message, etc).  
**        Table updates cannot occur while table addresses have not been released.
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table whose address is to be released.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_TBL_INFO_UPDATED       \retdesc \copydoc CFE_TBL_INFO_UPDATED        \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_NEVER_LOADED   \retdesc \copydoc CFE_TBL_ERR_NEVER_LOADED    \endcode
** \endreturns
**
** \sa #CFE_TBL_GetAddress, #CFE_TBL_GetAddresses, #CFE_TBL_ReleaseAddresses
**
******************************************************************************/
int32 CFE_TBL_ReleaseAddress( CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Obtain the current addresses of an array of specified tables
**
** \par Description
**        When a table has been created and initialized, it is available to 
**        any application that can identify it with its unique handle.  In 
**        order to view the data contained in the table, an application must 
**        call this function or #CFE_TBL_GetAddresses.
**
** \par Assumptions, External Events, and Notes:
**        -# This call can be a blocking call when the table is not double buffered 
**           and is shared with another application of lower priority that just happens 
**           to be in the middle of a table update of the specific table.  If this occurs, 
**           the application performing the table update will automatically have its 
**           priority elevated in order to release the resource as soon as possible.
**        -# An application must always release the returned table address using the 
**           #CFE_TBL_ReleaseAddress or #CFE_TBL_ReleaseAddresses function prior to 
**           either a #CFE_TBL_Update call or any blocking call (e.g. - pending on software 
**           bus message, etc).  Table updates cannot occur while table addresses have not 
**           been released.
**
** \param[in] TblPtrs    Array of Pointers to variables that calling Application
**                       wishes to hold the start addresses of the Tables.
**
** \param[in] NumTables  Size of TblPtrs and TblHandles arrays.
**
** \param[in] TblHandles Array of Table Handles, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                       of those tables whose start addresses are to be obtained.
**
** \param[out] *TblPtrs  Array of addresses of the first byte of data associated with the
**                       specified tables.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                \endcode
** \retcode #CFE_TBL_INFO_UPDATED       \retdesc \copydoc CFE_TBL_INFO_UPDATED       \endcode
** \retcode                             \retdesc <BR><BR>                            \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID     \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS      \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME         \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER          \endcode
** \retcode #CFE_TBL_ERR_UNREGISTERED   \retdesc \copydoc CFE_TBL_ERR_UNREGISTERED   \endcode
** \retcode #CFE_TBL_ERR_NEVER_LOADED   \retdesc \copydoc CFE_TBL_ERR_NEVER_LOADED   \endcode
** \endreturns
**
** \sa #CFE_TBL_GetAddress, #CFE_TBL_ReleaseAddress, #CFE_TBL_ReleaseAddresses
**
******************************************************************************/
int32 CFE_TBL_GetAddresses( void **TblPtrs[],
                            uint16 NumTables,
                            const CFE_TBL_Handle_t TblHandles[] );

/*****************************************************************************/
/**
** \brief Release the addresses of an array of specified tables
**
** \par Description
**        Each application is \b required to release a table address obtained through 
**        the #CFE_TBL_GetAddress function.
**
** \par Assumptions, External Events, and Notes:
**        An application must always release the returned table address using the 
**        #CFE_TBL_ReleaseAddress function prior to either a #CFE_TBL_Update call 
**        or any blocking call (e.g. - pending on software bus message, etc).  
**        Table updates cannot occur while table addresses have not been released.
**
** \param[in] NumTables  Size of TblHandles array.
**
** \param[in] TblHandles Array of Table Handles, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                       of those tables whose start addresses are to be released.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_TBL_INFO_UPDATED       \retdesc \copydoc CFE_TBL_INFO_UPDATED        \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_NEVER_LOADED   \retdesc \copydoc CFE_TBL_ERR_NEVER_LOADED    \endcode
** \endreturns
**
** \sa #CFE_TBL_GetAddress, #CFE_TBL_ReleaseAddress, #CFE_TBL_GetAddresses
**
******************************************************************************/
int32 CFE_TBL_ReleaseAddresses( uint16 NumTables,
                                const CFE_TBL_Handle_t TblHandles[] );

/*****************************************************************************/
/**
** \brief Perform steps to validate the contents of a table image.
**
** \par Description
**        An application is \b required to perform a periodic check for an update 
**        or a validation request for all the tables that it creates.  Typically, 
**        the application that created the table would call this function at the 
**        start or conclusion of any routine processing cycle.  To determine whether 
**        a validation request is pending prior to making this call, the Application 
**        can use the #CFE_TBL_GetStatus API first.  If a table validation is pending, 
**        the Application would call this function to perform the necessary actions.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \returns
** \retcode #CFE_SUCCESS                        \retdesc \copydoc CFE_SUCCESS                        \endcode
** \retcode #CFE_TBL_INFO_NO_VALIDATION_PENDING \retdesc \copydoc CFE_TBL_INFO_NO_VALIDATION_PENDING \endcode
** \retcode                                     \retdesc <BR><BR>                                    \endcode
** \retcode #CFE_ES_ERR_APPNAME                 \retdesc \copydoc CFE_ES_ERR_APPNAME                 \endcode
** \retcode #CFE_ES_ERR_BUFFER                  \retdesc \copydoc CFE_ES_ERR_BUFFER                  \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID             \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID             \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS              \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS              \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE         \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE         \endcode
** \endreturns
**
** \sa #CFE_TBL_Update, #CFE_TBL_Manage, #CFE_TBL_Load
**
******************************************************************************/
int32 CFE_TBL_Validate( CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Perform standard operations to maintain a table.
**
** \par Description
**        An application is \b required to perform a periodic check for an update 
**        or a validation request for all the tables that it creates.  Typically, 
**        the application that created the table would call this function at the 
**        start or conclusion of any routine processing cycle.  If a table update 
**        or validation request is pending, this function would perform either or 
**        both before returning.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_TBL_INFO_UPDATED       \retdesc \copydoc CFE_TBL_INFO_UPDATED        \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \endreturns
**
** \sa #CFE_TBL_Update, #CFE_TBL_Validate, #CFE_TBL_Load, #CFE_TBL_DumpToBuffer
**
******************************************************************************/
int32 CFE_TBL_Manage( CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Obtain current status of pending actions for a table.
**
** \par Description
**        An application is \b required to perform a periodic check for an update 
**        or a validation request for all the tables that it creates.  Typically, 
**        the application that created the table would call this function at the 
**        start or conclusion of any routine processing cycle.  If a table update 
**        or validation request is pending, the Application should follow up with 
**        a call to #CFE_TBL_Update or #CFE_TBL_Validate respectively.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \returns
** \retcode #CFE_SUCCESS                     \retdesc \copydoc CFE_SUCCESS                      \endcode
** \retcode #CFE_TBL_INFO_UPDATE_PENDING     \retdesc \copydoc CFE_TBL_INFO_UPDATE_PENDING      \endcode
** \retcode #CFE_TBL_INFO_VALIDATION_PENDING \retdesc \copydoc CFE_TBL_INFO_VALIDATION_PENDING  \endcode
** \retcode #CFE_TBL_INFO_DUMP_PENDING       \retdesc \copydoc CFE_TBL_INFO_DUMP_PENDING        \endcode
** \retcode                                  \retdesc <BR><BR>                                  \endcode
** \retcode #CFE_ES_ERR_APPNAME              \retdesc \copydoc CFE_ES_ERR_APPNAME               \endcode
** \retcode #CFE_ES_ERR_BUFFER               \retdesc \copydoc CFE_ES_ERR_BUFFER                \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID          \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID           \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS           \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS            \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE      \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE       \endcode
** \endreturns
**
** \sa #CFE_TBL_Manage, #CFE_TBL_Update, #CFE_TBL_Validate, #CFE_TBL_GetInfo
**
******************************************************************************/
int32 CFE_TBL_GetStatus( CFE_TBL_Handle_t TblHandle );


/*****************************************************************************/
/**
** \brief Obtain characteristics/information of/about a specified table.
**
** \par Description
**        This API provides the registry information associated with the specified
**        table.  The function fills the given data structure with the data found
**        in the Table Registry.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblInfoPtr    A pointer to a CFE_TBL_Info_t data structure that is to be populated
**                           with table characteristics and information.
** 
** \param[in]  TblName       The processor specific name of the table.  It is important to note 
**                           that the processor specific table name is different from the table 
**                           name specified in the #CFE_TBL_Register API call.  The processor 
**                           specific table name includes the name of the application that created 
**                           the table.  The name would be of the form "ApplicationName.TableName".  
**                           An example of this would be "ACS.TamParams" for a table called "TamParams" 
**                           that was registered by the application called "ACS".
**
** \param[out] *TblInfoPtr   Description of the tables characteristics and registry information stored in
**                           the #CFE_TBL_Info_t data structure format.
**
** \returns
** \retcode #CFE_SUCCESS                     \retdesc \copydoc CFE_SUCCESS               \endcode      
** \retcode                                  \retdesc <BR><BR>                           \endcode
** \retcode #CFE_TBL_ERR_INVALID_NAME        \retdesc \copydoc CFE_TBL_ERR_INVALID_NAME  \endcode
** \endreturns
**
** \sa #CFE_TBL_GetStatus
**
******************************************************************************/
int32 CFE_TBL_GetInfo( CFE_TBL_Info_t *TblInfoPtr, const char *TblName );

/*****************************************************************************/
/**
** \brief Copies the contents of a Dump Only Table to a shared buffer
**
** \par Description
**        Copies contents of a Dump Only table to a shared buffer so that it
**        can be written to a file by the Table Services routine.  This function
**        is called by the Application that owns the table in response to a #CFE_TBL_INFO_DUMP_PENDING
**        status obtained via #CFE_TBL_GetStatus.
**
** \par Assumptions, External Events, and Notes:
**        -# If the table does not have a dump pending status, nothing will occur (no error, no dump)
**        -# Applications may wish to use this function in lieu of #CFE_TBL_Manage for their Dump Only tables
**
** \param[in]  TblHandle      Handle of Table to be dumped.
** 
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \endreturns
**
** \sa #CFE_TBL_Manage
**
******************************************************************************/
int32   CFE_TBL_DumpToBuffer( CFE_TBL_Handle_t TblHandle );

/*****************************************************************************/
/**
** \brief Notify cFE Table Services that table contents have been modified by the Application
**
** \par Description
**        This API notifies Table Services that the contents of the specified table has been
**        modified by the Application.  This notification is important when a table has been
**        registered as "Critical" because Table Services can then update the contents of the
**        table kept in the Critical Data Store.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]  TblHandle      Handle of Table that was modified.
** 
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \endreturns
**
** \sa #CFE_TBL_Manage
**
******************************************************************************/
int32   CFE_TBL_Modified( CFE_TBL_Handle_t TblHandle );
 

/*****************************************************************************/
/**
** \brief Instruct cFE Table Services to notify Application via message when table requires management
**
** \par Description
**        This API instructs Table Services to send a message to the calling Application
**        whenever the specified table requires management by the application.  This feature
**        allows applications to avoid polling table services via the #CFE_TBL_Manage call
**        to determine whether a table requires updates, validation, etc.  This API should
**        be called following the #CFE_TBL_Register API whenever the owning application requires
**        this feature.
**
** \par Assumptions, External Events, and Notes:
**        - Only the application that owns the table is allowed to register a notification message
**
** \param[in]  TblHandle      Handle of Table with which the message should be associated.
** 
** \param[in]  MsgId          Message ID to be used in notification message sent by Table Services.
** 
** \param[in]  CommandCode    Command Code value to be placed in secondary header of message 
**                            sent by Table Services.
** 
** \param[in]  Parameter      Application defined value to be passed as a parameter in the 
**                            message sent by Table Services.  Suggested use includes an application's
**                            table index that allows the same MsgId and Command Code to be used for
**                            all table management notifications.
** 
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                 \endcode
** \retcode                             \retdesc <BR><BR>                             \endcode
** \retcode #CFE_ES_ERR_APPNAME         \retdesc \copydoc CFE_ES_ERR_APPNAME          \endcode
** \retcode #CFE_ES_ERR_BUFFER          \retdesc \copydoc CFE_ES_ERR_BUFFER           \endcode
** \retcode #CFE_TBL_ERR_BAD_APP_ID     \retdesc \copydoc CFE_TBL_ERR_BAD_APP_ID      \endcode
** \retcode #CFE_TBL_ERR_NO_ACCESS      \retdesc \copydoc CFE_TBL_ERR_NO_ACCESS       \endcode
** \retcode #CFE_TBL_ERR_INVALID_HANDLE \retdesc \copydoc CFE_TBL_ERR_INVALID_HANDLE  \endcode
** \endreturns
**
** \sa #CFE_TBL_Register
**
******************************************************************************/
int32 CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, uint32 MsgId, uint16 CommandCode, uint32 Parameter);

#endif  /* _cfe_tbl_ */
