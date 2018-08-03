 /*************************************************************************
 ** File:
 **   $Id: sc_loads.c 1.19 2015/03/02 12:58:29EST sstrege Exp  $
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
 **   This file contains functions to handle validation of TBL tables,
 **   as well as setting up Stored Command's internal data structures for
 **   those tables
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_loads.c  $ 
 **   Revision 1.19 2015/03/02 12:58:29EST sstrege  
 **   Added copyright information 
 **   Revision 1.18 2014/06/06 14:10:33EDT sjudy  
 **   Initialized NewCmdTime=0 in SC_Insert. 
 **   Revision 1.17 2011/11/16 10:59:10GMT-08:00 lwalling  
 **   Removed local var NumberOfCommands from function SC_ParseRts() 
 **   Revision 1.16 2011/02/09 13:29:00EST lwalling  
 **   Process append command references wrong ATS buffer 
 **   Revision 1.15 2011/02/01 11:38:02EST lwalling  
 **   Remove command checksum test from ATS table verify -- no requirement 
 **   Revision 1.14 2011/01/28 18:02:04EST lwalling  
 **   Fix test for duplicate ATS command ID numbers 
 **   Revision 1.13 2010/09/28 10:33:52EDT lwalling  
 **   Update list of included header files 
 **   Revision 1.12 2010/05/18 15:31:38EDT lwalling  
 **   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber 
 **   Revision 1.11 2010/05/18 14:13:29EDT lwalling  
 **   Change AtsCmdIndexBuffer contents from entry pointer to entry index 
 **   Revision 1.10 2010/05/05 11:22:03EDT lwalling  
 **   Use common ATS verify events and return code definitions, 
 **   Revision 1.9 2010/04/21 15:42:19EDT lwalling  
 **   Changed local storage of Append ATS table use from bytes to words, wrote process Append function 
 **   Revision 1.8 2010/04/16 15:29:09EDT lwalling  
 **   Create function to update Append ATS table data 
 **   Revision 1.7 2010/04/15 15:22:16EDT lwalling  
 **   Create function to verify one ATS entry, create function to verify Append ATS table data 
 **   Revision 1.6 2010/04/05 11:51:16EDT lwalling  
 **   Create stub functions for validate, update and process Append ATS tables 
 **   Revision 1.5 2010/03/26 18:02:50EDT lwalling  
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values 
 **   Revision 1.4 2010/03/26 11:27:45EDT lwalling  
 **   Fixed packet length calculation to support odd byte length ATS and RTS commands 
 **   Revision 1.3 2009/01/26 14:44:46EST nyanchik  
 **   Check in of Unit test 
 **   Revision 1.2 2009/01/05 08:26:52EST nyanchik  
 **   Check in after code review changes 
 *************************************************************************/

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "sc_app.h"
#include "sc_atsrq.h"
#include "sc_utils.h"
#include "sc_events.h"

/**************************************************************************
 **
 ** Local #defines
 **
 **************************************************************************/

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/************************************************************************/
/** \brief Parses an RTS to see if it is valid
 **  
 **  \par Description
 **         This routine is called to validate an RTS buffer. It parses through
 **           the RTS to make sure all of the commands look in reasonable shape.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    Buffer          A pointer to the area to validate
 **
 **
 *************************************************************************/
boolean SC_ParseRts (uint16 Buffer []);

/************************************************************************/
/** \brief Buids the Time index buffer for the ATS
 **  
 **  \par Description
 **            This routine builds the ATS Time Index Table after an ATS buffer
 **            has been loaded and the ATS Command Index Table has been built.
 **            This routine will take the commands that are pointed to by the
 **            pointers in the command index table and sort the commands by
 **            time order.       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    AtsIndex        ATS array index
 **
 **
 *************************************************************************/

void SC_BuildTimeIndexTable (uint16 AtsIndex);

