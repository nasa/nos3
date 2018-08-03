/*************************************************************************
** File:
**   $Id: mm_filedefs.h 1.4 2015/03/02 14:27:02EST sstrege Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**   Specification for the CFS Memory Manager file structures.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_filedefs.h  $
**   Revision 1.4 2015/03/02 14:27:02EST sstrege 
**   Added copyright information
**   Revision 1.3 2008/05/19 15:23:13EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_filedefs_
#define _mm_filedefs_

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_msg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name cFE file header constants for MM created files */ 
/** \{ */
#define MM_CFE_HDR_DESCRIPTION     "Memory Manager Dump File"  /**< \brief What the cFE file header
                                                                     description field should be set to */
#define MM_CFE_HDR_SUBTYPE         0                           /**< \brief What the cFE file header
                                                                     subtype should be set to           */
/** \} */

/*************************************************************************
** Type Definitions
*************************************************************************/

/** \brief MM Load and Dump file header structure
   We use the same header structure for both dump and load files so a dump
   file can be reloaded back into memory if desired (providing a memory save
   and restore capability). This MM header is the secondary header, the
   standard cFE file header is primary header for all load and dump files. 
*/ 
typedef struct {
    CFS_SymAddr_t          SymAddress;      /**< \brief Symbolic load address or fully   
                                                  resolved dump address                    */
    uint32                 NumOfBytes;      /**< \brief Bytes to load or bytes dumped      */
    uint32                 Crc;             /**< \brief CRC value for load or dump data    */
    uint8                  MemType;         /**< \brief Memory type used                   */
    uint8                  Spare[3];        /**< \brief Structure Padding                  */

} MM_LoadDumpFileHeader_t;

#endif /*_mm_filedefs_*/

/************************/
/*  End of File Comment */
/************************/
