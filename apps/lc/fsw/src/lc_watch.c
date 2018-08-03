/*************************************************************************
** File:
**   $Id: lc_watch.c 1.5 2015/03/04 16:09:52EST sstrege Exp  $
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
**   Functions used for CFS Limit Checker watchpoint processing
**
**   $Log: lc_watch.c  $
**   Revision 1.5 2015/03/04 16:09:52EST sstrege 
**   Added copyright information
**   Revision 1.4 2012/08/22 17:19:06EDT lwalling 
**   Modified true to false transition monitor to also accept stale to false
**   Revision 1.3 2012/08/01 12:42:12PDT lwalling 
**   Add STALE counters to watchpoint definition and result tables
**   Revision 1.2 2012/08/01 11:19:54PDT lwalling 
**   Change NOT_MEASURED to STALE
**   Revision 1.1 2012/07/31 13:53:40PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.7 2011/06/08 16:17:16EDT lwalling 
**   Added hash table functions, modified LC_CheckMsgForWPs() to use hash table functions
**   Revision 1.6 2011/01/19 12:43:11EST jmdagost 
**   Added lc_watch.h to include list.
**   Revision 1.5 2011/01/19 11:38:40EST jmdagost 
**   Initialize local variables per IV&V.
**   Revision 1.4 2009/02/26 11:01:01EST dahardis 
**   Modified so a watchponit transition from Not Measured to True
**   will get treated the same as a transition from False to True (DCR #7097)
**   Revision 1.3 2009/01/15 15:36:17EST dahardis 
**   Unit test fixes
**   Revision 1.2 2008/12/03 13:59:46EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:52EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_watch.h"
#include "lc_events.h"
#include "lc_custom.h"
#include "lc_perfids.h"

#include "cfe_platform_cfg.h" /* for CFE_SB_HIGHEST_VALID_MSGID */

/************************************************************************
** Local Macro Definitions
*************************************************************************/

/*************************************************************************
** Local Function Prototypes
*************************************************************************/
/************************************************************************/
/** \brief Process a single watchpoint
**  
**  \par Description
**       Support function for watchpoint processing that will
**       evaluate a single watchpoint
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   WatchIndex  The watchpoint number to evaluate (zero
**                            based watchpoint definition table index)
**
**  \param [in]   MessagePtr  A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message that
**                            contains the watchpoint data
**
**  \param [in]   Timestamp   A #CFE_TIME_SysTime_t timestamp to use
**                            to update the watchpoint results data
**                            if a state transition is detected
**
*************************************************************************/
void LC_ProcessWP(uint16             WatchIndex, 
                  CFE_SB_MsgPtr_t    MessagePtr,
                  CFE_TIME_SysTime_t Timestamp);