/************************************************************************/
/** \brief Inserts an item in a sorted list
 **  
 **  \par Description
 **            This function will insert a new element into the list of
 **            ATS commands sorted by execution time.       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    AtsIndex        ATS array index selection
 **
 **  \param [in]    NewCmdIndex     ATS command index for new list element
 ** 
 **  \param [in]    ListLength      Number of elements currently in list
 **
 **
 *************************************************************************/
void SC_Insert (uint16 AtsIndex, int32 NewCmdIndex, int32 ListLength);

/************************************************************************/
/** \brief Initializes ATS tables before a load starts
 **  
 **  \par Description
 **            This function simply clears out the ats tables in preparation
 **            for a load.     
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    AtsIndex        ATS array index
 **
 *************************************************************************/
void SC_InitAtsTables (uint16 AtsIndex);

/************************************************************************/
/** \brief Validation function for ATS or Append ATS table data
 **  
 **  \par Description
 **              This routine is called to validate the contents of an ATS
 **            or Apppend ATS table.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS   \endcode
 **  \retcode #SC_ERROR            \retdesc \copydoc SC_ERROR   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SC_VerifyAtsTable (uint16 *Buffer, int32 BufferWords);

/************************************************************************/
/** \brief Validation function for a single ATS or Append ATS table entry
 **  
 **  \par Description
 **              This routine is called to validate the contents of a
 **            single ATS or Append ATS table entry.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retstmt Returns 0 if no more entries in the table  \endcode
 **  \retstmt Returns -1 if the current entry is invalid  \endcode
 **  \retstmt Returns positive integer equal to table entry length (in words) \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SC_VerifyAtsEntry(uint16 *Buffer, int32 EntryIndex, int32 BufferWords);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load the ATS from its table to memory                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_LoadAts (uint16 AtsIndex)
{
    uint16                  AtsEntryWords;      /* current ats entry length in words */
    uint16                  AtsCmdNum;          /* current ats entry command number */
    uint16                  AtsEntryIndex;      /* index into the load for current ats entry */
    CFE_SB_MsgPtr_t         AtsCmd;             /* a pointer to an ats command */
    SC_AtsEntryHeader_t    *AtsEntryPtr;        /* a pointer to an ats entry header */
    uint16                 *AtsTablePtr;        /* pointer to the start of the Ats table */

    int32   Result = CFE_SUCCESS;
    boolean StillProcessing = TRUE;

    /*
     ** Initialize all structrures
     */
    SC_InitAtsTables (AtsIndex);
 
    /* initialize pointers and counters */
    AtsTablePtr = SC_OperData.AtsTblAddr[AtsIndex];
    AtsEntryIndex = 0;
  
        while (StillProcessing)
    {
        /*
         ** Make sure that the pointer as well as the primary packet
         ** header fit in the buffer, so a G.P fault is not caused.
         */
        if (AtsEntryIndex < SC_ATS_BUFF_SIZE)
        {
            /* get the next command number from the buffer */
            AtsCmdNum = ((SC_AtsEntryHeader_t *)&AtsTablePtr[AtsEntryIndex]) ->CmdNumber;
    
            if (AtsCmdNum == 0)
            {   
                /* end of the load reached */
                Result = CFE_SUCCESS;
                StillProcessing = FALSE;
            }
           
                    /* make sure the CmdPtr can fit in a whole Ats Cmd Header at the very least */
            else if (AtsEntryIndex > (SC_ATS_BUFF_SIZE - (sizeof(SC_AtsEntryHeader_t)/SC_BYTES_IN_WORD)))
            {
                /* even the smallest command will not fit in the buffer */
                Result = SC_ERROR;
                StillProcessing = FALSE;
            }  /* else if the cmd number is valid and the command */
            /* has not already been loaded                     */
            else
                if (AtsCmdNum <= SC_MAX_ATS_CMDS &&
                    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][AtsCmdNum - 1] == SC_EMPTY)
                {
                    /* get a pointer to the ats command in the table */
                    AtsEntryPtr = (SC_AtsEntryHeader_t *) &AtsTablePtr[AtsEntryIndex];
                    AtsCmd = (CFE_SB_MsgPtr_t) AtsEntryPtr->CmdHeader;
                                       
                    /* if the length of the command is valid */
                    if (CFE_SB_GetTotalMsgLength(AtsCmd) >= SC_PACKET_MIN_SIZE && 
                        CFE_SB_GetTotalMsgLength(AtsCmd) <= SC_PACKET_MAX_SIZE)
                    {
                        /* get the length of the entry in WORDS (plus 1 to round byte len up to word len) */
                        AtsEntryWords = (CFE_SB_GetTotalMsgLength(AtsCmd) + 1 + SC_ATS_HEADER_SIZE) / SC_BYTES_IN_WORD; 
                        
                        /* if the command does not run off of the end of the buffer */
                        if (AtsEntryIndex + AtsEntryWords <= SC_ATS_BUFF_SIZE)
                        {
                            /* set the command pointer in the command index table */
                            /* CmdNum starts at one....                          */
                            
                            SC_AppData.AtsCmdIndexBuffer[AtsIndex][AtsCmdNum -1] = AtsEntryIndex;
                            
                            /* set the command status to loaded in the command status table */
                            SC_OperData.AtsCmdStatusTblAddr[AtsIndex][AtsCmdNum - 1] = SC_LOADED;
                            
                            /* increment the number of commands loaded */
                            SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands++;
                            
                            /* increment the ats_entry index to the next ats entry */
                            AtsEntryIndex = AtsEntryIndex + AtsEntryWords;
                        }
                        else
                        { /* the command runs off the end of the buffer */
                            Result = SC_ERROR;
                            StillProcessing = FALSE;
                        } /* end if */
                    }
                    else
                    { /* the command length was invalid */
                        Result = SC_ERROR;
                        StillProcessing = FALSE;
                    } /* end if */
                }
                else
                { /* the cmd number is invalid */                    
                    Result = SC_ERROR;
                    StillProcessing = FALSE;
                } /* end if */
        }
        else
        {
            if (AtsEntryIndex == SC_ATS_BUFF_SIZE)
            {
                /* we encountered a load exactly as long as the buffer */
                Result = CFE_SUCCESS;
                StillProcessing = FALSE;

            }
            else
            { /* the pointer is over the end of the buffer */
                
                Result = SC_ERROR;
                StillProcessing = FALSE;
            } /* end if */
        }/*end else */
    } /* end while */
    
    /*
     **   Now the commands are parsed through, need to build the tables
     **   if the load was a sucess, need to build the tables
     */
    
    /* if the load finished without errors and there was at least one command */
    if ((Result == CFE_SUCCESS) && (SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands > 0))
    {  
        /* record the size of the load in the ATS info table */
        SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize = AtsEntryIndex;  /* size in WORDS */
             
        /* build the time index table */
        SC_BuildTimeIndexTable(AtsIndex);   
    }
    else
    { /* there was an error */
        SC_InitAtsTables (AtsIndex);
    } /* end if */ 

} /* end SC_LoadAts */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Builds the time table for the ATS buffer                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_BuildTimeIndexTable (uint16 AtsIndex)
{
    int32 i;
    int32 ListLength;

    /* initialize sorted list contents */
    for (i = 0; i < SC_MAX_ATS_CMDS; i++)
    {
        SC_AppData.AtsTimeIndexBuffer[AtsIndex][i] = SC_ERROR;
    }
    
    /* initialize sorted list length */
    ListLength = 0;

    /* create time sorted list */
    for (i = 0; i < SC_MAX_ATS_CMDS; i++)
    {
        /* add in-use command entries to time sorted list */
        if (SC_AppData.AtsCmdIndexBuffer[AtsIndex][i] != SC_ERROR)
        { 
            SC_Insert(AtsIndex, i, ListLength);
            ListLength++;   
        }
    }

} /* end SC_BuildTimeIndexTable */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Inserts and element into a sorted list                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_Insert (uint16 AtsIndex, int32 NewCmdIndex, int32 ListLength)
{
    SC_AtsEntryHeader_t *Entry;         /* ATS table entry pointer */
    SC_AbsTimeTag_t      NewCmdTime=0;    /* new command execution time */
    SC_AbsTimeTag_t      ListCmdTime;   /* list entry execution time */
    int32                CmdIndex;      /* ATS command index (cmd num - 1) */
    int32                EntryIndex;    /* ATS entry location in table */
    int32                TimeBufIndex;  /* this must be signed */        

    /* get execution time for new list entry */
    if (ListLength > 0)
    {
        /* first get the entry index in the selected ATS table for the new command */
        EntryIndex = SC_AppData.AtsCmdIndexBuffer[AtsIndex][NewCmdIndex];
        /* then get a pointer to the ATS entry */
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][EntryIndex];
        /* then get the execution time from the ATS entry for the new command */
        NewCmdTime = SC_GetAtsEntryTime(Entry);
    }

    /* start at last element in the sorted by time list */
    TimeBufIndex = ListLength - 1;

    while (TimeBufIndex >= 0)
    {
        /* first get the cmd index for this list entry */
        CmdIndex = SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeBufIndex];
        /* then get the entry index from the ATS table */
        EntryIndex = SC_AppData.AtsCmdIndexBuffer[AtsIndex][CmdIndex];
        /* then get a pointer to the ATS entry data */
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][EntryIndex];
        /* then get cmd execution time from the ATS entry */
        ListCmdTime = SC_GetAtsEntryTime(Entry);

        /* compare time for this list entry to time for new cmd */
        if (SC_CompareAbsTime(ListCmdTime, NewCmdTime))
        {
            /* new cmd will execute before this list entry */

            /* move this list entry to make room for new cmd */
            SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeBufIndex + 1] =
               SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeBufIndex];

            /* back up to previous list entry (ok if -1) */
            TimeBufIndex--;
        }
        else
        {
            /* new cmd will execute at same time or after this list entry */
            break;
        }
    }

    /*
    ** TimeBufIndex is now one slot before the target slot...
    **   if new cmd time is earlier than all other entries
    **     then TimeBufIndex is -1 and all others have been moved
    **   else only entries with later times have been moved
    ** In either case, there is an empty slot next to TimeBufIndex
    */    
    SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeBufIndex + 1] = NewCmdIndex;

} /* end SC_Insert */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Clears out Ats Tables before a load                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_InitAtsTables (uint16 AtsIndex)
{
    int32 i; 
        
    /* loop through and set the ATS tables to zero */
    for (i = 0; i < SC_MAX_ATS_CMDS; i++)
    {
        SC_AppData.AtsCmdIndexBuffer[AtsIndex][i]    = SC_ERROR;
        SC_OperData.AtsCmdStatusTblAddr[AtsIndex][i] = SC_EMPTY;
        SC_AppData.AtsTimeIndexBuffer[AtsIndex][i]   = SC_ERROR;
    }
    
    /* initialize the pointers and counters   */
    SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize = 0;
    SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands = 0;

} /* end SC_InitAtsTables */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load an RTS into memory                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_LoadRts (uint16 RtsIndex)
{    
    /* Clear out the RTS info table */
    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_LOADED;
    SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr = 0;
    SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr = 0;
    SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr = 0;
    SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandTime = 0;
    SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr = 0;
       
    /* Make sure the RTS is disabled */
    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = TRUE;
        
} /* SC_LoadRts */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Validate ATS table data                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SC_ValidateAts (void *TableData)
{
    int32 Result;

    /* Common ATS table verify function needs size of this table */
    Result = SC_VerifyAtsTable((uint16 *) TableData, SC_ATS_BUFF_SIZE);
        
    return(Result);
    
} /* end SC_ValidateAts */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Parses the RTS to make sure it looks good                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean SC_ParseRts (uint16 Buffer[])
{
    uint16                          i;
    boolean                         Done;
    boolean                         Error;
    CFE_SB_MsgPtr_t                 RtsCmd;
    SC_RtsEntryHeader_t            *RtsEntryPtr;
    uint16                          RtsCmdSize;

    i = 0;
    Done = Error = FALSE;
        
    while (Error == FALSE && Done == FALSE)
    {
        
        /*
         ** Check to see if a minimum command fits within an RTS
         */
        if (i <= (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD)))
        {
            
            /*
             ** Cast a header to the RTS buffer current location
             ** and get the size of the packet
             */
            
            RtsEntryPtr  = (SC_RtsEntryHeader_t *) &Buffer[i];
            RtsCmd = (CFE_SB_MsgPtr_t) RtsEntryPtr->CmdHeader; 
            
            RtsCmdSize = CFE_SB_GetTotalMsgLength(RtsCmd) + SC_RTS_HEADER_SIZE;
               
            if ( (RtsEntryPtr->TimeTag == 0) && (CFE_SB_GetMsgId(RtsCmd) == 0))
            {
                Done = TRUE;     /* assumed end of file */
            }
            else if (CFE_SB_GetMsgId(RtsCmd) == 0)
            {
                CFE_EVS_SendEvent (SC_RTS_INVLD_MID_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "RTS cmd loaded with invalid MID at %d",
                                   i);
                                   
                Error = TRUE;     /* invalid message id */
            }
            else
            {
                 /* check to see if the length field in the RTS is valid */
                if (CFE_SB_GetTotalMsgLength(RtsCmd) < SC_PACKET_MIN_SIZE ||
                    CFE_SB_GetTotalMsgLength(RtsCmd) > SC_PACKET_MAX_SIZE) 
                {
                CFE_EVS_SendEvent (SC_RTS_LEN_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "RTS cmd loaded with invalid length at %d, len: %d",
                                   i,
                                   CFE_SB_GetTotalMsgLength(RtsCmd));
                                   
                    Error = TRUE;  /* Length error */
                    
                }
                else if ((i + ((RtsCmdSize + 1) / SC_BYTES_IN_WORD)) > SC_RTS_BUFF_SIZE)
                {
                     CFE_EVS_SendEvent (SC_RTS_LEN_BUFFER_ERR_EID,
                                        CFE_EVS_ERROR,
                                       "RTS cmd at %d runs off end of buffer",
                                        i);
                    Error = TRUE; /* command runs off of the end of the buffer */   
                }
                else if ((i + ((RtsCmdSize + 1) / SC_BYTES_IN_WORD)) == SC_RTS_BUFF_SIZE)
                {
                    Done = TRUE;
                }
                else
                {  /* command fits in buffer */
                    
                    i += ((RtsCmdSize + 1) / SC_BYTES_IN_WORD);   /* remember 'i' is expressed in words */
                    
                } /* end if */
                
            } /* endif */    
        }
        else
        {  /* command does not fit in the buffer */

            /*
             ** If it looks like there is data, reject the load,
             ** if it looks empty then we are done
             */
            if (Buffer[i] == 0)
            {
                Done = TRUE;
            }
            else
            {
                CFE_EVS_SendEvent (SC_RTS_LEN_TOO_LONG_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "RTS cmd loaded won't fit in buffer at %d",
                                   i);
                Error = TRUE;
            }
        } /* endif */
        
    } /* endwhile */
        
    /*
     ** finished, report results
     */
     
    /* If Error was TRUE, then SC_ParseRts must return FALSE */
    return (!Error);

} /* end SC_ParseRts */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate an RTS                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SC_ValidateRts (void *TableData)
{

    uint16 *TableDataPtr;
    int32   Result = CFE_SUCCESS;
    
    TableDataPtr = (uint16 *)TableData;
    
    /*
     ** make a rough check on the first command to see if there is
     ** something in the buffer
     */
    if (SC_ParseRts (TableDataPtr) == FALSE)
    {
        /* event message is put out by Parse RTS */
        Result = SC_ERROR;
    }

    return (Result);

} /* end SC_ValidateRts */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Validate Append ATS table data                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SC_ValidateAppend (void *TableData)
{
    int32 Result;

    /* Common ATS table verify function needs size of this table */
    Result = SC_VerifyAtsTable((uint16 *) TableData, SC_APPEND_BUFF_SIZE);
        
    return(Result);
    
} /* end SC_ValidateAppend */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Have new Append ATS table data, update Append ATS Info table    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_UpdateAppend (void)
{
    SC_AtsEntryHeader_t *Entry;
    CFE_SB_MsgPtr_t  CmdPacket;
    int32 CommandBytes;
    int32 CommandWords;
    int32 EntryIndex = 0;
    int32 EntryCount = 0;
    boolean StillProcessing = TRUE;

    /* Count Append ATS table entries and get total size */
    while (StillProcessing)
    {
        if (EntryIndex >= SC_APPEND_BUFF_SIZE)
        {
            /* End of Append ATS table buffer */
            StillProcessing = FALSE;
        }
        else
        {
            Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[EntryIndex];

            if ((Entry->CmdNumber == 0) || (Entry->CmdNumber > SC_MAX_ATS_CMDS))
            {
                /* End of valid command numbers */
                StillProcessing = FALSE;
            }
            else
            {
                /* Compute entry command packet length */
                CmdPacket = (CFE_SB_MsgPtr_t) Entry->CmdHeader;
                CommandBytes = CFE_SB_GetTotalMsgLength(CmdPacket);
                CommandWords = (CommandBytes + 1) / 2;

                if ((CommandBytes < SC_PACKET_MIN_SIZE) || (CommandBytes > SC_PACKET_MAX_SIZE))
                {
                    /* Entry command packet must have a valid length */
                    StillProcessing = FALSE;
                }
                else if ((EntryIndex + SC_ATS_HDR_NOPKT_WORDS + CommandWords) > SC_APPEND_BUFF_SIZE)
                {
                    /* Entry command packet must fit within ATS append table buffer */
                    StillProcessing = FALSE;
                }
                else
                {
                    /* Compute buffer index for next Append ATS table entry */
                    EntryIndex += (SC_ATS_HDR_NOPKT_WORDS + CommandWords);
                    EntryCount++;
                }
            }
        }
    }

    /* Results will also be reported in HK */
    SC_AppData.AppendLoadCount++;
    SC_AppData.AppendEntryCount = EntryCount;
    SC_AppData.AppendWordCount = EntryIndex;

    CFE_EVS_SendEvent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION,
           "Update Append ATS Table: load count = %d, command count = %d, byte count = %d",
                      SC_AppData.AppendLoadCount, EntryCount, EntryIndex * 2);
    return;

} /* end SC_UpdateAppend */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Append contents of Append ATS table to indicated ATS table      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_ProcessAppend (uint16 AtsIndex)
{
    SC_AtsEntryHeader_t *Entry;
    CFE_SB_MsgPtr_t  CmdPacket;

    int32 CommandBytes;
    int32 CommandWords;
    int32  EntryIndex;
    int32  i;
    uint16 CmdIndex;

    /* save index of free area at end of ATS table data */
    EntryIndex = SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize;

    /* copy Append table data to end of ATS table data */
    CFE_PSP_MemCpy(&SC_OperData.AtsTblAddr[AtsIndex][EntryIndex],
                    SC_OperData.AppendTblAddr, SC_AppData.AppendWordCount * 2);

    /* update size of ATS table data */
    SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize += SC_AppData.AppendWordCount;

    /* add appended entries to ats process tables */
    for (i = 0; i < SC_AppData.AppendEntryCount; i++)
    {
        /* get pointer to next appended entry */
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][EntryIndex];

        /* convert base one cmd number to base zero index */
        CmdIndex = Entry->CmdNumber - 1;

        /* count only new commands, not replaced commands */
        if (SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] == SC_EMPTY)
        {
            SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands++;
        }

        /* update array of pointers to ats entries */
        SC_AppData.AtsCmdIndexBuffer[AtsIndex][CmdIndex] = EntryIndex;
        SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_LOADED;

        /* update entry index to point to the next entry */
        CmdPacket = (CFE_SB_MsgPtr_t) Entry->CmdHeader;
        CommandBytes = CFE_SB_GetTotalMsgLength(CmdPacket);
        CommandWords = (CommandBytes + 1) / 2;
        EntryIndex += (SC_ATS_HDR_NOPKT_WORDS + CommandWords);
    }

    /* rebuild time sorted list of commands */
    SC_BuildTimeIndexTable(AtsIndex);

    /* did we just append to an ats that was executing? */
    if ((SC_OperData.AtsCtrlBlckAddr->AtpState == SC_EXECUTING) &&
        (SC_OperData.AtsCtrlBlckAddr->AtsNumber == (AtsIndex + 1)))
    {
        /*
        ** re-start the ats -- this will go thru the process of skipping
        **  past due entries (all of the old entries that had already
        **  been executed and all of the new entries with an old time)
        */
        if (SC_BeginAts(AtsIndex, 0))
        {
            SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
        }
    }                            

    /* notify cFE that we have modified the ats table */
    CFE_TBL_Modified(SC_OperData.AtsTblHandle[AtsIndex]);

    return;

} /* end SC_ProcessAppend */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Verify contents of ATS table data                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SC_VerifyAtsTable (uint16 *Buffer, int32 BufferWords)
{
    int32   Result = CFE_SUCCESS;
    int32   BufferIndex = 0;
    int32   CommandCount = 0;
    int32   i;

    boolean StillProcessing = TRUE;


    /* Initialize all command numbers as unused */
    for (i = 0; i < SC_MAX_ATS_CMDS; i++)
    {
        SC_OperData.AtsDupTestArray[i] = SC_DUP_TEST_UNUSED;
    }

    while (StillProcessing)
    {
        /* Verify the ATS table entry at the current buffer index */
        Result = SC_VerifyAtsEntry(Buffer, BufferIndex, BufferWords);

        if (Result == SC_ERROR)
        {
            /* Entry at current buffer index is invalid */
            StillProcessing = FALSE;
        }
        else if (Result == CFE_SUCCESS)
        {
            /* No more entries -- end of buffer or cmd num = 0 */
            StillProcessing = FALSE;
        }
        else
        {
            /* Result is size (in words) of this entry */
            BufferIndex += Result;
            CommandCount++;
        }
    }

    if (Result == CFE_SUCCESS)
    {
        if (CommandCount == 0)
        {
            /* Table must contain at least one valid entry */
            Result = SC_ERROR;

            CFE_EVS_SendEvent(SC_VERIFY_ATS_MPT_ERR_EID, CFE_EVS_ERROR,
                             "Verify ATS Table error: table is empty");
        }
        else
        {
            CFE_EVS_SendEvent(SC_VERIFY_ATS_EID, CFE_EVS_INFORMATION,
               "Verify ATS Table: command count = %d, byte count = %d",
                              CommandCount, BufferIndex * 2);
        }
    }
        
    return(Result);
    
} /* end SC_VerifyAtsTable */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify contents of one ATS table entry                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SC_VerifyAtsEntry(uint16 *Buffer, int32 EntryIndex, int32 BufferWords)
{
    SC_AtsEntryHeader_t *Entry = (SC_AtsEntryHeader_t *) &Buffer[EntryIndex];
    CFE_SB_MsgPtr_t  CmdPacket;

    int32 CommandBytes;
    int32 CommandWords;
    int32 Result = CFE_SUCCESS;

    /*
    ** Verify the ATS table entry located at the indicated buffer offset
    */
    if (EntryIndex >= BufferWords)
    {
        /*
        ** The process logic will prevent the index from ever exceeding
        **  the size of the buffer due to bad table data content.  Still,
        **  we must include the "greater than" in the test above to
        **  protect against our own potential coding errors.
        */

        /* All done -- end of ATS buffer */
        Result = CFE_SUCCESS;
    }
    else if (Entry->CmdNumber == 0)
    {
        /*
        ** If there is at least one word remaining in the buffer then it
        **  is OK to test the command number without fear of accessing
        **  past the end of valid data because the command number is the
        **  first element in an ATS entry structure.
        */

        /* All done -- end of in-use portion of buffer */
        Result = CFE_SUCCESS;
    }
    else if (Entry->CmdNumber > SC_MAX_ATS_CMDS)
    {
        /* Error -- invalid command number */
        Result = SC_ERROR;

        CFE_EVS_SendEvent(SC_VERIFY_ATS_NUM_ERR_EID, CFE_EVS_ERROR,
           "Verify ATS Table error: invalid command number: buf index = %d, cmd num = %d",
                          EntryIndex, Entry->CmdNumber);
    }
    else if ((EntryIndex + SC_ATS_HDR_WORDS) > BufferWords)
    {
        /* Error -- not enough room for smallest possible ATS entry */
        Result = SC_ERROR;

        CFE_EVS_SendEvent(SC_VERIFY_ATS_END_ERR_EID, CFE_EVS_ERROR,
           "Verify ATS Table error: buffer full: buf index = %d, cmd num = %d, buf words = %d",
                          EntryIndex, Entry->CmdNumber, BufferWords);
    }
    else
    {
        /* Now it is OK to de-reference ATS cmd packet */
        CmdPacket = (CFE_SB_MsgPtr_t) Entry->CmdHeader;

        /* Start with the byte length of the command packet */
        CommandBytes = CFE_SB_GetTotalMsgLength(CmdPacket);

        /* Convert packet byte length to word length (round up odd bytes) */
        CommandWords = (CommandBytes + 1) / 2;

        if ((CommandBytes < SC_PACKET_MIN_SIZE) || (CommandBytes > SC_PACKET_MAX_SIZE))
        {
            /* Error -- invalid command packet byte length */
            Result = SC_ERROR;

            CFE_EVS_SendEvent(SC_VERIFY_ATS_PKT_ERR_EID, CFE_EVS_ERROR,
               "Verify ATS Table error: invalid length: buf index = %d, cmd num = %d, pkt len = %d",
                              EntryIndex, Entry->CmdNumber, CommandBytes);
        }
        else if ((EntryIndex + SC_ATS_HDR_NOPKT_WORDS + CommandWords) > BufferWords)
        {
            /* Error -- packet must fit within buffer */
            Result = SC_ERROR;

            CFE_EVS_SendEvent(SC_VERIFY_ATS_BUF_ERR_EID, CFE_EVS_ERROR,
               "Verify ATS Table error: buffer overflow: buf index = %d, cmd num = %d, pkt len = %d",
                              EntryIndex, Entry->CmdNumber, CommandBytes);
        }
        else if (SC_OperData.AtsDupTestArray[Entry->CmdNumber -1] != SC_DUP_TEST_UNUSED)
        {
            /* Entry with duplicate command number is invalid */
            Result = SC_ERROR;

            CFE_EVS_SendEvent(SC_VERIFY_ATS_DUP_ERR_EID, CFE_EVS_ERROR,
               "Verify ATS Table error: dup cmd number: buf index = %d, cmd num = %d, dup index = %d",
                              EntryIndex, Entry->CmdNumber,
                              SC_OperData.AtsDupTestArray[Entry->CmdNumber -1]);
        }
        else
        {
            /* Compute length (in words) for this ATS table entry */
            Result = SC_ATS_HDR_NOPKT_WORDS + CommandWords;

            /* Mark this ATS command ID as in use at this table index */
            SC_OperData.AtsDupTestArray[Entry->CmdNumber -1] = EntryIndex;
        }
    }

    return(Result);

} /* End of SC_VerifyAtsEntry */


/************************/
/*  End of File Comment */
/************************/



