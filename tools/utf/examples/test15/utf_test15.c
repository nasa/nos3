/* File: utf_test15.c
 *
 * This test makes ES api calls.
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
#include <stdlib.h>  /* for malloc */

#include "cfe.h"
#include "ccsds.h"
#include "cfe_sb.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osfilesys.h"
#include "utf_osapi.h"
#include "utf_cfe_es.h"

/* Function Hook Test routines */
int32 CFE_ES_PoolCreateHook(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_ES_BAD_ARGUMENT);
}

int32 CFE_ES_GetPoolBufHook(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_ES_ERR_MEM_BLOCK_SIZE);
}

int32 CFE_ES_PutPoolBufHook(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_ES_ERR_MEM_HANDLE);
}

int main(void)
{
	
	uint8 *MemPtr1;
	uint8 *MemPtr2;
	uint32 HandlePtr1, HandlePtr2, HandlePtr3;
	uint8 *BufPtr1, *BufPtr2, *BufPtr3;
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test15.ActualOutput");
    UTF_init_sim_time(1.5);

    /* CFE_ES_PoolCreate Test */
    UTF_put_text("\n***Test CFE_ES_PoolCreate API***\n");
	MemPtr1 = (uint8*) malloc(1088);
	MemPtr2 = (uint8*) malloc(100);

	CFE_ES_RegisterApp();
	
    UTF_put_text("\n***CFE_ES_PoolCreate Return Code Setting Test ***\n");
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_POOLCREATE_PROC, CFE_ES_BAD_ARGUMENT);
    UTF_put_text("CFE_ES_PoolCreate Return Code %x\n", CFE_ES_PoolCreate(&HandlePtr3,MemPtr1,82));
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_POOLCREATE_PROC);
    UTF_put_text("CFE_ES_PoolCreate Return Code %x\n", CFE_ES_PoolCreate(&HandlePtr1,MemPtr1,82));

    UTF_put_text("\nTest Case #1: Call CFE_ES_PoolCreate called with size not multiple of 4B.\n"); 
    UTF_put_text("CFE_ES_PoolCreate returned %x. Expected error return %x.\n", 
    					CFE_ES_PoolCreate(&HandlePtr3, MemPtr1, 82),CFE_ES_BAD_ARGUMENT);

    UTF_put_text("\nTest Case #2: Call CFE_ES_PoolCreate called with HandlePtr1 and valid size 1088.\n"); 
    UTF_put_text("CFE_ES_PoolCreate returned %x. Expected %x.\n", 
    					CFE_ES_PoolCreate(&HandlePtr1, MemPtr1, 1088),CFE_SUCCESS);

    UTF_put_text("\nTest Case #3: Call CFE_ES_PoolCreate called with size (100) too small to be useful.\n"); 
    UTF_put_text("CFE_ES_PoolCreate returned %x. Expected %x.  Expect UTF warning.\n", 
    					CFE_ES_PoolCreate(&HandlePtr2, MemPtr2, 100),CFE_ES_BAD_ARGUMENT);

    UTF_put_text("\nHandlePtr1 (hex) = %x; HandlePtr2 (hex) = %x; HandlePtr3 (hex) = %x.\n",HandlePtr1, HandlePtr2, HandlePtr3); 

    /* CFE_ES_GetPoolBuf Test */
    UTF_put_text("\n***Test CFE_ES_GetPoolBuf API***\n");
	HandlePtr2 += 32; 

    UTF_put_text("\n***CFE_ES_GetPoolBuf Return Code Setting Test ***\n");
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_GETPOOLBUF_PROC, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UTF_put_text("CFE_ES_GetPoolBuf Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr2,24));
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_GETPOOLBUF_PROC);
    UTF_put_text("CFE_ES_GetPoolBuf Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr2,24));

    UTF_put_text("\nTest Case #1: Call CFE_ES_GetPoolBuf with invalid HandlePtr argument\n"); 
    UTF_put_text("CFE_ES_GetPoolBuf returned %x. Expected %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr2,24),CFE_ES_ERR_MEM_HANDLE);
    UTF_put_text("\nTest Case #2: Call CFE_ES_GetPoolBuf with size argument which exceeds max allowed\n"); 
    UTF_put_text("CFE_ES_GetPoolBuf returned %x. Expected %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr1,40000),CFE_ES_ERR_MEM_BLOCK_SIZE);
    UTF_put_text("\nTest Case #3: Call CFE_ES_GetPoolBuf with HandlePtr1 and valid size=32.\n"); 
    UTF_put_text("CFE_ES_GetPoolBuf returned %d. Expected %d\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr1,HandlePtr1,32),32);
    UTF_put_text("\nTest Case #4: Call CFE_ES_GetPoolBuf with HandlePtr1 and valid size=20.\n"); 
    UTF_put_text("CFE_ES_GetPoolBuf returned %d. Expected %d\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr2,HandlePtr1,20),32);
    UTF_put_text("\nTest Case #5: Call CFE_ES_GetPoolBuf with HandlePtr1 and size > remaining pool resources (size=2000).\n"); 
    UTF_put_text("CFE_ES_GetPoolBuf returned %x. Expected %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr1,2000),CFE_ES_ERR_MEM_BLOCK_SIZE);
    
    UTF_put_text("\nBufPtr1 (hex) = %x; BufPtr2 (hex) = %x; BufPtr3 (hex) = %x.\n",BufPtr1, BufPtr2, BufPtr3); 

    /* CFE_ES_PutPoolBuf */
    UTF_put_text("\n***Test CFE_ES_PutPoolBuf API***\n");

    UTF_put_text("\n***CFE_ES_PutPoolBuf Return Code Setting Test ***\n");
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_PUTPOOLBUF_PROC, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UTF_put_text("CFE_ES_PutPoolBuf Return Code %x\n", CFE_ES_PutPoolBuf(HandlePtr1,(uint32 *)BufPtr3));
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_PUTPOOLBUF_PROC);
    UTF_put_text("CFE_ES_PutPoolBuf Return Code %x\n", CFE_ES_PutPoolBuf(HandlePtr1,(uint32 *)BufPtr3));

    UTF_put_text("\nTest Case #1: Call CFE_ES_PutPoolBuf with HandlePtr1 and BufPtr1\n");
    int32 retStatus = CFE_ES_PutPoolBuf(HandlePtr1,(uint32 *)BufPtr1);
    UTF_put_text("CFE_ES_PutPoolBuf returned %d. Expected %d.\n", retStatus,32);

    UTF_put_text("\nTest Case #2: Call CFE_ES_PutPoolBuf with HandlePtr2 (invalid)\n"); 
    UTF_put_text("CFE_ES_PutPoolBuf returned %x. Expected %x.\n", CFE_ES_PutPoolBuf(HandlePtr2,(uint32 *)BufPtr2),CFE_ES_ERR_MEM_HANDLE);

    UTF_put_text("\nTest Case #3: Call CFE_ES_GetPoolBuf following buffer release with HandlePtr1 and valid size=32.\n"); 
    UTF_put_text("CFE_ES_GetPoolBuf returned %d. Expected %d\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr1,HandlePtr1,32),32);

    UTF_put_text("\n***Function Hook Tests***\n");
    UTF_ES_set_function_hook(CFE_ES_POOLCREATE_HOOK, (void *)&CFE_ES_PoolCreateHook);
    UTF_put_text("PoolCreate Hook Return Code %x\n", CFE_ES_PoolCreate(&HandlePtr2,MemPtr2,100));  
    UTF_put_text("PoolCreate Hook Return Code %x\n", CFE_ES_PoolCreate(&HandlePtr2,MemPtr2,100));  
    UTF_put_text("PoolCreate Hook Return Code %x\n", CFE_ES_PoolCreate(&HandlePtr2,MemPtr2,100));  
    UTF_put_text("PoolCreate Hook Return Code %x\n", CFE_ES_PoolCreate(&HandlePtr2,MemPtr2,100));  

    UTF_ES_set_function_hook(CFE_ES_GETPOOLBUF_HOOK, (void *)&CFE_ES_GetPoolBufHook);
    UTF_put_text("GetPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr1,2000));  
    UTF_put_text("GetPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr1,2000));  
    UTF_put_text("GetPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr1,2000));  
    UTF_put_text("GetPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr3,HandlePtr1,2000));  

    UTF_ES_set_function_hook(CFE_ES_PUTPOOLBUF_HOOK, (void *)&CFE_ES_PutPoolBufHook);
    UTF_put_text("PutPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr1,HandlePtr1,32));  
    UTF_put_text("PutPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr1,HandlePtr1,32));  
    UTF_put_text("PutPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr1,HandlePtr1,32));  
    UTF_put_text("PutPoolBuf Hook Return Code %x\n", CFE_ES_GetPoolBuf((uint32 **)&BufPtr1,HandlePtr1,32));  

 	exit(0);
}
