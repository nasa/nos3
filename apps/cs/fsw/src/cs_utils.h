/************************************************************************
 ** File:
 **   $Id: cs_utils.h 1.3 2017/02/16 15:33:14EST mdeschu Exp  $
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
 **   Specification for the CFS utilty functions
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 *************************************************************************/
#ifndef _cs_utils_
#define _cs_utils_

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbldefs.h"

/************************************************************************/
/** \brief Zeros out temporary checksum values of Eeprom table entries
 **  
 **  \par Description
 **       Zeros the TempChecksumValue and the byte offset for every entry
 **       in the table. This allows all entries in the table to have their
 **       checksum started 'fresh'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_ZeroEepromTempValues(void);

/************************************************************************/
/** \brief Zeros out temporary checksum values of Memory table entries
 **  
 **  \par Description
 **       Zeros the TempChecksumValue and the byte offset for every entry
 **       in the table. This allows all entries in the table to have their
 **       checksum started 'fresh'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_ZeroMemoryTempValues(void);

/************************************************************************/
/** \brief Zeros out temporary checksum values of Tables table entries
 **  
 **  \par Description
 **       Zeros the TempChecksumValue and the byte offset for every entry
 **       in the table. This allows all entries in the table to have their
 **       checksum started 'fresh'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_ZeroTablesTempValues(void);

/************************************************************************/
/** \brief Zeros out temporary checksum values of App table entries
 **  
 **  \par Description
 **       Zeros the TempChecksumValue and the byte offset for every entry
 **       in the table. This allows all entries in the table to have their
 **       checksum started 'fresh'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_ZeroAppTempValues(void);

/************************************************************************/
/** \brief Zeros out temporary checksum values of the cFE Core
 **  
 **  \par Description
 **       Zeros the TempChecksumValue and the byte offset for the cFE core.
 **       This allows the cFE core checksum to be started 'fresh'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_ZeroCfeCoreTempValues(void);

/************************************************************************/
/** \brief Zeros out temporary checksum values of the OS code segment
 **  
 **  \par Description
 **       Zeros the TempChecksumValue and the byte offset for the OS.
 **       This allows the OS checksum to be started 'fresh'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_ZeroOSTempValues(void);

/************************************************************************/
/** \brief Initializes the default definition tables
 **  
 **  \par Description
 **       Sets all of the entries in the default definitions tables for 
 **       Eeprom,Memory, Tables, and Apps to zero and sets theri states 
 **       to 'empty'.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_InitializeDefaultTables(void);

/************************************************************************/
/** \brief Moves global variables to point to the next table
 **  
 **  \par Description
 **       Moves the global variables to point to the next table to checksum
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void CS_GoToNextTable(void);

/************************************************************************/
/** \brief Gets a pointer to the results entry given a table name
 **  
 **  \par Description
 **       This routine will look through the Tables results table  
 **       to find an entry that has the given name. It returns
 **       a pointer to the entry through a parameter.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]    *EntryPtr    A pointer to a #CS_Res_Tables_Table_Entry_t
 **                              that will be assigned an entry where the name
 **                              field matches the name passed in.
 **
 **  \param [in]    *Name        The name associated with the entry we want
 **                              to find.
 **
 **  \param [out]  **EntryPtr    A pointer to a #CS_Res_Tables_Table_Entry_t
 **                              pointer that contains the start address of the 
 **                              entry whose name field matches the name passed in
 **                              in the table passed in.
 **
 **  \returns
 **  \retstmt Returns TRUE if the name was found in the table      \endcode
 **  \retstmt Returns FALSE if the name was not found in the table \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t ** EntryPtr,
                                     char                         * Name);

/************************************************************************/
/** \brief Gets a pointer to the definition entry given a table name
 **  
 **  \par Description
 **       This routine will look through the Tables definition table  
 **       to find an entry that has the given name. It returns
 **       a pointer to the entry through a parameter.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]    *EntryPtr    A pointer to a #CS_Def_Tables_Table_Entry_t
 **                              that will be assigned an entry where the name
 **                              field matches the name passed in.
 **
 **  \param [in]    *Name        The name associated with the entry we want
 **                              to find.
 **
 **  \param [out]  **EntryPtr    A pointer to a #CS_Def_Tables_Table_Entry_t
 **                              pointer that contains the start address of the 
 **                              entry whose name field matches the name passed in
 **                              in the table passed in.
 **
 **  \returns
 **  \retstmt Returns TRUE if the name was found in the table      \endcode
 **  \retstmt Returns FALSE if the name was not found in the table \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t ** EntryPtr,
                                     char                         * Name);

/************************************************************************/
/** \brief Gets a pointer to the results entry given a app name
 **  
 **  \par Description
 **       This routine will look through the App Results table  
 **       to find an entry that has the given name. It returns
 **       a pointer to the entry through a parameter.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]     *EntryPtr   A pointer to a #CS_Res_App_Table_Entry_t
 **                              that will be assigned an entry where the name
 **                              field matches the name passed in.
 **
 **  \param [in]      Name       The name associated with the entry we want
 **                              to find.
 **
 **  \param [out]   **EntryPtr   A pointer to a #CS_Res_App_Table_Entry_t
 **                              pointer that contains the start address of the 
 **                              entry whose name field matches the name passed in
 **                              in the table passed in.
 **
 **  \returns
 **  \retstmt Returns TRUE if the name was found in the table      \endcode
 **  \retstmt Returns FALSE if the name was not found in the table \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t ** EntryPtr,
                                   char                      * Name);


/************************************************************************/
/** \brief Gets a pointer to the definition entry given a app name
 **  
 **  \par Description
 **       This routine will look through the App Definition table  
 **       to find an entry that has the given name. It returns
 **       a pointer to the entry through a parameter.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]     *EntryPtr   A pointer to a #CS_Def_App_Table_Entry_t
 **                              that will be assigned an entry where the name
 **                              field matches the name passed in.
 **
 **  \param [in]      Name       The name associated with the entry we want
 **                              to find.
 **
 **  \param [out]   **EntryPtr   A pointer to a #CS_Res_Def_Table_Entry_t
 **                              pointer that contains the start address of the 
 **                              entry whose name field matches the name passed in
 **                              in the table passed in.
 **
 **  \returns
 **  \retstmt Returns TRUE if the name was found in the table      \endcode
 **  \retstmt Returns FALSE if the name was not found in the table \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t ** EntryPtr,
                                   char                      * Name);


/************************************************************************/
/** \brief Find an enabled Eeprom entry 
 **  
 **  \par Description
 **       This routine will look from the current position to the end of
 **       the table to find an enabled entry.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 **                                  assigned an enabled entry ID, if
 **                                  one exists
 **
 **  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 **                                  table, if the function resturs TRUE
 **
 **  \returns
 **  \retstmt Returns TRUE if an enabled entry was found      \endcode
 **  \retstmt Returns FALSE if an enabled entry was not found \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_FindEnabledEepromEntry(uint16* EnabledEntry);

/************************************************************************/
/** \brief Find an enabled Memory entry 
 **  
 **  \par Description
 **       This routine will look from the current position to the end of
 **       the table to find an enabled entry.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 **                                  assigned an enabled entry ID, if
 **                                  one exists
 **
 **  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 **                                  table, if the function resturs TRUE
 **
 **  \returns
 **  \retstmt Returns TRUE if an enabled entry was found      \endcode
 **  \retstmt Returns FALSE if an enabled entry was not found \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_FindEnabledMemoryEntry(uint16* EnabledEntry);

/************************************************************************/
/** \brief Find an enabled Tables entry 
 **  
 **  \par Description
 **       This routine will look from the current position to the end of
 **       the table to find an enabled entry.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 **                                  assigned an enabled entry ID, if
 **                                  one exists
 **
 **  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 **                                  table, if the function resturs TRUE
 **
 **  \returns
 **  \retstmt Returns TRUE if an enabled entry was found      \endcode
 **  \retstmt Returns FALSE if an enabled entry was not found \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_FindEnabledTablesEntry(uint16* EnabledEntry);

/************************************************************************/
/** \brief Find an enabled App entry 
 **  
 **  \par Description
 **       This routine will look from the current position to the end of
 **       the table to find an enabled entry.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 **                                  assigned an enabled entry ID, if
 **                                  one exists
 **
 **  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 **                                  table, if the function resturs TRUE
 **
 **  \returns
 **  \retstmt Returns TRUE if an enabled entry was found      \endcode
 **  \retstmt Returns FALSE if an enabled entry was not found \endcode
 **  \endreturns
 **
 **
 *************************************************************************/
