/*************************************************************************
** File:
**   $Id: md_dwell_tbl.h 1.6 2015/03/01 17:17:54EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   Functions used for validating and copying CFS Memory Dwell Tables.
**
** Notes:
**
**   $Log: md_dwell_tbl.h  $
**   Revision 1.6 2015/03/01 17:17:54EST sstrege 
**   Added copyright information
**   Revision 1.5 2009/04/02 14:46:55EDT nschweis 
**   Modified code so that function signature and corresponding user documentation only compiles 
**   if signature option has been enabled.
**   CPID 7326:1.
**   Revision 1.4 2009/02/11 16:08:55EST nschweis 
**   Updated comments for MD_CopyUpdatedTbl function.  
**   CPID 4205:2.
**   Revision 1.3 2008/12/10 15:04:07EST nschweis 
**   Added functions to change contents of Table Services buffer.
**   CPID 2624:1.
**   Revision 1.2 2008/08/08 14:56:31EDT nsschweiss 
**   Function signature and description of MD_CopyUpdatedTbl are modified to reflect addition of a
**   table pointer argument.
**   Revision 1.1 2008/07/02 13:49:59EDT nsschweiss 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/src/project.pj
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_dwell_tbl_h_
#define _md_dwell_tbl_h_

/*************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "md_tbldefs.h"

/*****************************************************************************/
/**
** \brief Dwell Table Validation Function
**
** \par Description
**  This function indicates whether the Dwell Table corresponding to the
**  input table pointer is valid.
** 
** \par Assumptions, External Events, and Notes:
**  This function gets registered with Table Services as a callback function 
**  for validating Dwell Tables that are loaded from the ground, and so must
**  be in accordance with the call signature specified by Table Services.
**  In addition, the function is used by Memory Dwell to validate Dwell Tables
**  that have been recovered from a Critical Data Store.
**
** \param[in] TblPtr Table pointer  
**
** \returns
** \retcode #CFE_SUCCESS             \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #MD_TBL_ENA_FLAG_ERROR   \retdesc \copydoc MD_TBL_ENA_FLAG_ERROR  \endcode
** \retcode #MD_ZERO_RATE_TBL_ERROR  \retdesc \copydoc MD_ZERO_RATE_TBL_ERROR \endcode
** \retcode #MD_RESOLVE_ERROR        \retdesc \copydoc MD_RESOLVE_ERROR       \endcode
** \retcode #MD_INVALID_ADDR_ERROR   \retdesc \copydoc MD_INVALID_ADDR_ERROR  \endcode
** \retcode #MD_INVALID_LEN_ERROR    \retdesc \copydoc MD_INVALID_LEN_ERROR   \endcode
** \retcode #MD_NOT_ALIGNED_ERROR    \retdesc \copydoc MD_NOT_ALIGNED_ERROR   \endcode
** \endreturns
******************************************************************************/
int32 MD_TableValidationFunc (void *TblPtr);


/*****************************************************************************/
/**
** \brief Generate internal data structures based on Dwell Table Load.
**
** \par Description
**          Copies Enabled field.
**          Copies Signature field.
**          For each dwell table entry, copies field length, and delay value.
**          Evaluates and saves resolved dwell address for each dwell entry.
**          Evaluates and saves additional summary data based on entry contents.
** 
** \par Assumptions, External Events, and Notes:
**          Dwell table contents have been validated before reaching this point.
**          Run when a table is loaded by command, or when a table is recovered
**          on start up.
**
** \param[in] MD_LoadTablePtr Pointer to Table Services buffer.
**    
** \param[in] TblIndex An identifier specifying which dwell table is to be 
**             copied.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
**                                 
** \retval None
******************************************************************************/
void MD_CopyUpdatedTbl(MD_DwellTableLoad_t *MD_LoadTablePtr, uint8 TblIndex);

/*****************************************************************************/
/**
** \brief Update Dwell Table's Enabled Field.
**
** \par Description
**          Update Dwell Table's Enabled Field.
** 
** \par Assumptions, External Events, and Notes:
**          TableIndex is in [0..MD_NUM_DWELL_TABLES-1] range.
**          FieldValue is MD_DWELL_STREAM_ENABLED or MD_DWELL_STREAM_DISABLED.
**
** \param[in] TableIndex An identifier specifying which dwell table is to be 
**             modified.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
** \param[in] FieldValue New value for Enabled field.
**    
**                                 
** \retval None
******************************************************************************/
void MD_UpdateTableEnabledField (uint16 TableIndex, uint16 FieldValue);

/*****************************************************************************/
/**
** \brief Update Values for a Dwell Table Entry.
**
** \par Description
**          Update Values for a Dwell Table Entry.
** 
** \par Assumptions, External Events, and Notes:
**          TableIndex is in [0..MD_NUM_DWELL_TABLES-1] range.
**          EntryIndex is in [0..MD_DWELL_TABLE_SIZE-1] range.
**          NewLength is 0, 1, 2, or 4.
**          NewDwellAddress is a valid dwell address.
**
** \param[in] TableIndex An identifier specifying which dwell table is to be 
**             modified.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
** \param[in] EntryIndex An identifier specifying which entry is to be 
**             modified.  Internal values [0..MD_DWELL_TABLE_SIZE-1] are used.
** \param[in] NewLength         Number of bytes to be read.
** \param[in] NewDelay          Number of counts before next dwell.
** \param[in] NewDwellAddress   Memory address to be dwelled on.
**                                 
** \retval None
******************************************************************************/
void MD_UpdateTableDwellEntry (uint16 TableIndex, 
                               uint16 EntryIndex, 
                               uint16 NewLength,
                               uint16 NewDelay,
                               CFS_SymAddr_t NewDwellAddress);

#if MD_SIGNATURE_OPTION == 1   
/*****************************************************************************/
/**
** \brief Update Dwell Table Signature.
**
** \par Description
**          Update Dwell Table Signature.
** 
** \par Assumptions, External Events, and Notes:
**          TableIndex is in [0..MD_NUM_DWELL_TABLES-1] range.
**
** \param[in] TableIndex An identifier specifying which dwell table is to be 
**             modified.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
** \param[in] NewSignature   New Dwell Table signature.
**                                 
** \retval None
******************************************************************************/
void MD_UpdateTableSignature (uint16 TableIndex, 
                               char NewSignature[MD_SIGNATURE_FIELD_LENGTH]);
#endif

#endif /* _md_dwell_tbl_h_ */
/************************/
/*  End of File Comment */
/************************/
