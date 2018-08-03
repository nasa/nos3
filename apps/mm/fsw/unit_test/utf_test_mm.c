/************************************************************************
** File:
**   $Id: utf_test_mm.c 1.14 2015/03/20 14:16:46EDT lwalling Exp  $
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
**   This is a test driver used to unit test the CFS Memory Manager (MM)
**   Application.
**
**   The MM Application provides onboard hardware and software maintenance 
**   services by processing commands for memory operations and read and 
**   write accesses to memory mapped hardware.
** 
**   Output can be directed either to screen or to file:
**   To direct output to screen, 
**      comment in '#define UTF_USE_STDOUT' statement in the
**      utf_custom.h file.
**
**   To direct output to file, 
**      comment out '#define UTF_USE_STDOUT' statement in 
**      utf_custom.h file.
** 
**   $Log: utf_test_mm.c  $
**   Revision 1.14 2015/03/20 14:16:46EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.13 2015/03/02 14:26:48EST sstrege 
**   Added copyright information
**   Revision 1.12 2011/12/05 15:17:57EST jmdagost 
**   Updates for zero-bytes read on file load.
**   Revision 1.11 2011/11/30 16:08:05EST jmdagost 
**   Broke out sub-test functions into separate files.
**   Revision 1.10 2010/12/08 14:39:57EST jmdagost 
**   Added filename validation test for symbol table dump command.
**   Revision 1.9 2010/11/29 08:49:25EST jmdagost 
**   Added unit tests for EEPROM write-enable/disable commands.
**   Revision 1.8 2010/11/26 12:55:28EST jmdagost 
**   Added declaration of new symbol table dump command.
**   Revision 1.7 2010/11/24 17:10:04EST jmdagost 
**   Added tests for the Write Symbol Table to File command.
**   Revision 1.6 2010/05/27 15:17:15EDT jmdagost 
**   Updated header file references.
**   Revision 1.5 2009/06/18 10:17:10EDT rmcgraw 
**   DCR8291:1 Changed OS_MEM_ #defines to CFE_PSP_MEM_
**   Revision 1.4 2009/06/12 14:39:35EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.3 2008/09/23 13:43:46EDT dahardison 
**   Fixed dump and load file naming problem where
**   some names were getting reused
**   Revision 1.2 2008/09/22 14:29:41EDT dahardison 
**   Updated for cFE 5.2/OSAL 2.12 and MM v1.0.0.0
**   Revision 1.1 2008/05/19 15:28:10EDT dahardison 
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
#define MESSAGE_FORMAT_IS_CCSDS

#define MM_CMD_PIPE		      1

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
** MM global data
*************************************************************************/
extern  MM_AppData_t MM_AppData;
extern  OS_FDTableEntry OS_FDTable[OS_MAX_NUM_OPEN_FILES];

/************************************************************************
** MM test data set
*************************************************************************/
uint8   MM_TestDataSet[1000] =  {0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,
                                 0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,
                                 0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
                                 0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,
                                 0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,
                                 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x2A,0x2A,0x2A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,
                                 0x2B,0x2B,0x2B,0x2B,0x2B,0x2B,0x2B,0x2B,0x2B,0x2B,
                                 0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,
                                 0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,
                                 0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
                                 0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,
                                 0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,
                                 0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,
                                 0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,
                                 0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,
                                 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,
                                 0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,
                                 0x4C,0x4C,0x4C,0x4C,0x4C,0x4C,0x4C,0x4C,0x4C,0x4C,
                                 0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,
                                 0x4E,0x4E,0x4E,0x4E,0x4E,0x4E,0x4E,0x4E,0x4E,0x4E,
                                 0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
                                 0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,
                                 0x5C,0x5C,0x5C,0x5C,0x5C,0x5C,0x5C,0x5C,0x5C,0x5C,
                                 0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,
                                 0x5E,0x5E,0x5E,0x5E,0x5E,0x5E,0x5E,0x5E,0x5E,0x5E,
                                 0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x6A,0x6A,0x6A,0x6A,0x6A,0x6A,0x6A,0x6A,0x6A,0x6A,
                                 0x6B,0x6B,0x6B,0x6B,0x6B,0x6B,0x6B,0x6B,0x6B,0x6B,
                                 0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,
                                 0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,
                                 0x6E,0x6E,0x6E,0x6E,0x6E,0x6E,0x6E,0x6E,0x6E,0x6E,
                                 0x6F,0x6F,0x6F,0x6F,0x6F,0x6F,0x6F,0x6F,0x6F,0x6F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,
                                 0x7B,0x7B,0x7B,0x7B,0x7B,0x7B,0x7B,0x7B,0x7B,0x7B,
                                 0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,
                                 0x7D,0x7D,0x7D,0x7D,0x7D,0x7D,0x7D,0x7D,0x7D,0x7D,
                                 0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,
                                 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x8A,0x8A,0x8A,0x8A,0x8A,0x8A,0x8A,0x8A,0x8A,0x8A,
                                 0x8B,0x8B,0x8B,0x8B,0x8B,0x8B,0x8B,0x8B,0x8B,0x8B,
                                 0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,
                                 0x8D,0x8D,0x8D,0x8D,0x8D,0x8D,0x8D,0x8D,0x8D,0x8D,
                                 0x8E,0x8E,0x8E,0x8E,0x8E,0x8E,0x8E,0x8E,0x8E,0x8E,
                                 0x8F,0x8F,0x8F,0x8F,0x8F,0x8F,0x8F,0x8F,0x8F,0x8F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x9A,0x9A,0x9A,0x9A,0x9A,0x9A,0x9A,0x9A,0x9A,0x9A,
                                 0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,
                                 0x9C,0x9C,0x9C,0x9C,0x9C,0x9C,0x9C,0x9C,0x9C,0x9C,
                                 0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,
                                 0x9E,0x9E,0x9E,0x9E,0x9E,0x9E,0x9E,0x9E,0x9E,0x9E,
                                 0x9F,0x9F,0x9F,0x9F,0x9F,0x9F,0x9F,0x9F,0x9F,0x9F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
                                 0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,
                                 0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,
                                 0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,
                                 0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,
                                 0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,
                                 0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                 0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13};

