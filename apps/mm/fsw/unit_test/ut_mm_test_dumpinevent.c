/************************************************************************
** File:
**   $Id: ut_mm_test_dumpinevent.c 1.2 2015/03/02 14:27:01EST sstrege Exp  $
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
**   This is a test driver subroutine to test CFS Memory Manager (MM) dump
**   in event messages
**
**   $Log: ut_mm_test_dumpinevent.c  $
**   Revision 1.2 2015/03/02 14:27:01EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:06EST jmdagost 
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
** Macro Definitions
*************************************************************************/
#define SIM_RAM_MEM_ADDR      0xB0000000  /* UTF Simulated RAM Memory Address */   

#define SIM_EEPROM_MEM_ADDR   0xC0000000  /* UTF Simulated EEPROM Memory Address */   

/*
** Use this address when you want CFE_PSP_MemValidateRange to fail since 
** it's not registered using the CFE_PSP_MemRangeSet calls below
*/
#define SIM_BAD_MEM_ADDR      0xD0000000  

/************************************************************************
** Local function prototypes
*************************************************************************/

void    Test_DumpInEvent(void);

extern void    PrintLocalHKVars(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test dump in event message                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_DumpInEvent(void)
{
   MM_DumpInEventCmd_t  CmdMsg;
   
   /* Setup the test message header */ 
   CFE_SB_InitMsg(&CmdMsg, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
   
   UTF_put_text("**********************************************\n");
   UTF_put_text("* Memory Dump In Event Message Tests         *\n");
   UTF_put_text("**********************************************\n");
   UTF_put_text("\n");

   /*
   ** Test bad symbol name 
   */
   UTF_put_text("Test Dump In Event With Bad Symbol Name \n");
   UTF_put_text("----------------------------------------\n");
   strcpy(CmdMsg.SrcSymAddress.SymName, "BadSymName");
   CmdMsg.SrcSymAddress.Offset  = 0;
   CmdMsg.NumOfBytes            = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType               = MM_RAM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test dump size zero
   */
   UTF_put_text("Test Dump In Event With Dump Size Zero \n");
   UTF_put_text("---------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = 0;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test dump size too large 
   */
   UTF_put_text("Test Dump In Event With Dump Size Too Big \n");
   UTF_put_text("------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES + 10;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test dump with invalid RAM address 
   */
   UTF_put_text("Test Dump In Event With Invalid RAM Address \n");
   UTF_put_text("--------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test dump with invalid EEPROM address 
   */
   UTF_put_text("Test Dump In Event With Invalid EEPROM Address \n");
   UTF_put_text("-----------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 dump with misaligned address 
   */
   UTF_put_text("Test MEM32 Dump In Event With Misaligned Address \n");
   UTF_put_text("-------------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 dump with misaligned data size 
   */
   UTF_put_text("Test MEM32 Dump In Event With Misaligned Data Size \n");
   UTF_put_text("---------------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES - 3;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 dump with invalid address 
   */
   UTF_put_text("Test MEM32 Dump In Event With Invalid Address \n");
   UTF_put_text("----------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 dump with misaligned address 
   */
   UTF_put_text("Test MEM16 Dump In Event With Misaligned Address \n");
   UTF_put_text("-------------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 dump with misaligned data size 
   */
   UTF_put_text("Test MEM16 Dump In Event With Misaligned Data Size \n");
   UTF_put_text("---------------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES - 3;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 dump with invalid address 
   */
   UTF_put_text("Test MEM16 Dump In Event With Invalid Address \n");
   UTF_put_text("----------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 dump with invalid address 
   */
   UTF_put_text("Test MEM8 Dump In Event With Invalid Address \n");
   UTF_put_text("---------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test dump with invalid memory type 
   */
   UTF_put_text("Test Dump In Event With Invalid Memory Type \n");
   UTF_put_text("--------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = 0xFF;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid RAM dump of maximum size
   */
   UTF_put_text("Test RAM Dump In Event Max Bytes \n");
   UTF_put_text("---------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid RAM dump less than maximum size
   */
   UTF_put_text("Test RAM Dump In Event Less Than Max Bytes \n");
   UTF_put_text("-------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES / 2;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid EEPROM dump of maximum size
   */
   UTF_put_text("Test EEPROM Dump In Event Max Bytes \n");
   UTF_put_text("------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid EEPROM dump less than maximum size
   */
   UTF_put_text("Test EEPROM Dump In Event Less Than Max Bytes \n");
   UTF_put_text("----------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES / 2;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid MEM32 dump of maximum size
   */
   UTF_put_text("Test MEM32 Dump In Event Max Bytes \n");
   UTF_put_text("-----------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid MEM32 dump less than maximum size
   */
   UTF_put_text("Test MEM32 Dump In Event Less Than Max Bytes \n");
   UTF_put_text("---------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES / 2;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test valid MEM16 dump of maximum size
   */
   UTF_put_text("Test MEM16 Dump In Event Max Bytes \n");
   UTF_put_text("-----------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid MEM16 dump less than maximum size
   */
   UTF_put_text("Test MEM16 Dump In Event Less Than Max Bytes \n");
   UTF_put_text("---------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES / 2;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid MEM8 dump of maximum size
   */
   UTF_put_text("Test MEM8 Dump In Event Max Bytes \n");
   UTF_put_text("----------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test valid MEM8 dump less than maximum size
   */
   UTF_put_text("Test MEM8 Dump In Event Less Than Max Bytes \n");
   UTF_put_text("--------------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_INEVENT_BYTES / 2;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpInEventCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

} /* end Test_DumpInEvent */

/**** end file: ut_mm_test_dumpinevent.c ****/