/************************************************************************/
/** \brief Operator comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       the watchpoint data comparison based upon the operator and
**       data type specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex         The watchpoint number to compare (zero
**                                 based watchpoint definition table index)
**
**  \param [in] ProcessedWPData    The watchpoint data extracted from
**                                 the message that it was contained
**                                 in. This is the data after any
**                                 sizing, bit-masking, and endianess
**                                 fixing that LC might have done
**                                 according to the watchpoint definition
**
**  \returns
**  \retcode #LC_WATCH_ERROR  \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \retstmt Return codes from #LC_SignedCompare    \endcode
**  \retstmt Return codes from #LC_UnsignedCompare  \endcode
**  \retstmt Return codes from #LC_FloatCompare     \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_OperatorCompare(uint16 WatchIndex,
                         uint32 ProcessedWPData);

/************************************************************************/
/** \brief Signed comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       a signed watchpoint data comparison based upon the operator
**       specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex    The watchpoint number to compare (zero
**                            based watchpoint definition table index)
**
**  \param [in] WPValue       The watchpoint data extracted from
**                            the message that it was contained
**                            in. This is the data after any
**                            sizing, bit-masking, and endianess
**                            fixing that LC might have done
**                            according to the watchpoint definition
**
**  \param [in] CompareValue  The comparison value specified in the 
**                            watchpoint definition table (sign
**                            extended, if needed, in an int32)
**
**  \returns
**  \retcode #LC_WATCH_TRUE  \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_SignedCompare(uint16 WatchIndex,
                       int32  WPValue, 
                       int32  CompareValue);
  
/************************************************************************/
/** \brief Unsigned comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       an unsigned watchpoint data comparison based upon the operator
**       specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex    The watchpoint number to compare (zero
**                            based watchpoint definition table index)
**
**  \param [in] WPValue       The watchpoint data extracted from
**                            the message that it was contained
**                            in. This is the data after any
**                            sizing, bit-masking, and endianess
**                            fixing that LC might have done
**                            according to the watchpoint definition
**
**  \param [in] CompareValue  The comparison value specified in the 
**                            watchpoint definition table (zero
**                            extended, if needed, in an uint32)
**
**  \returns
**  \retcode #LC_WATCH_TRUE  \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_UnsignedCompare(uint16 WatchIndex,
                         uint32 WPValue, 
                         uint32 CompareValue);
  
/************************************************************************/
/** \brief Float comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       an floating point watchpoint data comparison based upon the operator
**       specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex        The watchpoint number to compare (zero
**                                based watchpoint definition table index)
**
**  \param [in] WPMultiType       The watchpoint data extracted from
**                                the message that it was contained
**                                in. Stored in a multi-type union.
**                                This is the data after any sizing,
**                                bit-masking, and endianess fixing
**                                that LC might have done according
**                                to the watchpoint definition
**
**  \param [in] CompareMultiType  The comparison value specified in the 
**                                watchpoint definition table. Stored
**                                in a muti-type union so it can easily
**                                be accessed as a uint32 for validity
**                                checks
**
**  \returns
**  \retcode #LC_WATCH_TRUE  \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_FloatCompare(uint16 WatchIndex,
                      LC_MultiType_t WPMultiType, 
                      LC_MultiType_t CompareMultiType);

/************************************************************************/
/** \brief Watchpoint offset valid
**  
**  \par Description
**       Support function for watchpoint processing that will check if
**       the watchpoint offset specified in the definition table would
**       extend past the message that contains the watchpoint data
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   WatchIndex  The watchpoint number to check (zero
**                            based watchpoint definition table index)
**
**  \param [in]   MessagePtr  A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message that
**                            contains the watchpoint data
**
**  \returns
**  \retstmt Returns TRUE if the offset is within the message size \endcode
**  \retstmt Returns FALSE if the offset extends past message end  \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_WPOffsetValid(uint16          WatchIndex, 
                         CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Get sized data
**  
**  \par Description
**       Support function for watchpoint processing that will extract
**       the watchpoint data from a software bus message based upon the
**       data type specified in the watchpoint definition table and 
**       store it in a uint32. If there are any endian differences between
**       LC and the watchpoint data, this is where it will get fixed up.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  WatchIndex     The watchpoint number to extract (zero
**                              based watchpoint definition table index)
**
**  \param [in]  WPDataPtr      A pointer to the first byte of the 
**                              watchpoint data as it exists in the 
**                              software bus message it was received in
**
**  \param [in]  SizedDataPtr   A pointer to where the extracted watchpoint
*                               data should be stored
**
**  \param [out] *SizedDataPtr  Contains the extracted watchpoint data.
**                              This will be set to zero on error
**
**  \returns
**  \retstmt Returns TRUE if no error           \endcode
**  \retstmt Returns FALSE if an error occurred \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_GetSizedWPData(uint16 WatchIndex,
                          uint8  *WPDataPtr,
                          uint32 *SizedDataPtr);

/************************************************************************/
/** \brief Check uint32 for float NAN
**  
**  \par Description
**       Utility function for watchpoint processing that will test if
**       a uint32 value would result in a NAN (not-a-number) value if
**       it was interpreted as a float.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  Data     The uint32 value to check
**
**  \returns
**  \retstmt Returns TRUE if value is a float NAN      \endcode
**  \retstmt Returns FALSE if value is not a float NAN \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_Uint32IsNAN(uint32 Data);

/************************************************************************/
/** \brief Check uint32 for float infinite
**  
**  \par Description
**       Utility function for watchpoint processing that will test if
**       a uint32 value would result in an infinite value if
**       it was interpreted as a float.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  Data     The uint32 value to check
**
**  \returns
**  \retstmt Returns TRUE if value is an inifinite float      \endcode
**  \retstmt Returns FALSE if value is not an inifinite float \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_Uint32IsInfinite(uint32 Data);

/************************************************************************/
/** \brief Convert messageID into hash table index
**  
**  \par Description
**       Utility function for watchpoint processing that converts a
**       messageID into an index into the watchpoint hash table.
**       
**       The following code supports use of the watchpoint hash table:
**
**       1) #LC_GetHashTableIndex - convert messageID to hash table index
**       2) #LC_CreateHashTable   - after load Watchpoint Definition Table
**       3) #LC_AddWatchpoint     - add one watchpoint to hash table
**       4) #LC_CheckMsgForWPs    - process all WP's that reference messageID
**       
**       The following data structures support the hash table:
**
**       1) Hash table (256 entries)
**       2) Array of links for messageID linked lists (LC_MAX_WATCHPOINTS)
**       3) Array of links for watchpoint linked lists (LC_MAX_WATCHPOINTS)
**       
**       Rather than search the entire Watchpoint Definition Table to find
**       the watchpoints that reference a particular messageID, LC does
**       the following:
**
**       1) Call hash table function (convert messageID to hash table index)
**       2) Get messageID linked list from indexed hash table entry
**       3) Search messageID list (max 8) for matching messageID
**       4) Get watchpoint linked list from matching messageID link
**       5) Done - only watchpoints that reference messageID are in list
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessageID   SoftwareBus packet message ID
**
**  \returns
**  \retstmt Returns index into watchpoint hash table  \endcode
**  \endreturns
**
*************************************************************************/
uint32 LC_GetHashTableIndex(CFE_SB_MsgId_t MessageID);

