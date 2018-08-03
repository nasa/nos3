 /*************************************************************************
 ** File:
 **   $Id: sc_ats2b.c 1.2 2015/03/02 12:58:29EST sstrege Exp  $
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
 **   This file contains a sample of creating an RTS table
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_ats2b.c  $ 
 **   Revision 1.2 2015/03/02 12:58:29EST sstrege  
 **   Added copyright information 
 **   Revision 1.1 2009/01/26 14:42:03EST nyanchik  
 **   Initial revision 
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/unit_test/cf/tblsrc/project.pj 
 *************************************************************************/

#include "sc_app.h"
#include "cfe_tbl_filedef.h"


/************************************************************************
The structure of a cFE raw command using CCSDS. 
Note the command shown has no parameters. If more parameters are needed, the are added after the checksum.

0x1803, 0xC000, 0x0001, 0x0000
   ^       ^      ^     ^ ^ 
   |       |      |     | the last byte is the checksum for the command
   |       |      |     The first byte is the command code
   |       |      2 bytes for the length of command after these two bytes
   |       2 bytes for sequence counter always 0xC000
   The application to recieve and processs the command in app_msgids.h
   
note that in the case of SC, there is information before the cFE command structure,
like the time tag and the command number (for ATS's)
************************************************************************/

uint16     SC_Ats2b[SC_ATS_BUFF_SIZE] = {
/* This is a valid table that does not go to the end of the buffer 
This table is used to  test sending a command with an invalid length */
/* This table should not pass validation */

0x0000, 0x0001, 0x0000, 0x0040, 0x18A9, 0xC000, 0x0001, 0x008F,
0x0000, 0x0002, 0x0000, 0x0044, 0x18A9, 0xC000, 0x0001, 0x008F,

/* Length is set from 0x01 to 0xFF */
0x0000, 0x0003, 0x0000, 0x0048, 0x18A9, 0xC000, 0x00FF, 0x008F,

/* Unknown Command, but as long as I could make it, 68 words total, FD is checksum for this command */

0x0000, 0x0004, 0x0000, 0x0052, 0x18A9, 0xC000, 0x0079, 0x0AFD, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000


};

CFE_TBL_FILEDEF(SC_Ats2b, SC_APP.ATS_TBL2, SC ATS table 2b,ATS_TBL2b.tbl)


/************************/
/*  End of File Comment */
/************************/

