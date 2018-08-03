/************************************************************************
** File:
**   $Id: ut_mm_test_fill.c 1.2 2015/03/02 14:26:54EST sstrege Exp  $
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
**   This is a test driver subroutine to test CFS Memory Manager (MM) fills
**
**   $Log: ut_mm_test_fill.c  $
**   Revision 1.2 2015/03/02 14:26:54EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:11EST jmdagost 
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
#define SIM_RAM_MEM_SIZE      1024        /* UTF Simulated RAM Memory Size    */

#define SIM_EEPROM_MEM_ADDR   0xC0000000  /* UTF Simulated EEPROM Memory Address */   
#define SIM_EEPROM_MEM_SIZE   1024        /* UTF Simulated EEPROM Memory Size    */

/*
** Use this address when you want CFE_PSP_MemValidateRange to fail since 
** it's not registered using the CFE_PSP_MemRangeSet calls below
*/
#define SIM_BAD_MEM_ADDR      0xD0000000  

/************************************************************************
** Local function prototypes
*************************************************************************/
void    Test_Fill(void);

extern void    PrintLocalHKVars(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test memory fill                                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_Fill(void)
{
   MM_FillMemCmd_t        FillCmdMsg;
   MM_DumpMemToFileCmd_t  DumpCmdMsg;

   /* Setup the test message header for Fill commands */ 
   CFE_SB_InitMsg(&FillCmdMsg, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);

   /* Setup the test message header for Dump commands */ 
   CFE_SB_InitMsg(&DumpCmdMsg, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
   
   UTF_put_text("*********************\n");
   UTF_put_text("* Memory Fill Tests *\n");
   UTF_put_text("*********************\n");
   UTF_put_text("\n");

   /*
   ** Test bad symbol name 
   */
   UTF_put_text("Test Fill With Bad Symbol Name \n");
   UTF_put_text("-------------------------------\n");
   strcpy(FillCmdMsg.DestSymAddress.SymName, "BadSymName");
   FillCmdMsg.DestSymAddress.Offset = 0;
   FillCmdMsg.FillPattern           = 0x1234ABCD;
   FillCmdMsg.NumOfBytes            = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType               = MM_RAM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM fill that exceeds configuration limits 
   */
   UTF_put_text("Test Fill That Exceeds RAM Configuration Limits \n");
   UTF_put_text("-------------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = MM_MAX_FILL_DATA_RAM + 200;
   FillCmdMsg.MemType                   = MM_RAM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM fill that exceeds configuration limits 
   */
   UTF_put_text("Test Fill That Exceeds EEPROM Configuration Limits \n");
   UTF_put_text("-------------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = MM_MAX_FILL_DATA_EEPROM + 200;
   FillCmdMsg.MemType                   = MM_EEPROM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
 
   /*
   ** Test MEM32 fill that exceeds configuration limits 
   */
   UTF_put_text("Test Fill That Exceeds MEM32 Configuration Limits \n");
   UTF_put_text("--------------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = MM_MAX_FILL_DATA_MEM32 + 200;
   FillCmdMsg.MemType                   = MM_MEM32;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 fill that exceeds configuration limits 
   */
   UTF_put_text("Test Fill That Exceeds MEM16 Configuration Limits \n");
   UTF_put_text("--------------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = MM_MAX_FILL_DATA_MEM16 + 200;
   FillCmdMsg.MemType                   = MM_MEM16;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
  
   /*
   ** Test MEM8 fill that exceeds configuration limits 
   */
   UTF_put_text("Test Fill That Exceeds MEM8 Configuration Limits \n");
   UTF_put_text("--------------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = MM_MAX_FILL_DATA_MEM8 + 200;
   FillCmdMsg.MemType                   = MM_MEM8;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 fill with misaligned address 
   */
   UTF_put_text("Test MEM32 Fill With Misaligned Address \n");
   UTF_put_text("----------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR + 3;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM32;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 fill with misaligned data size 
   */
   UTF_put_text("Test MEM32 Fill With Misaligned Data Size \n");
   UTF_put_text("------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE - 3;
   FillCmdMsg.MemType                   = MM_MEM32;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 fill with misaligned address 
   */
   UTF_put_text("Test MEM16 Fill With Misaligned Address \n");
   UTF_put_text("----------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR + 3;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM16;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 fill with misaligned data size 
   */
   UTF_put_text("Test MEM16 Fill With Misaligned Data Size \n");
   UTF_put_text("------------------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE - 3;
   FillCmdMsg.MemType                   = MM_MEM16;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test fill with invalid memory type specified 
   */
   UTF_put_text("Test Fill With Invalid Memory Type \n");
   UTF_put_text("-----------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = 0xFF;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM fill with bad address 
   */
   UTF_put_text("Test RAM Fill With Bad Address \n");
   UTF_put_text("-------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_RAM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM fill with bad address 
   */
   UTF_put_text("Test EEPROM Fill With Bad Address \n");
   UTF_put_text("----------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_EEPROM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_EEPROM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 fill with bad address 
   */
   UTF_put_text("Test MEM32 Fill With Bad Address \n");
   UTF_put_text("---------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM32;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 fill with bad address 
   */
   UTF_put_text("Test MEM16 Fill With Bad Address \n");
   UTF_put_text("---------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM16;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 fill with bad address 
   */
   UTF_put_text("Test MEM8 Fill With Bad Address \n");
   UTF_put_text("--------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x1234ABCD;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM8;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test RAM fill larger than a segment size 
   */
   UTF_put_text("Test Large Valid Fill To RAM \n");
   UTF_put_text("-----------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x12345678;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_RAM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill1.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   DumpCmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test RAM fill smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Fill To RAM \n");
   UTF_put_text("-----------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0xAABCDEFF;
   FillCmdMsg.NumOfBytes                = 100;
   FillCmdMsg.MemType                   = MM_RAM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill2.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = 112;
   DumpCmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test EEPROM fill larger than a segment size 
   */
   UTF_put_text("Test Large Valid Fill To EEPROM \n");
   UTF_put_text("--------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x98765432;
   FillCmdMsg.NumOfBytes                = SIM_EEPROM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_EEPROM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill3.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = SIM_EEPROM_MEM_SIZE;
   DumpCmdMsg.MemType                  = MM_EEPROM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test EEPROM fill smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Fill To EEPROM \n");
   UTF_put_text("--------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0xFFEDCBAA;
   FillCmdMsg.NumOfBytes                = 100;
   FillCmdMsg.MemType                   = MM_EEPROM;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill4.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = 112;
   DumpCmdMsg.MemType                  = MM_EEPROM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 fill larger than a segment size 
   */
   UTF_put_text("Test Large Valid Fill To MEM32 \n");
   UTF_put_text("-------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x12345678;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM32;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill5.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   DumpCmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 fill smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Fill To MEM32 \n");
   UTF_put_text("-------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0xAABCDEFF;
   FillCmdMsg.NumOfBytes                = 100;
   FillCmdMsg.MemType                   = MM_MEM32;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill6.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = 112;
   DumpCmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 fill larger than a segment size 
   */
   UTF_put_text("Test Large Valid Fill To MEM16 \n");
   UTF_put_text("-------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x98765432;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM16;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill7.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   DumpCmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 fill smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Fill To MEM16 \n");
   UTF_put_text("-------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0xFFEDCBAA;
   FillCmdMsg.NumOfBytes                = 100;
   FillCmdMsg.MemType                   = MM_MEM16;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill8.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = 112;
   DumpCmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 fill larger than a segment size 
   */
   UTF_put_text("Test Large Valid Fill To MEM8 \n");
   UTF_put_text("------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0x12345678;
   FillCmdMsg.NumOfBytes                = SIM_RAM_MEM_SIZE;
   FillCmdMsg.MemType                   = MM_MEM8;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill9.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   DumpCmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 fill smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Fill To MEM8 \n");
   UTF_put_text("------------------------------\n");
   FillCmdMsg.DestSymAddress.SymName[0] = '\0';
   FillCmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   FillCmdMsg.FillPattern               = 0xAABCDEFF;
   FillCmdMsg.NumOfBytes                = 100;
   FillCmdMsg.MemType                   = MM_MEM8;
   MM_FillMemCmd((CFE_SB_MsgPtr_t)&FillCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Dump the fill area for post run inspection 
   */
   UTF_put_text("Dumping Fill Area \n");
   UTF_put_text("------------------\n");
   strcpy(DumpCmdMsg.FileName, "/ram/fill10.dump");
   DumpCmdMsg.SrcSymAddress.SymName[0] = '\0';
   DumpCmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   DumpCmdMsg.NumOfBytes               = 112;
   DumpCmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&DumpCmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
} /* end Test_Fill */

/**** end file: ut_mm_test_fill.c ****/
