/*************************************************************************
** File:
**   $Id: md_utils.h 1.4 2015/03/01 17:18:01EST sstrege Exp  $
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
**   Specification for the CFS Memory Dwell utility functions.
**
** Notes:
**
**   $Log: md_utils.h  $
**   Revision 1.4 2015/03/01 17:18:01EST sstrege 
**   Added copyright information
**   Revision 1.3 2009/04/18 15:06:41EDT dkobe 
**   Corrected parameter description for function prolog
**   Revision 1.2 2008/07/02 13:54:42EDT nsschweiss 
**   CFS MD Post Code Review Version
**   Date: 08/05/09
**   CPID: 1653:2
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_utils_h_
#define _md_utils_h_

/*************************************************************************
** Includes
*************************************************************************/

/* md_msg needs to be included for MD_SymAddr_t definition */ 
#include "md_msg.h"
#include "cfe.h"



/*****************************************************************************/
/**
** \brief  Determine if specified TableId is contained in argument mask.   
**
** \par Description
**          Determines whether specified Table Id is contained in argument mask.
**
** \param[in] TableId    identifies dwell table  (1..#MD_NUM_DWELL_TABLES)
**
** \param[in] TableMask  Mask representing current status of all dwell tables.
**
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
******************************************************************************/
boolean MD_TableIsInMask(int16 TableId, uint16 TableMask);

/*****************************************************************************/
/**
** \brief Update Dwell Table Control Info
**
** \par Description
**          Updates the control structure used by the application for
**          dwell packet processing with address count, data size, and rate.
** 
** \par Assumptions, External Events, and Notes:
**          A zero value for length in a dwell table entry
**    represents the end of the active portion of a dwell table.
**
** \param[in] TableIndex identifies dwell control structure  (0..#MD_NUM_DWELL_TABLES-1)
**                                      
** \retval None
******************************************************************************/
void MD_UpdateDwellControlInfo (uint16 TableIndex);

/*****************************************************************************/
/**
** \brief Validate Address count
**
** \par Description
**        Checks for valid value (0..MD_DWELL_TABLE_SIZE) used to 
**        index internal structure.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Count  Index for internal dwell control structure.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
boolean MD_ValidAddrIndex    ( uint16 Count );

/*****************************************************************************/
/**
** \brief Validate Entry Index
**
** \par Description
**        Checks for valid value (1..MD_DWELL_TABLE_SIZE ) for entry id
**        specified in Jam command.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] EntryId  EntryId (starting at one) for dwell control structure entry.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
boolean MD_ValidEntryId            ( uint16 EntryId );

/*****************************************************************************/
/**
** \brief Validate Dwell Address
**
** \par Description
**        This function validates that the memory range as specified by the
**        input address and size is valid for reading.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Addr  Dwell address.
**                                      
** \param[in] Size Size, in bytes, of field to be read.
**
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
boolean MD_ValidAddrRange( uint32 Addr, uint32 Size );

/*****************************************************************************/
/**
** \brief Validate Table ID
**
** \par Description
**        Check valid range for TableId argument used in several
**        Memory Dwell commands.
**        Valid range is 1..#MD_NUM_DWELL_TABLES.
** 
** \par Assumptions, External Events, and Notes:
**        Note that this value will be internally converted to 
**        0..(#MD_NUM_DWELL_TABLES-1) for indexing into arrays.
**
** \param[in] TableId  Table ID.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
boolean MD_ValidTableId( uint16 TableId );

/*****************************************************************************/
/**
** \brief Validate Field Length
**
** \par Description
**        Check valid range for dwell field length.
** 
** \par Assumptions, External Events, and Notes:
**   Valid values for dwell field length are 0, 1, 2, and 4.
**   0 corresponds to a null entry in Dwell Table.
**
** \param[in] FieldLength  Length of field, in bytes, to be copied for dwell.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
boolean MD_ValidFieldLength(uint16 FieldLength);





#endif /* _md_utils_ */
/************************/
/*  End of File Comment */
/************************/
