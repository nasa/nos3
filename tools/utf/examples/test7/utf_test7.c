/* File: utf_test7.c
 * 
 * This test makes calls to osapi function.
 *
 * Output can be directed either to screen or to file.
 * To direct output to screen, 
 *    comment in '#define UTF_USE_STDOUT' statement in the
 *    utf_custom.h file.
 *
 * To direct output to file, 
 *    comment out '#define UTF_USE_STDOUT' statement in 
 *    utf_custom.h file.
 *
 * Change History
 * 2006/01/27 nschweiss Original version.
 */ 

#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
/*#include "cfe.h" */
/*#include "cfe_mission_cfg.h"  defines CCSDS msg format */
/* #include "ccsds.h"            defines CCSDS_PriHdr_t */
/* #include "cfe_msgids.h"		 defines TT message ids */
#include "cfe_psp.h"
#include "utf_custom.h"      /* defines UTF_QUEUE_SIZE */
#include "utf_types.h"       /* defines UTF_queue_add() */
#include "utf_osapi.h"
#include "utf_cfe_psp.h"
#include "utf_osloader.h"
#include "string.h"

/* Function Hook Test routines */
int32 CFE_PSP_EepromWrite8Hook(uint32 Address, uint8 Data)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(CFE_PSP_INVALID_POINTER);
}

int32 CFE_PSP_EepromWrite16Hook(uint32 Address, uint16 Data)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(CFE_PSP_INVALID_POINTER);
}

int32 CFE_PSP_EepromWrite32Hook(uint32 Address, uint32 Data)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(CFE_PSP_INVALID_POINTER);
}

