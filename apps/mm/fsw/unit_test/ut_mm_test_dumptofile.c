/************************************************************************
** File:
**   $Id: ut_mm_test_dumptofile.c 1.2 2015/03/02 14:26:46EST sstrege Exp  $
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
**   to file
**
**   $Log: ut_mm_test_dumptofile.c  $
**   Revision 1.2 2015/03/02 14:26:46EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:09EST jmdagost 
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
** Global variables used by function hooks
*************************************************************************/
extern uint32   OS_WriteHookCallCount;
extern uint32   OS_WriteHookFailCount;

/************************************************************************
** Local function prototypes
*************************************************************************/
void    Test_DumpToFile(void);

extern void    PrintLocalHKVars(void);

/*
** Prototypes for function hooks
*/
extern int32   OS_WriteFailonN_Hook  (uint32 filedes, void* buffer, uint32 nbytes);
extern int32   OS_WriteShortonN_Hook (uint32 filedes, void* buffer, uint32 nbytes);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test memory dump to file processing                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_DumpToFile(void)
{
   MM_DumpMemToFileCmd_t  CmdMsg;

   /* Setup the test message header */ 
   CFE_SB_InitMsg(&CmdMsg, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
   
   UTF_put_text("*****************************\n");
   UTF_put_text("* Memory Dump To File Tests *\n");
   UTF_put_text("*****************************\n");
   UTF_put_text("\n");

   /*
   ** Test no dump filename specified
   */ 
   UTF_put_text("Test NUL Filename \n");
   UTF_put_text("------------------\n");
   CmdMsg.FileName[0] = '\0';
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);   
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test invalid dump filename
   */ 
   UTF_put_text("Test Invalid Filename \n");
   UTF_put_text("----------------------\n");
   strcpy(CmdMsg.FileName, "/ram/Bad*(Filename).dump");
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);   
   PrintLocalHKVars();   
   UTF_put_text("\n");      
      
   /*
   ** Test bad symbol name 
   */
   UTF_put_text("Test Command With Bad Symbol Name \n");
   UTF_put_text("----------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test1.dump");
   strcpy(CmdMsg.SrcSymAddress.SymName, "BadSymName");
   CmdMsg.SrcSymAddress.Offset = 0;
   CmdMsg.NumOfBytes           = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType              = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test RAM dump that exceeds configuration limits 
   */
   UTF_put_text("Test Dump That Exceeds RAM Configuration Limits \n");
   UTF_put_text("------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test2.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_FILE_DATA_RAM + 200;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM dump that exceeds configuration limits 
   */
   UTF_put_text("Test Dump That Exceeds EEPROM Configuration Limits \n");
   UTF_put_text("---------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test3.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_FILE_DATA_EEPROM + 200;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 dump that exceeds configuration limits 
   */
   UTF_put_text("Test Dump That Exceeds MEM32 Configuration Limits \n");
   UTF_put_text("--------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test4.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_FILE_DATA_MEM32 + 200;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 dump that exceeds configuration limits 
   */
   UTF_put_text("Test Dump That Exceeds MEM16 Configuration Limits \n");
   UTF_put_text("--------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test5.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_FILE_DATA_MEM16 + 200;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 dump that exceeds configuration limits 
   */
   UTF_put_text("Test Dump That Exceeds MEM8 Configuration Limits \n");
   UTF_put_text("-------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test6.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_FILE_DATA_MEM8 + 200;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 dump with misaligned address 
   */
   UTF_put_text("Test MEM32 Dump With Misaligned Address \n");
   UTF_put_text("----------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test7.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 dump with misaligned data size
   */
   UTF_put_text("Test MEM32 Dump With Misaligned Data Size \n");
   UTF_put_text("------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test8.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE - 3;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 dump with misaligned address 
   */
   UTF_put_text("Test MEM16 Dump With Misaligned Address \n");
   UTF_put_text("----------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test9.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR + 3;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 dump with misaligned data size
   */
   UTF_put_text("Test MEM16 Dump With Misaligned Data Size \n");
   UTF_put_text("------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test10.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE - 3;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test dump with an undefined memory type 
   */
   UTF_put_text("Test Dump With Invalid Memory Type \n");
   UTF_put_text("-----------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test11.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = 0xFF;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test dump with bad file path 
   */
   UTF_put_text("Test Dump With Bad File Path \n");
   UTF_put_text("-----------------------------\n");
   strcpy(CmdMsg.FileName, "/nodir/test12.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test RAM dump with bad address 
   */
   UTF_put_text("Test RAM Dump With Bad Address \n");
   UTF_put_text("-------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test13.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM dump with bad address 
   */
   UTF_put_text("Test EEPROM Dump With Bad Address \n");
   UTF_put_text("----------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test14.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_EEPROM_MEM_SIZE;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 dump with bad address 
   */
   UTF_put_text("Test MEM32 Dump With Bad Address \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test15.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
  
   /*
   ** Test MEM16 dump with bad address 
   */
   UTF_put_text("Test MEM16 Dump With Bad Address \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test16.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 dump with bad address 
   */
   UTF_put_text("Test MEM8 Dump With Bad Address \n");
   UTF_put_text("--------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test17.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /************************************************************************
   ** The following tests force return codes from OSAL file I/O routines
   ** to increase code coverage. 
   *************************************************************************/
   /*
   ** Test RAM dump with OS_close error
   ** Forces error in MM_DumpMemToFileCmd routine
   */
   UTF_OSFILEAPI_Set_Api_Return_Code(OS_CLOSE_PROC, OS_FS_ERROR);
   UTF_put_text("Test RAM Dump With OS_close Error \n");
   UTF_put_text("----------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test18.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CLOSE_PROC);

   /*
   ** Test RAM dump with OS_read error
   ** Forces error return from CFS_ComputeCRCFromFile routine
   */
   UTF_OSFILEAPI_Set_Api_Return_Code(OS_READ_PROC, OS_FS_ERROR);
   UTF_put_text("Test RAM Dump With OS_read Error \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test19.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_READ_PROC);

   /*
   ** Test RAM dump with OS_write error on first call
   ** Forces first error catch in MM_WriteFileHeaders routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 1;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteFailonN_Hook);
   UTF_put_text("Test RAM Dump With OS_write On Call 1 Error \n");
   UTF_put_text("--------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test20.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);
   
   /*
   ** Test RAM dump with OS_write error on second call
   ** Forces second error catch in MM_WriteFileHeaders routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 2;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteFailonN_Hook);
   UTF_put_text("Test RAM Dump With OS_write On Call 2 Error \n");
   UTF_put_text("--------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test21.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);
   
   /*
   ** Test RAM dump smaller than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMemToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Small RAM Dump With OS_write On Call 3 Short \n");
   UTF_put_text("--------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test22.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = (MM_MAX_DUMP_DATA_SEG / 2);
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test RAM dump larger than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMemToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large RAM Dump With OS_write On Call 3 Short \n");
   UTF_put_text("--------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test23.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test RAM dump larger than a segment size but not a multiple
   ** of the segment size with OS_write returning a short write count 
   ** on the 4th call
   ** Forces error catch in MM_DumpMemToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 4;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large RAM Dump With OS_write On Call 4 Short \n");
   UTF_put_text("--------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test24.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_DATA_SEG + 24;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);
   
   /*
   ** Test MEM32 dump smaller than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMem32ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Small MEM32 Dump With OS_write On Call 3 Short \n");
   UTF_put_text("----------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test25.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = (MM_MAX_DUMP_DATA_SEG / 2);
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM32 dump larger than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMem32ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large MEM32 Dump With OS_write On Call 3 Short \n");
   UTF_put_text("----------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test26.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM32 dump larger than a segment size but not a multiple
   ** of the segment size with OS_write returning a short write count 
   ** on the 4th call
   ** Forces error catch in MM_DumpMem32ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 4;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large MEM32 Dump With OS_write On Call 4 Short \n");
   UTF_put_text("----------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test27.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_DATA_SEG + 24;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM16 dump smaller than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMem16ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Small MEM16 Dump With OS_write On Call 3 Short \n");
   UTF_put_text("----------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test28.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = (MM_MAX_DUMP_DATA_SEG / 2);
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM16 dump larger than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMem16ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large MEM16 Dump With OS_write On Call 3 Short \n");
   UTF_put_text("----------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test29.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM16 dump larger than a segment size but not a multiple
   ** of the segment size with OS_write returning a short write count 
   ** on the 4th call
   ** Forces error catch in MM_DumpMem16ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 4;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large MEM16 Dump With OS_write On Call 4 Short \n");
   UTF_put_text("----------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test30.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_DATA_SEG + 24;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);
   
   /*
   ** Test MEM8 dump smaller than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMem16ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Small MEM8 Dump With OS_write On Call 3 Short \n");
   UTF_put_text("---------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test31.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = (MM_MAX_DUMP_DATA_SEG / 2);
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM8 dump larger than a segment size with 
   ** OS_write returning a short write count on the 3rd call
   ** Forces error catch in MM_DumpMem16ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large MEM8 Dump With OS_write On Call 3 Short \n");
   UTF_put_text("---------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test32.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);

   /*
   ** Test MEM8 dump larger than a segment size but not a multiple
   ** of the segment size with OS_write returning a short write count 
   ** on the 4th call
   ** Forces error catch in MM_DumpMem16ToFile routine
   */
   OS_WriteHookCallCount = 1;   /* Always need to init this */
   OS_WriteHookFailCount = 4;
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, OS_WriteShortonN_Hook);
   UTF_put_text("Test Large MEM8 Dump With OS_write On Call 4 Short \n");
   UTF_put_text("---------------------------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test33.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = MM_MAX_DUMP_DATA_SEG + 24;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, NULL);
   
   /************************************************************************
   ** End OSAL file I/O error return tests
   *************************************************************************/
   
   /*
   ** Test RAM dump larger than a segment size 
   */
   UTF_put_text("Test Large Valid Dump From RAM \n");
   UTF_put_text("-------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test34.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test RAM dump smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Dump From RAM \n");
   UTF_put_text("-------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test35.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = 100;
   CmdMsg.MemType                  = MM_RAM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test EEPROM dump larger than a segment size 
   */
   UTF_put_text("Test Large Valid Dump From EEPROM \n");
   UTF_put_text("----------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test36.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_EEPROM_MEM_SIZE;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test EEPROM dump smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Dump From EEPROM \n");
   UTF_put_text("----------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test37.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_EEPROM_MEM_ADDR;
   CmdMsg.NumOfBytes               = 100;
   CmdMsg.MemType                  = MM_EEPROM;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM32 dump larger than a segment size 
   */
   UTF_put_text("Test Large Valid Dump From MEM32 \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test38.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM32 dump smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Dump From MEM32 \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test39.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = 100;
   CmdMsg.MemType                  = MM_MEM32;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM16 dump larger than a segment size 
   */
   UTF_put_text("Test Large Valid Dump From MEM16 \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test40.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM16 dump smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Dump From MEM16 \n");
   UTF_put_text("---------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test41.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = 100;
   CmdMsg.MemType                  = MM_MEM16;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test MEM8 dump larger than a segment size 
   */
   UTF_put_text("Test Large Valid Dump From MEM8 \n");
   UTF_put_text("--------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test42.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = SIM_RAM_MEM_SIZE;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test MEM8 dump smaller than a segment size 
   */
   UTF_put_text("Test Small Valid Dump From MEM8 \n");
   UTF_put_text("--------------------------------\n");
   strcpy(CmdMsg.FileName, "/ram/test43.dump");
   CmdMsg.SrcSymAddress.SymName[0] = '\0';
   CmdMsg.SrcSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes               = 100;
   CmdMsg.MemType                  = MM_MEM8;
   MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
} /* Test_DumpToFile */

/**** end file: ut_mm_test_dumptofile.c ****/
