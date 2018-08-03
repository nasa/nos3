/************************************************************************
** File:
**   $Id: ut_mm_test_pokes.c 1.2 2015/03/02 14:26:50EST sstrege Exp  $
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
**   This is a test driver subroutine to test CFS Memory Manager (MM) pokes
**
**   $Log: ut_mm_test_pokes.c  $
**   Revision 1.2 2015/03/02 14:26:50EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:18EST jmdagost 
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
void    Test_Pokes(void);

extern void    PrintLocalHKVars(void);

extern int32   OS_EepromWrite32_Hook (uint32 MemoryAddress, uint32 Value);
extern int32   OS_EepromWrite16_Hook (uint32 MemoryAddress, uint16 Value);
extern int32   OS_EepromWrite8_Hook  (uint32 MemoryAddress,  uint8 Value);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test memory pokes                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_Pokes(void)
{
   MM_PokeCmd_t  CmdMsg;

   /* Setup the test message header */ 
   CFE_SB_InitMsg(&CmdMsg, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
   
   UTF_put_text("\n");
   UTF_put_text("*********************\n");
   UTF_put_text("* Memory Poke Tests *\n");
   UTF_put_text("*********************\n");
   UTF_put_text("\n");

   /*
   ** Test bad symbol name 
   */
   UTF_put_text("Test Poke With Bad Symbol Name \n");
   UTF_put_text("-------------------------------\n");
   strcpy(CmdMsg.DestSymAddress.SymName, "BadSymName");
   CmdMsg.DestSymAddress.Offset = 0;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test 16 bit poke with misaligned address 
   */
   UTF_put_text("Test Poke 16 With Misaligned Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test 32 bit poke with misaligned address 
   */
   UTF_put_text("Test Poke 32 With Misaligned Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test poke with invalid data size 
   */
   UTF_put_text("Test Poke With Invalid Data Size \n");
   UTF_put_text("---------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 12;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test poke with invalid RAM address 
   */
   UTF_put_text("Test Poke With Invalid RAM Address \n");
   UTF_put_text("-----------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;   
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test poke with invalid EEPROM address 
   */
   UTF_put_text("Test Poke With Invalid EEPROM Address \n");
   UTF_put_text("--------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;   
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM 32 bit poke with error return from
   ** OS_EepromWrite32
   */
   UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE32_HOOK, 
                             OS_EepromWrite32_Hook);

   UTF_put_text("Test EEPROM Poke 32 with OS_EepromWrite32 error return \n");
   UTF_put_text("-------------------------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE32_HOOK, NULL);

   /*
   ** Test EEPROM 16 bit poke with error return from
   ** OS_EepromWrite16 
   */
   UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE16_HOOK, 
                             OS_EepromWrite16_Hook);
   
   UTF_put_text("Test EEPROM Poke 16 with OS_EepromWrite16 error return \n");
   UTF_put_text("-------------------------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0xFEDCBA98;
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE16_HOOK, NULL);

   /*
   ** Test EEPROM 8 bit poke with error return from
   ** OS_EepromWrite8
   **
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE8_HOOK, 
                             OS_EepromWrite8_Hook);
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test EEPROM Poke 8 with OS_EepromWrite8 error return \n");
   UTF_put_text("-----------------------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize              = 8;
   CmdMsg.Data                  = 0x1A2B3C4D;
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);
   UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE8_HOOK, NULL);
   
   /*
   ** Test poke with invalid memory type 
   */
   UTF_put_text("Test Poke With Invalid Memory Type \n");
   UTF_put_text("-----------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = 0xFF;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 poke with invalid data size 
   */
   UTF_put_text("Test MEM32 Poke With Invalid Data Size \n");
   UTF_put_text("---------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_MEM32;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 poke with invalid address 
   */
   UTF_put_text("Test MEM32 Poke With Invalid Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;   
   CmdMsg.MemType               = MM_MEM32;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 poke with invalid data size 
   */
   UTF_put_text("Test MEM16 Poke With Invalid Data Size \n");
   UTF_put_text("---------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_MEM16;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 poke with invalid address 
   */
   UTF_put_text("Test MEM16 Poke With Invalid Address \n");
   UTF_put_text("-------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0x1234ABCD;   
   CmdMsg.MemType               = MM_MEM16;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 poke with invalid data size 
   */
   UTF_put_text("Test MEM8 Poke With Invalid Data Size \n");
   UTF_put_text("--------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_MEM8;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM8 poke with invalid address 
   */
   UTF_put_text("Test MEM8 Poke With Invalid Address \n");
   UTF_put_text("------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_BAD_MEM_ADDR;
   CmdMsg.DataSize              = 8;
   CmdMsg.Data                  = 0x1234ABCD;   
   CmdMsg.MemType               = MM_MEM8;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM 32 bit poke 
   */
   UTF_put_text("Test RAM Poke 32 \n");
   UTF_put_text("-----------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM 16 bit poke 
   */
   UTF_put_text("Test RAM Poke 16 \n");
   UTF_put_text("-----------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0xFEDCBA98;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM 8 bit poke 
   **
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test RAM Poke 8 \n");
   UTF_put_text("----------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 8;
   CmdMsg.Data                  = 0x1A2B3C4D;
   CmdMsg.MemType               = MM_RAM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);

   /*
   ** Test EEPROM 32 bit poke 
   */
   UTF_put_text("Test EEPROM Poke 32 \n");
   UTF_put_text("--------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM 16 bit poke 
   */
   UTF_put_text("Test EEPROM Poke 16 \n");
   UTF_put_text("--------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0xFEDCBA98;
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM 8 bit poke 
   **
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test EEPROM Poke 8 \n");
   UTF_put_text("-------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   CmdMsg.DataSize              = 8;
   CmdMsg.Data                  = 0x1A2B3C4D;
   CmdMsg.MemType               = MM_EEPROM;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);

   /*
   ** Test MEM32 poke 
   */
   UTF_put_text("Test MEM32 Poke \n");
   UTF_put_text("----------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 32;
   CmdMsg.Data                  = 0x1234ABCD;
   CmdMsg.MemType               = MM_MEM32;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 poke 
   */
   UTF_put_text("Test MEM16 Poke \n");
   UTF_put_text("----------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 16;
   CmdMsg.Data                  = 0xFEDCBA98;
   CmdMsg.MemType               = MM_MEM16;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 poke 
   **
   ** Note: We force an OS_SUCCESS return from CFE_PSP_MemValidateRange
   ** because there is a bug in the current implementation (OSAL 2.12 / cFE 5.2) 
   ** that will cause a validation failure for a data size of 1 byte. This
   ** can be removed after it's fixed
   */
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
   UTF_put_text("Test MEM8 Poke \n");
   UTF_put_text("----------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset = SIM_RAM_MEM_ADDR;
   CmdMsg.DataSize              = 8;
   CmdMsg.Data                  = 0x1A2B3C4D;
   CmdMsg.MemType               = MM_MEM8;
   MM_PokeCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);

} /* end Test_Pokes */

/**** end file: ut_mm_test_pokes.c ****/
