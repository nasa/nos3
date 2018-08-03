 /*************************************************************************
 ** File:
 **   $Id: sc_cmds.c 1.21 2015/03/02 12:59:07EST sstrege Exp  $
 **
 **  Copyright � 2007-2014 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **  This software was created at NASA's Goddard Space Flight Center.
 **  This software is governed by the NASA Open Source Agreement and may be 
 **  used, distributed and modified only pursuant to the terms of that 
 **  agreement. 
 **
 ** Purpose: 
 **   This file contains the functions to handle processing of ground 
 **   command requests, housekeeping requests, and table updates
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_cmds.c  $ 
 **   Revision 1.21 2015/03/02 12:59:07EST sstrege  
 **   Added copyright information 
 **   Revision 1.20 2014/06/18 14:20:04EDT sjudy  
 **   Corrected table handle in call to CFE_TBL_Manage. 
 **   Revision 1.18 2014/06/06 07:37:51GMT-08:00 sjudy  
 **   Changed event msgs to have 'RTS' or 'ATS' instead of "Real Time Sequence", etc. 
 **   Revision 1.17 2011/09/26 09:47:57GMT-08:00 lwalling  
 **   Change function and structure names from HkStatus to HkPacket 
 **   Revision 1.16 2011/09/23 14:25:25EDT lwalling  
 **   Made group commands conditional on configuration definition 
 **   Revision 1.15 2011/09/07 10:58:55EDT lwalling  
 **   Fix typo that prevents calls to group command handlers 
 **   Revision 1.14 2011/03/15 17:26:21EDT lwalling  
 **   Modify housekeeping request command handler to start selected auto-exec RTS 
 **   Revision 1.13 2011/03/14 10:49:30EDT lwalling  
 **   Add RTS group commands to function SC_ProcessCommands(). 
 **   Revision 1.12 2011/01/28 10:53:02EST lwalling  
 **   Allow table notify commands before first table is activated 
 **   Revision 1.11 2010/10/01 10:38:16EDT lwalling  
 **   Support cFE Table Services notify commands for pending dump or validate, etc. 
 **   Revision 1.10 2010/09/28 10:42:28EDT lwalling  
 **   Update list of included header files, add table manage cmd handler functions 
 **   Revision 1.9 2010/05/18 14:13:41EDT lwalling  
 **   Change AtsCmdIndexBuffer contents from entry pointer to entry index 
 **   Revision 1.8 2010/04/21 15:40:21EDT lwalling  
 **   Changed local storage of Append ATS table use from bytes to words 
 **   Revision 1.7 2010/04/19 10:39:42EDT lwalling  
 **   Add case for Append ATS command 
 **   Revision 1.6 2010/04/16 15:22:56EDT lwalling  
 **   Update HK command to include new Append ATS table status variables 
 **   Revision 1.5 2010/04/05 11:48:43EDT lwalling  
 **   Add Append ATS tables to list of tables being maintained 
 **   Revision 1.4 2010/03/26 18:02:26EDT lwalling  
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values 
 **   Revision 1.3 2010/03/26 11:26:05EDT lwalling  
 **   Removed dead code references to CmdLength 
 **   Revision 1.2 2009/01/26 14:44:44EST nyanchik  
 **   Check in of Unit test 
 **   Revision 1.1 2009/01/05 07:37:34EST nyanchik  
 **   Initial revision 
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/src/project.pj 
 *************************************************************************/


/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "cfe_tbl_msg.h"
#include "sc_app.h"
#include "sc_cmds.h"
#include "sc_atsrq.h"
#include "sc_rtsrq.h"
#include "sc_loads.h"
#include "sc_utils.h"
#include "sc_state.h"
#include "sc_msgids.h"
#include "sc_events.h"
#include "sc_version.h"
#include "sc_rts.h"


/************************************************************************/
/** \brief Table manage request command handler
 **
 **  \par Description
 **       Handler for commands from cFE Table Service requesting that the
 **       application call the cFE table manage API function for the table
 **       indicated by the command packet argument.  Using this command
 **       interface allows applications to call the table API functions
 **       only when load or dump activity is pending.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_MANAGE_TABLE_CC
 **
 *************************************************************************/
void SC_TableManageCmd(CFE_SB_MsgPtr_t CmdPacket);


/************************************************************************/
/** \brief Manage pending update to an RTS table
 **
 **  \par Description
 **       This function is invoked in response to a command from cFE Table
 **       Services indicating that an RTS table has a pending update.  The
 **       function will release the data pointer for the specified table,
 **       allow cFE Table Services to update the table data and re-acquire
 **       the table data pointer.
 **
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         ArrayIndex     index into array of RTS tables
 **
 **  \sa #SC_TableManageCmd
 **
 *************************************************************************/
