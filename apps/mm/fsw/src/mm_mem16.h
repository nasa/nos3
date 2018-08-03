/*************************************************************************
** File:
**   $Id: mm_mem16.h 1.4 2015/03/02 14:27:06EST sstrege Exp  $
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
**   Specification for the CFS Memory Manager functions that are used
**   for the conditionally compiled MM_MEM16 optional memory type.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_mem16.h  $
**   Revision 1.4 2015/03/02 14:27:06EST sstrege 
**   Added copyright information
**   Revision 1.3 2008/05/19 15:23:22EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_mem16_
#define _mm_mem16_

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_filedefs.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Memory16 load from file
**  
**  \par Description
**       Support function for #MM_LoadMemFromFileCmd. This routine will 
**       read a file and write the data to memory that is defined to
**       only be 16 bit accessible
**
**  \par Assumptions, External Events, and Notes:
**       This function is specific to the optional #MM_MEM16 memory
**       type
**       
**  \param [in]   FileHandle   The open file handle of the load file  
**
**  \param [in]   FileName     A pointer to a character string holding  
**                             the load file name
**
**  \param [in]   FileHeader   A #MM_LoadDumpFileHeader_t pointer to 
**                             the load file header structure
**
**  \param [in]   DestAddress  The destination address for the requested 
**                             load operation 
** 
**  \returns
**  \retstmt Returns TRUE if the load completed successfully  \endcode
**  \retstmt Returns FALSE if the load failed due to an error \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_LoadMem16FromFile(uint32                  FileHandle, 
                             char                    *FileName, 
                             MM_LoadDumpFileHeader_t *FileHeader, 
                             uint32                  DestAddress);

/************************************************************************/
/** \brief Memory16 dump to file
**  
**  \par Description
**       Support function for #MM_DumpMemToFileCmd. This routine will 
**       read an address range that is defined to only be 16 bit
**       accessible and store the data in a file
**
**  \par Assumptions, External Events, and Notes:
**       This function is specific to the optional #MM_MEM16 memory
**       type
**       
**  \param [in]   FileHandle   The open file handle of the dump file  
**
**  \param [in]   FileName     A pointer to a character string holding  
**                             the dump file name
**
**  \param [in]   FileHeader   A #MM_LoadDumpFileHeader_t pointer to  
**                             the dump file header structure initialized
**                             with data based upon the command message 
**                             parameters
**
**  \returns
**  \retstmt Returns TRUE if the dump completed successfully  \endcode
**  \retstmt Returns FALSE if the dump failed due to an error \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_DumpMem16ToFile(uint32                  FileHandle, 
                           char                    *FileName, 
                           MM_LoadDumpFileHeader_t *FileHeader);

/************************************************************************/
/** \brief Fill memory16
**  
**  \par Description
**       Support function for #MM_FillMemCmd. This routine will  
**       load memory that is defined to only be 16 bit accessible
**       with a command specified fill pattern
**
**  \par Assumptions, External Events, and Notes:
**       This function is specific to the optional #MM_MEM16 memory
**       type
**       
**  \param [in]   DestAddress   The destination address for the fill 
**                              operation
**
**  \param [in]   CmdPtr     A #MM_FillMemCmd_t pointer to the fill
**                           command message
**
*************************************************************************/
void MM_FillMem16(uint32          DestAddress, 
                  MM_FillMemCmd_t *CmdPtr);

#endif /* _mm_mem16_ */

/************************/
/*  End of File Comment */
/************************/
