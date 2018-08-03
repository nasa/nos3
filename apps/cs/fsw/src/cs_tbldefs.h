/************************************************************************
 ** File:
 **   $Id: cs_tbldefs.h 1.3 2017/02/16 15:33:09EST mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   Specification for the CFS Checksum tables.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 *************************************************************************/
#ifndef _cs_tbl_
#define _cs_tbl_

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"


/**************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

/** \brief table names for definition tables */
/** \{ */
#define CS_DEF_EEPROM_TABLE_NAME                    "DefEepromTbl"
#define CS_DEF_MEMORY_TABLE_NAME                    "DefMemoryTbl"
#define CS_DEF_TABLES_TABLE_NAME                    "DefTablesTbl"
#define CS_DEF_APP_TABLE_NAME                       "DefAppTbl"
/** \} */

/** \brief names for the results tables */
/** \{ */
#define CS_RESULTS_EEPROM_TABLE_NAME                "ResEepromTbl"
#define CS_RESULTS_MEMORY_TABLE_NAME                "ResMemoryTbl"
#define CS_RESULTS_TABLES_TABLE_NAME                "ResTablesTbl"
#define CS_RESULTS_APP_TABLE_NAME                   "ResAppTbl"
/** \} */


/**************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/

/**
 ** \brief Data structure for the Eeprom or Memory definition table
 */
typedef struct 
{
    uint16                      State;              /**< \brief Uses the CS_STATE_... defines from above */
    uint16                      Filler16;           /** <\brief Padding */
    uint32                      StartAddress;       /**< \brief The Start address to Checksum */
    uint32                      NumBytesToChecksum; /**< \brief The number of Bytes to Checksum */
} CS_Def_EepromMemory_Table_Entry_t;

/**
 ** \brief Data structure for the Eeporom or Memory results table
 */
typedef struct 
{
    uint16                      State;              /**< \brief Uses the CS_STATE_... defines from above */
    uint16                      ComputedYet;        /**< \brief Have we computed an Integrity value yet */
    uint32                      StartAddress;       /**< \brief The Start address to Checksum */
    uint32                      NumBytesToChecksum; /**< \brief The number of Bytes to Checksum */
    uint32                      ComparisonValue;    /**< \brief The Memory Integrity Value */
    uint32                      ByteOffset;         /**< \brief Where a previous unfinished calculation left off */
    uint32                      TempChecksumValue;  /**< \brief The unfinished caluculation */
} CS_Res_EepromMemory_Table_Entry_t;


/**
 ** \brief Data structure for the Tables definition table
 */
typedef struct 
{
    uint16                      State;                              /**< \brief Uses the CS_STATE_... defines from above */
    char                        Name[CFE_TBL_MAX_FULL_NAME_LEN];    /**< \brief name of the table */
} CS_Def_Tables_Table_Entry_t;



/**
 ** \brief Data structure for the App definition table
 */
typedef struct 
{
    uint16                      State;                              /**< \brief Uses the CS_STATE_... defines from above */
    char                        Name[OS_MAX_API_NAME];              /**< \brief name of the app */
} CS_Def_App_Table_Entry_t;



/**
 ** \brief Data structure for the Tables result table
 */
