/******************************************************************************
** File: cfe_sb_init.c
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASAs Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**
**
** Purpose:
**      This file contains the source code for the SB Initialization.
**
** Author:   R.McGraw/SSI
**
** $Log: cfe_sb_init.c  $
** Revision 1.12 2012/01/13 12:15:12GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.11 2010/11/04 16:41:25EDT aschoeni 
** Added optional sender information storage
** Revision 1.10 2010/10/20 12:36:52EDT jmdagost 
** Updated memory pool creation to select no semaphore control.
** Revision 1.9 2009/07/24 18:24:57EDT aschoeni 
** Added Zero Copy Mode
** Revision 1.8 2009/07/17 17:57:56EDT aschoeni 
** Updated MsgMap (and associated variables) from a uint16 to an CFE_SB_MsgId_t
** Revision 1.7 2009/06/10 09:18:02EDT acudmore 
** Updated OS_Mem* and OS_BSP* to CFE_PSP_*
** Revision 1.6 2009/03/30 10:48:48EDT rmcgraw 
** DCR5801:2Added missing \n in mutex-creation-failed sys log msg
** Revision 1.5 2009/02/11 14:21:43EST rmcgraw 
** DCR6269:1 Changes for new SB defined mem pool sizes
** Revision 1.4 2009/02/06 11:29:05EST rmcgraw 
** DCR5801:2 General Cleanup
** Revision 1.3 2009/02/03 11:06:59EST rmcgraw 
** DCR5801:2 Changed destination desciptors from array based to linked list
** Revision 1.2 2009/01/30 10:35:39EST rmcgraw 
** DCR5801:1 Initialize destinations to zero
** Revision 1.1 2008/04/17 08:05:31EDT ruperera 
** Initial revision
** Member added to cfe project on tlserver3
** Revision 1.27 2007/09/13 09:38:20EDT rjmcgraw 
** Initialized new StopRecurseFlags array
** Revision 1.26 2007/07/12 17:03:49EDT rjmcgraw 
** DCR4680:1 Removed Event log mutes creation
** Revision 1.25 2007/06/04 13:24:07EDT rjmcgraw 
** DCR4473:1 Moved SB stat initialization from task init to early init
** Revision 1.24 2007/05/18 16:11:17EDT rjmcgraw 
** DCR3052:9 Moved initialization of subscription report state from app init to 
** early init
** Revision 1.23 2007/04/26 09:25:31EDT rjmcgraw 
** Changed EarlyInit to report and return errors
** Revision 1.22 2006/10/16 14:30:54EDT rjmcgraw 
** Minor changes to comply with MISRA standard
** Revision 1.21 2006/09/01 11:08:11EDT kkaudra 
** IV&V:Removed cfe_evs.h
** Revision 1.20 2006/07/26 07:51:53EDT rjmcgraw 
** Changed status format from dec to hex
**
******************************************************************************/

/*
**  Include Files
*/

#include "cfe_sb_priv.h"
#include "cfe_sb.h"
#include "osapi.h"
#include "cfe_msgids.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_error.h"
#include "cfe_sb_events.h"

/*
**  External Declarations
*/

uint32 CFE_SB_MemPoolDefSize[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES] = 
{
    CFE_SB_MAX_BLOCK_SIZE,
    CFE_SB_MEM_BLOCK_SIZE_16,
    CFE_SB_MEM_BLOCK_SIZE_15,
    CFE_SB_MEM_BLOCK_SIZE_14,
    CFE_SB_MEM_BLOCK_SIZE_13,
    CFE_SB_MEM_BLOCK_SIZE_12,
    CFE_SB_MEM_BLOCK_SIZE_11,
    CFE_SB_MEM_BLOCK_SIZE_10,
    CFE_SB_MEM_BLOCK_SIZE_09,
    CFE_SB_MEM_BLOCK_SIZE_08,
    CFE_SB_MEM_BLOCK_SIZE_07,
    CFE_SB_MEM_BLOCK_SIZE_06,
    CFE_SB_MEM_BLOCK_SIZE_05,
    CFE_SB_MEM_BLOCK_SIZE_04,
    CFE_SB_MEM_BLOCK_SIZE_03,
    CFE_SB_MEM_BLOCK_SIZE_02,
    CFE_SB_MEM_BLOCK_SIZE_01
};