boolean CS_FindEnabledAppEntry(uint16* EnabledEntry);

/************************************************************************/
/** \brief Verify command message length
 **  
 **  \par Description
 **       This routine will check if the actual length of a software bus
 **       command message matches the expected length and send an
 **       error event message if a mismatch occurs
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that
 **                                 references the software bus message 
 **
 **  \param [in]   ExpectedLength   The expected length of the message
 **                                 based upon the command code
 **
 **  \returns
 **  \retstmt Returns TRUE if the length is as expected      \endcode
 **  \retstmt Returns FALSE if the length is not as expected \endcode
 **  \endreturns
 **
 **  \sa #CS_LEN_ERR_EID
 **
 *************************************************************************/
boolean CS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength);

/************************************************************************/
/** \brief Compute a background check cycle on the OS 
 **  
 **  \par Description
 **       This routine will try and complete a cycle of background checking
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 **  \returns
 **  \retstmt Returns TRUE if checksumming was done      \endcode
 **  \retstmt Returns FALSE if checksumming was NOT done \endcode
 **  \endreturns
 **
 *************************************************************************/
boolean CS_BackgroundOS(void);

/************************************************************************/
/** \brief Compute a background check cycle on the cFE Core 
 **  
 **  \par Description
 **       This routine will try and complete a cycle of background checking
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 **  \returns
 **  \retstmt Returns TRUE if checksumming was done      \endcode
 **  \retstmt Returns FALSE if checksumming was NOT done \endcode
 **  \endreturns
 **
 *************************************************************************/