/************************************************************************/
/** \brief Add one watchpoint linked list entry during creation of hash table
**  
**  \par Description
**       Utility function that adds another link to the watchpoint linked list
**       for the specified messageID. The function will also add a messageID
**       linked list entry to the hash table if this is the first reference
**       to that messageID. The function will also subscribe to the messageID
**       if this is the first reference to that messageID. The function will
**       return a pointer to the watchpoint linked list entry just added.
**       
**       The following code supports use of the watchpoint hash table:
**
**       1) #LC_GetHashTableIndex - convert messageID to hash table index
**       2) #LC_CreateHashTable   - after load Watchpoint Definition Table
**       3) #LC_AddWatchpoint     - add one watchpoint to hash table
**       4) #LC_CheckMsgForWPs    - process all WP's that reference messageID
**       
**       The following data structures support the hash table:
**
**       1) Hash table (256 entries)
**       2) Array of links for messageID linked lists (LC_MAX_WATCHPOINTS)
**       3) Array of links for watchpoint linked lists (LC_MAX_WATCHPOINTS)
**       
**       Rather than search the entire Watchpoint Definition Table to find
**       the watchpoints that reference a particular messageID, LC does
**       the following:
**
**       1) Call hash table function (convert messageID to hash table index)
**       2) Get messageID linked list from indexed hash table entry
**       3) Search messageID list (max 8) for matching messageID
**       4) Get watchpoint linked list from matching messageID link
**       5) Done - only watchpoints that reference messageID are in list
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessageID   SoftwareBus packet message ID
**
**  \returns
**  \retstmt Returns pointer to the watchpoint linked list entry just added  \endcode
**  \endreturns
**
*************************************************************************/
LC_WatchPtList_t  *LC_AddWatchpoint(CFE_SB_MsgId_t MessageID);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_GetHashTableIndex() - convert messageID to hash table index  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 LC_GetHashTableIndex(CFE_SB_MsgId_t MessageID)
{
    /*
    **   The purpose of a hash function is to take the input value
    ** and convert it to an index into the hash table. Assume that
    ** the range of input values is much different (larger) than
    ** the number of entries in the hash table. Then multiple input
    ** values must resolve to the same output table index. This is ok
    ** because each entry in the hash table is a linked list of all
    ** the inputs with the same hash function result.
    **
    **   This particular hash function takes advantage of knowledge
    ** regarding the format of the input values (cFE MessageID). By
    ** ignoring the bits that define version number, packet type and
    ** secondary header (high 5 bits of 16) we are left with the bits
    ** (mask = 0x7FF) that can identify 2048 unique input telemetry
    ** packets. Also, by using a fixed hash table size of 256 entries
    ** and using only the lower 8 bits of the bitmask as the result
    ** of the hash function, no single hash table entry will have more
    ** than 8 elements in its linked list.
    */
    return((uint32) (MessageID & LC_HASH_TABLE_MASK));

} /* End of LC_GetHashTableIndex() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_CreateHashTable() - create watchpoint hash table             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void LC_CreateHashTable(void)
{
    LC_WatchPtList_t  *WatchPtLink;
    CFE_SB_MsgId_t     LastMessageID;
    CFE_SB_MsgId_t     MessageID;
    int32              MessageLinkIndex;
    int32              WatchPtTblIndex;
    int32              Result;


    /* Un-subscribe to any MessageID's referenced in previous Watchpoint Definition Table */
    for (MessageLinkIndex = 0; MessageLinkIndex < LC_OperData.MessageIDsCount; MessageLinkIndex++)
    {
        MessageID = LC_OperData.MessageLinks[MessageLinkIndex].MessageID;

        if ((Result = CFE_SB_Unsubscribe(MessageID, LC_OperData.CmdPipe)) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_UNSUB_WP_ERR_EID, CFE_EVS_ERROR,
                             "Error unsubscribing watchpoint: MID=0x%04X, RC=0x%08X", 
                              MessageID, Result);
        }
    }

    /* Initialize hash table structures */
    CFE_PSP_MemSet(LC_OperData.HashTable, 0, (LC_HASH_TABLE_ENTRIES * sizeof(LC_MessageList_t *)));
    CFE_PSP_MemSet(LC_OperData.MessageLinks, 0, (LC_MAX_WATCHPOINTS * sizeof(LC_MessageList_t)));
    CFE_PSP_MemSet(LC_OperData.WatchPtLinks, 0, (LC_MAX_WATCHPOINTS * sizeof(LC_WatchPtList_t)));

    LC_OperData.MessageIDsCount = 0;
    LC_OperData.WatchpointCount = 0;

    LastMessageID = 0xFFFF;
    WatchPtLink = (LC_WatchPtList_t *) NULL;

    /* Process each entry in the Watchpoint Definition Table */
    for (WatchPtTblIndex = 0; WatchPtTblIndex < LC_MAX_WATCHPOINTS; WatchPtTblIndex++)
    {
        /* Skip unused watchpoint table entries */
        if (LC_OperData.WDTPtr[WatchPtTblIndex].DataType != LC_WATCH_NOT_USED)
        {
            MessageID = LC_OperData.WDTPtr[WatchPtTblIndex].MessageID;

            /* Use optimized code path if same MessageID as last watchpoint */
            if ((LastMessageID == MessageID) && (WatchPtLink != (LC_WatchPtList_t *) NULL))
            {
                /* WatchPtLink points to last link in list for this Message ID */
                WatchPtLink->Next = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++];

                /* Add new link to end of list, point to new last link */
                WatchPtLink = WatchPtLink->Next;
            }
            else
            {
                /* May add message list link and subscribe to MessageID */
                WatchPtLink = LC_AddWatchpoint(MessageID);
            }

            /* Set watchpoint table index for this entry in list */
            WatchPtLink->WatchIndex = WatchPtTblIndex;

            LastMessageID = MessageID;
        }
    }

    return;

} /* End of LC_CreateHashTable() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_AddWatchpoint() - add one watchpoint entry to hash table     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

LC_WatchPtList_t  *LC_AddWatchpoint(CFE_SB_MsgId_t MessageID)
{
    LC_MessageList_t  *MessageLink;
    LC_WatchPtList_t  *WatchPtLink;
    boolean            NeedSubscription;
    int32              HashTableIndex;
    int32              Result;

    /* Hash function converts MessageID into hash table index */
    HashTableIndex = LC_GetHashTableIndex(MessageID);

    /* Subscribe only for first reference to each MessageID */
    NeedSubscription = FALSE;

    /* Each hash table entry is a linked list of MessageID's with same hash result */
    if (LC_OperData.HashTable[HashTableIndex] == (LC_MessageList_t *) NULL)
    {
        /* Get next unused MessageID linked list entry */
        MessageLink = &LC_OperData.MessageLinks[LC_OperData.MessageIDsCount++];

        /* Set first (and only) link in this hash table entry linked list */
        LC_OperData.HashTable[HashTableIndex] = MessageLink;

        /* Set the MessageID for this link */
        MessageLink->MessageID = MessageID;

        /* Subscribe to first link in list */
        NeedSubscription = TRUE;
    }
    else
    {
        /* Get start of linked list (all MID's with same hash result) */
        MessageLink = LC_OperData.HashTable[HashTableIndex];

        /* Find the link for this MessageID */
        while (MessageLink->MessageID != MessageID)
        {
            if (MessageLink->Next == (LC_MessageList_t *) NULL)
            {
                /* Reached end of list without finding MessageID */
                MessageLink->Next = &LC_OperData.MessageLinks[LC_OperData.MessageIDsCount++];
                MessageLink = MessageLink->Next;

                /* Add link with this MessageID (will exit loop) */
                MessageLink->MessageID = MessageID;

                /* Subscribe to new link in list */
                NeedSubscription = TRUE;
            }
            else
            {
                /* Try the next link in the list */
                MessageLink = MessageLink->Next;
            }
        }
    }

    /* Subscribe only once to each MessageID */
    if (NeedSubscription)
    {
        if ((Result = CFE_SB_Subscribe(MessageID, LC_OperData.CmdPipe)) != CFE_SUCCESS)
        {
            /* Signal the error, but continue */
            CFE_EVS_SendEvent(LC_SUB_WP_ERR_EID, CFE_EVS_ERROR,
               "Error subscribing watchpoint: MID=0x%04X, RC=0x%08X",
                MessageID, Result);    
        }
    }

    /* MessageLink points to the link for this MessageID */
    if (MessageLink->WatchPtList == (LC_WatchPtList_t *) NULL)
    {
        /* Get next unused watchpoint linked list entry */
        WatchPtLink = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++];

        /* Set the start (and only) link in the watchpoint link list */
        MessageLink->WatchPtList = WatchPtLink;
    }
    else
    {
        /* Find the end of the watchpoint linked list */
        WatchPtLink = MessageLink->WatchPtList;

        while (WatchPtLink->Next != (LC_WatchPtList_t *) NULL)
        {
            WatchPtLink = WatchPtLink->Next;
        }

        /* Add the new watchpoint link to the end of the list */
        WatchPtLink->Next = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++];
        WatchPtLink = WatchPtLink->Next;
    }

    /* Return pointer to last link in watchpoint linked list */
    return(WatchPtLink);

} /* End of LC_AddWatchpoint() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Handle a message with possible watchpoints                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_CheckMsgForWPs(CFE_SB_MsgId_t MessageID, CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_TIME_SysTime_t Timestamp;
    LC_MessageList_t  *MessageList;
    LC_WatchPtList_t  *WatchPtList;
    boolean            WatchPtFound = FALSE;
    
    /* Do nothing if disabled at the application level */
    if (LC_AppData.CurrentLCState != LC_STATE_DISABLED)
    {
        /* Use message timestamp - if none, use current time */
        Timestamp = CFE_SB_GetMsgTime(MessagePtr);
        
        if ((Timestamp.Seconds == 0) && (Timestamp.Subseconds == 0))
        {
            Timestamp = CFE_TIME_GetTime();
        }
        
        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(LC_WDT_SEARCH_PERF_ID);

        /* Get start of linked list (all MID's with same hash result) */
        MessageList = LC_OperData.HashTable[LC_GetHashTableIndex(MessageID)];

        /* NULL when list is empty or end of list */
        while (MessageList != (LC_MessageList_t *) NULL)
        {
            /* Compare this linked list entry for matching MessageID */
            if (MessageList->MessageID == MessageID)
            {
                /* Stop the search - we found it */
                break;
            }

            /* Max of 8 links per design */
            MessageList = MessageList->Next;
        }

        /* Should be true - else wouldn't subscribe to MessageID */
        if (MessageList != (LC_MessageList_t *) NULL)
        {
            /* Get linked list of WP's that reference MessageID */
            WatchPtList = MessageList->WatchPtList;

            /* NULL when list is empty or end of list */
            while (WatchPtList != (LC_WatchPtList_t *) NULL)
            {
                WatchPtFound = TRUE;
                
                /* Verify that WP packet offset is within actual packet */
                if (LC_WPOffsetValid(WatchPtList->WatchIndex, MessagePtr) == TRUE)
                {
                    LC_ProcessWP(WatchPtList->WatchIndex, MessagePtr, Timestamp);
                }

                /* No limit to how many WP's can reference one MessageID */
                WatchPtList = WatchPtList->Next;
            }
        }

        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(LC_WDT_SEARCH_PERF_ID);

        if (WatchPtFound == TRUE)
        {
            LC_AppData.MonitoredMsgCount++;
        }
        else
        {
            /* MessageID with no defined watchpoints */
            CFE_EVS_SendEvent(LC_MID_INF_EID, CFE_EVS_INFORMATION,
               "Msg with unreferenced message ID rcvd: ID = 0x%04X", MessageID);
        }
    }
    
    return;
   
} /* end LC_CheckMsgForWPs */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a single watchpoint                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ProcessWP(uint16             WatchIndex, 
                  CFE_SB_MsgPtr_t    MessagePtr,
                  CFE_TIME_SysTime_t Timestamp)
{
    uint8   *WPDataPtr;
    uint8   PreviousResult;
    uint8   WPEvalResult;
    uint32  SizedWPData;
    uint32  MaskedWPData;
    uint32  StaleCounter;
    boolean SizedDataValid;
    
    /*
    ** Setup the pointer and get the massaged data
    */
    WPDataPtr = ((uint8 *)MessagePtr) + 
                 LC_OperData.WDTPtr[WatchIndex].WatchpointOffset;
    
    SizedDataValid = LC_GetSizedWPData(WatchIndex, 
                                       WPDataPtr,
                                       &SizedWPData);
    if (SizedDataValid == TRUE)
    {
        /*
        ** Get the last evalution result for this watchpoint
        */
        PreviousResult = LC_OperData.WRTPtr[WatchIndex].WatchResult;
        
        /*
        ** Apply the defined bitmask for this watchpoint and then
        ** call the mission defined custom function or do our own 
        ** relational comparison.
        */
        MaskedWPData = SizedWPData & LC_OperData.WDTPtr[WatchIndex].BitMask;
        
        if (LC_OperData.WDTPtr[WatchIndex].OperatorID == LC_OPER_CUSTOM)
        {
           WPEvalResult = LC_CustomFunction(WatchIndex,
                                            MaskedWPData,
                                            MessagePtr,
                                            LC_OperData.WDTPtr[WatchIndex].CustomFuncArgument);
        }
        else
        {
           WPEvalResult = LC_OperatorCompare(WatchIndex, MaskedWPData);
        }
     
        /*
        ** Update the watch result
        */
        LC_OperData.WRTPtr[WatchIndex].WatchResult = WPEvalResult;
        
        /*
        ** Update the watchpoint statistics based on the evaluation
        ** result
        */
        LC_OperData.WRTPtr[WatchIndex].EvaluationCount++;
        
        if (WPEvalResult == LC_WATCH_TRUE)
        {
            LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount++;
            LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount++;
            StaleCounter = LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = StaleCounter;
            
            if ((PreviousResult == LC_WATCH_FALSE) || 
                (PreviousResult == LC_WATCH_STALE))
            {
                LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount++;
                
                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue
                                              .Value = MaskedWPData;
                
                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue
                                              .Timestamp.Seconds = Timestamp.Seconds; 
                
                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue
                                              .Timestamp.Subseconds = Timestamp.Subseconds; 
            }
        }
        else if (WPEvalResult == LC_WATCH_FALSE)
        {
            LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount = 0;
            StaleCounter = LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = StaleCounter;
            
            if ((PreviousResult == LC_WATCH_TRUE) || 
                (PreviousResult == LC_WATCH_STALE))
            {
                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse
                                              .Value = MaskedWPData;
                
                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse
                                              .Timestamp.Seconds = Timestamp.Seconds; 
                
                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse
                                              .Timestamp.Subseconds = Timestamp.Subseconds; 
            }
        }
        else
        {
            /*
            ** WPEvalResult is STALE or ERROR
            */
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;
        }        
        
    } /* end SizedDataValid if */
    
    return;
    
} /* end LC_ProcessWP */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a watchpoint relational comparison                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_OperatorCompare(uint16 WatchIndex,
                         uint32 ProcessedWPData)
{
    uint8          EvalResult;
    LC_MultiType_t WatchpointValue;
    LC_MultiType_t ComparisonValue;

	/*
     * The "ProcessedWPData" has been already normalized to be 
     * 32 bits wide and in the native CPU byte order.  For actual
     * comparison, it needs to be truncated back down to the same
     * size as the reference value.
     */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
    {
        case LC_DATA_UBYTE:
        case LC_DATA_BYTE:
           WatchpointValue.Unsigned8 = ProcessedWPData & 0xFF;
           break;

        case LC_DATA_WORD_BE:
        case LC_DATA_WORD_LE:
        case LC_DATA_UWORD_BE:
        case LC_DATA_UWORD_LE:
           WatchpointValue.Unsigned16 = ProcessedWPData & 0xFFFF;
           break;

        case LC_DATA_DWORD_BE:
        case LC_DATA_DWORD_LE:
        case LC_DATA_UDWORD_BE:
        case LC_DATA_UDWORD_LE:
        case LC_DATA_FLOAT_BE:
        case LC_DATA_FLOAT_LE:
        default:
           WatchpointValue.Unsigned32 = ProcessedWPData;
           break;
    }
    ComparisonValue = LC_OperData.WDTPtr[WatchIndex].ComparisonValue;

    /*
    ** Handle the comparison appropriately depending on the data type
    ** Any endian difference was handled when the watchpoint 
    ** data was extracted from the SB message
    */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
    {
        /*
        ** Signed integer types will get sign extended
        */
        case LC_DATA_BYTE:
            EvalResult = LC_SignedCompare(WatchIndex,
                                          WatchpointValue.Signed8,
                                          ComparisonValue.Signed8);
            break;
              
        case LC_DATA_WORD_BE:
        case LC_DATA_WORD_LE:
            EvalResult = LC_SignedCompare(WatchIndex,
                                          WatchpointValue.Signed16,
                                          ComparisonValue.Signed16);
            break;

        case LC_DATA_DWORD_BE:
        case LC_DATA_DWORD_LE:
            EvalResult = LC_SignedCompare(WatchIndex,
                                          WatchpointValue.Signed32,
                                          ComparisonValue.Signed32);
            break;

        /*
        ** Unsigned integer types will get zero extended
        */
        case LC_DATA_UBYTE:
            EvalResult = LC_UnsignedCompare(WatchIndex,
                                            WatchpointValue.Unsigned8,
                                            ComparisonValue.Unsigned8);
            break;

        case LC_DATA_UWORD_BE:
        case LC_DATA_UWORD_LE:
            EvalResult = LC_UnsignedCompare(WatchIndex,
                                            WatchpointValue.Unsigned16,
                                            ComparisonValue.Unsigned16);
            break;

        case LC_DATA_UDWORD_BE:
        case LC_DATA_UDWORD_LE:
            EvalResult = LC_UnsignedCompare(WatchIndex,
                                            WatchpointValue.Unsigned32,
                                            ComparisonValue.Unsigned32);
            break;

        /*
        ** Floating point values are handled separately
        */
        case LC_DATA_FLOAT_BE:
        case LC_DATA_FLOAT_LE:            
            EvalResult = LC_FloatCompare(WatchIndex,
                                         WatchpointValue,
                                         ComparisonValue);
            break;

        default:
            /*
            ** This should have been caught before now, but we'll 
            ** handle it just in case we ever get here.
            */ 
            CFE_EVS_SendEvent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_ERROR,
                              "WP has undefined data type: WP = %d, DataType = %d",
                              WatchIndex, LC_OperData.WDTPtr[WatchIndex].DataType);
            
            EvalResult = LC_WATCH_ERROR;
            break;
    }
    
    return (EvalResult);
    
} /* end LC_OperatorCompare */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a watchpoint signed integer comparison                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_SignedCompare(uint16 WatchIndex,
                       int32  WPValue, 
                       int32  CompareValue)
{
    uint8   EvalResult;
    uint8   OperatorID;
    
    OperatorID      = LC_OperData.WDTPtr[WatchIndex].OperatorID;
    
    switch (OperatorID)
        {
        case LC_OPER_LE:
            EvalResult = (WPValue <= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_LT:
            EvalResult = (WPValue <  CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_EQ:
            EvalResult = (WPValue == CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_NE:
            EvalResult = (WPValue != CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GT:
            EvalResult = (WPValue >  CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GE:
            EvalResult = (WPValue >= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        default:
            /*
            ** This should have been caught before now, but we'll 
            ** handle it just in case we ever get here.
            */ 
            CFE_EVS_SendEvent(LC_WP_OPERID_ERR_EID, CFE_EVS_ERROR,
                              "WP has invalid operator ID: WP = %d, OperID = %d",
                              WatchIndex, OperatorID);
            
            EvalResult = LC_WATCH_ERROR;
            break;
        }
    
    return (EvalResult);
    
} /* end LC_SignedCompare */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a watchpoint unsigned integer comparison                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_UnsignedCompare(uint16 WatchIndex,
                         uint32 WPValue, 
                         uint32 CompareValue)
{
    uint8   EvalResult;
    uint8   OperatorID;
    
    OperatorID      = LC_OperData.WDTPtr[WatchIndex].OperatorID;
    
    switch (OperatorID)
        {
        case LC_OPER_LE:
            EvalResult = (WPValue <= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_LT:
            EvalResult = (WPValue <  CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_EQ:
            EvalResult = (WPValue == CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_NE:
            EvalResult = (WPValue != CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GT:
            EvalResult = (WPValue >  CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GE:
            EvalResult = (WPValue >= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        default:
            /*
            ** This should have been caught before now, but we'll 
            ** handle it just in case we ever get here.
            */ 
            CFE_EVS_SendEvent(LC_WP_OPERID_ERR_EID, CFE_EVS_ERROR,
                              "WP has invalid operator ID: WP = %d, OperID = %d",
                              WatchIndex, OperatorID);
            
            EvalResult = LC_WATCH_ERROR;
            break;
        }
    
    return (EvalResult);
    
} /* end LC_UnsignedCompare */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a floating point number comparison                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_FloatCompare(uint16 WatchIndex,
                      LC_MultiType_t WPMultiType, 
                      LC_MultiType_t CompareMultiType)
{
    uint8   EvalResult;
    uint8   OperatorID;
    float   WPFloat;
    float   CompareFloat;
    float   Diff;

    OperatorID      = LC_OperData.WDTPtr[WatchIndex].OperatorID;
   
    /*
    ** Before we do any comparison, check the watchpoint value for
    ** a floating point NAN (not-a-number). NAN comparisons don't
    ** work and can generate floating point exceptions. By contrast
    ** comparisons with infinite numbers will behave as they should
    ** so we don't try to catch those (we would rather they generate
    ** watchpoint violations).
    ** 
    ** The comparison (threshold) value comes from the Watchpoint
    ** Definition Table (WDT) and any weird values should get nailed
    ** during table validation.
    */
    if (LC_Uint32IsNAN(WPMultiType.Unsigned32) == FALSE)
    {
        WPFloat      = WPMultiType.Float32;
        CompareFloat = CompareMultiType.Float32;
        
        switch (OperatorID)
            {
            case LC_OPER_LE:
                EvalResult = (WPFloat <= CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_LT:
                EvalResult = (WPFloat <  CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_EQ:
                Diff = (WPFloat > CompareFloat) ? (WPFloat - CompareFloat) : (CompareFloat - WPFloat);
                EvalResult = (Diff <= (float) LC_FLOAT_TOLERANCE) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;
                
            case LC_OPER_NE:
                Diff = (WPFloat > CompareFloat) ? (WPFloat - CompareFloat) : (CompareFloat - WPFloat);
                EvalResult = (Diff > (float) LC_FLOAT_TOLERANCE) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;
                
            case LC_OPER_GT:
                EvalResult = (WPFloat >  CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_GE:
                EvalResult = (WPFloat >= CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            default:
                /*
                ** This should have been caught before now, but we'll 
                ** handle it just in case we ever get here.
                */ 
                CFE_EVS_SendEvent(LC_WP_OPERID_ERR_EID, CFE_EVS_ERROR,
                                  "WP has invalid operator ID: WP = %d, OperID = %d",
                                  WatchIndex, OperatorID);
                
                EvalResult = LC_WATCH_ERROR;
                break;
                     
            } /*  end of switch  */
        
    } /* end LC_WPIsNAN if */
    else
    {
        CFE_EVS_SendEvent(LC_WP_NAN_ERR_EID, CFE_EVS_ERROR,
                          "WP data value is a float NAN: WP = %d, Value = 0x%08X",
                          WatchIndex, WPMultiType.Unsigned32);
        
        EvalResult = LC_WATCH_ERROR;
    }
    
    return (EvalResult);
    
} /* end LC_FloatCompare */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Checks if a defined watchpoint offset will send us past the     */ 
/* end of the received message                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean LC_WPOffsetValid(uint16             WatchIndex, 
                         CFE_SB_MsgPtr_t    MessagePtr)
{
    uint16          MsgLength;
    uint32          Offset;
    uint32          NumOfDataBytes = 0;
    boolean         OffsetValid    = TRUE; 
    CFE_SB_MsgId_t  MessageID      = 0;
    
    /*
    ** Check the message length against the watchpoint
    ** offset and data type to make sure we won't
    ** try to read past it.
    */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
        {
        case LC_DATA_BYTE:
        case LC_DATA_UBYTE:
            NumOfDataBytes = sizeof (uint8);
            break;

        case LC_DATA_WORD_BE:
        case LC_DATA_WORD_LE:
        case LC_DATA_UWORD_BE:
        case LC_DATA_UWORD_LE:
            NumOfDataBytes = sizeof (uint16);
            break;

        case LC_DATA_DWORD_BE:
        case LC_DATA_DWORD_LE:
        case LC_DATA_UDWORD_BE:
        case LC_DATA_UDWORD_LE:
            NumOfDataBytes = sizeof (uint32);
            break;
            
        case LC_DATA_FLOAT_BE:
        case LC_DATA_FLOAT_LE:
            NumOfDataBytes = sizeof (float);
            break;
            
        default:
            /*
            ** This should have been caught before now, but we'll 
            ** handle it just in case we ever get here.
            */ 
            CFE_EVS_SendEvent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_ERROR,
                              "WP has undefined data type: WP = %d, DataType = %d",
                              WatchIndex, LC_OperData.WDTPtr[WatchIndex].DataType);
            
            LC_OperData.WRTPtr[WatchIndex].WatchResult = LC_WATCH_ERROR;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;
            
            return (FALSE);
            break;
            
        } /* end switch */     
    
    MsgLength = CFE_SB_GetTotalMsgLength(MessagePtr);
    
    Offset = LC_OperData.WDTPtr[WatchIndex].WatchpointOffset;
    
    if ((Offset + NumOfDataBytes) > MsgLength)
    {
        OffsetValid = FALSE;   

        MessageID = CFE_SB_GetMsgId(MessagePtr);
        
        CFE_EVS_SendEvent(LC_WP_OFFSET_ERR_EID, CFE_EVS_ERROR,
                "WP offset error: MID = %d, WP = %d, Offset = %d, DataSize = %d, MsgLen = %d",
                MessageID, WatchIndex, Offset, NumOfDataBytes, MsgLength);
        
        LC_OperData.WRTPtr[WatchIndex].WatchResult = LC_WATCH_ERROR;
        LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;
    }
    
    return (OffsetValid);
   
} /* end LC_WPOffsetValid */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Copy a single watchpoint datum and simultaneously byteswap it   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void LC_CopyBytesWithSwap(LC_MultiType_t *DestBuffer, const uint8 *SrcPtr,
                           const LC_MultiType_t SwapMap, uint32 NumBytes)
{
   while (NumBytes > 0)
   {
      --NumBytes;
      DestBuffer->RawByte[NumBytes] = SrcPtr[SwapMap.RawByte[NumBytes] & 0x3];
   }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get sized watchpoint data                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean LC_GetSizedWPData(uint16 WatchIndex,
                          uint8  *WPDataPtr,
                          uint32 *SizedDataPtr)
{
    boolean Success    = TRUE;
    LC_MultiType_t ConvBuffer;
    LC_MultiType_t TempBuffer;

    ConvBuffer.Unsigned32 = 0;
    TempBuffer.Unsigned32 = 0;

    /* 
    ** Get the watchpoint data value (which may be on a misaligned
    ** address boundary) and put it into an unsigned 32 properly
    ** handling endian and sign extension issues 
    */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
        {
        case LC_DATA_BYTE:
           TempBuffer.Unsigned8  = *WPDataPtr;
           ConvBuffer.Signed32 = TempBuffer.Signed8;  /* Extend signed 8 bit value to 32 bits */
           break;
              
        case LC_DATA_UBYTE:
           ConvBuffer.Unsigned32 = *WPDataPtr;        /* Extend unsigned 8 bit value to 32 bits */
           break;

        case LC_DATA_WORD_BE:
           ConvBuffer.Unsigned16 = 0x0001;
           LC_CopyBytesWithSwap(&TempBuffer,WPDataPtr,ConvBuffer,sizeof(int16));
           ConvBuffer.Signed32 = TempBuffer.Signed16; /* Extend signed 16 bit value to 32 bits */
           break;

        case LC_DATA_WORD_LE:
           ConvBuffer.Unsigned16 = 0x0100;
           LC_CopyBytesWithSwap(&TempBuffer,WPDataPtr,ConvBuffer,sizeof(int16));
           ConvBuffer.Signed32 = TempBuffer.Signed16; /* Extend signed 16 bit value to 32 bits */
           break;

        case LC_DATA_UWORD_BE:
           ConvBuffer.Unsigned16 = 0x0001;
           LC_CopyBytesWithSwap(&TempBuffer,WPDataPtr,ConvBuffer,sizeof(uint16));
           ConvBuffer.Unsigned32 = TempBuffer.Unsigned16;   /* Extend unsigned 16 bit value to 32 bits */
           break;
            
        case LC_DATA_UWORD_LE:
           ConvBuffer.Unsigned16 = 0x0100;
           LC_CopyBytesWithSwap(&TempBuffer,WPDataPtr,ConvBuffer,sizeof(uint16));
           ConvBuffer.Unsigned32 = TempBuffer.Unsigned16;   /* Extend unsigned 16 bit value to 32 bits */
           break;

        case LC_DATA_DWORD_BE:
        case LC_DATA_UDWORD_BE:
        case LC_DATA_FLOAT_BE:
           ConvBuffer.Unsigned32 = 0x00010203;
           LC_CopyBytesWithSwap(&TempBuffer,WPDataPtr,ConvBuffer,sizeof(uint32));
           ConvBuffer.Unsigned32 = TempBuffer.Unsigned32;   /* Straight copy - no extension (signed or unsigned) */
           break;
                
        case LC_DATA_DWORD_LE:
        case LC_DATA_UDWORD_LE:
        case LC_DATA_FLOAT_LE:
           ConvBuffer.Unsigned32 = 0x03020100;
           LC_CopyBytesWithSwap(&TempBuffer,WPDataPtr,ConvBuffer,sizeof(uint32));
           ConvBuffer.Unsigned32 = TempBuffer.Unsigned32;   /* Straight copy - no extension (signed or unsigned) */
           break;
            
        default:
            /*
            ** This should have been caught before now, but we'll 
            ** handle it just in case we ever get here.
            */ 
            CFE_EVS_SendEvent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_ERROR,
                              "WP has undefined data type: WP = %d, DataType = %d",
                              WatchIndex, LC_OperData.WDTPtr[WatchIndex].DataType);
            
            LC_OperData.WRTPtr[WatchIndex].WatchResult = LC_WATCH_ERROR;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;
            
            Success = FALSE;
            break;
            
        } /* end switch */     
    
    /*
    ** Set result value
    */
    *SizedDataPtr = ConvBuffer.Unsigned32;
    
    /*
    ** Return success flag
    */
    return (Success);
    
} /* end LC_GetSizedWPData */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the watchpoint definition table (WDT)                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
int32 LC_ValidateWDT(void *TableData)
{
    LC_WDTEntry_t *TableArray = (LC_WDTEntry_t *) TableData;

    int32 EntryResult = LC_WDTVAL_NO_ERR;
    int32 TableResult = CFE_SUCCESS;
    int32 TableIndex;
    
    uint8  DataType;
    uint8  OperatorID;
    uint16 MessageID;
    uint32 CompareValue;
    
    int32 GoodCount   = 0;
    int32 BadCount    = 0;
    int32 UnusedCount = 0;

    /*
    ** Verify each entry in the pending watchpoint definition table
    */
    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        DataType     = TableArray[TableIndex].DataType;
        OperatorID   = TableArray[TableIndex].OperatorID;
        MessageID    = TableArray[TableIndex].MessageID;
        CompareValue = TableArray[TableIndex].ComparisonValue.Unsigned32;

        if (DataType == LC_WATCH_NOT_USED)
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if ((DataType != LC_DATA_BYTE)      &&
                 (DataType != LC_DATA_UBYTE)     &&
                 (DataType != LC_DATA_BYTE)      &&
                 (DataType != LC_DATA_UBYTE)     &&
                 (DataType != LC_DATA_WORD_BE)   &&
                 (DataType != LC_DATA_WORD_LE)   &&
                 (DataType != LC_DATA_UWORD_BE)  &&
                 (DataType != LC_DATA_UWORD_LE)  &&
                 (DataType != LC_DATA_DWORD_BE)  &&
                 (DataType != LC_DATA_DWORD_LE)  &&
                 (DataType != LC_DATA_UDWORD_BE) &&
                 (DataType != LC_DATA_UDWORD_LE) &&
                 (DataType != LC_DATA_FLOAT_BE)  &&
                 (DataType != LC_DATA_FLOAT_LE))
        {
            /*
            ** Invalid data type
            */
            BadCount++;
            EntryResult = LC_WDTVAL_ERR_DATATYPE;
        }
        else if ((OperatorID != LC_OPER_LT)  &&
                 (OperatorID != LC_OPER_LT)  &&
                 (OperatorID != LC_OPER_LE)  &&
                 (OperatorID != LC_OPER_NE)  &&
                 (OperatorID != LC_OPER_EQ)  &&
                 (OperatorID != LC_OPER_GE)  &&
                 (OperatorID != LC_OPER_GT)  &&
                 (OperatorID != LC_OPER_CUSTOM))
        {
            /*
            ** Invalid operator
            */
            BadCount++;
            EntryResult = LC_WDTVAL_ERR_OPER;
        }
        else if (MessageID > CFE_SB_HIGHEST_VALID_MSGID)
        {
            /*
            ** Bad message ID (limit set by configuration parameter,
            ** see cfe_platform_cfg.h)
            */
            BadCount++;
            EntryResult = LC_WDTVAL_ERR_MID;
        }
        else if ((DataType == LC_DATA_FLOAT_BE) ||
                 (DataType == LC_DATA_FLOAT_LE))
        {
            /*
            ** Check the floating point comparison value for
            ** NAN (not-a-number) or infinite values
            */
            if (LC_Uint32IsNAN(CompareValue) == TRUE)
            {
                BadCount++;
                EntryResult = LC_WDTVAL_ERR_FPNAN;
            }
            else if (LC_Uint32IsInfinite(CompareValue) == TRUE)
            {
                BadCount++;
                EntryResult = LC_WDTVAL_ERR_FPINF;
            }
            else
            {
                /*
                ** We passed all checks for this floating point entry
                */
                GoodCount++;
            }
        }
        else
        {
            /*
            ** We passed all checks for this non-floating point entry
            */
            GoodCount++;
        }
        
        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != LC_WDTVAL_NO_ERR) && (TableResult == CFE_SUCCESS))
        {
            if ((EntryResult == LC_WDTVAL_ERR_FPNAN) ||
                (EntryResult == LC_WDTVAL_ERR_FPINF))
            {
                CFE_EVS_SendEvent(LC_WDTVAL_FPERR_EID, CFE_EVS_ERROR,
                        "WDT verify float err: WP = %d, Err = %d, ComparisonValue = 0x%08X",
                        TableIndex, EntryResult, CompareValue);
            }
            else
            {
                CFE_EVS_SendEvent(LC_WDTVAL_ERR_EID, CFE_EVS_ERROR,
                        "WDT verify err: WP = %d, Err = %d, DType = %d, Oper = %d, MID = %d",
                        TableIndex, EntryResult, DataType, OperatorID, MessageID);
            }
            
            TableResult = EntryResult;
        }
        
    } /* end TableIndex for */
    
    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION,
                     "WDT verify results: good = %d, bad = %d, unused = %d",
                      GoodCount, BadCount, UnusedCount);

    return(TableResult);
    
} /* end LC_ValidateWDT */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test if a 32 bit integer's value would be a floating point      */
/* NAN (not-a-number). Assumes IEEE-754 floating point format      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean LC_Uint32IsNAN(uint32 Data)
{
    boolean Result = FALSE;
    uint32  Exponent;
    uint32  Fraction;
    
    /*
    ** Check if the exponent field is all 1's
    */
    Exponent = Data & 0x7F800000;
    
    if (Exponent == 0x7F800000)
    {
        /*
        ** If the fraction field is also non-zero,
        ** it's a NAN
        */
        Fraction = Data & 0x007FFFFF;
        
        if (Fraction > 0)
        {
            Result = TRUE;
        }
    }
    
    return (Result);
    
} /* end LC_Uint32IsNAN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test if a 32 bit integer's value would be an infinite           */
/* (positive or negative) floating point number. Assumes           */
/* IEEE-754 floating point format                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean LC_Uint32IsInfinite(uint32 Data)
{
    boolean Result = FALSE;
    uint32  Exponent;
    uint32  Fraction;
    
    /*
    ** Check if the exponent field is all 1's
    */
    Exponent = Data & 0x7F800000;
    
    if (Exponent == 0x7F800000)
    {
        /*
        ** If the fraction field is also zero,
        ** it's infinite
        */
        Fraction = Data & 0x007FFFFF;
        
        if (Fraction == 0)
        {
            Result = TRUE;
        }
    }
    
    return (Result);
    
} /* end LC_Uint32IsInfinite */


/************************/
/*  End of File Comment */
/************************/
