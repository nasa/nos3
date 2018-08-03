/************************************************************************
** File:
**   $Id: ut_mm_test_symtbldump.c 1.2 2015/03/02 14:26:43EST sstrege Exp  $
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
**   This is a test driver subroutine to test CFS Memory Manager (MM) symbol
**   table dump
**
**   $Log: ut_mm_test_symtbldump.c  $
**   Revision 1.2 2015/03/02 14:26:43EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:21EST jmdagost 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/unit_test/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "mm_app.h"            /* Application headers */
#include "mm_filedefs.h"
#include "mm_load.h"
#include "mm_dump.h"
#include "mm_msgids.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_cfe.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"

#include <sys/fcntl.h>         /* System headers      */
#include <unistd.h>
#include <stdlib.h>

/************************************************************************
** Local function prototypes
*************************************************************************/
void    Test_SymTblDump(void);

extern void    PrintLocalHKVars(void);

/* These are private functions in the files that contain them
** but we need access to them in this test driver
*/
void    MM_SymTblToFileCmd(CFE_SB_MsgPtr_t msg);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test symbol table dump command                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_SymTblDump(void)
{
   MM_SymTblToFileCmd_t        DumpSymTblCmdMsg;

   /* Setup the test message header */ 
   CFE_SB_InitMsg(&DumpSymTblCmdMsg, MM_CMD_MID, sizeof(MM_SymTblToFileCmd_t), TRUE);

   UTF_put_text("***************************\n");
   UTF_put_text("* Symbol Table Dump Tests *\n");
   UTF_put_text("***************************\n");
   UTF_put_text("\n");

   /*
   ** Test empty file name 
   */
   UTF_put_text("Test symbol dump with empty file name \n");
   UTF_put_text("--------------------------------------\n");
   DumpSymTblCmdMsg.FileName[0] = '\0';

   MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)&DumpSymTblCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test invalid file name 
   */
   UTF_put_text("Test symbol dump with invalid file name \n");
   UTF_put_text("--------------------------------------\n");
   strcpy(DumpSymTblCmdMsg.FileName, "BadFile*Name");

   MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)&DumpSymTblCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test good file name but OS table dump failure 
   */
   UTF_put_text("Test symbol dump with good filename but bad OS return \n");
   UTF_put_text("------------------------------------------------------\n");
   UTF_SetOSSymbolTableDumpStatus(OS_ERROR);
   strcpy(DumpSymTblCmdMsg.FileName, "IrrelevantFileName");

   MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)&DumpSymTblCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test good file name but OS table dump failure 
   */
   UTF_put_text("Test symbol dump with good filename but bad OS file \n");
   UTF_put_text("----------------------------------------------------\n");
   UTF_SetOSSymbolTableDumpStatus(OS_ERR_FILE);
   strcpy(DumpSymTblCmdMsg.FileName, "IrrelevantFileName");

   MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)&DumpSymTblCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test good file name and success from OS 
   */
   UTF_put_text("Test symbol dump with good filename and successful return\n");
   UTF_put_text("---------------------------------------------------------\n");
   UTF_SetOSSymbolTableDumpStatus(OS_SUCCESS);
   strcpy(DumpSymTblCmdMsg.FileName, "GoodFileName");

   MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)&DumpSymTblCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

} /* end Test_SymTblDump */

/**** end file: ut_mm_test_symtbldump.c ****/