boolean CS_BackgroundCfeCore(void);

/************************************************************************/
/** \brief Compute a background check cycle on Eeprom 
 **  
 **  \par Description
 **       This routine will try and complete a cycle of background checking
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 **  \returns
 **  \retstmt Returns TRUE if checksumming was done      \endcode
 **  \retstmt Returns FALSE if checksumming was NOT done \endcode
 **  \endreturns
 **
 *************************************************************************/
boolean CS_BackgroundEeprom(void);

/************************************************************************/
/** \brief Compute a background check cycle on the Memory 
 **  
 **  \par Description
 **       This routine will try and complete a cycle of background checking
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 **  \returns
 **  \retstmt Returns TRUE if checksumming was done      \endcode
 **  \retstmt Returns FALSE if checksumming was NOT done \endcode
 **  \endreturns
 **
 *************************************************************************/
boolean CS_BackgroundMemory(void);

/************************************************************************/
/** \brief Compute a background check cycle on Tables 
 **  
 **  \par Description
 **       This routine will try and complete a cycle of background checking
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 **  \returns
 **  \retstmt Returns TRUE if checksumming was done      \endcode
 **  \retstmt Returns FALSE if checksumming was NOT done \endcode
 **  \endreturns
 **
 *************************************************************************/
boolean CS_BackgroundTables(void);

/************************************************************************/
/** \brief Compute a background check cycle on Apps 
 **  
 **  \par Description
 **       This routine will try and complete a cycle of background checking
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 **  \returns
 **  \retstmt Returns TRUE if checksumming was done      \endcode
 **  \retstmt Returns FALSE if checksumming was NOT done \endcode
 **  \endreturns
 **
 *************************************************************************/
boolean CS_BackgroundApp(void);


/************************************************************************/
/** \brief Reset the checksum for a CS table entry in the CS tables table
 **  
 **  \par Description
 **       If CS tables are listed in the CS tables table, then those tables
 **       must have their checksums recomputed when any of their entries
 **       have their enable/disable state flags modified.
 **
 **       This function will set ByteOffset and TempChecksumValue to zero,
 **       and ComputedYet to FALSE for the specifified CS tables table entry.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **    
 *************************************************************************/
void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry);


#endif /* _cs_utils_ */

/************************/
/*  End of File Comment */
/************************/