/* Test data set CRC, this gets calculated in the main routine */
uint32   MM_TestDataSetCRC;

/************************************************************************
** Global variables used by function hooks
*************************************************************************/
uint32   OS_ReadHookCallCount  = 1;
uint32   OS_ReadHookFailCount  = 1;
uint32   OS_WriteHookCallCount = 1;
uint32   OS_WriteHookFailCount = 1;

/************************************************************************
** Local function prototypes
*************************************************************************/
extern void    Test_Peeks(void);

extern void    Test_Pokes(void);

extern void    Test_DumpInEvent(void);

extern void    Test_LoadWID(void);

extern void    Test_LoadFromFile(void);

extern void    Test_DumpToFile(void);

extern void    Test_Fill(void);

extern void    Test_SymLookup(void);

extern void    Test_SymTblDump(void);

boolean CreateTruncCFEHdrLoadFile(char FileName[]); 

boolean CreateTruncMMHdrLoadFile(char                     FileName[], 
                                 MM_LoadDumpFileHeader_t *MMFileHdr);

boolean CreateLoadFile(char                     FileName[], 
                       MM_LoadDumpFileHeader_t *MMFileHdr,
                       int32                    DataSetBytes);

boolean CreateBigLoadFile(char                     FileName[], 
                          MM_LoadDumpFileHeader_t *MMFileHdr,
                          int32                    NumDataSets);

void    PrintHKPacket(void);

void    PrintLocalHKVars(void);

/*
** Prototypes for function hooks
*/
int32   OS_ReadFailonN_Hook   (uint32 filedes, void* buffer, uint32 nbytes);
int32   OS_ReadZero_Hook      (uint32 filedes, void* buffer, uint32 nbytes);
int32   OS_WriteFailonN_Hook  (uint32 filedes, void* buffer, uint32 nbytes);
int32   OS_WriteShortonN_Hook (uint32 filedes, void* buffer, uint32 nbytes);
int32   OS_EepromWrite32_Hook (uint32 MemoryAddress, uint32 Value);
int32   OS_EepromWrite16_Hook (uint32 MemoryAddress, uint16 Value);
int32   OS_EepromWrite8_Hook  (uint32 MemoryAddress,  uint8 Value);
int32   CFE_SB_SubscribeHook  (CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);
void    TimeHook(void);

