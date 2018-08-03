/************************************************************************
** File:
**   $Id: hk_utils.c 1.7.1.1 2015/03/04 15:02:50EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**  The CFS Housekeeping (HK) Application file containing the functions
**  used combine the input messages into output messages.
**
** Notes:
**
** $Log: hk_utils.c  $
** Revision 1.7.1.1 2015/03/04 15:02:50EST sstrege 
** Added copyright information
** Revision 1.7 2012/08/15 18:33:03EDT aschoeni 
** Added ability to discard incomplete combo packets
** Revision 1.6 2012/03/23 17:29:26EDT lwalling 
** Limit error events to one per HK input packet
** Revision 1.5 2009/12/03 15:34:55EST jmdagost 
** Uncommented proper code to check the dump pending status of a table.  Commented out the old code (instead of deleting it) for reference.
** Revision 1.4 2009/06/12 14:16:58EDT rmcgraw 
** DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
** Revision 1.3 2008/09/11 10:19:45EDT rjmcgraw 
** DCR4040:1 Removed tabs and removed #include osapi-hw-core.h
** Revision 1.2 2008/06/19 13:24:49EDT rjmcgraw 
** DCR3052:1 Changed Table check logic to call getstatus before processing new copy table
** Revision 1.1 2008/04/09 16:42:24EDT rjmcgraw 
** Initial revision
** Member added to CFS project
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "hk_utils.h"
#include "hk_app.h"
#include "hk_events.h"
#include <string.h>
                             

/*************************************************************************
** Function definitions
**************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK process incoming housekeeping data                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_ProcessIncomingHkData (CFE_SB_MsgPtr_t MessagePtr)
{
    hk_copy_table_entry_t         * StartOfCopyTable;
    hk_copy_table_entry_t         * CpyTblEntry;
    hk_runtime_tbl_entry_t        * StartOfRtTable;
    hk_runtime_tbl_entry_t        * RtTblEntry;
    uint16                          Loop;
    CFE_SB_MsgId_t                  MessageID;
    uint8                         * DestPtr;
    uint8                         * SrcPtr;
    int32                           MessageLength;
    int32                           MessageErrors;
    int32                           LastByteAccessed;


    StartOfCopyTable = (hk_copy_table_entry_t *)  HK_AppData.CopyTablePtr;
    StartOfRtTable   = (hk_runtime_tbl_entry_t *) HK_AppData.RuntimeTablePtr;
    MessageID        = CFE_SB_GetMsgId (MessagePtr);
    MessageErrors    = 0;
    
    /* Spin thru the entire table looking for matches */
    for (Loop=0; Loop < HK_COPY_TABLE_ENTRIES; Loop++)
    {
        CpyTblEntry = & StartOfCopyTable [Loop];
        RtTblEntry  = & StartOfRtTable [Loop];
        
        /* Does the inputMID for this table entry match what we're looking for */       
        if (MessageID == CpyTblEntry->InputMid)
        {
            /* Ensure that we don't reference past the end of the input packet */
            MessageLength    = CFE_SB_GetTotalMsgLength(MessagePtr);
            LastByteAccessed = CpyTblEntry->InputOffset + CpyTblEntry->NumBytes;
            if (MessageLength >= LastByteAccessed)
            {
                /* We have a match.  Build the Source and Destination addresses
                   and move the data */
                DestPtr = ( (uint8 *) RtTblEntry->OutputPktAddr) + CpyTblEntry->OutputOffset;
                SrcPtr  = ( (uint8 *) MessagePtr) + CpyTblEntry->InputOffset;

                CFE_PSP_MemCpy (DestPtr, SrcPtr, CpyTblEntry->NumBytes);
                
                /* Set the data present field to indicate the data is there */
                RtTblEntry->DataPresent = HK_DATA_PRESENT;

            }
            else
            {
                /* Error: copy data is past the end of the input packet */
                MessageErrors++;
            }
        }
    }

    /* Send, at most, one error event per input packet */
    if (MessageErrors != 0)
    {
        CFE_EVS_SendEvent (HK_ACCESSING_PAST_PACKET_END_EID, CFE_EVS_ERROR,
                           "HK table definition exceeds packet length. MID:0x%04X, Length:%d, Count:%d",
                           MessageID, MessageLength, MessageErrors);
    }

    return;

}   /* end HK_ProcessIncomingHkData */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK validate the copy table contents                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HK_ValidateHkCopyTable (void * TblPtr)
{
    return HK_SUCCESS;

}   /* end HK_ValidateHkCopyTable */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK process new copy table                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_ProcessNewCopyTable (hk_copy_table_entry_t * CpyTblPtr, 
                             hk_runtime_tbl_entry_t * RtTblPtr)
{
    hk_copy_table_entry_t         * StartOfCopyTable;
    hk_copy_table_entry_t         * OuterCpyEntry;
    hk_copy_table_entry_t         * InnerDefEntry;
    hk_runtime_tbl_entry_t        * StartOfRtTable;
    hk_runtime_tbl_entry_t        * OuterRtEntry;
    hk_runtime_tbl_entry_t        * InnerRtEntry;
    int32                           Loop1;
    int32                           Loop2;
    CFE_SB_MsgId_t                  MidOfThisPacket;
    int32                           SizeOfThisPacket;
    int32                           FurthestByteFromThisEntry;
    CFE_SB_MsgPtr_t                 NewPacketAddr;
    int32                           Result = CFE_SUCCESS;
     
    StartOfCopyTable = CpyTblPtr;
    StartOfRtTable  = RtTblPtr;
    
    /* Loop thru the RunTime table initializing the fields */
    for (Loop1 = 0; Loop1 < HK_COPY_TABLE_ENTRIES; Loop1++)
    {
        OuterRtEntry  = & StartOfRtTable [Loop1];

        OuterRtEntry->OutputPktAddr      = NULL;
        OuterRtEntry->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
        OuterRtEntry->DataPresent        = HK_DATA_NOT_PRESENT;
        
    }

    /* Loop thru the table looking for all of the SB packets that need to be built */
    for (Loop1 = 0; Loop1 < HK_COPY_TABLE_ENTRIES; Loop1++)
    {
        OuterCpyEntry = & StartOfCopyTable [Loop1];
        OuterRtEntry  = & StartOfRtTable [Loop1];

        /* If the both MIDs exists but the Packet Address has yet to be assigned, 
           we need to build an SB packet, so compute the size */
        if ( (OuterCpyEntry->OutputMid     != HK_UNDEFINED_ENTRY) && 
             (OuterCpyEntry->InputMid      != HK_UNDEFINED_ENTRY) &&
             (OuterRtEntry->OutputPktAddr  == NULL) )
        {
            /* We have a table entry that needs a SB message to be built */        
            MidOfThisPacket  = OuterCpyEntry->OutputMid;
            SizeOfThisPacket = 0;

            /* Spin thru entire table looking for duplicate OutputMid's.  This will let 
               us find the byte offset furthest from the beginning of the packet */
            for (Loop2=0; Loop2 < HK_COPY_TABLE_ENTRIES; Loop2++)
            {
                InnerDefEntry = & StartOfCopyTable [Loop2];
                
                /* If this entry's MID matches the one we're looking for */
                if (InnerDefEntry->OutputMid == MidOfThisPacket)
                {
                    /* The byte furthest away from the section described by this entry */
                    FurthestByteFromThisEntry = InnerDefEntry->OutputOffset +
                                                InnerDefEntry->NumBytes;

                    /* Save the byte offset of the byte furthest from the packet start */
                    if (FurthestByteFromThisEntry > SizeOfThisPacket)
                    {
                        SizeOfThisPacket = FurthestByteFromThisEntry;
                                               
                    }
                }
            }

            /* Build the packet with the size computed above */
            NewPacketAddr = NULL;
            if (SizeOfThisPacket > 0)
            {
                Result = CFE_ES_GetPoolBuf ((uint32 **) & NewPacketAddr,
                                            HK_AppData.MemPoolHandle,
                                            SizeOfThisPacket);
                
                if (Result >= CFE_SUCCESS)
                {
                   /* Spin thru entire table (again) looking for duplicate OutputMid's.  
                       This will let us assign the packet created above to all 
                       of the table entries that need to use it */
                    for (Loop2=0; Loop2 < HK_COPY_TABLE_ENTRIES; Loop2++)
                    {
                        InnerDefEntry = & StartOfCopyTable [Loop2];
                        InnerRtEntry  = & StartOfRtTable [Loop2];
                
                        /* If this entry's MID matches the one we're looking for */
                        if (InnerDefEntry->OutputMid == MidOfThisPacket)
                        {
                            InnerRtEntry->OutputPktAddr = NewPacketAddr;
                        }
                    }

                    /* Init the SB Packet only once regardless of how many times its in the table */
                    CFE_SB_InitMsg (NewPacketAddr, MidOfThisPacket, SizeOfThisPacket, TRUE);                    
                    
                }
                else
                {
                    CFE_EVS_SendEvent (HK_MEM_POOL_MALLOC_FAILED_EID, CFE_EVS_ERROR,
                                       "HK Processing New Table: ES_GetPoolBuf for size %d returned 0x%04X",
                                       SizeOfThisPacket, Result);
                }
            }
        }

        /* If HK needs to subscribe to this Input packet... */
        if ( (OuterRtEntry->InputMidSubscribed == HK_INPUTMID_NOT_SUBSCRIBED) &&
             (OuterCpyEntry->InputMid          != HK_UNDEFINED_ENTRY) )
        {
            Result = CFE_SB_Subscribe (OuterCpyEntry->InputMid, HK_AppData.CmdPipe);
            
            if (Result == CFE_SUCCESS)
            {
                /* Spin thru entire table (again) looking for duplicate InputMid's.  
                   This will let us mark each duplicate as already having been subscribed */
                for (Loop2=0; Loop2 < HK_COPY_TABLE_ENTRIES; Loop2++)
                {
                    InnerDefEntry = & StartOfCopyTable [Loop2];
                    InnerRtEntry  = & StartOfRtTable [Loop2];
            
                    /* If this entry's MID matches the one we're looking for */
                    if (OuterCpyEntry->InputMid == InnerDefEntry->InputMid)
                    {
                        InnerRtEntry->InputMidSubscribed = HK_INPUTMID_SUBSCRIBED;
                    }
                }
            }
            else
            {
                CFE_EVS_SendEvent (HK_CANT_SUBSCRIBE_TO_SB_PKT_EID, CFE_EVS_ERROR,
                                   "HK Processing New Table:SB_Subscribe for Mid 0x%04X returned 0x%04X",
                                   OuterCpyEntry->InputMid, Result);
            }
        }
    }

}   /* end HK_ProcessNewCopyTable */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK Tear down old copy table                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_TearDownOldCopyTable (hk_copy_table_entry_t * CpyTblPtr, 
                              hk_runtime_tbl_entry_t * RtTblPtr)
{
    hk_copy_table_entry_t             * StartOfCopyTable;
    hk_copy_table_entry_t             * OuterCpyEntry;
    hk_copy_table_entry_t             * InnerDefEntry;
    hk_runtime_tbl_entry_t            * StartOfRtTable;
    hk_runtime_tbl_entry_t            * OuterRtEntry;
    hk_runtime_tbl_entry_t            * InnerRtEntry;
    int32                               Loop1;
    int32                               Loop2;
    CFE_SB_MsgId_t                      MidOfThisPacket;
    void                              * OutputPktAddr;
    void                              * SavedPktAddr;
    int32                               Result = CFE_SUCCESS;

    StartOfCopyTable = CpyTblPtr;
    StartOfRtTable  = RtTblPtr;
   
    /* Loop thru the table looking for all of the SB packets that need to be freed */
    for (Loop1 = 0; Loop1 < HK_COPY_TABLE_ENTRIES; Loop1++)
    {
        OuterCpyEntry = & StartOfCopyTable [Loop1];
        OuterRtEntry  = & StartOfRtTable  [Loop1];

        /* If a Packet Address has been assigned, it needs to get deleted */
        if (OuterRtEntry->OutputPktAddr != NULL)
        {
            OutputPktAddr   = OuterRtEntry->OutputPktAddr;
            MidOfThisPacket = OuterCpyEntry->OutputMid;

            SavedPktAddr = OutputPktAddr;
            Result = CFE_ES_PutPoolBuf (HK_AppData.MemPoolHandle, (uint32 *) OutputPktAddr);
            if (Result >= CFE_SUCCESS)
            {               
                /* Spin thru the entire table looking for entries that used the same SB packets */
                for (Loop2=0; Loop2 < HK_COPY_TABLE_ENTRIES; Loop2++)
                {
                    InnerDefEntry = & StartOfCopyTable [Loop2];
                    InnerRtEntry  = & StartOfRtTable [Loop2];
                    
                    if ( (InnerDefEntry->OutputMid    == MidOfThisPacket) &&
                         (InnerRtEntry->OutputPktAddr == SavedPktAddr) )
                    {
                        /* NULL out the table entry whose packet was freed above */
                        InnerRtEntry->OutputPktAddr = (CFE_SB_MsgPtr_t) NULL;
                    }
                }
            }
            else
            {
                CFE_EVS_SendEvent (HK_MEM_POOL_FREE_FAILED_EID, CFE_EVS_ERROR,
                                   "HK TearDown: ES_putPoolBuf Err pkt:0x%08X ret 0x%04X, hdl 0x%08x",
                                   SavedPktAddr, Result,HK_AppData.MemPoolHandle);
            }
        }

        /* If the InputMid for this Table Entry has been subscribed, it needs to
           get Unsubscribed as do any other identical InputMids throughout the table.
           We don't have to worry about leaving any Mid's subscribed since the entire table
           is getting clobbered. */
        if (OuterRtEntry->InputMidSubscribed == HK_INPUTMID_SUBSCRIBED)
        {
            CFE_SB_Unsubscribe (OuterCpyEntry->InputMid, HK_AppData.CmdPipe);
                        
            /* Spin thru the entire table looking for entries that used the same SB packets */
            for (Loop2=0; Loop2 < HK_COPY_TABLE_ENTRIES; Loop2++)
            {
                InnerDefEntry = & StartOfCopyTable [Loop2];
                InnerRtEntry  = & StartOfRtTable [Loop2];
                
                if (InnerDefEntry->InputMid == OuterCpyEntry->InputMid)
                {
                    InnerRtEntry->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
                }
            }
        }

    }

}   /* end HK_TearDownOldCopyTable */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK Send combined output message                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_SendCombinedHkPacket (CFE_SB_MsgId_t WhichMidToSend)
{
    boolean                         PacketFound = FALSE;
    hk_copy_table_entry_t         * StartOfCopyTable;
    hk_copy_table_entry_t         * CpyTblEntry;
    hk_runtime_tbl_entry_t        * StartOfRtTable;
    hk_runtime_tbl_entry_t        * RtTblEntry;
    int32                           Loop;
    CFE_SB_MsgId_t                  ThisEntrysOutMid;
    CFE_SB_MsgId_t                  InputMidMissing;

    StartOfCopyTable = (hk_copy_table_entry_t *) HK_AppData.CopyTablePtr;
    StartOfRtTable  = (hk_runtime_tbl_entry_t *)  HK_AppData.RuntimeTablePtr;

    /* Look thru each item in this Table, but only send this packet once, at most */
    for (Loop = 0; ( (Loop < HK_COPY_TABLE_ENTRIES) && (PacketFound == FALSE) ); Loop++)
    {
        CpyTblEntry = & StartOfCopyTable [Loop];
        RtTblEntry  = & StartOfRtTable [Loop];

        /* Empty table entries are defined by NULL's in this field */
        if (RtTblEntry->OutputPktAddr != NULL)
        {
            ThisEntrysOutMid = CFE_SB_GetMsgId (RtTblEntry->OutputPktAddr);
            if (ThisEntrysOutMid == WhichMidToSend)
            {
                if(HK_CheckForMissingData(ThisEntrysOutMid,&InputMidMissing)==HK_MISSING_DATA_DETECTED)
                {
                    HK_AppData.MissingDataCtr++;
                    
                    CFE_EVS_SendEvent (HK_OUTPKT_MISSING_DATA_EID, CFE_EVS_DEBUG,
                       "Combined Packet 0x%04X missing data from Input Pkt 0x%04X", 
                       ThisEntrysOutMid,InputMidMissing);
                    
                }
#if HK_DISCARD_INCOMPLETE_COMBO == 1
                else /* This clause is only exclusive if discarding incomplete packets */
#endif
                { 
                    /* Send the combined housekeeping telemetry packet...        */
                    CFE_SB_TimeStampMsg ( (CFE_SB_Msg_t *) RtTblEntry->OutputPktAddr);
                    CFE_SB_SendMsg      ( (CFE_SB_Msg_t *) RtTblEntry->OutputPktAddr);
                
                    HK_AppData.CombinedPacketsSent ++ ;
                }

                HK_SetFlagsToNotPresent(ThisEntrysOutMid);               
                
                PacketFound = TRUE;
                
            }
        }
    }

    if (PacketFound == FALSE)
    {
        CFE_EVS_SendEvent (HK_UNKNOWN_COMBINED_PACKET_EID, CFE_EVS_INFORMATION,
                         "Combined HK Packet 0x%04X is not found in current HK Copy Table", 
                         WhichMidToSend);
    }

}   /* end HK_SendCombinedHkPacket */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Check the status of HK tables and perform any necessary action. */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_CheckStatusOfTables (void)
{

    int32   Status = CFE_SUCCESS;

    /* Determine if the copy table has a validation or update that needs to be performed */
    Status = CFE_TBL_GetStatus(HK_AppData.CopyTableHandle);

    if (Status == CFE_TBL_INFO_VALIDATION_PENDING)
    {
        /* Validate the specified Table */
        CFE_TBL_Validate(HK_AppData.CopyTableHandle);

    }
    else if (Status == CFE_TBL_INFO_UPDATE_PENDING)
    {
        /* Unsubscribe to input msgs and free out pkt buffers */
        HK_TearDownOldCopyTable (HK_AppData.CopyTablePtr, HK_AppData.RuntimeTablePtr);
            
        /* release address must be called for update to take */
        CFE_TBL_ReleaseAddress (HK_AppData.CopyTableHandle);

        /* Update the copy table */
        CFE_TBL_Update(HK_AppData.CopyTableHandle);

        /* Get address of the newly updated copy table */
        CFE_TBL_GetAddress((void *)(&HK_AppData.CopyTablePtr),
                                     HK_AppData.CopyTableHandle);
                   
        HK_ProcessNewCopyTable(HK_AppData.CopyTablePtr, HK_AppData.RuntimeTablePtr);
            
    }
    else if(Status != CFE_SUCCESS)
    {
            
        CFE_EVS_SendEvent (HK_UNEXPECTED_GETSTAT_RET_EID, CFE_EVS_ERROR,
               "Unexpected CFE_TBL_GetStatus return (0x%08X) for Copy Table", 
               Status);
    }
 
#if 0
    /* NOTE: The procedure for dumping tables is different for load-dump  and
     * dump-only tables. CFE_TBL_DumpToBuffer needs to be called by the owner
     * app for dump-only tables. For load-dump tables, CFE_TBL_DumpToBuffer is
     * called by Table Services when a dump cmd is received. Therefore,the owner
     * app should not call CFE_TBL_DumpToBuffer for load-dump tables. */
    
    /* Determine if the runtime table has a dump pending */
    CFE_TBL_Manage(HK_AppData.RuntimeTableHandle);      
#endif

    /* Below is the preferred way of checking for dump pending of the runtime 
     * table. But CFE_TBL_DumpToBuffer is a private function (in cFE 5.1.0 and 
     * earlier) and cannot be called by HK. The CFE_TBL_Manage api (used above) 
     * calls  CFE_TBL_DumpToBuffer but also makes unnecessary calls for 
     * dump-only tables. Until DumpToBuffer is made public, (see CFE FSW 
     * DCR3051) the CFE_TBL_Manage api call must be used. CFE_TBL_DumpToBuffer 
     * was made public in cFE version 5.2.0
     */

    /* Determine if the runtime table has a dump pending */   
    Status = CFE_TBL_GetStatus(HK_AppData.RuntimeTableHandle);
    
    if (Status == CFE_TBL_INFO_DUMP_PENDING)
    {
        /* Dump the specified Table, cfe tbl manager makes copy */
        CFE_TBL_DumpToBuffer(HK_AppData.RuntimeTableHandle);       

    }
    else if(Status != CFE_SUCCESS)
    {
        
        CFE_EVS_SendEvent (HK_UNEXPECTED_GETSTAT2_RET_EID, CFE_EVS_ERROR,
               "Unexpected CFE_TBL_GetStatus return (0x%08X) for Runtime Table", 
               Status);
    }
    
    return;

}   /* end HK_CheckStatusOfTables */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK Check for missing combined output message data               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HK_CheckForMissingData(CFE_SB_MsgId_t OutPktToCheck, CFE_SB_MsgId_t *MissingInputMid)
{
    int32                         Loop = 0;
    int32                         Status = HK_NO_MISSING_DATA;
    hk_copy_table_entry_t       * StartOfCopyTable;
    hk_copy_table_entry_t       * CpyTblEntry;
    hk_runtime_tbl_entry_t      * StartOfRtTable;
    hk_runtime_tbl_entry_t      * RtTblEntry;

    StartOfCopyTable = (hk_copy_table_entry_t *) HK_AppData.CopyTablePtr;
    StartOfRtTable  = (hk_runtime_tbl_entry_t *)  HK_AppData.RuntimeTablePtr;

    /* Loop thru each item in the runtime table until end is reached or 
     * data-not-present detected */
    do
    {
        CpyTblEntry = &StartOfCopyTable[Loop];
        RtTblEntry  = &StartOfRtTable[Loop];

        /* Empty table entries are defined by NULL's in this field */
        if ((RtTblEntry->OutputPktAddr != NULL)&&
           (CpyTblEntry->OutputMid==OutPktToCheck)&&
           (RtTblEntry->DataPresent==HK_DATA_NOT_PRESENT))           
        {           
            *MissingInputMid = CpyTblEntry->InputMid;
            Status = HK_MISSING_DATA_DETECTED;          
        }
        
        Loop++;
        
    }while((Loop < HK_COPY_TABLE_ENTRIES)&&(Status == HK_NO_MISSING_DATA));
    
    return Status;
    
}/* end HK_CheckForMissingData */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK Set data present flags to 'data-not-present'                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_SetFlagsToNotPresent(CFE_SB_MsgId_t OutPkt)
{
    int32                           Loop;
    hk_copy_table_entry_t         * StartOfCopyTable;
    hk_copy_table_entry_t         * CpyTblEntry;
    hk_runtime_tbl_entry_t        * StartOfRtTable;
    hk_runtime_tbl_entry_t        * RtTblEntry;

    StartOfCopyTable = (hk_copy_table_entry_t *) HK_AppData.CopyTablePtr;
    StartOfRtTable  = (hk_runtime_tbl_entry_t *)  HK_AppData.RuntimeTablePtr;

    /* Look thru each item in the runtime table until end is reached */ 
    for(Loop = 0;Loop < HK_COPY_TABLE_ENTRIES; Loop++)
    {
        CpyTblEntry = &StartOfCopyTable[Loop];
        RtTblEntry  = &StartOfRtTable[Loop];

        /* Empty table entries are defined by NULL's in this field */
        if ((RtTblEntry->OutputPktAddr != NULL)&&
           (CpyTblEntry->OutputMid==OutPkt))           
        {           
            RtTblEntry->DataPresent = HK_DATA_NOT_PRESENT;          
        }
                
    }
        
}/* end HK_SetFlagsToNotPresent */


/************************/
/*  End of File Comment */
/************************/
