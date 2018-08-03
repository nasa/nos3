/************************************************************************
** File:
**   $Id: ut_mm_test_peeks.c 1.2 2015/03/02 14:26:49EST sstrege Exp  $
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
**   This is a test driver subroutine to test CFS Memory Manager (MM) peeks
**
**   $Log: ut_mm_test_peeks.c  $
**   Revision 1.2 2015/03/02 14:26:49EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:16EST jmdagost 
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

void    Test_Peeks(void);

extern void    PrintLocalHKVars(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test memory peeks                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_Peeks(void)
{
   MM_PeekCmd_t  CmdMsg;

   /* Setup the test message header */ 
   CFE_SB_InitMsg(&CmdMsg, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
   
   UTF_put_text("\n");
   UTF_put_text("*********************\n");
   UTF_put_text("* Memory Peek Tests *\n");
   UTF_put_text("*********************\n");
   UTF_put_text("\n");

   /*
   ** Test bad symbol name 
   */
   UTF_put_text("Test Peek With Bad Symbol Name \n");
   UTF_put_text("-------------------------------\n");
   strcpy(CmdMsg.SrcSymAddress.SymName, "BadSymName");
   CmdMsg.SrcSymAddress.Offset = 0;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test good symbol name 
   */
   UTF_put_text("Test Peek With Good Symbol Name \n");
   UTF_put_text("--------------------------------\n");
   strcpy(CmdMsg.SrcSymAddress.SymName, "GoodSymName");
   CmdMsg.SrcSymAddress.Offset = 0;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test 16 bit peek with misaligned address 
   */
   UTF_put_text("Test Peek 16 With Misaligned Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test 32 bit peek with misaligned address 
   */
   UTF_put_text("Test Peek 32 With Misaligned Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test peek with invalid data size 
   */
   UTF_put_text("Test Peek With Invalid Data Size \n");
   UTF_put_text("---------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 12;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test peek with invalid RAM address 
   */
   UTF_put_text("Test Peek With Invalid RAM Address \n");
   UTF_put_text("-----------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test peek with invalid EEPROM address 
   */
   UTF_put_text("Test Peek With Invalid EEPROM Address \n");
   UTF_put_text("--------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_EEPROM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test peek with invalid memory type 
   */
   UTF_put_text("Test Peek With Invalid Memory Type \n");
   UTF_put_text("-----------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = 0xFF;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 peek with invalid data size 
   */
   UTF_put_text("Test MEM32 Peek With Invalid Data Size \n");
   UTF_put_text("---------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_MEM32;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 peek with invalid address 
   */
   UTF_put_text("Test MEM32 Peek With Invalid Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_MEM32;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 peek with invalid data size 
   */
   UTF_put_text("Test MEM16 Peek With Invalid Data Size \n");
   UTF_put_text("---------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_MEM16;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 peek with invalid address 
   */
   UTF_put_text("Test MEM16 Peek With Invalid Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_MEM16;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 peek with invalid data size 
   */
   UTF_put_text("Test MEM8 Peek With Invalid Data Size \n");
   UTF_put_text("--------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_MEM8;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM8 peek with invalid address 
   */
   UTF_put_text("Test MEM8 Peek With Invalid Address \n");
   UTF_put_text("------------------------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize             = 8;
   CmdMsg.MemType              = MM_MEM8;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test RAM 32 bit peek 
   */
   UTF_put_text("Test RAM Peek 32 \n");
   UTF_put_text("-----------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM 16 bit peek 
   */
   UTF_put_text("Test RAM Peek 16 \n");
   UTF_put_text("-----------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM 8 bit peek
   **  
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test RAM Peek 8 \n");
   UTF_put_text("----------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 8;
   CmdMsg.MemType              = MM_RAM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);
   
   /*
   ** Test EEPROM 32 bit peek 
   */
   UTF_put_text("Test EEPROM Peek 32 \n");
   UTF_put_text("--------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_EEPROM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM 16 bit peek 
   */
   UTF_put_text("Test EEPROM Peek 16 \n");
   UTF_put_text("--------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_EEPROM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM 8 bit peek 
   **
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test EEPROM Peek 8 \n");
   UTF_put_text("-------------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize             = 8;
   CmdMsg.MemType              = MM_EEPROM;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);

   /*
   ** Test MEM32 peek 
   */
   UTF_put_text("Test MEM32 Peek \n");
   UTF_put_text("----------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 32;
   CmdMsg.MemType              = MM_MEM32;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 peek 
   */
   UTF_put_text("Test MEM16 Peek \n");
   UTF_put_text("----------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 16;
   CmdMsg.MemType              = MM_MEM16;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 peek 
   **
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test MEM8 Peek \n");
   UTF_put_text("----------------\n");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize             = 8;
   CmdMsg.MemType              = MM_MEM8;
   MM_PeekCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);
   
} /* Test_Peeks */

/**** end file: ut_mm_test_peeks.c ****/