/* These are private functions in the files that contain them
** but we need access to them in this test driver
*/
void    MM_LookupSymbolCmd(CFE_SB_MsgPtr_t msg);
void    MM_SymTblToFileCmd(CFE_SB_MsgPtr_t msg);
void    CFS_LibInit(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Program main                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{
   char AppName[10];
   UTF_SymbolTable_t    UTF_Symbol;

   strcpy(AppName, "MM");
   
   /*
   ** Set up to read in script
   */
   UTF_add_input_file(MM_CMD_PIPE, "mm_utf_cmds.in");
   MM_AppData.CmdPipe = MM_CMD_PIPE;  /* Hook for application code */
   
   /*
   ** Set up output file and HK packet handler           
   */
   UTF_set_output_filename("mm_utf_test.out");
   UTF_set_packet_handler(MM_HK_TLM_MID, (utf_packet_handler)PrintHKPacket);
    
   /* 
   ** Set up simulated memory for loads and dumps 
   */
   UTF_add_sim_address(SIM_RAM_MEM_ADDR, SIM_RAM_MEM_SIZE, 
                                         "MM_RAM_ADDRESS_SPACE");

   UTF_add_sim_address(SIM_EEPROM_MEM_ADDR, SIM_EEPROM_MEM_SIZE, 
                                            "MM_EEPROM_ADDRESS_SPACE");
   
   /*
   ** Add these ranges to the OSAL memory table so the CFE_PSP_MemValidateRange 
   ** routine won't barf on them. We set these ranges much bigger than we're
   ** going to need so we can test bounds checking in MM and not 
   ** CFE_PSP_MemValidateRange.
   */
   CFE_PSP_MemRangeSet(0, CFE_PSP_MEM_RAM, SIM_RAM_MEM_ADDR, (MM_MAX_LOAD_FILE_DATA_RAM * 2), 
                                          CFE_PSP_MEM_SIZE_BYTE, CFE_PSP_MEM_ATTR_READWRITE);

   CFE_PSP_MemRangeSet(1, CFE_PSP_MEM_EEPROM, SIM_EEPROM_MEM_ADDR, (MM_MAX_LOAD_FILE_DATA_EEPROM * 10), 
                                                   CFE_PSP_MEM_SIZE_BYTE, CFE_PSP_MEM_ATTR_READWRITE);

   /*
   ** Setup the UTF symbol table structures
   */
   UTF_InitSymbolTable();
   
   strcpy(UTF_Symbol.symbolName, "GoodSymName");
   UTF_Symbol.symbolAddr = SIM_RAM_MEM_ADDR;
   
   UTF_SetSymbolTableEntry(UTF_Symbol);
   
   /*
   ** Initialize time data structures
   */
   UTF_init_sim_time(0.0);
   UTF_OSAPI_set_function_hook(OS_GETLOCALTIME_HOOK, TimeHook);

   /*
   ** Initialize the PSP EEPROM Write Ena/Dis return status
   */
   
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_EEPROMWRITEENA_PROC, CFE_PSP_SUCCESS);
   UTF_PSP_Set_Api_Return_Code(CFE_PSP_EEPROMWRITEDIS_PROC, CFE_PSP_SUCCESS);

   /*
   ** Register app MM with executive services.                         
   */
   UTF_ES_InitAppRecords();
   UTF_ES_AddAppRecord("MM",0);  
   CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
   
   /*
   ** Initialize table services data structures, though we
   ** don't use any tables for these tests
   */
   CFE_ES_CDS_EarlyInit();
   CFE_TBL_EarlyInit();

   /*
   ** Add an entry to the volume table
   */
   UTF_add_volume("/", "ram", FS_BASED, FALSE, FALSE, TRUE, "RAM", "/ram", 0);

   /*
   ** Add this hook so we can force a software bus read error
   ** in our command input file that will make the application exit
   */
   UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
   UTF_add_special_command("SET_PSP_RETURN_CODE", UTF_SCRIPT_PSP_Set_Api_Return_Code);

   /*
   ** Initialize the CRC value for our test data set
   */
   MM_TestDataSetCRC = CFE_ES_CalculateCRC(MM_TestDataSet, sizeof(MM_TestDataSet),
                                                           0, CFE_ES_DEFAULT_CRC);
   /*
   ** This is a function stub in cfs_utils.c that does nothing, but
   ** by calling it here we can increase our coverage statistics, so
   ** why not?
   */
   CFS_LibInit();
   
   /*
   ** Call test functions that invoke MM code directly 
   */
   printf("***UTF MM DRIVER TESTS START***\n\n");
   UTF_put_text("\n");
   UTF_put_text("***UTF MM DRIVER TESTS START***");
   UTF_put_text("\n\n");

   Test_Pokes();
   Test_Peeks();
   Test_LoadWID();
   Test_DumpInEvent();
   Test_LoadFromFile();
   Test_DumpToFile();
   Test_Fill();
   Test_SymLookup();
   Test_SymTblDump();
   
   printf("***UTF MM DRIVER TESTS END***\n\n");
   UTF_put_text("\n");
   UTF_put_text("***UTF MM DRIVER TESTS END***");
   UTF_put_text("\n\n");
   
   /* 
   ** Call Application Main procedure that will test command 
   ** processing through the software bus command pipe via
   ** the mm_utf_cmds.in command script
   */
   printf("***UTF MM CMD PIPE TESTS START***\n\n");
   UTF_put_text("\n");
   UTF_put_text("***UTF MM CMD PIPE TESTS START***");
   UTF_put_text("\n\n");
   
   MM_AppMain();
    
   printf("***UTF MM CMD PIPE TESTS END***\n\n");
   UTF_put_text("\n");
   UTF_put_text("***UTF MM CMD PIPE TESTS END***");
   UTF_put_text("\n\n");

   /*
   ** These tests force some CFE api error returns
   ** during MM initialization. This increases
   ** the gcov coverage metrics for the app startup
   ** code.
   */
   printf("***UTF MM APP INIT TESTS START***\n\n");
   UTF_put_text("\n");
   UTF_put_text("***UTF MM APP INIT TESTS START***");
   UTF_put_text("\n\n");
 
   UTF_put_text("\n");
   UTF_put_text("Test App Init Error conditions \n");
   UTF_put_text("-------------------------------\n");
   
   /*
   ** Set trigger so CFE_EVS_Register returns something
   ** other than CFE_SUCCESS (0). Then call app main, this
   ** should make the app init fail.
   */
   UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, 0xc2000003L);
   MM_AppMain();

   /* Go back to "normal" behavior */
   UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_REGISTER_PROC);
   
   /*
   ** Set trigger so CFE_SB_CreatePipe returns an error code
   */
   UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, 0xca000004L);
   MM_AppMain(); 
   UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_CREATEPIPE_PROC); 
   
   /*
   ** Set trigger so CFE_SB_Subscribe returns an error code
   */
   UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC, 0xca000009L);
   MM_AppMain();
   UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC); 

   /*
   ** Hook our own custom function to CFE_SB_Subscribe so we can
   ** trigger an error return on the SECOND call in MM_AppInit
   */
   UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *)&CFE_SB_SubscribeHook);
   MM_AppMain();
   
   printf("***UTF MM APP INIT TESTS END***\n\n");
   UTF_put_text("\n");
   UTF_put_text("***UTF MM APP INIT TESTS END***");
   UTF_put_text("\n\n");
   
   return 0;
   
} /* end main */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Creates a load file with a truncated cFE primary file header    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CreateTruncCFEHdrLoadFile(char FileName[]) 
{
   boolean Success = TRUE;
   int FileHandle;
   int BytesWritten;
   CFE_FS_Header_t   CFEFileHdr;
   
   /* Open the file */
   FileHandle = creat(FileName, 0755);
   
   if(FileHandle > -1)
   {
      /*
      ** Write a short cFE primary file header
      */
      memset(&CFEFileHdr, 0, sizeof(CFE_FS_Header_t));
      BytesWritten = write(FileHandle, &CFEFileHdr, (sizeof(CFE_FS_Header_t) - 4));
      if(BytesWritten != (sizeof(CFE_FS_Header_t) - 4))
      {
         printf("!!ERROR Writing Load File cFE Header, wrote: %d, expected: %d \n",
                                      BytesWritten, (sizeof(CFE_FS_Header_t) - 4));
         Success = FALSE;
         close(FileHandle);
      }
      else
      {
         close(FileHandle);
         
      }
      
   } /* end FileHandle > -1 if */
   else
   {
      printf("!!ERROR <CreateTruncCFEHdrLoadFile> Creating Load File, RC: %d \n", FileHandle);
      Success = FALSE;
   }
   
   return(Success);
   
} /* end CreateTruncCFEHdrLoadFile */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Creates a load file with a truncated MM secondary file header   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CreateTruncMMHdrLoadFile(char                     FileName[], 
                                 MM_LoadDumpFileHeader_t *MMFileHdr)
{
   boolean Success = TRUE;
   int FileHandle;
   int BytesWritten;
   CFE_FS_Header_t   CFEFileHdr;
   
   /* Open the file */
   FileHandle = creat(FileName, 0755);
   
   if(FileHandle > -1)
   {
      /*
      ** Write the cFE primary file header, we don't care about the values
      ** since MM doesn't use them for anything so we zero them out. 
      */
      memset(&CFEFileHdr, 0, sizeof(CFE_FS_Header_t));
      BytesWritten = write(FileHandle, &CFEFileHdr, sizeof(CFE_FS_Header_t));
      if(BytesWritten != sizeof(CFE_FS_Header_t))
      {
         printf("!!ERROR Writing Load File cFE Header, wrote: %d, expected: %d \n",
                                            BytesWritten, sizeof(CFE_FS_Header_t));
         Success = FALSE;
         close(FileHandle);
      }
      else
      {
         /*
         ** Write a short MM secondary header 
         */
         BytesWritten = write(FileHandle, MMFileHdr, (sizeof(MM_LoadDumpFileHeader_t) - 4));
         if(BytesWritten != (sizeof(MM_LoadDumpFileHeader_t) - 4))
         {
            printf("!!ERROR Writing Load File MM Header, wrote: %d, expected: %d \n",
                                   BytesWritten, (sizeof(MM_LoadDumpFileHeader_t) - 4));
            Success = FALSE;
            close(FileHandle);
         }
         else
         {
            close(FileHandle);
         }
         
      } /* end BytesWritten != sizeof(CFE_FS_Header_t) else */
      
   } /* end FileHandle > -1 if */
   else
   {
      printf("!!ERROR <CreateTruncMMHdrLoadFile> Creating Load File, RC: %d \n", FileHandle);
      Success = FALSE;
   }
   
   return(Success);
   
} /* end CreateTruncMMHdrLoadFile */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Creates a test load file, overwrites if it already exists       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CreateLoadFile(char                     FileName[], 
                       MM_LoadDumpFileHeader_t *MMFileHdr,
                       int32                    DataSetBytes)
{
   boolean Success = TRUE;
   int FileHandle;
   int BytesWritten;
   CFE_FS_Header_t   CFEFileHdr;
   
   /* Open the file */
   FileHandle = creat(FileName, 0755);
   
   if(FileHandle > -1)
   {
      /*
      ** Write the cFE primary file header, we don't care about the values
      ** since MM doesn't use them for anything so we zero them out. 
      */
      memset(&CFEFileHdr, 0, sizeof(CFE_FS_Header_t));
      BytesWritten = write(FileHandle, &CFEFileHdr, sizeof(CFE_FS_Header_t));
      if(BytesWritten != sizeof(CFE_FS_Header_t))
      {
         printf("!!ERROR Writing Load File cFE Header, wrote: %d, expected: %d \n",
                                            BytesWritten, sizeof(CFE_FS_Header_t));
         Success = FALSE;
         close(FileHandle);
      }
      else
      {
         /*
         ** Write the MM secondary file header 
         */
         BytesWritten = write(FileHandle, MMFileHdr, sizeof(MM_LoadDumpFileHeader_t));
         if(BytesWritten != sizeof(MM_LoadDumpFileHeader_t))
         {
            printf("!!ERROR Writing Load File MM Header, wrote: %d, expected: %d \n",
                                      BytesWritten, sizeof(MM_LoadDumpFileHeader_t));
            Success = FALSE;
            close(FileHandle);
         }
         else
         {
            /*
            ** Write the specified number of bytes from the data set array
            */
            BytesWritten = write(FileHandle, MM_TestDataSet, DataSetBytes);
            if(BytesWritten != DataSetBytes)
            {
               printf("!!ERROR Writing Load File Data Set, wrote: %d, expected: %ld \n",
                                                            BytesWritten, DataSetBytes);
               Success = FALSE;
               close(FileHandle);
            }
            else
            {
               close(FileHandle);
            }
         }
         
      } /* end BytesWritten != sizeof(CFE_FS_Header_t) else */

   } /* end FileHandle > -1 if */
   else
   {
      printf("!!ERROR <CreateLoadFile> Creating Load File, RC: %d \n", FileHandle);
      Success = FALSE;
   }
   
   return(Success);
   
} /* end CreateLoadFile */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Creates a load file that can be multiple data sets in size      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CreateBigLoadFile(char                     FileName[], 
                          MM_LoadDumpFileHeader_t *MMFileHdr,
                          int32                    NumDataSets)
{
   boolean Success = TRUE;
   int    i;
   int    FileHandle;
   int    BytesWritten;
   CFE_FS_Header_t   CFEFileHdr;
   uint32 CRCValue = 0;
   
   /* 
   ** Calculate the overall CRC value for the number of data sets that
   ** we are going to write to the file
   */ 
   for (i = 0; i < NumDataSets; i++)
   {
     CRCValue = CFE_ES_CalculateCRC(MM_TestDataSet, sizeof(MM_TestDataSet),
                                             CRCValue, CFE_ES_DEFAULT_CRC);
   }
  
   /* Set file header fields accordingly */
   MMFileHdr -> NumOfBytes = (sizeof(MM_TestDataSet)* NumDataSets);
   MMFileHdr -> Crc        = CRCValue;
   
   /* Open the file */
   FileHandle = creat(FileName, 0755);
   
   if(FileHandle > -1)
   {
      /*
      ** Write the cFE primary file header, we don't care about the values
      ** since MM doesn't use them for anything so we zero them out. 
      */
      memset(&CFEFileHdr, 0, sizeof(CFE_FS_Header_t));
      BytesWritten = write(FileHandle, &CFEFileHdr, sizeof(CFE_FS_Header_t));
      if(BytesWritten != sizeof(CFE_FS_Header_t))
      {
         printf("!!ERROR Writing Load File cFE Header, wrote: %d, expected: %d \n",
                                            BytesWritten, sizeof(CFE_FS_Header_t));
         Success = FALSE;
         close(FileHandle);
      }
      else
      {
         /*
         ** Write the MM secondary file header 
         */
         BytesWritten = write(FileHandle, MMFileHdr, sizeof(MM_LoadDumpFileHeader_t));
         if(BytesWritten != sizeof(MM_LoadDumpFileHeader_t))
         {
            printf("!!ERROR Writing Load File MM Header, wrote: %d, expected: %d \n",
                                      BytesWritten, sizeof(MM_LoadDumpFileHeader_t));
            Success = FALSE;
            close(FileHandle);
         }
         else
         {
            /*
            ** Write the data set segments
            */
            for (i = 0; i < NumDataSets; i++)
            {
               BytesWritten = write(FileHandle, MM_TestDataSet, sizeof(MM_TestDataSet));
               if(BytesWritten != sizeof(MM_TestDataSet))
               {
                  printf("!!ERROR Writing Big Load File, segment: %d, wrote: %d, expected: %d \n",
                                                         i, BytesWritten, sizeof(MM_TestDataSet));
                  Success = FALSE;
                  close(FileHandle);
                  break;
               }
               
            } /* end for */
            
            close(FileHandle);
            
         } /* end BytesWritten != sizeof(MM_LoadDumpFileHeader_t) else */
         
      } /* end BytesWritten != sizeof(CFE_FS_Header_t) else */
      
   } /* end FileHandle > -1 if */
   else
   {
      printf("!!ERROR <CreateBigLoadFile> Creating Load File, RC: %d \n", FileHandle);
      Success = FALSE;
   }
   
   return(Success);
   
} /* end CreateBigLoadFile */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values in the MM housekeeping packet     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintHKPacket (void)
{
   /* Output the LRO MM housekeeping data */
   UTF_put_text("MM HOUSEKEEPING DATA:\n");

   UTF_put_text("   Command Counter         = %d\n", MM_AppData.HkPacket.CmdCounter);
   UTF_put_text("   Command Error Counter   = %d\n", MM_AppData.HkPacket.ErrCounter);

   switch(MM_AppData.HkPacket.LastAction)
   {
      case MM_NOACTION:
         UTF_put_text("   Last Action             = MM_NOACTION\n");
         break;

      case MM_PEEK:
         UTF_put_text("   Last Action             = MM_PEEK\n");
         break;
         
      case MM_POKE:
         UTF_put_text("   Last Action             = MM_POKE\n");
         break;
         
      case MM_LOAD_FROM_FILE:
         UTF_put_text("   Last Action             = MM_LOAD_FROM_FILE\n");
         break;
         
      case MM_LOAD_WID:
         UTF_put_text("   Last Action             = MM_LOAD_WID\n");
         break;
         
      case MM_DUMP_TO_FILE:
         UTF_put_text("   Last Action             = MM_DUMP_TO_FILE\n");
         break;
         
      case MM_DUMP_INEVENT:
         UTF_put_text("   Last Action             = MM_DUMP_INEVENT\n");
         break;
         
      case MM_FILL:
         UTF_put_text("   Last Action             = MM_FILL\n");
         break;
         
      case MM_SYM_LOOKUP:
         UTF_put_text("   Last Action             = MM_SYM_LOOKUP\n");
         break;
         
      case MM_SYMTBL_SAVE:
         UTF_put_text("   Last Action             = MM_SYMTBL_SAVE\n");
         break;
            
      case MM_EEPROMWRITE_ENA:
         UTF_put_text("   Last Action             = MM_EEPROMWRITE_ENA\n");
         break;
            
      case MM_EEPROMWRITE_DIS:
         UTF_put_text("   Last Action             = MM_EEPROMWRITE_DIS\n");
         break;
            
      default:
         UTF_put_text("   Last Action             = %d\n", MM_AppData.HkPacket.LastAction);
         break;
   }

   switch(MM_AppData.HkPacket.MemType)
   {
      case MM_NOMEMTYPE:
         UTF_put_text("   Memory Type             = MM_NOMEMTYPE\n");
         break;

      case MM_RAM:
         UTF_put_text("   Memory Type             = MM_RAM\n");
         break;

      case MM_EEPROM:
         UTF_put_text("   Memory Type             = MM_EEPROM\n");
         break;

      case MM_MEM8:
         UTF_put_text("   Memory Type             = MM_MEM8\n");
         break;

      case MM_MEM16:
         UTF_put_text("   Memory Type             = MM_MEM16\n");
         break;

      case MM_MEM32:
         UTF_put_text("   Memory Type             = MM_MEM32\n");
         break;
         
      default:
         UTF_put_text("   Memory Type             = %d\n", MM_AppData.HkPacket.MemType);
         break;
   }
    
   UTF_put_text("   Address                 = 0x%08X\n", MM_AppData.HkPacket.Address);
   UTF_put_text("   Data Value              = 0x%08X\n", MM_AppData.HkPacket.DataValue);
   UTF_put_text("   Bytes Processed         = %d\n",     MM_AppData.HkPacket.BytesProcessed);
   UTF_put_text("   File Name               = '%s'\n",   MM_AppData.HkPacket.FileName);

} /* end PrintHKPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values in the local MM housekeeping      */
/* variables that don't get copied into the housekeeping packet    */
/* structure until a housekeeping request is received              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintLocalHKVars (void)
{
   /* Output the LRO MM housekeeping data */
   UTF_put_text("MM HOUSEKEEPING DATA:\n");

   UTF_put_text("   Command Counter         = %d\n", MM_AppData.CmdCounter);
   UTF_put_text("   Command Error Counter   = %d\n", MM_AppData.ErrCounter);

   switch(MM_AppData.LastAction)
   {
      case MM_NOACTION:
         UTF_put_text("   Last Action             = MM_NOACTION\n");
         break;

      case MM_PEEK:
         UTF_put_text("   Last Action             = MM_PEEK\n");
         break;
         
      case MM_POKE:
         UTF_put_text("   Last Action             = MM_POKE\n");
         break;
         
      case MM_LOAD_FROM_FILE:
         UTF_put_text("   Last Action             = MM_LOAD_FROM_FILE\n");
         break;
         
      case MM_LOAD_WID:
         UTF_put_text("   Last Action             = MM_LOAD_WID\n");
         break;
         
      case MM_DUMP_TO_FILE:
         UTF_put_text("   Last Action             = MM_DUMP_TO_FILE\n");
         break;
         
      case MM_DUMP_INEVENT:
         UTF_put_text("   Last Action             = MM_DUMP_INEVENT\n");
         break;
         
      case MM_FILL:
         UTF_put_text("   Last Action             = MM_FILL\n");
         break;
         
      case MM_SYM_LOOKUP:
         UTF_put_text("   Last Action             = MM_SYM_LOOKUP\n");
         break;
         
      case MM_SYMTBL_SAVE:
         UTF_put_text("   Last Action             = MM_SYMTBL_SAVE\n");
         break;
            
      default:
         UTF_put_text("   Last Action             = %d\n", MM_AppData.LastAction);
         break;
   }

   switch(MM_AppData.MemType)
   {
      case MM_NOMEMTYPE:
         UTF_put_text("   Memory Type             = MM_NOMEMTYPE\n");
         break;

      case MM_RAM:
         UTF_put_text("   Memory Type             = MM_RAM\n");
         break;

      case MM_EEPROM:
         UTF_put_text("   Memory Type             = MM_EEPROM\n");
         break;

      case MM_MEM8:
         UTF_put_text("   Memory Type             = MM_MEM8\n");
         break;

      case MM_MEM16:
         UTF_put_text("   Memory Type             = MM_MEM16\n");
         break;

      case MM_MEM32:
         UTF_put_text("   Memory Type             = MM_MEM32\n");
         break;
         
      default:
         UTF_put_text("   Memory Type             = %d\n", MM_AppData.MemType);
         break;
   }
    
   UTF_put_text("   Address                 = 0x%08X\n", MM_AppData.Address);
   UTF_put_text("   Data Value              = 0x%08X\n", MM_AppData.DataValue);
   UTF_put_text("   Bytes Processed         = %d\n",     MM_AppData.BytesProcessed);
   UTF_put_text("   File Name               = '%s'\n",   MM_AppData.FileName);

} /* end PrintLocalHKVars */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for OS_read that will return a zero               */
/* on the first call.                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 OS_ReadZero_Hook(uint32 filedes, void* buffer, uint32 nbytes)
{
   int32 Status = 0;

   if (OS_ReadHookCallCount == OS_ReadHookFailCount)
   {
       Status = 0;
   }
   else
   {
       /*
       ** Note: we can't call OS_read here or we'll recurse back
       ** into this routine infinitely.
       */
       Status = read (OS_FDTable[filedes].OSfd, (char*) buffer, (size_t) nbytes);
       OS_ReadHookCallCount++;
   }

   return(Status);
   
} /* end OS_ReadZero_Hook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for OS_read that will return an error             */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 OS_ReadFailonN_Hook(uint32 filedes, void* buffer, uint32 nbytes)
{
   int32 Status;

   /* 
    * Since the function hook's argument list has to match OS_read,
    * we use two global variables that are set by the calling
    * routine to control when we return an error
    */
   if (OS_ReadHookCallCount == OS_ReadHookFailCount)
   {
       Status = OS_FS_ERROR;
   }
   else
   {
       /*
       ** Note: we can't call OS_read here or we'll recurse back
       ** into this routine infinitely.
       */
       Status = read (OS_FDTable[filedes].OSfd, (char*) buffer, (size_t) nbytes);
       OS_ReadHookCallCount++;
   }

   return(Status);
   
} /* end OS_ReadFailonN_Hook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for OS_write that will return an error            */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 OS_WriteFailonN_Hook(uint32 filedes, void* buffer, uint32 nbytes)
{
   int32 Status;

   /* 
    * Since the function hook's argument list has to match OS_write,
    * we use two global variables that are set by the calling
    * routine to control when we return an error
    */
   if (OS_WriteHookCallCount == OS_WriteHookFailCount)
   {
       Status = OS_FS_ERROR;
   }
   else
   {
       /*
       ** Note: we can't call OS_write here or we'll recurse back
       ** into this routine infinitely.
       */
       Status = write(OS_FDTable[filedes].OSfd, (char*) buffer, (size_t) nbytes );
       OS_WriteHookCallCount++;
   }

   return(Status);
   
} /* end OS_WriteFailonN_Hook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for OS_write that will return a short byte count  */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 OS_WriteShortonN_Hook(uint32 filedes, void* buffer, uint32 nbytes)
{
   int32  Status;
   uint32 ShortCount;
   
   /* 
    * Since the function hook's argument list has to match OS_write,
    * we use two global variables that are set by the calling
    * routine to control when we return an error
    */
   if (OS_WriteHookCallCount == OS_WriteHookFailCount)
   {
       ShortCount = nbytes - 4;
       Status = write(OS_FDTable[filedes].OSfd, (char*) buffer, (size_t) ShortCount );
   }
   else
   {
       /*
       ** Note: we can't call OS_write here or we'll recurse back
       ** into this routine infinitely.
       */
       Status = write(OS_FDTable[filedes].OSfd, (char*) buffer, (size_t) nbytes );
       OS_WriteHookCallCount++;
   }

   return(Status);
   
} /* end OS_WriteShortonN_Hook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook functions for EEPROM write calls that will return          */
/* an error every time                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32   OS_EepromWrite32_Hook(uint32 MemoryAddress, uint32 Value)
{
    return (OS_ERROR_TIMEOUT);    
}

int32   OS_EepromWrite16_Hook(uint32 MemoryAddress, uint16 Value)
{
    return (OS_ERROR_TIMEOUT);
}

int32   OS_EepromWrite8_Hook (uint32 MemoryAddress,  uint8 Value)
{
    return (OS_ERROR_TIMEOUT);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_SB_Subscribe that will return an error    */
/* on every call except the first one                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId) 
{
    static uint32 Count = 0;

    if (Count == 0)
    {
       Count++;
       return(CFE_SUCCESS);
    }
    else
       return(CFE_SB_MAX_MSGS_MET);
    
}/* end CFE_SB_SubscribeHook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to simulate time incrementing                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TimeHook(void)
{
   UTF_set_sim_time(UTF_get_sim_time() + 1.0);
} /* end TimeHook */