typedef struct 
{
    uint16                      State;                 /**< \brief Uses the CS_STATE_... defines from above */
    uint16                      ComputedYet;           /**< \brief Have we computed an Integrity value yet */
    uint32                      StartAddress;          /**< \brief The Start address to Checksum */
    uint32                      NumBytesToChecksum;    /**< \brief The number of Bytes to Checksum */
    uint32                      ComparisonValue;       /**< \brief The Memory Integrity Value */
    uint32                      ByteOffset;            /**< \brief Where a previous unfinished calculation left off */
    uint32                      TempChecksumValue;     /**< \brief The unfinished caluculation */
    CFE_TBL_Handle_t            TblHandle;             /**< \brief handle recieved from table services */
    boolean                     IsCSOwner;             /**< \brief Is CS the original owner of this table */
    char                        Name[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief name of the table */
} CS_Res_Tables_Table_Entry_t;


/**
 ** \brief Data structure for the app result table
 */
typedef struct 
{
    uint16                      State;                 /**< \brief Uses the CS_STATE_... defines from above */
    uint16                      ComputedYet;           /**< \brief Have we computed an Integrity value yet */
    uint32                      StartAddress;          /**< \brief The Start address to Checksum */
    uint32                      NumBytesToChecksum;    /**< \brief The number of Bytes to Checksum */
    uint32                      ComparisonValue;       /**< \brief The Memory Integrity Value */
    uint32                      ByteOffset;            /**< \brief Where a previous unfinished calculation left off */
    uint32                      TempChecksumValue;     /**< \brief The unfinished caluculation */
    char                        Name[OS_MAX_API_NAME]; /**< \brief name of the app */
} CS_Res_App_Table_Entry_t;


/**************************************************************************
 **
 **  Function Prototypes
 **
 **************************************************************************/

/************************************************************************/
/** \brief Validate Eeprom definition table
 **  
 **  \par Description
 **       This function is a callback to cFE Table Services that gets called
 **       when a validation is requested.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]  TblPtr        A pointer to the table to be validated
 **
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retcode #CS_TABLE_ERROR \retdesc \copydoc CS_TABLE_ERROR \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
int32 CS_ValidateEepromChecksumDefinitionTable (void * TblPtr);
/************************************************************************/
/** \brief Validate Memory definition table
 **  
 **  \par Description
 **       This function is a callback to cFE Table Services that gets called
 **       when a validation is requested.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]  TblPtr        A pointer to the table to be validated
 **
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retcode #CS_TABLE_ERROR \retdesc \copydoc CS_TABLE_ERROR \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
int32 CS_ValidateMemoryChecksumDefinitionTable (void * TblPtr);
/************************************************************************/
/** \brief Validate Tables definition table
 **  
 **  \par Description
 **       This function is a callback to cFE Table Services that gets called
 **       when a validation is requested.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]  TblPtr        A pointer to the table to be validated
 **
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retcode #CS_TABLE_ERROR \retdesc \copydoc CS_TABLE_ERROR \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
int32 CS_ValidateTablesChecksumDefinitionTable (void * TblPtr);
/************************************************************************/
/** \brief Validate App definition table
 **  
 **  \par Description
 **       This function is a callback to cFE Table Services that gets called
 **       when a validation is requested.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]  TblPtr        A pointer to the table to be validated
 **
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retcode #CS_TABLE_ERROR \retdesc \copydoc CS_TABLE_ERROR \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
int32 CS_ValidateAppChecksumDefinitionTable (void * TblPtr);

/************************************************************************/
/** \brief Processes a new definition table for Eeprom or Memory tables
 **  
 **  \par Description
 **       Copies data from the definition table to the results table
 **       because the results table is where CS keeps all of its 
 **       checksum data
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]    DefinitionTblPtr    A pointer to the definiton table 
 **                                     (#CS_Def_EepromMemory_Table_Entry_t)
 **                                     that we are operating on
 **
 **  \param [in]    ResultsTblPtr       A pointer to the result table 
 **                                     (#CS_Res_EepromMemory_Table_Entry_t)
 **                                     to operate on 
 **  \param [in]    NumEntries          The number of entries in the table
 **  \param [in]    Table               The specific table we are operating on
 **
 *************************************************************************/
void CS_ProcessNewEepromMemoryDefinitionTable (CS_Def_EepromMemory_Table_Entry_t   * DefinitionTblPtr, 
                                               CS_Res_EepromMemory_Table_Entry_t   * ResultsTblPtr,
                                               uint16 NumEntries, 
                                               uint16 Table);

/************************************************************************/
/** \brief Processes a new definition table for the Tables table
 **  
 **  \par Description
 **       Copies data from the definition table to the results table
 **       because the results table is where CS keeps all of its 
 **       checksum data
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]    DefinitionTblPtr    A pointer to the definiton table 
 **                                     (#CS_Def_Tables_Table_Entry_t)
 **                                     that we are operating on
 **
 **  \param [in]    ResultsTblPtr       A pointer to the result table 
 **                                     (#CS_Res_Tables_Table_Entry_t)
 **                                     to operate on 
 **
 *************************************************************************/
void CS_ProcessNewTablesDefinitionTable       (CS_Def_Tables_Table_Entry_t      * DefinitionTblPtr, 
                                               CS_Res_Tables_Table_Entry_t      * ResultsTblPtr);

/************************************************************************/
/** \brief Processes a new definition table for the App table
 **  
 **  \par Description
 **       Copies data from the definition table to the results table
 **       because the results table is where CS keeps all of its 
 **       checksum data
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]    DefinitionTblPtr    A pointer to the definiton table 
 **                                     (#CS_Def_App_Table_Entry_t)
 **                                     that we are operating on
 **
 **  \param [in]    ResultsTblPtr       A pointer to the result table 
 **                                     (#CS_Res_App_Table_Entry_t)
 **                                     to operate on 
 **
 *************************************************************************/
void CS_ProcessNewAppDefinitionTable          (CS_Def_App_Table_Entry_t      * DefinitionTblPtr, 
                                               CS_Res_App_Table_Entry_t      * ResultsTblPtr);


/************************************************************************/
/** \brief Initializes the results and definition table on startup
 **  
 **  \par Description
 **       Completes the Table Services registration and table load for 
 **       the definition table and the registration for the results table
 **
 **  \par Assumptions, External Events, and Notes:
 **       This function is used on all four type of tables individally.
 **       Also, if the default table file is not found for the definitionm
 **       table, this function loads a 'blank' table from memory
 **       
 **  \param [in]    DefinitionTableHandle       A #CFE_TBL_Handle_t pointer
 **                                             that will get filled in with the 
 **                                             table handle to the definition table
 **
 **  \param [in]    ResultsTableHandle          A #CFE_TBL_Handle_t pointer
 **                                             that will get filled in with the 
 **                                             table handle to the results table
 **
 **  \param [in]    DefinitionTblPtr            A pointer to the definiton table 
 **                                             that we are operating on, it will get
 **                                             assigned during this call
 **
 **  \param [in]    ResultsTblPtr               A pointer to the result table 
 **                                             to operate on , it will get
 **                                             assigned during this call
 **
 **  \param [in]    Table                       The specific table we are operating on
 **
 **  \param [in]    DefinitionTableName         The name of the definition table
 **
 **  \param [in]    ResultsTableName            The name of the results table
 **
 **  \param [in]    NumEntries                  The number of entries in the table
 **
 **  \param [in]    DefinitionTableFileName     The name of the file to load the definition
 **                                             table from
 **  \param [in]    DefaultDefTableAddress      The address of the default definition table
 **                                             that we may have to load from memory if the 
 **                                             file is absent
 **
 **  \param [in]    SizeofDefinitionTableEntry  The sizeof an entry in the definition table
 **
 **  \param [in]    SizeofResultsTableEntry     The size of an enrty in the results table
 **
 **  \param [in]    CallBackFunction            A pointer to a function used to validate the
 **                                             definition table
 **  \param [out]  * DefinitionTableHandle      A #CFE_TBL_Handle_t pointer
 **                                             that will get filled in with the 
 **                                             table handle to the definition table
 **  \param [out]  * ResultsTableHandle         A #CFE_TBL_Handle_t pointer
 **                                             that will get filled in with the 
 **                                             table handle to the results table
 **
 **  \param [out]  * DefinitionTblPtr           A pointer to the definiton table 
 **                                             that we are operating on
 **
 **  \param [in]   * ResultsTblPtr              A pointer to the result table 
 **                                             to operate on , it will get
 **                                             be used to access the table
 **
 **  \returns
 **  \retstmt Returns Results from #CFE_TBL_GetAddress or 
 **          #CFE_TBL_Register or #CFE_TBL_Load \endcode
 **  \endreturns
 *************************************************************************/
int32 CS_TableInit (CFE_TBL_Handle_t          * DefinitionTableHandle,
                    CFE_TBL_Handle_t          * ResultsTableHandle,
                    void                      * DefinitionTblPtr,
                    void                      * ResultsTblPtr,
                    uint16                      Table, 
                    char                      * DefinitionTableName,
                    char                      * ResultsTableName,
                    uint16                      NumEntries,
                    char                      * DefinitionTableFileName,
                    void                      * DefaultDefTableAddress,
                    uint16                      SizeofDefinitionTableEntry,
                    uint16                      SizeofResultsTableEntry,
                    CFE_TBL_CallbackFuncPtr_t   CallBackFunction);

/************************************************************************/
/** \brief Handles table updates for all CS tables
 **  
 **  \par Description
 **       Completes the handshake with Table Services that releases 
 **       Addresses for the tables and checks for updates
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]    DefinitionTblPtr        A pointer to the definiton table 
 **                                         that we are operating on
 **
 **  \param [in]    ResultsTblPtr           A pointer to the result table 
 **                                         to operate on 
 **  \param [in]    DefinitionTableHandle   A table handle to the definition table
 **  \param [in]    ResultsTableHandle      A table handle to the results table
 **
 **  \param [in]    NumEntries              The number of entries in the table
 **  \param [in]    Table                   The specific table we are operating on
 **
 **  \returns
 **  \retstmt Returns Results from #CFE_TBL_GetAddress or #CFE_TBL_Manage \endcode
 **  \endreturns
 *************************************************************************/
int32 CS_HandleTableUpdate (void           * DefinitionTblPtr,
                            void           * ResultsTblPtr,
                            CFE_TBL_Handle_t DefinitionTableHandle,
                            CFE_TBL_Handle_t ResultsTableHandle,
                            uint16 Table,
                            uint16 NumEntries);


#endif  /* _cs_table_ */

/************************/
/*  End of File Comment */
/************************/