void SC_ManageRtsTable(int32 ArrayIndex);


/************************************************************************/
/** \brief Manage pending update to an ATS table
 **
 **  \par Description
 **       This function is invoked in response to a command from cFE Table
 **       Services indicating that an ATS table has a pending update.  The
 **       function will release the data pointer for the specified table,
 **       allow cFE Table Services to update the table data and re-acquire
 **       the table data pointer.
 **
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         ArrayIndex     index into array of ATS tables
 **
 **  \sa #SC_TableManageCmd
 **
 *************************************************************************/
void SC_ManageAtsTable(int32 ArrayIndex);


/************************************************************************/
/** \brief Manage pending update to the ATS Append table
 **
 **  \par Description
 **       This function is invoked in response to a command from cFE Table
 **       Services indicating that the ATS Append table has a pending update.
 **       The function will release the data pointer for the specified table,
 **       allow cFE Table Services to update the table data and re-acquire
 **       the table data pointer.
 **
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         (none)
 **
 **  \sa #SC_TableManageCmd
 **
 *************************************************************************/
void SC_ManageAppendTable(void);


/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/************************************************************************/
/** \brief Processes commands
 **  
 **  \par Description
 **       Process commands. Commands can be from external sources or from SC
 **       itself.
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **
 *************************************************************************/