/******************************************************************************
**  Function:  CFE_SB_EarlyInit()
**
**  Purpose:
**    Initialize the Software Bus routing tables.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    CFE_SUCCESS
*/
int32 CFE_SB_EarlyInit (void) {

    int32 Stat = CFE_SUCCESS;

    CFE_SB_Default_Qos.Priority    = CFE_SB_QOS_LOW_PRIORITY;
    CFE_SB_Default_Qos.Reliability = CFE_SB_QOS_LOW_RELIABILITY;

    Stat = OS_MutSemCreate(&CFE_SB.SharedDataMutexId, "CFE_SB_DataMutex", 0);
    if(Stat != OS_SUCCESS){
      CFE_ES_WriteToSysLog("SB shared data mutex creation failed! RC=0x%08x\n",(unsigned int)Stat);
      return Stat;
    }/* end if */
    
    /* Initialize the state of susbcription reporting */
    CFE_SB.SubscriptionReporting = CFE_SB_DISABLE;

    /* Initialize the state of sender reporting */
    CFE_SB.SenderReporting = CFE_SB_DEFAULT_REPORT_SENDER;

     /* Initialize memory partition. */
    Stat = CFE_SB_InitBuffers();
    if(Stat != CFE_SUCCESS){
      /* error reported in CFE_SB_InitBuffers */
      return Stat;
    }/* end if */

    /* Initialize the pipe table. */
    CFE_SB_InitPipeTbl();

    /* Initialize the routing index look up table */
    CFE_SB_InitMsgMap();

    /* Initialize the routing table. */
    CFE_SB_InitRoutingTbl();
    
    /* Initialize the SB Statistics Pkt */
    CFE_SB_InitMsg(&CFE_SB.StatTlmMsg.Hdr.Pri,
                   CFE_SB_STATS_TLM_MID,
                   sizeof(CFE_SB_StatMsg_t),
                   TRUE);    

   CFE_SB.ZeroCopyTail = NULL;

    return Stat;

}/* end CFE_SB_EarlyInit */



/******************************************************************************
**  Function:  CFE_SB_InitBuffers()
**
**  Purpose:
**    Initialize the Software Bus Buffer Pool.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
int32  CFE_SB_InitBuffers(void) {

    int32 Stat = 0;

    Stat = CFE_ES_PoolCreateEx(&CFE_SB.Mem.PoolHdl, 
                                CFE_SB.Mem.Partition, 
                                CFE_SB_BUF_MEMORY_BYTES, 
                                CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, 
                                &CFE_SB_MemPoolDefSize[0],
                                CFE_ES_NO_MUTEX);
    
    if(Stat != CFE_SUCCESS){
        CFE_ES_WriteToSysLog("PoolCreate failed for SB Buffers, gave adr 0x%lx,size %d,stat=0x%x\n",
              (unsigned long)CFE_SB.Mem.Partition,CFE_SB_BUF_MEMORY_BYTES,(unsigned int)Stat);
        return Stat;
    }
    
    return CFE_SUCCESS;
    
}/* end CFE_SB_InitBuffers */


/******************************************************************************
**  Function:  CFE_SB_InitPipeTbl()
**
**  Purpose:
**    Initialize the Software Bus Pipe Table.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
void CFE_SB_InitPipeTbl(void){

    uint8  i;

    for(i=0;i<CFE_SB_MAX_PIPES;i++){
        CFE_SB.PipeTbl[i].InUse         = CFE_SB_NOT_IN_USE;
        CFE_SB.PipeTbl[i].SysQueueId    = CFE_SB_UNUSED_QUEUE;
        CFE_SB.PipeTbl[i].PipeId        = CFE_SB_INVALID_PIPE;
        CFE_SB.PipeTbl[i].CurrentBuff   = NULL;
        CFE_PSP_MemSet(&CFE_SB.PipeTbl[i].PipeName[0],0,OS_MAX_API_NAME);
    }/* end for */

}/* end CFE_SB_InitPipeTbl */



/******************************************************************************
**  Function:  CFE_SB_InitMsgMap()
**
**  Purpose:
**    Initialize the Software Bus Message Map.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
void CFE_SB_InitMsgMap(void){

#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_MsgId_t   appId;

    for (appId=0; appId < (CFE_SB_HIGHEST_VALID_MSGID + 1); appId++) {
        CFE_SB.MsgMap[appId] = CFE_SB_AVAILABLE;
    }

#endif

}/* end CFE_SB_InitMsgMap */



/******************************************************************************
**  Function:  CFE_SB_InitRoutingTbl()
**
**  Purpose:
**    Initialize the Software Bus Routing Table.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
void CFE_SB_InitRoutingTbl(void){

    uint32  i;

    /* Initialize routing table */
    for(i=0;i<CFE_SB_MAX_MSG_IDS;i++){

        CFE_SB.RoutingTbl[i].MsgId = CFE_SB_INVALID_MSG_ID;
        CFE_SB.RoutingTbl[i].SeqCnt = 0;
        CFE_SB.RoutingTbl[i].Destinations = 0;
        CFE_SB.RoutingTbl[i].ListHeadPtr = NULL;
              
    }/* end for */

}/* end CFE_SB_InitRoutingTbl */

/*****************************************************************************/
