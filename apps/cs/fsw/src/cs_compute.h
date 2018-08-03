/************************************************************************
 ** File:
 **   $Id: cs_compute.h 1.3 2017/02/16 15:33:09EST mdeschu Exp  $
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
 **   Specification for the CFS computation functions.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 *************************************************************************/
#ifndef _cs_compute_
#define _cs_compute_

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbldefs.h"

/************************************************************************/
/** \brief Computes checksums on Eeprom or Memory types
 **  
 **  \par Description
 **       Computes checksums up to MaxBytesPerCycle bytes every call. This 
 **       function is used to compute checksums for Eeprom, Memory, the 
 **       OS code segment and the cFE core code segment
 **        
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **       
 **  \param [in]   *ResultsEntry        A pointer to the entry in a table
 **                                     that we want to compute the
 **                                     checksum on.
 **
 **  \param [in]   ComputedCSValue      A pointer to the computed checksum
 **                                     that will be assigned the checksum of
 **                                     the entry.
 **
 **  \param [in]   DoneWithEntry        A pointer to a boolean that will be 
 **                                     assigned a value based on whether or 
 **                                     not the specified entry's checksum
 **                                     was completed.
 **
 **  \param [out]  *ComputedCSValue     Value used to determine the computed 
 **                                     checksum, if completed
 **
 **  \param [out]  *DoneWithEntry       Value that specifies whether or not  
 **                                     the specified entry's checksum was
 **                                     completed during this call.
 **
 **  \returns 
 **  \retcode #CS_SUCCESS  \retdesc \copydoc CS_SUCCESS \endcode
 **  \retcode #CS_ERROR    \retdesc \copydoc CS_ERROR   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 CS_ComputeEepromMemory (CS_Res_EepromMemory_Table_Entry_t         * ResultsEntry,
                              uint32                                    * ComputedCSValue,
                              boolean                                   * DoneWithEntry);

/************************************************************************/
/** \brief Computes checksums on tables
 **  
 **  \par Description
 **       Computes checksums up to MaxBytesPerCycle bytes every call. This 
 **       function is used to compute checksums for tables.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **       
 **  \param [in]   *ResultsEntry        A pointer to the entry in a table
 **                                     that we want to compute the
 **                                     checksum on.
 **
 **  \param [in]   ComputedCSValue      A pointer to the computed checksum
 **                                     that will be assigned the checksum of
 **                                     the entry.
 **
 **  \param [in]   DoneWithEntry        A pointer to a boolean that will be 
 **                                     assigned a value based on whether or 
 **                                     not the specified entry's checksum
 **                                     was completed.
 **
 **  \param [out]  *ComputedCSValue     Value used to determine the computed 
 **                                     checksum, if completed
 **
 **  \param [out]  *DoneWithEntry       Value that specifies whether or not  
 **                                     the specified entry's checksum was
 **                                     completed during this call.
 **
 **  \returns 
 **  \retcode #CS_SUCCESS       \retdesc \copydoc CS_SUCCESS         \endcode
 **  \retcode #CS_ERROR         \retdesc \copydoc CS_ERROR           \endcode
 **  \retcode #CS_ERR_NOT_FOUND \retdesc \copydoc CS_ERR_NOT_FOUND   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 CS_ComputeTables (CS_Res_Tables_Table_Entry_t    * ResultsEntry,
                        uint32                         * ComputedCSValue,
                        boolean                        * DoneWithEntry);


/************************************************************************/
/** \brief Computes checksums on applications
 **  
 **  \par Description
 **       Computes checksums up to MaxBytesPerCycle bytes every call. This 
 **       function is used to compute checksums for applications.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **       
 **  \param [in]   *ResultsEntry        A pointer to the entry in a table
 **                                     that we want to compute the
 **                                     checksum on.
 **
 **  \param [in]   ComputedCSValue      A pointer to the computed checksum
 **                                     that will be assigned the checksum of
 **                                     the entry.
 **
 **  \param [in]   DoneWithEntry        A pointer to a boolean that will be 
 **                                     assigned a value based on whether or 
 **                                     not the specified entry's checksum
 **                                     was completed.
 **
 **  \param [out]  *ComputedCSValue     Value used to determine the computed 
 **                                     checksum, if completed
 **
 **  \param [out]  *DoneWithEntry       Value that specifies whether or not  
 **                                     the specified entry's checksum was
 **                                     completed during this call.
 **
 **  \returns 
 **  \retcode #CS_SUCCESS       \retdesc \copydoc CS_SUCCESS         \endcode
 **  \retcode #CS_ERROR         \retdesc \copydoc CS_ERROR           \endcode
 **  \retcode #CS_ERR_NOT_FOUND \retdesc \copydoc CS_ERR_NOT_FOUND   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 CS_ComputeApp (CS_Res_App_Table_Entry_t       * ResultsEntry,
                     uint32                         * ComputedCSValue,
                     boolean                        * DoneWithEntry);


/************************************************************************/
/** \brief Child task main function for recomputing  baselines for 
 **        Eeprom and Memory types
 **  
 **  \par Description
 **       Child task main function that is spawned when a recompute
 **       baseline command is received for Eeprom, Memory, OS code segment
 **       or cFE core code segment.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        Only one child task for CS can be running at any one time.
 **
 *************************************************************************/
void CS_RecomputeEepromMemoryChildTask(void);

/************************************************************************/
/** \brief Child task main function for recomputing baselines for 
 **        Tables
 **  
 **  \par Description
 **       Child task main function that is spawned when a recompute
 **       baseline command is received for a table.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        Only one child task for CS can be running at any one time.
 **
 *************************************************************************/
void CS_RecomputeTablesChildTask(void);


/************************************************************************/
/** \brief Child task main function for recomputing  baselines for 
 **        Applications
 **  
 **  \par Description
 **       Child task main function that is spawned when a recompute
 **       baseline command is received for Applications.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        Only one child task for CS can be running at any one time.
 **
 *************************************************************************/
void CS_RecomputeAppChildTask(void);


/************************************************************************/
/** \brief Child task main function for computing a one shot calculatipn
 **  
 **  \par Description
 **       Child task main function that is spawned when a one shot 
 **       command is received. 
 **       
 **  \par Assumptions, External Events, and Notes:
 **        Only one child task for CS can be running at any one time.
 **
 *************************************************************************/
void CS_OneShotChildTask(void);

#endif /* _cs_compute_ */
/************************/
/*  End of File Comment */
/************************/