int main(void)
{
	uint8  DataWord8;
	uint16 DataWord16;
	uint32 DataWord32;
	uint32 maxRangeNum;
	uint32 memType;
	uint32 startAddr;
	uint32 byteSize;
	uint32 wordSize;
	uint32 attribs;
	
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test7.ActualOutput");
	
	/*************************************/
	/* Create simulated memory segments  */
	/*************************************/
	UTF_put_text("#\n# Create simulated memory regions\n#");
	UTF_put_text("\n ->Create MemoryRange1 start=0x1000 size=0x100\n");
	UTF_add_sim_address(0x1000,0x100, "MemoryRange1");	
	UTF_put_text(" ->Create MemoryRange2 start=0x2000 size=0x100\n");
	UTF_add_sim_address(0x2000,0x100, "MemoryRange2");

	UTF_put_text("#\n# Set simulated memory ranges\n#");
	UTF_put_text("CFE_PSP_MemRangeSet(0,RAM,%x,%x,BYTE,RW) returned 0x%08x. Expected 0x%08x\n",
		0x1000,0x100,
		CFE_PSP_MemRangeSet(0,CFE_PSP_MEM_RAM,0x1000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE),
		CFE_PSP_SUCCESS);
	UTF_put_text("CFE_PSP_MemRangeSet(1,EEPROM,%x,%x,BYTE,RW) returned 0x%08x. Expected 0x%08x\n",
		0x2000,0x100,
		CFE_PSP_MemRangeSet(1,CFE_PSP_MEM_EEPROM,0x2000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE),
		CFE_PSP_SUCCESS);
	
	/**************************************************/
	/* Use osapi functions      */
	/**************************************************/
	UTF_put_text("\n#\n# Test Memory Range calls\n#\n");
    UTF_put_text("CFE_PSP_MemValidateRange(x1000,x100,EEPROM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x1000,0x100,CFE_PSP_MEM_EEPROM), CFE_PSP_INVALID_MEM_TYPE);
    UTF_put_text("CFE_PSP_MemValidateRange(x2000,x100,EEPROM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x2000,0x100,CFE_PSP_MEM_EEPROM), CFE_PSP_SUCCESS);
    UTF_put_text("CFE_PSP_MemValidateRange(x1000,x100,RAM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x1000,0x100,CFE_PSP_MEM_RAM), CFE_PSP_SUCCESS);
    UTF_put_text("CFE_PSP_MemValidateRange(x2000,x100,RAM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x2000,0x100,CFE_PSP_MEM_RAM), CFE_PSP_INVALID_MEM_TYPE);
    UTF_put_text("CFE_PSP_MemValidateRange(x1000,x101,RAM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x1000,0x101,CFE_PSP_MEM_RAM), CFE_PSP_INVALID_MEM_RANGE);
    UTF_put_text("CFE_PSP_MemValidateRange(x2000,x101,EEPROM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x2000,0x101,CFE_PSP_MEM_EEPROM), CFE_PSP_INVALID_MEM_RANGE);
    UTF_put_text("CFE_PSP_MemValidateRange(x3000,x50,EEPROM) returned %x; Expected %x\n",
    	CFE_PSP_MemValidateRange(0x3000,0x50,CFE_PSP_MEM_EEPROM), CFE_PSP_INVALID_MEM_ADDR);

    UTF_put_text("\n***CFE_PSP_MemValidateRange Return Code Setting Test ***\n");
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, CFE_PSP_INVALID_POINTER);
    UTF_put_text("CFE_PSP_MemValidateRange Return Code %x\n", CFE_PSP_MemValidateRange(0x1000,0x100,CFE_PSP_MEM_EEPROM));
    UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC);

	maxRangeNum = CFE_PSP_MemRanges();
	UTF_put_text("The maximum number of memory ranges supported is %d\n",maxRangeNum);
	UTF_put_text("\nSet memory range Test\n");
    UTF_put_text("\n***CFE_PSP_MemRangeSet Return Code Setting Test ***\n");
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMRANGESET_PROC, CFE_PSP_INVALID_MEM_ATTR);
    UTF_put_text("CFE_PSP_MemRangeSet Return Code %x\n",
    CFE_PSP_MemRangeSet(maxRangeNum+1,CFE_PSP_MEM_RAM,0x1000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE));
    UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMRANGESET_PROC);

	UTF_put_text("CFE_PSP_MemRangeSet(%d,RAM,%d,%d,BYTE,RW) returned 0x%08x. Expected 0x%08x\n",
			maxRangeNum+1,0x1000,0x100,
			CFE_PSP_MemRangeSet(maxRangeNum+1,CFE_PSP_MEM_RAM,0x1000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE),
			CFE_PSP_INVALID_MEM_RANGE);
	UTF_put_text("CFE_PSP_MemRangeSet(0,%d,%d,%d,BYTE,RW) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MEM_INVALID,0x1000,0x100,
			CFE_PSP_MemRangeSet(0,CFE_PSP_MEM_INVALID,0x1000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE),
			CFE_PSP_INVALID_MEM_TYPE);
	UTF_put_text("CFE_PSP_MemRangeSet(0,RAM,%d,%d,%d,RW) returned 0x%08x. Expected 0x%08x\n",
			0x1000,0x100,3,
			CFE_PSP_MemRangeSet(0,CFE_PSP_MEM_RAM,0x1000,0x100,3,CFE_PSP_MEM_ATTR_READWRITE),
			CFE_PSP_INVALID_MEM_WORDSIZE);
	UTF_put_text("CFE_PSP_MemRangeSet(0,RAM,%d,%d,BYTE,%d) returned 0x%08x. Expected 0x%08x\n",
			0x1000,0x100,CFE_PSP_MEM_ATTR_READWRITE+1,
			CFE_PSP_MemRangeSet(0,CFE_PSP_MEM_RAM,0x1000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE+1),
			CFE_PSP_INVALID_MEM_ATTR);
	UTF_put_text("CFE_PSP_MemRangeSet(0,RAM,%d,%d,BYTE,RW) returned 0x%08x. Expected 0x%08x\n",
			0x1000,0x100,
			CFE_PSP_MemRangeSet(0,CFE_PSP_MEM_RAM,0x1000,0x100,CFE_PSP_MEM_SIZE_BYTE,CFE_PSP_MEM_ATTR_READWRITE),
			CFE_PSP_SUCCESS);
			
	UTF_put_text("\nGet memory range Test\n");
    UTF_put_text("\n***CFE_PSP_MemRangeGet Return Code Setting Test ***\n");
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMRANGEGET_PROC, CFE_PSP_INVALID_POINTER);
    UTF_put_text("CFE_PSP_MemRangeGet Return Code %x\n",
	CFE_PSP_MemRangeGet(maxRangeNum,&memType,&startAddr,&byteSize,&wordSize,&attribs));
    UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_MEMRANGEGET_PROC);

	UTF_put_text("CFE_PSP_MemRangeGet(%d,validPtrs) returned 0x%08x. Expected 0x%08x\n",
			maxRangeNum+1,
			CFE_PSP_MemRangeGet(maxRangeNum+1,&memType,&startAddr,&byteSize,&wordSize,&attribs),
			CFE_PSP_INVALID_MEM_RANGE);
	UTF_put_text("CFE_PSP_MemRangeGet(0,invalidPtr,validPtr,validPtr,validPtr,validPtr) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MemRangeGet(0,0,&startAddr,&byteSize,&wordSize,&attribs),CFE_PSP_INVALID_POINTER);
	UTF_put_text("CFE_PSP_MemRangeGet(0,validPtr,invalidPtr,validPtr,validPtr,validPtr) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MemRangeGet(0,&memType,NULL,&byteSize,&wordSize,&attribs),CFE_PSP_INVALID_POINTER);
	UTF_put_text("CFE_PSP_MemRangeGet(0,validPtr,validPtr,invalidPtr,validPtr,validPtr) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MemRangeGet(0,&memType,&startAddr,0,&wordSize,&attribs),CFE_PSP_INVALID_POINTER);
	UTF_put_text("CFE_PSP_MemRangeGet(0,validPtr,validPtr,validPtr,invalidPtr,validPtr) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MemRangeGet(0,&memType,&startAddr,&byteSize,NULL,&attribs),CFE_PSP_INVALID_POINTER);
	UTF_put_text("CFE_PSP_MemRangeGet(0,validPtr,validPtr,validPtr,validPtr,invalidPtr) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MemRangeGet(0,&memType,&startAddr,&byteSize,&wordSize,0),CFE_PSP_INVALID_POINTER);
	UTF_put_text("CFE_PSP_MemRangeGet(0,validPtrs) returned 0x%08x. Expected 0x%08x\n",
			CFE_PSP_MemRangeGet(0,&memType,&startAddr,&byteSize,&wordSize,&attribs),CFE_PSP_SUCCESS);
	UTF_put_text("Return values are:\n");
	UTF_put_text("    Memory Type   = %d\n",memType);
	UTF_put_text("    Start Address = %d\n",startAddr);
	UTF_put_text("    Range Size    = %d\n",byteSize);
	UTF_put_text("    Word Size     = %d\n",wordSize);
	UTF_put_text("    Attributes    = %d\n",attribs);

	UTF_put_text("\n#\n# Test valid read/write calls\n#\n");
	
	/* Display initial values at 0x1000 */	
	UTF_put_text("Display initial values in MemoryRange1.  Values should be 0.\n");
	if (CFE_PSP_MemRead32(0x1000, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x1000 : 0x%08x\n", DataWord32);

	/* Write new values to 0x1000 */
	UTF_put_text("Write '0x1a2b3c8f' to simulated addr 0x1000\n");
	CFE_PSP_MemWrite32(0x1000,0x1a2b3c8f);
	if (CFE_PSP_MemRead32(0x1000, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x1000 : 0x%08x\n", DataWord32);
	UTF_put_text("Write 0xc1 to valid address 0x2000 using CFE_PSP_PortWrite8\n");
	CFE_PSP_PortWrite8(0x2000,0xc1);
	CFE_PSP_PortRead8(0x2000, &DataWord8);
	UTF_put_text("CFE_PSP_PortRead8 returned 0x%2x\n", DataWord8);

	UTF_put_text("\n#\n# Test valid CFE_PSP_MemCpy, CFE_PSP_MemSet calls\n#\n");

	/* Copy values in memory */
	UTF_put_text("\n->Test CFE_PSP_MemCpy(0x1020,0x1000,32)\n");
	/* Show current value of Dest */
	if (CFE_PSP_MemRead32(0x1020, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x1020 : 0x%08x\n", DataWord32);
	/* Perform CFE_PSP_MemCpy */
	CFE_PSP_MemCpy((void *)0x1020,(const void *)0x1000,32);
	/* Show value of Dest */
	if (CFE_PSP_MemRead32(0x1020, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x1020 : 0x%08x\n", DataWord32);
	
	UTF_put_text("\n->Set 32 bits at 0x1020 to 0\n");
	CFE_PSP_MemSet((void *)0x1020,0x00,32);
	/* Show value of Dest */
	if (CFE_PSP_MemRead32(0x1020, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x1020 : 0x%08x\n", DataWord32);

	UTF_put_text("\n->Test CFE_PSP_MemCpy(0x2000,0x1000,32)\n");
	/* Show current value of Dest */
	if (CFE_PSP_MemRead32(0x2000, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at Dest 0x2000 : 0x%08x\n", DataWord32);
	/* Show current value of Src */
	if (CFE_PSP_MemRead32(0x1000, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at Src 0x1000 : 0x%08x\n", DataWord32);
	/* Perform CFE_PSP_MemCpy */
	CFE_PSP_MemCpy((void *)0x2000,(const void *)0x1000,32);
	/* Show value of Dest */
	if (CFE_PSP_MemRead32(0x2000, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x2000 : 0x%08x\n", DataWord32);

	UTF_put_text("\n->Set 32 bits at 0x2000 to 0xff\n");
	CFE_PSP_MemSet((void *)0x2000,0xff,32);
	/* Show value of Dest */
	if (CFE_PSP_MemRead32(0x2000, &DataWord32) != CFE_PSP_SUCCESS)
		printf("Error status returned from CFE_PSP_MemRead32\n");
	UTF_put_text("32 bits at address 0x2000 : 0x%08x\n", DataWord32);
	
	/*	UTF_put_text("CFE_PSP_MemCpy */
	UTF_put_text("\n#\n# Attempt reads & writes to invalid addresses\n#\n");
	UTF_put_text("->Test CFE_PSP_PortWrite8 with address=0x500\n");
	CFE_PSP_PortWrite8(0x500,0xc1);
	UTF_put_text("->Test CFE_PSP_EepromWrite8 with address=0x07a9\n");
	CFE_PSP_EepromWrite8(0x07a9,0xb1);

	UTF_put_text("\n#\n# Attempt reads & writes on invalid boundaries\n#\n");
	UTF_put_text("#CFE_PSP_PortWrite16 to 0x2001\n");
	CFE_PSP_PortWrite16(0x2001,0xdab1);
	UTF_put_text("#CFE_PSP_PortRead16 to 0x2031\n");
	CFE_PSP_PortRead16(0x2031, &DataWord16);
	UTF_put_text("#CFE_PSP_MemWrite16 to 0x2055\n");
	CFE_PSP_MemWrite16(0x2055,0xdab1);
	UTF_put_text("#CFE_PSP_MemRead16 to 0x2099\n");
	CFE_PSP_MemRead16(0x2099, &DataWord16);
	UTF_put_text("#CFE_PSP_PortWrite32 to 0x2006\n");
	CFE_PSP_PortWrite32(0x2006,0x76abcd84);
	
	/* Test the new Symbol Table APIs */
	UTF_SymbolTable_t myEntry, myEntry2;
	UTF_put_text("\nSetup the Symbol Table\n");
	UTF_InitSymbolTable();
	myEntry2.symbolName[0] = '\0';
	UTF_put_text("Length of entry 2 name = %d\n",strlen(myEntry2.symbolName));
	strcpy(myEntry.symbolName,"testSymbolVar");
	myEntry.symbolAddr = 0x2048;
	UTF_put_text("Return from UTF_SetSymbolTableEntry(myEntry) = %x\n",
		UTF_SetSymbolTableEntry(myEntry));
	UTF_put_text("Return from UTF_SetSymbolTableEntry(myEntry2) = %x\n",
		UTF_SetSymbolTableEntry(myEntry2));
	strcpy(myEntry2.symbolName,"thisIsSymbolThatIsTooLong");
	UTF_put_text("Return from UTF_SetSymbolTableEntry with a symbol name length > 20 = %x\n",
		UTF_SetSymbolTableEntry(myEntry2));
	strcpy(myEntry2.symbolName,"thisIsSymbolThatIs20");
	UTF_put_text("Return from UTF_SetSymbolTableEntry with a symbol name length = 20 = %x\n",UTF_SetSymbolTableEntry(myEntry2));
	strcpy(myEntry2.symbolName,"thisIsSymbolof19cOK");
	myEntry2.symbolAddr = 0x4096;
	UTF_put_text("Return from UTF_SetSymbolTableEntry with a symbol name length = 19 = %x\n",UTF_SetSymbolTableEntry(myEntry2));

	uint32 ptrToSize;
	UTF_put_text("\nSymbol Table API Tests\n");
    UTF_put_text("OS_SymbolLookup(%s) returned %x\n","testSymbolVar",
    	OS_SymbolLookup(&ptrToSize,"testSymbolVar"));
	UTF_put_text("Pointer returned contains %x\n",ptrToSize);
    UTF_put_text("OS_SymbolTableDump('symTab.dmp',1024) returned %x\n",
    	OS_SymbolTableDump("symTab.dmp", 1024));
    
	/*	Need to test adding more than UTF_MAX_SYMBOLS */
	int i;
	for (i=0; i<UTF_MAX_SYMBOLS; i++)
	{
		sprintf(myEntry2.symbolName,"symbol%d",i+3);
		UTF_put_text("Return from UTF_SetSymbolTableEntry #%d = %x\n",i+3,
			UTF_SetSymbolTableEntry(myEntry2));
	}

    UTF_put_text("Symbol Table Dump #2\n");
    UTF_DumpSymbolTable();
	
    UTF_put_text("\n***Function Hook Tests***\n");
    UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE8_HOOK, (void *)&CFE_PSP_EepromWrite8Hook);
    UTF_put_text("EepromWrite8 Hook Return Code %x\n", CFE_PSP_EepromWrite8(0x07a9,0xb1));  
    UTF_put_text("EepromWrite8 Hook Return Code %x\n", CFE_PSP_EepromWrite8(0x07a9,0xb1));  
    UTF_put_text("EepromWrite8 Hook Return Code %x\n", CFE_PSP_EepromWrite8(0x07a9,0xb1));  
    UTF_put_text("EepromWrite8 Hook Return Code %x\n", CFE_PSP_EepromWrite8(0x07a9,0xb1));  

    UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE16_HOOK, (void *)&CFE_PSP_EepromWrite16Hook);
    UTF_put_text("EepromWrite16 Hook Return Code %x\n", CFE_PSP_EepromWrite16(0x2055,0xdab1));
    UTF_put_text("EepromWrite16 Hook Return Code %x\n", CFE_PSP_EepromWrite16(0x2055,0xdab1));
    UTF_put_text("EepromWrite16 Hook Return Code %x\n", CFE_PSP_EepromWrite16(0x2055,0xdab1));
    UTF_put_text("EepromWrite16 Hook Return Code %x\n", CFE_PSP_EepromWrite16(0x2055,0xdab1));

    UTF_PSP_set_function_hook(CFE_PSP_EEPROMWRITE32_HOOK, (void *)&CFE_PSP_EepromWrite32Hook);
    UTF_put_text("EepromWrite32 Hook Return Code %x\n", CFE_PSP_EepromWrite32(0x2006,0x76abcd84));  
    UTF_put_text("EepromWrite32 Hook Return Code %x\n", CFE_PSP_EepromWrite32(0x2006,0x76abcd84));  
    UTF_put_text("EepromWrite32 Hook Return Code %x\n", CFE_PSP_EepromWrite32(0x2006,0x76abcd84));  
    UTF_put_text("EepromWrite32 Hook Return Code %x\n", CFE_PSP_EepromWrite32(0x2006,0x76abcd84));  

	return 0;
}




