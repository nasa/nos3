/*
** $Id: fm_defs.h 1.7 2015/02/28 17:50:51EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Macro Definitions File
**
** Purpose: Value definitions
**
** Author: Scott Walling (Microtel)
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_defs.h  $
** Revision 1.7 2015/02/28 17:50:51EST sstrege 
** Added copyright information
** Revision 1.6 2014/12/12 16:23:51EST lwalling 
** Change table state definitions such that 0 = disabled, 1 = enabled, 2 = unused
** Revision 1.5 2014/10/22 17:51:02EDT lwalling 
** Allow zero as a valid semaphore ID, use FM_CHILD_SEM_INVALID instead
** Revision 1.4 2010/03/04 15:44:14EST lwalling 
** Remove include of cfe.h - not needed
** Revision 1.3 2010/02/25 13:31:01EST lwalling 
** Remove local definition of uint64 data type
** Revision 1.2 2009/11/13 16:28:17EST lwalling 
** Modify macro names, move some macros to platform cfg file, delete TableID
** Revision 1.1 2009/11/09 16:47:46EST lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj
*/

#ifndef _fm_defs_h_
#define _fm_defs_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task semaphore does not exist                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_CHILD_SEM_INVALID        0xFFFFFFFF


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM argument to not calculate CRC during Get File Info command   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_IGNORE_CRC               0


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM directory entry definitions                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_THIS_DIRECTORY           "."
#define FM_PARENT_DIRECTORY         ".."


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM filename status definitions                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_NAME_IS_INVALID          0
#define FM_NAME_IS_NOT_IN_USE       1
#define FM_NAME_IS_FILE_OPEN        2
#define FM_NAME_IS_FILE_CLOSED      3
#define FM_NAME_IS_DIRECTORY        4


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM free space table entry state definitions                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_TABLE_ENTRY_DISABLED     0
#define FM_TABLE_ENTRY_ENABLED      1
#define FM_TABLE_ENTRY_UNUSED       2


#endif /* _fm_defs_h_ */

/************************/
/*  End of File Comment */
/************************/