void SC_ProcessCommand (CFE_SB_MsgPtr_t CmdPacket);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Processes a command from the ATS                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_ProcessAtpCmd (void)
{
    CFE_SB_MsgPtr_t             CmdPtr;            /* ATS command pointer */
    SC_AtsEntryHeader_t*        Entry;             /* ATS entry pointer */
    int32                       EntryIndex;        /* ATS entry location in table */
    uint8                       AtsIndex;          /* ATS selection index */
    uint32                      CmdIndex;          /* ATS command index */
    uint16                      TempAtsChar = ' ';
    int32                       Result;
    boolean                     AbortATS = FALSE;

    /*
     ** The following conditions must be met before the ATS command will be
     ** executed:
     ** 1.) The next time is <= the current time
     ** 2.) The next processor number = ATP
     ** 3.) The atp is currently EXECUTING
     */


    if ((!SC_CompareAbsTime (SC_AppData.NextCmdTime[SC_ATP], SC_AppData.CurrentTime)) &&
            (SC_AppData.NextProcNumber == SC_ATP) &&
            (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING))
    {
        /*
         ** Get a pointer to the next ats command
         */
        AtsIndex   = SC_OperData.AtsCtrlBlckAddr->AtsNumber - 1; /* remember 0..1 */
        CmdIndex   = SC_OperData.AtsCtrlBlckAddr->CmdNumber;
        EntryIndex = SC_AppData.AtsCmdIndexBuffer[AtsIndex][CmdIndex];
        Entry      = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][EntryIndex];
        CmdPtr     = (CFE_SB_MsgPtr_t) Entry->CmdHeader;

        /*
         ** Make sure the command has not been executed, skipped or has any other bad status
         */
        if (SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] == SC_LOADED)
        {
            /*
             ** Make sure the command number matches what the command
             ** number is supposed to be
             */
            if (Entry->CmdNumber == (CmdIndex + 1))
            {
                /*
                 ** Check the checksum on the command
                 **
                 */
                if (CFE_SB_ValidateChecksum(CmdPtr) == TRUE)
                {
                    /*
                     ** Count the command for the rate limiter
                     */
                    SC_OperData.NumCmdsSec++;

                    /*
                     **  First check to see if the command is a switch command,
                     **  if it is, then execute the command now instead of sending
                     **  it out on the Software Bus (this is the only exception to
                     **  way stored commands are sent out).
                     */
                     
                    if (CFE_SB_GetMsgId(CmdPtr) == SC_CMD_MID && 
                        CFE_SB_GetCmdCode(CmdPtr) == SC_SWITCH_ATS_CC)
                    {
                        /*
                         ** call the ground switch module
                         */
                        if (SC_InlineSwitch())
                        {
                            /*
                             ** Increment the counter and update the status for
                             ** this command
                             */
                            SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_EXECUTED;
                            SC_AppData.AtsCmdCtr++;
                        }
                        else
                        { /* the switch failed for some reason */

                            SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_FAILED_DISTRIB;
                            SC_AppData.AtsCmdErrCtr++;
                            SC_AppData.LastAtsErrSeq = SC_OperData.AtsCtrlBlckAddr -> AtsNumber;
                            SC_AppData.LastAtsErrCmd = SC_OperData.AtsCtrlBlckAddr -> CmdNumber + 1;

                        } /* end if */
                    }
                    else
                    {
                        Result = CFE_SB_SendMsg(CmdPtr);
                        
                        if ( Result == CFE_SUCCESS)
                        {
                        /* The command sent OK */
                        SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_EXECUTED;
                        SC_AppData.AtsCmdCtr++;

                        }
                        else
                        { /* the command had Software Bus problems */
                            SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_FAILED_DISTRIB;
                            SC_AppData.AtsCmdErrCtr++;
                            SC_AppData.LastAtsErrSeq = SC_OperData.AtsCtrlBlckAddr -> AtsNumber;
                            SC_AppData.LastAtsErrCmd = SC_OperData.AtsCtrlBlckAddr -> CmdNumber + 1;
                            
                            CFE_EVS_SendEvent(SC_ATS_DIST_ERR_EID, CFE_EVS_ERROR,
                               "ATS Command Distribution Failed, Cmd Number: %d, SB returned: 0x%08X",
                                              Entry->CmdNumber, Result);
                        
                            /* Mark this ATS for abortion */
                            AbortATS = TRUE;                      
                        }
                    }
                }
                else
                { /* the checksum failed */
                    /*
                     ** Send an event message to report the invalid command status
                     */
                    CFE_EVS_SendEvent(SC_ATS_CHKSUM_ERR_EID, CFE_EVS_ERROR,
                                     "ATS Command Failed Checksum: Command #%d Skipped",
                                      Entry->CmdNumber);
                    /*
                     ** Increment the ATS error counter
                     */
                    SC_AppData.AtsCmdErrCtr++;

                    /*
                     ** Update the last ATS error information structure
                     */
                    SC_AppData.LastAtsErrSeq = SC_OperData.AtsCtrlBlckAddr -> AtsNumber;
                    SC_AppData.LastAtsErrCmd = SC_OperData.AtsCtrlBlckAddr -> CmdNumber + 1;

                    /* update the command status index table */
                    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_FAILED_CHECKSUM;


                    if (SC_AppData.ContinueAtsOnFailureFlag == FALSE)
                    { /* Stop ATS execution */
                        /*
                         ** Set the temp ATS ID if it is valid
                         */
                        if (SC_OperData.AtsCtrlBlckAddr -> AtsNumber == SC_ATSA)
                            TempAtsChar = 'A';
                        else if (SC_OperData.AtsCtrlBlckAddr -> AtsNumber == SC_ATSB)
                            TempAtsChar = 'B';

                        /* Mark this ATS for abortion */
                        AbortATS = TRUE; 
                    }
                }  /* end checksum test */
            }
            else
            { /* the command number does not match */
                /*
                 ** Send an event message to report the invalid command status
                 */

                CFE_EVS_SendEvent(SC_ATS_MSMTCH_ERR_EID, CFE_EVS_ERROR,
                   "ATS Command Number Mismatch: Command Skipped, expected: %d received: %d",
                                  CmdIndex + 1, Entry->CmdNumber);
                /*
                 ** Increment the ATS error counter
                 */
                SC_AppData.AtsCmdErrCtr++;

                /*
                 ** Update the last ATS error information structure
                 */
                SC_AppData.LastAtsErrSeq = SC_OperData.AtsCtrlBlckAddr -> AtsNumber;
                SC_AppData.LastAtsErrCmd = SC_OperData.AtsCtrlBlckAddr -> CmdNumber + 1;

                /* update the command status index table */
                SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_SKIPPED;
                
                /*
                ** Set the temp ATS ID if it is valid
                */
                if (SC_OperData.AtsCtrlBlckAddr -> AtsNumber == SC_ATSA)
                    TempAtsChar = 'A';
                else if (SC_OperData.AtsCtrlBlckAddr -> AtsNumber == SC_ATSB)
                    TempAtsChar = 'B';
                
                /* Mark this ATS for abortion */
                AbortATS = TRUE; 
            } /* end if  the command number does not match */
        }
        else  /* command isn't marked as loaded */
        {
            /*
             ** Send an event message to report the invalid command status
             */
            CFE_EVS_SendEvent(SC_ATS_SKP_ERR_EID, CFE_EVS_ERROR,
                             "Invalid ATS Command Status: Command Skipped, Status: %d",
                              SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex]);
            /*
             ** Increment the ATS error counter
             */
            SC_AppData.AtsCmdErrCtr++;

            /*
             ** Update the last ATS error information structure
             */
            SC_AppData.LastAtsErrSeq = SC_OperData.AtsCtrlBlckAddr -> AtsNumber;
            SC_AppData.LastAtsErrCmd = SC_OperData.AtsCtrlBlckAddr -> CmdNumber + 1;
            
            /* Do Not Mark this ATS for abortion. The command could be marked as EXECUTED
               if we alerady jumped back in time */ 

        } /* end if */

        if (AbortATS == TRUE)
        {
            CFE_EVS_SendEvent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR,
                             "ATS %c Aborted",
                              TempAtsChar);
                               
            /* Stop the ATS from executing */
            SC_KillAts();
            SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
        }


        /*
         ** Get the next ATS command set up to execute
         */
        SC_GetNextAtsCommand();
        
      
    } /* end if next ATS command time */
} /* end SC_ProccessAtpCommand */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Processes a command from an RTS                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SC_ProcessRtpCommand (void)
{


    CFE_SB_MsgPtr_t             CmdPtr;            /* a pointer to an RTS entry command */
    SC_RtsEntryHeader_t*        RtsEntryPtr;       /* a pointer to an RTS entry header */
    uint16                      RtsNum;            /* the RTS number for the cmd */
    uint16                      CmdOffset;         /* the location of the cmd    */
    uint32                      Result;

    /*
     ** The following conditions must be met before a RTS command is executed:
     ** 1.) The next command time must be <= the current time
     ** 2.) The next processor number must be SC_RTP
     ** 3.) The RTS number in the RTP control block must be valid and
     ** 4.) the RTS must be EXECUTING
     */

    if ((SC_AppData.NextCmdTime[SC_AppData.NextProcNumber] <= SC_AppData.CurrentTime) &&
            (SC_AppData.NextProcNumber == SC_RTP) &&
            (SC_OperData.RtsCtrlBlckAddr -> RtsNumber > 0) &&
            (SC_OperData.RtsCtrlBlckAddr -> RtsNumber <= SC_NUMBER_OF_RTS) &&
            (SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr -> RtsNumber - 1].RtsStatus == SC_EXECUTING))
    {
        /*
         ** Count the command for the rate limiter
         ** even if the command fails
         */
        SC_OperData.NumCmdsSec++;

        /* get the RTS number that can be directly indexed into the table*/
        RtsNum = SC_OperData.RtsCtrlBlckAddr -> RtsNumber - 1;

        /*
         ** Get the Command offset within the RTS
         */
        CmdOffset = SC_OperData.RtsInfoTblAddr[RtsNum].NextCommandPtr;
        
        /*
         ** Get a pointer to the RTS entry using the RTS number and the offset
         */
        RtsEntryPtr = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsNum][CmdOffset];
        CmdPtr = (CFE_SB_MsgPtr_t) RtsEntryPtr->CmdHeader;

        if (CFE_SB_ValidateChecksum(CmdPtr) ==  TRUE)
        {
            /*
             ** Try Sending the command on the Software Bus
             */
             
             Result = CFE_SB_SendMsg(CmdPtr);
             
            if (Result == CFE_SUCCESS)
            {
                /* the command was sent OK */
                SC_AppData.RtsCmdCtr++;
                SC_OperData.RtsInfoTblAddr[RtsNum].CmdCtr++;

                /*
                 ** Get the next command.
                 */
                SC_GetNextRtsCommand();
            }
            else
            { /* the software bus return code was bad */

                /*
                 ** Send an event message to report the invalid command status
                 */
                CFE_EVS_SendEvent (SC_RTS_DIST_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "RTS %03d Command Distribution Failed: RTS Stopped. SB returned 0x%08X",
                                   RtsNum + 1);

                SC_AppData.RtsCmdErrCtr++;
                SC_OperData.RtsInfoTblAddr[RtsNum].CmdErrCtr++;
                SC_AppData.LastRtsErrSeq = SC_OperData.RtsCtrlBlckAddr -> RtsNumber;
                SC_AppData.LastRtsErrCmd = CmdOffset;

                /*
                 ** Stop the RTS from executing
                 */
                SC_KillRts (RtsNum);

            } /* end if */

        }
        else
        { /* the checksum failed */

            /*
             ** Send an event message to report the invalid command status
             */
            CFE_EVS_SendEvent (SC_RTS_CHKSUM_ERR_EID,
                               CFE_EVS_ERROR,
                               "RTS %03d Command Failed Checksum: RTS Stopped",
                               RtsNum + 1);
             /*
             ** Update the RTS command error counter and last RTS error info
             */
            SC_AppData.RtsCmdErrCtr++;
            SC_OperData.RtsInfoTblAddr[RtsNum].CmdErrCtr++;
            SC_AppData.LastRtsErrSeq = SC_OperData.RtsCtrlBlckAddr -> RtsNumber;
            SC_AppData.LastRtsErrCmd = CmdOffset;

            /*
             ** Stop the RTS from executing
             */
            SC_KillRts (RtsNum);
        } /* end if */
    } /* end if */
} /* end SC_ProcessRtpCommand */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Sends Housekeeping Data                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_SendHkPacket (void)
{
    uint16               i;       
 
    SC_OperData.HkPacket.CmdErrCtr       = SC_AppData.CmdErrCtr;
    SC_OperData.HkPacket.CmdCtr          = SC_AppData.CmdCtr;
    SC_OperData.HkPacket.RtsActiveErrCtr = SC_AppData.RtsActiveErrCtr;
    SC_OperData.HkPacket.RtsActiveCtr    = SC_AppData.RtsActiveCtr;
    SC_OperData.HkPacket.AtsCmdCtr       = SC_AppData.AtsCmdCtr;
    SC_OperData.HkPacket.AtsCmdErrCtr    = SC_AppData.AtsCmdErrCtr;
    SC_OperData.HkPacket.RtsCmdCtr       = SC_AppData.RtsCmdCtr;
    SC_OperData.HkPacket.RtsCmdErrCtr    = SC_AppData.RtsCmdErrCtr;
    SC_OperData.HkPacket.LastAtsErrSeq   = SC_AppData.LastAtsErrSeq;
    SC_OperData.HkPacket.LastAtsErrCmd   = SC_AppData.LastAtsErrCmd;
    SC_OperData.HkPacket.LastRtsErrSeq   = SC_AppData.LastRtsErrSeq;
    SC_OperData.HkPacket.LastRtsErrCmd   = SC_AppData.LastRtsErrCmd;

    SC_OperData.HkPacket.AppendCmdArg     = SC_AppData.AppendCmdArg;
    SC_OperData.HkPacket.AppendEntryCount = SC_AppData.AppendEntryCount;
    SC_OperData.HkPacket.AppendByteCount  = SC_AppData.AppendWordCount * 2;
    SC_OperData.HkPacket.AppendLoadCount  = SC_AppData.AppendLoadCount;
    
    /*
     ** fill in the free bytes in each ATS
     */
    SC_OperData.HkPacket.AtpFreeBytes[0] = (SC_ATS_BUFF_SIZE * SC_BYTES_IN_WORD) -
    (SC_OperData.AtsInfoTblAddr[0].AtsSize * SC_BYTES_IN_WORD);
    SC_OperData.HkPacket.AtpFreeBytes[1] = (SC_ATS_BUFF_SIZE * SC_BYTES_IN_WORD) -
    (SC_OperData.AtsInfoTblAddr[1].AtsSize * SC_BYTES_IN_WORD);
    
    /*
     **
     ** fill in the ATP Control Block information
     **
     */

    SC_OperData.HkPacket.AtsNumber = SC_OperData.AtsCtrlBlckAddr -> AtsNumber;

    
    SC_OperData.HkPacket.AtpState       = SC_OperData.AtsCtrlBlckAddr -> AtpState;
    SC_OperData.HkPacket.AtpCmdNumber   = SC_OperData.AtsCtrlBlckAddr -> CmdNumber + 1;
    SC_OperData.HkPacket.SwitchPendFlag = SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag;
    
    SC_OperData.HkPacket.NextAtsTime = SC_AppData.NextCmdTime[SC_ATP];
    
    /*
     ** Fill out the RTP control block information
     */
    
    SC_OperData.HkPacket.NumRtsActive = SC_OperData.RtsCtrlBlckAddr -> NumRtsActive;
    SC_OperData.HkPacket.RtsNumber    = SC_OperData.RtsCtrlBlckAddr -> RtsNumber;
    SC_OperData.HkPacket.NextRtsTime  = SC_AppData.NextCmdTime[SC_RTP];
    
    /*
     ** Fill out the RTS status bit mask
     ** First clear out the status mask
     */
    for (i = 0; i < (SC_NUMBER_OF_RTS+15)/16; i++)
    {
        
        SC_OperData.HkPacket.RtsExecutingStatus[i] = 0;
        SC_OperData.HkPacket.RtsDisabledStatus[i] = 0;
        
    } /* end for */
    
    for (i = 0; i < SC_NUMBER_OF_RTS ; i++)
    {
        
        if (SC_OperData.RtsInfoTblAddr[i].DisabledFlag == TRUE)
        {
            CFE_SET(SC_OperData.HkPacket.RtsDisabledStatus[i/16], i % 16);
        }
        if (SC_OperData.RtsInfoTblAddr[i].RtsStatus == SC_EXECUTING)
        {
            CFE_SET(SC_OperData.HkPacket.RtsExecutingStatus[i/16], i % 16);
        }
    } /* end for */
    
    SC_OperData.HkPacket.ContinueAtsOnFailureFlag = SC_AppData.ContinueAtsOnFailureFlag ;
    
    /* send the status packet */
    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &SC_OperData.HkPacket);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&SC_OperData.HkPacket);
    
} /* end SC_SendHkPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset Counters Command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SC_ResetCountersCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_NoArgsCmd_t)))
    {  
        CFE_EVS_SendEvent (SC_RESET_DEB_EID,
                           CFE_EVS_DEBUG,
                           "Reset counters command");
        
        SC_AppData.CmdCtr = 0;
        SC_AppData.CmdErrCtr = 0;
        SC_AppData.AtsCmdCtr = 0;
        SC_AppData.AtsCmdErrCtr = 0;
        SC_AppData.RtsCmdCtr = 0;
        SC_AppData.RtsCmdErrCtr = 0;
        SC_AppData.RtsActiveCtr = 0;
        SC_AppData.RtsActiveErrCtr = 0;  
    }
} /* end SC_ResetCountersCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* No Op Command                                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_NoOpCmd(CFE_SB_MsgPtr_t CmdPacket)
{
    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_NoArgsCmd_t)))
    {
        SC_AppData.CmdCtr++;
        CFE_EVS_SendEvent(SC_NOOP_INF_EID,
                          CFE_EVS_INFORMATION,
                          "No-op command. Version %d.%d.%d.%d",
                          SC_MAJOR_VERSION,
                          SC_MINOR_VERSION,
                          SC_REVISION,
                          SC_MISSION_REV);
    }     
}/* End SC_NoOpCmd */
       

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process Requests                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_ProcessRequest (CFE_SB_MsgPtr_t CmdPacket)
{      
    CFE_SB_MsgId_t  MessageID;
    int8            IsThereAnotherCommandToExecute = FALSE;

    /* cast the packet header pointer on the packet buffer */

    MessageID = CFE_SB_GetMsgId (CmdPacket);
    
    /*
     ** Get the current system time in the global SC_AppData.CurrentTime
     */
    SC_GetCurrentTime();
  
    switch (MessageID)
    {        
        case SC_CMD_MID:
            /* request from the ground */
            SC_ProcessCommand (CmdPacket);
            break;
            
        case SC_SEND_HK_MID:
            if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_NoArgsCmd_t)))
            {
             
               /* set during init to power on or processor reset auto-exec RTS */
               if (SC_AppData.AutoStartRTS != 0)
               {
                   /* make sure the selected auto-exec RTS is enabled */
                   if (SC_OperData.RtsInfoTblAddr[SC_AppData.AutoStartRTS - 1].RtsStatus == SC_LOADED)
                   {
                       SC_OperData.RtsInfoTblAddr[SC_AppData.AutoStartRTS - 1].DisabledFlag = FALSE;
                   }
                   
                   /* send ground cmd to have SC start the RTS */
                   SC_AutoStartRts(SC_AppData.AutoStartRTS);

                   /* only start it once */
                   SC_AppData.AutoStartRTS = 0;
               }
               
               /* request from health and safety for housekeeping status */
               SC_SendHkPacket();
            }
            break;
            
            case SC_1HZ_WAKEUP_MID:
            /*
             ** Time to execute a command in the SC memory
             */
            
            do
            {
                /*
                 **  Check to see if there is an ATS switch Pending, if so service it.
                 */
                if (SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag == TRUE)
                {
                    SC_ServiceSwitchPend();
                }
                
                if (SC_AppData.NextProcNumber == SC_ATP)
                {
                    SC_ProcessAtpCmd();
                }
                else
                {
                    if (SC_AppData.NextProcNumber == SC_RTP)
                    {
                        SC_ProcessRtpCommand();
                    }
                }
                
                SC_UpdateNextTime();
                
                if ((SC_AppData.NextProcNumber == SC_NONE) ||
                    (SC_AppData.NextCmdTime[SC_AppData.NextProcNumber] > SC_AppData.CurrentTime))
                {
                    SC_OperData.NumCmdsSec = 0;
                    IsThereAnotherCommandToExecute = FALSE;
                }
                else /* Command needs to run immediately */
                {
                    if (SC_OperData.NumCmdsSec >= SC_MAX_CMDS_PER_SEC)
                    {
                        SC_OperData.NumCmdsSec = 0;
                        IsThereAnotherCommandToExecute = FALSE;
                    }
                    else
                    {
                        IsThereAnotherCommandToExecute = TRUE;
                    }
                    
                }
            } while (IsThereAnotherCommandToExecute);
            
            break;
            
            default:
            CFE_EVS_SendEvent (SC_MID_ERR_EID,
                               CFE_EVS_ERROR,
                               "Invalid command pipe message ID: 0x%08X",
                               MessageID);
                               
            SC_AppData.CmdErrCtr++;
            break;
    } /* end switch */
} /* end SC_ProcessRequest */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process a command                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_ProcessCommand (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16              CommandCode;

    CommandCode = CFE_SB_GetCmdCode(CmdPacket);
    
    switch (CommandCode)
    {
        case SC_NOOP_CC:
            SC_NoOpCmd(CmdPacket);
            break;
            
        case SC_RESET_COUNTERS_CC:
            SC_ResetCountersCmd(CmdPacket);
            break;
            
        case SC_START_ATS_CC:
            SC_StartAtsCmd(CmdPacket);
            break;
            
        case SC_STOP_ATS_CC:
            SC_StopAtsCmd(CmdPacket);
            break;
            
        case SC_START_RTS_CC:
            SC_StartRtsCmd (CmdPacket);
            break;
            
        case SC_STOP_RTS_CC:
            SC_StopRtsCmd (CmdPacket);
            break;
            
        case SC_DISABLE_RTS_CC:
            SC_DisableRtsCmd (CmdPacket);
            break;
            
        case SC_ENABLE_RTS_CC:
            SC_EnableRtsCmd (CmdPacket);
            break;
            
        case SC_SWITCH_ATS_CC:
            SC_GroundSwitchCmd(CmdPacket);
            break;
            
        case SC_JUMP_ATS_CC:
            SC_JumpAtsCmd(CmdPacket);
            break;
            
        case SC_CONTINUE_ATS_ON_FAILURE_CC :
            SC_ContinueAtsOnFailureCmd(CmdPacket);
            break;
            
        case SC_APPEND_ATS_CC:
            SC_AppendAtsCmd(CmdPacket);
            break;
            
        case SC_MANAGE_TABLE_CC:
            SC_TableManageCmd(CmdPacket);
            break;

    #if (SC_ENABLE_GROUP_COMMANDS == TRUE)

        case SC_START_RTSGRP_CC:
            SC_StartRtsGrpCmd (CmdPacket);
            break;
            
        case SC_STOP_RTSGRP_CC:
            SC_StopRtsGrpCmd (CmdPacket);
            break;
            
        case SC_DISABLE_RTSGRP_CC:
            SC_DisableRtsGrpCmd (CmdPacket);
            break;
            
        case SC_ENABLE_RTSGRP_CC:
            SC_EnableRtsGrpCmd (CmdPacket);
            break;
    #endif

        default:
            CFE_EVS_SendEvent (SC_INVLD_CMD_ERR_EID,
                               CFE_EVS_ERROR,
                               "Invalid Command Code: MID =  0x%04X CC =  %d",
                                CFE_SB_GetMsgId (CmdPacket),
                               CommandCode);
            SC_AppData.CmdErrCtr++;
            break;     
    } /* end switch */
} /* end ProcessSequenceRequest */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Table Manage Request Command (sent by cFE Table Services)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SC_TableManageCmd(CFE_SB_MsgPtr_t CmdPacket)
{
    int32 ArrayIndex;
    int32 TableID = (int32) ((CFE_TBL_NotifyCmd_t *) CmdPacket)->Payload.Parameter;

    /* Manage selected table as appropriate for each table type */
    if ((TableID >= SC_TBL_ID_ATS_0) &&
        (TableID < (SC_TBL_ID_ATS_0 + SC_NUMBER_OF_ATS)))
    {
        ArrayIndex = TableID - SC_TBL_ID_ATS_0;
        SC_ManageAtsTable(ArrayIndex);
    }
    else if (TableID == SC_TBL_ID_APPEND)
    {
        SC_ManageAppendTable();
    }
    else if ((TableID >= SC_TBL_ID_RTS_0) &&
             (TableID < (SC_TBL_ID_RTS_0 + SC_NUMBER_OF_RTS)))
    {
        ArrayIndex = TableID - SC_TBL_ID_RTS_0;
        SC_ManageRtsTable(ArrayIndex);
    }
    else if (TableID == SC_TBL_ID_RTS_INFO)
    {
        /* No need to release dump only table pointer */
        CFE_TBL_Manage(SC_OperData.RtsInfoHandle);
    }
    else if (TableID == SC_TBL_ID_RTP_CTRL)
    {
        /* No need to release dump only table pointer */
        CFE_TBL_Manage(SC_OperData. RtsCtrlBlckHandle);
    }
    else if (TableID == SC_TBL_ID_ATS_INFO)
    {
        /* No need to release dump only table pointer */
        CFE_TBL_Manage(SC_OperData.AtsInfoHandle);
    }
    else if (TableID == SC_TBL_ID_APP_INFO)
    {
        /* No need to release dump only table pointer */
        CFE_TBL_Manage(SC_OperData.AppendInfoHandle);
    }
    else if (TableID == SC_TBL_ID_ATP_CTRL)
    {
        /* No need to release dump only table pointer */
        CFE_TBL_Manage(SC_OperData.AtsCtrlBlckHandle);
    }
    else if ((TableID >= SC_TBL_ID_ATS_CMD_0) &&
             (TableID < (SC_TBL_ID_ATS_CMD_0 + SC_NUMBER_OF_ATS)))
    {
        /* No need to release dump only table pointer */
        ArrayIndex = TableID - SC_TBL_ID_ATS_CMD_0;
        CFE_TBL_Manage(SC_OperData.AtsCmdStatusHandle[ArrayIndex]);
    }
    else
    {
        /* Invalid table ID */
        CFE_EVS_SendEvent(SC_TABLE_MANAGE_ID_ERR_EID, CFE_EVS_ERROR,
                         "Table manage command packet error: table ID = %d", TableID);
    }

    return;    

} /* End SC_TableManageCmd() */    


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Allow cFE Table Services to manage loadable ATS table           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SC_ManageRtsTable(int32 ArrayIndex)
{
    int32 Result;

    /* Release RTS table data pointer */
    CFE_TBL_ReleaseAddress(SC_OperData.RtsTblHandle[ArrayIndex]);

    /* Allow cFE to manage table */
    CFE_TBL_Manage(SC_OperData.RtsTblHandle[ArrayIndex]);

    /* Re-acquire RTS table data pointer */
    Result = CFE_TBL_GetAddress((void *) &SC_OperData.RtsTblAddr[ArrayIndex],
                                          SC_OperData.RtsTblHandle[ArrayIndex]);
    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /* Process new RTS table data */
        SC_LoadRts(ArrayIndex);
    } 
    else if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_ERR_NEVER_LOADED))
    {
        /* Ignore successful dump or validate and cmds before first activate. */
        CFE_EVS_SendEvent(SC_TABLE_MANAGE_RTS_ERR_EID, CFE_EVS_ERROR,
                         "RTS table manage process error: RTS = %d, Result = 0x%X",
                          ArrayIndex + 1, Result);
    }
    
    return;    

} /* End SC_ManageRtsTable() */    



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Allow cFE Table Services to manage loadable ATS table           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SC_ManageAtsTable(int32 ArrayIndex)
{
    int32 Result;

    /* Release ATS table data pointer */
    CFE_TBL_ReleaseAddress(SC_OperData.AtsTblHandle[ArrayIndex]);

    /* Allow cFE to manage table */
    CFE_TBL_Manage(SC_OperData.AtsTblHandle[ArrayIndex]);

    /* Re-acquire ATS table data pointer */
    Result = CFE_TBL_GetAddress((void *) &SC_OperData.AtsTblAddr[ArrayIndex],
                                          SC_OperData.AtsTblHandle[ArrayIndex]);
    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /* Process new ATS table data */
        SC_LoadAts(ArrayIndex);
    } 
    else if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_ERR_NEVER_LOADED))
    {
        /* Ignore successful dump or validate and cmds before first activate. */
        CFE_EVS_SendEvent(SC_TABLE_MANAGE_ATS_ERR_EID, CFE_EVS_ERROR,
                         "ATS table manage process error: ATS = %d, Result = 0x%X",
                          ArrayIndex + 1, Result);
    }
    
    return;    

} /* End SC_ManageAtsTable() */    



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Allow cFE Table Services to manage loadable ATS Append table    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SC_ManageAppendTable(void)
{
    int32 Result;

    /* Release ATS Append table data pointer */
    CFE_TBL_ReleaseAddress(SC_OperData.AppendTblHandle);

    /* Allow cFE to manage table */
    CFE_TBL_Manage(SC_OperData.AppendTblHandle);

    /* Re-acquire ATS Append table data pointer */
    Result = CFE_TBL_GetAddress((void *) &SC_OperData.AppendTblAddr,
                                          SC_OperData.AppendTblHandle);
    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /* Process new ATS Append table data */
        SC_UpdateAppend();
    } 
    else if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_ERR_NEVER_LOADED))
    {
        /* Ignore successful dump or validate and cmds before first activate. */
        CFE_EVS_SendEvent(SC_TABLE_MANAGE_APPEND_ERR_EID, CFE_EVS_ERROR,
                         "ATS Append table manage process error: Result = 0x%X", Result);
    }
    
    return;    

} /* End SC_ManageAppendTable() */    


/************************/
/*  End of File Comment */
/************************/

