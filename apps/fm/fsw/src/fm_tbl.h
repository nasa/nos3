/*
** $Id: fm_tbl.h 1.14 2015/02/28 17:50:54EST sstrege Exp  $
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
** Title: File Manager (FM) Table Definitions
**
** Purpose: Unit specification for the CFS File Manager table structures.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_tbl.h  $
** Revision 1.14 2015/02/28 17:50:54EST sstrege 
** Added copyright information
** Revision 1.13 2010/03/04 10:41:56EST lwalling 
** Removed another empty Doxygen param section
** Revision 1.12 2010/03/03 18:16:44EST lwalling 
** Removed empty param sections, changed some pound symbols to forward slashes
** Revision 1.11 2009/11/13 16:30:22EST lwalling 
** Modify macro names
** Revision 1.10 2009/11/09 16:53:11EST lwalling 
** Cleanup and expand function prototype comments, move value defs to fm_defs.h
** Revision 1.9 2009/10/30 16:01:44EDT lwalling 
** Modify free space table entry state definitions
** Revision 1.8 2009/10/30 14:02:33EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.7 2009/10/30 10:42:18EDT lwalling
** Move table definition structures to fm_msg.h
** Revision 1.6 2009/10/09 17:23:49EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.5 2009/10/08 15:58:18EDT lwalling
** Remove size field from device table structure
** Revision 1.4 2009/10/07 15:57:57EDT lwalling
** Changed device table structure definition, changed prototype for FM_AcquireTablePointers()
** Revision 1.3 2008/12/11 12:02:49EST sstrege
** Moved configurable table defs to platform_cfg header file
** Revision 1.2 2008/06/20 16:21:44EDT slstrege
** Member moved from fsw/src/fm_tbl.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_tbl.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:44ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
**
*/

#ifndef _fm_tbl_h_
#define _fm_tbl_h_

#include "cfe.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM table global function prototypes                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Table Initialization Function
**
**  \par Description
**       This function is invoked during FM application startup initialization to
**       create and initialize the FM file system free space table.  The purpose
**       for the table is to define the list of file systems for which free space
**       must be reported.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS  \endcode
**  \retstmt Error return codes from #CFE_TBL_Register    \endcode
**  \endreturns
**
**  \sa /FM_AppInit
**/
int32 FM_TableInit(void);


/**
**  \brief Table Verification Function
**
**  \par Description
**       This function is called from the CFE Table Services as part of the
**       initial table load, and later inresponse to a Table Validate command.
**       The function verifies that the table data is acceptable to populate the
**       FM file system free space table.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  TableData - Pointer to table data for verification.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt #FM_TABLE_VALIDATION_ERR if table validation fails \endcode
**  \endreturns
**
**  \sa /FM_AppInit
**/
int32 FM_ValidateTable(void *TableData);


/**
**  \brief Acquire Table Data Pointer Function
**
**  \par Description
**       This function is invoked to acquire a pointer to the FM file system free
**       space table data.  The pointer is maintained in the FM global data
**       structure.  Note that the table data pointer will be set to NULL if the
**       table has not yet been successfully loaded.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_GlobalData_t
**/
void  FM_AcquireTablePointers(void);


/**
**  \brief Release Table Data Pointer Function
**
**  \par Description
**       This function is invoked to release the pointer to the FM file system free
**       space table data.  The pointer is maintained in the FM global data
**       structure.  The table data pointer must be periodically released to allow
**       CFE Table Services an opportunity to load or dump the table without risk
**       of interfering with users of the table data.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_GlobalData_t
**/
void  FM_ReleaseTablePointers(void);


#endif /* _fm_tbl_h_ */

/************************/
/*  End of File Comment */
/************************/
